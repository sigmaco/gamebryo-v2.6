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

#ifndef NIPHYSXPSPARTICLESYSTEM_H
#define NIPHYSXPSPARTICLESYSTEM_H

#include "NiPhysXParticleLibType.h"

#include <NiPSParticleSystem.h>
#include <NiPSMeshParticleSystem.h>
#include "NiPhysXPSParticleSystemDest.h"
#include "NiPhysXPSParticleSystemProp.h"

/**
    The NiPhysXPSParticleSystem class implements Gamebryo particle
    systems that use PhysX actors for the particles.
    
    The class is derived from, and borrows almost all functionality from,
    the NiPSParticleSystem class. Additional functionality is related
    to creation and management of PhysX actors to represent the particles.
*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSParticleSystem : 
    public NiPSParticleSystem
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareFlags(NiUInt8);
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXPSParticleSystem);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:

    /**
        Creation function.
        
        There is no public constructor for NiPhysXPSParticleSystem objects.
        Rather, this creation function is provided. It will return 0 if 
        creation fails for some reason.
        
        @param uiMaxNumParticles The maximum number of particles (see
            NiPSParticleSystem).
        @param bHasColors Whether or not this particle system has per-particle
            colors (see NiPSParticleSystem).
        @param bHasRotations Whether or not this particle system has
            per-particle rotation (see NiPSParticleSystem).
        @param bWorldSpace If true, particles are assumed to be in world
            coordinates (see NiPSParticleSystem).
        @param bDynamicBounds If true, the particle bounds are updated on
            every frame. See the NiPSParticleSystem documentation for
            a discussion of the relative merits of this. 
        @param bCreateDefaultGenerator If true, a default particle
            generator, of type NiPSFacingQuadGenerator, is created to generate
            particle geometry for rendering (see NiPSParticleSystem).
        @param bAttachMeshModifiers If true, mesh modifiers associated with
            the system are attached on creation (see NiPSParticleSystem).
        @param pkProp The NiPhysXMeshPSysProp object that this fluid system
            should be part of (note that this is a special sub-class of
            NiPhysXProp for particle systems). This can be NULL at the time
            of construction but then must be later set with AttachToProp.
        @param uiCompartmentID The PhysX simulation compartment in which to
            create the particle actors (see NiPhysXPropDesc).
        @param bPhysXSpace If true, the PhysX particle actors are assumed
            to be in the same coordinate system as the
            NiPhysXMeshParticleSystem. That requires the
            NiPhysXScene::GetSceneXform always return the same transformation
            as NiPhysXPSParticleSystem::GetWorldTransform.
        @param bPhysXRotations If true, the rotations on the Gamebryo
            particles are taken from the PhysX particle actors, otherwise
            they are controlled by standard Gamebryo particle rotation
            controllers.
        @param uiActorPoolSize The maximum size of the actor pool to use. This
            pool avoids repeated calls to create and release PhysX actors.
            It uses actor flags to effectively remove the pooled particles
            from the scene.
        @param bFillActorPoolsOnLoad If true, the actor pools are filled when
            the file is streamed or when a PhysX scene is specified for the
            system.
        @param bKeepsMeshes Controls what happens to cooked collision mesh
            data once the meshes are instantiated.
            See the NiPhysXProp documentation for more information. 
    */
    static NiPhysXPSParticleSystem* Create(NiUInt32 uiMaxNumParticles, 
        bool bHasColors, bool bHasRotations, bool bWorldSpace = true,
        bool bDynamicBounds = true, bool bCreateDefaultGenerator = true,
        bool bAttachMeshModifiers = true, 
        NiPhysXPSParticleSystemProp* pkProp = 0, NiUInt32 uiCompartmentID = 0,
        bool bPhysXSpace = false, bool bPhysXRotations = false,
        NiUInt32 uiActorPoolSize = NiPSMeshParticleSystem::DEFAULT_POOL_SIZE,
        bool bFillActorPoolsOnLoad = false, bool bKeepsMeshes = false);

    /// Destructor.
    virtual ~NiPhysXPSParticleSystem();
    
    /**
        @name Particle Rotation Control
        
        The rotation of the Gamebry particles may be driven by that of the
        PhysX actors. When PhysX actor rotations are used any controllers
        for particle rotation are ignored.
        
        Particles use PhysX actor rotations if the flag for Uses PhysX
        Rotations is true.
    */
    
    /// @{
    
    /// Get the flag for PhysX rotations.
    bool GetUsesPhysXRotations() const;

    /// Set the flag for PhysX rotations.
    void SetUsesPhysXRotations(const bool bPhysXRotations);

    /// @}

    /**
        @name Coordinate System Management
        
        A significant optimization is possible when the particle system
        is in the same coordinate system as the PhysX scene. Typically,
        this will be the case if the various transformations in the NiPhysXProp
        object and the NiPhysXScene object that the system belongs to are
        set to the identity, and the particles themselves are in world space.
        
        Particles are assumed to be in PhysX world coordinates if the flag
        for Uses PhysX Space is true.
    */
    
    /// @{
    
    /// Get the flag for PhysX space.
    bool GetUsesPhysXSpace() const;

    /// Set the flag for PhysX space.
    void SetUsesPhysXSpace(const bool bPhysXSpace);

    /// @}

    /// Get the Gamebryo-PhysX prop object that this particle system is
    /// a part of.
    NiPhysXPSParticleSystemProp* GetProp() const;
    
    /**
        Attach this particle system to a prop.

        The particle system must be attached to a prop, and the prop must be
        added to a NiPhysXScene object, and that NiPhysXScene object must have
        a NxSceen in order for the particle system to function.
    */
    void AttachToProp(NiPhysXPSParticleSystemProp* pkProp);

    /// Remove this system from its prop.
    ///
    /// This will cause all the particles to be deleted.
    void DetachFromProp();

    /**
        @name Prototype Actor Management
        
        This class stores the prototypical actor used to represent particles.
        The contents of the actor descriptor defines the particle behavior
        from a physics point of view. It can be used to enable or disable
        particle collisions, turn gravity on or off, lock degrees of freedom,
        etc. 

        This class also maintains a pool of actors that are un-used by any
        particle. The aim is to avoid expensive actor creation and deletion
        calls within PhysX. The un-used actors must be placed at some location
        in the PhysX world, and it is very un-wise to put them all at the same
        place. Hence, a region must be specified into which the actors will
        be randomly placed when not in use. When the pools are filled on load,
        each actor is placed in a random location in a box with a given
        center and half-side length. These values are given in the PhysX scene
        coordinate system. 
    */
    /// @{
    
    /// Get the actor descriptor.
    NiPhysXActorDesc* GetActorDesc() const;

    /// Set the actor descriptor.
    ///
    /// The actor will be stored as a smart pointer, so do not delete it.
    NiPhysXActorDescPtr SetActorDesc(NiPhysXActorDesc* pkActor);

    /// Get the flag controlling whether or not actor pools are filled
    /// when the particle system is first attached to a PhysX scene.
    bool GetFillActorPoolOnLoad();
    
    /**
        Set the flag controlling whether or not actor pools are filled
        when the particle system is first attached to a PhysX scene.
        
        This flag must be set before the NiPhysXProp object that
        contains this system is added to a NiPhysXScene object.
    */
    void SetFillActorPoolOnLoad(bool bFill);

    /// Get the default size to use for the PhysX actor pools.
    NiUInt32 GetDefaultActorPoolSize();

    /**
        Set the default size to use for the PhysX actor pools.
        
        This value must be set before the NiPhysXProp object that
        contains this system is added to a NiPhysXScene object.
    */
    void SetDefaultActorPoolSize(NiUInt32 uiSize);

    /// Obtain the region used to store un-used particle actors.
    ///
    /// The region is a box with center kCenter and half size length kDim.
    void GetActorPoolRegion(NiPoint3& kCenter, NiPoint3& kDim);

    /**
        Set the region used to store un-used particle actors.

        The region is a box with center kCenter and half size length kDim.
        
        The region must be set before the NiPhysXProp object that
        contains this system is added to a NiPhysXScene object.
    */
    void SetActorPoolRegion(const NiPoint3 kCenter, const NiPoint3 kDim);

    /**
        Get the flag controlling the treatment of PhysX collision mesh data
        on load.
        
        See NiPhysXProp for a discussion of mesh keeping.
    */
    bool GetKeepsActorMeshes();

    /**
        Set the flag controlling the treatment of PhysX collision mesh data
        on load.
        
        See NiPhysXProp for a discussion of mesh keeping.
    */
    void SetKeepsActorMeshes(const bool bKeep);

    /// Get the PhysX compartment ID for use when creating particle actors.
    ///
    /// See the NiPhysXPropDesc class for details.
    NiUInt32 GetCompartmentID() const;

    /**
        Get the PhysX compartment ID for use when creating particle actors.

        See the NiPhysXPropDesc class for details.
        
        The compartment must be set before the NiPhysXProp object that
        contains this system is added to a NiPhysXScene object.
    */
    void SetCompartmentID(const NiUInt32 uiCompartmentID);

    /// @}

    /// Remove all particles.
    virtual void ResetParticleSystem();
    
    /// @cond EMERGENT_INTERNAL

    // Managing the PhysX scene this system is attached to.
    NiPhysXScene* GetScene() const;
    void SetScene(NiPhysXScene* pkScene);
    void AttachToScene(NiPhysXScene* pkScene);
    void DetachFromScene(void);

    void SetActorAt(NiUInt16 usIndex, NxActor* pkActor);

    const NiTransform& GetPhysXToPSysTransform();
    void SetPhysXToPSysTransform(const NiTransform& kPhysXToPSys);

    void SetScalePhysXToWorld(float fScalePToW);
    float GetScalePhysXToWorld();

    bool GetNeedsUpdate() const;
    void SetNeedsUpdate(const bool bNeedsUpdate);

    NxActor** GetPhysXParticleActors();

    NiPoint3* GetPhysXParticleLastVelocities();
    const NiPoint3* GetPhysXParticleLastVelocities() const;

    NxMat34* GetPhysXParticleActorPose();
    const NxMat34* GetPhysXParticleActorPose() const;

    NxVec3* GetPhysXParticleActorVelocity();
    const NxVec3* GetPhysXParticleActorVelocity() const;

    virtual void InitializeParticle(NiUInt32 uiNewParticle);

    // Removes a particle from the simulation.
    virtual void RemoveParticle(NiUInt32 uiIndexToRemove);

    int GetActorPoolSize();
    void SetActorPoolSize(NiUInt32 uiSize);

    int GetActorPoolCount();
    bool IsActorPoolEmpty();
    void EmptyActorPool();
    void FillActorPool(NiPhysXActorDesc* pkOriginal);

    NxActor* RemoveActorFromPool();
    void AddActorToPool(NxActor* pkActor);

    void MapActorPool(NiTMap<NxActor*, bool>& kMap);

    void GetPhysXState();

    static NiPhysXPSParticleSystem* Create(NiPSParticleSystem *pkPSys,
        NiPhysXPSParticleSystemProp* pkProp = 0, NiUInt32 uiCompartmentID = 0,
        bool bPhysXSpace = false, bool bPhysXRotations = false,
        NiUInt32 uiActorPoolSize = NiPSMeshParticleSystem::DEFAULT_POOL_SIZE,
        bool bFillActorPoolsOnLoad = false, bool bKeepsMeshes = false);
   
    virtual void ProcessClone(NiCloningProcess& kCloning);

    virtual void PostLinkObject(NiStream& kStream);

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    NiPhysXPSParticleSystem(NiPSSimulator* pkSimulator, 
        NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations, 
        bool bWorldSpace, NiPSBoundUpdater* pkBoundUpdater, 
        NiPhysXPSParticleSystemProp* pkProp, NiUInt32 uiCompartmentID,
        bool bPhysXSpace, bool bPhysXRotations, NiUInt32 uiActorPoolSize,
        bool bFillActorPoolsOnLoad, bool bKeepsMeshes);

    NiPhysXPSParticleSystem(NiPSParticleSystem* pkPSys, 
        NiPhysXPSParticleSystemProp* pkProp, NiUInt32 uiCompartmentID, 
        bool bPhysXSpace, bool bPhysXRotations, NiUInt32 uiActorPoolSize, 
        bool bFillActorPoolsOnLoad, bool bKeepsMeshes);

    // For streaming and cloning only.
    NiPhysXPSParticleSystem();

    // Fill in all the data upon construction
    void SetOnConstruction(NiUInt32 usMaxNumParticles,
        NiPhysXPSParticleSystemProp* pkProp, 
        NiUInt32 uiCompartmentID, NiUInt32 uiActorPoolSize,  
        bool bFillActorPoolOnLoad, bool bKeepsMeshes, bool bPhysXRotations,
        bool bPhysXSpace);

    void AllocateDataBuffers(NiUInt32 uiBufferSize,bool bAllocateColors,
        bool bAllocateRotations, bool bAllocateRotationAxes);

    void FreeDataBuffers();

    // Inherited from NiPSParticleSystem.
    virtual void UpdateParticlesUponCompletion();
    
    // An array of pointers to NxActor objects and a matching
    // array that stores the last know velocities of those actors.
    NxActor** m_ppkActors;
    NiPoint3* m_pkLastVelocities;

    // Arrays to stream data from PhysX to floodgate and from floodgate to
    // PhysX.
    NxMat34* m_pkActorPose;
    NxVec3* m_pkActorVelocity;

    // Information about the PhysX scene
    NiPhysXScene* m_pkScene;
    NiPhysXPSParticleSystemProp* m_pkProp;
    NiPhysXPSParticleSystemDestPtr m_spDest;
    NiUInt32 m_uiCompartmentID;

    // Transformations
    NiTransform m_kPhysXToPSys;
    float m_fScalePToW;

    NiUInt32 m_uiDefaultActorPoolSize;
    NiTPrimitiveArray<NxActor*> m_kActorPool;
    NiPoint3 m_kActorPoolCenter;
    NiPoint3 m_kActorPoolDim;

    NiPhysXActorDescPtr m_spActorDescriptor;

    enum Flags {
        FLAG_KEEP_ACTOR_MESHES = 0x01,
        FLAG_FILL_POOLS_ON_LOAD = 0x02,
        FLAG_PHYSX_ROTATIONS = 0x04,
        FLAG_PHYSX_SPACE = 0x08,
        FLAG_NEEDS_UPDATE = 0x10
    };
};

NiSmartPointer(NiPhysXPSParticleSystem);

#include "NiPhysXPSParticleSystem.inl"

#endif  // #ifndef NIPHYSXPSPARTICLESYSTEM_H
