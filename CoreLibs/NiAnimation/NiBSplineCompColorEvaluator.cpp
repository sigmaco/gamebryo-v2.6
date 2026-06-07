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

#include "NiBSplineCompColorEvaluator.h"
#include <NiCompUtility.h>
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBSplineCompColorEvaluator,
    NiBSplineColorEvaluator);

//---------------------------------------------------------------------------
NiBSplineCompColorEvaluator::NiBSplineCompColorEvaluator() : 
    NiBSplineColorEvaluator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiBSplineCompColorEvaluator::NiBSplineCompColorEvaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kColorCPHandle,
    NiBSplineBasisData* pkBasisData): 
    NiBSplineColorEvaluator(pkData, kColorCPHandle, pkBasisData, true)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------
float NiBSplineCompColorEvaluator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case COLORA:
            return m_afCompScalars[COLORA_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
void NiBSplineCompColorEvaluator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case COLORA:
            m_afCompScalars[COLORA_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompColorEvaluator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case COLORA:
            return m_afCompScalars[COLORA_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
void NiBSplineCompColorEvaluator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case COLORA:
            m_afCompScalars[COLORA_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(GetControlPointCount(COLORA) > 0);
        NIASSERT(m_spData);
        const short* psValue0 = m_spData->GetCompactControlPoint(
            m_kColorACPHandle, 0, 4);

        NiCompUtility::DecompressFloatArray(psValue0, 4, 
            m_afCompScalars[COLORA_OFFSET], m_afCompScalars[COLORA_RANGE], 
            (float*)pvResult, 4);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::GetChannelScratchPadInfo(
    unsigned int uiChannel, bool bForceAlwaysUpdate, 
    NiAVObjectPalette* pkPalette, unsigned int& uiFillSize, 
    bool& bSharedFillData, NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel == COLORA);

    // Initialize using the base class, then fix up a few entries.
    if (!NiBSplineColorEvaluator::GetChannelScratchPadInfo(uiChannel, 
        bForceAlwaysUpdate, pkPalette, uiFillSize, bSharedFillData, 
        eSPBSegmentData, pkBasisData))
    {
        return false;
    }

    // Fix up the fill size.
    if (uiFillSize > 0)
    {
        uiFillSize = sizeof(NiScratchPad::BSplineCompColorFillData);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::InitChannelScratchPadData(
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    NiBSplineBasisData* pkSPBasisData, bool bInitSharedData, 
    NiAVObjectPalette* pkPalette, NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_kColorACPHandle != NiBSplineData::INVALID_HANDLE);
    NIASSERT(m_spData);
    NIASSERT(m_spBasisData);

    // Initialize using the base class, then fix up a few entries.
    if (!NiBSplineColorEvaluator::InitChannelScratchPadData(uiChannel, 
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
    // compact color control points.
    pkEvalSPData->SetSPFillFunc(&BSplineCompColorFillFunction);

    // Initialize the scratch pad fill data members in the derived class.
    NiScratchPad::BSplineCompColorFillData* pkFillData = 
        (NiScratchPad::BSplineCompColorFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NIASSERT((void*)&pkFillData->m_kBaseData == (void*)pkFillData);
    pkFillData->m_fOffset = m_afCompScalars[COLORA_OFFSET];
    pkFillData->m_fHalfRange = m_afCompScalars[COLORA_RANGE];

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::BSplineCompColorFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill and segment data.
    NiScratchPad::BSplineCompColorFillData* pkFillData = 
        (NiScratchPad::BSplineCompColorFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);
    NiScratchPad::BSplineColorFillData* pkBaseFillData = 
        &pkFillData->m_kBaseData;

    NiScratchPad::BSplineColorSegmentData* pkBSplineSeg = 
        (NiScratchPad::BSplineColorSegmentData*)
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
NiImplementCreateClone(NiBSplineCompColorEvaluator);
//---------------------------------------------------------------------------
void NiBSplineCompColorEvaluator::CopyMembers(
    NiBSplineCompColorEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineColorEvaluator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompColorEvaluator);
//---------------------------------------------------------------------------
void NiBSplineCompColorEvaluator::LoadBinary(NiStream& kStream)
{
    NiBSplineColorEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompColorEvaluator::LinkObject(NiStream& kStream)
{
    NiBSplineColorEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineColorEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompColorEvaluator::SaveBinary(NiStream& kStream)
{
    NiBSplineColorEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineColorEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompColorEvaluator* pkDest = 
        (NiBSplineCompColorEvaluator*) pkObject;

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
void NiBSplineCompColorEvaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineColorEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompColorEvaluator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
