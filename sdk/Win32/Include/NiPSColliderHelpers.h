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

#ifndef NIPSCOLLIDERHELPERS_H
#define NIPSCOLLIDERHELPERS_H

#include "NiPSColliderDefinitions.h"

/**
    Contains static helper functions used for particle collision.

    The NiPSSimulatorCollidersKernel Floodgate kernel calls the static helper
    functions contained in this class to detect and handle particle collisions.
    Each particle collider has two functions in this class: one to detect
    a collision and one to handle a collision. For any given particle, only
    the collider with the closest collision will have its collision handler
    function called.

    Due to the implementation of Floodgate kernels on different platforms, it
    is necessary that all output (non-const) parameters to these functions get
    set to some value. It is not valid to simply not set an output value, as
    garbage memory could result on some platforms.

    If a new particle collider class is added, both detection and handler
    functions must be added to this class, and the NiPSSimulatorCollidersKernel
    Floodgate kernel must be changed to call those functions.
*/
class NIPARTICLE_ENTRY NiPSColliderHelpers
{
public:
    /**
        Detects a collision between a particle and a plane.

        @param fCollisionTime Holds the time at which the collision would
            occur if one is detected.
        @param kCollisionPoint Holds the point in space at which the particle
            would collide if a collision is detected.
        @param fInitialTime The current time.
        @param kParticlePosition The current position of the particle.
        @param kParticleVelocity The current velocity of the particle.
        @param kInputData A structure containing the data needed to detect
            a planar collision.
        @return Whether or not a collision was detected.
    */
    static bool DetectPlanarCollision(
        float& fCollisionTime,
        NiPoint3& kCollisionPoint,
        const float fInitialTime,
        const NiPoint3& kParticlePosition,
        const NiPoint3& kParticleVelocity,
        const NiPSColliderDefinitions::PlanarColliderData& kInputData);

    /**
        Detects a collision between a particle and a sphere.

        @param fCollisionTime Holds the time at which the collision would
            occur if one is detected.
        @param kCollisionPoint Holds the point in space at which the particle
            would collide if a collision is detected.
        @param fInitialTime The current time.
        @param kParticlePosition The current position of the particle.
        @param kParticleVelocity The current velocity of the particle.
        @param kInputData A structure containing the data needed to detect
            a spherical collision.
        @return Whether or not a collision was detected.
    */
    static bool DetectSphericalCollision(
        float& fCollisionTime,
        NiPoint3& kCollisionPoint,
        const float fInitialTime,
        const NiPoint3& kParticlePosition,
        const NiPoint3& kParticleVelocity,
        const NiPSColliderDefinitions::SphericalColliderData& kInputData);

    /**
        Handles a planar collision.

        @param kOParticlePosition The new particle position after the
            collision.
        @param kOParticleVelocity The new particle velocity after the
            collision.
        @param uiOParticleFlags The new flags bitfield for the particle.
        @param fOLastUpdateTime The new last update time for the particle.
        @param fCollisionTime The time at which the collision will occur.
        @param kCollisionPoint The point in space at which the collision will
            occur.
        @param kIParticleVelocity The current velocity of the particle.
        @param uiIParticleFlags The current flags bitfield for the particle.
        @param fILastUpdateTime The current last update time for the particle.
        @param kInputData A structure containing the data needed to handle
            the planar collision.
        @param fCurrentTime The current time.
    */
    static void HandlePlanarCollision(
        NiPoint3& kOParticlePosition,
        NiPoint3& kOParticleVelocity,
        NiUInt32& uiOParticleFlags,
        float& fOLastUpdateTime,
        const float fCollisionTime,
        const NiPoint3& kCollisionPoint,
        const NiPoint3& kIParticleVelocity,
        const NiUInt32 uiIParticleFlags,
        const float fILastUpdateTime,
        const NiPSColliderDefinitions::PlanarColliderData& kInputData,
        const float fCurrentTime);

    /**
        Handles a spherical collision.

        @param kOParticlePosition The new particle position after the
            collision.
        @param kOParticleVelocity The new particle velocity after the
            collision.
        @param uiOParticleFlags The new flags bitfield for the particle.
        @param fOLastUpdateTime The new last update time for the particle.
        @param fCollisionTime The time at which the collision will occur.
        @param kCollisionPoint The point in space at which the collision will
            occur.
        @param kIParticleVelocity The current velocity of the particle.
        @param uiIParticleFlags The current flags bitfield for the particle.
        @param fILastUpdateTime The current last update time for the particle.
        @param kInputData A structure containing the data needed to handle
            the spherical collision.
        @param fCurrentTime The current time.
    */
    static void HandleSphericalCollision(
        NiPoint3& kOParticlePosition,
        NiPoint3& kOParticleVelocity,
        NiUInt32& uiOParticleFlags,
        float& fOLastUpdateTime,
        const float fCollisionTime,
        const NiPoint3& kCollisionPoint,
        const NiPoint3& kIParticleVelocity,
        const NiUInt32 uiIParticleFlags,
        const float fILastUpdateTime,
        const NiPSColliderDefinitions::SphericalColliderData& kInputData,
        const float fCurrentTime);

protected:
    /**
        A general handler function for all collisions.

        This function is used by the other collision handler functions to
        perform common tasks. It updates the particle position and handles
        spawning and death upon collision.

        @param kOParticlePosition The new particle position after the
            collision.
        @param uiOParticleFlags The new flags bitfield for the particle.
        @param fOLastUpdateTime The new last update time for the particle.
        @param fCollisionTime The time at which the collision will occur.
        @param kCollisionPoint The point in space at which the collision will
            occur.
        @param uiIParticleFlags The current flags bitfield for the particle.
        @param fILastUpdateTime The current last update time for the particle.
        @param kInputData A structure containing the data needed to handle the
            collision.
    */
    static void HandleGeneralCollision(
        NiPoint3& kOParticlePosition,
        NiUInt32& uiOParticleFlags,
        float& fOLastUpdateTime,
        const float fCollisionTime,
        const NiPoint3& kCollisionPoint,
        const NiUInt32 uiIParticleFlags,
        const float fILastUpdateTime,
        const NiPSColliderDefinitions::ColliderData& kInputData);
};

#include "NiPSColliderHelpers.inl"

#endif  // #ifndef NIPSCOLLIDERHELPERS_H
