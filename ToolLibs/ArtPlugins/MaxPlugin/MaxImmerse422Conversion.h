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

#ifndef MAXIMMERSE422CONVERSION_H
#define MAXIMMERSE422CONVERSION_H
#include "MaxImmerse.h"

class MaxImmerse422Conversion
{
public:
    static bool NeedsINIConversion(Interface* pkInterface);
    static bool NeedsNodeConversion(Interface* pkInterface);
    static void ConvertINIOptions(Interface* pkInterface);
    static void ConvertNodeOptions(Interface* pkInterface);
};
#endif
