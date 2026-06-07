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

#include "NiBSplineColorEvaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBSplineColorEvaluator, NiBSplineEvaluator);

//---------------------------------------------------------------------------
NiBSplineColorEvaluator::NiBSplineColorEvaluator() : 
    NiBSplineEvaluator(NULL, NULL),
    m_kColorACPHandle(NiBSplineData::INVALID_HANDLE)
{
    // No need to adjust the eval channel types.
}
//---------------------------------------------------------------------------
NiBSplineColorEvaluator::NiBSplineColorEvaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kColorACPHandle,
    NiBSplineBasisData* pkBasisData) : NiBSplineEvaluator(pkData,
    pkBasisData), m_kColorACPHandle(kColorACPHandle)
{
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiBSplineColorEvaluator::NiBSplineColorEvaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kColorACPHandle,
    NiBSplineBasisData* pkBasisData, 
#ifdef NIDEBUG
    bool bUseCompactCPs) : 
#else
    bool) : 
#endif
    NiBSplineEvaluator(pkData, pkBasisData), 
    m_kColorACPHandle(kColorACPHandle)
{
    NIASSERT(bUseCompactCPs);
    // No need to adjust the eval channel types. The derived class will do it.
}
//---------------------------------------------------------------------------
unsigned short NiBSplineColorEvaluator::GetChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineColorEvaluator::GetDimension(
    unsigned short usChannel) const
{
    if (COLORA == usChannel)
        return 4;
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineColorEvaluator::GetDegree(
    unsigned short usChannel) const
{
    if (COLORA == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplineColorEvaluator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case COLORA:
            return m_kColorACPHandle;
    }
    return NiBSplineData::INVALID_HANDLE;
}
//---------------------------------------------------------------------------
void NiBSplineColorEvaluator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case COLORA:
            m_kColorACPHandle = kControlHandle;
            break;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(GetControlPointCount(COLORA) > 0);
        NIASSERT(m_spData);
        const NiColorA* pkValue0 = (const NiColorA*)m_spData->
            GetControlPoint(m_kColorACPHandle, 0, 4);
        *(NiColorA*)pvResult = *pkValue0;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
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
        NiScratchPadFillFunc pfnFillFunc = 
            pkEvalSPData->GetSPFillFunc();
        if (!pfnFillFunc || !(*pfnFillFunc)(fTime, pkEvalSPData))
        {
            return false;
        }
    }

    if (IsRawEvalChannelPosed(uiChannel))
    {
        // Compute the value based on the linear segment data.
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NiScratchPad::LinearColorSegmentData* pkLinearSeg = 
            (NiScratchPad::LinearColorSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkLinearSeg);
        *(NiColorA*)pvResult = pkLinearSeg->m_kValue0;
        NIASSERT(pkLinearSeg->m_kOutTangent0 == NiColorA(0, 0, 0, 0));
    }
    else
    {
        // Compute the value based on the b-spline segment data.
        NiScratchPad::BSplineColorSegmentData* pkBSplineSeg = 
            (NiScratchPad::BSplineColorSegmentData*)
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
            fBasis1 * pkBSplineSeg->m_afSourceArray[4] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[8] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[12];
        ((float*)pvResult)[1] = 
            fBasis0 * pkBSplineSeg->m_afSourceArray[1] + 
            fBasis1 * pkBSplineSeg->m_afSourceArray[5] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[9] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[13];
        ((float*)pvResult)[2] = 
            fBasis0 * pkBSplineSeg->m_afSourceArray[2] + 
            fBasis1 * pkBSplineSeg->m_afSourceArray[6] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[10] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[14];
        ((float*)pvResult)[3] = 
            fBasis0 * pkBSplineSeg->m_afSourceArray[3] + 
            fBasis1 * pkBSplineSeg->m_afSourceArray[7] + 
            fBasis2 * pkBSplineSeg->m_afSourceArray[11] + 
            fBasis3 * pkBSplineSeg->m_afSourceArray[15];
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
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
    if (bPosed)
    {
        uiFillSize = 0;
        eSPBSegmentData = SPBLINEARCOLORSEGMENT;
        pkBasisData = NULL;
    }
    else
    {
        uiFillSize = sizeof(NiScratchPad::BSplineColorFillData);
        eSPBSegmentData = SPBBSPLINECOLORSEGMENT;
        pkBasisData = m_spBasisData;
    }
    bSharedFillData = false;

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData* pkSPBasisData, 
    bool, NiAVObjectPalette*, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel == COLORA);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(m_kColorACPHandle != NiBSplineData::INVALID_HANDLE);
    NIASSERT(m_spData);
    NIASSERT(m_spBasisData);

    if (IsRawEvalChannelPosed(uiChannel))
    {
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
        NIVERIFY(GetChannelPosedValue(uiChannel, &pkLinearSeg->m_kValue0));
        pkLinearSeg->m_kOutTangent0 = NiColorA(0, 0, 0, 0);
    }
    else
    {
        // Initialize the evaluator scratch pad data header.
        NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
        NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
        pkEvalSPData->SetSPFillFunc(&BSplineColorFillFunction);

        // Initialize the scratch pad fill data.
        NiScratchPad::BSplineColorFillData* pkFillData = 
            (NiScratchPad::BSplineColorFillData*)
            pkEvalSPData->GetSPFillData();
        NIASSERT(pkFillData);
        pkFillData->m_kCPHandle = m_kColorACPHandle;
        pkFillData->m_fStartTime = m_fStartTime;
        pkFillData->m_fInvDeltaTime = (m_fEndTime != m_fStartTime) ? 
            1.0f / (m_fEndTime - m_fStartTime) : 0.0f;
        pkFillData->m_iLastMin = -INT_MAX;
        // Reference the source control point data since these remain 
        // unchanged regardless of the specified time.
        pkFillData->m_pkData = m_spData;

        // Partially initialize the scratch pad segment data.
        NiScratchPad::BSplineColorSegmentData* pkBSplineSeg = 
            (NiScratchPad::BSplineColorSegmentData*)
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
            NIASSERT(kPBHandle.GetChannelType() == PBCOLORCHANNEL);
            pkBSplineSeg->m_kHeader.m_usOutputIndex = 
                kPBHandle.GetChannelIndex();
        }
        // Reference the scratch pad basis data since this must be 
        // updated based on the specified time.
        NIASSERT(pkSPBasisData);
        pkBSplineSeg->m_pkSPBasisData = pkSPBasisData;
#ifdef NIDEBUG
        // The fill function should set these fields.
        for (unsigned int ui = 0; ui < 16; ui++)
        {
            pkBSplineSeg->m_afSourceArray[ui] = 0.0f;
        }
#endif
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::BSplineColorFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill and segment data.
    NiScratchPad::BSplineColorFillData* pkFillData = 
        (NiScratchPad::BSplineColorFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);

    NiScratchPad::BSplineColorSegmentData* pkBSplineSeg = 
        (NiScratchPad::BSplineColorSegmentData*)
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
            pkFillData->m_kCPHandle, iMin, 4);
        float* pfDest = pkBSplineSeg->m_afSourceArray;
        float* pfEndDest = pfDest + 16;
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
void NiBSplineColorEvaluator::SetEvalChannelTypes()
{
    NiUInt32 uiCPCount = GetControlPointCount(COLORA);
    if (uiCPCount > 0)
    {
        NIASSERT(m_kColorACPHandle != NiBSplineData::INVALID_HANDLE);
        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);

        m_aiEvalChannelTypes[COLORA] = EVALCOLORCHANNEL;

        // Determine if the channel is posed.
        bool bPosed = true;
        if (UsesCompressedControlPoints())
        {
            const short* psValue = m_spData->GetCompactControlPoint(
                m_kColorACPHandle, 0, 4);
            const short* psEndValue = psValue + uiCPCount * 4;
            short sValue0 = *psValue;
            short sValue1 = *(psValue + 1);
            short sValue2 = *(psValue + 2);
            short sValue3 = *(psValue + 3);
            psValue += 4;
            while (psValue < psEndValue)
            {
                if (*psValue != sValue0 ||
                    *(psValue + 1) != sValue1 ||
                    *(psValue + 2) != sValue2 ||
                    *(psValue + 3) != sValue3)
                {
                    bPosed = false;
                    break;
                }
                psValue += 4;
            }
        }
        else
        {
            const NiColorA* pkValue = (const NiColorA*)m_spData->
                GetControlPoint(m_kColorACPHandle, 0, 4);
            const NiColorA* pkEndValue = pkValue + uiCPCount;
            NiColorA kValue0 = *pkValue;
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
NiImplementCreateClone(NiBSplineColorEvaluator);
//---------------------------------------------------------------------------
void NiBSplineColorEvaluator::CopyMembers(
    NiBSplineColorEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kColorACPHandle = m_kColorACPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineColorEvaluator);
//---------------------------------------------------------------------------
void NiBSplineColorEvaluator::LoadBinary(NiStream& kStream)
{
    NiBSplineEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_kColorACPHandle);
}
//---------------------------------------------------------------------------
void NiBSplineColorEvaluator::LinkObject(NiStream& kStream)
{
    NiBSplineEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineColorEvaluator::SaveBinary(NiStream& kStream)
{
    NiBSplineEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_kColorACPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplineColorEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineColorEvaluator* pkDest = 
        (NiBSplineColorEvaluator*) pkObject;

    if (pkDest->m_kColorACPHandle != m_kColorACPHandle)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineColorEvaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplineColorEvaluator::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_kColorACPHandle",
        m_kColorACPHandle));
}
//---------------------------------------------------------------------------
