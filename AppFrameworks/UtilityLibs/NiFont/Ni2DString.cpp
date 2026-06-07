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
#include "NiFontPCH.h"

#include "Ni2DString.h"

#include <NiAlphaProperty.h>
#include <NiMaterialProperty.h>
#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>
#include <NiZBufferProperty.h>
#include <NiPoint3.h>
#include <NiPoint2.h>
#include <NiRenderTargetGroup.h>

//---------------------------------------------------------------------------
Ni2DString::Ni2DString(
    NiFont* pkFont, 
    unsigned int uiFlags,
    unsigned int uiMaxStringLen, 
    const NiWChar* pkString,
    const NiColorA& kColor, 
    short sX, 
    short sY,
    unsigned char ucDirection /*= NIFONTSTRING_LEFT_TO_RIGHT*/,
    unsigned short usPointSize /*= DEFAULT_POINT_SIZE*/,
    const NiRenderTargetGroup* pkTarget /*= NULL*/,
    bool bStatic /*= false*/) :
    NiFontString(pkFont, 
        uiFlags, 
        uiMaxStringLen, 
        pkString, 
        kColor,
        ucDirection,
        bStatic),
    m_usPointSize(usPointSize),
    m_sX(sX),
    m_sY(sY),
    m_pkTarget(pkTarget),
    m_bUseBatchUpdate(true)
{
    // Ensure CreateScreenElements will do useful work.
    m_bModified = true;

    CreateScreenElements();
}
//---------------------------------------------------------------------------
Ni2DString::Ni2DString(NiFont* pkFont, unsigned int uiFlags,
    unsigned int uiMaxStringLen, const char* pcString, const NiColorA& kColor,
    short sX, short sY,
    unsigned char ucDirection /*= NIFONTSTRING_LEFT_TO_RIGHT*/,
    unsigned short usPointSize /*= DEFAULT_POINT_SIZE*/,
    const NiRenderTargetGroup* pkTarget /*= NULL*/,
    bool bStatic /*= false*/) :
    NiFontString(
        pkFont, 
        uiFlags, 
        uiMaxStringLen, 
        pcString, 
        kColor, 
        ucDirection,
        bStatic),
    m_usPointSize(usPointSize),
    m_sX(sX),
    m_sY(sY),
    m_pkTarget(pkTarget),
    m_bUseBatchUpdate(true)
{
    // Ensure CreateScreenElements will do useful work.
    m_bModified = true;

    CreateScreenElements();
}
//---------------------------------------------------------------------------
Ni2DString::~Ni2DString()
{
}
//---------------------------------------------------------------------------
void Ni2DString::sprintf(const char* pcFormatString, ...)
{
    if (m_bStatic)
        return;

    const unsigned int uiDestSize = 2048;
    char acMessage[uiDestSize];

    va_list args;
    va_start(args, pcFormatString);

    NiVsprintf(acMessage, uiDestSize, pcFormatString, args);

    va_end(args);

    // We duplicate the currently set flags
    SetText(acMessage, m_uiFlags);
}
//---------------------------------------------------------------------------
void Ni2DString::sprintf(const NiWChar* pkFormatString, ...)
{
    if (m_bStatic)
        return;

    const unsigned int uiDestSize = 2048;
    NiWChar akDest[uiDestSize];

    va_list kArgs;
    va_start(kArgs, pkFormatString);

#if defined(_PS3)
    vswprintf((wchar_t*)akDest, uiDestSize, (const wchar_t*)pkFormatString,
        kArgs);
#else   // #if defined(_PS3)
#if _MSC_VER >= 1400
    vswprintf_s((wchar_t*)akDest, uiDestSize, (const wchar_t*)pkFormatString, 
        kArgs);
#else   // _MSC_VER >= 1400
    vswprintf((wchar_t*)akDest, uiDestSize, (const wchar_t*)pkFormatString, 
        kArgs);
#endif   // _MSC_VER >= 1400
#endif  // #if defined(_PS3)

    akDest[uiDestSize - 1] = '\0';  // Force last character to be null.

    va_end(kArgs);

    SetText(akDest, m_uiFlags); // Preserve the current flags.
}
//---------------------------------------------------------------------------
void Ni2DString::SetColor(const NiColorA& kColor)
{
    m_kColor = kColor;

    if (m_uiFlags & COLORED)
    {
        // All screen elements will share same material property
        NiMeshScreenElementsPtr spScreenElement = 
            m_kMeshScreenElements.GetAt(0);

        if (spScreenElement)
        {
            NiMaterialProperty* pkMatProp = (NiMaterialProperty*)
                spScreenElement->GetProperty(NiProperty::MATERIAL);
            NIASSERT(pkMatProp);
            pkMatProp->SetEmittance(
                NiColor(m_kColor.r, m_kColor.g, m_kColor.b));
            pkMatProp->SetAlpha(m_kColor.a);
        }
    }
}
//---------------------------------------------------------------------------
void Ni2DString::Draw(NiRenderer* pkRenderer)
{
    if (m_uiStringLen <= 1)
        return;

    if (m_kMeshScreenElements.GetAt(0) == NULL)
        CreateScreenElements();
    else if (m_bModified)
        UpdateScreenElements();

    // Draw the NiMeshScreenElements
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_kMeshScreenElements.GetEffectiveSize(); 
        uiLoop++)
    {
        m_kMeshScreenElements.GetAt(uiLoop)->RenderImmediate(pkRenderer);
    }
}
//---------------------------------------------------------------------------
void Ni2DString::GetTextExtent(float& fWidth, float& fHeight)
{
    NIASSERT(m_spFont);

    // These values should have been updated in NiFontString::SplitText 
    // function. This is done internally! All of these computations assume
    // the point size found in the font. These values must be scaled to 
    // compensate for the new point size.
    float fScale = 1.0f;

    // Compute a scale to convert from the point height desired from the point
    // height found in the NiFont
    if (m_usPointSize != DEFAULT_POINT_SIZE)
        fScale = (float)m_usPointSize / (float)m_spFont->GetHeight();


    fWidth  = m_fStringWidth * fScale;
    fHeight = m_fStringHeight * fScale;
}
//---------------------------------------------------------------------------
void Ni2DString::CreateScreenElements()
{
    // We will use m_uiMaxStringLen to determine the number of rectangles and
    // vertices we will use. This will allow users to set the max length, and
    // not have to modify the ScreenElements and re-create it every time the 
    // set their text a little longer than it was previously.
    
    // A 1 character string will have a string length of 2
    if (m_uiStringLen > 0)
    {        
        // Create a new screen elements for each texture
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < m_spFont->GetNumTextures(); uiLoop++)
        {
            unsigned char ucAccessMask = 
                (unsigned char)(NiDataStream::ACCESS_GPU_READ |
                (m_bStatic ? 
                NiDataStream::ACCESS_CPU_WRITE_MUTABLE : 
                NiDataStream::ACCESS_CPU_WRITE_VOLATILE));
            
            // Create the New Screen Elements Data. A grow by size of 20
            // was chosen because the we will be adding string where each 
            // glyph is a polygon.
            NiMeshScreenElements* pkNewElements = 
                NiNew NiMeshScreenElements(false,
                false, 1, m_uiMaxStringLen, 20, m_uiMaxStringLen * 4, 80,
                m_uiMaxStringLen * 2, 40, ucAccessMask);

            NIASSERT(pkNewElements);

            m_kMeshScreenElements.Add(pkNewElements);

            // Attach the font's texture
            NiTexturingProperty* pkTexture = NiNew NiTexturingProperty();
            pkTexture->SetBaseTexture(
                m_spFont->GetTextureAtIndex((unsigned char)uiLoop));
            m_kMeshScreenElements.GetAt(uiLoop)->AttachProperty(pkTexture);
        }

        // Create the default alpha property
        NiAlphaProperty* pkDefaultAlphaProperty = NiNew NiAlphaProperty;
        pkDefaultAlphaProperty->SetAlphaBlending(true);
        pkDefaultAlphaProperty->SetSrcBlendMode(
            NiAlphaProperty::ALPHA_SRCALPHA);
        pkDefaultAlphaProperty->SetDestBlendMode(
            NiAlphaProperty::ALPHA_INVSRCALPHA);

        // Create the default vertex color property
        NiVertexColorProperty* pkDefaultVertexColorsProperty = 
            NiNew NiVertexColorProperty();
        pkDefaultVertexColorsProperty->SetSourceMode(
            NiVertexColorProperty::SOURCE_IGNORE);
        pkDefaultVertexColorsProperty->SetLightingMode(
            NiVertexColorProperty::LIGHTING_E);

        // Default to white if a color hasn't been specified
        NiColorA kColor = NiColorA::WHITE;
        if (m_uiFlags & COLORED)
            kColor = m_kColor;

        // Create the default material property
        NiMaterialProperty* pkDefaultMaterialProperty = 
            NiNew NiMaterialProperty();
        pkDefaultMaterialProperty->SetEmittance(
            NiColor(kColor.r, kColor.g, kColor.b));
        pkDefaultMaterialProperty->SetAlpha(kColor.a);

        // Create the default Z Buffer property
        NiZBufferProperty* pkDefaultZBufferProperty = NiNew NiZBufferProperty;
        pkDefaultZBufferProperty->SetZBufferTest(false);
        pkDefaultZBufferProperty->SetZBufferWrite(false);
        pkDefaultZBufferProperty->SetTestFunction(
            NiZBufferProperty::TEST_LESSEQUAL);

        // Attach the properties
        for (uiLoop = 0; uiLoop < m_spFont->GetNumTextures(); uiLoop++)
        {
            m_kMeshScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultAlphaProperty);

            m_kMeshScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultVertexColorsProperty);

            m_kMeshScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultMaterialProperty);

            m_kMeshScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultZBufferProperty);

            m_kMeshScreenElements.GetAt(uiLoop)->UpdateProperties();
            m_kMeshScreenElements.GetAt(uiLoop)->UpdateEffects();
        }
    }
    else
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < m_spFont->GetNumTextures(); uiLoop++)
            m_kMeshScreenElements.SetAt(uiLoop, NULL);
    }
}
//---------------------------------------------------------------------------
void Ni2DString::SetUseBatchUpdate(bool bUseBatchUpdate)
{
    m_bUseBatchUpdate = bUseBatchUpdate;
}
//---------------------------------------------------------------------------
void Ni2DString::UpdateScreenElements(bool bForceRecreate)
{
    if (m_bStatic || (!m_bModified && !bForceRecreate))
        return;

    // Turn back to normalized display coordinates
    float fScreenWidth;
    float fScreenHeight;

    // Determine the width and height of the expected render target
    if (m_pkTarget)
    {
        fScreenWidth = (float)m_pkTarget->GetWidth(0);
        fScreenHeight = (float)m_pkTarget->GetHeight(0);
    }
    else
    {
        // Use the default render target because one hasn't been defined
        fScreenWidth = (float)NiRenderer::GetRenderer()->
            GetDefaultRenderTargetGroup()->GetWidth(0);

        fScreenHeight = (float)NiRenderer::GetRenderer()->
            GetDefaultRenderTargetGroup()->GetHeight(0);
    }

    NIASSERT(m_kMeshScreenElements.GetAt(0));

    // Resize the arrays, if necessary
    unsigned int uiMaxVertexCount = m_uiMaxStringLen * 4;
    unsigned int uiMaxIndexCount = m_uiMaxStringLen * 6;

    // Clear the 'string'
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_kMeshScreenElements.GetEffectiveSize();
        uiLoop++)
    {
        m_kMeshScreenElements.GetAt(uiLoop)->
            ResizeAttributeStreams((NiUInt16)uiMaxVertexCount);
        m_kMeshScreenElements.GetAt(uiLoop)->
            ResizeIndexStream((NiUInt16)uiMaxIndexCount);

        if (m_bUseBatchUpdate)
        {
            m_kMeshScreenElements.GetAt(uiLoop)->BeginBatchUpdate();
        }
        m_kMeshScreenElements.GetAt(uiLoop)->RemoveAll();
    }

    // Fill in the data
    NiWChar ch;
    unsigned int ui;
    float pfUVSets[4];
    float fScale = 1.0f;

    // Compute a scale to convert from the point height desired from the point
    // height found in the NiFont
    if (m_usPointSize != DEFAULT_POINT_SIZE)
        fScale = (float)m_usPointSize / (float)m_spFont->GetHeight();

    // Setup the starting position of the string
    float fStartX = (float)m_sX;
    float fStartY = (float)m_sY;

    bool bVertical =
        m_ucDirection == NIFONTSTRING_TOP_TO_BOTTOM ||
        m_ucDirection == NIFONTSTRING_BOTTOM_TO_TOP;
    bool bBackwards =
        m_ucDirection == NIFONTSTRING_RIGHT_TO_LEFT ||
        m_ucDirection == NIFONTSTRING_BOTTOM_TO_TOP;

    float fDirection = NiSelect(bBackwards, -1.0f, 1.0f);
    float fMaxCharHeight = (float)m_spFont->GetCharHeight() * fScale;
    float fMaxCharWidth = (float)m_spFont->GetMaxPixelWidth() * fScale;
    float fNumLines = (float)m_uiStringCount;

    float fCurrX, fCurrY;
    float fLineWidth, fLineHeight;

    // Check if it should be centered...
    if (m_uiFlags & CENTERED)
    {
        // Get the dimensions of the first line.
        m_spFont->GetTextLineExtent(
            m_pkString, fLineWidth, fLineHeight, m_ucDirection);
        fLineWidth *= fScale;
        fLineHeight *= fScale;

        // Account for tracking. String lengths are cached on
        // NiFontString the way we (used to) cache line widths.
        fLineWidth += (m_puiStringLength[0] - 1) *
            m_fTracking * fScale;

        if (bVertical)
        {
            fCurrX = fStartX - (fNumLines * fMaxCharWidth * 0.5f)
                - ((fNumLines - 1.0f) * m_fLeading * fScale * 0.5f);
            fCurrY = fStartY - (fDirection * fLineHeight * 0.5f);
        }
        else
        {
            fCurrX = fStartX - (fDirection * fLineWidth * 0.5f);
            fCurrY = fStartY - (fNumLines * fMaxCharHeight * 0.5f)
                - ((fNumLines - 1.0f) * m_fLeading * fScale * 0.5f);
        }
    }
    else
    {
        fCurrX = fStartX;
        fCurrY = fStartY;
    }

    bool bLineStart = true;
    unsigned int uiLineNumber = 0;
    for (ui = 0; ui < m_uiStringLen; ui++)
    {
        ch = m_pkString[ui];

        // Check for end of string.
        if (ch == 0)
            break;

        m_spFont->GetCharUVPair(ch, pfUVSets);

        // Determine the dimensions of this character in pixels
        float fW, fH;
        m_spFont->GetCharExtent(ch, fW, fH);
        fW *= fScale;
        fH *= fScale;

        // Process character
        if (ch == '\n')
        {
            uiLineNumber++;
            // Check if it should be centered...
            if (m_uiFlags & CENTERED)
            {
                // Get the dimensions of the first line.
                m_spFont->GetTextLineExtent(
                    &m_pkString[ui + 1], fLineWidth, fLineHeight,
                    m_ucDirection);
                fLineWidth *= fScale;
                fLineHeight *= fScale;

                // Account for tracking. String lengths are cached on
                // on NiFontString the way we (used to) cache line widths.
                fLineWidth += (m_puiStringLength[uiLineNumber] - 1) *
                    m_fTracking * fScale;

                if (bVertical)
                {
                    fCurrX += fMaxCharWidth + m_fLeading * fScale;
                    fCurrY = fStartY - (fDirection * fLineHeight *
                        0.5f);
                }
                else
                {
                    fCurrX = fStartX - (fDirection * fLineWidth * 0.5f);
                    fCurrY += fMaxCharHeight + m_fLeading * fScale;
                }
            }
            else
            {
                if (bVertical)
                {
                    fCurrX += fMaxCharWidth + m_fLeading * fScale;
                    fCurrY = fStartY;
                }
                else
                {
                    fCurrX = fStartX;
                    fCurrY += fMaxCharHeight + m_fLeading * fScale;
                }
            }
            bLineStart = true;
        }
        // Doesn't display \r; this parallels the not-counting-\r-
        // in-line-length that is imposed by Ni2DString.
        else if (ch != '\r')
        {
            // Before the character, forwards text adds left overhang;
            // backwards text subtract right overhang. After, forwards
            // will add right overhang, backwards subtract left.

            // For backwards text, with first character of the line,
            // don't add tracking or overhang to the offset
            if (bBackwards)
            {
                if (bVertical)
                {
                    fCurrY -= fH;
                    if (!bLineStart)
                        fCurrY -= m_fTracking * fScale;

                }
                else
                {
                    fCurrX -= fW;
                    if (!bLineStart)
                    {
                        fCurrX -= m_fTracking * fScale;
                        fCurrX -= m_spFont->GetRightOverhang(ch) * fScale;
                    }
                }
            }
            else
            {
                if (!bVertical)
                    fCurrX += m_spFont->GetLeftOverhang(ch) * fScale;
            }


            // Add character glyph
            short sCurrX = (short)fCurrX, sCurrY = (short)fCurrY;
            CreateScreenPolygon(ch, sCurrX, sCurrY, (short)fW, (short)fH,
                fScreenWidth, fScreenHeight, pfUVSets);

            if (!bBackwards)
            {
                if (bVertical)
                    fCurrY += fH + m_fTracking * fScale;
                else
                {
                    fCurrX += fW + m_fTracking * fScale;
                    fCurrX += m_spFont->GetRightOverhang(ch) * fScale;
                }

            }
            else
            {
                if (!bVertical)
                    fCurrX -= m_spFont->GetLeftOverhang(ch) * fScale;
            }
            bLineStart = false;

        }
    }

    for (uiLoop = 0; uiLoop < m_kMeshScreenElements.GetEffectiveSize(); 
        uiLoop++)
    {
        m_kMeshScreenElements.GetAt(uiLoop)->Update(0.0f);
        if (m_bUseBatchUpdate)
        {
            m_kMeshScreenElements.GetAt(uiLoop)->EndBatchUpdate();
        }
    }

    m_bModified = false;
}
//---------------------------------------------------------------------------
void Ni2DString::CreateScreenPolygon(
    NiWChar ch, 
    short& sCurrX, 
    short& sCurrY, 
    short sW, 
    short sH, 
    float& fScreenWidth, 
    float& fScreenHeight, 
    float* pfUVSets)
{
    if (m_bStatic)
        return;

    // Get the texture and thereby determine which Screen Elements to
    // "add to".
    unsigned char ucTexture = m_spFont->GetTextureIndex(ch);

    // Create the new letter / polygon.
    int iPolygon = m_kMeshScreenElements.GetAt(ucTexture)->Insert(4);

    m_kMeshScreenElements.GetAt(ucTexture)->SetRectangle(iPolygon, 
        (float)sCurrX / fScreenWidth, (float)sCurrY / fScreenHeight,
        sW / fScreenWidth, sH / fScreenHeight);

    // Get the UV coordinates from the source texture.
    m_kMeshScreenElements.GetAt(ucTexture)->SetTextures(iPolygon, 0, 
        pfUVSets[0], pfUVSets[1], pfUVSets[2], pfUVSets[3]);
}
//---------------------------------------------------------------------------
