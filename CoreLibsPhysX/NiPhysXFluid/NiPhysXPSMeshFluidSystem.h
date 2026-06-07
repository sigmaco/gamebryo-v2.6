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

#ifndef NIPHYSXPSMESHFLUIDSYSTEM_H
#define NIPHYSXPSMESHFLUIDSYSTEM_H

#include "NiPhysXFluidLibType.h"

#include <NiPSMeshParticleSystem.h>
#include "NiPhysXPSSimulator.h"
#include <NiPhysXProp.h>
#include "NiPhysXFluidEmitterDesc.h"
#include "NiPhysXFluidDest.h"
#include "NiPhysXPSFluidEmitter.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <fluids/NxFluid.h>

/**
    NiPhysXPSMeshFluidSystem objects are the scene graph objects for a fluid
    particle system rendered with meshes.
    
    The class is derived from, and borrows almost all functionality from,
    the NiPSMeshParticleSystem class.
    
    PhysX fluid particle systems can make use of all particle system
    functionality except that the emission, positions and life spans
    of particles are determined entirely by the PhysX fluid simulation.
*/
class NIPHYSXFLUID_ENTRY NiPhysXPSMeshFluidSystem : 
    public NiPSMeshParticleSystem
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareClone(NiPhysXPSMeshFluidSystem);
    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    virtual ~NiPhysXPSMeshFluidSystem();

    /**
        Construct a fluid particle system and return the new object.
        
        The constructed object uses smart pointers for all of the arguments
        passed by pointer, so applications should not explicitly delete that
        data.
        
        The function returns NULL if, for some reason, construction fails.
        
        @param pkEmitter The emitter to use for this system.
        @param uiMaxNumParticles The maximum number of particles supported
            by the PhysX particle system.
        @param bHasColors True if this particle system has per-particle colors.
            See NiPSParticleSystem for details.
        @param bHasRotations True if this particle system support particle
            rotation. See NiPSParticleSystem for details.
        @param bWorldSpace True if the particles should be considered
            in world space once emitted. See NiPSParticleSystem for details.
        @param bDynamicBounds True if the particle system should dynamically
            update the bounds per-frame. This can be expensive, so static
            bounds are recommended for systems with many particles.
            See NiPSParticleSystem for details.
        @param bAttachMeshModifiers If true, any mesh modifiers associated
            with the particle system will be attached. See NiPSParticleSystem
            for details.
        @param uiPoolSize The maximum number of meshes in the memory pool
            used to buffer un-used particle meshes. See NiPSMeshParticleSystem
            for details.
        @param usNumGenerations The number of particle generations for which
            meshes are provided. See NiPSMeshParticleSystem for details.
        @param bFillPoolsOnLoad If true, the mesh pool is filled upon creation
            of the particle system, trading a large initial computation for
            many subsequent per-frame computations.
        @param pkProp The NiPhysXProp object that this system is associated
            with. That is, the prop that is managing the PhysX fluid system.
        @param bPhysXSpace If true, the Gamebryo and PhysX particles are
            considered to be in the same coordinate system, so only the
            NiPhysXScene object's scale factor is applied (no translation
            or rotation of the particles from PhysX to Gamebryo). This is
            more efficient but should only be used when the systems really are
            in the same coordinate system.
        @param fExpansionFactor On any given frame, new fluid
            particles are created before dead ones are removed, so the
            Gamebryo system must have enough extra particles to cover the
            maxiumum number of particles that might be emitted on a frame.
            This multiplier is applied to the uiMaxNumParticles value to set
            the maximum number of Gamebryo particles.
    */
    static NiPhysXPSMeshFluidSystem* Create(NiPhysXPSFluidEmitter* pkEmitter,
        NiUInt32 uiMaxNumParticles, 
        bool bHasColors, bool bHasRotations,  bool bWorldSpace = true, 
        bool bDynamicBounds = true, bool bAttachMeshModifiers = true,
        NiUInt32 uiPoolSize = NiPSMeshParticleSystem::DEFAULT_POOL_SIZE,
        NiUInt16 usNumGenerations = 1, bool bFillPoolsOnLoad = false, 
        NiPhysXProp* pkProp = 0, bool bPhysXSpace = false, 
        float fExpansionFactor = 2.0f);

    /**
        Obtain the PhysX fluid data used by this object.
        
        The data object returned must be used by the NxFluid system that is
        providing the particle data. The buffers in the data are owned by this
        object and are set at construction.
    */
    NxParticleData& GetFluidData();

    /**
        Get the particle creation data, which is used to notify the system
        of newly created PhysX particles.
        
        The data object returned must be used by the NxFluid system that is
        providing the particle data. The buffers in the data are owned by this
        object and are set at construction.
    */
    NxParticleIdData& GetParticleCreationData();

    /**
        Get the particle deletion data, which is used to notify the system
        of recently deleted PhysX particles.
        
        The data object returned must be used by the NxFluid system that is
        providing the particle data. The buffers in the data are owned by this
        object and are set at construction.
    */
    NxParticleIdData& GetParticleDeletionData();

    /**
        Associate this particle system with a NiPhysXProp object that will be
        used to interface between Gamebryo and PhysX.
    
        Call this function only when creating a fluid particle system in code,
        or when a system is cloned but the prop it is part of is not cloned.

        Particle systems created using the Gamebryo art tools will already
        be associated with a prop, so this function does not need to be called.
        Also, the function does not need to be called if an entire prop
        containing fluid systems is cloned.
    */
    void AttachToProp(NiPhysXProp* pkProp);

    /// Disassociate this particle system from its NiPhysXProp object.
    ///
    /// The given prop must be the same one that was used with AttachToProp.
    void DetachFromProp(NiPhysXProp* pkProp);

    /// Return the flag indicating whether or not this particle system
    /// uses the same coordinate system as the PhysX simulation.
    bool GetUsesPhysXSpace();

    /// Set the flag indicating whether or not this particle system
    /// uses the same coordinate system as the PhysX simulation.
    void SetUsesPhysXSpace(const bool bPhysXSpace);

    /// @cond EMERGENT_INTERNAL

    NiUInt32* GetPhysXToGBMap();

    NxVec3* GetPositionsBuffer();

    NiUInt32 GetMaxNumPhysXParticles();
    
    NiPhysXPSFluidEmitter* GetFluidEmitter();

    void SetFluidEmitter(NiPhysXPSFluidEmitter* pkFluidEmitter);

    void SetDestination(NiPhysXFluidDest* pkFluidDest);

    virtual void InitializeParticle(NiUInt32 uiNewParticle);
    virtual void RemoveParticle(NiUInt32 uiIndexToRemove);

    // Constructor that converts from a Gamebryo NiPSParticleSystem
    static NiPhysXPSMeshFluidSystem* Create(NiPSMeshParticleSystem *pkPSys,
        NiPhysXProp* pkProp, bool bPhysXSpace,
        NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap,
        float fExpansionFactor);

    // The transformation to go from PhysX space to the fluid system space.
    NiTransform& GetPhysXToFluid();
    float GetScalePhysXToFluid();
    
    virtual void ProcessClone(NiCloningProcess& kCloning);

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    // For streaming and cloning only.
    NiPhysXPSMeshFluidSystem();
    NiPhysXPSMeshFluidSystem(const NiPhysXPSMeshFluidSystem&);

    NiPhysXPSMeshFluidSystem(NiPhysXPSFluidEmitter* pkEmitter, 
        NiPhysXPSSimulator* pkSimulator,
        NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations, 
        bool bWorldSpace, NiPSBoundUpdater* pkBoundUpdater, 
        NiUInt32 uiPoolSize, NiUInt16 usNumGenerations, bool bFillPoolsOnLoad,
        NiPhysXProp* pkProp, bool bPhysXSpace, float fExpansionFactor);

    NiPhysXPSMeshFluidSystem(NiPSMeshParticleSystem *pkPSys, 
        NiPhysXProp* pkProp, bool bPhysXSpace, 
        NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap, 
        float fExpansionFactor);
    
    void OnCreation();
    void OnDeletion();
    
    NiPhysXFluidDestPtr m_spDestination;

    NiPhysXPSFluidEmitterPtr m_spFluidEmitter;

    NiUInt32 m_uiMaxNumPhysXParticles;

    // Principle fluid data from PhysX. Particles are ordered arbitrarily.
    NxParticleData m_kFluidData;
    
    // Data for newly created particles in the simulation.
    NxParticleIdData m_kFluidCreationData;
    
    // Data for newly deleted particles in the simulation.
    NxParticleIdData m_kFluidDeletionData;
    
    // Buffer storing PhysX particle locations in Gamebryo order.
    NxVec3* m_pkPositionsBuffer;
    
    // Mapping from PhysX IDs to Gamebryo indexes.
    NiUInt32* m_puiPhysXToGBMap;

    // Mapping from Gamebryo indexes to PhysX IDs.
    NiUInt32* m_puiGBToPhysXMap;

    bool m_bPhysXSpace;
};

NiSmartPointer(NiPhysXPSMeshFluidSystem);

#include "NiPhysXPSMeshFluidSystem.inl"

#endif  // #ifndef NIPHYSXPSMESHFLUIDSYSTEM_H
