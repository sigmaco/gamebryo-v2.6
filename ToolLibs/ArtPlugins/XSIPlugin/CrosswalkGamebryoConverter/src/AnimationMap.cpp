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

#include "CrosswalkGamebryoAnimMap.h"
#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoSceneInfo.h"

#include "Action.h"
#include "ActionClip.h"
#include "FCurve.h"
#include "ActionFCurve.h"
#include "StaticValue.h"
#include "Scene.h"

#include "NiTransformController.h"
#include "NiMorphWeightsController.h"
#include "NiObjectNET.h"
#include "NiMesh.h"

namespace
{
    //---------------------------------------------------------------------------
    // Parse the XSI parameter path to extract the object name and the animated
    // parameter name.
    //---------------------------------------------------------------------------
    bool ParseParameterXSIPath(
        const char* in_XSIPath,
        NiString& out_ObjectName,
        NiString& out_ParamName)
    {
        if (!in_XSIPath || !in_XSIPath[0])
            return false;

        out_ObjectName = in_XSIPath;
        unsigned int pos = out_ObjectName.Find('.');
        if (NiString::INVALID_INDEX == pos)
            return false;
        else
            out_ObjectName = out_ObjectName.Left(pos);

        out_ParamName = in_XSIPath;
        pos = out_ParamName.FindReverse('.');
        if (NiString::INVALID_INDEX == pos)
            return false;
        else
            out_ParamName = out_ParamName.GetSubstring(pos+1, out_ParamName.Length());

        return true;
    }

    //---------------------------------------------------------------------------
    // Find the model corresponding to an action reference path.
    //---------------------------------------------------------------------------
    CSLModel* FindXSIPathModel(
        epg::Context& io_Context,
        CSLAction& in_Action,
        const char* in_XSIPath)
    {
        NiString objectName;
        NiString paramName;
        if (!ParseParameterXSIPath(in_XSIPath, objectName, paramName))
        {
            io_Context.Logf(
                epg::LOG_ERROR,
                "Invalid object reference (\"%s\") in action \"%s\".",
                in_XSIPath,
                in_Action.Name().GetText());
            return 0;
        }

        CSLScene& scene = io_Context.GetScene();
        const char* name = objectName;
        // Note: Crosswalk is not const-correct most of the time... oh well.
        return scene.FindModelRecursively((SI_Char*)name, scene.Root());
    }

    //---------------------------------------------------------------------------
    // The amoun tof time we put in between consecutive clips to avoid key
    // overlap at either ends.
    //---------------------------------------------------------------------------
    const float gs_ClipSeparationGap = 1.0f;

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Action animation curve helpers.
    //---------------------------------------------------------------------------
    CSLModel* FindActionCurveReference(
        Context& io_Context,
        CSLAction& in_Action,
        CSLActionFCurve& in_Curve)
    {
        const char* xsiPath = in_Curve.GetParameterXSIPath();
        return FindXSIPathModel(io_Context, in_Action, xsiPath);
    }

    //---------------------------------------------------------------------------
    // Action static value helpers.
    //---------------------------------------------------------------------------
    CSLModel* FindActionStaticValueReference(
        Context& io_Context,
        CSLAction& in_Action,
        CSLStaticValue& in_Value)
    {
        const char* xsiPath = in_Value.GetParameterName();
        return FindXSIPathModel(io_Context, in_Action, xsiPath);
    }

    //---------------------------------------------------------------------------
    /// Verify that a time falls within the given timeline.
    //---------------------------------------------------------------------------
    bool IsInTimeline(const ActionTimeline& in_Timeline, float in_Time)
    {
        #define IN_TIMELINE_EPSILON 0.00001

        return (in_Timeline.startTime - IN_TIMELINE_EPSILON) < in_Time
            && (in_Timeline.startTime + in_Timeline.duration + IN_TIMELINE_EPSILON) > in_Time;
    }

    //---------------------------------------------------------------------------
    // Creates an empty action descriptor.
    //---------------------------------------------------------------------------
    ActionDescriptor::ActionDescriptor()
    {
        m_Timeline.startTime    = 0.0f;
        m_Timeline.duration     = 0.0f;
        m_Timeline.xsiStartTime = 0.0f;
    }

    //---------------------------------------------------------------------------
    // Retrieve the timeline of this action.
    //---------------------------------------------------------------------------
    const ActionTimeline& ActionDescriptor::GetTimeline() const
    {
        return m_Timeline;
    }

    //---------------------------------------------------------------------------
    // Set the timeline of this action.
    //---------------------------------------------------------------------------
    void ActionDescriptor::SetTimeline(const ActionTimeline& in_Timeline)
    {
        m_Timeline = in_Timeline;
    }

    //---------------------------------------------------------------------------
    // Add an animation curve to the list of animation in this action.
    //---------------------------------------------------------------------------
    bool ActionDescriptor::AddAnimCurve(
        Context& io_Context,
        CSLAction& in_Action,
        CSLActionFCurve& in_Curve)
    {
        const char* xsiPath = in_Curve.GetParameterXSIPath();
        NiString objectName;
        NiString paramName;
        if (!ParseParameterXSIPath(xsiPath, objectName, paramName))
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid animation curve reference (\"%s\") in action \"%s\".",
                xsiPath,
                in_Action.Name().GetText());
        }

        if (paramName.Length() > 0)
            m_Curves[paramName] = &in_Curve;

        return true;
    }

    //---------------------------------------------------------------------------
    // Find an animation curve by its animated property name.
    //---------------------------------------------------------------------------
    CSLActionFCurve* ActionDescriptor::FindAnimCurve(const char* in_Name) const
    {
        if (!in_Name)
            return 0;

        typedef Curves::const_iterator iter;
        const iter pos = m_Curves.find(in_Name);
        if (pos != m_Curves.end())
            return pos->second;
        else
            return 0;
    }

    //---------------------------------------------------------------------------
    // Add a static value to the list of values in this action.
    //---------------------------------------------------------------------------
    bool ActionDescriptor::AddStaticValue(
        Context& io_Context,
        CSLAction& in_Action,
        CSLStaticValue& in_Value)
    {
        const char* xsiPath = in_Value.GetParameterName();
        NiString objectName;
        NiString paramName;
        if (!ParseParameterXSIPath(xsiPath, objectName, paramName))
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Invalid static value reference (\"%s\") in action \"%s\".",
                xsiPath,
                in_Action.Name().GetText());
        }

        if (paramName.Length() > 0)
            m_Values[paramName] = &in_Value;

        return true;
    }

    //---------------------------------------------------------------------------
    // Find a static value by its property name.
    //---------------------------------------------------------------------------
    CSLStaticValue* ActionDescriptor::FindStaticValue(const char* in_Name) const
    {
        if (!in_Name)
            return 0;

        typedef Values::const_iterator iter;
        const iter pos = m_Values.find(in_Name);
        if (pos != m_Values.end())
            return pos->second;
        else
            return 0;
    }

    //---------------------------------------------------------------------------
    // Create an empty animation map.
    //---------------------------------------------------------------------------
    AnimationMap::AnimationMap()
        : m_TimelineEnd(0.0f)
    {
        Clear();
    }

    //---------------------------------------------------------------------------
    // resets the animation map.
    //---------------------------------------------------------------------------
    void AnimationMap::Clear()
    {
        m_TimelineEnd = 0.0f;
        m_ActionTimelines.clear();
        m_AnimatedObjects.clear();
        m_ModelActions.clear();
        m_TransformAnims.clear();
        m_MorphAnims.clear();

        ActionTimeline timeline;
        timeline.startTime = 0.0f;
        timeline.duration  = 0.0f;
        timeline.xsiStartTime = 0.0f;
        m_ActionTimelines[0] = timeline;
    }

    //---------------------------------------------------------------------------
    // Reserves the duration of non-mixer-based animations in the timeline.
    // *Must* be called before any animation processing is done.
    //---------------------------------------------------------------------------
    void AnimationMap::ReserveSceneAnimationTimeline(float in_Duration)
    {
        NIASSERT(0.0f == m_TimelineEnd);
        m_TimelineEnd = in_Duration;

        // We use the special NULL entry in the action timeline for teh scene.
        ActionTimeline timeline;
        timeline.startTime = 0.0f;
        timeline.duration  = in_Duration;
        timeline.xsiStartTime = 0.0f;
        m_ActionTimelines[0] = timeline;
    }

    //---------------------------------------------------------------------------
    // Retrieve the reserved non-mixer animation timeline.
    //---------------------------------------------------------------------------
    const ActionTimeline& AnimationMap::GetSceneAnimationTimeline() const
    {
        NIASSERT(m_ActionTimelines.find(0) != m_ActionTimelines.end());

        static const ActionTimeline invalidTimeline;
        typedef ActionTimelines::const_iterator iter;
        const iter pos = m_ActionTimelines.find(0);
        if (pos != m_ActionTimelines.end())
            return pos->second;
        else
            return invalidTimeline;
    }

    //---------------------------------------------------------------------------
    // Add an animated object to the map.
    //---------------------------------------------------------------------------
    void AnimationMap::AddAnimatedObject(NiObjectNET& in_Object)
    {
        m_AnimatedObjects.insert(&in_Object);
    }


    //---------------------------------------------------------------------------
    // Retrieve the list of animated objects.
    //---------------------------------------------------------------------------
    AnimationMap::AnimatedObjects& AnimationMap::GetAnimatedObjects()
    {
        return m_AnimatedObjects;
    }

    //---------------------------------------------------------------------------
    // Add an animation action to the animation timeline.
    // Registers all its animation curves.
    //---------------------------------------------------------------------------
    bool AnimationMap::AddAnimAction(Context& io_Context, CSLActionClip& in_Clip)
    {
        CSLAction* pAction = in_Clip.GetReference();
        if (!pAction)
            return true;

        const epg::SceneInfo& sceneInfo = io_Context.GetSceneInfo();
        // Note: the XSI start time comes from the action because it's the value
        //       used to "reset" the action keyframe time to zero so that they
        //       can be re-purpose and offset to their proper place in the Gamebryo
        //       timeline.
        const float xsiStartTime = pAction->GetStartTime();
        // Note: the duration comes from the action clip because the action clip
        //       can crop the underlying action and make it shorter than the
        //       action source.
        const float duration = sceneInfo.ConvertTime(in_Clip.GetDuration());

        switch (pAction->GetActionType())
        {
            case CSLAction::SI_AT_FCURVE:
                return AddActionFCurves(io_Context, *pAction, xsiStartTime, duration);
                break;

            case CSLAction::SI_AT_STATICVALUE:
                return AddActionStaticValues(io_Context, *pAction, xsiStartTime, duration);
                break;

            default:
                return io_Context.Logf(
                    LOG_WARNING,
                    "Animation action \"%s\" of unsupported type will be ignored.",
                    pAction->Name().GetText());
                break;
        }
    }

    //---------------------------------------------------------------------------
    // Extract the FCurve of the given XSI action.
    //---------------------------------------------------------------------------
    bool AnimationMap::AddActionFCurves(
        Context& io_Context,
        CSLAction& in_Action,
        float in_XSIStartTime,
        float in_Duration)
    {
        const SI_Int count = in_Action.GetAnimationCount();
        CSLActionFCurve** pCurveArray = in_Action.GetAnimationList();
        if (!count || !pCurveArray)
            return true;

        ActionTimeline timeline;
        ReserveTimeline(timeline, in_XSIStartTime, in_Duration);

        // Avoid extending the timeline if no action gets converted.
        int actionCount = 0;

        for (SI_Int i = 0; i < count; ++i)
        {
            CSLActionFCurve* pCurve = pCurveArray[i];
            if (!pCurve)
                continue;

            CSLModel* pAnimModel = FindActionCurveReference(io_Context, in_Action, *pCurve);
            if (!pAnimModel)
                continue;

            AnimActions& actions = m_ModelActions[pAnimModel];
            ActionDescriptor& actionDesc = actions[timeline.startTime];

            actionDesc.SetTimeline(timeline);
            if (!actionDesc.AddAnimCurve(io_Context, in_Action, *pCurve))
                return false;

            ++actionCount;
        }

        if (actionCount > 0)
            ConfirmTimeline(in_Action, timeline);

        return true;
    }

    //---------------------------------------------------------------------------
    // Extract the static values of an XSI action.
    //---------------------------------------------------------------------------
    bool AnimationMap::AddActionStaticValues(
        Context& io_Context,
        CSLAction& in_Action,
        float in_XSIStartTime,
        float in_Duration)
    {
        const SI_Int count = in_Action.GetStaticValueCount();
        CSLStaticValue** pValueArray = in_Action.GetStaticValueList();
        if (!count || !pValueArray)
            return true;

        ActionTimeline timeline;
        ReserveTimeline(timeline, in_XSIStartTime, in_Duration);

        // Avoid extending the timeline if no action gets converted.
        int actionCount = 0;

        for (SI_Int i = 0; i < count; ++i)
        {
            CSLStaticValue* pValue = pValueArray[i];
            if (!pValue)
                continue;

            CSLModel* pAnimModel = FindActionStaticValueReference(io_Context, in_Action, *pValue);
            if (!pAnimModel)
                continue;

            AnimActions& actions = m_ModelActions[pAnimModel];
            ActionDescriptor& actionDesc = actions[timeline.startTime];

            actionDesc.SetTimeline(timeline);
            if (!actionDesc.AddStaticValue(io_Context, in_Action, *pValue))
                return false;

            ++actionCount;
        }

        if (actionCount > 0)
            ConfirmTimeline(in_Action, timeline);

        return true;
    }

    //---------------------------------------------------------------------------
    // Timeline management helpers.
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    // Initialize the specified timeline for the specified duration.
    // @note Does *not* increase the current overall end time.
    //       That is done in ConfirmTimeline().
    //---------------------------------------------------------------------------
    void AnimationMap::ReserveTimeline(
        ActionTimeline& out_Timeline,
        float in_XSIStartTime,
        float in_Duration) const
    {
        out_Timeline.startTime    = m_TimelineEnd + gs_ClipSeparationGap;
        out_Timeline.duration     = in_Duration;
        out_Timeline.xsiStartTime = in_XSIStartTime;
    }

    //---------------------------------------------------------------------------
    // Confirm that the specified timeline is actually used.
    // Registers the action and updates the overall end time.
    //---------------------------------------------------------------------------
    void AnimationMap::ConfirmTimeline(CSLAction& in_Action, const ActionTimeline& in_Timeline)
    {
        const float newEnd = in_Timeline.startTime + in_Timeline.duration;
        if (newEnd > m_TimelineEnd)
            m_TimelineEnd = newEnd;
        m_ActionTimelines[&in_Action] = in_Timeline;
    }

    //---------------------------------------------------------------------------
    // Retrieve the timeline of an action.
    //---------------------------------------------------------------------------
    const ActionTimeline& AnimationMap::GetActionTimeline(CSLActionClip& in_Clip) const
    {
        static const ActionTimeline empty;

        CSLAction* pAction = in_Clip.GetReference();
        if (!pAction)
            return empty;

        typedef ActionTimelines::const_iterator iter;
        const iter pos = m_ActionTimelines.find(pAction);
        if (pos != m_ActionTimelines.end())
            return pos->second;
        else
            return empty;
    }

    //---------------------------------------------------------------------------
    // Lookup the list of animation actions of a model.
    //---------------------------------------------------------------------------
    AnimationMap::AnimActions* AnimationMap::FindAnimActions(CSLModel& in_Model)
    {
        typedef ModelActions::iterator iter;
        const iter pos = m_ModelActions.find(&in_Model);
        if (pos != m_ModelActions.end())
            return &(pos->second);
        else
            return 0;
    }

    //---------------------------------------------------------------------------
    // Retrieve the transform controller associated with the specified object.
    // May create it if needed.
    //---------------------------------------------------------------------------
    NiTransformController& AnimationMap::GetTransformAnim(NiObjectNET& in_Object)
    {
        NiTransformControllerPtr& spTransformCtrl = m_TransformAnims[&in_Object];
        if (!spTransformCtrl)
        {
            spTransformCtrl = NiNew NiTransformController;
            spTransformCtrl->SetTarget(&in_Object);
        }
        
        return *spTransformCtrl;
    }

    //---------------------------------------------------------------------------
    // Retrieve the morph controller associated with the specified mesh.
    // May create it if needed, settings the number of different meshes
    // that will be blended together.
    //---------------------------------------------------------------------------
    NiMorphWeightsController& AnimationMap::GetMorphAnim(NiMesh& in_Mesh, int in_MeshBlendCount)
    {
        NiMorphWeightsControllerPtr& spMorphCtrl = m_MorphAnims[&in_Mesh];
        if (!spMorphCtrl)
        {
            spMorphCtrl = NiNew NiMorphWeightsController(in_MeshBlendCount);
        }
        else
        {
            NIASSERT(spMorphCtrl->GetNumTargets() == in_MeshBlendCount);
        }
        
        return *spMorphCtrl;
    }

    //---------------------------------------------------------------------------
}
