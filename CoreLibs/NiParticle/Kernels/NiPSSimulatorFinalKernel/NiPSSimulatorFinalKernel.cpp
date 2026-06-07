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

// Precompiled Header
#include <NiParticlePCH.h>

#include "NiPSSimulatorFinalKernel.h"
#include <NiPSSimulatorKernelHelpers.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPSSimulatorFinalKernel)
{
    // Get input streams.
    const NiPSSimulatorFinalKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPSSimulatorFinalKernelStruct>(0);
    const NiPoint3* pkIPositions = kWorkload.GetInput<NiPoint3>(1);
    const NiPoint3* pkIVelocities = kWorkload.GetInput<NiPoint3>(2);
    const float* pfIAges = kWorkload.GetInput<float>(3);
    const float* pfILifeSpans = kWorkload.GetInput<float>(4);
    const float* pfILastUpdateTimes = kWorkload.GetInput<float>(5);
    const NiUInt32* puiIFlags = kWorkload.GetInput<NiUInt32>(6);

    // Get output streams.
    NiPoint3* pkOPositions = kWorkload.GetOutput<NiPoint3>(0);
    float* pfOAges = kWorkload.GetOutput<float>(1);
    NiUInt32* puiOFlags = kWorkload.GetOutput<NiUInt32>(2);
    float* pfOLastUpdateTimes = kWorkload.GetOutput<float>(3);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
    {
        // Update position.
        NiPSSimulatorKernelHelpers::UpdateParticlePosition(
            pkOPositions[ui],
            pkIPositions[ui],
            pkIVelocities[ui],
            pfILastUpdateTimes[ui],
            pkIStruct->m_fCurrentTime);

        // Update age/death.
        NiPSSimulatorKernelHelpers::UpdateParticleAgeDeath(
            pfOAges[ui],
            pfOLastUpdateTimes[ui],
            puiOFlags[ui],
            pfIAges[ui],
            pfILifeSpans[ui],
            pfILastUpdateTimes[ui],
            puiIFlags[ui],
            pkIStruct->m_fCurrentTime,
            pkIStruct->m_bSpawnOnDeath,
            pkIStruct->m_ucSpawnerID);

        // Only update the last update time if the particle is not set to
        // spawn, in which case the last update time stores the spawn time.
        if (!NiPSFlagsHelpers::GetShouldSpawn(puiOFlags[ui]))
        {
            // Update particle last update time.
            pfOLastUpdateTimes[ui] = pkIStruct->m_fCurrentTime;
        }
    }
}
NiSPEndKernelImpl(NiPSSimulatorFinalKernel)
//---------------------------------------------------------------------------
