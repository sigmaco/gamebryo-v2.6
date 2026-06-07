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

#include "NiPath.h"
#include <direct.h>

//---------------------------------------------------------------------------
const char* NiPath::StripAbsoluteBase(const char* pcAbsolutePath)
{
    NIASSERT(pcAbsolutePath && !NiPath::IsRelative(pcAbsolutePath));
    
    // Absolute base is one of the following:
    if (NiStrnicmp(pcAbsolutePath, "\\\\", 2) == 0)
    {
        // [1] Double-slash + network path + slash
        pcAbsolutePath = strchr(pcAbsolutePath+2, NI_PATH_DELIMITER_CHAR);
        NIASSERT(pcAbsolutePath);
        pcAbsolutePath++;
    }
    else if (NiStrnicmp(pcAbsolutePath, "\\", 1) == 0)
    {
        // [2] a single "\" can be followed by a file name.
        pcAbsolutePath++;
    }
    else 
    {
        // [3] A drive letter followed by ":" followed [optionally] by a slash
        NIASSERT(strlen(pcAbsolutePath) >= 2);
        char c1stChar = static_cast<char>(toupper(pcAbsolutePath[0]));
        char c2ndChar = pcAbsolutePath[1];
        if (c2ndChar == ':' && (c1stChar >= 'A' && c1stChar <= 'Z'))
        {
            pcAbsolutePath += 2;
            if (pcAbsolutePath[0] == NI_PATH_DELIMITER_CHAR)
                pcAbsolutePath++;
        }
        else
        {
            NIASSERT(!"NiPath::StripAbsoluteBase error.");
        }
    }

    return pcAbsolutePath;
}
//---------------------------------------------------------------------------
bool NiPath::IsRelative(const char *pcPath)
{
    // An absolute path is one that begins with either of the following:
    // [1] a forward or backward slash
    // [2] A drive letter followed by ":"

    size_t stLen = strlen(pcPath);
    if (stLen < 2)
    {
        // the smallest absolute path is slash followed by letter, so
        // this must be a relative path.
        return true;
    }

    // check for case 1
    char c1stChar = pcPath[0];   
    if (c1stChar == '\\' || c1stChar == '/')
    {
        // test for case 1 indicates this is an absolute path
        return false;
    }

    // check for case 2
    c1stChar = static_cast<char>(toupper(c1stChar));
    char c2ndChar = pcPath[1];
    if (c2ndChar == ':' && (c1stChar >= 'A' && c1stChar <= 'Z'))
    {   
        // test for case 2 indicates this is an absolute path
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPath::GetCurrentWorkingDirectory(char* pcPath, size_t stDestSize)
{
    return (_getcwd(pcPath, (int)stDestSize) != NULL);
}
//---------------------------------------------------------------------------
bool NiPath::GetExecutableDirectory(char* pcPath, size_t stDestSize)
{
    size_t stWrittenChars = GetModuleFileName(GetModuleHandle(NULL), pcPath,
        (DWORD)stDestSize);

    if (stWrittenChars != NULL && stWrittenChars != stDestSize)
    {
        char* pcLastDirSlash = strrchr(pcPath, '\\');
        if (pcLastDirSlash == NULL)
            pcLastDirSlash = strrchr(pcPath, '/');

        if (pcLastDirSlash)
        {
            pcLastDirSlash[1] = '\0';
            Standardize(pcPath);
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
