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

#ifndef NIPHYSXTOOLS_H
#define NIPHYSXTOOLS_H

#include "NiPhysXToolsLibType.h"

#include <NiSystem.h>
#include <NiMain.h>
#include <NiPhysX.h>
#include <NiPhysXFluid.h>
#include <NiPhysXParticle.h>

#include <NxPhysics.h>
#include <NxCooking.h>


class NIPHYSXTOOLS_ENTRY NiPhysXTools : NiMemObject
{
public:
    /// Enumeration of methods for obtaining a velocity.
    enum VelocitySource
    {
        Zero = 0, /// Set the velocity to zero.
        DefaultState = 1 /// Copy the velocity from the default state.
    };

    /**
        Add state to a PhysX prop based on the scene graph state.
        
        This function iterates through every actor descriptor in
        the prop, and adds the scene graph state as a new state in the
        appropriate actor if that actor has a source or destination.
        The prop's set of states is updated to reflect the new state.
        
        PhysX materials for the new state are copied from the default state.
        Actors that do not have a Gamebryo node associated with them also
        have the new state copied from the default.
        
        @param pkProp The NiPhysX prop containig the sources, destinations,
            actors and materials.
        @param kStateName The name to record for this state. If a state of
            this name already exists in the prop, then it is replaced with the
            new state.
        @param eVelocitySource The method to use for obtaining velocity for
            the state.
    */
    static void AddStateFromSceneGraph(NiPhysXProp* pkProp,
        NiFixedString& kStateName, VelocitySource eVelocitySource = Zero);
    
protected:
    /// Extracts state for a single actor.
    static void AddStateFromTransformDest(NiPhysXActorDesc* pkActorDesc,
        NiPhysXTransformDest* pkDest, float fScale, NiUInt32 uiStateIndex);
    static void AddStateFromDynamicSrc(NiPhysXActorDesc* pkActorDesc,
        NiPhysXDynamicSrc* pkSrc, float fScale, NiUInt32 uiStateIndex);
    static void AddStateFromCloth(NiPhysXClothDesc* pkClothDesc,
        float fScale, NiUInt32 uiStateIndex);
};

#endif  // #ifndef NIPHYSXTOOLS_H

