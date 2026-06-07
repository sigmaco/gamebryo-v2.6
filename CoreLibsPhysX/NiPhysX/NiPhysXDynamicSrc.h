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

#ifndef NIPHYSXDYNAMICSRC_H
#define NIPHYSXDYNAMICSRC_H

#include "NiPhysXLibType.h"

#include "NiPhysXRigidBodySrc.h"
#include <NiControllerSequence.h>


/**
    NiPhysXDynamicSrc objects are intended for pushing Gamebryo scene graph
    state to a PhysX dynamic or kinematic actor.
    
    These objects are used for both kinematic actors and actors that are
    dynamic (although the latter is not recommended). They also provide
    feedback of the blended state of a PhysX controlled character back to
    the PhysX actor doing the controlling.

    This class provides only the the UpdateToActors function. All member
    variables and other functionality are inherited from the base class.

    @note Interpolation will fail for fast rotating objects due to the
        fundamental limitations of rotation representations and discrete time
        sampling. Disable interpolation for fast moving objects. 
*/
class NIPHYSX_ENTRY NiPhysXDynamicSrc : public NiPhysXRigidBodySrc
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXDynamicSrc);
    /// @endcond

public:
    /// The constructor passes its arguments directly to the base class.
    NiPhysXDynamicSrc(NiAVObject* pkSource, NxActor* pkActor);
    
    /// Destructor.
    ~NiPhysXDynamicSrc();
    
    /**
        Push the state onto the PhysX actors.
        
        If the interpolation flag (defined in NiPhysXSrc) is false, this
        function uses the most recent internally cached transformation with
        either NxActor::moveGlobalPose or NxActor::setGlobalPose to set the
        actor's pose. If interpolation is active, then the pose is interpolated
        for time fTBegin for dynamic actors or fTEnd for kinematic actors.
        Regardless of the interpolation flag, when the actor
        is dynamic the velocity of the object is estimated using the two stored
        poses and is set with NxActor::setLinearVelocity and
        NxActor::setAngularVelocity. 
    */
    virtual void UpdateToActors(const float fTBegin, const float fTEnd);
    
    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
    
protected:
    /// Used for cloning and streaming.
    NiPhysXDynamicSrc();
};

NiSmartPointer(NiPhysXDynamicSrc);

#endif  // #ifndef NIPHYSXDYNAMICSRC_H
