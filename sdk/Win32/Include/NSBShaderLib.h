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

#ifndef NSBSHADERLIB_H
#define NSBSHADERLIB_H

// DLL support
#include "NSBShaderLibLibType.h"
#include <NiShaderLibrary.h>

#if defined(_LIB)
bool NSBShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, const char* pszDirectories[], bool bRecurseSubFolders,
    NiShaderLibrary** ppkLibrary);
#endif  //#if defined(_LIB)

#endif  //#ifndef NSBSHADERLIB_H
