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

#ifndef XSI_GAMEBRYO_CONVERTER_ANIMATION_MAP_H
#define XSI_GAMEBRYO_CONVERTER_ANIMATION_MAP_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiString.h"

#include <map>
#include <set>
#include <vector>

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLModel;
class XSIEXPORT CSLActionFCurve;
class XSIEXPORT CSLAction;
class XSIEXPORT CSLActionClip;
class XSIEXPORT CSLStaticValue;

class NIMAIN_ENTRY NiObjectNET;
class NIMAIN_ENTRY NiTransformController;
class NIMAIN_ENTRY NiMesh;
class NIMAIN_ENTRY NiMorphWeightsController;
template <class T> class NiPointer;
typedef NiPointer<NiObjectNET> NiObjectNETPtr;
typedef NiPointer<NiTransformController> NiTransformControllerPtr;
typedef NiPointer<NiMesh> NiMeshPtr;
typedef NiPointer<NiMorphWeightsController> NiMorphWeightsControllerPtr;

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    /// Timeline of an action.
    //---------------------------------------------------------------------------
    struct ActionTimeline
    {
        /// The start time in seconds reserved in the Gamebryo time line.
        float startTime;

        /// The duration in seconds in the Gamebryo time line.
        float duration;

        /// The original start time of the action,
        /// used to offset time value in the curves.
        float xsiStartTime;
    };

    /// Verify that a time falls within the given timeline.
    bool IsInTimeline(const ActionTimeline& in_Timeline, float in_Time);

    //---------------------------------------------------------------------------
    /// Animation action descriptor.
    //---------------------------------------------------------------------------
    class ActionDescriptor
    {
    public:
        /// Creates an empty action descriptor.
        ActionDescriptor();

        /// Retrieve the timeline of this action.
        const ActionTimeline& GetTimeline() const;

        /// Set the timeline of this action.
        void SetTimeline(const ActionTimeline& in_Timeline);

        /** Add an animation curve to the list of animation in this action.
            @param io_Context the conversion context.
            @param in_Action the XSI animation action (found in the XSI animation mixer).
            @param in_Curve the curve to add.
            @return true if successfully added.
        */
        bool AddAnimCurve(Context& io_Context, CSLAction& in_Action, CSLActionFCurve& in_Curve);

        /** Find an animation curve by its animated property name.
            @param in_Name the curve to find.
            @return the animation curve, if found. May return NULL.
        */
        CSLActionFCurve* FindAnimCurve(const char* in_Name) const;

        /** Add a static value to the list of values in this action.
            @param io_Context the conversion context.
            @param in_Action the XSI animation action (found in the XSI animation mixer).
            @param in_Value the value to add.
            @return true if successfully added.
        */
        bool AddStaticValue(Context& io_Context, CSLAction& in_Action, CSLStaticValue& in_Value);

        /** Find a static value by its property name.
            @param in_Name the static value to find.
            @return the static value, if found. May return NULL.
        */
        CSLStaticValue* FindStaticValue(const char* in_Name) const;

    private:
        typedef std::map<NiString, CSLActionFCurve*> Curves;
        typedef std::map<NiString, CSLStaticValue*>  Values;

        ActionTimeline                  m_Timeline;
        Curves                          m_Curves;
        Values                          m_Values;
    };

    //---------------------------------------------------------------------------
    /// Manage animations: animated objects and animation actions.
    ///
    /// Animation actions are indexed by model and by start time.
    /// This is done to support the controller extractor plugin,
    /// which expect animation sequences to be determined by special
    /// extra data attached to node and be delimited by specific time
    /// in the animation timeline.
    //---------------------------------------------------------------------------
    class AnimationMap
    {
    public:
        /// Container of animated objects.
        typedef std::set<NiObjectNET*> AnimatedObjects;

        // Container of a model animation actions indexed by its reserved start time.
        typedef std::map<float,ActionDescriptor> AnimActions;

        /// Create an empty animation map.
        AnimationMap();

        /// resets the animation map.
        void Clear();

        /** Reserves the duration of non-mixer-based animations in the timeline.
            @note *Must* be called before any animation processing is done.
            @param in_Duration the duration of the scene, in seconds.
        */
        void ReserveSceneAnimationTimeline(float in_Duration);

        /// Retrieve the reserved non-mixer animation timeline.
        const ActionTimeline& GetSceneAnimationTimeline() const;

        /** Add an animated object to the map.
            @param in_Object the object to add.
        */
        void AddAnimatedObject(NiObjectNET& in_Object);

        /// Retrieve the list of animated objects.
        AnimatedObjects& GetAnimatedObjects();

        /** Add an animation action clip to the animation timeline.
            @note Reserves its duration into the timeline.
            @note Registers all its animation curves.
            @param io_Context the conversion context.
            @param in_Clip the XSI action clip (found in the XSI animation mixer).
            @return true if successful. Doing nothing is a success.
        */
        bool AddAnimAction(Context& io_Context, CSLActionClip& in_Clip);

        /** Retrieve the timeline of an action clip.
            @param in_Clip the action clip to lookup.
            @return the timeline. Will have zero startTime and duration if not found.
        */
        const ActionTimeline& GetActionTimeline(CSLActionClip& in_Clip) const;

        /** Lookup the list of animation actions of a model.
            @param in_Model the model to lookup.
            @return the list of animation actions, if any. May return NULL.
        */
        AnimActions* FindAnimActions(CSLModel& in_Model);

        /** Retrieve the transform controller associated with the specified object.
            @note May create it if needed.
            @param in_Object the associated object.
        */
        NiTransformController& GetTransformAnim(NiObjectNET& in_Object);

        /** Retrieve the morph controller associated with the specified mesh.
            @note May create it if needed, settings the number of different meshes
                   that will be blended together.
            @param in_Mesh the associated mesh.
            @param in_MeshBlendCount the number of meshes that will
                                     contribute in the morph.
        */
        NiMorphWeightsController& GetMorphAnim(NiMesh& in_Mesh, int in_MeshBlendCount);

    private:
        /// Extract the FCurve of the given XSI action.
        /// @note the XSI start time is in XSI units.
        bool AddActionFCurves(
            epg::Context& io_Context,
            CSLAction& in_Action,
            float in_XSIStartTime,
            float in_Duration);
        /// Extract the static values of an XSI action.
        /// @note the XSI start time is in XSI units.
        bool AddActionStaticValues(
            Context& io_Context,
            CSLAction& in_Action,
            float in_XSIStartTime,
            float in_Duration);

        // Initialize the specified timeline for the specified duration.
        // @note Does *not* increase the current overall end time.
        //       That is done in ConfirmTimeline().
        void ReserveTimeline(
            ActionTimeline& out_Timeline,
            float in_XSIStartTime,
            float in_Duration) const;

        // Confirm that the specified timeline is actually used.
        // Registers the action and updates the overall end time.
        void ConfirmTimeline(CSLAction& in_Action, const ActionTimeline& in_Timeline);

        // Prevent copies.
        AnimationMap(const AnimationMap &);
        void operator=(const AnimationMap &);

        /// Container of action timelines.
        typedef std::map<CSLAction*,ActionTimeline> ActionTimelines;

        /// Container of model animation actions list.
        typedef std::map<CSLModel*,AnimActions> ModelActions;

        /// Container for the transform animation controller of an object.
        typedef std::map<NiObjectNETPtr,NiTransformControllerPtr> TransformAnims;

        /// Container for the morph weight animation controller of a mesh.
        typedef std::map<NiMeshPtr,NiMorphWeightsControllerPtr> MorphAnims;

        float               m_TimelineEnd;
        ActionTimelines     m_ActionTimelines;
        AnimatedObjects     m_AnimatedObjects;
        ModelActions        m_ModelActions;
        TransformAnims      m_TransformAnims;
        MorphAnims          m_MorphAnims;
    };

}

#endif /* XSI_GAMEBRYO_CONVERTER_ANIMATION_MAP_H */
