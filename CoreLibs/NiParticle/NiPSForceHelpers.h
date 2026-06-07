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

#ifndef NIPSFORCEHELPERS_H
#define NIPSFORCEHELPERS_H

#include "NiPSForceDefinitions.h"
#include <NiOS.h>

/**
    Contains static helper functions used for particle forces.

    The NiPSSimulatorForcesKernel Floodgate kernel calls the static helper
    functions contained in this class to apply forces to particles. Each
    particle force has a function in this class that allows it to modify the
    velocities of the particles.

    Due to the implementation of Floodgate kernels on different platforms, it
    is necessary that all output (non-const) parameters to these functions get
    set to some value. It is not valid to simply not set an output value, as
    garbage memory could result on some platforms.

    If a new particle force class is added, an associated function must be
    added to this class, and the NiPSSimulatorForcesKernel Floodgate kernel
    must be changed to call that function.
*/
class NIPARTICLE_ENTRY NiPSForceHelpers
{
public:
    /**
        Applies a bomb force to particles.

        Associated with the NiPSBombForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyBombForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::BombData& kInputData,
        const float fCurrentTime);

    /**
        Applies a drag force to particles.

        Associated with the NiPSDragForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyDragForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::DragData& kInputData,
        const float fCurrentTime);

    /**
        Applies an air field force to particles.

        Associated with the NiPSAirFieldForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyAirFieldForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::AirFieldData& kInputData,
        const float fCurrentTime);

    /**
        Applies a drag field force to particles.

        Associated with the NiPSDragFieldForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyDragFieldForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::DragFieldData& kInputData,
        const float fCurrentTime);

    /**
        Applies a gravity field force to particles.

        Associated with the NiPSGravityFieldForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyGravityFieldForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::GravityFieldData& kInputData,
        const float fCurrentTime);

    /**
        Applies a radial field force to particles.

        Associated with the NiPSRadialFieldForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyRadialFieldForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::RadialFieldData& kInputData,
        const float fCurrentTime);

    /**
        Applies a turbulence field force to particles.

        Associated with the NiPSTurbulenceFieldForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyTurbulenceFieldForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::TurbulenceFieldData& kInputData,
        const float fCurrentTime);

    /**
        Applies a vortex field force to particles.

        Associated with the NiPSVortexFieldForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyVortexFieldForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::VortexFieldData& kInputData,
        const float fCurrentTime);

    /**
        Applies a planar gravity force to particles.

        Associated with the NiPSGravityForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplyPlanarGravityForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::GravityData& kInputData,
        const float fCurrentTime);

    /**
        Applies a spherical gravity force to particles.

        Associated with the NiPSGravityForce class.

        @param uiBlockCount The size of the input and output arrays.
        @param pkOVelocities An array containing the computed new velocity for
            each particle.
        @param pfILastUpdateTimes An array containing the last update time for
            each particle.
        @param pkIPositions An array containing the current position for each
            particle.
        @param pkIVelocities An array containing the current velocity for each
            particle.
        @param kInputData A structure containing the data needed to apply the
            force.
        @param fCurrentTime The current simulation time.
    */
    static void ApplySphericalGravityForce(
        const NiUInt32 uiBlockCount,
        NiPoint3* pkOVelocities,
        const float* pfILastUpdateTimes,
        const NiPoint3* pkIPositions,
        const NiPoint3* pkIVelocities,
        const NiPSForceDefinitions::GravityData& kInputData,
        const float fCurrentTime);
};

#include "NiPSForceHelpers.inl"

#endif  // #ifndef NIPSFORCEHELPERS_H
