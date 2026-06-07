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

#ifndef NIUIHOTKEYIMAGE_H
#define NIUIHOTKEYIMAGE_H

#include "NiUserInterfaceLibType.h"
#include <NiInputMouse.h>
#include <NiInputGamePad.h>
#include <NiInputKeyboard.h>
#include "NiUIBaseElement.h"
#include "NiUIImage.h"
#include "NiUILabel.h"
#include "NiUIAtlasMap.h"

//---------------------------------------------------------------------------
class NIUSERINTERFACE_ENTRY NiUIHotkeyImage : public NiUIBaseElement
{
    NiDeclareRTTI;
public:
    NiUIHotkeyImage();
    virtual ~NiUIHotkeyImage();

    void SetKeyboardHotkey(NiInputKeyboard::KeyCode eKey,
        NiInputKeyboard::Modifiers eModifier = NiInputKeyboard::KMOD_NONE);
    void SetGamePadAxis(NiInputGamePad::DefaultAxis eAxis, 
        bool bUseBothDirs = false);
    void SetGamePadHotkey(NiInputGamePad::Button eButton, 
        NiInputGamePad::ButtonMask eModifier = NiInputGamePad::NIGP_MASK_NONE);
    void SetMouseHotkey(NiInputMouse::Button eButton,
        NiInputMouse::Modifiers eModifier = NiInputMouse::MMOD_NONE);
    void SetMouseMotion();

    virtual void Draw(NiRenderer* pkRenderer);
    virtual void SetVisible(bool bVisible);

    virtual void EnterPressed(unsigned char ucButtonState);
    virtual void LeftMouseButton(unsigned char ucButtonState);
    inline float GetTotalWidth();
    
    virtual unsigned int GetChildElementCount() const;
    virtual NiUIBaseElement* GetChildElement(unsigned int uiElement) const;

protected:
    virtual unsigned int NumQuadsRequired();
    virtual void UpdateLayout();
    
    virtual void InsertHotkeyElement(NiUIAtlasMap::UIElement eElement,
        float fWidth, bool bInsertSpacerPrior = true, 
        const char* pcLabelText = NULL, NiUIAtlasMap::TextColor eTextColor = 
        NiUIAtlasMap::TEXT_ON_KEYBOARD_BUTTON);

    inline bool IsDoubleWide(NiInputKeyboard::KeyCode eKey);


    // The texture coords for the key itself
    NiTObjectArray<NiUIImagePtr> m_kImages;
    // NULL if there is no text for this image
    // otherwise, this stores the text for the key
    NiTObjectArray<NiUILabelPtr> m_kLabels;
    // This contains a 1 or 2 depending on the width of the image
    // most are 1.  Shift, ctrl, etc. are 2
    NiTPrimitiveArray<float> m_kImageWidth;

    NiTPrimitiveSet<NiUIAtlasMap::UIElement> m_kElementIds;

    static const char* ms_pcKeyNameMap[NiInputKeyboard::KEY_TOTAL_COUNT];
    static const float ms_fSpacerWidth;
};
NiSmartPointer(NiUIHotkeyImage);
//---------------------------------------------------------------------------
#include "NiUIHotkeyImage.inl"

#endif

