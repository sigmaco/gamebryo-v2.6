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
#include "NiBinaryShaderLibPCH.h"

#include "NiBinaryShaderLib.h"
#include "NiBinaryShaderLibLibType.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
#if defined(_USRDLL)
BOOL WINAPI DllMain(HINSTANCE, ULONG fdwReason, LPVOID)
{
    NiOutputDebugString("NiBinaryShaderLib> DLLMain CALL - ");

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
#else
// Define a dummy function to prevent linker warnings rather than specifying 
// that the linker ignore empty assemblies via a command line option.
NIBINARYSHADERLIB_ENTRY void NiBinaryShaderLib_Dummy()
{
    return;
}
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
