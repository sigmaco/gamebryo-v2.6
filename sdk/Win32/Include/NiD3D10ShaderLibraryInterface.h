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

#ifndef NID3D10SHADERLIBRARYINTERFACE_H
#define NID3D10SHADERLIBRARYINTERFACE_H

// DLL Library interface function
#if defined(_USRDLL)
#if defined(NID3D10RENDERER_EXPORT)
    // DLL library project uses this
    #define NID3D10SHADERLIBRARY_ENTRY __declspec(dllimport)
#else
#if defined(NID3D10RENDERER_IMPORT)
    // client of DLL uses this
    #define NID3D10SHADERLIBRARY_ENTRY __declspec(dllexport)
#else
    // static library project uses this
    #define NID3D10SHADERLIBRARY_ENTRY
#endif
#endif

class NiRenderer;
class NiShaderLibrary;

typedef bool (*NID3D10SLI_LOADLIBRARY)(NiRenderer*, int, 
    const char*[], bool, NiShaderLibrary**);
typedef unsigned int (*NID3D10SLI_RUNPARSER)(const char*, bool);
typedef unsigned int (*NIGETCOMPILERVERSIONFUNCTION)(void);

extern "C"
{
NID3D10SHADERLIBRARY_ENTRY bool LoadShaderLibrary(
    NiRenderer* pkRenderer, int iDirectoryCount, const char* apcDirectories[],
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary);
NID3D10SHADERLIBRARY_ENTRY unsigned int RunShaderParser(
    const char* pcDirectory, bool bRecurseSubFolders);
NID3D10SHADERLIBRARY_ENTRY unsigned int GetCompilerVersion(void);
};
#endif  //#if defined(_USRDLL)

#endif  //NID3D10SHADERLIBRARYINTERFACE_H
