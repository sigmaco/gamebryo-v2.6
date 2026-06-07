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

#ifndef NIPHYSXPARTICLESYSTEM_H
#define NIPHYSXPARTICLESYSTEM_H

#include "NiPhysXParticleLibType.h"

#include <NiParticleSystem.h>

#include "NiPhysXPSysData.h"
#include "NiPhysXPSysDest.h"
#include "NiPhysXPSysSrc.h"
#include "NiPhysXPSysProp.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXPSParticleSystem class.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXParticleSystem : public NiParticleSystem
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    /// Destructor.
    ~NiPhysXParticleSystem();

    /// @cond EMERGENT_INTERNAL

    NiPhysXPSysPropPtr GetProp() const;

    NiPhysXScenePtr GetScene() const;

    bool GetPhysXSpace() const;

    virtual bool StreamCanSkip();

    virtual void PostLinkObject(NiStream& kStream);

    /// @endcond

protected:
    // For streaming only.
    NiPhysXParticleSystem();
    
    NiPhysXPSysPropPtr m_spProp;
    NiPhysXPSysDestPtr m_spDestination;
    NiPhysXPSysSrcPtr m_spSource;

    bool m_bPhysXSpace;
};

NiSmartPointer(NiPhysXParticleSystem);

#include "NiPhysXParticleSystem.inl"

/// @endcond

#endif  // #ifndef NIPHYSXPARTICLESYSTEM_H
