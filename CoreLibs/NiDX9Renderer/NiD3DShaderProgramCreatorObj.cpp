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

#include "NiD3DShaderProgramCreatorObj.h"
#include "NiD3DShaderFactory.h"

NiD3DShaderProgramCreatorObj* 
    NiD3DShaderProgramCreatorObj::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorObj::CreateVertexShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char*, 
    const char*, NiD3DVertexDeclaration hDecl, unsigned int uiUsage,
    bool bRecoverable)
{
    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;

    if (!LoadShaderCodeFromFile(pcFileName, pvCode, uiCodeSize, 
        bRecoverable))
    {
        return 0;
    }

    // It's possible that there's a shader constant table if this is a 
    // precompiled HLSL shader. If so, we'll attempt to get it and set it on
    // the vertex shader.
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;
    D3DXGetShaderConstantTable((const DWORD*)pvCode, &pkConstantTable);

    NiD3DVertexShader* pkVertexShader = NULL;

    // If we have a constant table, we need
    // an NiD3DHLSLVertexShader so we can set the constant table on it.
    if (pkConstantTable)
    {
        NiD3DHLSLVertexShader* pkHLSL = 
            NiNew NiD3DHLSLVertexShader(ms_pkD3DRenderer);
        pkHLSL->SetConstantTable(pkConstantTable);
        pkVertexShader = pkHLSL;
    }
    else
    {
        pkVertexShader = NiNew NiD3DVertexShader(ms_pkD3DRenderer);
    }

    if (!pkVertexShader)
        return NULL;

    // Now, create the shader
    NiD3DVertexShaderHandle hShader = CreateVertexShader(pvCode, hDecl, 
        uiUsage);

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

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorObj::CreateVertexShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcShaderName, 
    const char*, const char*, NiD3DVertexDeclaration hDecl, 
    unsigned int uiUsage, bool bRecoverable)
{
    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;

    if (!LoadShaderCodeFromBuffer(pvBuffer, uiBufferSize, pvCode, uiCodeSize, 
        bRecoverable))
    {
        return 0;
    }

    // It's possible that there's a shader constant table if this is a 
    // precompiled HLSL shader. If so, we'll attempt to get it and set it on
    // the vertex shader.
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;
    D3DXGetShaderConstantTable((const DWORD*)pvCode, &pkConstantTable);

    NiD3DVertexShader* pkVertexShader = NULL;

    // If we have a constant table, we need
    // an NiD3DHLSLVertexShader so we can set the constant table on it.
    if (pkConstantTable)
    {
        NiD3DHLSLVertexShader* pkHLSL = 
            NiNew NiD3DHLSLVertexShader(ms_pkD3DRenderer);
        pkHLSL->SetConstantTable(pkConstantTable);
        pkVertexShader = pkHLSL;
    }
    else
    {
        pkVertexShader = NiNew NiD3DVertexShader(ms_pkD3DRenderer);
    }

    if (!pkVertexShader)
        return NULL;

    // Now, create the shader
    NiD3DVertexShaderHandle hShader = CreateVertexShader(pvCode, hDecl, 
        uiUsage);

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

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorObj::CreatePixelShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char*, 
    const char*, bool bRecoverable)
{
    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = 0;

    if (!LoadShaderCodeFromFile(pcFileName, pvCode, uiCodeSize, 
        bRecoverable))
    {
        return 0;
    }

    // It's possible that there's a shader constant table if this is a 
    // precompiled HLSL shader. If so, we'll attempt to get it and set it on
    // the pixel shader.
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;
    D3DXGetShaderConstantTable((const DWORD*)pvCode, &pkConstantTable);

    NiD3DPixelShader* pkPixelShader = NULL;

    // If we have a constant table, we need
    // an NiD3DHLSLPixelShader so we can set the constant table on it.
    if (pkConstantTable)
    {
        NiD3DHLSLPixelShader* pkHLSL = 
            NiNew NiD3DHLSLPixelShader(ms_pkD3DRenderer);
        pkHLSL->SetConstantTable(pkConstantTable);
        pkPixelShader = pkHLSL;
    }
    else
    {
        pkPixelShader = NiNew NiD3DPixelShader(ms_pkD3DRenderer);
    }

    if (!pkPixelShader)
        return NULL;

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

    return pkPixelShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorObj::CreatePixelShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcShaderName, 
    const char*, const char*, bool bRecoverable)
{
    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = 0;

    if (!LoadShaderCodeFromBuffer(pvBuffer, uiBufferSize, pvCode, uiCodeSize, 
        bRecoverable))
    {
        return 0;
    }

    // It's possible that there's a shader constant table if this is a 
    // precompiled HLSL shader. If so, we'll attempt to get it and set it on
    // the pixel shader.
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;
    D3DXGetShaderConstantTable((const DWORD*)pvCode, &pkConstantTable);

    NiD3DPixelShader* pkPixelShader = NULL;

    // If we have a constant table, we need
    // an NiD3DHLSLPixelShader so we can set the constant table on it.
    if (pkConstantTable)
    {
        NiD3DHLSLPixelShader* pkHLSL = 
            NiNew NiD3DHLSLPixelShader(ms_pkD3DRenderer);
        pkHLSL->SetConstantTable(pkConstantTable);
        pkPixelShader = pkHLSL;
    }
    else
    {
        pkPixelShader = NiNew NiD3DPixelShader(ms_pkD3DRenderer);
    }

    if (!pkPixelShader)
        return NULL;

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

    return pkPixelShader;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorObj::LoadShaderCodeFromFile(
    const char* pcFileName, void*& pvCode, unsigned int& uiCodeSize, 
    bool bRecoverable)
{
    pvCode = NULL;
    uiCodeSize = 0;

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

    //  Open the file
    HANDLE hFile = CreateFile(acShaderPath, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Invalid shader file %s\n", acShaderPath);
        return false;
    }

    //  Get the file size and allocate memory to hold the shader file
    DWORD dwSize = GetFileSize(hFile, NULL);
    BYTE* pbyData = NiAlloc(BYTE, dwSize + 4);
    if (NULL == pbyData)
        return false;

    memset(pbyData, 0, dwSize + 4);

    //  Read the vertex shader
    DWORD dwBytesRead;

    ReadFile(hFile, pbyData, dwSize, &dwBytesRead, NULL);
    CloseHandle(hFile);
    if (dwBytesRead != dwSize)
    {
        NiFree(pbyData);
        return false;
    }

    uiCodeSize = dwSize;
    pvCode = (void*)pbyData;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorObj::LoadShaderCodeFromBuffer(
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

    pvCode = NiAlloc(BYTE, uiBufferSize);
    NiMemcpy(pvCode, pvBuffer, uiBufferSize);
    uiCodeSize = uiBufferSize;

    return true;
}
//---------------------------------------------------------------------------
