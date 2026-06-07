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

#ifndef NIPHYSXPROP_H
#define NIPHYSXPROP_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>
#include <NiMeshLib.h>

#include "NiPhysXDest.h"
#include "NiPhysXPropDesc.h"
#include "NiPhysXSrc.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiSmartPointer(NiPhysXPropDesc);
NiSmartPointer(NiPhysXProp);
class NiPhysXScene;


/**
    NiPhysXProp objects hold information about a PhysX prop, that is, part
    of a Gamebryo scene that is controlled by, or controls, a PhysX actor
    (including cloth, fluids or particles).
    
    Props are added to NiPhysXScene objects to activate them in the PhysX
    scene. A prop is the smallest "unit" for PhysX objects, and is intended
    to hold a self-contained thing, such as a crate, a rag-doll, or a physical
    machine. The same prop can be cloned to create many instances of the same
    object. Props are also the objects exported in NIF files in the Gamebryo
    art pipeline.
    
    Each prop hold a description of the PhysX contents that the prop
    represents, along with the objects that relate the PhysX content to the
    Gamebryo scene (NiPhysXSrc objects, NiPhysXDest objects, and
    NiMesh objects with PhysX-related modifiers).
    
    In addition, each prop holds a transformation that is applied to all
    PhysX poses and velocities when the prop is created in a scene. The
    transformation places the object relative to the global coordinate
    system of the NxScene that the prop is created in. This transformation
    is intended to allow clones of a prop template to be easily placed in
    different locations in the scene. Note that it is only used when the
    PhysX content is instantiated - it has no effect once the objects have
    been created.
    
    The scale factor relating the PhysX coordinate system to the Gamebryo
    coordinate system is stored as a convenience. This scale should match
    that specified for the NiPhysXScene that the prop is attached to.
    
    The prop also keeps a flag that controls the handling of PhysX collision
    mesh data. In almost all cases this flag should be false so that mesh
    data is deleted once the mesh is instantiated in the PhysX SDK. If this
    flag is true, then the cooked mesh data is kept for later use.
    Applications would only do this if they were planning to explicitly
    release the mesh object from the PhysX SDK and create it again later,
    or if planning to release the SDK and recreate all physics content again
    later. However, in this case it may be better to stream the scene in
    again.  
*/
class NIPHYSX_ENTRY NiPhysXProp : public NiObjectNET
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXProp);

    /// @endcond

public:
    /**
        Constructor.
        
        The transformation is set to the identity, the scale factor is 1.0,
        and meshes are set to not keep. All other data structures are empty or
        null.
    */
    NiPhysXProp();
    
    /**
        Destructor.

        This releases all smart pointers. It does not remove the PhysX content
        represented by the prop from the NxScene it is in.
    */
    ~NiPhysXProp();

    /**
        @name Attaching to a NiPhysXScene object
        
        For additional information, see NiPhysXScene::AttachProp and
        NiPhysXScene::DetachProp.
    */
    /// @{
    
    /**
        Return the scene to which this object is currently attached.
        
        The scene is automatically set when the prop is attached to a scene,
        or detached. It cannot be set by the application.
    */
    NiPhysXScene* GetScene();
    
    /// Get the transformation that is applied to all PhysX poses and
    /// velocities when the prop is created in a scene.
    NxMat34 GetXform() const;

    /// Set the transformation that is applied to all PhysX poses and
    /// velocities when the prop is created in a scene.
    void SetXform(const NxMat34& kXform);
    
    /// Get the scale.
    float GetScaleFactor() const;

    /**
        Set the scale.
        
        World size is fScale times physics size. All of the props in a
        scene should have the same scale, or strange things will happen
        as positions and velocities are scaled differently for different
        props.
    */
    void SetScaleFactor(const float fScale);
    
    /**
        Callback used when a prop is attached to a scene.
        
        This function is called when the prop is attached to a NiPhysXScene
        object. Derived classes should implement this function if special
        action must be taken. If a derived class implements this, it must
        call the base class version also.
    */
    virtual void AttachSceneCallback(NiPhysXScene* pkScene);

    /**
        Callback used when a prop is detached from a scene.
        
        This function is called when the prop is detached from a NiPhysXScene
        object. Derived classes should implement this function if special
        action must be taken. If a derived class implements this, it must
        call the base class version also.
    */
    virtual void DetachSceneCallback(NiPhysXScene* pkScene);

    /// @}


    /**
        @name Sources and Destinations

        Each NiPhysXProp object maintains a NiTObjectArray of sources and
        another of destinations. The contents of these arrays are
        automatically added and removed from an NiPhysXScene object when the
        prop is added and removed.
        
        The arrays store smart pointers, so objects passed in must be heap
        allocated with NiNew and applications should never explicitly delete
        an object stored in one of these arrays.
    */
    
    /// @{
    
    /// A convenience functions to set the interpolation flag on all sources
    /// in this prop.
    void SetAllSrcInterp(const bool bActive);
    
    /// A convenience functions to set the interpolation flag on all
    /// destinations in this prop.
    void SetAllDestInterp(const bool bActive);
    
    /// Obtain the number of NiPhysXSrc-derived objects stored.
    NiUInt32 GetSourcesCount() const;

    /// Add a source to the array, and return the index it was added at.
    NiUInt32 AddSource(NiPhysXSrc* pkSrc);

    /// Return a pointer to the source at a given location (not range checked).
    NiPhysXSrc* GetSourceAt(const NiUInt32 uiIndex);

    /**
        Remove a source from the array.
    
        Sources are stored in smart pointers, so the source will be deleted
        if no other object is holding a pointer to the source.
        
        Returns true if the source was present and deleted, false otherwise.
    */
    bool DeleteSource(NiPhysXSrc* pkTarget);

    /**
        Remove the source at the given index from the array.
    
        Sources are stored in smart pointers, so the source will be deleted
        if no other object is holding a pointer to the source.
    */
    void DeleteSourceAt(const NiUInt32 uiIndex);    

    /// Obtain the number of NiPhysXDest-derived objects stored.
    NiUInt32 GetDestinationsCount() const;

    /// Add a destination to the array, and return the index it was added at.
    NiUInt32 AddDestination(NiPhysXDest* pkDest);

    /// Return a pointer to the destination at a given location (not range
    /// checked).
    NiPhysXDest* GetDestinationAt(const NiUInt32 uiIndex);

    /**
        Remove a destination from the array.
    
        Destinations are stored in smart pointers, so the destination will
        be deleted if no other object is holding a pointer to the destination.
        
        Returns true if the destination was present and deleted, false
        otherwise.
    */
    bool DeleteDestination(NiPhysXDest* pkTarget);

    /**
        Remove the destination at the given index from the array.
    
        Destinations are stored in smart pointers, so the destination will be
        deleted if no other object is holding a pointer to the destination.
    */
    void DeleteDestinationAt(const NiUInt32 uiIndex);

    /// @}

    /**
        @name Mesh Modifier Functionality
        
        The NiPhysXProp class holds an array of objects, derived from
        NiMesh, that need to have mesh modifiers submitted or completed
        when a simulation step begins or ends. Each of the meshes in the
        array has NiMesh::CompleteModifiers and NiMesh::SubmitModifiers
        called on it immediately before NxScene::simulate is called
        (with SyncPoint SYNC_PHYSICS_SIMULATE) and immediately after
        NxScene::fetchResults succeeds (with SyncPoint SYNC_PHYSICS_COMPLETED).
        
        Modified meshes that are held by a NiPhysXProp object will be
        automatically added and removed from an NiPhysXScene object when the
        prop is added and removed.
        
        The meshes are stored as smart pointers, so they must be heap
        allocated with NiNew and applications should never explicitly delete
        a mesh stored by this class.
    */
    /// @{
    /// Obtain the number of meshes stored.
    NiUInt32 GetModifiedMeshCount() const;

    /// Add a mesh to the set that requires modifier notification, and return
    /// the index it was added at.
    NiUInt32 AddModifiedMesh(NiMesh* pkMesh);

    /// Return a pointer to the mesh at a given location (not range checked).
    NiMesh* GetModifiedMeshAt(const NiUInt32 uiIndex);

    /**
        Remove a mesh from the set that requires modifier notification.
    
        Meshes are stored in smart pointers, so the mesh will be deleted
        if no other object is holding a pointer to the mesh.
        
        Returns true if the mesh was present and deleted, false otherwise.
    */
    bool DeleteModifiedMesh(NiMesh* pkMesh);
 
    /**
        Remove the mesh at the given index from the set that requires
        modifier notification.
    
        Meshes are stored in smart pointers, so the mesh will be deleted
        if no other object is holding a pointer to the mesh.
    */
    void DeleteModifiedMeshAt(const NiUInt32 uiIndex);

    /// @}
    
    /**
        @name Snapshots and Reset

        A snapshot is the state of all the Gamebryo-supported elements of a
        PhysX scene. The snapshot is used to stream the PhysX content and to
        provide streaming, cloning and reset functionality on actor poses and
        velocities. Each NiPhysXProp keeps a single snapshot (possibly NULL),
        which in turn contains multiple snapshot states for each actor and
        material. The individual actor states can be accessed directly, or
        the functions in NiPhysXScene can be used to manipulate the states
        on all actors in the scene. The two approaches should not be mixed
        because the state indexes will become confused. 
    */
    
    /// @{

    /**
        Returns the snapshot for this prop, which is a smart pointer passed
        as a regular pointer for simplicity.
        
        Applications will frequently use this function to get the snapshot
        after streaming a prop into Gamebryo, so that they can access the
        actors, materials and joints before the PhysX content is instantiated.
        This is useful, for instance, in setting collision group flags or
        joint motors that cannot be authored in the art tools. Note that any
        modifications must be made to the snapshot before the prop is added
        to the scene, or before NiPhysXScene::CreateSceneFromSnapshot is
        called on the scene.
    */
    NiPhysXPropDesc* GetSnapshot();

    /**
        Sets the snapshot, which is internally stored as a smart pointer.
        
        Applications should not delete the memory themselves. Rather, use
        ReleaseSnapshot.
    */
    void SetSnapshot(NiPhysXPropDesc* pkSnapshot);

    /**
        Sets the internal smart pointer to the snapshot to 0.
        
        Applications must not continue to hold a smart pointer to the
        snapshot after this function is called.
    */
    void ReleaseSnapshot();

    /// Get the flag controlling deletion of mesh data on instantiation.
    bool GetKeepMeshes() const;

    /// Set the flag controlling deletion of mesh data on instantiation.
    void SetKeepMeshes(const bool bVal);
        
    /// @}
    
    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
            
protected:
    // The scene we are attached to, or NULL
    NiPhysXScene* m_pkScene;
    
    // The transform that takes this scene's PhysX coordinate system to the
    // master scene's coordinate system. Used in snapshots.
    NxMat34 m_kXform;
    
    // Scale factor
    float m_fScalePToW;

    // Lists of objects to update
    NiTObjectArray<NiPhysXSrcPtr> m_kSources;
    NiTObjectArray<NiPhysXDestPtr> m_kDestinations;
    NiTObjectArray<NiMeshPtr> m_kModifiedMeshes;
    
    // Snapshots
    NiPhysXPropDescPtr m_spSnapshot;
    bool m_bKeepMeshes;
};

#include "NiPhysXProp.inl"

#endif  // #ifndef NIPHYSXPROP_H

