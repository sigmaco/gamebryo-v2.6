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

#ifndef NIPHYSXSYNCARGS_H
#define NIPHYSXSYNCARGS_H

#include "NiPhysXLibType.h"

#include <NiSyncArgs.h>
#include "NiPhysXScene.h"

/**
    This class holds data for mesh modifiers and other parallel, synchronized
    tasks related to PhysX.

    Mesh modifiers that have their Submit or Complete functions called
    with sync points of NiSyncArgs::SYNC_PHYSICS_SIMULATE or
    NiSyncArgs::SYNC_PHSYICS_COMPLETED are guaranteed to have one of these
    objects as an argument.
*/
class NIPHYSX_ENTRY NiPhysXSyncArgs : public NiSyncArgs
{
public:
    /// Constructor sets the scene to NULL, time to 0, and forced to false.
    NiPhysXSyncArgs();

    /// The scene from which this set of SyncArgs originated
    NiPhysXScene* m_pkPhysXScene;
    
    /// Whether or not this is a forced update.
    bool m_bForce;
};

#include "NiPhysXSyncArgs.inl"

#endif //#ifndef NIPHYSXSYNCARGS_H
