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

#ifndef NIPSDRAGFIELDFORCE_H
#define NIPSDRAGFIELDFORCE_H

#include "NiPSFieldForce.h"

/**
    Implements a drag field force for particles.

    The drag force is applied to the entire particle velocity vector unless
    GetUseDirection is true. In that case, the force only applies in the
    direction returned by GetDirection.
*/
class NIPARTICLE_ENTRY NiPSDragFieldForce : public NiPSFieldForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSDragFieldForce);
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
        @param bUseDirection Whether or not the drag force applies only in the
            direction specified by kDirection.
        @param kDirection The direction in which the force applies if
            bUseDirection is true.
    */
    NiPSDragFieldForce(
        const NiFixedString& kName,
        NiAVObject* pkFieldObj,
        float fMagnitude,
        float fAttenuation,
        bool bUseMaxDistance,
        float fMaxDistance,
        bool bUseDirection = false,
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
    /// Accesses whether or not the drag force applies only in the direction
    /// returned by GetDirection.
    inline const bool GetUseDirection() const;
    inline void SetUseDirection(bool bUseDirection);
    //@}

    //@{
    /**
        Accesses the direction in which the drag force applies.

        This direction is only used if GetUseDirection returns true. If it
        returns false, the force applies to the entire particle velocity
        vector.
    */
    inline const NiPoint3& GetDirection() const;
    inline void SetDirection(const NiPoint3& kDirection);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSDragFieldForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::DragFieldData m_kInputData;
};

NiSmartPointer(NiPSDragFieldForce);

#include "NiPSDragFieldForce.inl"

#endif  // #ifndef NIPSDRAGFIELDFORCE_H
