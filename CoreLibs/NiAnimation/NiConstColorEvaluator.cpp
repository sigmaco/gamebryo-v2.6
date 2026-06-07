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

#include "NiConstColorEvaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiConstColorEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiConstColorEvaluator::NiConstColorEvaluator() :
    m_kColorValue(INVALID_COLORA)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiConstColorEvaluator::NiConstColorEvaluator(const NiColorA& kPoseValue) :
    m_kColorValue(kPoseValue)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiConstColorEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        *(NiColorA*)pvResult = m_kColorValue;
        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstColorEvaluator::UpdateChannel(float, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NiScratchPad::LinearColorSegmentData* pkLinearSeg = 
            (NiScratchPad::LinearColorSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkLinearSeg);
        *(NiColorA*)pvResult = pkLinearSeg->m_kValue0;
        NIASSERT(pkLinearSeg->m_kOutTangent0 == NiColorA(0, 0, 0, 0));
        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstColorEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == COLORA);

    if (IsEvalChannelInvalid(uiChannel) ||
        (!bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = 0;
    bSharedFillData = false;
    eSPBSegmentData = SPBLINEARCOLORSEGMENT;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiConstColorEvaluator::InitChannelScratchPadData(
#ifdef NIDEBUG
    unsigned int uiChannel, 
#else
    unsigned int, 
#endif
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // Mark the segment data as valid for all sequence times.
    pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

    // Initialize the scratch pad segment data.
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
    pkLinearSeg->m_fStartTime = 0.0f;
    pkLinearSeg->m_fInvDeltaTime = 0.0f;
    pkLinearSeg->m_kValue0 = m_kColorValue;
    pkLinearSeg->m_kOutTangent0 = NiColorA(0, 0, 0, 0);

    return true;
}
//---------------------------------------------------------------------------
void NiConstColorEvaluator::SetEvalChannelTypes()
{
    if (m_kColorValue != INVALID_COLORA)
    {
        m_aiEvalChannelTypes[COLORA] = EVALCOLORCHANNEL | EVALPOSEDFLAG;
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
NiImplementCreateClone(NiConstColorEvaluator);
//---------------------------------------------------------------------------
void NiConstColorEvaluator::CopyMembers(NiConstColorEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kColorValue = m_kColorValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiConstColorEvaluator);
//---------------------------------------------------------------------------
void NiConstColorEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);

    m_kColorValue.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiConstColorEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiConstColorEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiConstColorEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);

    m_kColorValue.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiConstColorEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiConstColorEvaluator* pkDest = (NiConstColorEvaluator*)pkObject;

    if (m_kColorValue != pkDest->m_kColorValue)
        return false;
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiConstColorEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiConstColorEvaluator::ms_RTTI.GetName()));

    pkStrings->Add(m_kColorValue.GetViewerString("m_kColorValue"));
}
//---------------------------------------------------------------------------
