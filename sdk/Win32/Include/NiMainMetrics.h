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

#ifndef NIMAINMETRICS_H
#define NIMAINMETRICS_H

#include "NiMainLibType.h"
#include "NiMetricsLayer.h"
#include "NiMetricsScopeTimer.h"

#include <NiCriticalSection.h>

#define NIMETRICS_MAIN_PREFIX NIMETRICS_PREFIX "main."

class NIMAIN_ENTRY NiMainMetrics
{
public:
    enum NiMetric
    {
        // Number of NiAVObjects that were updated per frame
        NUM_UPDATES,

        NUM_METRICS
    };

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];

    static void RecordMetrics();
    static void IncrementUpdates();

protected:
    static NiUInt32 ms_uiNumUpdates;
};

#include "NiMainMetrics.inl"

#if NIMETRICS
    #define NIMETRICS_MAIN_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiMainMetrics::ms_acNames[ \
            NiMainMetrics::eMetric], fValue);
    #define NIMETRICS_MAIN_INCREMENTUPDATES() \
        NiMainMetrics::IncrementUpdates();
    #define NIMETRICS_MAIN_RECORDMETRICS() NiMainMetrics::RecordMetrics();
#else
    #define NIMETRICS_MAIN_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_MAIN_INCREMENTUPDATES()
    #define NIMETRICS_MAIN_RECORDMETRICS()
#endif // #if NIMETRICS

#endif //#ifndef NIMAINMETRICS_H
