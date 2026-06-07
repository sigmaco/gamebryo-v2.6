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

#ifndef NIPSSIMULATORGENERALKERNEL_H
#define NIPSSIMULATORGENERALKERNEL_H

#include <NiParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiUniversalTypes.h>

/// A structure containing data required by the NiPSSimulatorGeneralKernel
/// Floodgate kernel.
struct NiPSSimulatorGeneralKernelStruct
{
    /// The current simulation time.
    float m_fCurrentTime;

    /// The amount of time in seconds that a particle should grow.
    float m_fGrowTime;

    /// The amount of time in seconds that a particle should shrink.
    float m_fShrinkTime;

    /// The generation at which a particle should grow.
    NiUInt16 m_usGrowGeneration;

    /// The generation at which a particle should shrink.
    NiUInt16 m_usShrinkGeneration;

    /// The number of color keys specified in the input stream.
    NiUInt8 m_ucNumColorKeys;
};

/**
    Simulates particle colliders.

    This Floodgate kernel calls the static helper functions in
    NiPSSimulatorKernelHelpers to grow and shrink particles, to animate the
    color of particles, and to update particle rotation.
*/
NiSPDeclareKernelLib(NiPSSimulatorGeneralKernel, NIPARTICLE_ENTRY)

#endif  // #ifndef NIPSSIMULATORGENERALKERNEL_H
