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

#ifndef NIPHYSXMESHFLUIDSYSTEM_H
#define NIPHYSXMESHFLUIDSYSTEM_H

#include "NiPhysXFluidLibType.h"

#include <NiMeshParticleSystem.h>

#include "NiPhysXFluidDest.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <fluids/NxFluid.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. Its functionality has
    been replaced with the NiPhysXPSMeshFluidSystem class.
*/
class NIPHYSXFLUID_ENTRY NiPhysXMeshFluidSystem : public NiMeshParticleSystem
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond
public:
    
    /// @cond EMERGENT_INTERNAL
    bool GetPhysXSpace() const;

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    // For streaming and cloning only.
    NiPhysXMeshFluidSystem();
    
    NiPhysXFluidDestPtr m_spDestination;

    bool m_bPhysXSpace;
};

NiSmartPointer(NiPhysXMeshFluidSystem);

/// @endcond

#include "NiPhysXMeshFluidSystem.inl"

#endif  // #ifndef NIPHYSXMESHFLUIDSYSTEM_H
