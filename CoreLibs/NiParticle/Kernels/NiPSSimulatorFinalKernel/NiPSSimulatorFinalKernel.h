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

#ifndef NIPSSIMULATORFINALKERNEL_H
#define NIPSSIMULATORFINALKERNEL_H

#include <NiParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiUniversalTypes.h>

/// A structure containing data required by the NiPSSimulatorFinalKernel
/// Floodgate kernel.
struct NiPSSimulatorFinalKernelStruct
{
    /// The current simulation time.
    float m_fCurrentTime;

    /// The ID of the spawner to use when spawning on death.
    NiUInt16 m_ucSpawnerID;

    /// Whether or not particles should be spawned on death.
    bool m_bSpawnOnDeath;
};

/**
    Performs particle simulation.

    This Floodgate kernel calls the static helper functions in
    NiPSSimulatorKernelHelpers to update the particle positions and ages,
    killing off particles whose ages have exceeded their life spans. As a
    final step, this kernel also sets the last update time for each particle
    to the current simulation time.
*/
NiSPDeclareKernelLib(NiPSSimulatorFinalKernel, NIPARTICLE_ENTRY)

#endif  // #ifndef NIPSSIMULATORFINALKERNEL_H
