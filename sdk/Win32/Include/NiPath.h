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

#ifndef NIPATH_H
#define NIPATH_H

#include "NiSystem.h"
#include "NiSystemLibType.h"

//---------------------------------------------------------------------------

#if defined(_MAX_PATH)
#define NI_MAX_PATH _MAX_PATH
#else //#if defined(_MAX_PATH)
#define NI_MAX_PATH 260
#endif //#if defined(_MAX_PATH)

//---------------------------------------------------------------------------

class NISYSTEM_ENTRY NiPath
{
public:
    static bool IsRelative(const char *pcPath);
    static bool IsUniqueAbsolute(const char *pcPath);

    static size_t ConvertToRelative(
        char* pcRelativePath, 
        size_t stRelBytes, 
        const char* pcAbsolutePath, 
        const char* pcRelativeToHere);

    static size_t ConvertToAbsolute(
        char* pcPath, 
        size_t stBytes, 
        const char* pcRelativeToHere);

    static size_t ConvertToAbsolute(
        char* pcPath, 
        size_t stBytes);

    static size_t ConvertToAbsolute(
        char* pcAbsolutePath,
        size_t stBytes, 
        const char* pcRelativePath, 
        const char* pcRelativeToHere);

    static void RemoveSlashDotSlash(char* pcPath);
    static void RemoveDotDots(char* pcPath);
    static bool GetCurrentWorkingDirectory(char* pcPath, 
        size_t stDestSize);
    static bool GetExecutableDirectory(char* pcPath,
        size_t stDestSize);
    static bool Standardize(char* pcPath);

    // Note: this function should only be used with filenames, not entire
    // paths. It detects '/' and '\\' characters as invalid.
    static void ReplaceInvalidFilenameCharacters(char* pcFilename,
        char cReplacement);

protected:
    static const char* StripAbsoluteBase(const char* pcAbsolutePath);
    static char* StripAbsoluteBase(char* pcAbsolutePath);

#ifdef _PS3
public:
    static bool SetCWDandEWDfromArgV0(const char* pcArgV0);
    static void SetExecutableDirectory(const char* pcPath);
    static void SetCurrentWorkingDirectory(const char* pcPath);
protected:
    static char ms_acExecutableDirectory[NI_MAX_PATH];
    static char ms_acCurrentWorkingDirectory[NI_MAX_PATH];
#endif
};

//---------------------------------------------------------------------------
inline char* NiPath::StripAbsoluteBase(char* pcAbsolutePath)
{
    return const_cast<char*>(
        StripAbsoluteBase(const_cast<const char*>(pcAbsolutePath)));
}
//---------------------------------------------------------------------------

#ifdef _PS3
#define NI_PATH_DELIMITER_STR "/"
#define NI_PATH_DELIMITER_CHAR '/'
#define NI_PATH_DELIMITER_INCORRECT_STR "\\"
#define NI_PATH_DELIMITER_INCORRECT_CHAR '\\'
#elif defined(_WII)
#define NI_PATH_DELIMITER_STR "/"
#define NI_PATH_DELIMITER_CHAR '/'
#define NI_PATH_DELIMITER_INCORRECT_STR "\\"
#define NI_PATH_DELIMITER_INCORRECT_CHAR '\\'
#else
#define NI_PATH_DELIMITER_STR "\\"
#define NI_PATH_DELIMITER_CHAR '\\'
#define NI_PATH_DELIMITER_INCORRECT_STR "/"
#define NI_PATH_DELIMITER_INCORRECT_CHAR '/'
#endif

//---------------------------------------------------------------------------

#endif
