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
#include "NiParticlePCH.h"

#include "NiPSMetrics.h"

const char NiPSMetrics::ms_acNames
    [NiPSMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_PARTICLE_PREFIX "UpdatedParticles",
    NIMETRICS_PARTICLE_PREFIX "ParticlesSpawned",
    NIMETRICS_PARTICLE_PREFIX "ParticlesDestroyed",
    NIMETRICS_PARTICLE_PREFIX "PSysUpdateTime"
};
