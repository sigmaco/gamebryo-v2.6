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

#include <NiCgShaderContext.h>
#include <NiD3DShaderFactory.h>
#include <NiD3DShaderProgramFactory.h>

#include "NiD3DShaderProgramCreatorCgObj.h"

NiD3DShaderProgramCreatorCgObj* 
    NiD3DShaderProgramCreatorCgObj::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorCgObj::LoadShaderCodeFromFile(
    const char* pcFileName, const char* pcEntryPoint, 
    CGprofile eProfile, void*& pvCode, unsigned int& uiCodeSize, 
    void*& pvConstantData, unsigned int& uiConstantSize, CGprogram& kProgram, 
    bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;
    pvConstantData = NULL;
    uiConstantSize = 0;

    // Verify the name
    if (pcFileName == NULL || pcFileName[0] == '\0')
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
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

    kProgram = cgCreateProgramFromFile(NiCgShaderContext::GetCgContext(), 
        CG_OBJECT, acShaderPath, eProfile, pcEntryPoint, 
#if defined(WIN32)
        cgD3D9GetOptimalOptions(eProfile)
#endif  // #if defined(WIN32)
        );

    CGerror eError = cgGetError();
    if (kProgram == NULL || eError != CG_NO_ERROR)
    {
        const char* pcErr = NULL;
        if (eError != CG_NO_ERROR)
            pcErr = cgGetErrorString(eError);

        if (pcErr)
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to generate shader %s in file %s\nError: %s\n",
                pcEntryPoint, pcFileName, pcErr);
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to generate shader %s in file %s\nError: "
                "NONE REPORTED\n", pcEntryPoint, pcFileName);
        }

        return false;
    }

    const char* pcProgram = cgGetProgramString(kProgram, CG_COMPILED_PROGRAM);
    NIASSERT(pcProgram && pcProgram[0] != '\0');

    LPD3DXBUFFER pkCode;
    LPD3DXBUFFER pkErrors;

    // Assemble the shader from the file
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    unsigned int uiFlags = 0;
    if (pkRenderer)
    {
        uiFlags = pkRenderer->GetAllShaderCreationFlags("cg");
    }

#if defined(WIN32)
    // D3DXAssembleShader fails if any flags but these are set
    uiFlags &= (D3DXSHADER_DEBUG | D3DXSHADER_SKIPVALIDATION);

    HRESULT eResult = D3DXAssembleShader(pcProgram, strlen(pcProgram), NULL, 
        NULL, uiFlags, &pkCode, &pkErrors);
#endif

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
                    "Failed to assemble shader %s in file %s\nError: %s\n",
                    pcEntryPoint, pcFileName, pcErr);
            }
            pkErrors->Release();
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to assemble shader %s in file %s\nError: "
                "NONE REPORTED\n", pcEntryPoint, pcFileName);
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
bool NiD3DShaderProgramCreatorCgObj::LoadShaderCodeFromBuffer(
    const void* pvBuffer, const char* pcEntryPoint, CGprofile eProfile, 
    void*& pvCode, unsigned int& uiCodeSize, void*& pvConstantData, 
    unsigned int& uiConstantSize, CGprogram& kProgram, bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;
    pvConstantData = NULL;
    uiConstantSize = 0;

    // Verify the buffer
    if (pvBuffer == NULL || ((char*)pvBuffer)[0] == '\0')
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Invalid shader buffer\n");
        return false;
    }

    kProgram = cgCreateProgram(NiCgShaderContext::GetCgContext(), 
        CG_OBJECT, (char*)pvBuffer, eProfile, pcEntryPoint, 
#if defined(WIN32)
        cgD3D9GetOptimalOptions(eProfile)
#endif  // #if defined(WIN32);
    );

    CGerror eError = cgGetError();
    if (kProgram == NULL || eError != CG_NO_ERROR)
    {
        const char* pcErr = NULL;
        if (eError != CG_NO_ERROR)
            pcErr = cgGetErrorString(eError);

        if (pcErr)
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to compile shader %s from memory\nError: %s\n",
                pcEntryPoint, pcErr);
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to compile shader %s from memory\nError: "
                "NONE REPORTED\n", pcEntryPoint);
        }

        return false;
    }

    const char* pcProgram = cgGetProgramString(kProgram, CG_COMPILED_PROGRAM);
    NIASSERT(pcProgram && pcProgram[0] != '\0');
 
    LPD3DXBUFFER pkCode;
    LPD3DXBUFFER pkErrors;

    // Assemble the shader from the file
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    unsigned int uiFlags = 0;
    if (pkRenderer)
    {
        uiFlags = pkRenderer->GetAllShaderCreationFlags("cg");
    }

#if defined(WIN32)
    // D3DXAssembleShader fails if any flags but these are set
    uiFlags &= (D3DXSHADER_DEBUG | D3DXSHADER_SKIPVALIDATION);

    HRESULT eResult = D3DXAssembleShader(pcProgram, strlen(pcProgram), NULL, 
        NULL, uiFlags, &pkCode, &pkErrors);
#endif

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
                    "Failed to assemble shader %s from memory\nError: %s\n",
                    pcEntryPoint, pcErr);
            }
            pkErrors->Release();
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to assemble shader %s from memory\nError: "
                "NONE REPORTED\n", pcEntryPoint);
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
