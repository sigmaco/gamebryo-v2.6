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

#ifndef NIPHYSXSCENE_H
#define NIPHYSXSCENE_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>
#include <NiMeshLib.h>

#include "NiPhysXDest.h"
#include "NiPhysXProp.h"
#include "NiPhysXSrc.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', 
                               // possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', 
                               // signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

class NiPhysXManager;

NiSmartPointer(NiPhysXSceneDesc);
NiSmartPointer(NiPhysXScene);

/**
    Each NiPhysXScene object manages a NxScene object and the Gamebryo
    objects that interact with it.
*/
class NIPHYSX_ENTRY NiPhysXScene : public NiObjectNET
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXScene);

    /// @endcond

public:
    /**
        Constructor.
      
        The optional workflow manager argument is used by mesh modifiers
        associated with the scene. A locally owned workflow manager
        is created if one is not given here.  
    */
    NiPhysXScene(NiSPWorkflowManager* pkWorkflowManager = 0);
    
    /**
        Destructor.
        
        In addition to freeing all data associated with the scene, this
        function also removes from the NxScene object any contents associated
        with this scene.
    */
    ~NiPhysXScene();


    /**
        @name NxScene Management 

        There are two ways to set the NxScene object for this scene.
        The SetPhysXScene function should be used when an application has
        already created the PhysX scene, and it is best called before any
        props are added to the scene. Otherwise, the application must
        explicitly iterate over the existing props and instantiate them in
        the scene. The alternative is to create a snapshot for this scene,
        and use CreateSceneFromSnapshot which automatically instantiates
        any props in the scene.
    */
    
    /// @{
    
    /// Get the NxScene that is associated with this scene.
    NxScene* GetPhysXScene() const;
    
    /**
        Set the NxScene that is associated with this scene.
        
        In almost all circumstances applications must call this function
        before attempting simulation - the only exception is if you instead
        create a snapshot and use CreateSceneFromSnapshot. This function only
        stores the pointer; props that are already attached to this object
        are NOT created in the PhysX scene. Use the snapshot in each prop to
        explicitly create the props.

        Note that the NiPhysXManager::CreateSceneOnDifferentCore function is
        available to help with the creation of NxScene objects that execute
        on a specific core. This is particularly important on the Xenon
        platform, where by default the PhysX scene runs on the same core as
        the application main thread.

        It is an error to set the scene when a scene has already been set.
        Release the old scene first.
    */
    void SetPhysXScene(NxScene* pkScene);
    
    /**
        Release the NxScene associated with this scene.
        
        This first removes all props from the scene (to clear pointers into
        the NxScene object) then releases the PhysX scene from the SDK.
    */
    void ReleaseScene();
    
    /// Get the simulation compartment that was registered with the given ID.
    NxCompartment* GetCompartmentAt(NiUInt32 uiID);

    /**
        Add a compartment using an existing pointer.

        The compartment IDs are used by props to determine which compartment
        to instantiate their contents in.
    */
    bool AddCompartment(NiUInt32 uiID, NxCompartment* pkCompartment);
    
    /**
        Add a compartment using a descriptor.
    
        This causes a new compartment to be created in the target scene,
        which must have been specified at the time this function is called.
        The compartment IDs are used by props to determine which compartment
        to instantiate their contents in.
    */
    bool AddCompartment(NiUInt32 uiID, NxCompartmentDesc& pkDescriptor);

    /// @}


    /**
        @name PhysX Coordinate System 

        The PhysX global coordinate system may not coincide with the Gamebryo
        global coordinate system, so a transformation is stored that relates
        the two. It is the transform to apply to PhysX state to place it in
        Gamebryo's coordinates. In addition, there is a scale factor which is
        applied to all state. It is a convenience so that applications do not
        need to manage scaling themselves. The scale factor on the scene
        should match the scale factor on all of the props attached to the
        scene, or ill-defined behavior will result.
    */

    /// Get the PhysX to Gamebryo relative transformation.
    const NiTransform& GetSceneXform() const;

    /// Set the PhysX to Gamebryo relative transformation. Default is identity.
    void SetSceneXform(const NiTransform& kXform);
    
    /// Get the scale.
    ///
    /// Gamebryo world size is fScale times physics size.
    float GetScaleFactor() const;

    /// Set the scale.
    ///
    /// Gamebryo world size is fScale times physics size. Default is 1.0.
    void SetScaleFactor(const float fScale);
    
    /// @}
    
    /**
        @name Prop Management

        A NiPhysXScene object stores an array of smart pointers to props,
        and all simulation and update operations on the scene affect all of
        the props. Think of the scene as the container that holds the props
        and causes them to interact with each other. Props in different
        scenes will not interact with those in this one.
    */
    /// @{
    
    /**
        Return the number of props this scene holds.

        The array is packed, so the value returned is the actual number of
        props. 
    */
    NiUInt32 GetPropCount() const;

    /**
        Add a prop to the scene.
        
        The prop's PhysX content is also instantiated in the NxScene object
        if that has already been specified or created. The uiState argument
        specifies which cached snapshot state to use when the prop is
        instantiated. The return value is the index in the internal array of
        props at which this prop was added, or (unsigned int)-1 if there was
        an error instantiating the PhysX contents of the prop.
    */
    NiUInt32 AddProp(NiPhysXProp* pkProp, NiUInt32 uiState = 0);

    /// Retrieve the prop stored at the given index.
    NiPhysXProp* GetPropAt(const NiUInt32 uiIndex);

    /**
        Remove the specified prop.
        
        The prop's PhysX content is removed from the NxScene object if that
        has already been specified or created and the prop has a snapshot to
        tell us which PhysX content belongs to the prop.
    */
    void RemoveProp(NiPhysXProp* pkTarget);

    /**
        Remove the prop at the specified index.
        
        The prop's PhysX content is removed from the NxScene object if that
        has already been specified or created and the prop has a snapshot to
        tell us which PhysX content belongs to the prop.
    */
    void RemovePropAt(const NiUInt32 uiIndex);
    
    /// @}
    
    /**
        @name Update Functions for Sources and Destinations

        NiPhysXScene objects control the updating of the sources and
        destinations within them. The following functions control various
        aspects of updates.
    */
    /// @{
    
    /**
        Get the flag controlling the updating of sources.
        
        If false, no information is collected from Gamebryo objects nor pushed
        to PhysX actors, regardless of which other functions are called. 
    */
    bool GetUpdateSrc() const;

    /// Set the flag controlling updating of sources.
    ///
    /// Default is false.
    void SetUpdateSrc(const bool bActive);
    
    /**
        Get the flag controlling the updating of destinations.
        
        If false, no information is collected from PhysX actors nor pushed
        to Gamebryo nodes, regardless of which other functions are called. 
    */
    bool GetUpdateDest() const;

    /// Set the flag controlling updating of destinations.
    ///
    /// Default is false.
    void SetUpdateDest(const bool bActive);

    /**
        Add a source.
        
        This function should only be used if PhysX actors are being created
        completely outside of the NiPhysXProp system. Any source associated
        with a NiPhysXProp object is automatically added to the scene when
        the prop is added, and adding it again is an error.
    */
    void AddSource(NiPhysXSrc* pkSrc);

    /**
        Delete a source.
        
        This function should only be used if PhysX actors are being created
        completely outside of the NiPhysXProp system. Any source associated
        with a NiPhysXProp object is automatically removed from the scene when
        the prop is removed, and removing it again is an error.
    */
    void DeleteSource(NiPhysXSrc* pkTarget);

    /**
        Add a destination.
        
        This function should only be used if PhysX actors are being created
        completely outside of the NiPhysXProp system. Any destination
        associated with a NiPhysXProp object is automatically added to the
        scene when the prop is added, and adding it again is an error.
    */
    void AddDestination(NiPhysXDest* pkDest);

    /**
        Delete a destination.
        
        This function should only be used if PhysX actors are being created
        completely outside of the NiPhysXProp system. Any destination
        associated with a NiPhysXProp object is automatically removed from the
        scene when the prop is removed, and removing it again is an error.
    */
    void DeleteDestination(NiPhysXDest* pkTarget);


    /**
        Update all the sources associated with this scene.
        
        The set of sources includes all the sources in any prop attached
        to the scene. This is typically done before a simulation step to set
        the state for Gamebryo-controlled actors. The time is Gamebryo
        application time, and it is up to individual sources to determine
        how best to set the state for the given time. In most cases, no
        update will be done if the internal PhysX time is already at or past
        the requested time, except if bForce is true, in which case the
        update is always done. This function does nothing if the update
        sources flag is not set, and individual sources will not be updated
        if their own active flag is not set. There is no guarantee on the
        order in which sources will be updated. 
    */
    void UpdateSources(const float fTime, bool bForce = false);


    /**
        Update all the destinations associated with this scene.
        
        This function is typically called by applications after a simulation
        step. The time is Gamebryo application time, and it is up to
        individual destinations to determine how best to set the state for
        the given time. In most cases, no update will be done if the internal
        PhysX time is already at or past the requested time, except if bForce
        is true, in which case the update is always done. This function does
        nothing if the update destinations flag is not set, and individual
        destinations will not be updated if their own active flag is not set.
        There is no guarantee on the order in which destinations will be
        updated.
    */
    void UpdateDestinations(const float fTime, bool bForce = false);

    /**
        Update a single source.
        
        Arguments are as above for UpdateSources. This update is done
        regardless of the value returned by GetUpdateSrc.
    */
    void UpdateSource(NiPhysXSrc* pkSrc, const float fTime,
        const bool bForce = false);

    /**
        Update a single destination.
        
        Arguments are as above for UpdateDestinations. This update is done
        regardless of the value returned by GetUpdateDest.
    */
    void UpdateDestination(NiPhysXDest* pkDest, const float fTime,
        const bool bForce = false);

    /// @}
    
    /**
        @name Mesh Modifier Functionality
        
        The NiPhysXScene class holds an array of objects, derived from
        NiMesh, that need to have mesh modifiers submitted or completed
        when a simulation step begins or ends. Each of the meshes in the
        array has NiMesh::CompleteModifiers and NiMesh::SubmitModifiers
        called on it immediately before NxScene::simulate is called
        (with SyncPoint SYNC_PHYSICS_SIMULATE) and immediately after
        NxScene::fetchResults succeeds (with SyncPoint SYNC_PHYSICS_COMPLETED).
        
        Modified meshes that are held by a NiPhysXProp object will be
        automatically added and removed when the prop is added and removed
        from the scene. Applications should only use the functionality here
        if they are creating PhysX content outside of the prop system.
    */
    /// @{
    /**
        Add a mesh to the set that requires modifier notification.
    */
    void AddModifiedMesh(NiMesh* pkMesh);
 
    /**
        Remove a mesh from the set that requires modifier notification.
    */
    void DeleteModifiedMesh(NiMesh* pkMesh);

    /**
        Notify modified meshes that something about the scene has changed.
        
        This function causes the SceneChanged function to be called on all
        mesh modifiers derived from NiPhysXMeshModifier and associated with
        meshes in the modified mesh array. This function is called whenever
        any of the following are called on this scene:
        SetPhysXScene, SetSceneXform, SetScaleFactor,
        SetAsynchronousSimulation, SetUpdateSrc and SetUpdateDest.
        
        In cases where correct setup of the modifier depends on both being
        attached to the mesh and associated with a PhysX scene, applications
        may need to call this function, or NotifySceneChanged, when
        NiPhysXMeshModifier-derived modifiers are attached to a mesh that is
        already associated with a scene. For example, when
        NiMesh::AttachModifier is called, or NiMesh::ResetModifier is called,
        or any other function that causes a modifier to have the
        NiMeshModifier::Attach or NiMeshModifier::Detach function called.
    */
    void NotifyAllSceneChanged();

    /**
        Notify a mesh that something about the scene has changed.
        
        This function causes the SceneChanged function to be called on all
        mesh modifiers derived from NiPhysXMeshModifier and associated with
        the given mesh. This function is called by NotifyAllSceneChanged and
        when the mesh is first associated with the scene and when it is removed
        from the scene. The bRemoving argument is true only when the
        mesh has been removed from the scene.
        
        In cases where correct setup of the modifier depends on both being
        attached to the mesh and associated with a PhysX scene, applications
        may need to call this function, or NotifyAllSceneChanged,
        when NiPhysXMeshModifier-derived modifiers are attached to a mesh that
        is already associated with a scene. For example, when
        NiMesh::AttachModifier is called, or NiMesh::ResetModifiers is called. 
    */
    void NotifySceneChanged(NiMesh* pkMesh, bool bRemoving = false);

    /// @}

    /**
        @name Simulation Functions 

        NiPhysXScene objects provide the interface to NxScene::simulate()
        and NxScene::fetchResults() calls. Applications must always work
        through these functions to maintain a consistent simulation state.
        These functions enforce various rules related to simulation, such as
        making certain that simulate and fetchResults are always interleaved.

        The NiPhysXScene object takes complete control over the timing of the
        physics scene. Applications should not make any calls to
        NxScene::setTiming - they will be over-ridden by this object.
    
        Some of these functions control the time step that Gamebryo uses
        when calling simulate() for this scene and the time steps that
        PhysX uses. If active, fixed stepping uses a multiple of the given
        time step (set through SetTimestep) sufficient to get to or surpass
        the requested update time, otherwise it uses the difference between
        the target time and the most recent time (which depends on how the
        application calls update). The PhysX scene is set to use the same
        stepping mechanism, fixed or variable, as this object. The max time
        step passed to NxScene::setTiming is controlled by SetNumSubSteps:
        maxTimestep = fStep / uiNumSteps. The argument to SetMaxNumSteps is
        the same as would be passed as the maxIter argument to
        NxScene::setTiming.
    */
    
    /// @{

    /// Get the flag controlling fixed or variable simulation timing.
    bool GetDoFixedStep() const;

    /// Set the flag controlling fixed or variable simulation timing.
    ///
    /// Default is true.
    void SetDoFixedStep(const bool bActive);

    /// Get the timestep that Gamebryo uses with fixed time stepping.
    float GetTimestep() const;

    /// Set the timestep that Gamebryo uses with fixed time stepping.
    ///
    /// Default is 1.0 / 60.0.
    void SetTimestep(const float fStep);

    /**
        Get the number of steps that each Gamebryo step is broken into before
        being passed to PhysX.
        
        For example, if GetNumSubSteps returns 2, then each PhysX time step is
        half of the value returned by GetTimestep (above).
    */
    NiUInt32 GetNumSubSteps() const;

    /**
        Set the number of steps that each Gamebryo step is broken into before
        being passed to PhysX.
        
        For example, if SetNumSubSteps is set to 4, then each PhysX time step
        is one quarter of the value returned by GetTimestep (above).
        
        Default is 1.
    */
    void SetNumSubSteps(const NiUInt32 uiNumSteps);

    /// Get the maximum number of steps that PhysX will be asked to take.
    NiUInt32 GetMaxSubSteps() const;

    /// Set the maximum number of steps that PhysX will be asked to take.
    ///
    /// Default is 8.
    void SetMaxSubSteps(const NiUInt32 uiMaxSteps);

    /// Get the asynchronous simulation flag.
    ///
    /// When true, the PhysX simulation is running in parallel with rendering.
    bool GetAsynchronousSimulation() const;

    /**
        Set the asynchronous simulation flag.

        This flag must be set to true if the PhysX simulation is being done
        in parallel with rendering, or false otherwise. Failure to correctly
        set it may result in poor performance, PhysX errors, or even deadlock.
    */
    void SetAsynchronousSimulation(const bool bActive);

    /// Get the workflow manager that is being used for mesh modifiers.
    NiSPWorkflowManager* GetWorkflowManager() const;
    
    /// Set the workflow manager to be used for mesh modifiers.
    ///
    /// Any existing, locally owned manager is deleted.
    void SetWorkflowManager(NiSPWorkflowManager* pkWorkflowManager);
    
    /**
        Returns whether or not the scene is currently within a
        simulate/fetchResults pair.
        
        This is important information, because many actions on the PhysX
        SDK are prohibited during such times. This state may not always be
        easy to keep track of in applications, due to fixed length timesteps,
        asynchronous simulation or multi-threading. This function guarantees
        that, if the calling thread holds the PhysX SDK lock from
        NiPhysXManager, calls this function and receives false, then the
        simulate/fetchResults pair will not be entered until the lock is
        released. Threads can hence safely manipulate the SDK while they
        continue to hold the lock. See the Multithreading documentation in
        the Programmer's Guide for information on using the SDK lock. 
    */
    bool GetInSimFetch() const;

    /**
        Enter a simulation step.
        
        The function returns true if the step was actually started. It will
        return false if it is already within a simulate/fetchResults pair
        or if the target time has already been simulated to. You can force
        simulation with the bForceSim flag, but this might result in a zero
        length step. 
    */
    bool Simulate(const float fTargetTime, const bool bForceSim = false);

    /**
        Fetch the results of a simulation step.
        
        This function returns true if the results were actually fetched.
        It blocks on results if the target time is greater than the most
        recent simulation results or if forced to do so by the bBlock
        argument. When using fixed timesteps, the function may return false
        because there is already sufficient state to cover the requested
        time, in which case the application is still inside the
        simulate/fetchResults pair. This function also returns true if not
        in simulate/fetchResults when it is called. The puiErrorState
        argument is passed directly to PhysX to catch any hardware error
        code. 
    */
    bool FetchResults(const float fTargetTime, const bool bBlock = false,
        NxU32* puiErrorState = 0);

    /**
        Set the time that this scene was most recently simulated to.
        
        The PhysX scene works only on incremental time, so the NiPhysXScene
        maintains its own absolute time measurement. This function resets
        that time. This should not be done while inside a
        simulate/fetchResults pair. It should always be done before
        simulation begins, and can also be done whenever it is necessary to
        re-synchronize application and PhysX time. 
    */
    void SetFetchTime(const float fCurrentTime);

    /**
        Get the most recent time that this scene was simulated to.
        
        If currently in a simulate/fetchResults pair, then the time is the
        start time for this step. 
    */
    float GetPrevFetchTime() const;

    /**
        Returns the time that we are currently simulating to.
        
        This value is only valid when the scene is within a
        simulate/fetchResults pair (i.e. when GetInSimFetch returns true).
    */
    float GetNextFetchTime() const;
    
    /// @}
    
    /**
        @name Snapshots

        A snapshot is the state of all the Gamebryo-supported elements of a
        PhysX scene. The snapshot is used to stream the PhysX content and to
        provide save and restore functionality on actor poses and velocities.
        
        Each NiPhysXProp attached to the scene keeps a single snapshot
        (possibly NULL), which in turn contains multiple snapshot states for
        each actor, cloth and material. The individual actor states can be
        accessed directly, or the functions below can be used to operate on
        all actors in the scene. The two approaches should not be mixed
        because the state indexes will become confused.
    */
    
    /// @{
    
    /**
        Add a new set of actor poses and velocities, cloth vertex positions,
        and material properties, to the snapshots for all of the props in the
        scene.
        
        The poses are taken from the current state of the scene. Only actors,
        cloth and materials already in a snapshot are captured. Do not mix
        this function with NiPhysXActorDesc::AddState and similar functions;
        use one or the other.
    */
    NiUInt32 AddSnapshotState(NiFixedString& kName);

    /// Restore the cached state for the specific index. 
    ///
    /// This resets all of the positions and velocities in the scene. 
    void RestoreSnapshotState(const NiUInt32 uiIndex);

    /// Restore the cached state for the specific named state.
    ///
    /// This resets all of the positions and velocities in the scene. 
    void RestoreSnapshotState(const NiFixedString& kName);

    /**
        Remove a single stored state, by name.
       
        Convenience function that simply calls the corresponding function
        on all props.
    */
    virtual void RemoveSnapshotState(NiFixedString& kName);

    /**
        Remove a single stored state, by index.
       
        Convenience function that simply calls the corresponding function
        on all props.
    */
    virtual void RemoveSnapshotState(NiUInt32 uiIndex);

    /**
        Returns the snapshot for this scene, which is a smart pointer passed
        as a regular pointer for simplicity.

        The snapshot for the scene itself contains only data required to
        recreate the NxScene object. Snapshot data for objects in the scene is
        found in the attached props' snapshots.
    */
    NiPhysXSceneDesc* GetSnapshot();

    /// Sets the snapshot, which is internally stored as a smart pointer.
    ///
    /// Applications should not delete the memory themselves. 
    void SetSnapshot(NiPhysXSceneDesc* pkSnapshot);

    /**
        Sets the internal smart pointer to the snapshot to 0.

        Applications must not continue to hold a smart pointer to the snapshot
        after this function is called. 
    */
    void ReleaseSnapshot();

    /**
        Create a PhysX scene and fill it from the snapshots contained
        in props that have been added to the scene.
        
        Any existing NxScene object held by this scene is released, and a
        new NxScene object is created. Scene properties stored in the snapshot
        define the properties of the new scene. Any props in the scene are
        instantiated in the new scene. The index is the state index to use
        in setting poses, which should be 0 unless a tool or application has
        added additional states to the snapshot. 
        
        This function requires the existence of a snapshot on this object.
        In many cases, it is more convenient to create the NxScene object
        explicitly, set it on this scene, and then add props. The result is
        the same as adding props first, creating a snapshot for this object,
        and then calling this function.
    */
    bool CreateSceneFromSnapshot(const NiUInt32 uiIndex = 0);

    /**
        Create a PhysX scene and fill it from the snapshots contained
        in props that have been added to the scene.
        
        See the version of CreateSceneFromSnapshot above. This function uses
        a name to look up the initial conditions but is otherwise identical.
    */
    bool CreateSceneFromSnapshot(const NiFixedString& kName);

    /// @}

    /**
        @name Debug Rendering 

        If enabled, Gamebryo draws any debug rendering output produced by
        PhysX for this scene. You must separately enable debug visualization
        in PhysX via the SDK parameter NX_VISUALIZATION_SCALE and individual
        parameters for each form of debug output. Note that debug geometry is
        not produced until FetchResults returns true, so do not expect output
        immediately.
    */
    
    /// @{
    
    /// Returns true if debug rendering is active. 
    bool GetDebugRender() const;
    
    /**
        Set the debug rendering flag.
        
        If bActive is true, the pkDebugParent argument must be a scene
        graph node to which the debug geometry can be attached as a child.
        It does not really matter which node (provided it is updated and
        rendered) because coordinate systems are correctly accounted for
        by the debug rendering. 
        
        Default is false.
    */
    void SetDebugRender(const bool bActive, NiNode* pkDebugParent = 0);

    /**
        Deletes any Gamebryo nodes representing debug geometry.
        
        Use this when simulation has stopped but the debug flag remains set,
        or when the simulation is reset. Note that debug geometry is not
        updated until FetchResults returns true, so in some cases this
        function can be used to prevent the rendering of inaccurate debug
        geometry from a previous time step. 
    */
    void ClearDebugGeometry();
    
    /// @}
    
    /// @cond EMERGENT_INTERNAL
    
    virtual void PostLinkObject(NiStream& kStream);

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
            
protected:
    // Update
    void UpdateSourcesToActors(const float fTBegin, const float fTEnd);
    void UpdateActorsToDests(const bool bForce);
    
    // Mesh modifiers.
    void DoMeshModifiers(NiSyncArgs::SyncPoint kSyncPoint,
        bool bForced = false);


    // Debug geometry
    void SetDebugGeometry(const NxDebugRenderable* pkRenderState);
    void SetDebugTriangleGeometry(const NxDebugRenderable* pkRenderState);
    void SetDebugLineGeometry(const NxDebugRenderable* pkRenderState);
    void SetDebugPointGeometry(const NxDebugRenderable* pkRenderState);
    void ResizeDataStreamIfNecessary(NiMesh* pkMesh, NiUInt32 uiElementCount);
    NiMesh* CreateMesh(NiPrimitiveType::Type ePrimitiveType, 
        NiUInt32 uiElementCount, const NiDataStreamElementSet& kElements, 
        const NiTObjectArray<NiFixedString>& kSemmantics);

    // Creating scenes
    bool CreateSceneCompartments(NxScene* pkScene);
    bool CreateSceneFromProp(NiPhysXProp* pkProp,  NxScene* pkScene,
        const NiUInt32 uiIndex);

    // Back compatability for streaming
    void LoadOldBinary(NiStream& kStream);

    // A local pointer to the manager
    NiPhysXManager* m_pkManager;

    // The transform from Gamebryo world coordinates to the PhysX world
    // coordinates. Only valid if not slaved, otherwise the master's
    // transform is used, with the m_kSlavedXform used when caching and
    // restoring state.
    NiTransform m_kSceneXform;
    
    // Scale factor
    float m_fScalePToW;

    // PhysX scene object
    NxScene* m_pkPhysXScene;
    
    // Compartments
    NiTPointerMap<NiUInt32, NxCompartment*> m_kCompartmentMap;

    // Props
    NiTObjectArray<NiPhysXPropPtr> m_kProps;

    // Lists of objects to update
    NiTObjectArray<NiPhysXSrcPtr> m_kSources;
    NiTObjectArray<NiPhysXDestPtr> m_kDestinations;
    NiTObjectArray<NiMeshPtr> m_kModifiedMeshes;
   
    // Management of time for PhysX simulation
    float m_fTimestep; // Only used if fixed timestep
    NiUInt32 m_uiNumSubSteps; // Num PhysX steps per GB step
    NiUInt32 m_uiMaxSubSteps; // Max num PhysX steps

    float m_fPrevFetchTime; // The time of the most recent FetchResults.
    float m_fNextFetchTime; // The time of any upcoming FecthResults.
    
    // Debug rendering
    NiNodePtr m_spDebugRoot;
    NiMeshPtr m_spTriangleDebugGeometries;
    NiMeshPtr m_spLineDebugGeometries;
    NiMeshPtr m_spPointDebugGeometries;
    
    // Snapshots
    NiPhysXSceneDescPtr m_spSnapshot;
    
    // The workflow manager used to update mesh modifiers.
    NiSPWorkflowManager* m_pkWorkflowManager;

    // Flags
    enum {
        // Control of update
        UPDATE_SRC_MASK = 0x0002,
        UPDATE_DEST_MASK = 0x0004,
        
        // Time step control
        FIXED_TIMESTEP_MASK = 0x0010,

        // Is simulation parallel with rendering?
        SIM_PARALLEL_RENDER_MASK = 0x0020,
        
        // Do we own the workflow manager?
        OWN_MANAGER_MASK = 0x0040,
        
        // Debug rendering enabled
        DEBUG_RENDER_MASK       = 0x0400
    };
    
    NiDeclareFlags(NiUInt16);
    
    // Protect the simulate/fetchResult pair.
    bool m_bInSimFetch;

    //
    // For back compatable streaming only!!! Remove when we drop support
    // for streaming versions prior to Gamebryo 2.3.
    //
    bool m_bKeepMeshesBackCompatable;
};

#include "NiPhysXScene.inl"

#endif  // #ifndef NIPHYSXSCENE_H

