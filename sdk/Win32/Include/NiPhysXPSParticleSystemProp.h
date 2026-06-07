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

#ifndef NIPHYSXPSPARTICLESYSTEMPROP_H
#define NIPHYSXPSPARTICLESYSTEMPROP_H

#include "NiPhysXParticleLibType.h"

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXProp.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>

class NiPhysXPSParticleSystem;

/**
    NiPhysXPSParticleSystemProp objects hold information about a PhysX
    particle system prop.
    
    Each prop stores an array of physics-based particle systems that are
    part of the prop, and implement callback functions to attach the systems
    to a scene, or remove them from a scene.

    Particle systems are added to the prop via the function
    NiPhysXPSParticleSystem::AttachToProp, and functionality is provided
    here only to access the particle systems.

*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSParticleSystemProp : public NiPhysXProp
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareStream;
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXPSParticleSystemProp);

    /// @endcond

public:
    /// Constructor creates an empty prop.
    NiPhysXPSParticleSystemProp();

    /// Destructor.
    ~NiPhysXPSParticleSystemProp();
    
    /// Returns the number of array entries to iterate over to get all
    /// non-zero particle systems.
    NiUInt32 GetSystemCount() const;

    /// Obtain the particle system at the specified array index.
    ///
    /// May return 0. Treat the returned piointer as a smart pointer.
    NiPhysXPSParticleSystem* GetSystemAt(NiUInt32 uiIndex);
       
    /// @cond EMERGENT_INTERNAL

    // Functions called upon attachment and detachment from a scene
    virtual void AttachSceneCallback(NiPhysXScene* pkScene);
    virtual void DetachSceneCallback(NiPhysXScene* pkScene);
    
    // Should only be called via NiPhysXMeshParticleSystem::AttachToProp
    void AddSystem(NiPhysXPSParticleSystem* pkSystem);
    void RemoveSystem(NiPhysXPSParticleSystem* pkSystem);

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// @endcond
            
protected:
    NiTPrimitiveArray<NiPhysXPSParticleSystem*> m_kSystems;
};

NiSmartPointer(NiPhysXPSParticleSystemProp);

#endif  // #ifndef NIPHYSXPSPARTICLESYSTEMPROP_H

