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

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include "NiFileFinder.h"
#include "NiString.h"
#include <NiSystem.h>

//---------------------------------------------------------------------------
NiFileFinder::NiFileFinder(const char* pcPath, bool bRecurseSubDirs, 
                           const char* pcExtension) : m_kFoundFiles(50,50)
{
    Find(pcPath, bRecurseSubDirs, pcExtension);
    ResetIterator();
}

//---------------------------------------------------------------------------
NiFoundFile* NiFileFinder::GetNextFile()
{
    if (HasMoreFiles())
    {
        NiFoundFile* pkFile = m_kFoundFiles.GetAt(m_uiIterator);
        m_uiIterator++;
        return pkFile;
    }
    else
    {
        return NULL;
    }
}

//---------------------------------------------------------------------------
NiFileFinder::~NiFileFinder()
{
    ResetIterator();
    unsigned int uiSize = m_kFoundFiles.GetEffectiveSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiFoundFile* pkFile = m_kFoundFiles.RemoveAt(ui);
        NiDelete pkFile;
    }
}

//---------------------------------------------------------------------------
void NiFileFinder::AddFile(const char* pcPath, unsigned int uiSize)
{
    NiFoundFile* pkFile = NiNew NiFoundFile;
    pkFile->m_strPath = pcPath;
    pkFile->m_uiSize = uiSize;
    m_kFoundFiles.AddFirstEmpty(pkFile);
}

//---------------------------------------------------------------------------
bool NiFileFinder::HasMoreFiles()
{
    if (m_uiIterator < m_kFoundFiles.GetEffectiveSize())
        return true;
    else
        return false;
}

//---------------------------------------------------------------------------
unsigned int NiFileFinder::GetFileCount()
{
    return m_kFoundFiles.GetEffectiveSize();
}

//---------------------------------------------------------------------------
void NiFileFinder::ResetIterator()
{
    m_uiIterator = 0;
}

//---------------------------------------------------------------------------
void NiFileFinder::Find(NiString strPath, bool bRecurseSubDirs, 
    const char* pcExtension)
{
    bool bIsFile = false;
    int iFind = strPath.FindReverse('\\');
    int iFindStar = strPath.FindReverse('.');
    if (iFindStar > iFind)
        bIsFile = true;

    NiString strFilePath((int) strPath.Length());

    // If we have a directory as a path, add in the search terms
    if ((unsigned int) iFind == strPath.Length()-1)
    {
        strFilePath = strPath;
        strPath += "*";
    }
    else if (!bIsFile)
    {
        strFilePath = strPath + "\\";
        strPath += "\\*";
    }

    LPWIN32_FIND_DATA lpFindFileData = NiExternalNew WIN32_FIND_DATA;

    HANDLE hFindHandle = FindFirstFile(strPath, lpFindFileData);
    
    if(hFindHandle == INVALID_HANDLE_VALUE)
    {
        //NiMessageBox("Unable to find file(s)", "Found File Error");
        return;
    }
    if(lpFindFileData->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
    {
        NiString strFile(lpFindFileData->cFileName);
        int iExtLength = (int)strlen(pcExtension);
        NiString strExt = strFile.GetSubstring(strFile.Length() - iExtLength, 
            strFile.Length());

        if (strExt.EqualsNoCase(pcExtension))
        {
            unsigned int uiSize = (lpFindFileData->nFileSizeHigh * 
                MAXDWORD) + lpFindFileData->nFileSizeLow;
            AddFile(strFilePath + strFile, uiSize);
        }
    }
    else if(bRecurseSubDirs && lpFindFileData->cFileName[0] != '.' &&
        lpFindFileData->cFileName[1] != '.')
    {
        Find(strFilePath + lpFindFileData->cFileName + '\\', bRecurseSubDirs, 
            pcExtension);
    }

    while(FindNextFile(hFindHandle, lpFindFileData))
    {
        if(lpFindFileData->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
            NiString strFile(lpFindFileData->cFileName);
            int iExtLength = (int)strlen(pcExtension);
            NiString strExt = strFile.GetSubstring(strFile.Length() - 
                iExtLength, strFile.Length());

            if (strExt.EqualsNoCase(pcExtension))
            {
                unsigned int uiSize = (lpFindFileData->nFileSizeHigh * 
                    MAXDWORD) + lpFindFileData->nFileSizeLow;
                AddFile(strFilePath + strFile, uiSize);
            }
        }
        else if(bRecurseSubDirs && lpFindFileData->cFileName[0] != '.' &&
            lpFindFileData->cFileName[1] != '.')
        {
            Find(strFilePath + lpFindFileData->cFileName + '\\', 
                bRecurseSubDirs, pcExtension);
        }
    }

    FindClose(hFindHandle);
    NiExternalDelete lpFindFileData;    
}
//---------------------------------------------------------------------------
