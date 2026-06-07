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

#ifndef NICHECKBOX_H
#define NICHECKBOX_H

#include "NiUserInterfaceLibType.h"
#include "NiUIHotkeyImage.h"
#include "NiUILabel.h"
//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUICheckBox : public NiUIBaseElement
{
    NiDeclareRTTI;
public:

    NiUICheckBox(const char* pcCheckBoxText = "");
    virtual ~NiUICheckBox();

    // Rendering functions
    virtual void Draw(NiRenderer* pkRenderer);

    // Set and Get functions
    virtual void SetVisible(bool bVisible);
    inline void SetPressed(bool bPressed = true);
    inline bool IsPressed() const;
    inline void SetText(const char* pcCheckBoxText);

    // Event functions
    virtual void EnterPressed(unsigned char ucCheckBoxState);
    virtual void LeftMouseButton(unsigned char ucCheckBoxState);
    virtual bool MouseLocation(unsigned int uiMouseX, unsigned int uiMouseY);
    inline void SubscribeToToggleEvent(NiUIBaseSlot1<bool>* pkSlot);

    // These return true if the hotkey could be reserved
    bool AddKeyboardHotkey(NiInputKeyboard::KeyCode eKey,
        NiInputKeyboard::Modifiers eModifier = NiInputKeyboard::KMOD_NONE);
    bool AddGamePadAxis(NiInputGamePad::DefaultAxis eAxis, 
        bool bUseBothDirs = false);
    bool AddGamePadHotkey(NiInputGamePad::Button eButton, 
        NiInputGamePad::ButtonMask eModifier = NiInputGamePad::NIGP_MASK_NONE);
    bool AddMouseHotkey(NiInputMouse::Button eButton,
        NiInputMouse::Modifiers eModifier = NiInputMouse::MMOD_NONE);
    bool AddMouseMotion();
    virtual bool SetFocus(bool bFocus);

    virtual unsigned int GetChildElementCount() const;
    virtual NiUIBaseElement* GetChildElement(unsigned int uiElement) const;

protected:
    virtual unsigned int NumQuadsRequired();
    virtual void ReinitializeDisplayElements();
    virtual void UpdateLayout();

    // Communication members
    NiUISignal0 m_kPressEvent;
    NiUISignal0 m_kReleaseEvent;
    NiUISignal1<bool> m_kToggleEvent;
    NiUIMemberSlot1<NiUICheckBox, unsigned char> m_kHotkeyPressed;
    void HotkeyPressed(unsigned char ucCheckBoxState);
    void PressLogic(unsigned char ucCheckBoxState, bool bHotkeyCaused);

    static const float ms_fHotkeyImageWidth;
    static const float ms_fHotkeyImageTotalWidth;
    NiUIHotkeyImagePtr m_spHotkeyImages;
    NiUILabelPtr m_spLabel;

    NiRect<float> m_kStoredRect;
    NiRect<float> m_kCheckRect;
    bool m_bPressed;
    bool m_bHighlit;
    bool m_bMouseHeld;
};
NiSmartPointer(NiUICheckBox);
//---------------------------------------------------------------------------
#include "NiUICheckBox.inl"

#endif
