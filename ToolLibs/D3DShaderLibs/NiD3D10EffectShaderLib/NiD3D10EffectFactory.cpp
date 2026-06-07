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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectFactory.h"
#include "NiD3D10EffectFile.h"
#include "NiD3D10EffectLoader.h"

#include <NiD3D10Renderer.h>
#include <NiD3D10ShaderProgramCreator.h>
#include <NiShaderFactory.h>

NiD3D10EffectFactory* NiD3D10EffectFactory::ms_pkFactory = 0;
char* NiD3D10EffectFactory::ms_pcD3D10EffectDirectory = 0;

NiUInt32 NiD3D10EffectFactory::ms_uiFXFlags = 0;

// Enable FXL for all platforms that don't support it.
bool NiD3D10EffectFactory::ms_bFXLSupport = false;

//---------------------------------------------------------------------------
NiD3D10EffectFactory::NiD3D10EffectFactory() :
    m_uiLostCounter(UINT_MAX),
    m_uiResetCounter(UINT_MAX)
{
    // Maps for D3D10Effect names to shaders.
    m_pkD3D10EffectMap = NiNew NiTStringPointerMap<NiD3D10EffectFile*>(59,
        false);
    NIASSERT(m_pkD3D10EffectMap);

    if (NiRenderer::GetRenderer())
    {
        // Update the macros.
        UpdateMacros();
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectFactory::~NiD3D10EffectFactory()
{
    DestroyRendererData();

    if (m_pkD3D10EffectMap)
        m_pkD3D10EffectMap->RemoveAll();
    NiDelete m_pkD3D10EffectMap;
}
//---------------------------------------------------------------------------
NiD3D10EffectFactory* NiD3D10EffectFactory::GetInstance(bool bCreateNew)
{
    if (bCreateNew && ms_pkFactory == NULL)
        ms_pkFactory = NiNew NiD3D10EffectFactory;
    return ms_pkFactory;
}
//---------------------------------------------------------------------------
NiD3D10EffectFile* NiD3D10EffectFactory::LoadD3D10EffectFromMemory(
    ID3D10Device* pkDevice,
    const char* pcEffectName, 
    NiUInt32 uiCodeSize, 
    void* pvCode,
    bool bCompiled,
    const char* pcPathToFile)
{
    if (!ms_pkFactory)
        GetInstance();

    if (!pcEffectName || pcEffectName[0] == '\0')
        return 0;

    // See if it exists already, and return it
    NiD3D10EffectFile* pkD3D10EffectFile = 
        ms_pkFactory->GetD3D10EffectFile(pcEffectName);

    if (pkD3D10EffectFile == NULL)
    {
        // Compile effect
        pkD3D10EffectFile = ms_pkFactory->CompileD3D10Effect(
            pcEffectName, 
            uiCodeSize, 
            pvCode, 
            bCompiled,
            pcPathToFile);

        if (pkD3D10EffectFile == NULL)
            return NULL;

        // Insert it in the list
        ms_pkFactory->InsertD3D10EffectFileIntoMap(pkD3D10EffectFile);
    }

    // Attempt to create an ID3D10Effect
    ms_pkFactory->CreateD3D10Effect(pkDevice, pkD3D10EffectFile);

    return pkD3D10EffectFile;
}
//---------------------------------------------------------------------------
const char* NiD3D10EffectFactory::GetD3D10EffectDirectory()
{
    return ms_pcD3D10EffectDirectory;
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::SetD3D10EffectDirectory(const char* pcDirectory)
{
    NiDelete[] ms_pcD3D10EffectDirectory;
    ms_pcD3D10EffectDirectory = 0;

    if (pcDirectory && pcDirectory[0] != '\0')
    {
        size_t stLen = strlen(pcDirectory) + 1;
        ms_pcD3D10EffectDirectory = NiAlloc(char, stLen);
        NIASSERT(ms_pcD3D10EffectDirectory);
        NiStrcpy(ms_pcD3D10EffectDirectory, stLen, pcDirectory);
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectFile* NiD3D10EffectFactory::CompileD3D10Effect(
    const char* pcEffectName, 
    NiUInt32 uiCodeSize, 
    void* pvCode,
    bool bCompiled,
    const char* pcPathToFile)
{
    // Be sure D3D10 library is loaded
    NiD3D10Renderer::EnsureD3D10Loaded();

    ID3D10Blob* pkCompiledEffect = NULL;
    if (bCompiled)
    {
#ifdef NIDEBUG
        HRESULT hr = 
#endif
            NiD3D10Renderer::D3D10CreateBlob(uiCodeSize, &pkCompiledEffect);
        NIASSERT(SUCCEEDED(hr));
        memcpy(pkCompiledEffect->GetBufferPointer(), pvCode, uiCodeSize);
    }
    else
    {
        m_kCriticalSection.Lock();
        m_kD3D10Include.SetBasePath(pcPathToFile);

        NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
        const D3D10_SHADER_MACRO* pkMacros = NULL;
        NiUInt32 uiFlags = 0;
        if (pkRenderer)
        {
            pkMacros = pkRenderer->GetD3D10MacroList("fx");
            uiFlags = pkRenderer->GetAllShaderCreationFlags("fx");
        }

        const char* pcProfile = "fx_4_0";

        ID3D10Blob* pkErrors = NULL;
        HRESULT hr = D3DX10CompileFromMemory(
            (const char*)pvCode,
            uiCodeSize,
            pcEffectName,
            pkMacros,
            &m_kD3D10Include,
            NULL,
            pcProfile,
            uiFlags,
            ms_uiFXFlags,
            NULL,
            &pkCompiledEffect,
            &pkErrors,
            NULL);

        m_kCriticalSection.Unlock();

        if (FAILED(hr) || pkCompiledEffect == NULL)
        {
            NIASSERT(pkErrors && FAILED(hr));

            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to compile D3D10 Effect %s with HRESULT = "
                "0x%08X:\n %s\n",
                pcEffectName, 
                (NiUInt32)hr, 
                (const char*)pkErrors->GetBufferPointer());

            if (pkCompiledEffect)
            {
                pkCompiledEffect->Release();
                pkCompiledEffect = NULL;
            }

            pkErrors->Release();
            pkErrors = NULL;
        }
        else if (pkErrors != NULL)
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, true, 
                "Error message compiling D3D10 Effect %s:\n %s\n",
                pcEffectName,
                (const char*)pkErrors->GetBufferPointer());

            pkErrors->Release();
            pkErrors = NULL;
        }
    }

    NiD3D10EffectFile* pkD3D10EffectFile = NULL;
    if (pkCompiledEffect)
    {
        pkD3D10EffectFile = NiNew NiD3D10EffectFile(pkCompiledEffect);
        pkD3D10EffectFile->SetName(pcEffectName);

        pkCompiledEffect->Release();
    }

    return pkD3D10EffectFile;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectFactory::CreateD3D10Effect(
    ID3D10Device* pkDevice,
    NiD3D10EffectFile* pkD3D10EffectFile,
    bool bParse)
{
    NIASSERT(pkDevice);
    NIASSERT(pkD3D10EffectFile->GetEffect() == NULL);

    ID3D10Effect* pkEffect = NULL;
    HRESULT hr = NiD3D10Renderer::D3D10CreateEffectFromMemory(
        pkD3D10EffectFile->GetCompiledEffectData(),
        pkD3D10EffectFile->GetCompiledEffectDataSize(),
        ms_uiFXFlags,
        pkDevice,
        NULL,
        &pkEffect);

    if (FAILED(hr) || pkEffect == NULL)
    {
        if (FAILED(hr))
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to create D3D10 Effect %s\n"
                "Error HRESULT = 0x%08X.\n",
                pkD3D10EffectFile->GetName(), 
                (NiUInt32)hr);
        }
        else
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to create D3D10 Effect %s\n"
                "No error message from D3D10, but effect is NULL.\n",
                pkD3D10EffectFile->GetName());
        }

        if (pkEffect)
        {
            pkEffect->Release();
            pkEffect = NULL;
        }
    }

    if (bParse)
        pkD3D10EffectFile->ParseEffect(pkEffect);

    pkD3D10EffectFile->SetD3D10Effect(pkEffect);


    if (pkEffect != NULL)
    {
        // SetEffect will have added a ref to the effect
        pkEffect->Release();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiD3D10EffectFile* NiD3D10EffectFactory::GetD3D10EffectFile(
    const char* pcShaderName) const
{
    NiD3D10EffectFile* pkD3D10EffectFile = NULL;
    if (m_pkD3D10EffectMap->GetAt(pcShaderName, pkD3D10EffectFile))
        return pkD3D10EffectFile;

    return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::InsertD3D10EffectFileIntoMap(
    NiD3D10EffectFile* pkD3D10EffectFile)
{
    if (pkD3D10EffectFile == NULL)
        return;

    const char* pcName = pkD3D10EffectFile->GetName();

    if (!pcName || pcName[0] == '\0')
        return;

    if (!m_pkD3D10EffectMap)
    {
        NIASSERT(!"InsertD3D10EffectFileIntoMap> Invalid Map!");
        return;
    }

    m_pkD3D10EffectMap->SetAt(pcName, pkD3D10EffectFile);
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::RemoveD3D10EffectFileFromMap(const char* pcName)
{
    if (!pcName)
        return;

    if (!m_pkD3D10EffectMap)
    {
        NIASSERT(!"RemoveD3D10EffectFileFromMap> Invalid Map!");
        return;
    }

    m_pkD3D10EffectMap->RemoveAt(pcName);
}

//---------------------------------------------------------------------------
bool NiD3D10EffectFactory::ResolveD3D10EffectFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiBufferSize)
{
    // See if the file exists as-is
    if (NiFile::Access(pcOriginalName, NiFile::READ_ONLY))
    {
        NiStrcpy(pcTrueName, uiBufferSize, pcOriginalName);
    }
    else
    {
        if (!ms_pkFactory)
        {
            // We don't have a valid factory, so we can't grab the directory
            // the app set to check for the file.
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "No valid D3D10 Effect factory\n");
            return false;
        }

        NiFilename kFilename(pcOriginalName);

        // Try the current shader directory
        const char* pcProgDir = GetD3D10EffectDirectory();
        if (!pcProgDir || pcProgDir[0] == '\0')
        {
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Invalid or no D3D10 Effect directory\n");
            return false;
        }

        if ((pcProgDir[strlen(pcProgDir) - 1] != '/') &&
            (pcProgDir[strlen(pcProgDir) - 1] != '\\'))
        {
            NiSprintf(pcTrueName, uiBufferSize, "%s\\%s%s", pcProgDir, 
                kFilename.GetFilename(), kFilename.GetExt());
        }
        else
        {
            NiSprintf(pcTrueName, uiBufferSize, "%s%s%s", pcProgDir, 
                kFilename.GetFilename(), kFilename.GetExt());
        }

        if (!NiFile::Access(pcTrueName, NiFile::READ_ONLY))
        {
            // Not found!
            NiShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "D3D10 Effect file not found %s\n", pcOriginalName);
            return false;
        }
    }

    // It can be assumed the file was found at this point!
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::DestroyRendererData()
{
    NiTMapIterator kIter = m_pkD3D10EffectMap->GetFirstPos();
    while (kIter)
    {
        const char* pcName = NULL;
        NiD3D10EffectFile* pkFile;
        m_pkD3D10EffectMap->GetNext(kIter, pcName, pkFile);
        pkFile->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::_SDMInit()
{
    // Nothing to do in the init at the moment
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::_SDMShutdown()
{
    // Free the D3D10Effect directory
    NiDelete ms_pkFactory;
    ms_pkFactory = 0;

    SetD3D10EffectDirectory(0);
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::SetFXLSupport(bool bEnable)
{
    ms_bFXLSupport = bEnable;
    NiD3D10EffectLoader* pkLoader = NiD3D10EffectLoader::GetInstance(false);
    if (pkLoader)
    {
        if (ms_bFXLSupport)
            pkLoader->AddFileExtension(".FXL", false);
        else
            pkLoader->RemoveFileExtension("FXL");
    }

    if (GetInstance())
        GetInstance()->UpdateMacros();
}
//---------------------------------------------------------------------------
void NiD3D10EffectFactory::UpdateMacros()
{
    // Allocate the macros and put the platform in place.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->AddMacro("fx", "_D3D10", "1");

    // If support is enabled, then we need to make sure the macros are set
    // for D3D10 fallbacks.
    if (ms_bFXLSupport)
        pkRenderer->AddMacro("fx", "GAMEBRYO_USE_D3D10_FALLBACKS", "1");
}
//---------------------------------------------------------------------------
