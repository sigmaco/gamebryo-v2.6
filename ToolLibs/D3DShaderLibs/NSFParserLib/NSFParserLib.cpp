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
#include "NSFParserLibPCH.h"

#include "NSFParserLib.h"
#include "NSFLoader.h"
#include "NSFParserLibLibType.h"

//---------------------------------------------------------------------------
#if defined(_USRDLL)
BOOL WINAPI DllMain(HINSTANCE, ULONG fdwReason, LPVOID)
{
    NiOutputDebugString("NSFParserLib> DLLMain CALL - ");

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        {
            //  Initialize anything needed here
            //  If failed, return FALSE
            NiOutputDebugString("PROCESS ATTACH!\n");
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
NSFPARSERLIB_ENTRY unsigned int RunShaderParser(const char* pszDirectory,
    bool bRecurseSubFolders)
{
    // Create a loader...
    NSFLoader* pkLoader = NSFLoader::Create();
    if (!pkLoader)
        return 0;

    unsigned int uiCount = pkLoader->ParseAllNSFFiles(pszDirectory, 
        bRecurseSubFolders);

    NSFLoader::Destroy();

    return uiCount;
}
//---------------------------------------------------------------------------
NSFPARSERLIB_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
//---------------------------------------------------------------------------
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
#if defined(_LIB) || defined (_PS3)
//---------------------------------------------------------------------------
unsigned int NSFParserLib_RunShaderParser(const char* pszDirectory, 
    bool bRecurseSubFolders)
{
    // Create a loader...
    NSFLoader* pkLoader = NSFLoader::Create();
    if (!pkLoader)
        return 0;

    unsigned int uiCount = pkLoader->ParseAllNSFFiles(pszDirectory, 
        bRecurseSubFolders);

    NSFLoader::Destroy();

    return uiCount;
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
