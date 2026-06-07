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

#ifndef NIPSSPHERICALCOLLIDER_H
#define NIPSSPHERICALCOLLIDER_H

#include "NiPSCollider.h"
#include <NiTransform.h>

/**
    A spherical collider for particles.

    The sphere's center is defined by the world-space translation of a
    collider object in the scene graph. The radius of the sphere is specified
    as a class member variable.
*/
class NIPARTICLE_ENTRY NiPSSphericalCollider : public NiPSCollider
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSphericalCollider);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param fBounce The bounce value for the collider.
        @param bSpawnOnCollide Whether or not new particles should be spawned
            upon collision.
        @param bDieOnCollide Whether or not particles should immediately die
            upon collision.
        @param pkSpawner The spawner object to use for the collider.
        @param pkColliderObj An object in the scene graph whose world-space
            transformation will be used to represent the position and
            orientation of the collider.
        @param fRadius The radius of the spherical collider.
    */
    NiPSSphericalCollider(
        float fBounce = 1.0f,
        bool bSpawnOnCollide = false,
        bool bDieOnCollide = false,
        NiPSSpawner* pkSpawner = NULL,
        NiAVObject* pkColliderObj = NULL,
        float fRadius = 1.0f);
    //@}

    /// @name Base Class Overrides
    //@{
    virtual size_t GetDataSize() const;
    virtual void* GetData() const;
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}
    ///

    //@{
    /// Accesses the object in the scene graph whose world-space
    /// transformation will represent the position and orientation of the
    /// collider.
    inline NiAVObject* GetColliderObj() const;
    inline void SetColliderObj(NiAVObject* pkColliderObj);
    //@}

    //@{
    /// Accesses the radius of the spherical collider.
    inline float GetRadius() const;
    inline void SetRadius(float fRadius);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// A structure containing the data used by this class.
    NiPSColliderDefinitions::SphericalColliderData m_kInputData;

    /// A pointer to an object in the scene graph whose world-space
    /// transformations will be used to represent the position and orientation
    /// of the collider.
    NiAVObject* m_pkColliderObj;

    //@{
    /// Caching variable used to reduce unnecessary per-frame computations.
    NiTransform m_kLastColliderTransform;
    NiTransform m_kLastPSysTransform;
    //@}
};

NiSmartPointer(NiPSSphericalCollider);

#include "NiPSSphericalCollider.inl"

#endif  // #ifndef NIPSSPHERICALCOLLIDER_H
