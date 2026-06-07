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

#ifndef NIPSAIRFIELDFORCE_H
#define NIPSAIRFIELDFORCE_H

#include "NiPSFieldForce.h"

/**
    Updates the particle velocity to simulate the effects of air movements
    like wind, fans, or wake.

    The field object, direction, and inherit rotation define the location and
    orientation of the air field. The magnitude defines the maximum velocity
    in particle space. Inherit velocity describes how much of the air field's
    velocity is added to the magnitude and direction of the air field.
    
    It is important to remember that this class dynamically computes the
    velocity of the air field from frame to frame. Radically changing the
    position of the field object can cause large instantaneous velocities, so
    ClearLastFieldPosition should be called in such a case.
    
    Here are some settings to define typical effects:

    - Wind
        - Magnitude = wind speed
        - Inherit Velocity = 0.0
        - Inherit Rotation = false
        - Use Spread = false
    - Fan
        - Magnitude = air speed
        - Inherit Velocity = 0.5
        - Inherit Rotation = true
        - Use Spread = true
    - Wake
        - Magnitude = 0.0
        - Inherit Velocity = 1.0
        - Inherit Rotation = true
        - Use Spread = false

*/
class NIPARTICLE_ENTRY NiPSAirFieldForce : public NiPSFieldForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSAirFieldForce);
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
        @param kDirection The direction of the air flow.
        @param fAirFriction How quickly particles will accelerate to the
            magnitude of the air field.
        @param fInheritedVelocity How much of the air field's velocity will be
            added to the magnitude and direction.
        @param bInheritRotation Whether or not changes in the rotation of the
            field object will be reflected in the final direction of the air
            field.
        @param bEnableSpread Whether or not the air flow is cone-shaped
            relative to the direction of the air field.
        @param fSpread The angle of the air field cone if bEnableSpread is
            true.
    */
    NiPSAirFieldForce(
        const NiFixedString& kName,
        NiAVObject* pkFieldObj,
        float fMagnitude,
        float fAttenuation,
        bool bUseMaxDistance,
        float fMaxDistance,
        const NiPoint3& kDirection,
        float fAirFriction,
        float fInheritedVelocity,
        bool bInheritRotation,
        bool bEnableSpread,
        float fSpread);
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
        Accesses the last position of the air field object in particle space.

        The field's velocity is computed using the last updated position of
        the field. Radically changing the position of the field object can
        cause large instantaneous velocities. In this case,
        ClearLastFieldPosition should be called.
    */ 
    inline const NiPoint3& GetLastFieldPosition() const;
    inline void SetLastFieldPosition(const NiPoint3& kLastFieldPosition);
    //@}

    /**
        Clears the stored last position of the air field object.

        This is useful when the air field object's position will be radically
        changed so as to avoid large instantaneous velocities.
    */
    inline void ClearLastFieldPosition();

    //@{
    /**
        Accesses the direction of the air flow.

        This direction vector will either be in particle space or in the space
        of the air field object, depending on the value of GetInheritRotation.
    */
    inline const NiPoint3& GetDirection() const;
    inline void SetDirection(const NiPoint3& kDirection);
    //@}

    //@{
    /**
        Accesses how quickly particles will accelerate to the magnitude of the
        air field.

        Valid air friction values range from 0.0 to 1.0. An air friction of 0.0
        indicates that particles are not affected by air at all. A value of
        1.0 means that particles almost instantaneously accelerate to the
        magnitude of the air field.
    */
    inline float GetAirFriction() const;
    inline void SetAirFriction(float fAirFriction);
    //@}

    //@{
    /**
        Accesses how much of the air field's velocity will be added to the
        magnitude and direction.

        Valid values range from 0.0 to 1.0.
    */
    inline float GetInheritedVelocity() const;
    inline void SetInheritedVelocity(float fInheritedVelocity);
    //@}

    //@{
    /**
        Accesses whether or not changes in the rotation of the field object
        will be reflected in the final direction of the air field.

        If this value is true, the direction of the air field is in the space
        of the field object. Any changes to the rotation of the field object
        will be reflected in the final direction of the air field. If this
        value is false, the direction of the air field is in particle space.
    */
    inline bool GetInheritRotation() const;
    inline void SetInheritRotation(bool bInheritRotation);
    //@}

    //@{
    /**
        Accesses whether or not the air flow is cone-shaped relative to the
        direction of the air field.

        If this value is false, the air flow is parallel to the direction of
        the air field. If this value is true, the air flow is a cone shaped
        from the field object. The angle of the cone is specified by
        SetSpread. An example of an air field with this option enabled would
        be the air patter created by a fan.
    */
    inline bool GetEnableSpread() const;
    inline void SetEnableSpread(bool bEnableSpread);
    //@}

    //@{
    /**
        Accesses the angle of the air field cone if GetEnableSpread returns
        true.

        Valid spread values range from 0.0 to 1.0, where 0.0 is a line and 1.0
        is the 180-degree half-sphere.
    */
    inline float GetSpread() const;
    inline void SetSpread(float fSpread);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSAirFieldForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::AirFieldData m_kInputData;

    /**
        The cached last position of the field object in particle space.

        The difference between field positions from frame to frame is used
        to compute the field velocity.
    */ 
    NiPoint3 m_kLastFieldPosition;

    /// The last time at which the force was updated.
    float m_fLastUpdateTime;

    /// A constant value representing an uninitialized last field position.
    static const NiPoint3 ms_kUninitializedPosition;
};

NiSmartPointer(NiPSAirFieldForce);

#include "NiPSAirFieldForce.inl"

#endif  // #ifndef NIPSAIRFIELDFORCE_H
