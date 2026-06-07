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

#include "NiSearchPath.h"
#include "NiFilename.h"

char NiSearchPath::ms_acDefPath[NI_MAX_PATH];

//---------------------------------------------------------------------------
NiSearchPath::NiSearchPath()
{
    m_uiNextPath = 0;
    m_acFilePath[0] = '\0';
    m_acReferencePath[0] = '\0';
}
//---------------------------------------------------------------------------
NiSearchPath::~NiSearchPath()
{
    /* */
}
//---------------------------------------------------------------------------
void NiSearchPath::SetFilePath(const char* pcFilePath)
{
    if (pcFilePath && pcFilePath[0] != '\0')
    {
        NiStrncpy(m_acFilePath, NI_MAX_PATH, pcFilePath, NI_MAX_PATH - 1);
        NiPath::Standardize(m_acFilePath);
    }
    else
    {
        m_acFilePath[0] = '\0';
    }
}
//---------------------------------------------------------------------------
void NiSearchPath::SetReferencePath(const char* pcReferencePath)
{
    if (pcReferencePath && pcReferencePath[0] != '\0')
    {
        NiFilename kSearchPath(pcReferencePath);
        kSearchPath.SetFilename("");
        kSearchPath.SetExt("");
        kSearchPath.GetFullPath(m_acReferencePath, NI_MAX_PATH);

        NiPath::Standardize(m_acReferencePath);
    }
    else
    {
        m_acReferencePath[0] = '\0';
    }
}
//---------------------------------------------------------------------------
void NiSearchPath::Reset()
{
    m_uiNextPath = 0;
}
//---------------------------------------------------------------------------
bool NiSearchPath::GetNextSearchPath(char* pcPath, unsigned int uiStringLen)
{
    //
    // This method will seek to resolve the true path to a file in 5 steps:
    //  1) using the path as specified in the input parameter
    //  2) searching the current directory
    //  3) appending the input parameter path to the directory where the 
    //     original NIF file was located
    //  4) searching the directory where the original NIF file was located
    //  5) searching the directory specified by the optional environment
    //     path set through the SetEnvFilePath method
    //

    NiFilename kFullPath(m_acFilePath);

    switch (m_uiNextPath)
    {
        case 0:
        {
            // Check incoming path (1)

            break;
        }    
        case 1:
        {
            // Check current directory (2)
            //
            // Note: fname and ext are set here.
            // They are potentially used for the next 2 cases.

            kFullPath.SetDrive("");
            kFullPath.SetDir("");

            break;
        }
        case 2:
        {
            // Check reference file directory with input path appended (3)
            //
            // Note: fname and ext are set here.
            // They are potentially used for the next 2 cases.

            kFullPath.SetDrive("");
            kFullPath.SetDir(m_acReferencePath);

            // This is a bit of a misuse of this, but it is a path that
            // is appended on the end of the main path
            const NiFilename inputURL(m_acFilePath);
            kFullPath.SetPlatformSubDir(inputURL.GetDir());

            break;
        }
        case 3:
        {
            // Check search path (4)

            kFullPath.SetDrive("");
            kFullPath.SetDir(m_acReferencePath);
            kFullPath.SetPlatformSubDir("");

            break;
        }
        case 4:
        {
            // Check optional environment variable directory (5)
            if (ms_acDefPath[0] != '\0') 
            {
                kFullPath.SetDrive("");
                kFullPath.SetDir(ms_acDefPath);

                break;
            }

            // no env path - fall through to next case
        }
        default:
            return false;
    }

    // grab the built path, standardize it, and return it.
    kFullPath.GetFullPath(pcPath, uiStringLen);
    NiPath::Standardize(pcPath);

    m_uiNextPath++;

    return true;
}
//---------------------------------------------------------------------------
