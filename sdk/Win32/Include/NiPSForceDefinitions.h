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

#ifndef NIPSFORCEDEFINITIONS_H
#define NIPSFORCEDEFINITIONS_H

#include "NiParticleLibType.h"
#include <NiPoint3.h>
#include <NiUniversalTypes.h>

/**
    Holds data structures used by particle forces.

    This class defines several structures that contain the data used by
    particle forces. They must be defined in a separate class so that they
    can be easily included in the NiPSSimulatorForcesKernel Floodgate kernel.

    New forces should add new struct definitions to this class.

    The structures included in this class are the following:

    - BombData
    - DragData
    - FieldData
    - AirFieldData
    - DragFieldData
    - GravityFieldData
    - RadialFieldData
    - TurbulenceFieldData
    - VortexFieldData
    - GravityData
*/
class NIPARTICLE_ENTRY NiPSForceDefinitions
{
public:
    /**
        The type of force.

        This is used by the Floodgate kernel to determine which
        NiPSForceHelpers functions to call.

        New forces should add new values to this enumeration.
    */
    enum ForceType
    {
        /// NiPSBombForce.
        FORCE_BOMB = 0,

        /// NiPSDragForce.
        FORCE_DRAG,

        /// NiPSAirFieldForce.
        FORCE_AIR_FIELD,

        /// NiPSDragFieldForce.
        FORCE_DRAG_FIELD,

        /// NiPSGravityFieldForce.
        FORCE_GRAVITY_FIELD,

        /// NiPSRadialFieldForce.
        FORCE_RADIAL_FIELD,

        /// NiPSTurbulenceFieldForce.
        FORCE_TURBULENCE_FIELD,

        /// NiPSVortexFieldForce.
        FORCE_VORTEX_FIELD,

        /// NiPSGravityForce.
        FORCE_GRAVITY,
        
        /// The maximum number of force types. This should be increased if
        /// more force types are added.
        NUM_FORCE_TYPES
    };

    /// Data required by the NiPSBombForce class.
    struct BombData
    {
        /// The type of decay used by the bomb.
        enum DecayType
        {
            /// The force will not decay over distance.
            DECAY_NONE,

            /// The force will decay linearly with distance:
            /// (Decay - Distance) / Decay
            DECAY_LINEAR,

            /// The force will decay exponentially with distance:
            /// NiExp(-Distance / Decay)
            DECAY_EXPONENTIAL
        };

        /// The type of symmetry employed by the bomb.
        enum SymmType
        {
            /// The force is applied spherically about the position of the
            /// bomb object.
            SYMM_SPHERICAL,

            /// The force is applied perpendicular to the direction specified
            /// by the bomb axis and the rotation of the bomb object but
            /// centered about the position of the bomb object.
            SYMM_CYLINDRICAL,

            /// The force is applied parallel to the direction specified by
            /// the bomb axis and the rotation of the bomb object.
            SYMM_PLANAR
        };

        /// The axis of the bomb object's transforms to use as a direction
        /// vector for planar and spherical bombs.
        NiPoint3 m_kBombAxis;

        /// How the bomb's force will decrease with distance.
        float m_fDecay;

        /// The acceleration the bomb will apply to the particles.
        float m_fDeltaV;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kPosition;
        NiPoint3 m_kDirection;

        /// @endcond

        /// The type of decay used by the bomb.
        DecayType m_eDecayType;

        /// The type of symmetry employed by the bomb.
        SymmType m_eSymmType;
    };

    /// Data required by the NiPSDragForce class.
    struct DragData
    {
        /// A unit vector specified in the local space of the drag object that
        /// represents the direction of the drag force.
        NiPoint3 m_kDragAxis;

        /// The amount of drag to apply to particles.
        float m_fPercentage;

        /// The distance up to which particles are affected by the full force.
        float m_fRange;

        /// The distance at which particles cease to be affected by the force.
        float m_fRangeFalloff;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kPosition;
        NiPoint3 m_kDirection;

        /// @endcond
    };

    /// Data required by the NiPSFieldForce class.
    struct FieldData
    {
        /// The magnitude of the force.
        float m_fMagnitude;

        /// How the magnitude of the force diminishes with distance from the
        /// field object.
        float m_fAttenuation;
        
        /// The maximum distance from the field object at which the force has
        /// any effect. This distance is only used if m_bUseMaxDistance is
        /// true.
        float m_fMaxDistance;

        /// @cond EMERGENT_INTERNAL

        float m_fMaxDistanceSqr;

        /// @endcond

        /// Whether or not to use a maximum distance from the field object
        /// after which the force has no effect.
        bool m_bUseMaxDistance;
    };

    /// Data required by the NiPSAirFieldForce class.
    struct AirFieldData : public FieldData
    {
        /// The direction of the air flow.
        NiPoint3 m_kDirection;

        /// How quickly particles will accelerate to the magnitude of the air
        /// field.
        float m_fAirFriction;

        /// How much of the air field's velocity will be added to the
        /// magnitude and direction.
        float m_fInheritedVelocity;

        /// The angle of the air field cone if m_bEnableSpread is true.
        float m_fSpread;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kUnitDirection;
        NiPoint3 m_kTransformedPosition;
        NiPoint3 m_kAirFieldVelocity;
        NiPoint3 m_kUnitAirFieldVelocity;
        float m_fAirFieldVelocityLength;
        float m_fCosSpread;

        /// @endcond

        /// Whether or not changes in the rotation of the field object will be
        /// reflected in the final direction of the air field.
        bool m_bInheritRotation;

        /// Whether or not the air flow is cone-shaped relative to the
        /// direction of the air field.
        bool m_bEnableSpread;
    };

    /// Data required by the NiPSDragFieldForce class.
    struct DragFieldData : public FieldData
    {
        /// The direction in which the force applies if m_bUseDirection is
        /// true.
        NiPoint3 m_kDirection;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kPosition;
        NiPoint3 m_kDragAxis;

        /// @endcond

        /// Whether or not the drag force applies only in the direction
        /// specified by m_kDirection.
        bool m_bUseDirection;
    };

    /// Data required by the NiPSGravityFieldForce class.
    struct GravityFieldData : public FieldData
    {
        /// The direction of the gravity field in field object space.
        NiPoint3 m_kDirection;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kUnitDirection;
        NiPoint3 m_kTransformedDirection;
        NiPoint3 m_kTransformedPosition;
        float m_fHalfAttenuation;

        /// @endcond
    };

    /// Data required by the NiPSRadialFieldForce class.
    struct RadialFieldData : public FieldData
    {
        /// How the radial field's effect diminishes with attenuation.
        float m_fRadialType;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kTransformedPosition;
        float m_fHalfAttenuation;

        /// @endcond
    };

    /// Data required by the NiPSTurbulenceFieldForce class.
    struct TurbulenceFieldData : public FieldData
    {
        /// How many turbulence updates should occur per second.
        float m_fFrequency;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kTransformedPosition;

        /// @endcond
    };

    /// Data required by the NiPSVortexFieldForce class.
    struct VortexFieldData : public FieldData
    {
        /// A unit vector that defines the direction of the vortex in field
        /// object space.
        NiPoint3 m_kDirection;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kTransformedPosition;
        NiPoint3 m_kTransformedDirection;

        /// @endcond
    };

    /// Data required by the NiPSGravityForce class.
    struct GravityData
    {
        /// The type of the gravity force.
        enum ForceType
        {
            /// The force operates parallel to the direction computed from the
            /// gravity object and the gravity axis.
            FORCE_PLANAR,

            /// The force operates spherically about the position of the
            /// gravity object.
            FORCE_SPHERICAL
        };

        /// A unit vector specified in gravity object space that determines
        /// the direction of the gravity force.
        NiPoint3 m_kGravityAxis;

        /// How the gravity force is diminished by distance.
        float m_fDecay;

        /// The acceleration of the gravity force.
        float m_fStrength;

        /// The amount of turbulence to apply to particles.
        float m_fTurbulence;

        /// The amount by which the turbulence value will be scaled prior to
        /// being applied.
        float m_fTurbulenceScale;

        /// @cond EMERGENT_INTERNAL

        NiPoint3 m_kPosition;
        NiPoint3 m_kDirection;

        /// @endcond

        /// The type of gravity force to apply.
        ForceType m_eForceType;
    };
};

#endif  // #ifndef NIPSFORCEDEFINITIONS_H
