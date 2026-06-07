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

#ifndef NIPSPLANARCOLLIDER_H
#define NIPSPLANARCOLLIDER_H

#include "NiPSCollider.h"
#include <NiTransform.h>

/**
    A planar collider for particles.

    The planar collider is defined by several parameters. A position in space,
    defined by the world translation of a collider object in the scene graph,
    sits at the center of the collider. An x-axis and y-axis, defined in the
    collider object's model space, define a plane at that point. The normal of
    the plane is the cross product of the x-axis and the y-axis. A width and a
    height determine how far along the x-axis and y-axis the collider extends.
*/
class NIPARTICLE_ENTRY NiPSPlanarCollider : public NiPSCollider
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSPlanarCollider);
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
        @param fWidth The width of the collider, measured along kXAxis.
        @param fHeight The height of the collider, measured along kYAxis.
        @param kXAxis A unit vector in the model space of pkColliderObj that
            represents one axis defining a plane.
        @param kYAxis A unit vector in the model space of pkColliderObj that
            represents one axis defining a plane.
    */
    NiPSPlanarCollider(
        float fBounce = 1.0f,
        bool bSpawnOnCollide = false,
        bool bDieOnCollide = false,
        NiPSSpawner* pkSpawner = NULL,
        NiAVObject* pkColliderObj = NULL,
        float fWidth = 1.0f,
        float fHeight = 1.0f,
        const NiPoint3& kXAxis = NiPoint3::UNIT_X,
        const NiPoint3& kYAxis = NiPoint3::UNIT_Y);
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
    /// Accesses the width of the collider, measured along the plane's x-axis.
    inline float GetWidth() const;
    inline void SetWidth(float fWidth);
    //@}

    //@{
    /// Accesses the height of the collider, measured along the plane's y-axis.
    inline float GetHeight() const;
    inline void SetHeight(float fHeight);
    //@}

    //@{
    /// Accesses one of the unit vectors defining the plane, specified in the
    /// model space of the collider object.
    inline const NiPoint3& GetXAxis() const;
    inline void SetXAxis(const NiPoint3& kXAxis);
    //@}

    //@{
    /// Accesses one of the unit vectors defining the plane, specified in the
    /// model space of the collider object.
    inline const NiPoint3& GetYAxis() const;
    inline void SetYAxis(const NiPoint3& kYAxis);
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /// A structure containing the data used by this class.
    NiPSColliderDefinitions::PlanarColliderData m_kInputData;

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

NiSmartPointer(NiPSPlanarCollider);

#include "NiPSPlanarCollider.inl"

#endif  // #ifndef NIPSPLANARCOLLIDER_H
