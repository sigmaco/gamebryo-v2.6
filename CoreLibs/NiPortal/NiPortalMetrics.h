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

#ifndef NIPORTALMETRICS_H
#define NIPORTALMETRICS_H

#include "NiPortalLibType.h"
#include <NiMetricsLayer.h>
#include <NiMetricsScopeTimer.h>

#define NIMETRICS_PORTAL_PREFIX NIMETRICS_PREFIX "portal."

class NIPORTAL_ENTRY NiPortalMetrics
{
public:
    enum NiMetric
    {
        // Time in seconds for each RoomGroup::OnVisible call
        ROOMGROUP_ONVISIBLE,

        // Time in seconds for each FixtureCuller::Process call
        FIXTURECULLER_PROCESS,

        NUM_METRICS
    };

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];
};

#if NIMETRICS
    #define NIMETRICS_PORTAL_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiPortalMetrics::ms_acNames[ \
            NiPortalMetrics::eMetric], fValue);
    #define NIMETRICS_PORTAL_SCOPETIMER(eMetric) \
        NiMetricsScopePCTimer kMetricsTimer(NiPortalMetrics::ms_acNames[ \
        NiPortalMetrics::eMetric]);
#else //#if NIMETRICS
    #define NIMETRICS_PORTAL_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_PORTAL_SCOPETIMER(eMetric)
#endif //#if NIMETRICS

#endif //#ifndef NIPORTALMETRICS_H
