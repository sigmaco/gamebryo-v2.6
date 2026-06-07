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

#include "NiPSSimulatorGeneralKernel.h"
#include <NiPSSimulatorKernelHelpers.h>
#include <NiPSFlagsHelpers.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPSSimulatorGeneralKernel)
{
    // Get input streams.
    const NiPSSimulatorGeneralKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPSSimulatorGeneralKernelStruct>(0);
    const NiPSKernelColorKey* pkIColorKeys =
        kWorkload.GetInput<NiPSKernelColorKey>(1);
    const float* pfILastUpdateTimes = kWorkload.GetInput<float>(2);
    const NiUInt32* puiIFlags = kWorkload.GetInput<NiUInt32>(3);
    const float* pfIAges = kWorkload.GetInput<float>(4);
    const float* pfILifeSpans = kWorkload.GetInput<float>(5);
    const float* pfIRotAngles = kWorkload.GetInput<float>(6);
    const float* pfIRotSpeeds = kWorkload.GetInput<float>(7);

    // Get output streams.
    float* pfOSizes = kWorkload.GetOutput<float>(0);
    NiRGBA* pkOColors = kWorkload.GetOutput<NiRGBA>(1);
    float* pfORotAngles = kWorkload.GetOutput<float>(2);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    // Simulate particles.
    for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
    {
        // Update grow/shrink size.
        NiPSSimulatorKernelHelpers::UpdateParticleGrowShrink(
            pfOSizes[ui],
            NiPSFlagsHelpers::GetGeneration(puiIFlags[ui]),
            pfIAges[ui],
            pfILifeSpans[ui],
            pkIStruct->m_fGrowTime,
            pkIStruct->m_fShrinkTime,
            pkIStruct->m_usGrowGeneration,
            pkIStruct->m_usShrinkGeneration);

        if (pkOColors && pkIColorKeys)
        {
            // Update color animation.
            NiPSSimulatorKernelHelpers::UpdateParticleColor(
                pkOColors[ui],
                pfIAges[ui],
                pfILifeSpans[ui],
                pkIColorKeys,
                pkIStruct->m_ucNumColorKeys);
        }

        if (pfIRotAngles && pfIRotSpeeds)
        {
            // Update rotation angle.
            NiPSSimulatorKernelHelpers::UpdateParticleRotation(
                pfORotAngles[ui],
                pfIRotAngles[ui],
                pfIRotSpeeds[ui],
                pkIStruct->m_fCurrentTime - pfILastUpdateTimes[ui]);
        }
    }
}
NiSPEndKernelImpl(NiPSSimulatorGeneralKernel)
//---------------------------------------------------------------------------
