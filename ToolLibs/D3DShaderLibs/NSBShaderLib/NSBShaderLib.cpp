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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NSBShaderLibPCH.h"

#include "NSBShaderLib.h"
#include "NSBShaderLibrary.h"

#ifdef WIN32
#include "NSBD3D10ShaderLibrary.h"
#endif

#include "NSBLoader.h"
#include "NSBShaderLibLibType.h"
#include "NSBShaderLibSDM.h"

// These headers must be included for DLLs to build properly.
#if defined(WIN32)
#include <NiD3DShaderLibraryInterface.h>
#include <NiD3D10ShaderLibraryInterface.h>
#elif defined(_XENON)
#include <NiD3DShaderLibraryInterface.h>
#endif

#include <NiShaderAttributeDesc.h>
#include <NiShaderDesc.h>
#include <NiShaderLibraryDesc.h>

//---------------------------------------------------------------------------

static NSBShaderLibSDM NSBShaderLibSDMObject;

//---------------------------------------------------------------------------
#if defined(_USRDLL)
BOOL WINAPI DllMain(HINSTANCE, ULONG fdwReason, LPVOID)
{
    NiOutputDebugString("NSBShaderLib> DLLMain CALL - ");

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            //  Initialize anything needed here
            //  If failed, return FALSE
            NiOutputDebugString("PROCESS ATTACH!\n");
            NiStaticDataManager::ProcessAccumulatedLibraries();
        }
        break;
    case DLL_THREAD_ATTACH:
        {
            NiOutputDebugString("THREAD ATTACH!\n");
        }
        break;
    case DLL_PROCESS_DETACH:
        {
            //  Shutdown anything needed here
            NiOutputDebugString("PROCESS DETACH!\n");
            NiStaticDataManager::RemoveLibrary("NSBShaderLib");
        }
        break;
    case DLL_THREAD_DETACH:
        {
            NiOutputDebugString("THREAD DETACH!\n");
        }
        break;
    }

    return (TRUE);
}
//---------------------------------------------------------------------------
NSBSHADERLIB_ENTRY bool LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, const char* pszDirectories[], bool bRecurseSubFolders,
    NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

#if defined(WIN32)
    if (NiIsKindOf(NiDX9Renderer, pkRenderer) || pkRenderer == NULL)
    {
        NSBShaderLibrary* pkLibrary = NSBShaderLibrary::Create(
            iDirectoryCount, pszDirectories, bRecurseSubFolders);
        if (pkLibrary)
            pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);
        
        *ppkLibrary = pkLibrary;
    }
    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
    {
        NSBD3D10ShaderLibrary* pkLibrary = NSBD3D10ShaderLibrary::Create(
            iDirectoryCount, pszDirectories, bRecurseSubFolders);

        *ppkLibrary = pkLibrary;
    }
#elif defined(_XENON)
    NSBShaderLibrary* pkLibrary = NSBShaderLibrary::Create(iDirectoryCount, 
        pszDirectories, bRecurseSubFolders);
    if (pkLibrary)
        pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);
    
    *ppkLibrary = pkLibrary;
#else
    #error "Unknown platform";
#endif
    return (*ppkLibrary != NULL);
}
//---------------------------------------------------------------------------
NSBSHADERLIB_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
//---------------------------------------------------------------------------
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
#if defined(_LIB) || defined (_PS3)
//---------------------------------------------------------------------------
bool NSBShaderLib_LoadShaderLibrary(NiRenderer* pkRenderer, 
    int iDirectoryCount, const char* pszDirectories[], 
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

#if defined(WIN32)
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        NSBShaderLibrary* pkLibrary = NSBShaderLibrary::Create(
            iDirectoryCount, pszDirectories, bRecurseSubFolders);
        if (pkLibrary)
            pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);

        *ppkLibrary = pkLibrary;
        return (*ppkLibrary != NULL);
    }
    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
    {
        NSBD3D10ShaderLibrary* pkLibrary = NSBD3D10ShaderLibrary::Create(
            iDirectoryCount, pszDirectories, bRecurseSubFolders);

        *ppkLibrary = pkLibrary;
        return (*ppkLibrary != NULL);
    }
    else
    {
        ppkLibrary = NiAlloc(NiShaderLibrary*, 2);

        NSBShaderLibrary* pkLibrary = NSBShaderLibrary::Create(
            iDirectoryCount, pszDirectories, bRecurseSubFolders);
        if (pkLibrary)
            pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);
        ppkLibrary[0] = pkLibrary;

        NSBD3D10ShaderLibrary* pkD3D10Library = NSBD3D10ShaderLibrary::Create(
            iDirectoryCount, pszDirectories, bRecurseSubFolders);
        ppkLibrary[1] = pkD3D10Library;

        return ((ppkLibrary[0] != NULL) || (ppkLibrary[1] != NULL));
    }
#elif defined(_XENON)
    NSBShaderLibrary* pkLibrary = NSBShaderLibrary::Create(iDirectoryCount, 
        pszDirectories, bRecurseSubFolders);
    if (pkLibrary)
        pkLibrary->SetRenderer((NiD3DRenderer*)pkRenderer);

    *ppkLibrary = pkLibrary;
    return (*ppkLibrary != NULL);
#elif defined(_PS3)
    NSBShaderLibrary* pkLibrary = NSBShaderLibrary::Create(iDirectoryCount, 
        pszDirectories, bRecurseSubFolders);

    *ppkLibrary = pkLibrary;
    return (*ppkLibrary != NULL);
#else
    #error "Unknown platform";
    return false;
#endif

}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
