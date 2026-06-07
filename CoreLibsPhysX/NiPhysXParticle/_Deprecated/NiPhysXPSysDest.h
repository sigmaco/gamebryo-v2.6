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

#ifndef NIPHYSXPSYSDEST_H
#define NIPHYSXPSYSDEST_H

#include "NiPhysXParticleLibType.h"

#include <NiParticle.h>

#include "NiPhysXDest.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>

class NiParticleSystem;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXPSSimulatorInitialStep class and other particle system classes.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXPSysDest : public NiPhysXDest
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;
    
    /// @endcond

public:
    /// Destructor.
    ~NiPhysXPSysDest();

    /// Obtain the target particle system for this destination.
    NiParticleSystem* GetTarget();

    /// @cond EMERGENT_INTERNAL

    // Update functions
    virtual void UpdateFromActors(const float fT, 
        const NiTransform& kRootTransform, const float fScalePToW, 
        const bool bForce);
    virtual void UpdateSceneGraph(const float fT, 
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    NiPhysXPSysDest();
    
    NiParticleSystem* m_pkTarget;
};

NiSmartPointer(NiPhysXPSysDest);

#include "NiPhysXPSysDest.inl"

/// @endcond

#endif  // #ifndef NIPHYSXPSYSDEST_H
