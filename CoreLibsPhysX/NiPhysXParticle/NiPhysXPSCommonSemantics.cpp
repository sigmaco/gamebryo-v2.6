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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXPSCommonSemantics.h"

NiFixedString NiPhysXPSCommonSemantics::ms_akSemantics[];

//---------------------------------------------------------------------------
void NiPhysXPSCommonSemantics::_SDMInit()
{
    NiUInt16 usCount = 1;

    ms_akSemantics[PHYSXPS_INVALID] = NULL;

#define DefSemantic(string) ms_akSemantics[PHYSXPS_##string] = #string; \
    usCount++;
    
    DefSemantic(ACTORPOSE);
    DefSemantic(ACTORVELOCITY);
    DefSemantic(ACTORLASTVELOCITY);

#undef DefSemantic

    NIASSERT(usCount == PHYSXPS_SEMANTICSCOUNT);
}
//---------------------------------------------------------------------------
void NiPhysXPSCommonSemantics::_SDMShutdown()
{
    NiUInt16 usCount = 0;

#define UndefSemantic(string) ms_akSemantics[PHYSXPS_##string] = NULL; \
    usCount++;
    
    UndefSemantic(INVALID);
    UndefSemantic(ACTORPOSE);
    UndefSemantic(ACTORVELOCITY);
    UndefSemantic(ACTORLASTVELOCITY);

#undef UndefSemantic

    NIASSERT(usCount == PHYSXPS_SEMANTICSCOUNT);
}
//---------------------------------------------------------------------------
NiPhysXPSCommonSemantics::CommonSemantics 
NiPhysXPSCommonSemantics::GetSemanticEnum(const NiFixedString& kSemantic)
{
    for (NiUInt16 us = 0; us < PHYSXPS_SEMANTICSCOUNT; ++us)
    {
        if (kSemantic == ms_akSemantics[us])
        {
            return (CommonSemantics) us;
        }
    }

    return PHYSXPS_INVALID;
}
//---------------------------------------------------------------------------
