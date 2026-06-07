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

#ifndef NIPSDRAGFORCE_H
#define NIPSDRAGFORCE_H

#include "NiPSForce.h"

/**
    Applies a linear drag force to particles.

    The center of the force is represented by a drag object in the scene
    graph. A drag axis specified in the local space of the drag object
    represents the direction of the force.
    
    Any particles within the specified range of the drag object experience the
    full drag force. The force is linearly diminished from the range distance
    to the range falloff distance, after which the force does not affect any
    particles.
    
    This force most closely approximates the behavior of the Drag space warp
    in 3ds max.
*/
class NIPARTICLE_ENTRY NiPSDragForce : public NiPSForce
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSDragForce);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kName The name of the force.
        @param pkDragObj The object in the scene graph whose world-space
            transformations will be used to represent the position and
            orientation of the bomb.
        @param kDragAxis A unit vector specified in the local space of the
            drag object that represents the direction of the drag force.
        @param fPercentage The amount of drag to apply to particles.
        @param fRange The distance up to which particles are affected by the
            full force.
        @param fRangeFalloff The distance at which particles cease to be
            affected by the force.
    */
    NiPSDragForce(
        const NiFixedString& kName,
        NiAVObject* pkDragObj,
        const NiPoint3& kDragAxis = NiPoint3::UNIT_X,
        float fPercentage = 0.05f,
        float fRange = NI_INFINITY,
        float fRangeFalloff = NI_INFINITY);
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
    /// transformations will be used to represent the position and orientation
    /// of the drag force.
    inline NiAVObject* GetDragObj() const;
    inline void SetDragObj(NiAVObject* pkDragObj);
    //@}

    //@{
    /// Accesses a unit vector specified in the local space of the drag object
    /// that represents the direction of the drag force.
    inline const NiPoint3& GetDragAxis() const;
    inline void SetDragAxis(const NiPoint3& kDragAxis);
    //@}

    //@{
    /**
        Accesses the amount of drag to apply to particles.

        The velocity of particle that are fully in range will be reduced by
        this amount every frame.
    */
    inline float GetPercentage() const;
    inline void SetPercentage(float fPercentage);
    //@}
   
    //@{
    /**
        Accesses the range of the drag force.

        Particles that are closer than this range are affected by the full
        force. Particles outside this range experience a diminished force.
    */
    inline float GetRange() const;
    inline void SetRange(float fDistance);
    //@}

    //@{
    /**
        Accesses the distance at which particle cease to be affected by the
        full force.

        Particles that are further than the range but closer than the range
        falloff will have the drag percentage linearly reduced by distance.
        Particles that are further than the range falloff are not affected by
        the force at all.
    */
    inline float GetRangeFalloff() const;
    inline void SetRangeFalloff(float fRangeFalloff);
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
    NiPSDragForce();
    //@}

    /// A structure containing the data used by this class.
    NiPSForceDefinitions::DragData m_kInputData;

    /// A pointer to the drag object in the scene graph.
    NiAVObject* m_pkDragObj;
};

NiSmartPointer(NiPSDragForce);

#include "NiPSDragForce.inl"

#endif  // #ifndef NIPSDRAGFORCE_H
