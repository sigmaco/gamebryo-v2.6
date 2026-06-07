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

#ifndef NIPSGRAVITYFIELDFORCE_H
#define NIPSGRAVITYFIELDFORCE_H

#include "NiPSFieldForce.h"

/**
    Implements a gravity field force for particles.

    Modifies the velocity of each particle to simulate the effects of
    directional gravity. The field object and direction define the location
    and orientation of the gravity field. The magnitude defines the
    acceleration in particle space. The attenuation and max distance define
    the area and influence of the gravity field.
*/
class NIPARTICLE_ENTRY NiPSGravityFieldForce : public NiPSFieldForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSGravityFieldForce);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the force.
        @param pkFieldObj The object in the scene graph whose world-space
            transformations will be used to represent the position and
            orientation of the field.
        @param fMagnitude The magnitude of the force.
        @param fAttenuation How the magnitude of the force diminishes with
            distance from the field object.
        @param bUseMaxDistance Whether or not to use a maximum distance from
            the field object after which the force has no effect.
        @param fMaxDistance The maximum distance from the field object at
            which the force has any effect. This distance is only used if
            bUseMaxDistance is true.
        @param kDirection The direction of the gravity field in field object
            space.
    */
    NiPSGravityFieldForce(
        const NiFixedString& kName,
        NiAVObject* pkFieldObj,
        float fMagnitude,
        float fAttenuation,
        bool bUseMaxDistance,
        float fMaxDistance,
        const NiPoint3& kDirection = NiPoint3::UNIT_X);
    //@}

    /// @name Base Class Overrides
    //@{
    virtual size_t GetDataSize() const;
    virtual void* GetData() const;
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}

    //@{
    /// Accesses the direction in which the gravity force applies, in field
    /// object space.
    inline const NiPoint3& GetDirection() const;
    inline void SetDirection(const NiPoint3& kDirection);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSGravityFieldForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::GravityFieldData m_kInputData;
};

NiSmartPointer(NiPSGravityFieldForce);

#include "NiPSGravityFieldForce.inl"

#endif  // #ifndef NIPSGRAVITYFIELDFORCE_H
