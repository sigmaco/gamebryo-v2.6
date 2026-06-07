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

#ifndef NIPSSIMULATORCOLLIDERSKERNEL_H
#define NIPSSIMULATORCOLLIDERSKERNEL_H

#include <NiParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiPSColliderDefinitions.h>

/// A structure containing data required by the NiPSSimulatorCollidersKernel
/// Floodgate kernel.
struct NiPSSimulatorCollidersKernelStruct
{
    /// The number of colliders being passed down to the kernel
    unsigned int m_uiColliderCount;

    /// The current simulation time.
    float m_fCurrentTime;

    /// The size of the collider data buffer in bytes.
    size_t m_stDataBufferSize;

    /**
        An array containing range values in bytes for the collider data
        sections in the data buffer.

        A data range is provided here for each type of collider. The data range
        includes from the first byte of the collider data section through the
        16-byte-aligned end of that section. It does not include the 16 byte
        header preceding the data block that specifies the collider type.
    */
    size_t m_astDataRanges[NiPSColliderDefinitions::NUM_COLLIDER_TYPES];
};

/**
    Simulates particle colliders.

    This Floodgate kernel calls the static helper functions in
    NiPSColliderHelpers to simulate particle colliders. The data for each
    collider is provided via a fixed input stream.
*/
NiSPDeclareKernelLib(NiPSSimulatorCollidersKernel, NIPARTICLE_ENTRY)

#endif  // #ifndef NIPSSIMULATORCOLLIDERSKERNEL_H
