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

#ifndef NIPHYSXPSPARTICLESYSTEMDEST_H
#define NIPHYSXPSPARTICLESYSTEMDEST_H

#include "NiPhysXParticleLibType.h"

#include "NiPhysXDest.h"

class NiPSParticleSystem;

/**
    NiPhysXPSParticleSystemDest objects are responsible for interfacing
    between the PhysX scene and the Gamebryo particle system.
    
    As a destination object, its update function is called upon
    NiPhysXScene::UpdateDestinations (assuming is it active).

    An object of this type is created and stored by NiPhysXPSParticleSystem and
    NiPhysXPSMeshParticleSystem objects when they are created or their physics
    scene is changed. Applications never should need to create one of these.
    Documentation is provided for cases where the class must be extended.
*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSParticleSystemDest : public NiPhysXDest
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    /// @endcond

public:
    /**
        Constructor.
        
        The particle system provided as the target of this destination must
        be either a NiPhysXPSParticleSystem or NiPhysXPSMeshParticleSystem.
    */
    NiPhysXPSParticleSystemDest(NiPSParticleSystem* pkSource);

    /// Destructor.
    ~NiPhysXPSParticleSystemDest();

    /// Access the particle system this object is working with. 
    NiPSParticleSystem* GetTarget();
    
    /// Update state in the particle system after a simulation step.
    ///
    /// This sets the transformations and the NeedsUpdate flag on the target.
    virtual void UpdateFromActors(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// Required by NiPhysXDest, this function does nothing in this class.
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    // For streaming only
    NiPhysXPSParticleSystemDest();
    
    NiPSParticleSystem* m_pkTarget;
};

NiSmartPointer(NiPhysXPSParticleSystemDest);

#include "NiPhysXPSParticleSystemDest.inl"

#endif  // #ifndef NIPHYSXPSPARTICLESYSTEMDEST_H
