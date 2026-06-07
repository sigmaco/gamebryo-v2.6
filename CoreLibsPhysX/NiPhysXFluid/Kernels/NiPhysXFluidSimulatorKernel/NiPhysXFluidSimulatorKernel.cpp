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
#include <NiPhysXFluidPCH.h>

#include "NiPhysXFluidSimulatorKernel.h"
#include <NiPSSimulatorKernelHelpers.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPhysXFluidSimulatorKernel)
{
    // Get input streams.
    const NiPhysXFluidSimulatorKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPhysXFluidSimulatorKernelStruct>(0);
    const NxVec3* pkIPositions = kWorkload.GetInput<NxVec3>(1);
    const float* pfIAges = kWorkload.GetInput<float>(2);
    const float* pfILastUpdateTimes = kWorkload.GetInput<float>(3);
    const NiUInt32* puiIFlags = kWorkload.GetInput<NiUInt32>(4);

    // Get output streams.
    NiPoint3* pkOPositions = kWorkload.GetOutput<NiPoint3>(0);
    float* pfOAges = kWorkload.GetOutput<float>(1);
    NiUInt32* puiOFlags = kWorkload.GetOutput<NiUInt32>(2);
    float* pfOLastUpdateTimes = kWorkload.GetOutput<float>(3);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    // Optimize for case where the particle system and PhysX root are
    // in the same coordinate system.
    if (pkIStruct->m_bPhysXSpace)
    {
        for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
        {
            pkOPositions[ui].x =
                pkIPositions[ui].x * pkIStruct->m_fScalePhysXToFluid;
            pkOPositions[ui].y =
                pkIPositions[ui].y * pkIStruct->m_fScalePhysXToFluid;
            pkOPositions[ui].z =
                pkIPositions[ui].z * pkIStruct->m_fScalePhysXToFluid;

            // Update age.
            pfOAges[ui] = pfIAges[ui] +
                (pkIStruct->m_fCurrentTime - pfILastUpdateTimes[ui]);

            // Update particle last update time.
            pfOLastUpdateTimes[ui] = pkIStruct->m_fCurrentTime;

            // Copy particle flags.
            puiOFlags[ui] = puiIFlags[ui];

            // Handle spawning.
            if (pkIStruct->m_bSpawnOnDeath &&
                NiPSFlagsHelpers::GetShouldDie(puiIFlags[ui]))
            {
                // Set the particle to spawn.
                NiPSFlagsHelpers::SetShouldSpawn(puiOFlags[ui], true);
                NiPSFlagsHelpers::SetSpawnerID(
                    puiOFlags[ui], pkIStruct->m_ucSpawnerID);
            }
        }
    }
    else
    {
        for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
        {
            NiPoint3 kPosn(
                pkIPositions[ui].x * pkIStruct->m_fScalePhysXToFluid,
                pkIPositions[ui].y * pkIStruct->m_fScalePhysXToFluid,
                pkIPositions[ui].z * pkIStruct->m_fScalePhysXToFluid);

            pkOPositions[ui] = pkIStruct->m_kPhysXToFluid * kPosn;

            // Update age.
            pfOAges[ui] = pfIAges[ui] +
                (pkIStruct->m_fCurrentTime - pfILastUpdateTimes[ui]);

            // Update particle last update time.
            pfOLastUpdateTimes[ui] = pkIStruct->m_fCurrentTime;

            // Copy particle flags.
            puiOFlags[ui] = puiIFlags[ui];

            // Handle spawning.
            if (pkIStruct->m_bSpawnOnDeath &&
                NiPSFlagsHelpers::GetShouldDie(puiIFlags[ui]))
            {
                // Set the particle to spawn.
                NiPSFlagsHelpers::SetShouldSpawn(puiOFlags[ui], true);
                NiPSFlagsHelpers::SetSpawnerID(
                    puiOFlags[ui], pkIStruct->m_ucSpawnerID);
            }
        }
    }
}
NiSPEndKernelImpl(NiPhysXFluidSimulatorKernel)
//---------------------------------------------------------------------------
