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
#include "NSFParserLibPCH.h"

#include "NSFLoader.h"
#include "NiD3DUtility.h"
//---------------------------------------------------------------------------
unsigned int NSFLoader::LoadAllNSFFilesInDirectory(
    const char* pszDirectory, const char* pszExt, bool bRecurseDirectories,
    NiTPointerList<char*>* pkFileList)
{
    if (!pszDirectory || (strcmp(pszDirectory, "") == 0))
        return 0;
    if (!pszExt || (strcmp(pszExt, "") == 0))
        return 0;

    unsigned int uiCount    = 0;
    char szFilePath[_MAX_PATH];

    NiStrncpy(szFilePath, _MAX_PATH, pszDirectory, _MAX_PATH - 1);
    size_t stLen = strlen(szFilePath);
    if ((szFilePath[stLen - 1] != '\\') && (szFilePath[stLen - 1] != '/'))
    {
        szFilePath[stLen] = '\\';
        szFilePath[stLen + 1] = 0;
    }

    NiPath::Standardize(szFilePath);

    WIN32_FIND_DATA wfd ;
    HANDLE hFile = NULL;
    char szFileName[_MAX_PATH];
    char szFileName2[_MAX_PATH];
    DWORD dwAttrib;
    bool bDone = false;

    memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

    NiStrcpy(szFileName, _MAX_PATH, pszDirectory);
    stLen = strlen(szFileName);
    if ((szFileName[stLen - 1] != '\\') && (szFileName[stLen - 1] != '/'))
    {
        szFileName[stLen] = '\\';
        szFileName[stLen + 1] = 0;
    }
    // This will cover the case when the directory is a mapped network
    // drive...
    NiStrcat(szFileName, _MAX_PATH, "*");

    NiPath::Standardize(szFileName);

    hFile = FindFirstFile(szFileName, &wfd);
    if (INVALID_HANDLE_VALUE != hFile)
    {
        NiStrcpy(szFileName2, _MAX_PATH, szFilePath);
        NiStrcat(szFileName2, _MAX_PATH, wfd.cFileName);

        NiPath::Standardize(szFileName2);
        while (!bDone)
        {
            NiStrcpy(szFileName2, _MAX_PATH, szFilePath);
            NiStrcat(szFileName2, _MAX_PATH, wfd.cFileName);

            if (hFile == INVALID_HANDLE_VALUE)
            {
                GetLastError();
                NiD3DUtility::Log("Invalid handle on FindXXXXXFile\n");
                bDone = true;
            }
            else
            {
                dwAttrib = GetFileAttributes(szFileName2);
                if ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    if (strcmp(wfd.cFileName, "."))
                    {
                        if (strcmp(wfd.cFileName, ".."))
                        {
                            // If we are recursing... do it
                            if (bRecurseDirectories)
                            {
                                NiStrcat(szFileName2, _MAX_PATH, "\\");
                                NiD3DUtility::Log("    Recurse directory %s\n",
                                    szFileName2);
                                uiCount += LoadAllNSFFilesInDirectory(
                                    szFileName2, pszExt, bRecurseDirectories,
                                    pkFileList);
                            }
                        }
                    }
                }
                else
                {
                    if (ProcessNSFFile(szFileName2, pszExt, pkFileList))
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
