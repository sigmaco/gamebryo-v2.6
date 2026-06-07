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

#include "NiRTLib.h"
#include "NiFilename.h"
#include "NiOS.h"

#define DIR_SEPERATOR_STRING "\\"

//---------------------------------------------------------------------------
NiFilename::NiFilename(const char* pcFullPath)
{
    m_acSubDir[0] = '\0';
    // Break down the file name here
    Splitpath(pcFullPath);
}
//---------------------------------------------------------------------------
bool NiFilename::GetFullPath(char* pcFullPath, unsigned int uiStrLen) const
{
    return Makepath(pcFullPath, uiStrLen);
}
//---------------------------------------------------------------------------
// Wrapper/replacement for _splitpath. The char arrays should be allocated
// prior to calling this function.
// This function _does not_ handle multibyte characters.
void NiFilename::Splitpath(const char* pcStr)
{
#if _MSC_VER >= 1400
    _splitpath_s(pcStr, m_acDrive, _MAX_DRIVE, m_acDir, _MAX_DIR, 
        m_acFname, _MAX_FNAME, m_acExt, _MAX_EXT);
#else //#if _MSC_VER >= 1400
    _splitpath(pcStr, m_acDrive, m_acDir, m_acFname, m_acExt);
#endif //#if _MSC_VER >= 1400
}

//---------------------------------------------------------------------------
// Wrapper/replacement for _makepath.  pcStr should already be allocated prior
// to calling this function
bool NiFilename::Makepath(char* pcStr, size_t stStrLen) const
{
    if (pcStr == NULL)
        return false;

    // First test to make sure that the buffer is large enough to hold the data
    // before actually writing the data.
    size_t stRequiredLength = 1;
    if (m_acDrive[0] != '\0')
    {
        stRequiredLength += 2;
    }

    if (m_acDir[0] != '\0')
    {
        size_t stLen = strlen(m_acDir);
        stRequiredLength += stLen;
        if (m_acDir[stLen-1] != '\\' && m_acDir[stLen-1] != '/')
            stRequiredLength += strlen(DIR_SEPERATOR_STRING);
    }

    if (m_acSubDir[0] != '\0')
    {
        size_t stLen = strlen(m_acSubDir);
        stRequiredLength += stLen;
        if (m_acSubDir[stLen-1] != '\\' && m_acSubDir[stLen-1] != '/')
            stRequiredLength += strlen(DIR_SEPERATOR_STRING);    
    }

    if (m_acFname[0] != '\0')
    {
        stRequiredLength += strlen(m_acFname);
    }

    if (m_acExt[0] != '\0')
    {   
        if (m_acExt[0] != '.')
            stRequiredLength++;
        stRequiredLength += strlen(m_acExt);
    }

    // The data is too large, do not write anything and return false.
    if (stRequiredLength > stStrLen)
        return false;

    // Begin copying the string
    pcStr[0] = '\0'; // Initialize the destination string

    // Copy the drive letter, if provided
    if (m_acDrive[0] != '\0')
    {
        pcStr[0] = m_acDrive[0];
        pcStr[1] = ':';
        pcStr[2] = '\0';
    }

    // Copy the directory, if provided
    if (m_acDir[0] != '\0')
    {
        NiStrcat(pcStr, stStrLen, m_acDir);
        // Do we need to append a trailing directory separator?
        size_t stLen = strlen(pcStr);

        if (pcStr[stLen-1] != '\\' && pcStr[stLen-1] != '/')
        {
            NiStrcat(pcStr, stStrLen, DIR_SEPERATOR_STRING);

        }
    }

    // Copy the subdirectory, if provided
    if (m_acSubDir[0] != '\0')
    {
        NiStrcat(pcStr, stStrLen, m_acSubDir);
        // Do we need to append a trailing directory separator?
        size_t stLen = strlen(pcStr);

        if (pcStr[stLen-1] != '\\' && pcStr[stLen-1] != '/')
        {
            NiStrcat(pcStr, stStrLen, DIR_SEPERATOR_STRING);
        }
    }

    // Copy the filename, if provided
    if (m_acFname[0] != '\0')
        NiStrcat(pcStr, stStrLen, m_acFname);

    // Copy the extension, if provided
    if (m_acExt[0] != '\0')
    {   // Do we need to prepend a period?
        if (m_acExt[0] != '.')
            NiStrcat(pcStr, stStrLen, ".");
        NiStrcat(pcStr, stStrLen, m_acExt);
    }

    return true;
}
//---------------------------------------------------------------------------