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
#include <NiPhysXParticlePCH.h>

#include "NiPhysXPSSimulatorFinalKernel.h"
#include <NiPSSimulatorKernelHelpers.h>
#include <NxVec3.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPhysXPSSimulatorFinalKernel)
{
    // Get input streams.
    const NiPhysXPSSimulatorFinalKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPhysXPSSimulatorFinalKernelStruct>(0);
    const NiPoint3* pkIVelocities = kWorkload.GetInput<NiPoint3>(1);
    const NiPoint3* pkILastVelocities = kWorkload.GetInput<NiPoint3>(2);
    const float* pfIAges = kWorkload.GetInput<float>(3);
    const float* pfILifeSpans = kWorkload.GetInput<float>(4);
    const float* pfILastUpdateTimes = kWorkload.GetInput<float>(5);
    const NiUInt32* puiIFlags = kWorkload.GetInput<NiUInt32>(6);

    // Get output streams.
    float* pfOAges = kWorkload.GetOutput<float>(0);
    NiUInt32* puiOFlags = kWorkload.GetOutput<NiUInt32>(1);
    float* pfOLastUpdateTimes = kWorkload.GetOutput<float>(2);
    NxVec3* pkOActorVelocityChange = kWorkload.GetOutput<NxVec3>(3);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    // Optimize for same coordinate system.
    if (pkIStruct->m_bPhysXSpace)
    {
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            // Update age/death.
            NiPSSimulatorKernelHelpers::UpdateParticleAgeDeath(pfOAges[ui], 
                pfOLastUpdateTimes[ui], puiOFlags[ui], pfIAges[ui], 
                pfILifeSpans[ui], pfILastUpdateTimes[ui], puiIFlags[ui], 
                pkIStruct->m_fCurrentTime, pkIStruct->m_bSpawnOnDeath, 
                pkIStruct->m_ucSpawnerID);

            // Only update the last update time if the particle is not set to
            // spawn, in which case the last update time stores the spawn time.
            if (!NiPSFlagsHelpers::GetShouldSpawn(puiOFlags[ui]))
            {
                // Update particle last update time.
                pfOLastUpdateTimes[ui] = pkIStruct->m_fCurrentTime;
            }        

            NiPoint3 kVelChange = (pkIVelocities[ui] - pkILastVelocities[ui]) 
                * pkIStruct->m_fScaleWToP;

            pkOActorVelocityChange[ui].set(kVelChange.x, kVelChange.y, 
                kVelChange.z);
        }
        return;
    }

    for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
    {
        // Update age/death.
        NiPSSimulatorKernelHelpers::UpdateParticleAgeDeath(pfOAges[ui], 
            pfOLastUpdateTimes[ui], puiOFlags[ui], pfIAges[ui], 
            pfILifeSpans[ui], pfILastUpdateTimes[ui], puiIFlags[ui], 
            pkIStruct->m_fCurrentTime, pkIStruct->m_bSpawnOnDeath,
            pkIStruct->m_ucSpawnerID);

        // Only update the last update time if the particle is not set to
        // spawn, in which case the last update time stores the spawn time.
        if (!NiPSFlagsHelpers::GetShouldSpawn(puiOFlags[ui]))
        {
            // Update particle last update time.
            pfOLastUpdateTimes[ui] = pkIStruct->m_fCurrentTime;
        } 

        NiPoint3 kVelChange = (pkIVelocities[ui] - pkILastVelocities[ui]) * 
            pkIStruct->m_fScaleWToP;

        kVelChange = pkIStruct->m_kPSysToPhysX.m_Rotate * kVelChange;

        pkOActorVelocityChange[ui].set(kVelChange.x, kVelChange.y, 
            kVelChange.z);
    }
}
NiSPEndKernelImpl(NiPhysXPSSimulatorFinalKernel)
//---------------------------------------------------------------------------
