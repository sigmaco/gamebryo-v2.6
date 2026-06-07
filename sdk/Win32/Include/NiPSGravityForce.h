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

#ifndef NIPSGRAVITYFORCE_H
#define NIPSGRAVITYFORCE_H

#include "NiPSForce.h"

/**
    Applies a gravitational force to particles.

    The position and/or direction for the gravitational force are determined
    by the world-space transformations of a gravity object in the scene graph
    and a gravity axis specified in gravity object space. The force type
    determines how the force will be applied to the particles.
*/
class NIPARTICLE_ENTRY NiPSGravityForce : public NiPSForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSGravityForce);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the force.
        @param pkGravityObj The object in the scene graph whose world-space
            transformations will be used to represent the position and
            orientation of the gravity force.
        @param kGravityAxis A unit vector specified in gravity object space
            that determines the direction of the gravity force.
        @param fDecay How the gravity force is diminished by distance.
        @param fStrength The acceleration of the gravity force.
        @param eForceType The type of gravity force to apply.
        @param fTurbulence The amount of turbulence to apply to particles.
        @param fTurbulenceScale The amount by which the turbulence value will
            be scaled prior to being applied.
    */
    NiPSGravityForce(
        const NiFixedString& kName,
        NiAVObject* pkGravityObj = NULL,
        const NiPoint3& kGravityAxis = NiPoint3::UNIT_X,
        float fDecay = 0.0f,
        float fStrength = 0.0f,
        NiPSForceDefinitions::GravityData::ForceType eForceType =
            NiPSForceDefinitions::GravityData::FORCE_PLANAR,
        float fTurbulence = 0.0f,
        float fTurbulenceScale = 1.0f);
    //@}

    /// @name Base Class Overrides
    //@{
    virtual size_t GetDataSize() const;
    virtual void* GetData() const;
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}
    ///

    //@{
    /**
        Accesses the gravity object in the scene graph.

        The world transforms are obtained from this object to determine the
        position and orientation of the gravity force.
    */
    inline NiAVObject* GetGravityObj() const;
    inline void SetGravityObj(NiAVObject* pkGravityObj);
    //@}

    //@{
    /// Accesses a unit vector specified in gravity object space that
    /// determines the direction of the gravity force.
    inline const NiPoint3& GetGravityAxis() const;
    inline void SetGravityAxis(const NiPoint3& kGravityAxis);
    //@}

    //@{
    /**
        Accesses the amount of decay for the gravity force.

        The distance of objects from the center of the gravity force is
        affected by the decay in an exponential manner:

        Decay = NiExp(Decay * Distance)
    */
    inline float GetDecay() const;
    inline void SetDecay(float fDecay);
    //@}

    //@{
    /**
        Accesses the acceleration of the gravity force.

        This value is specified in units per second^2.
    */
    inline float GetStrength() const;
    inline void SetStrength(float fStrength);
    //@}

    //@{
    /**
        Accesses the force type.

        The force type determines how the gravity force is applied.
    */
    inline NiPSForceDefinitions::GravityData::ForceType GetForceType() const;
    inline void SetForceType(
        NiPSForceDefinitions::GravityData::ForceType eForceType);
    //@}

    //@{
    /**
        Accesses the gravity force turbulence.

        The turbulence value is multiplied first by the turbulence scale, then
        by 500, and then by a random number between -1.0 and 1.0 to arrive at
        a final turbulence vector. The turbulence vector is then applied to
        the particle velocity to create random fluctuations in the movement.

        The turbulence approximates the Wind space warp in 3ds max.
    */
    inline float GetTurbulence() const;
    inline void SetTurbulence(float fTurbulence);
    inline float GetTurbulenceScale() const;
    inline void SetTurbulenceScale(float fTurbulenceScale);
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
    /// Protected default constructor for cloning and streaming only.
    NiPSGravityForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::GravityData m_kInputData;

    /// A pointer to the gravity object in the scene graph.
    NiAVObject* m_pkGravityObj;
};

NiSmartPointer(NiPSGravityForce);

#include "NiPSGravityForce.inl"

#endif  // #ifndef NIPSGRAVITYFORCE_H
