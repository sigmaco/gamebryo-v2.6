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

#include "NiUILabel.h"
#include "NiUIManager.h"

#include <NiRect.h>
NiImplementRTTI(NiUILabel, NiUIBaseElement);
//---------------------------------------------------------------------------
NiUILabel::NiUILabel(const char* pcLabelText, 
    const NiColorA& kBackgroundColor,const NiColor& kTextColor) : 
    NiUIBaseElement(false),
    m_kBackgroundColor(kBackgroundColor),
    m_kFontColor(kTextColor.r, kTextColor.g, kTextColor.b, 1.0f),
    m_kLabelOriginalText(pcLabelText),
    m_spViewableText(NULL),
    m_eVertAlign(TOP),
    m_eHorizAlign(LEFT)
{
}
//---------------------------------------------------------------------------
NiUILabel::~NiUILabel()
{
    m_spViewableText = NULL;
}
//---------------------------------------------------------------------------
void NiUILabel::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;

    if (NiUIManager::GetUIManager() == NULL)
        return;

    if (m_bVisible)
    {
        const NiUIAtlasMap* pkAtlasMap = 
            NiUIManager::GetUIManager()->GetAtlasMap(); 
        NiRect<float> kUVs = pkAtlasMap->GetOuterBounds(
            NiUIAtlasMap::UI_LABEL);
        
        bool bSuccess = InitializeQuad(m_spScreenElements, m_iBaseQuadIdx, 
            m_kNSCRect, kUVs, m_kBackgroundColor);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUILabel.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUILabel::ReinitializeDisplayElements.");
        }
    }
    else
    {
        bool bSuccess = HideQuad(m_spScreenElements, m_iBaseQuadIdx);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUILabel.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUILabel::ReinitializeDisplayElements.");
        }
    }
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
void NiUILabel::FormatViewableText()
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Attempting to initialize an NiUILabel without a "
            "valid NiUIManager.  This leads to unpredictable behavior.");
        return;
    }
    NiFont* pkFont = NiUIManager::GetUIManager()->GetFont();
    if (pkFont == NULL)
    {
        NiOutputDebugString("Attempting to initialize an NiUILabel without a "
            "valid NiUIManager.  This leads to unpredictable behavior.");
        return;
    }

    if (!m_spViewableText)
    {
        m_spViewableText = NiNew Ni2DString(pkFont, NiFontString::COLORED, 
            256, "", m_kFontColor, 
            (short)m_kPixelRect.m_left, 
            (short)m_kPixelRect.m_top); 
    }
    m_spViewableText->SetPosition(
        (short)m_kPixelRect.m_left, 
        (short)m_kPixelRect.m_top);

    float fWidth = (float) m_kPixelRect.GetWidth();
    float fHeight = (float) m_kPixelRect.GetHeight();

    if (fWidth == 0.0f || fHeight == 0.0f)
        return;

    float fStringWidth = 0.0f;
    float fStringHeight = 0.0f;

    // Force the formatted text to be a unique copy of the label text.
    m_kFormattedText = (const char*)m_kLabelOriginalText;
    
    // Replace all tabs with spaces
    m_kFormattedText.Replace("\t", "    ");
    
    // Get the dimensions of the text in pixels
    pkFont->GetTextExtent(m_kFormattedText, fStringWidth, fStringHeight);

    // Try the fast path first, which is if the full string can fit 
    // without any modifications.
    if (fStringWidth <= fWidth && fStringHeight <= fHeight)
    {
        m_spViewableText->SetText(m_kFormattedText);
        AlignViewableText();
        return;
    }

    bool bAdjustedWidth = false;

    // Try splitting lines that are too long
    if (fStringWidth > fWidth)
    {
        float fLineWidth = 0.0f;
        float fLineHeight = 0.0f;

        unsigned int uiEndIdx = 0;
        uiEndIdx = m_kFormattedText.Find('\n', uiEndIdx);
        unsigned int uiActiveIdx = 0;
        unsigned int uiStartIdx = 0;
        if (uiEndIdx == NiString::INVALID_INDEX)
            uiEndIdx =  m_kFormattedText.Length();

        while (uiEndIdx != NiString::INVALID_INDEX)
        {
            fLineWidth = 0.0f;
            fLineHeight = 0.0f;
            
            NiString kActiveString = m_kFormattedText.GetSubstring(
                uiStartIdx,    uiEndIdx);
            uiActiveIdx = pkFont->FitTextRangeExtent(
                kActiveString, kActiveString.Length(), fWidth);

            if (uiActiveIdx < kActiveString.Length())
            {
                unsigned int uiLastSpaceIdx = uiActiveIdx;
                while (uiLastSpaceIdx > 0)
                {
                    if (kActiveString.GetAt(uiLastSpaceIdx) == ' ')
                        break;
                    uiLastSpaceIdx--;
                }
                if (uiLastSpaceIdx != 0)
                {
                    // If this line is too long, we'll try and split it.
                    // uiLastSpaceIdx is relative to kActiveString;
                    // add uiStart to get into m_kFormattedText frame
                    m_kFormattedText.SetAt(uiStartIdx + uiLastSpaceIdx, '\n');
                    uiEndIdx = uiStartIdx + uiLastSpaceIdx;
                    bAdjustedWidth = true;
                }
                else if (uiEndIdx - uiStartIdx <= 1 ||
                    uiActiveIdx - uiStartIdx <= 1)
                {
                    // If a single character in the buffer is already too
                    // long, then remove that character.
                    if (uiActiveIdx > 0)
                        m_kFormattedText.RemoveRange(uiActiveIdx - 1, 1);
                    else
                        m_kFormattedText.RemoveRange(uiStartIdx, 1);
                    uiActiveIdx = uiStartIdx;
                    uiEndIdx = uiEndIdx - 1; 
                    bAdjustedWidth = true;   
                    continue;
                }
                else
                {
                    m_kFormattedText.Insert("\n", uiActiveIdx - 1);
                    uiEndIdx = uiActiveIdx - 1; 
                    uiActiveIdx = uiStartIdx;
                    bAdjustedWidth = true;     
                    continue;
                }
            }

            // Move on to the next line, we don't assume that the buffer
            // has stayed the same as allocations may have occurred underneath
            // the hood.
            uiActiveIdx = uiEndIdx + 1;
            uiStartIdx = uiEndIdx + 1;
            uiEndIdx = m_kFormattedText.Find('\n', uiEndIdx + 1);

            if (uiActiveIdx >= m_kFormattedText.Length())
                break;

            if (uiEndIdx == NiString::INVALID_INDEX)
                uiEndIdx = m_kFormattedText.Length();
        }
    }

    if (bAdjustedWidth)
        pkFont->GetTextExtent(m_kFormattedText, fStringWidth, fStringHeight);

    // Remove lines that overflow the maximum height
    while (fStringHeight > fHeight)
    {
        NIASSERT(fStringWidth <= fWidth);
        unsigned int uiFoundIdx = m_kFormattedText.FindReverse('\n');
        if (uiFoundIdx == NiString::INVALID_INDEX)
        {
            // No more lines to remove. Just don't set anything here.
            m_kFormattedText = "";
            m_spViewableText->SetText(m_kFormattedText);
            return;
        }
        else
        {
            m_kFormattedText.RemoveRange(uiFoundIdx, 
                m_kFormattedText.Length() - uiFoundIdx);
        }

        pkFont->GetTextExtent(m_kFormattedText, fStringWidth, fStringHeight);
    }

    NIASSERT(fStringHeight <= fHeight);
    NIASSERT(fStringWidth <= fWidth);
    m_spViewableText->SetText(m_kFormattedText);
    AlignViewableText();
}
//---------------------------------------------------------------------------
void NiUILabel::AlignViewableText()
{
    if (m_spViewableText == NULL)
        return;

    unsigned int uiTotWidth, uiTotHeight;
    unsigned int uiDx = 0;
    unsigned int uiDy = 0;
    float fWidth, fHeight;
    m_spViewableText->GetTextExtent(fWidth, fHeight);
    switch (m_eVertAlign)
    {
        case(TOP):
            uiDy = 0;
            break;
        case(BOTTOM):
            uiTotHeight = m_kPixelRect.GetHeight();
            uiDy = (unsigned int)(uiTotHeight - fHeight);
            break;
        case(VERT_MIDDLE):
            uiTotHeight = m_kPixelRect.GetHeight();
            uiDy = (unsigned int)((uiTotHeight - fHeight) / 2);
            break;
    }

    switch (m_eHorizAlign)
    {
        case(LEFT):
            uiDx = 0;
            break;
        case(RIGHT):
            uiTotWidth = m_kPixelRect.GetWidth();
            uiDx = (unsigned int)(uiTotWidth - fWidth);
            break;
        case(HORIZ_MIDDLE):
            uiTotWidth = m_kPixelRect.GetWidth();
            uiDx = (unsigned int)((uiTotWidth - fWidth) / 2);
            break;
    }
    
    m_spViewableText->SetPosition(
        (short)(m_kPixelRect.m_left + uiDx), 
        (short)(m_kPixelRect.m_top + uiDy));
}
//---------------------------------------------------------------------------
unsigned int NiUILabel::GetVisualLineCount() const
{
    if (m_spViewableText)
    {
        unsigned int uiCount = 0;
        const char* pcText = m_spViewableText->GetText();
        while (pcText && pcText[0] != '\0')
        {
            if (pcText[0] == '\n')
                uiCount++;
            pcText++;
        }
        uiCount++;
        return uiCount;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
