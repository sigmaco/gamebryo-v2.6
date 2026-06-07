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

#ifndef NIPSMETRICS_H
#define NIPSMETRICS_H

#include "NiParticleLibType.h"
#include <NiMetricsLayer.h>
#include <NiMetricsScopeTimer.h>

#define NIMETRICS_PARTICLE_PREFIX NIMETRICS_PREFIX "particle."

/**
    Helper class for reporting particle system metrics.

    This class contains enumerations that are used when reporting metrics
    about particle system classes. The header file also contains several
    macros that are used to report the metrics.
*/
class NIPARTICLE_ENTRY NiPSMetrics
{
public:
    /// Defines metrics values that can be reported for particle systems.
    enum NiMetric
    {
        /// Number of particles updated in NiPSParticleSystem::Update.
        UPDATED_PARTICLES,

        /// Number of particles spawned in NiPSParticleSystem::Update.
        PARTICLES_SPAWNED,

        /// Number of particles destroyed in NiPSParticleSystem::Update.
        PARTICLES_DESTROYED,

        /// Time for each particle update call (in seconds).
        UPDATE_PSYS_TIME,

        /// Maximum number of metrics enumerations. Invalid to use as an
        /// enumeration.
        NUM_METRICS
    };

    /// Array of metrics names.
    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];
};

#if NIMETRICS
    #define NIMETRICS_PARTICLE_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiPSMetrics::ms_acNames[NiPSMetrics::eMetric], \
        fValue);
    #define NIMETRICS_PARTICLE_SCOPETIMER(eMetric) \
        NiMetricsScopePCTimer kMetricsTimer(NiPSMetrics::ms_acNames[ \
        NiPSMetrics::eMetric]);
#else //#if NIMETRICS
    #define NIMETRICS_PARTICLE_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_PARTICLE_SCOPETIMER(eMetric)
#endif //#if NIMETRICS

#endif //#ifndef NIPSMETRICS_H
