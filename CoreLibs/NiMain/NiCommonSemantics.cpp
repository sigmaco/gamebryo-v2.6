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
#include "NiMainPCH.h"

#include "NiCommonSemantics.h"

NiFixedString NiCommonSemantics::ms_kStrings[];

//---------------------------------------------------------------------------
void NiCommonSemantics::_SDMInit()
{
    unsigned int uiCount = 1;
    ms_kStrings[E__Invalid] = NULL;
#define Macro(string) ms_kStrings[E_##string] = #string; uiCount++;
#define Unused(string) ms_kStrings[E_##string] = NULL; uiCount++;
    
    // Vertex semantics
    Macro(POSITION);
    Macro(NORMAL);
    Macro(BINORMAL);
    Macro(TANGENT);
    Macro(TEXCOORD);
    Macro(BLENDWEIGHT);
    Macro(BLENDINDICES);
    Macro(COLOR);
    Macro(PSIZE);
    Macro(TESSFACTOR);
    Macro(DEPTH);
    Macro(FOG);
    Macro(POSITIONT);
    Macro(SAMPLE);
    Macro(DATASTREAM);
    Macro(INDEX);

    // Skinning semantics
    Macro(BONEMATRICES);
    Macro(BONE_PALETTE);
    Unused(UNUSED0);
    Macro(POSITION_BP);
    Macro(NORMAL_BP);
    Macro(BINORMAL_BP);
    Macro(TANGENT_BP);

    // Morph weights semantics
    Macro(MORPHWEIGHTS);

    // Normal sharing semantics, for use in runtime normal calculation
    Macro(NORMALSHAREINDEX);
    Macro(NORMALSHAREGROUP);

    // Instancing Semantics
    Macro(TRANSFORMS);
    Macro(INSTANCETRANSFORMS);

    // Display list semantics
    Macro(DISPLAYLIST);

#undef Macro 
#undef Unused
    NIASSERT(uiCount == E__EnumerationCount);
}
//---------------------------------------------------------------------------
void NiCommonSemantics::_SDMShutdown()
{
    unsigned int uiCount = 1;
    ms_kStrings[E__Invalid] = NULL;
#define Macro(string) ms_kStrings[E_##string] = NULL; uiCount++;

    // Vertex semantics
    Macro(POSITION);
    Macro(NORMAL);
    Macro(BINORMAL);
    Macro(TANGENT);
    Macro(TEXCOORD);
    Macro(BLENDWEIGHT);
    Macro(BLENDINDICES);
    Macro(COLOR);
    Macro(PSIZE);
    Macro(TESSFACTOR);
    Macro(DEPTH);
    Macro(FOG);
    Macro(POSITIONT);
    Macro(SAMPLE);
    Macro(DATASTREAM);
    Macro(INDEX);

    // Skinning semantics
    Macro(BONEMATRICES);
    Macro(BONE_PALETTE);
    Macro(UNUSED0);
    Macro(POSITION_BP);
    Macro(NORMAL_BP);
    Macro(BINORMAL_BP);
    Macro(TANGENT_BP);

    // Morph weights semantic
    Macro(MORPHWEIGHTS);

    // Normal sharing semantics, for use in runtime normal calculation
    Macro(NORMALSHAREINDEX);
    Macro(NORMALSHAREGROUP);

    // Instancing Semantics
    Macro(TRANSFORMS);
    Macro(INSTANCETRANSFORMS);

    // Display list semantics
    Macro(DISPLAYLIST);

#undef Macro 
    NIASSERT(uiCount == E__EnumerationCount);
}
//---------------------------------------------------------------------------
