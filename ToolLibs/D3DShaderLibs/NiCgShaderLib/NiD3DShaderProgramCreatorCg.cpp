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

#include <NiD3DShaderFactory.h>
#include <NiD3DShaderProgramFactory.h>

#include "NiCgShaderContext.h"
#include "NiD3DShaderProgramCreatorCg.h"
#include "NiD3DCgVertexShader.h"
#include "NiD3DCgPixelShader.h"

NiD3DShaderProgramCreatorCg* 
    NiD3DShaderProgramCreatorCg::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorCg::CreateVertexShaderFromFile(
    const char* pcFileName, const char* pcShaderName,
    const char* pcEntryPoint, const char* pcShaderTarget,
    NiD3DVertexDeclaration hDecl, unsigned int uiUsage, bool bRecoverable)
{

#if defined(WIN32)
    cgD3D9SetDevice(ms_pkD3DDevice);
#endif

    NiD3DCgVertexShader* pkVertexShader = 
        NiNew NiD3DCgVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    CGprofile eProfile = GetEnumFromProfileString(pcShaderTarget);
    if (eProfile == CG_PROFILE_UNKNOWN)

#if defined(WIN32)
        eProfile = cgD3D9GetLatestVertexProfile();
#endif

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    unsigned int uiConstantSize = 0;
    void* pvConstantData = NULL;
    CGprogram kProgram = NULL;

    if (!LoadShaderCodeFromFile(pcFileName, pcEntryPoint, eProfile, pvCode, 
        uiCodeSize, pvConstantData, uiConstantSize, kProgram, bRecoverable))
    {
        NiDelete pkVertexShader;
        return 0;
    }

    // Now, create the shader
    NiD3DVertexShaderHandle hShader = CreateVertexShader(pvCode, hDecl, 
        uiUsage, pvConstantData, uiConstantSize);

    if (hShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed CreateVertexShader call on %s\n", pcShaderName);
        NiDelete pkVertexShader;
        return NULL;
    }

    // Populate the NiD3DVertexShader
    pkVertexShader->SetName(pcShaderName);
    pkVertexShader->SetShaderProgramName(pcFileName);
    pkVertexShader->SetCode(uiCodeSize, pvCode);
    pkVertexShader->SetCreator(this);
    pkVertexShader->SetShaderHandle(hShader);
    pkVertexShader->SetUsage(uiUsage);
    pkVertexShader->SetVertexDeclaration(hDecl);
    pkVertexShader->SetEntryPoint(pcEntryPoint);
    pkVertexShader->SetShaderTarget(pcShaderTarget);
    pkVertexShader->SetCgProgram(kProgram);

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorCg::CreateVertexShaderFromBuffer(
    const void* pvBuffer, unsigned int, const char* pcShaderName,
    const char* pcEntryPoint, const char* pcShaderTarget,
    NiD3DVertexDeclaration hDecl, unsigned int uiUsage, bool bRecoverable)
{

#if defined(WIN32)
    cgD3D9SetDevice(ms_pkD3DDevice);
#endif

    NiD3DCgVertexShader* pkVertexShader = 
        NiNew NiD3DCgVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    CGprofile eProfile = GetEnumFromProfileString(pcShaderTarget);
    if (eProfile == CG_PROFILE_UNKNOWN)

#if defined(WIN32)
        eProfile = cgD3D9GetLatestVertexProfile();
#endif

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    unsigned int uiConstantSize = 0;
    void* pvConstantData = NULL;
    CGprogram kProgram = NULL;

    if (!LoadShaderCodeFromBuffer(pvBuffer, pcEntryPoint, eProfile, pvCode, 
        uiCodeSize, pvConstantData, uiConstantSize, kProgram, bRecoverable))
    {
        NiDelete pkVertexShader;
        return 0;
    }

    // Now, create the shader
    NiD3DVertexShaderHandle hShader = CreateVertexShader(pvCode, hDecl, 
        uiUsage, pvConstantData, uiConstantSize);

    if (hShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed CreateVertexShader call on %s\n", pcShaderName);
        NiDelete pkVertexShader;
        return NULL;
    }

    // Populate the NiD3DVertexShader
    pkVertexShader->SetName(pcShaderName);
    pkVertexShader->SetShaderProgramName(NULL);
    pkVertexShader->SetCode(uiCodeSize, pvCode);
    pkVertexShader->SetCreator(this);
    pkVertexShader->SetShaderHandle(hShader);
    pkVertexShader->SetUsage(uiUsage);
    pkVertexShader->SetVertexDeclaration(hDecl);
    pkVertexShader->SetEntryPoint(pcEntryPoint);
    pkVertexShader->SetShaderTarget(pcShaderTarget);
    pkVertexShader->SetCgProgram(kProgram);

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorCg::CreatePixelShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char* pcEntryPoint,
    const char* pcShaderTarget, bool bRecoverable)
{

#if defined(WIN32)
    cgD3D9SetDevice(ms_pkD3DDevice);
#endif

    NiD3DCgPixelShader* pkPixelShader = 
        NiNew NiD3DCgPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    CGprofile eProfile = GetEnumFromProfileString(pcShaderTarget);

#if defined(WIN32)
    if (eProfile == CG_PROFILE_UNKNOWN)
        eProfile = cgD3D9GetLatestPixelProfile();
#endif

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    unsigned int uiConstantSize = 0;
    void* pvConstantData = NULL;
    CGprogram kProgram = NULL;

    if (!LoadShaderCodeFromFile(pcFileName, pcEntryPoint, eProfile, pvCode, 
        uiCodeSize, pvConstantData, uiConstantSize, kProgram, bRecoverable))
    {
        NiDelete pkPixelShader;
        return 0;
    }

    // Now, create the shader
    NiD3DPixelShaderHandle hShader = CreatePixelShader(pvCode);

    if (hShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed CreatePixelShader call on %s\n", pcShaderName);
        NiDelete pkPixelShader;
        return NULL;
    }

    // Populate the NiD3DPixelShader
    pkPixelShader->SetName(pcShaderName);
    pkPixelShader->SetShaderProgramName(pcFileName);
    pkPixelShader->SetCode(uiCodeSize, pvCode);
    pkPixelShader->SetCreator(this);
    pkPixelShader->SetShaderHandle(hShader);
    pkPixelShader->SetEntryPoint(pcEntryPoint);
    pkPixelShader->SetShaderTarget(pcShaderTarget);
    pkPixelShader->SetCgProgram(kProgram);

    return pkPixelShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorCg::CreatePixelShaderFromBuffer(
    const void* pvBuffer, unsigned int, const char* pcShaderName,
    const char* pcEntryPoint, const char* pcShaderTarget, bool bRecoverable)
{

#if defined(WIN32)
    cgD3D9SetDevice(ms_pkD3DDevice);
#endif

    NiD3DCgPixelShader* pkPixelShader = 
        NiNew NiD3DCgPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    CGprofile eProfile = GetEnumFromProfileString(pcShaderTarget);

#if defined(WIN32)
    if (eProfile == CG_PROFILE_UNKNOWN)
        eProfile = cgD3D9GetLatestPixelProfile();
#endif

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    unsigned int uiConstantSize = 0;
    void* pvConstantData = NULL;
    CGprogram kProgram = NULL;

    if (!LoadShaderCodeFromBuffer(pvBuffer, pcEntryPoint, eProfile, pvCode, 
        uiCodeSize, pvConstantData, uiConstantSize, kProgram, bRecoverable))
    {
        NiDelete pkPixelShader;
        return 0;
    }

    // Now, create the shader
    NiD3DPixelShaderHandle hShader = CreatePixelShader(pvCode);

    if (hShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed CreatePixelShader call on %s\n", pcShaderName);
        NiDelete pkPixelShader;
        return NULL;
    }

    // Populate the NiD3DPixelShader
    pkPixelShader->SetName(pcShaderName);
    pkPixelShader->SetShaderProgramName(NULL);
    pkPixelShader->SetCode(uiCodeSize, pvCode);
    pkPixelShader->SetCreator(this);
    pkPixelShader->SetShaderHandle(hShader);
    pkPixelShader->SetEntryPoint(pcEntryPoint);
    pkPixelShader->SetShaderTarget(pcShaderTarget);
    pkPixelShader->SetCgProgram(kProgram);

    return pkPixelShader;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorCg::RecreateVertexShader(
    NiD3DVertexShader* pkVertexShader)
{
    void* pvCode = pkVertexShader->GetCode();

    if (pvCode == NULL)
        return false;

    NiD3DVertexDeclaration hDecl = pkVertexShader->GetVertexDeclaration();
    NiD3DVertexShaderHandle hShader = CreateVertexShader(pvCode, 
        hDecl, pkVertexShader->GetUsage());

    if (hShader == 0)
        return false;
    
    // Set the shader handle
    pkVertexShader->SetShaderHandle(hShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorCg::RecreatePixelShader(
    NiD3DPixelShader* pkPixelShader)
{
    void* pvCode = pkPixelShader->GetCode();

    if (pvCode == NULL)
        return false;

    NiD3DPixelShaderHandle hShader = CreatePixelShader(pvCode);

    if (hShader == 0)
        return false;

    // Set the shader handle
    pkPixelShader->SetShaderHandle(hShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorCg::LoadShaderCodeFromFile(
    const char* pcFileName, const char* pcEntryPoint, 
    CGprofile eProfile, void*& pvCode, unsigned int& uiCodeSize, 
    void*& pvConstantData, unsigned int& uiConstantSize,
    CGprogram& kProgram, bool bRecoverable)
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

    // Compile the shader from the file
    // This code should be parallel between these functions:
    // NiD3DShaderProgramCreatorCg::LoadShaderCodeFromFile
    // NiD3DShaderProgramCreatorCg::LoadShaderCodeFromBuffer
    // NiD3DShaderProgramCreatorCgObj::LoadShaderCodeFromFile
    // NiD3DShaderProgramCreatorCgObj::LoadShaderCodeFromBuffer

    const char** ppcCGCArgs = NULL;

#if defined(WIN32)
    ppcCGCArgs = cgD3D9GetOptimalOptions(eProfile);
    NiD3DRenderer* pkD3DRenderer = NiD3DRenderer::GetRenderer();
    NIASSERT(pkD3DRenderer);
    ppcCGCArgs = pkD3DRenderer->GetCGMacroList("cg", ppcCGCArgs);
#endif  // #if defined(WIN32)

    kProgram = cgCreateProgramFromFile(NiCgShaderContext::GetCgContext(),
        CG_SOURCE, acShaderPath, eProfile, pcEntryPoint, ppcCGCArgs);

    CGerror eError = cgGetError();
    if (kProgram == NULL || eError != CG_NO_ERROR)
    {
        const char* pcErr = NULL;
        if (eError != CG_NO_ERROR)
            pcErr = cgGetErrorString(eError);

        if (pcErr)
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to compile shader %s in file %s\nError: %s\n",
                pcEntryPoint, pcFileName, pcErr);
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to compile shader %s in file %s\nError: "
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
bool NiD3DShaderProgramCreatorCg::LoadShaderCodeFromBuffer(
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

    const char** ppcCGCArgs = NULL;

#if defined(WIN32)
    ppcCGCArgs = cgD3D9GetOptimalOptions(eProfile);
    NiD3DRenderer* pkD3DRenderer = NiD3DRenderer::GetRenderer();
    NIASSERT(pkD3DRenderer);
    ppcCGCArgs = pkD3DRenderer->GetCGMacroList("cg", ppcCGCArgs);
#endif  // #if defined(WIN32)

    kProgram = cgCreateProgram(NiCgShaderContext::GetCgContext(),
        CG_SOURCE, (char*)pvBuffer, eProfile, pcEntryPoint, ppcCGCArgs);

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
CGprofile NiD3DShaderProgramCreatorCg::GetEnumFromProfileString(
    const char* pcShaderTarget)
{
    if (pcShaderTarget == NULL || pcShaderTarget[0] == '\0')
        return CG_PROFILE_UNKNOWN;

    unsigned int uiStrLen = strlen(pcShaderTarget) + 1;
    if (uiStrLen > m_uiShaderTargetTempSize)
    {
        NiFree(m_pcShaderTargetTemp);
        m_pcShaderTargetTemp = NiAlloc(char, uiStrLen);
        m_uiShaderTargetTempSize = uiStrLen;
    }

    for (unsigned int i = 0; i < uiStrLen; i++)
        m_pcShaderTargetTemp[i] = (char)(tolower(pcShaderTarget[i]));

    return cgGetProfile(m_pcShaderTargetTemp);
}
//---------------------------------------------------------------------------
