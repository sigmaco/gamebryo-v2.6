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

#ifndef NICOLLISIONMETRICS_H
#define NICOLLISIONMETRICS_H

#include "NiCollisionLibType.h"
#include <NiMetricsLayer.h>
#include <NiMetricsScopeTimer.h>

#define NIMETRICS_COLLISION_PREFIX NIMETRICS_PREFIX "collision."

class NICOLLISION_ENTRY NiCollisionMetrics
{
public:
    enum NiMetric
    {
        // Time in seconds per NiCollisionGroup::TestCollisions call
        TEST_COLLISION_TIME,

        // Object comparisons per each NiCollisionGroup::TestCollisions call
        TEST_COMPARISIONS,

        // Sum for each type of comparison in TestHandleCollisions
        TEST_NIBOUND_NIBOUND,
        TEST_OBB_OBB,
        TEST_OBB_TRI,
        TEST_OBB_ABV,
        TEST_TRI_TRI,
        TEST_TRI_ABV,
        TEST_ABV_ABV,

        // Time in seconds per NiCollisionGroup::FindCollisions call
        FIND_COLLISION_TIME,

        // Object comparisons per each NiCollisionGroup::FindCollisions call
        FIND_COMPARISIONS,

        // Sum for each type of comparison in HandleCollisions
        FIND_NIBOUND_NIBOUND,
        FIND_OBB_OBB,
        FIND_OBB_TRI,
        FIND_OBB_ABV,
        FIND_TRI_TRI,
        FIND_TRI_ABV,
        FIND_ABV_ABV,

        // Time in seconds per NiPick::PickObjects
        PICK_TIME,

        // Number of pick results (before finding closest, if not
        // FIND_ALL)
        PICK_RESULTS,

        // Number of nodes tested per NiPick::PickObjects
        PICK_COMPARISONS_NODE,
        // Number of triangles tested per NiPick::PickObjects
        PICK_COMPARISONS_TRI,

        NUM_METRICS
    };

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];
};

#if NIMETRICS
    #define NIMETRICS_COLLISION_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiCollisionMetrics::ms_acNames[ \
            NiCollisionMetrics::eMetric], fValue);
    #define NIMETRICS_COLLISION_SCOPETIMER(eMetric) \
        NiMetricsScopePCTimer kMetricsTimer(NiCollisionMetrics::ms_acNames[ \
        NiCollisionMetrics::eMetric]);
#else //#if NIMETRICS
    #define NIMETRICS_COLLISION_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_COLLISION_SCOPETIMER(eMetric)
#endif //#if NIMETRICS

#endif //#ifndef NICOLLISIONMETRICS_H
