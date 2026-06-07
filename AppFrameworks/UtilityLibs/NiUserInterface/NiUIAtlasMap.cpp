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
#include "NiUserInterfacePCH.h"

#include "NiUIAtlasMap.h"
#include <NiRenderer.h>
#include "NiUIManager.h"

//---------------------------------------------------------------------------
const char* NiUIAtlasMap::ms_apcGroupNames[NUM_GROUPS] = {
    "FILES",
    "IMAGERECTS",
    "COLORS",
};
//---------------------------------------------------------------------------
const char* NiUIAtlasMap::ms_apcElementNames[NUM_ELEMENTS] = {
    "GAMEPAD_BUTTON_RRIGHT",
    "GAMEPAD_BUTTON_RLEFT",
    "GAMEPAD_BUTTON_RUP",
    "GAMEPAD_BUTTON_RDOWN",
    "GAMEPAD_BUTTON_R2",
    "GAMEPAD_BUTTON_L2",
    "GAMEPAD_BUTTON_R1",
    "GAMEPAD_BUTTON_L1",
    "GAMEPAD_BUTTON_SELECT",
    "GAMEPAD_BUTTON_START",
    "GAMEPAD_CONTROL_STICK_HORIZONTAL",
    "GAMEPAD_CONTROL_STICK_VERTICAL",
    "GAMEPAD_CONTROL_STICK_OMNI",
    "GAMEPAD_CONTROL_STICK",
    "GAMEPAD_DPAD_HORIZONTAL",
    "GAMEPAD_DPAD_VERTICAL",
    "GAMEPAD_DPAD_OMNI",
    "GAMEPAD_DPAD",
    "MOUSE_BUTTON_LEFT",
    "MOUSE_BUTTON_RIGHT",
    "MOUSE_BUTTON_SCROLL",
    "MOUSE_MOVE",
    "MOUSE_NULL",
    "KEYBOARD_BUTTON_WIDE",
    "KEYBOARD_BUTTON",
    "UI_LABEL",
    "UI_BUTTON_DEFAULT",
    "UI_BUTTON_DOWN",
    "UI_BUTTON_HIGH",
    "UI_CHECK_BOX_DEFAULT",
    "UI_CHECK_BOX_HIGH",
    "UI_CHECK_MARK",
    "UI_SLIDER_TRACK_DEFAULT",
    "UI_SLIDER_TRACK_HIGH",
    "UI_SLIDER_MARK_DEFAULT",
    "UI_SLIDER_MARK_HIGH",
    "UI_GROUP",
    "UI_MIN_DEFAULT",   
    "UI_MIN_DOWN",  
    "UI_MIN_HIGH",      
    "UI_MAX_DEFAULT",   
    "UI_MAX_DOWN",      
    "UI_MAX_HIGH",
#if defined(_WII)
        "WII_REMOTE_WAGGLE",
        "WII_REMOTE_HOME",
#endif
};
//---------------------------------------------------------------------------
const char* NiUIAtlasMap::ms_apcTextColorNames[NUM_TEXT_COLORS] = {
    "TEXT_ON_KEYBOARD_BUTTON",
    "TEXT_ON_UI_DEFAULT",
    "TEXT_ON_UI_DOWN",
    "TEXT_ON_UI_HIGH",
    "TEXT_ON_GAMEPAD_CONTROL_STICK"
};

//---------------------------------------------------------------------------
const char* NiUIAtlasMap::ms_apcTextSizeNames[NUM_TEXT_SIZES] = {
    "TEXT_FOR_LOW_RESOLUTION",
    "TEXT_FOR_MED_RESOLUTION",
    "TEXT_FOR_HIGH_RESOLUTION"
};
//---------------------------------------------------------------------------
NiUIAtlasMap::NiUIAtlasMap() :
    m_spAtlas(NULL)
{
    // Default initialization of NiRect is to zero the elements
    unsigned int uiIdx;
    for (uiIdx = 0; uiIdx < NUM_ELEMENTS; ++uiIdx)
    {
        m_akBounds[uiIdx].m_left = 0.0f;
        m_akBounds[uiIdx].m_right = 1.0f;
        m_akBounds[uiIdx].m_top = 0.0f;
        m_akBounds[uiIdx].m_bottom = 1.0f;

        m_akInternalBounds[uiIdx].m_left = 0.0f;
        m_akInternalBounds[uiIdx].m_right = 1.0f;
        m_akInternalBounds[uiIdx].m_top = 0.0f;
        m_akInternalBounds[uiIdx].m_bottom = 1.0f;
    }

    // Default initial text color is white
    for (uiIdx = 0; uiIdx < NUM_TEXT_COLORS; ++uiIdx)
    {
        m_akTextColors[uiIdx].r = m_akTextColors[uiIdx].g = 
            m_akTextColors[uiIdx].b = m_akTextColors[uiIdx].a = 1.0f;
    }
}
//---------------------------------------------------------------------------
NiUIAtlasMap::~NiUIAtlasMap()
{
    m_spAtlas = NULL;
}
//---------------------------------------------------------------------------
bool NiUIAtlasMap::ReadFile(const char* pcAtlasFilename)
{
    if (pcAtlasFilename == NULL)
        return false;

    // delete previous file
    m_spAtlas = NULL;
    NiString kAtlasFilename(pcAtlasFilename);

    m_spAtlas = NiSourceTexture::Create(NiFixedString(kAtlasFilename));

    if (m_spAtlas == NULL)
    {
        return false;
    }
    m_spAtlas->LoadPixelDataFromFile();

    NiPoint2 kDimensions((float)m_spAtlas->GetWidth(), 
        (float)m_spAtlas->GetHeight());

    
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!pkRenderer)
        return false;

    pkRenderer->ConvertFromPixelsToNDC(1, 1, m_kPixelToNDC.x, m_kPixelToNDC.y);

    NiString kBdrFilename;
    unsigned int uiDotLocation = kAtlasFilename.FindReverse('.');
    kBdrFilename = kAtlasFilename.GetSubstring(0, uiDotLocation);
    kBdrFilename += ".bdr";

    NiFile* pkBdrFile = NiFile::GetFile(kBdrFilename, NiFile::READ_ONLY);

    // These files must exist and have size
    if (!pkBdrFile || (!*(pkBdrFile)) || (pkBdrFile->GetFileSize() == 0))
    {
        NiDelete pkBdrFile;
        return false;
    }

    const unsigned int uiMaxBytes = 256;
    char acLine[uiMaxBytes];
    unsigned int uiLineLen;
    int iCurrentGroup = -1;
    int iNextGroup = -1;

    int iLine = -1;
    while ((uiLineLen = pkBdrFile->GetLine(acLine, uiMaxBytes)) > 0)
    {
        iLine++;

        // First check for a comment or empty line
        // As it turns out a line with /r/n on it returns a line len of 2
        // although it does not return either character
        if ((acLine[0] == '#') || (acLine[0] == '\0'))
        {
            continue;
        }
        
        char* pcLine = &acLine[0];
            
        // Is the current line a group definition?
        iNextGroup = ParseGroup(pcLine);

        // If what is on the line is not a group,
        // continue processing from previous group
        if (iNextGroup == -1 && iCurrentGroup != -1)
        {
            
            switch (iCurrentGroup)
            {
                case FILES:
                    {
                        int iTextSize = ParseTextSize(pcLine);
                        if (iTextSize == -1 || 
                            !ParseFilename(pcLine, 
                            m_akTextFilenames[iTextSize]))
                        {
#ifdef NIDEBUG
                            char acError[512];
                            NiSprintf(acError, 512, 
                                "PARSE ERROR! Line %d is invalid!\n", iLine);
                            NiOutputDebugString(acError);
                            NiOutputDebugString(acLine);
#endif
                            NiDelete pkBdrFile;
                            return false;
                        }
                    }
                    break;
                case IMAGERECTS:
                    {
                        int iElement = ParseElement(pcLine);
                        int iXOffset = 0;
                        int iYOffset = 0;
                        int iWidth = 0;
                        int iHeight = 0;
                        if (iElement == -1 || 
                            !ParseInt(pcLine, iXOffset) || 
                            !ParseInt(pcLine, iYOffset) ||
                            !ParseInt(pcLine, iWidth) ||
                            !ParseInt(pcLine, iHeight))
                        {
#ifdef NIDEBUG
                            char acError[512];
                            NiSprintf(acError, 512, 
                                "PARSE ERROR! Line %d is invalid!\n", iLine);
                            NiOutputDebugString(acError);       
                            NiOutputDebugString(acLine);

#endif
                            NiDelete pkBdrFile;
                            return false;
                        }

                        m_akBounds[iElement].m_left = 
                            ((float)iXOffset) / kDimensions.x;
                        m_akBounds[iElement].m_right = 
                            m_akBounds[iElement].m_left + 
                            ((float)iWidth) / kDimensions.x;
                        m_akBounds[iElement].m_top = 
                            ((float)iYOffset) / kDimensions.y;
                        m_akBounds[iElement].m_bottom = 
                            m_akBounds[iElement].m_top + 
                            ((float)iHeight) / kDimensions.y;

                        m_akInternalBounds[iElement] = m_akBounds[iElement];

                        if (ParseInt(pcLine, iXOffset) && 
                            ParseInt(pcLine, iYOffset) &&
                            ParseInt(pcLine, iWidth)   &&
                            ParseInt(pcLine, iHeight))
                        {
                            m_akInternalBounds[iElement].m_left = 
                                ((float)iXOffset) / kDimensions.x;
                            m_akInternalBounds[iElement].m_right = 
                                m_akInternalBounds[iElement].m_left + 
                                ((float)iWidth) / kDimensions.x;
                            m_akInternalBounds[iElement].m_top = 
                                ((float)iYOffset) / kDimensions.y;
                            m_akInternalBounds[iElement].m_bottom = 
                                m_akInternalBounds[iElement].m_top + 
                                ((float)iHeight) / kDimensions.y;
                        }

                    }
                    break;
                case COLORS:
                    {
                        int iColor = ParseTextColor(pcLine);
                        float fColors[4];
                        fColors[0] = 0.0f;
                        fColors[1] = 0.0f;
                        fColors[2] = 0.0f;
                        fColors[3] = 0.0f;
                        if (iColor == -1 ||
                            !ParseFloat(pcLine, fColors[0]) || 
                            !ParseFloat(pcLine, fColors[1]) ||
                            !ParseFloat(pcLine, fColors[2]) ||
                            !ParseFloat(pcLine, fColors[3]))
                        {
#ifdef NIDEBUG
                            char acError[512];
                            NiSprintf(acError, 512, 
                                "PARSE ERROR! Line %d is invalid!\n", iLine);
                            NiOutputDebugString(acError);
                            NiOutputDebugString(acLine);

#endif
                            NiDelete pkBdrFile;
                            return false;
                        }

                        m_akTextColors[iColor].r = fColors[0];
                        m_akTextColors[iColor].g = fColors[1];
                        m_akTextColors[iColor].b = fColors[2];
                        m_akTextColors[iColor].a = fColors[3];

                    }
                    break;
                default:
                    break;
            }
        }
        // If what is on this line is a group,
        // next line should be the first element of a new group
        else if (iNextGroup != -1)
        {
            iCurrentGroup = iNextGroup;
        }

    }

    NiDelete pkBdrFile;

    return true;
}
//---------------------------------------------------------------------------
NiRect<float> NiUIAtlasMap::ConvertAtlasUVToPixels(
    const NiRect<float>& kCoords) const
{
    NIASSERT(m_spAtlas);
    float fWidth = (float) m_spAtlas->GetWidth();
    float fHeight = (float) m_spAtlas->GetHeight();
    
    NiRect<float> kPixels = kCoords;
    kPixels.m_left *= fWidth;
    kPixels.m_right *= fWidth;
    kPixels.m_top *= fHeight;
    kPixels.m_bottom *= fHeight;

    return kPixels;
}
//---------------------------------------------------------------------------
NiRect<float> NiUIAtlasMap::ConvertAtlasUVToNDC(
    const NiRect<float>& kCoords) const
{
    NiRect<float> kNDC = ConvertAtlasUVToPixels(kCoords);
    
    kNDC.m_left *= m_kPixelToNDC.x;
    kNDC.m_right *= m_kPixelToNDC.x;
    kNDC.m_top *= m_kPixelToNDC.y;
    kNDC.m_bottom *= m_kPixelToNDC.y;

    return kNDC;
}
//---------------------------------------------------------------------------
const NiRect<float> NiUIAtlasMap::GetOuterBoundsDifference(
    UIElement eElement, CoordinateSystem eSystem) const
{
    NiRect<float> kOuter = GetOuterBounds(eElement, eSystem);
    NiRect<float> kInner = GetInnerBounds(eElement, eSystem);

    NiRect<float> kDiff;
    kDiff.m_left = NiAbs(kInner.m_left - kOuter.m_left);
    kDiff.m_right = NiAbs(kOuter.m_right - kInner.m_right);
    kDiff.m_top =  NiAbs(kInner.m_top - kOuter.m_top);
    kDiff.m_bottom = NiAbs(kOuter.m_bottom - kInner.m_bottom);
    return kDiff;
}
//---------------------------------------------------------------------------
