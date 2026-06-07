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

#include "CrosswalkGamebryoImpl.h"
#include "CrosswalkGamebryoAnimCurves.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoAnimMap.h"

#include "Model.h"
#include "XSITransform.h"
#include "Transform.h"

#include "NiTransform.h"
#include "NiNode.h"

namespace
{
    //---------------------------------------------------------------------------
    // Invalid pointer returned by the various dummy node creation function
    // to signal an error. We can't simply return NULL because not creating
    // a node is valid.
    //---------------------------------------------------------------------------
    NiNode*     gpInvalidNode   = reinterpret_cast<NiNode*    >(-1L);
    NiAVObject* gpInvalidObject = reinterpret_cast<NiAVObject*>(-1L);

    //---------------------------------------------------------------------------
    // Some useful constants.
    //---------------------------------------------------------------------------
    const CSIBCVector3D noScale(1.0f, 1.0f, 1.0f);
    const CSIBCVector3D noTranslation(0.0f, 0.0f, 0.0f);
    const CSIBCVector3D noRotation(0.0f, 0.0f, 0.0f);

    enum TransformConversionFlags
    {
        NO_TRANSFORM_CONVERSION_FLAG        =  0,
        IGNORE_ANIMATIONS_FLAG              =  1 << 0,
        ALWAYS_CREATE_NODE_FLAG             =  1 << 1,
        USE_AVERAGE_SCALE_FLAG              =  1 << 2,
        ALL_TRANSFORM_CONVERSION_FLAGS      = (1 << 3) - 1
    };

    //---------------------------------------------------------------------------
    // Delete the specified object if it not referenced elsewhere.
    //---------------------------------------------------------------------------
    void DeleteObjectIfUnreferenced(NiNode * pkNode)
    {
        // Note: we use a temporary smart pointer instead of NiDelete
        //       because the object might referenced by another object
        //       and thus cannot be safely deleted. (For example,
        //       if the object is animated it will be referenced
        //       by the animation controller.)
        NiNodePtr spTemp = pkNode;
    }

    //---------------------------------------------------------------------------
    // Verify that the scale is uniform (i.e. X == Y == Z).
    //---------------------------------------------------------------------------
    bool IsUniformScale(const CSIBCVector3D& in_Scale)
    {
        static const float SCALE_COMPARISON_EPSILON = 0.00001f;
        return ::fabs(in_Scale.GetX() - in_Scale.GetY()) < SCALE_COMPARISON_EPSILON
            && ::fabs(in_Scale.GetX() - in_Scale.GetZ()) < SCALE_COMPARISON_EPSILON;
    }

    //---------------------------------------------------------------------------
    // Verify that a transformation does nothing: no translation, rotation, scaling.
    //---------------------------------------------------------------------------
    bool IsIdentity(
        const CSIBCVector3D& in_Scale,
        const CSIBCVector3D& in_Translation,
        const CSIBCVector3D& in_Rotation)
    {
        if (in_Scale.GetX() != 1.0f)
            return false;
        if (in_Scale.GetY() != 1.0f)
            return false;
        if (in_Scale.GetZ() != 1.0f)
            return false;
        if (in_Translation.GetX() != 0.0f)
            return false;
        if (in_Translation.GetY() != 0.0f)
            return false;
        if (in_Translation.GetZ() != 0.0f)
            return false;
        if (in_Rotation.GetX() != 0.0f)
            return false;
        if (in_Rotation.GetY() != 0.0f)
            return false;
        if (in_Rotation.GetZ() != 0.0f)
            return false;
        return true;
    }

    //---------------------------------------------------------------------------
    // Create a node if it's not the identity transformation.
    //---------------------------------------------------------------------------
    NiNode* CreateNodeIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI,
        epg::DummyNodeType in_DummyNodeType,
        const CSIBCVector3D& in_Scale,
        const CSIBCVector3D& in_Translation,
        const CSIBCVector3D& in_Rotation,
        CSLTemplate::ERotationOrder in_RotOrder,
        const TransformConversionFlags in_ConversionFlags)
    {
        bool needed = (in_ConversionFlags & ALWAYS_CREATE_NODE_FLAG) != 0
                    || ! IsIdentity(in_Scale, in_Translation, in_Rotation)
                    || IsTransformAnimated(io_Context, in_Model, transformXSI, in_DummyNodeType);
        if (needed)
        {
            NiNode* pkNode = NiNew NiNode;

            // Convert the transform itself.
            NiTransform gbTransform;

            const CSIBCVector3D* parentScale;
            if (in_ConversionFlags & USE_AVERAGE_SCALE_FLAG)
            {
                parentScale = epg::ConvertAverageScale(
                    io_Context, in_Model, in_Scale, gbTransform.m_fScale);
            }
            else
            {
                parentScale = epg::ConvertScale(
                    io_Context, in_Model, in_Scale, gbTransform.m_fScale);
            }

            epg::ConvertTranslation(in_Translation, gbTransform.m_Translate, parentScale);
            if (!epg::ConvertRotation(
                io_Context, in_Model, in_Rotation, in_RotOrder, gbTransform.m_Rotate))
            {
                DeleteObjectIfUnreferenced(pkNode);
                return gpInvalidNode;
            }

            pkNode->SetLocalTransform(gbTransform);

            // Setup the special node name.
            CSIBCString nodeName = in_Model.Name();
            nodeName.Concat("-");
            nodeName.Concat(GetDummyNodeTypeName(in_DummyNodeType));
            epg::CopyNodeName(nodeName, *pkNode, false);

            // Handle animation of the transform if permitted.
            if ((in_ConversionFlags & IGNORE_ANIMATIONS_FLAG) == 0)
            {
                const bool animConversionResult = ConvertTransformAnimation(
                    io_Context,
                    in_Model,
                    *pkNode,
                    transformXSI,
                    in_DummyNodeType,
                    in_Scale,
                    in_Translation,
                    in_Rotation,
                    in_RotOrder);
                if (!animConversionResult)
                {
                    DeleteObjectIfUnreferenced(pkNode);
                    io_Context.Logf(
                        epg::LOG_ERROR,
                        "Transform animation conversion failed for \"%s\" of \"%s\".",
                        nodeName.GetText(),
                        in_Model.Name().GetText());
                    return gpInvalidNode;
                }
            }

            return pkNode;
        }
        else
        {
            return 0;
        }
    }

    //---------------------------------------------------------------------------
    // Create a node to undo what the pivot did.
    //---------------------------------------------------------------------------
    NiNode* CreateAntiPivotIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI)
    {
        // Note: pivot scaling is not applied by XSI, so do not apply it
        // here either.
        NiNode* antiPivot = CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::ANTI_PIVOT_DUMMY_NODE,
            noScale,
            transformXSI.GetPivotPosition(),
            transformXSI.GetPivotRotation(),
            transformXSI.GetRotationOrder(),
            NO_TRANSFORM_CONVERSION_FLAG);
        if (antiPivot && antiPivot != gpInvalidNode)
        {
            NiTransform transform = antiPivot->GetLocalTransform();
            NiTransform inverted;
            transform.Invert(inverted);
            antiPivot->SetLocalTransform(inverted);
        }
        return antiPivot;
    }

    //---------------------------------------------------------------------------
    // Create a node to hold the actual XSI transform scale and rotation.
    //---------------------------------------------------------------------------
    NiNode* CreateScaleRotateIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI)
    {
        // Note: we don't apply the translation because it was applied
        // before the pivot level since the translation is done on pre-rotated
        // axis.
        //
        // Note: we can apply the scaling right away because Gamebryo
        // positions the scaling matrix closest to the object. In other
        // words, Gamebryo does: Mt * Mr * Ms * Object, where Mt is
        // the translation matrix, Mr the rotation and Ms the scaling.
        // This saves us a node.
        return CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::SCALE_ROTATE_DUMMY_NODE,
            transformXSI.GetScale(),
            noTranslation,
            transformXSI.GetEulerRotation(),
            transformXSI.GetRotationOrder(),
            NO_TRANSFORM_CONVERSION_FLAG);
    }

    //---------------------------------------------------------------------------
    // Create a node to mimick XSI pivot.
    //---------------------------------------------------------------------------
    NiNode* CreatePivotIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI)
    {
        // Note: pivot scaling is not applied by XSI, so do not apply it
        // here either.
        if (transformXSI.GetPivotScale() != noScale)
        {
            io_Context.Logf(
                epg::LOG_WARNING,
                "Pivot scale will be ignored in \"%s\", as per XSI behavior.",
                in_Model.Name().GetText());
        }

        // Note: Out of the translation, rotation and scaling of the object,
        // only the object translation affects the pivot.
        return CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::PIVOT_DUMMY_NODE,
            noScale,
            transformXSI.GetPivotPosition(),
            transformXSI.GetPivotRotation(),
            transformXSI.GetRotationOrder(),
            NO_TRANSFORM_CONVERSION_FLAG);
    }

    //---------------------------------------------------------------------------
    // Create a node to hold the actual XSI transform translation.
    //---------------------------------------------------------------------------
    NiNode* CreateTranslateIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI)
    {
        return CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::TRANSLATE_DUMMY_NODE,
            noScale,
            transformXSI.GetTranslation(),
            noRotation,
            transformXSI.GetRotationOrder(),
            NO_TRANSFORM_CONVERSION_FLAG);
    }

    //---------------------------------------------------------------------------
    // Create a node to mimick the XSI pivot compensation.
    //---------------------------------------------------------------------------
    NiNode* CreatePivotCompIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI)
    {
        if (transformXSI.GetPivotCompScale() != noScale)
        {
            io_Context.Logf(
                epg::LOG_WARNING,
                "Pivot compensation scale will be ignored in \"%s\", as per XSI behavior.",
                in_Model.Name().GetText());
        }

        return CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::PIVOT_COMPENSATION_DUMMY_NODE,
            noScale,
            transformXSI.GetPivotCompPosition(),
            transformXSI.GetPivotCompRotation(),
            transformXSI.GetRotationOrder(),
            NO_TRANSFORM_CONVERSION_FLAG);
    }

    //---------------------------------------------------------------------------
    // Create a node to mimick XSI neutral pose.
    //---------------------------------------------------------------------------
    NiNode* CreateNeutralPoseIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        CSLXSITransform& transformXSI)
    {
        // Note: we do not support non-uniform scale for the base pose
        // due to the difficulty of properly propagating this to all sub-nodes.
        CSIBCVector3D scale = transformXSI.GetNeutralPoseScale();
        if (!IsUniformScale(scale))
        {
            io_Context.Logf(
                epg::LOG_WARNING,
                "Neutral position non-uniform scale will be ignored in \"%s\"."
                " This is a conversion limitation. Average will be used instead",
                in_Model.Name().GetText());
            const float average = (scale.GetX() + scale.GetY() + scale.GetZ()) / 3.0f;
            scale.Set(average, average, average);
        }

        return CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::NEUTRAL_POSE_DUMMY_NODE,
            scale,
            transformXSI.GetNeutralPosePosition(),
            transformXSI.GetNeutralPoseRotation(),
            transformXSI.GetRotationOrder(),
            NO_TRANSFORM_CONVERSION_FLAG);
    }

    //---------------------------------------------------------------------------
    // Collapse the transforms of the given node tree if the nodes are not animated.
    //---------------------------------------------------------------------------
    bool CollapseNonAnimatedNodeTree(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object,
        NiNode* in_pTop,
        NiNode* in_pBottom)
    {
        epg::AnimationMap::AnimatedObjects& animated =
            io_Context.GetAnimationMap().GetAnimatedObjects();

        for (NiNode* node = in_pBottom; node; node = node->GetParent())
            if (animated.count(node) > 0)
                return false;

        NiTransform transform = io_Object.GetLocalTransform();
        for (NiNode* node = in_pBottom; node; node = node->GetParent())
            transform = node->GetLocalTransform() * transform;
        io_Object.SetLocalTransform(transform);

        return true;
    }

    //---------------------------------------------------------------------------
    // Create multiple nodes to handle the complex transformation needed
    // to mimick the XSI transformation.
    //---------------------------------------------------------------------------
    NiAVObject* CreateTransformNodesIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object,
        CSLXSITransform& transformXSI)
    {
        // Note: maybe we could "simply" multiply all the transformation, but maybe not.
        //       The problems is what happen when we will introduce animation into the mix:
        //       if we don't separate each positioning factor and some factors are animated
        //       it might get ugly fast.
        //
        //       Plus there's always the non-uniform scaling problem.
        //
        //       So for now we create separate nodes to keep a minimum of sanity.
        NiNode* top = 0;
        NiNode* bottom = 0;

        // Note: we treat the special node in this order because when we will
        //       handle the non-uniform scales correctly, this is the order
        //       that will be needed.
        //
        // Here's a brief overview of what each matrix represents inside XSI:
        //
        //   - Neutral pose: it's directly under the parent and represent
        //     a convenient offset from the parent so that the object own
        //     transformation can be more "natural" for the end-user.
        //     For example, if the object has an animated rotation going
        //     from 4.78 degrees to 79.78 degrees, it's easier to
        //     visualize as a rotation going from 0 to 75 degrees, starting
        //     with a neutral pose of 4.78 degrees.
        //
        //     As such, the neutral pose is directly under the parent.
        //
        //   - Pivot compensation: this is an automatically calculated value
        //     (calculated by XSI) which allows changing the pivot position
        //     without affecting the object position when doing interactive
        //     changes to the pivot. Thus when the pivot is moved, rotated,
        //     etc, the object stay puts because the pivot compensation changes
        //     to compensate for the changes in pivot position. This is a 
        //     convenience provided by XSI.
        //
        //   - Pivot: this is the point around which the object rotates and
        //     scales The rotation around the pivot uses the axis of the pivot.
        //     The scaling uses the axis of the object but centered at the pivot.
        //
        //   - Transform: the object the rotation and scale is applied at this level.
        //
        // To which we add, due to Gamebryo limitations:
        //
        //   - AntiPivot: this undoes the translation and rotation of the pivot.
        //     Since the pivot can be rotated and translated independently of the
        //     object, we need to undo its effect on the object. This is the node
        //     that does the trick.
        //
        // And we divide the Transform matrix into two: a translate matrix applied
        // before the pivot and a scale+rotate matrix applied after the pivot. This
        // is needed due to the axis used for each operation and which one affect
        // the pivot or not (translation affects the pivot, rotation does not).

        // Node-creating function pointer type.
        typedef NiNode*(*CreateIfNeededFunction)(
            epg::Context & io_Context,
            CSLModel & in_Model,
            CSLXSITransform& transformXSI);

        static const CreateIfNeededFunction functions[] =
        {
            CreateNeutralPoseIfNeeded,
            CreatePivotCompIfNeeded,
            CreateTranslateIfNeeded,
            CreatePivotIfNeeded,
            CreateScaleRotateIfNeeded,
            CreateAntiPivotIfNeeded,
            0
        };

        for (int i = 0; functions[i]; ++i)
        {
            CreateIfNeededFunction func = functions[i];
            NiNode* node = func(io_Context, in_Model, transformXSI);
            if (gpInvalidNode == node)
            {
                DeleteObjectIfUnreferenced(top);
                return gpInvalidObject;
            }
            else if (node)
            {
                if (bottom)
                    bottom->AttachChild(node);
                else
                    top = node;
                bottom = node;
            }
        }

        if (CollapseNonAnimatedNodeTree(io_Context, in_Model, io_Object, top, bottom))
        {
            DeleteObjectIfUnreferenced(top);
            return &io_Object;
        }
        else if (bottom)
        {
            bottom->AttachChild(&io_Object);
            return top;
        }
        else
        {
            return &io_Object;
        }
    }

    //---------------------------------------------------------------------------
    // Create multiple nodes to handle the complex transformation needed
    // to mimick the base pose XSI transformation.
    //---------------------------------------------------------------------------
    NiNode* CreateBasePoseNodeIfNeeded(
        epg::Context& io_Context,
        CSLModel& in_Model,
        NiAVObject& io_Object,
        CSLXSITransform& transformXSI)
    {
        CSLXSIBasePose* pBasePose = in_Model.GetXSIBasePose();
        if (!pBasePose)
            return 0;

        // Note: the base pose is already in world-space, so we
        //       only ever need to create only one node.
        //
        // Note: even if the base pose is the identity we *must*
        //       create it so that the skinning can detect that
        //       there was a base pose and use it instead of the
        //       current position.
        //
        // Note: the base pose is never animated, by definition...
        //       (Or at least we certainly hope so and Gamebryo
        //       doesn't support animating the base pose anyway.)
        //
        // Note: we use the non-base pose rotation order, assuming the
        //       base pose use the same order...
        return CreateNodeIfNeeded(
            io_Context,
            in_Model,
            transformXSI,
            epg::TRANSLATE_DUMMY_NODE,
            pBasePose->GetScale(),
            pBasePose->GetTranslation(),
            pBasePose->GetEulerRotation(),
            transformXSI.GetRotationOrder(),
            ALL_TRANSFORM_CONVERSION_FLAGS);
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert dummy node type to a descriptive string.
    //---------------------------------------------------------------------------

    const char * GetDummyNodeTypeName(DummyNodeType in_Type)
    {
        switch (in_Type)
        {
            case NEUTRAL_POSE_DUMMY_NODE:       return "Neutral-Pose";
            case PIVOT_COMPENSATION_DUMMY_NODE: return "Pivot-Compensation";
            case TRANSLATE_DUMMY_NODE:          return "Translate";
            case PIVOT_DUMMY_NODE:              return "Pivot";
            case SCALE_ROTATE_DUMMY_NODE:       return "Scale-Rotate";
            case ANTI_PIVOT_DUMMY_NODE:         return "Anti-Pivot";
            default:                            return "Unknown-Dummy-Node-Type";
        }
    }

    //---------------------------------------------------------------------------
    // Convert angle in degree to angle in radian. Surprise, surprise.
    //---------------------------------------------------------------------------
    SI_Float DegreeToRadian(SI_Float rot)
    {
        return rot * NI_PI / 180.0f;
    }

    //---------------------------------------------------------------------------
    // Convert rotation supporting the proper rotation order and doing the proper
    // rotation order re-ordering to handle the difference of axis between XSI
    // and Gamebryo.
    //---------------------------------------------------------------------------

    bool ConvertRotation(
        Context& io_Context,
        CSLModel& in_Model,
        const CSIBCVector3D& in_Rotation,
        CSLTemplate::ERotationOrder in_RotOrder,
        NiMatrix3& io_Rotation)
    {
        const float radRotX = DegreeToRadian(-in_Rotation.GetX());
        const float radRotY = DegreeToRadian(-in_Rotation.GetY());
        const float radRotZ = DegreeToRadian(-in_Rotation.GetZ());

        // Note: *** YES *** X and Z rotation order are swapped. I don't have
        //       a mathematical explanation at this time, but it is related
        //       to the fact that XSI uses left-handed axis while Gamebryo
        //       uses right-handed axis and the fact that we don't mirror
        //       Z values when converting objects, meshes, translations, etc.
        switch (in_RotOrder)
        {
            case CSLTemplate::SI_XYZ:
                io_Rotation.FromEulerAnglesZYX(radRotZ, radRotY, radRotX);
                break;
            case CSLTemplate::SI_XZY:
                io_Rotation.FromEulerAnglesZXY(radRotZ, radRotX, radRotY);
                break;
            case CSLTemplate::SI_YXZ:
                io_Rotation.FromEulerAnglesYZX(radRotY, radRotZ, radRotX);
                break;
            case CSLTemplate::SI_YZX:
                io_Rotation.FromEulerAnglesYXZ(radRotY, radRotX, radRotZ);
                break;
            case CSLTemplate::SI_ZXY:
                io_Rotation.FromEulerAnglesXZY(radRotX, radRotZ, radRotY);
                break;
            case CSLTemplate::SI_ZYX:
                io_Rotation.FromEulerAnglesXYZ(radRotX, radRotY, radRotZ);
                break;
            default:
                // Should never happen.
                return io_Context.Logf(
                    LOG_ERROR,
                    "Invalid rotation order found in \"%s\".",
                    in_Model.Name().GetText());
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the translation, applying the non-uniform scaling if any.
    //---------------------------------------------------------------------------
    void ConvertTranslation(
        const CSIBCVector3D& in_Translation,
        NiPoint3& io_Translation,
        const CSIBCVector3D* in_Scale)
    {
        io_Translation.x = in_Translation.GetX();
        io_Translation.y = in_Translation.GetY();
        io_Translation.z = in_Translation.GetZ();
        // If there is an irregular scaling, apply it in the same manner XSI would apply it
        // in Hierarchical (Softimage) scaling mode. That mode applies parent scaling values
        // in the direction of the child axis. It does not apply the current model scaling to
        // its owntranslation though, so that is why we start at the parent scaling level.
        if (in_Scale)
        {
            io_Translation.x *= in_Scale->GetX();
            io_Translation.y *= in_Scale->GetY();
            io_Translation.z *= in_Scale->GetZ();
        }
    }

    //---------------------------------------------------------------------------
    // Convert the scaling, registering the non-uniform scaling if needed.
    //---------------------------------------------------------------------------
    const CSIBCVector3D* ConvertScale(
        Context & io_Context,
        CSLModel & in_Model,
        const CSIBCVector3D & in_Scale,
        float & io_Scale)
    {
        NodeMap& nodeMap = io_Context.GetNodeMap();
        CSLModel* parent = in_Model.Parent();
        const CSIBCVector3D* parentScale = parent ? nodeMap.FindModelScale(*parent) : NULL;
        if (NULL == parentScale && IsUniformScale(in_Scale))
        {
            io_Scale = in_Scale.GetX();
            return NULL;
        }
        else
        {
            io_Scale = 1.0f;
            if (parentScale)
            {
                CSIBCVector3D scale = in_Scale;
                scale.m_fX *= parentScale->m_fX;
                scale.m_fY *= parentScale->m_fY;
                scale.m_fZ *= parentScale->m_fZ;
                nodeMap.SetModelScale(in_Model, scale);
                return parentScale;
            }
            else
            {
                nodeMap.SetModelScale(in_Model, in_Scale);
                return NULL;
            }
        }
    }

    //---------------------------------------------------------------------------
    // Convert the scaling, using the average for non-uniform scaling if needed.
    //---------------------------------------------------------------------------
    const CSIBCVector3D* ConvertAverageScale(
        Context & io_Context,
        CSLModel & in_Model,
        const CSIBCVector3D & in_Scale,
        float & io_Scale)
    {
        if (!IsUniformScale(in_Scale))
        {
            io_Context.Logf(
                LOG_WARNING,
                "Non-uniform scale used in \"%s\", using average instead,"
                " result may not be correct.",
                in_Model.Name().GetText());
        }

        io_Scale = (in_Scale.GetX() + in_Scale.GetY() + in_Scale.GetZ()) / 3.0f;
        return NULL;
    }

    //---------------------------------------------------------------------------
    // Scale points according the the non-uniform scaling provided.
    //---------------------------------------------------------------------------
    void ScalePoints(const CSIBCVector3D& in_Scale, std::vector<float>& io_Vertices)
    {
        SI_Float scales[3] = { in_Scale.GetX(), in_Scale.GetY(), in_Scale.GetZ() };
        for (size_t i = 0; i < io_Vertices.size(); ++i)
        {
            io_Vertices[i] *= scales[i % 3];
        }
    }

    //---------------------------------------------------------------------------
    // Convert the transform of a node to the Gamebryo equivalent.
    //---------------------------------------------------------------------------
    bool ConvertTransform(Context& io_Context, CSLModel& in_Model, NiAVObject& io_Object)
    {
        // Note: We assume only one type of transformation is there at a time.
        NIASSERT((in_Model.XSITransform() ? 1 : 0 ) + (in_Model.Transform() ? 1 : 0) < 2);

        NiAVObject* pTopObject = &io_Object;

        CSLXSITransform * transformXSI = in_Model.XSITransform();
        if (transformXSI)
        {
            // Convert the XSI base pose into a hierarchy of equivalent Gamebryo nodes.
            NiNode* pBasePoseNode = CreateBasePoseNodeIfNeeded(
                io_Context, in_Model, io_Object, *transformXSI);
            if (gpInvalidNode == pBasePoseNode)
                return false;
            else if (pBasePoseNode)
                io_Context.GetNodeMap().SetBasePose(io_Object, *pBasePoseNode);

            // Convert the XSI transform into a hierarchy of equivalent Gamebryo nodes.
            pTopObject = CreateTransformNodesIfNeeded(
                io_Context, in_Model, io_Object, *transformXSI);
            if (gpInvalidNode == pTopObject)
                return false;
        }
        else
        {
            // Note: this transform is much simpler: no pivot, etc.
            CSLTransform* transform = in_Model.Transform();
            if (transform)
            {
                NiTransform gbTransform;
                const CSIBCVector3D* scale = ConvertScale(
                    io_Context, in_Model, transform->GetScale(), gbTransform.m_fScale);
                ConvertTranslation(transform->GetTranslation(), gbTransform.m_Translate, scale);
                if (!ConvertRotation(
                    io_Context,
                    in_Model,
                    transform->GetEulerRotation(),
                    CSLTemplate::SI_XYZ,
                    gbTransform.m_Rotate))
                {
                    return false;
                }
                io_Object.SetLocalTransform(gbTransform);

                // Animation conversion.
                const bool translateAnimResult = ConvertTransformAnimation(
                    io_Context,
                    in_Model,
                    io_Object,
                    *transform,
                    TRANSLATE_DUMMY_NODE,
                    transform->GetScale(),
                    transform->GetTranslation(),
                    transform->GetEulerRotation(),
                    CSLTemplate::SI_XYZ);
                const bool scaleRotAnimResult = ConvertTransformAnimation(
                    io_Context,
                    in_Model,
                    io_Object,
                    *transform,
                    SCALE_ROTATE_DUMMY_NODE,
                    transform->GetScale(),
                    transform->GetTranslation(),
                    transform->GetEulerRotation(),
                    CSLTemplate::SI_XYZ);
                if (!translateAnimResult || !scaleRotAnimResult)
                {
                    return io_Context.Logf(
                        LOG_ERROR,
                        "Transform animation conversion failed for \"%s\".",
                        in_Model.Name().GetText());
                }
            }
            else
            {
                // Must convert dummy scale in case parent object had an irregular scale.
                CSIBCVector3D scale(1.0f, 1.0f, 1.0f);
                float dummy;
                ConvertScale(io_Context, in_Model, scale, dummy);
            }
        }

        return ConnectNodes(io_Context, in_Model, *pTopObject, io_Object);
    }

    //---------------------------------------------------------------------------
}
