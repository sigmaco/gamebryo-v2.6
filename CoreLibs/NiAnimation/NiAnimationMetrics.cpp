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

#include "NiAnimationPCH.h"
#include "NiAnimationMetrics.h"

const char NiAnimationMetrics::ms_acNames
    [NiAnimationMetrics::NUM_METRICS][NIMETRICS_NAMELENGTH] =
{
    NIMETRICS_ANIMATION_PREFIX "ActorManagerUpdate",
    NIMETRICS_ANIMATION_PREFIX "UpdatedSequences",
    NIMETRICS_ANIMATION_PREFIX "ControllerManagerUpdate"
};
