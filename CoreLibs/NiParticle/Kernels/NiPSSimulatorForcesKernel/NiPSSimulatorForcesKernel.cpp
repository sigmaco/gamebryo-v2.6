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

#include "NiPSSimulatorForcesKernel.h"
#include <NiPSForceHelpers.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPSSimulatorForcesKernel)
{
#ifdef __SPU__
    // Seed the random number generator with the current time to allow for
    // pseudorandom generation on the SPU.
    NiSrand(spu_read_decrementer());
#endif

    // Get input streams.
    const NiPSSimulatorForcesKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPSSimulatorForcesKernelStruct>(0);
    const NiUInt8* pucIData = kWorkload.GetInput<NiUInt8>(1);
    const float* pfILastUpdateTimes = kWorkload.GetInput<float>(2);
    const NiPoint3* pkIPositions = kWorkload.GetInput<NiPoint3>(3);
    const NiPoint3* pkIVelocities = kWorkload.GetInput<NiPoint3>(4);

    // Get output streams.
    NiPoint3* pkOVelocities = kWorkload.GetOutput<NiPoint3>(0);

    // Get block count.
    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();

    // Apply active forces. Iteration over the particles is performed by each
    // force function to reduce per-particle conditionals.
    size_t stDataOffset = 0;
    bool bFirstIteration = true;
    while (stDataOffset < pkIStruct->m_stDataBufferSize)
    {
        // Get force type. The force type is contained in pucIData immediately
        // followed by the data struct for that force.
        NiUInt32 uiForceType = *((NiUInt32*) (pucIData + stDataOffset));
        stDataOffset += 16;

        // Apply the force.
        switch (uiForceType)
        {
            case NiPSForceDefinitions::FORCE_BOMB:
            {
                const NiPSForceDefinitions::BombData* pkBombData =
                    (const NiPSForceDefinitions::BombData*)
                    (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyBombForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkBombData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_DRAG:
            {
                const NiPSForceDefinitions::DragData* pkDragData =
                    (const NiPSForceDefinitions::DragData*)
                    (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyDragForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkDragData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_AIR_FIELD:
            {
                const NiPSForceDefinitions::AirFieldData* pkAirFieldData =
                    (const NiPSForceDefinitions::AirFieldData*)
                    (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyAirFieldForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkAirFieldData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_DRAG_FIELD:
            {
                const NiPSForceDefinitions::DragFieldData* pkDragFieldData =
                    (const NiPSForceDefinitions::DragFieldData*)
                    (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyDragFieldForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkDragFieldData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_GRAVITY_FIELD:
            {
                const NiPSForceDefinitions::GravityFieldData*
                    pkGravityFieldData =
                        (const NiPSForceDefinitions::GravityFieldData*)
                        (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyGravityFieldForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkGravityFieldData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_RADIAL_FIELD:
            {
                const NiPSForceDefinitions::RadialFieldData*
                    pkRadialFieldData =
                        (const NiPSForceDefinitions::RadialFieldData*)
                        (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyRadialFieldForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkRadialFieldData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_TURBULENCE_FIELD:
            {
                const NiPSForceDefinitions::TurbulenceFieldData*
                    pkTurbulenceFieldData =
                        (const NiPSForceDefinitions::TurbulenceFieldData*)
                        (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyTurbulenceFieldForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkTurbulenceFieldData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_VORTEX_FIELD:
            {
                const NiPSForceDefinitions::VortexFieldData*
                    pkVortexFieldData =
                        (const NiPSForceDefinitions::VortexFieldData*)
                        (pucIData + stDataOffset);
                NiPSForceHelpers::ApplyVortexFieldForce(
                    uiBlockCount,
                    pkOVelocities,
                    pfILastUpdateTimes,
                    pkIPositions,
                    pkIVelocities,
                    *pkVortexFieldData,
                    pkIStruct->m_fCurrentTime);
                break;
            }
            case NiPSForceDefinitions::FORCE_GRAVITY:
            {
                const NiPSForceDefinitions::GravityData* pkGravityData =
                    (const NiPSForceDefinitions::GravityData*)
                    (pucIData + stDataOffset);

                switch (pkGravityData->m_eForceType)
                {
                    case NiPSForceDefinitions::GravityData::FORCE_PLANAR:
                        NiPSForceHelpers::ApplyPlanarGravityForce(
                            uiBlockCount,
                            pkOVelocities,
                            pfILastUpdateTimes,
                            pkIPositions,
                            pkIVelocities,
                            *pkGravityData,
                            pkIStruct->m_fCurrentTime);
                        break;
                    case NiPSForceDefinitions::GravityData::FORCE_SPHERICAL:
                        NiPSForceHelpers::ApplySphericalGravityForce(
                            uiBlockCount,
                            pkOVelocities,
                            pfILastUpdateTimes,
                            pkIPositions,
                            pkIVelocities,
                            *pkGravityData,
                            pkIStruct->m_fCurrentTime);
                        break;
                    default:
                        // Unknown force type.
                        break;
                }

                break;
            }
            default:
                // Unknown force. Copy the input velocity data to the output
                // buffer.
                for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
                {
                    pkOVelocities[ui] = pkIVelocities[ui];
                }
                break;
        }

        // Increment the data offset to point to the next force.
        stDataOffset += pkIStruct->m_astDataRanges[uiForceType];

        // For all iterations of this loop after the first one, the input
        // velocities buffer should really be pointing to the output buffer,
        // since the output buffer is what holds the most current value.
        if (bFirstIteration)
        {
            pkIVelocities = pkOVelocities;
            bFirstIteration = false;
        }
    }

    if (bFirstIteration)
    {
        // No forces applied. Copy the input velocity data to the output
        // buffer.
        for (NiUInt32 ui = 0; ui < uiBlockCount; ++ui)
        {
            pkOVelocities[ui] = pkIVelocities[ui];
        }
    }
}
NiSPEndKernelImpl(NiPSSimulatorForcesKernel)
//---------------------------------------------------------------------------
