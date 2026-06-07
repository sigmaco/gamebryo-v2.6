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

#include "NiBSplineCompTransformEvaluator.h"
#include <NiCompUtility.h>
#include "NiBSplineCompFloatEvaluator.h"
#include "NiBSplineCompPoint3Evaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBSplineCompTransformEvaluator,
    NiBSplineTransformEvaluator);

//---------------------------------------------------------------------------
NiBSplineCompTransformEvaluator::NiBSplineCompTransformEvaluator() : 
    NiBSplineTransformEvaluator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiBSplineCompTransformEvaluator::NiBSplineCompTransformEvaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kTransCPHandle,
    NiBSplineData::Handle kRotCPHandle, NiBSplineData::Handle kScaleCPHandle,
    NiBSplineBasisData* pkBasisData) : 
    NiBSplineTransformEvaluator(pkData, kTransCPHandle,
    kRotCPHandle, kScaleCPHandle, pkBasisData, true)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiBSplineCompTransformEvaluator::NiBSplineCompTransformEvaluator(
    NiQuatTransform kPoseValue) : 
    NiBSplineTransformEvaluator(kPoseValue, true)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformEvaluator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------
float NiBSplineCompTransformEvaluator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POSITION:
            return m_afCompScalars[POSITION_OFFSET];
        case ROTATION:
            return m_afCompScalars[ROTATION_OFFSET];
        case SCALE:
            return m_afCompScalars[SCALE_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POSITION:
            m_afCompScalars[POSITION_OFFSET] = fOffset;
            break;
        case ROTATION:
            m_afCompScalars[ROTATION_OFFSET] = fOffset;
            break;
        case SCALE:
            m_afCompScalars[SCALE_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompTransformEvaluator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POSITION:
            return m_afCompScalars[POSITION_RANGE];
        case ROTATION:
            return m_afCompScalars[ROTATION_RANGE];
        case SCALE:
            return m_afCompScalars[SCALE_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POSITION:
            m_afCompScalars[POSITION_RANGE] = fHalfRange;
            break;
        case ROTATION:
            m_afCompScalars[ROTATION_RANGE] = fHalfRange;
            break;
        case SCALE:
            m_afCompScalars[SCALE_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformEvaluator::GetChannelScratchPadInfo(
    unsigned int uiChannel, bool bForceAlwaysUpdate, 
    NiAVObjectPalette* pkPalette, unsigned int& uiFillSize, 
    bool& bSharedFillData, NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);

    // Initialize using the base class, then fix up a few entries.
    if (!NiBSplineTransformEvaluator::GetChannelScratchPadInfo(uiChannel, 
        bForceAlwaysUpdate, pkPalette, uiFillSize, bSharedFillData, 
        eSPBSegmentData, pkBasisData))
    {
        return false;
    }

    // Fix up the fill size.
    if (uiFillSize > 0)
    {
        if (uiChannel == POSITION)
        {
            uiFillSize = sizeof(NiScratchPad::BSplineCompPoint3FillData);
        }
        else if (uiChannel == ROTATION)
        {
            uiFillSize = sizeof(NiScratchPad::BSplineCompRotFillData);
        }
        else
        {
            NIASSERT(uiChannel == SCALE);
            uiFillSize = sizeof(NiScratchPad::BSplineCompFloatFillData);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformEvaluator::InitChannelScratchPadData(
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    NiBSplineBasisData* pkSPBasisData, bool bInitSharedData, 
    NiAVObjectPalette* pkPalette, NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // Initialize using the base class, then fix up a few entries.
    if (!NiBSplineTransformEvaluator::InitChannelScratchPadData(uiChannel, 
        pkEvalSPData, pkSPBasisData, bInitSharedData, pkPalette, kPBHandle))
    {
        return false;
    }

    if (IsRawEvalChannelPosed(uiChannel))
    {
        // Nothing to fix up.
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == 0.0f);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);
        return true;
    }

    if (uiChannel == POSITION)
    {
        // Switch to the scratch pad fill function for 
        // compact point3 control points in NiBSplineCompPoint3Evaluator.
        pkEvalSPData->SetSPFillFunc(
            &NiBSplineCompPoint3Evaluator::BSplineCompPoint3FillFunction);

        // Initialize the scratch pad fill data members in the derived class.
        NiScratchPad::BSplineCompPoint3FillData* pkFillData = 
            (NiScratchPad::BSplineCompPoint3FillData*)
            pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);
        NIASSERT((void*)&pkFillData->m_kBaseData == (void*)pkFillData);
        pkFillData->m_fOffset = m_afCompScalars[POSITION_OFFSET];
        pkFillData->m_fHalfRange = m_afCompScalars[POSITION_RANGE];
    }
    else if (uiChannel == ROTATION)
    {
        // Switch to the scratch pad fill function for 
        // compact rot control points.
        pkEvalSPData->SetSPFillFunc(&BSplineCompRotFillFunction);

        // Initialize the scratch pad fill data members in the derived class.
        NiScratchPad::BSplineCompRotFillData* pkFillData = 
            (NiScratchPad::BSplineCompRotFillData*)
            pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);
        NIASSERT((void*)&pkFillData->m_kBaseData == (void*)pkFillData);
        pkFillData->m_fOffset = m_afCompScalars[ROTATION_OFFSET];
        pkFillData->m_fHalfRange = m_afCompScalars[ROTATION_RANGE];
    }
    else
    {
        NIASSERT(uiChannel == SCALE);

        // Switch to the scratch pad fill function for 
        // compact float control points in NiBSplineCompFloatEvaluator.
        pkEvalSPData->SetSPFillFunc(
            &NiBSplineCompFloatEvaluator::BSplineCompFloatFillFunction);

        // Initialize the scratch pad fill data members in the derived class.
        NiScratchPad::BSplineCompFloatFillData* pkFillData = 
            (NiScratchPad::BSplineCompFloatFillData*)
            pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);
        NIASSERT((void*)&pkFillData->m_kBaseData == (void*)pkFillData);
        pkFillData->m_fOffset = m_afCompScalars[SCALE_OFFSET];
        pkFillData->m_fHalfRange = m_afCompScalars[SCALE_RANGE];
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformEvaluator::BSplineCompRotFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill and segment data.
    NiScratchPad::BSplineCompRotFillData* pkFillData = 
        (NiScratchPad::BSplineCompRotFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NiScratchPad::BSplineRotFillData* pkBaseFillData = 
        &pkFillData->m_kBaseData;

    NiScratchPad::BSplineFloatSegmentData* pkBSplineSeg = 
        (NiScratchPad::BSplineFloatSegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkBSplineSeg);

    // Update the basis to the specified time.
    int iMin, iMax;
    NiBSplineBasis<float, 3>& kBasis = 
        pkBSplineSeg->m_pkSPBasisData->GetDegree3Basis();
    float fNormTime = (fTime - pkBaseFillData->m_fStartTime) * 
        pkBaseFillData->m_fInvDeltaTime;
    kBasis.Compute(fNormTime, iMin, iMax);

    // Check if we need to load new control points.
    if (iMin != pkBaseFillData->m_iLastMin)
    {
        const short* psSource = 
            pkBaseFillData->m_pkData->GetCompactControlPoint(
            pkBaseFillData->m_kCPHandle, iMin, 4);

        NiCompUtility::DecompressFloatArray(psSource, 16, 
            pkFillData->m_fOffset, pkFillData->m_fHalfRange, 
            pkBSplineSeg->m_afSourceArray, 16);

        pkBaseFillData->m_iLastMin = iMin;
    }

    // There's no need to fill the scratch pad at an identical time.
    pkEvalSPData->SetSPSegmentTimeRange(fTime, fTime);

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineCompTransformEvaluator);
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::CopyMembers(
    NiBSplineCompTransformEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineTransformEvaluator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompTransformEvaluator);
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::LoadBinary(NiStream& kStream)
{
    NiBSplineTransformEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::LinkObject(NiStream& kStream)
{
    NiBSplineTransformEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineTransformEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::SaveBinary(NiStream& kStream)
{
    NiBSplineTransformEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineTransformEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompTransformEvaluator* pkDest = 
        (NiBSplineCompTransformEvaluator*) pkObject;

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        if (m_afCompScalars[ui] != pkDest->m_afCompScalars[ui])
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineCompTransformEvaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineTransformEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompTransformEvaluator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
