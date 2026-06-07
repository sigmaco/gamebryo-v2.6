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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluidCommonSemantics.h"

NiFixedString NiPhysXFluidCommonSemantics::ms_akSemantics[];

//---------------------------------------------------------------------------
void NiPhysXFluidCommonSemantics::_SDMInit()
{
    NiUInt16 usCount = 1;
ms_akSemantics[PHYSXFLUID_INVALID] = NULL;

#define DefSemantic(string) ms_akSemantics[PHYSXFLUID_##string] = #string; \
    usCount++;
    
    DefSemantic(POSITION);

#undef DefSemantic

    NIASSERT(usCount == PHYSXFLUID_SEMANTICSCOUNT);
}
//---------------------------------------------------------------------------
void NiPhysXFluidCommonSemantics::_SDMShutdown()
{
    NiUInt16 usCount = 0;

#define UndefSemantic(string) ms_akSemantics[PHYSXFLUID_##string] = NULL; \
    usCount++;
    
    UndefSemantic(INVALID);
    UndefSemantic(POSITION);

#undef UndefSemantic

    NIASSERT(usCount == PHYSXFLUID_SEMANTICSCOUNT);
}
//---------------------------------------------------------------------------
NiPhysXFluidCommonSemantics::CommonSemantics 
NiPhysXFluidCommonSemantics::GetSemanticEnum(const NiFixedString& kSemantic)
{
    for (NiUInt16 us = 0; us < PHYSXFLUID_SEMANTICSCOUNT; ++us)
    {
        if (kSemantic == ms_akSemantics[us])
        {
            return (CommonSemantics) us;
        }
    }

    return PHYSXFLUID_INVALID;
}
//---------------------------------------------------------------------------
