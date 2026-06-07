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

#ifndef NIMATERIALLIBRARYINTERFACE_H
#define NIMATERIALLIBRARYINTERFACE_H

// DLL Library interface function
#if defined(_USRDLL)
#if defined(NIMAIN_EXPORT)
    // DLL library project uses this
    #define NIMATERIALLIBRARY_ENTRY __declspec(dllimport)
#else
#if defined(NIMAIN_IMPORT)
    // client of DLL uses this
    #define NIMATERIALLIBRARY_ENTRY __declspec(dllexport)
#else
    // static library project uses this
    #define NIMATERIALLIBRARY_ENTRY
#endif
#endif

class NiMaterialLibrary;
class NiRenderer;

typedef bool (*NIMLI_LOADLIBRARYFUNCTION)(NiRenderer*, 
    NiMaterialLibrary*& pkLibrary, unsigned int uiWhichLibrary);
typedef unsigned int (*NIMLI_GETMATERIALLIBRARYCOUNTFUNCTION)(void);
typedef unsigned int (*NIMLI_GETCOMPILERVERSIONFUNCTION)(void);

extern "C"
{
NIMATERIALLIBRARY_ENTRY bool LoadMaterialLibrary(
    NiRenderer* pkRenderer, NiMaterialLibrary*& pkLibrary, 
    unsigned int uiWhichLibrary);
NIMATERIALLIBRARY_ENTRY unsigned int GetMaterialLibraryCount();
NIMATERIALLIBRARY_ENTRY unsigned int GetCompilerVersion(void);
};
#endif  //#if defined(_USRDLL)

#endif  //NIMATERIALLIBRARYINTERFACE_H
