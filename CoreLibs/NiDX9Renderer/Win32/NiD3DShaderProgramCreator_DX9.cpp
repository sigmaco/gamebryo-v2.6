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
#include "NiD3DRendererPCH.h"

#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderFactory.h"

#include <D3DX9.h>
#if defined(NIDEBUG)
    // In February 2005 SDK update, D3DX9dt.lib no longer exists
    #if D3DX_SDK_VERSION >= 24
        #pragma comment(lib, "D3DX9d.lib")
    #else //#if D3DX_SDK_VERSION >= 24
        #pragma comment(lib, "D3DX9dt.lib")
    #endif //#if D3DX_SDK_VERSION >= 24
#else //#if defined(NIDEBUG)
    #pragma comment(lib, "D3DX9.lib")
#endif //#if defined(NIDEBUG)

//---------------------------------------------------------------------------
NiD3DVertexShaderHandle NiD3DShaderProgramCreator::CreateVertexShader(
    void* pvCode, NiD3DVertexDeclaration&, unsigned int, void*, unsigned int)
{
    NIASSERT(ms_pkD3DDevice);

    NiD3DVertexShaderHandle hShader;

    HRESULT eResult = ms_pkD3DDevice->CreateVertexShader((const DWORD*)pvCode,
        &hShader);
    if (FAILED(eResult))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to create vertex shader\nError 0x%08x\n", eResult);
        return 0;
    }

    return hShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShaderHandle NiD3DShaderProgramCreator::CreatePixelShader(
    void* pvCode)
{
    NIASSERT(ms_pkD3DDevice);

    NiD3DPixelShaderHandle hShader;

    HRESULT eResult = ms_pkD3DDevice->CreatePixelShader((const DWORD*)pvCode, 
        &hShader);
    if (FAILED(eResult))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to create pixel shader\nError 0x%08x\n", eResult);
        return 0;
    }

    return hShader;}
//---------------------------------------------------------------------------
