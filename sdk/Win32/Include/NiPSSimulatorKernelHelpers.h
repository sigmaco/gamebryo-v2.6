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

#ifndef NIPSSIMULATORKERNELHELPERS_H
#define NIPSSIMULATORKERNELHELPERS_H

#include "NiParticleLibType.h"
#include "NiPSKernelDefinitions.h"
#include <NiUniversalTypes.h>
#include <NiPoint3.h>
#include <NiColor.h>

/**
    Contains static helper functions used for various particle simulation
    tasks.

    The NiPSSimulatorGeneralKernel and NiPSSimulatorFinalKernel Floodgate
    kernels call the static helper functions contained in this class to
    implement various aspects of the particle simulation.

    Due to the implementation of Floodgate kernels on different platforms, it
    is necessary that all output (non-const) parameters to these functions get
    set to some value. It is not valid to simply not set an output value, as
    garbage memory could result on some platforms.
*/
class NIPARTICLE_ENTRY NiPSSimulatorKernelHelpers
{
public:
    /**
        Updates a particle's new position based on its current position, its
        velocity, and the time delta since the last update.

        @param kOPosition The computed new position of the particle.
        @param kIPosition The current position of the particle.
        @param kIVelocity The velocity of the particle.
        @param fILastUpdate The last time at which the particle was updated.
        @param fCurrentTime The current simulation time.
    */
    static void UpdateParticlePosition(
        NiPoint3& kOPosition,
        const NiPoint3& kIPosition,
        const NiPoint3& kIVelocity,
        const float fILastUpdate,
        const float fCurrentTime);

    /**
        Calculates the size parameter for a particle based on its grow/shrink
        settings.

        The size value for a particle ranges from 0.0 to 1.0 and is multiplied
        by the particle radius to calculate the final particle size.

        @param fOSize The computed particle size. Will be between 0.0 and 1.0.
        @param usIGeneration The generation of the particle.
        @param fIAge The age of the particle.
        @param fILifeSpan The life span of the particle.
        @param fGrowTime The time in seconds during which the particle size
            will be ramped from 0.0 to 1.0.
        @param fShrinkTime The time in seconds during which the particle size
            will be ramped from 1.0 to 0.0.
        @param usGrowGeneration The generation during which the particle will
            grow. If the particle's generation is different from this value,
            no grow computation will be applied.
        @param usShrinkGeneration The generation during which the particle
            will shrink. If the particle's generation is different from this
            value, no shrink computation will be applied.
    */
    static void UpdateParticleGrowShrink(
        float& fOSize,
        const NiUInt16 usIGeneration,
        const float fIAge,
        const float fILifeSpan,
        const float fGrowTime,
        const float fShrinkTime,
        const NiUInt16 usGrowGeneration,
        const NiUInt16 usShrinkGeneration);

    /**
        Performs color animation for a particle.

        A color key array is used to determine the color of a particle
        throughout its life time. The color key array should have a start time
        of 0.0 and an end time of 1.0. Color keys are linearly interpolated.

        @param kOColor The computed color for the particle.
        @param fIAge The age of the particle.
        @param fILifeSpan The life span of the particle.
        @param pkColorKeys An array of color keys that specify color
            animation over the lifetime of the particle.
        @param ucNumColorKeys The number of color keys in the pkColorKeys
            array.
    */
    static void UpdateParticleColor(
        NiRGBA& kOColor,
        const float fIAge,
        const float fILifeSpan,
        const NiPSKernelColorKey* pkColorKeys,
        const NiUInt8 ucNumColorKeys);

    /**
        Updates the rotation angle for a particle based on its rotation speed.
        
        @param fORotAngle The computed rotation angle for the particle.
        @param fIRotAngle The current rotation angle for the particle.
        @param fIRotSpeed The rotation speed for the particle.
        @param fTimeDelta A time delta since the last update of the particle.
    */
    static void UpdateParticleRotation(
        float& fORotAngle,
        const float fIRotAngle,
        const float fIRotSpeed,
        const float fTimeDelta);

    /**
        Updates the age of a particle, killing off the particle if its age
        exceeds its life time.

        When a particle is killed, the appropriate bits in its flags bitfield
        are set. If bSpawnOnDeath is specified, the appropriate bit will be
        set in the particle flags, along with the spawner ID specified by
        ucSpawnerID. Killed and spawned particles will be resolved when the
        simulation completes.

        @param fOAge The computed age of the particle.
        @param fOLastUpdateTime Holds the time of death if bSpawnOnDeath is
            true and the particle has died. Otherwise, the input last update
            time is simply passed through to this parameter.
        @param uiOFlags The updated particle flags bitfield.
        @param fIAge The current age of the particle.
        @param fILifeSpan The life span of the particle.
        @param fILastUpdateTime The last time at which the particle was
            updated.
        @param uiIFlags The current particle flags bitfield.
        @param fCurrentTime The current simulation time.
        @param bSpawnOnDeath Whether or not the spawn flag should be set for
            the particle if it has been killed.
        @param ucSpawnerID The spawner ID to use when spawing on death.
    */
    static void UpdateParticleAgeDeath(
        float& fOAge,
        float& fOLastUpdateTime,
        NiUInt32& uiOFlags,
        const float fIAge,
        const float fILifeSpan,
        const float fILastUpdateTime,
        const NiUInt32 uiIFlags,
        const float fCurrentTime,
        const bool bSpawnOnDeath,
        const NiUInt16 ucSpawnerID);
};

#include "NiPSSimulatorKernelHelpers.inl"

#endif  // #ifndef NIPSSIMULATORKERNELHELPERS_H
