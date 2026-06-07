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

#ifndef NIPSVOLUMEEMITTER_H
#define NIPSVOLUMEEMITTER_H

#include "NiPSEmitter.h"

class NiTransform;

/**
    The abstract base class for particle emitters that emit particles from a
    volume.

    It contains a pointer to an object in the scene graph that will serve as
    the emitter object. The emitter gets the world transforms of this object
    to determine the position and orientation in space of the emitter.
    
    This class overrides the inherited protected
    ComputeInitialPositionAndVelocity function and defines its own pure
    virtual protected function ComputeVolumeInitialPositionAndVelocity.
    Classes derived from NiPSVolumeEmitter should override the
    ComputeVolumeInitialPositionAndVelocity function instead.
*/
class NIPARTICLE_ENTRY NiPSVolumeEmitter : public NiPSEmitter
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSVolumeEmitter);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    //@{
    /// Accesses the object in the scene graph whose world-space transformation
    /// will be used to represent the position and orientation of the emitter.
    inline NiAVObject* GetEmitterObj() const;
    inline void SetEmitterObj(NiAVObject* pkEmitterObj);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Protected main constructor.

        @param kName The name of the emitter.
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
    NiPSVolumeEmitter(
        const NiFixedString& kName,
        NiAVObject* pkEmitterObj,
        float fSpeed,
        float fSpeedVar,
        float fDeclination,
        float fDeclinationVar,
        float fPlanarAngle,
        float fPlanarAngleVar,
        const NiRGBA& kColor,
        float fRadius,
        float fRadiusVar,
        float fLifeSpan,
        float fLifeSpanVar,
        float fRotAngle,
        float fRotAngleVar,
        float fRotSpeed,
        float fRotSpeedVar,
        bool bRandomRotSpeedSign,
        const NiPoint3& kRotAxis,
        bool bRandomRotAxis);

    /// Protected default constructor for cloning and streaming only.
    NiPSVolumeEmitter();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool ComputeInitialPositionAndVelocity(
        NiPSParticleSystem* pkParticleSystem,
        NiPoint3& kPosition,
        NiPoint3& kVelocity);
    //@}

    /**
        Called from ComputeInitialPositionAndVelocity to compute the position
        and velocity of the new particle in a volume.

        The transformation from the space of the emitter to the space of the
        particle system is computed and provided to this function. This
        function should be implemented in derived classes instead of
        ComputeInitialPositionAndVelocity.

        @param kEmitterToPSys The transformatio from the space of the emitter
            to the space of the particle system.
        @param kPosition The computed position for the new particle.
        @param kVelocity The computed velocity for the new particle.
    */
    virtual bool ComputeVolumeInitialPositionAndVelocity( 
        NiTransform& kEmitterToPSys,
        NiPoint3& kPosition,
        NiPoint3& kVelocity) = 0;

    /// The object in the scene graph whose world-space transformation will
    /// be used to represent the position and orientation of the emitter
    /// volume.
    NiAVObject* m_pkEmitterObj;
};

NiSmartPointer(NiPSVolumeEmitter);

#include "NiPSVolumeEmitter.inl"

#endif  // #ifndef NIPSVOLUMEEMITTER_H
