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

#ifndef NIPHYSXKINEMATICSRC_H
#define NIPHYSXKINEMATICSRC_H

#include "NiPhysXLibType.h"

#include "NiPhysXRigidBodySrc.h"

/**
    NiPhysXKinematicSrc objects are intended for use when a PhysX kinematic
    actor is driven by the state of an animated Gamebryo object.
    
    @note This class is targeted for removal in future versions of Gamebryo.
        Do not use it in new code. Rather, use NiPhysXDynamicSrc.

    It is assumed that the target actor is kinematic, and not dynamic (to drive
    a dynamic actor, use NiPhysXDynamicSrc). 

    @note Fast Spinning Objects: Interpolation will fail for fast rotating
        objects due to the fundamental limitations of rotation representations
        and discrete time sampling. Disable interpolation for fast moving
        objects.
*/
class NIPHYSX_ENTRY NiPhysXKinematicSrc : public NiPhysXRigidBodySrc
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXKinematicSrc);
    /// @endcond

public:
    /// The constructor passes its arguments directly to the base class. 
    NiPhysXKinematicSrc(NiAVObject* pkSource, NxActor* pkActor);

    /// Destructor does nothing.
    ~NiPhysXKinematicSrc();
    
    /**
        Update state to the PhysX actors.
        
        If the interpolation flag (defined in NiPhysXSrc) is false, this
        function uses the NxActor::moveGlobalPose with the most recent stored
        pose. Otherwise, it interpolates or extrapolates the stored state to
        time fTEnd and uses the result for NxActor::moveGlobalPose. This
        results in the actor moving to the new pose during the course of the
        simulation step. See the Multi-threading documentation for a
        discussion of the implications of extrapolating kinematic sources. 
    */
    virtual void UpdateToActors(const float fTBegin, const float fTEnd);

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// @endcond
    
protected:
    NiPhysXKinematicSrc();
};

NiSmartPointer(NiPhysXKinematicSrc);

#endif  // #ifndef NIPHYSXKINEMATICSRC_H
