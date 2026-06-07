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

#include "NiColorEvaluator.h"
#include "NiLinColorKey.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiColorEvaluator, NiKeyBasedEvaluator);

//---------------------------------------------------------------------------
NiColorEvaluator::NiColorEvaluator() :
    m_spColorData(NULL)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiColorEvaluator::NiColorEvaluator(NiColorData* pkColorData) :
    m_spColorData(pkColorData)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
unsigned short NiColorEvaluator::GetKeyChannelCount() const 
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiColorEvaluator::GetKeyCount(unsigned short) const
{
    if (!m_spColorData)
        return 0;
    else
        return m_spColorData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiColorEvaluator::GetKeyType(
    unsigned short) const
{
    if (!m_spColorData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiColorEvaluator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::COLORKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiColorEvaluator::GetKeyArray(unsigned short) const 
{
    if (!m_spColorData)
        return NULL;

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    NiColorKey * pkKeys = m_spColorData->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiColorEvaluator::GetKeyStride(unsigned short) const 
{
    if (!m_spColorData)
        return 0;

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
void NiColorEvaluator::Collapse()
{
    if (m_spColorData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiColorKey* pkKeys = m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spColorData = NULL;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiColorKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                NiColorKey* pkLinKeys = 
                    (NiColorKey*)NiNew NiLinColorKey[1];
                pkLinKeys[0].SetTime(0.0f);
                NiColorA kValue = pkKeys->GetKeyAt(0, ucSize)->GetColor();
                pkLinKeys[0].SetColor(kValue);                    
                m_spColorData->ReplaceAnim(pkLinKeys, 1, 
                    NiAnimationKey::LINKEY);
            }
        }
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiColorEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(m_spColorData);
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiColorKey* pkKeys = m_spColorData->GetAnim(uiNumKeys, 
            eType, ucSize);
        NIASSERT(uiNumKeys == 1);
        *(NiColorA*)pvResult = pkKeys->GetKeyAt(0, ucSize)->GetColor();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiColorEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel == COLORA);
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
    NiScratchPad::LinearColorSegmentData* pkLinearSeg = 
        (NiScratchPad::LinearColorSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkLinearSeg);
    float fT = (fTime - pkLinearSeg->m_fStartTime) * 
        pkLinearSeg->m_fInvDeltaTime;
    *(NiColorA*)pvResult = pkLinearSeg->m_kValue0 + 
        pkLinearSeg->m_kOutTangent0 * fT;

    return true;
}
//---------------------------------------------------------------------------
bool NiColorEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == COLORA);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = bPosed ? 0 : sizeof(NiScratchPad::LinearColorFillData);
    bSharedFillData = false;
    eSPBSegmentData = SPBLINEARCOLORSEGMENT;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiColorEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_spColorData);

    // Partially initialize the scratch pad segment data.
    NiScratchPad::LinearColorSegmentData* pkLinearSeg = 
        (NiScratchPad::LinearColorSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkLinearSeg);
    pkLinearSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
    if (IsReferencedEvaluator())
    {
        NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
        pkLinearSeg->m_kHeader.m_usOutputIndex = 
            NiScratchPad::INVALIDOUTPUTINDEX;
    }
    else
    {
        NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
        pkLinearSeg->m_kHeader.m_usOutputIndex = 
            kPBHandle.GetChannelIndex();
    }
#ifdef NIDEBUG
    // The fill function should set these fields.
    pkLinearSeg->m_fStartTime = NI_INFINITY;
    pkLinearSeg->m_fInvDeltaTime = 0.0f;
    pkLinearSeg->m_kValue0 = NiColorA(0, 0, 0, 0);
    pkLinearSeg->m_kOutTangent0 = NiColorA(0, 0, 0, 0);
#endif

    unsigned int uiNumKeys;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;
    NiColorKey* pkKeys = m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
    NIASSERT(pkKeys);
    NIASSERT(uiNumKeys > 0);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (bPosed)
    {
        // Mark the segment data as valid for all sequence times.
        pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

        // Fill the remainder of the scratch pad segment data.
        pkLinearSeg->m_fStartTime = 0.0f;
        pkLinearSeg->m_fInvDeltaTime = 0.0f;
        pkLinearSeg->m_kValue0 = pkKeys->GetKeyAt(0, ucSize)->GetColor();
        pkLinearSeg->m_kOutTangent0 = NiColorA(0, 0, 0, 0);
    }
    else
    {
        if (eType == NiAnimationKey::STEPKEY)
        {
            pkEvalSPData->SetStepFunction(true);
        }
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
        pkEvalSPData->SetSPFillFunc(&LinearColorFillFunction);

        // Initialize the scratch pad fill data.
        NiScratchPad::LinearColorFillData* pkFillData = 
            (NiScratchPad::LinearColorFillData*)pkEvalSPData->GetSPFillData();
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
bool NiColorEvaluator::LinearColorFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::LinearColorFillData* pkFillData = 
        (NiScratchPad::LinearColorFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT(pkFillData->m_pkKeys);
    NIASSERT(pkFillData->m_uiNumKeys > 0);

    // Fill the scratch pad with the cubic segment matching fTime.
    NiScratchPad::LinearColorSegmentData* pkLinearSeg = 
        (NiScratchPad::LinearColorSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkLinearSeg);
    float fMinTime;
    float fMaxTime;
    NiColorA kA0;
    NiColorA kB0;
    NiColorKey::GenCubicCoefs(fTime, pkFillData->m_pkKeys, 
        pkFillData->m_eType, pkFillData->m_uiNumKeys, 
        pkFillData->m_uiLastIndex, pkFillData->m_ucKeySize, 
        fMinTime, fMaxTime, pkLinearSeg->m_kValue0, 
        pkLinearSeg->m_kOutTangent0, kA0, kB0);
    NIASSERT(kA0 == NiColorA(0, 0, 0, 0));
    NIASSERT(kB0 == NiColorA(0, 0, 0, 0));
    pkLinearSeg->m_fStartTime = fMinTime;
    pkLinearSeg->m_fInvDeltaTime = (fMaxTime != fMinTime) ? 
        1.0f / (fMaxTime - fMinTime) : 0.0f;

    // Update the time range to match the segment.
    pkEvalSPData->SetSPSegmentTimeRange(fMinTime, fMaxTime);

    return true;
}
//---------------------------------------------------------------------------
void NiColorEvaluator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spColorData)
    {
        m_spColorData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiColorEvaluator::GetSequenceEvaluator(
    float fStartTime, float fEndTime)
{
    NiColorEvaluator* pkSeqEval = (NiColorEvaluator*)
        NiKeyBasedEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);
    if (m_spColorData)
    {
        NiColorDataPtr spNewColorData = m_spColorData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqEval->SetColorData(spNewColorData);
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
void NiColorEvaluator::SetEvalChannelTypes()
{
    if (m_spColorData && m_spColorData->GetNumKeys() > 0)
    {
        m_aiEvalChannelTypes[COLORA] = EVALCOLORCHANNEL;

        if (m_spColorData->GetNumKeys() == 1)
        {
            m_aiEvalChannelTypes[COLORA] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[COLORA] = EVALINVALIDCHANNEL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiColorEvaluator);
//---------------------------------------------------------------------------
void NiColorEvaluator::CopyMembers(NiColorEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_spColorData = m_spColorData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiColorEvaluator);
//---------------------------------------------------------------------------
void NiColorEvaluator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::LoadBinary(kStream);

    m_spColorData = (NiColorData*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiColorEvaluator::LinkObject(NiStream& kStream)
{
    NiKeyBasedEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiColorEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spColorData)
    {
        m_spColorData->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiColorEvaluator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedEvaluator::SaveBinary(kStream);

    kStream.SaveLinkID(m_spColorData);
}
//---------------------------------------------------------------------------
bool NiColorEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiColorEvaluator* pkDest = (NiColorEvaluator*) pkObject;

    if ((m_spColorData && !pkDest->m_spColorData) ||
        (!m_spColorData && pkDest->m_spColorData) ||
        (m_spColorData && !m_spColorData->IsEqual(pkDest->m_spColorData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiColorEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiColorEvaluator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_spColorData", m_spColorData));
}
//---------------------------------------------------------------------------
