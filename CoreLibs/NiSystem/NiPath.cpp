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

//---------------------------------------------------------------------------
bool NiPath::IsUniqueAbsolute(const char *pcPath)
{
    if (IsRelative(pcPath))
        return false;
    
    // strip off absolute base
    pcPath = StripAbsoluteBase(pcPath);

    // After absolute base, are there any occurrences of dotdots?
    const char* pcRelative = strstr(pcPath, "..");
    if (pcRelative)
        return false;

    NIASSERT(strlen(pcPath) + 1 < NI_MAX_PATH);
    char acTestStandardize[NI_MAX_PATH];
    NiStrcpy(acTestStandardize, NI_MAX_PATH, pcPath);
    bool bStandardized = NiPath::Standardize(acTestStandardize);

    return (!bStandardized);
}
//---------------------------------------------------------------------------
size_t NiPath::ConvertToRelative(char* pcRelativePath, size_t stRelBytes,
    const char* pcAbsolutePath, const char* pcRelativeToHere)
{
    NIASSERT(strlen(pcAbsolutePath) + 1 < NI_MAX_PATH);
    NIASSERT(strlen(pcRelativeToHere) + 1 < NI_MAX_PATH);

    // This function takes pcAbsolutePath and converts it to an relative path.
    // The relative path is stored in pcRelativePath which is assumed to be
    // allocated with stRelBytes. The function returns the number of bytes
    // written.
    //
    // The function fails if:
    // - acRelativePath is too small (stRelBytes) to hold acAbsolutePath
    // - acAbsolutePath is larger than NI_MAX_PATH
    // - acAbsolutePath and acRelativeToHere do not have a common root

    NIASSERT(pcAbsolutePath && !IsRelative(pcAbsolutePath));
    NIASSERT(pcRelativeToHere && !IsRelative(pcRelativeToHere));

    size_t stAbsBytes = strlen(pcAbsolutePath);
    if (stRelBytes <= stAbsBytes)
        return 0;
    if (stAbsBytes >= NI_MAX_PATH)
        return 0;

    // First, remove dotdots and standardize paths...
    char acAbsolutePath[NI_MAX_PATH];
    char acRelativeToHere[NI_MAX_PATH];
    NiStrcpy(acAbsolutePath, NI_MAX_PATH, pcAbsolutePath);
    NiStrcpy(acRelativeToHere, NI_MAX_PATH, pcRelativeToHere);

    // acRelativeToHere is a directory. If there is no ending delimiter,
    // one is added. This is done for the case where acRelativeToHere IS
    // the absolute base and we want to ensure that our comparison of 
    // dtRoot1Size against dtRoot2Size holds true. 
    size_t stEndChar = strlen(acRelativeToHere) - 1;
    NIASSERT(stEndChar < NI_MAX_PATH);
    if (acRelativeToHere[stEndChar] != NI_PATH_DELIMITER_CHAR)
    {
        NiStrcat(acRelativeToHere, NI_MAX_PATH, NI_PATH_DELIMITER_STR);
    }

    NiPath::RemoveDotDots(acAbsolutePath);
    NiPath::RemoveDotDots(acRelativeToHere);

    // From here on out, only the local copies are used.

    // search for a common root 
    const char* pcAbsoluteNoRoot = StripAbsoluteBase(acAbsolutePath);
    const char* pcRelativeNoRoot = StripAbsoluteBase(acRelativeToHere);
    ptrdiff_t dtRoot1Size = pcAbsoluteNoRoot - acAbsolutePath;
    ptrdiff_t dtRoot2Size = pcRelativeNoRoot - acRelativeToHere;

    if (dtRoot1Size != dtRoot2Size)
        return 0;

    if (NiStrnicmp(acAbsolutePath, acRelativeToHere, dtRoot1Size) != 0)
        return 0;

    // common root found, now construct the relative path 
    // always start with "./"
    // acSubString contains the common portion in both 
    // absolute & relative strings
    char acSubString[NI_MAX_PATH];
    NiStrcpy(pcRelativePath, stRelBytes, "." NI_PATH_DELIMITER_STR);
    NiStrcpy(acSubString, NI_MAX_PATH, pcRelativeNoRoot);
    
    // Remove trailing delimiter there is one
    size_t stLenOfCommon = strlen(acSubString);
    if (stLenOfCommon >= 1 && 
        acSubString[stLenOfCommon-1] == NI_PATH_DELIMITER_CHAR)
    {
        acSubString[stLenOfCommon-1] = '\0';
    }

    for( ; ; )
    {
        stLenOfCommon = strlen(acSubString);
        // Does pcAbsoluteNoRoot start with acSubString?  If so, done.
        if (NiStrnicmp(pcAbsoluteNoRoot, acSubString, stLenOfCommon) == 0)
            break;

        // append a ../ to relative path
        NiStrcat(pcRelativePath, stRelBytes, ".." NI_PATH_DELIMITER_STR);

        // remove this subdir from acSubString
        char* pcEndOfNextSubDir = strrchr(acSubString, NI_PATH_DELIMITER_CHAR);
        if (pcEndOfNextSubDir)
        {
            *pcEndOfNextSubDir = '\0'; 
        }
        else
        {
            // no more subdirs to remove, done.
            acSubString[0] = '\0';
            break;
        }
    }

    // append the remaining path and filename
    stLenOfCommon = strlen(acSubString);
    if (pcAbsoluteNoRoot[stLenOfCommon] == NI_PATH_DELIMITER_CHAR)
    {
        // skip leading slash if there is one
        stLenOfCommon++;
    }
    NiStrcat(pcRelativePath, stRelBytes, &pcAbsoluteNoRoot[stLenOfCommon]);

    return strlen(pcRelativePath);
}
//---------------------------------------------------------------------------
size_t NiPath::ConvertToAbsolute(char* pcPath, size_t stPathBytes,
    const char* pcRelativeToHere)
{
    // This ConvertToAbsolute modifies pcPath in place. 
    const size_t stMaximumPathBufferSize = NI_MAX_PATH * 2 + 2;
    char acAbsPath[stMaximumPathBufferSize];
    size_t uiAbsBytes = ConvertToAbsolute(acAbsPath, stMaximumPathBufferSize, 
        pcPath, pcRelativeToHere);

    if (uiAbsBytes < stPathBytes)
    {
        NiStrcpy(pcPath, stPathBytes, acAbsPath);
        return uiAbsBytes;
    }
    else
    {
        pcPath[0] = NULL;
        return 0;
    }

}
//---------------------------------------------------------------------------
size_t NiPath::ConvertToAbsolute(char* pcPath, size_t stPathBytes)
{
    char acCWD[NI_MAX_PATH];
    if (!GetCurrentWorkingDirectory(acCWD, NI_MAX_PATH))
        return 0;
    return ConvertToAbsolute(pcPath, stPathBytes, acCWD);
}
//---------------------------------------------------------------------------
size_t NiPath::ConvertToAbsolute(char* pcAbsolutePath, size_t stAbsBytes,
    const char* pcRelativePath, const char* pcRelativeToHere)
{
    // This function takes pcRelativePath and converts it to an absolute path 
    // by concatenating it with pcRelativeToHere and removing dotdots.
    
    // The resulting absolute path is stored in pcAbsolutePath, which is 
    // assumed to have been allocated with size stAbsBytes. 
    // The function returns the number of bytes written.

    NIASSERT(pcAbsolutePath && pcRelativePath);
    NIASSERT(IsRelative(pcRelativePath));
    NIASSERT(pcAbsolutePath != pcRelativePath); // destination cannot be source

    // If pcRelativeToHere is null or an empty string, use the current working
    // directory.
    if (!pcRelativeToHere)
    {
        pcRelativeToHere = "";
    }
    size_t stLenRelativeToHere = strlen(pcRelativeToHere);
    if (stLenRelativeToHere == 0)
    {
        char acCWD[NI_MAX_PATH];
        if (!GetCurrentWorkingDirectory(acCWD, NI_MAX_PATH))
        {
            if (stAbsBytes > 0)
                pcAbsolutePath[0] = NULL;
            return 0;
        }
        NIASSERT(strlen(acCWD) != 0);
        return ConvertToAbsolute(pcAbsolutePath, stAbsBytes, pcRelativePath,
            acCWD);
    }

    size_t stLenRelativePath = strlen(pcRelativePath);

    // Concatenate a delimiter if necessary
    bool bInsertDelimiter = 
        (pcRelativeToHere[stLenRelativeToHere-1] != NI_PATH_DELIMITER_CHAR);

    size_t stRequiredSize = 1 // null termination
        + stLenRelativeToHere 
        + stLenRelativePath
        + ((bInsertDelimiter) ? 1 : 0);

    // Fail if not enough storage
    if (stAbsBytes < stRequiredSize)
    {
        if (stAbsBytes > 0)
            pcAbsolutePath[0] = NULL;
        return 0;
    }

    // Store pcRelativeToHere into pcAbsolutePath
    NiStrcpy(pcAbsolutePath, stAbsBytes, pcRelativeToHere);

    // Concatenate a delimiter if necessary
    if (bInsertDelimiter)
        NiStrcat(pcAbsolutePath, stAbsBytes, NI_PATH_DELIMITER_STR);

    // Concatenate pcRelativePath
    NiStrcat(pcAbsolutePath, stAbsBytes, pcRelativePath);

    RemoveDotDots(pcAbsolutePath);
    return strlen(pcAbsolutePath);
}
//---------------------------------------------------------------------------
void NiPath::RemoveSlashDotSlash(char* pcPath)
{
    // Remove any "/./" that remain in pcPath
    const char acSlashDotSlash[] = 
        NI_PATH_DELIMITER_STR "." NI_PATH_DELIMITER_STR;

    char* pcNext = strstr(pcPath, acSlashDotSlash);
    while(pcNext)
    {
        size_t uiLen = strlen(pcNext);
        const char* pcAfterDotSlash = pcNext + 2;
        NiStrcpy(pcNext, uiLen, pcAfterDotSlash);
        pcNext = strstr(pcPath, acSlashDotSlash);
    }
}
//---------------------------------------------------------------------------
void NiPath::RemoveDotDots(char* pcPath)
{
    // Ensure that path is standardized first
    NiPath::Standardize(pcPath);

    RemoveSlashDotSlash(pcPath);

    // This function collapses any "../" paths not found at the start
    // of the string.  i.e. "../../foo/quux/../bar" -> "../../foo/bar".
    // It assumes that all slashes are backslashes and that there are no
    // sequential backslashes.

    char* pcPtr;
    if (IsRelative(pcPath))
    {
        // Find the first non-dot/slash character in case the string
        // has a ../ path at the beginning.  This will be the first directory.
        pcPtr = pcPath;
        while (pcPtr && (*pcPtr == '.' || *pcPtr == NI_PATH_DELIMITER_CHAR))
        {
            pcPtr++;
        }
    }
    else
    {
        pcPtr = StripAbsoluteBase(pcPath);
    }

    // If the string consists of only dots and slashes, then nothing to do.
    if (!pcPtr)
        return;

    // pointer to past the end of the string
    char* pcEndPlusOne = pcPtr + strlen(pcPtr) + 1;
    const char acSlashDotDot[] = NI_PATH_DELIMITER_STR "..";

    char* pcNextDir = strstr(pcPtr, acSlashDotDot);
    while(pcNextDir)
    {
        // Found a /.. in the string

        // Because we're going to remove the /.., we can zero
        // out the beginning as a temporary end of string.
        *pcNextDir = 0;

        // Find the directory before the temporary end.
        char* pcLastDir = strrchr(pcPtr, NI_PATH_DELIMITER_CHAR);

        // Advance pcNextDir past the /..
        if (pcLastDir)
        {
            // If we found a slash, then pcLastDir points to a slash
            // advance pcNextDir forward to point to after the
            // /.., which will be a slash or a /0.
            pcNextDir += 3;   
        }
        else
        {
            // If we didn't find a slash, then pcLastDir needs to be
            // set to the start of the string.  In this case,
            // pcNextDir will be: (1) "/../dir", (2) "/../0" or
            // (3) "/../0".
            if (pcNextDir + 3)
            {
                // Case 1, Case 2
                pcNextDir += 4;
            }
            else
            {
                // Case 3
                pcNextDir += 3;
            }
            pcLastDir = pcPtr;
        }

        // Now move everything at pcNextDir to the end of the string
        // backwards onto pcLastDir.
        for (int i = 0; i < pcEndPlusOne - pcNextDir; i++)
        {
            pcLastDir[i] = pcNextDir[i];
        }

        // Find the first non-dot/slash character in case the string
        // has a ../ path at the beginning. This will be the first directory.
        while (pcPtr && (*pcPtr == '.' || *pcPtr == NI_PATH_DELIMITER_CHAR))
        {
              ++pcPtr;
        }

        pcNextDir = strstr(pcPtr, acSlashDotDot);
    }
}
//---------------------------------------------------------------------------
// Convert a file path from Windows/Unix to the specific platform's format. 
bool NiPath::Standardize(char* pcPath)
{
    bool bModified = false;
    if (!pcPath)
        return bModified;

    size_t stLen = strlen(pcPath);
    bool bLastCharWasSlash = false;

    unsigned int uiWrite = 0;
    for (unsigned int uiRead = 0; uiRead < stLen; uiRead++)
    {
        char c = pcPath[uiRead];

        //  convert NI_PATH_DELIMITER_INCORRECT_CHAR to NI_PATH_DELIMITER_CHAR
        if (c == NI_PATH_DELIMITER_INCORRECT_CHAR)
        {
            bModified = true;
            c = NI_PATH_DELIMITER_CHAR;
        }

        bool bThisCharIsSlash = (c == NI_PATH_DELIMITER_CHAR);

        // Drop duplicate slashes
        if (bThisCharIsSlash && bLastCharWasSlash) 
        {
            // Unless this is a network resource name, e.g. "\\share"
            // which is true only in first two characters of string.
            if (uiRead > 1)
            {
                bModified = true;
                continue; // skip this char
            }
        }            

        bLastCharWasSlash = bThisCharIsSlash;

        // write char
        pcPath[uiWrite++] = c;
    }

    // Terminate (string may have shrunk)
    pcPath[uiWrite] = NULL;
    return bModified;
}
//---------------------------------------------------------------------------
void NiPath::ReplaceInvalidFilenameCharacters(char* pcFilename,
    char cReplacement)
{
    NIASSERT(pcFilename);

    size_t stLen = strlen(pcFilename);
    for (unsigned int uiChar = 0; uiChar < stLen; uiChar++)
    {
        // Detect illegal filename characters. This will detect '/' and '\\'
        // characters, so this function should not be used on full paths, only
        // filenames.
        switch (pcFilename[uiChar])
        {
            case '/':
            case '\\':
            case ':':
            case '*':
            case '?':
            case '\"':
            case '<':
            case '>':
            case '|':
                // Replace invalid character with replacement character.
                pcFilename[uiChar] = cReplacement;
                break;
        }
    }
}
//---------------------------------------------------------------------------
