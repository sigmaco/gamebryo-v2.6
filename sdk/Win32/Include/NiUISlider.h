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

#ifndef NISLIDER_H
#define NISLIDER_H

#include "NiUserInterfaceLibType.h"
#include "NiUIHotkeyImage.h"
#include "NiUILabel.h"
#include "NiUIManager.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUISlider : public NiUIBaseElement
{
    NiDeclareRTTI;
public:

    NiUISlider(const char* pcText = "", float fValue = 0.0f, 
        float fMin = 0.0f, float fMax = 100.0f, unsigned int uiNumSteps = 101, 
        unsigned int uiDecimalPlaces = 2);
    virtual ~NiUISlider();

    // Rendering functions
    virtual void Draw(NiRenderer* pkRenderer);

    // Set and Get functions
    virtual void SetVisible(bool bVisible);
    inline void SetPressed(bool bPressed = true);
    inline bool IsPressed() const;
    inline void SetText(const char* pcText);
    inline void SetValue(float fValue);
    inline void SetStep(int iValue);
    inline void SetRange(float fMin, float fMax, unsigned int uiNumSteps,
        bool bUpdateValue);

    // Event functions
    virtual void EnterPressed(unsigned char ucButtonState);
    virtual void LeftMouseButton(unsigned char ucButtonState);
    virtual bool MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY);
    inline void SubscribeToValueChangedEvent(NiUIBaseSlot1<float>* pkSlot);

    enum InputAction
    {
        INPUT_INCREMENT = 0,
        INPUT_DECREMENT,
        INPUT_MAX
    };

    // Hotkeys to increment/decrement the slider when UI element has focus
    bool SetFocusKeyboardHotkey(InputAction eAction, 
        NiInputKeyboard::KeyCode eKey,
        NiInputKeyboard::Modifiers eModifier = NiInputKeyboard::KMOD_NONE);
    bool SetFocusGamePadHotkey(InputAction eAction, 
        NiInputGamePad::Button eButton, 
        NiInputGamePad::ButtonMask eModifier = NiInputGamePad::NIGP_MASK_NONE);

    bool AddMouseMotion();
    virtual bool SetFocus(bool bFocus);

    virtual unsigned int GetChildElementCount() const;
    virtual NiUIBaseElement* GetChildElement(unsigned int uiElement) const;

protected:
    static const float ms_fMarkRelativeWidth;
    static const float ms_fTrackRelativeHeight;

    virtual void UpdateLayout();
    void UpdateLabelText();
    void HandleCursor(unsigned char ucButtonState);

    struct KeyboardHotkey
    {
    public:
        NiInputKeyboard::KeyCode eKey;
        NiInputKeyboard::Modifiers eModifier;
    };
    
    struct GamePadHotkey
    {
    public:
        NiInputGamePad::Button eButton;
        NiInputGamePad::ButtonMask eModifier;
    };

    KeyboardHotkey m_akKeyboardHotkeys[INPUT_MAX];
    GamePadHotkey  m_akGamePadHotkeys[INPUT_MAX];

    enum {NUM_SLIDER_QUADS = 18};
    virtual unsigned int NumQuadsRequired();
    virtual void ReinitializeDisplayElements();

    // Communication members
    NiUISignal1<float> m_kValueChangedEvent;
    NiUIMemberSlot1<NiUISlider, unsigned char> m_kIncrementPressed;
    NiUIMemberSlot1<NiUISlider, unsigned char> m_kDecrementPressed;

    inline void IncrementPressed(unsigned char ucButtonState);
    inline void DecrementPressed(unsigned char ucButtonState);
    
    NiUILabelPtr m_spLabel;

    NiRect<float> m_kStoredRect;
    NiRect<float> m_kWorkingRect;
    NiRect<float> m_kTrackOuterRect;
    NiRect<float> m_kTrackInnerRect;

    float m_fMarkNDCWidth;
    float m_fMarkNDCHeight;

    float m_fTextPercent;
    float m_fTextVertPaddingPercent;
    float m_fMarkToTrackHeightRatio;

    float m_fMin;
    float m_fMax;
    int m_iStep;
    float m_fValue;
    unsigned int m_uiMaxStep;
    unsigned int m_uiNumDecimalPlaces;

    float m_fLastStepTime;
    float m_fTimeStepIncrement;

    bool m_bPressed;
    bool m_bHighlit;
    bool m_bMouseHeld;

    bool m_bReservedHotkeys;

    NiString m_kSliderName;
};
NiSmartPointer(NiUISlider);
//---------------------------------------------------------------------------
#include "NiUISlider.inl"

#endif
