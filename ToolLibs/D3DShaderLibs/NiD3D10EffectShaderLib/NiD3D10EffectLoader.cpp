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

#include "NiD3D10EffectLoader.h"
#include "NiD3D10EffectFactory.h"
#include "NiD3D10EffectShaderLibrary.h"

#include <NiShaderLibraryDesc.h>
#include <NiD3D10Renderer.h>
#include <NiD3D10ShaderFactory.h>

NiD3D10EffectLoader* NiD3D10EffectLoader::ms_pkLoader = NULL;

//---------------------------------------------------------------------------
NiD3D10EffectLoader::NiD3D10EffectLoader() :
    m_kShaderFileExtensions(7)
{
    AddFileExtension(".FX10", false);
    AddFileExtension(".FXO10", true);
    if (NiD3D10EffectFactory::GetFXLSupport())
    {
        AddFileExtension(".FXL", false);
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectLoader::~NiD3D10EffectLoader()
{
    m_kD3D10EffectList.RemoveAll();

    m_kShaderFileExtensions.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10EffectLoader* NiD3D10EffectLoader::GetInstance(bool bCreateNew)
{
    if (ms_pkLoader == NULL && bCreateNew)
    {
        ms_pkLoader = NiNew NiD3D10EffectLoader();
    }

    return ms_pkLoader;
}
//---------------------------------------------------------------------------
void NiD3D10EffectLoader::Destroy()
{
    if (ms_pkLoader)
        NiDelete ms_pkLoader;

    ms_pkLoader = 0;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectLoader::LoadAllD3D10EffectFiles(
    NiD3D10EffectShaderLibrary* pkLibrary, const char* pcDirectory, 
    bool bRecurseSubDirs)
{
    FindAllD3D10EffectFiles(pcDirectory, bRecurseSubDirs);
    if (!LoadStoredD3D10EffectFiles(pkLibrary))
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectLoader::GetD3D10EffectFileCount() const
{
    return m_kD3D10EffectList.GetSize();
}
//---------------------------------------------------------------------------
const char* NiD3D10EffectLoader::GetFirstD3D10EffectFile() const
{
    m_kD3D10EffectListIter = m_kD3D10EffectList.GetHeadPos();
    
    if (m_kD3D10EffectListIter)
        return m_kD3D10EffectList.GetNext(m_kD3D10EffectListIter);
    return 0;
}
//---------------------------------------------------------------------------
const char* NiD3D10EffectLoader::GetNextD3D10EffectFile() const
{
    if (m_kD3D10EffectListIter)
        return m_kD3D10EffectList.GetNext(m_kD3D10EffectListIter);
    return 0;
}
//---------------------------------------------------------------------------
void NiD3D10EffectLoader::FindAllD3D10EffectFiles(const char* pcDirectory, 
    bool bRecurseDirectories)
{
    NiTMapIterator pkExtensionIter = m_kShaderFileExtensions.GetFirstPos();
    while (pkExtensionIter)
    {
        NiFixedString kExt;
        bool bCompiled;
        m_kShaderFileExtensions.GetNext(pkExtensionIter, kExt, bCompiled);
        FindAllD3D10EffectFilesInDirectory(
            pcDirectory, 
            kExt, 
            bRecurseDirectories, 
            &m_kD3D10EffectList);
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectLoader::FindAllD3D10EffectFilesInDirectory(
    const char* pcDirectory, 
    const NiFixedString& kExt, 
    bool bRecurseDirectories, 
    NiTPointerList<NiFixedString>* pkFileList)
{
    if (!pcDirectory || pcDirectory[0] == '\0')
        return 0;
    if (!kExt.Exists())
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
        for (NiInt32 iPos = 0; iPos < (int)stLen; iPos++)
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
        for (NiInt32 iPos = 0; iPos < (int)stLen; iPos++)
        {
            if (acFileName[iPos] == '/')
                acFileName[iPos] = '\\';
        }
    }

    // This will cover the case when the directory is a mapped network
    // drive...
    NiStrcat(acFileName, _MAX_PATH, "*");

    NiUInt32 uiCount = 0;
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
                NILOG(
                    "%s> Invalid handle on FindXXXXXFile\n",
                    __FUNCTION__);
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
                                NILOG(
                                    "    Recurse directory %s\n",
                                    acFileName2);
                                
                                uiCount += FindAllD3D10EffectFilesInDirectory(
                                    acFileName2, 
                                    kExt, 
                                    bRecurseDirectories,
                                    pkFileList);
                            }
                        }
                    }
                }
                else
                {
                    if (ProcessD3D10EffectFile(
                        acFileName2, 
                        kExt, 
                        pkFileList))
                    {
                        uiCount++;
                    }
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
bool NiD3D10EffectLoader::ProcessD3D10EffectFile(
    const char* pcFilename, 
    const NiFixedString& kExt,
    NiTPointerList<NiFixedString>* pkFileList)
{
    if (!pcFilename || pcFilename[0] == '\0')
        return false;

    NiFilename kFilename(pcFilename);
    NIASSERT(kFilename.GetExt() != NULL && kFilename.GetExt()[0] == '.');
    if (kExt.EqualsNoCase(kFilename.GetExt() + 1))
    {
        NILOG("         Found %s File %s\n", kExt, pcFilename);

        // Add it to the list
        NiFixedString kNewFile(pcFilename);
        pkFileList->AddTail(pcFilename);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectLoader::LoadStoredD3D10EffectFiles(
    NiD3D10EffectShaderLibrary* pkLibrary)
{
    // Lock the renderer if necessary so that we can have unfettered 
    // access to Direct3D.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer)
        pkRenderer->LockRenderer();

    const char* pcFile = GetFirstD3D10EffectFile();
    while (pcFile)
    {
        NIASSERT(pkLibrary);

        // Parse the name and see if it's in the list.
        NiFilename kFilename(pcFile);
        if (pkLibrary->GetD3D10EffectFile(kFilename.GetFilename()))
        {
            NILOG(
                "%s> %s (%s) already loaded?\n", 
                __FUNCTION__,
                kFilename.GetFilename(), 
                pcFile);
        }
        else
        {
            char acFullPath[_MAX_PATH];
            if (!NiD3D10EffectFactory::ResolveD3D10EffectFileName(
                pcFile, acFullPath, _MAX_PATH))
            {
                // Can't resolve the effect!
                NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, false, 
                    "Failed to find D3D10 Effect file %s\n", pcFile);
                continue;
            }

            NiUInt32 uiLength = 0;
            char* pcShaderText = GetShaderText(pcFile, uiLength);

            NiFixedString kExtension = StandardizeExtension(acFullPath);

            // It's OK if the extension is missing for some reason - assume 
            // it's text
            bool bCompiled = false;
            m_kShaderFileExtensions.GetAt(kExtension, bCompiled);

            bool bSuccess = false;
            if (pcShaderText && uiLength > 0)
            {
                bSuccess = LoadEffect(
                    pkLibrary, 
                    pcShaderText, 
                    uiLength, 
                    bCompiled,
                    pcFile);
            }

            NiFree(pcShaderText);

        }
        pcFile = GetNextD3D10EffectFile();
    }

    if (pkRenderer)
        pkRenderer->UnlockRenderer();

    // Always return true, even if no files successfully loaded
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectLoader::LoadEffect(
    NiD3D10EffectShaderLibrary* pkLibrary, 
    void* pvBuffer, 
    NiUInt32 uiBufferSize, 
    bool bCompiled,
    const char* pcEffectFileName)
{
    if (pkLibrary->GetD3D10EffectFile(pcEffectFileName))
    {
        NILOG(
            "%s> Effect %s already loaded?\n", 
            __FUNCTION__,
            pcEffectFileName);
        return false;
    }
    else
    {
        NILOG("Attempting to load %s\n", pcEffectFileName);

        // Get the device to use - use the temp device if necessary.
        ID3D10Device* pkDevice = NULL;
        NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
        if (pkRenderer)
            pkDevice = pkRenderer->GetD3D10Device();
        else
            pkDevice = pkLibrary->GetTempDevice();
        NIASSERT(pkDevice);

        NiD3D10EffectFile* pkD3D10EffectFile = 
            NiD3D10EffectFactory::LoadD3D10EffectFromMemory(
            pkDevice,
            pcEffectFileName,
            uiBufferSize, 
            pvBuffer,
            bCompiled,
            pcEffectFileName);
        if (pkD3D10EffectFile)
        {
            NILOG("Loaded %s\n", pcEffectFileName);
            pkLibrary->InsertD3D10EffectFileIntoList(pkD3D10EffectFile);
            return true;
        }
        else
        {
            NILOG("Failed to load %s\n", pcEffectFileName);
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10EffectLoader::AddFileExtension(const char* pcExtension, 
    bool bCompiled)
{
    NiFixedString kExtension = StandardizeExtension(pcExtension);

    m_kShaderFileExtensions.SetAt(kExtension, bCompiled);
}
//---------------------------------------------------------------------------
void NiD3D10EffectLoader::RemoveFileExtension(const char* pcExtension)
{
    NiFixedString kExtension = StandardizeExtension(pcExtension);

    m_kShaderFileExtensions.RemoveAt(kExtension);
}
//---------------------------------------------------------------------------
char* NiD3D10EffectLoader::GetShaderText(const char* pcShaderName, 
    NiUInt32& uiLength)
{
    // Open a file. We'll use the default buffer of 32KB because the odds
    // of FX code being longer than that are fairly low.
    NiFile* pkFile = NiFile::GetFile(pcShaderName, NiFile::READ_ONLY);

    // Test that the file allocated and that it opened. The Boolean operator
    // on an NiFile object returns the m_bGood flag from that object.
    if (!pkFile || !*pkFile)
    {
        uiLength = 0;
        NiDelete pkFile;
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
NiFixedString NiD3D10EffectLoader::StandardizeExtension(const char* pcFilename)
{
    if (pcFilename == NULL)
        return NiFixedString("");

    // Search for a dot - we don't want to include that.
    const char* pcExtension = strrchr(pcFilename, '.');
    if (pcExtension == NULL)
        pcExtension = pcFilename;
    else
        pcExtension++;

    char acLowerCaseExtension[_MAX_PATH];

    for (NiUInt32 i = 0; i < _MAX_PATH; i++)
    {
        const char cChar = *pcExtension++;
        acLowerCaseExtension[i] = (char)tolower(cChar);
        if (cChar == '\0')
            break;
    }

    return NiFixedString(acLowerCaseExtension);
}
//---------------------------------------------------------------------------
