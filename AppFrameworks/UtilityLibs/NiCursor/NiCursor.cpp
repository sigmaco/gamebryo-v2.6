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
#include "NiCursorPCH.h"

#include "NiCursor.h"

#include <NiRenderTargetGroup.h>
#include <NiSourceTexture.h>

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiCursor);
//---------------------------------------------------------------------------
NiCursor* NiCursor::Create(NiRenderer* pkRenderer, 
    NiRect<int>& kRect, unsigned int uiHotSpotX, unsigned int uiHotSpotY, 
    const char* pcImageFile, const NiRenderTargetGroup* pkTarget)
{
    NiCursor* pkCursor = NiNew NiCursor(pkRenderer);
    if (!pkCursor)
        return 0;

    // Set the hotspot
    pkCursor->SetHotSpot(uiHotSpotX, uiHotSpotY);
    pkCursor->SetRect(kRect);
    pkCursor->SetRenderTargetGroup(pkTarget);

    // Set the image
    if (!pkCursor->SetImage(pcImageFile))
    {
        NiDelete pkCursor;
        return 0;
    }

    return pkCursor;
}
//---------------------------------------------------------------------------
NiCursor* NiCursor::Create(NiRenderer* pkRenderer, 
    NiRect<int>& kRect, unsigned int uiHotSpotX, unsigned int uiHotSpotY, 
    NiTexture* pkTexture, const NiRenderTargetGroup* pkTarget)
{
    NiCursor* pkCursor = NiNew NiCursor(pkRenderer);
    if (!pkCursor)
        return 0;

    // Set the hotspot
    pkCursor->SetHotSpot(uiHotSpotX, uiHotSpotY);
    pkCursor->SetRect(kRect);
    pkCursor->SetRenderTargetGroup(pkTarget);

    // Set the image
    if (!pkCursor->SetImage(pkTexture))
    {
        NiDelete pkCursor;
        return 0;
    }

    return pkCursor;
}
//---------------------------------------------------------------------------
NiCursor::NiCursor(NiRenderer* pkRenderer) :
    m_spRenderer(pkRenderer), 
    m_uiHotSpotX(0), 
    m_uiHotSpotY(0), 
    m_spImage(0), 
    m_spElements(0), 
    m_iScreenSpaceX(0), 
    m_iScreenSpaceY(0), 
    m_iShowCount(0), 
    m_bSystemCursorShowing(false),
    m_pkRenderTarget(NULL)
{
    m_kRect.m_left = 0;
    m_kRect.m_top = 0;
    m_kRect.m_right = 0;
    m_kRect.m_bottom = 0;
}
//---------------------------------------------------------------------------
NiCursor::~NiCursor()
{
    m_spImage = 0;
    m_spElements = 0;
    m_spRenderer = 0;
}
//---------------------------------------------------------------------------
int NiCursor::Show(bool bShow)
{
    if (bShow)
        m_iShowCount++;
    else
        m_iShowCount--;

    return m_iShowCount;
}
//---------------------------------------------------------------------------
bool NiCursor::SetPosition(float, int iScreenSpaceX, int iScreenSpaceY)
{
    bool bChanged = false;

    if (m_iScreenSpaceX != iScreenSpaceX)
    {
        if ((iScreenSpaceX >= m_kRect.m_left) &&
            (iScreenSpaceX < m_kRect.m_right))
        {
            m_iScreenSpaceX = iScreenSpaceX;
            bChanged = true;
        }
    }

    if (m_iScreenSpaceY != iScreenSpaceY)
    {
        if ((iScreenSpaceY >= m_kRect.m_top) &&
            (iScreenSpaceY < m_kRect.m_bottom))
        {
            m_iScreenSpaceY = iScreenSpaceY;
            bChanged = true;
        }
    }

    if (bChanged)
        UpdateScreenGeometry();

    return bChanged;
}
//---------------------------------------------------------------------------
bool NiCursor::Move(float, int iDeltaX, int iDeltaY)
{
    bool bChanged = false;

    if (iDeltaX < 0)
    {
        if ((m_iScreenSpaceX + iDeltaX) >= m_kRect.m_left)
        {
            m_iScreenSpaceX += iDeltaX;
            bChanged = true;
        }
    }
    else
    if (iDeltaX > 0)
    {
        if ((m_iScreenSpaceX + iDeltaX) < m_kRect.m_right)
        {
            m_iScreenSpaceX += iDeltaX;
            bChanged = true;
        }
    }

    if (iDeltaY < 0)
    {
        if ((m_iScreenSpaceY + iDeltaY) >= m_kRect.m_top)
        {
            m_iScreenSpaceY += iDeltaY;
            bChanged = true;
        }
    }
    else
    if (iDeltaY > 0)
    {
        if ((m_iScreenSpaceY + iDeltaY) < m_kRect.m_bottom)
        {
            m_iScreenSpaceY += iDeltaY;
            bChanged = true;
        }
    }

    if (bChanged)
        UpdateScreenGeometry();

    return bChanged;
}
//---------------------------------------------------------------------------
void NiCursor::Draw()
{
    if (m_iShowCount > 0)
    {
        m_spRenderer->SetScreenSpaceCameraData();
        m_spElements->RenderImmediate(m_spRenderer);
    }
}
//---------------------------------------------------------------------------
bool NiCursor::SetImage(const char* pcImageFile)
{
    // Load the texture. It must be A8R8G8B8 format on PC.
    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;

    NiSourceTexture* pkSource = 
        NiSourceTexture::Create(pcImageFile, kPrefs);
    if (!pkSource)
        return false;

    return SetImage(pkSource);
}
//---------------------------------------------------------------------------
bool NiCursor::SetImage(NiTexture* pkTexture)
{
    if (!pkTexture)
        return false;

    m_spImage = pkTexture;

    if (!m_spTextureProp)
    {
        m_spTextureProp = NiNew NiTexturingProperty;
        if (!m_spTextureProp)
            return false;
    }

    m_spTextureProp->SetBaseTexture(pkTexture);
    m_spTextureProp->SetBaseFilterMode(NiTexturingProperty::FILTER_NEAREST);
    m_spTextureProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);

    if (!m_spElements)
    {
        if (!m_spAlphaProp)
        {
            m_spAlphaProp = NiNew NiAlphaProperty;
            NIASSERT(m_spAlphaProp);
            m_spAlphaProp->SetAlphaBlending(true);
        }

        if (!m_spZBufferProp)
        {
            m_spZBufferProp = NiNew NiZBufferProperty;
            NIASSERT(m_spZBufferProp);
            m_spZBufferProp->SetZBufferTest(false);
            m_spZBufferProp->SetZBufferWrite(true);
        }

        // Create the screen object that represents the cursor using a
        // square polygon.
        m_spElements = NiNew NiMeshScreenElements(false, false, 1);

        m_spElements->Insert(4);

        // We assume a 32x32 cursor, offset for the hot spot.
        const NiRenderTargetGroup* pkRTGroup = m_pkRenderTarget;
        if (pkRTGroup == NULL)
            pkRTGroup = m_spRenderer->GetDefaultRenderTargetGroup();
        float fInvScrW = 1.0f / pkRTGroup->GetWidth(0);
        float fInvScrH = 1.0f / pkRTGroup->GetHeight(0);
        float fLeft = -fInvScrW * (float)m_uiHotSpotX;
        float fTop = -fInvScrH * (float)m_uiHotSpotY;
        float fWidth = 32.0f * fInvScrW;
        float fHeight = 32.0f * fInvScrH;
        m_spElements->SetRectangle(0, fLeft, fTop, fWidth, fHeight);
        m_spElements->UpdateBound();

        // By default, we will assume the whole texture is used.
        m_spElements->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);

        // Add the correct properties.
        m_spElements->AttachProperty(m_spTextureProp);
        m_spElements->AttachProperty(m_spAlphaProp);
        m_spElements->AttachProperty(m_spZBufferProp);

        m_spElements->SetTranslate(fInvScrW * (float)m_iScreenSpaceX,
            fInvScrH * (float)m_iScreenSpaceX, 0.5f);
    }

    // By default, the draw width and height are the image w/h.
    m_uiDrawWidth = pkTexture->GetWidth();
    m_uiDrawHeight = pkTexture->GetHeight();

    m_spElements->Update(0.0f);
    m_spElements->UpdateProperties();
    return true;
}
//---------------------------------------------------------------------------
bool NiCursor::UpdateScreenGeometry()
{
    if (!m_spElements)
        return false;

    const NiRenderTargetGroup* pkRTGroup = m_pkRenderTarget;
    if (pkRTGroup == NULL)
        pkRTGroup = m_spRenderer->GetDefaultRenderTargetGroup();
    float fInvScrW = 1.0f / (float)pkRTGroup->GetWidth(0);
    float fInvScrH = 1.0f / (float)pkRTGroup->GetHeight(0);
    m_spElements->SetTranslate(fInvScrW * (float)m_iScreenSpaceX,
        fInvScrH * (float)m_iScreenSpaceY, 0.5f);
    m_spElements->Update(0.0f);

    return true;
}
//---------------------------------------------------------------------------
void NiCursor::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    pStrings->Add(NiGetViewerString(NiCursor::ms_RTTI.GetName()));
    pStrings->Add(NiGetViewerString("m_uiHotSpotX", m_uiHotSpotX));
    pStrings->Add(NiGetViewerString("m_uiHotSpotY", m_uiHotSpotY));
    m_spImage->GetViewerStrings(pStrings);
    m_spElements->GetViewerStrings(pStrings);
}
//---------------------------------------------------------------------------
