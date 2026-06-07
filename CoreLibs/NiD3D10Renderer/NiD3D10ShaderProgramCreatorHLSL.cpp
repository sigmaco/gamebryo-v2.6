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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10GeometryShader.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10ShaderProgramCreatorHLSL.h"
#include "NiD3D10ShaderProgramFactory.h"
#include "NiD3D10VertexShader.h"

NiD3D10ShaderProgramCreatorHLSL* 
    NiD3D10ShaderProgramCreatorHLSL::ms_pkCreator = NULL;

//---------------------------------------------------------------------------
NiD3D10ShaderProgramCreatorHLSL::NiD3D10ShaderProgramCreatorHLSL()
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgramCreatorHLSL::~NiD3D10ShaderProgramCreatorHLSL()
{
    /* */
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderProgramCreatorHLSL::GetShaderCreationFlags() const
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    return pkRenderer->GetShaderCreationFlags("hlsl");
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramCreatorHLSL::SetShaderCreationFlags(
    unsigned int uiFlags)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->SetShaderCreationFlags("hlsl", uiFlags);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::CompileAndCreateVertexShader(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int uiFlags,
    const char* pcShaderName, NiD3D10VertexShaderPtr& spVertexShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
    {
        pcShaderTarget = NiD3D10Renderer::D3D10GetVertexShaderProfile(
            pkRenderer->GetD3D10Device());
    }

    ID3D10Blob* pkShaderByteCode = NULL;
    ID3D10Blob* pkError = NULL;

    // Update macro definitions table and shader creation flags
    pkDefines = pkRenderer->GetD3D10MacroList("hlsl", pkDefines);
    uiFlags = pkRenderer->GetAllShaderCreationFlags("hlsl", uiFlags);

    // Compile the shader
    HRESULT hr = D3DX10CompileFromMemory(
        (char*)pkShaderCode->GetBufferPointer(), 
        pkShaderCode->GetBufferSize(),
        pcFileName, 
        pkDefines, 
        pkInclude, 
        pcEntryPoint, 
        pcShaderTarget,
        uiFlags,
        0,
        NULL,
        &pkShaderByteCode, 
        &pkError,
        NULL);

    if (FAILED(hr) || pkShaderByteCode == NULL)
    {
        if (pkShaderByteCode)
            pkShaderByteCode->Release();
        if (pkError) 
        {
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, 
                "Failed to compile vertex shader %s in file %s\nError: %s\n",
                pcEntryPoint, pcFileName, 
                (const char*)pkError->GetBufferPointer());
            pkError->Release();
        }
        return false;
    }

    if (pkError)
        pkError->Release();

    bool bResult = CreateVertexShader(pkShaderByteCode, pcFileName, pkDefines,
        pkInclude, pcEntryPoint, pcShaderTarget, uiFlags, pcShaderName, 
        spVertexShader);
    pkShaderByteCode->Release();

    return bResult;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::CreateVertexShader(
    ID3D10Blob* pkShaderByteCode, const char*, 
    const D3D10_SHADER_MACRO*, ID3D10Include*, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int,
    const char*, NiD3D10VertexShaderPtr& spVertexShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
    {
        pcShaderTarget = NiD3D10Renderer::D3D10GetVertexShaderProfile(
            pkRenderer->GetD3D10Device());
    }

    ID3D10VertexShader* pkVertexShader = NULL;
    HRESULT hr = pkRenderer->GetD3D10Device()->CreateVertexShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkVertexShader);
    if (FAILED(hr) || pkVertexShader == NULL)
    {
        if (pkVertexShader)
            pkVertexShader->Release();
        pkShaderByteCode->Release();
        return false;
    }
    spVertexShader = 
        NiNew NiD3D10VertexShader(pkVertexShader, pkShaderByteCode);
    NIASSERT(spVertexShader);
    pkVertexShader->Release();

    // Store shader reflection

    ID3D10ShaderReflection* pkVertexShaderReflection = NULL;
    hr = NiD3D10Renderer::D3D10ReflectShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkVertexShaderReflection);
    if (SUCCEEDED(hr) && pkVertexShaderReflection)
    {
        spVertexShader->SetShaderReflection(pkVertexShaderReflection, true);

        pkVertexShaderReflection->Release();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::CompileAndCreateGeometryShader(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int uiFlags,
    const char* pcShaderName, 
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
    {
        pcShaderTarget = NiD3D10Renderer::D3D10GetGeometryShaderProfile(
            pkRenderer->GetD3D10Device());
    }

    ID3D10Blob* pkShaderByteCode = NULL;
    ID3D10Blob* pkError = NULL;

    // Update macro definitions table and shader creation flags
    pkDefines = pkRenderer->GetD3D10MacroList("hlsl", pkDefines);
    uiFlags = pkRenderer->GetAllShaderCreationFlags("hlsl", uiFlags);

    // Compile the shader
    HRESULT hr = D3DX10CompileFromMemory(
        (char*)pkShaderCode->GetBufferPointer(), 
        pkShaderCode->GetBufferSize(),
        pcFileName, 
        pkDefines, 
        pkInclude, 
        pcEntryPoint, 
        pcShaderTarget,
        uiFlags,
        0,
        NULL,
        &pkShaderByteCode, 
        &pkError,
        NULL);

    if (FAILED(hr) || pkShaderByteCode == NULL)
    {
        if (pkShaderByteCode)
            pkShaderByteCode->Release();
        if (pkError) 
        {
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, 
                "Failed to compile geometry shader %s in file %s\nError: %s\n",
                pcEntryPoint, pcFileName, 
                (const char*)pkError->GetBufferPointer());
            pkError->Release();
        }
        return false;
    }
    if (pkError)
        pkError->Release();

    bool bResult = CreateGeometryShader(pkShaderByteCode, pcFileName, 
        pkDefines, pkInclude, pcEntryPoint, pcShaderTarget, uiFlags, 
        pcShaderName, pkSODeclaration, uiNumEntries, uiOutputStreamStride, 
        spGeometryShader);
    pkShaderByteCode->Release();

    return bResult;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::CreateGeometryShader(
    ID3D10Blob* pkShaderByteCode, const char*, 
    const D3D10_SHADER_MACRO*, ID3D10Include*, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int,
    const char*, 
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    unsigned int uiNumEntries, unsigned int uiOutputStreamStride,
    NiD3D10GeometryShaderPtr& spGeometryShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
    {
        pcShaderTarget = NiD3D10Renderer::D3D10GetGeometryShaderProfile(
            pkRenderer->GetD3D10Device());
    }

    HRESULT hr;
    ID3D10GeometryShader* pkGeometryShader = NULL;
    if (pkSODeclaration && uiNumEntries != 0)
    {
        // D3D10 requests that "if any OutputSlot 
        //  other than 0 is declared, or NumEntries is 0, then 
        //  OutputStreamStride must be 0."
        unsigned int uiTempOutputStreamStride = uiOutputStreamStride;
        if (uiNumEntries == 0)
        {
            uiTempOutputStreamStride = 0;
        }
        else 
        {
            for (unsigned int i = 0; i < uiNumEntries; i++)
            {
                if (pkSODeclaration[i].OutputSlot != 0)
                    uiTempOutputStreamStride = 0;
            }
        }

        hr = pkRenderer->GetD3D10Device()->
            CreateGeometryShaderWithStreamOutput(
            pkShaderByteCode->GetBufferPointer(), 
            pkShaderByteCode->GetBufferSize(), pkSODeclaration,
            uiNumEntries, uiTempOutputStreamStride, &pkGeometryShader);
    }
    else
    {
        hr = pkRenderer->GetD3D10Device()->CreateGeometryShader(
            pkShaderByteCode->GetBufferPointer(), 
            pkShaderByteCode->GetBufferSize(), &pkGeometryShader);

        // Be sure these values are all 0.
        pkSODeclaration = NULL;
        uiNumEntries = 0;
        uiOutputStreamStride = 0;
    }
    if (FAILED(hr) || pkGeometryShader == NULL)
    {
        if (pkGeometryShader)
            pkGeometryShader->Release();
        pkShaderByteCode->Release();
        return false;
    }
    spGeometryShader = 
        NiNew NiD3D10GeometryShader(pkGeometryShader, pkShaderByteCode);
    NIASSERT(spGeometryShader);
    pkGeometryShader->Release();

    // Fill in stream output arrays
    spGeometryShader->SetStreamOutputDeclaration(pkSODeclaration, uiNumEntries,
        uiOutputStreamStride);

    // Store shader reflection

    ID3D10ShaderReflection* pkGeometryShaderReflection = NULL;
    hr = NiD3D10Renderer::D3D10ReflectShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkGeometryShaderReflection);
    if (SUCCEEDED(hr) && pkGeometryShaderReflection)
    {
        spGeometryShader->SetShaderReflection(pkGeometryShaderReflection, 
            true);

        pkGeometryShaderReflection->Release();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::CompileAndCreatePixelShader(
    ID3D10Blob* pkShaderCode, const char* pcFileName, 
    const D3D10_SHADER_MACRO* pkDefines, ID3D10Include* pkInclude, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int uiFlags,
    const char* pcShaderName, NiD3D10PixelShaderPtr& spPixelShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
    {
        pcShaderTarget = NiD3D10Renderer::D3D10GetPixelShaderProfile(
            pkRenderer->GetD3D10Device());
    }

    ID3D10Blob* pkShaderByteCode = NULL;
    ID3D10Blob* pkError = NULL;

    // Update macro definitions table and shader creation flags
    pkDefines = pkRenderer->GetD3D10MacroList("hlsl", pkDefines);
    uiFlags = pkRenderer->GetAllShaderCreationFlags("hlsl", uiFlags);

    // Compile the shader
    HRESULT hr = D3DX10CompileFromMemory(
        (char*)pkShaderCode->GetBufferPointer(), 
        pkShaderCode->GetBufferSize(),
        pcFileName, 
        pkDefines, 
        pkInclude, 
        pcEntryPoint, 
        pcShaderTarget,
        uiFlags,
        0,
        NULL,
        &pkShaderByteCode, 
        &pkError,
        NULL);

    if (FAILED(hr) || pkShaderByteCode == NULL)
    {
        if (pkShaderByteCode)
            pkShaderByteCode->Release();
        if (pkError) 
        {
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, 
                true, 
                "Failed to compile pixel shader %s in file %s\nError: %s\n",
                pcEntryPoint, pcFileName, 
                (const char*)pkError->GetBufferPointer());
            pkError->Release();
        }
        return false;
    }
    if (pkError)
        pkError->Release();

    bool bResult = CreatePixelShader(pkShaderByteCode, pcFileName, pkDefines,
        pkInclude, pcEntryPoint, pcShaderTarget, uiFlags, pcShaderName,
        spPixelShader);

    pkShaderByteCode->Release();
    return bResult;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::CreatePixelShader(
    ID3D10Blob* pkShaderByteCode, const char*, 
    const D3D10_SHADER_MACRO*, ID3D10Include*, 
    const char* pcEntryPoint, const char* pcShaderTarget, unsigned int,
    const char*, NiD3D10PixelShaderPtr& spPixelShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    if (pcEntryPoint == NULL)
        pcEntryPoint = "main";
    if (pcShaderTarget == NULL)
    {
        pcShaderTarget = NiD3D10Renderer::D3D10GetPixelShaderProfile(
            pkRenderer->GetD3D10Device());
    }

    ID3D10PixelShader* pkPixelShader = NULL;
    HRESULT hr = pkRenderer->GetD3D10Device()->CreatePixelShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkPixelShader);
    if (FAILED(hr) || pkPixelShader == NULL)
    {
        if (pkPixelShader)
            pkPixelShader->Release();
        pkShaderByteCode->Release();
        return false;
    }
    spPixelShader = 
        NiNew NiD3D10PixelShader(pkPixelShader, pkShaderByteCode);
    NIASSERT(spPixelShader);
    pkPixelShader->Release();

    // Store shader reflection

    ID3D10ShaderReflection* pkPixelShaderReflection = NULL;
    hr = NiD3D10Renderer::D3D10ReflectShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkPixelShaderReflection);
    if (SUCCEEDED(hr) && pkPixelShaderReflection)
    {
        spPixelShader->SetShaderReflection(pkPixelShaderReflection, true);

        pkPixelShaderReflection->Release();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::RecreateVertexShader(
    NiD3D10VertexShader* pkVertexShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    ID3D10Blob* pkShaderByteCode = pkVertexShader->GetShaderByteCode();

    if (pkShaderByteCode == NULL)
        return false;

    ID3D10VertexShader* pkD3DVertexShader = NULL;
    HRESULT hr = pkRenderer->GetD3D10Device()->CreateVertexShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkD3DVertexShader);
    if (FAILED(hr) || pkD3DVertexShader == NULL)
    {
        if (pkD3DVertexShader)
            pkD3DVertexShader->Release();
        return false;
    }

    pkVertexShader->SetVertexShader(pkD3DVertexShader);

    pkD3DVertexShader->Release();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::RecreateGeometryShader(
    NiD3D10GeometryShader* pkGeometryShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    ID3D10Blob* pkShaderByteCode = pkGeometryShader->GetShaderByteCode();

    if (pkShaderByteCode == NULL)
        return false;

    ID3D10GeometryShader* pkD3DGeometryShader = NULL;
    HRESULT hr = S_OK;
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration = 
        pkGeometryShader->GetStreamOutputDeclaration();
    unsigned int uiNumEntries = pkGeometryShader->GetNumStreamOutputEntries();
    unsigned int uiOutputStreamStride = 
        pkGeometryShader->GetOutputStreamStride();
    if (pkSODeclaration && uiNumEntries != 0)
    {
        NIASSERT(uiOutputStreamStride != 0);
        hr = pkRenderer->GetD3D10Device()->
            CreateGeometryShaderWithStreamOutput(
            pkShaderByteCode->GetBufferPointer(), 
            pkShaderByteCode->GetBufferSize(), pkSODeclaration,
            uiNumEntries, uiOutputStreamStride, &pkD3DGeometryShader);
    }
    else
    {
        hr = pkRenderer->GetD3D10Device()->CreateGeometryShader(
            pkShaderByteCode->GetBufferPointer(), 
            pkShaderByteCode->GetBufferSize(), &pkD3DGeometryShader);
    }
    if (FAILED(hr) || pkD3DGeometryShader == NULL)
    {
        if (pkD3DGeometryShader)
            pkD3DGeometryShader->Release();
        return false;
    }

    pkGeometryShader->SetGeometryShader(pkD3DGeometryShader);

    pkD3DGeometryShader->Release();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderProgramCreatorHLSL::RecreatePixelShader(
    NiD3D10PixelShader* pkPixelShader)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL || pkRenderer->GetD3D10Device() == NULL)
    {
        return false;
    }

    ID3D10Blob* pkShaderByteCode = pkPixelShader->GetShaderByteCode();

    if (pkShaderByteCode == NULL)
        return false;

    ID3D10PixelShader* pkD3DPixelShader = NULL;
    HRESULT hr = pkRenderer->GetD3D10Device()->CreatePixelShader(
        pkShaderByteCode->GetBufferPointer(), 
        pkShaderByteCode->GetBufferSize(), &pkD3DPixelShader);
    if (FAILED(hr) || pkD3DPixelShader == NULL)
    {
        if (pkD3DPixelShader)
            pkD3DPixelShader->Release();
        return false;
    }

    pkPixelShader->SetPixelShader(pkD3DPixelShader);

    pkD3DPixelShader->Release();

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramCreatorHLSL::_SDMInit()
{
    ms_pkCreator = NiNew NiD3D10ShaderProgramCreatorHLSL();

    NiD3D10ShaderProgramFactory::RegisterShaderCreator("hlsl", ms_pkCreator);
    NiD3D10ShaderProgramFactory::RegisterShaderCreator("vsh", ms_pkCreator);
    NiD3D10ShaderProgramFactory::RegisterShaderCreator("gsh", ms_pkCreator);
    NiD3D10ShaderProgramFactory::RegisterShaderCreator("psh", ms_pkCreator);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramCreatorHLSL::_SDMShutdown()
{
    Shutdown();
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgramCreatorHLSL* NiD3D10ShaderProgramCreatorHLSL::GetInstance()
{
    return ms_pkCreator;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgramCreatorHLSL::Shutdown()
{
    if (ms_pkCreator)
        NiD3D10ShaderProgramFactory::UnregisterShaderCreator(ms_pkCreator);
    NiDelete ms_pkCreator;
    ms_pkCreator = NULL;
}
//---------------------------------------------------------------------------
