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

#ifndef NIPHYSXPSSIMULATORFINALKERNEL_H
#define NIPHYSXPSSIMULATORFINALKERNEL_H

#include <NiPhysXParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiUniversalTypes.h>
#include <NiTransform.h>

/// @cond EMERGENT_INTERNAL
struct NiPhysXPSSimulatorFinalKernelStruct
{
    // The current simulation time.
    float m_fCurrentTime;

    // Whether or not particles should be spawned on death.
    bool m_bSpawnOnDeath;

    // The ID of the spawner to use when spawning on death.
    NiUInt16 m_ucSpawnerID;

    NiTransform m_kPSysToPhysX;
    float m_fScaleWToP;
    bool m_bPhysXSpace;
    bool m_bPhysXRotations;
};
/// @endcond

NiSPDeclareKernelLib(NiPhysXPSSimulatorFinalKernel, NIPHYSXPARTICLE_ENTRY)

#endif  // #ifndef NIPHYSXPSSIMULATORFINALKERNEL_H
