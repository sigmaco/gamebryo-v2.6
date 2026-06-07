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

#include "NiBSplineTransformEvaluator.h"
#include "NiBSplineFloatEvaluator.h"
#include "NiBSplinePoint3Evaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiBSplineTransformEvaluator, NiBSplineEvaluator);

//---------------------------------------------------------------------------
NiBSplineTransformEvaluator::NiBSplineTransformEvaluator() : 
    NiBSplineEvaluator(NULL, NULL), 
    m_kTransCPHandle(NiBSplineData::INVALID_HANDLE), 
    m_kRotCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kScaleCPHandle(NiBSplineData::INVALID_HANDLE)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    // No need to adjust the other eval channel types.
}
//---------------------------------------------------------------------------
NiBSplineTransformEvaluator::NiBSplineTransformEvaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kTransCPHandle,
    NiBSplineData::Handle kRotCPHandle, NiBSplineData::Handle kScaleCPHandle,
    NiBSplineBasisData* pkBasisData) : 
    NiBSplineEvaluator(pkData, pkBasisData), 
    m_kTransCPHandle(kTransCPHandle), 
    m_kRotCPHandle(kRotCPHandle),
    m_kScaleCPHandle(kScaleCPHandle)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiBSplineTransformEvaluator::NiBSplineTransformEvaluator(
    const NiQuatTransform& kPoseValue) : NiBSplineEvaluator(),
    m_kTransformValue(kPoseValue), 
    m_kTransCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kRotCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kScaleCPHandle(NiBSplineData::INVALID_HANDLE)
{
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiBSplineTransformEvaluator::NiBSplineTransformEvaluator(
    NiBSplineData* pkData, NiBSplineData::Handle kTransCPHandle,
    NiBSplineData::Handle kRotCPHandle, NiBSplineData::Handle kScaleCPHandle,
    NiBSplineBasisData* pkBasisData, 
#ifdef NIDEBUG
    bool bUseCompactCPs) : 
#else
    bool) : 
#endif    
    NiBSplineEvaluator(pkData, pkBasisData), 
    m_kTransCPHandle(kTransCPHandle), 
    m_kRotCPHandle(kRotCPHandle), 
    m_kScaleCPHandle(kScaleCPHandle)
{
    NIASSERT(bUseCompactCPs);
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    // No need to adjust the eval channel types. The derived class will do it.
}
//---------------------------------------------------------------------------
NiBSplineTransformEvaluator::NiBSplineTransformEvaluator(
    const NiQuatTransform& kPoseValue, 
#ifdef NIDEBUG
    bool bUseCompactCPs) : 
#else
    bool) : 
#endif    
    NiBSplineEvaluator(), m_kTransformValue(kPoseValue), 
    m_kTransCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kRotCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kScaleCPHandle(NiBSplineData::INVALID_HANDLE)
{
    NIASSERT(bUseCompactCPs);
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= EVALTRANSFORMFLAG;
    // No need to adjust the eval channel types. The derived class will do it.
}
//---------------------------------------------------------------------------
unsigned short NiBSplineTransformEvaluator::GetChannelCount() const
{
    return 3;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineTransformEvaluator::GetDimension(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            return 3;
        case ROTATION:
            return 4;
        case SCALE:
            return 1;
        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplineTransformEvaluator::GetDegree(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            return 3;
        case ROTATION:
            return 3;
        case SCALE:
            return 3;
        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::SetPoseTranslate(
    const NiPoint3& kTranslate)
{
    m_kTransformValue.SetTranslate(kTranslate);
    if (m_spData)
        m_kTransCPHandle = NiBSplineData::INVALID_HANDLE;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::SetPoseRotate(
    const NiQuaternion& kRotate)
{
    m_kTransformValue.SetRotate(kRotate);
    if (m_spData)
        m_kRotCPHandle = NiBSplineData::INVALID_HANDLE;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::SetPoseScale(float fScale)
{
    m_kTransformValue.SetScale(fScale);
    if (m_spData)
        m_kScaleCPHandle = NiBSplineData::INVALID_HANDLE;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::SetPoseValue(
    const NiQuatTransform& kPoseValue)
{
    SetPoseTranslate(kPoseValue.GetTranslate());
    SetPoseRotate(kPoseValue.GetRotate());
    SetPoseScale(kPoseValue.GetScale());
    m_spData = NULL;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplineTransformEvaluator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            return m_kTransCPHandle;
        case ROTATION:
            return m_kRotCPHandle;
        case SCALE:
            return m_kScaleCPHandle;
        default:
            return NiBSplineData::INVALID_HANDLE;
    }
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case POSITION:
            m_kTransCPHandle = kControlHandle;
            break;
        case ROTATION:
            m_kRotCPHandle = kControlHandle;
            break;
        case SCALE:
            m_kScaleCPHandle = kControlHandle;
            break;
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
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

    return false;
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::UpdateChannel(float fTime, 
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    void* pvResult) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
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
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        // Compute the value based on the segment data.
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
    }
    else
    {
        // Compute the value based on the segment data.
        if (uiChannel == POSITION)
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
        else if (uiChannel == ROTATION)
        {
            // Compute the value based on the b-spline segment data.
            NiScratchPad::BSplineRotSegmentData* pkBSplineSeg = 
                (NiScratchPad::BSplineRotSegmentData*)
                pkEvalSPData->GetSPSegmentData();
            NIASSERT(pkBSplineSeg);
            NIASSERT(pkBSplineSeg->m_pkSPBasisData);
            NiBSplineBasis<float, 3>& kBasis = 
                pkBSplineSeg->m_pkSPBasisData->GetDegree3Basis();
            float fBasis0 = kBasis.GetValue(0);
            float fBasis1 = kBasis.GetValue(1);
            float fBasis2 = kBasis.GetValue(2);
            float fBasis3 = kBasis.GetValue(3);
            NiQuaternion kQuat;
            kQuat.SetW(fBasis0 * pkBSplineSeg->m_afSourceArray[0] + 
                       fBasis1 * pkBSplineSeg->m_afSourceArray[4] + 
                       fBasis2 * pkBSplineSeg->m_afSourceArray[8] + 
                       fBasis3 * pkBSplineSeg->m_afSourceArray[12]);
            kQuat.SetX(fBasis0 * pkBSplineSeg->m_afSourceArray[1] + 
                       fBasis1 * pkBSplineSeg->m_afSourceArray[5] + 
                       fBasis2 * pkBSplineSeg->m_afSourceArray[9] + 
                       fBasis3 * pkBSplineSeg->m_afSourceArray[13]);
            kQuat.SetY(fBasis0 * pkBSplineSeg->m_afSourceArray[2] + 
                       fBasis1 * pkBSplineSeg->m_afSourceArray[6] + 
                       fBasis2 * pkBSplineSeg->m_afSourceArray[10] + 
                       fBasis3 * pkBSplineSeg->m_afSourceArray[14]);
            kQuat.SetZ(fBasis0 * pkBSplineSeg->m_afSourceArray[3] + 
                       fBasis1 * pkBSplineSeg->m_afSourceArray[7] + 
                       fBasis2 * pkBSplineSeg->m_afSourceArray[11] + 
                       fBasis3 * pkBSplineSeg->m_afSourceArray[15]);
            kQuat.FastNormalize();
            *(NiQuaternion*)pvResult = kQuat;
        }
        else
        {
            NIASSERT(uiChannel == SCALE);
            // Compute the value based on the b-spline segment data.
            NiScratchPad::BSplineFloatSegmentData* pkBSplineSeg = 
                (NiScratchPad::BSplineFloatSegmentData*)
                pkEvalSPData->GetSPSegmentData();
            NIASSERT(pkBSplineSeg);
            NIASSERT(pkBSplineSeg->m_pkSPBasisData);
            NiBSplineBasis<float, 3>& kBasis = 
                pkBSplineSeg->m_pkSPBasisData->GetDegree3Basis();
            float fBasis0 = kBasis.GetValue(0);
            float fBasis1 = kBasis.GetValue(1);
            float fBasis2 = kBasis.GetValue(2);
            float fBasis3 = kBasis.GetValue(3);
            *(float*)pvResult = fBasis0 * pkBSplineSeg->m_afSourceArray[0] + 
                fBasis1 * pkBSplineSeg->m_afSourceArray[1] + 
                fBasis2 * pkBSplineSeg->m_afSourceArray[2] + 
                fBasis3 * pkBSplineSeg->m_afSourceArray[3];
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::GetChannelScratchPadInfo(
    unsigned int uiChannel, bool bForceAlwaysUpdate, 
    NiAVObjectPalette*, unsigned int& uiFillSize, 
    bool& bSharedFillData, NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Indicate the scratch pad info that is required.
    if (uiChannel == POSITION)
    {
        if (bPosed)
        {
            // Use a cubic point3 segment to store the constant value.
            uiFillSize = 0;
            eSPBSegmentData = SPBCUBICPOINT3SEGMENT;
        }
        else
        {
            uiFillSize = sizeof(NiScratchPad::BSplinePoint3FillData);
            eSPBSegmentData = SPBBSPLINEPOINT3SEGMENT;
        }
    }
    else if (uiChannel == ROTATION)
    {
        if (bPosed)
        {
            // Use a slerp rot segment to store the constant value.
            uiFillSize = 0;
            eSPBSegmentData = SPBSLERPROTSEGMENT;
        }
        else
        {
            uiFillSize = sizeof(NiScratchPad::BSplineRotFillData);
            eSPBSegmentData = SPBBSPLINEROTSEGMENT;
        }
    }
    else
    {
        NIASSERT(uiChannel == SCALE);
        if (bPosed)
        {
            // Use a cubic float segment to store the constant value.
            uiFillSize = 0;
            eSPBSegmentData = SPBCUBICFLOATSEGMENT;
        }
        else
        {
            uiFillSize = sizeof(NiScratchPad::BSplineFloatFillData);
            eSPBSegmentData = SPBBSPLINEFLOATSEGMENT;
        }
    }

    bSharedFillData = false;
    pkBasisData = bPosed ? NULL : m_spBasisData;

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::InitChannelScratchPadData(
    unsigned int uiChannel, NiEvaluatorSPData* pkEvalSPData, 
    NiBSplineBasisData* pkSPBasisData, bool, 
    NiAVObjectPalette*, NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // Initialize the evaluator scratch pad data header.
    NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);

    // Initialize the scrach pad based on channel type.
    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (uiChannel == POSITION)
    {
        if (bPosed)
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
            pkCubicSeg->m_kValue0 = m_kTransformValue.GetTranslate();
            pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
            pkCubicSeg->m_kA0 = NiPoint3::ZERO;
            pkCubicSeg->m_kB0 = NiPoint3::ZERO;
        }
        else
        {
            NIASSERT(m_kTransCPHandle != NiBSplineData::INVALID_HANDLE);
            NIASSERT(m_spData);
            NIASSERT(m_spBasisData);

            // Use the b-spline fill function in NiBSplinePoint3Evaluator.
            pkEvalSPData->SetSPFillFunc(
                &NiBSplinePoint3Evaluator::BSplinePoint3FillFunction);

            // Initialize the scratch pad fill data.
            NiScratchPad::BSplinePoint3FillData* pkFillData = 
                (NiScratchPad::BSplinePoint3FillData*)
                pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);
            pkFillData->m_kCPHandle = m_kTransCPHandle;
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
    }
    else if (uiChannel == ROTATION)
    {
        if (bPosed)
        {
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
            pkSlerpSeg->m_kQuat0 = m_kTransformValue.GetRotate();
            pkSlerpSeg->m_kQuat1 = pkSlerpSeg->m_kQuat0;
        }
        else
        {
            NIASSERT(m_kRotCPHandle != NiBSplineData::INVALID_HANDLE);
            NIASSERT(m_spData);
            NIASSERT(m_spBasisData);

            // Use the b-spline rot fill function.
            pkEvalSPData->SetSPFillFunc(&BSplineRotFillFunction);

            // Initialize the scratch pad fill data.
            NiScratchPad::BSplineRotFillData* pkFillData = 
                (NiScratchPad::BSplineRotFillData*)
                pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);
            pkFillData->m_kCPHandle = m_kRotCPHandle;
            pkFillData->m_fStartTime = m_fStartTime;
            pkFillData->m_fInvDeltaTime = (m_fEndTime != m_fStartTime) ? 
                1.0f / (m_fEndTime - m_fStartTime) : 0.0f;
            pkFillData->m_iLastMin = -INT_MAX;
            // Reference the source control point data since these remain 
            // unchanged regardless of the specified time.
            pkFillData->m_pkData = m_spData;

            // Partially initialize the scratch pad segment data.
            NiScratchPad::BSplineRotSegmentData* pkBSplineSeg = 
                (NiScratchPad::BSplineRotSegmentData*)
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
                NIASSERT(kPBHandle.GetChannelType() == PBROTCHANNEL);
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
    }
    else
    {
        NIASSERT(uiChannel == SCALE);

        if (bPosed)
        {
            // Mark the segment data as valid for all sequence times.
            pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
            NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

            // Initialize the scratch pad segment data.
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
        else
        {
            NIASSERT(m_kScaleCPHandle != NiBSplineData::INVALID_HANDLE);
            NIASSERT(m_spData);
            NIASSERT(m_spBasisData);

            // Use the b-spline fill function in NiBSplineFloatEvaluator.
            pkEvalSPData->SetSPFillFunc(
                &NiBSplineFloatEvaluator::BSplineFloatFillFunction);

            // Initialize the scratch pad fill data.
            NiScratchPad::BSplineFloatFillData* pkFillData = 
                (NiScratchPad::BSplineFloatFillData*)
                pkEvalSPData->GetSPFillData();
            NIASSERT(pkFillData);
            pkFillData->m_kCPHandle = m_kScaleCPHandle;
            pkFillData->m_fStartTime = m_fStartTime;
            pkFillData->m_fInvDeltaTime = (m_fEndTime != m_fStartTime) ? 
                1.0f / (m_fEndTime - m_fStartTime) : 0.0f;
            pkFillData->m_iLastMin = -INT_MAX;
            // Reference the source control point data since these remain 
            // unchanged regardless of the specified time.
            pkFillData->m_pkData = m_spData;

            // Partially initialize the scratch pad segment data.
            NiScratchPad::BSplineFloatSegmentData* pkBSplineSeg = 
                (NiScratchPad::BSplineFloatSegmentData*)
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
                NIASSERT(kPBHandle.GetChannelType() == PBFLOATCHANNEL);
                pkBSplineSeg->m_kHeader.m_usOutputIndex = 
                    kPBHandle.GetChannelIndex();
            }
            // Reference the scratch pad basis data since this must be 
            // updated based on the specified time.
            NIASSERT(pkSPBasisData);
            pkBSplineSeg->m_pkSPBasisData = pkSPBasisData;
#ifdef NIDEBUG
            // The fill function should set these fields.
            pkBSplineSeg->m_afSourceArray[0] = 0.0f;
            pkBSplineSeg->m_afSourceArray[1] = 0.0f;
            pkBSplineSeg->m_afSourceArray[2] = 0.0f;
            pkBSplineSeg->m_afSourceArray[3] = 0.0f;
#endif
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::BSplineRotFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::BSplineRotFillData* pkFillData = 
        (NiScratchPad::BSplineRotFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkFillData);

    NiScratchPad::BSplineRotSegmentData* pkBSplineSeg = 
        (NiScratchPad::BSplineRotSegmentData*)
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
void NiBSplineTransformEvaluator::SetEvalChannelTypes()
{
    bool bValidBSplinePos = (GetControlPointCount(POSITION) > 0);
    if (bValidBSplinePos || m_kTransformValue.IsTranslateValid())
    {
        m_aiEvalChannelTypes[POSITION] = EVALPOINT3CHANNEL;
        if (!bValidBSplinePos)
        {
            m_aiEvalChannelTypes[POSITION] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[POSITION] = EVALINVALIDCHANNEL;
    }

    bool bValidBSplineRot = (GetControlPointCount(ROTATION) > 0);
    if (bValidBSplineRot || m_kTransformValue.IsRotateValid())
    {
        m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL;
        if (!bValidBSplineRot)
        {
            m_aiEvalChannelTypes[ROTATION] |= EVALPOSEDFLAG;
        }
    }
    else
    {
        m_aiEvalChannelTypes[ROTATION] = EVALINVALIDCHANNEL;
    }

    bool bValidBSplineScale = (GetControlPointCount(SCALE) > 0);
    if (bValidBSplineScale || m_kTransformValue.IsScaleValid())
    {
        m_aiEvalChannelTypes[SCALE] = EVALFLOATCHANNEL;
        if (!bValidBSplineScale)
        {
            m_aiEvalChannelTypes[SCALE] |= EVALPOSEDFLAG;
        }
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
NiImplementCreateClone(NiBSplineTransformEvaluator);
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::CopyMembers(
    NiBSplineTransformEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_kTransformValue = m_kTransformValue;
    pkDest->m_kTransCPHandle = m_kTransCPHandle;
    pkDest->m_kRotCPHandle = m_kRotCPHandle;
    pkDest->m_kScaleCPHandle = m_kScaleCPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineTransformEvaluator);

//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::LoadBinary(NiStream& kStream)
{
    NiBSplineEvaluator::LoadBinary(kStream);

    m_kTransformValue.LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_kTransCPHandle);
    NiStreamLoadBinary(kStream, m_kRotCPHandle);
    NiStreamLoadBinary(kStream, m_kScaleCPHandle);
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::LinkObject(NiStream& kStream)
{
    NiBSplineEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineEvaluator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::SaveBinary(NiStream& kStream)
{
    NiBSplineEvaluator::SaveBinary(kStream);

    m_kTransformValue.SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_kTransCPHandle);
    NiStreamSaveBinary(kStream, m_kRotCPHandle);
    NiStreamSaveBinary(kStream, m_kScaleCPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplineTransformEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineTransformEvaluator* pkDest = 
        (NiBSplineTransformEvaluator*) pkObject;

    if (m_kTransformValue != pkDest->m_kTransformValue)
    {
        return false;
    }

    if (pkDest->m_kTransCPHandle != m_kTransCPHandle ||
        pkDest->m_kRotCPHandle != m_kRotCPHandle ||
        pkDest->m_kScaleCPHandle != m_kScaleCPHandle)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineTransformEvaluator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplineTransformEvaluator::ms_RTTI
        .GetName()));

    m_kTransformValue.GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString("m_kTransCPHandle",
        m_kTransCPHandle));
    pkStrings->Add(NiGetViewerString("m_kRotCPHandle",
        m_kRotCPHandle));
    pkStrings->Add(NiGetViewerString("m_kScaleCPHandle",
        m_kScaleCPHandle));
}
//---------------------------------------------------------------------------
