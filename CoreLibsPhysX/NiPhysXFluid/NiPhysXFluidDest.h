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

#ifndef NIPHYSXFLUIDDEST_H
#define NIPHYSXFLUIDDEST_H

#include "NiPhysXFluidLibType.h"
#include "NiPhysXDest.h"
#include "NiPhysXPSFluidEmitter.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#include <fluids/NxFluid.h>

#include "NiPHysXFluidSimulatorStep.h"

class NiPSParticleSystem;
class NiPhysXFluidConverter;

/**
    NiPhysXFluidDest objects are responsible for interfacing between the
    state from the PhysX particles and the Gamebryo particle system that is
    rendering the fluid.
    
    As a destination object, its update function is called upon
    NiPhysXScene::FetchResult when a simulation step completes
    (assuming is it active). The particle system must be updated at least
    once for every simulation step so that the actions performed by this
    object will be processed by the particle system update.

    An object of this type is created and stored by NiPhysXPSFluidSystem and
    NiPhysXPSMeshFluidSystem objects when they are created or their physics
    scene is changed. Applications never should need to create one of these.
    Documentation is provided for cases where the class must be extended.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidDest : public NiPhysXDest
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXFluidDest);
    /// @endcond

public:

    /**
        Constructor.
        
        The particle system provided as the target of this destination must
        be either a NiPhysXFluidSystem or NiPhysXMeshFluidSystem. The emitter
        should be the sole emitter attached to the particle system.
    */
    NiPhysXFluidDest(NiPSParticleSystem* pkTarget, 
        NiPhysXPSFluidEmitter* pkEmitter);
    
    /// Destructor.
    ~NiPhysXFluidDest();

    /// Access the particle system we are driving. 
    NiPSParticleSystem* GetTarget();

    /**
        Return a reference to the transformation from PhysX space to the
        Gamebryo particle system's space.
        
        Use this function in particle simulation steps that might need to know
        it, such as the NiPhysXFluidSimulatorStep class. 
    */
    NiTransform& GetPhysXToFluid();
 
    /// Obtain the most recently cached PhysX scale factor.
    float GetScalePtoW();

    /**
        Update the fluid particle system after a simulation step.
        
        This function caches its arguments for use during particle system
        update. It also emits any newly created fluid particles.
    */
    virtual void UpdateFromActors(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// Required by NiPhysXDest, this function does nothing in this class.
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// @cond EMERGENT_INTERNAL

    void SetEmitter(NiPhysXPSFluidEmitter* pkEmitter);

    virtual void ProcessClone(NiCloningProcess& kCloning);

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    NiPhysXFluidDest();

    void SetUpdateStep();
    
    NiPSParticleSystem* m_pkTarget;
    NiPhysXPSFluidEmitterPtr m_spEmitter;
    NiPhysXFluidSimulatorStepPtr m_spUpdateStep;

    // Cached for updating of particles
    NiTransform m_kPhysXToFluid;
    float m_fScalePToW;

    friend class NiPhysXFluidConverter;
};

NiSmartPointer(NiPhysXFluidDest);

#include "NiPhysXFluidDest.inl"

#endif  // #ifndef NIPHYSXFLUIDDEST_H
