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

#include "NiUICheckBox.h"
#include "NiUIManager.h"

NiImplementRTTI(NiUICheckBox, NiUIBaseElement);
//---------------------------------------------------------------------------
const float NiUICheckBox::ms_fHotkeyImageWidth = 0.1f;
const float NiUICheckBox::ms_fHotkeyImageTotalWidth = 0.6f;
//---------------------------------------------------------------------------
NiUICheckBox::NiUICheckBox(const char* pcCheckBoxText) :
    NiUIBaseElement(true),
    m_spHotkeyImages(NULL),
    m_spLabel(NULL),
    m_bPressed(false),
    m_bHighlit(false),
    m_bMouseHeld(false)
{
    m_kHotkeyPressed.Initialize(this, &NiUICheckBox::HotkeyPressed);
    m_spHotkeyImages = NiNew NiUIHotkeyImage();
    NiColor kButtonTextColor = NiColor::BLACK;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("CheckBox initialized before the NiUIManager.  "
            "This can lead to unexpected results.");
    }
    else
    {
        kButtonTextColor = NiColor(
            NiUIManager::GetUIManager()->GetAtlasMap()->GetTextColor(
            NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
    }
    m_spLabel = NiNew NiUILabel(pcCheckBoxText,
        NiColorA(0.0f, 0.0f, 0.0f, 0.5f), kButtonTextColor);
    m_spLabel->SetParent(this);
    m_spLabel->SetAlignment(NiUILabel::VERT_MIDDLE, NiUILabel::LEFT);
    m_spHotkeyImages = NiNew NiUIHotkeyImage();
    m_spHotkeyImages->SetParent(this);
}
//---------------------------------------------------------------------------
NiUICheckBox::~NiUICheckBox()
{
    m_spHotkeyImages = NULL;
    m_spLabel = NULL;
}
//---------------------------------------------------------------------------
void NiUICheckBox::UpdateLayout()
{
    float fCheckBoxBorder = 0.0f;
    float fCheckLeftOffset = 0.0f;
    float fCheckTopOffset = 0.0f;

    float fCheckMaxHeight = (0.95f - (2.0f * fCheckBoxBorder)) * 
        m_kNSCRect.GetHeight();
    float fCheckSquare = fCheckMaxHeight;

    NiRect<float> kCheckOuterRect = OriginAndDimensionsToRect(0.0f, 0.0f,
        fCheckSquare, fCheckSquare);
    NiRect<float> kCheckInnerRect = ComputeAspectRatioPreservingInnerNSCRect(
        NiUIAtlasMap::UI_CHECK_BOX_HIGH, kCheckOuterRect);
    
    m_kCheckRect = OriginAndDimensionsToRect(
        m_kNSCRect.m_left + fCheckLeftOffset,
        m_kNSCRect.m_top + fCheckTopOffset + 
        (kCheckInnerRect.m_top - kCheckOuterRect.m_top),
        kCheckInnerRect.GetWidth(),
        kCheckInnerRect.GetHeight());
    
    float fCheckUOffset = fCheckLeftOffset + 
        m_kCheckRect.GetWidth() + fCheckBoxBorder;

    float fTotalWidth, fTotalHeight;
    fTotalWidth = m_kNSCRect.GetWidth() - fCheckUOffset;
    fTotalHeight = m_kNSCRect.GetHeight() - (2.0f * fCheckBoxBorder);

    // I want to make the space for the images approximately pixel-square.
    // However, in NSC, the width and height ratios for pixel size are 
    // different.  So, I will take the height as the edge length defined
    // in Y NSC.  Then find the edge len in X NSC that is the same number
    // of pixels.
    // The units for fEdgeLenInY_NSC could be considered Y_NSC
    // fScale shrinks the hotkey images slightly so they don't abut at the
    // top and bottom
    const float fScale = 0.85f;
    float fEdgeLenInY_NSC = (fTotalHeight * fScale) * 
        m_spHotkeyImages->GetTotalWidth();
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

#if defined(_WII)
    fCheckUOffset += 0.15f * m_kCheckRect.GetWidth();
#endif

    m_spLabel->SetOffset(fCheckUOffset, fCheckBoxBorder);
    m_spLabel->SetDimensions(fTotalWidth - fHotkeySpace,
        fTotalHeight);

    m_spHotkeyImages->SetOffset(
        m_kNSCRect.GetWidth() - fHotkeySpace, fCheckBoxBorder);
    m_spHotkeyImages->SetDimensions(fHotkeySpace, fTotalHeight);
}
//---------------------------------------------------------------------------
bool NiUICheckBox::AddKeyboardHotkey(NiInputKeyboard::KeyCode eKey,
    NiInputKeyboard::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUICheckBox hotkey events "
            "without first initializing NiUIManager singleton.");
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
bool NiUICheckBox::AddGamePadAxis(NiInputGamePad::DefaultAxis eAxis, 
    bool bUseBothDirs)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUICheckBox hotkey events "
            "without first initializing NiUIManager singleton.");
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
                    NiInputGamePad::NIGP_DEFAULT_X1_VERT, &m_kHotkeyPressed);
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
                NIASSERT(!"Unknown axis mapping");
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
bool NiUICheckBox::AddGamePadHotkey(NiInputGamePad::Button eButton, 
    NiInputGamePad::ButtonMask eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUICheckBox hotkey events "
            "without first initializing NiUIManager singleton.");
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
void NiUICheckBox::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString(
            "NiUICheckBox::ReinitializeDisplayElements called without "
            "NiUIManager initialization.  This is prohibited.");
        return;
    }

    if (m_bVisible)
    {
        NiColorA kBoxColor = NiColorA::WHITE;
        NiColorA kMarkColor = NiColorA::WHITE;
        const NiUIAtlasMap* pkAtlasMap = NiUIManager::GetUIManager()->
            GetAtlasMap();

        NiRect<float> kBoxBounds;
        NiRect<float> kMarkBounds;

        if (m_bHighlit)
        {
            kBoxBounds = pkAtlasMap->GetOuterBounds(
                NiUIAtlasMap::UI_CHECK_BOX_HIGH);
        }
        else
        {
            kBoxBounds = pkAtlasMap->GetOuterBounds(
                NiUIAtlasMap::UI_CHECK_BOX_DEFAULT);
        }

        if (m_bPressed)
            kMarkBounds = pkAtlasMap->GetOuterBounds(
                NiUIAtlasMap::UI_CHECK_MARK);
        else
            kMarkColor = NiColorA();
        
        bool bSuccess = InitializeQuad(m_spScreenElements, m_iBaseQuadIdx, 
            m_kCheckRect, kBoxBounds, kBoxColor);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUICheckBox."
                "  Probable cause: invalid uiBaseIdx passed in to "
                "NiUICheckBox::ReinitializeDisplayElements.");
        }

        bSuccess = InitializeQuad(m_spScreenElements, m_iBaseQuadIdx + 1, 
            m_kCheckRect, kMarkBounds, kMarkColor);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUICheckBox."
                "  Probable cause: invalid uiBaseIdx passed in to "
                "NiUICheckBox::ReinitializeDisplayElements.");
        }
    }
    else
    {
        bool bSuccess = HideQuad(m_spScreenElements, m_iBaseQuadIdx);

        bSuccess |= HideQuad(m_spScreenElements, m_iBaseQuadIdx + 1);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUICheckBox."
                "  Probable cause: invalid uiBaseIdx passed in to "
                "NiUICheckBox::ReinitializeDisplayElements.");
        }
    }
    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
void NiUICheckBox::HotkeyPressed(unsigned char ucCheckBoxState)
{
    PressLogic(ucCheckBoxState, true);
}
//---------------------------------------------------------------------------
void NiUICheckBox::PressLogic(unsigned char ucCheckBoxState, bool)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;
    
    if (ucCheckBoxState & NiUIManager::WASPRESSED)
    {
        m_bPressed = !m_bPressed;
        NiUIAtlasMap::TextColor kTextColor = 
            NiUIAtlasMap::TEXT_ON_UI_DEFAULT;
        if (m_bHighlit)
            kTextColor = NiUIAtlasMap::TEXT_ON_UI_HIGH;
        m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
            GetAtlasMap()->GetTextColor(kTextColor));
        m_kToggleEvent.EmitSignal(m_bPressed);
        ReinitializeDisplayElements();
    }
}
//---------------------------------------------------------------------------
bool NiUICheckBox::AddMouseMotion()
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUICheckBox hotkey events "
            "without first initializing NiUIManager singleton.");
        return false;
    }

    bool bHotkeyReserved = NiUIManager::GetUIManager()->ReserveMouse(
        m_kHotkeyPressed);

    if (bHotkeyReserved)
        m_spHotkeyImages->SetMouseMotion();

    return bHotkeyReserved;
}
//---------------------------------------------------------------------------
bool NiUICheckBox::AddMouseHotkey(NiInputMouse::Button eButton,
    NiInputMouse::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUICheckBox hotkey events "
            "without first initializing NiUIManager singleton.");
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
bool NiUICheckBox::SetFocus(bool bFocus)
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    NiUIBaseElement::SetFocus(bFocus);
    if (bFocus && !m_bHighlit)
    {
        m_bHighlit = true;
        m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
            GetAtlasMap()->GetTextColor(
            NiUIAtlasMap::TEXT_ON_UI_HIGH));
        ReinitializeDisplayElements();
    }
    else if (!bFocus && m_bHighlit)
    {
        m_bHighlit = false;
        m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
            GetAtlasMap()->GetTextColor(
            NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
        ReinitializeDisplayElements();
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiUICheckBox::MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY)
{
    if (NiUIManager::GetUIManager() == NULL)
        return false;

    bool bIsInside = NiUIBaseElement::MouseLocation(uiMouseX, uiMouseY);
    if (bIsInside && !m_bHighlit)
    {
        m_bHighlit = true;
        m_spLabel->SetTextColor(NiUIManager::GetUIManager()->
            GetAtlasMap()->GetTextColor(
            NiUIAtlasMap::TEXT_ON_UI_HIGH));
        ReinitializeDisplayElements();
    }
    return bIsInside;
}
//---------------------------------------------------------------------------
unsigned int NiUICheckBox::GetChildElementCount() const
{
    return 2;
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUICheckBox::GetChildElement(unsigned int uiElement) const
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
