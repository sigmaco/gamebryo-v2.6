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

#include <NiEnumerationStrings.h>
#include "NiPrimitiveType.h"

//---------------------------------------------------------------------------
const char* NiPrimitiveType::GetStringFromType(NiPrimitiveType::Type eType)
{
    const char* pcStr = NULL;

    ENUM_STRING_BEGIN(eType, pcStr);
    ENUM_STRING_ITEM(PRIMITIVE_TRIANGLES);
    ENUM_STRING_ITEM(PRIMITIVE_TRISTRIPS);
    ENUM_STRING_ITEM(PRIMITIVE_LINES);
    ENUM_STRING_ITEM(PRIMITIVE_LINESTRIPS);
    ENUM_STRING_ITEM(PRIMITIVE_QUADS);
    ENUM_STRING_ITEM(PRIMITIVE_POINTS);
    ENUM_STRING_END(eType, pcStr);
    return pcStr;
}
//---------------------------------------------------------------------------
