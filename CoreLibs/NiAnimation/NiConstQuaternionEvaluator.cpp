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

#include "NiConstQuaternionEvaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiConstQuaternionEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiConstQuaternionEvaluator::NiConstQuaternionEvaluator() : 
    m_kQuaternionValue(INVALID_QUATERNION)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiConstQuaternionEvaluator::NiConstQuaternionEvaluator(
    const NiQuaternion& kPoseValue) : m_kQuaternionValue(kPoseValue)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiConstQuaternionEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == ROTATION);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        *(NiQuaternion*)pvResult = m_kQuaternionValue;
        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstQuaternionEvaluator::UpdateChannel(float, 
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    void* pvResult) const
{
    NIASSERT(uiChannel == ROTATION);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NiScratchPad::SlerpRotSegmentData* pkSlerpSeg = 
            (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSlerpSeg);
        *(NiQuaternion*)pvResult = pkSlerpSeg->m_kQuat0;
        NIASSERT(pkSlerpSeg->m_kQuat1 == pkSlerpSeg->m_kQuat0);
        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstQuaternionEvaluator::GetChannelScratchPadInfo(
    unsigned int uiChannel, bool bForceAlwaysUpdate, 
    NiAVObjectPalette*, unsigned int& uiFillSize, 
    bool& bSharedFillData, NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == ROTATION);

    if (IsEvalChannelInvalid(uiChannel) ||
        (!bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = 0;
    bSharedFillData = false;
    eSPBSegmentData = SPBSLERPROTSEGMENT;
    pkBasisData = 0;
    return true;
}
//---------------------------------------------------------------------------
bool NiConstQuaternionEvaluator::InitChannelScratchPadData(
#ifdef NIDEBUG
    unsigned int uiChannel, 
#else
    unsigned int, 
#endif
    NiEvaluatorSPData* pkEvalSPData, 
    NiBSplineBasisData*, bool, 
    NiAVObjectPalette*, NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == ROTATION);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // Mark the segment data as valid for all sequence times.
    pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

    // Initialize the scratch pad segment data.
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
    pkSlerpSeg->m_fStartTime = 0.0f;
    pkSlerpSeg->m_fInvDeltaTime = 0.0f;
    pkSlerpSeg->m_kQuat0 = m_kQuaternionValue;
    pkSlerpSeg->m_kQuat1 = m_kQuaternionValue;

    return true;
}
//---------------------------------------------------------------------------
void NiConstQuaternionEvaluator::SetEvalChannelTypes()
{
    if (m_kQuaternionValue != INVALID_QUATERNION)
    {
        m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL | EVALPOSEDFLAG;
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
NiImplementCreateClone(NiConstQuaternionEvaluator);
//---------------------------------------------------------------------------
void NiConstQuaternionEvaluator::CopyMembers(
    NiConstQuaternionEvaluator* pkDest, NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kQuaternionValue = m_kQuaternionValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiConstQuaternionEvaluator);
//---------------------------------------------------------------------------
void NiConstQuaternionEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);

    m_kQuaternionValue.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiConstQuaternionEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiConstQuaternionEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiConstQuaternionEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);

    m_kQuaternionValue.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiConstQuaternionEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiConstQuaternionEvaluator* pkDest = 
        (NiConstQuaternionEvaluator*) pkObject;

    if (m_kQuaternionValue != pkDest->m_kQuaternionValue)
    {
        return false;
    }
        
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiConstQuaternionEvaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiConstQuaternionEvaluator::ms_RTTI
        .GetName()));

    pkStrings->Add(m_kQuaternionValue.GetViewerString("m_kQuaternionValue"));
}
//---------------------------------------------------------------------------
