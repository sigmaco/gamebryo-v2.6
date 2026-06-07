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
//----------------------------------------------------------------------------
// NiUnicodeFontCFG.cpp
//----------------------------------------------------------------------------

#include "stdafx.h"
#include "NiUnicodeFontCFG.h"

#include <stdio.h>
#include <fcntl.h>
#include <io.h>


//----------------------------------------------------------------------------
NiUnicodeFontCFG::NiUnicodeFontCFG()
{
    strcpy_s(m_pcCurrentFontCfgFileName, NIUNICODEFONTCFG_PATHLEN, "");

    m_kHeader.m_uiVersion = 0;
    strcpy_s(m_kHeader.m_acFontName, NIUNICODEFONTCFG_NAMELEN, "");
    m_kHeader.m_usFontSize = 10;
    m_kHeader.m_usFontStyle = (unsigned short)FONTSTYLE_NONE;
}
//----------------------------------------------------------------------------
NiUnicodeFontCFG::~NiUnicodeFontCFG()
{
    unsigned int uiCount = m_kRawData.GetSize();
    for (unsigned int i = 0; i < uiCount; i++ )
    {
        unsigned short* pusData = m_kRawData.GetAt(i);
        if (pusData != NULL)
            delete[] pusData;
    }
    m_kRawData.RemoveAll();

    // Clean sorted list.
    uiCount = m_kSortedData.GetSize();
    for (unsigned int i = 0; i < uiCount; i++ )
    {
        CharData* pkData = m_kSortedData.GetAt(i);
        if (pkData != NULL)
            delete pkData;
    }

    m_kSortedData.RemoveAll();
}
//----------------------------------------------------------------------------
void NiUnicodeFontCFG::Reset()
{
    // Reset header
    strcpy_s(m_kHeader.m_acFontName, NIUNICODEFONTCFG_NAMELEN, "");
    m_kHeader.m_usFontSize = 10;
    m_kHeader.m_usFontStyle = (unsigned short)FONTSTYLE_NONE;

    unsigned int uiCount = m_kRawData.GetSize();
    for (unsigned int i = 0; i < uiCount; i++ )
    {
        unsigned short* pusData = m_kRawData.GetAt(i);
        if (pusData != NULL)
            delete[] pusData;
    }
    m_kRawData.RemoveAll();

    // Clean sorted list.
    uiCount = m_kSortedData.GetSize();
    for (unsigned int i = 0; i < uiCount; i++ )
    {
        CharData* pkData = m_kSortedData.GetAt(i);
        if (pkData != NULL)
            delete pkData;
    }

    m_kSortedData.RemoveAll();
}
//----------------------------------------------------------------------------
bool NiUnicodeFontCFG::LoadFontCFG(const char* pcFileName,
    BOOL bAddDefaultASCII)
{
    // If the given file name is same as the current file name, return.
    if ( !strcmp (pcFileName, m_pcCurrentFontCfgFileName) )
        return false;

    strcpy_s(m_pcCurrentFontCfgFileName, NIUNICODEFONTCFG_PATHLEN, pcFileName);

    FILE *pkFile;
    errno_t errnum = fopen_s (&pkFile, pcFileName, "rb");
    if (( errnum != 0 ) || ( pkFile == NULL ))
        return false;

    // Clean the data arrays.
    Reset();

    bool bStatus = LoadFontCFG(pkFile);

    fclose (pkFile);

    // Check for adding the default ASCII
    if (bAddDefaultASCII)
    {
        unsigned short usChar;
        for (usChar = 32; usChar <= 126; usChar++)
        {
            // If the character doesn't allready exists then add it
            if (!CharExists(usChar))
            {
                CharData* pkData = new CharData(1, usChar);
                m_kSortedData.Add(pkData);
            }
        }
    }

    return bStatus;
}
//----------------------------------------------------------------------------
bool NiUnicodeFontCFG::LoadFontCFG(FILE* pkFile)
{
    // Temp buffer. 
    NiWChar ausReadBuffer[1024];

    unsigned int uiLength = 0;
    unsigned int uiTotal = 0;
    do
    {
        uiLength = (unsigned int)fread (ausReadBuffer, 2, 1024, pkFile);
        int iReadStatus = ferror (pkFile);
        if ( iReadStatus > 0 )
        {
            NILOG(NIMESSAGE_GENERAL_0,
                "  NiUnicodeFontCFG::LoadFontCFG: read error happended.");
            break;
        }

        if ( uiLength > 0 )
        {
            NiWChar* pkStr = new NiWChar[uiLength+1];
            memcpy (pkStr, ausReadBuffer, uiLength * sizeof(NiWChar));
            pkStr[uiLength] = 0;
            m_kRawData.Add(pkStr);
        }

        uiTotal += uiLength;
    }
    while (uiLength == 1024);

    return true;
}
//----------------------------------------------------------------------------
// Process font configuration data.
// This function will clean out m_kSortedData first, then re-generate it from
// the m_kRawData.
void NiUnicodeFontCFG::ProcessFontCFG()
{
    int iRawDataCount = (int)m_kRawData.GetSize();
    if ( iRawDataCount == 0 )
        return;

    // Re-generate m_kSortedData.
    for ( int i = 0; i < iRawDataCount; i++ )
    {
        unsigned short* pusRawData = m_kRawData.GetAt(i);
        int iLen = (int)_tcslen ((TCHAR*)pusRawData);

        for ( int j =0; j < iLen; j++ )
            AccumulateCharacter(pusRawData[j]);
    }

    // Sort m_kSortedData.
    SortUniqueData();
}
//----------------------------------------------------------------------------
// This function will do two things
// - if the usCoding is not in sorted data list, create a CharData and add it.
// - if it has been in list, increase the m_uiProirity value.
void NiUnicodeFontCFG::AccumulateCharacter(unsigned short usCoding)
{
    int iSortedDataCount = (int)m_kSortedData.GetSize();

    // If the list is empty, just create a CharData and add it into list.
    if (iSortedDataCount == 0)
    {
        //CharData *data = NiNew CharData(1, usCoding);
        CharData* pkData = new CharData(1, usCoding);
        m_kSortedData.Add(pkData);
        return;
    }

    // Otherwise, try to find it.
    int iFoundIndex = -1;
    for (int i = 0; i < iSortedDataCount; i++ )
    {
        CharData* pkData = m_kSortedData.GetAt(i);
        if (pkData->m_usChar == usCoding)
        {
            iFoundIndex = i;
            break;
        }
    }

    if (iFoundIndex != -1)
    {
        // The char exists, increase the priority vlaue.
        CharData* pkData = m_kSortedData.GetAt(iFoundIndex);
        pkData->m_uiPriority += 1;
        m_kSortedData.SetAt(iFoundIndex, pkData);
    }
    else
    {
        //CharData *data = NiNew CharData(1, usCoding);
        CharData* pkData = new CharData(1, usCoding);
        m_kSortedData.Add(pkData);
    }
}
//----------------------------------------------------------------------------
// Sort m_kSortedData. Bubble up sort for at this time.
//----------------------------------------------------------------------------
void NiUnicodeFontCFG::SortUniqueData()
{
    int iSortedDataCount = (int)m_kSortedData.GetSize();
    for (int i = 0; i < iSortedDataCount - 1; i++)
    {
        CharData* pkData0 = m_kSortedData.GetAt(i);
        for (int j = i + 1; j < iSortedDataCount; j++)
        {
            CharData* pkData1 = m_kSortedData.GetAt(j);
            if (pkData0->m_uiPriority < pkData1->m_uiPriority )
            {
                // Swap
                m_kSortedData.SetAt(i, pkData1);
                m_kSortedData.SetAt(j, pkData0);
                pkData0 = m_kSortedData.GetAt(i);
            }
        }
    }
}
//----------------------------------------------------------------------------
// UnicodeFontCfgHeader
//----------------------------------------------------------------------------
NiUnicodeFontCFG::UnicodeFontCfgHeader::UnicodeFontCfgHeader()
{
    m_uiVersion = 0;
    strcpy_s(m_acFontName, NIUNICODEFONTCFG_NAMELEN, "");
    m_usFontSize = 10;
    m_usFontStyle = NiUnicodeFontCFG::FONTSTYLE_NONE;
}
//----------------------------------------------------------------------------
void NiUnicodeFontCFG::UnicodeFontCfgHeader::SetFontStyle(const char* pcStyle)
{
    if (!strcmp(pcStyle, "Bold"))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_BOLD;
    else if (!strcmp(pcStyle, "Italic"))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_ITALIC;
    else if (!strcmp(pcStyle, "StrikeOut"))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_STRIKEOUT;
    else if (!strcmp(pcStyle, "Underline"))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_UNDERLINE;
    else if (!strcmp(pcStyle, "AntiAliased"))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_ANTIALIASED;
}
//----------------------------------------------------------------------------
void NiUnicodeFontCFG::UnicodeFontCfgHeader::SetFontStyle(
    unsigned short* pusStyle)
{
    if (!_tcscmp((TCHAR*)pusStyle, _T("Bold")))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_BOLD;
    else if (!_tcscmp((TCHAR*)pusStyle, _T("Italic")))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_ITALIC;
    else if (!_tcscmp((TCHAR*)pusStyle, _T("StrikeOut")))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_STRIKEOUT;
    else if (!_tcscmp((TCHAR*)pusStyle, _T("Underline")))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_UNDERLINE;
    else if (!_tcscmp((TCHAR*)pusStyle, _T("AntiAliased")))
        m_usFontStyle |= NiUnicodeFontCFG::FONTSTYLE_ANTIALIASED;
}

