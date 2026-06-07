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

#include "NiBoolEvaluator.h"
#include "NiStepBoolKey.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBoolEvaluator, NiKeyBasedEvaluator);

//---------------------------------------------------------------------------
NiBoolEvaluator::NiBoolEvaluator() :
    m_spBoolData(NULL)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiBoolEvaluator::NiBoolEvaluator(NiBoolData* pkBoolData) :
    m_spBoolData(pkBoolData)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
unsigned short NiBoolEvaluator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiBoolEvaluator::GetKeyCount(unsigned short) const
{
    if (!m_spBoolData)
        return 0;
    else
        return m_spBoolData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiBoolEvaluator::GetKeyType(
    unsigned short) const
{
    if (!m_spBoolData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiBoolKey::KeyType eType;
    unsigned char ucSize;
    m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiBoolEvaluator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::BOOLKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiBoolEvaluator::GetKeyArray(unsigned short)
    const 
{
    if (!m_spBoolData)
        return NULL;

    unsigned int uiNumKeys;
    NiBoolKey::KeyType eType;
    unsigned char ucSize;
    NiBoolKey* pkKeys = m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiBoolEvaluator::GetKeyStride(unsigned short) const 
{
    if (!m_spBoolData)
        return 0;

    unsigned int uiNumKeys;
    NiBoolKey::KeyType eType;
    unsigned char ucSize;
    m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
void NiBoolEvaluator::Collapse()
{
    if (m_spBoolData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiBoolKey* pkKeys = m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spBoolData = NULL;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiBoolKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                NiBoolKey* pkStepKeys = 
                    (NiBoolKey*)NiNew NiStepBoolKey[1];
                pkStepKeys[0].SetTime(0.0f);
                bool bValue = pkKeys->GetKeyAt(0, ucSize)->GetBool();
                pkStepKeys[0].SetBool(bValue);
                m_spBoolData->ReplaceAnim(pkStepKeys, 1, 
                    NiAnimationKey::STEPKEY);
            }
        }
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == BOOL);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(m_spBoolData);
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiBoolKey* pkKeys = m_spBoolData->GetAnim(uiNumKeys, 
            eType, ucSize);
        NIASSERT(uiNumKeys == 1);
        *(float*)pvResult = pkKeys->GetKeyAt(0, ucSize)->GetBool() ? 
            1.0f : 0.0f;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel == BOOL);
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
        NiScratchPadFillFunc pfnFillFunc = pkEvalSPData->GetSPFillFunc();
        if (!pfnFillFunc || !(*pfnFillFunc)(fTime, pkEvalSPData))
        {
            return false;
        }
    }

    // Copy the value in the constant segment data.
    NiScratchPad::ConstantBoolSegmentData* pkConstantSeg = 
        (NiScratchPad::ConstantBoolSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkConstantSeg);
    *(float*)pvResult = pkConstantSeg->m_fValue0;

    return true;
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == BOOL);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = bPosed ? 0 : sizeof(NiScratchPad::ConstantBoolFillData);
    bSharedFillData = false;
    eSPBSegmentData = SPBCONSTANTBOOLSEGMENT;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == BOOL);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_spBoolData);

    // Partially initialize the scratch pad segment data.
    NiScratchPad::ConstantBoolSegmentData* pkConstantSeg = 
        (NiScratchPad::ConstantBoolSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkConstantSeg);
    pkConstantSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
    if (IsReferencedEvaluator())
    {
        NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
        pkConstantSeg->m_kHeader.m_usOutputIndex = 
            NiScratchPad::INVALIDOUTPUTINDEX;
    }
    else
    {
        NIASSERT(kPBHandle.GetChannelType() == PBBOOLCHANNEL);
        pkConstantSeg->m_kHeader.m_usOutputIndex = 
            kPBHandle.GetChannelIndex();
    }
#ifdef NIDEBUG
    // The fill function should set these fields.
    pkConstantSeg->m_fValue0 = 0.0f;
#endif

    unsigned int uiNumKeys;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;
    NiBoolKey* pkKeys = m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (bPosed)
    {
        // Mark the segment data as valid for all sequence times.
        pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

        // Fill the remainder of the scratch pad segment data.
        pkConstantSeg->m_fValue0 = pkKeys->GetKeyAt(0, ucSize)->GetBool() ? 
            1.0f : 0.0f;
    }
    else
    {
        if (eType == NiAnimationKey::STEPKEY)
        {
            pkEvalSPData->SetStepFunction(true);
        }
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
        pkEvalSPData->SetSPFillFunc(&ConstantBoolFillFunction);

        // Initialize the scratch pad fill data.
        NiScratchPad::ConstantBoolFillData* pkFillData = 
            (NiScratchPad::ConstantBoolFillData*)pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);
        pkFillData->m_uiLastIndex = 0;
        pkFillData->m_pkKeys = pkKeys;
        pkFillData->m_uiNumKeys = uiNumKeys;
        pkFillData->m_eType = eType;
        pkFillData->m_ucKeySize = ucSize;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::ConstantBoolFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::ConstantBoolFillData* pkFillData = 
        (NiScratchPad::ConstantBoolFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT(pkFillData->m_pkKeys);
    NIASSERT(pkFillData->m_uiNumKeys > 0);

    // Fill the scratch pad with the constant segment matching fTime.
    NiScratchPad::ConstantBoolSegmentData* pkConstantSeg = 
        (NiScratchPad::ConstantBoolSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkConstantSeg);
    float fMinTime;
    float fMaxTime;
    float fOutTangent0;
    float fA0;
    float fB0;
    NiBoolKey::GenCubicCoefs(fTime, pkFillData->m_pkKeys, 
        pkFillData->m_eType, pkFillData->m_uiNumKeys, 
        pkFillData->m_uiLastIndex, pkFillData->m_ucKeySize, 
        fMinTime, fMaxTime, pkConstantSeg->m_fValue0, 
        fOutTangent0, fA0, fB0);
    NIASSERT(fOutTangent0 == 0.0f);
    NIASSERT(fA0 == 0.0f);
    NIASSERT(fB0 == 0.0f);

    // Update the time range to match the segment.
    pkEvalSPData->SetSPSegmentTimeRange(fMinTime, fMaxTime);

    return true;
}
//---------------------------------------------------------------------------
void NiBoolEvaluator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spBoolData)
    {
        m_spBoolData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiBoolEvaluator::GetSequenceEvaluator(float fStartTime,
    float fEndTime)
{
    NiBoolEvaluator* pkSeqEval = (NiBoolEvaluator*)
        NiKeyBasedEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);
    if (m_spBoolData)
    {
        NiBoolDataPtr spNewBoolData = m_spBoolData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqEval->SetBoolData(spNewBoolData);
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
void NiBoolEvaluator::SetEvalChannelTypes()
{
    if (m_spBoolData && m_spBoolData->GetNumKeys() > 0)
    {
        m_aiEvalChannelTypes[BOOL] = EVALBOOLCHANNEL;

        if (m_spBoolData->GetNumKeys() == 1)
        {
            m_aiEvalChannelTypes[BOOL] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[BOOL] = EVALINVALIDCHANNEL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBoolEvaluator);
//---------------------------------------------------------------------------
void NiBoolEvaluator::CopyMembers(NiBoolEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_spBoolData = m_spBoolData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBoolEvaluator);
//---------------------------------------------------------------------------
void NiBoolEvaluator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::LoadBinary(kStream);

    m_spBoolData = (NiBoolData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiBoolEvaluator::LinkObject(NiStream& kStream)
{
    NiKeyBasedEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spBoolData)
    {
        m_spBoolData->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoolEvaluator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::SaveBinary(kStream);

    kStream.SaveLinkID(m_spBoolData);
}
//---------------------------------------------------------------------------
bool NiBoolEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBoolEvaluator* pkDest = (NiBoolEvaluator*) pkObject;

    if ((m_spBoolData && !pkDest->m_spBoolData) ||
        (!m_spBoolData && pkDest->m_spBoolData) ||
        (m_spBoolData && !m_spBoolData->IsEqual(pkDest->m_spBoolData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBoolEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBoolEvaluator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_spBoolData", m_spBoolData));
}
//---------------------------------------------------------------------------
