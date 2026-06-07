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

#ifndef NIPHYSXMESHPARTICLESYSTEM_H
#define NIPHYSXMESHPARTICLESYSTEM_H

#include "NiPhysXParticleLibType.h"

#include <NiMeshParticleSystem.h>

#include "NiPhysXMeshPSysData.h"
#include "NiPhysXPSysDest.h"
#include "NiPhysXMeshPSysSrc.h"
#include "NiPhysXMeshPSysProp.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXPSMeshParticleSystem class.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXMeshParticleSystem :
    public NiMeshParticleSystem
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:       
    /// Destructor.
    virtual ~NiPhysXMeshParticleSystem();

    /// @cond EMERGENT_INTERNAL

    NiPhysXMeshPSysPropPtr GetProp() const;

    NiPhysXScenePtr GetScene() const;

    bool GetUsesPhysXRotations() const;

    bool GetPhysXSpace() const;

    virtual bool StreamCanSkip();

    virtual void PostLinkObject(NiStream& kStream);

    /// @endcond
    
protected:
    // For streaming only.
    NiPhysXMeshParticleSystem();

    NiPhysXMeshPSysPropPtr m_spProp;
    NiPhysXPSysDestPtr m_spDestination;
    NiPhysXMeshPSysSrcPtr m_spSource;

    bool m_bPhysXRotations;
    bool m_bPhysXSpace;
};

NiSmartPointer(NiPhysXMeshParticleSystem);

#include "NiPhysXMeshParticleSystem.inl"

/// @endcond

#endif  // #ifndef NIPHYSXMESHPARTICLESYSTEM_H
