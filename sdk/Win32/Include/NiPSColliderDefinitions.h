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

#ifndef NIPSCOLLIDERDEFINITIONS_H
#define NIPSCOLLIDERDEFINITIONS_H

#include "NiParticleLibType.h"
#include <NiPlane.h>
#include <NiMatrix3.h>
#include <NiUniversalTypes.h>

/**
    Holds data structures used by particle colliders.

    This class defines several structures that contain the data used by
    particle colliders. They must be defined in a separate class so that they
    can be easily included in the NiPSSimulatorCollidersKernel Floodgate
    kernel.

    New colliders should add new struct definitions to this class.

    The structures included in this class are the following:

    - ColliderData
    - PlanarColliderData
    - SphericalColliderData
*/
class NIPARTICLE_ENTRY NiPSColliderDefinitions
{
public:
    /**
        The type of collider.

        This is used by the Floodgate kernel to determine which
        NiPSColliderHelpers functions to call.

        New colliders should add new values to this enumeration.
    */
    enum ColliderType
    {
        /// NiPSPlanarCollider.
        COLLIDER_PLANAR = 0,

        /// NiPSSphericalCollider.
        COLLIDER_SPHERICAL,

        /// The maximum number of collider types. This should be increased if
        /// more collider types are added.
        NUM_COLLIDER_TYPES
    };

    /// Data required by the NiPSCollider class.
    struct ColliderData
    {
        /// The bounce value for the collider.
        float m_fBounce;

        /// @cond EMERGENT_INTERNAL

        NiUInt16 m_ucSpawnerID;

        /// @endcond

        /// Whether or not new particles should be spawned upon collision.
        bool m_bSpawnOnCollide;

        /// Whether or not particles should immediately die upon collision.
        bool m_bDieOnCollide;
    };

    /// Data required by the NiPSPlanarCollider class.
    struct PlanarColliderData : public ColliderData
    {
        /// The width of the collider, measured along m_kXAxis.
        float m_fWidth;

        /// The height of the collider, measured along m_kYAxis.
        float m_fHeight;

        /// A unit vector in the model space of the collider object that
        /// represents one axis defining a plane.
        NiPoint3 m_kXAxis;

        /// A unit vector in the model space of the collider object that
        /// represents one axis defining a plane.
        NiPoint3 m_kYAxis;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kColliderPosition;
        NiMatrix3 m_kColliderRotation;
        NiPlane m_kPlane;
        NiPoint3 m_kRotatedXAxis;
        NiPoint3 m_kRotatedYAxis;
        float m_fHalfSqScaledWidth;
        float m_fHalfSqScaledHeight;

        /// @endcond
    };

    /// Data required by the NiPSSphericalCollider class.
    struct SphericalColliderData : public ColliderData
    {
        /// The radius of the spherical collider.
        float m_fRadius;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kColliderPosition;
        float m_fRadiusSq;
        float m_fScaledRadius;

        /// @endcond
    };
};

#endif  // #ifndef NIPSCOLLIDERDEFINITIONS_H
