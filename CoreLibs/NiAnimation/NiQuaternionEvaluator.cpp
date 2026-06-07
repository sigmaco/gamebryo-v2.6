// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiQuaternionEvaluator.h"
#include "NiLinRotKey.h"
#include "NiEulerRotKey.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiQuaternionEvaluator, NiKeyBasedEvaluator);

//---------------------------------------------------------------------------
NiQuaternionEvaluator::NiQuaternionEvaluator() : 
    m_spQuaternionData(NULL)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiQuaternionEvaluator::NiQuaternionEvaluator(NiRotData* pkRotData) : 
    m_spQuaternionData(pkRotData)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
unsigned short NiQuaternionEvaluator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiQuaternionEvaluator::GetKeyCount(unsigned short)
    const
{
    if (!m_spQuaternionData)
        return 0;
    else
        return m_spQuaternionData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiQuaternionEvaluator::GetKeyType(
    unsigned short) const
{
    if (!m_spQuaternionData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiQuaternionEvaluator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::ROTKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiQuaternionEvaluator::GetKeyArray(
    unsigned short) const
{
    if (!m_spQuaternionData)
        return NULL;

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    NiRotKey * pkKeys = m_spQuaternionData->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiQuaternionEvaluator::GetKeyStride(
    unsigned short) const
{
    if (!m_spQuaternionData)
        return 0;

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::Collapse()
{
    if (m_spQuaternionData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiRotKey* pkKeys = m_spQuaternionData->GetAnim(uiNumKeys, eType,
            ucSize);
        if (uiNumKeys == 0)
        {
            m_spQuaternionData = NULL;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiRotKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                NiRotKey* pkLinKeys = 
                    (NiRotKey*)NiNew NiLinRotKey[1];
                pkLinKeys[0].SetTime(0.0f);
                NiQuaternion kValue;
                if (eType == NiRotKey::EULERKEY)
                {
                    NiRotKey::InterpFunction interp = 
                        NiRotKey::GetInterpFunction(eType);
                    NIASSERT(interp);
                    interp(0.0f, pkKeys->GetKeyAt(0, ucSize), 0, &kValue);
                }
                else
                {
                    kValue = pkKeys->GetKeyAt(0, ucSize)->GetQuaternion();
                }
                pkLinKeys[0].SetQuaternion(kValue); 
                m_spQuaternionData->ReplaceAnim(pkLinKeys, 1, 
                    NiAnimationKey::LINKEY);
            }
        }
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == ROTATION);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(m_spQuaternionData);
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiRotKey* pkKeys = m_spQuaternionData->GetAnim(uiNumKeys, 
            eType, ucSize);
        NIASSERT(uiNumKeys == 1);
        // Posed EULERKEY channels should be converted 
        // to LINKEY channels via Collapse.
        NIASSERT(eType != NiRotKey::EULERKEY);
        *(NiQuaternion*)pvResult = 
            pkKeys->GetKeyAt(0, ucSize)->GetQuaternion();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel == ROTATION);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(pvResult);

    if (IsEvalChannelInvalid(uiChannel))
    {
        return false;
    }

    // Fill the segment data, if stale.
    if (!pkEvalSPData->IsSPSegmentDataValid(fTime))
    {
        NiScratchPadFillFunc pfnFillFunc = 
            pkEvalSPData->GetSPFillFunc();
        if (!pfnFillFunc || !(*pfnFillFunc)(fTime, pkEvalSPData))
        {
            return false;
        }
    }

    // Compute the value based on the segment data.
    if (!pkEvalSPData->IsAlternateSPData())
    {
        // Slerp segment data.
        NiScratchPad::SlerpRotSegmentData* pkSlerpSeg = 
            (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSlerpSeg);
        float fT = (fTime - pkSlerpSeg->m_fStartTime) * 
            pkSlerpSeg->m_fInvDeltaTime;
        if (fT == 0.0f)
        {
            // Directly copy the value to avoid round-off errors 
            // in the slerp computation. This ensures an exact
            // value for posed channels.
            *(NiQuaternion*)pvResult = pkSlerpSeg->m_kQuat0;
        }
        else
        {
            *(NiQuaternion*)pvResult = NiQuaternion::Slerp(fT, 
                pkSlerpSeg->m_kQuat0, pkSlerpSeg->m_kQuat1);
        }
    }
    else
    {
        // Squad segment data.
        NiScratchPad::SquadRotSegmentData* pkSquadSeg = 
            (NiScratchPad::SquadRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSquadSeg);
        NiScratchPad::SlerpRotSegmentData* pkBaseSeg = 
            &pkSquadSeg->m_kBaseData;
        float fT = (fTime - pkBaseSeg->m_fStartTime) * 
            pkBaseSeg->m_fInvDeltaTime;
        *(NiQuaternion*)pvResult = NiQuaternion::Squad(fT, 
            pkBaseSeg->m_kQuat0, pkSquadSeg->m_kOutQuat0, 
            pkSquadSeg->m_kInQuat1, pkBaseSeg->m_kQuat1);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == ROTATION);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    NIASSERT(m_spQuaternionData);
    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);

    // Bezier and TCB keys require squad interpolation.
    // Other key types (and posed channels) utilize slerp interpolation.
    if (bPosed)
    {
        uiFillSize = 0;
        eSPBSegmentData = SPBSLERPROTSEGMENT;
    }
    else
    {
        // EULERKEY is a special case.
        if (eType == NiRotKey::EULERKEY)
        {
            uiFillSize = sizeof(NiScratchPad::EulerRotFillData);
        }
        else
        {
            uiFillSize = sizeof(NiScratchPad::RotFillData);
        }

        if (eType == NiRotKey::BEZKEY || eType == NiRotKey::TCBKEY)
        {
            eSPBSegmentData = SPBSQUADROTSEGMENT;
        }
        else
        {
            eSPBSegmentData = SPBSLERPROTSEGMENT;
        }
    }
    bSharedFillData = false;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == ROTATION);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_spQuaternionData);

    // Partially initialize the scratch pad segment data.
    NiScratchPad::SlerpRotSegmentData* pkSlerpSeg = 
        (NiScratchPad::SlerpRotSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkSlerpSeg);
    pkSlerpSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
    if (IsReferencedEvaluator())
    {
        NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
        pkSlerpSeg->m_kHeader.m_usOutputIndex = 
            NiScratchPad::INVALIDOUTPUTINDEX;
    }
    else
    {
        NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
        pkSlerpSeg->m_kHeader.m_usOutputIndex = 
            kPBHandle.GetChannelIndex();
    }
#ifdef NIDEBUG
    // The fill function should set these fields.
    pkSlerpSeg->m_fStartTime = NI_INFINITY;
    pkSlerpSeg->m_fInvDeltaTime = 0.0f;
    pkSlerpSeg->m_kQuat0 = NiQuaternion::IDENTITY;
    pkSlerpSeg->m_kQuat1 = NiQuaternion::IDENTITY;
#endif

    unsigned int uiNumKeys;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;
    NiRotKey* pkKeys = m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    // Bezier and TCB keys require squad interpolation.
    // Other key types (and posed channels) utilize slerp interpolation.
    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (bPosed)
    {
        // Mark the segment data as valid for all sequence times.
        pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

        // Fill the remainder of the scratch pad segment data.
        pkSlerpSeg->m_fStartTime = 0.0f;
        pkSlerpSeg->m_fInvDeltaTime = 0.0f;
        // Posed EULERKEY channels should be converted 
        // to LINKEY channels via Collapse.
        NIASSERT(eType != NiRotKey::EULERKEY);
        pkSlerpSeg->m_kQuat0 = pkKeys->GetKeyAt(0, ucSize)->GetQuaternion();
        pkSlerpSeg->m_kQuat1 = pkSlerpSeg->m_kQuat0;
    }
    else
    {
        if (eType == NiAnimationKey::STEPKEY)
        {
            pkEvalSPData->SetStepFunction(true);
        }
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);

        // EULERKEY is a special case.
        if (eType == NiRotKey::EULERKEY)
        {
            // Initialize the scratch pad fill data.
            NiEulerRotKey* pkEulerKey = (NiEulerRotKey*)
                pkKeys->GetKeyAt(0, sizeof(NiEulerRotKey));
            NIASSERT(pkEulerKey);

            NiScratchPad::EulerRotFillData* pkFillData = 
                (NiScratchPad::EulerRotFillData*)pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);

            // Initialize each of 3 sets of float keys.
            for (unsigned char uc = 0; uc < 3; uc++)
            {
                pkFillData->m_afMinTime[uc] = NI_INFINITY;
                pkFillData->m_afMaxTime[uc] = -NI_INFINITY;

                NiScratchPad::CubicFloatFillData* pkFill = 
                    &pkFillData->m_akFloatFill[uc];
                pkFill->m_uiLastIndex = 0;
                pkFill->m_uiNumKeys = pkEulerKey->GetNumKeys(uc);
                pkFill->m_pkKeys = pkEulerKey->GetKeys(uc);
                pkFill->m_eType = pkEulerKey->GetType(uc);
                pkFill->m_ucKeySize = pkEulerKey->GetKeySize(uc);

                NiScratchPad::CubicFloatSegmentData* pkSeg = 
                    &pkFillData->m_akFloatSeg[uc];
                pkSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
                pkSeg->m_kHeader.m_usOutputIndex = 
                    NiScratchPad::INVALIDOUTPUTINDEX;
#ifdef NIDEBUG
                // The fill function should set these fields.
                pkSeg->m_fStartTime = NI_INFINITY;
                pkSeg->m_fInvDeltaTime = 0.0f;
                pkSeg->m_fValue0 = 0.0f;
                pkSeg->m_fOutTangent0 = 0.0f;
                pkSeg->m_fA0 = 0.0f;
                pkSeg->m_fB0 = 0.0f;
#endif
            }

            pkEvalSPData->SetSPFillFunc(&EulerRotFillFunction);
        }
        else
        {
            // Initialize the scratch pad fill data.
            NiScratchPad::RotFillData* pkFillData = 
                (NiScratchPad::RotFillData*)pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);
            pkFillData->m_uiLastIndex = 0;
            pkFillData->m_pkKeys = pkKeys;
            pkFillData->m_uiNumKeys = uiNumKeys;
            pkFillData->m_eType = eType;
            pkFillData->m_ucKeySize = ucSize;

            if (eType == NiRotKey::BEZKEY || eType == NiRotKey::TCBKEY)
            {
                // Mark the alternate SP data flag for squad interpolation.
                pkEvalSPData->SetAlternateSPData(true);
                pkEvalSPData->SetSPFillFunc(&SquadRotFillFunction);

#ifdef NIDEBUG
                // The fill function should set these extra fields.
                NiScratchPad::SquadRotSegmentData* pkSquadSeg = 
                    (NiScratchPad::SquadRotSegmentData*)
                    pkEvalSPData->GetSPSegmentData();
                NIASSERT((void*)&pkSquadSeg->m_kBaseData == (void*)pkSlerpSeg);
                pkSquadSeg->m_kOutQuat0 = NiQuaternion::IDENTITY;
                pkSquadSeg->m_kInQuat1 = NiQuaternion::IDENTITY;
#endif
            }
            else
            {
                pkEvalSPData->SetSPFillFunc(&SlerpRotFillFunction);
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::SlerpRotFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::RotFillData* pkFillData = 
        (NiScratchPad::RotFillData*)pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT(pkFillData->m_pkKeys);
    NIASSERT(pkFillData->m_uiNumKeys > 0);

    // Fill the scratch pad with the slerp segment matching fTime.
    // Ignore the intermediate quaternions used by squad interpolation.
    NiScratchPad::SlerpRotSegmentData* pkSlerpSeg = 
        (NiScratchPad::SlerpRotSegmentData*)pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkSlerpSeg);
    float fMinTime;
    float fMaxTime;
    NiQuaternion kOutQuat0;
    NiQuaternion kInQuat1;
    NiRotKey::GenCubicCoefs(fTime, pkFillData->m_pkKeys, 
        pkFillData->m_eType, pkFillData->m_uiNumKeys, 
        pkFillData->m_uiLastIndex, pkFillData->m_ucKeySize, 
        fMinTime, fMaxTime, pkSlerpSeg->m_kQuat0, pkSlerpSeg->m_kQuat1, 
        kOutQuat0, kInQuat1);
    NIASSERT(kOutQuat0 == pkSlerpSeg->m_kQuat0);
    NIASSERT(kInQuat1 == pkSlerpSeg->m_kQuat1);
    pkSlerpSeg->m_fStartTime = fMinTime;
    pkSlerpSeg->m_fInvDeltaTime = (fMaxTime != fMinTime) ? 
        1.0f / (fMaxTime - fMinTime) : 0.0f;

    // Update the time range to match the segment.
    pkEvalSPData->SetSPSegmentTimeRange(fMinTime, fMaxTime);

    return true;
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::SquadRotFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::RotFillData* pkFillData = 
        (NiScratchPad::RotFillData*)pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT(pkFillData->m_pkKeys);
    NIASSERT(pkFillData->m_uiNumKeys > 0);

    // Fill the scratch pad with the squad segment matching fTime.
    NiScratchPad::SquadRotSegmentData* pkSquadSeg = 
        (NiScratchPad::SquadRotSegmentData*)pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkSquadSeg);
    NiScratchPad::SlerpRotSegmentData* pkBaseSeg = 
        &pkSquadSeg->m_kBaseData;
    float fMinTime;
    float fMaxTime;
    NiRotKey::GenCubicCoefs(fTime, pkFillData->m_pkKeys, 
        pkFillData->m_eType, pkFillData->m_uiNumKeys, 
        pkFillData->m_uiLastIndex, pkFillData->m_ucKeySize, 
        fMinTime, fMaxTime, pkBaseSeg->m_kQuat0, pkBaseSeg->m_kQuat1, 
        pkSquadSeg->m_kOutQuat0, pkSquadSeg->m_kInQuat1);
    pkBaseSeg->m_fStartTime = fMinTime;
    pkBaseSeg->m_fInvDeltaTime = (fMaxTime != fMinTime) ? 
        1.0f / (fMaxTime - fMinTime) : 0.0f;

    // Update the time range to match the segment.
    pkEvalSPData->SetSPSegmentTimeRange(fMinTime, fMaxTime);

    return true;
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::EulerRotFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::EulerRotFillData* pkEulerFill = 
        (NiScratchPad::EulerRotFillData*)pkEvalSPData->GetSPFillData();
    NIASSERT(pkEulerFill);

    // Compute new Euler angles.
    float fAngle[3];
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        NiScratchPad::CubicFloatSegmentData* pkFloatSeg = 
            &pkEulerFill->m_akFloatSeg[ui];

        // Fill the segment data, if stale.
        if (fTime > pkEulerFill->m_afMaxTime[ui] || 
            fTime < pkEulerFill->m_afMinTime[ui])
        {
            NiScratchPad::CubicFloatFillData* pkFloatFill = 
                &pkEulerFill->m_akFloatFill[ui];
            NIASSERT(pkFloatFill->m_pkKeys);
            NIASSERT(pkFloatFill->m_uiNumKeys > 0);

            float fMinTime;
            float fMaxTime;
            NiFloatKey::GenCubicCoefs(fTime, pkFloatFill->m_pkKeys, 
                pkFloatFill->m_eType, pkFloatFill->m_uiNumKeys, 
                pkFloatFill->m_uiLastIndex, pkFloatFill->m_ucKeySize, 
                fMinTime, fMaxTime, pkFloatSeg->m_fValue0, 
                pkFloatSeg->m_fOutTangent0, pkFloatSeg->m_fA0, 
                pkFloatSeg->m_fB0);
            pkFloatSeg->m_fStartTime = fMinTime;
            pkFloatSeg->m_fInvDeltaTime = (fMaxTime != fMinTime) ? 
                1.0f / (fMaxTime - fMinTime) : 0.0f;

            // Update the time range to match the segment.
            pkEulerFill->m_afMinTime[ui] = fMinTime;
            pkEulerFill->m_afMaxTime[ui] = fMaxTime;
        }

        // Compute the angle based on the segment data.
        float fT = (fTime - pkFloatSeg->m_fStartTime) * 
            pkFloatSeg->m_fInvDeltaTime;
        fAngle[ui] = pkFloatSeg->m_fValue0 + (pkFloatSeg->m_fOutTangent0 + 
            (pkFloatSeg->m_fA0 + pkFloatSeg->m_fB0 * fT) * fT) * fT;
    }

    // Fill the slerp segment with the equivalent quaternion.
    NiScratchPad::SlerpRotSegmentData* pkSlerpSeg = 
        (NiScratchPad::SlerpRotSegmentData*)pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkSlerpSeg);
    pkSlerpSeg->m_fStartTime = fTime;
    pkSlerpSeg->m_fInvDeltaTime = 0.0f;
    pkSlerpSeg->m_kQuat0.FromAngleAxesXYZ(fAngle[0], fAngle[1], fAngle[2]);
    pkSlerpSeg->m_kQuat1 = pkSlerpSeg->m_kQuat0;

    // There's no need to fill the scratch pad at an identical time.
    pkEvalSPData->SetSPSegmentTimeRange(fTime, fTime);
        
    return true;
}
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    NiRotKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    bool bKeys = false;
    if (uiNumKeys > 0)
    {
        if (eType == NiRotKey::EULERKEY)
        {
            NiEulerRotKey* pkRotKey = (NiEulerRotKey*) 
                pkKeys->GetKeyAt(0, ucSize);

            float fTempBeginKeyTime = NI_INFINITY;
            float fTempEndKeyTime = -NI_INFINITY;
            for (unsigned char uc = 0; uc < 3; uc++)
            {
                unsigned int uiNumFloatKeys = pkRotKey->GetNumKeys(uc);
                unsigned char ucFloatSize = pkRotKey->GetKeySize(uc);
                if (uiNumFloatKeys > 0)
                {
                    NiFloatKey* pkFloatKeys = pkRotKey->GetKeys(uc);
                    float fKeyTime = pkFloatKeys->GetKeyAt(0,ucFloatSize)->
                        GetTime();
                    if (fKeyTime < fTempBeginKeyTime)
                    {
                        fTempBeginKeyTime = fKeyTime;
                    }
                    fKeyTime = pkFloatKeys->GetKeyAt(uiNumFloatKeys - 1,
                        ucFloatSize)->GetTime();
                    if (fKeyTime > fTempEndKeyTime)
                    {
                        fTempEndKeyTime = fKeyTime;
                    }
                    bKeys = true;
                }
            }
            if (bKeys)
            {
                fBeginKeyTime = fTempBeginKeyTime;
                fEndKeyTime = fTempEndKeyTime;
            }
        }
        else
        {
            fBeginKeyTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
            fEndKeyTime = pkKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
            bKeys = true;
        }
    }

    if (!bKeys)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spQuaternionData)
    {
        m_spQuaternionData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiQuaternionEvaluator::GetSequenceEvaluator(
    float fStartTime, float fEndTime)
{
    NiQuaternionEvaluator* pkSeqEval = (NiQuaternionEvaluator*)
        NiKeyBasedEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);
    if (m_spQuaternionData)
    {
        NiRotDataPtr spNewQuaternionData = m_spQuaternionData
            ->GetSequenceData(fStartTime, fEndTime);
        pkSeqEval->SetQuaternionData(spNewQuaternionData);
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::SetEvalChannelTypes()
{
    if (m_spQuaternionData)
    {
        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        GetKeys(uiNumKeys, eType, ucSize);
        if (uiNumKeys > 0)
        {
            m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL;

            // Posed EULERKEY channels should be converted 
            // to LINKEY channels via Collapse.
            if (m_spQuaternionData->GetNumKeys() == 1 && 
                eType != NiRotKey::EULERKEY)
            {
                m_aiEvalChannelTypes[ROTATION] |= EVALPOSEDFLAG;
            }
        }
        else
        {
            m_aiEvalChannelTypes[ROTATION] = EVALINVALIDCHANNEL;
        }
    }
    else
    {
        m_aiEvalChannelTypes[ROTATION] = EVALINVALIDCHANNEL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiQuaternionEvaluator);
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::CopyMembers(NiQuaternionEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_spQuaternionData = m_spQuaternionData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiQuaternionEvaluator);
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::LoadBinary(kStream);

    m_spQuaternionData = (NiRotData*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::LinkObject(NiStream& kStream)
{
    NiKeyBasedEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spQuaternionData)
    {
        m_spQuaternionData->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::SaveBinary(kStream);

    kStream.SaveLinkID(m_spQuaternionData);
}
//---------------------------------------------------------------------------
bool NiQuaternionEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiQuaternionEvaluator* pkDest = (NiQuaternionEvaluator*) pkObject;

    if ((m_spQuaternionData && !pkDest->m_spQuaternionData) ||
        (!m_spQuaternionData && pkDest->m_spQuaternionData) ||
        (m_spQuaternionData && !m_spQuaternionData->IsEqual(
            pkDest->m_spQuaternionData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiQuaternionEvaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiKeyBasedEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiQuaternionEvaluator::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_spQuaternionData",
        m_spQuaternionData));
}
//---------------------------------------------------------------------------
