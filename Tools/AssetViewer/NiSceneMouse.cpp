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

// NiSceneMouse.cpp

#include "stdafx.h"
#include "NiSceneMouse.h"
#include "NiSceneKeyboard.h"

//---------------------------------------------------------------------------
NiSceneMouse::NiSceneMouse() : NiDevice(5, true, 0.5f)
{

    m_abMouseState[0] = false;
    m_abMouseState[1] = false;
    m_abMouseState[2] = false;
    m_abMouseState[3] = false;
    m_iPreviousMouseHor = -1;
    m_iPreviousMouseVer = -1;
    m_iCurrentMouseHor = 0;
    m_iCurrentMouseVer = 0;
    m_sWheelDelta = 0;

}
//---------------------------------------------------------------------------
long NiSceneMouse::GetModifiers()
{
    long lResult = MMOD_NONE;
    if (m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_CONTROL))
        lResult |= MMOD_CONTROL;
    if (m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_MENU))
        lResult |= MMOD_MENU;
    if (m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_LWIN) || 
        m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_RWIN))
        lResult |= MMOD_WIN;
    if (m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_SHIFT))
        lResult |= MMOD_SHIFT;
    if (ButtonIsDown(SCENEMOUSE_LBUTTON))
        lResult |= MMOD_LEFT;
    if (ButtonIsDown(SCENEMOUSE_RBUTTON))
        lResult |= MMOD_RIGHT;
    if (ButtonIsDown(SCENEMOUSE_MBUTTON))
        lResult |= MMOD_MIDDLE;
    if (ButtonIsDown(SCENEMOUSE_WHEEL))
        lResult |= MMOD_WHEEL;
    return lResult;
}
//---------------------------------------------------------------------------
bool NiSceneMouse::ButtonIsDown(Button eButton)
{
    if (eButton == SCENEMOUSE_NOBUTTON)
        return false;
    return (m_abMouseState[eButton - 1]);
}
//---------------------------------------------------------------------------
void NiSceneMouse::GetMouseLocation(float* pfHor, float* pfVer,
    NiWindowRef pWnd)
{
    POINT kPt;
    RECT kRect;

    GetCursorPos(&kPt);
    GetWindowRect(pWnd, &kRect);

    if (!m_bFullscreen)
        ScreenToClient(pWnd, &kPt);

    *pfHor = (kPt.x - kRect.left) / (float)(kRect.right - kRect.left);
    *pfVer = 1.0f - (kPt.y - kRect.top) / (float)(kRect.bottom - kRect.top);
}
//---------------------------------------------------------------------------
void NiSceneMouse::GetMouseLocation(int* piHor, int* piVer, NiWindowRef pWnd)
{
    POINT kPt;

    GetCursorPos(&kPt);

    if (!m_bFullscreen)
        ScreenToClient(pWnd, &kPt);

    *piHor = kPt.x;
    *piVer = kPt.y;
}
//---------------------------------------------------------------------------
void NiSceneMouse::RecordButtonPress(Button eButton)
{
    m_abMouseState[eButton-1] = true;
    DeviceStateChange(NiDevice::PRESSED_EVENT, eButton);
}
//---------------------------------------------------------------------------
void NiSceneMouse::RecordButtonRelease(Button eButton)
{
    m_abMouseState[eButton - 1] = false;
    DeviceStateChange(NiDevice::RELEASED_EVENT, eButton);
}
//---------------------------------------------------------------------------
void NiSceneMouse::RecordMouseMove(int iX, int iY)
{
    UpdatePreviousMousePosition(m_iCurrentMouseHor, m_iCurrentMouseVer);
    m_iCurrentMouseHor = iX;
    m_iCurrentMouseVer = iY;
}
//---------------------------------------------------------------------------
void NiSceneMouse::RecordWheelMoved(short sDelta)
{
    m_sWheelDelta += sDelta;
    RecordButtonPress(SCENEMOUSE_WHEEL);
}
//---------------------------------------------------------------------------
void NiSceneMouse::GetCurrentMouseLocation(int& iX, int& iY)
{
    iX = m_iCurrentMouseHor;
    iY = m_iCurrentMouseVer;
}
//---------------------------------------------------------------------------
void NiSceneMouse::SetKeyboard(NiSceneKeyboard* pkKeyboard)
{
    m_pkKeyboard = pkKeyboard;
}
//---------------------------------------------------------------------------
void NiSceneMouse::SetFullscreenMode(bool bFullscreen)
{
    m_bFullscreen = bFullscreen;
}
//---------------------------------------------------------------------------
unsigned int NiSceneMouse::GetMouseButtons()
{
    unsigned int uiButtons = 0;
    if(ButtonIsDown(SCENEMOUSE_WHEEL))
        uiButtons |= 8;
    if(ButtonIsDown(SCENEMOUSE_MBUTTON))
        uiButtons |= 4;
    if(ButtonIsDown(SCENEMOUSE_RBUTTON))
        uiButtons |= 2;
    if(ButtonIsDown(SCENEMOUSE_LBUTTON))
        uiButtons |= 1;

    return uiButtons;     
}
//---------------------------------------------------------------------------
void NiSceneMouse::GetPreviousMouseLocation(int& iHor, int& iVer)
{
    iHor = m_iPreviousMouseHor;
    iVer = m_iPreviousMouseVer;
}
//---------------------------------------------------------------------------
void NiSceneMouse::ResetPreviousMouseLocation()
{
    m_iPreviousMouseHor = -1;
    m_iPreviousMouseVer = -1;
}
//---------------------------------------------------------------------------
void NiSceneMouse::UpdatePreviousMousePosition(int iHor, int iVer)
{
    m_iPreviousMouseHor = iHor;
    m_iPreviousMouseVer = iVer;
}
//---------------------------------------------------------------------------
short NiSceneMouse::GetWheelDelta()
{
    return m_sWheelDelta;
}
//---------------------------------------------------------------------------
void NiSceneMouse::UpdateDevice()
{
    NiDevice::UpdateDevice();
    m_sWheelDelta = 0;
    RecordButtonRelease(SCENEMOUSE_WHEEL);
    SetDeviceState(NOT_PRESSED, SCENEMOUSE_WHEEL);
    if(GetMouseButtons() != SCENEMOUSE_NOBUTTON)
        SetDeviceState(NOT_PRESSED, SCENEMOUSE_NOBUTTON);
    else
        SetDeviceState(CONTINUOUS_PRESSED, SCENEMOUSE_NOBUTTON);
}
//---------------------------------------------------------------------------
