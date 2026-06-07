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
#include "NiFontPCH.h"

#include "NiFontString.h"
#include "NiFont.h"

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiFontString);
//---------------------------------------------------------------------------
NiWChar NiFontString::ms_akWorkingString[NI_MAX_PATH] = { 0 };
//---------------------------------------------------------------------------
NiFontString::NiFontString(NiFont* pkFont, 
    unsigned int uiFlags, 
    unsigned int uiMaxStringLen, 
    const char* pcString, 
    const NiColorA& kColor,
    unsigned char ucDirection /* = NIFONTSTRING_LEFT_TO_RIGHT*/,
    bool bStatic /* = false*/) :
    m_spFont(pkFont), 
    m_uiFlags(0),
    m_uiMaxStringLen(uiMaxStringLen),
    m_uiStringLen(0),
    m_pkString(NULL),
    m_pcString(NULL),
    m_kColor(kColor), 
    m_ucDirection(ucDirection),
    m_fLeading(0.0f),
    m_fTracking(0.0f),
    m_uiStringCount(0),
    m_pfStringWidth(0),
    m_puiStringLength(NULL),
    m_fStringWidth(0.0f),
    m_fStringHeight(0.0f),
    m_bModified(false),
    m_bStatic(false)
{
    SetText(pcString, uiFlags);

    // Set static flag here rather than in initialization so SetText will work.
    m_bStatic = bStatic;
}
//---------------------------------------------------------------------------
NiFontString::NiFontString(NiFont* pkFont, 
    unsigned int uiFlags, 
    unsigned int uiMaxStringLen, 
    const NiWChar* pkString, 
    const NiColorA& kColor,
    unsigned char ucDirection /* = NIFONTSTRING_LEFT_TO_RIGHT*/,
    bool bStatic /* = false*/) :
    m_spFont(pkFont), 
    m_uiFlags(0),
    m_uiMaxStringLen(uiMaxStringLen),
    m_uiStringLen(0),
    m_pkString(NULL),
    m_pcString(NULL),
    m_kColor(kColor), 
    m_ucDirection(ucDirection),
    m_fLeading(0.0f),
    m_fTracking(0.0f),
    m_uiStringCount(0),
    m_pfStringWidth(NULL),
    m_puiStringLength(NULL),
    m_fStringWidth(0.0f),
    m_fStringHeight(0.0f),
    m_bModified(false),
    m_bStatic(false)
{
    SetText(pkString, uiFlags);

    // Set static flag here rather than in initialization so SetText will work.
    m_bStatic = bStatic;
}
//---------------------------------------------------------------------------
NiFontString::~NiFontString()
{
    NiFree(m_pfStringWidth);
    NiFree(m_puiStringLength);
    NiFree(m_pcString);
    NiFree(m_pkString);
}
//---------------------------------------------------------------------------
void NiFontString::SetText(const char* pcText, unsigned int uiFlags)
{
    if (m_bStatic)
        return;

    if (uiFlags != 0xffffffff)
        m_uiFlags = uiFlags;

    unsigned int uiNewLen = m_uiStringLen;
    if (pcText)
        uiNewLen = (unsigned int)(strlen(pcText) + 1);

    if (m_pcString)
    {
        if (strcmp(m_pcString, pcText))
        {
            //  Check the length
            if (m_uiMaxStringLen > uiNewLen)
            {
                //  Just copy it in...
                NiStrcpy(m_pcString, m_uiMaxStringLen, pcText);

                // Duplicate the string to Unicode
                NiFont::AsciiToUnicode(m_pcString, m_uiMaxStringLen, 
                    m_pkString, m_uiMaxStringLen);

                m_uiStringLen = uiNewLen;
                m_bModified = true;
            }
            else
            {
                //  We need to free it and re-create it...
                ShutdownString();
                m_uiMaxStringLen = uiNewLen + 1;
            }
        }
        else
        {
            //  They're the same! Do nothing...
            return;
        }
    }

    if (m_pcString == 0)
    {
        m_uiStringLen = uiNewLen;

        // Make Sure the maximum length is at least the new string length
        m_uiMaxStringLen = NiMax((int)m_uiMaxStringLen, (int)uiNewLen);

        // Allocate the ASCII and Unicode strings
        m_pcString = NiAlloc(char, m_uiMaxStringLen);
        m_pkString = NiAlloc(NiWChar, m_uiMaxStringLen);

        if (pcText)
        {
            NiStrcpy(m_pcString, m_uiMaxStringLen, pcText);

            // Fill in the Unicode strings
            NiFont::AsciiToUnicode(m_pcString, m_uiMaxStringLen,
                m_pkString, m_uiMaxStringLen);
        }
        m_bModified = true;
    }

    SplitString();
}
//---------------------------------------------------------------------------
void NiFontString::SetText(const NiWChar* pkText, unsigned int uiFlags)
{
    if (m_bStatic)
        return;

    if (uiFlags != 0xffffffff)
        m_uiFlags = uiFlags;

    unsigned int uiNewLen = m_uiStringLen;
    if (pkText)
        uiNewLen = (unsigned int)(wcslen((const wchar_t *)pkText) + 1);

    if (m_pkString)
    {
        if (wcscmp((const wchar_t *)m_pkString, (const wchar_t *)pkText))
        {
            //  Check the length
            if (m_uiMaxStringLen > uiNewLen)
            {
                //  Just copy it in...
                NiWStrcpy(m_pkString, m_uiMaxStringLen, pkText);

                // Null the string because it can't be displayed
                m_pcString[0] = NULL;

                m_uiStringLen = uiNewLen;
                m_bModified = true;
            }
            else
            {
                //  We need to free it and re-create it...
                ShutdownString();
                m_uiMaxStringLen = uiNewLen + 1;
            }
        }
        else
        {
            //  They're the same! Do nothing...
            return;
        }
    }

    if (m_pkString == 0)
    {
        m_uiStringLen = uiNewLen;

        // Make Sure the maximum length is at least the new string length
        m_uiMaxStringLen = NiMax((int)m_uiMaxStringLen, (int)uiNewLen);

        // Allocate the ASCII and Unicode strings
        m_pcString = NiAlloc(char, m_uiMaxStringLen);
        m_pkString = NiAlloc(NiWChar, m_uiMaxStringLen);

        if (pkText)
        {
            NiWStrcpy(m_pkString, m_uiMaxStringLen, pkText);

            // Null the string because it can't be displayed
            m_pcString[0] = NULL;
        }
        m_bModified = true;
    }

    SplitString();
}
//---------------------------------------------------------------------------
void NiFontString::SetColor(const NiColorA& kColor)
{
    if (m_bStatic)
        return;

    if (m_kColor != kColor)
    {
        m_kColor = kColor;
        m_bModified = true;
    }
}
//---------------------------------------------------------------------------
void NiFontString::SplitString()
{
    if (m_bStatic)
        return;

    if (m_pcString == 0)
        return;

    //  Examine the string to see if there are any '\n' instances.
    unsigned int uiIndex = 0;
    unsigned int uiIndexNew = 0;
    unsigned int uiNewLineCount = 1;

    while (m_pkString[uiIndex] != 0)
    {
        if (m_pkString[uiIndex] == '\n')
            uiNewLineCount++;
        uiIndex++;
    }
    // ...code further down depends on the value of uiIndex computed here!

    // store the width of each line (in pixels)
    if ((uiNewLineCount > m_uiStringCount) || 
        (m_pfStringWidth == NULL))
    {
        m_pfStringWidth = (float*) NiRealloc(m_pfStringWidth,
            sizeof(float)*uiNewLineCount);
    }
    // store the length of each line (in characters)
    if ((uiNewLineCount > m_uiStringCount) ||
        (m_puiStringLength == NULL))
    {
        m_puiStringLength = (unsigned int*) NiRealloc(m_puiStringLength,
            sizeof(unsigned int)*uiNewLineCount);
    }


    //  Now, determine the max width
    float fTemp = 0.0f;
    m_uiStringCount = uiNewLineCount;
    if (m_uiStringCount == 1)
    {
        m_spFont->GetTextExtent(m_pkString, m_fStringWidth, m_fStringHeight,
            m_ucDirection);

        // Account for tracking: m_fTracking pixels between characters.
        // Length of this string was left in uiIndex above.
        m_fStringWidth += m_fTracking * (uiIndex - 1);

        m_pfStringWidth[0] = m_fStringWidth;
        // uiIndex should be equal to wcslen(m_pkString)
        m_puiStringLength[0] = uiIndex;
    }
    else
    {
        //  Need to check each 'string'
        unsigned int uiCurrentLine = 0;
        uiIndex = 0;
        uiIndexNew = 0;
        float fNewW = 0.0f;
        m_fStringWidth = 0.0f;
        m_fStringHeight = 0.0f;
        
        while (uiIndex < m_uiStringLen)
        {
            if ((m_pkString[uiIndex] == '\n') ||
                (m_pkString[uiIndex] == 0))
            {
                ms_akWorkingString[uiIndexNew] = 0;
                m_spFont->GetTextExtent(ms_akWorkingString, fNewW, fTemp,
                     m_ucDirection);

                // Account for tracking: m_fTracking pixels between characters
                fNewW += m_fTracking * (uiIndexNew - 1);
                NIASSERT(uiCurrentLine < m_uiStringCount);
                m_puiStringLength[uiCurrentLine] = uiIndexNew;
                m_pfStringWidth[uiCurrentLine] = fNewW;
                uiCurrentLine++;
                if (m_fStringWidth < fNewW)
                    m_fStringWidth = fNewW;
                m_fStringHeight += (float)(m_spFont->GetCharHeight());
                uiIndexNew = 0;
            }
            else
            {
                // Don't copy in carriage returns (Ascii 13)
                if (m_pkString[uiIndex] != '\r')
                    ms_akWorkingString[uiIndexNew++] = m_pkString[uiIndex];
            }
            uiIndex++;
        }

        // Adds (StringCount - 1) inter-line spaces of fLeading pixels
        float fExtraSpace = m_fLeading * (m_uiStringCount - 1);
        switch (m_ucDirection)
        {
            case NiFont::NIFONT_LEFT_TO_RIGHT:
            case NiFont::NIFONT_RIGHT_TO_LEFT:
                m_fStringHeight += fExtraSpace;
                break;
            case NiFont::NIFONT_TOP_TO_BOTTOM:
            case NiFont::NIFONT_BOTTOM_TO_TOP:
                m_fStringWidth += fExtraSpace;
                break;
            default:
                // You should never get here.
                NIASSERT(false);
        }
    }
}
//---------------------------------------------------------------------------
void NiFontString::ShutdownString()
{
    NiFree(m_pcString);
    NiFree(m_pkString);
    m_pcString = NULL;
    m_pkString = NULL;
    m_uiStringLen = 0;
    NiFree(m_pfStringWidth);
    NiFree(m_puiStringLength);
    m_pfStringWidth = NULL;
    m_puiStringLength = NULL;
}
//---------------------------------------------------------------------------
