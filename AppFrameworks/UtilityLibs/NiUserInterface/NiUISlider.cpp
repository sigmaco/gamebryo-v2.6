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

#include "NiUISlider.h"
#include "NiUIManager.h"

NiImplementRTTI(NiUISlider, NiUIBaseElement);
//---------------------------------------------------------------------------
const float NiUISlider::ms_fMarkRelativeWidth = 0.1f;
const float NiUISlider::ms_fTrackRelativeHeight = 0.25f;
//---------------------------------------------------------------------------
NiUISlider::NiUISlider(const char* pcText, float fValue, float fMin, 
    float fMax, unsigned int uiNumSteps, unsigned int uiDecimalPlaces) :
    NiUIBaseElement(true),
    m_spLabel(NULL),
#if defined(_WII)
    // The Wii's resolution requires a different default.
    m_fTextPercent(0.35f),
#else
    m_fTextPercent(0.25f),
#endif
    m_fTextVertPaddingPercent(0.01f),
    m_fMarkToTrackHeightRatio(2.0f),
    m_fMin(0.f),
    m_fMax(1.f),
    m_iStep(0),
    m_fValue(0),
    m_uiMaxStep(1),
    m_uiNumDecimalPlaces(uiDecimalPlaces),
    m_fLastStepTime(0.0f),
    m_fTimeStepIncrement(0.1f),
    m_bPressed(false),
    m_bHighlit(false),
    m_bMouseHeld(false),
    m_bReservedHotkeys(false)
{
    m_kIncrementPressed.Initialize(this, &NiUISlider::IncrementPressed);
    m_kDecrementPressed.Initialize(this, &NiUISlider::DecrementPressed);
    m_kSliderName = pcText;
    SetRange(fMin, fMax, uiNumSteps, false);
    SetValue(fValue);

    NiColor eButtonTextColor = NiColor::BLACK;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Button initialized before the NiUIManager.  "
            "This can lead to unexpected results.");
    }
    else
    {
        eButtonTextColor = NiColor(NiUIManager::GetUIManager()->GetAtlasMap()->
            GetTextColor(NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
    }
    m_spLabel = NiNew NiUILabel(NULL, NiColorA(0.0f, 0.0f, 0.0f, 0.5f), 
        eButtonTextColor);
    m_spLabel->SetParent(this);
    m_spLabel->SetAlignment(NiUILabel::VERT_MIDDLE, NiUILabel::LEFT);
    UpdateLabelText();

    SetFocusKeyboardHotkey(INPUT_INCREMENT, NiInputKeyboard::KEY_RIGHT);
    SetFocusKeyboardHotkey(INPUT_DECREMENT, NiInputKeyboard::KEY_LEFT);
    SetFocusGamePadHotkey(INPUT_INCREMENT, NiInputGamePad::NIGP_LRIGHT);
    SetFocusGamePadHotkey(INPUT_DECREMENT, NiInputGamePad::NIGP_LLEFT);
}
//---------------------------------------------------------------------------
NiUISlider::~NiUISlider()
{
    m_spLabel = NULL;
}
//---------------------------------------------------------------------------
void NiUISlider::UpdateLayout()
{
    float fTextNSCWitdh = m_fTextPercent * m_kNSCRect.GetWidth();

    m_spLabel->SetOffset(0.0f, 0.0f);

#if defined(_WII)
    // The Wii's resolution requires a different default.
    m_spLabel->SetDimensions(fTextNSCWitdh, 1.25f*m_kNSCRect.GetHeight());
#else
    m_spLabel->SetDimensions(fTextNSCWitdh, m_kNSCRect.GetHeight());
#endif

    m_kWorkingRect = m_kNSCRect;
    // Adjust to fill remaining space after text
    m_kWorkingRect.m_left += fTextNSCWitdh;

    // Compute the Mark dimensions
    m_fMarkNDCWidth = m_kWorkingRect.GetWidth() * ms_fMarkRelativeWidth;
    m_fMarkNDCHeight = m_kWorkingRect.GetHeight();

    // Compute the Track height and offset from Mark vertical start
    float fTrackHeight = m_fMarkNDCHeight * ms_fTrackRelativeHeight;
    float fTrackOffset = (m_fMarkNDCHeight - fTrackHeight) / 2.0f;

    m_kTrackOuterRect = m_kWorkingRect;

    // Adjust to fit into the space provided for Track
    m_kTrackOuterRect.m_top += fTrackOffset;
    m_kTrackOuterRect.m_bottom -= fTrackOffset;
    m_kTrackOuterRect.m_left += m_fMarkNDCWidth / 2.0f;
    m_kTrackOuterRect.m_right -= m_fMarkNDCWidth / 2.0f;

    m_kTrackInnerRect = ComputeInnerNSCRectPixelAccurateInner(
        NiUIAtlasMap::UI_SLIDER_TRACK_DEFAULT, m_kTrackOuterRect);
}
//---------------------------------------------------------------------------
bool NiUISlider::SetFocusKeyboardHotkey(InputAction eAction, 
    NiInputKeyboard::KeyCode eKey, NiInputKeyboard::Modifiers eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUISlider hotkey events without "
            "first initializing NiUIManager singleton.");
        return false;
    }
    
    m_akKeyboardHotkeys[eAction].eKey = eKey;
    m_akKeyboardHotkeys[eAction].eModifier = eModifier;

    return true;
}
//---------------------------------------------------------------------------
bool NiUISlider::SetFocusGamePadHotkey(InputAction eAction, 
    NiInputGamePad::Button eButton, NiInputGamePad::ButtonMask eModifier)
{
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString("Can not add any NiUISlider hotkey events without "
            "first initializing NiUIManager singleton.");
        return false;
    }

    m_akGamePadHotkeys[eAction].eButton = eButton;
    m_akGamePadHotkeys[eAction].eModifier = eModifier;

    return true;
}
//---------------------------------------------------------------------------
void NiUISlider::ReinitializeDisplayElements()
{
    if (m_spScreenElements == NULL)
        return;
    if (NiUIManager::GetUIManager() == NULL)
    {
        NiOutputDebugString(
            "NiUISlider::ReinitializeDisplayElements called without "
            "NiUIManager initialization.  This is prohibited.");
        return;
    }
    
    if (m_bVisible)
    {
        const NiUIAtlasMap* pkAtlas = 
            NiUIManager::GetUIManager()->GetAtlasMap();
        
        NiRect<float> kTrackOuterUVs;
        NiRect<float> kTrackInnerUVs;
        NiUIAtlasMap::UIElement kTrackStateName;
        NiUIAtlasMap::UIElement kMarkStateName;

        if (m_bHighlit)
        {
            kTrackStateName = NiUIAtlasMap::UI_SLIDER_TRACK_HIGH;
            kMarkStateName = NiUIAtlasMap::UI_SLIDER_MARK_HIGH;
        }
        else
        {
            kTrackStateName = NiUIAtlasMap::UI_SLIDER_TRACK_DEFAULT;
            kMarkStateName = NiUIAtlasMap::UI_SLIDER_MARK_DEFAULT;
        }

        kTrackOuterUVs = pkAtlas->GetOuterBounds(kTrackStateName);
        kTrackInnerUVs = pkAtlas->GetInnerBounds(kTrackStateName);

        bool bSuccess = InitializeNineQuad(m_spScreenElements, m_iBaseQuadIdx,
            m_kTrackOuterRect, m_kTrackInnerRect, kTrackOuterUVs, 
            kTrackInnerUVs, NiColorA::WHITE);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUISlider.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUISlider::ReinitializeDisplayElements.");
        }

        // Compute the current value of the Mark in NDC coords
        float fNormalizedValue;
        
        if (m_fMax == m_fMin)
        {
            fNormalizedValue = 0.5f;
        }
        else
        {
            fNormalizedValue = (m_fValue - m_fMin) / (m_fMax - m_fMin);
        }
        float fNDCMarkHorzOrigin = m_kTrackOuterRect.m_left + 
            fNormalizedValue * m_kTrackOuterRect.GetWidth();
        float fNDCMarkVertOrigin = m_kWorkingRect.m_top + 0.5f * 
            m_kWorkingRect.GetHeight();
        float fNDCMarkHalfWidth = m_fMarkNDCWidth / 2.0f;
        float fNDCMarkHalfHeight = m_fMarkNDCHeight / 2.0f;

        NiRect<float> kMarkOuterRect;
        kMarkOuterRect.m_left = fNDCMarkHorzOrigin - fNDCMarkHalfWidth;
        kMarkOuterRect.m_right = fNDCMarkHorzOrigin + fNDCMarkHalfWidth;
        kMarkOuterRect.m_top = fNDCMarkVertOrigin - fNDCMarkHalfHeight;
        kMarkOuterRect.m_bottom = fNDCMarkVertOrigin + fNDCMarkHalfHeight;

        NiRect<float> kMarkInnerRect = ComputeInnerNSCRectPixelAccurateOuter(
            kMarkStateName, kMarkOuterRect);

        NiRect<float> kMarkOuterUVs;
        NiRect<float> kMarkInnerUVs;

        kMarkOuterUVs = pkAtlas->GetOuterBounds(kMarkStateName);
        kMarkInnerUVs = pkAtlas->GetInnerBounds(kMarkStateName);

        bSuccess = InitializeNineQuad(m_spScreenElements, 
            m_iBaseQuadIdx + 9, kMarkOuterRect, kMarkInnerRect, 
            kMarkOuterUVs, kMarkInnerUVs, NiColorA::WHITE);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUISlider.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUISlider::ReinitializeDisplayElements.");
        }
    }
    else
    {
        bool bSuccess = HideNineQuad(m_spScreenElements, m_iBaseQuadIdx);
        bSuccess |= HideNineQuad(m_spScreenElements, 
            m_iBaseQuadIdx + 9);

        if (!bSuccess)
        {
            NiOutputDebugString("Unable to properly initialize a NiUISlider.  "
                "Probable cause: invalid uiBaseIdx passed in to "
                "NiUISlider::ReinitializeDisplayElements.");
        }
    }

    ReinitializeChildDisplayElements();
}
//---------------------------------------------------------------------------
void NiUISlider::UpdateLabelText()
{
    char acText[256];
    NiSprintf(acText, 256, "%s: %.*f", (const char*) m_kSliderName,
        m_uiNumDecimalPlaces, m_fValue);
    if (m_spLabel)
        m_spLabel->SetText(acText);
}
//---------------------------------------------------------------------------
bool NiUISlider::SetFocus(bool bFocus)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    if (pkManager == NULL)
        return false;

    NiUIBaseElement::SetFocus(bFocus);
    if (bFocus && !m_bHighlit)
    {
        //m_bHotkeyCaused = true;
        m_bHighlit = true;
        if (!m_bPressed)
        {
            m_spLabel->SetTextColor(pkManager->
                GetAtlasMap()->GetTextColor(
                NiUIAtlasMap::TEXT_ON_UI_HIGH));
        }
        ReinitializeDisplayElements();
    }
    else if (!bFocus && m_bHighlit)// && m_bHotkeyCaused)
    {
        //m_bHotkeyCaused = false;
        m_bHighlit = false;
        if (!m_bPressed)
        {
            m_spLabel->SetTextColor(pkManager->
                GetAtlasMap()->GetTextColor(
                NiUIAtlasMap::TEXT_ON_UI_DEFAULT));
        }
        ReinitializeDisplayElements();
    }

    if (bFocus && !m_bReservedHotkeys)
    {
        bool bHotkeyReserved = true;
        unsigned char ucButtonState = (NiUIManager::ISPRESSED | 
            NiUIManager::WASPRESSED | NiUIManager::WASRELEASED);
        bHotkeyReserved = pkManager->ReserveKeyboardButton(
            m_akKeyboardHotkeys[INPUT_INCREMENT].eKey, &m_kIncrementPressed, 
            ucButtonState, m_akKeyboardHotkeys[INPUT_INCREMENT].eModifier);

        bHotkeyReserved = pkManager->ReserveGamePadButton(
            m_akGamePadHotkeys[INPUT_INCREMENT].eButton, &m_kIncrementPressed, 
            ucButtonState, m_akGamePadHotkeys[INPUT_INCREMENT].eModifier);

        bHotkeyReserved = pkManager->ReserveKeyboardButton(
            m_akKeyboardHotkeys[INPUT_DECREMENT].eKey, &m_kDecrementPressed, 
            ucButtonState, m_akKeyboardHotkeys[INPUT_DECREMENT].eModifier);

        bHotkeyReserved = pkManager->ReserveGamePadButton(
            m_akGamePadHotkeys[INPUT_DECREMENT].eButton, &m_kDecrementPressed, 
            ucButtonState, m_akGamePadHotkeys[INPUT_DECREMENT].eModifier);

        m_bReservedHotkeys = true;
        
    }
    else if (!bFocus && m_bReservedHotkeys)
    {
        pkManager->UnreserveKeyboardButton(
            m_akKeyboardHotkeys[INPUT_INCREMENT].eKey, &m_kIncrementPressed,
            m_akKeyboardHotkeys[INPUT_INCREMENT].eModifier);

        pkManager->UnreserveGamePadButton(
            m_akGamePadHotkeys[INPUT_INCREMENT].eButton, &m_kIncrementPressed,
            m_akGamePadHotkeys[INPUT_INCREMENT].eModifier);
        
        pkManager->UnreserveKeyboardButton(
            m_akKeyboardHotkeys[INPUT_DECREMENT].eKey, &m_kDecrementPressed,
            m_akKeyboardHotkeys[INPUT_DECREMENT].eModifier);

        pkManager->UnreserveGamePadButton(
            m_akGamePadHotkeys[INPUT_DECREMENT].eButton, &m_kDecrementPressed,
            m_akGamePadHotkeys[INPUT_DECREMENT].eModifier);
        
        m_bReservedHotkeys = false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiUISlider::MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY)
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
unsigned int NiUISlider::GetChildElementCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
NiUIBaseElement* NiUISlider::GetChildElement(unsigned int uiElement) const
{
    switch (uiElement)
    {
    case 0:
        return m_spLabel;
    default:
        return NULL;
    }

}
//---------------------------------------------------------------------------
