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

#ifndef NIANIMATIONMETRICS_H
#define NIANIMATIONMETRICS_H

#include "NiAnimationLibType.h"
#include <NiMetricsLayer.h>
#include <NiMetricsScopeTimer.h>

#define NIMETRICS_ANIMATION_PREFIX NIMETRICS_PREFIX "animation."

class NIANIMATION_ENTRY NiAnimationMetrics
{
public:
    enum NiMetric
    {
        // time in seconds for NiActorManager::Update
        AM_UPDATE_TIME,
        
        // sequences updated in each NiControllerManager::Update call
        UPDATED_SEQUENCES,

        // time in seconds for NiControllerManager::Update
        CM_UPDATE_TIME,

        NUM_METRICS
    };

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];
};

#if NIMETRICS
    #define NIMETRICS_ANIMATION_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiAnimationMetrics::ms_acNames[ \
            NiAnimationMetrics::eMetric], fValue);
    #define NIMETRICS_ANIMATION_SCOPETIMER(eMetric) \
        NiMetricsScopePCTimer kMetricsTimer(NiAnimationMetrics::ms_acNames[ \
        NiAnimationMetrics::eMetric]);
#else //#if NIMETRICS
    #define NIMETRICS_ANIMATION_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_ANIMATION_SCOPETIMER(eMetric)
#endif //#if NIMETRICS

#endif //#ifndef NIANIMATIONMETRICS_H
