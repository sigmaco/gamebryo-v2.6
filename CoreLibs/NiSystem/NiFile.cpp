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
#include "NiSystemPCH.h"

#include "NiSystem.h"
#include "NiFile.h"

NiFile::FILECREATEFUNC NiFile::ms_pfnFileCreateFunc = 
    NiFile::DefaultFileCreateFunc;

NiFile::FILEACCESSFUNC NiFile::ms_pfnFileAccessFunc =
    NiFile::DefaultFileAccessFunc;
    
NiFile::CREATEDIRFUNC NiFile::ms_pfnCreateDirFunc =
    NiFile::DefaultCreateDirectoryFunc;

NiFile::DIREXISTSFUNC NiFile::ms_pfnDirExistsFunc =
    NiFile::DefaultDirectoryExistsFunc;

NiImplementDerivedBinaryStream(NiFile, FileRead, FileWrite);

//---------------------------------------------------------------------------
NiFile* NiFile::GetFile(const char *pcName, OpenMode eMode,
    unsigned int uiSize)
{
    return ms_pfnFileCreateFunc(pcName, eMode, uiSize);
}

//---------------------------------------------------------------------------
NiFile* NiFile::DefaultFileCreateFunc(const char *pcName, OpenMode eMode,
    unsigned int uiSize)
{
    return NiNew NiFile(pcName, eMode, uiSize);
}

//---------------------------------------------------------------------------
void NiFile::SetFileCreateFunc(FILECREATEFUNC pfnFunc)
{
    ms_pfnFileCreateFunc = (pfnFunc == NULL) ? DefaultFileCreateFunc : pfnFunc;
}

//---------------------------------------------------------------------------
bool NiFile::Access(const char *pcName, OpenMode eMode)
{
    return ms_pfnFileAccessFunc(pcName, eMode);
}

//---------------------------------------------------------------------------
bool NiFile::DefaultFileAccessFunc(const char *pcName, OpenMode eMode)
{
    NiFile kFile(pcName, eMode, 0);
    return kFile.m_bGood;
}

//---------------------------------------------------------------------------
void NiFile::SetFileAccessFunc(FILEACCESSFUNC pfnFunc)
{
    ms_pfnFileAccessFunc = (pfnFunc == NULL) ? DefaultFileAccessFunc : pfnFunc;
}

//---------------------------------------------------------------------------
NiFile::operator bool() const
{
    return m_bGood;
}
//---------------------------------------------------------------------------
void NiFile::Seek(int iNumBytes)
{
    Seek(iNumBytes, ms_iSeekCur);
}
//---------------------------------------------------------------------------
bool NiFile::CreateDirectory(const char* pcDirName)
{
    return ms_pfnCreateDirFunc(pcDirName);
}
//---------------------------------------------------------------------------
void NiFile::SetCreateDirectoryFunc(CREATEDIRFUNC pfnFunc)
{
    ms_pfnCreateDirFunc = (pfnFunc == NULL) ? DefaultCreateDirectoryFunc : 
        pfnFunc;
}
//---------------------------------------------------------------------------
bool NiFile::DirectoryExists(const char* pcDirName)
{
    return ms_pfnDirExistsFunc(pcDirName);
}
//---------------------------------------------------------------------------
void NiFile::SetDirectoryExistsFunc(DIREXISTSFUNC pfnFunc)
{
    ms_pfnDirExistsFunc = (pfnFunc == NULL) ? DefaultDirectoryExistsFunc : 
        pfnFunc;
}
//---------------------------------------------------------------------------
bool NiFile::CreateDirectoryRecursive(const char* pcFullPath)
{
    if (DirectoryExists(pcFullPath))
        return true;

    if (strlen(pcFullPath) > NI_MAX_PATH)
        return false;

    char acFullPathCopy[NI_MAX_PATH];
    NiStrcpy(acFullPathCopy, NI_MAX_PATH, pcFullPath);
    NiPath::Standardize(acFullPathCopy);

    unsigned int uiStart = 0;

    // Check for drive start path
    if (acFullPathCopy[uiStart + 1] == ':')
    {
        uiStart += 2;
    }
  
    // Consume the leading slash characters
    while (uiStart < NI_MAX_PATH && 
        (acFullPathCopy[uiStart] == '\\' || acFullPathCopy[uiStart] == '/'))
    {
        uiStart++;
    }

    // Search through the string buffer for any '\\' or '/' and 
    // make sure that the directory exists. If not, create it.
    bool bDealtWithNetworkPath = false;
    for (unsigned int ui = uiStart; ui < NI_MAX_PATH; ui++)
    {
        char cCurChar = acFullPathCopy[ui];
        if ((cCurChar == '/' || cCurChar == '\\')
#ifdef _PS3
            // handle paths such as /app_home/c:/foo/bar 
            // We will skip this code block if the current and last char
            // are ":/" or "//"
            // that will save us from attempting /app_home/c:
            //
            && !(ui == 0 || acFullPathCopy[ui-1] == ':' || 
            acFullPathCopy[ui-1] == '/' || acFullPathCopy[ui+1] == '/')
#endif
            )
        {
            acFullPathCopy[ui] = '\0';

            if (uiStart == 2 && bDealtWithNetworkPath == false &&
                acFullPathCopy[0] == '\\' && acFullPathCopy[1] == '\\')
            {
                // A network path such as "\\CPU1" would fail the 
                // DirectoryExists test below since no directory is
                // technically being specified. That case is detected by the
                // condition above and the DirectoryExists test is skipped 
                // because it is a network path only.
                bDealtWithNetworkPath = true;
            }
            else if (!DirectoryExists(acFullPathCopy))
            {
                if (!CreateDirectory(acFullPathCopy))
                    return false;
                NIASSERT(DirectoryExists(acFullPathCopy));
            }
            acFullPathCopy[ui] = cCurChar;
        }
    }

    // Assume that the last characters of the array may define a directory as 
    // well even though the string was not necessarily
    // terminated with a seperator.
    if (!DirectoryExists(acFullPathCopy))
    {
        if (!CreateDirectory(acFullPathCopy))
            return false;
        NIASSERT(DirectoryExists(acFullPathCopy));
    }

    NIASSERT(DirectoryExists(pcFullPath));
    return true;
}
//---------------------------------------------------------------------------
