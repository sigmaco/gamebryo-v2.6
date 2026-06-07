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

#ifndef NID3DSHADERLIBRARYINTERFACE_H
#define NID3DSHADERLIBRARYINTERFACE_H

// DLL Library interface function
#if defined(_USRDLL)
#if defined(NIDX9RENDERER_EXPORT)
    // DLL library project uses this
    #define NID3DSHADERLIBRARY_ENTRY __declspec(dllimport)
#else
#if defined(NIDX9RENDERER_IMPORT)
    // client of DLL uses this
    #define NID3DSHADERLIBRARY_ENTRY __declspec(dllexport)
#else
    // static library project uses this
    #define NID3DSHADERLIBRARY_ENTRY
#endif
#endif

class NiD3DShaderLibrary;

typedef bool (*NID3DSLI_LOADLIBRARY)(NiRenderer*, int, 
    const char*[], bool, NiShaderLibrary** ppkLibrary);
typedef unsigned int (*NID3DSLI_RUNPARSER)(const char*, bool);
typedef unsigned int (*NIGETCOMPILERVERSIONFUNCTION)(void);

extern "C"
{
NID3DSHADERLIBRARY_ENTRY bool LoadShaderLibrary(
    NiRenderer* pkRenderer, int iDirectoryCount, 
    const char* pszDirectories[], bool bRecurseSubFolders, 
    NiShaderLibrary** ppkLibrary);
NID3DSHADERLIBRARY_ENTRY unsigned int RunShaderParser(
    const char* pszDirectory, bool bRecurseSubFolders);
NID3DSHADERLIBRARY_ENTRY unsigned int GetCompilerVersion(void);
};
#endif  //#if defined(_USRDLL)

#endif  //NID3DSHADERLIBRARYINTERFACE_H
