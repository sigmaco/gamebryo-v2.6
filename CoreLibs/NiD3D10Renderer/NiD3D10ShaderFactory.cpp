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

#include "NiD3D10ShaderFactory.h"

#include "NiD3D10Renderer.h"
#include "NiD3D10ShaderInterface.h"
#include "NiD3D10ShaderLibrary.h"
#include "NiD3D10ShaderLibraryInterface.h"
#include "NiD3D10ShaderProgramFactory.h"

NiD3D10ShaderFactory* NiD3D10ShaderFactory::ms_pkD3D10ShaderFactory = NULL;

//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::_SDMInit()
{
    NiD3D10ShaderFactory* pkFactory = 
        (NiD3D10ShaderFactory*)NiD3D10ShaderFactory::Create();
    if (!pkFactory)
    {
        NiD3D10Renderer::Error("Failed to create shader factory!");
    }

#if defined(_USRDLL)
    pkFactory->m_pfnClassCreate = pkFactory->GetDefaultClassCreateCallback();
    pkFactory->m_pfnRunParser = pkFactory->GetDefaultRunParserCallback();
    pkFactory->m_pfnErrorCallback = pkFactory->GetDefaultErrorCallback();
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::_SDMShutdown()
{
    NiD3D10ShaderFactory::Destroy();
}
//---------------------------------------------------------------------------
NiShaderFactory* NiD3D10ShaderFactory::Create()
{
    if (ms_pkD3D10ShaderFactory == 0)
    {
        NiD3D10ShaderFactory* pkFactory = NiNew NiD3D10ShaderFactory();
        NIASSERT(pkFactory);
        ms_pkD3D10ShaderFactory = pkFactory;
    }

    // Set the ms_pkShaderFactory variable only if it is not currently set.
    if (ms_pkShaderFactory == NULL)
        ms_pkShaderFactory = ms_pkD3D10ShaderFactory;

    return ms_pkD3D10ShaderFactory;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::Destroy()
{
    if (ms_pkD3D10ShaderFactory)
    {
        bool bReleaseFactory = false;
        if (IsActiveFactory())
            bReleaseFactory = true;
          
        NiDelete ms_pkD3D10ShaderFactory;
        ms_pkD3D10ShaderFactory = NULL;

        if (bReleaseFactory)
            ms_pkShaderFactory = NULL;
    }
}
//---------------------------------------------------------------------------
NiD3D10ShaderFactory::NiD3D10ShaderFactory() :
    m_kShaderMap(59),
    m_kLibraryMap(37),
#if defined(_USRDLL)
    m_kLibraryDLLIter(NULL),
#endif //#if defined(_USRDLL)
    m_kLibraryIter(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10ShaderFactory::~NiD3D10ShaderFactory()
{
    // We could call the PurgeAllShaderRendererData() function here to 
    // guarantee that all renderer data is released. We will assume that if 
    // the renderer has been deleted this will have already occurred, and if
    // it hasn't, the renderer will take care of it.

    RemoveAllShaders();
    RemoveAllLibraries();
    
#if defined(_USRDLL)
    FreeLibraryDLLs();
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
#if defined(_USRDLL)
void NiD3D10ShaderFactory::FreeLibraryDLLs()
{
    RemoveAllShaders();
    RemoveAllLibraries();

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
void* NiD3D10ShaderFactory::GetFirstLibraryDLL(const char*& pcName)
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
void* NiD3D10ShaderFactory::GetNextLibraryDLL(const char*& pcName)
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
void NiD3D10ShaderFactory::ClearLibraryDLLs()
{
    m_kLoadedShaderLibDLLs.RemoveAll();

    m_kLibraryDLLIter = NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderFactory::DefaultDLLCreateClass(const char* pcLibName, 
    NiRenderer* pkRenderer, int iDirectoryCount, const char* apcDirectories[],
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

    if (!ms_pkD3D10ShaderFactory)
        return false;

    NID3D10SLI_LOADLIBRARY pfnLoad = NULL;
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
        pfnLoad = (NID3D10SLI_LOADLIBRARY)GetProcAddress(hLibrary, 
            "LoadShaderLibrary");
        if (pfnLoad == 0)
        {
            ReportError(NISHADERERR_UNKNOWN, false, 
                "Failed to retrieve 'LoadShaderLibrary' function from %s\n", 
                pcLibName);
            FreeLibrary(hLibrary);
            return false;
        }

        ms_pkD3D10ShaderFactory->m_kLoadedShaderLibDLLs.SetAt(
            pcLibName, hLibrary);
    }
    else
    {
        // It was found once; it damn well better be found again!
        pfnLoad = (NID3D10SLI_LOADLIBRARY)GetProcAddress(hLibrary, 
            "LoadShaderLibrary");
        NIASSERT(pfnLoad);
    }

    NiShaderLibrary* pkLibrary;
    bool bSuccess = pfnLoad(pkRenderer, iDirectoryCount, 
        apcDirectories, bRecurseSubFolders, &pkLibrary);

    // This code should change if non-D3D10 NiShader libraries are supported.
    if (!bSuccess || pkLibrary == NULL)
    {
        ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to load library from %s\n", pcLibName);
        return false;
    }

    NiShaderLibrary* pkExistingLib = 
        ms_pkD3D10ShaderFactory->FindLibrary(pkLibrary->GetName());
    if (pkExistingLib && (pkExistingLib != pkLibrary))
    {
        NiOutputDebugString("Library ");
        NiOutputDebugString(pkLibrary->GetName());
        NiOutputDebugString(" has been reloaded!\n");
        NiOutputDebugString("REPLACING EXISTING SHADER LIBRARY!\n");

        ms_pkD3D10ShaderFactory->RemoveLibrary(pkLibrary->GetName());
        // Library will be released when it is removed from the library; 
        // no need to delete it explicitly.
    }

    *ppkLibrary = pkLibrary;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderFactory::DefaultDLLRunParser(const char* pcLibName, 
    NiRenderer*, const char* pcDirectory, bool bRecurseSubFolders)
{
    if (!ms_pkD3D10ShaderFactory)
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
    NID3D10SLI_RUNPARSER pfnParse = (NID3D10SLI_RUNPARSER)GetProcAddress(
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
NiShader* NiD3D10ShaderFactory::RetrieveShader(const char* pcName, 
    unsigned int uiImplementation, bool bReportNotFound)
{
    NIASSERT(IsActiveFactory());

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL)
    {
        NiOutputDebugString("Attempting to RetrieveShader without a valid "
            "renderer!\n");
        return NULL;
    }
    if (pcName == NULL || pcName[0] == '\0')
    {
        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Attempting to RetrieveShader w/out a valid name!\n");
        return NULL;
    }

    NiShader* pkShader = FindShader(pcName, uiImplementation);
    if (pkShader == NULL)
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

        while (pkLibrary)
        {
            pkShader = pkLibrary->GetShader(pkRenderer, pcName, 
                uiImplementation);
            if (pkShader)
            {
                NIASSERT(NiIsKindOf(NiD3D10ShaderInterface, pkShader));
                // Insert it into the map
                InsertShader(pkShader, uiImplementation);
                
                // Stop
                break;
            }

            pkLibrary = GetNextLibrary();
        }

        m_kShaderMapLock.Unlock();
    }

    if (bReportNotFound && pkShader == 0)
    {
        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
            "Failed to find shader %s, Implementation %d\n", 
            pcName, uiImplementation);
    }

    return pkShader;

}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::InsertShader(NiShader* pkShader, 
    unsigned int uiImplementation)
{
    NI_UNUSED_ARG(uiImplementation);
    NIASSERT(IsActiveFactory());

    if (!pkShader)
        return;

    NIASSERT(uiImplementation == NiShader::DEFAULT_IMPLEMENTATION ||
        uiImplementation == pkShader->GetImplementation());

    // We want to allow for the same shader, but different implementations.
    // So, we will tack the implementation number on the end of the shader
    // name when we place it in the list.
    char acFullName[NI_MAX_PATH];
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pkShader->GetName(), 
        pkShader->GetImplementation());

    NiFixedString kFullName = acFullName;
    m_kShaderMap.SetAt(kFullName, pkShader);
    // Register the shader as the best implementation as well, if it is.
    NiD3D10ShaderInterface* pkD3D10Shader = 
        NiVerifyStaticCast(NiD3D10ShaderInterface, pkShader);
    if (pkD3D10Shader->GetIsBestImplementation())
    {
        NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pkShader->GetName(), 
            NiShader::DEFAULT_IMPLEMENTATION);
        NiFixedString kFullNameBest = acFullName;
        m_kShaderMap.SetAt(kFullNameBest, pkShader);
    }
}
//---------------------------------------------------------------------------
NiShader* NiD3D10ShaderFactory::FindShader(const char* pcName,
    unsigned int uiImplementation)
{
    NIASSERT(IsActiveFactory());

    char acFullName[NI_MAX_PATH];
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pcName, uiImplementation);

    NiShader* pkShader = NULL;
    NiFixedString kFullName = acFullName;
    m_kShaderMap.GetAt(kFullName, pkShader);

    return pkShader;
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderFactory::ReleaseShaderFromLibrary(const char* pcName, 
    unsigned int uiImplementation)
{
    NIASSERT(IsActiveFactory());

    if (!pcName || pcName[0] == '\0')
        return false;

    return ReleaseShaderFromLibrary(FindShader(pcName, uiImplementation));
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderFactory::ReleaseShaderFromLibrary(NiShader* pkShader)
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
void NiD3D10ShaderFactory::RemoveShader(const char* pcName,
    unsigned int uiImplementation)
{
    NIASSERT(IsActiveFactory());

    char acFullName[NI_MAX_PATH];
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pcName, uiImplementation);

    NiFixedString kFullName = acFullName;
    m_kShaderMap.RemoveAt(kFullName);

    // Unregister the shader as the best implementation as well, if it is.
    NiSprintf(acFullName, NI_MAX_PATH, "%s%d", pcName, 
        NiShader::DEFAULT_IMPLEMENTATION);
    NiFixedString kFullNameBext = acFullName;
    m_kShaderMap.RemoveAt(kFullNameBext);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::RemoveAllShaders()
{
    NIASSERT(IsActiveFactory() || m_kShaderMap.IsEmpty());

    m_kShaderMap.RemoveAll();
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::InsertLibrary(NiShaderLibrary* pkLibrary)
{
    NIASSERT(IsActiveFactory());

    if (!pkLibrary)
        return;

    NiFixedString kName = pkLibrary->GetName();
    m_kLibraryMap.SetAt(kName, pkLibrary);
}
//---------------------------------------------------------------------------
NiShaderLibrary* NiD3D10ShaderFactory::FindLibrary(const char* pcName)
{
    NIASSERT(IsActiveFactory());

    NiShaderLibraryPtr spLibrary;

    NiFixedString kName = pcName;
    m_kLibraryMap.GetAt(kName, spLibrary);

    return spLibrary;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::RemoveLibrary(const char* pcName)
{
    NIASSERT(IsActiveFactory());

    NiShaderLibraryPtr spLibrary;

    NiFixedString kName = pcName;
    if (m_kLibraryMap.GetAt(kName, spLibrary))
    {
        if (spLibrary->GetRefCount() == 2)
        {
            m_kLibraryMap.RemoveAt(kName);

            // This doesn't have to be here, as it will occur when the
            // function exits. For testing purposes, it is here.
            spLibrary = 0;
        }
    }

    m_kLibraryIter = NULL;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::RemoveAllLibraries()
{
    NIASSERT(IsActiveFactory() || m_kLibraryMap.IsEmpty());

    NiD3D10ShaderLibrary* pkLibrary = 
        (NiD3D10ShaderLibrary*)NiD3D10ShaderFactory::GetFirstLibrary();
    while (pkLibrary)
    {
        NiFixedString kName = pkLibrary->GetName();
        m_kLibraryMap.RemoveAt(kName);

        pkLibrary = 
            (NiD3D10ShaderLibrary*)NiD3D10ShaderFactory::GetNextLibrary();
    }

    m_kLibraryMap.RemoveAll();

    m_kLibraryIter = NULL;
}
//---------------------------------------------------------------------------
NiShaderLibrary* NiD3D10ShaderFactory::GetFirstLibrary()
{
    NIASSERT(IsActiveFactory() || m_kLibraryMap.IsEmpty());

    m_kLibraryIter = m_kLibraryMap.GetFirstPos();
    if (m_kLibraryIter)
    {
        NiFixedString kName;
        NiShaderLibraryPtr spLibrary;
        m_kLibraryMap.GetNext(m_kLibraryIter, kName, spLibrary);
        return spLibrary;
    }

    return 0;
}
//---------------------------------------------------------------------------
NiShaderLibrary* NiD3D10ShaderFactory::GetNextLibrary()
{
    NIASSERT(IsActiveFactory() || m_kLibraryMap.IsEmpty());

    if (m_kLibraryIter)
    {
        NiFixedString kName;
        NiShaderLibraryPtr spLibrary;
        m_kLibraryMap.GetNext(m_kLibraryIter, kName, spLibrary);
        return spLibrary;
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::PurgeAllShaderRendererData()
{
    NIASSERT(IsActiveFactory());

    // PurgeAllShaderRendererData is called when the application wishes to
    // destroy the renderer, but retain the currently 'loaded' shader list.
    // This will call NiD3D10Renderer::PurgeAllD3D10Shaders to destroy the
    // renderer specific data contained in them. However, the 'shell' of the
    // shader will still exist to allow for recreation of the data and reuse
    // of the shader.

    NiTMapIterator kIter = m_kShaderMap.GetFirstPos();
    while (kIter)
    {
        NiShader* pkShader = 0;
        NiFixedString kName;
        m_kShaderMap.GetNext(kIter, kName, pkShader);
        if (pkShader)
        {
            // Set the renderer pointer
            NiD3D10ShaderInterface* pkD3D10Shader = 
                (NiD3D10ShaderInterface*)pkShader;

            pkD3D10Shader->DestroyRendererData();
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::RestoreAllShaderRendererData()
{
    NIASSERT(IsActiveFactory());

    if (NiD3D10Renderer::GetRenderer() == NULL)
    {
        return;
    }

    // The shader map will be walked, with each shader being 'recreated'.
    NiTMapIterator kIter = m_kShaderMap.GetFirstPos();
    while (kIter)
    {
        NiShader* pkShader = 0;
        NiFixedString kName;
        m_kShaderMap.GetNext(kIter, kName, pkShader);
        if (pkShader)
        {
            // Set the renderer pointer
            NiD3D10ShaderInterface* pkD3D10Shader = 
                (NiD3D10ShaderInterface*)pkShader;
            if (pkD3D10Shader->IsInitialized() == false)
            {
                pkD3D10Shader->RecreateRendererData();
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderFactory::IsDefaultImplementation(NiShader* pkShader)
{
    NIASSERT(IsActiveFactory());

    NiD3D10ShaderInterface* pkShaderIf = NiDynamicCast(NiD3D10ShaderInterface, 
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
const unsigned int NiD3D10ShaderFactory::GetMajorVertexShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return (uiVersion >> 8) & 0xFF;
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::GetMinorVertexShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return (uiVersion >> 0) & 0xFF;
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::CreateVertexShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    NIASSERT(IsActiveFactory());

    return (0xFFFE0000 | (uiMajorVersion << 8) | uiMinorVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::GetMajorGeometryShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return (uiVersion >> 8) & 0xFF;
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::GetMinorGeometryShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return (uiVersion >> 0) & 0xFF;
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::CreateGeometryShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    NIASSERT(IsActiveFactory());

    return (0xFFFD0000 | (uiMajorVersion << 8) | uiMinorVersion);
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::GetMajorPixelShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return (uiVersion >> 8) & 0xFF;
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::GetMinorPixelShaderVersion(
    const unsigned int uiVersion)
{
    NIASSERT(IsActiveFactory());

    return (uiVersion >> 0) & 0xFF;
}
//---------------------------------------------------------------------------
const unsigned int NiD3D10ShaderFactory::CreatePixelShaderVersion(
    const unsigned int uiMajorVersion, const unsigned int uiMinorVersion)
{
    NIASSERT(IsActiveFactory());

    return (0xFFFF0000 | (uiMajorVersion << 8) | uiMinorVersion);
}

//---------------------------------------------------------------------------
bool NiD3D10ShaderFactory::SetAsActiveFactory()
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
            ms_pkD3D10ShaderFactory->m_pfnClassCreate = 
                ms_pkShaderFactory->GetClassCreateCallback();
        }
        if (ms_pkShaderFactory->GetRunParserCallback() != 
            ms_pkShaderFactory->GetDefaultRunParserCallback())
        {
            ms_pkD3D10ShaderFactory->m_pfnRunParser = 
                ms_pkShaderFactory->GetRunParserCallback();
        }
        if (ms_pkShaderFactory->GetErrorCallback() != 
            ms_pkShaderFactory->GetDefaultErrorCallback())
        {
            ms_pkD3D10ShaderFactory->m_pfnErrorCallback = 
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
      //    ms_pkD3D10ShaderFactory->m_kGlobalConstantMap.SetAt(kKey, pkEntry);
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
            ms_pkD3D10ShaderFactory->m_kLoadedShaderLibDLLs.SetAt(pcDLLName, 
                hDLL);
            hDLL = (HMODULE)ms_pkShaderFactory->GetNextLibraryDLL(pcDLLName);
        }
        ms_pkShaderFactory->ClearLibraryDLLs();
#endif //#if defined(_USRDLL)
    }

    ms_pkShaderFactory = ms_pkD3D10ShaderFactory;

    NiTListIterator kIter = kShaderLibrariesToTransfer.GetHeadPos();
    while (kIter)
    {
        NiShaderLibraryPtr spLib = kShaderLibrariesToTransfer.GetNext(kIter);
        ms_pkD3D10ShaderFactory->InsertLibrary(spLib);
    }

    return true;
}
//---------------------------------------------------------------------------
NiD3D10ShaderFactory* NiD3D10ShaderFactory::GetD3D10ShaderFactory()
{
    return ms_pkD3D10ShaderFactory;
}
//---------------------------------------------------------------------------
const char* NiD3D10ShaderFactory::GetFirstProgramDirectory(
    NiTListIterator& kIter)
{
    NiD3D10ShaderProgramFactory* pkSPFactory = 
        NiD3D10ShaderProgramFactory::GetInstance();
    if (!pkSPFactory)
        return 0;

    return pkSPFactory->GetFirstProgramDirectory(kIter);
}
//---------------------------------------------------------------------------
const char* NiD3D10ShaderFactory::GetNextProgramDirectory(
    NiTListIterator& kIter)
{
    NiD3D10ShaderProgramFactory* pkSPFactory = 
        NiD3D10ShaderProgramFactory::GetInstance();
    if (!pkSPFactory)
        return 0;

    return pkSPFactory->GetNextProgramDirectory(kIter);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::AddProgramDirectory(const char* pcDirectory)
{
    NiD3D10ShaderProgramFactory* pkFactory = 
        NiD3D10ShaderProgramFactory::GetInstance();
    if (pkFactory)
        pkFactory->AddProgramDirectory(pcDirectory);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::RemoveProgramDirectory(const char* pcDirectory)
{
    NiD3D10ShaderProgramFactory* pkFactory = 
        NiD3D10ShaderProgramFactory::GetInstance();
    if (pkFactory)
        pkFactory->RemoveProgramDirectory(pcDirectory);
}
//---------------------------------------------------------------------------
void NiD3D10ShaderFactory::RemoveAllProgramDirectories()
{
    NiD3D10ShaderProgramFactory* pkFactory = 
        NiD3D10ShaderProgramFactory::GetInstance();
    if (pkFactory)
        pkFactory->RemoveAllProgramDirectories();
}
//---------------------------------------------------------------------------
const char* NiD3D10ShaderFactory::GetCompilerName(unsigned int uiVersion)
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
const char* NiD3D10ShaderFactory::GetRendererString() const
{
    return "D3D10";
}
//---------------------------------------------------------------------------
NiShaderFactory::NISHADERFACTORY_CLASSCREATIONCALLBACK 
    NiD3D10ShaderFactory::GetDefaultClassCreateCallback() const
{
#if defined(_USRDLL)
    return DefaultDLLCreateClass;
#else //#if defined(_USRDLL)
    return NULL;
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
NiShaderFactory::NISHADERFACTORY_RUNPARSERCALLBACK 
    NiD3D10ShaderFactory::GetDefaultRunParserCallback() const
{
#if defined(_USRDLL)
    return DefaultDLLRunParser;
#else //#if defined(_USRDLL)
    return NULL;
#endif //#if defined(_USRDLL)
}
//---------------------------------------------------------------------------
NiShaderFactory::NISHADERFACTORY_ERRORCALLBACK 
    NiD3D10ShaderFactory::GetDefaultErrorCallback() const
{
    return NULL;
}
//---------------------------------------------------------------------------
