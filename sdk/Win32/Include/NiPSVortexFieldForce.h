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

#ifndef NIPSVORTEXFIELDFORCE_H
#define NIPSVORTEXFIELDFORCE_H

#include "NiPSFieldForce.h"

/**
    Simulates the effects of a vortex force for particles.

    The field object and direction define the location and orientation of the
    vortex field. The magnitude defines the acceleration around the
    directional axis. The attenuation and max distance define the area and
    influence of the vortex field.
*/
class NIPARTICLE_ENTRY NiPSVortexFieldForce : public NiPSFieldForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSVortexFieldForce);
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
        @param kDirection A unit vector that defines the direction of the
            vortex in field object space.
    */
    NiPSVortexFieldForce(
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
    ///

    //@{
    /// Accesses a unit vector in field object space that defines the
    /// direction of the vortex force.
    inline const NiPoint3& GetDirection() const;
    inline void SetDirection(const NiPoint3& kDirection);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSVortexFieldForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::VortexFieldData m_kInputData;
};

NiSmartPointer(NiPSVortexFieldForce);

#include "NiPSVortexFieldForce.inl"

#endif  // #ifndef NIPSVORTEXFIELDFORCE_H
