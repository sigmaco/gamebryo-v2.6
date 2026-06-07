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

#ifndef NIPHYSXFLUIDSIMULATORKERNEL_H
#define NIPHYSXFLUIDSIMULATORKERNEL_H

#include <NiPhysXFluidLibType.h>
#include <NiSPKernelMacros.h>
#include <NiUniversalTypes.h>
#include <NiTransform.h>

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxSimpleTypes.h>
#include <NxVec3.h>

/// A structure used to pass data to a NiPhysXFluidSimulatorKernel object.
///
/// Applications should never need to create or modify one of these.
struct NiPhysXFluidSimulatorKernelStruct
{
    /// @cond EMERGENT_INTERNAL
    
    /// A scale factor that is always applied to PhysX particle positions.
    float m_fScalePhysXToFluid;
    
    /// The transformation required to take PhysX particles to the particle
    /// system coordinate frame.
    NiTransform m_kPhysXToFluid;

    /// The current simulation time.
    float m_fCurrentTime;

    /// The ID of the spawner to use when spawning on death.
    NiUInt16 m_ucSpawnerID;

    /// True iff the particle system is in the same coordinate frame as
    /// the fluid simulation.
    bool m_bPhysXSpace;

    /// Whether or not particles should be spawned on death.
    bool m_bSpawnOnDeath;

    /// @endcond
};

NiSPDeclareKernelLib(NiPhysXFluidSimulatorKernel, NIPHYSXFLUID_ENTRY)

#endif  // #ifndef NIPHYSXFLUIDSIMULATORKERNEL_H
