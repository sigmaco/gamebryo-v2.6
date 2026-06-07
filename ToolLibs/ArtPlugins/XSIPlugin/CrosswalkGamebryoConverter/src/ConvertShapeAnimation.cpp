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
#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoAnimCurves.h"
#include "CrosswalkGamebryoAnimMap.h"
#include "CrosswalkGamebryoMeshHelpers.h"
#include "CrosswalkGamebryoSettings.h"


#include "Model.h"
#include "XSIMesh.h"
#include "XSIShapeAnimation.h"
#include "XSIShape.h"
#include "XSIShapeAnimation.h"
#include "ActionClip.h"
#include "TimeControl.h"
#include "Action.h"

#include "NiMorphWeightsController.h"
#include "NiInterpolator.h"
#include "NiCommonSemantics.h"
#include "NiFloatInterpolator.h"
#include "NiLinFloatKey.h"
#include "NiDataStreamRef.h"
#include "NiDataStream.h"
#include "NiMesh.h"
#include "NiSkinningMeshModifier.h"

namespace
{
    //---------------------------------------------------------------------------
    // The container of semantics of the animation curves of the animated shapes.
    //---------------------------------------------------------------------------
    typedef epg::ShapeAnimSemanticMapper::Semantics Semantics;

    //---------------------------------------------------------------------------
    // Scan all shapes used in the shape animation to extract the semantics
    // that are animated.
    //---------------------------------------------------------------------------
    int VerifyShapeAnimSemantics(
        epg::Context& io_Context,
        CSLXSIShape& in_BaseShape,
        CSLXSIShapeAnimation& in_ShapeAnim,
        const epg::StandardSemanticMapper::Mode in_Mode,
        Semantics& out_stdSemantics,
        Semantics& out_MorphSemantics,
        int& out_AnimatedShapeCount)
    {
        out_AnimatedShapeCount = 0;

        out_stdSemantics.clear();
        out_MorphSemantics.clear();

        SI_Int shapeCount = in_ShapeAnim.GetXSIShapeCount();
        CSLXSIShape** pShapeArray = in_ShapeAnim.XSIShapes();
        for (SI_Int shapeIndex = 0; shapeIndex < shapeCount; ++shapeIndex)
        {
            CSLXSIShape* pMorphedShape = pShapeArray[shapeIndex];
            if (!pMorphedShape)
                continue;

            CSLActionClip* pClip = in_ShapeAnim.GetFirstActionClipByShape(pMorphedShape);
            if (!pClip)
                continue;

            ++out_AnimatedShapeCount;

            epg::ShapeAnimSemanticMapper shapeAnimSemantic(in_Mode);
            epg::ShapeAttributeMap shapeAnimAttrMap(*pMorphedShape, shapeAnimSemantic);

            if (out_MorphSemantics.size())
            {
                if (out_MorphSemantics != shapeAnimSemantic.GetMorphSemanticsFound())
                {
                    return io_Context.Logf(
                        epg::LOG_ERROR,
                        "Shape \"%s\" is animated with multiple source shapes"
                        " with different animation semantics.",
                        in_BaseShape.Name().GetText());
                }
            }
            else
            {
                out_stdSemantics = shapeAnimSemantic.GetStandardSemanticsFound();
                out_MorphSemantics = shapeAnimSemantic.GetMorphSemanticsFound();
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the given shape to the proper shape anim streams.
    //---------------------------------------------------------------------------
    bool ConvertMorphStreams(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_Shape,
        CSLXSITriangleList& in_Triangles,
        const epg::StandardSemanticMapper::Mode in_Mode,
        const Semantics& in_Semantics,
        NiMesh& io_Mesh)
    {
        epg::ValidatedSemanticMapper semanticMapper(in_Mode, in_Semantics);
        epg::ShapeAttributeMap attrMap(in_Shape, semanticMapper);
        return epg::ConvertTriangleListToMesh(
            io_Context, in_Model, in_Shape, attrMap, in_Triangles, io_Mesh);
    }

    //---------------------------------------------------------------------------
    // Create the morph weight interpolator.
    //---------------------------------------------------------------------------
    bool CreateMorphInterpolator(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITriangleList& in_Triangles,
        CSLTemplate& in_AnimatedObject,
        int in_TargetIndex,
        NiMorphWeightsController& in_MorphController,
        float& io_InitialValue)
    {
        epg::AnimCurvesDescriptor shapeWeightAnimDesc =
        {
            1,
            { "weight" },
            { "weight" },
            { io_InitialValue },
            0.0f,
            epg::NO_ANIM_CURVES_FLAG
        };
        epg::AnimCurves curves(
            io_Context,
            in_AnimatedObject,
            in_Model.Name().GetText(),
            shapeWeightAnimDesc);
        NiFloatInterpolatorPtr spInterpolator = ConvertFloatCurves(
            io_Context,
            in_Model.Name().GetText(),
            curves,
            io_InitialValue);
        if (!spInterpolator)
            return false;

        NiString targetName;
        if (0 == in_TargetIndex)
            targetName = "Base";
        else
            targetName.Format("Morph #%d", in_TargetIndex);
        in_MorphController.SetTargetName(in_TargetIndex, (const char*)targetName);
        in_MorphController.SetInterpolator(spInterpolator, in_TargetIndex);

        // Note: this reduces the memory footprint of the interpolator.
        spInterpolator->Collapse();

        return true;
    }

    //---------------------------------------------------------------------------
    // Determine the semantic mode to be used depending on the presence
    // of a skinning modifier on the mesh.
    //
    // Note: we can't infer the presence of skinning byother means since
    //       skinning might be disabled or its conversion might have failed.
    //---------------------------------------------------------------------------
    epg::StandardSemanticMapper::Mode GetSemanticMode(const NiMesh& in_Mesh)
    {
        if (in_Mesh.GetModifierByType(&NiSkinningMeshModifier::ms_RTTI))
            return epg::StandardSemanticMapper::SKINNING_MODE;
        else
            return epg::StandardSemanticMapper::STANDARD_MODE;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Conversion of node transform animation.
    //---------------------------------------------------------------------------
    bool ConvertShapeAnimation(
        Context& io_Context,
        CSLModel& in_Model,
        CSLXSIShape& in_BaseShape,
        CSLXSIShapeAnimation& in_ShapeAnim,
        CSLXSITriangleList& in_Triangles,
        NiMesh& io_Mesh)
    {
        if (!io_Context.GetSettings().GetExportShapeAnimations())
            return true;

        const StandardSemanticMapper::Mode semanticMode = GetSemanticMode(io_Mesh);

        // Verify that all shapes contributing the the animation have the
        // same set of attributes.
        Semantics stdSemantics;
        Semantics morphSemantics;
        int shapeSemanticCount = -1;
        if (!VerifyShapeAnimSemantics(
            io_Context,
            in_BaseShape,
            in_ShapeAnim,
            semanticMode,
            stdSemantics,
            morphSemantics,
            shapeSemanticCount))
        {
            return false;
        }

        const int targetCount = 1 + shapeSemanticCount;
        if (targetCount == 1)
            return true;
        if (morphSemantics.size() < 0)
            return true;

        // Retrieve or create the morph controller.
        AnimationMap& animMap = io_Context.GetAnimationMap();
        NiMorphWeightsController& morphController = animMap.GetMorphAnim(io_Mesh, targetCount);

        // The list of initial values for the shape weights.
        // Will be used to create the morph weight stream.
        std::vector<float> initialWeights;
        int currentTarget = 0;

        // Convert the base shape to streams with morph semantics.
        // The base shape *must* be the first stream, as per Gamebryo API requirements.
        if (!ConvertMorphStreams(
            io_Context,
            in_Model,
            in_BaseShape,
            in_Triangles,
            semanticMode,
            morphSemantics,
            io_Mesh))
        {
            return false;
        }

        // Set the base shape interpolator.
        float initialWeight = 1.0f;
        if (!CreateMorphInterpolator(
            io_Context,
            in_Model,
            in_Triangles,
            in_BaseShape,
            currentTarget++,
            morphController,
            initialWeight))
        {
            return false;
        }

        // Set the initial weight for the base shape.
        initialWeights.push_back(initialWeight);

        // Now add each shape contributing the to morph animation.
        SI_Int shapeCount = in_ShapeAnim.GetXSIShapeCount();
        CSLXSIShape** pShapeArray = in_ShapeAnim.XSIShapes();
        for (SI_Int shapeIndex = 0; shapeIndex < shapeCount; ++shapeIndex)
        {
            CSLXSIShape* pMorphedShape = pShapeArray[shapeIndex];
            if (!pMorphedShape)
                continue;

            // Note: we ignore the fill action and active flags found in the clip
            //       because the Corsswalk exporter doesn't set them properly!!!
            //       Oh well... We also used the animation curve timing instead
            //       the action clip since we gain nothing from using the clip.
            CSLActionClip* pClip = in_ShapeAnim.GetFirstActionClipByShape(pMorphedShape);
            if (!pClip)
                continue;

            // Convert the morphed shape to mesh streams with morph semantics.
            if (!ConvertMorphStreams(
                io_Context,
                in_Model,
                *pMorphedShape,
                in_Triangles,
                semanticMode,
                morphSemantics,
                io_Mesh))
            {
                return false;
            }

            // Set the morph shape weight interpolator.
            initialWeight = pClip->GetWeight();
            if (!CreateMorphInterpolator(
                io_Context,
                in_Model,
                in_Triangles,
                *pClip,
                currentTarget++,
                morphController,
                initialWeight))
            {
                return false;
            }

            // Set correct initial weight based on first value of the curve.
            initialWeights.push_back(initialWeight);
        }

        NIASSERT(currentTarget == targetCount);

        // Create the weights stream, which will be animated by the morph controller, see below.
        const int selectedIndex = AddStreamToMesh(
            io_Context,
            io_Mesh,
            NiCommonSemantics::MORPHWEIGHTS(),
            -1,
            1,
            initialWeights);
        if (selectedIndex < 0)
            return false;

        // Create the morph modifier that describes the morph.
        // Make sure it use the relative mode so that morph are relative
        // to a base shape.
        NiMorphMeshModifier* pMorphModifier = NiNew NiMorphMeshModifier(targetCount);
        pMorphModifier->SetRelativeTargets(true);

        // Add the semantics that are being morphed and adjust their stream access flags.
        typedef Semantics::iterator iter;
        int semIndex = 0;
        for (iter pos = stdSemantics.begin(); pos != stdSemantics.end(); ++pos, ++semIndex)
        {
            const NiFixedString & semantic = *pos;
            pMorphModifier->AddMorphedElement(semantic, semIndex);
            if (!SetStreamFlags(
                io_Context,
                in_Model,
                io_Mesh,
                semantic,
                NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
                NiDataStream::USAGE_VERTEX))
            {
                return false;
            }
        }

        // Update the morph weight stream access flags.
        if (!SetStreamFlags(
            io_Context,
            in_Model,
            io_Mesh,
            NiCommonSemantics::MORPHWEIGHTS(),
            NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStream::USAGE_USER))
        {
            return false;
        }

        // Update start end end time of the animation.
        morphController.ResetTimeExtrema();

        // Attach modifier and controller.
        io_Mesh.AddModifier(pMorphModifier);
        morphController.SetTarget(&io_Mesh);

        // Set the animation update flags.
        io_Mesh.SetSelectiveUpdate(true);
        io_Mesh.SetSelectiveUpdateTransforms(true);
        io_Mesh.SetSelectiveUpdateRigid(false);

        animMap.AddAnimatedObject(io_Mesh);

        // Update the object bounding box in case the morph makes it bigger.
        pMorphModifier->CalculateMorphBound(&io_Mesh);

        return true;
    }

    //---------------------------------------------------------------------------
}
