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

#include "NiBSplinePoint3Evaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBSplinePoint3Evaluator, NiBSplineEvaluator);

//---------------------------------------------------------------------------
NiBSplinePoint3Evaluator::NiBSplinePoint3Evaluator() : 
    NiBSplineEvaluator(NULL, NULL), 
    m_kPoint3CPHandle(NiBSplineData::INVALID_HANDLE)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiBSplinePoint3Evaluator::NiBSplinePoint3Evaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kPoint3CPHandle,
    NiBSplineBasisData* pkBasisData) : NiBSplineEvaluator(pkData,
    pkBasisData), m_kPoint3CPHandle(kPoint3CPHandle)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiBSplinePoint3Evaluator::NiBSplinePoint3Evaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kPoint3CPHandle,
    NiBSplineBasisData* pkBasisData, 
#ifdef NIDEBUG
    bool bUseCompactCPs) : 
#else
    bool) : 
#endif
    NiBSplineEvaluator(pkData, pkBasisData), 
    m_kPoint3CPHandle(kPoint3CPHandle)
{
    NIASSERT(bUseCompactCPs);
    // No need to adjust the eval channel types. The derived class will do it.
}
//---------------------------------------------------------------------------
unsigned short NiBSplinePoint3Evaluator::GetChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiBSplinePoint3Evaluator::GetDimension(
    unsigned short usChannel) const
{
    if (POINT3 == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiBSplinePoint3Evaluator::GetDegree(
    unsigned short usChannel) const
{
    if (POINT3 == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplinePoint3Evaluator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POINT3:
            return m_kPoint3CPHandle;
        default:
            return NiBSplineData::INVALID_HANDLE;
    }
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case POINT3:
            m_kPoint3CPHandle = kControlHandle;
            break;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == POINT3);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(GetControlPointCount(POINT3) > 0);
        NIASSERT(m_spData);
        const NiPoint3* pkValue0 = (const NiPoint3*)m_spData->
            GetControlPoint(m_kPoint3CPHandle, 0, 3);
        *(NiPoint3*)pvResult = *pkValue0;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::UpdateChannel(float fTime, unsigned int uiChannel,
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
        NiScratchPadFillFunc pfnFillFunc = 
            pkEvalSPData->GetSPFillFunc();
        if (!pfnFillFunc || !(*pfnFillFunc)(fTime, pkEvalSPData))
        {
            return false;
        }
    }

    if (IsRawEvalChannelPosed(uiChannel))
    {
        // Compute the value based on the cubic segment data.
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
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
        // Compute the value based on the b-spline segment data.
        NiScratchPad::BSplinePoint3SegmentData* pkBSplineSeg = 
            (NiScratchPad::BSplinePoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkBSplineSeg);
        NIASSERT(pkBSplineSeg->m_pkSPBasisData);
        NiBSplineBasis<float, 3>& kBasis = 
            pkBSplineSeg->m_pkSPBasisData->GetDegree3Basis();
        float fBasis0 = kBasis.GetValue(0);
        float fBasis1 = kBasis.GetValue(1);
        float fBasis2 = kBasis.GetValue(2);
        float fBasis3 = kBasis.GetValue(3);
        ((float*)pvResult)[0] = 
            fBasis0 * pkBSplineSeg->m_afSourceArray[0] + 
            fBasis1 * pkBSplineSeg->m_afSourceArray[3] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[6] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[9];
        ((float*)pvResult)[1] = 
            fBasis0 * pkBSplineSeg->m_afSourceArray[1] + 
            fBasis1 * pkBSplineSeg->m_afSourceArray[4] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[7] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[10];
        ((float*)pvResult)[2] = 
            fBasis0 * pkBSplineSeg->m_afSourceArray[2] + 
            fBasis1 * pkBSplineSeg->m_afSourceArray[5] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[8] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[11];
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
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
    if (bPosed)
    {
        uiFillSize = 0;
        eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
        pkBasisData = NULL;
    }
    else
    {
        uiFillSize = sizeof(NiScratchPad::BSplinePoint3FillData);
        eSPBSegmentData = SPBBSPLINEPOINT3SEGMENT;
        pkBasisData = m_spBasisData;
    }
    bSharedFillData = false;

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == POINT3);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_kPoint3CPHandle != NiBSplineData::INVALID_HANDLE);
    NIASSERT(m_spData);
    NIASSERT(m_spBasisData);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        // Mark the segment data as valid for all sequence times.
        pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

        // Initialize the scratch pad segment data.
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
        NIVERIFY(GetChannelPosedValue(uiChannel, &pkCubicSeg->m_kValue0));
        pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
        pkCubicSeg->m_kA0 = NiPoint3::ZERO;
        pkCubicSeg->m_kB0 = NiPoint3::ZERO;
    }
    else
    {
        // Initialize the evaluator scratch pad data header.
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
        pkEvalSPData->SetSPFillFunc(&BSplinePoint3FillFunction);

        // Initialize the scratch pad fill data.
        NiScratchPad::BSplinePoint3FillData* pkFillData = 
            (NiScratchPad::BSplinePoint3FillData*)
            pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);
        pkFillData->m_kCPHandle = m_kPoint3CPHandle;
        pkFillData->m_fStartTime = m_fStartTime;
        pkFillData->m_fInvDeltaTime = (m_fEndTime != m_fStartTime) ? 
            1.0f / (m_fEndTime - m_fStartTime) : 0.0f;
        pkFillData->m_iLastMin = -INT_MAX;
        // Reference the source control point data since these remain 
        // unchanged regardless of the specified time.
        pkFillData->m_pkData = m_spData;

        // Partially initialize the scratch pad segment data.
        NiScratchPad::BSplinePoint3SegmentData* pkBSplineSeg = 
            (NiScratchPad::BSplinePoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkBSplineSeg);
        pkBSplineSeg->m_kHeader.m_sLOD = pkEvalSPData->GetLOD();
        if (IsReferencedEvaluator())
        {
            NIASSERT(kPBHandle.GetChannelType() == PBREFERENCEDCHANNEL);
            pkBSplineSeg->m_kHeader.m_usOutputIndex = 
                NiScratchPad::INVALIDOUTPUTINDEX;
        }
        else
        {
            NIASSERT(kPBHandle.GetChannelType() == PBPOINT3CHANNEL);
            pkBSplineSeg->m_kHeader.m_usOutputIndex = 
                kPBHandle.GetChannelIndex();
        }
        // Reference the scratch pad basis data since this must be 
        // updated based on the specified time.
        NIASSERT(pkSPBasisData);
        pkBSplineSeg->m_pkSPBasisData = pkSPBasisData;
#ifdef NIDEBUG
        // The fill function should set these fields.
        for (unsigned int ui = 0; ui < 12; ui++)
        {
            pkBSplineSeg->m_afSourceArray[ui] = 0.0f;
        }
#endif
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::BSplinePoint3FillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill and segment data.
    NiScratchPad::BSplinePoint3FillData* pkFillData = 
        (NiScratchPad::BSplinePoint3FillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);

    NiScratchPad::BSplinePoint3SegmentData* pkBSplineSeg = 
        (NiScratchPad::BSplinePoint3SegmentData*)
        pkEvalSPData->GetSPSegmentData();
    NIASSERT(pkBSplineSeg);

    // Update the basis to the specified time.
    int iMin, iMax;
    NiBSplineBasis<float, 3>& kBasis = 
        pkBSplineSeg->m_pkSPBasisData->GetDegree3Basis();
    float fNormTime = (fTime - pkFillData->m_fStartTime) * 
        pkFillData->m_fInvDeltaTime;
    kBasis.Compute(fNormTime, iMin, iMax);

    // Check if we need to load new control points.
    if (iMin != pkFillData->m_iLastMin)
    {
        const float* pfSource = 
            pkFillData->m_pkData->GetControlPoint(
            pkFillData->m_kCPHandle, iMin, 3);
        float* pfDest = pkBSplineSeg->m_afSourceArray;
        float* pfEndDest = pfDest + 12;
        while (pfDest < pfEndDest)
        {
            *pfDest = *pfSource;
            pfSource++;
            pfDest++;
        }
        pkFillData->m_iLastMin = iMin;
    }

    // There's no need to fill the scratch pad at an identical time.
    pkEvalSPData->SetSPSegmentTimeRange(fTime, fTime);

    return true;
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::SetEvalChannelTypes()
{
    NiUInt32 uiCPCount = GetControlPointCount(POINT3);
    if (uiCPCount > 0)
    {
        NIASSERT(m_kPoint3CPHandle != NiBSplineData::INVALID_HANDLE);
        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);

        m_aiEvalChannelTypes[POINT3] = EVALPOINT3CHANNEL;

        // Determine if the channel is posed.
        bool bPosed = true;
        if (UsesCompressedControlPoints())
        {
            const short* psValue = m_spData->GetCompactControlPoint(
                m_kPoint3CPHandle, 0, 3);
            const short* psEndValue = psValue + uiCPCount * 3;
            short sValue0 = *psValue;
            short sValue1 = *(psValue + 1);
            short sValue2 = *(psValue + 2);
            psValue += 3;
            while (psValue < psEndValue)
            {
                if (*psValue != sValue0 ||
                    *(psValue + 1) != sValue1 ||
                    *(psValue + 2) != sValue2)
                {
                    bPosed = false;
                    break;
                }
                psValue += 3;
            }
        }
        else
        {
            const NiPoint3* pkValue = (const NiPoint3*)m_spData->
                GetControlPoint(m_kPoint3CPHandle, 0, 3);
            const NiPoint3* pkEndValue = pkValue + uiCPCount;
            NiPoint3 kValue0 = *pkValue;
            pkValue++;
            while (pkValue < pkEndValue)
            {
                if (*pkValue != kValue0)
                {
                    bPosed = false;
                    break;
                }
                pkValue++;
            }
        }

        if (bPosed)
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
NiImplementCreateClone(NiBSplinePoint3Evaluator);
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::CopyMembers(
    NiBSplinePoint3Evaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kPoint3CPHandle = m_kPoint3CPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplinePoint3Evaluator);
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::LoadBinary(NiStream& kStream)
{
    NiBSplineEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_kPoint3CPHandle);
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::LinkObject(NiStream& kStream)
{
    NiBSplineEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::SaveBinary(NiStream& kStream)
{
    NiBSplineEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_kPoint3CPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Evaluator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplinePoint3Evaluator* pkDest = 
        (NiBSplinePoint3Evaluator*) pkObject;

    if (pkDest->m_kPoint3CPHandle != m_kPoint3CPHandle)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplinePoint3Evaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplinePoint3Evaluator::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_kPoint3CPHandle", 
        m_kPoint3CPHandle));
}
//---------------------------------------------------------------------------
