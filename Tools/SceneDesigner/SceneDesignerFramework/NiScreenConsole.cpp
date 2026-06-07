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
#include "SceneDesignerFrameworkPCH.h"

#include "NiScreenConsole.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

const unsigned int NiScreenConsole::ms_uiASCIIMin = 33;
const unsigned int NiScreenConsole::ms_uiASCIIMax = 122;
const unsigned int NiScreenConsole::ms_uiASCIICols = 12;

//---------------------------------------------------------------------------
NiScreenConsole::NiScreenConsole() : m_pcDefaultFontPath(NULL),
    m_bIsOn(false), m_iTextBoxDimensionsX(100), m_iTextBoxDimensionsY(100), 
    m_iTextOriginX(0), m_iTextOriginY(0), m_spScreenTexture(NULL), 
    m_spTextTexture(NULL)

    , m_pkHoverFont(NULL),
    m_iLastHoverTextLength(0),
    m_spHoverScreenTexture(NULL), 
    m_spHoverTextTexture(NULL)
{
    m_apcText.SetSize(50);
    m_abTextIsReserved.SetSize(50);

    m_spCamera = NULL;
    for(unsigned int ui = 0; ui < 50; ui++)
    {
        m_apcText.SetAt(ui, NULL);
        m_abTextIsReserved.SetAt(ui, false);
    }
}
//---------------------------------------------------------------------------
NiScreenConsole::~NiScreenConsole()
{
    NiFree(m_pcDefaultFontPath);

    if (m_pkHoverFont)
        NiDelete m_pkHoverFont;

    NiDelete m_pkFont;
    for(unsigned int ui = 0; ui < m_apcText.GetSize(); ui++)
    {
        NiFree(m_apcText.GetAt(ui));
    }
    if(m_spCamera)
    {
        m_spActiveScreenTexture = NULL;
    }

    m_spHoverScreenTexture = NULL;
    m_spHoverTextTexture = NULL;

    m_spScreenTexture = NULL;
    m_spTextTexture = NULL;

}
//---------------------------------------------------------------------------
void NiScreenConsole::Enable(bool bOn)
{
    m_bIsOn = bOn;
    if(IsEnabled())
        SetCamera(m_spCamera);
    else if(m_spCamera)
        m_spActiveScreenTexture = NULL;
}
//---------------------------------------------------------------------------
bool NiScreenConsole::IsEnabled() const
{
    return m_bIsOn;
}
//---------------------------------------------------------------------------
// Internal font representation information
// We load fonts from a texture file and cannot actually
// scale them, so you need a different texture image file
// for each scale of font you wish to use
// Creates standard fonts for use
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateLargeFont()
{ 
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "asciilarge.tga";
    const size_t stLength = strlen(pkFont->m_pcTextImage) + 
        strlen(m_pcDefaultFontPath) + 1;
    char* pcFullFontPath = NiAlloc(char, stLength);
    NiSprintf(pcFullFontPath, stLength, "%s%s", m_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;
    pkFont->m_uiCharWidth = 20;
    pkFont->m_uiCharHeight = 30;
    pkFont->m_uiCharSpacingX = 21;
    pkFont->m_uiCharSpacingY = 30;
    pkFont->m_uiCharBaseU = 2;
    pkFont->m_uiCharBaseV = 7;

    return pkFont;
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateTinyFont()
{
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "asciitiny.tga";
    const size_t stLength = strlen(pkFont->m_pcTextImage) + 
        strlen(m_pcDefaultFontPath)+1;
    char* pcFullFontPath = NiAlloc(char, stLength);
    NiSprintf(pcFullFontPath, stLength, "%s%s", m_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;
    pkFont->m_uiCharWidth = 5;
    pkFont->m_uiCharHeight = 8;
    pkFont->m_uiCharSpacingX = 6;
    pkFont->m_uiCharSpacingY = 9;
    pkFont->m_uiCharBaseU = 0;
    pkFont->m_uiCharBaseV = 0;

    return pkFont;
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateConsoleFont()
{
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "ascii_con.tga";
    const size_t stLength = strlen(pkFont->m_pcTextImage) + 
        strlen(m_pcDefaultFontPath)+1;
    char* pcFullFontPath = NiAlloc(char, stLength);
    NiSprintf(pcFullFontPath, stLength, "%s%s", m_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;
    pkFont->m_uiCharWidth = 8;
    pkFont->m_uiCharHeight = 12;
    pkFont->m_uiCharSpacingX = 9;
    pkFont->m_uiCharSpacingY = 13;
    pkFont->m_uiCharBaseU = 0;
    pkFont->m_uiCharBaseV = 0;

    return pkFont;
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetFont(NiScreenConsole::NiConsoleFont* pkFont)
{ 
    if(pkFont != NULL)
    {
        m_pkFont = pkFont;
        if(m_spCamera)
            m_spActiveScreenTexture = NULL;

        NiImageConverter::SetPlatformSpecificSubdirectory(NULL);
    
        NiTexture::FormatPrefs kPrefs;
        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
        kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
        kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;

        m_spTextTexture = NiSourceTexture::Create(
            pkFont->m_pcTextImage, kPrefs );

        m_spScreenTexture = NiMeshScreenElements::Create(m_spTextTexture,
            NiTexturingProperty::APPLY_REPLACE);
        if(m_spCamera)
            m_spActiveScreenTexture = m_spScreenTexture;
    }
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::GetFont() const
{
    return m_pkFont;
}
//---------------------------------------------------------------------------
unsigned int NiScreenConsole::GetMaxLineCount() const
{ 
    if(m_pkFont)
        return m_iTextBoxDimensionsY / m_pkFont->m_uiCharHeight;
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiScreenConsole::GetMaxLineLength() const
{
    if(m_pkFont)
        return m_iTextBoxDimensionsX / m_pkFont->m_uiCharWidth;
    return 0;
}
//---------------------------------------------------------------------------
// Works in screen polygon coords:  <0-1, 0-1> to set the position of 
// the lower-left edge of the text.
//---------------------------------------------------------------------------
NiPoint2 NiScreenConsole::GetOrigin() const
{ 
    return NiPoint2((float)m_iTextOriginX, (float)m_iTextOriginY);
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetOrigin(NiPoint2 kOrigin)
{ 
    m_iTextOriginX = (int) kOrigin.x;
    m_iTextOriginY = (int) kOrigin.y;
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetCamera(NiCamera* pkCamera)
{
    if(pkCamera != NULL)
    {
        if(m_spScreenTexture != NULL && m_spCamera)
            m_spActiveScreenTexture = NULL;
        m_spCamera = pkCamera;
        m_spActiveScreenTexture = m_spScreenTexture;
    }
    else
        m_spCamera = NULL;
}    
//---------------------------------------------------------------------------
// Defines the clipping region of the text in pixels. If a line will 
// not fit within the dimensions at the current text size, it will 
// not be rendered.
//---------------------------------------------------------------------------
NiPoint2 NiScreenConsole::GetDimensions() const
{ 
    return NiPoint2((float)m_iTextBoxDimensionsX,
        (float)m_iTextBoxDimensionsY);
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetDimensions(NiPoint2 kDimensions)
{ 
    m_iTextBoxDimensionsX = (int) kDimensions.x;
    m_iTextBoxDimensionsY = (int) kDimensions.y;
}
//---------------------------------------------------------------------------
// Some applications may wish to reserve a free line number.
// This is not enforced in the SetLine method, but if handled 
// responsibly, this will act as a valid mechanism for holding line  
// numbers for specific information. If a valid index cannot be  
// generated, the function returns -1, an invalid index.
//---------------------------------------------------------------------------
int NiScreenConsole::GrabFreeLineNumber()
{ 
    int iReturn = -1;
    for (int i = 0; i < (int) m_abTextIsReserved.GetSize() && iReturn == -1;
        i++)
    {
        if(m_abTextIsReserved.GetAt(i) == false)
            iReturn = i;
    }

    if(iReturn != -1)
        m_abTextIsReserved.SetAt(iReturn, true);

    return iReturn;
}
//---------------------------------------------------------------------------
void NiScreenConsole::ReleaseLineNumber(unsigned int uiLineNumber)
{
    m_abTextIsReserved.SetAt(uiLineNumber, false);
    NiFree(m_apcText.GetAt(uiLineNumber));
    m_apcText.SetAt(uiLineNumber, NULL);
}
//---------------------------------------------------------------------------
unsigned int NiScreenConsole::GetCurrentReservedLineCount() const
{ 
    unsigned int uiReturn = 0;
    for(int i = 0; i < (int) m_abTextIsReserved.GetSize(); i++)
    {
        if(m_abTextIsReserved.GetAt(i) == true)
            uiReturn++;
    }

    return uiReturn;
}
//---------------------------------------------------------------------------
// Manipulates the given line number. Strings will be truncated
// to fit within the dimensions of the screen.
//---------------------------------------------------------------------------
void NiScreenConsole::SetLine(const char* pcLine, unsigned int uiWhichLine)
{ 
    NiFree (m_apcText.GetAt(uiWhichLine));
    m_apcText.SetAt(uiWhichLine, TruncateString(pcLine));
}
//---------------------------------------------------------------------------
const char* NiScreenConsole::GetLine(unsigned int uiWhichLine) const
{ 
    return (const char*)(m_apcText.GetAt(uiWhichLine));
}
//---------------------------------------------------------------------------
char* NiScreenConsole::TruncateString(const char* pcString)
{ 
    unsigned int uiLength = GetMaxLineLength();
    char* pcReturnString = NiAlloc(char, uiLength + 1);
    
    NiStrncpy(pcReturnString, uiLength + 1, pcString, uiLength);
    return pcReturnString;
}
//---------------------------------------------------------------------------
void NiScreenConsole::RecreateText()
{
    m_spScreenTexture->RemoveAll();

    unsigned int uiNumCurrentRows = 0;
    unsigned int uiCurrentColumn = 0;
    unsigned int uiMaxLines = GetMaxLineCount();
    int iMaxLineLength = (int) GetMaxLineLength();

    for(unsigned int uiLine = 0; uiLine < m_apcText.GetSize() && 
        uiLine < uiMaxLines; uiLine++)
    {
        char* pcString = m_apcText.GetAt(uiLine);
        //if(pcString == NULL)
        //    continue;   
        if (pcString != NULL)
        {
            size_t stStrLength = strlen(pcString);    
            stStrLength = NiMin((int) stStrLength, iMaxLineLength);

            for (unsigned int i = 0; i < stStrLength; i++)
            {
                char cChar = pcString[i];
           
                // if the extra char is a '\n', skip it
                if (cChar == '\n')
                    continue;

                unsigned int uiChar = (unsigned int)cChar;

                // skip whitespace or unprintable character
                if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
                {
                    uiChar -= ms_uiASCIIMin;
        
                    unsigned short usPixTop = (unsigned short)m_iTextOriginY;
                    usPixTop = (unsigned short)(usPixTop +(uiNumCurrentRows + 1) *
                        m_pkFont->m_uiCharSpacingY);
                    
                    unsigned short usPixLeft = (unsigned short)(m_iTextOriginX + 
                        uiCurrentColumn * m_pkFont->m_uiCharSpacingX);

                    unsigned short usTexTop = (unsigned short)(m_pkFont->m_uiCharBaseV + 
                        (uiChar / ms_uiASCIICols) * m_pkFont->m_uiCharSpacingY);
                    unsigned short usTexLeft = (unsigned short)(m_pkFont->m_uiCharBaseU + 
                        (uiChar % ms_uiASCIICols) * m_pkFont->m_uiCharSpacingX);

                    m_spScreenTexture->AddNewScreenRect(usPixTop, usPixLeft, 
                        (unsigned short)m_pkFont->m_uiCharWidth, 
                        (unsigned short)m_pkFont->m_uiCharHeight, 
                        usTexTop, usTexLeft, NiColorA::WHITE, 
                        m_iTextBoxDimensionsX, m_iTextBoxDimensionsY);
                }

                uiCurrentColumn++;
            }
        }

        uiNumCurrentRows++;
        uiCurrentColumn = 0;

    }
}
//---------------------------------------------------------------------------
NiMeshScreenElements* NiScreenConsole::GetActiveScreenTexture() const
{
    return m_spActiveScreenTexture;
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetDefaultFontPath(const char* pcPath)
{
    NiFree(m_pcDefaultFontPath);

    if(pcPath != NULL)
    {
        size_t stLen = strlen(pcPath) + 1;
        m_pcDefaultFontPath = NiAlloc(char, stLen);
        NiSprintf(m_pcDefaultFontPath, stLen, "%s", pcPath);
    }
}
//---------------------------------------------------------------------------
const char* NiScreenConsole::GetDefaultFontPath()
{
    return m_pcDefaultFontPath;
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateHoverFont()
{
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "asciihover.tga";
    const size_t stLength = strlen(pkFont->m_pcTextImage) + 
        strlen(m_pcDefaultFontPath)+1;
    char* pcFullFontPath = NiAlloc(char, stLength);
    NiSprintf(pcFullFontPath, stLength, "%s%s", m_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;

    // The following values are dependent on the font image used.
    // If you modify the dimensions of the characters in the
    // font image, you will need to adjust these values.
    pkFont->m_uiCharWidth = 6;
    pkFont->m_uiCharHeight = 11;
    pkFont->m_uiCharSpacingX = 6;
    pkFont->m_uiCharSpacingY = 11;
    pkFont->m_uiCharBaseU = 0;
    pkFont->m_uiCharBaseV = 0;

    return pkFont;
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetHoverFont(NiScreenConsole::NiConsoleFont* pkFont)
{ 
    if(pkFont != NULL)
    {
        m_pkHoverFont = pkFont;

        NiTexture::FormatPrefs kPrefs;
        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
        kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
        kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;

        char acOldImagePath[NI_MAX_PATH];
        acOldImagePath[0] = '\0';
        const char* pcOldImagePath = NiDevImageConverter::
            GetPlatformSpecificSubdirectory();
        if (pcOldImagePath)
        {
            NiStrcpy(acOldImagePath, NI_MAX_PATH, pcOldImagePath);
            NiDevImageConverter::SetPlatformSpecificSubdirectory(NULL);
        }

        m_spHoverTextTexture = NiSourceTexture::Create(
            pkFont->m_pcTextImage, kPrefs );

        if (strlen(acOldImagePath) > 0)
        {
            NiDevImageConverter::SetPlatformSpecificSubdirectory(
                acOldImagePath);
        }

        m_spHoverScreenTexture = NiMeshScreenElements::Create(
            m_spHoverTextTexture, NiTexturingProperty::APPLY_REPLACE);
    }
}
//---------------------------------------------------------------------------
void NiScreenConsole::CreateHoverText(const char* pcHoverText, int iX, int iY)
{
    unsigned int iNumScreenRects;
    unsigned int i;
    int j;

    if (m_pkHoverFont)
        iNumScreenRects = m_spHoverScreenTexture->GetNumPolygons();
    else
        iNumScreenRects = 0;
    if (m_iLastHoverTextLength)
    {
        for (j = 0, i = iNumScreenRects-1; i>=0 && j<m_iLastHoverTextLength;
            i--, j++)
        {
            m_spHoverScreenTexture->Remove(i);
        }
    }
    m_iLastHoverTextLength = 0;

    size_t stStrLength = strlen(pcHoverText);    

    if (stStrLength > 0 &&
        iX >=0 && iX < m_iTextBoxDimensionsX &&
        iY >= 0 && iY < m_iTextBoxDimensionsY)
    {
        if (m_pkHoverFont == NULL)
            SetHoverFont(CreateHoverFont());
        if (m_pkHoverFont)
        {
            unsigned int uiCurrentColumn = 0;

            // Adjust point so that hover text fits in viewport
            iY -= (m_pkHoverFont->m_uiCharHeight + 12);
            iX += (2 - m_pkHoverFont->m_uiCharWidth);

            // check for top cutoff
            if (iY < 0)
            {
                iY += (m_pkHoverFont->m_uiCharHeight + 12);
            }

            // check for right cutoff
            if ((int)(iX + ((stStrLength + 3) *
                m_pkHoverFont->m_uiCharSpacingX)) > m_iTextBoxDimensionsX)
            {
                iX = (int)(iX - (((stStrLength + 2) * m_pkHoverFont->m_uiCharSpacingX)));
            }

            if (stStrLength)
            {
                unsigned int uiChar;

                unsigned short usPixTop;
                unsigned short usPixLeft;
                unsigned short usTexTop;
                unsigned short usTexLeft;

                // add a half character width padding to start string
                uiChar = ms_uiASCIIMax + 1 - ms_uiASCIIMin;

                usPixTop = (unsigned short)(iY + m_iTextOriginY +
                    m_pkHoverFont->m_uiCharSpacingY);

                usPixLeft = (unsigned short)(iX + m_iTextOriginX);

                usTexTop = (unsigned short)(m_pkHoverFont->m_uiCharBaseV + 
                    (uiChar / ms_uiASCIICols) *
                    m_pkHoverFont->m_uiCharSpacingY);
                usTexLeft = (unsigned short)(m_pkHoverFont->m_uiCharBaseU + 
                    (uiChar % ms_uiASCIICols) *
                    m_pkHoverFont->m_uiCharSpacingX);

                m_spHoverScreenTexture->AddNewScreenRect(usPixTop, usPixLeft, 
                    (unsigned short)m_pkHoverFont->m_uiCharWidth / 2,
                    (unsigned short)m_pkHoverFont->m_uiCharHeight, 
                    usTexTop, usTexLeft, NiColorA::BLACK, 
                    m_iTextBoxDimensionsX, m_iTextBoxDimensionsY);
                
                m_iLastHoverTextLength++;

                for (i = 0; i < stStrLength; i++)
                {
                    char cChar = pcHoverText[i];
               
                    // if the extra char is a '\n', skip it
                    if (cChar == '\n')
                        continue;

                    uiChar = (unsigned int)cChar;

                    // add a space for whitespace or unprintable characters
                    if ((uiChar < ms_uiASCIIMin) || (uiChar > ms_uiASCIIMax))
                        uiChar = ms_uiASCIIMax + 1;

                    uiChar -= ms_uiASCIIMin;

                    usPixLeft = (unsigned short)(iX + m_iTextOriginX +
                        (m_pkHoverFont->m_uiCharSpacingX / 2) +
                        uiCurrentColumn * m_pkHoverFont->m_uiCharSpacingX);

                    usTexTop = (unsigned short)(m_pkHoverFont->m_uiCharBaseV + 
                        (uiChar / ms_uiASCIICols) *
                        m_pkHoverFont->m_uiCharSpacingY);
                    usTexLeft = (unsigned short)(m_pkHoverFont->m_uiCharBaseU + 
                        (uiChar % ms_uiASCIICols) *
                        m_pkHoverFont->m_uiCharSpacingX);

                    m_spHoverScreenTexture->AddNewScreenRect(usPixTop,
                        usPixLeft, 
                        (unsigned short)m_pkHoverFont->m_uiCharWidth,
                        (unsigned short)m_pkHoverFont->m_uiCharHeight, 
                        usTexTop, usTexLeft, NiColorA::BLACK, 
                        m_iTextBoxDimensionsX, m_iTextBoxDimensionsY);

                    m_iLastHoverTextLength++;

                    uiCurrentColumn++;
                }

                // add a half character width padding to end string
                uiChar = ms_uiASCIIMax + 1 - ms_uiASCIIMin;

                usPixLeft = (unsigned short)(iX + m_iTextOriginX + 
                    (m_pkHoverFont->m_uiCharSpacingX / 2) +
                    uiCurrentColumn * m_pkHoverFont->m_uiCharSpacingX);

                usTexTop = (unsigned short)(m_pkHoverFont->m_uiCharBaseV + 
                    (uiChar / ms_uiASCIICols) *
                    m_pkHoverFont->m_uiCharSpacingY);
                usTexLeft = (unsigned short)(m_pkHoverFont->m_uiCharBaseU + 
                    (uiChar % ms_uiASCIICols) *
                    m_pkHoverFont->m_uiCharSpacingX);

                m_spHoverScreenTexture->AddNewScreenRect(usPixTop,
                    usPixLeft, 
                    (unsigned short)m_pkHoverFont->m_uiCharWidth / 2,
                    (unsigned short)m_pkHoverFont->m_uiCharHeight, 
                    usTexTop, usTexLeft, NiColorA::BLACK, 
                    m_iTextBoxDimensionsX, m_iTextBoxDimensionsY);
                
                m_iLastHoverTextLength++;
            }
        }
    }
}
//---------------------------------------------------------------------------
NiMeshScreenElements* NiScreenConsole::GetHoverScreenTexture() const
{
    return m_spHoverScreenTexture;
}
//---------------------------------------------------------------------------
