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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoAnimMap.h"
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoAnimCurves.h"

#include "FCurve.h"
#include "XSITransform.h"
#include "Model.h"

#include "NiTransformController.h"
#include "NiTransformInterpolator.h"
#include "NiFloatInterpolator.h"
#include "NiLinPosKey.h"
#include "NiLinRotKey.h"
#include "NiLinFloatKey.h"
#include "NiNode.h"

//---------------------------------------------------------------------------
// For reference, here is the NiAnimation class hierarchy:
//
//   NiTimeController
//     NiControllerManager
//     NiBoneLODController
//     NiInterpController
//       NiSingleInterpController
//         NiBoolInterpController
//           NiVisController
//         NiColorInterpController
//         NiFloatInterpController
//           NiAlphaController
//           NiFlipController
//           NiLightDimmerController
//           NiTextureTransformController
//         NiExtraDataController
//           NiColorExtraDataController
//           NiFloatExtraDataController
//           NiFloatsExtraDataController
//           NiFloatsExtraDataPoint3Controller
//         NiPoint3InterpController
//           NiLightColorController
//           NiMaterialColorController
//         NiQuaternionInterpController
//         NiTransformController
//       NiMorphWeightsController
//       NiMultiTargetTransformController
//
//   NiControllerSequence
//
//   NiInterpolator
//     NiBlendInterpolator
//       NiBlendBoolInterpolator
//       NiBlendColorInterpolator
//       NiBlendFloatInterpolator
//       NiBlendPoint3Interpolator
//       NiBlendQuaternionInterpolator
//       NiBlendTransformInterpolator
//     NiKeyBasedInterpolator
//       NiBoolInterpolator
//         NiBoolTimelineInterpolator
//       NiColorInterpolator
//       NiFloatInterpolator
//       NiPathInterpolator
//       NiPoint3Interpolator
//       NiQuaternionInterpolator
//       NiTransformInterpolator
//     NiBSplineInterpolator
//       NiBSplineColorInterpolator
//         NiBSplineCompColorInterpolator
//       NiBSplineFloatInterpolator
//         NiBSplineCompFloatInterpolator
//       NiBSplinePoint3Interpolator
//         NiBSplineCompPoint3Interpolator
//       NiBSplineTransformInterpolator
//         NiBSplineCompTransformInterpolator
//     NiLookAtInterpolator
//
//   NiAnimationKey
//     NiFloatKey           (FLOATKEY)
//       NiLinFloatKey        (LINKEY)
//       NiBezFloatKey        (BEZKEY)
//       NiTCBFloatKey        (TCBKEY)
//       NiStepFloatKey       (STEPKEY)
//     NiPosKey             (POSKEY)
//       NiLinPosKey          (LINKEY)
//       NiBezPosKey          (BEZKEY)
//       NiTCBPosKey          (TCBKEY)
//       NiStepPosKey         (STEPKEY)
//     NiRotKey             (ROTKEY)
//       NiLinRotKey          (LINKEY)
//       NiBezRotKey          (BEZKEY)
//       NiTCBRotKey          (TCBKEY)
//       NiEulerRotKey        (EULERKEY)
//       NiStepRotKey         (STEPKEY)
//     NiColorKey           (COLORKEY)
//       NiLinColorKey        (LINKEY)
//       NiStepColorKey       (STEPKEY)
//     NiTextKey            (TEXTKEY)
//     NiBoolKey            (BOOLKEY)
//       NiStepBoolKey        (STEPKEY)
//
//   NiBoolData
//   NiColorData
//   NiExtraData
//     NiColorExtraData
//     NiFloatExtraData
//     NiFloatsExtraData
//     NiTextKeyExtraData
//   NiFloatData
//   NiPosData
//   NiQuatTransform
//   NiRotData
//   NiTransformData
//
//   NiBSlineBasis<DEGREE,REALTYPE>
//   NiBplineBasisData
//   NiBSplineFit<DIMENSION,DEGREE>
//
//   NiKFMTool
//
// Usage:
//   - NiTimeController updates the targeted object (SetTarget())
//     at the proper interval.
//   - NiInterpController uses interpolators to determine the value.
//   - Each NiInterpController sub-classes is specialized for
//     a particular parameter of a particular object type,
//     for example the color of a light.
//   - NiInterpController defers the interpolation to an attached
//     NiInterpolator.
//   - NiKeyBasedInterpolator interpolates between key-frames
//     (or key-times) using an interpolation algorithm based on
//     the type of the keys. Supported types: step, linear,
//     hermite or spline.
//   - NiBSplineInterpolator interpolates using a BSpline.
//   - NiBlendInterpolator blends multiple interpolators into
//     one value.
//
//---------------------------------------------------------------------------

namespace
{
    //---------------------------------------------------------------------------
    // The number of curves for each type of transform curve.
    // Three (3) since transforms are always 3-dimensional.
    //---------------------------------------------------------------------------
    enum { TRANSFORM_AXIS_COUNT = 3 };

    //---------------------------------------------------------------------------
    // Table of X, Y and Z curve names based on node type and curve type.
    //
    // Note: we keep the X and Z rotations in place, we don't swap them here.
    //       We assume that whoever use the curve will do the necessary swapping.
    //       For example, ConvertTransformCurvesKeys() (see below) calls
    //       ConvertRotation() which does the proper swapping.
    //---------------------------------------------------------------------------
    const char* gTransformCurveNames[epg::DUMMY_NODE_TYPE_COUNT]
                                    [epg::TRANSFORM_CURVE_TYPE_COUNT]
                                    [TRANSFORM_AXIS_COUNT] =
    {
        // NEUTRAL_POSE_DUMMY_NODE
        {
            // CURVE_POS
            { "nposx", "nposy", "nposz" },
            // CURVE_ROT
            { "nrotx", "nroty", "nrotz" },
            // CURVE_SCALE
            { "nsclx", "nscly", "nsclz" }
        },
        // PIVOT_COMPENSATION_DUMMY_NODE
        {
            // CURVE_POS
            { "pcposx", "pcposy", "pcposz" },
            // CURVE_ROT
            { "pcrotx", "pcroty", "pcrotz" },
            // CURVE_SCALE
            { "pcsclx", "pcscly", "pcsclz" }
        },
        // TRANSLATE_DUMMY_NODE
        {
            // CURVE_POS
            { "posx", "posy", "posz" },
            // CURVE_ROT
            { 0, 0, 0 },
            // CURVE_SCALE
            { 0, 0, 0 }
        },
        // PIVOT_DUMMY_NODE
        {
            // CURVE_POS
            { "pposx", "pposy", "pposz" },
            // CURVE_ROT
            { "protx", "proty", "protz" },
            // CURVE_SCALE
            { "psclx", "pscly", "psclz" }
        },
        // SCALE_ROTATE_DUMMY_NODE
        {
            // CURVE_POS
            { 0, 0, 0 },
            // CURVE_ROT
            { "rotx", "roty", "rotz" },
            // CURVE_SCALE
            { "sclx", "scly", "sclz" }
        },
        // ANTI_PIVOT_DUMMY_NODE
        // Note: if the pivot is animated, we're in trouble here...
        {
            // CURVE_POS
            { 0, 0, 0 },
            // CURVE_ROT
            { 0, 0, 0 },
            // CURVE_SCALE
            { 0, 0, 0 }
        }
    };

    //---------------------------------------------------------------------------
    // Table of printable curve names
    //---------------------------------------------------------------------------
    const char* gTransformPrintableCurveNames[epg::TRANSFORM_CURVE_TYPE_COUNT]
                                             [TRANSFORM_AXIS_COUNT] =
    {
        { "X position", "Y position", "Z position" },
        { "X rotation", "Y rotation", "Z rotation" },
        { "X scale",    "Y scale",    "Z scale"    }
    };

    //---------------------------------------------------------------------------
    // Maximum deltas before sub-dividing curves, by curve type.
    // For now, only rotation must be sub-divided because angle
    // angles greater than 180 confuse the interpolator or controler.
    //
    // Note: 0.0 means large deltas are no sub0divided.
    //---------------------------------------------------------------------------
    const float gTransformMaxDeltas[epg::TRANSFORM_CURVE_TYPE_COUNT] =
    {
        // CURVE_POS
        0.0f,
        // CURVE_ROT
        150.0f,
        // CURVE_SCALE
        0.0f
    };

    //---------------------------------------------------------------------------
    // This control if all axis are required to be animated uniformly.
    // IOW, if all values must be the same.
    //
    // Note: for now, only the scale must be uniform.
    //---------------------------------------------------------------------------
    const epg::AnimCurvesFlags gTransformCurveFlags[epg::TRANSFORM_CURVE_TYPE_COUNT] =
    {
        // CURVE_POS
        epg::NO_ANIM_CURVES_FLAG,
        // CURVE_ROT
        epg::NO_ANIM_CURVES_FLAG,
        // CURVE_SCALE
        epg::CURVE_MUST_BE_UNIFORM_FLAG
    };

    //---------------------------------------------------------------------------
    // Convert a transform animation curves to the appropriate keys.
    //---------------------------------------------------------------------------
    bool ConvertTransformCurvesKeys(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLTemplate::ERotationOrder in_RotOrder,
        epg::AnimCurves& in_Curves,
        epg::TransformCurveType in_CurveType,
        const epg::ActionTimeline& in_Timeline,
        std::vector<NiLinPosKey>& io_PosKeys,
        std::vector<NiLinRotKey>& io_RotKeys,
        std::vector<NiLinFloatKey>& io_FloatKeys)
    {
        if (!in_Curves.IsValid())
            return false;

        if (!in_Curves.HasAnimation())
            return true;

        typedef epg::AnimCurves::TimedValues TimedValues;
        const TimedValues& values = in_Curves.GetTimedValues();

        const int count = values.size();
        if (count <= 0)
            return true;

        switch (in_CurveType)
        {
            case epg::TRANSFORM_CURVE_POS:
            {
                TimedValues::const_iterator pos = values.begin();
                for (SI_Int i = 0; i < count; ++i, ++pos)
                {
                    if (!IsInTimeline(in_Timeline, pos->first))
                        continue;

                    NiLinPosKey key;
                    key.SetTime(pos->first);
                    key.SetPos(NiPoint3(
                        pos->second[0], pos->second[1], pos->second[2]));
                    io_PosKeys.push_back(key);
                }

                break;
            }

            case epg::TRANSFORM_CURVE_ROT:
            {
                TimedValues::const_iterator pos = values.begin();
                for (SI_Int i = 0; i < count; ++i, ++pos)
                {
                    if (!IsInTimeline(in_Timeline, pos->first))
                        continue;

                    NiLinRotKey key;
                    key.SetTime(pos->first);

                    CSIBCVector3D xsiRot(
                        pos->second[0], pos->second[1], pos->second[2]);

                    NiMatrix3 gbRot;
                    if (!epg::ConvertRotation(io_Context, in_Model, xsiRot, in_RotOrder, gbRot))
                        return false;

                    NiQuaternion gbQuaternion;
                    gbQuaternion.FromRotation(gbRot);

                    key.SetQuaternion(gbQuaternion);
                    io_RotKeys.push_back(key);
                }

                break;
            }

            case epg::TRANSFORM_CURVE_SCALE:
            {
                TimedValues::const_iterator pos = values.begin();
                for (SI_Int i = 0; i < count; ++i, ++pos)
                {
                    if (!IsInTimeline(in_Timeline, pos->first))
                        continue;

                    NiLinFloatKey key;
                    key.SetTime(pos->first);
                    // Note: we assume that the AnimCurves properly
                    //       verified that the scaling is uniform, so we
                    //       can use a single value.
                    key.SetValue(pos->second[0]);
                    io_FloatKeys.push_back(key);
                }

                break;
            }

            default:
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Trying to convert an unsupported animation curve type in \"%s\".",
                    in_Model.Name().GetText());
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the animation curves found in the XSI transform object.
    //---------------------------------------------------------------------------
    int ConvertTransformCurves(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& in_Object,
        CSLTemplate& in_Transform,
        CSLTemplate::ERotationOrder in_RotOrder,
        epg::TransformCurveType in_CurveType,
        const epg::AnimCurvesDescriptor& in_AnimDesc,
        std::vector<NiLinPosKey> &posKeys,
        std::vector<NiLinRotKey> &rotKeys,
        std::vector<NiLinFloatKey> &scaleKeys)
    {
        epg::AnimCurves curves(
            io_Context,
            in_Transform,
            in_Object.GetName(),
            in_AnimDesc);

        if (!curves.IsValid())
            return -1;

        if (!curves.HasAnimation())
            return 0;

        epg::AnimationMap& animMap = io_Context.GetAnimationMap();
        NiTransformController& controller = animMap.GetTransformAnim(in_Object);
        // Note: as far as we can see, all XSI FCurve never loop.
        controller.SetCycleType(NiTimeController::CLAMP);
        if (!ConvertTransformCurvesKeys(io_Context, in_Model, in_RotOrder, curves, in_CurveType,
                animMap.GetSceneAnimationTimeline(), posKeys, rotKeys, scaleKeys))
                return -1;

        return 1;
    }

    //---------------------------------------------------------------------------
    // Convert the animation curves contained in the XSI actions.
    //---------------------------------------------------------------------------
    int ConvertActionCurves(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& in_Object,
        CSLTemplate::ERotationOrder in_RotOrder,
        epg::TransformCurveType in_CurveType,
        const epg::AnimCurvesDescriptor& in_AnimDesc,
        std::vector<NiLinPosKey> &posKeys,
        std::vector<NiLinRotKey> &rotKeys,
        std::vector<NiLinFloatKey> &scaleKeys)
    {
        epg::AnimationMap& animMap = io_Context.GetAnimationMap();
        epg::AnimationMap::AnimActions* pActions = animMap.FindAnimActions(in_Model);
        if (!pActions)
            return 0;

        int actionCurveCount = 0;

        typedef epg::AnimationMap::AnimActions::iterator iter;

        for (iter pos = pActions->begin(); pos != pActions->end(); ++pos)
        {
            epg::ActionDescriptor& actionDesc = pos->second;

            epg::AnimCurves curves(
                io_Context,
                actionDesc,
                in_Object.GetName(),
                in_AnimDesc);

            if (!curves.IsValid())
                return -1;

            if (!curves.HasAnimation())
                continue;

            if (!ConvertTransformCurvesKeys(
                io_Context, in_Model, in_RotOrder, curves, in_CurveType,
                actionDesc.GetTimeline(), posKeys, rotKeys, scaleKeys))
            {
                return -1;
            }

            ++actionCurveCount;
        }
        
        return actionCurveCount;
    }

    //---------------------------------------------------------------------------
    // Get the default values for a given anim curve type.
    //---------------------------------------------------------------------------
    void GetTransformAnimCurveDefaultValues(
        const CSIBCVector3D& in_DefaultScale,
        const CSIBCVector3D& in_DefaultTranslation,
        const CSIBCVector3D& in_DefaultRotation,
        CSLTemplate::ERotationOrder in_RotOrder,
        epg::TransformCurveType in_CurveType,
        float* out_pDefaultValue,
        int in_DefaultValueCount)
    {
        NIASSERT(TRANSFORM_AXIS_COUNT == in_DefaultValueCount);

        switch (in_CurveType)
        {
            case epg::TRANSFORM_CURVE_POS:
            {
                out_pDefaultValue[0] = in_DefaultTranslation.GetX();
                out_pDefaultValue[1] = in_DefaultTranslation.GetY();
                out_pDefaultValue[2] = in_DefaultTranslation.GetZ();
                break;
            }
            case epg::TRANSFORM_CURVE_ROT:
            {
                out_pDefaultValue[0] = in_DefaultRotation.GetX();
                out_pDefaultValue[1] = in_DefaultRotation.GetY();
                out_pDefaultValue[2] = in_DefaultRotation.GetZ();
                break;
            }
            case epg::TRANSFORM_CURVE_SCALE:
            {
                out_pDefaultValue[0] = in_DefaultScale.GetX();
                out_pDefaultValue[1] = in_DefaultScale.GetY();
                out_pDefaultValue[2] = in_DefaultScale.GetZ();
                break;
            }
            default:
            {
                out_pDefaultValue[0] = 0.0f;
                out_pDefaultValue[1] = 0.0f;
                out_pDefaultValue[2] = 0.0f;
            }
        }
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Verify if there are animations applied to the transform for the specified
    // dummy node type.
    //---------------------------------------------------------------------------
    bool IsTransformAnimated(
        Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& in_TransformXSI,
        DummyNodeType in_DummyNodeType)
    {
        epg::AnimationMap& animMap = io_Context.GetAnimationMap();
        epg::AnimationMap::AnimActions* pActions = animMap.FindAnimActions(in_Model);

        static const int MAX_DUMMY_NODE_TYPE = sizeof(gTransformCurveNames)
                                             / sizeof(gTransformCurveNames[0]);
        if (in_DummyNodeType >= 0 && in_DummyNodeType < MAX_DUMMY_NODE_TYPE)
        {
            for (int curveType = 0; curveType < TRANSFORM_CURVE_TYPE_COUNT; ++curveType)
            {
                for (int i = 0; i < TRANSFORM_AXIS_COUNT; ++i)
                {
                    const char* pName = gTransformCurveNames[in_DummyNodeType][curveType][i];
                    if (pName && pName[0])
                    {
                        // Note: Crosswalk is not const-correct with strings... oh well.
                        if (in_TransformXSI.GetParameterFCurve((SI_Char*)pName))
                        {
                            return true;
                        }

                        if (pActions)
                        {
                            typedef epg::AnimationMap::AnimActions::iterator iter;
                            for (iter pos = pActions->begin(); pos != pActions->end(); ++pos)
                            {
                                epg::ActionDescriptor& actionDesc = pos->second;
                                if (actionDesc.FindAnimCurve(pName))
                                {
                                    return true;
                                }
                                if (actionDesc.FindStaticValue(pName))
                                {
                                    return true;
                                }
                            }
                        }

                    }
                }
            }
        }

        return false;
    }

    //---------------------------------------------------------------------------
    // Convert an animation curve to a float interpolator.
    //---------------------------------------------------------------------------
    NiFloatInterpolator* ConvertFloatCurves(
        Context& io_Context,
        const char* in_ModelName,
        AnimCurves& in_Curves,
        float& io_InitialValue)
    {
        if (!in_Curves.IsValid())
            return 0;

        if (!in_Curves.HasAnimation() || in_Curves.GetTimedValues().size() < 1)
        {
            // No curve: use constant interpolator.
            NiFloatInterpolator* pInterpolator = NiNew NiFloatInterpolator(io_InitialValue);
            return pInterpolator;
        }
        else
        {
            if (in_Curves.GetCurveCount() != 1)
            {
                io_Context.Logf(
                    epg::LOG_ERROR,
                    "The animation in \"%s\" uses an animation curve"
                    " with more than one dimension.",
                    in_ModelName);
                return 0;
            }

            // Use linear interpolator, linearity is verified by the AnimCurves class.
            typedef AnimCurves::TimedValues TimedValues;
            const TimedValues& values = in_Curves.GetTimedValues();
            const int count = values.size();
            NiLinFloatKey* pGBKeys = NiNew NiLinFloatKey[count];
            TimedValues::const_iterator pos = values.begin();
            for (SI_Int i = 0; i < count; ++i, ++pos)
            {
                pGBKeys[i].SetTime(pos->first);
                pGBKeys[i].SetValue(pos->second[0]);
            }

            NiFloatKey::FillDerivedValsFunction pDeriv;
            pDeriv = NiFloatKey::GetFillDerivedFunction(NiAnimationKey::LINKEY);
            NIASSERT(pDeriv);
            (*pDeriv)(pGBKeys, count, sizeof(NiLinFloatKey));

            io_InitialValue = pGBKeys[0].GetValue();

            NiFloatInterpolator* pInterpolator = NiNew NiFloatInterpolator;
            pInterpolator->ReplaceKeys(pGBKeys, count, NiAnimationKey::LINKEY);

            return pInterpolator;
        }
    }

    //---------------------------------------------------------------------------
    // Conversion of node transform animation.
    //---------------------------------------------------------------------------
    bool ConvertTransformAnimation(
        Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& in_Object,
        CSLTemplate& in_Transform,
        DummyNodeType in_DummyNodeType,
        const CSIBCVector3D& in_DefaultScale,
        const CSIBCVector3D& in_DefaultTranslation,
        const CSIBCVector3D& in_DefaultRotation,
        CSLTemplate::ERotationOrder in_RotOrder)
    {
        int animationCount = 0;

        // Keep a list of these keys so we can generate one
        // single NiTransformController that has all the keyframes.
        // -- bsowers 8/26/08
        std::vector<NiLinPosKey> posKeys;
        std::vector<NiLinRotKey> rotKeys;
        std::vector<NiLinFloatKey> scaleKeys;

        for (int i = 0; i < TRANSFORM_CURVE_TYPE_COUNT; ++i)
        {
            const TransformCurveType curveType = (TransformCurveType) i;

            float defaultValues[TRANSFORM_AXIS_COUNT];
            GetTransformAnimCurveDefaultValues(
                in_DefaultScale,
                in_DefaultTranslation,
                in_DefaultRotation,
                in_RotOrder,
                curveType,
                defaultValues,
                TRANSFORM_AXIS_COUNT);

            const AnimCurvesDescriptor animDesc =
            {
                TRANSFORM_AXIS_COUNT,
                {
                    gTransformPrintableCurveNames[curveType][0],
                    gTransformPrintableCurveNames[curveType][1],
                    gTransformPrintableCurveNames[curveType][2]
                },
                {
                    gTransformCurveNames[in_DummyNodeType][curveType][0],
                    gTransformCurveNames[in_DummyNodeType][curveType][1],
                    gTransformCurveNames[in_DummyNodeType][curveType][2]
                },
                {
                    defaultValues[0],
                    defaultValues[1],
                    defaultValues[2]
                },
                gTransformMaxDeltas[curveType],
                gTransformCurveFlags[curveType]
            };

            const int transformCount = ConvertTransformCurves(io_Context, in_Model, in_Object, 
                in_Transform, in_RotOrder, curveType, animDesc,
                posKeys, rotKeys, scaleKeys);
            if (transformCount < 0)
                return false;
            else
                animationCount += transformCount;

            const int actionCount = ConvertActionCurves(
                io_Context, in_Model, in_Object, in_RotOrder,
                curveType, animDesc, posKeys, rotKeys, scaleKeys);
            if (actionCount < 0)
                return false;
            else
                animationCount += actionCount;
        }

        

        if (animationCount > 0)
        {
            // Now that all the animation clips have been processed,
            // setup one NiTransformController to represent all the
            // animations and add the keyframe data.
            // -- bsowers 8/26/08
            epg::AnimationMap& animMap = io_Context.GetAnimationMap();
            NiTransformController& controller = animMap.GetTransformAnim(in_Object);
            controller.SetCycleType(NiTimeController::CLAMP);
            NiTransformInterpolator* pInterpolator = NiDynamicCast(
                NiTransformInterpolator, controller.GetInterpolator());
            if (!pInterpolator)
            {
                pInterpolator = NiNew NiTransformInterpolator;
                controller.SetInterpolator(pInterpolator);
            }
            
            if (posKeys.size() > 0)
            {
                NiLinPosKey* pPosKeys = NiNew NiLinPosKey[posKeys.size()];
                for (NiUInt32 ui = 0; ui < posKeys.size(); ui++)
                    pPosKeys[ui] = posKeys[ui];
                pInterpolator->ReplacePosData(pPosKeys, posKeys.size(), NiAnimationKey::LINKEY);

                NiPosKey::FillDerivedValsFunction pDeriv;
                pDeriv = NiPosKey::GetFillDerivedFunction(NiAnimationKey::LINKEY);
                NIASSERT(pDeriv);
                (*pDeriv)(pPosKeys, posKeys.size(), sizeof(NiLinPosKey));
            }
            
            if (rotKeys.size() > 0)
            {
                NiLinRotKey* pRotKeys = NiNew NiLinRotKey[rotKeys.size()];
                for (NiUInt32 ui = 0; ui < rotKeys.size(); ui++)
                    pRotKeys[ui] = rotKeys[ui];
                pInterpolator->ReplaceRotData(pRotKeys, rotKeys.size(), NiAnimationKey::LINKEY);

                NiRotKey::FillDerivedValsFunction pDeriv;
                pDeriv = NiFloatKey::GetFillDerivedFunction(NiAnimationKey::LINKEY);
                NIASSERT(pDeriv);
                (*pDeriv)(pRotKeys, rotKeys.size(), sizeof(NiLinRotKey));
            }

            if (scaleKeys.size() > 0)
            {
                NiLinFloatKey* pScaleKeys = NiNew NiLinFloatKey[scaleKeys.size()];
                for (NiUInt32 ui = 0; ui < scaleKeys.size(); ui++)
                    pScaleKeys[ui] = scaleKeys[ui];
                pInterpolator->ReplaceScaleData(
                    pScaleKeys, scaleKeys.size(), NiAnimationKey::LINKEY);

                NiFloatKey::FillDerivedValsFunction pDeriv;
                pDeriv = NiFloatKey::GetFillDerivedFunction(NiAnimationKey::LINKEY);
                NIASSERT(pDeriv);
                (*pDeriv)(pScaleKeys, scaleKeys.size(), sizeof(NiLinFloatKey));
            }

            pInterpolator->Collapse();
            controller.ResetTimeExtrema();

            in_Object.SetSelectiveUpdate(true);
            in_Object.SetSelectiveUpdateTransforms(true);
            if (NiNode* pParent = in_Object.GetParent())
                pParent->SetSelectiveUpdateTransforms(true);

            io_Context.GetAnimationMap().AddAnimatedObject(in_Object);
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
