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

#include "NiPoint3Evaluator.h"
#include "NiLinPosKey.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiPoint3Evaluator, NiKeyBasedEvaluator);

//---------------------------------------------------------------------------
NiPoint3Evaluator::NiPoint3Evaluator() :
    m_spPoint3Data(NULL)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiPoint3Evaluator::NiPoint3Evaluator(NiPosData* pkPosData) :
    m_spPoint3Data(pkPosData)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
unsigned short NiPoint3Evaluator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiPoint3Evaluator::GetKeyCount(unsigned short) const 
{
    if (!m_spPoint3Data)
        return 0;
    else
        return m_spPoint3Data->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiPoint3Evaluator::GetKeyType(
    unsigned short) const
{
    if (!m_spPoint3Data)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiPoint3Evaluator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::POSKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiPoint3Evaluator::GetKeyArray(unsigned short)
    const
{
    if (!m_spPoint3Data)
        return NULL;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    NiPosKey * pkKeys = m_spPoint3Data->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiPoint3Evaluator::GetKeyStride(unsigned short)
    const
{
    if (!m_spPoint3Data)
        return 0;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
void NiPoint3Evaluator::Collapse()
{
    if (m_spPoint3Data)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiPosKey* pkKeys = m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spPoint3Data = NULL;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiPosKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                NiPosKey* pkLinKeys = 
                    (NiPosKey*)NiNew NiLinPosKey[1];
                pkLinKeys[0].SetTime(0.0f);
                NiPoint3 kValue = pkKeys->GetKeyAt(0, ucSize)->GetPos();
                pkLinKeys[0].SetPos(kValue); 
                m_spPoint3Data->ReplaceAnim(pkLinKeys, 1, 
                    NiAnimationKey::LINKEY);
            }
        }
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == POINT3);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(m_spPoint3Data);
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiPosKey* pkKeys = m_spPoint3Data->GetAnim(uiNumKeys, 
            eType, ucSize);
        NIASSERT(uiNumKeys == 1)
        *(NiPoint3*)pvResult = pkKeys->GetKeyAt(0, ucSize)->GetPos();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel == POINT3);
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

    // Compute the value based on the cubic segment data.
    NiScratchPad::CubicPoint3SegmentData* pkCubicSeg = 
        (NiScratchPad::CubicPoint3SegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkCubicSeg);
    float fT = (fTime - pkCubicSeg->m_fStartTime) * 
        pkCubicSeg->m_fInvDeltaTime;
    *(NiPoint3*)pvResult = pkCubicSeg->m_kValue0 + 
        (pkCubicSeg->m_kOutTangent0 + (pkCubicSeg->m_kA0 + 
        pkCubicSeg->m_kB0 * fT) * fT) * fT;

    return true;
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == POINT3);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = bPosed ? 0 : sizeof(NiScratchPad::CubicPoint3FillData);
    bSharedFillData = false;
    eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == POINT3);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_spPoint3Data);

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

    unsigned int uiNumKeys;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;
    NiPosKey* pkKeys = m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (bPosed)
    {
        // Mark the segment data as valid for all sequence times.
        pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

        // Fill the remainder of the scratch pad segment data.
        pkCubicSeg->m_fStartTime = 0.0f;
        pkCubicSeg->m_fInvDeltaTime = 0.0f;
        pkCubicSeg->m_kValue0 = pkKeys->GetKeyAt(0, ucSize)->GetPos();
        pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
        pkCubicSeg->m_kA0 = NiPoint3::ZERO;
        pkCubicSeg->m_kB0 = NiPoint3::ZERO;
    }
    else
    {
        if (eType == NiAnimationKey::STEPKEY)
        {
            pkEvalSPData->SetStepFunction(true);
        }
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
        pkEvalSPData->SetSPFillFunc(&CubicPoint3FillFunction);

        // Initialize the scratch pad fill data.
        NiScratchPad::CubicPoint3FillData* pkFillData = 
            (NiScratchPad::CubicPoint3FillData*)pkEvalSPData->GetSPFillData();
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
void NiPoint3Evaluator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spPoint3Data)
    {
        m_spPoint3Data->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiPoint3Evaluator::GetSequenceEvaluator(
    float fStartTime, float fEndTime)
{
    NiPoint3Evaluator* pkSeqEval = (NiPoint3Evaluator*)
        NiKeyBasedEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);
    if (m_spPoint3Data)
    {
        NiPosDataPtr spNewPoint3Data = m_spPoint3Data->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqEval->SetPoint3Data(spNewPoint3Data);
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::CubicPoint3FillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::CubicPoint3FillData* pkFillData = 
        (NiScratchPad::CubicPoint3FillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT(pkFillData->m_pkKeys);
    NIASSERT(pkFillData->m_uiNumKeys > 0);

    // Fill the scratch pad with the cubic segment matching fTime.
    NiScratchPad::CubicPoint3SegmentData* pkCubicSeg = 
        (NiScratchPad::CubicPoint3SegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkCubicSeg);
    float fMinTime;
    float fMaxTime;
    NiPosKey::GenCubicCoefs(fTime, pkFillData->m_pkKeys, 
        pkFillData->m_eType, pkFillData->m_uiNumKeys, 
        pkFillData->m_uiLastIndex, pkFillData->m_ucKeySize, 
        fMinTime, fMaxTime, pkCubicSeg->m_kValue0, 
        pkCubicSeg->m_kOutTangent0, pkCubicSeg->m_kA0, 
        pkCubicSeg->m_kB0);
    pkCubicSeg->m_fStartTime = fMinTime;
    pkCubicSeg->m_fInvDeltaTime = (fMaxTime != fMinTime) ? 
        1.0f / (fMaxTime - fMinTime) : 0.0f;

    // Update the time range to match the segment.
    pkEvalSPData->SetSPSegmentTimeRange(fMinTime, fMaxTime);

    return true;
}
//---------------------------------------------------------------------------
void NiPoint3Evaluator::SetEvalChannelTypes()
{
    if (m_spPoint3Data && m_spPoint3Data->GetNumKeys() > 0)
    {
        m_aiEvalChannelTypes[POINT3] = EVALPOINT3CHANNEL;

        if (m_spPoint3Data->GetNumKeys() == 1)
        {
            m_aiEvalChannelTypes[POINT3] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[POINT3] = EVALINVALIDCHANNEL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPoint3Evaluator);
//---------------------------------------------------------------------------
void NiPoint3Evaluator::CopyMembers(NiPoint3Evaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_spPoint3Data = m_spPoint3Data;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPoint3Evaluator);
//---------------------------------------------------------------------------
void NiPoint3Evaluator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::LoadBinary(kStream);

    m_spPoint3Data = (NiPosData*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPoint3Evaluator::LinkObject(NiStream& kStream)
{
    NiKeyBasedEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spPoint3Data)
    {
        m_spPoint3Data->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPoint3Evaluator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::SaveBinary(kStream);

    kStream.SaveLinkID(m_spPoint3Data);
}
//---------------------------------------------------------------------------
bool NiPoint3Evaluator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiPoint3Evaluator* pkDest = (NiPoint3Evaluator*) pkObject;

    if ((m_spPoint3Data && !pkDest->m_spPoint3Data) ||
        (!m_spPoint3Data && pkDest->m_spPoint3Data) ||
        (m_spPoint3Data && !m_spPoint3Data->IsEqual(pkDest->m_spPoint3Data)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPoint3Evaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPoint3Evaluator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_spPoint3Data", m_spPoint3Data));
}
//---------------------------------------------------------------------------
