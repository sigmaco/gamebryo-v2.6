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

#include "NiBSplineCompPoint3Evaluator.h"
#include <NiCompUtility.h>
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBSplineCompPoint3Evaluator,
    NiBSplinePoint3Evaluator);

//---------------------------------------------------------------------------
NiBSplineCompPoint3Evaluator::NiBSplineCompPoint3Evaluator() : 
    NiBSplinePoint3Evaluator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiBSplineCompPoint3Evaluator::NiBSplineCompPoint3Evaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kPoint3CPHandle,
    NiBSplineBasisData* pkBasisData) : 
    NiBSplinePoint3Evaluator(pkData, kPoint3CPHandle, pkBasisData, true)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------
float NiBSplineCompPoint3Evaluator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POINT3:
            return m_afCompScalars[POINT3_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Evaluator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POINT3:
            m_afCompScalars[POINT3_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompPoint3Evaluator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POINT3:
            return m_afCompScalars[POINT3_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Evaluator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POINT3:
            m_afCompScalars[POINT3_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == POINT3);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(GetControlPointCount(POINT3) > 0);
        NIASSERT(m_spData);
        const short* psValue0 = m_spData->GetCompactControlPoint(
            m_kPoint3CPHandle, 0, 3);

        NiCompUtility::DecompressFloatArray(psValue0, 3, 
            m_afCompScalars[POINT3_OFFSET], m_afCompScalars[POINT3_RANGE], 
            (float*)pvResult, 3);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::GetChannelScratchPadInfo(
    unsigned int uiChannel, bool bForceAlwaysUpdate, 
    NiAVObjectPalette* pkPalette, unsigned int& uiFillSize, 
    bool& bSharedFillData, NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == POINT3);

    // Initialize using the base class, then fix up a few entries.
    if (!NiBSplinePoint3Evaluator::GetChannelScratchPadInfo(uiChannel, 
        bForceAlwaysUpdate, pkPalette, uiFillSize, bSharedFillData, 
        eSPBSegmentData, pkBasisData))
    {
        return false;
    }

    // Fix up the fill size.
    if (uiFillSize > 0)
    {
        uiFillSize = sizeof(NiScratchPad::BSplineCompPoint3FillData);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::InitChannelScratchPadData(
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    NiBSplineBasisData* pkSPBasisData, bool bInitSharedData, 
    NiAVObjectPalette* pkPalette, NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == POINT3);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_kPoint3CPHandle != NiBSplineData::INVALID_HANDLE);
    NIASSERT(m_spData);
    NIASSERT(m_spBasisData);

    // Initialize using the base class, then fix up a few entries.
    if (!NiBSplinePoint3Evaluator::InitChannelScratchPadData(uiChannel, 
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

    // Switch to the scratch pad fill function for 
    // compact point3 control points.
    pkEvalSPData->SetSPFillFunc(&BSplineCompPoint3FillFunction);

    // Initialize the scratch pad fill data members in the derived class.
    NiScratchPad::BSplineCompPoint3FillData* pkFillData = 
        (NiScratchPad::BSplineCompPoint3FillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT((void*)&pkFillData->m_kBaseData == (void*)pkFillData);
    pkFillData->m_fOffset = m_afCompScalars[POINT3_OFFSET];
    pkFillData->m_fHalfRange = m_afCompScalars[POINT3_RANGE];

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::BSplineCompPoint3FillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill and segment data.
    NiScratchPad::BSplineCompPoint3FillData* pkFillData = 
        (NiScratchPad::BSplineCompPoint3FillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NiScratchPad::BSplinePoint3FillData* pkBaseFillData = 
        &pkFillData->m_kBaseData;

    NiScratchPad::BSplinePoint3SegmentData* pkBSplineSeg = 
        (NiScratchPad::BSplinePoint3SegmentData*)
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
            pkBaseFillData->m_kCPHandle, iMin, 3);

        NiCompUtility::DecompressFloatArray(psSource, 12, 
            pkFillData->m_fOffset, pkFillData->m_fHalfRange, 
            pkBSplineSeg->m_afSourceArray, 12);

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
NiImplementCreateClone(NiBSplineCompPoint3Evaluator);
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Evaluator::CopyMembers(
    NiBSplineCompPoint3Evaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplinePoint3Evaluator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompPoint3Evaluator);
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Evaluator::LoadBinary(NiStream& kStream)
{
    NiBSplinePoint3Evaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Evaluator::LinkObject(NiStream& kStream)
{
    NiBSplinePoint3Evaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplinePoint3Evaluator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Evaluator::SaveBinary(NiStream& kStream)
{
    NiBSplinePoint3Evaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Evaluator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplinePoint3Evaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompPoint3Evaluator* pkDest = 
        (NiBSplineCompPoint3Evaluator*) pkObject;

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
void NiBSplineCompPoint3Evaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplinePoint3Evaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompPoint3Evaluator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
