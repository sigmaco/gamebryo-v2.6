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


#ifndef NIPHYSXSEQUENCEMANAGER_H
#define NIPHYSXSEQUENCEMANAGER_H

#include "NiPhysXLibType.h"

#include <NiSystem.h>
#include <NiMain.h>
#include <NiAnimation.h>

#include "NiPhysXDynamicSrc.h"
#include "NiPhysXProp.h"
#include "NiPhysXTransformDest.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
// conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4244)
// conversion from 'type' to 'type', signed/unsigned mismatch
#pragma warning(disable: 4245)
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXSequenceManager objects interface between an NiControllerManager
    object and a NiPhysXProp object to automatically manage the activation
    and de-activation of NiPhysX and PhysX objects as animations are activated
    and de-activated in the Gamebryo Animation System.
    
    This class is intended for situations in which a character or object is
    sometimes key-frame animated and sometimes physically active, such as a
    rag-doll. The object is set up with a NiControllerManager object and an
    NiPhysXProp object. Connections, in the form of NiPhysXDynamicSrc objects,
    are established between all the physics objects in the prop and all the
    scene graph objects in sequences controlled by the manager. These are
    used to feed animated state back to PhysX. An application then explicitly
    identifies some NiSequenceData objects as "physics controlled", and
    NiPhysXPoseBufferDest objects are created to tie the PhysX actors to 
    pose buffer data items for the sequence. This allows PhysX to provide 
    input to animation blends, or to drive the character outright.

    When the character is key-frame animated, the NiPhysXSequenceManager
    object activates the NiPhysXDynamicSrc objects to drive physics with the
    animation. When one of the "physics" sequences is activated, the
    NiPhysXPoseBufferDest objects are activated to provide the physics
    input and the NiPhysXDynamicSrc object for that scene graph node is
    disabled. The PhysX actors are automatically switched back to kinematic
    control when they are not driving any physical sequences.

    The result of all this is efficient and stable mixing of physics control
    with animated sequences, using the standard Gamebryo animation system.

    @note Cloning: NiPhysXSequenceManager objects cannot be cloned - it is
        cheaper to create a new one from scratch. To clone an entire character
        and its physics, start by cloning the NiActorManager object or
        NiControllerManager object which clones the character's scene graph.
        Then, clone the NiPhysXProp object representing the character's physics
        using the same NiCloningProcess object as was used for the scene graph.
        This must be done before the original NiPhysXProp object is associated
        with any NiPhysXSequenceManager object. If you clone a prop object that
        is already associated with a NiPhysXSequenceManager object you will
        also clone all of the sources and destinations added by that manager,
        which you must not do. Finally, create a new NiPhysXSequenceManager
        object and call SetManager on it.
*/
class NIPHYSX_ENTRY NiPhysXSequenceManager
    : public NiObject, public NiSequenceData::ActivationCallback
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    /// @endcond

public:
    /// Constructor.
    ///
    /// Sets the controller manager and NiPhysXProp object that 
    /// we are controlling to null.
    NiPhysXSequenceManager();
    
    /// Destructor.
    ///
    /// Cleans up all data structures.
    virtual ~NiPhysXSequenceManager();
    
    /**
        Set the controller manager and prop we are working with.
        
        This function adds sources for all of the actors in the prop that
        are controlled by a sequence in the manager. It returns true if the
        operations all succeeded, false otherwise. It will return false if
        a manager has already been specified.
    */
    bool SetManager(NiControllerManager* pkCM, NiPhysXProp* pkProp);
    
    /// Return the character controlled by this NiPhysX sequence manager.
    NiControllerManager* GetControllerManager();

    /// Return the prop controlled by this NiPhysX sequence manager.
    NiPhysXProp* GetProp();
    
    /**
        Add a sequence that will be key-frame controlled, or later converted
        to physical.
        
        This function is intended for use when adding additional sequences to
        an actor manager, so that those sequences can have physics
        appropriately attached. SetManager must have already been called
        before this function. This function does not need to be called if
        the sequence was managed by the NiControllerManager at the time it
        was passed to SetManager.
    */
    bool AddSequence(NiSequenceData* pkAnimSeqData);
    
    /**
        Make a sequence physical.
        
        This sets up the sequence for control by PhysX when the sequence 
        is activated. Returns true if the operation succeeded. Failure 
        indicates either that the sequence is already physical, is not 
        from the controller manager we are working with, or an unexpected 
        set-up of the PhysX components or the animation components.
    */
    bool MakeSequencePhysical(NiSequenceData* pkAnimSeqData);

    /**
        Reverse the effects of MakeSequencePhysical.
        
        This removes the sequence from the control of PhysX when 
        the sequence is activated. The sequence effectively reverts
        back to the control of the animation system. Returns true 
        if the operation succeeded. Failure indicates the sequence 
        is not physical on the character we are working with.
    */
    bool RevertPhysicalSequence(NiSequenceData* pkAnimSeqData);

    /// @cond EMERGENT_INTERNAL

    // Objects to hold one controller's data. An array of actor sources
    // and destinations, and activity and type information.
    class NIPHYSX_ENTRY SequenceActorData : public NiMemObject
    {
    public:
        SequenceActorData();
        ~SequenceActorData();

        NiPhysXRigidBodyDest* GetDestination() const;
        void SetDestination(NiPhysXRigidBodyDest* pkDestination);
        NiPhysXDynamicSrc* GetSource() const;
        void SetSource(NiPhysXDynamicSrc* pkSource);

    protected:
        NiPhysXRigidBodyDestPtr m_spDestination;
        NiPhysXDynamicSrcPtr m_spSource;
    };
    typedef NiTPrimitiveArray<SequenceActorData*> ActorDataArray;

    class NIPHYSX_ENTRY SequenceData : public NiMemObject
    {
    public:
        SequenceData();
        ~SequenceData();

        bool IsActive() const;
        ActorDataArray& GetActorDataArray();
        NiPhysXRigidBodyDest* GetAccumDestination() const;
        void SetAccumDestination(NiPhysXRigidBodyDest* pkAccumDest);
        NiControllerSequence* GetActiveAnimSequence() const;
        void SetActiveAnimSequence(NiControllerSequence* pkActiveAnimSeq);
        void UpdateDestPoseBufferDataPointers();

    protected:
        // Destinations are strictly per-sequence
        ActorDataArray m_kActorData;
        NiPhysXRigidBodyDest* m_pkAccumDest;

        // Strictly per-sequence
        NiControllerSequence* m_pkActiveAnimSeq;
        NiUInt32 m_uiTotalPoseBufferItems;
    };

    /// @endcond

protected:
    // Clean out all the maps.
    void CleanMaps();
    
    // Find all the objects controlled by transform destinations.
    // Results are put in m_kOriginalDestMap
    void FindTransformDests(NiPhysXProp* pkProp);

    // Activation change callback
    void ActivationChanged(NiControllerSequence* pkAnimSeq,
        NiAnimState eState);

    // Deactivate/Activate sources. Track the active animation.
    bool SetActivity(SequenceData* pkSeqData, 
        NiControllerSequence* pkActiveAnimSeq);

    // The character controlled by this NiPhysX sequence manager.
    NiControllerManagerPtr m_spControllerManager;

    // The PhysX Prop that is managing the PhysX objects.
    NiPhysXPropPtr m_spProp;
    
    // A map of all the destinations in the prop
    NiTMap<NiAVObject*, SequenceActorData*> m_kOriginalDestMap;
    
    // A map, indexed by sequence, of the sequence information. This includes
    // the destinations and sources used for the sequence, and whether it
    // is physical or not.
    NiTMap<NiSequenceData*, SequenceData*> m_kSequences;
    
    // A map, keyed on actors, containing the number of active physics
    // sequences for that actor. Actors with no active physical sequences
    // are set to kinematic.
    NiTMap<NxActor*, NiUInt16> m_kActorActiveCount;
};

NiSmartPointer(NiPhysXSequenceManager);

#include "NiPhysXSequenceManager.inl"

#endif // #ifdef NIPHYSXSEQUENCEMANAGER_H

