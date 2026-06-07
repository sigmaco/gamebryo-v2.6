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

#ifndef NIPSSIMULATORFORCESKERNEL_H
#define NIPSSIMULATORFORCESKERNEL_H

#include <NiParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiPSForceDefinitions.h>

/// A structure containing data required by the NiPSSimulatorForcesKernel
/// Floodgate kernel.
struct NiPSSimulatorForcesKernelStruct
{
    /// The current simulation time.
    float m_fCurrentTime;

    /// The size of the force data buffer in bytes.
    size_t m_stDataBufferSize;

    /**
        An array containing range values in bytes for the force data sections
        in the data buffer.

        A data range is provided here for each type of force. The data range
        includes from the first byte of the force data section through the
        16-byte-aligned end of that section. It does not include the 16 byte
        header preceding the data block that specifies the force type.
    */
    size_t m_astDataRanges[NiPSForceDefinitions::NUM_FORCE_TYPES];
};

/**
    Simulates particle forces.

    This Floodgate kernel calls the static helper functions in
    NiPSForceHelpers to simulate particle forces. The data for each force is
    provided via a fixed input stream.
*/
NiSPDeclareKernelLib(NiPSSimulatorForcesKernel, NIPARTICLE_ENTRY)

#endif  // #ifndef NIPSSIMULATORFORCESKERNEL_H
