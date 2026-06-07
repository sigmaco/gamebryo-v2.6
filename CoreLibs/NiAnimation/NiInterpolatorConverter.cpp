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

#include "NiInterpolatorConverter.h"
#include "NiAnimationConstants.h"

#include "NiBezPosKey.h"
#include "NiBezRotKey.h"
#include "NiTCBFloatKey.h"
#include "NiTCBPosKey.h"
#include "NiTCBRotKey.h"

#include "NiColorInterpolator.h"
#include "NiBSplineColorInterpolator.h"
#include "NiBSplineCompColorInterpolator.h"

#include "NiBoolInterpolator.h"
#include "NiBoolTimelineInterpolator.h"

#include "NiFloatInterpolator.h"
#include "NiBSplineFloatInterpolator.h"
#include "NiBSplineCompFloatInterpolator.h"

#include "NiPoint3Interpolator.h"
#include "NiBSplinePoint3Interpolator.h"
#include "NiBSplineCompPoint3Interpolator.h"

#include "NiQuaternionInterpolator.h"

#include "NiTransformInterpolator.h"
#include "NiBSplineTransformInterpolator.h"
#include "NiBSplineCompTransformInterpolator.h"

#include "NiLookAtInterpolator.h"
#include "NiPathInterpolator.h"

#include "NiConstColorEvaluator.h"
#include "NiColorEvaluator.h"
#include "NiBSplineColorEvaluator.h"
#include "NiBSplineCompColorEvaluator.h"

#include "NiConstBoolEvaluator.h"
#include "NiBoolEvaluator.h"
#include "NiBoolTimelineEvaluator.h"

#include "NiConstFloatEvaluator.h"
#include "NiFloatEvaluator.h"
#include "NiBSplineFloatEvaluator.h"
#include "NiBSplineCompFloatEvaluator.h"

#include "NiConstPoint3Evaluator.h"
#include "NiPoint3Evaluator.h"
#include "NiBSplinePoint3Evaluator.h"
#include "NiBSplineCompPoint3Evaluator.h"

#include "NiConstQuaternionEvaluator.h"
#include "NiQuaternionEvaluator.h"

#include "NiConstTransformEvaluator.h"
#include "NiTransformEvaluator.h"
#include "NiBSplineTransformEvaluator.h"
#include "NiBSplineCompTransformEvaluator.h"

#include "NiLookAtEvaluator.h"
#include "NiPathEvaluator.h"

//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::Convert(
    NiInterpolator* pkInterpolator, const NiEvaluator::IDTag& kIDTag,
    bool bCollapseInterpolator)
{
    NIASSERT(pkInterpolator);

    if (bCollapseInterpolator)
    {
        pkInterpolator->Collapse();
    }

    NiEvaluator* pkEvaluator = NULL;
    if (pkInterpolator->IsTransformValueSupported())
    {
        pkEvaluator = ConvertTransformInterpolator(pkInterpolator, 
            kIDTag.GetAVObjectName(), bCollapseInterpolator);
    }
    else
    {
        // Not a transform interpolator.
        if (pkInterpolator->IsQuaternionValueSupported())
        {
            pkEvaluator = ConvertQuaternionInterpolator(pkInterpolator);
        }
        else if (pkInterpolator->IsPoint3ValueSupported())
        {
            pkEvaluator = ConvertPoint3Interpolator(pkInterpolator);
        }
        else if (pkInterpolator->IsFloatValueSupported())
        {
            pkEvaluator = ConvertFloatInterpolator(pkInterpolator);
        }
        else if (pkInterpolator->IsBoolValueSupported())
        {
            pkEvaluator = ConvertBoolInterpolator(pkInterpolator);
        }
        else
        {
            NIASSERT(pkInterpolator->IsColorAValueSupported());
            pkEvaluator = ConvertColorInterpolator(pkInterpolator);
        }
    }

    if (!pkEvaluator)
    {
        return NULL;
    }

    // Mark any referenced evaluators.
    NiFixedString kCtlrType;
    if (kIDTag.GetCtlrType().Exists())
    {
        // Convert old controller names to new names.
        kCtlrType = ConvertCtlrType(kIDTag.GetCtlrType());
        if (kCtlrType == NiAnimationConstants::GetFlipCtlrType())
        {
            pkEvaluator->SetReferencedEvaluator();
        }
        else if (kCtlrType == 
            NiAnimationConstants::GetPSEmitParticlesCtlrType())
        {
            if (kIDTag.GetEvaluatorID() == 
                NiAnimationConstants::GetEmitterActiveInterpID() || 
                kIDTag.GetEvaluatorID() == 
                NiAnimationConstants::GetBirthRateInterpID())
            {
                pkEvaluator->SetReferencedEvaluator();
            }
        }
    }

    // Set the evaluator's tag.
    pkEvaluator->SetIDTag(kIDTag.GetAVObjectName(),
        kIDTag.GetPropertyType(), kCtlrType,
        kIDTag.GetCtlrID(), kIDTag.GetEvaluatorID());

    return pkEvaluator;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertColorInterpolator(
    NiInterpolator* pkInterpolator)
{
    NIASSERT(pkInterpolator->IsColorAValueSupported());

    NiBSplineCompColorInterpolator* pkBSCCInterp = 
        NiDynamicCast(NiBSplineCompColorInterpolator, pkInterpolator);
    if (pkBSCCInterp)
    {
        if (pkBSCCInterp->GetChannelPosed(0))
        {
            NiColorA kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstColorEvaluator(kPosedValue);
        }

        NiBSplineCompColorEvaluator* pkBSCCEval = 
            NiNew NiBSplineCompColorEvaluator();
        pkBSCCEval->SetData(pkBSCCInterp->GetData(), 
            pkBSCCInterp->GetBasisData());
        float fStartTime;
        float fEndTime;
        pkBSCCInterp->GetActiveTimeRange(fStartTime, fEndTime);
        pkBSCCEval->SetTimeRange(fStartTime, fEndTime);
        pkBSCCEval->SetControlHandle(pkBSCCInterp->GetControlHandle(0), 0);
        pkBSCCEval->SetOffset(pkBSCCInterp->GetOffset(0), 0);
        pkBSCCEval->SetHalfRange(pkBSCCInterp->GetHalfRange(0), 0);

        return pkBSCCEval;
    }

    NiBSplineColorInterpolator* pkBSCInterp = 
        NiDynamicCast(NiBSplineColorInterpolator, pkInterpolator);
    if (pkBSCInterp)
    {
        if (pkBSCInterp->GetChannelPosed(0))
        {
            NiColorA kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstColorEvaluator(kPosedValue);
        }

        NiBSplineColorEvaluator* pkBSCEval = NiNew NiBSplineColorEvaluator();
        pkBSCEval->SetData(pkBSCInterp->GetData(), 
            pkBSCInterp->GetBasisData());
        float fStartTime;
        float fEndTime;
        pkBSCInterp->GetActiveTimeRange(fStartTime, fEndTime);
        pkBSCEval->SetTimeRange(fStartTime, fEndTime);
        pkBSCEval->SetControlHandle(pkBSCInterp->GetControlHandle(0), 0);

        return pkBSCEval;
    }

    NiColorInterpolator* pkCInterp = 
        NiDynamicCast(NiColorInterpolator, pkInterpolator);
    if (pkCInterp)
    {
        if (pkCInterp->GetChannelPosed(0))
        {
            NiColorA kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstColorEvaluator(kPosedValue);
        }

        NiColorData* pkData = pkCInterp->GetColorData();
        if (pkData)
        {
            return NiNew NiColorEvaluator(pkData);
        }
    }

    // Unsupported color interpolator type.
    NIASSERT(false);

    return NULL;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertBoolInterpolator(
    NiInterpolator* pkInterpolator)
{
    NIASSERT(pkInterpolator->IsBoolValueSupported());

    NiBoolTimelineInterpolator* pkBTInterp = 
        NiDynamicCast(NiBoolTimelineInterpolator, pkInterpolator);
    if (pkBTInterp)
    {
        if (pkBTInterp->GetChannelPosed(0))
        {
            bool bValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, bValue));
            float fValue = bValue ? 1.0f : 0.0f;
            return NiNew NiConstBoolEvaluator(fValue);
        }

        NiBoolData* pkData = pkBTInterp->GetBoolData();
        if (pkData)
        {
            return NiNew NiBoolTimelineEvaluator(pkData);
        }
    }

    NiBoolInterpolator* pkBInterp = 
        NiDynamicCast(NiBoolInterpolator, pkInterpolator);
    if (pkBInterp)
    {
        if (pkBInterp->GetChannelPosed(0))
        {
            bool bValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, bValue));
            float fValue = bValue ? 1.0f : 0.0f;
            return NiNew NiConstBoolEvaluator(fValue);
        }

        NiBoolData* pkData = pkBInterp->GetBoolData();
        if (pkData)
        {
            return NiNew NiBoolEvaluator(pkData);
        }
    }

    // Unsupported bool interpolator type.
    NIASSERT(false);

    return NULL;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertFloatInterpolator(
    NiInterpolator* pkInterpolator)
{
    NIASSERT(pkInterpolator->IsFloatValueSupported());

    NiBSplineCompFloatInterpolator* pkBSCFInterp = 
        NiDynamicCast(NiBSplineCompFloatInterpolator, pkInterpolator);
    if (pkBSCFInterp)
    {
        if (pkBSCFInterp->GetChannelPosed(0))
        {
            float fValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, fValue));
            return NiNew NiConstFloatEvaluator(fValue);
        }

        NiUInt32 uiCount = pkBSCFInterp->GetControlPointCount(0);
        if (uiCount > 0)
        {
            NiBSplineCompFloatEvaluator* pkBSCFEval =
                NiNew NiBSplineCompFloatEvaluator(
                pkBSCFInterp->GetData(), 
                pkBSCFInterp->GetControlHandle(0), 
                pkBSCFInterp->GetBasisData());
            float fStartTime;
            float fEndTime;
            pkBSCFInterp->GetActiveTimeRange(fStartTime, fEndTime);
            pkBSCFEval->SetTimeRange(fStartTime, fEndTime);
            pkBSCFEval->SetOffset(pkBSCFInterp->GetOffset(0), 0);
            pkBSCFEval->SetHalfRange(pkBSCFInterp->GetHalfRange(0), 0);

            return pkBSCFEval;
        }
    }

    NiBSplineFloatInterpolator* pkBSFInterp = 
        NiDynamicCast(NiBSplineFloatInterpolator, pkInterpolator);
    if (pkBSFInterp)
    {
        if (pkBSFInterp->GetChannelPosed(0))
        {
            float fValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, fValue));
            return NiNew NiConstFloatEvaluator(fValue);
        }

        NiUInt32 uiCount = pkBSFInterp->GetControlPointCount(0);
        if (uiCount > 0)
        {
            NiBSplineFloatEvaluator* pkBSFEval =
                NiNew NiBSplineFloatEvaluator(
                pkBSFInterp->GetData(), 
                pkBSFInterp->GetControlHandle(0), 
                pkBSFInterp->GetBasisData());
            float fStartTime;
            float fEndTime;
            pkBSCFInterp->GetActiveTimeRange(fStartTime, fEndTime);
            pkBSFEval->SetTimeRange(fStartTime, fEndTime);

            return pkBSFEval;
        }
    }

    NiFloatInterpolator* pkFInterp = 
        NiDynamicCast(NiFloatInterpolator, pkInterpolator);
    if (pkFInterp)
    {
        if (pkFInterp->GetChannelPosed(0))
        {
            float fValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, fValue));
            return NiNew NiConstFloatEvaluator(fValue);
        }

        NiFloatData* pkData = pkFInterp->GetFloatData();
        if (pkData)
        {
            return NiNew NiFloatEvaluator(pkData);
        }
    }

    // Unsupported float interpolator type.
    NIASSERT(false);

    return NULL;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertPoint3Interpolator(
    NiInterpolator* pkInterpolator)
{
    NIASSERT(pkInterpolator->IsPoint3ValueSupported());

    NiBSplineCompPoint3Interpolator* pkBSCPInterp = 
        NiDynamicCast(NiBSplineCompPoint3Interpolator, pkInterpolator);
    if (pkBSCPInterp)
    {
        if (pkBSCPInterp->GetChannelPosed(0))
        {
            NiPoint3 kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstPoint3Evaluator(kPosedValue);
        }

        NiBSplineCompPoint3Evaluator* pkBSCPEval = 
            NiNew NiBSplineCompPoint3Evaluator();
        pkBSCPEval->SetData(pkBSCPInterp->GetData(), 
            pkBSCPInterp->GetBasisData());
        float fStartTime;
        float fEndTime;
        pkBSCPInterp->GetActiveTimeRange(fStartTime, fEndTime);
        pkBSCPEval->SetTimeRange(fStartTime, fEndTime);
        pkBSCPEval->SetControlHandle(pkBSCPInterp->GetControlHandle(0), 0);
        pkBSCPEval->SetOffset(pkBSCPInterp->GetOffset(0), 0);
        pkBSCPEval->SetHalfRange(pkBSCPInterp->GetHalfRange(0), 0);

        return pkBSCPEval;
    }

    NiBSplinePoint3Interpolator* pkBSPInterp = 
        NiDynamicCast(NiBSplinePoint3Interpolator, pkInterpolator);
    if (pkBSPInterp)
    {
        if (pkBSPInterp->GetChannelPosed(0))
        {
            NiPoint3 kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstPoint3Evaluator(kPosedValue);
        }

        NiBSplinePoint3Evaluator* pkBSPEval = 
            NiNew NiBSplinePoint3Evaluator();
        pkBSPEval->SetData(pkBSPInterp->GetData(), 
            pkBSPInterp->GetBasisData());
        float fStartTime;
        float fEndTime;
        pkBSPInterp->GetActiveTimeRange(fStartTime, fEndTime);
        pkBSPEval->SetTimeRange(fStartTime, fEndTime);
        pkBSPEval->SetControlHandle(pkBSPInterp->GetControlHandle(0), 0);

        return pkBSPEval;
    }

    NiPoint3Interpolator* pkPInterp = 
        NiDynamicCast(NiPoint3Interpolator, pkInterpolator);
    if (pkPInterp)
    {
        if (pkPInterp->GetChannelPosed(0))
        {
            NiPoint3 kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstPoint3Evaluator(kPosedValue);
        }

        NiPosData* pkData = pkPInterp->GetPoint3Data();
        if (pkData)
        {
            return NiNew NiPoint3Evaluator(pkData);
        }
    }

    // Unsupported point3 interpolator type.
    NIASSERT(false);

    return NULL;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertQuaternionInterpolator(
    NiInterpolator* pkInterpolator)
{
    NIASSERT(pkInterpolator->IsQuaternionValueSupported());

    NiQuaternionInterpolator* pkQInterp = 
        NiDynamicCast(NiQuaternionInterpolator, pkInterpolator);
    if (pkQInterp)
    {
        if (pkQInterp->GetChannelPosed(0))
        {
            NiQuaternion kPosedValue;
            NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));
            return NiNew NiConstQuaternionEvaluator(kPosedValue);
        }

        NiRotData* pkData = pkQInterp->GetQuaternionData();
        if (pkData)
        {
            return NiNew NiQuaternionEvaluator(pkData);
        }
    }

    // Unsupported quaternion interpolator type.
    NIASSERT(false);

    return NULL;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertTransformInterpolator(
    NiInterpolator* pkInterpolator, const NiFixedString& kAVObjectName,
    bool bCollapseInterpolator)
{
    NIASSERT(pkInterpolator->IsTransformValueSupported());

    NiLookAtInterpolator* pkLookAtInterp = 
        NiDynamicCast(NiLookAtInterpolator, pkInterpolator);
    if (pkLookAtInterp)
    {
        return ConvertLookAtInterpolator(pkLookAtInterp, kAVObjectName, 
            bCollapseInterpolator);
    }

    NiPathInterpolator* pkPathInterp = 
        NiDynamicCast(NiPathInterpolator, pkInterpolator);
    if (pkPathInterp)
    {
        return ConvertPathInterpolator(pkPathInterp);
    }

    NiQuatTransform kPosedValue;
    NIVERIFY(pkInterpolator->Update(0, NULL, kPosedValue));

    NiBSplineCompTransformInterpolator* pkBSCTInterp = 
        NiDynamicCast(NiBSplineCompTransformInterpolator, pkInterpolator);
    if (pkBSCTInterp)
    {
        if (pkBSCTInterp->GetChannelPosed(0) && 
            pkBSCTInterp->GetChannelPosed(1) && 
            pkBSCTInterp->GetChannelPosed(2))
        {
            return NiNew NiConstTransformEvaluator(kPosedValue);
        }

        return ConvertBSplineCompTransformInterpolator(pkBSCTInterp, 
            kPosedValue);
    }

    NiBSplineTransformInterpolator* pkBSTInterp = 
        NiDynamicCast(NiBSplineTransformInterpolator, pkInterpolator);
    if (pkBSTInterp)
    {
        if (pkBSTInterp->GetChannelPosed(0) && 
            pkBSTInterp->GetChannelPosed(1) && 
            pkBSTInterp->GetChannelPosed(2))
        {
            return NiNew NiConstTransformEvaluator(kPosedValue);
        }

        return ConvertBSplineTransformInterpolator(pkBSTInterp, kPosedValue);
    }

    NiTransformInterpolator* pkTInterp = 
        NiDynamicCast(NiTransformInterpolator, pkInterpolator);
    if (pkTInterp)
    {
        if (pkTInterp->GetChannelPosed(0) && 
            pkTInterp->GetChannelPosed(1) && 
            pkTInterp->GetChannelPosed(2))
        {
            return NiNew NiConstTransformEvaluator(kPosedValue);
        }

        return ConvertTransformInterpolator(pkTInterp, kPosedValue);
    }

    // Unsupported transform interpolator type.
    NIASSERT(false);

    return NULL;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertBSplineCompTransformInterpolator(
    NiBSplineCompTransformInterpolator* pkBSCTInterp, 
    const NiQuatTransform& kPosedValue)
{
    // Create the evaluator.
    NiBSplineCompTransformEvaluator* pkBSCTEval = 
        NiNew NiBSplineCompTransformEvaluator();

    // Initialize the evaluator from the interpolator.
    pkBSCTEval->SetData(pkBSCTInterp->GetData(), pkBSCTInterp->GetBasisData());
    float fStartTime;
    float fEndTime;
    pkBSCTInterp->GetActiveTimeRange(fStartTime, fEndTime);
    pkBSCTEval->SetTimeRange(fStartTime, fEndTime);

    // Convert the position, rotation, and scale channels.
    if (kPosedValue.IsTranslateValid())
    {
        NIASSERT(NiBSplineCompTransformInterpolator::POSITION == 0);
        if (pkBSCTInterp->GetChannelPosed(0))
        {
            pkBSCTEval->SetPoseTranslate(kPosedValue.GetTranslate());
        }
        else
        {
            pkBSCTEval->SetControlHandle(
                pkBSCTInterp->GetControlHandle(0), 0);
            pkBSCTEval->SetOffset(pkBSCTInterp->GetOffset(0), 0);
            pkBSCTEval->SetHalfRange(pkBSCTInterp->GetHalfRange(0), 0);
        }
    }

    if (kPosedValue.IsRotateValid())
    {
        NIASSERT(NiBSplineCompTransformInterpolator::ROTATION == 1);
        if (pkBSCTInterp->GetChannelPosed(1))
        {
            pkBSCTEval->SetPoseRotate(kPosedValue.GetRotate());
        }
        else
        {
            pkBSCTEval->SetControlHandle(
                pkBSCTInterp->GetControlHandle(1), 1);
            pkBSCTEval->SetOffset(pkBSCTInterp->GetOffset(1), 1);
            pkBSCTEval->SetHalfRange(pkBSCTInterp->GetHalfRange(1), 1);
        }
    }

    if (kPosedValue.IsScaleValid())
    {
        NIASSERT(NiBSplineCompTransformInterpolator::SCALE == 2);
        if (pkBSCTInterp->GetChannelPosed(2))
        {
            pkBSCTEval->SetPoseScale(kPosedValue.GetScale());
        }
        else
        {
            pkBSCTEval->SetControlHandle(
                pkBSCTInterp->GetControlHandle(2), 2);
            pkBSCTEval->SetOffset(pkBSCTInterp->GetOffset(2), 2);
            pkBSCTEval->SetHalfRange(pkBSCTInterp->GetHalfRange(2), 2);
        }
    }

    return pkBSCTEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertBSplineTransformInterpolator(
    NiBSplineTransformInterpolator* pkBSTInterp, 
    const NiQuatTransform& kPosedValue)
{
    // Create the evaluator.
    NiBSplineTransformEvaluator* pkBSTEval = 
        NiNew NiBSplineTransformEvaluator();

    // Initialize the evaluator from the interpolator.
    pkBSTEval->SetData(pkBSTInterp->GetData(), pkBSTInterp->GetBasisData());
    float fStartTime;
    float fEndTime;
    pkBSTInterp->GetActiveTimeRange(fStartTime, fEndTime);
    pkBSTEval->SetTimeRange(fStartTime, fEndTime);

    // Convert the position, rotation, and scale channels.
    if (kPosedValue.IsTranslateValid())
    {
        NIASSERT(NiBSplineTransformInterpolator::POSITION == 0);
        if (pkBSTInterp->GetChannelPosed(0))
        {
            pkBSTEval->SetPoseTranslate(kPosedValue.GetTranslate());
        }
        else
        {
            pkBSTEval->SetControlHandle(
                pkBSTInterp->GetControlHandle(0), 0);
        }
    }

    if (kPosedValue.IsRotateValid())
    {
        NIASSERT(NiBSplineTransformInterpolator::ROTATION == 1);
        if (pkBSTInterp->GetChannelPosed(1))
        {
            pkBSTEval->SetPoseRotate(kPosedValue.GetRotate());
        }
        else
        {
            pkBSTEval->SetControlHandle(
                pkBSTInterp->GetControlHandle(1), 1);
        }
    }

    if (kPosedValue.IsScaleValid())
    {
        NIASSERT(NiBSplineTransformInterpolator::SCALE == 2);
        if (pkBSTInterp->GetChannelPosed(2))
        {
            pkBSTEval->SetPoseScale(kPosedValue.GetScale());
        }
        else
        {
            pkBSTEval->SetControlHandle(
                pkBSTInterp->GetControlHandle(2), 2);
        }
    }

    return pkBSTEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertTransformInterpolator(
    NiTransformInterpolator* pkTInterp, 
    const NiQuatTransform& kPosedValue)
{
    // Create the evaluator.
    NiTransformEvaluator* pkTEval = NiNew NiTransformEvaluator();

    // Initialize the evaluator from the interpolator.

    // Convert the position channel.
    if (kPosedValue.IsTranslateValid())
    {
        NIASSERT(NiTransformInterpolator::POSITION == 0);
        if (pkTInterp->GetChannelPosed(0))
        {
            pkTEval->SetPoseTranslate(kPosedValue.GetTranslate());
        }
        else
        {
            unsigned int uiNumKeys;
            NiPosKey::KeyType eType;
            unsigned char ucSize;
            NiPosKey* pkKeys = pkTInterp->GetPosData(uiNumKeys, 
                eType, ucSize);
            if (pkKeys)
            {
                // Create new pos key array.
                NiPosKey::ArrayFunction pfnArrayFunc = 
                    NiPosKey::GetArrayFunction(eType);
                NIASSERT(pfnArrayFunc);
                NiPosKey* pkNewKeys = (NiPosKey*)pfnArrayFunc(uiNumKeys);

                // Copy keys to new array.
                NiPosKey::CopyFunction copy =
                    NiPosKey::GetCopyFunction(eType);
                NIASSERT(copy);
                for (unsigned int uk = 0; uk < uiNumKeys; uk++)
                {
                    copy(pkNewKeys->GetKeyAt(uk, ucSize),
                        pkKeys->GetKeyAt(uk, ucSize));
                }

                pkTEval->ReplacePosData(pkNewKeys, uiNumKeys, eType);
            }
        }
    }

    // Convert the rotation channel.
    if (kPosedValue.IsRotateValid())
    {
        NIASSERT(NiTransformInterpolator::ROTATION == 1);
        if (pkTInterp->GetChannelPosed(1))
        {
            pkTEval->SetPoseRotate(kPosedValue.GetRotate());
        }
        else
        {
            unsigned int uiNumKeys;
            NiRotKey::KeyType eType;
            unsigned char ucSize;
            NiRotKey* pkKeys = pkTInterp->GetRotData(uiNumKeys, 
                eType, ucSize);
            if (pkKeys)
            {
                // Create new rot key array.
                NiRotKey::ArrayFunction pfnArrayFunc = 
                    NiRotKey::GetArrayFunction(eType);
                NIASSERT(pfnArrayFunc);
                NiRotKey* pkNewKeys = (NiRotKey*)pfnArrayFunc(uiNumKeys);

                // Copy keys to new array.
                NiRotKey::CopyFunction copy =
                    NiRotKey::GetCopyFunction(eType);
                NIASSERT(copy);
                for (unsigned int uk = 0; uk < uiNumKeys; uk++)
                {
                    copy(pkNewKeys->GetKeyAt(uk, ucSize),
                        pkKeys->GetKeyAt(uk, ucSize));
                }

                pkTEval->ReplaceRotData(pkNewKeys, uiNumKeys, eType);
            }
        }
    }

    // Convert the scale channel.
    if (kPosedValue.IsScaleValid())
    {
        NIASSERT(NiTransformInterpolator::SCALE == 2);
        if (pkTInterp->GetChannelPosed(2))
        {
            pkTEval->SetPoseScale(kPosedValue.GetScale());
        }
        else
        {
            unsigned int uiNumKeys;
            NiFloatKey::KeyType eType;
            unsigned char ucSize;
            NiFloatKey* pkKeys = pkTInterp->GetScaleData(uiNumKeys, 
                eType, ucSize);
            if (pkKeys)
            {
                // Create new float key array.
                NiFloatKey::ArrayFunction pfnArrayFunc = 
                    NiFloatKey::GetArrayFunction(eType);
                NIASSERT(pfnArrayFunc);
                NiFloatKey* pkNewKeys = (NiFloatKey*)pfnArrayFunc(uiNumKeys);

                // Copy keys to new array.
                NiFloatKey::CopyFunction copy =
                    NiFloatKey::GetCopyFunction(eType);
                NIASSERT(copy);
                for (unsigned int uk = 0; uk < uiNumKeys; uk++)
                {
                    copy(pkNewKeys->GetKeyAt(uk, ucSize),
                        pkKeys->GetKeyAt(uk, ucSize));
                }

                pkTEval->ReplaceScaleData(pkNewKeys, uiNumKeys, eType);
            }
        }
    }

    return pkTEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertLookAtInterpolator(
    NiLookAtInterpolator* pkLookAtInterp, 
    const NiFixedString& kAVObjectName, 
    bool bCollapseInterpolator)
{
    // Convert the internal interpolators to evaluators: exit on failure.
    NiEvaluatorPtr spTranslateEval;
    NiEvaluatorPtr spRollEval;
    NiEvaluatorPtr spScaleEval;

    NiEvaluator::IDTag kInternalIDTag;

    NiInterpolator* pkInterp = pkLookAtInterp->GetTranslateInterpolator();
    if (pkInterp)
    {
        spTranslateEval = NiInterpolatorConverter::Convert(pkInterp, 
            kInternalIDTag, bCollapseInterpolator);
        if (!spTranslateEval)
        {
            return NULL;
        }
    }

    pkInterp = pkLookAtInterp->GetRollInterpolator();
    if (pkInterp)
    {
        spRollEval = NiInterpolatorConverter::Convert(pkInterp, 
            kInternalIDTag, bCollapseInterpolator);
        if (!spRollEval)
        {
            return NULL;
        }
    }

    pkInterp = pkLookAtInterp->GetScaleInterpolator();
    if (pkInterp)
    {
        spScaleEval = NiInterpolatorConverter::Convert(pkInterp, 
            kInternalIDTag, bCollapseInterpolator);
        if (!spScaleEval)
        {
            return NULL;
        }
    }

    // Create the evaluator.
    NiLookAtEvaluator* pkLookAtEval = NiNew NiLookAtEvaluator();

    // Initialize the evaluator from the interpolator.
    pkLookAtEval->SetTranslateEvaluator(spTranslateEval);
    pkLookAtEval->SetRollEvaluator(spRollEval);
    pkLookAtEval->SetScaleEvaluator(spScaleEval);
    pkLookAtEval->SetFlip(pkLookAtInterp->GetFlip());
    pkLookAtEval->SetAxis(
        (NiLookAtEvaluator::Axis)pkLookAtInterp->GetAxis());

    pkLookAtEval->SetLookAtObjectName(pkLookAtInterp->GetLookAtName());
    pkLookAtEval->SetDrivenObjectName(kAVObjectName);

    return pkLookAtEval;
}
//---------------------------------------------------------------------------
NiEvaluator* NiInterpolatorConverter::ConvertPathInterpolator(
    NiPathInterpolator* pkPathInterp)
{
    // Check if the source path and percentage data are defined.
    NiPosData* pkPathData = pkPathInterp->GetPathData();
    NiFloatData* pkPctData = pkPathInterp->GetPctData();
    if (!pkPathData || !pkPctData)
    {
        return NULL;
    }

    // Create the evaluator.
    NiPathEvaluator* pkPathEval = NiNew NiPathEvaluator();

    // Reuse the source path and percentage data.
    pkPathEval->SetPathData(pkPathData);
    pkPathEval->SetPctData(pkPctData);

    // Initialize the evaluator from the interpolator.
    pkPathEval->SetAllowFlip(pkPathInterp->GetAllowFlip());
    pkPathEval->SetBank(pkPathInterp->GetBank());
    pkPathEval->SetBankDir(
        (NiPathEvaluator::BankDir)pkPathInterp->GetBankDir());
    pkPathEval->SetConstVelocity(pkPathInterp->GetConstVelocity());
    pkPathEval->SetFollow(pkPathInterp->GetFollow());
    pkPathEval->SetMaxBankAngle(pkPathInterp->GetMaxBankAngle());
    pkPathEval->SetSmoothing(pkPathInterp->GetSmoothing());
    pkPathEval->SetFollowAxis(pkPathInterp->GetFollowAxis());
    pkPathEval->SetFlip(pkPathInterp->GetFlip());

    return pkPathEval;
}
//---------------------------------------------------------------------------
NiFixedString NiInterpolatorConverter::ConvertCtlrType(
    const NiFixedString& kCtlrType)
{
    NIASSERT(kCtlrType.Exists());

    NiFixedString kNewCtlrType(kCtlrType);

    // Convert old animation system types.
    if (strcmp(kNewCtlrType, NiAnimationConstants::GetGeomMorpherCtlrType()) == 0)
    {
        kNewCtlrType = NiAnimationConstants::GetMorphWeightsCtlrType();
    }
    // Convert old particle system types.
    else if (strcmp(kNewCtlrType, "NiPSysEmitterCtlr") == 0)
    {
        kNewCtlrType = NiAnimationConstants::GetPSEmitParticlesCtlrType();
    }
    else if (strcmp(kNewCtlrType, "NiPSysModifierActiveCtlr") == 0)
    {
        kNewCtlrType = "NiPSForceActiveCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysAirFieldAirFrictionCtlr") == 0)
    {
        kNewCtlrType = "NiPSAirFieldAirFrictionCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysAirFieldInheritVelocityCtlr") == 0)
    {
        kNewCtlrType = "NiPSAirFieldInheritedVelocityCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysAirFieldSpreadCtlr") == 0)
    {
        kNewCtlrType = "NiPSAirFieldSpreadCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterDeclinationCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterDeclinationCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterDeclinationVarCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterDeclinationVarCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterInitialRadiusCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterRadiusCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterLifeSpanCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterLifeSpanCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterPlanarAngleCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterPlanarAngleCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterPlanarAngleVarCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterPlanarAngleVarCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysEmitterSpeedCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterSpeedCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysFieldAttenuationCtlr") == 0)
    {
        kNewCtlrType = "NiPSFieldAttenuationCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysFieldMagnitudeCtlr") == 0)
    {
        kNewCtlrType = "NiPSFieldMagnitudeCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysFieldMaxDistanceCtlr") == 0)
    {
        kNewCtlrType = "NiPSFieldMaxDistanceCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysGravityStrengthCtlr") == 0)
    {
        kNewCtlrType = "NiPSGravityStrengthCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysInitialRotAngleCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterRotAngleCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysInitialRotAngleVarCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterRotAngleVarCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysInitialRotSpeedCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterRotSpeedCtlr";
    }
    else if (strcmp(kNewCtlrType, "NiPSysInitialRotSpeedVarCtlr") == 0)
    {
        kNewCtlrType = "NiPSEmitterRotSpeedVarCtlr";
    }

    return kNewCtlrType;
}
//---------------------------------------------------------------------------
