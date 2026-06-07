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

#include "NiTransformEvaluator.h"
#include "NiFloatEvaluator.h"
#include "NiPoint3Evaluator.h"
#include "NiQuaternionEvaluator.h"
#include "NiEulerRotKey.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiTransformEvaluator, NiKeyBasedEvaluator);

//---------------------------------------------------------------------------
NiTransformEvaluator::NiTransformEvaluator() :
    m_kTransformValue(), m_spData(NULL)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    // No need to adjust the other eval channel types.
}
//---------------------------------------------------------------------------
NiTransformEvaluator::NiTransformEvaluator(NiTransformData* pkData) :
    m_kTransformValue(), m_spData(pkData)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiTransformEvaluator::NiTransformEvaluator(
    const NiQuatTransform& kPoseValue) :
    m_kTransformValue(kPoseValue), m_spData(NULL)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
unsigned short NiTransformEvaluator::GetKeyChannelCount() const
{
    return 3;
}
//---------------------------------------------------------------------------
unsigned int NiTransformEvaluator::GetKeyCount(unsigned short usChannel) const
{
    if (!m_spData)
        return 0;

    if (usChannel == POSITION)
    {
        return m_spData->GetNumPosKeys();
    }
    else if (usChannel == ROTATION)
    {
        return m_spData->GetNumRotKeys();
    }
    else if (usChannel == SCALE)
    {
        return m_spData->GetNumScaleKeys();
    }

    NIASSERT(usChannel < 3);
    return 0;
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiTransformEvaluator::GetKeyType(
    unsigned short usChannel) const
{
    if (!m_spData)
        return NiAnimationKey::NOINTERP;

    if (usChannel == POSITION)
    {
        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
        return eType; 
    }
    else if (usChannel == ROTATION)
    {
        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
        return eType;     
    }
    else if (usChannel == SCALE)
    {
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
        return eType;     
    }

    NIASSERT(usChannel < 3);
    return NiAnimationKey::NOINTERP;
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiTransformEvaluator::GetKeyContent(
    unsigned short usChannel) const
{
    if (usChannel == POSITION)
        return NiAnimationKey::POSKEY;
    else if (usChannel == ROTATION)
        return NiAnimationKey::ROTKEY;
    else if (usChannel == SCALE)
        return NiAnimationKey::FLOATKEY;
    
    NIASSERT(usChannel < 3);
    return NiAnimationKey::NUMKEYCONTENTS;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiTransformEvaluator::GetKeyArray(unsigned short usChannel)
    const
{
    if (!m_spData)
        return NULL;

    if (usChannel == POSITION)
    {       
        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        NiPosKey * pkKeys = m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
        return pkKeys; 
    }
    else if (usChannel == ROTATION)
    {
        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        NiRotKey * pkKeys = m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
        return pkKeys; 
    }
    else if (usChannel == SCALE)
    {
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        NiFloatKey * pkKeys = m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
        return pkKeys; 
    }

    NIASSERT(usChannel < 3);
    return NULL;
}
//---------------------------------------------------------------------------
unsigned char NiTransformEvaluator::GetKeyStride(unsigned short usChannel)
    const
{
    if (!m_spData)
        return 0;

    if (usChannel == POSITION)
    {       
        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else if (usChannel == ROTATION)
    {
        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else if (usChannel == SCALE)
    {
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }

    NIASSERT(usChannel < 3);
    return 0;
}
//---------------------------------------------------------------------------
void NiTransformEvaluator::Collapse()
{
    if (!m_spData)
    {
        return;
    }

    NiQuatTransform kCollapsedTransform;

    unsigned int uiNumKeys;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;

    // Collapse position keys.
    NiPosKey* pkPosKeys = m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
    if (uiNumKeys == 0)
    {
        m_spData->ReplacePosAnim(NULL, 0, NiAnimationKey::NOINTERP);
        if (m_kTransformValue.IsTranslateValid())
        {
            kCollapsedTransform.SetTranslate(
                m_kTransformValue.GetTranslate());
        }
    }
    else
    {
        NiAnimationKey::IsPosedFunction isposed = 
            NiPosKey::GetIsPosedFunction(eType);
        NIASSERT(isposed);
        if (isposed(pkPosKeys, uiNumKeys))
        {
            NiPoint3 kValue = pkPosKeys->GetKeyAt(0, ucSize)->GetPos();
            kCollapsedTransform.SetTranslate(kValue);
            m_spData->ReplacePosAnim(NULL, 0, NiAnimationKey::NOINTERP);
        }
        else if (m_kTransformValue.IsTranslateValid())
        {
            kCollapsedTransform.SetTranslate(
                m_kTransformValue.GetTranslate());
        }
    }

    // Collapse rotation keys.
    NiRotKey* pkRotKeys = m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
    if (uiNumKeys == 0)
    {
        m_spData->ReplaceRotAnim(NULL, 0, NiAnimationKey::NOINTERP);
        if (m_kTransformValue.IsRotateValid())
        {
            kCollapsedTransform.SetRotate(m_kTransformValue.GetRotate());
        }
    }
    else
    {
        NiAnimationKey::IsPosedFunction isposed = 
            NiRotKey::GetIsPosedFunction(eType);
        NIASSERT(isposed);
        if (isposed(pkRotKeys, uiNumKeys))
        {
            NiQuaternion kValue;
            if (eType == NiRotKey::EULERKEY)
            {
                NiRotKey::InterpFunction interp = 
                    NiRotKey::GetInterpFunction(eType);
                NIASSERT(interp);
                interp(0.0f, pkRotKeys->GetKeyAt(0, ucSize), 0, &kValue);
            }
            else
            {
                kValue = pkRotKeys->GetKeyAt(0, ucSize)->GetQuaternion();
            }
            kCollapsedTransform.SetRotate(kValue);
            m_spData->ReplaceRotAnim(NULL, 0, NiAnimationKey::NOINTERP);
        }
        else if (m_kTransformValue.IsRotateValid())
        {
            kCollapsedTransform.SetRotate(m_kTransformValue.GetRotate());
        }
    }

    // Collapse scale keys.
    NiFloatKey* pkScaleKeys = m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
    if (uiNumKeys == 0)
    {
        m_spData->ReplaceScaleAnim(NULL, 0, NiAnimationKey::NOINTERP);
        if (m_kTransformValue.IsScaleValid())
        {
            kCollapsedTransform.SetScale(m_kTransformValue.GetScale());
        }
    }
    else
    {
        NiAnimationKey::IsPosedFunction isposed = 
            NiFloatKey::GetIsPosedFunction(eType);
        NIASSERT(isposed);
        if (isposed(pkScaleKeys, uiNumKeys))
        {
            float fValue = pkScaleKeys->GetKeyAt(0, ucSize)->GetValue();
            kCollapsedTransform.SetScale(fValue);
            m_spData->ReplaceScaleAnim(NULL, 0, NiAnimationKey::NOINTERP);
        }
        else if (m_kTransformValue.IsScaleValid())
        {
            kCollapsedTransform.SetScale(m_kTransformValue.GetScale());
        }
    }

    if (m_spData->GetNumPosKeys() == 0 && m_spData->GetNumRotKeys() == 0 &&
        m_spData->GetNumScaleKeys() == 0)
    {
        m_spData = NULL;
    }
    m_kTransformValue = kCollapsedTransform;

    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiTransformEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        // Check the scale first as it's the most likely to be posed.
        if (uiChannel == SCALE)
        {
            *(float*)pvResult = m_kTransformValue.GetScale();
        }
        else if (uiChannel == POSITION)
        {
            *(NiPoint3*)pvResult = m_kTransformValue.GetTranslate();
        }
        else
        {
            NIASSERT(uiChannel == ROTATION);
            *(NiQuaternion*)pvResult = m_kTransformValue.GetRotate();
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiTransformEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
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
    if (uiChannel == POSITION)
    {
        NiScratchPad::CubicPoint3SegmentData* pkCubicSeg = 
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicSeg);
        float fT = (fTime - pkCubicSeg->m_fStartTime) * 
            pkCubicSeg->m_fInvDeltaTime;
        *(NiPoint3*)pvResult = pkCubicSeg->m_kValue0 + 
            (pkCubicSeg->m_kOutTangent0 + (pkCubicSeg->m_kA0 + 
            pkCubicSeg->m_kB0 * fT) * fT) * fT;
    }
    else if (uiChannel == ROTATION)
    {
        // Check if the segment requires slerp or squad interpolation.
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
    }
    else
    {
        NIASSERT(uiChannel == SCALE);
        NiScratchPad::CubicFloatSegmentData* pkCubicSeg = 
            (NiScratchPad::CubicFloatSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicSeg);
        float fT = (fTime - pkCubicSeg->m_fStartTime) * 
            pkCubicSeg->m_fInvDeltaTime;
        *(float*)pvResult = pkCubicSeg->m_fValue0 + 
            (pkCubicSeg->m_fOutTangent0 + (pkCubicSeg->m_fA0 + 
            pkCubicSeg->m_fB0 * fT) * fT) * fT;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTransformEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    if (uiChannel == POSITION)
    {
        uiFillSize = bPosed ? 0 : sizeof(NiScratchPad::CubicPoint3FillData);
        eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
    }
    else if (uiChannel == ROTATION)
    {
        // Bezier and TCB keys require squad interpolation.
        // Other key types (and posed channels) utilize slerp interpolation.
        if (bPosed)
        {
            uiFillSize = 0;
            eSPBSegmentData = SPBSLERPROTSEGMENT;
        }
        else
        {
            NIASSERT(m_spData);
            unsigned int uiNumKeys;
            NiRotKey::KeyType eType;
            unsigned char ucSize;
            m_spData->GetRotAnim(uiNumKeys, eType, ucSize);

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
    }
    else
    {
        NIASSERT(uiChannel == SCALE);
        uiFillSize = bPosed ? 0 : sizeof(NiScratchPad::CubicFloatFillData);
        eSPBSegmentData = SPBCUBICFLOATSEGMENT;
    }

    bSharedFillData = false;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiTransformEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // Initialize the evaluator scratch pad data header.
    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (!bPosed && GetKeyType((unsigned short)uiChannel) == 
        NiAnimationKey::STEPKEY)
    {
        pkEvalSPData->SetStepFunction(true);
    }
    NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);

    // Initialize the scrach pad based on channel type.
    if (uiChannel == POSITION)
    {
        // Partially initialize the scratch pad segment data.
        NiScratchPad::CubicPoint3SegmentData* pkCubicSeg = 
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicSeg);
        pkCubicSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
        if (IsReferencedEvaluator())
        {
            NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
            pkCubicSeg->m_kHeader.m_usOutputIndex = 
                NiScratchPad::INVALIDOUTPUTINDEX;
        }
        else
        {
            NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
            pkCubicSeg->m_kHeader.m_usOutputIndex = 
                kPBHandle.GetChannelIndex();
        }
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkCubicSeg->m_fStartTime = NI_INFINITY;
        pkCubicSeg->m_fInvDeltaTime = 0.0f;
        pkCubicSeg->m_kValue0 = NiPoint3::ZERO;
        pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
        pkCubicSeg->m_kA0 = NiPoint3::ZERO;
        pkCubicSeg->m_kB0 = NiPoint3::ZERO;
#endif

        if (bPosed)
        {
            // Mark the segment data as valid for all sequence times.
            pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
            NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

            // Fill the remainder of the scratch pad segment data.
            pkCubicSeg->m_fStartTime = 0.0f;
            pkCubicSeg->m_fInvDeltaTime = 0.0f;
            pkCubicSeg->m_kValue0 = m_kTransformValue.GetTranslate();
            pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
            pkCubicSeg->m_kA0 = NiPoint3::ZERO;
            pkCubicSeg->m_kB0 = NiPoint3::ZERO;
        }
        else
        {
            // Use the cubic fill function in NiPoint3Evaluator.
            pkEvalSPData->SetSPFillFunc(
                &NiPoint3Evaluator::CubicPoint3FillFunction);

            // Initialize the scratch pad fill data.
            NiScratchPad::CubicPoint3FillData* pkFillData = 
                (NiScratchPad::CubicPoint3FillData*)pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);
            pkFillData->m_uiLastIndex = 0;
            NIASSERT(m_spData);
            pkFillData->m_pkKeys = m_spData->GetPosAnim(
                pkFillData->m_uiNumKeys, pkFillData->m_eType, 
                pkFillData->m_ucKeySize);
            NIASSERT(pkFillData->m_pkKeys);
            NIASSERT(pkFillData->m_uiNumKeys > 0);
        }
    }
    else if (uiChannel == ROTATION)
    {
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

        if (bPosed)
        {
            // Mark the segment data as valid for all sequence times.
            pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
            NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

            // Fill the remainder of the scratch pad segment data.
            pkSlerpSeg->m_fStartTime = 0.0f;
            pkSlerpSeg->m_fInvDeltaTime = 0.0f;
            pkSlerpSeg->m_kQuat0 = m_kTransformValue.GetRotate();
            pkSlerpSeg->m_kQuat1 = pkSlerpSeg->m_kQuat0;
        }
        else
        {
            NIASSERT(m_spData);
            unsigned int uiNumKeys;
            NiAnimationKey::KeyType eType;
            unsigned char ucSize;
            NiRotKey* pkKeys = m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
            NIASSERT(pkKeys);
            NIASSERT(uiNumKeys > 0);

            // EULERKEY is a special case.
            if (eType == NiRotKey::EULERKEY)
            {
                // Initialize the scratch pad fill data.
                NiEulerRotKey* pkEulerKey = (NiEulerRotKey*)
                    pkKeys->GetKeyAt(0, sizeof(NiEulerRotKey));
                NIASSERT(pkEulerKey);

                NiScratchPad::EulerRotFillData* pkFillData = 
                    (NiScratchPad::EulerRotFillData*)
                    pkEvalSPData->GetSPFillData();
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

                // Use the Euler fill function in NiQuaternionEvaluator.
                pkEvalSPData->SetSPFillFunc(
                    &NiQuaternionEvaluator::EulerRotFillFunction);
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

                // Bezier and TCB keys require squad interpolation.
                // Other key types utilize slerp interpolation.
                if (eType == NiRotKey::BEZKEY || eType == NiRotKey::TCBKEY)
                {
                    // Mark the alternate SP data flag for squad interpolation.
                    pkEvalSPData->SetAlternateSPData(true);
                    // Use the squad fill function in NiQuaternionEvaluator.
                    pkEvalSPData->SetSPFillFunc(
                        &NiQuaternionEvaluator::SquadRotFillFunction);

#ifdef NIDEBUG
                    // The fill function should set these extra fields.
                    NiScratchPad::SquadRotSegmentData* pkSquadSeg = 
                        (NiScratchPad::SquadRotSegmentData*)
                        pkEvalSPData->GetSPSegmentData();
                    NIASSERT((void*)&pkSquadSeg->m_kBaseData == 
                        (void*)pkSlerpSeg);
                    pkSquadSeg->m_kOutQuat0 = NiQuaternion::IDENTITY;
                    pkSquadSeg->m_kInQuat1 = NiQuaternion::IDENTITY;
#endif
                }
                else
                {
                    // Use the slerp fill function in NiQuaternionEvaluator.
                    pkEvalSPData->SetSPFillFunc(
                        &NiQuaternionEvaluator::SlerpRotFillFunction);
                }
            }
        }
    }
    else
    {
        NIASSERT(uiChannel == SCALE);

        // Partially initialize the scratch pad segment data.
        NiScratchPad::CubicFloatSegmentData* pkCubicSeg = 
            (NiScratchPad::CubicFloatSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicSeg);
        pkCubicSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
        if (IsReferencedEvaluator())
        {
            NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
            pkCubicSeg->m_kHeader.m_usOutputIndex = 
                NiScratchPad::INVALIDOUTPUTINDEX;
        }
        else
        {
            NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
            pkCubicSeg->m_kHeader.m_usOutputIndex = 
                kPBHandle.GetChannelIndex();
        }
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkCubicSeg->m_fStartTime = NI_INFINITY;
        pkCubicSeg->m_fInvDeltaTime = 0.0f;
        pkCubicSeg->m_fValue0 = 0.0f;
        pkCubicSeg->m_fOutTangent0 = 0.0f;
        pkCubicSeg->m_fA0 = 0.0f;
        pkCubicSeg->m_fB0 = 0.0f;
#endif

        if (bPosed)
        {
            // Mark the segment data as valid for all sequence times.
            pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
            NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

            // Fill the remainder of the scratch pad segment data.
            pkCubicSeg->m_fStartTime = 0.0f;
            pkCubicSeg->m_fInvDeltaTime = 0.0f;
            pkCubicSeg->m_fValue0 = m_kTransformValue.GetScale();
            pkCubicSeg->m_fOutTangent0 = 0.0f;
            pkCubicSeg->m_fA0 = 0.0f;
            pkCubicSeg->m_fB0 = 0.0f;
        }
        else
        {
            // Use the cubic fill function in NiFloatEvaluator.
            pkEvalSPData->SetSPFillFunc(
                &NiFloatEvaluator::CubicFloatFillFunction);

            // Initialize the scratch pad fill data.
            NiScratchPad::CubicFloatFillData* pkFillData = 
                (NiScratchPad::CubicFloatFillData*)
                pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);
            pkFillData->m_uiLastIndex = 0;
            NIASSERT(m_spData);
            pkFillData->m_pkKeys = m_spData->GetScaleAnim(
                pkFillData->m_uiNumKeys, pkFillData->m_eType, 
                pkFillData->m_ucKeySize);
            NIASSERT(pkFillData->m_pkKeys);
            NIASSERT(pkFillData->m_uiNumKeys > 0);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTransformEvaluator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;
    bool bKeys = false;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eTransType;
    unsigned char ucSize;
    NiPosKey* pkTransKeys = GetPosData(uiNumKeys, eTransType, ucSize);
    if (uiNumKeys > 0)
    {
        float fKeyTime = pkTransKeys->GetKeyAt(0, ucSize)->GetTime();
        if (fKeyTime < fBeginKeyTime)
        {
            fBeginKeyTime = fKeyTime;
        }
        fKeyTime = pkTransKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
        if (fKeyTime > fEndKeyTime)
        {
            fEndKeyTime = fKeyTime;
        }
        bKeys = true;
    }

    NiRotKey::KeyType eRotType;
    NiRotKey* pkRotKeys = GetRotData(uiNumKeys, eRotType, ucSize);
    if (uiNumKeys > 0)
    {
        if (eRotType == NiRotKey::EULERKEY)
        {
            NiEulerRotKey* pkEulerKey = (NiEulerRotKey*) pkRotKeys->GetKeyAt(
                0, ucSize);

            for (unsigned char uc = 0; uc < 3; uc++)
            {
                unsigned int uiNumFloatKeys = pkEulerKey->GetNumKeys(uc);
                if (uiNumFloatKeys > 0)
                {
                    NiFloatKey* pkFloatKeys = pkEulerKey->GetKeys(uc);
                    unsigned char ucFloatKeySize = pkEulerKey->GetKeySize(uc);
                    float fKeyTime = pkFloatKeys->GetKeyAt(0, 
                        ucFloatKeySize)->GetTime();
                    if (fKeyTime < fBeginKeyTime)
                    {
                        fBeginKeyTime = fKeyTime;
                    }
                    fKeyTime = pkFloatKeys->GetKeyAt(uiNumFloatKeys - 1, 
                        ucFloatKeySize)->GetTime();
                    if (fKeyTime > fEndKeyTime)
                    {
                        fEndKeyTime = fKeyTime;
                    }
                    bKeys = true;
                }
            }
        }
        else
        {
            float fKeyTime = pkRotKeys->GetKeyAt(0, ucSize)->GetTime();
            if (fKeyTime < fBeginKeyTime)
            {
                fBeginKeyTime = fKeyTime;
            }
            fKeyTime = pkRotKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
            if (fKeyTime > fEndKeyTime)
            {
                fEndKeyTime = fKeyTime;
            }
            bKeys = true;
        }
    }

    NiFloatKey::KeyType eScaleType;
    NiFloatKey* pkScaleKeys = GetScaleData(uiNumKeys, eScaleType, ucSize);
    if (uiNumKeys > 0)
    {
        float fKeyTime = pkScaleKeys->GetKeyAt(0, ucSize)->GetTime();
        if (fKeyTime < fBeginKeyTime)
        {
            fBeginKeyTime = fKeyTime;
        }
        fKeyTime = pkScaleKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
        if (fKeyTime > fEndKeyTime)
        {
            fEndKeyTime = fKeyTime;
        }
        bKeys = true;
    }

    if (!bKeys)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiTransformEvaluator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spData)
    {
        m_spData->GuaranteeKeysAtStartAndEnd(fStartTime, fEndTime);
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiTransformEvaluator::GetSequenceEvaluator(
    float fStartTime, float fEndTime)
{
    NiTransformEvaluator* pkSeqEval = (NiTransformEvaluator*)
        NiKeyBasedEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);

    if (m_spData)
    {
        NiTransformDataPtr spNewData = m_spData->GetSequenceData(fStartTime,
            fEndTime);
        pkSeqEval->SetTransformData(spNewData);
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
void NiTransformEvaluator::SetEvalChannelTypes()
{
    bool bPosKeyFrames = (GetKeyCount(POSITION) > 0);
    if (bPosKeyFrames || m_kTransformValue.IsTranslateValid())
    {
        m_aiEvalChannelTypes[POSITION] = EVALPOINT3CHANNEL;
        if (!bPosKeyFrames)
        {
            m_aiEvalChannelTypes[POSITION] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[POSITION] = EVALINVALIDCHANNEL;
    }

    bool bRotKeyFrames = (GetKeyCount(ROTATION) > 0);
    if (bRotKeyFrames || m_kTransformValue.IsRotateValid())
    {
        m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL;
        if (!bRotKeyFrames)
        {
            m_aiEvalChannelTypes[ROTATION] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[ROTATION] = EVALINVALIDCHANNEL;
    }

    bool bScaleKeyFrames = (GetKeyCount(SCALE) > 0);
    if (bScaleKeyFrames || m_kTransformValue.IsScaleValid())
    {
        m_aiEvalChannelTypes[SCALE] = EVALFLOATCHANNEL;
        if (!bScaleKeyFrames)
        {
            m_aiEvalChannelTypes[SCALE] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[SCALE] = EVALINVALIDCHANNEL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTransformEvaluator);
//---------------------------------------------------------------------------
void NiTransformEvaluator::CopyMembers(NiTransformEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kTransformValue = m_kTransformValue;

    pkDest->m_spData = m_spData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTransformEvaluator);
//---------------------------------------------------------------------------
void NiTransformEvaluator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::LoadBinary(kStream);

    m_kTransformValue.LoadBinary(kStream);

    m_spData = (NiTransformData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiTransformEvaluator::LinkObject(NiStream& kStream)
{
    NiKeyBasedEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTransformEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spData)
        m_spData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiTransformEvaluator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::SaveBinary(kStream);

    m_kTransformValue.SaveBinary(kStream);

    kStream.SaveLinkID(m_spData);
}
//---------------------------------------------------------------------------
bool NiTransformEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiTransformEvaluator* pkDest = (NiTransformEvaluator*) pkObject;

    if (m_kTransformValue != pkDest->m_kTransformValue)
    {
        return false;
    }

    if ((m_spData && !pkDest->m_spData) ||
        (!m_spData && pkDest->m_spData) ||
        (m_spData && !m_spData->IsEqual(pkDest->m_spData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiTransformEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiTransformEvaluator::ms_RTTI
        .GetName()));

    m_kTransformValue.GetViewerStrings(pkStrings);
    
    if (m_spData)
        m_spData->GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
