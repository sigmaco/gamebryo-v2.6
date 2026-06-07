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

#include "NiLookAtEvaluator.h"
#include <NiNode.h>
#include <NiAVObjectPalette.h>
#include "NiFloatEvaluator.h"
#include "NiPoint3Evaluator.h"
#include "NiEvaluatorSPData.h"
#include "NiScratchPad.h"

NiImplementRTTI(NiLookAtEvaluator, NiEvaluator);

//---------------------------------------------------------------------------
NiLookAtEvaluator::NiLookAtEvaluator() : 
    m_uFlags(0), m_kLookAtObjectName(), m_kDrivenObjectName()
{
    SetFlip(false);
    SetAxis(X);

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        m_aspEvaluators[ui] = NULL;
    }

    // Set the evaluator to always update as well as shut down the scratch pad.
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= 
        (EVALTRANSFORMFLAG | EVALALWAYSUPDATEFLAG | EVALSHUTDOWNFLAG);
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiLookAtEvaluator::NiLookAtEvaluator(const NiFixedString& kLookAtObjectName, 
    const NiFixedString& kDrivenObjectName, Axis eAxis, bool bFlip) : 
    m_uFlags(0), m_kLookAtObjectName(kLookAtObjectName), 
    m_kDrivenObjectName(kDrivenObjectName)
{
    SetFlip(bFlip);
    SetAxis(eAxis);

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        m_aspEvaluators[ui] = NULL;
    }

    // Set the evaluator to always update as well as shut down the scratch pad.
    m_aiEvalChannelTypes[EVALFLAGINDEX] |= 
        (EVALTRANSFORMFLAG | EVALALWAYSUPDATEFLAG | EVALSHUTDOWNFLAG);
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
NiLookAtEvaluator::~NiLookAtEvaluator()
{
    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        m_aspEvaluators[ui] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::GetActiveTimeRange(float& fBeginKeyTime, 
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;
    
    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui] != NULL)
        {
            float fBegin, fEnd;
            m_aspEvaluators[ui]->GetActiveTimeRange(fBegin, fEnd);
            if (fBegin != fEnd)
            {
                if (fBegin < fBeginKeyTime)
                {
                    fBeginKeyTime = fBegin;
                }
                if (fEnd > fEndKeyTime)
                {
                    fEndKeyTime = fEnd;
                }
            }
        }
    }

    if (fBeginKeyTime == NI_INFINITY && fEndKeyTime == -NI_INFINITY)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::GuaranteeTimeRange(float fStartTime, float fEndTime)
{
    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui] != NULL)
        {
            m_aspEvaluators[ui]->GuaranteeTimeRange(fStartTime, fEndTime);
        }
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiLookAtEvaluator::GetSequenceEvaluator(float fStartTime, 
    float fEndTime)
{
    NiLookAtEvaluator* pkSeqEval = (NiLookAtEvaluator*)
        NiEvaluator::GetSequenceEvaluator(fStartTime, fEndTime);

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui] != NULL)
        {
            pkSeqEval->m_aspEvaluators[ui] = 
                m_aspEvaluators[ui]->GetSequenceEvaluator(fStartTime,
                fEndTime);
        }
    }

    return pkSeqEval;
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::Collapse()
{
    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui] != NULL)
        {
            m_aspEvaluators[ui]->Collapse();
        }
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::GetChannelPosedValue(unsigned int uiChannel, 
    void* pvResult) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pvResult);

    if (IsRawEvalChannelPosed(uiChannel))
    {
        NIASSERT(!IsEvalChannelInvalid(uiChannel));
        NIASSERT(uiChannel != ROTATION);
        if (uiChannel == POSITION)
        {
            NIASSERT(m_aspEvaluators[TRANSLATEEVAL]);
            NIVERIFY(m_aspEvaluators[TRANSLATEEVAL]->GetChannelPosedValue(
                0, pvResult));
        }
        else
        {
            NIASSERT(uiChannel == SCALE);
            NIASSERT(m_aspEvaluators[SCALEEVAL]);
            NIVERIFY(m_aspEvaluators[SCALEEVAL]->GetChannelPosedValue(
                0, pvResult));
        }

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::UpdateChannel(float fTime, unsigned int uiChannel,
    NiEvaluatorSPData* pkEvalSPData, void* pvResult) const
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
        NiScratchPadFillFunc pfnFillFunc = pkEvalSPData->GetSPFillFunc();
        if (!pfnFillFunc || !(*pfnFillFunc)(fTime, pkEvalSPData))
        {
            return false;
        }
    }

    // Copy the computed transform component.
    if (uiChannel == ROTATION)
    {
        NiScratchPad::SlerpRotSegmentData* pkSlerpRot = 
            (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSlerpRot);
        *(NiQuaternion*)pvResult = pkSlerpRot->m_kQuat0;
    }
    else if (uiChannel == POSITION)
    {
        NiScratchPad::CubicPoint3SegmentData* pkCubicPt3 = 
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicPt3);
        *(NiPoint3*)pvResult = pkCubicPt3->m_kValue0;
    }
    else
    {
        NIASSERT(uiChannel == SCALE);
        NiScratchPad::CubicFloatSegmentData* pkCubicFloat = 
            (NiScratchPad::CubicFloatSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicFloat);
        *(float*)pvResult = pkCubicFloat->m_fValue0;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::GetChannelScratchPadInfo(unsigned int uiChannel, 
    bool bForceAlwaysUpdate, NiAVObjectPalette* pkPalette, 
    unsigned int& uiFillSize, bool& bSharedFillData, 
    NiScratchPadBlock& eSPBSegmentData, 
    NiBSplineBasisData*& pkBasisData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkPalette);

    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    if (IsEvalChannelInvalid(uiChannel) ||
        (bPosed && !bForceAlwaysUpdate && !AlwaysUpdate()))
    {
        // Channel is invalid or constant: scratch pad is not required.
        return false;
    }

    // Check if the dependent objects can be found.
    if (!m_kLookAtObjectName.Exists() || !m_kDrivenObjectName.Exists())
    {
        return false;
    }
    NiAVObject* pkLookAtObject = pkPalette->GetAVObject(m_kLookAtObjectName);
    if (!pkLookAtObject)
    {
        return false;
    }
    NiAVObject* pkDrivenObject = pkPalette->GetAVObject(m_kDrivenObjectName);
    if (!pkDrivenObject)
    {
        return false;
    }

    // Compute the extra scratch pad space required by the internal evaluators.
    unsigned int uiTotalExtraFillSize = 0;

    unsigned int uiExtraFillSize;
    bool bExtraSharedFillData;
    NiScratchPadBlock eExtraSPBSegmentData;
    NiBSplineBasisData* pkExtraBasisData;

    if (m_aspEvaluators[TRANSLATEEVAL])
    {
        if (m_aspEvaluators[TRANSLATEEVAL]->GetChannelScratchPadInfo(
            0, false, pkPalette, uiExtraFillSize, bExtraSharedFillData, 
            eExtraSPBSegmentData, pkExtraBasisData))
        {
            uiTotalExtraFillSize += uiExtraFillSize;

            if (eExtraSPBSegmentData == SPBCUBICPOINT3SEGMENT)
            {
                uiTotalExtraFillSize += sizeof(
                    NiScratchPad::CubicPoint3SegmentData);
                NIASSERT(!pkExtraBasisData);
            }
            else
            {
                NIASSERT(eExtraSPBSegmentData == SPBBSPLINEPOINT3SEGMENT);
                uiTotalExtraFillSize += sizeof(
                    NiScratchPad::BSplinePoint3SegmentData);
                NIASSERT(pkExtraBasisData);
                uiTotalExtraFillSize += sizeof(NiBSplineBasisData);
            }
        }
    }

    if (m_aspEvaluators[ROLLEVAL])
    {
        if (m_aspEvaluators[ROLLEVAL]->GetChannelScratchPadInfo(
            0, false, pkPalette, uiExtraFillSize, bExtraSharedFillData, 
            eExtraSPBSegmentData, pkExtraBasisData))
        {
            uiTotalExtraFillSize += uiExtraFillSize;

            if (eExtraSPBSegmentData == SPBCUBICFLOATSEGMENT)
            {
                uiTotalExtraFillSize += sizeof(
                    NiScratchPad::CubicFloatSegmentData);
                NIASSERT(!pkExtraBasisData);
            }
            else
            {
                NIASSERT(eExtraSPBSegmentData == SPBBSPLINEFLOATSEGMENT);
                uiTotalExtraFillSize += sizeof(
                    NiScratchPad::BSplineFloatSegmentData);
                NIASSERT(pkExtraBasisData);
                uiTotalExtraFillSize += sizeof(NiBSplineBasisData);
            }
        }
    }

    if (m_aspEvaluators[SCALEEVAL])
    {
        if (m_aspEvaluators[SCALEEVAL]->GetChannelScratchPadInfo(
            0, false, pkPalette, uiExtraFillSize, bExtraSharedFillData, 
            eExtraSPBSegmentData, pkExtraBasisData))
        {
            uiTotalExtraFillSize += uiExtraFillSize;

            if (eExtraSPBSegmentData == SPBCUBICFLOATSEGMENT)
            {
                uiTotalExtraFillSize += sizeof(
                    NiScratchPad::CubicFloatSegmentData);
                NIASSERT(!pkExtraBasisData);
            }
            else
            {
                NIASSERT(eExtraSPBSegmentData == SPBBSPLINEFLOATSEGMENT);
                uiTotalExtraFillSize += sizeof(
                    NiScratchPad::BSplineFloatSegmentData);
                NIASSERT(pkExtraBasisData);
                uiTotalExtraFillSize += sizeof(NiBSplineBasisData);
            }
        }
    }

    uiFillSize = sizeof(NiScratchPad::LookAtFillData) + uiTotalExtraFillSize;

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

    // Share the fill data with all channels.
    bSharedFillData = true;

    pkBasisData = 0;

    return true;
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::InitChannelScratchPadData(unsigned int uiChannel, 
    NiEvaluatorSPData* pkEvalSPData, NiBSplineBasisData*, 
    bool bInitSharedData, NiAVObjectPalette* pkPalette, 
    NiPoseBufferHandle kPBHandle) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));
    NIASSERT(pkPalette);

    // Check if the dependent objects can be found.
    if (!m_kLookAtObjectName.Exists() || !m_kDrivenObjectName.Exists())
    {
        return false;
    }
    NiAVObject* pkLookAtObject = pkPalette->GetAVObject(m_kLookAtObjectName);
    if (!pkLookAtObject)
    {
        return false;
    }
    NiAVObject* pkDrivenObject = pkPalette->GetAVObject(m_kDrivenObjectName);
    if (!pkDrivenObject)
    {
        return false;
    }

    // Initialize the evaluator scratch pad data header.
    bool bPosed = IsRawEvalChannelPosed(uiChannel);
    NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
    NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
    // Use the LookAt fill function.
    pkEvalSPData->SetSPFillFunc(&LookAtFillFunction);

    // Initialize the scratch pad fill data.
    NiScratchPad::LookAtFillData* pkLookAtFill = 
        (NiScratchPad::LookAtFillData*)pkEvalSPData->GetSPFillData();
    NIASSERT(pkLookAtFill);
    if (bInitSharedData)
    {
        // Clear the contents of the scratch pad fill data.
        unsigned int uiFillSize;
        bool bSharedFillData;
        NiScratchPadBlock eSPBSegmentData;
        NiBSplineBasisData* pkBasisData;
        NIVERIFY(GetChannelScratchPadInfo(uiChannel, true, pkPalette, 
            uiFillSize, bSharedFillData, eSPBSegmentData, pkBasisData));
        memset(pkLookAtFill, 0, uiFillSize);

        pkLookAtFill->m_fLastTime = -NI_INFINITY;
        pkLookAtFill->m_usFlags = m_uFlags;
        pkLookAtFill->m_spLookAtObject = pkLookAtObject;
        pkLookAtFill->m_spDrivenObject = pkDrivenObject;
        pkLookAtFill->m_fRoll = -NI_INFINITY;
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkLookAtFill->m_kPos = NiPoint3::ZERO;
        pkLookAtFill->m_fScale = 1.0f;
#endif

        // The extra fill data begins immediately after the 
        // LookAtFillData struct.
        char* pcExtraFillData = (char*)(pkLookAtFill + 1);
        for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
        {
            NIASSERT(pkLookAtFill->m_apkSPBasisData[ui] == NULL);

            // Use placement new to initialize the internal eval SP data.
            NiEvaluatorSPData* pkIntEvalSPData = 
                &pkLookAtFill->m_akEvalSPData[ui];
            new(pkIntEvalSPData) NiEvaluatorSPData();
            pkIntEvalSPData->SetLOD(pkEvalSPData->GetLOD());
            NIASSERT(pkIntEvalSPData->GetEvalChannelIndex() == 
                NiEvaluatorSPData::EVALBASEINDEX);
            NIASSERT(pkIntEvalSPData->GetFlags() == 0);
            NIASSERT(pkIntEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
            NIASSERT(pkIntEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
            NIASSERT(pkIntEvalSPData->GetSPFillFunc() == NULL);
            NIASSERT(pkIntEvalSPData->GetSPFillData() == NULL);
            NIASSERT(pkIntEvalSPData->GetSPSegmentData() == NULL);

            pkIntEvalSPData->SetEvaluator(m_aspEvaluators[ui]);

            if (m_aspEvaluators[ui] != NULL)
            {
                unsigned int uiExtraFillSize;
                bool bExtraSharedFillData;
                NiScratchPadBlock eExtraSPBSegmentData;
                NiBSplineBasisData* pkExtraBasisData;
                if (m_aspEvaluators[ui]->GetChannelScratchPadInfo(0, false, 
                    pkPalette, uiExtraFillSize, bExtraSharedFillData, 
                    eExtraSPBSegmentData, pkExtraBasisData))
                {
                    if (uiExtraFillSize > 0)
                    {
                        pkIntEvalSPData->SetSPFillData(pcExtraFillData);
                        pcExtraFillData += uiExtraFillSize;
                    }

                    NiPoseBufferChannelType ePBChannelType = PBFLOATCHANNEL;
                    pkIntEvalSPData->SetSPSegmentData(pcExtraFillData);
                    switch(eExtraSPBSegmentData)
                    {
                        case SPBCUBICFLOATSEGMENT:
                            pcExtraFillData += sizeof(
                                NiScratchPad::CubicFloatSegmentData);
                            break;
                        case SPBBSPLINEFLOATSEGMENT:
                            pcExtraFillData += sizeof(
                                NiScratchPad::BSplineFloatSegmentData);
                            break;
                        case SPBCUBICPOINT3SEGMENT:
                            pcExtraFillData += sizeof(
                                NiScratchPad::CubicPoint3SegmentData);
                            ePBChannelType = PBPOINT3CHANNEL;
                            break;
                        case SPBBSPLINEPOINT3SEGMENT:
                            pcExtraFillData += sizeof(
                                NiScratchPad::BSplinePoint3SegmentData);
                            ePBChannelType = PBPOINT3CHANNEL;
                            break;
                        default:
                            NIASSERT(false);
                            break;
                    }

                    NiBSplineBasisData* pkSPBasisData = NULL;
                    if (pkExtraBasisData)
                    {
                        // Use placement new to initialize the basis data.
                        new(pcExtraFillData) NiBSplineBasisData();
                        pkSPBasisData = (NiBSplineBasisData*)pcExtraFillData;
                        pkLookAtFill->m_apkSPBasisData[ui] = pkSPBasisData;
                        pcExtraFillData += sizeof(NiBSplineBasisData);

                        // Increment the ref count to prevent deletion.
                        pkBasisData->IncRefCount();

                        // Copy the current value to the scratch pad.
                        pkBasisData->SetBasis(
                            &pkExtraBasisData->GetDegree3Basis());
                    }

                    // Initialize the channel's scratch pad data using 
                    // an invalid pose buffer index since this evaluator 
                    // should never write to a pose buffer.
                    m_aspEvaluators[ui]->InitChannelScratchPadData(
                        0, pkIntEvalSPData, pkSPBasisData, true, pkPalette, 
                        NiPoseBufferHandle(ePBChannelType, 
                        NiScratchPad::INVALIDOUTPUTINDEX));
                }
            }
        }

#ifdef NIDEBUG
        NIASSERT(pcExtraFillData <= ((char*)pkLookAtFill) + uiFillSize);
#endif

        // Fill in any posed translation or scale values.
        if (m_aspEvaluators[TRANSLATEEVAL])
        {
            NiPoint3 kPos;
            if (m_aspEvaluators[TRANSLATEEVAL]->GetChannelPosedValue(0, &kPos))
            {
                pkLookAtFill->m_kPos = kPos;
            }
        }

        if (m_aspEvaluators[ROLLEVAL])
        {
            float fRoll;
            if (m_aspEvaluators[ROLLEVAL]->GetChannelPosedValue(0, &fRoll))
            {
                pkLookAtFill->m_fRoll = fRoll;
            }
        }

        if (m_aspEvaluators[SCALEEVAL])
        {
            float fScale;
            if (m_aspEvaluators[SCALEEVAL]->GetChannelPosedValue(0, &fScale))
            {
                pkLookAtFill->m_fScale = fScale;
            }
        }
    }

    if (uiChannel == POSITION)
    {
        // Partially initialize the scratch pad segment data.
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
        pkCubicSeg->m_kOutTangent0 = NiPoint3::ZERO;
        pkCubicSeg->m_kA0 = NiPoint3::ZERO;
        pkCubicSeg->m_kB0 = NiPoint3::ZERO;
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkCubicSeg->m_kValue0 = NiPoint3::ZERO;
#endif
        if (bPosed)
        {
            // Fill the remainder of the scratch pad segment data.
            pkCubicSeg->m_kValue0 = pkLookAtFill->m_kPos;

            // Mark the segment data as valid for all sequence times.
            pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        }
    }
    else if (uiChannel == ROTATION)
    {
        NIASSERT(!bPosed);

        // Partially initialize the scratch pad segment data.
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
 #ifdef NIDEBUG
        // The fill function should set these fields.
        pkSlerpSeg->m_kQuat0 = NiQuaternion::IDENTITY;
        pkSlerpSeg->m_kQuat1 = NiQuaternion::IDENTITY;
#endif
    }
    else
    {
        NIASSERT(uiChannel == SCALE);

        // Partially initialize the scratch pad segment data.
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
        pkCubicSeg->m_fOutTangent0 = 0.0f;
        pkCubicSeg->m_fA0 = 0.0f;
        pkCubicSeg->m_fB0 = 0.0f;
#ifdef NIDEBUG
        // The fill function should set these fields.
        pkCubicSeg->m_fValue0 = 0.0f;
#endif

        if (bPosed)
        {
            // Fill the remainder of the scratch pad segment data.
            pkCubicSeg->m_fValue0 = pkLookAtFill->m_fScale;

            // Mark the segment data as valid for all sequence times.
            pkEvalSPData->SetSPSegmentTimeRange(0.0f, NI_INFINITY);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::ShutdownChannelScratchPadData(
#ifdef NIDEBUG
    unsigned int uiChannel, 
#else
    unsigned int, 
#endif
    NiEvaluatorSPData* pkEvalSPData) const
{
    NIASSERT(uiChannel < EVALMAXCHANNELS);
    NIASSERT(pkEvalSPData);
    NIASSERT(pkEvalSPData->GetEvaluator() == this);
    NIASSERT((unsigned int)pkEvalSPData->GetEvalChannelIndex() == uiChannel);
    NIASSERT(!IsEvalChannelInvalid(uiChannel));

    // The fill data is shared by all channels so we check the flags to
    // prevent multiple shut down attempts.
    NiScratchPad::LookAtFillData* pkLookAtFill = 
        (NiScratchPad::LookAtFillData*)pkEvalSPData->GetSPFillData();
    NIASSERT(pkLookAtFill);

    if ((pkLookAtFill->m_usFlags & SHUTDOWNCALLED) == 0)
    {
        pkLookAtFill->m_usFlags |= SHUTDOWNCALLED;

        // Clean up the smart pointers.
        pkLookAtFill->m_spLookAtObject = NULL;
        pkLookAtFill->m_spDrivenObject = NULL;

        // Use placement delete to destroy the evaluator data headers.
        for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
        {
            pkLookAtFill->m_akEvalSPData[ui].~NiEvaluatorSPData();
        }

        // Use placement delete to destroy the b-spline basis data.
        for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
        {
            if (pkLookAtFill->m_apkSPBasisData[ui])
            {
                // At this point the ref count should be exactly one.
                // This final reference ensured the object wasn't deleted.
                NIASSERT(pkLookAtFill->m_apkSPBasisData[ui]->GetRefCount() == 
                    1);
                pkLookAtFill->m_apkSPBasisData[ui]->~NiBSplineBasisData();
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::LookAtFillFunction(float fTime, 
    NiEvaluatorSPData* pkEvalSPData)
{
    NIASSERT(pkEvalSPData);

    // Get the scratch pad fill data.
    NiScratchPad::LookAtFillData* pkLookAtFill = 
        (NiScratchPad::LookAtFillData*)
        pkEvalSPData->GetSPFillData();
    NIASSERT(pkLookAtFill);
    NIASSERT(pkLookAtFill->m_spLookAtObject);
    NIASSERT(pkLookAtFill->m_spDrivenObject);

    // Update the local position, roll, and scale. The internal evaluators must
    // be updated when not posed and the time doesn't match the prior time.
    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        NiEvaluatorSPData* pkIntEvalSPData = 
            &pkLookAtFill->m_akEvalSPData[ui];
        NiEvaluator* pkEvaluator = pkIntEvalSPData->GetEvaluator();

        if (pkEvaluator && (pkEvaluator->IsRawEvalChannelPosed(0) ||
            fTime == pkLookAtFill->m_fLastTime))
        {
            continue;
        }

        if (ui == TRANSLATEEVAL)
        {
            if (pkEvaluator)
            {
                // Obtain the local position from the evaluator.
                pkEvaluator->UpdateChannel(fTime, 0, pkIntEvalSPData, 
                    &pkLookAtFill->m_kPos);
            }
            else
            {
                // Obtain the local position from the driven node.
                pkLookAtFill->m_kPos = 
                    pkLookAtFill->m_spDrivenObject->GetTranslate();
            }
        }
        else if (ui == ROLLEVAL)
        {
            if (pkEvaluator)
            {
                // Obtain the roll from the evaluator.
                pkEvaluator->UpdateChannel(fTime, 0, pkIntEvalSPData, 
                    &pkLookAtFill->m_fRoll);
            }
        }
        else
        {
            NIASSERT(ui == SCALEEVAL);
            if (pkEvaluator)
            {
                // Obtain the local scale from the evaluator.
                pkEvaluator->UpdateChannel(fTime, 0, pkIntEvalSPData, 
                    &pkLookAtFill->m_fScale);
            }
            else
            {
                // Obtain the local scale from the driven node.
                pkLookAtFill->m_fScale = 
                    pkLookAtFill->m_spDrivenObject->GetScale();
            }
        }
    }

    // Fill the scratch pad segment data for the given channel.
    unsigned int uiChannel = pkEvalSPData->GetEvalChannelIndex();
    if (uiChannel == POSITION)
    {
        // Fill the scratch pad with the current position.
        NiScratchPad::CubicPoint3SegmentData* pkCubicSeg = 
            (NiScratchPad::CubicPoint3SegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicSeg);
        NIASSERT(pkCubicSeg->m_fStartTime == 0);
        NIASSERT(pkCubicSeg->m_fInvDeltaTime == 0);
        pkCubicSeg->m_kValue0 = pkLookAtFill->m_kPos;
        NIASSERT(pkCubicSeg->m_kOutTangent0 == NiPoint3::ZERO);
        NIASSERT(pkCubicSeg->m_kA0 == NiPoint3::ZERO);
        NIASSERT(pkCubicSeg->m_kB0 == NiPoint3::ZERO);
    }
    else if (uiChannel == ROTATION)
    {
        // m_spDrivenObject has not yet had its world data updated. 
        // We compute what the world translation will be in here so 
        // that we can set kUp the rotation correctly.

        NiMatrix3 kParentWorldRot;
        NiPoint3 kMyWorldPos;
        NiNode* pkParent = pkLookAtFill->m_spDrivenObject->GetParent();
        if (pkParent)
        {
            kParentWorldRot = pkParent->GetWorldRotate();
            kMyWorldPos = pkParent->GetWorldTranslate() +
                pkParent->GetWorldScale() * (kParentWorldRot *
                pkLookAtFill->m_kPos);
        }
        else
        {
            kParentWorldRot = NiMatrix3::IDENTITY;
            kMyWorldPos = pkLookAtFill->m_kPos;
        }

        // Calculate kUp.
        NiPoint3 kLookAtWorldPos = 
            pkLookAtFill->m_spLookAtObject->GetWorldTranslate();
    
        NiMatrix3 kRot;
        NiPoint3 kAt = kLookAtWorldPos - kMyWorldPos;
        if (kAt.SqrLength() < 0.001f)
        {
            // We have a zero vector so don't update the transform.
            kRot.MakeIdentity();
        }
        else    
        {
            NiPoint3 kUp;
            float fDot;

            kAt.Unitize();

            if (((kAt.z < 1.001f) && (kAt.z > 0.999f)) ||
                ((kAt.z > -1.001f) && (kAt.z < -0.999f)))
            {
                // kUp & kAt are too close: use the Y axis as an alternate kUp.
                kUp = NiPoint3::UNIT_Y;
                fDot = kAt.y;
            }
            else
            {
                kUp = NiPoint3::UNIT_Z;
                fDot = kAt.z;
            }


            // Subtract off the component of kUp in the kAt direction.
            kUp -= fDot*kAt;
            kUp.Unitize();

            // Check the flip flag.
            if ((pkLookAtFill->m_usFlags & FLIP_MASK) == 0)
            {
                kAt = -kAt;
            }

            NiPoint3 kRight = kUp.Cross(kAt);

            // Check the desired axis.
            switch ((pkLookAtFill->m_usFlags & AXIS_MASK) >> AXIS_POS)
            {
                case X:
                    kRot.SetCol(0, kAt);
                    kRot.SetCol(1, kUp);
                    kRot.SetCol(2, -kRight);
                    break;
                case Y:
                    kRot.SetCol(0, kRight);
                    kRot.SetCol(1, kAt);
                    kRot.SetCol(2, -kUp);
                    break;
                case Z:
                    kRot.SetCol(0, kRight);
                    kRot.SetCol(1, kUp);
                    kRot.SetCol(2, kAt);
                    break;
            }

            // kAt this point "kRot" is the desired world rotation. The next
            // line converts it into the model space rotation necessary to
            // result in the desired world space rotation.
            kRot = kParentWorldRot.TransposeTimes(kRot);

            // Re-orient the local space rotation by the roll angle, 
            // if enabled.
            if (pkLookAtFill->m_fRoll != -NI_INFINITY)
            {
                NiMatrix3 roll;
                roll.MakeZRotation(-pkLookAtFill->m_fRoll);
                kRot = kRot * roll;
            }
        }

        // Convert from an NiMatrix to an NiQuaternion.
        NiQuaternion kQuatValue;
        kQuatValue.FromRotation(kRot);

        // Fill the scratch pad with the current rotation.
        NiScratchPad::SlerpRotSegmentData* pkSlerpRot = 
            (NiScratchPad::SlerpRotSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkSlerpRot);
        pkSlerpRot->m_kQuat0 = kQuatValue;
        pkSlerpRot->m_kQuat1 = kQuatValue;
    }
    else
    {
        NIASSERT(uiChannel == SCALE);

        // Fill the scratch pad with the current scale.
        NiScratchPad::CubicFloatSegmentData* pkCubicSeg = 
            (NiScratchPad::CubicFloatSegmentData*)
            pkEvalSPData->GetSPSegmentData();
        NIASSERT(pkCubicSeg);
        NIASSERT(pkCubicSeg->m_fStartTime == 0);
        NIASSERT(pkCubicSeg->m_fInvDeltaTime == 0);
        pkCubicSeg->m_fValue0 = pkLookAtFill->m_fScale;
        NIASSERT(pkCubicSeg->m_fOutTangent0 == 0.0f);
        NIASSERT(pkCubicSeg->m_fA0 == 0.0f);
        NIASSERT(pkCubicSeg->m_fB0 == 0.0f);
    }

    pkLookAtFill->m_fLastTime = fTime;

    return true;
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::SetEvalChannelTypes()
{
    // Rotation channel is always valid (and never posed).
    m_aiEvalChannelTypes[ROTATION] = EVALROTCHANNEL;

    // Check if the translation channel is valid.
    m_aiEvalChannelTypes[POSITION] = EVALINVALIDCHANNEL;
    if (m_aspEvaluators[TRANSLATEEVAL])
    {
        NIASSERT(m_aspEvaluators[TRANSLATEEVAL]->GetRawEvalPBChannelType(0) ==
            PBPOINT3CHANNEL);
        if (!m_aspEvaluators[TRANSLATEEVAL]->IsEvalChannelInvalid(0))
        {
            m_aiEvalChannelTypes[POSITION] = EVALPOINT3CHANNEL;

            if (m_aspEvaluators[TRANSLATEEVAL]->IsRawEvalChannelPosed(0))
            {
                m_aiEvalChannelTypes[POSITION] |= EVALPOSEDFLAG;
            }
        }
    }

    // Check if the scale channel is valid.
    m_aiEvalChannelTypes[SCALE] = EVALINVALIDCHANNEL;
    if (m_aspEvaluators[SCALEEVAL])
    {
        NIASSERT(m_aspEvaluators[SCALEEVAL]->GetRawEvalPBChannelType(0) == 
            PBFLOATCHANNEL);

        if (!m_aspEvaluators[SCALEEVAL]->IsEvalChannelInvalid(0))
        {
            m_aiEvalChannelTypes[SCALE] = EVALFLOATCHANNEL;

            if (m_aspEvaluators[SCALEEVAL]->IsRawEvalChannelPosed(0))
            {
                m_aiEvalChannelTypes[SCALE] |= EVALPOSEDFLAG;
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLookAtEvaluator);
//---------------------------------------------------------------------------
void NiLookAtEvaluator::CopyMembers(NiLookAtEvaluator* pkDest,
    NiCloningProcess& kCloning)
{
    NiEvaluator::CopyMembers(pkDest, kCloning);

    pkDest->m_uFlags = m_uFlags;
    pkDest->m_kLookAtObjectName = m_kLookAtObjectName;
    pkDest->m_kDrivenObjectName = m_kDrivenObjectName;
    
    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui] != NULL)
        {
            pkDest->m_aspEvaluators[ui] = (NiEvaluator*)
                m_aspEvaluators[ui]->Clone();
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLookAtEvaluator);
//---------------------------------------------------------------------------
void NiLookAtEvaluator::LoadBinary(NiStream& kStream)
{
    NiEvaluator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);
    kStream.LoadFixedString(m_kLookAtObjectName);
    kStream.LoadFixedString(m_kDrivenObjectName);

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        m_aspEvaluators[ui] = (NiEvaluator*)kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::LinkObject(NiStream& kStream)
{
    NiEvaluator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::RegisterStreamables(NiStream& kStream)
{
    if (!NiEvaluator::RegisterStreamables(kStream))
        return false;

    kStream.RegisterFixedString(m_kLookAtObjectName);
    kStream.RegisterFixedString(m_kDrivenObjectName);

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui] && !m_aspEvaluators[ui]->
            RegisterStreamables(kStream))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiLookAtEvaluator::SaveBinary(NiStream& kStream)
{
    NiEvaluator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);
    kStream.SaveFixedString(m_kLookAtObjectName);
    kStream.SaveFixedString(m_kDrivenObjectName);

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        kStream.SaveLinkID(m_aspEvaluators[ui]);
    }
}
//---------------------------------------------------------------------------
bool NiLookAtEvaluator::IsEqual(NiObject* pkObject)
{
    if (!NiEvaluator::IsEqual(pkObject))
    {
        return false;
    }

    NiLookAtEvaluator* pkDest = (NiLookAtEvaluator*)pkObject;

    if (m_uFlags != pkDest->m_uFlags ||
        m_kLookAtObjectName != pkDest->m_kLookAtObjectName || 
        m_kDrivenObjectName != pkDest->m_kDrivenObjectName)
    {
        return false;
    }

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        NiEvaluator* pkEval = m_aspEvaluators[ui];
        NiEvaluator* pkDestEval = pkDest->m_aspEvaluators[ui];
        if ((pkEval && !pkDestEval) ||
            (!pkEval && pkDestEval) ||
            (pkEval && !pkEval->IsEqual(pkDestEval)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiLookAtEvaluator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiEvaluator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiLookAtEvaluator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_kLookAtObjectName", 
        m_kLookAtObjectName));

    pkStrings->Add(NiGetViewerString("m_kDrivenObjectName", 
        m_kDrivenObjectName));

    for (unsigned int ui = 0; ui < MAXEVALUATORS; ui++)
    {
        if (m_aspEvaluators[ui])
        {
            m_aspEvaluators[ui]->GetViewerStrings(pkStrings);
        }
    }
}
//---------------------------------------------------------------------------
