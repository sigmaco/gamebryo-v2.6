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

#include "NiMaterialLibrary.h"
#include "NiMaterialLibraryInterface.h"
#include "NiRenderer.h"

#if defined(_USRDLL)

NiTPrimitiveSet<NiModuleRef> NiMaterialLibrary::ms_kModules;
//---------------------------------------------------------------------------
void NiMaterialLibrary::FreeAllModules()
{
    for (unsigned int ui = 0; ui < ms_kModules.GetSize(); ui++)
    {
        FreeLibrary(ms_kModules.GetAt(ui));
    }

    ms_kModules.RemoveAll();
    ms_kModules.Realloc();
}
//---------------------------------------------------------------------------
bool NiMaterialLibrary::LoadMaterialLibraryDLL(const char* pcLibName)
{
    NIMLI_LOADLIBRARYFUNCTION pfnLoad = NULL;
    NIMLI_GETMATERIALLIBRARYCOUNTFUNCTION pfnLibraryCount = NULL;

    HMODULE hLibrary = GetModuleHandle(pcLibName);
    if (hLibrary == NULL)
    {
        // Load the interface library
        hLibrary = LoadLibrary(pcLibName);
        if (hLibrary == NULL)
        {
#ifdef NIDEBUG
            char acString[1024];
            NiSprintf(acString, 1024, "WARNING> LoadLibrary failed on %s\n",
                pcLibName);
            NiOutputDebugString(acString);
            DWORD dwError = GetLastError();
            NiSprintf(acString, 1024,  
                "Get last error returned 0x%08x\n", dwError);
            NiOutputDebugString(acString);
#endif
            return false;
        }

        
        // Ensure library was built with the same compiler
        NIMLI_GETCOMPILERVERSIONFUNCTION pfnGetCompilerVersionFunc =
            (NIMLI_GETCOMPILERVERSIONFUNCTION)GetProcAddress(hLibrary, 
            "GetCompilerVersion");
        if (pfnGetCompilerVersionFunc)
        {
            unsigned int uiShaderVersion = pfnGetCompilerVersionFunc();
            if (uiShaderVersion != (_MSC_VER))
            {
#ifdef NIDEBUG
                char acString[1024];
                NiSprintf(acString, 1024, "WARNING> Library \"%s\"was built"
                    " on different version of Visual Studio. Failing the load"
                    ".\n",pcLibName);
                NiOutputDebugString(acString);
#endif
                FreeLibrary(hLibrary);
                return false;
            }
        }
        
        // Fetch the library creation function
        pfnLoad = (NIMLI_LOADLIBRARYFUNCTION)GetProcAddress(hLibrary, 
            "LoadMaterialLibrary");
        pfnLibraryCount = (NIMLI_GETMATERIALLIBRARYCOUNTFUNCTION)
            GetProcAddress(hLibrary, "GetMaterialLibraryCount");


        if (pfnLoad == 0)
        {
#ifdef NIDEBUG
            char acString[1024];
            NiSprintf(acString, 1024, "WARNING> Library \"%s\" does not have "
                "a LoadMaterialLibrary function.\n",pcLibName);
            NiOutputDebugString(acString);
#endif
            FreeLibrary(hLibrary);
            return false;
        }

        if (pfnLibraryCount == 0)
        {
#ifdef NIDEBUG
            char acString[1024];
            NiSprintf(acString, 1024, "WARNING> Library \"%s\" does not have "
                "a GetMaterialLibraryCount function.\n", pcLibName);
            NiOutputDebugString(acString);
#endif
            FreeLibrary(hLibrary);
            return false;
        }
    }
    else
    {
        // It was found once; it better be found again!
        pfnLoad = (NIMLI_LOADLIBRARYFUNCTION)GetProcAddress(hLibrary, 
            "LoadMaterialLibrary");
        NIASSERT(pfnLoad);

        pfnLibraryCount = (NIMLI_GETMATERIALLIBRARYCOUNTFUNCTION)
            GetProcAddress(hLibrary, "GetMaterialLibraryCount");
        NIASSERT(pfnLibraryCount);
    }

    unsigned int uiCount = pfnLibraryCount();
    ms_kModules.Add(hLibrary);

    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NiMaterialLibrary* pkLibrary = NULL;
        bool bSuccess = pfnLoad(NiRenderer::GetRenderer(), pkLibrary, ui);

        if (!bSuccess || pkLibrary == NULL)
        {
    #ifdef NIDEBUG
            char acString[1024];
            NiSprintf(acString, 1024, "Failed to load \"%s\".\n",
                pcLibName);
            NiOutputDebugString(acString);
            FreeLibrary(hLibrary);
    #endif
            return false;
        }

        NiMaterialLibrary* pkExistingLib = GetMaterialLibrary(
            pkLibrary->GetName());

        if (pkExistingLib && pkExistingLib != pkLibrary)
        {
            NiOutputDebugString("Library ");
            NiOutputDebugString(pkLibrary->GetName());
            NiOutputDebugString(" has been reloaded!\n");
            NiOutputDebugString("REPLACING EXISTING SHADER LIBRARY!\n");

            // Deleting should cause this library to remove itself
            NiDelete pkExistingLib;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
#else
// Define a dummy function to prevent linker warnings in static lib builds
// rather than specifying /IGNORE:4221 to the linker.
NIMAIN_ENTRY void NiMaterialLib_Dummy()
{
    return;
}
#endif