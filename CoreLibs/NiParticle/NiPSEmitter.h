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

#ifndef NIPSEMITTER_H
#define NIPSEMITTER_H

#include "NiParticleLibType.h"
#include <NiObject.h>
#include <NiColor.h>
#include <NiPoint3.h>

class NiPSParticleSystem;

/**
    The abstract base class for all particle emitters.

    This class contains an EmitParticles function that does the work of
    emitting new particles using a bunch of initial parameters that are
    stored as member variables in the class.
    
    Emitter classes that derive from NiPSEmitter have the option of overriding
    the protected ComputeInitialPositionAndVelocity function and using the
    base class EmitParticles function if the only thing they are customizing
    is the position and velocity of new particles as they are emitted. They
    also have the option to override EmitParticles entirely to fully customize
    new particle creation.
*/
class NIPARTICLE_ENTRY NiPSEmitter : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSEmitter);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /**
        Emits particles into the specified particle system.

        This function is generally called from an NiPSEmitParticlesCtlr object,
        but it can be called by anyone.
        
        Internally, it calls the protected ComputeInitialPositionAndVelocity
        function, which is pure virtual and must be overridden by derived
        classes. Many derived classes will be able to just override
        ComputeInitialPositionAndVelocity and let this version of
        EmitParticles do the rest, but it is possible to override
        EmitParticles entirely to completely customize how particles are
        emitted.
        
        EmitParticles uses the various initial parameters stored in this class
        to initialize new particles. It also calls InitializeParticle on
        the NiPSParticleSystem object to allow for further particle
        initialization.

        @param pkParticleSystem The particle system in which the particles
            should be emitted.
        @param fTime The current time.
        @param uiNumParticles The number of particles to emit.
        @param pfAges An array of size uiNumParticles that holds initial age
            values for each new particle. This allows particles to be emitted
            in the past in order to ensure even particle emission with an
            uneven frame rate.
    */
    virtual void EmitParticles(
        NiPSParticleSystem* pkParticleSystem,
        float fTime,
        NiUInt32 uiNumParticles,
        const float* pfAges);

    //@{
    /**
        Accesses the name of the emitter.

        The emitter name is used to locate the proper emitter when attaching
        an NiPSEmitterCtlr object to the particle system.
    */
    inline const NiFixedString& GetName() const;
    inline void SetName(const NiFixedString& kName);
    //@}

    //@{
    /// Accesses the speed assigned to new particles.
    inline float GetSpeed() const;
    inline void SetSpeed(float fSpeed);
    //@}

    //@{
    /**
        Accesses the speed variation for new particles.
        
        The resulting speed values assigned to new particles will be evenly
        distributed over the range
        [fSpeed - fSpeedVar/2, fSpeed + fSpeedVar/2].
    */
    inline float GetSpeedVar() const;
    inline void SetSpeedVar(float fSpeedVar);
    //@}

    //@{
    /**
        Accesses the declination angle in radians from the positive z-axis for
        the velocity vector of newly created particles.
        
        The expected range of declination is from 0.0, which will set the
        velocity vector to [0,0,1], to NI_PI, which will set the velocity
        vector to [0,0,-1].
    */
    inline float GetDeclination() const;
    inline void SetDeclination(float fDeclination);
    //@}

    //@{
    /**
        Accesses the declination variation in radians for new particles.
        
        The resulting declination values assigned to new particles will be
        evenly distributed over the range
        [fDeclination - fDeclinationVar, fDeclination + fDeclinationVar].
    */
    inline float GetDeclinationVar() const;
    inline void SetDeclinationVar(float fDeclinationVar);
    //@}

    //@{
    /**
        Accesses the planar angle in radians about the z-axis from the
        positive x-axis that will serve as the plane in which the declination
        will occur.
        
        The expected range of the planar angle is [0, 2*NI_PI).
    */
    inline float GetPlanarAngle() const;
    inline void SetPlanarAngle(float fPlanarAngle);
    //@}

    //@{
    /**
        Accesses the planar angle variation in radians for new particles.
        
        The resulting planar angle values assigned to new particles will be
        evenly distributed over the range
        [fPlanarAngle - fPlanarAngleVar, fPlanarAngle + fPlanarAngleVar].
    */
    inline float GetPlanarAngleVar() const;
    inline void SetPlanarAngleVar(float fPlanarAngleVar);
    //@}

    //@{
    /// Accesses the color assigned to new particles.
    inline const NiRGBA& GetColor() const;
    inline void SetColor(const NiRGBA& kColor);
    //@}

    //@{
    /// Accesses the radius assigned to new particles.
    inline float GetRadius() const;
    inline void SetRadius(float fRadius);
    //@}

    //@{
    /**
        Accesses the radius variation for new particles.
        
        The resulting radius values assigned to new particles will be evenly
        distributed over the range
        [fRadius - fRadiusVar, fRadius + fRadiusVar].
        
        @note If fRadiusVar is larger than fRadius, it is possible that the
            radius will be set to a negative value. In this case, the particle
            will be flipped, and the bounding volume may be computed
            incorrectly. Thus, it is recommended that fRadiusVar be less than
            or equal to fRadius.
    */
    inline float GetRadiusVar() const;
    inline void SetRadiusVar(float fRadiusVar);
    //@}

    //@{
    /// Accesses the life span in seconds assigned to new particles.
    inline float GetLifeSpan() const;
    inline void SetLifeSpan(float fLifeSpan);
    //@}

    //@{
    /**
        Accesses the life span variation for new particles.
        
        The resulting life span values assigned to new particles will be
        evenly distributed over the range
        [fLifeSpan - fLifeSpanVar/2, fLifeSpan + fLifeSpanVar/2].
    */
    inline float GetLifeSpanVar() const;
    inline void SetLifeSpanVar(float fLifeSpanVar);
    //@}

    //@{
    /// Accesses the rotation angle in radians assigned to new particles.
    inline float GetRotAngle() const;
    inline void SetRotAngle(float fRotAngle);
    //@}

    //@{
    /**
        Accesses the rotation angle variation in radians for new particles.
        
        The resulting rotation angle values assigned to new particles will be
        evenly distributed over the range
        [fRotAngle - fRotAngleVar, fRotAngle + fRotAngleVar].
    */
    inline float GetRotAngleVar() const;
    inline void SetRotAngleVar(float fRotAngleVar);
    //@}

    //@{
    /// Accesses the rotation speed in radians per second assigned to new
    /// particles.
    inline float GetRotSpeed() const;
    inline void SetRotSpeed(float fRotSpeed);
    //@}

    //@{
    /**
        Accesses the rotation speed variation in radians per second for new
        particles.
        
        The resulting rotation speed values assigned to new particles will be
        evenly distributed over the range
        [fRotSpeed - fRotSpeedVar, fRotSpeed + fRotSpeedVar].
    */
    inline float GetRotSpeedVar() const;
    inline void SetRotSpeedVar(float fRotSpeedVar);
    //@}

    //@{
    /// Accesses whether or not the rotation speed sign should be randomly
    /// flipped when being assigned to new particles.
    inline bool GetRandomRotSpeedSign() const;
    inline void SetRandomRotSpeedSign(bool bRandomRotSpeedSign);
    //@}

    //@{
    /**
        Accesses the rotation axis assigned to new particles.
        
        This value is only used if bRandomRotAxis is false.
    */
    inline const NiPoint3& GetRotAxis() const;
    inline void SetRotAxis(const NiPoint3& kRotAxis);
    //@}

    //@{
    /// Accesses whether or not a random rotation axis will be assigned to new
    /// particles.
    inline bool GetRandomRotAxis() const;
    inline void SetRandomRotAxis(bool bRandomRotAxis);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Protected main constructor.

        @param kName The name of the emitter.
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
    NiPSEmitter(
        const NiFixedString& kName,
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
    NiPSEmitter();
    //@}

    /**
        Called by EmitParticles to determine the position and velocity for a
        new particle.

        Derived classes may be able to only override this function and use
        the default EmitParticles function.

        @param pkParticleSystem The particle system in which the particle will
            be emitted.
        @param kPosition The computed particle position.
        @param kVelocity The computed particle velocity.
        @return Return true if the function succeeded and you wish to add the 
            particle to the system. Return false if the function did not
            succeed and you do not want to add the particle to the system.
    */
    virtual bool ComputeInitialPositionAndVelocity(
        NiPSParticleSystem* pkParticleSystem,
        NiPoint3& kPosition,
        NiPoint3& kVelocity) = 0;

    /// The name of the emitter.
    NiFixedString m_kName;

    /// @name Initial Particle Values
    //@{
    /// See accessor documentation for details.
    float m_fSpeed;
    float m_fSpeedVar;
    float m_fDeclination;
    float m_fDeclinationVar;
    float m_fPlanarAngle;
    float m_fPlanarAngleVar;
    NiRGBA m_kColor;
    float m_fRadius;
    float m_fRadiusVar;
    float m_fLifeSpan;
    float m_fLifeSpanVar;
    float m_fRotAngle;
    float m_fRotAngleVar;
    float m_fRotSpeed;
    float m_fRotSpeedVar;
    NiPoint3 m_kRotAxis;
    bool m_bRandomRotSpeedSign;
    bool m_bRandomRotAxis;
    //@}
};

NiSmartPointer(NiPSEmitter);

#include "NiPSEmitter.inl"

#endif  // #ifndef NIPSEMITTER_H
