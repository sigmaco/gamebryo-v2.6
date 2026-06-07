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

#include "NiD3DXEffectLoader.h"
#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectShaderLibrary.h"

#include <NiD3DUtility.h>
#include <NiShaderLibraryDesc.h>

#if defined (WIN32)
    // Must link in the libraries this way because the name changes based on
    // the DX SDK version.
    #if defined(NIDEBUG)
        // In February 2005 SDK update, D3DX9dt.lib no longer exists
        #if D3DX_SDK_VERSION >= 24
            #pragma comment(lib, "D3DX9d.lib")
        #else //#if D3DX_SDK_VERSION >= 24
            #pragma comment(lib, "D3DX9dt.lib")
        #endif //#if D3DX_SDK_VERSION >= 24
    #else //#if defined(NIDEBUG)
        #pragma comment(lib, "D3DX9.lib")
    #endif //#if defined(NIDEBUG)
#endif

//---------------------------------------------------------------------------
NiD3DXEffectLoader* NiD3DXEffectLoader::ms_pkLoader = 0;

//---------------------------------------------------------------------------
NiD3DXEffectLoader::NiD3DXEffectLoader()
{
    m_kD3DXEffectList.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3DXEffectLoader::~NiD3DXEffectLoader()
{
    m_kD3DXEffectListIter = m_kD3DXEffectList.GetHeadPos();
    while (m_kD3DXEffectListIter)
    {
        char* pcName = m_kD3DXEffectList.GetNext(m_kD3DXEffectListIter);
        if (pcName)
            NiFree(pcName);
    }
    m_kD3DXEffectList.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3DXEffectLoader* NiD3DXEffectLoader::Create()
{
    if (ms_pkLoader)
    {
        NiTListIterator kIter = ms_pkLoader->m_kD3DXEffectList.GetHeadPos();
        while (kIter)
        {
            char* pcName = ms_pkLoader->m_kD3DXEffectList.GetNext(kIter);
            if (pcName)
                NiFree(pcName);
        }
        ms_pkLoader->m_kD3DXEffectList.RemoveAll();
    }
    else
    {
        ms_pkLoader = NiNew NiD3DXEffectLoader();
    }

    return ms_pkLoader;
}
//---------------------------------------------------------------------------
void NiD3DXEffectLoader::Destroy()
{
    if (ms_pkLoader)
        NiDelete ms_pkLoader;

    ms_pkLoader = 0;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectLoader::LoadAllD3DXEffectFiles(
    NiD3DXEffectShaderLibrary* pkLibrary, const char* pcDirectory, 
    bool bRecurseSubDirs)
{
    FindAllD3DXEffectFiles(pcDirectory, bRecurseSubDirs);
    if (!LoadAllD3DXEffectFiles(pkLibrary))
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectLoader::GetD3DXEffectFileCount()
{
    return m_kD3DXEffectList.GetSize();
}
//---------------------------------------------------------------------------
const char* NiD3DXEffectLoader::GetFirstD3DXEffectFile()
{
    m_kD3DXEffectListIter = m_kD3DXEffectList.GetHeadPos();
    
    if (m_kD3DXEffectListIter)
        return m_kD3DXEffectList.GetNext(m_kD3DXEffectListIter);
    return 0;
}
//---------------------------------------------------------------------------
const char* NiD3DXEffectLoader::GetNextD3DXEffectFile()
{
    if (m_kD3DXEffectListIter)
        return m_kD3DXEffectList.GetNext(m_kD3DXEffectListIter);
    return 0;
}
//---------------------------------------------------------------------------
void NiD3DXEffectLoader::FindAllD3DXEffectFiles(const char* pcDirectory, 
    bool bRecurseDirectories)
{
    FindAllD3DXEffectFilesInDirectory(pcDirectory, ".FX", bRecurseDirectories,
        &m_kD3DXEffectList);
    FindAllD3DXEffectFilesInDirectory(pcDirectory, ".FXO", bRecurseDirectories,
        &m_kD3DXEffectList);
    if (NiD3DXEffectFactory::GetFXLSupport())
    {
        FindAllD3DXEffectFilesInDirectory(pcDirectory, ".FXL", 
            bRecurseDirectories, &m_kD3DXEffectList);
    }
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectLoader::FindAllD3DXEffectFilesInDirectory(
    const char* pcDirectory, const char* pcExt, bool bRecurseDirectories, 
    NiTPointerList<char*>* pkFileList)
{
    if (!pcDirectory || (strcmp(pcDirectory, "") == 0))
        return 0;
    if (!pcExt || (strcmp(pcExt, "") == 0))
        return 0;

    char acFilePath[_MAX_PATH];
    {
        NiStrncpy(acFilePath, _MAX_PATH, pcDirectory, _MAX_PATH - 1);
        size_t stLen = strlen(acFilePath);
        if ((acFilePath[stLen - 1] != '\\') && (acFilePath[stLen - 1] != '/'))
        {
            acFilePath[stLen] = '\\';
            acFilePath[stLen + 1] = 0;
        }

        // replace / with \ .
        for (int iPos = 0; iPos < (int)stLen; iPos++)
        {
            if (acFilePath[iPos] == '/')
                acFilePath[iPos] = '\\';
        }
    }

    char acFileName[_MAX_PATH];
    {
        NiStrcpy(acFileName, _MAX_PATH, pcDirectory);
        size_t stLen = strlen(acFileName);
        if ((acFileName[stLen - 1] != '\\') && (acFileName[stLen - 1] != '/'))
        {
            acFileName[stLen] = '\\';
            acFileName[stLen + 1] = 0;
        }

        // replace / with \ .
        for (int iPos = 0; iPos < (int)stLen; iPos++)
        {
            if (acFileName[iPos] == '/')
                acFileName[iPos] = '\\';
        }
    }

    // This will cover the case when the directory is a mapped network
    // drive...
    NiStrcat(acFileName, _MAX_PATH, "*");

    unsigned int uiCount = 0;
    bool bDone = false;
    char acFileName2[_MAX_PATH];

    WIN32_FIND_DATA wfd;
    memset(&wfd, 0, sizeof(WIN32_FIND_DATA));
    HANDLE hFile = FindFirstFile(acFileName, &wfd);

    if (INVALID_HANDLE_VALUE != hFile)
    {
        NiStrcpy(acFileName2, _MAX_PATH, acFilePath);
        NiStrcat(acFileName2, _MAX_PATH, wfd.cFileName);

        while (!bDone)
        {
            NiStrcpy(acFileName2, _MAX_PATH, acFilePath);
            NiStrcat(acFileName2, _MAX_PATH, wfd.cFileName);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                GetLastError();
                NiD3DUtility::Log("Invalid handle on FindXXXXXFile\n");
                bDone = true;
            }
            else
            {
                DWORD dwAttrib = GetFileAttributes(acFileName2);
                if ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    if (strcmp(wfd.cFileName, "."))
                    {
                        if (strcmp(wfd.cFileName, ".."))
                        {
                            // If we are recursing... do it
                            if (bRecurseDirectories)
                            {
                                NiStrcat(acFileName2, _MAX_PATH, "\\");
                                NiD3DUtility::Log("    Recurse directory %s\n",
                                    acFileName2);
                                uiCount += FindAllD3DXEffectFilesInDirectory(
                                    acFileName2, pcExt, bRecurseDirectories,
                                    pkFileList);
                            }
                        }
                    }
                }
                else
                {
                    if (ProcessD3DXEffectFile(acFileName2, pcExt, pkFileList))
                        uiCount++;
                }
            }

            if (FindNextFile(hFile, &wfd) == false)
                bDone = true;
        }

        FindClose(hFile);
    }

    return uiCount;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectLoader::ProcessD3DXEffectFile(const char* pcFilename, 
    const char* pcExt, NiTPointerList<char*>* pkFileList)
{
    if (!pcFilename || (strcmp(pcFilename, "") == 0))
        return false;

    char acExt[_MAX_EXT];

    NiD3DUtility::GetFileExtension(pcFilename, acExt, _MAX_EXT);
    if (NiStricmp(acExt, pcExt) == 0)
    {
        NiD3DUtility::Log("        Found %s File %s\n", pcExt, pcFilename);

        // Add it to the list
        size_t stLen = strlen(pcFilename) + 1;
        char* pcNew = NiAlloc(char, stLen);
        NIASSERT(pcNew);

        NiStrcpy(pcNew, stLen, pcFilename);

        pkFileList->AddTail(pcNew);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectLoader::LoadAllD3DXEffectFiles(
    NiD3DXEffectShaderLibrary* pkLibrary)
{
    // Lock the renderer if necessary so that we can have unfettered 
    // access to Direct3D.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer)
        pkRenderer->LockRenderer();

    const char* pcFile = GetFirstD3DXEffectFile();
    while (pcFile)
    {
        NIASSERT(pkLibrary);

        // Parse the name and see if it's in the list.
        char acFileName[_MAX_PATH];
        NiD3DUtility::GetFileName(pcFile, acFileName, _MAX_PATH);

        if (pkLibrary->GetD3DXEffectFile(acFileName))
        {
            NiD3DUtility::Log("%s (%s) already loaded?\n", 
                acFileName, pcFile);
        }
        else
        {
            NiD3DUtility::Log("Attempting to load %s\n", pcFile);

            NiD3DXEffectFile* pkD3DXEffectFile = 
                NiD3DXEffectFactory::CreateD3DXEffectFromFile(pcFile);
            if (pkD3DXEffectFile)
            {
                NiD3DUtility::Log("Loaded %s\n", pcFile);
                pkLibrary->InsertD3DXEffectFileIntoList(pkD3DXEffectFile);
            }
            else
            {
                NiD3DUtility::Log("Failed to load %s\n", pcFile);
            }
        }
        pcFile = GetNextD3DXEffectFile();
    }

    if (pkRenderer)
        pkRenderer->UnlockRenderer();

    // Always return true, even if no files successfully loaded
    return true;
}
//---------------------------------------------------------------------------
void* NiD3DXEffectLoader::CreateEffectFromFile(void* pvD3DDevice, 
    const char* pcShaderPath, unsigned int uiCreationFlags, 
    const D3DXMACRO* pkMacros, char*& pcError)
{
    // When no device exists, parse using ID3DXEffectCompiler
    if (pvD3DDevice == NULL)
    {
        return NiD3DXEffectLoader::CreateEffectCompilerFromFile(
            pcShaderPath, uiCreationFlags, pkMacros, pcError);
    }

    char* pcText = NULL;
    unsigned int uiLength = 0;
    pcText = GetShaderText(pcShaderPath, uiLength);

    if (!pcText)
    {
        return NULL;
    }

    NIASSERT(ms_pkLoader);
    ms_pkLoader->m_kCriticalSection.Lock();
    ms_pkLoader->m_kD3DXInclude.SetBasePath(pcShaderPath);
    void* pvReturn = CreateEffectFromCode(pvD3DDevice, pcText, uiLength, 
        uiCreationFlags, pkMacros, pcError, &ms_pkLoader->m_kD3DXInclude);
    ms_pkLoader->m_kCriticalSection.Unlock();

    // Clean up from GetShaderText.
    NiFree(pcText);
    return pvReturn;

}
//---------------------------------------------------------------------------
void* NiD3DXEffectLoader::CreateEffectCompilerFromFile(
    const char* pcShaderPath, unsigned int uiCreationFlags, 
    const D3DXMACRO* pkMacros, char*& pcError)
{
    char* pcText = NULL;
    unsigned int uiLength = 0;
    pcText = GetShaderText(pcShaderPath, uiLength);

    if (!pcText)
    {
        return NULL;
    }

    NIASSERT(ms_pkLoader);
    ms_pkLoader->m_kCriticalSection.Lock();
    ms_pkLoader->m_kD3DXInclude.SetBasePath(pcShaderPath);
    void* pvReturn = CreateEffectCompilerFromCode(pcText, uiLength, 
        uiCreationFlags, pkMacros, pcError, &ms_pkLoader->m_kD3DXInclude);
    ms_pkLoader->m_kCriticalSection.Unlock();

    // Clean up from GetShaderText.
    NiFree(pcText);
    return pvReturn;
}
//---------------------------------------------------------------------------
void* NiD3DXEffectLoader::CreateEffectFromCode(void* pvD3DDevice, 
    void* pvCode, unsigned int uiCodeSize, unsigned int uiCreationFlags, 
    const D3DXMACRO* pkMacros, char*& pcError, ID3DXInclude* pkInclude)
{
    // When no device exists, parse using ID3DXEffectCompiler
    if (pvD3DDevice == NULL)
    {
        return NiD3DXEffectLoader::CreateEffectCompilerFromCode(pvCode, 
            uiCodeSize, uiCreationFlags, pkMacros, pcError);
    }

    LPDIRECT3DDEVICE9 pkD3DDevice = (LPDIRECT3DDEVICE9)pvD3DDevice;

    LPD3DXEFFECT pkEffect;
    LPD3DXBUFFER pkErrors;

#if defined WIN32
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
#elif defined _XENON
    NiXenonRenderer* pkRenderer = NiXenonRenderer::GetRenderer();
#else
#error "Unsupported platform"
#endif

    NIASSERT(pkRenderer);
    pkMacros = pkRenderer->GetD3DXMacroList("fx", pkMacros);

    // Create the effect from the code
    HRESULT eResult = ::D3DXCreateEffect(pkD3DDevice, pvCode, uiCodeSize, 
        pkMacros, pkInclude, uiCreationFlags, NULL, &pkEffect, &pkErrors);

    if (FAILED(eResult))
    {
        if (pkErrors)
        {
            LPVOID pvBuff = pkErrors->GetBufferPointer();
            if (pvBuff)
            {
                unsigned int uiLen = pkErrors->GetBufferSize();
                pcError = NiAlloc(char, uiLen);
                NIASSERT(pcError);
                NiStrcpy(pcError, uiLen, (const char*)pvBuff);
            }
            pkErrors->Release();
        }
        else
        {
            NiOutputDebugString("No Error Reported\n");
        }
        return NULL;
    }

    if (pkErrors)
        pkErrors->Release();

    return pkEffect;
}
//---------------------------------------------------------------------------
void* NiD3DXEffectLoader::CreateEffectCompilerFromCode(void* pvCode, 
    unsigned int uiCodeSize, unsigned int uiCreationFlags, 
    const D3DXMACRO* pkMacros, char*& pcError, ID3DXInclude* pkInclude)
{
    LPD3DXEFFECTCOMPILER pkEffectCompiler;
    LPD3DXBUFFER pkErrors;

#if defined WIN32
    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
#elif defined _XENON
    NiXenonRenderer* pkRenderer = NiXenonRenderer::GetRenderer();
#else
#error "Unsupported platform"
#endif

    if (pkRenderer)
        pkMacros = pkRenderer->GetD3DXMacroList("fx", pkMacros);

    // Create the effect compiler from the code
    HRESULT eResult = ::D3DXCreateEffectCompiler((LPCSTR)pvCode, uiCodeSize, 
        pkMacros, pkInclude, uiCreationFlags, &pkEffectCompiler, &pkErrors);

    if (FAILED(eResult))
    {
        if (pkErrors)
        {
            LPVOID pvBuff = pkErrors->GetBufferPointer();
            if (pvBuff)
            {
                unsigned int uiLen = pkErrors->GetBufferSize();
                pcError = NiAlloc(char, uiLen);
                NIASSERT(pcError);
                NiStrcpy(pcError, uiLen, (const char*)pvBuff);
            }
            pkErrors->Release();
        }
        else
        {
            NiOutputDebugString("No Error Reported\n");
        }
        return NULL;
    }

    if (pkErrors)
        pkErrors->Release();

    return pkEffectCompiler;
}
//---------------------------------------------------------------------------
char* NiD3DXEffectLoader::GetShaderText(const char* pcShaderName, 
    unsigned int& uiLength)
{
    // Open a file. We'll use the default buffer of 32KB because the odds
    // of FXL code being longer than that are fairly low.
    NiFile* pkFile = NiFile::GetFile(pcShaderName, NiFile::READ_ONLY);

    // Test that the file allocated and that it opened. The boolean operator
    // on an NiFile object returns the m_bGood flag from that object.
    if (!pkFile || !*pkFile)
    {
        uiLength = 0;
        return NULL;
    }

    uiLength = pkFile->GetFileSize();

    char* pkText = NULL;
    if (uiLength != 0)
    {
        pkText = NiAlloc(char, uiLength);
        pkFile->Read((void*)pkText, uiLength);
    }

    NiDelete pkFile;

    return pkText;
}
//---------------------------------------------------------------------------
