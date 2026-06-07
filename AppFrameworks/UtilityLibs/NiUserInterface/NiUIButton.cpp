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

#include "NiUIButton.h"
#include "NiUIManager.h"

NiImplementRTTI(NiUIButton, NiUIBaseElement);
//---------------------------------------------------------------------------
const float NiUIButton::ms_fHotkeyImageWidth = 0.1f;
const float NiUIButton::ms_fHotkeyImageTotalWidth = 0.6f;
//---------------------------------------------------------------------------
NiUIButton::NiUIButton(const char* pcButtonText, NiButtonType eButtonType) :
    NiUIBaseElement(true),
    m_eButtonType(eButtonType),
    m_spHotkeyImages(NULL),
    m_spLabel(NULL),
    m_bPressed(false),
    m_bHighlit(false),
    m_bMouseHeld(false)
{
    m_kHotkeyPressed.Initialize(this, &NiUIButton::HotkeyPressed);
    m_spHotkeyImages = NiNew NiUIHotkeyImage();
    NiColor kButtonTextColor = NiColor::BLACK;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Button initialized before the NiUIManager.  "
            "This can lead to unexpected results.");
    }
    else
    {
        kButtonTextColor = NiColor(
            NiUIManager::GetUIManager()->GetAtlasMap()->GetTextColor(
            NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
    }
    m_spLabel = NiNew NiUILabel(pcButtonText, 
        NiColorA(0.0f, 0.0f, 0.0f, 0.0f), kButtonTextColor);
    m_spLabel->SetAlignment(NiUILabel::VERT_MIDDLE, NiUILabel::LEFT);
    m_spLabel->SetParent(this);
    m_spHotkeyImages = NiNew NiUIHotkeyImage();
    m_spHotkeyImages->SetParent(this);
}
//---------------------------------------------------------------------------
NiUIButton::~NiUIButton()
{
    m_spHotkeyImages = NULL;
    m_spLabel = NULL;
}
//---------------------------------------------------------------------------
void NiUIButton::UpdateLayout()
{
    m_kInnerNSCRect = ComputeInnerNSCRectPixelAccurateOuter(
        NiUIAtlasMap::UI_BUTTON_DEFAULT, m_kNSCRect);

    float fTotalWidth = 0.0f;
    float fTotalHeight = 0.0f;
    fTotalWidth = m_kInnerNSCRect.GetWidth();
    fTotalHeight = m_kInnerNSCRect.GetHeight();

    // I want to make the space for the images approximately pixel-square.
    // However, in NSC, the width and height ratios for pixel size are 
    // different.  So, I will take the height as the edge length defined
    // in Y NSC.  Then find the edge len in X NSC that is the same number
    // of pixels.
    // The units for fEdgeLenInY_NSC could be considered Y_NSC
    float fEdgeLenInY_NSC = fTotalHeight * m_spHotkeyImages->GetTotalWidth();
    NiPoint2 kOnePixelInNSC = NiUIManager::GetUIManager()->
        ConvertPixelDimensionsToNSCDimensions(1, 1);
    // The units for kOnePixelInNSC.y are Y_NSC per pixel
    // so, fNumEdgePixels is in pixels
    float fNumEdgePixels = fEdgeLenInY_NSC / kOnePixelInNSC.y;
    // The units for kOnePixelInNSC.x are X_NSC per pixel
    // so, fEdgeLenInX_NSC is in X_NSC
    float fEdgeLenInX_NSC = fNumEdgePixels * kOnePixelInNSC.x;

    float fHotkeySpace = fEdgeLenInX_NSC;
#if !defined(HOTKEYS)
    fHotkeySpace = 0.0f;
#endif

    float fLeftOffset = m_kInnerNSCRect.m_left - m_kNSCRect.m_left;
    float fTopOffset = m_kInnerNSCRect.m_top - m_kNSCRect.m_top;

    m_spLabel->SetOffset(fLeftOffset, fTopOffset);
    m_spLabel->SetDimensions(fTotalWidth - fHotkeySpace, 
        m_kInnerNSCRect.GetHeight());
    
    m_spHotkeyImages->SetOffset(fLeftOffset + fTotalWidth - 
        fHotkeySpace, fTopOffset);
    m_spHotkeyImages->SetDimensions(fHotkeySpace, fTotalHeight);
}
//---------------------------------------------------------------------------
bool NiUIButton::AddKeyboardHotkey(NiInputKeyboard::KeyCode eKey,
    NiInputKeyboard::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUIButton hotkey events without"
            " first initializing NiUIManager singleton.");
        return false;
    }

    bool bHotkeyReserved = NiUIManager::GetUIManager()->ReserveKeyboardButton(
        eKey, &m_kHotkeyPressed, (NiUIManager::ISPRESSED | 
        NiUIManager::WASPRESSED | NiUIManager::WASRELEASED), eModifier);
    if (bHotkeyReserved)
    {
        m_spHotkeyImages->SetKeyboardHotkey(eKey, eModifier);
        UpdateRect();
    }

    return bHotkeyReserved;
}
//---------------------------------------------------------------------------
bool NiUIButton::AddGamePadAxis(NiInputGamePad::DefaultAxis eAxis, 
    bool bUseBothDirs)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUIButton hotkey events without"
            " first initializing NiUIManager singleton.");
        return false;
    }

    bool bHotkeyReserved = NiUIManager::GetUIManager()->ReserveGamePadAxis(
        eAxis, &m_kHotkeyPressed);
    if (bHotkeyReserved)
    {
        if (bUseBothDirs)
        {
            switch (eAxis)
            {
            case (NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_LEFT_VERT, &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_LEFT_VERT):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_LEFT_HORZ, &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT, 
                    &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_RIGHT_VERT):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_RIGHT_HORZ, 
                    &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_X1_HORZ):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_X1_VERT, 
                    &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_X1_VERT):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_X1_HORZ, &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_X2_HORZ):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_X2_VERT, &m_kHotkeyPressed);
                break;
            case (NiInputGamePad::NIGP_DEFAULT_X2_VERT):
                bHotkeyReserved = bHotkeyReserved && 
                    NiUIManager::GetUIManager()->ReserveGamePadAxis(
                    NiInputGamePad::NIGP_DEFAULT_X2_HORZ, &m_kHotkeyPressed);
                break;
            default:
                NIASSERT(!"Invalid axis enumeration");
                break;
            }
            if (!bHotkeyReserved)
                return false;
        }
        m_spHotkeyImages->SetGamePadAxis(eAxis, bUseBothDirs);
        UpdateRect();

    }

    return bHotkeyReserved;
}
//---------------------------------------------------------------------------
bool NiUIButton::AddGamePadHotkey(NiInputGamePad::Button eButton, 
    NiInputGamePad::ButtonMask eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUIButton hotkey events without"
            " first initializing NiUIManager singleton.");
        return false;
    }

    bool bHotkeyReserved = NiUIManager::GetUIManager()->ReserveGamePadButton(
        eButton, &m_kHotkeyPressed, (NiUIManager::ISPRESSED | 
        NiUIManager::WASPRESSED | NiUIManager::WASRELEASED), eModifier);

    if (bHotkeyReserved)
    {
        m_spHotkeyImages->SetGamePadHotkey(eButton, eModifier);
        UpdateRect();

    }
    return bHotkeyReserved;
}
//---------------------------------------------------------------------------
void NiUIButton::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString(
            "NiUIButton::ReinitializeDisplayElements called without "
            "NiUIManager initialization.  This is prohibited.");
        return;
    }

    if (m_bVisible)
    {
        const NiUIAtlasMap* pkAtlas = NiUIManager::GetUIManager()->
            GetAtlasMap();
    
        NiRect<float> kOuterUVs;
        NiRect<float> kInnerUVs;
        NiUIAtlasMap::UIElement kStateName;

        if (m_bHighlit)
            kStateName = NiUIAtlasMap::UI_BUTTON_HIGH;
        else if (m_bPressed)
            kStateName = NiUIAtlasMap::UI_BUTTON_DOWN;
        else
            kStateName = NiUIAtlasMap::UI_BUTTON_DEFAULT;

        kOuterUVs = pkAtlas->GetOuterBounds(kStateName);
        kInnerUVs = pkAtlas->GetInnerBounds(kStateName);
        
        // Compute internal rect
        bool bSuccess = InitializeNineQuad(m_spScreenElements, m_iBaseQuadIdx,
            m_kNSCRect, m_kInnerNSCRect, kOuterUVs, kInnerUVs, 
            NiColorA::WHITE);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUIButton. "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUIButton::ReinitializeDisplayElements.");
        }
    }
    else
    {
        bool bSuccess = HideNineQuad(m_spScreenElements, m_iBaseQuadIdx);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUIButton. "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUIButton::ReinitializeDisplayElements.");
        }
    }
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
void NiUIButton::HotkeyPressed(unsigned char ucButtonState)
{
    PressLogic(ucButtonState, true);
}
//---------------------------------------------------------------------------
void NiUIButton::PressLogic(unsigned char ucButtonState, bool)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;

    if (m_eButtonType == PRESSBUTTON)
    {
        if (ucButtonState & NiUIManager::WASPRESSED)
            m_kPressEvent.EmitSignal();
        if (m_bPressed)
        {
            if (ucButtonState & NiUIManager::ISPRESSED)
                m_kWhilePressEvent.EmitSignal();
            if (ucButtonState & NiUIManager::WASRELEASED)
                m_kReleaseEvent.EmitSignal();
        }

        // These are separate from the above because signals should be sent 
        // even in the crazy case where the press and release events happen 
        // in the same time step.  However, the UI should only show the 
        // current state
        if (m_bPressed && (ucButtonState & NiUIManager::WASRELEASED))
        {
            m_bPressed = false;
            if (!m_bHighlit)
            {
                m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                    GetAtlasMap()->GetTextColor(
                    NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
            }
            else
            {
                m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                    GetAtlasMap()->GetTextColor(
                    NiUIAtlasMap::TEXT_ON_UI_HIGH));
            }
            ReinitializeDisplayElements();
        }
        else if (ucButtonState & NiUIManager::WASPRESSED)
        {
            m_bPressed = true;
            m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                GetAtlasMap()->GetTextColor(
                NiUIAtlasMap::TEXT_ON_UI_DOWN));
            ReinitializeDisplayElements();
        }
    }
    else // then it's a toggle button
    {
        if (ucButtonState & NiUIManager::WASPRESSED)
        {
            m_bPressed = !m_bPressed;
            NiUIAtlasMap::TextColor kTextColor = 
                NiUIAtlasMap::TEXT_ON_UI_DEFAULT;
            if (m_bPressed)
                kTextColor = NiUIAtlasMap::TEXT_ON_UI_DOWN;
            else if (m_bHighlit)
                kTextColor = NiUIAtlasMap::TEXT_ON_UI_HIGH;
            m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                GetAtlasMap()->GetTextColor(kTextColor));
            m_kToggleEvent.EmitSignal(m_bPressed);
            ReinitializeDisplayElements();
        }
    }
}
//---------------------------------------------------------------------------
bool NiUIButton::AddMouseMotion()
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUIButton hotkey events without"
            " first initializing NiUIManager singleton.");
        return false;
    }

    bool bHotkeyReserved = NiUIManager::GetUIManager()->ReserveMouse(
        m_kHotkeyPressed);

    if (bHotkeyReserved)
        m_spHotkeyImages->SetMouseMotion();

    return bHotkeyReserved;
}
//---------------------------------------------------------------------------
bool NiUIButton::AddMouseHotkey(NiInputMouse::Button eButton,
    NiInputMouse::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUIButton hotkey events without"
            " first initializing NiUIManager singleton.");
        return false;
    }

    bool bHotkeyReserved = NiUIManager::GetUIManager()->ReserveMouseButton(
        eButton, &m_kHotkeyPressed, (NiUIManager::ISPRESSED | 
        NiUIManager::WASPRESSED | NiUIManager::WASRELEASED), eModifier);

    if (bHotkeyReserved)
        m_spHotkeyImages->SetMouseHotkey(eButton, eModifier);

    return bHotkeyReserved;
}
//---------------------------------------------------------------------------
bool NiUIButton::SetFocus(bool bFocus)
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    NiUIBaseElement::SetFocus(bFocus);
    if (bFocus && !m_bHighlit)
    {
        m_bHighlit = true;
        if (!m_bPressed)
        {
            m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                GetAtlasMap()->GetTextColor(
                NiUIAtlasMap::TEXT_ON_UI_HIGH));
        }
        ReinitializeDisplayElements();
    }
    else if (!bFocus && m_bHighlit)
    {
        m_bHighlit = false;
        if (!m_bPressed)
        {
            m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                GetAtlasMap()->GetTextColor(
                NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
        }
        ReinitializeDisplayElements();
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiUIButton::MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY)
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    bool bIsInside = NiUIBaseElement::MouseLocation(uiMouseX, uiMouseY);
    if (bIsInside && !m_bHighlit)
    {
        m_bHighlit = true;
        if (!m_bPressed)
        {
            m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
                GetAtlasMap()->GetTextColor(
                NiUIAtlasMap::TEXT_ON_UI_HIGH));
        }
        ReinitializeDisplayElements();
    }
    
    return bIsInside;
}
//---------------------------------------------------------------------------
unsigned int NiUIButton::GetChildElementCount() const
{
    return 2;
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUIButton::GetChildElement(unsigned int uiElement) const
{
    switch (uiElement)
    {
    case 0:
        return m_spHotkeyImages;
    case 1:
        return m_spLabel;
    default:
        return NULL;
    }

}
//---------------------------------------------------------------------------
void NiUIButton::SetPressed(bool bPressed)
{
    m_bPressed = bPressed;

    NiUIAtlasMap::TextColor kTextColor = NiUIAtlasMap::TEXT_ON_UI_DEFAULT;
    if (m_bPressed)
        kTextColor = NiUIAtlasMap::TEXT_ON_UI_DOWN;
    else if (m_bHighlit)
        kTextColor = NiUIAtlasMap::TEXT_ON_UI_HIGH;
    if (NiUIManager::GetUIManager() != NULL)
    {
        m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
            GetAtlasMap()->GetTextColor(kTextColor));
    }
    ReinitializeDisplayElements();
}
//---------------------------------------------------------------------------
