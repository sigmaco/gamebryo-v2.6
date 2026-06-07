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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiSyncArgs.h"

//---------------------------------------------------------------------------
const char* NiSyncArgs::GetSyncPointName(SyncPoint uiSyncPt)
{
    switch (uiSyncPt)
    {
    case SYNC_ANY:
        return "SyncAny";
    case SYNC_UPDATE:
        return "SyncUpdate";
    case SYNC_POST_UPDATE:
        return "SyncPostUpdate";
    case SYNC_VISIBLE:
        return "SyncVisible";
    case SYNC_RENDER:
        return "SyncRender";
    case SYNC_PHYSICS_SIMULATE:
        return "SyncPhysicsSimulate";
    case SYNC_PHYSICS_COMPLETED:
        return "SyncPhysicsCompleted";
    case SYNC_REFLECTIONS:
        return "SyncReflections";
    default:;
    }
    
    return "SyncPointUnknown";
}
//---------------------------------------------------------------------------

