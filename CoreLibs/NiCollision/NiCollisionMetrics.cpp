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

#include "NiCollisionPCH.h"
#include "NiCollisionMetrics.h"

const char NiCollisionMetrics::ms_acNames
    [NiCollisionMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_COLLISION_PREFIX "TestTime",
    NIMETRICS_COLLISION_PREFIX "TestCompares",
    NIMETRICS_COLLISION_PREFIX "test.NiBound_NiBound",
    NIMETRICS_COLLISION_PREFIX "test.OBB-OBB",
    NIMETRICS_COLLISION_PREFIX "test.OBB-TRI",
    NIMETRICS_COLLISION_PREFIX "test.OBB-ABV",
    NIMETRICS_COLLISION_PREFIX "test.TRI-TRI",
    NIMETRICS_COLLISION_PREFIX "test.TRI-ABV",
    NIMETRICS_COLLISION_PREFIX "test.ABV-ABV",
    NIMETRICS_COLLISION_PREFIX "FindTime",
    NIMETRICS_COLLISION_PREFIX "FindCompares",
    NIMETRICS_COLLISION_PREFIX "find.NiBound_NiBound",
    NIMETRICS_COLLISION_PREFIX "find.OBB-OBB",
    NIMETRICS_COLLISION_PREFIX "find.OBB-TRI",
    NIMETRICS_COLLISION_PREFIX "find.OBB-ABV",
    NIMETRICS_COLLISION_PREFIX "find.TRI-TRI",
    NIMETRICS_COLLISION_PREFIX "find.TRI-ABV",
    NIMETRICS_COLLISION_PREFIX "find.ABV-ABV",
    NIMETRICS_COLLISION_PREFIX "PickTime",
    NIMETRICS_COLLISION_PREFIX "PickResults",
    NIMETRICS_COLLISION_PREFIX "PickNodeComparisons",
    NIMETRICS_COLLISION_PREFIX "PickTriComparisons"
};
