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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectFile.h"
#include "NiD3DXEffectLoader.h"

#include <NiD3DShaderFactory.h>
#include <NiD3DShaderProgramCreator.h>
#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
NiD3DXEffectFactory* NiD3DXEffectFactory::ms_pkFactory = 0;
char* NiD3DXEffectFactory::ms_pcD3DXEffectDirectory = 0;

NiD3DRenderer* NiD3DXEffectFactory::ms_pkD3DRenderer = 0;
D3DDevicePtr NiD3DXEffectFactory::ms_pkD3DDevice = 0;

// Enable FXL for all platforms that don't support it.
bool NiD3DXEffectFactory::ms_bFXLSupport =
#if !defined(_XENON)
    true;
#else
    false;
#endif

#if defined(WIN32)
HINSTANCE NiD3DXEffectFactory::ms_hAppInstance = NULL;
#endif //#if defined(WIN32)

//---------------------------------------------------------------------------
// Device and renderer access functions
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    if (ms_pkD3DRenderer != pkD3DRenderer)
    {
        if (pkD3DRenderer)
        {
            SetDevice(pkD3DRenderer->GetD3DDevice());
            if (GetInstance())
                GetInstance()->UpdateMacros();
        }
        else
        {
            SetDevice(0);
        }
        ms_pkD3DRenderer = pkD3DRenderer;
    }
}

//---------------------------------------------------------------------------
void NiD3DXEffectFactory::SetDevice(D3DDevicePtr pkDevice)
{
    if (pkDevice != ms_pkD3DDevice)
    {
        if (ms_pkD3DDevice)
        {
            ms_pkD3DDevice->Release();
            ms_pkD3DDevice = NULL;
        }
        ms_pkD3DDevice = pkDevice;
        if (ms_pkD3DDevice)
        {
            ms_pkD3DDevice->AddRef();
        }

        // Invalidate all existing NiD3DXEffect objects
        if (ms_pkFactory)
            ms_pkFactory->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
NiD3DXEffectFactory::NiD3DXEffectFactory() :
    m_uiLostCounter(UINT_MAX),
    m_uiResetCounter(UINT_MAX)
{
    // Maps for D3DXEffect names to shaders.
    m_pkD3DXEffectMap = NiNew NiTStringPointerMap<NiD3DXEffectFile*>(59,
        false);
    NIASSERT(m_pkD3DXEffectMap);

    if (NiRenderer::GetRenderer())
    {
        // Update the macros.
        UpdateMacros();
    }
}
//---------------------------------------------------------------------------
NiD3DXEffectFactory::~NiD3DXEffectFactory()
{
    DestroyRendererData();

    if (m_pkD3DXEffectMap)
        m_pkD3DXEffectMap->RemoveAll();
    NiDelete m_pkD3DXEffectMap;
}
//---------------------------------------------------------------------------
NiD3DXEffectFactory* NiD3DXEffectFactory::GetInstance(bool bCreateNew)
{
    if (bCreateNew && ms_pkFactory == NULL)
        ms_pkFactory = NiNew NiD3DXEffectFactory;
    return ms_pkFactory;
}
//---------------------------------------------------------------------------
NiD3DXEffectFile* NiD3DXEffectFactory::CreateD3DXEffectFromFile(
    const char* pcFileName)
{
    if (!ms_pkFactory)
        GetInstance();

    if (!pcFileName || (strcmp(pcFileName, "") == 0))
        return 0;

    // See if it exists already, and return it
    NiD3DXEffectFile* pkD3DXEffectFile = 
        ms_pkFactory->GetD3DXEffectFile(pcFileName);
    if (pkD3DXEffectFile)
    {
        LPD3DXEFFECT pkEffect = pkD3DXEffectFile->GetRenderableEffect();
        if (pkEffect == NULL && ms_pkD3DRenderer)
            ms_pkFactory->LoadD3DXEffectFromFile(pcFileName, pkD3DXEffectFile);

        return pkD3DXEffectFile;
    }

    if (!ms_pkFactory->LoadD3DXEffectFromFile(pcFileName, pkD3DXEffectFile))
    {
        return 0;
    }

    NIASSERT(pkD3DXEffectFile);

#if defined(WIN32)
    pkD3DXEffectFile->SetDXVersion(NiShader::NISHADER_DX9);
#elif defined(_XENON)
    pkD3DXEffectFile->SetDXVersion(NiShader::NISHADER_XENON);
#endif

    if (pkD3DXEffectFile->GetRenderableEffect() == NULL)
        pkD3DXEffectFile->DestroyRendererData();

    // Load the code from the file

    // Insert it in the list
    ms_pkFactory->InsertD3DXEffectFileIntoMap(pkD3DXEffectFile);

    return pkD3DXEffectFile;
}
//---------------------------------------------------------------------------
NiD3DXEffectFile* NiD3DXEffectFactory::CreateD3DXEffectFromCode(
    const char* pcEffectName, unsigned int uiCodeSize, void* pvCode)
{
    if (!ms_pkFactory)
        return 0;

    if (!pcEffectName || (strcmp(pcEffectName, "") == 0))
        return 0;

    // See if it exists already, and return it
    NiD3DXEffectFile* pkD3DXEffectFile = 
        ms_pkFactory->GetD3DXEffectFile(pcEffectName);
    if (pkD3DXEffectFile)
        return pkD3DXEffectFile;

    if (ms_pkD3DDevice == NULL)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "No D3D device exists for creating D3D Effects\n");
        return 0;
    }

    // Load the code from the binary
    if (!ms_pkFactory->LoadD3DXEffectFromCode(pcEffectName, uiCodeSize, 
        pvCode, pkD3DXEffectFile))
    {
        return 0;
    }

    // Insert it in the list
    ms_pkFactory->InsertD3DXEffectFileIntoMap(pkD3DXEffectFile);

    return pkD3DXEffectFile;
}
//---------------------------------------------------------------------------
const char* NiD3DXEffectFactory::GetD3DXEffectDirectory()
{
    return ms_pcD3DXEffectDirectory;
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::SetD3DXEffectDirectory(const char* pcDirectory)
{
    NiDelete[] ms_pcD3DXEffectDirectory;
    ms_pcD3DXEffectDirectory = 0;

    if (pcDirectory && pcDirectory[0] != '\0')
    {
        size_t stLen = strlen(pcDirectory) + 1;
        ms_pcD3DXEffectDirectory = NiAlloc(char, stLen);
        NIASSERT(ms_pcD3DXEffectDirectory);
        NiStrcpy(ms_pcD3DXEffectDirectory, stLen, pcDirectory);
    }
}
//---------------------------------------------------------------------------
bool NiD3DXEffectFactory::LoadD3DXEffectFromFile(const char* pcFileName, 
    NiD3DXEffectFile*& pkD3DXEffectFile)
{
    // Verify the name
    if (!pcFileName || (strcmp(pcFileName, "") == 0))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Missing D3D Effect file name\n");
        return false;
    }

    // 'Parse' off the extension
    char acExt[_MAX_EXT];

    NiD3DUtility::GetFileExtension(pcFileName, acExt, _MAX_EXT);
    if (NiStricmp(acExt, ".fx") == 0 ||
        NiStricmp(acExt, ".fxo") == 0 ||
        (ms_bFXLSupport && NiStricmp(acExt, ".fxl") == 0))
    {
        char acShaderPath[_MAX_PATH];

        if (!ResolveD3DXEffectFileName(pcFileName, acShaderPath, _MAX_PATH))
        {
            // Can't resolve the effect!
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to find D3D Effect file %s\n", pcFileName);
            return false;
        }

        char* pcError = NULL;
        NiRenderer* pkRenderer = NiRenderer::GetRenderer();
        NiUInt32 uiFlags;
        if (pkRenderer)
            uiFlags = pkRenderer->GetAllShaderCreationFlags("fx");
        else
            uiFlags = 0;

        LPD3DXBASEEFFECT pkEffect = (LPD3DXBASEEFFECT)
            NiD3DXEffectLoader::CreateEffectFromFile(
            ms_pkD3DDevice, acShaderPath, uiFlags,
            NULL, pcError);

        if (pkEffect == NULL)
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to compile D3D Effect using Temp Device:\n %s\n",
                pcError);
            NiFree(pcError);
            pkD3DXEffectFile = NULL;
            return false;
        }
        NiFree(pcError);

        if (pkD3DXEffectFile == NULL)
        {
            pkD3DXEffectFile = NiNew NiD3DXEffectFile();
            NIASSERT(pkD3DXEffectFile);
            pkD3DXEffectFile->SetName(pcFileName);
        }

        NIVERIFY(pkD3DXEffectFile->Initialize(pkEffect, ms_pkD3DDevice));

        return true;
    }

    pkD3DXEffectFile = NULL;

    NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
        "Invalid D3D Effect file %s\n", pcFileName);

    return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectFactory::LoadD3DXEffectFromCode(const char* pcEffectName, 
    unsigned int uiCodeSize, void* pvCode, NiD3DXEffectFile*& pkD3DXEffectFile)
{
    char* pcError = NULL;

    NiUInt32 uiFlags = 0;
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer)
        uiFlags = pkRenderer->GetAllShaderCreationFlags("fx");

    LPD3DXBASEEFFECT pkEffect = (LPD3DXBASEEFFECT)
        NiD3DXEffectLoader::CreateEffectFromCode(ms_pkD3DDevice,
        pvCode, uiCodeSize, uiFlags, NULL, pcError);

    if (pkEffect == NULL)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to compile D3D Effect using Temp Device:\n %s\n",
            pcError);
        NiFree(pcError);
        pkD3DXEffectFile = NULL;
        return false;
    }
    NiFree(pcError);

    if (pkD3DXEffectFile == NULL)
    {
        pkD3DXEffectFile = NiNew NiD3DXEffectFile();
    }
    NIASSERT(pkD3DXEffectFile);

    pkD3DXEffectFile->SetName(pcEffectName);
    NIVERIFY(pkD3DXEffectFile->Initialize(pkEffect, ms_pkD3DDevice));

    return true;
}
//---------------------------------------------------------------------------
NiD3DXEffectFile* NiD3DXEffectFactory::GetD3DXEffectFile(
    const char* pcShaderName) const
{
    NiD3DXEffectFile* spD3DXEffectFile = NULL;
    if (m_pkD3DXEffectMap->GetAt(pcShaderName, spD3DXEffectFile))
        return spD3DXEffectFile;

    return NULL;
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::InsertD3DXEffectFileIntoMap(
    NiD3DXEffectFile* pkD3DXEffectFile)
{
    if (pkD3DXEffectFile == NULL)
        return;

    const char* pcName = pkD3DXEffectFile->GetName();

    if (!pcName || strcmp(pcName, "") == 0)
        return;

    if (!m_pkD3DXEffectMap)
    {
        NIASSERT(!"InsertD3DXEffectFileIntoMap> Invalid Map!");
        return;
    }

    m_pkD3DXEffectMap->SetAt(pcName, pkD3DXEffectFile);
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::RemoveD3DXEffectFileFromMap(const char* pcName)
{
    if (!pcName)
        return;

    if (!m_pkD3DXEffectMap)
    {
        NIASSERT(!"RemoveD3DXEffectFromMap> Invalid Map!");
        return;
    }

    m_pkD3DXEffectMap->RemoveAt(pcName);
}

//---------------------------------------------------------------------------
bool NiD3DXEffectFactory::ResolveD3DXEffectFileName(
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
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "No valid D3D Effect factory\n");
            return false;
        }

        // Path was not valid. 
        char acDrive[_MAX_DRIVE];
        char acDir[_MAX_PATH];
        char acFileName[_MAX_PATH];
        char acFileExt[_MAX_EXT];

        NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, 
            acFileName, acFileExt);

        // Try the current shader directory
        const char* pcProgDir = GetD3DXEffectDirectory();
        if (!pcProgDir || (strcmp(pcProgDir, "") == 0))
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "Invalid or no D3D Effect directory\n");
            return false;
        }

        if ((pcProgDir[strlen(pcProgDir) - 1] != '/') &&
            (pcProgDir[strlen(pcProgDir) - 1] != '\\'))
        {
            NiSprintf(pcTrueName, uiBufferSize, "%s\\%s%s", pcProgDir, 
                acFileName, acFileExt);
        }
        else
        {
            NiSprintf(pcTrueName, uiBufferSize, "%s%s%s", pcProgDir, 
                acFileName, acFileExt);
        }

        if (!NiFile::Access(pcTrueName, NiFile::READ_ONLY))
        {
            // Not found!
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                "D3D Effect file not found %s\n", pcOriginalName);
            return false;
        }
    }

    // It can be assumed the file was found at this point!
    return true;
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::HandleLostDevice(unsigned int uiCounter)
{
    if (m_uiLostCounter != uiCounter)
    {
        NiTMapIterator kIter = m_pkD3DXEffectMap->GetFirstPos();
        while (kIter)
        {
            const char* pcName = NULL;
            NiD3DXEffectFile* pkFile;
            m_pkD3DXEffectMap->GetNext(kIter, pcName, pkFile);
            pkFile->HandleLostDevice();
        }

        m_uiLostCounter = uiCounter;
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::HandleResetDevice(unsigned int uiCounter)
{
    if (m_uiResetCounter != uiCounter)
    {
        NiTMapIterator kIter = m_pkD3DXEffectMap->GetFirstPos();
        while (kIter)
        {
            const char* pcName = NULL;
            NiD3DXEffectFile* pkFile;
            m_pkD3DXEffectMap->GetNext(kIter, pcName, pkFile);
            pkFile->HandleResetDevice();
        }

        m_uiResetCounter = uiCounter;
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::DestroyRendererData()
{
    NiTMapIterator kIter = m_pkD3DXEffectMap->GetFirstPos();
    while (kIter)
    {
        const char* pcName = NULL;
        NiD3DXEffectFile* pkFile;
        m_pkD3DXEffectMap->GetNext(kIter, pcName, pkFile);
        pkFile->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::_SDMInit()
{
    // Nothing to do in the init at the moment
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::_SDMShutdown()
{
    // Free the D3DXEffect directory
    NiDelete ms_pkFactory;
    ms_pkFactory = 0;
    SetD3DRenderer(NULL);
    SetD3DXEffectDirectory(0);
}
//---------------------------------------------------------------------------
HINSTANCE NiD3DXEffectFactory::GetApplicationInstance()
{
#if defined(WIN32)
    return ms_hAppInstance;
#elif defined(_XENON)
    return 0;
#endif  //#if defined(WIN32)
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::SetApplicationInstance(HINSTANCE hInstance)
{
#if defined(WIN32)
    ms_hAppInstance = hInstance;
#endif  //#if defined(WIN32)
}
//---------------------------------------------------------------------------
void NiD3DXEffectFactory::UpdateMacros()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    // Allocate the macros and put the platform in place.
#if defined(WIN32)
    pkRenderer->AddMacro("fx", "_DX9", "1");
#elif defined(_XENON)
    pkRenderer->AddMacro("fx", "_XENON", "1");
#endif

    // If support is enabled, then we need to make sure the macros are set
    // for D3DX fallbacks.
    if (ms_bFXLSupport)
        pkRenderer->AddMacro("fx", "GAMEBRYO_USE_D3DX_FALLBACKS", "1");
}
//---------------------------------------------------------------------------
