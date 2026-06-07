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

#include "NiD3DShaderFactory.h"
#include "NiD3DShaderLibrary.h"
#include "NiD3DShaderLibraryInterface.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DShader.h"

unsigned int NiD3DShaderFactory::ms_uiNewShaderValue = 0;
NiD3DShaderFactory* NiD3DShaderFactory::ms_pkD3DShaderFactory = NULL;

//---------------------------------------------------------------------------
void NiD3DShaderFactory::_SDMInit()
{
    NiD3DShaderFactory* pkFactory = 
        (NiD3DShaderFactory*)NiD3DShaderFactory::Create();
    if (!pkFactory)
    {
        NiD3DRenderer::Error("Failed to create shader factory!");
    }

#if defined(_USRDLL)
    pkFactory->m_pfnClassCreate = pkFactory->GetDefaultClassCreateCallback();
    pkFactory->m_pfnRunParser = pkFactory->GetDefaultRunParserCallback();
    pkFactory->m_pfnErrorCallback = pkFactory->GetDefaultErrorCallback();
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::_SDMShutdown()
{
    NiD3DShaderFactory::Destroy();
}
//---------------------------------------------------------------------------
NiShaderFactory* NiD3DShaderFactory::Create()
{
    if (ms_pkD3DShaderFactory == 0)
    {
        NiD3DShaderFactory* pkFactory = NiNew NiD3DShaderFactory();
        NIASSERT(pkFactory);
        ms_pkD3DShaderFactory = pkFactory;
    }

    // Set the ms_pkShaderFactory variable only if it is not currently set.
    if (ms_pkShaderFactory == NULL)
        ms_pkShaderFactory = ms_pkD3DShaderFactory;

    return ms_pkD3DShaderFactory;
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::Destroy()
{
    if (ms_pkD3DShaderFactory)
    {
        NiDelete ms_pkD3DShaderFactory;
        ms_pkD3DShaderFactory = NULL;
    }
}
//---------------------------------------------------------------------------
NiD3DShaderFactory::NiD3DShaderFactory() :
    NiShaderFactory(), 
#if defined(_USRDLL)
    m_kLibraryDLLIter(NULL),
#endif //#if defined(_USRDLL)
    m_pkRenderer(NULL),
    m_kLibraryIter(NULL)
{
    m_pkShaderMap = NiNew NiTStringPointerMap<NiShader*>(59, true);
    NIASSERT(m_pkShaderMap);
    m_pkLibraryMap = 
        NiNew NiTStringPointerMap<NiShaderLibraryPtr>(37, false);
    NIASSERT(m_pkLibraryMap);
}
//---------------------------------------------------------------------------
NiD3DShaderFactory::~NiD3DShaderFactory()
{
    // We could call the PurgeAllShaderRendererData() function here to 
    // guarantee that all renderer data is released. We will assume that if 
    // the renderer has been deleted this will have already occured, and if
    // it hasn't, the renderer will take care of it.

    RemoveAllShaders();
    RemoveAllLibraries();
    
    NiDelete m_pkShaderMap;
    NiDelete m_pkLibraryMap;
    m_pkShaderMap = NULL;
    m_pkLibraryMap = NULL;

#if defined(_USRDLL)
    FreeLibraryDLLs();
#endif //#if defined(_USRDLL)

    m_pkRenderer = 0;
}
//---------------------------------------------------------------------------
#if defined(_USRDLL)
void NiD3DShaderFactory::FreeLibraryDLLs()
{
    RemoveAllShaders();
    RemoveAllLibraries();
    
    NiDelete m_pkShaderMap;
    NiDelete m_pkLibraryMap;
    m_pkShaderMap = NULL;
    m_pkLibraryMap = NULL;

    // Release libraries
    NiTMapIterator kIter = m_kLoadedShaderLibDLLs.GetFirstPos();
    while (kIter)
    {
        const char* pcName;
        HMODULE hModule;
        m_kLoadedShaderLibDLLs.GetNext(kIter, pcName, hModule);
        if (hModule)
            FreeLibrary(hModule);
    }
    m_kLoadedShaderLibDLLs.RemoveAll();

    m_kLibraryDLLIter = NULL;
}
//---------------------------------------------------------------------------
void* NiD3DShaderFactory::GetFirstLibraryDLL(const char*& pcName)
{
    m_kLibraryDLLIter = m_kLoadedShaderLibDLLs.GetFirstPos();
    if (m_kLibraryDLLIter)
    {
        HMODULE hModule;
        m_kLoadedShaderLibDLLs.GetNext(m_kLibraryDLLIter, pcName, hModule);
        if (hModule)
            return hModule;
    }
    pcName = NULL;
    return NULL;
}
//---------------------------------------------------------------------------
void* NiD3DShaderFactory::GetNextLibraryDLL(const char*& pcName)
{
    if (m_kLibraryDLLIter)
    {
        HMODULE hModule;
        m_kLoadedShaderLibDLLs.GetNext(m_kLibraryDLLIter, pcName, hModule);
        if (hModule)
            return hModule;
    }
    pcName = NULL;
    return NULL;
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::ClearLibraryDLLs()
{
    m_kLoadedShaderLibDLLs.RemoveAll();

    m_kLibraryDLLIter = NULL;
}
//---------------------------------------------------------------------------
bool NiD3DShaderFactory::DefaultDLLCreateClass(const char* pcLibName, 
    NiRenderer* pkRenderer, int iDirectoryCount, const char* apcDirectories[],
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

    if (!ms_pkD3DShaderFactory)
        return false;

    NID3DSLI_LOADLIBRARY pfnLoad = NULL;
    HMODULE hLibrary = GetModuleHandle(pcLibName);
    if (hLibrary == NULL)
    {
        // Load the interface library
        hLibrary = LoadLibrary(pcLibName);
        if (hLibrary == NULL)
        {
#ifdef NIDEBUG
            ReportError(NISHADERERR_UNKNOWN, false, 
                "LoadLibrary failed on %s\n", pcLibName);
            DWORD dwError = GetLastError();
            ReportError(NISHADERERR_UNKNOWN, false, 
                "Get last error returned 0x%08x\n", dwError);
#endif
            return false;
        }

        // Ensure library was built with the same compiler
        NIGETCOMPILERVERSIONFUNCTION pfnGetCompilerVersionFunc =
            (NIGETCOMPILERVERSIONFUNCTION)GetProcAddress(hLibrary, 
            "GetCompilerVersion");
        if (pfnGetCompilerVersionFunc)
        {
            unsigned int uiShaderVersion = pfnGetCompilerVersionFunc();
            if (uiShaderVersion != (_MSC_VER))
            {
                ReportError(NISHADERERR_UNKNOWN, false, 
                    "Shader Library %s\n"
                    "was built with different version of Visual Studio.\n\n"
                    "Library compiled with %s.\n"
                    "This application compiled with %s.", pcLibName,
                    GetCompilerName(uiShaderVersion), 
                    GetCompilerName(_MSC_VER));
                FreeLibrary(hLibrary);
                return false;
            }
        }
        else
        {
            // Older version - run it anyway
        }

        // Fetch the library creation function
        pfnLoad = (NID3DSLI_LOADLIBRARY)GetProcAddress(hLibrary, 
            "LoadShaderLibrary");
        if (pfnLoad == 0)
        {
            ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to retrieve 'LoadShaderLibrary' function from %s\n", 
                pcLibName);
            FreeLibrary(hLibrary);
            return false;
        }

        ms_pkD3DShaderFactory->m_kLoadedShaderLibDLLs.SetAt(
            pcLibName, hLibrary);
    }
    else
    {
        // It was found once; it damn well better be found again!
        pfnLoad = (NID3DSLI_LOADLIBRARY)GetProcAddress(hLibrary, 
            "LoadShaderLibrary");
        NIASSERT(pfnLoad);
    }

    NiShaderLibrary* pkLibrary = NULL;
    bool bSuccess = pfnLoad(pkRenderer, iDirectoryCount, 
        apcDirectories, bRecurseSubFolders, &pkLibrary);

    // This code should change if non-D3D NiShader libraries are supported.
    if (!bSuccess || pkLibrary == NULL)
    {
        ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to load library from %s\n", pcLibName);
        return false;
    }

    NiShaderLibrary* pkExistingLib = 
        ms_pkD3DShaderFactory->FindLibrary(pkLibrary->GetName());
    if (pkExistingLib && (pkExistingLib != pkLibrary))
    {
        NiOutputDebugString("Library ");
        NiOutputDebugString(pkLibrary->GetName());
        NiOutputDebugString(" has been reloaded!\n");
        NiOutputDebugString("REPLACING EXISTING SHADER LIBRARY!\n");

        ms_pkD3DShaderFactory->RemoveLibrary(pkLibrary->GetName());
        // Library will be released when it is removed from the library; 
        // no need to delete it explicitly.
    }

    *ppkLibrary = pkLibrary;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderFactory::DefaultDLLRunParser(const char* pcLibName, 
    NiRenderer*, const char* pcDirectory, bool bRecurseSubFolders)
{
    if (!ms_pkD3DShaderFactory)
        return false;

    // Load the interface library
    HMODULE hLibrary = LoadLibrary(pcLibName);
    if (hLibrary == 0)
    {
        ReportError(NISHADERERR_UNKNOWN, false, 
            "LoadLibrary failed on %s\n", pcLibName);
        return 0;
    }

    // Ensure library was built with the same compiler
    NIGETCOMPILERVERSIONFUNCTION pfnGetCompilerVersionFunc =
        (NIGETCOMPILERVERSIONFUNCTION)GetProcAddress(hLibrary, 
        "GetCompilerVersion");
    if (pfnGetCompilerVersionFunc)
    {
        unsigned int uiShaderVersion = pfnGetCompilerVersionFunc();
        if (uiShaderVersion != (_MSC_VER))
        {
            ReportError(NISHADERERR_UNKNOWN, false, 
                "Shader Library %s\n"
                "was built with different version of Visual Studio.\n\n"
                "Library compiled with %s.\n"
                "This application compiled with %s.", pcLibName,
                GetCompilerName(uiShaderVersion), GetCompilerName(_MSC_VER));
            FreeLibrary(hLibrary);
            return 0;
        }
    }
    else
    {
        // Older version - run it anyway
    }

    // Fetch the library parse function
    NID3DSLI_RUNPARSER pfnParse = (NID3DSLI_RUNPARSER)GetProcAddress(
        hLibrary, "RunShaderParser");

    if (pfnParse == 0)
    {
        ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to retrieve 'RunShaderParser' function from %s\n", 
            pcLibName);
        FreeLibrary(hLibrary);
        return 0;
    }

    unsigned int uiCount = pfnParse(pcDirectory, bRecurseSubFolders);

    FreeLibrary(hLibrary);

    return uiCount;
}
#endif //#if defined(_USRDLL)
//---------------------------------------------------------------------------
void NiD3DShaderFactory::SetRenderer(NiD3DRenderer* pkRenderer)
{
    if (IsActiveFactory() == false)
        ms_pkD3DShaderFactory->SetAsActiveFactory();

    if (m_pkRenderer != pkRenderer)
    {
        m_pkRenderer = pkRenderer;

        NiShaderLibrary* pkLibrary = GetFirstLibrary();
        while (pkLibrary)
        {
            NiShaderLibraryDesc* pkDesc = pkLibrary->GetShaderLibraryDesc();
            
            if (pkDesc->IsPlatformDX9())
            {
                NiD3DShaderLibrary* pkD3DShaderLibrary = 
                    (NiD3DShaderLibrary*)pkLibrary;
                pkD3DShaderLibrary->SetRenderer(pkRenderer);

            }
            pkLibrary = GetNextLibrary();
        }
    }
}
//---------------------------------------------------------------------------
NiShader* NiD3DShaderFactory::RetrieveShader(const char* pcName, 
    unsigned int uiImplementation, bool bReportNotFound)
{
    NIASSERT(IsActiveFactory());

    if (!m_pkRenderer)
    {
        NiOutputDebugString("Attempting to RetrieveShader w/out a valid "
            "renderer!\n");
        return 0;
    }

    if (!pcName || pcName[0] == '\0')
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Attempting to RetrieveShader w/out a valid name!\n");
        return 0;
    }

    NiShader* pkShader = FindShader(pcName, uiImplementation);
    if (pkShader == 0)
    {
        // The shader is not currently loaded...
        // Check the libraries for it.

        m_kShaderMapLock.Lock();

        // NOTE: Since we iterate over the libraries, if more than one 
        // library contains a shader of the same name, the first one
        // found is the one returned. This means ALL SHADERS MUST HAVE
        // UNIQUE NAMES to avoid issues.
        NiShaderLibrary* pkLibrary = GetFirstLibrary();

        // Atomically re-verify our assumption w.r.t. other shaders creation.
        pkShader = FindShader(pcName, uiImplementation);

        if (pkShader)
        {
            // Another shader created this resource before we could lock the
            // critical section so we're all set. We'll just short circuit 
            // the loop.
            pkLibrary = 0;
        }

        // Lock the renderer, since resource allocation may happen here.
        m_pkRenderer->LockRenderer();

        while (pkLibrary)
        {
            pkShader = pkLibrary->GetShader(m_pkRenderer, pcName, 
                uiImplementation);
            if (pkShader)
            {
                // Register the shader w/ the renderer
                NiD3DShaderInterface* pkD3DSI = NiVerifyStaticCast
                    (NiD3DShaderInterface, pkShader);

                // Insert it into the map
                InsertShader(pkShader, uiImplementation);

                m_pkRenderer->RegisterD3DShader(pkD3DSI);
                pkD3DSI->SetD3DRenderer(m_pkRenderer);

                // Stop
                break;
            }

            pkLibrary = GetNextLibrary();
        }

        m_pkRenderer->UnlockRenderer();

        m_kShaderMapLock.Unlock();
    }

    if (bReportNotFound && pkShader == 0)
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to find shader %s, Implementation %d\n", 
            pcName, uiImplementation);
    }

    return pkShader;

}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::InsertShader(NiShader* pkShader, 
    unsigned int uiImplementation)
{
    NI_UNUSED_ARG(uiImplementation);
    NIASSERT(IsActiveFactory());

    if (pkShader == NULL || m_pkShaderMap == NULL)
        return;

    NIASSERT(uiImplementation == NiShader::DEFAULT_IMPLEMENTATION ||
        uiImplementation == pkShader->GetImplementation());

    // We want to allow for the same shader, but different implementations.
    // So, we will tack the implementation number on the end of the shader
    // name when we place it in the list.
    char acFullName[NI_MAX_PATH];
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pkShader->GetName(), 
        pkShader->GetImplementation());

    m_pkShaderMap->SetAt(acFullName, pkShader);

    // Register the shader as the best implementation as well, if it is.
    NiD3DShaderInterface* pkD3DShader = 
        NiVerifyStaticCast(NiD3DShaderInterface, pkShader);
    if (pkD3DShader->GetIsBestImplementation())
    {
        NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pkShader->GetName(), 
            NiShader::DEFAULT_IMPLEMENTATION);
        m_pkShaderMap->SetAt(acFullName, pkShader);
    }
}
//---------------------------------------------------------------------------
NiShader* NiD3DShaderFactory::FindShader(const char* pcName,
    unsigned int uiImplementation)
{
    NIASSERT(IsActiveFactory());

    if (!m_pkShaderMap)
        return NULL;

    NiShader* pkShader = 0;

    char acFullName[NI_MAX_PATH];
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pcName, uiImplementation);

    if (m_pkShaderMap->GetAt(acFullName, pkShader))
        return pkShader;

    return 0;
}
//---------------------------------------------------------------------------
bool NiD3DShaderFactory::ReleaseShaderFromLibrary(const char* pcName, 
    unsigned int uiImplementation)
{
    NIASSERT(IsActiveFactory());

    if (!pcName || pcName[0] == '\0')
        return false;

    return ReleaseShaderFromLibrary(FindShader(pcName, uiImplementation));
}
//---------------------------------------------------------------------------
bool NiD3DShaderFactory::ReleaseShaderFromLibrary(NiShader* pkShader)
{
    NIASSERT(IsActiveFactory());

    if (pkShader == NULL)
        return false;

    NiShader* pkTestShader = FindShader(pkShader->GetName(), 
        pkShader->GetImplementation());

    if (pkTestShader == NULL || pkShader != pkTestShader)
    {
        // Is this a more serious problem?
        return false;
    }

    m_kShaderMapLock.Lock();
    NiShaderLibrary* pkLibrary = GetFirstLibrary();
    bool bRet = false;
    while (pkLibrary)
    {
        if (pkLibrary->ReleaseShader(pkShader))
        {
            bRet = true;
            break;
        }
        pkLibrary = GetNextLibrary();
    }

    m_kShaderMapLock.Unlock();

    // Remove shader from factory as well
    RemoveShader(pkShader->GetName(), pkShader->GetImplementation());

    return bRet;
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RemoveShader(const char* pcName,
    unsigned int uiImplementation)
{
    NIASSERT(IsActiveFactory());

    if (!m_pkShaderMap)
        return;

    char acFullName[NI_MAX_PATH];
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pcName, uiImplementation);

    m_pkShaderMap->RemoveAt(acFullName);

    // Unregister the shader as the best implementation as well, if it is.
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pcName, 
        NiShader::DEFAULT_IMPLEMENTATION);
    m_pkShaderMap->RemoveAt(acFullName);
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RemoveAllShaders()
{
    NIASSERT(IsActiveFactory() || m_pkShaderMap == NULL || 
        m_pkShaderMap->IsEmpty());

    if (m_pkShaderMap)
        m_pkShaderMap->RemoveAll();
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::InsertLibrary(NiShaderLibrary* pkLibrary)
{
    NIASSERT(IsActiveFactory());

    if (!pkLibrary)
        return;

    NIASSERT(m_pkLibraryMap);

    m_pkLibraryMap->SetAt(pkLibrary->GetName(), pkLibrary);
}
//---------------------------------------------------------------------------
NiShaderLibrary* NiD3DShaderFactory::FindLibrary(const char* pcName)
{
    NIASSERT(IsActiveFactory());

    NIASSERT(m_pkLibraryMap);

    NiShaderLibraryPtr spLibrary = 0;

    if (m_pkLibraryMap->GetAt(pcName, spLibrary))
        return spLibrary;

    return 0;
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RemoveLibrary(const char* pcName)
{
    NIASSERT(IsActiveFactory());

    NIASSERT(m_pkLibraryMap);

    NiShaderLibraryPtr spLibrary = 0;

    if (m_pkLibraryMap->GetAt(pcName, spLibrary))
    {
        if (spLibrary->GetRefCount() == 2)
        {
            m_pkLibraryMap->RemoveAt(pcName);
            // This doesn't have to be here, as it will occur when the
            // function exits. For testing purposes, it is here.
            spLibrary = 0;
        }
    }

    m_kLibraryIter = NULL;
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RemoveAllLibraries()
{
    NIASSERT(IsActiveFactory() || m_pkLibraryMap == NULL ||
        m_pkLibraryMap->IsEmpty());

    if (!m_pkLibraryMap)
        return;

    NiShaderLibrary* pkLibrary = NiD3DShaderFactory::GetFirstLibrary();
    while (pkLibrary)
    {
        m_pkLibraryMap->RemoveAt(pkLibrary->GetName());

        pkLibrary = NiD3DShaderFactory::GetNextLibrary();
    }

    m_pkLibraryMap->RemoveAll();

    m_kLibraryIter = NULL;
}
//---------------------------------------------------------------------------
NiShaderLibrary* NiD3DShaderFactory::GetFirstLibrary()
{
    NIASSERT(IsActiveFactory() || m_pkLibraryMap->IsEmpty());

    if (!m_pkLibraryMap)
        return 0;

    m_kLibraryIter = m_pkLibraryMap->GetFirstPos();
    if (m_kLibraryIter)
    {
        const char* pcName;
        NiShaderLibraryPtr spLibrary;
        m_pkLibraryMap->GetNext(m_kLibraryIter, pcName, spLibrary);
        return spLibrary;
    }

    return 0;
}
//---------------------------------------------------------------------------
NiShaderLibrary* NiD3DShaderFactory::GetNextLibrary()
{
    NIASSERT(IsActiveFactory() || m_pkLibraryMap->IsEmpty());

    if (!m_pkLibraryMap)
        return 0;

    if (m_kLibraryIter)
    {
        const char* pcName;
        NiShaderLibraryPtr spLibrary;
        m_pkLibraryMap->GetNext(m_kLibraryIter, pcName, spLibrary);
        return spLibrary;
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::PurgeAllShaderRendererData()
{
    NIASSERT(IsActiveFactory());

    // PurgeAllShaderRendererData is called when the application wishes to
    // destroy the renderer, but retain the currently 'loaded' shader list.
    // This will call NiD3DRenderer::PurgeAllD3DShaders to destroy the
    // renderer specific data contained in them. However, the 'shell' of the
    // shader will still exist to allow for recreation of the data and reuse
    // of the shader.
    if (!m_pkRenderer)
        return;

    m_pkRenderer->PurgeAllD3DShaders();
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RestoreAllShaderRendererData()
{
    NIASSERT(IsActiveFactory());

    if (!m_pkRenderer || !m_pkShaderMap)
        return;

    // The shader map will be walked, with each shader being 'recreated'.
    NiShader* pkShader = 0;
    const char* pcName;
    NiTMapIterator kIter = m_pkShaderMap->GetFirstPos();
    while (kIter)
    {
        m_pkShaderMap->GetNext(kIter, pcName, pkShader);
        if (pkShader)
        {
            // Set the renderer pointer
            NiD3DShaderInterface* pkD3DShader = 
                (NiD3DShaderInterface*)pkShader;
            if (pkD3DShader->IsInitialized() == false)
            {
                pkD3DShader->SetD3DRenderer(m_pkRenderer);
                pkD3DShader->RecreateRendererData();
                m_pkRenderer->RegisterD3DShader(pkD3DShader);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiD3DShaderFactory::IsDefaultImplementation(NiShader* pkShader)
{
    NIASSERT(IsActiveFactory());

    NiD3DShaderInterface* pkShaderIf = NiDynamicCast(NiD3DShaderInterface, 
        pkShader);

    if (pkShaderIf)
    {
        return pkShaderIf->GetIsBestImplementation();
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::GetMajorVertexShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DSHADER_VERSION_MAJOR(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::GetMinorVertexShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DSHADER_VERSION_MINOR(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::CreateVertexShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DVS_VERSION(uiMajorVersion, uiMinorVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::GetMajorGeometryShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DSHADER_VERSION_MAJOR(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::GetMinorGeometryShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DSHADER_VERSION_MINOR(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::CreateGeometryShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    NIASSERT(IsActiveFactory());

    return (0xFFFD0000 | (uiMajorVersion << 8) | uiMinorVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::GetMajorPixelShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DSHADER_VERSION_MAJOR(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::GetMinorPixelShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DSHADER_VERSION_MINOR(uiVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3DShaderFactory::CreatePixelShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    NIASSERT(IsActiveFactory());

    return D3DPS_VERSION(uiMajorVersion, uiMinorVersion);
}
//---------------------------------------------------------------------------
bool NiD3DShaderFactory::IsActiveFactory()
{
    return (ms_pkShaderFactory == ms_pkD3DShaderFactory);
}
//---------------------------------------------------------------------------
bool NiD3DShaderFactory::SetAsActiveFactory()
{
    if (IsActiveFactory())
        return false;

    NiTPointerList<NiShaderLibraryPtr> kShaderLibrariesToTransfer;

    if (ms_pkShaderFactory)
    {
        // Transfer data from the former active factory to the new one

        // Transfer callback functions, but only if they're not the defaults
        if (ms_pkShaderFactory->GetClassCreateCallback() != 
            ms_pkShaderFactory->GetDefaultClassCreateCallback())
        {
            ms_pkD3DShaderFactory->m_pfnClassCreate = 
                ms_pkShaderFactory->GetClassCreateCallback();
        }
        if (ms_pkShaderFactory->GetRunParserCallback() != 
            ms_pkShaderFactory->GetDefaultRunParserCallback())
        {
            ms_pkD3DShaderFactory->m_pfnRunParser = 
                ms_pkShaderFactory->GetRunParserCallback();
        }
        if (ms_pkShaderFactory->GetErrorCallback() != 
            ms_pkShaderFactory->GetDefaultErrorCallback())
        {
            ms_pkD3DShaderFactory->m_pfnErrorCallback = 
                ms_pkShaderFactory->GetErrorCallback();
        }

        // Transfer global shader constant entries
        //NiTMapIterator kIter = NULL;
        //NiFixedString kKey;
        //NiGlobalConstantEntry* pkEntry = 
        //    ms_pkShaderFactory->GetFirstGlobalShaderConstant(kIter, kKey);
        //while (pkEntry)
        //{
        //    NIASSERT(kKey.Exists());
        //    ms_pkD3DShaderFactory->m_kGlobalConstantMap.SetAt(kKey, pkEntry);
        //    ms_pkShaderFactory->ReleaseGlobalShaderConstant(kKey);
        //    pkEntry = 
        //        ms_pkShaderFactory->GetNextGlobalShaderConstant(kIter, kKey);
        //}

        // We should not have loaded any actual shaders at this point, so
        // there's no need to transfer them.

        // Transfer shader libraries
        NiShaderLibrary* pkLibrary = ms_pkShaderFactory->GetFirstLibrary();
        while (pkLibrary)
        {
            NiShaderLibraryPtr spLibrary = pkLibrary;
            kShaderLibrariesToTransfer.AddTail(spLibrary);
            pkLibrary = ms_pkShaderFactory->GetNextLibrary();
        }
        ms_pkShaderFactory->RemoveAllLibraries();

        // Shader program directories should not yet be applied, since
        // no shader program factory should yet exist.

#if defined(_USRDLL)
        // Transfer DLL HMODULEs
        const char* pcDLLName = NULL;
        HMODULE hDLL = 
            (HMODULE)ms_pkShaderFactory->GetFirstLibraryDLL(pcDLLName);
        if (hDLL)
        {
            NIASSERT(pcDLLName);
            ms_pkD3DShaderFactory->m_kLoadedShaderLibDLLs.SetAt(pcDLLName, 
                hDLL);
            hDLL = (HMODULE)ms_pkShaderFactory->GetNextLibraryDLL(pcDLLName);
        }
        ms_pkShaderFactory->ClearLibraryDLLs();
#endif //#if defined(_USRDLL)
    }

    ms_pkShaderFactory = ms_pkD3DShaderFactory;

    NiTListIterator kIter = kShaderLibrariesToTransfer.GetHeadPos();
    while (kIter)
    {
        NiShaderLibraryPtr spLib = kShaderLibrariesToTransfer.GetNext(kIter);
        ms_pkD3DShaderFactory->InsertLibrary(spLib);
    }

    return true;
}
//---------------------------------------------------------------------------
NiD3DShaderFactory* NiD3DShaderFactory::GetD3DShaderFactory()
{
    return ms_pkD3DShaderFactory;
}
//---------------------------------------------------------------------------
const char* NiD3DShaderFactory::GetFirstProgramDirectory(
    NiTListIterator& kIter)
{
    NiD3DShaderProgramFactory* pkSPFactory = 
        NiD3DShaderProgramFactory::GetInstance();
    if (!pkSPFactory)
        return 0;

    return pkSPFactory->GetFirstProgramDirectory(kIter);
}
//---------------------------------------------------------------------------
const char* NiD3DShaderFactory::GetNextProgramDirectory(
    NiTListIterator& kIter)
{
    NiD3DShaderProgramFactory* pkSPFactory = 
        NiD3DShaderProgramFactory::GetInstance();
    if (!pkSPFactory)
        return 0;

    return pkSPFactory->GetNextProgramDirectory(kIter);
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::AddProgramDirectory(const char* pacDirectory)
{
    NiD3DShaderProgramFactory* pkFactory = 
        NiD3DShaderProgramFactory::GetInstance();
    if (pkFactory)
        pkFactory->AddProgramDirectory(pacDirectory);
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RemoveProgramDirectory(const char* pacDirectory)
{
    NiD3DShaderProgramFactory* pkFactory = 
        NiD3DShaderProgramFactory::GetInstance();
    if (pkFactory)
        pkFactory->RemoveProgramDirectory(pacDirectory);
}
//---------------------------------------------------------------------------
void NiD3DShaderFactory::RemoveAllProgramDirectories()
{
    NiD3DShaderProgramFactory* pkFactory = 
        NiD3DShaderProgramFactory::GetInstance();
    if (pkFactory)
        pkFactory->RemoveAllProgramDirectories();
}
//---------------------------------------------------------------------------
const char* NiD3DShaderFactory::GetCompilerName(unsigned int uiVersion)
{
    if (uiVersion == 1310)
        return "VS 7.1";
    else if (uiVersion == 1400)
        return "VS 8.0";
    else if (uiVersion == 1500)
        return "VS 9.0";
    else
        return "Unknown version";
}
//---------------------------------------------------------------------------
const char* NiD3DShaderFactory::GetRendererString() const
{
    return "DX9";
}
//---------------------------------------------------------------------------
NiShaderFactory::NISHADERFACTORY_CLASSCREATIONCALLBACK 
    NiD3DShaderFactory::GetDefaultClassCreateCallback() const
{
#if defined(_USRDLL)
    return DefaultDLLCreateClass;
#else //#if defined(_USRDLL)
    return NULL;
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
NiShaderFactory::NISHADERFACTORY_RUNPARSERCALLBACK 
    NiD3DShaderFactory::GetDefaultRunParserCallback() const
{
#if defined(_USRDLL)
    return DefaultDLLRunParser;
#else //#if defined(_USRDLL)
    return NULL;
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
NiShaderFactory::NISHADERFACTORY_ERRORCALLBACK 
    NiD3DShaderFactory::GetDefaultErrorCallback() const
{
    return NULL;
}
//---------------------------------------------------------------------------
