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

#ifndef NIPSSPHEREEMITTER_H
#define NIPSSPHEREEMITTER_H

#include "NiPSVolumeEmitter.h"

/**
    A particle emitter that emits particles from a spherical volume.

    Particles are emitted from a sphere that is centered at the world-space
    translation of the emitter object and with a specified radius.

    The actual computation work for setting the initial position and velocity
    of new particles is done in the overridden protected function
    ComputeVolumeInitialPositionAndVelocity.
*/
class NIPARTICLE_ENTRY NiPSSphereEmitter : public NiPSVolumeEmitter
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSphereEmitter);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the emitter.
        @param fEmitterRadius The radius of the emitter sphere.
        @param pkEmitterObj An object in the scene graph whose world-space
            transformation will be used to represent the position and
            orientation of the emitter.
        @param fSpeed The speed assigned to new particles.
        @param fSpeedVar The speed variation for new particles. The resulting
            speed values assigned to new particles will be evenly
            distributed over the range [fSpeed - fSpeedVar/2,
            fSpeed + fSpeedVar/2].
        @param fDeclination The declination angle in radians from the positive
            z-axis for the velocity vector of newly created particles. The
            expected range of declination is from 0.0, which will set the
            velocity vector to [0,0,1], to NI_PI, which will set the velocity
            vector to [0,0,-1].
        @param fDeclinationVar The declination variation in radians for new
            particles. The resulting declination values assigned to new
            particles will be evenly distributed over the range
            [fDeclination - fDeclinationVar, fDeclination + fDeclinationVar].
        @param fPlanarAngle The planar angle in radians about the z-axis from
            the positive x-axis that will serve as the plane in which the
            declination will occur. The expected range of the planar angle is
            [0, 2*NI_PI).
        @param fPlanarAngleVar The planar angle variation in radians for new
            particles. The resulting planar angle values assigned to new
            particles will be evenly distributed over the range
            [fPlanarAngle - fPlanarAngleVar, fPlanarAngle + fPlanarAngleVar].
        @param kColor The color assigned to new particles.
        @param fRadius The radius assigned to new particles.
        @param fRadiusVar The radius variation for new particles. The resulting
            radius values assigned to new particles will be evenly distributed
            over the range [fRadius - fRadiusVar, fRadius + fRadiusVar]. If
            fRadiusVar is larger than fRadius, it is possible that the radius
            will be set to a negative value. In this case, the particle will
            be flipped, and the bounding volume may be computed incorrectly.
            Thus, it is recommended that fRadiusVar be less than or equal to
            fRadius.
        @param fLifeSpan The life span in seconds assigned to new particles.
        @param fLifeSpanVar The life span variation for new particles. The
            resulting life span values assigned to new particles will be
            evenly distributed over the range [fLifeSpan - fLifeSpanVar/2,
            fLifeSpan + fLifeSpanVar/2].
        @param fRotAngle The rotation angle in radians assigned to new
            particles.
        @param fRotAngleVar The rotation angle variation in radians for new
            particles. The resulting rotation angle values assigned to new
            particles will be evenly distributed over the range
            [fRotAngle - fRotAngleVar, fRotAngle + fRotAngleVar].
        @param fRotSpeed The rotation speed in radians per second assigned to
            new particles.
        @param fRotSpeedVar The rotation speed variation in radians per second
            for new particles. The resulting rotation speed values assigned to
            new particles will be evenly distributed over the range
            [fRotSpeed - fRotSpeedVar, fRotSpeed + fRotSpeedVar].
        @param bRandomRotSpeedSign Whether or not the rotation speed sign
            should be randomly flipped when being assigned to new particles.
        @param kRotAxis The rotation axis assigned to new particles. This
            value is only used if bRandomRotAxis is false.
        @param bRandomRotAxis Whether or not a random rotation axis will be
            assigned to new particles.
    */
    NiPSSphereEmitter(
        const NiFixedString& kName,
        float fEmitterRadius = 0.0f,
        NiAVObject* pkEmitterObj = NULL,
        float fSpeed = 1.0f,
        float fSpeedVar = 0.0f,
        float fDeclination = 0.0f,
        float fDeclinationVar = 0.0f,
        float fPlanarAngle = 0.0f,
        float fPlanarAngleVar = 0.0f,
        const NiRGBA& kColor = NiRGBA::WHITE,
        float fRadius = 1.0f,
        float fRadiusVar = 0.0f,
        float fLifeSpan = 1.0f,
        float fLifeSpanVar = 0.0f,
        float fRotAngle = 0.0f,
        float fRotAngleVar = 0.0f,
        float fRotSpeed = 0.0f,
        float fRotSpeedVar = 0.0f,
        bool bRandomRotSpeedSign = false,
        const NiPoint3& kRotAxis = NiPoint3::UNIT_X,
        bool bRandomRotAxis = true);
    //@}

    //@{
    /// Accesses the radius of the emitter sphere.
    inline float GetEmitterRadius() const;
    inline void SetEmitterRadius(float fEmitterRadius);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSSphereEmitter();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool ComputeVolumeInitialPositionAndVelocity(
        NiTransform& kEmitterToPSys,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);
    //@}

    /// The radius of the emitter sphere.
    float m_fEmitterRadius;
};

NiSmartPointer(NiPSSphereEmitter);

#include "NiPSSphereEmitter.inl"

#endif  // #ifndef NIPSSPHEREEMITTER_H
