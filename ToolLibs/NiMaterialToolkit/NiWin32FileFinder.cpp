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

#include "NiWin32FileFinder.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiWin32FileFinder::NiWin32FileFinder(const char* strPath, bool bRecurseSubDirs,
    const char* pcExtension) : m_kFoundFiles(50,50)
{
    Find(strPath, bRecurseSubDirs, pcExtension);
    ResetIterator();
}
//---------------------------------------------------------------------------
NiWin32FoundFile* NiWin32FileFinder::GetNextFile()
{
    if (HasMoreFiles())
    {
        NiWin32FoundFile* pkFile = m_kFoundFiles.GetAt(m_uiIterator);
        m_uiIterator++;
        return pkFile;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiWin32FileFinder::~NiWin32FileFinder()
{
    ResetIterator();
    unsigned int uiSize = m_kFoundFiles.GetEffectiveSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiWin32FoundFile* pkFile = m_kFoundFiles.RemoveAt(ui);
        NiDelete pkFile;
    }
}
//---------------------------------------------------------------------------
void NiWin32FileFinder::AddFile(const char* strPath, unsigned int uiSize,
    FILETIME kFiletime)
{
    NiWin32FoundFile* pkFile = NiNew NiWin32FoundFile;
    NiStrncpy(pkFile->m_strPath, MAX_PATH, strPath, MAX_PATH-1);
    pkFile->m_uiSize = uiSize;
    pkFile->ftLastWriteTime = kFiletime;
    m_kFoundFiles.AddFirstEmpty(pkFile);
}
//---------------------------------------------------------------------------
bool NiWin32FileFinder::HasMoreFiles()
{
    if (m_uiIterator < m_kFoundFiles.GetEffectiveSize())
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
void NiWin32FileFinder::ResetIterator()
{
    m_uiIterator = 0;
}
//---------------------------------------------------------------------------
void NiWin32FileFinder::Find(const char* strOriginalPath, bool bRecurseSubDirs,
    const char* pcExtension)
{
    NIASSERT(strlen(strOriginalPath) < MAX_PATH);

    bool bIsFile = false;
    int iFind = (int)(strrchr(strOriginalPath, '\\') - strOriginalPath);
    int iFindStar = (int)(strrchr(strOriginalPath, '.') - strOriginalPath);
    if (iFindStar > iFind)
        bIsFile = true;

    char strFilePath[MAX_PATH];
    char strPath[MAX_PATH];
    memset(strFilePath, '\0', MAX_PATH);
    memset(strPath, '\0', MAX_PATH);
    NiStrcpy(strPath, MAX_PATH, strOriginalPath);

    // If we have a directory as a path, add in the search terms
    if ((unsigned int) iFind == strlen(strPath)-1)
    {
        NiStrcpy(strFilePath, MAX_PATH, strPath);
        NiStrcpy(strPath + strlen(strPath), MAX_PATH - strlen(strPath), "*");
    }
    else if (!bIsFile)
    {
        NiStrcpy(strFilePath, MAX_PATH, strPath);
        NiStrcat(strFilePath, MAX_PATH, "\\");
        NiStrcpy(strPath + strlen(strPath), MAX_PATH - strlen(strPath), "\\*");
    }

    LPWIN32_FIND_DATA lpFindFileData = NiExternalNew WIN32_FIND_DATA;

    HANDLE hFindHandle = FindFirstFile(strPath, lpFindFileData);
    
    if(hFindHandle == INVALID_HANDLE_VALUE)
    {
        NiExternalDelete lpFindFileData;    

//        NiMessageBox("Unable to find file(s)", "Found File Error");
        return;
    }
    if(lpFindFileData->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
    {
        char strFile[MAX_PATH];
        memset(strFile, '\0', MAX_PATH);
        NiStrcpy(strFile, MAX_PATH, lpFindFileData->cFileName);

        int iExtLength = (int)strlen(pcExtension);
        char strExt[MAX_PATH];
        memset(strExt, '\0', MAX_PATH);
        NiStrcpy(strExt, MAX_PATH, strFile + strlen(strFile) - iExtLength);

        if (NiStricmp(strExt, pcExtension) == 0)
        {
            unsigned int uiSize = (lpFindFileData->nFileSizeHigh * 
                MAXDWORD) + lpFindFileData->nFileSizeLow;
            
            char strFull[MAX_PATH];
            memset(strFull, '\0', MAX_PATH);
            NiStrcpy(strFull, MAX_PATH, strFilePath);
            NiStrcpy(strFull + strlen(strFilePath), 
                MAX_PATH - strlen(strFilePath), strFile);
            AddFile(strFull, uiSize, lpFindFileData->ftLastWriteTime);
        }
    }
    else if(bRecurseSubDirs && lpFindFileData->cFileName[0] != '.' &&
        lpFindFileData->cFileName[1] != '.')
    {
        char strFull[MAX_PATH];
        memset(strFull, '\0', MAX_PATH);
        NiStrcpy(strFull, MAX_PATH, strFilePath);
        NiStrcpy(strFull + strlen(strFilePath), 
            MAX_PATH - strlen(strFilePath), lpFindFileData->cFileName);
        NiStrcpy(strFull + strlen(strFull), MAX_PATH - strlen(strFull), "\\");
            
        Find(strFull, bRecurseSubDirs, pcExtension);
    }

    while(FindNextFile(hFindHandle, lpFindFileData))
    {
        if(lpFindFileData->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
            char strFile[MAX_PATH];
            memset(strFile, '\0', MAX_PATH);
            NiStrcpy(strFile, MAX_PATH, lpFindFileData->cFileName);
            int iExtLength = (int)strlen(pcExtension);
            char strExt[MAX_PATH];
            memset(strExt, '\0', MAX_PATH);
            int iLength = (int)strlen(strFile);
            NiStrncpy(strExt, MAX_PATH, strFile + iLength - iExtLength, 
                iLength);

            if (NiStricmp(strExt, pcExtension) == 0)
            {
                unsigned int uiSize = (lpFindFileData->nFileSizeHigh * 
                    MAXDWORD) + lpFindFileData->nFileSizeLow;
                char strFull[MAX_PATH];
                memset(strFull, '\0', MAX_PATH);
                NiStrcpy(strFull, MAX_PATH, strFilePath);
                NiStrcpy(strFull + strlen(strFilePath),
                    MAX_PATH - strlen(strFilePath), strFile);
                AddFile(strFull, uiSize,lpFindFileData->ftLastWriteTime);
            }
        }
        else if(bRecurseSubDirs && lpFindFileData->cFileName[0] != '.' &&
            lpFindFileData->cFileName[1] != '.')
        {

            char strFull[MAX_PATH];
            memset(strFull, '\0', MAX_PATH);
            NiStrcpy(strFull, MAX_PATH, strFilePath);
            NiStrcpy(strFull + strlen(strFilePath), 
                MAX_PATH - strlen(strFilePath), lpFindFileData->cFileName);
            NiStrcpy(strFull + strlen(strFull), MAX_PATH - strlen(strFull), 
                "\\");
        
            Find(strFull, bRecurseSubDirs, pcExtension);
        }
    }

    FindClose(hFindHandle);
    NiExternalDelete lpFindFileData;    
}
//---------------------------------------------------------------------------
