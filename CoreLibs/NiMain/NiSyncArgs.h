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

#ifndef NISYNCARGS_H
#define NISYNCARGS_H

#include "NiMemObject.h"
#include "NiRTTI.h"
#include "NiUniversalTypes.h"

/**
    Sync arguments are passed to mesh modifiers and other parallel
    tasks to convey data required for submitting and completing tasks.
    
    The class also defines the SyncPoint type for specifying times when
    an application must synchronize for some reason. Several Gamebryo standard
    sync points are also defined here in the SyncPoints enumeration.
*/
class NIMAIN_ENTRY NiSyncArgs : public NiMemObject
{
public:
    /// Type definition for synchronization points.
    typedef NiUInt16 SyncPoint;
    
    /// Predefined sync points.
    enum {
        /// Value used when no specific sync point is desired.
        SYNC_ANY = 0x8000,

        /// Synchronize when an object is updated.
        SYNC_UPDATE = 0x8010,

        /// Synchronize when an entire scene graph has been updated.
        SYNC_POST_UPDATE = 0x8020,

        /// Synchronize when an object is determined to be potentially
        /// visible.
        SYNC_VISIBLE = 0x8030,

        /// Synchronize when an object is rendered.
        SYNC_RENDER = 0x8040,
        
        /// Synchronize when a physics simulation step is about to begin.
        SYNC_PHYSICS_SIMULATE = 0x8050,
        
        /// Synchronize when a physics simulation step has produced results.
        SYNC_PHYSICS_COMPLETED = 0x8060,

        /// Syncronize after all data necessary to calculate reflections is ready.
        SYNC_REFLECTIONS = 0x8070
    } SyncPoints;

    /// The sync point at which we are starting a task, or SYNC_ANY
    SyncPoint m_uiSubmitPoint;

    /// The sync point at which the task should finish, or SYNC_ANY
    SyncPoint m_uiCompletePoint;

    /// Convert the sync points into a task group ID suitable
    /// for passing to a NiSPWorkflowManage.
    static NiUInt32 GetTaskGroupID(SyncPoint eSubmit, SyncPoint eComplete);

    /// Convert the sync points into a task group ID and mask suitable
    /// for passing to a NiSPWorkflowManager
    inline void GetTaskGroupID(NiUInt32& uiTaskGroupID,
        NiUInt32& uiTaskGroupMask);

    /// Get the string form of a sync point.
    static const char* GetSyncPointName(SyncPoint uiSyncPt);
};

#include "NiSyncArgs.inl"

#endif // #ifndef NISYNCARGS_H
