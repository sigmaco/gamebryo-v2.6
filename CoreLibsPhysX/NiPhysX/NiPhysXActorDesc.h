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


#ifndef NIPHYSXACTORDESC_H
#define NIPHYSXACTORDESC_H

#include "NiPhysXLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXBodyDesc.h"
#include "NiPhysXShapeDesc.h"
#include "NiPhysXRigidBodyDest.h"
#include "NiPhysXRigidBodySrc.h"

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

/**
    NiPhysXActorDesc objects are for caching snapshots of NxActor objects,
    used for streaming, cloning and restoring simulation state.
    
    The primary reason for an application to create an NiPhysXActorDesc
    object is to ease cloning of PhysX content in code, or to specify a
    particle actor for a PhysX-based particle system. However, applications
    can also access these objects between loading a NIF file containing
    NiPhysXProp objects and instantiating the PhysX scene. This enables
    post-load modification of the properties of actors and is an important
    way to overcome limitations in the authoring of PhysX content. 

    The class stores the state necessary to create a PhysX actor and to
    create the actor's links with a Gamebryo scene graph. After the PhysX
    content has been instantiated, the class also contains a pointer to the
    NxActor object created from the description. Multiple initial conditions
    may be stored, enabling the actor to be reset to a variety of poses.

    @note This class does not cache the userData associated with the actor.
        Applications should derive from this class if they wish to cache or
        stream such data. 
*/
class NIPHYSX_ENTRY NiPhysXActorDesc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXActorDesc);
    /// @endcond
    
public:
    /**
        The actor descriptor is initialized to the PhysX defaults for an
        NxActorDesc.
        
        All pointers into the Gamebryo scene are set to null. No initial
        conditions are stored. Note that the NxActor is not created at
        this time.
    */
    NiPhysXActorDesc();
    
    /// Destructor.
    ///
    /// Frees all memory and sets any smart pointers to 0.
    virtual ~NiPhysXActorDesc();

    /**
        @name Constant State Access.
        
        This group of functions provides access to the actor properties
        that are assumed by Gamebryo-PhysX to be constant for the lifetime
        of an actor (once that actor has been created).

        All of the properties correspond to the contents of an NxActorDesc,
        but use Gamebryo types that support streaming. The pkBodyDesc
        argument is stored in a smart pointer. The shape array is
        copied.

        The set functions only modify the value stored in this object, which
        is used when the actor is first created and for streaming and cloning.
        It does not modify the property of an already existing NxActor created
        from the object. If you would like to modify an existing actor, use
        the GetActor function to obtain the NxActor object and set the
        property using a function call on that.
    */
    /// @{
    
    /// Set all of the actor properties in one function.
    void SetConstants(const NiFixedString& kActorName, const NxReal fDensity, 
        const NxU32 uiActorFlags,const NxActorGroup uiActorGroup,
        const NxDominanceGroup uiDominanceGroup, 
        const NxU32 uiContactReportFlags, const NxU16 uiForceFieldMaterial,
        NiPhysXBodyDesc* pkBodyDesc, 
        const NiTObjectArray<NiPhysXShapeDescPtr>& kActorShapes);

    /// Get the actor's name.
    NiFixedString GetActorName() const;

    /// Set the actor's name.
    void SetActorName(NiFixedString& kActorName);

    /// Get the actor's density.
    NxReal GetDensity() const;

    /// Set the actor's density.
    void SetDensity(const NxReal fDensity);

    /// Get the actor flags.
    NxU32 GetActorFlags() const;

    /// Set the actor flags.
    void SetActorFlags(const NxU32 uiActorFlags);

    /// Get the actor group.
    NxActorGroup GetActorGroup() const;

    /// Set the actor group.
    void SetActorGroup(const NxActorGroup uiActorGroup);

    /// Get the actor's dominance group.
    NxDominanceGroup GetDominanceGroup();

    /// Set the actor's dominance group.
    void SetDominanceGroup(NxDominanceGroup uiDominanceGroup);

    /// Get the actor's contact report flags.
    NxU32 GetContactReportFlags();

    /// Set the actor's contact report flags.
    void SetContactReportFlags(NxU32 uiContactReportFlags);

    /// Get the actor's force field material.
    NxU16 GetForceFieldMaterial();

    /// Set the actor's force field material.
    void SetForceFieldMaterial(NxU16 uiForceFieldMaterial);

    /// Get the body descriptor object (no not free this pointer).
    NiPhysXBodyDesc* GetBodyDesc();

    /// Set the body descriptor object (stored as a smart pointer).
    void SetBodyDesc(NiPhysXBodyDesc* pkBodyDesc);

    /// Get a reference to the internal array of shapes for the actor.
    NiTObjectArray<NiPhysXShapeDescPtr>& GetActorShapes();

    /// Set the array of shapes for the actor (the argument is copied).
    void SetActorShapes(
        const NiTObjectArray<NiPhysXShapeDescPtr>& kActorShapes);
    
    /// @}

    /**
        @name Actor Creation and Removal
    */
    /// @{

    /// Returns true if the current state of this object is a valid
    /// PhysX actor (according to NxActorDesc::isValid), false otherwise.
    virtual bool IsValid();

    /**
        Create the actor in a PhysX scene.
        
        A pointer to the newly created actor is kept by this object.
        
        @param pkScene The PhysX scene in which to create the actor.
        @param pkCompartment The compartment to use, or NULL to use the
            default compartment.
        @param uiStateIndex The initial state to use, which must be less
            than the number of stored initial states.
        @param kXform A transformation to apply to all initial states before
            creating the actor. This allows the actor to be moved within the
            scene without modifying all its initial poses.
        @param bKeepMeshes Control whether or not the mesh data used to create
            collision meshes for this object is kept after the collision mesh
            is created. Such data is not necessary after creation unless you
            plan to remove the mesh from the set cached by the NiPhysXManager
            and recreate it later, such as when releasing the PhysX SDK
            completely and recreating it.
        
        @return The PhysX actor object, or NULL on failure.
    */
    virtual NxActor* CreateActor(NxScene* pkScene,
        NxCompartment* pkCompartment, const NiUInt32 uiStateIndex,
        const NxMat34& kXform, const bool bKeepMeshes = false);

    /**
        Removes the actor represented by this object from a given scene.
        
        The actor must have been created, or else no work is done. The scene
        must be the same scene that the actor resides in, but this is not
        checked.
    */
    virtual void RemoveActor(NxScene* pkScene);

    /// Get a pointer to the actor, or NULL if this object does not yet have
    /// a PhysX actor assigned or created.    
    NxActor* GetActor() const;

    /// Set the actor that this object is representing.
    ///
    /// This is not necessary when CreateActor is used to create the actor.
    void SetActor(NxActor* pkActor);

    /// @}

    /**
        @name Initial State Management
        
        This object can store multiple sets of initial poses for the actor.
        Any one of these may be used when creating the actor or the actor
        may be repositioned to one of these states on demand.
        
        If no poses are provided, the actor is assumed to have the identity
        matrix for its pose.
    */
    /// @{
    
    /// Obtain the number of poses stored for the actor.
    NiUInt32 GetNumPoses() const;
    
    /// Get the pose stored at a specific index.
    ///
    /// The index must be less than the number returned by GetNumPoses.
    NxMat34 GetPose(const NiUInt32 uiIndex) const;

    /**
        Set the pose at a given index.
        
        More space is allocated when the requested index is beyond the
        number already stored. All the newly allocated states up to the
        given index, if not already set, are filled with the identity.
    **/
    void SetPose(const NxMat34& kPose, const NiUInt32 uiIndex);

    /**
        Add a new initial state taken from the actor's current pose.
        
        The actor's current pose is extracted, multiplied by the given
        transform, and stored in the next unused state index. The index for
        the newly stored state is returned. The actor must have been
        created or set for this function to do anything.
        
        This function also causes a new state to be added for any body
        descriptor associated with this object.

        Applications should choose to either use this function directly
        on all the actors in a NiPhysXScene or to use
        NiPhysXScene::AddSnapshotState(), but avoid mixing the two or
        the state indexes may become confused. 
    */
    virtual NiUInt32 AddState(const NxMat34& kXform);

    /**
        Restore the actor's state to that stored in the given index.
        
        The index must be less than the number returned by GetNumPoses.
        The stored pose is first multiplied by the given transform. This
        function uses NxActor::setGlobalPose, which is moderately expensive.
        It is intended for infrequent resets of game state.
        
        If the actor has a body descriptor, the velocities are also restored.
    */
    virtual void RestoreState(const NiUInt32 uiIndex,
        const NxMat34& kXform);
    
    /// Remove all the stored states starting at and beyond the given index
    /// (and also remove the body descriptor's states).
    virtual void RemoveStatesBeyond(const NiUInt32 uiState);

    /// Remove the stored state at the given index, and shuffle down other
    /// states to fill any hole in stored state indexes
    /// (and also remove the body descriptor's states).
    virtual void RemoveState(const NiUInt32 uiState);

    /// @}
    
    /**
        Get the descriptor for the parent of this actor in a joint hierarchy.

        This data is used internally to reconstruct the sources and
        destinations that tie the actor to the Gamebryo scene graph.
    */
    NiPhysXActorDesc* GetActorParent() const;

    /// Set the descriptor for the parent of this actor in a joint hierarchy.
    void SetActorParent(NiPhysXActorDesc* pkActorDesc); 

    /// Get the source object that ties the Gamebryo scene graph to this actor
    /// (null if no source has been specified).
    NiPhysXRigidBodySrcPtr GetSource() const;

    /// Set the source object.
    ///
    /// The given object is stored as a smart pointer.
    void SetSource(NiPhysXRigidBodySrcPtr spSource);
    
    /// Get the destination object that ties the Gamebryo scene graph to
    /// this actor (null if no destination has been specified).
    NiPhysXRigidBodyDestPtr GetDest() const;

    /// Set the destination object.
    ///
    /// The given object is stored as a smart pointer.
    void SetDest(NiPhysXRigidBodyDestPtr spDest);

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
        
protected:
    /// Create an NxActorDesc object based on this object.
    virtual void ToActorDesc(NxActorDesc& kActorDesc,
        NxCompartment* pkCompartment, const NiUInt32 uiIndex,
        const NxMat34& kNxMat34, const bool bKeepMeshes,
        const bool bCheckPlatform = false);
    
    /// Constant through the life of an actor
    NiFixedString m_kActorName;
    NxReal m_fDensity;
    NxU32 m_uiActorFlags;
    NxActorGroup m_uiActorGroup;
    NxDominanceGroup m_uiDominanceGroup;
    NxU32 m_uiContactReportFlags;
    NxU16 m_uiForceFieldMaterial;
    NiUInt32 m_uiCompartmentID;
    NiPhysXBodyDescPtr m_spBodyDesc;
    NiTObjectArray<NiPhysXShapeDescPtr> m_kActorShapes;
    
    /// An array of poses
    NiUInt32 m_uiNumPoses;
    NxMat34* m_pkPoses;

    /// Pointers to the PhysX objects.
    NxActor* m_pkActor;
    NiPhysXActorDesc* m_pkActorParent;
    
    /// Source and or destination for this actor. Needed so that their
    /// internal actor pointers can be reset after streaming or cloning.
    NiPhysXRigidBodySrcPtr m_spSource;
    NiPhysXRigidBodyDestPtr m_spDest;

    /// Temporary for use while working with PhysX descriptors.
    /// Should always be freed within the same operation that creates it.
    NxBodyDesc* m_pkBodyDesc;
};

NiSmartPointer(NiPhysXActorDesc);

#include "NiPhysXActorDesc.inl"

#endif // #ifdef NIPHYSXACTORDESC_H
