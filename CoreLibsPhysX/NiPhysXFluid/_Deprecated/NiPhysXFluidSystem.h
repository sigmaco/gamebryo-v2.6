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

#ifndef NIPHYSXFLUIDSYSTEM_H
#define NIPHYSXFLUIDSYSTEM_H

#include "NiPhysXFluidLibType.h"

#include <NiParticleSystem.h>

#include "NiPhysXFluidDest.h"
#include "NiPhysXFluidDesc.h"

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <fluids/NxFluid.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. Its functionality has
    been replaced, approximately, with the NiPhysXPSFluidSystem class.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidSystem : public NiParticleSystem
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:
    
    /// @cond EMERGENT_INTERNAL

    bool GetPhysXSpace() const;
   
    /// @endcond

protected:
    // For streaming and cloning only.
    NiPhysXFluidSystem();
    
    NiPhysXFluidDestPtr m_spDestination;

    bool m_bPhysXSpace;
};

NiSmartPointer(NiPhysXFluidSystem);

/// @endcond

#include "NiPhysXFluidSystem.inl"

#endif  // #ifndef NIPHYSXFLUIDSYSTEM_H
