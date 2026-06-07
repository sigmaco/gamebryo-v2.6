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

#ifndef NIPSTURBULENCEFIELDFORCE_H
#define NIPSTURBULENCEFIELDFORCE_H

#include "NiPSFieldForce.h"

/**
    Implements a turbulence field force for particles.

    The frequency of the turbulence dictates how many turbulence updates
    should occur per second.
*/
class NIPARTICLE_ENTRY NiPSTurbulenceFieldForce : public NiPSFieldForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSTurbulenceFieldForce);
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
        @param fFrequency How many turbulence updates should occur per second.
    */
    NiPSTurbulenceFieldForce(
        const NiFixedString& kName,
        NiAVObject* pkFieldObj,
        float fMagnitude,
        float fAttenuation,
        bool bUseMaxDistance,
        float fMaxDistance,
        float fFrequency);
    //@}

    /// @name Base Class Overrides
    //@{
    virtual size_t GetDataSize() const;
    virtual void* GetData() const;
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}
    ///

    //@{
    /// Accesses how many turbulence updates should occur per second.
    inline const float GetFrequency() const;
    inline void SetFrequency(float fFrequency);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSTurbulenceFieldForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::TurbulenceFieldData m_kInputData;

    /// The time interval between turbulence updates, computed as 1.0 /
    /// Frequency.
    float m_fFrequencyUpdateTime;

    /// The last time at which the force was updated.
    float m_fLastUpdateTime;
};

NiSmartPointer(NiPSTurbulenceFieldForce);

#include "NiPSTurbulenceFieldForce.inl"

#endif  // #ifndef NIPSTURBULENCEFIELDFORCE_H
