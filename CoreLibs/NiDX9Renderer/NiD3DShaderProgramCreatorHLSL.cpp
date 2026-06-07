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

#include "NiD3DShaderProgramCreatorHLSL.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DHLSLVertexShader.h"
#include "NiD3DHLSLPixelShader.h"

NiD3DShaderProgramCreatorHLSL* 
    NiD3DShaderProgramCreatorHLSL::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorHLSL::CreateVertexShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char* pcEntryPoint,
    const char* pcShaderTarget, NiD3DVertexDeclaration hDecl, 
    unsigned int uiUsage, bool bRecoverable)
{
    NiD3DHLSLVertexShader* pkVertexShader = 
        NiNew NiD3DHLSLVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
        pcShaderTarget = D3DXGetVertexShaderProfile(ms_pkD3DDevice);

    if (!LoadShaderCodeFromFile(pcFileName, pcEntryPoint, pcShaderTarget, 
        pvCode, uiCodeSize, pkConstantTable, bRecoverable))
    {
        NiDelete pkVertexShader;
        return 0;
    }

    // Unused in DX9
    void* pvConstantData = NULL;
    unsigned int uiConstantSize = 0;

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
    pkVertexShader->SetEntryPoint(pcEntryPoint);
    pkVertexShader->SetShaderTarget(pcShaderTarget);
    pkVertexShader->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorHLSL::CreateVertexShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcShaderName, 
    const char* pcEntryPoint, const char* pcShaderTarget, 
    NiD3DVertexDeclaration hDecl, unsigned int uiUsage, bool bRecoverable)
{
    NiD3DHLSLVertexShader* pkVertexShader = 
        NiNew NiD3DHLSLVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
        pcShaderTarget = D3DXGetVertexShaderProfile(ms_pkD3DDevice);

    if (!LoadShaderCodeFromBuffer(pvBuffer, uiBufferSize, pcEntryPoint, 
        pcShaderTarget, pvCode, uiCodeSize, pkConstantTable, bRecoverable))
    {
        NiDelete pkVertexShader;
        return 0;
    }

    // Unused in DX9
    void* pvConstantData = NULL;
    unsigned int uiConstantSize = 0;

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
    pkVertexShader->SetEntryPoint(pcEntryPoint);
    pkVertexShader->SetShaderTarget(pcShaderTarget);
    pkVertexShader->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DVertexShader* 
    NiD3DShaderProgramCreatorHLSL::CreateVertexShaderFromD3DXBuffer(
    LPD3DXBUFFER pkBuffer, unsigned int uiBufferSize, const char* pcShaderName,
    const char* pcEntryPoint, const char* pcShaderTarget, 
    NiD3DVertexDeclaration hDecl, unsigned int uiUsage, bool bRecoverable)
{
    NiD3DHLSLVertexShader* pkVertexShader = 
        NiNew NiD3DHLSLVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    // Load the code from the file
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;

    if (FAILED(D3DXGetShaderConstantTable(
        (const DWORD*)(pkBuffer->GetBufferPointer()), &pkConstantTable)))
    {
        return NULL;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
        pcShaderTarget = D3DXGetVertexShaderProfile(ms_pkD3DDevice);

    // Unused in DX9
    void* pvConstantData = NULL;
    unsigned int uiConstantSize = 0;

    // Now, create the shader
    NiD3DVertexShaderHandle hShader = CreateVertexShader(
        pkBuffer->GetBufferPointer(), hDecl, 
        uiUsage, pvConstantData, uiConstantSize);

    if (hShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed CreateVertexShader call on %s\n", pcShaderName);
        NiDelete pkVertexShader;
        return NULL;
    }

    BYTE* pbBuffer = NiAlloc(BYTE, uiBufferSize);
    NiMemcpy(pbBuffer, pkBuffer->GetBufferPointer(), 
        pkBuffer->GetBufferSize());


    // Populate the NiD3DVertexShader
    pkVertexShader->SetName(pcShaderName);
    pkVertexShader->SetShaderProgramName(NULL);
    pkVertexShader->SetCode(uiBufferSize, pbBuffer);
    pkVertexShader->SetCreator(this);
    pkVertexShader->SetShaderHandle(hShader);
    pkVertexShader->SetUsage(uiUsage);
    pkVertexShader->SetEntryPoint(pcEntryPoint);
    pkVertexShader->SetShaderTarget(pcShaderTarget);
    pkVertexShader->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return pkVertexShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorHLSL::CreatePixelShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char* pcEntryPoint,
    const char* pcShaderTarget, bool bRecoverable)
{
    NiD3DHLSLPixelShader* pkPixelShader = 
        NiNew NiD3DHLSLPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
        pcShaderTarget = D3DXGetPixelShaderProfile(ms_pkD3DDevice);

    if (!LoadShaderCodeFromFile(pcFileName, pcEntryPoint, pcShaderTarget, 
        pvCode, uiCodeSize, pkConstantTable, bRecoverable))
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
    pkPixelShader->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return pkPixelShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorHLSL::CreatePixelShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcShaderName, 
    const char* pcEntryPoint, const char* pcShaderTarget, bool bRecoverable)
{
    NiD3DHLSLPixelShader* pkPixelShader = 
        NiNew NiD3DHLSLPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
        pcShaderTarget = D3DXGetPixelShaderProfile(ms_pkD3DDevice);

    if (!LoadShaderCodeFromBuffer(pvBuffer, uiBufferSize, pcEntryPoint, 
        pcShaderTarget, pvCode, uiCodeSize, pkConstantTable, bRecoverable))
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
    pkPixelShader->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return pkPixelShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* 
    NiD3DShaderProgramCreatorHLSL::CreatePixelShaderFromD3DXBuffer(
    LPD3DXBUFFER pkBuffer, unsigned int uiBufferSize, const char* pcShaderName,
    const char* pcEntryPoint, const char* pcShaderTarget, bool bRecoverable)
{
    NiD3DHLSLPixelShader* pkPixelShader = 
        NiNew NiD3DHLSLPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    // Load the code from the file
    LPD3DXCONSTANTTABLE pkConstantTable = NULL;
    
    if (FAILED(D3DXGetShaderConstantTable(
        (const DWORD*)(pkBuffer->GetBufferPointer()), &pkConstantTable)))
    {
        return NULL;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
        pcShaderTarget = D3DXGetPixelShaderProfile(ms_pkD3DDevice);

   
    // Now, create the shader
    NiD3DPixelShaderHandle hShader = CreatePixelShader(
        pkBuffer->GetBufferPointer());

    if (hShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
            "Failed CreatePixelShader call on %s\n", pcShaderName);
        NiDelete pkPixelShader;
        return NULL;
    }

    BYTE* pbBuffer = NiAlloc(BYTE, uiBufferSize);
    NiMemcpy(pbBuffer, pkBuffer->GetBufferPointer(), 
        pkBuffer->GetBufferSize());

    // Populate the NiD3DPixelShader
    pkPixelShader->SetName(pcShaderName);
    pkPixelShader->SetShaderProgramName(NULL);
    pkPixelShader->SetCode(uiBufferSize, pbBuffer);
    pkPixelShader->SetCreator(this);
    pkPixelShader->SetShaderHandle(hShader);
    pkPixelShader->SetEntryPoint(pcEntryPoint);
    pkPixelShader->SetShaderTarget(pcShaderTarget);
    pkPixelShader->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return pkPixelShader;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorHLSL::RecreateVertexShader(
    NiD3DVertexShader* pkVertexShader)
{
    void* pvCode = pkVertexShader->GetCode();

    if (pvCode == NULL)
        return false;

    NiD3DVertexDeclaration hDecl = pkVertexShader->GetVertexDeclaration();
    NiD3DVertexShaderHandle hShader = CreateVertexShader(pvCode, hDecl, 
        pkVertexShader->GetUsage());

    if (hShader == 0)
        return false;
    
    // Set the shader handle
    pkVertexShader->SetShaderHandle(hShader);

    LPD3DXCONSTANTTABLE pkConstantTable;
    NiD3DHLSLVertexShader* pkHLSLVS = (NiD3DHLSLVertexShader*)pkVertexShader;
    HRESULT hr = D3DXGetShaderConstantTable((const DWORD*)pvCode,
        &pkConstantTable);
    if (SUCCEEDED(hr))
        pkHLSLVS->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorHLSL::RecreatePixelShader(
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

    LPD3DXCONSTANTTABLE pkConstantTable;
    NiD3DHLSLPixelShader* pkHLSLPS = (NiD3DHLSLPixelShader*)pkPixelShader;
    HRESULT hr = D3DXGetShaderConstantTable((const DWORD*)pvCode,
        &pkConstantTable);
    if (SUCCEEDED(hr))
        pkHLSLPS->SetConstantTable(pkConstantTable);

    if (pkConstantTable)
        pkConstantTable->Release();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorHLSL::LoadShaderCodeFromFile(
    const char* pcFileName, const char* pcEntryPoint, 
    const char* pcShaderTarget, void*& pvCode, unsigned int& uiCodeSize,
    LPD3DXCONSTANTTABLE& pkConstantTable, bool bRecoverable)
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

    LPD3DXBUFFER pkCode;
    LPD3DXBUFFER pkErrors;

    // Get macro definitions table and shader creation flags
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    const D3DXMACRO* pkDefines = NULL;
    NiUInt32 uiFlags = 0;
    if (pkRenderer)
    {
        pkDefines = pkRenderer->GetD3DXMacroList("hlsl");
        uiFlags = pkRenderer->GetAllShaderCreationFlags("hlsl");
    }

    // Compile the shader from the file
    HRESULT eResult = D3DXCompileShaderFromFile(acShaderPath, pkDefines,
        NULL, pcEntryPoint, pcShaderTarget, uiFlags, &pkCode, &pkErrors,
        &pkConstantTable);

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
                    "Failed to compile shader %s in file %s\nError: %s\n",
                    pcEntryPoint, pcFileName, pcErr);
            }
            pkErrors->Release();
        }
        else
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, bRecoverable, 
                "Failed to compile shader %s in file %s\nError: "
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
    unsigned int uiDestSize = uiCodeSize * sizeof(BYTE);
    NiMemcpy(pvCode, uiDestSize, pkCode->GetBufferPointer(), uiCodeSize);
    pkCode->Release();

    if (pkErrors)
        pkErrors->Release();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorHLSL::LoadShaderCodeFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcEntryPoint, 
    const char* pcShaderTarget, void*& pvCode, unsigned int& uiCodeSize,
    LPD3DXCONSTANTTABLE& pkConstantTable, bool bRecoverable)
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
        pkDefines = pkRenderer->GetD3DXMacroList("hlsl");
        uiFlags = pkRenderer->GetAllShaderCreationFlags("hlsl");
    }

    // Compile the shader from the buffer
    HRESULT eResult = D3DXCompileShader((char*)pvBuffer, uiBufferSize,
        pkDefines, NULL, pcEntryPoint, pcShaderTarget, uiFlags, &pkCode,
        &pkErrors, &pkConstantTable);

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
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                bRecoverable, 
                "Failed to assemble shader %s from memory\n"
                "Error: NONE REPORTED\n", pcEntryPoint);
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
