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
#include "NiCgShaderLibPCH.h"
#include "NiCgShaderLibSDM.h"

#include "NiCgShaderLib.h"
#include "NiCgShaderLibType.h"

#include "NiD3DCgPixelShader.h"
#include "NiD3DCgVertexShader.h"
#include "NiD3DShaderProgramCreatorCg.h"

#if defined (WIN32)
    #if defined(NIDEBUG)
        // In February 2005 SDK update, D3DX9dt.lib no longer exists
        #if D3DX_SDK_VERSION >= 24
            #pragma comment(lib, "D3DX9d.lib")
        #else   // #if D3DX_SDK_VERSION >= 24
            #pragma comment(lib, "D3DX9dt.lib")
        #endif  // #if D3DX_SDK_VERSION >= 24
    #else   // #if defined(NIDEBUG)
        #pragma comment(lib, "D3DX9.lib")
    #endif  // #if defined(NIDEBUG)
#endif  // #if defined (WIN32)

static NiCgShaderLibSDM NiCgShaderLibSDMObject;

//---------------------------------------------------------------------------
#if defined(_USRDLL)
NICGSHADERLIB_ENTRY unsigned int GetCompilerVersion(void)
{
    return (_MSC_VER);
}
#endif  //#if defined(_USRDLL)
//---------------------------------------------------------------------------
#if defined(_LIB)
void NiCgShaderLib_RegisterShaderCreator()
{
    // Just make sure library gets linked and initialized OK
    NiD3DShaderProgramCreatorCg* pkCreator = 
        NiD3DShaderProgramCreatorCg::GetInstance();
    if (pkCreator == NULL)
        NiOutputDebugString("Error - NiCgShaderLib not linked\n");
}
#endif  //#if defined(_LIB)
//---------------------------------------------------------------------------
