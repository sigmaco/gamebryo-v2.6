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

#ifndef NIPRIMITIVETYPE_H
#define NIPRIMITIVETYPE_H

#include "NiMainLibType.h"
#include <NiRTLib.h>

class NIMAIN_ENTRY NiPrimitiveType
{
public:

    enum Type
    {
        PRIMITIVE_TRIANGLES,
        PRIMITIVE_TRISTRIPS,
        PRIMITIVE_LINES,
        PRIMITIVE_LINESTRIPS,
        PRIMITIVE_QUADS,
        PRIMITIVE_POINTS,
        PRIMITIVE_MAX
        // if updating, also update GetPrimitiveTypeString()
    };

    static const char* GetStringFromType(Type eType);
};

#endif
