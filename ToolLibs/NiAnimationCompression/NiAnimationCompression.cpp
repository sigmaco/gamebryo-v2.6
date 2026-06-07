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

#include "NiAnimationCompression.h"
#include <NiAnimation.h>
#include <NiNode.h>

#define SPLINE_LIN_POS_SIZE (sizeof(NiPoint3)) 
#define SPLINE_LIN_ROT_SIZE (sizeof(NiQuaternion)) 
#define SPLINE_LIN_FLOAT_SIZE (sizeof(float)) 
#define SPLINE_LIN_COLOR_SIZE (sizeof(NiColorA)) 
#define SPLINE_LIN_BOOL_SIZE (sizeof(float)) 
#define COMP_LIN_POS_SIZE (3 * sizeof(short)) 
#define COMP_LIN_ROT_SIZE (4 * sizeof(short)) 
#define COMP_LIN_FLOAT_SIZE (sizeof(short)) 
#define COMP_LIN_COLOR_SIZE (4 * sizeof(short))
#define COMP_LIN_BOOL_SIZE (sizeof(short)) 

//---------------------------------------------------------------------------
void NiAnimationCompression::CompressEvaluators(
    NiSequenceData* pkSeqData, NiAVObject* pkCharacterRoot,
    float fCompressionRatio, bool bAlwaysCompress, bool bCompactControlPoints)
{
    float fDuration = pkSeqData->GetDuration();
    float fTimeStep = 1.0f/61.0f;

    // How many keys will we have if we sample? If this is less than the 
    // standard degree of our BSpline's, then the fitter will end up
    // trying to add points and will assert. Instead, we ignore this 
    // pathological case and just early out.
    float fSampledKeyCount = fDuration / fTimeStep;
    if (fSampledKeyCount < 4.0f)
        return;

    NiBasisDataList kBasisList;
    NiBSplineDataPtr spBSplineData = NiNew NiBSplineData();
    for (unsigned int ui = 0; ui < pkSeqData->GetNumEvaluators(); ui++)
    {
        NiEvaluatorPtr spEvaluator = pkSeqData->GetEvaluatorAt(ui);

        if (spEvaluator)
        {
            bool bObjCompress = true; 
            float fObjCompressionRatio = fCompressionRatio;
            bool bObjCompactControlPoints = bCompactControlPoints;

            const NiEvaluator::IDTag& kIDTag = spEvaluator->GetIDTag();
            const char* pcAVObjectName = kIDTag.GetAVObjectName();

            if (pcAVObjectName && pkCharacterRoot)
            {
                NiAVObject* pkObj = pkCharacterRoot->GetObjectByName(
                    pcAVObjectName);
                NIASSERT(pkObj);
                ComputePerObjectOverrides(pkObj, bObjCompress, 
                    fObjCompressionRatio, bObjCompactControlPoints);
            }

            if (bObjCompress == false || IgnoreEvaluator(spEvaluator))
            {
                continue;
            }

            
            NiEvaluatorPtr spNewEval = 
                CompressEvaluator(spEvaluator,
                fObjCompressionRatio, bAlwaysCompress, 
                spBSplineData, bObjCompactControlPoints, kBasisList, 
                fTimeStep);

            if (spNewEval)
            {
                pkSeqData->ReplaceEvaluatorAt(spNewEval, ui);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiAnimationCompression::CompressEvaluator(
    NiEvaluator* pkEvaluator,
    float fCompressionRatio, bool bAlwaysCompress, 
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList, float fTimeStep)
{
    
    if (!bAlwaysCompress)
    {
        if (NiIsKindOf(NiKeyBasedEvaluator, pkEvaluator))
        {
            NiKeyBasedEvaluator* pkKeyEval = (NiKeyBasedEvaluator*)
                pkEvaluator;

            unsigned int uiCurrentTotal = 0;
            unsigned int uiCompTotal = 0;
            for (unsigned int ui = 0; ui < pkKeyEval->GetKeyChannelCount();
                ui++)
            {
                bool bCannotCompress = false;
                if (!pkKeyEval->IsRawEvalChannelPosed(ui))
                {
                    uiCurrentTotal += pkKeyEval->GetAllocatedSize((unsigned short)ui);
                   
                    uiCompTotal += EstimateCompressedSize(pkKeyEval, ui,
                        bCannotCompress, fTimeStep, fCompressionRatio,
                        bCompactControlPoints);
                    
                    if (bCannotCompress)
                        return NULL;

                    NIASSERT((uiCurrentTotal == 0) || (uiCompTotal != 0));
                }
            }

            if (uiCompTotal >= uiCurrentTotal)
                return NULL;
        }
    }

    NiEvaluator* pkNewEval = NULL;

    if (pkEvaluator->IsTransformEvaluator())
    {
        pkNewEval = CompressTransformEvaluator(pkEvaluator, 
            fCompressionRatio, fTimeStep, pkBSplineData, 
            bCompactControlPoints, kBasisList);
    }
    else if (pkEvaluator->GetRawEvalPBChannelType(0) == PBCOLORCHANNEL)
    {
        pkNewEval = CompressColorEvaluator(pkEvaluator, fCompressionRatio,
            fTimeStep, pkBSplineData, bCompactControlPoints, kBasisList);
    }
    else if (pkEvaluator->GetRawEvalPBChannelType(0) == PBFLOATCHANNEL)
    {
        pkNewEval = CompressFloatEvaluator(pkEvaluator, fCompressionRatio,
            fTimeStep, pkBSplineData, bCompactControlPoints, kBasisList);
    }
    else if (pkEvaluator->GetRawEvalPBChannelType(0) == PBPOINT3CHANNEL)
    {
        pkNewEval = CompressPoint3Evaluator(pkEvaluator, fCompressionRatio,
            fTimeStep, pkBSplineData, bCompactControlPoints, kBasisList);
    }

    if (pkNewEval)
    {
        pkNewEval->SetIDTag(pkEvaluator->GetIDTag());
    }

    return pkNewEval;
}
//---------------------------------------------------------------------------
void NiAnimationCompression::ComputePerObjectOverrides(NiAVObject* pkObj, 
    bool& bCompress, float& fCompressionRatio, bool& bCompactControlPoints)
{
    NiBooleanExtraDataPtr spCompressED = NiDynamicCast(
        NiBooleanExtraData,
        pkObj->GetExtraData(NI_COMPRESS_KEY));
    if (spCompressED)
    {
        bCompress = spCompressED->GetValue();   
    }
    
    NiFloatExtraDataPtr spRatioED =  NiDynamicCast(
        NiFloatExtraData,
        pkObj->GetExtraData(NI_COMPRESSION_RATIO_KEY));
    if (spRatioED)
    {
        fCompressionRatio = spRatioED->GetValue();
    }
    
    NiBooleanExtraDataPtr spCompactED = NiDynamicCast(
        NiBooleanExtraData, pkObj->GetExtraData(
        NI_COMPACT_CONTROL_POINTS_KEY));
    if (spCompactED)
    {
        bCompactControlPoints = spCompactED->GetValue();
    }
}
//---------------------------------------------------------------------------
NiEvaluator* NiAnimationCompression::CompressTransformEvaluator(
    NiEvaluator* pkEvaluator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    NiQuatTransform kValue;
    float fStart;
    float fEnd;
    pkEvaluator->GetActiveTimeRange(fStart, fEnd);

    // Create a temporary scratch pad for the evaluator.
    NiScratchPadPtr spScratchPad = 
        (NiScratchPad*)NiNew NiScratchPad(pkEvaluator);

    // Sequence through the scratch pad entry for each evaluator channel.
    unsigned int uiNumEvalSPDatas = 
        spScratchPad->GetNumBlockItems(SPBEVALUATORSPDATA);
    if (uiNumEvalSPDatas == 0)
    {
        return NULL;
    }
    NIASSERT(uiNumEvalSPDatas <= NiEvaluator::EVALMAXCHANNELS);
    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)spScratchPad->GetDataBlock(SPBEVALUATORSPDATA);
    NIASSERT(pkEvalSPData);

    for (unsigned int uj = 0; uj < uiNumEvalSPDatas; uj++)
    {
        unsigned int uiChannelIdx = pkEvalSPData[uj].GetEvalChannelIndex();
        if (uiChannelIdx == NiTransformEvaluator::POSITION)
        {
            if (!pkEvaluator->UpdateChannel(fStart + fEnd / 2.0f, 
                uiChannelIdx, &pkEvalSPData[uj], 
                kValue.GetTranslatePointer()))
            {
                return NULL;
            }
        }
        else if (uiChannelIdx == NiTransformEvaluator::ROTATION)
        {
            if (!pkEvaluator->UpdateChannel(fStart + fEnd / 2.0f, 
                uiChannelIdx, &pkEvalSPData[uj], kValue.GetRotatePointer()))
            {
                return NULL;
            }
        }
        else 
        {
            NIASSERT(uiChannelIdx == NiTransformEvaluator::SCALE);
            if (!pkEvaluator->UpdateChannel(fStart + fEnd / 2.0f, 
                uiChannelIdx, &pkEvalSPData[uj], kValue.GetScalePointer()))
            {
                return NULL;
            }
        }
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplineTransformEvaluator* pkBSplineEval = NULL;
    
    if (bCompactControlPoints)
        pkBSplineEval = NiNew NiBSplineCompTransformEvaluator();
    else
        pkBSplineEval = NiNew NiBSplineTransformEvaluator();

    bool bTranslate = kValue.IsTranslateValid();
    bool bRotate = kValue.IsRotateValid();
    bool bScale = kValue.IsScaleValid();

    if (!bTranslate && !bRotate && !bScale)
    {
        NiDelete pkBSplineEval;
        return NULL;
    }
    float fNyquistTimeStep = fTimeStep;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    NiPoint3* pakTranslate = NULL;
    if (bTranslate)
        pakTranslate = NiNew NiPoint3[uiNumSampledKeys];
    
    NiQuaternion* pakRotate = NULL;
    if (bRotate)
        pakRotate = NiNew NiQuaternion[uiNumSampledKeys];
    
    float* pafScale = NULL;
    if (bScale)
        pafScale = NiAlloc(float, uiNumSampledKeys);

    // Sample the evaluator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        NIASSERT(fCurrentTime <= fEnd);
        pafTimes[ui] = fCurrentTime;

        for (unsigned int uj = 0; uj < uiNumEvalSPDatas; uj++)
        {
            unsigned int uiChannelIdx = pkEvalSPData[uj].GetEvalChannelIndex();
            if (uiChannelIdx == NiTransformEvaluator::POSITION)
            {
                if (!pkEvaluator->UpdateChannel(fCurrentTime, 
                    uiChannelIdx, &pkEvalSPData[uj], 
                    kValue.GetTranslatePointer()))
                {
                    return NULL;
                }
            }
            else if (uiChannelIdx == NiTransformEvaluator::ROTATION)
            {
                if (!pkEvaluator->UpdateChannel(fCurrentTime, 
                    uiChannelIdx, &pkEvalSPData[uj], 
                    kValue.GetRotatePointer()))
                {
                    return NULL;
                }
            }
            else 
            {
                NIASSERT(uiChannelIdx == NiTransformEvaluator::SCALE);
                if (!pkEvaluator->UpdateChannel(fCurrentTime, 
                    uiChannelIdx, &pkEvalSPData[uj], 
                    kValue.GetScalePointer()))
                {
                    return NULL;
                }
            }
        }

        if (bTranslate)
            pakTranslate[ui] = kValue.GetTranslate();

        if (bRotate)
            pakRotate[ui] = kValue.GetRotate();
        
        if (bScale)
            pafScale[ui] = kValue.GetScale();

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Determine if the evaluator is posed in translate
    if (bTranslate)
    {
        bool bAllSame = true;
        for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
        {
            if (pakTranslate[ui] != pakTranslate[ui - 1])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
        {
            bTranslate = false;
            pkBSplineEval->SetPoseTranslate(kValue.GetTranslate());
            NiDelete [] pakTranslate;
            pakTranslate = NULL;
        }
    }

    // Determine if the evaluator is posed in rotate
    if (bRotate)
    {
        bool bAllSame = true;
        for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
        {
            if (pakRotate[ui] != pakRotate[ui - 1])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
        {
            bRotate = false;
            pkBSplineEval->SetPoseRotate(kValue.GetRotate());
            NiDelete [] pakRotate;
            pakRotate = NULL;
        }
    }

    // Determine if the evaluator is posed in scale
    if (bScale)
    {
        bool bAllSame = true;
        for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
        {
            if (pafScale[ui] != pafScale[ui - 1])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
        {
            bScale = false;
            pkBSplineEval->SetPoseScale(kValue.GetScale());
            NiFree(pafScale);
            pafScale = NULL;
        }
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kTranslateHandle = NiBSplineData::INVALID_HANDLE;
    NiBSplineData::Handle kRotateHandle = NiBSplineData::INVALID_HANDLE;
    NiBSplineData::Handle kScaleHandle = NiBSplineData::INVALID_HANDLE;

    float fCompactScalars[6];
    for (unsigned int ui = 0; ui < 6; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (bTranslate)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<3,3> kFit(uiNumSampledKeys, 
            (const float*) pakTranslate, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineEval, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kTranslateHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kTranslateHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3);
        }

    }

    if (bRotate)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<4,3> kFit(uiNumSampledKeys, 
            (const float*) pakRotate, uiNumCompKeys);
        
        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineEval, kBasisList, 
                kFit.GetBasis());
        }


        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kRotateHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4,
                fCompactScalars[2], fCompactScalars[3]);
        }
        else
        {
            kRotateHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4);
        }
    }

    if (bScale)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<1,3> kFit(uiNumSampledKeys, 
            (const float*) pafScale, uiNumCompKeys);
        
        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineEval, kBasisList, 
                kFit.GetBasis());
        }


        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kScaleHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1,
                fCompactScalars[4], fCompactScalars[5]);
        }
        else
        {
            kScaleHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1);
        }
    }

    pkBSplineEval->SetTimeRange(fStart, fEnd);
    pkBSplineEval->SetData(pkBSplineData, spBasisData);
    pkBSplineEval->SetControlHandle(kTranslateHandle, 
        NiBSplineTransformEvaluator::POSITION);
    pkBSplineEval->SetControlHandle(kRotateHandle, 
        NiBSplineTransformEvaluator::ROTATION);
    pkBSplineEval->SetControlHandle(kScaleHandle, 
        NiBSplineTransformEvaluator::SCALE);

    if (bCompactControlPoints)
    {
        NiBSplineCompTransformEvaluator* pkCompEval = 
            (NiBSplineCompTransformEvaluator*) pkBSplineEval;
        
        pkCompEval->SetOffset(fCompactScalars[0], 
            NiBSplineTransformEvaluator::POSITION);
        pkCompEval->SetHalfRange(fCompactScalars[1], 
            NiBSplineTransformEvaluator::POSITION);
        pkCompEval->SetOffset(fCompactScalars[2], 
            NiBSplineTransformEvaluator::ROTATION);
        pkCompEval->SetHalfRange(fCompactScalars[3],
            NiBSplineTransformEvaluator::ROTATION);
        pkCompEval->SetOffset(fCompactScalars[4],
            NiBSplineTransformEvaluator::SCALE);       
        pkCompEval->SetHalfRange(fCompactScalars[5],
            NiBSplineTransformEvaluator::SCALE);

    }

    // delete the temp arrays and scratch pad
    NiFree(pafTimes);
    NiDelete [] pakTranslate;
    NiDelete [] pakRotate;
    NiFree(pafScale);
    spScratchPad = NULL;

    return pkBSplineEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiAnimationCompression::CompressColorEvaluator(
    NiEvaluator* pkEvaluator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    NiColorA kValue;
    float fStart;
    float fEnd;
    pkEvaluator->GetActiveTimeRange(fStart, fEnd);

    // Create a temporary scratch pad for the evaluator.
    NiScratchPadPtr spScratchPad = 
        (NiScratchPad*)NiNew NiScratchPad(pkEvaluator);

    // Obtain the scratch pad entry for the evaluator channel.
    unsigned int uiNumEvalSPDatas = 
        spScratchPad->GetNumBlockItems(SPBEVALUATORSPDATA);
    if (uiNumEvalSPDatas == 0)
    {
        return NULL;
    }
    NIASSERT(uiNumEvalSPDatas == 1);
    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)spScratchPad->GetDataBlock(SPBEVALUATORSPDATA);
    NIASSERT(pkEvalSPData);

    if (!pkEvaluator->UpdateChannel(fStart, 0, pkEvalSPData, &kValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplineColorEvaluator* pkBSplineEval = NULL;
    if (bCompactControlPoints)
        pkBSplineEval = NiNew NiBSplineCompColorEvaluator();
    else
        pkBSplineEval = NiNew NiBSplineColorEvaluator();

    float fNyquistTimeStep = fTimeStep;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    NiColorA* pakColor = NiNew NiColorA[uiNumSampledKeys];

    // Sample the evaluator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkEvaluator->UpdateChannel(fCurrentTime, 0, 
            pkEvalSPData, &kValue));

        pakColor[ui] = kValue;

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kColorAHandle = NiBSplineData::INVALID_HANDLE;

    // Determine if the evaluator is posed
    bool bAllSame = true;
    for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
    {
        if (pakColor[ui] != pakColor[ui - 1])
        {
            bAllSame = false;
            break;
        }
    }

    float fCompactScalars[2];
    for (unsigned int ui = 0; ui < 2; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (!bAllSame)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<4,3> kFit(uiNumSampledKeys, 
            (const float*) pakColor, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineEval, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kColorAHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kColorAHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4);
        }
    }

    pkBSplineEval->SetTimeRange(fStart, fEnd);
    pkBSplineEval->SetData(pkBSplineData, spBasisData);
    pkBSplineEval->SetControlHandle(kColorAHandle, 
        NiBSplineColorEvaluator::COLORA);

    if (bCompactControlPoints)
    {
        NiBSplineCompColorEvaluator* pkCompEval = 
            (NiBSplineCompColorEvaluator*) pkBSplineEval;
        
        pkCompEval->SetOffset(fCompactScalars[0], 
            NiBSplineColorEvaluator::COLORA);
        pkCompEval->SetHalfRange(fCompactScalars[1], 
            NiBSplineColorEvaluator::COLORA);
    }

    // delete the temp arrays and scratch pad
    NiFree(pafTimes);
    NiDelete [] pakColor;
    spScratchPad = NULL;

    if (bAllSame)
    {
        // Create a const color evaluator in place of the b-spline evaluator.
        NiDelete pkBSplineEval;
        return NiNew NiConstColorEvaluator(kValue);
    }

    return pkBSplineEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiAnimationCompression::CompressFloatEvaluator(
    NiEvaluator* pkEvaluator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    float fValue;
    float fStart;
    float fEnd;
    pkEvaluator->GetActiveTimeRange(fStart, fEnd);

    // Create a temporary scratch pad for the evaluator.
    NiScratchPadPtr spScratchPad = 
        (NiScratchPad*)NiNew NiScratchPad(pkEvaluator);

    // Obtain the scratch pad entry for the evaluator channel.
    unsigned int uiNumEvalSPDatas = 
        spScratchPad->GetNumBlockItems(SPBEVALUATORSPDATA);
    if (uiNumEvalSPDatas == 0)
    {
        return NULL;
    }
    NIASSERT(uiNumEvalSPDatas == 1);
    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)spScratchPad->GetDataBlock(SPBEVALUATORSPDATA);
    NIASSERT(pkEvalSPData);

    if (!pkEvaluator->UpdateChannel(fStart, 0, pkEvalSPData, &fValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplineFloatEvaluator* pkBSplineEval = NULL;
    if (bCompactControlPoints)
        pkBSplineEval = NiNew NiBSplineCompFloatEvaluator();
    else
        pkBSplineEval = NiNew NiBSplineFloatEvaluator();

    float fNyquistTimeStep = fTimeStep;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    float* pafFloat = NiAlloc(float, uiNumSampledKeys);

    // Sample the evaluator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkEvaluator->UpdateChannel(fCurrentTime, 0, 
            pkEvalSPData, &fValue));

        pafFloat[ui] = fValue;

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kFloatHandle = NiBSplineData::INVALID_HANDLE;

    // Determine if the evaluator is posed
    bool bAllSame = true;
    for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
    {
        if (pafFloat[ui] != pafFloat[ui - 1])
        {
            bAllSame = false;
            break;
        }
    }
    
    float fCompactScalars[2];
    for (unsigned int ui = 0; ui < 2; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (!bAllSame)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<1,3> kFit(uiNumSampledKeys, 
            pafFloat, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineEval, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kFloatHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kFloatHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1);
        }
    }

    pkBSplineEval->SetTimeRange(fStart, fEnd);
    pkBSplineEval->SetData(pkBSplineData, spBasisData);
    pkBSplineEval->SetControlHandle(kFloatHandle, 
        NiBSplineFloatEvaluator::FLOAT);

    if (bCompactControlPoints)
    {
        NiBSplineCompFloatEvaluator* pkCompEval = 
            (NiBSplineCompFloatEvaluator*) pkBSplineEval;
        
        pkCompEval->SetOffset(fCompactScalars[0], 
            NiBSplineFloatEvaluator::FLOAT);
        pkCompEval->SetHalfRange(fCompactScalars[1], 
            NiBSplineFloatEvaluator::FLOAT);
    }

    // delete the temp arrays and scratch pad
    NiFree(pafTimes);
    NiFree(pafFloat);
    spScratchPad = NULL;

    if (bAllSame)
    {
        // Create a const float evaluator in place of the b-spline evaluator.
        NiDelete pkBSplineEval;
        return NiNew NiConstFloatEvaluator(fValue);
    }

    return pkBSplineEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiAnimationCompression::CompressPoint3Evaluator(
    NiEvaluator* pkEvaluator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    NiPoint3 kValue;
    float fStart;
    float fEnd;
    pkEvaluator->GetActiveTimeRange(fStart, fEnd);

    // Create a temporary scratch pad for the evaluator.
    NiScratchPadPtr spScratchPad = 
        (NiScratchPad*)NiNew NiScratchPad(pkEvaluator);

    // Obtain the scratch pad entry for the evaluator channel.
    unsigned int uiNumEvalSPDatas = 
        spScratchPad->GetNumBlockItems(SPBEVALUATORSPDATA);
    if (uiNumEvalSPDatas == 0)
    {
        return NULL;
    }
    NIASSERT(uiNumEvalSPDatas == 1);
    NiEvaluatorSPData* pkEvalSPData = 
        (NiEvaluatorSPData*)spScratchPad->GetDataBlock(SPBEVALUATORSPDATA);
    NIASSERT(pkEvalSPData);

    if (!pkEvaluator->UpdateChannel(fStart, 0, pkEvalSPData, &kValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplinePoint3Evaluator* pkBSplineEval = NULL;
    if (bCompactControlPoints)
        pkBSplineEval = NiNew NiBSplineCompPoint3Evaluator();
    else
        pkBSplineEval = NiNew NiBSplinePoint3Evaluator();

    float fNyquistTimeStep = fTimeStep ;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    NiPoint3* pakPoint3 = NiNew NiPoint3[uiNumSampledKeys];

    // Sample the evaluator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkEvaluator->UpdateChannel(fCurrentTime, 0, 
            pkEvalSPData, &kValue));

        pakPoint3[ui] = kValue;

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kPoint3Handle = NiBSplineData::INVALID_HANDLE;

    // Determine if the evaluator is posed
    bool bAllSame = true;
    for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
    {
        if (pakPoint3[ui] != pakPoint3[ui - 1])
        {
            bAllSame = false;
            break;
        }
    }

    float fCompactScalars[2];
    for (unsigned int ui = 0; ui < 2; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (!bAllSame)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<3,3> kFit(uiNumSampledKeys, 
            (const float*) pakPoint3, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineEval, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kPoint3Handle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kPoint3Handle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3);
        }
    }

    pkBSplineEval->SetTimeRange(fStart, fEnd);
    pkBSplineEval->SetData(pkBSplineData, spBasisData);
    pkBSplineEval->SetControlHandle(kPoint3Handle, 
        NiBSplinePoint3Evaluator::POINT3);

    if (bCompactControlPoints)
    {
        NiBSplineCompPoint3Evaluator* pkCompEval = 
            (NiBSplineCompPoint3Evaluator*) pkBSplineEval;
        
        pkCompEval->SetOffset(fCompactScalars[0], 
            NiBSplinePoint3Evaluator::POINT3);
        pkCompEval->SetHalfRange(fCompactScalars[1], 
            NiBSplinePoint3Evaluator::POINT3);
    }

    // delete the temp arrays and scratch pad
    NiFree(pafTimes);
    NiDelete [] pakPoint3;
    spScratchPad = NULL;

    if (bAllSame)
    {
        // Create a const point3 evaluator in place of the b-spline evaluator.
        NiDelete pkBSplineEval;
        return NiNew NiConstPoint3Evaluator(kValue);
    }

    return pkBSplineEval;
}
//---------------------------------------------------------------------------
unsigned int NiAnimationCompression::EstimateCompressedSize(
    NiKeyBasedEvaluator* pkEvaluator, unsigned int uiChannel, 
    bool& bCannotCompress, float fSampleTimeStep, float fCompressionRatio,
    bool bCompactControlPoints)
{
    bCannotCompress = false;
    unsigned int uiNumKeys = pkEvaluator->GetKeyCount((unsigned short)uiChannel);
    if (uiNumKeys != 0)
    {
        NiAnimationKey::KeyType eType = 
            pkEvaluator->GetKeyType((unsigned short)uiChannel);

        NiAnimationKey::KeyContent eContent = 
            pkEvaluator->GetKeyContent((unsigned short)uiChannel);

        float fBegin;
        float fEnd;

        pkEvaluator->GetActiveTimeRange(fBegin, fEnd);
        // The number of compressed keys are half that of the number of 
        // sampled keys
        unsigned int uiNumSampledKeys = (unsigned int)
            (((fEnd - fBegin) / (2.0f * fSampleTimeStep)) * fCompressionRatio);

        if (!uiNumSampledKeys)
        {
            // If there aren't going to be any keys, then we
            // cannot compress.
            bCannotCompress = true;
            return 0;
        }

        if (bCompactControlPoints)
        {
            switch (eType)
            {
                case NiAnimationKey::LINKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::BEZKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::TCBKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::EULERKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                default:
                    bCannotCompress = true;
                    break;

            }
        }
        else
        {
            switch (eType)
            {
                case NiAnimationKey::LINKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::BEZKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::TCBKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::EULERKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                default:
                    bCannotCompress = true;
                    break;

            }
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NiAnimationCompression::IgnoreEvaluator(NiEvaluator* pkEvaluator)
{
    if (pkEvaluator->GetMaxEvalChannels() == 1)
    {
        if (pkEvaluator->GetRawEvalPBChannelType(0) == PBROTCHANNEL)
        {
            return true;
        }

        if (pkEvaluator->GetRawEvalPBChannelType(0) == PBBOOLCHANNEL)
        {
            return true;
        }
    }

    if (NiIsKindOf(NiBSplineEvaluator, pkEvaluator))
    {
        return true;
    }
    
    if (NiIsKindOf(NiLookAtEvaluator, pkEvaluator))
    {
        return true;
    }

    const char* pcCtlrType = pkEvaluator->GetIDTag().GetCtlrType();
    if (pcCtlrType && NiStricmp(pcCtlrType, 
        NiAnimationConstants::GetPSEmitParticlesCtlrType()) == 0)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBSplineBasisData* NiAnimationCompression::InsertBasis(
    NiBSplineEvaluator*, NiBasisDataList& kBasisList, 
    const NiBSplineBasis<float, 3>* pkBasis)
{
    NiTListIterator kIter = kBasisList.GetHeadPos();

    while (kIter != NULL)
    {
        NiBSplineBasisData* pkPrevData = kBasisList.GetNext(kIter);
        NiBSplineBasis<float, 3>& kPrevBasis = pkPrevData->GetDegree3Basis();
        if (kPrevBasis.IsEqual(pkBasis))
        {
            return pkPrevData;
        }
    }

    NiBSplineBasisData* pkBasisData = NiNew NiBSplineBasisData();
    pkBasisData->SetBasis(pkBasis);
    kBasisList.AddTail(pkBasisData);
    return pkBasisData;

}
//---------------------------------------------------------------------------
NiInterpolator* NiAnimationCompression::CompressInterpolator(
    NiInterpolator* pkInterp,
    float fCompressionRatio, bool bAlwaysCompress, 
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList, float fTimeStep)
{
    if (!bAlwaysCompress)
    {
        if (NiIsKindOf(NiKeyBasedInterpolator, pkInterp))
        {
            NiKeyBasedInterpolator* pkKeyInterp = (NiKeyBasedInterpolator*)
                pkInterp;

            unsigned int uiCurrentTotal = 0;
            unsigned int uiCompTotal = 0;
            for (unsigned int ui = 0; ui < pkKeyInterp->GetKeyChannelCount();
                ui++)
            {
                bool bCannotCompress = false;
                if (!pkKeyInterp->GetChannelPosed((unsigned short)ui))
                {
                    uiCurrentTotal += pkKeyInterp->GetAllocatedSize(
                        (unsigned short)ui);
                   
                    uiCompTotal += EstimateCompressedSize(pkKeyInterp, ui,
                        bCannotCompress, fTimeStep, fCompressionRatio,
                        bCompactControlPoints);
                    
                    if (bCannotCompress)
                        return NULL;

                    NIASSERT((uiCurrentTotal == 0) || (uiCompTotal != 0));
                }
            }

            if (uiCompTotal >= uiCurrentTotal)
                return NULL;
        }
    }

    NiInterpolator* pkNewInterp = NULL;

    if (pkInterp->IsColorAValueSupported())
    {
        pkNewInterp = CompressColorInterpolator(pkInterp, fCompressionRatio,
            fTimeStep, pkBSplineData, bCompactControlPoints, kBasisList);
    }
    else if (pkInterp->IsFloatValueSupported())
    {
        pkNewInterp = CompressFloatInterpolator(pkInterp, fCompressionRatio,
            fTimeStep, pkBSplineData, bCompactControlPoints, kBasisList);
    }
    else if (pkInterp->IsPoint3ValueSupported())
    {
        pkNewInterp = CompressPoint3Interpolator(pkInterp, fCompressionRatio,
            fTimeStep, pkBSplineData, bCompactControlPoints, kBasisList);
    }
    else if (pkInterp->IsTransformValueSupported())
    {
        pkNewInterp = CompressTransformInterpolator(pkInterp, 
            fCompressionRatio, fTimeStep, pkBSplineData, 
            bCompactControlPoints, kBasisList);
    }

    return pkNewInterp;
}
//---------------------------------------------------------------------------
NiInterpolator* NiAnimationCompression::CompressTransformInterpolator(
    NiInterpolator* pkInterpolator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    NiQuatTransform kValue;
    float fStart;
    float fEnd;
    pkInterpolator->GetActiveTimeRange(fStart, fEnd);

    if (!pkInterpolator->Update(fStart + fEnd / 2.0f, NULL, kValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplineTransformInterpolator* pkBSplineInterp = NULL;
    
    if (bCompactControlPoints)
        pkBSplineInterp = NiNew NiBSplineCompTransformInterpolator();
    else
        pkBSplineInterp = NiNew NiBSplineTransformInterpolator();

    bool bTranslate = kValue.IsTranslateValid();
    bool bRotate = kValue.IsRotateValid();
    bool bScale = kValue.IsScaleValid();

    if (!bTranslate && !bRotate && !bScale)
    {
        NiDelete pkBSplineInterp;
        return NULL;
    
    }
    float fNyquistTimeStep = fTimeStep;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    NiPoint3* pakTranslate = NULL;
    if (bTranslate)
        pakTranslate = NiNew NiPoint3[uiNumSampledKeys];
    
    NiQuaternion* pakRotate = NULL;
    if (bRotate)
        pakRotate = NiNew NiQuaternion[uiNumSampledKeys];
    
    float* pafScale = NULL;
    if (bScale)
        pafScale = NiAlloc(float, uiNumSampledKeys);

    // Sample the interpolator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        NIASSERT(fCurrentTime <= fEnd);
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkInterpolator->Update(fCurrentTime, NULL, kValue));

        if (bTranslate)
            pakTranslate[ui] = kValue.GetTranslate();

        if (bRotate)
            pakRotate[ui] = kValue.GetRotate();
        
        if (bScale)
            pafScale[ui] = kValue.GetScale();

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Determine if the interpolator is posed in translate
    if (bTranslate)
    {
        bool bAllSame = true;
        for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
        {
            if (pakTranslate[ui] != pakTranslate[ui - 1])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
        {
            bTranslate = false;
            pkBSplineInterp->SetPoseTranslate(kValue.GetTranslate());
            NiDelete [] pakTranslate;
            pakTranslate = NULL;
        }
    }

    // Determine if the interpolator is posed in rotate
    if (bRotate)
    {
        bool bAllSame = true;
        for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
        {
            if (pakRotate[ui] != pakRotate[ui - 1])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
        {
            bRotate = false;
            pkBSplineInterp->SetPoseRotate(kValue.GetRotate());
            NiDelete [] pakRotate;
            pakRotate = NULL;
        }
    }

    // Determine if the interpolator is posed in scale
    if (bScale)
    {
        bool bAllSame = true;
        for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
        {
            if (pafScale[ui] != pafScale[ui - 1])
            {
                bAllSame = false;
                break;
            }
        }

        if (bAllSame)
        {
            bScale = false;
            pkBSplineInterp->SetPoseScale(kValue.GetScale());
            NiFree(pafScale);
            pafScale = NULL;
        }
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kTranslateHandle = NiBSplineData::INVALID_HANDLE;
    NiBSplineData::Handle kRotateHandle = NiBSplineData::INVALID_HANDLE;
    NiBSplineData::Handle kScaleHandle = NiBSplineData::INVALID_HANDLE;

    float fCompactScalars[6];
    for (unsigned int ui = 0; ui < 6; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (bTranslate)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<3,3> kFit(uiNumSampledKeys, 
            (const float*) pakTranslate, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineInterp, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kTranslateHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kTranslateHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3);
        }

    }

    if (bRotate)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<4,3> kFit(uiNumSampledKeys, 
            (const float*) pakRotate, uiNumCompKeys);
        
        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineInterp, kBasisList, 
                kFit.GetBasis());
        }


        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kRotateHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4,
                fCompactScalars[2], fCompactScalars[3]);
        }
        else
        {
            kRotateHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4);
        }
    }

    if (bScale)
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<1,3> kFit(uiNumSampledKeys, 
            (const float*) pafScale, uiNumCompKeys);
        
        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineInterp, kBasisList, 
                kFit.GetBasis());
        }


        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kScaleHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1,
                fCompactScalars[4], fCompactScalars[5]);
        }
        else
        {
            kScaleHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1);
        }
    }

    pkBSplineInterp->SetTimeRange(fStart, fEnd);
    pkBSplineInterp->SetData(pkBSplineData, spBasisData);
    pkBSplineInterp->SetControlHandle(kTranslateHandle, 
        NiBSplineTransformInterpolator::POSITION);
    pkBSplineInterp->SetControlHandle(kRotateHandle, 
        NiBSplineTransformInterpolator::ROTATION);
    pkBSplineInterp->SetControlHandle(kScaleHandle, 
        NiBSplineTransformInterpolator::SCALE);

    if (bCompactControlPoints)
    {
        NiBSplineCompTransformInterpolator* pkCompInterp = 
            (NiBSplineCompTransformInterpolator*) pkBSplineInterp;
        
        pkCompInterp->SetOffset(fCompactScalars[0], 
            NiBSplineTransformInterpolator::POSITION);
        pkCompInterp->SetHalfRange(fCompactScalars[1], 
            NiBSplineTransformInterpolator::POSITION);
        pkCompInterp->SetOffset(fCompactScalars[2], 
            NiBSplineTransformInterpolator::ROTATION);
        pkCompInterp->SetHalfRange(fCompactScalars[3],
            NiBSplineTransformInterpolator::ROTATION);
        pkCompInterp->SetOffset(fCompactScalars[4],
            NiBSplineTransformInterpolator::SCALE);       
        pkCompInterp->SetHalfRange(fCompactScalars[5],
            NiBSplineTransformInterpolator::SCALE);

    }

    // delete the temp arrays
    NiFree(pafTimes);
    NiDelete [] pakTranslate;
    NiDelete [] pakRotate;
    NiFree(pafScale);

    return pkBSplineInterp;
}
//---------------------------------------------------------------------------
NiInterpolator* NiAnimationCompression::CompressColorInterpolator(
    NiInterpolator* pkInterpolator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    NiColorA kValue;
    float fStart;
    float fEnd;
    pkInterpolator->GetActiveTimeRange(fStart, fEnd);

    if (!pkInterpolator->Update(fStart, NULL, kValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplineColorInterpolator* pkBSplineInterp = NULL;
    if (bCompactControlPoints)
        pkBSplineInterp = NiNew NiBSplineCompColorInterpolator();
    else
        pkBSplineInterp = NiNew NiBSplineColorInterpolator();

    float fNyquistTimeStep = fTimeStep;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    NiColorA* pakColor = NiNew NiColorA[uiNumSampledKeys];

    // Sample the interpolator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkInterpolator->Update(fCurrentTime, NULL, kValue));        

        pakColor[ui] = kValue;

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kColorAHandle = NiBSplineData::INVALID_HANDLE;

    // Determine if the interpolator is posed in translate
    bool bAllSame = true;
    for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
    {
        if (pakColor[ui] != pakColor[ui - 1])
        {
            bAllSame = false;
            break;
        }
    }

    float fCompactScalars[2];
    for (unsigned int ui = 0; ui < 2; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (bAllSame)
    {
        pkBSplineInterp->SetPoseValue(kValue);
    }
    else
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<4,3> kFit(uiNumSampledKeys, 
            (const float*) pakColor, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineInterp, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kColorAHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kColorAHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 4);
        }
    }

    pkBSplineInterp->SetTimeRange(fStart, fEnd);
    pkBSplineInterp->SetData(pkBSplineData, spBasisData);
    pkBSplineInterp->SetControlHandle(kColorAHandle, 
        NiBSplineColorInterpolator::COLORA);

    if (bCompactControlPoints)
    {
        NiBSplineCompColorInterpolator* pkCompInterp = 
            (NiBSplineCompColorInterpolator*) pkBSplineInterp;
        
        pkCompInterp->SetOffset(fCompactScalars[0], 
            NiBSplineColorInterpolator::COLORA);
        pkCompInterp->SetHalfRange(fCompactScalars[1], 
            NiBSplineColorInterpolator::COLORA);
    }

    // delete the temp arrays
    NiFree(pafTimes);
    NiDelete [] pakColor;

    return pkBSplineInterp;
}
//---------------------------------------------------------------------------
NiInterpolator* NiAnimationCompression::CompressFloatInterpolator(
    NiInterpolator* pkInterpolator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    float fValue;
    float fStart;
    float fEnd;
    pkInterpolator->GetActiveTimeRange(fStart, fEnd);

    if (!pkInterpolator->Update(fStart, NULL, fValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplineFloatInterpolator* pkBSplineInterp = NULL;
    if (bCompactControlPoints)
        pkBSplineInterp = NiNew NiBSplineCompFloatInterpolator();
    else
        pkBSplineInterp = NiNew NiBSplineFloatInterpolator();

    float fNyquistTimeStep = fTimeStep;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    float* pafFloat = NiAlloc(float, uiNumSampledKeys);

    // Sample the interpolator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkInterpolator->Update(fCurrentTime, NULL, fValue));

        pafFloat[ui] = fValue;

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kFloatHandle = NiBSplineData::INVALID_HANDLE;

    // Determine if the interpolator is posed in translate
    bool bAllSame = true;
    for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
    {
        if (pafFloat[ui] != pafFloat[ui - 1])
        {
            bAllSame = false;
            break;
        }
    }
    
    float fCompactScalars[2];
    for (unsigned int ui = 0; ui < 2; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (bAllSame)
    {
        pkBSplineInterp->SetPoseValue(fValue);
    }
    else
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<1,3> kFit(uiNumSampledKeys, 
            pafFloat, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineInterp, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kFloatHandle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kFloatHandle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 1);
        }
    }

    pkBSplineInterp->SetTimeRange(fStart, fEnd);
    pkBSplineInterp->SetData(pkBSplineData, spBasisData);
    pkBSplineInterp->SetControlHandle(kFloatHandle, 
        NiBSplineFloatInterpolator::FLOAT);

    if (bCompactControlPoints)
    {
        NiBSplineCompFloatInterpolator* pkCompInterp = 
            (NiBSplineCompFloatInterpolator*) pkBSplineInterp;
        
        pkCompInterp->SetOffset(fCompactScalars[0], 
            NiBSplineFloatInterpolator::FLOAT);
        pkCompInterp->SetHalfRange(fCompactScalars[1], 
            NiBSplineFloatInterpolator::FLOAT);
    }

    // delete the temp arrays
    NiFree(pafTimes);
    NiFree(pafFloat);

    return pkBSplineInterp;
}
//---------------------------------------------------------------------------
NiInterpolator* NiAnimationCompression::CompressPoint3Interpolator(
    NiInterpolator* pkInterpolator, float fCompressionRatio, float fTimeStep,
    NiBSplineData* pkBSplineData, bool bCompactControlPoints,
    NiBasisDataList& kBasisList)
{
    NiPoint3 kValue;
    float fStart;
    float fEnd;
    pkInterpolator->GetActiveTimeRange(fStart, fEnd);

    if (!pkInterpolator->Update(fStart, NULL, kValue))
    {
        return NULL;
    }

    // Note that we aren't holding a smart pointer here.
    // Since this method doesn't return a smart pointer,
    // it would be deleted as soon as this method left
    // scope. If we aren't going to use it, make sure that we 
    // delete it manually.
    NiBSplinePoint3Interpolator* pkBSplineInterp = NULL;
    if (bCompactControlPoints)
        pkBSplineInterp = NiNew NiBSplineCompPoint3Interpolator();
    else
        pkBSplineInterp = NiNew NiBSplinePoint3Interpolator();

    float fNyquistTimeStep = fTimeStep ;

    unsigned int uiNumSampledKeys = (unsigned int)
        (((fEnd - fStart) / fNyquistTimeStep) + 0.5f) + 1;

    float* pafTimes = NiAlloc(float, uiNumSampledKeys);
    NiPoint3* pakPoint3 = NiNew NiPoint3[uiNumSampledKeys];

    // Sample the interpolator at a regular interval.
    float fCurrentTime = fStart;
    for (unsigned int ui = 0; ui < uiNumSampledKeys; ui++)
    {
        pafTimes[ui] = fCurrentTime;

        NIVERIFY(pkInterpolator->Update(fCurrentTime, NULL, kValue));

        pakPoint3[ui] = kValue;

        fCurrentTime += fNyquistTimeStep;
        fCurrentTime = NiMin(fCurrentTime, fEnd);
    }

    // Perform the BSpline fit for each channel
    // We will use half the number of compressed keys as sampled keys
    unsigned int uiNumCompKeys = (unsigned int)
        ((((float)uiNumSampledKeys / 2.0f))*fCompressionRatio + 0.5f);
    
    NiBSplineBasisDataPtr spBasisData;
    NiBSplineData::Handle kPoint3Handle = NiBSplineData::INVALID_HANDLE;

    // Determine if the interpolator is posed in translate
    bool bAllSame = true;
    for (unsigned int ui = 1; ui < uiNumSampledKeys; ui++)
    {
        if (pakPoint3[ui] != pakPoint3[ui - 1])
        {
            bAllSame = false;
            break;
        }
    }

    float fCompactScalars[2];
    for (unsigned int ui = 0; ui < 2; ui++)
        fCompactScalars[ui] = NI_INFINITY;

    if (bAllSame)
    {
        pkBSplineInterp->SetPoseValue(kValue);
    }
    else
    {
        uiNumCompKeys = NiMax(uiNumCompKeys, 4);
        NiBSplineFit<3,3> kFit(uiNumSampledKeys, 
            (const float*) pakPoint3, uiNumCompKeys);

        if (!spBasisData)
        {
            spBasisData = InsertBasis(pkBSplineInterp, kBasisList, 
                kFit.GetBasis());
        }

        // We get the basis quantity here due to the potential for replication
        // within the spline and we want ALL of the control points
        if (bCompactControlPoints)
        {
            kPoint3Handle = pkBSplineData->InsertAndCompactControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3,
                fCompactScalars[0], fCompactScalars[1]);
        }
        else
        {
            kPoint3Handle = pkBSplineData->InsertControlPoints(
                kFit.GetOutControl(),
                kFit.GetBasis()->GetQuantity(), 3);
        }
    }

    pkBSplineInterp->SetTimeRange(fStart, fEnd);
    pkBSplineInterp->SetData(pkBSplineData, spBasisData);
    pkBSplineInterp->SetControlHandle(kPoint3Handle, 
        NiBSplinePoint3Interpolator::POINT3);

    if (bCompactControlPoints)
    {
        NiBSplineCompPoint3Interpolator* pkCompInterp = 
            (NiBSplineCompPoint3Interpolator*) pkBSplineInterp;
        
        pkCompInterp->SetOffset(fCompactScalars[0], 
            NiBSplinePoint3Interpolator::POINT3);
        pkCompInterp->SetHalfRange(fCompactScalars[1], 
            NiBSplinePoint3Interpolator::POINT3);
    }

    // delete the temp arrays
    NiFree(pafTimes);
    NiDelete [] pakPoint3;

    return pkBSplineInterp;
}
//---------------------------------------------------------------------------
unsigned int NiAnimationCompression::EstimateCompressedSize(
    NiKeyBasedInterpolator* pkInterpolator, unsigned int uiChannel, 
    bool& bCannotCompress, float fSampleTimeStep, float fCompressionRatio,
    bool bCompactControlPoints)
{
    bCannotCompress = false;
    unsigned int uiNumKeys = pkInterpolator->GetKeyCount(
        (unsigned short)uiChannel);
    if (uiNumKeys != 0)
    {
        NiAnimationKey::KeyType eType = 
            pkInterpolator->GetKeyType((unsigned short)uiChannel);

        NiAnimationKey::KeyContent eContent = 
            pkInterpolator->GetKeyContent((unsigned short)uiChannel);

        float fBegin;
        float fEnd;

        pkInterpolator->GetActiveTimeRange(fBegin, fEnd);
        // The number of compressed keys are half that of the number of 
        // sampled keys
        unsigned int uiNumSampledKeys = (unsigned int)
            (((fEnd - fBegin) / (2.0f * fSampleTimeStep)) * fCompressionRatio);

        if (!uiNumSampledKeys)
        {
            // If there aren't going to be any keys, then we
            // cannot compress.
            bCannotCompress = true;
            return 0;
        }

        if (bCompactControlPoints)
        {
            switch (eType)
            {
                case NiAnimationKey::LINKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::BEZKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::TCBKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::EULERKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * COMP_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * COMP_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * COMP_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * COMP_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * COMP_LIN_COLOR_SIZE;
                    break;
                default:
                    bCannotCompress = true;
                    break;

            }
        }
        else
        {
            switch (eType)
            {
                case NiAnimationKey::LINKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::BEZKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::TCBKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                case NiAnimationKey::EULERKEY:
                    if (eContent == NiAnimationKey::POSKEY)
                        return uiNumSampledKeys * SPLINE_LIN_POS_SIZE;
                    else if (eContent == NiAnimationKey::ROTKEY)
                        return uiNumSampledKeys * SPLINE_LIN_ROT_SIZE;
                    else if (eContent == NiAnimationKey::FLOATKEY)
                        return uiNumSampledKeys * SPLINE_LIN_FLOAT_SIZE;
                    else if (eContent == NiAnimationKey::BOOLKEY)
                        return uiNumSampledKeys * SPLINE_LIN_BOOL_SIZE;
                    else if (eContent == NiAnimationKey::COLORKEY)
                        return uiNumSampledKeys * SPLINE_LIN_COLOR_SIZE;
                    break;
                default:
                    bCannotCompress = true;
                    break;

            }
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NiAnimationCompression::IgnoreInterpolator(
    NiInterpolator* pkInterp,
    const char*,
    const char*,
    const char* pcCtlrType,
    const char*,
    const char*)
{
    if (pkInterp->IsBoolValueSupported())
    {
        return true;
    }
    else if (pkInterp->IsQuaternionValueSupported())
    {
        return true;
    }
    else if (NiIsKindOf(NiLookAtInterpolator, pkInterp))
    {
        return true;
    }
    else if (NiIsKindOf(NiBSplineInterpolator, pkInterp))
    {
        return true;
    }

    if (NiStricmp(pcCtlrType, 
        NiAnimationConstants::GetPSEmitParticlesCtlrType()) == 0)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
NiBSplineBasisData* NiAnimationCompression::InsertBasis(
    NiBSplineInterpolator*, NiBasisDataList& kBasisList, 
    const NiBSplineBasis<float, 3>* pkBasis)
{
    NiTListIterator kIter = kBasisList.GetHeadPos();

    while (kIter != NULL)
    {
        NiBSplineBasisData* pkPrevData = kBasisList.GetNext(kIter);
        NiBSplineBasis<float, 3>& kPrevBasis = pkPrevData->GetDegree3Basis();
        if (kPrevBasis.IsEqual(pkBasis))
        {
            return pkPrevData;
        }
    }

    NiBSplineBasisData* pkBasisData = NiNew NiBSplineBasisData();
    pkBasisData->SetBasis(pkBasis);
    kBasisList.AddTail(pkBasisData);
    return pkBasisData;

}
//---------------------------------------------------------------------------
