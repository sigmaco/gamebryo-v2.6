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

#include "NiPSCommonSemantics.h"

NiFixedString NiPSCommonSemantics::ms_akSemantics[];

//---------------------------------------------------------------------------
void NiPSCommonSemantics::_SDMInit()
{
    NiUInt16 usCount = 1;

    ms_akSemantics[PS_INVALID] = NULL;

#define DefSemantic(string) ms_akSemantics[PS_##string] = #string; usCount++;
    
    DefSemantic(PARTICLEPOSITION);
    DefSemantic(PARTICLEVELOCITY);
    DefSemantic(PARTICLEAGE);
    DefSemantic(PARTICLELIFESPAN);
    DefSemantic(PARTICLELASTUPDATE);
    DefSemantic(PARTICLEFLAGS);
    DefSemantic(PARTICLERADIUS);
    DefSemantic(PARTICLESIZE);
    DefSemantic(PARTICLEROTAXIS);
    DefSemantic(PARTICLEROTANGLE);
    DefSemantic(PARTICLEROTSPEED);
    DefSemantic(PARTICLECOLOR);

#undef DefSemantic

    NIASSERT(usCount == PS_SEMANTICSCOUNT);
}
//---------------------------------------------------------------------------
void NiPSCommonSemantics::_SDMShutdown()
{
    NiUInt16 usCount = 0;

#define UndefSemantic(string) ms_akSemantics[PS_##string] = NULL; usCount++;
    
    UndefSemantic(INVALID);
    UndefSemantic(PARTICLEPOSITION);
    UndefSemantic(PARTICLEVELOCITY);
    UndefSemantic(PARTICLEAGE);
    UndefSemantic(PARTICLELIFESPAN);
    UndefSemantic(PARTICLELASTUPDATE);
    UndefSemantic(PARTICLEFLAGS);
    UndefSemantic(PARTICLERADIUS);
    UndefSemantic(PARTICLESIZE);
    UndefSemantic(PARTICLEROTAXIS);
    UndefSemantic(PARTICLEROTANGLE);
    UndefSemantic(PARTICLEROTSPEED);
    UndefSemantic(PARTICLECOLOR);

#undef UndefSemantic

    NIASSERT(usCount == PS_SEMANTICSCOUNT);
}
//---------------------------------------------------------------------------
NiPSCommonSemantics::CommonSemantics NiPSCommonSemantics::GetSemanticEnum(
    const NiFixedString& kSemantic)
{
    for (NiUInt16 us = 0; us < PS_SEMANTICSCOUNT; ++us)
    {
        if (kSemantic == ms_akSemantics[us])
        {
            return (CommonSemantics) us;
        }
    }

    return PS_INVALID;
}
//---------------------------------------------------------------------------
