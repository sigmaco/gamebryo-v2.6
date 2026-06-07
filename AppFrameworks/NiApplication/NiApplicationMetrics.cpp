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

#include "NiApplicationPCH.h"
#include "NiApplicationMetrics.h"

const char NiApplicationMetrics::ms_acNames
    [NiApplicationMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_APPLICATION_PREFIX "FPS",
    NIMETRICS_APPLICATION_PREFIX "frametime.CullTime",
    NIMETRICS_APPLICATION_PREFIX "frametime.UpdateTime",
    NIMETRICS_APPLICATION_PREFIX "frametime.RenderTime",
    NIMETRICS_APPLICATION_PREFIX "CreateSceneTime",

    NIMETRICS_APPLICATION_PREFIX "Initialized",

    NIMETRICS_MEMORY_PREFIX "ActiveMemory",
    NIMETRICS_MEMORY_PREFIX "AccumulatedMemory",
    NIMETRICS_MEMORY_PREFIX "ActiveAllocations",
    NIMETRICS_MEMORY_PREFIX "AccumulatedAllocations",
    NIMETRICS_MEMORY_PREFIX "ActiveTrackerOverhead",
    NIMETRICS_MEMORY_PREFIX "AccumulatedTrackerOverhead",
    NIMETRICS_MEMORY_PREFIX "UnusedButAllocatedMemory"
};
