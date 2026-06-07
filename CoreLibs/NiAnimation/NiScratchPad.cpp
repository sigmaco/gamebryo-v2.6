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

#include "NiScratchPad.h"
#include "NiPoseBinding.h"
#include "NiPoseBuffer.h"
#include "NiLookAtEvaluator.h"
#include "NiControllerSequence.h"
#include "NiControllerManager.h"

NiImplementRTTI(NiScratchPad,NiObject);

//---------------------------------------------------------------------------
NiScratchPad::NiScratchPad(NiControllerSequence* pkSequence) : 
    m_pkControllerManager(NULL), m_uiScratchPadBufferSize(0), 
    m_pcScratchPadBuffer(NULL), m_usNumPoseBufferItems(0), 
    m_sLastPoseBufferUpdateLOD(NiControllerSequence::LODSKIPEVALUATOR), 
    m_fLastPoseBufferUpdateTime(-NI_INFINITY), 
    m_pkAccumEvaluator(NULL), m_fLastAccumScaledTime(-NI_INFINITY), 
    m_kLastAccumTransform(), m_kLoopAccumTransform(), 
    m_kStartAccumTransform(), m_kStartAccumRot()
{
    // Fill data is variable in size and does not have a constant offset.
    m_ausBlockItemSize[SPBFILLDATA] = 0;
    m_ausBlockItemSize[SPBEVALUATORSPDATA] = 
        sizeof(NiEvaluatorSPData);
    m_ausBlockItemSize[SPBBSPLINEBASISDATA] = 
        sizeof(NiBSplineBasisData);
    m_ausBlockItemSize[SPBLINEARCOLORSEGMENT] = 
        sizeof(LinearColorSegmentData);
    m_ausBlockItemSize[SPBBSPLINECOLORSEGMENT] = 
        sizeof(BSplineColorSegmentData);
    m_ausBlockItemSize[SPBCONSTANTBOOLSEGMENT] = 
        sizeof(ConstantBoolSegmentData);
    m_ausBlockItemSize[SPBCUBICFLOATSEGMENT] = 
        sizeof(CubicFloatSegmentData);
    m_ausBlockItemSize[SPBBSPLINEFLOATSEGMENT] = 
        sizeof(BSplineFloatSegmentData);
    m_ausBlockItemSize[SPBCUBICPOINT3SEGMENT] = 
        sizeof(CubicPoint3SegmentData);
    m_ausBlockItemSize[SPBBSPLINEPOINT3SEGMENT] = 
        sizeof(BSplinePoint3SegmentData);
    m_ausBlockItemSize[SPBSLERPROTSEGMENT] = 
        sizeof(SlerpRotSegmentData);
    m_ausBlockItemSize[SPBSQUADROTSEGMENT] = 
        sizeof(SquadRotSegmentData);
    m_ausBlockItemSize[SPBBSPLINEROTSEGMENT] = 
        sizeof(BSplineRotSegmentData);
    NIASSERT(SPBBSPLINEROTSEGMENT == SPBMAXSCRATCHPADBLOCKS - 1);

    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        m_apcBlock[ui] = NULL;
        m_ausNumBlockItems[ui] = 0;
        // Confirm 4 byte alignment
        NIASSERT(m_ausBlockItemSize[ui] % 4 == 0);
    }

    m_apkAccumEvalSPData[POSITION] = NULL;
    m_apkAccumEvalSPData[ROTATION] = NULL;
    m_apkAccumEvalSPData[SCALE] = NULL;

    Init(pkSequence);
}
//---------------------------------------------------------------------------
NiScratchPad::NiScratchPad() : 
    m_pkControllerManager(NULL), m_uiScratchPadBufferSize(0), 
    m_pcScratchPadBuffer(NULL), m_usNumPoseBufferItems(0), 
    m_sLastPoseBufferUpdateLOD(NiControllerSequence::LODSKIPEVALUATOR), 
    m_fLastPoseBufferUpdateTime(-NI_INFINITY), 
    m_pkAccumEvaluator(NULL), m_fLastAccumScaledTime(-NI_INFINITY), 
    m_kLastAccumTransform(), m_kLoopAccumTransform(), 
    m_kStartAccumTransform(), m_kStartAccumRot()
{
    // Fill data is variable in size and does not have a constant offset.
    m_ausBlockItemSize[SPBFILLDATA] = 0;
    m_ausBlockItemSize[SPBEVALUATORSPDATA] = 
        sizeof(NiEvaluatorSPData);
    m_ausBlockItemSize[SPBBSPLINEBASISDATA] = 
        sizeof(NiBSplineBasisData);
    m_ausBlockItemSize[SPBLINEARCOLORSEGMENT] = 
        sizeof(LinearColorSegmentData);
    m_ausBlockItemSize[SPBBSPLINECOLORSEGMENT] = 
        sizeof(BSplineColorSegmentData);
    m_ausBlockItemSize[SPBCONSTANTBOOLSEGMENT] = 
        sizeof(ConstantBoolSegmentData);
    m_ausBlockItemSize[SPBCUBICFLOATSEGMENT] = 
        sizeof(CubicFloatSegmentData);
    m_ausBlockItemSize[SPBBSPLINEFLOATSEGMENT] = 
        sizeof(BSplineFloatSegmentData);
    m_ausBlockItemSize[SPBCUBICPOINT3SEGMENT] = 
        sizeof(CubicPoint3SegmentData);
    m_ausBlockItemSize[SPBBSPLINEPOINT3SEGMENT] = 
        sizeof(BSplinePoint3SegmentData);
    m_ausBlockItemSize[SPBSLERPROTSEGMENT] = 
        sizeof(SlerpRotSegmentData);
    m_ausBlockItemSize[SPBSQUADROTSEGMENT] = 
        sizeof(SquadRotSegmentData);
    m_ausBlockItemSize[SPBBSPLINEROTSEGMENT] = 
        sizeof(BSplineRotSegmentData);
    NIASSERT(SPBBSPLINEROTSEGMENT == SPBMAXSCRATCHPADBLOCKS - 1);

    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        m_apcBlock[ui] = NULL;
        m_ausNumBlockItems[ui] = 0;
        // Confirm 4 byte alignment
        NIASSERT(m_ausBlockItemSize[ui] % 4 == 0);
        NIASSERT(ui == SPBFILLDATA || m_ausBlockItemSize[ui] > 0);
    }

    m_apkAccumEvalSPData[POSITION] = NULL;
    m_apkAccumEvalSPData[ROTATION] = NULL;
    m_apkAccumEvalSPData[SCALE] = NULL;
}
//---------------------------------------------------------------------------
NiScratchPad::NiScratchPad(NiEvaluator* pkEvaluator) : 
    m_pkControllerManager(NULL), m_uiScratchPadBufferSize(0), 
    m_pcScratchPadBuffer(NULL), m_usNumPoseBufferItems(0), 
    m_sLastPoseBufferUpdateLOD(NiControllerSequence::LODSKIPEVALUATOR), 
    m_fLastPoseBufferUpdateTime(-NI_INFINITY), 
    m_pkAccumEvaluator(NULL), m_fLastAccumScaledTime(-NI_INFINITY), 
    m_kLastAccumTransform(), m_kLoopAccumTransform(), 
    m_kStartAccumTransform(), m_kStartAccumRot()
{
    // Fill data is variable in size and does not have a constant offset.
    m_ausBlockItemSize[SPBFILLDATA] = 0;
    m_ausBlockItemSize[SPBEVALUATORSPDATA] = 
        sizeof(NiEvaluatorSPData);
    m_ausBlockItemSize[SPBBSPLINEBASISDATA] = 
        sizeof(NiBSplineBasisData);
    m_ausBlockItemSize[SPBLINEARCOLORSEGMENT] = 
        sizeof(LinearColorSegmentData);
    m_ausBlockItemSize[SPBBSPLINECOLORSEGMENT] = 
        sizeof(BSplineColorSegmentData);
    m_ausBlockItemSize[SPBCONSTANTBOOLSEGMENT] = 
        sizeof(ConstantBoolSegmentData);
    m_ausBlockItemSize[SPBCUBICFLOATSEGMENT] = 
        sizeof(CubicFloatSegmentData);
    m_ausBlockItemSize[SPBBSPLINEFLOATSEGMENT] = 
        sizeof(BSplineFloatSegmentData);
    m_ausBlockItemSize[SPBCUBICPOINT3SEGMENT] = 
        sizeof(CubicPoint3SegmentData);
    m_ausBlockItemSize[SPBBSPLINEPOINT3SEGMENT] = 
        sizeof(BSplinePoint3SegmentData);
    m_ausBlockItemSize[SPBSLERPROTSEGMENT] = 
        sizeof(SlerpRotSegmentData);
    m_ausBlockItemSize[SPBSQUADROTSEGMENT] = 
        sizeof(SquadRotSegmentData);
    m_ausBlockItemSize[SPBBSPLINEROTSEGMENT] = 
        sizeof(BSplineRotSegmentData);
    NIASSERT(SPBBSPLINEROTSEGMENT == SPBMAXSCRATCHPADBLOCKS - 1);

    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        m_apcBlock[ui] = NULL;
        m_ausNumBlockItems[ui] = 0;
        // Confirm 4 byte alignment
        NIASSERT(m_ausBlockItemSize[ui] % 4 == 0);
    }

    m_apkAccumEvalSPData[POSITION] = NULL;
    m_apkAccumEvalSPData[ROTATION] = NULL;
    m_apkAccumEvalSPData[SCALE] = NULL;

    Init(pkEvaluator);
}
//---------------------------------------------------------------------------
NiScratchPad::~NiScratchPad()
{
    Shutdown();
}
//---------------------------------------------------------------------------
void NiScratchPad::Update(float fTime, float fScaledTime, NiInt16 sLOD,
    NiPoseBuffer* pkPoseBuffer)
{
    NIASSERT(pkPoseBuffer);

    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)m_apcBlock[SPBEVALUATORSPDATA];
    NiEvaluatorSPData* pkEndEvalSPData = pkEvalSPData + 
        m_ausNumBlockItems[SPBEVALUATORSPDATA];

    if (fTime != m_fLastPoseBufferUpdateTime ||
        sLOD != m_sLastPoseBufferUpdateLOD)
    {
        // This is a new time or new LOD. 
        // Update all pose buffer entries based on the current LOD.
        while (pkEvalSPData < pkEndEvalSPData)
        {
            if (sLOD <= pkEvalSPData->GetLOD())
            {
                NIASSERT(pkEvalSPData->GetEvaluator());

                // Fill the scratch pad segment data, if stale.
                if (fScaledTime > pkEvalSPData->GetSPSegmentMaxTime() || 
                   (fScaledTime == pkEvalSPData->GetSPSegmentMaxTime() && 
                   (pkEvalSPData->IsStepFunction())) ||
                   fScaledTime < pkEvalSPData->GetSPSegmentMinTime())
                {
                    NiScratchPadFillFunc pfnFillFunc = 
                        pkEvalSPData->GetSPFillFunc();
                    NIASSERT(pfnFillFunc);
                    NIVERIFY((*pfnFillFunc)(fScaledTime, pkEvalSPData));
                }
            }
            pkEvalSPData++;
        }

        // Update the pose buffer from the scratch pad segment data.
        NiBSplineBasisData* pkSPBasisDataBlock = 
            (NiBSplineBasisData*)m_apcBlock[SPBBSPLINEBASISDATA];
        UpdatePoseBufferFromScratchPadSegmentData(fScaledTime, sLOD, 
            pkSPBasisDataBlock, pkPoseBuffer);

        // Update accum delta in pose buffer.
        if (m_pkAccumEvaluator)
        {
            UpdateAccumTransformDelta(fTime, fScaledTime, sLOD, pkPoseBuffer);
        }
    }
    else
    {
        // This is a duplicate time and LOD. Update selected pose buffer 
        // entries if the multitarget pose handler has not already been 
        // updated at this time.
        NIASSERT(m_pkControllerManager);
        NiMultiTargetPoseHandler* pkMTPoseHandler = 
            m_pkControllerManager->GetMultiTargetPoseHandler();

        if (fTime != pkMTPoseHandler->GetLastUpdateTime())
        {
            // Update evaluators marked as "always update".
            while (pkEvalSPData < pkEndEvalSPData)
            {
                if (sLOD <= pkEvalSPData->GetLOD() &&
                    pkEvalSPData->AlwaysUpdate())
                {
                    // Find the pose buffer entry.
                    NiEvaluator* pkEvaluator = pkEvalSPData->GetEvaluator();
                    NIASSERT(pkEvaluator);
                    NiUInt32 uiIndex = pkEvalSPData->GetEvalChannelIndex();
                    NiPoseBufferChannelType ePBChannelType = 
                        pkEvaluator->GetEvalPBChannelType(uiIndex);
                    SegmentDataHeader* pkSegHeader = 
                        (SegmentDataHeader*)pkEvalSPData->GetSPSegmentData();
                    NIASSERT(pkSegHeader);
                    NiPoseBufferHandle kPBHandle(ePBChannelType, 
                        pkSegHeader->m_usOutputIndex);
#ifdef NIDEBUG
                    const NiEvaluator::IDTag& kIDTag = pkEvaluator->GetIDTag();
                    NiPoseBufferHandle kPBHandleDebug;
                    NiInt16 sLODDebug;
                    NIASSERT(pkPoseBuffer->GetHandleAndLOD(
                        kIDTag.GetAVObjectName(), kIDTag.GetPropertyType(), 
                        kIDTag.GetCtlrType(), kIDTag.GetCtlrID(), 
                        kIDTag.GetEvaluatorID(), ePBChannelType, 
                        kPBHandleDebug, sLODDebug));
                    NIASSERT(kPBHandleDebug == kPBHandle);
#endif
                    // Call UpdateChannel to update the pose buffer.
                    float* pfPBDataPointer = 
                        pkPoseBuffer->GetDataPointer(kPBHandle);
                    NIASSERT(pfPBDataPointer);
                    pkEvaluator->UpdateChannel(fScaledTime, uiIndex,
                        pkEvalSPData, pfPBDataPointer);
                }
                pkEvalSPData++;
            }

            // Update accum delta in pose buffer.
            if (m_pkAccumEvaluator)
            {
                UpdateAccumTransformDelta(fTime, fScaledTime, sLOD, 
                    pkPoseBuffer);
            }
        }
    }

    m_fLastPoseBufferUpdateTime = fTime;
    m_sLastPoseBufferUpdateLOD = sLOD;
}
//---------------------------------------------------------------------------
void NiScratchPad::InitRecycledScratchPad(NiControllerSequence* pkSequence)
{
    m_usNumPoseBufferItems = 0;
    m_sLastPoseBufferUpdateLOD = NiControllerSequence::LODSKIPEVALUATOR;
    m_fLastPoseBufferUpdateTime = -NI_INFINITY;
    m_pkAccumEvaluator = NULL;
    m_apkAccumEvalSPData[POSITION] = NULL;
    m_apkAccumEvalSPData[ROTATION] = NULL;
    m_apkAccumEvalSPData[SCALE] = NULL;
    m_fLastAccumScaledTime = -NI_INFINITY;
    m_kLastAccumTransform.MakeInvalid();

    Init(pkSequence);
}
//---------------------------------------------------------------------------
void NiScratchPad::PrepareForRecycling()
{
    // Use placement delete to destroy the evaluator data headers.
    NiEvaluatorSPData* pkEvalSPDataArray = 
        (NiEvaluatorSPData*)m_apcBlock[SPBEVALUATORSPDATA];
    for (NiUInt32 ui = 0; ui < m_ausNumBlockItems[SPBEVALUATORSPDATA]; ui++)
    {
        NiEvaluator* pkEvaluator = pkEvalSPDataArray[ui].GetEvaluator();
        // Shut down the evaluator scratch pad entries, if required.
        if (pkEvaluator && pkEvaluator->IsShutdownRequired())
        {
            pkEvaluator->ShutdownChannelScratchPadData(
                pkEvalSPDataArray[ui].GetEvalChannelIndex(), 
                &pkEvalSPDataArray[ui]);
        }
        pkEvalSPDataArray[ui].~NiEvaluatorSPData();
    }

    // Use placement delete to destroy the b-spline basis data objects.
    NiBSplineBasisData* pkSPBasisDataBlock = 
        (NiBSplineBasisData*)m_apcBlock[SPBBSPLINEBASISDATA];
    for (NiUInt32 ui = 0; ui < m_ausNumBlockItems[SPBBSPLINEBASISDATA]; ui++)
    {
        // At this point the ref count should be exactly one.
        // This final reference ensured the object wasn't deleted.
        NIASSERT(pkSPBasisDataBlock[ui].GetRefCount() == 1);
        pkSPBasisDataBlock[ui].~NiBSplineBasisData();
    }

    // Clear the pointers to the internal data blocks 
    // within the scratch pad buffer.
    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        m_apcBlock[ui] = NULL;
        m_ausNumBlockItems[ui] = 0;
    }

    m_pkAccumEvaluator = NULL;

    m_pkControllerManager = NULL;
}
//---------------------------------------------------------------------------
void NiScratchPad::Init(NiControllerSequence* pkSequence)
{
    NIASSERT(pkSequence);

    m_pkControllerManager = pkSequence->GetOwner();
    NIASSERT(m_pkControllerManager);

    NiAVObjectPalette* pkObjectPalette = 
        m_pkControllerManager->GetObjectPalette();
    NIASSERT(pkObjectPalette);

    const NiFixedString& kAccumRootName = 
        m_pkControllerManager->GetAccumRootName();

    NiPoseBinding* pkPoseBinding = m_pkControllerManager->GetPoseBinding();
    NIASSERT(pkPoseBinding);
    NiPoseBuffer* pkPoseBuffer = pkSequence->GetPoseBuffer();
    NIASSERT(pkPoseBuffer);
    NIASSERT(pkPoseBuffer->GetPoseBinding() == pkPoseBinding);
    NIASSERT(pkPoseBuffer->GetNumTotalItems() == 
        pkPoseBinding->GetNumTotalBindings());

    // Ensure the scratch pad buffer is large enough to handle any
    // registered NiSequenceData.
    unsigned int uiMaxScratchPadSize = 
        m_pkControllerManager->GetMaxScratchPadBufferSize();
    if (m_uiScratchPadBufferSize < uiMaxScratchPadSize)
    {
        if (m_pcScratchPadBuffer)
        {
            NiFree(m_pcScratchPadBuffer);
        }
        m_pcScratchPadBuffer = NiAlloc(char, uiMaxScratchPadSize);
        m_uiScratchPadBufferSize = uiMaxScratchPadSize;
    }

    // Create an array on the stack to store the unique 
    // b-spline basis data objects. This is the order in 
    // which these objects will be stored in the scratch pad.
    NiUInt32 uiMaxBasisDatas = 
        m_pkControllerManager->GetMaxUniqueBasisDatas() + 1;
    NiBSplineBasisData** apkBasisDatas = NiStackAlloc(NiBSplineBasisData*, 
        uiMaxBasisDatas);

    // Determine the required composition and size of the scratch pad buffer.
    NiUInt32 uiNumBasisDatas = 0;
    NiUInt32 uiTotalFillSize = 0;
    NiUInt16 ausMaxItems[SPBMAXSCRATCHPADBLOCKS];
    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        ausMaxItems[ui] = 0;
    }

    // Determine the scratch pad requirements for all 
    // evaluators in the sequence.
    NiSequenceData* pkSeqData = pkSequence->GetSequenceData();
    NIASSERT(pkSeqData);
    NiUInt32 uiNumEvaluators = pkSeqData->GetNumEvaluators();
    for (NiUInt32 ui = 0; ui < uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = pkSeqData->GetEvaluatorAt(ui);
        NIASSERT(pkEvaluator);

        // Determine if the evaluator computes the accum transform.
        bool bForceAlwaysUpdate = false;
        unsigned int uiMaxEvalChannels = 
            pkEvaluator->GetMaxEvalChannels();
        NIASSERT(uiMaxEvalChannels == 1 ||
            (uiMaxEvalChannels == 3 && pkEvaluator->IsTransformEvaluator()));
        if (uiMaxEvalChannels == 3 && kAccumRootName.Exists() &&
            kAccumRootName == pkEvaluator->GetAVObjectName())
        {
            NIASSERT(!pkEvaluator->IsReferencedEvaluator());
            m_pkAccumEvaluator = pkEvaluator;
            bForceAlwaysUpdate = true;
        }

        // Process each channel separately.
        NiInt32 iSharedFillSize = 0;
        for (unsigned int uj = 0; uj < uiMaxEvalChannels; uj++)
        {
            NiPoseBufferChannelType ePBChannelType = 
                pkEvaluator->GetEvalPBChannelType(uj);
            if (ePBChannelType != PBINVALIDCHANNEL)
            {
                // Does the channel need the scratch pad?
                NiUInt32 uiFillSize;
                bool bSharedFillData;
                NiScratchPadBlock eSPBSegmentData;
                NiBSplineBasisData* pkBasisData;
                if (pkEvaluator->GetChannelScratchPadInfo(uj, 
                    bForceAlwaysUpdate, pkObjectPalette, uiFillSize, 
                    bSharedFillData, eSPBSegmentData, pkBasisData))
                {
                    // Confirm 4 byte alignment
                    NIASSERT(uiFillSize % 4 == 0);
                    NIASSERT(eSPBSegmentData != SPBEVALUATORSPDATA &&
                        eSPBSegmentData != SPBFILLDATA &&
                        eSPBSegmentData != SPBBSPLINEBASISDATA && 
                        eSPBSegmentData != SPBMAXSCRATCHPADBLOCKS);

                    if (bSharedFillData)
                    {
                        // All channels must share the fill data.
                        NIASSERT(iSharedFillSize >= 0);
                        if (iSharedFillSize == 0)
                        {
                            iSharedFillSize = uiFillSize;
                        }
                        else
                        {
                            // This shared memory has already been counted.
                            NIASSERT((NiInt32)uiFillSize == iSharedFillSize);
                            uiFillSize = 0;
                        }
                    }
                    else
                    {
                        // No channels can share the fill data.
                        NIASSERT(iSharedFillSize <= 0);
                        iSharedFillSize = -1; // -1 means not shared.
                    }

                    if (uiFillSize > 0)
                    {
                        ausMaxItems[SPBFILLDATA]++;
                        uiTotalFillSize += uiFillSize;
                    }
                    ausMaxItems[SPBEVALUATORSPDATA]++;
                    ausMaxItems[eSPBSegmentData]++;

                    // Check if this channel has a new basis data.
                    if (pkBasisData)
                    {
                        bool bFound = false;
                        for (NiUInt32 uk = 0; uk < uiNumBasisDatas; uk++)
                        {
                            if (pkBasisData == apkBasisDatas[uk])
                            {
                                bFound = true;
                                break;
                            }
                        }
                        if (!bFound)
                        {
                            // Add the new basis data to the array.
                            NIASSERT(uiNumBasisDatas < uiMaxBasisDatas);
                            apkBasisDatas[uiNumBasisDatas] = pkBasisData;
                            uiNumBasisDatas++;
                        }
                    }
                }
            }
        }
    }
    ausMaxItems[SPBBSPLINEBASISDATA] = (NiUInt16)uiNumBasisDatas;

#ifdef NIDEBUG
    // Check the scratch pad buffer size.
    NiUInt32 uiBufferSize = uiTotalFillSize;
    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        uiBufferSize += ausMaxItems[ui] * m_ausBlockItemSize[ui];
    }
    NIASSERT(uiBufferSize <= m_uiScratchPadBufferSize);

    unsigned int uiNumUniqueBasisDatas;
    NIASSERT(uiBufferSize == pkSeqData->GetScratchPadBufferSize(
        m_pkControllerManager->GetCumulativeAnimations(), 
        pkObjectPalette, uiNumUniqueBasisDatas));
        NIASSERT(uiNumBasisDatas == uiNumUniqueBasisDatas);
#endif

    // Initialize the pointers to the internal data blocks 
    // within the scratch pad buffer.
    m_apcBlock[SPBFILLDATA] = m_pcScratchPadBuffer;
    m_apcBlock[SPBEVALUATORSPDATA] = m_apcBlock[SPBFILLDATA] + uiTotalFillSize;
    NIASSERT(SPBBSPLINEROTSEGMENT == SPBMAXSCRATCHPADBLOCKS - 1);
    for (NiUInt32 ui = SPBEVALUATORSPDATA; ui < SPBBSPLINEROTSEGMENT; ui++)
    {
        m_apcBlock[ui + 1] = m_apcBlock[ui] + 
            ausMaxItems[ui] * m_ausBlockItemSize[ui];
    }
    NIASSERT(m_apcBlock[SPBBSPLINEROTSEGMENT] + 
        ausMaxItems[SPBBSPLINEROTSEGMENT] * 
        m_ausBlockItemSize[SPBBSPLINEROTSEGMENT] ==
        m_pcScratchPadBuffer + uiBufferSize);

    // Use placement new to initialize the evaluator data headers.
    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)m_apcBlock[SPBEVALUATORSPDATA];
    NiEvaluatorSPData* pkEndEvalSPData = pkEvalSPData + 
        ausMaxItems[SPBEVALUATORSPDATA];
    while (pkEvalSPData < pkEndEvalSPData)
    {
        // NiEvaluatorSPData is not a ref counted class.
        new(pkEvalSPData) NiEvaluatorSPData();
        pkEvalSPData++;
    }

    // Use placement new to initialize the b-spline basis data objects.
    NiBSplineBasisData* pkSPBasisDataBlock = 
        (NiBSplineBasisData*)m_apcBlock[SPBBSPLINEBASISDATA];
    NiBSplineBasisData* pkNewBasisData = pkSPBasisDataBlock;
    for (NiUInt32 ui = 0; ui < uiNumBasisDatas; ui++, pkNewBasisData++)
    {
        new(pkNewBasisData) NiBSplineBasisData();
        // Increment the ref count to prevent deletion.
        pkNewBasisData->IncRefCount();
        // Copy the current value to the scratch pad.
        pkNewBasisData->SetBasis(&apkBasisDatas[ui]->GetDegree3Basis());
    }
    m_ausNumBlockItems[SPBBSPLINEBASISDATA] = (NiUInt16)uiNumBasisDatas;

    // Initialize the scratch pad and pose buffer for each evaluator.
    // Note the segment data within the scratch pad won't be fully initialized
    // until the scratch pad fill functions are called at a later time.
    pkEvalSPData = (NiEvaluatorSPData*)m_apcBlock[SPBEVALUATORSPDATA];
    char* pcFillData = (char*)m_apcBlock[SPBFILLDATA];
    for (NiUInt32 ui = 0; ui < uiNumEvaluators; ui++)
    {
        NiEvaluator* pkEvaluator = pkSeqData->GetEvaluatorAt(ui);
        NIASSERT(pkEvaluator);

        // Find corresponding pose buffer handles.
        const NiEvaluator::IDTag& kIDTag = pkEvaluator->GetIDTag();
        const NiPoseBinding::BindInfo* pkBindInfo = 
            pkPoseBinding->GetBindInfo(kIDTag);
        if (!pkBindInfo)
        {
            NILOG("NiScratchPad::Init failed to find a bind info with "
                "the following identifiers:"
                "\n\tkAVObjectName\t%s\n\tkPropertyType\t%s\n"
                "\tkCtlrType\t\t%s\n\tkCtlrID\t\t\t%s\n"
                "\tkEvaluatorID\t%s\n", 
                (const char*)kIDTag.GetAVObjectName(),
                (const char*)kIDTag.GetPropertyType(), 
                (const char*)kIDTag.GetCtlrType(),
                (const char*)kIDTag.GetCtlrID(), 
                (const char*)kIDTag.GetEvaluatorID());
            NIASSERT(false);
            continue;
        }

        char* pcSharedFillData = NULL;
        bool bInitSharedData = true;
        unsigned int uiMaxEvalChannels = 
            pkEvaluator->GetMaxEvalChannels();
        for (unsigned int uj = 0; uj < uiMaxEvalChannels; uj++)
        {
            NiPoseBufferChannelType ePBChannelType;
            bool bPosed;
            pkEvaluator->GetEvalPBChannelType(uj, ePBChannelType, bPosed);
            if (ePBChannelType != PBINVALIDCHANNEL)
            {
                NiPoseBufferHandle kPBHandle = 
                    pkBindInfo->GetPBHandle(ePBChannelType);
                if (kPBHandle.IsValid())
                {
                    // As an efficiency, we validate the pose buffer entry 
                    // during initialization as opposed to every update call
                    // with the assumption the associated channel data is 
                    // valid for the entire duration of the sequence.
                    pkPoseBuffer->SetValid(kPBHandle, true);

                    // Determine if the evaluator channel must always 
                    // be updated (which includes the accum transform).
                    NiUInt8 ucFlags = 0;
                    if (pkEvaluator->AlwaysUpdate() || 
                        pkEvaluator == m_pkAccumEvaluator)
                    {
                        ucFlags = NiEvaluatorSPData::FLAGALWAYSUPDATE;
                        bPosed = false;
                    }

                    // Does the evaluator need the scratch pad?
                    NiUInt32 uiFillSize;
                    bool bSharedFillData;
                    NiScratchPadBlock eSPBSegmentData;
                    NiBSplineBasisData* pkBasisData;
                    bool bIsScratchPadRequired = 
                        pkEvaluator->GetChannelScratchPadInfo(uj, 
                        (pkEvaluator == m_pkAccumEvaluator), pkObjectPalette, 
                        uiFillSize, bSharedFillData, eSPBSegmentData, 
                        pkBasisData);

                    if (bPosed)
                    {
                        // Write posed (i.e. constant) channels directly to 
                        // the pose buffer. This optimization allows these 
                        // constant channels to be skipped during subsequent 
                        // Update calls.
                        float* pfPBDataPointer = 
                            pkPoseBuffer->GetDataPointer(kPBHandle);
                        NIASSERT(pfPBDataPointer);
                        if (pkEvaluator->IsReferencedEvaluator())
                        {
                            NiPoseBuffer::ReferencedItem* pkPBRefItem = 
                                (NiPoseBuffer::ReferencedItem*)pfPBDataPointer;
                            pkPBRefItem->m_pkControllerSequence = pkSequence;
                            pkPBRefItem->m_pkReferencedEvaluator = pkEvaluator;
                            pkPBRefItem->m_apkEvalSPData[uj] = 
                                bIsScratchPadRequired ? pkEvalSPData : NULL;
                        }
                        else
                        {
                            NIVERIFY(pkEvaluator->GetChannelPosedValue(
                                uj, pfPBDataPointer));
                        }
                    }

                    if (!bIsScratchPadRequired)
                    {
                        continue;
                    }

                    // Cache the scratch pad headers for the accum transform.
                    if (pkEvaluator == m_pkAccumEvaluator)
                    {
                        m_apkAccumEvalSPData[uj] = pkEvalSPData;
                    }

                    // Assign portions of the scratch pad to the evaluator.
                    pkEvalSPData->SetEvaluator(pkEvaluator);
                    if (ePBChannelType == PBREFERENCEDCHANNEL)
                    {
                        pkEvalSPData->SetLOD(
                            NiControllerSequence::LODSKIPEVALUATOR);
                    }
                    else
                    {
                        pkEvalSPData->SetLOD(pkBindInfo->m_sLOD);
                    }
                    pkEvalSPData->SetEvalChannelIndex(
                        (NiEvaluatorSPData::EvalChannelIndex)uj);
                    pkEvalSPData->SetFlags(ucFlags);
                    NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == 
                        NI_INFINITY);
                    NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == 
                        -NI_INFINITY);
                    NIASSERT(pkEvalSPData->GetSPFillFunc() == 
                        NULL);

                    if (uiFillSize > 0)
                    {
                        if (pcSharedFillData)
                        {
                            pkEvalSPData->SetSPFillData(pcSharedFillData);
                            bInitSharedData = false;
                        }
                        else
                        {
                            if (bSharedFillData)
                            {
                                pcSharedFillData = pcFillData;
                            }
                            pkEvalSPData->SetSPFillData(pcFillData);
                            pcFillData += uiFillSize;
                            NIASSERT(SPBFILLDATA + 1 == SPBEVALUATORSPDATA);
                            NIASSERT(pcFillData <= 
                                m_apcBlock[SPBEVALUATORSPDATA]);
                            m_ausNumBlockItems[SPBFILLDATA]++;
                            NIASSERT(m_ausNumBlockItems[SPBFILLDATA] <= 
                                ausMaxItems[SPBFILLDATA]);
                        }
                    }

                    char* pcSegmentData = m_apcBlock[eSPBSegmentData] + 
                        m_ausNumBlockItems[eSPBSegmentData] * 
                        m_ausBlockItemSize[eSPBSegmentData];
                    pkEvalSPData->SetSPSegmentData(pcSegmentData);
                    NIASSERT((eSPBSegmentData == SPBMAXSCRATCHPADBLOCKS - 1) ||
                        (pcSegmentData + m_ausBlockItemSize[eSPBSegmentData] <=
                        m_apcBlock[eSPBSegmentData + 1]));
                    NIASSERT((eSPBSegmentData < SPBMAXSCRATCHPADBLOCKS - 1) ||
                        (pcSegmentData + m_ausBlockItemSize[eSPBSegmentData] <=
                        m_pcScratchPadBuffer + m_uiScratchPadBufferSize));

                    m_ausNumBlockItems[eSPBSegmentData]++;
                    NIASSERT(m_ausNumBlockItems[eSPBSegmentData] <= 
                        ausMaxItems[eSPBSegmentData]);

                    m_ausNumBlockItems[SPBEVALUATORSPDATA]++;
                    NIASSERT(m_ausNumBlockItems[SPBEVALUATORSPDATA] <= 
                        ausMaxItems[SPBEVALUATORSPDATA]);

                    // Find the matching b-spline basis data.
                    NiUInt32 uiBSplineBasisDataIndex = 0;
                    if (pkBasisData)
                    {
                        for (NiUInt32 uk = 0; uk < uiNumBasisDatas; uk++)
                        {
                            if (pkBasisData == apkBasisDatas[uk])
                            {
                                // Add one for error checking purposes.
                                uiBSplineBasisDataIndex = uk + 1;
                                break;
                            }
                        }
                        NIASSERT(uiBSplineBasisDataIndex != 0);
                        uiBSplineBasisDataIndex--;
                    }

                    // Ask the evaluator to initialize the channel's
                    // assigned sections within the scratch pad.
                    pkEvaluator->InitChannelScratchPadData(uj, pkEvalSPData, 
                        pkSPBasisDataBlock + uiBSplineBasisDataIndex, 
                        bInitSharedData, pkObjectPalette, kPBHandle);

                    pkEvalSPData++;
                }
                else
                {
                    NILOG("NiScratchPad::Init failed to find a "
                        "pose buffer handle with the following identifiers:\n"
                        "\tkAVObjectName\t%s\n\tkPropertyType\t%s\n"
                        "\tkCtlrType\t\t%s\n\tkCtlrID\t\t\t%s\n"
                        "\tkEvaluatorID\t%s\n"
                        "\tePBChannelType\t%d\n", 
                        (const char*)kIDTag.GetAVObjectName(),
                        (const char*)kIDTag.GetPropertyType(), 
                        (const char*)kIDTag.GetCtlrType(),
                        (const char*)kIDTag.GetCtlrID(), 
                        (const char*)kIDTag.GetEvaluatorID(),
                        ePBChannelType);
                    NIASSERT(false);
                }
            }
        }
    }

    if (m_pkAccumEvaluator)
    {
        // Validate the pose buffer entries for the accum delta value.
        pkPoseBuffer->ValidateAccumDeltaItemsFromValidAccumItems();

        // Initialize the accum transform that is applied 
        // when the sequence loops.

        // Get the transforms at the begin and end times.
        // Ensure all components are valid.

        NiQuatTransform kBeginTransform;
        NiQuatTransform kEndTransform;
        float fDuration = pkSequence->GetDuration();
        if (m_apkAccumEvalSPData[POSITION])
        {
            m_pkAccumEvaluator->UpdateChannel(fDuration, 
                POSITION, m_apkAccumEvalSPData[POSITION], 
                kEndTransform.GetTranslatePointer());

            m_pkAccumEvaluator->UpdateChannel(0.0f, 
                POSITION, m_apkAccumEvalSPData[POSITION], 
                kBeginTransform.GetTranslatePointer());
        }
        if (!kBeginTransform.IsTranslateValid())
        {
            kBeginTransform.SetTranslate(NiPoint3::ZERO);
            NIASSERT(!kEndTransform.IsTranslateValid());
            kEndTransform.SetTranslate(NiPoint3::ZERO);
        }

        if (m_apkAccumEvalSPData[ROTATION])
        {
            m_pkAccumEvaluator->UpdateChannel(fDuration, 
                ROTATION, m_apkAccumEvalSPData[ROTATION], 
                kEndTransform.GetRotatePointer());

            m_pkAccumEvaluator->UpdateChannel(0.0f, 
                ROTATION, m_apkAccumEvalSPData[ROTATION], 
                kBeginTransform.GetRotatePointer());
        }
        if (!kBeginTransform.IsRotateValid())
        {
            kBeginTransform.SetRotate(NiQuaternion::IDENTITY);
            NIASSERT(!kEndTransform.IsRotateValid());
            kEndTransform.SetRotate(NiQuaternion::IDENTITY);
        }

        if (m_apkAccumEvalSPData[SCALE])
        {
            m_pkAccumEvaluator->UpdateChannel(fDuration, 
                SCALE, m_apkAccumEvalSPData[SCALE], 
                kEndTransform.GetScalePointer());

            m_pkAccumEvaluator->UpdateChannel(0.0f, 
                SCALE, m_apkAccumEvalSPData[SCALE], 
                kBeginTransform.GetScalePointer());
        }
        if (!kBeginTransform.IsScaleValid())
        {
            kBeginTransform.SetScale(1.0f);
            NIASSERT(!kEndTransform.IsScaleValid());
            kEndTransform.SetScale(1.0f);
        }

        // Initialize the loop transform to account for the end frame of
        // the completed loop and the first frame of the next loop.
        NiQuatTransform kInvBegin;
        kBeginTransform.Invert(kInvBegin);
        NiMatrix3 kRefRot;
        kInvBegin.GetRotate().ToRotation(kRefRot);
        kInvBegin.SetTranslate(kRefRot * kInvBegin.GetTranslate());

        kEndTransform.GetRotate().ToRotation(kRefRot);
        kInvBegin.SetTranslate(kRefRot * kInvBegin.GetTranslate());
        m_kLoopAccumTransform = kEndTransform * kInvBegin;
    }

    NiStackFree(apkBasisDatas);
}
//---------------------------------------------------------------------------
void NiScratchPad::Init(NiEvaluator* pkEvaluator)
{
    NIASSERT(pkEvaluator);

    // This function cannot handle evaluators which need to resolve 
    // dependencies such as referencing other nodes.
    if (NiIsKindOf(NiLookAtEvaluator, pkEvaluator))
    {
        NIASSERT(false);
        return;
    }

    // Obtain the maximum number of channels.
    unsigned int uiMaxEvalChannels = 
        pkEvaluator->GetMaxEvalChannels();
    NIASSERT(uiMaxEvalChannels == 1 ||
        (uiMaxEvalChannels == 3 && pkEvaluator->IsTransformEvaluator()));

    // Create an array on the stack to store the unique 
    // b-spline basis data objects. This is the order in 
    // which these objects will be stored in the scratch pad.
    NiUInt32 uiMaxBasisDatas = uiMaxEvalChannels + 1;
    NiBSplineBasisData** apkBasisDatas = NiStackAlloc(NiBSplineBasisData*, 
        uiMaxBasisDatas);

    // Determine the required composition and size of the scratch pad buffer.
    NiUInt32 uiNumBasisDatas = 0;
    NiUInt32 uiTotalFillSize = 0;
    NiUInt16 ausMaxItems[SPBMAXSCRATCHPADBLOCKS];
    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        ausMaxItems[ui] = 0;
    }

    // Process each raw channel separately without regard to 
    // the referenced channel flag.
    NiInt32 iSharedFillSize = 0;
    for (unsigned int uj = 0; uj < uiMaxEvalChannels; uj++)
    {
        NiPoseBufferChannelType ePBChannelType = 
            pkEvaluator->GetRawEvalPBChannelType(uj);
        if (ePBChannelType != PBINVALIDCHANNEL)
        {
            // Does the channel need the scratch pad?
            // It should since we are forcing the issue.
            NiUInt32 uiFillSize;
            bool bSharedFillData;
            NiScratchPadBlock eSPBSegmentData;
            NiBSplineBasisData* pkBasisData;
            NIVERIFY(pkEvaluator->GetChannelScratchPadInfo(uj, true, NULL, 
                uiFillSize, bSharedFillData, eSPBSegmentData, pkBasisData));

            // Confirm 4 byte alignment
            NIASSERT(uiFillSize % 4 == 0);
            NIASSERT(eSPBSegmentData != SPBEVALUATORSPDATA &&
                eSPBSegmentData != SPBFILLDATA &&
                eSPBSegmentData != SPBBSPLINEBASISDATA && 
                eSPBSegmentData != SPBMAXSCRATCHPADBLOCKS);

            if (bSharedFillData)
            {
                // All channels must share the fill data.
                NIASSERT(iSharedFillSize >= 0);
                if (iSharedFillSize == 0)
                {
                    iSharedFillSize = uiFillSize;
                }
                else
                {
                    // This shared memory has already been counted.
                    NIASSERT((NiInt32)uiFillSize == iSharedFillSize);
                    uiFillSize = 0;
                }
            }
            else
            {
                // No channels can share the fill data.
                NIASSERT(iSharedFillSize <= 0);
                iSharedFillSize = -1; // -1 means not shared.
            }

            if (uiFillSize > 0)
            {
                ausMaxItems[SPBFILLDATA]++;
                uiTotalFillSize += uiFillSize;
            }
            ausMaxItems[SPBEVALUATORSPDATA]++;
            ausMaxItems[eSPBSegmentData]++;

            // Check if this channel has a new basis data.
            if (pkBasisData)
            {
                bool bFound = false;
                for (NiUInt32 uk = 0; uk < uiNumBasisDatas; uk++)
                {
                    if (pkBasisData == apkBasisDatas[uk])
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    // Add the new basis data to the array.
                    NIASSERT(uiNumBasisDatas < uiMaxBasisDatas);
                    apkBasisDatas[uiNumBasisDatas] = pkBasisData;
                    uiNumBasisDatas++;
                }
            }
        }
    }
    ausMaxItems[SPBBSPLINEBASISDATA] = (NiUInt16)uiNumBasisDatas;

    // Calculate the scratch pad buffer size.
    NiUInt32 uiBufferSize = uiTotalFillSize;
    for (NiUInt32 ui = 0; ui < SPBMAXSCRATCHPADBLOCKS; ui++)
    {
        uiBufferSize += ausMaxItems[ui] * m_ausBlockItemSize[ui];
    }

    // Create the scratch pad buffer assuming one LOD level.
    m_pcScratchPadBuffer = NiAlloc(char, uiBufferSize);
    m_uiScratchPadBufferSize = uiBufferSize;

    // Initialize the pointers to the internal data blocks 
    // within the scratch pad buffer.
    m_apcBlock[SPBFILLDATA] = m_pcScratchPadBuffer;
    m_apcBlock[SPBEVALUATORSPDATA] = m_apcBlock[SPBFILLDATA] + uiTotalFillSize;
    NIASSERT(SPBBSPLINEROTSEGMENT == SPBMAXSCRATCHPADBLOCKS - 1);
    for (NiUInt32 ui = SPBEVALUATORSPDATA; ui < SPBBSPLINEROTSEGMENT; ui++)
    {
        m_apcBlock[ui + 1] = m_apcBlock[ui] + 
            ausMaxItems[ui] * m_ausBlockItemSize[ui];
    }
    NIASSERT(m_apcBlock[SPBBSPLINEROTSEGMENT] + 
        ausMaxItems[SPBBSPLINEROTSEGMENT] * 
        m_ausBlockItemSize[SPBBSPLINEROTSEGMENT] ==
        m_pcScratchPadBuffer + uiBufferSize);

    // Use placement new to initialize the evaluator data headers.
    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)m_apcBlock[SPBEVALUATORSPDATA];
    NiEvaluatorSPData* pkEndEvalSPData = pkEvalSPData + 
        ausMaxItems[SPBEVALUATORSPDATA];
    while (pkEvalSPData < pkEndEvalSPData)
    {
        // NiEvaluatorSPData is not a ref counted class.
        new(pkEvalSPData) NiEvaluatorSPData();
        pkEvalSPData++;
    }

    // Use placement new to initialize the b-spline basis data objects.
    NiBSplineBasisData* pkSPBasisDataBlock = 
        (NiBSplineBasisData*)m_apcBlock[SPBBSPLINEBASISDATA];
    NiBSplineBasisData* pkNewBasisData = pkSPBasisDataBlock;
    for (NiUInt32 ui = 0; ui < uiNumBasisDatas; ui++, pkNewBasisData++)
    {
        new(pkNewBasisData) NiBSplineBasisData();
        // Increment the ref count to prevent deletion.
        pkNewBasisData->IncRefCount();
        // Copy the current value to the scratch pad.
        pkNewBasisData->SetBasis(&apkBasisDatas[ui]->GetDegree3Basis());
    }
    m_ausNumBlockItems[SPBBSPLINEBASISDATA] = (NiUInt16)uiNumBasisDatas;

    // Initialize the scratch pad for the evaluator.
    // Note the segment data within the scratch pad won't be fully initialized
    // until the scratch pad fill functions are called at a later time.
    pkEvalSPData = (NiEvaluatorSPData*)m_apcBlock[SPBEVALUATORSPDATA];
    char* pcFillData = (char*)m_apcBlock[SPBFILLDATA];

    char* pcSharedFillData = NULL;
    bool bInitSharedData = true;
    for (unsigned int uj = 0; uj < uiMaxEvalChannels; uj++)
    {
        NiPoseBufferChannelType ePBChannelType = 
            pkEvaluator->GetRawEvalPBChannelType(uj);
        if (ePBChannelType != PBINVALIDCHANNEL)
        {
            // Does the evaluator need the scratch pad?
            // It should since we are forcing the issue.
            NiUInt32 uiFillSize;
            bool bSharedFillData;
            NiScratchPadBlock eSPBSegmentData;
            NiBSplineBasisData* pkBasisData;
            NIVERIFY(pkEvaluator->GetChannelScratchPadInfo(uj, true, NULL, 
                uiFillSize, bSharedFillData, eSPBSegmentData, pkBasisData));

            // Assign portions of the scratch pad to the evaluator.
            // Without any LOD information we must assume LOD 0.
            pkEvalSPData->SetEvaluator(pkEvaluator);
            pkEvalSPData->SetLOD(0);
            pkEvalSPData->SetEvalChannelIndex(
                (NiEvaluatorSPData::EvalChannelIndex)uj);
            pkEvalSPData->SetFlags(NiEvaluatorSPData::FLAGALWAYSUPDATE);
            NIASSERT(pkEvalSPData->GetSPSegmentMinTime() == NI_INFINITY);
            NIASSERT(pkEvalSPData->GetSPSegmentMaxTime() == -NI_INFINITY);
            NIASSERT(pkEvalSPData->GetSPFillFunc() == NULL);

            if (uiFillSize > 0)
            {
                if (pcSharedFillData)
                {
                    pkEvalSPData->SetSPFillData(pcSharedFillData);
                    bInitSharedData = false;
                }
                else
                {
                    if (bSharedFillData)
                    {
                        pcSharedFillData = pcFillData;
                    }
                    pkEvalSPData->SetSPFillData(pcFillData);
                    pcFillData += uiFillSize;
                    NIASSERT(SPBFILLDATA + 1 == SPBEVALUATORSPDATA);
                    NIASSERT(pcFillData <= 
                        m_apcBlock[SPBEVALUATORSPDATA]);
                    m_ausNumBlockItems[SPBFILLDATA]++;
                    NIASSERT(m_ausNumBlockItems[SPBFILLDATA] <= 
                        ausMaxItems[SPBFILLDATA]);
                }
            }

            char* pcSegmentData = m_apcBlock[eSPBSegmentData] + 
                m_ausNumBlockItems[eSPBSegmentData] * 
                m_ausBlockItemSize[eSPBSegmentData];
            pkEvalSPData->SetSPSegmentData(pcSegmentData);
            NIASSERT((eSPBSegmentData == SPBMAXSCRATCHPADBLOCKS - 1) ||
                (pcSegmentData + m_ausBlockItemSize[eSPBSegmentData] <=
                m_apcBlock[eSPBSegmentData + 1]));
            NIASSERT((eSPBSegmentData < SPBMAXSCRATCHPADBLOCKS - 1) ||
                (pcSegmentData + m_ausBlockItemSize[eSPBSegmentData] <=
                m_pcScratchPadBuffer + m_uiScratchPadBufferSize));

            m_ausNumBlockItems[eSPBSegmentData]++;
            NIASSERT(m_ausNumBlockItems[eSPBSegmentData] <= 
                ausMaxItems[eSPBSegmentData]);

            m_ausNumBlockItems[SPBEVALUATORSPDATA]++;
            NIASSERT(m_ausNumBlockItems[SPBEVALUATORSPDATA] <= 
                ausMaxItems[SPBEVALUATORSPDATA]);

            // Find the matching b-spline basis data.
            NiUInt32 uiBSplineBasisDataIndex = 0;
            if (pkBasisData)
            {
                for (NiUInt32 uk = 0; uk < uiNumBasisDatas; uk++)
                {
                    if (pkBasisData == apkBasisDatas[uk])
                    {
                        // Add one for error checking purposes.
                        uiBSplineBasisDataIndex = uk + 1;
                        break;
                    }
                }
                NIASSERT(uiBSplineBasisDataIndex != 0);
                uiBSplineBasisDataIndex--;
            }

            // Ask the evaluator to initialize the channel's
            // assigned sections within the scratch pad.
            pkEvaluator->InitChannelScratchPadData(uj, pkEvalSPData, 
                pkSPBasisDataBlock + uiBSplineBasisDataIndex, 
                bInitSharedData, NULL, 
                NiPoseBufferHandle(ePBChannelType, INVALIDOUTPUTINDEX));

            pkEvalSPData++;
        }
    }

    NiStackFree(apkBasisDatas);
}
//---------------------------------------------------------------------------
void NiScratchPad::Shutdown()
{
    PrepareForRecycling();

    NiFree(m_pcScratchPadBuffer);
    m_pcScratchPadBuffer = NULL;
    m_uiScratchPadBufferSize = 0;
}
//---------------------------------------------------------------------------
void NiScratchPad::UpdatePoseBufferFromScratchPadSegmentData(
    float fScaledTime, NiInt16 sLOD, NiBSplineBasisData*, 
    NiPoseBuffer* pkPoseBuffer)
{
    NIASSERT(pkPoseBuffer);

    // Bring the pose buffer up-to-date with latest pose binding.
    // Copy existing data (i.e. validity flags and constant items).
    pkPoseBuffer->AddNewItemsFromPoseBinding(true, true);

    // Ensure the pose buffer data pointers are valid.
    if (m_usNumPoseBufferItems != pkPoseBuffer->GetNumTotalItems())
    {
        m_usNumPoseBufferItems = (NiUInt16)pkPoseBuffer->GetNumTotalItems();
    }

    // Process the color segment data.
    NiColorA* pkPBColorBlock = (NiColorA*)pkPoseBuffer->GetColorDataBlock();

    // Perform linear color interpolation.
    LinearColorSegmentData* pkLinearColor = 
        (LinearColorSegmentData*)m_apcBlock[SPBLINEARCOLORSEGMENT];
    LinearColorSegmentData* pkEndLinearColor = pkLinearColor + 
        m_ausNumBlockItems[SPBLINEARCOLORSEGMENT];
    while (pkLinearColor < pkEndLinearColor)
    {
        if (sLOD <= pkLinearColor->m_kHeader.m_sLOD)
        {
            float fT = (fScaledTime - pkLinearColor->m_fStartTime) * 
                pkLinearColor->m_fInvDeltaTime;
            NIASSERT(pkLinearColor->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pkPBColorBlock[pkLinearColor->m_kHeader.m_usOutputIndex] = 
                pkLinearColor->m_kValue0 + pkLinearColor->m_kOutTangent0 * fT;
        }
        pkLinearColor++;
    }

    // Perform b-spline color interpolation.
    BSplineColorSegmentData* pkBSplineColor = 
        (BSplineColorSegmentData*)m_apcBlock[SPBBSPLINECOLORSEGMENT];
    BSplineColorSegmentData* pkEndBSplineColor = pkBSplineColor + 
        m_ausNumBlockItems[SPBBSPLINECOLORSEGMENT];
    while (pkBSplineColor < pkEndBSplineColor)
    {
        if (sLOD <= pkBSplineColor->m_kHeader.m_sLOD)
        {
            NIASSERT(pkBSplineColor->m_pkSPBasisData);
            NiBSplineBasis<float, 3>& kBasis = 
                pkBSplineColor->m_pkSPBasisData->GetDegree3Basis();
            float fBasis0 = kBasis.GetValue(0);
            float fBasis1 = kBasis.GetValue(1);
            float fBasis2 = kBasis.GetValue(2);
            float fBasis3 = kBasis.GetValue(3);
            NIASSERT(pkBSplineColor->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pkPBColorBlock[pkBSplineColor->m_kHeader.m_usOutputIndex].r = 
                fBasis0 * pkBSplineColor->m_afSourceArray[0] + 
                fBasis1 * pkBSplineColor->m_afSourceArray[4] + 
                fBasis2 * pkBSplineColor->m_afSourceArray[8] + 
                fBasis3 * pkBSplineColor->m_afSourceArray[12];
            pkPBColorBlock[pkBSplineColor->m_kHeader.m_usOutputIndex].g = 
                fBasis0 * pkBSplineColor->m_afSourceArray[1] + 
                fBasis1 * pkBSplineColor->m_afSourceArray[5] + 
                fBasis2 * pkBSplineColor->m_afSourceArray[9] + 
                fBasis3 * pkBSplineColor->m_afSourceArray[13];
            pkPBColorBlock[pkBSplineColor->m_kHeader.m_usOutputIndex].b = 
                fBasis0 * pkBSplineColor->m_afSourceArray[2] + 
                fBasis1 * pkBSplineColor->m_afSourceArray[6] + 
                fBasis2 * pkBSplineColor->m_afSourceArray[10] + 
                fBasis3 * pkBSplineColor->m_afSourceArray[14];
            pkPBColorBlock[pkBSplineColor->m_kHeader.m_usOutputIndex].a = 
                fBasis0 * pkBSplineColor->m_afSourceArray[3] + 
                fBasis1 * pkBSplineColor->m_afSourceArray[7] + 
                fBasis2 * pkBSplineColor->m_afSourceArray[11] + 
                fBasis3 * pkBSplineColor->m_afSourceArray[15];
        }
        pkBSplineColor++;
    }

    // Process the bool segment data.
    //
    float* pfPBBoolBlock = (float*)pkPoseBuffer->GetBoolDataBlock();

    // Copy the constant bool values.
    ConstantBoolSegmentData* pkConstantBool = 
        (ConstantBoolSegmentData*)m_apcBlock[SPBCONSTANTBOOLSEGMENT];
    ConstantBoolSegmentData* pkEndConstantBool = pkConstantBool + 
        m_ausNumBlockItems[SPBCONSTANTBOOLSEGMENT];
    while (pkConstantBool < pkEndConstantBool)
    {
        if (sLOD <= pkConstantBool->m_kHeader.m_sLOD)
        {
            NIASSERT(pkConstantBool->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pfPBBoolBlock[pkConstantBool->m_kHeader.m_usOutputIndex] = 
                pkConstantBool->m_fValue0;
        }
        pkConstantBool++;
    }

    // Process the float segment data.
    //
    float* pfPBFloatBlock = (float*)pkPoseBuffer->GetFloatDataBlock();

    // Perform cubic float interpolation.
    CubicFloatSegmentData* pkCubicFloat = 
        (CubicFloatSegmentData*)m_apcBlock[SPBCUBICFLOATSEGMENT];
    CubicFloatSegmentData* pkEndCubicFloat = pkCubicFloat + 
        m_ausNumBlockItems[SPBCUBICFLOATSEGMENT];
    while (pkCubicFloat < pkEndCubicFloat)
    {
        if (sLOD <= pkCubicFloat->m_kHeader.m_sLOD)
        {
            float fT = (fScaledTime - pkCubicFloat->m_fStartTime) * 
                pkCubicFloat->m_fInvDeltaTime;
            NIASSERT(pkCubicFloat->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pfPBFloatBlock[pkCubicFloat->m_kHeader.m_usOutputIndex] = 
                pkCubicFloat->m_fValue0 + (pkCubicFloat->m_fOutTangent0 + 
                (pkCubicFloat->m_fA0 + pkCubicFloat->m_fB0 * fT) * fT) * fT;
        }
        pkCubicFloat++;
    }

    // Perform b-spline float interpolation.
    BSplineFloatSegmentData* pkBSplineFloat = 
        (BSplineFloatSegmentData*)m_apcBlock[SPBBSPLINEFLOATSEGMENT];
    BSplineFloatSegmentData* pkEndBSplineFloat = pkBSplineFloat + 
        m_ausNumBlockItems[SPBBSPLINEFLOATSEGMENT];
    while (pkBSplineFloat < pkEndBSplineFloat)
    {
        if (sLOD <= pkBSplineFloat->m_kHeader.m_sLOD)
        {
            NIASSERT(pkBSplineFloat->m_pkSPBasisData);
            NiBSplineBasis<float, 3>& kBasis = 
                pkBSplineFloat->m_pkSPBasisData->GetDegree3Basis();
            float fBasis0 = kBasis.GetValue(0);
            float fBasis1 = kBasis.GetValue(1);
            float fBasis2 = kBasis.GetValue(2);
            float fBasis3 = kBasis.GetValue(3);
            NIASSERT(pkBSplineFloat->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pfPBFloatBlock[pkBSplineFloat->m_kHeader.m_usOutputIndex] = 
                fBasis0 * pkBSplineFloat->m_afSourceArray[0] + 
                fBasis1 * pkBSplineFloat->m_afSourceArray[1] + 
                fBasis2 * pkBSplineFloat->m_afSourceArray[2] + 
                fBasis3 * pkBSplineFloat->m_afSourceArray[3];
        }
        pkBSplineFloat++;
    }

    // Process the point3 segment data.
    //
    NiPoint3* pkPBPoint3Block = (NiPoint3*)pkPoseBuffer->GetPoint3DataBlock();

    // Perform cubic point3 interpolation.
    CubicPoint3SegmentData* pkCubicPt3 = 
        (CubicPoint3SegmentData*)m_apcBlock[SPBCUBICPOINT3SEGMENT];
    CubicPoint3SegmentData* pkEndCubicPt3 = pkCubicPt3 + 
        m_ausNumBlockItems[SPBCUBICPOINT3SEGMENT];
    while (pkCubicPt3 < pkEndCubicPt3)
    {
        if (sLOD <= pkCubicPt3->m_kHeader.m_sLOD)
        {
            float fT = (fScaledTime - pkCubicPt3->m_fStartTime) * 
                pkCubicPt3->m_fInvDeltaTime;
            NIASSERT(pkCubicPt3->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pkPBPoint3Block[pkCubicPt3->m_kHeader.m_usOutputIndex] = 
                pkCubicPt3->m_kValue0 + (pkCubicPt3->m_kOutTangent0 + 
                (pkCubicPt3->m_kA0 + pkCubicPt3->m_kB0 * fT) * fT) * fT;
        }
        pkCubicPt3++;
    }

    // Perform b-spline point3 interpolation.
    BSplinePoint3SegmentData* pkBSplinePt3 = 
        (BSplinePoint3SegmentData*)m_apcBlock[SPBBSPLINEPOINT3SEGMENT];
    BSplinePoint3SegmentData* pkEndBSplinePt3 = pkBSplinePt3 + 
        m_ausNumBlockItems[SPBBSPLINEPOINT3SEGMENT];
    while (pkBSplinePt3 < pkEndBSplinePt3)
    {
        if (sLOD <= pkBSplinePt3->m_kHeader.m_sLOD)
        {
            NIASSERT(pkBSplinePt3->m_pkSPBasisData);
            NiBSplineBasis<float, 3>& kBasis = 
                pkBSplinePt3->m_pkSPBasisData->GetDegree3Basis();
            float fBasis0 = kBasis.GetValue(0);
            float fBasis1 = kBasis.GetValue(1);
            float fBasis2 = kBasis.GetValue(2);
            float fBasis3 = kBasis.GetValue(3);
            NIASSERT(pkBSplinePt3->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pkPBPoint3Block[pkBSplinePt3->m_kHeader.m_usOutputIndex][0] = 
                fBasis0 * pkBSplinePt3->m_afSourceArray[0] + 
                fBasis1 * pkBSplinePt3->m_afSourceArray[3] + 
                fBasis2 * pkBSplinePt3->m_afSourceArray[6] + 
                fBasis3 * pkBSplinePt3->m_afSourceArray[9];
            pkPBPoint3Block[pkBSplinePt3->m_kHeader.m_usOutputIndex][1] = 
                fBasis0 * pkBSplinePt3->m_afSourceArray[1] + 
                fBasis1 * pkBSplinePt3->m_afSourceArray[4] + 
                fBasis2 * pkBSplinePt3->m_afSourceArray[7] + 
                fBasis3 * pkBSplinePt3->m_afSourceArray[10];
            pkPBPoint3Block[pkBSplinePt3->m_kHeader.m_usOutputIndex][2] = 
                fBasis0 * pkBSplinePt3->m_afSourceArray[2] + 
                fBasis1 * pkBSplinePt3->m_afSourceArray[5] + 
                fBasis2 * pkBSplinePt3->m_afSourceArray[8] + 
                fBasis3 * pkBSplinePt3->m_afSourceArray[11];
        }
        pkBSplinePt3++;
    }

    // Process the rot segment data.
    //
    NiQuaternion* pkPBRotBlock = 
        (NiQuaternion*)pkPoseBuffer->GetRotDataBlock();

    // Perform slerp rotation interpolation.
    SlerpRotSegmentData* pkSlerpRot = 
        (SlerpRotSegmentData*)m_apcBlock[SPBSLERPROTSEGMENT];
    SlerpRotSegmentData* pkEndSlerpRot = pkSlerpRot + 
        m_ausNumBlockItems[SPBSLERPROTSEGMENT];
    while (pkSlerpRot < pkEndSlerpRot)
    {
        if (sLOD <= pkSlerpRot->m_kHeader.m_sLOD)
        {
            float fT = (fScaledTime - pkSlerpRot->m_fStartTime) * 
                pkSlerpRot->m_fInvDeltaTime;
            NIASSERT(pkSlerpRot->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            if (fT == 0.0f)
            {
                // Directly copy the value to avoid round-off errors 
                // in the slerp computation. This ensures an exact
                // value for posed channels.
                pkPBRotBlock[pkSlerpRot->m_kHeader.m_usOutputIndex] =
                    pkSlerpRot->m_kQuat0;
            }
            else
            {
                NiQuaternion::Slerp(fT, pkSlerpRot->m_kQuat0, 
                    pkSlerpRot->m_kQuat1, &pkPBRotBlock[
                    pkSlerpRot->m_kHeader.m_usOutputIndex]);
            }
        }
        pkSlerpRot++;
    }

    // Perform squad rotation interpolation.
    SquadRotSegmentData* pkSquadRot = 
        (SquadRotSegmentData*)m_apcBlock[SPBSQUADROTSEGMENT];
    SquadRotSegmentData* pkEndSquadRot = pkSquadRot + 
        m_ausNumBlockItems[SPBSQUADROTSEGMENT];
    while (pkSquadRot < pkEndSquadRot)
    {
        NiScratchPad::SlerpRotSegmentData* pkBaseSeg = 
            &pkSquadRot->m_kBaseData;
        if (sLOD <= pkBaseSeg->m_kHeader.m_sLOD)
        {
            float fT = (fScaledTime - pkBaseSeg->m_fStartTime) * 
                pkBaseSeg->m_fInvDeltaTime;
            NIASSERT(pkBaseSeg->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pkPBRotBlock[pkBaseSeg->m_kHeader.m_usOutputIndex] = 
                NiQuaternion::Squad(fT, 
                pkBaseSeg->m_kQuat0, pkSquadRot->m_kOutQuat0, 
                pkSquadRot->m_kInQuat1, pkBaseSeg->m_kQuat1);
        }
        pkSquadRot++;
    }

    // Perform b-spline rotation interpolation.
    BSplineRotSegmentData* pkBSplineRot = 
        (BSplineRotSegmentData*)m_apcBlock[SPBBSPLINEROTSEGMENT];
    BSplineRotSegmentData* pkEndBSplineRot = pkBSplineRot + 
        m_ausNumBlockItems[SPBBSPLINEROTSEGMENT];
    while (pkBSplineRot < pkEndBSplineRot)
    {
        if (sLOD <= pkBSplineRot->m_kHeader.m_sLOD)
        {
            NIASSERT(pkBSplineRot->m_pkSPBasisData);
            NiBSplineBasis<float, 3>& kBasis = 
                pkBSplineRot->m_pkSPBasisData->GetDegree3Basis();
            float fBasis0 = kBasis.GetValue(0);
            float fBasis1 = kBasis.GetValue(1);
            float fBasis2 = kBasis.GetValue(2);
            float fBasis3 = kBasis.GetValue(3);
            NiQuaternion kQuat;
            kQuat.SetW(fBasis0 * pkBSplineRot->m_afSourceArray[0] + 
                       fBasis1 * pkBSplineRot->m_afSourceArray[4] + 
                       fBasis2 * pkBSplineRot->m_afSourceArray[8] + 
                       fBasis3 * pkBSplineRot->m_afSourceArray[12]);
            kQuat.SetX(fBasis0 * pkBSplineRot->m_afSourceArray[1] + 
                       fBasis1 * pkBSplineRot->m_afSourceArray[5] + 
                       fBasis2 * pkBSplineRot->m_afSourceArray[9] + 
                       fBasis3 * pkBSplineRot->m_afSourceArray[13]);
            kQuat.SetY(fBasis0 * pkBSplineRot->m_afSourceArray[2] + 
                       fBasis1 * pkBSplineRot->m_afSourceArray[6] + 
                       fBasis2 * pkBSplineRot->m_afSourceArray[10] + 
                       fBasis3 * pkBSplineRot->m_afSourceArray[14]);
            kQuat.SetZ(fBasis0 * pkBSplineRot->m_afSourceArray[3] + 
                       fBasis1 * pkBSplineRot->m_afSourceArray[7] + 
                       fBasis2 * pkBSplineRot->m_afSourceArray[11] + 
                       fBasis3 * pkBSplineRot->m_afSourceArray[15]);
            kQuat.FastNormalize();
            NIASSERT(pkBSplineRot->m_kHeader.m_usOutputIndex != 
                NiScratchPad::INVALIDOUTPUTINDEX);
            pkPBRotBlock[pkBSplineRot->m_kHeader.m_usOutputIndex] = kQuat;
        }
        pkBSplineRot++;
    }
}
//---------------------------------------------------------------------------
void NiScratchPad::UpdateAccumTransformDelta(float fTime, float fScaledTime, 
    NiInt16 sLOD, NiPoseBuffer* pkPoseBuffer)
{
    NIASSERT(pkPoseBuffer);

    // Gather the accum transform components from the pose buffer.
    NiQuatTransform kValue;

    NiPoint3* pkTranslate;
    if (m_apkAccumEvalSPData[POSITION] && 
        sLOD <= m_apkAccumEvalSPData[POSITION]->GetLOD() &&
        pkPoseBuffer->IsPoint3Valid(NiPoseBufferHandle(PBPOINT3CHANNEL, 0)))
    {
        pkTranslate = &pkPoseBuffer->GetPoint3DataBlock()->m_kPoint3;
        NIASSERT(pkTranslate->x != -NI_INFINITY);
        kValue.SetTranslate(*pkTranslate);
    }
    else
    {
        pkTranslate = NULL;
        kValue.SetTranslate(NiPoint3::ZERO);
    }

    NiQuaternion* pkRotate;
    if (m_apkAccumEvalSPData[ROTATION] && 
        sLOD <= m_apkAccumEvalSPData[ROTATION]->GetLOD() &&
        pkPoseBuffer->IsRotValid(NiPoseBufferHandle(PBROTCHANNEL, 0)))
    {
        pkRotate = &pkPoseBuffer->GetRotDataBlock()->m_kRot;
        NIASSERT(pkRotate->GetX() != -NI_INFINITY);
        kValue.SetRotate(*pkRotate);
    }
    else
    {
        pkRotate = NULL;
        kValue.SetRotate(NiQuaternion::IDENTITY);
    }

    float* pfScale;
    if (m_apkAccumEvalSPData[SCALE] && 
        sLOD <= m_apkAccumEvalSPData[SCALE]->GetLOD() &&
        pkPoseBuffer->IsFloatValid(NiPoseBufferHandle(PBFLOATCHANNEL, 0)))
    {
        pfScale = &pkPoseBuffer->GetFloatDataBlock()->m_fFloat;
        NIASSERT(*pfScale != -NI_INFINITY);
        kValue.SetScale(*pfScale);
    }
    else
    {
        pfScale = NULL;
        kValue.SetScale(1.0f);
    }

    // Initialize the start accum transform so the first frame
    // results in the identity transform.
    if (m_fLastAccumScaledTime == -NI_INFINITY)
    {
        NiQuatTransform kInvValue;
        kValue.Invert(kInvValue);
        kInvValue.GetRotate().ToRotation(m_kStartAccumRot);
        kInvValue.SetTranslate(m_kStartAccumRot * kInvValue.GetTranslate());
        m_kStartAccumTransform = kInvValue;
    }

    // Check if we looped.
    if (fScaledTime < m_fLastAccumScaledTime)
    {
        NiQuatTransform kLoop(m_kLoopAccumTransform);
        kLoop.SetTranslate(m_kStartAccumRot * kLoop.GetTranslate());
        m_kStartAccumTransform = m_kStartAccumTransform * kLoop;
        m_kStartAccumTransform.GetRotate().ToRotation(m_kStartAccumRot);
    }

    // Transform the current value to the starting reference frame.
    kValue.SetTranslate(m_kStartAccumRot * kValue.GetTranslate());
    kValue = m_kStartAccumTransform * kValue;

#ifdef NIDEBUG
    if (m_fLastAccumScaledTime == -NI_INFINITY)
    {
        if (kValue.IsTranslateValid())
        {
            NIASSERT(fabs(kValue.GetTranslate().x) < 0.00001f);
            NIASSERT(fabs(kValue.GetTranslate().y) < 0.00001f);
            NIASSERT(fabs(kValue.GetTranslate().z) < 0.00001f);
        }
        if (kValue.IsRotateValid())
        {
            NIASSERT(fabs(kValue.GetRotate().GetW() - 1.0f) < 0.00001f);
            NIASSERT(fabs(kValue.GetRotate().GetX()) < 0.00001f);
            NIASSERT(fabs(kValue.GetRotate().GetY()) < 0.00001f);
            NIASSERT(fabs(kValue.GetRotate().GetZ()) < 0.00001f);
        }
        if (kValue.IsScaleValid())
        {
            NIASSERT(kValue.GetScale() == 1.0f);
        }
    }
#endif

    if (m_kLastAccumTransform.IsTransformInvalid())
    {
        m_kLastAccumTransform = kValue;
    }

    // Compute the delta from last frame to this frame.
    NiQuatTransform kLastInv;
    m_kLastAccumTransform.Invert(kLastInv);
    if (kLastInv.IsRotateValid())
    {
        NiMatrix3 kRefRot;
        kLastInv.GetRotate().ToRotation(kRefRot);
        // Reuse kLastInv to hold the delta value.
        kLastInv = kLastInv * kValue;
        kLastInv.SetTranslate(
            kRefRot * kLastInv.GetTranslate());
    }
    else
    {
        // Reuse kLastInv to hold the delta value.
        kLastInv = kLastInv * kValue;
    }

    // Save the revised accum transform via the pose buffer data pointers,
    // and save the delta at index 1.
    if (pkTranslate)
    {
        *pkTranslate = kValue.GetTranslate();
        NIASSERT(kLastInv.IsTranslateValid());
        NIASSERT((pkTranslate + 1) == (NiPoint3*)pkPoseBuffer->
            GetDataPointer(NiPoseBufferHandle(PBPOINT3CHANNEL, 1)));
        if (fTime == m_fLastPoseBufferUpdateTime)
        {
            // Accumulate delta from previous update at this same time.
            kLastInv.SetTranslate(*(pkTranslate + 1) + 
                kLastInv.GetTranslate());
        }
        *(pkTranslate + 1) = kLastInv.GetTranslate();
    }
    if (pkRotate)
    {
        *pkRotate = kValue.GetRotate();
        NIASSERT(kLastInv.IsRotateValid());
        NIASSERT((pkRotate + 1) == (NiQuaternion*)pkPoseBuffer->
            GetDataPointer(NiPoseBufferHandle(PBROTCHANNEL, 1)));
        if (fTime == m_fLastPoseBufferUpdateTime)
        {
            // Accumulate delta from previous update at this same time.
            kLastInv.SetRotate(*(pkRotate + 1) * kLastInv.GetRotate());
        }
        *(pkRotate + 1) = kLastInv.GetRotate();
    }
    if (pfScale)
    {
        *pfScale = kValue.GetScale();
        NIASSERT(kLastInv.IsScaleValid());
        NIASSERT((pfScale + 1) == pkPoseBuffer->GetDataPointer(
            NiPoseBufferHandle(PBFLOATCHANNEL, 1)));
        if (fTime == m_fLastPoseBufferUpdateTime)
        {
            // Accumulate delta from previous update at this same time.
            kLastInv.SetScale(*(pfScale + 1) * kLastInv.GetScale());
        }
        *(pfScale + 1) = kLastInv.GetScale();
    }

    // Save the current value as the last accum transform.
    m_kLastAccumTransform = kValue;
    m_fLastAccumScaledTime = fScaledTime;
}
//---------------------------------------------------------------------------
