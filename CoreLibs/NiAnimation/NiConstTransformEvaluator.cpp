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

#include "NiConstTransformEvaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiConstTransformEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiConstTransformEvaluator::NiConstTransformEvaluator() :
    m_kTransformValue()
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    // No need to adjust the other eval channel types.
}
//---------------------------------------------------------------------------
NiConstTransformEvaluator::NiConstTransformEvaluator(
    const NiQuatTransform& kPoseValue) :
    m_kTransformValue(kPoseValue)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiConstTransformEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
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

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstTransformEvaluator::UpdateChannel(float, 
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    void* pvResult) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        if (uiChannel == SCALE)
        {
            NiScratchPad::CubicFloatSegmentData* pkCubicSeg = 
                (NiScratchPad::CubicFloatSegmentData*)
                pkEvalSPData->GetSPSegmentData();
            NIASSERT(pkCubicSeg);
            *(float*)pvResult = pkCubicSeg->m_fValue0;
            NIASSERT(pkCubicSeg->m_fOutTangent0 == 0.0f);
            NIASSERT(pkCubicSeg->m_fA0 == 0.0f);
            NIASSERT(pkCubicSeg->m_fB0 == 0.0f);
        }
        else if (uiChannel == POSITION)
        {
            NiScratchPad::CubicPoint3SegmentData* pkCubicSeg = 
                (NiScratchPad::CubicPoint3SegmentData*)
                pkEvalSPData->GetSPSegmentData();
            NIASSERT(pkCubicSeg);
            *(NiPoint3*)pvResult = pkCubicSeg->m_kValue0;
            NIASSERT(pkCubicSeg->m_kOutTangent0 == NiPoint3::ZERO);
            NIASSERT(pkCubicSeg->m_kA0 == NiPoint3::ZERO);
            NIASSERT(pkCubicSeg->m_kB0 == NiPoint3::ZERO);
        }
        else
        {
            NIASSERT(uiChannel == ROTATION);
            NiScratchPad::SlerpRotSegmentData* pkSlerpSeg = 
                (NiScratchPad::SlerpRotSegmentData*)
                pkEvalSPData->GetSPSegmentData();
            NIASSERT(pkSlerpSeg);
            *(NiQuaternion*)pvResult = pkSlerpSeg->m_kQuat0;
            NIASSERT(pkSlerpSeg->m_kQuat1 == pkSlerpSeg->m_kQuat0);
        }

        return true;
    }

    NIASSERT(IsEvalChannelInvalid(uiChannel));
    return false;
}
//---------------------------------------------------------------------------
bool NiConstTransformEvaluator::GetChannelScratchPadInfo(
    unsigned int uiChannel, bool bForceAlwaysUpdate, 
    NiAVObjectPalette*, unsigned int& uiFillSize, 
    bool& bSharedFillData, NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);

    if (IsEvalChannelInvalid(uiChannel) ||
        (!bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    uiFillSize = 0;
    bSharedFillData = false;
    pkBasisData = 0;

    if (uiChannel == POSITION)
    {
        eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
    }
    else if (uiChannel == ROTATION)
    {
        eSPBSegmentData = SPBSLERPROTSEGMENT;
    }
    else
    {
        NIASSERT(uiChannel == SCALE);
        eSPBSegmentData = SPBCUBICFLOATSEGMENT;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiConstTransformEvaluator::InitChannelScratchPadData(
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    NiBSplineBasisData*, bool, 
    NiAVObjectPalette*, NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(IsRawEvalChannelPosed(uiChannel));

    // Mark the segment data as valid for all sequence times.
    pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

    // Initialize the scrach pad based on channel type.
    if (uiChannel == POSITION)
    {
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
        pkCubicSeg->m_fStartTime = 0.0f;
        pkCubicSeg->m_fInvDeltaTime = 0.0f;
        pkCubicSeg->m_kValue0 = m_kTransformValue.GetTranslate();
        pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
        pkCubicSeg->m_kA0 = NiPoint3::ZERO;
        pkCubicSeg->m_kB0 = NiPoint3::ZERO;
    }
    else if (uiChannel == ROTATION)
    {
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
        pkSlerpSeg->m_kQuat0 = m_kTransformValue.GetRotate();
        pkSlerpSeg->m_kQuat1 = m_kTransformValue.GetRotate();
    }
    else
    {
        NIASSERT(uiChannel == SCALE);

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
        pkCubicSeg->m_fStartTime = 0.0f;
        pkCubicSeg->m_fInvDeltaTime = 0.0f;
        pkCubicSeg->m_fValue0 = m_kTransformValue.GetScale();
        pkCubicSeg->m_fOutTangent0 = 0.0f;
        pkCubicSeg->m_fA0 = 0.0f;
        pkCubicSeg->m_fB0 = 0.0f;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiConstTransformEvaluator::SetEvalChannelTypes()
{
    if (m_kTransformValue.IsTranslateValid())
    {
        m_aiEvalChannelTypes[POSITION] = EVALPOINT3CHANNEL | EVALPOSEDFLAG;
    }
    else
    {
        m_aiEvalChannelTypes[POSITION] = EVALINVALIDCHANNEL;
    }

    if (m_kTransformValue.IsRotateValid())
    {
        m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL | EVALPOSEDFLAG;
    }
    else
    {
        m_aiEvalChannelTypes[ROTATION] = EVALINVALIDCHANNEL;
    }

    if (m_kTransformValue.IsScaleValid())
    {
        m_aiEvalChannelTypes[SCALE] = EVALFLOATCHANNEL | EVALPOSEDFLAG;
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
NiImplementCreateClone(NiConstTransformEvaluator);
//---------------------------------------------------------------------------
void NiConstTransformEvaluator::CopyMembers(NiConstTransformEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kTransformValue = m_kTransformValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiConstTransformEvaluator);
//---------------------------------------------------------------------------
void NiConstTransformEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);

    m_kTransformValue.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiConstTransformEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiConstTransformEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiConstTransformEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);

    m_kTransformValue.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiConstTransformEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiConstTransformEvaluator* pkDest = (NiConstTransformEvaluator*) pkObject;

    if (m_kTransformValue != pkDest->m_kTransformValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiConstTransformEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiConstTransformEvaluator::ms_RTTI
        .GetName()));

    m_kTransformValue.GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
