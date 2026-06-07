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

#include "NiD3DShaderProgramCreatorAsm.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderProgramFactory.h"

//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorAsm::LoadShaderCodeFromFile(
    const char* pcFileName, void*& pvCode, unsigned int& uiCodeSize, 
    bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;

    // Verify the name
    if (pcFileName == NULL || pcFileName[0] == '\0')
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Invalid shader file name\n");
        return false;
    }

    // Resolve shader program file
    char acShaderPath[_MAX_PATH];
    if (!NiD3DShaderProgramFactory::ResolveShaderFileName(pcFileName, 
        acShaderPath, _MAX_PATH))
    {
        // Can't resolve the shader!
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed to find shader program file %s\n", pcFileName);
        return false;
    }

    LPD3DXBUFFER pkCode;
    LPD3DXBUFFER pkErrors;

    // Get macro definitions table and shader creation flags
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    const D3DXMACRO* pkDefines = NULL;
    NiUInt32 uiFlags = 0;
    if (pkRenderer)
    {
        pkDefines = pkRenderer->GetD3DXMacroList("asm");
        uiFlags = pkRenderer->GetAllShaderCreationFlags("asm");
    }

    // D3DXAssembleShader fails if any flags but these are set
    uiFlags &= (D3DXSHADER_DEBUG | D3DXSHADER_SKIPVALIDATION);

    // Assemble the shader from the file
    HRESULT eResult = D3DXAssembleShaderFromFile(acShaderPath, pkDefines,
        NULL, uiFlags, &pkCode, &pkErrors);

    if (FAILED(eResult))
    {
        char* pcErr = NULL;
        if (pkErrors)
        {
            LPVOID pvBuff = pkErrors->GetBufferPointer();
            if (pvBuff)
            {
                unsigned int uiLen = pkErrors->GetBufferSize();
                pcErr = NiAlloc(char, uiLen);
                NIASSERT(pcErr);
                NiStrcpy(pcErr, uiLen, (const char*)pvBuff);

                NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    bRecoverable, 
                    "Failed to assemble shader %s\nError: %s\n",
                    pcFileName, pcErr);
            }
            pkErrors->Release();
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to assemble shader %s\nError: NONE REPORTED\n",
                pcFileName);
        }
        NiFree(pcErr);

        if (pkCode)
            pkCode->Release();
        return false;
    }

    NIASSERT(pkCode);
    uiCodeSize = pkCode->GetBufferSize();
    pvCode = NiAlloc(BYTE, uiCodeSize);
    NIASSERT(pvCode);
    NiMemcpy(pvCode, pkCode->GetBufferPointer(), uiCodeSize);
    pkCode->Release();

    if (pkErrors)
        pkErrors->Release();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorAsm::LoadShaderCodeFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, void*& pvCode, 
    unsigned int& uiCodeSize, bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;

    // Verify the buffer
    if (pvBuffer == NULL || uiBufferSize == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Invalid shader buffer\n");
        return false;
    }

    LPD3DXBUFFER pkCode;
    LPD3DXBUFFER pkErrors;

    // Get macro definitions table and shader creation flags
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    const D3DXMACRO* pkDefines = NULL;
    NiUInt32 uiFlags = 0;
    if (pkRenderer)
    {
        pkDefines = pkRenderer->GetD3DXMacroList("asm");
        uiFlags = pkRenderer->GetAllShaderCreationFlags("asm");
    }

    // D3DXAssembleShader fails if any flags but these are set
    uiFlags &= (D3DXSHADER_DEBUG | D3DXSHADER_SKIPVALIDATION);

    // Assemble the shader from the buffer
    HRESULT eResult = D3DXAssembleShader((char*)pvBuffer, uiBufferSize,
        pkDefines, NULL, uiFlags, &pkCode, &pkErrors);

    if (FAILED(eResult))
    {
        char* pcErr = NULL;
        if (pkErrors)
        {
            LPVOID pvBuff = pkErrors->GetBufferPointer();
            if (pvBuff)
            {
                unsigned int uiLen = pkErrors->GetBufferSize();
                pcErr = NiAlloc(char, uiLen);
                NIASSERT(pcErr);
                NiStrcpy(pcErr, uiLen, (const char*)pvBuff);

                NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                    bRecoverable, 
                    "Failed to assemble shader from memory\nError: %s\n",
                    pcErr);
            }
            pkErrors->Release();
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to assemble shader from memory\n"
                "Error: NONE REPORTED\n");
        }
        NiFree(pcErr);

        if (pkCode)
            pkCode->Release();
        return false;
    }

    NIASSERT(pkCode);
    uiCodeSize = pkCode->GetBufferSize();
    pvCode = NiAlloc(BYTE, uiCodeSize);
    NIASSERT(pvCode);
    NiMemcpy(pvCode, pkCode->GetBufferPointer(), uiCodeSize);
    pkCode->Release();

    if (pkErrors)
        pkErrors->Release();

    return true;
}
//---------------------------------------------------------------------------
