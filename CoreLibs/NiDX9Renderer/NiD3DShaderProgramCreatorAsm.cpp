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

NiD3DShaderProgramCreatorAsm* 
    NiD3DShaderProgramCreatorAsm::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
NiD3DVertexShader* NiD3DShaderProgramCreatorAsm::CreateVertexShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char*, 
    const char*, NiD3DVertexDeclaration hDecl, unsigned int uiUsage,
    bool bRecoverable)
{
    NiD3DVertexShader* pkVertexShader = 
        NiNew NiD3DVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;

    if (!LoadShaderCodeFromFile(pcFileName, pvCode, uiCodeSize, 
        bRecoverable))
    {
        NiDelete pkVertexShader;
        return 0;
    }

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
NiD3DVertexShader* NiD3DShaderProgramCreatorAsm::CreateVertexShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcShaderName, 
    const char*, const char*, NiD3DVertexDeclaration hDecl, 
    unsigned int uiUsage, bool bRecoverable)
{
    NiD3DVertexShader* pkVertexShader = 
        NiNew NiD3DVertexShader(ms_pkD3DRenderer);

    if (!pkVertexShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = NULL;

    if (!LoadShaderCodeFromBuffer(pvBuffer, uiBufferSize, pvCode, uiCodeSize, 
        bRecoverable))
    {
        NiDelete pkVertexShader;
        return 0;
    }

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
NiD3DPixelShader* NiD3DShaderProgramCreatorAsm::CreatePixelShaderFromFile(
    const char* pcFileName, const char* pcShaderName, const char*, 
    const char*, bool bRecoverable)
{
    NiD3DPixelShader* pkPixelShader = 
        NiNew NiD3DPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = 0;

    if (!LoadShaderCodeFromFile(pcFileName, pvCode, uiCodeSize, 
        bRecoverable))
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

    return pkPixelShader;
}
//---------------------------------------------------------------------------
NiD3DPixelShader* NiD3DShaderProgramCreatorAsm::CreatePixelShaderFromBuffer(
    const void* pvBuffer, unsigned int uiBufferSize, const char* pcShaderName, 
    const char*, const char*, bool bRecoverable)
{
    NiD3DPixelShader* pkPixelShader = 
        NiNew NiD3DPixelShader(ms_pkD3DRenderer);

    if (!pkPixelShader)
        return NULL;

    // Load the code from the file
    unsigned int uiCodeSize = 0;
    void* pvCode = 0;

    if (!LoadShaderCodeFromBuffer(pvBuffer, uiBufferSize, pvCode, uiCodeSize, 
        bRecoverable))
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

    return pkPixelShader;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorAsm::RecreateVertexShader(
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
    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgramCreatorAsm::RecreatePixelShader(
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
