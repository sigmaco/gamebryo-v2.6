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

#include "NiUIImageButton.h"
#include "NiUIManager.h"

NiImplementRTTI(NiUIImageButton, NiUIBaseElement);
//---------------------------------------------------------------------------
NiUIImageButton::NiUIImageButton() :
    NiUIBaseElement(true), m_bToggleState(false), m_eImageState(NORMAL)
{
    for (unsigned int ui = 0; ui < NUM_STATES; ui++)
    {
        m_aeOnElements[ui] = NiUIAtlasMap::NUM_ELEMENTS;
        m_aeOffElements[ui] = NiUIAtlasMap::NUM_ELEMENTS;;
    }
}
//---------------------------------------------------------------------------
NiUIImageButton::~NiUIImageButton()
{
}
//---------------------------------------------------------------------------
void NiUIImageButton::UpdateLayout()
{
    
}
//---------------------------------------------------------------------------
void NiUIImageButton::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;

    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString(
            "NiUIImageButton::ReinitializeDisplayElements called without "
            "NiUIManager initialization.  This is prohibited.");
        return;
    }

    if (m_bVisible)
    {
        const NiUIAtlasMap* pkAtlas = NiUIManager::GetUIManager()->
            GetAtlasMap();
    
        NiRect<float> kOuterUVs;
        NiUIAtlasMap::UIElement kStateName;

        if (m_bToggleState)
            kStateName = m_aeOnElements[m_eImageState];
        else
            kStateName = m_aeOffElements[m_eImageState];

        kOuterUVs = pkAtlas->GetOuterBounds(kStateName);
        NiRect<float> kAspectRatioSafeRect = 
            ComputeAspectRatioPreservingInnerNSCRect(kStateName, m_kNSCRect);

        // Compute internal rect
        bool bSuccess = InitializeQuad(m_spScreenElements, m_iBaseQuadIdx,
            kAspectRatioSafeRect, kOuterUVs, NiColorA::WHITE);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a "
                "NiUIImageButton. Probable cause: invalid uiBaseIdx passed "
                "in to NiUIImageButton::ReinitializeDisplayElements.");
        }
    }
    else
    {
        bool bSuccess = HideQuad(m_spScreenElements, m_iBaseQuadIdx);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a"
                " NiUIImageButton. Probable cause: invalid uiBaseIdx passed "
                "in to NiUIImageButton::ReinitializeDisplayElements.");
        }
    }
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
bool NiUIImageButton::SetFocus(bool bFocus)
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    NiUIBaseElement::SetFocus(bFocus);
    if (bFocus && m_eImageState == NORMAL)
    {
        m_eImageState = HIGHLIT;
        ReinitializeDisplayElements();
    }
    else if (!bFocus && m_eImageState == HIGHLIT)
    {
        m_eImageState = NORMAL;
        ReinitializeDisplayElements();
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiUIImageButton::MouseLocation(unsigned int uiMouseX, 
    unsigned int uiMouseY)
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    bool bIsInside = NiUIBaseElement::MouseLocation(uiMouseX, uiMouseY);
    if (bIsInside && m_eImageState == NORMAL)
    {
        m_eImageState = HIGHLIT;
        ReinitializeDisplayElements();
    }
    
    return bIsInside;
}
//---------------------------------------------------------------------------
void NiUIImageButton::EnterPressed(unsigned char ucButtonState)
{
    if (ucButtonState & NiUIManager::WASPRESSED)
        SetState(!m_bToggleState);
}
//---------------------------------------------------------------------------
void NiUIImageButton::LeftMouseButton(unsigned char ucButtonState)
{
    if (ucButtonState & NiUIManager::WASPRESSED)
        SetState(!m_bToggleState);
}
//---------------------------------------------------------------------------
