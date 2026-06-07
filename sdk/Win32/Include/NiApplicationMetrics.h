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

#ifndef NIAPPLICATIONMETRICS_H
#define NIAPPLICATIONMETRICS_H

#include <NiMetricsLayer.h>
#include <NiMetricsScopeTimer.h>

#define NIMETRICS_APPLICATION_PREFIX NIMETRICS_PREFIX "app."
#define NIMETRICS_MEMORY_PREFIX NIMETRICS_PREFIX "memory."

class NiApplicationMetrics
{
public:
    enum NiMetric
    {
        // NiApplication-specific
        FRAMERATE,
        CULL_TIME,
        UPDATE_TIME,
        RENDER_TIME,
        CREATE_SCENE_TIME,

        INITIALIZED,

        // Memory specific
        ACTIVE_MEMORY,
        ACCUMULATED_MEMORY,
        ACTIVE_ALLOCATIONS,
        ACCUMULATED_ALLOCATIONS,
        ACTIVE_TRACKER_OVERHEAD,
        ACCUMULATED_TRACKER_OVERHEAD,
        UNUSED_BUT_ALLOCATED_MEMORY,

        NUM_METRICS
    };

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];
};

#if NIMETRICS
    #define NIMETRICS_APPLICATION_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiApplicationMetrics::ms_acNames[ \
            NiApplicationMetrics::eMetric], fValue);
    #define NIMETRICS_APPLICATION_TIMER(classname, kTimer, eMetric) \
        classname kTimer( \
        NiApplicationMetrics::ms_acNames[NiApplicationMetrics::eMetric]);
    #define NIMETRICS_APPLICATION_STARTTIMER(kTimer) \
        NIMETRICS_STARTTIMER(kTimer)
    #define NIMETRICS_APPLICATION_ENDTIMER(kTimer) \
        NIMETRICS_ENDTIMER(kTimer)
    #define NIMETRICS_APPLICATION_EVENT(eMetric, fValue) \
        NIMETRICS_ADDEVENT( \
        NiApplicationMetrics::ms_acNames[NiApplicationMetrics::eMetric], \
        fValue);
#else //#if NIMETRICS
    #define NIMETRICS_APPLICATION_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_APPLICATION_TIMER(classname, kTimer, eMetric)
    #define NIMETRICS_APPLICATION_STARTTIMER(kTimer)
    #define NIMETRICS_APPLICATION_ENDTIMER(kTimer)
    #define NIMETRICS_APPLICATION_EVENT(eMetric, fValue)
#endif //#if NIMETRICS

#endif //#ifndef NIAPPLICATIONMETRICS_H
