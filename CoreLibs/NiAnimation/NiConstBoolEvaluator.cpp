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

#include "NiConstBoolEvaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiConstBoolEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiConstBoolEvaluator::NiConstBoolEvaluator() :
    m_fBoolValue(INVALID_FLOAT)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiConstBoolEvaluator::NiConstBoolEvaluator(float fPosedBool) :
    m_fBoolValue(fPosedBool)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiConstBoolEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == BOOL);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        *(float*)pvResult = m_fBoolValue;
        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstBoolEvaluator::UpdateChannel(float, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
{
    NIASSERT(uiChannel == BOOL);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NiScratchPad::ConstantBoolSegmentData* pkConstantSeg = 
            (NiScratchPad::ConstantBoolSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkConstantSeg);
        *(float*)pvResult = pkConstantSeg->m_fValue0;
        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstBoolEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette*, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == BOOL);

    if (IsEvalChannelInvalid(uiChannel) ||
        (!bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = 0;
    bSharedFillData = false;
    eSPBSegmentData = SPBCONSTANTBOOLSEGMENT;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiConstBoolEvaluator::InitChannelScratchPadData(
#ifdef NIDEBUG
    unsigned int uiChannel, 
#else
    unsigned int, 
#endif
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == BOOL);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // Mark the segment data as valid for all sequence times.
    pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

    // Initialize the scratch pad segment data.
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
    pkConstantSeg->m_fValue0 = m_fBoolValue;

    return true;
}
//---------------------------------------------------------------------------
void NiConstBoolEvaluator::SetEvalChannelTypes()
{
    if (m_fBoolValue != INVALID_FLOAT)
    {
        m_aiEvalChannelTypes[BOOL] = EVALBOOLCHANNEL | EVALPOSEDFLAG;
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
NiImplementCreateClone(NiConstBoolEvaluator);
//---------------------------------------------------------------------------
void NiConstBoolEvaluator::CopyMembers(NiConstBoolEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_fBoolValue = m_fBoolValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiConstBoolEvaluator);
//---------------------------------------------------------------------------
void NiConstBoolEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fBoolValue);
}
//---------------------------------------------------------------------------
void NiConstBoolEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiConstBoolEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiConstBoolEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fBoolValue);
}
//---------------------------------------------------------------------------
bool NiConstBoolEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiConstBoolEvaluator* pkDest = (NiConstBoolEvaluator*) pkObject;

    if (m_fBoolValue != pkDest->m_fBoolValue)
        return false;
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiConstBoolEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiConstBoolEvaluator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_fBoolValue", m_fBoolValue));
}
//---------------------------------------------------------------------------
