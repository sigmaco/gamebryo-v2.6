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

// NifRenderViewBufferedUIState.cpp

#include "stdafx.h"
#include "NifRenderViewBufferedUIState.h"
#include "NiSceneMouse.h"
#include "NifUserPreferences.h"

//---------------------------------------------------------------------------
CNifRenderViewBufferedUIState::CNifRenderViewBufferedUIState(
    CNifRenderView* pkView) : CNifRenderViewUIState(pkView)
{
    m_pkView = pkView;
    m_eForwardKey = NiSceneKeyboard::KEY_NOKEY;
    m_eBackwardKey = NiSceneKeyboard::KEY_NOKEY;
    m_eLeftKey = NiSceneKeyboard::KEY_NOKEY;
    m_eRightKey = NiSceneKeyboard::KEY_NOKEY;
    m_eUpKey = NiSceneKeyboard::KEY_NOKEY;
    m_eDownKey = NiSceneKeyboard::KEY_NOKEY;
    m_ePitchUpKey = NiSceneKeyboard::KEY_NOKEY;
    m_ePitchDownKey = NiSceneKeyboard::KEY_NOKEY;
    m_eYawLeftKey = NiSceneKeyboard::KEY_NOKEY;
    m_eYawRightKey = NiSceneKeyboard::KEY_NOKEY;
    m_kTranslateSpeed = NiPoint3::ZERO;
    m_bActive = false;
    m_fYawSpeed = 0.0f;
    m_fPitchSpeed = 0.0f;
    m_pkKeyboard = NULL;
    m_pkMouse = NULL;
    m_fLastTime = 0.0f;
    m_fAccumTime = 0.0f;
    m_fDeltaTime = 0.0f;
    m_bReInitialize = false;
}
//---------------------------------------------------------------------------
CNifRenderViewBufferedUIState::~CNifRenderViewBufferedUIState()
{ 
    m_pkKeyboard = NULL;
    m_pkMouse = NULL;
    m_pkView = NULL;
    m_kLastPoint.x = -1;
    m_kLastPoint.y = -1;
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::SetKeyboard(NiSceneKeyboard* pkKeyboard)
{
    NIASSERT(pkKeyboard);
    this->m_pkKeyboard = pkKeyboard;
    if(m_pkMouse)
        m_pkMouse->SetKeyboard(m_pkKeyboard);
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::SetMouse(NiSceneMouse* pkMouse)
{
    NIASSERT(pkMouse);
    m_pkMouse = pkMouse;
    if(m_pkKeyboard)
        m_pkMouse->SetKeyboard(m_pkKeyboard);
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::Initialize()
{
    m_bActive = InitUserPrefs() && m_pkKeyboard && m_pkMouse;
    m_fLastTime = NiGetCurrentTimeInSec();
    m_fAccumTime = 0.0f;
    m_fDeltaTime = 0.0f;
    m_fWheelDelta = 0.0f;
    m_bReInitialize = false;
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::CalculateTime()
{
    float fTime = NiGetCurrentTimeInSec();
    m_fDeltaTime = fTime - m_fLastTime;
    m_fAccumTime +=  m_fDeltaTime;
    m_fLastTime = fTime;
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnRButtonDblClk(UINT nFlags, 
                                                    CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->SetDeviceState(NiDevice::DOUBLE_CLICKED, 
            NiSceneMouse::SCENEMOUSE_RBUTTON);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnRButtonDown(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordButtonPress(NiSceneMouse::SCENEMOUSE_RBUTTON);    
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnRButtonUp(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordButtonRelease(NiSceneMouse::SCENEMOUSE_RBUTTON);     
    }
}
//---------------------------------------------------------------------------
BOOL CNifRenderViewBufferedUIState::OnMouseWheel(UINT nFlags, short zDelta,
    CPoint pt)
{
    if(m_pkMouse)
    {
        m_pkMouse->RecordWheelMoved(zDelta);
    }

    m_fWheelDelta += ((float)zDelta)/120.0f;

    return FALSE;
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnMouseMove(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordMouseMove(point.x, point.y);    
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnLButtonUp(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordButtonRelease(NiSceneMouse::SCENEMOUSE_LBUTTON);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnLButtonDown(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordButtonPress(NiSceneMouse::SCENEMOUSE_LBUTTON);
    
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnLButtonDblClk(UINT nFlags,
    CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->SetDeviceState(NiDevice::DOUBLE_CLICKED, 
            NiSceneMouse::SCENEMOUSE_LBUTTON);    
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnMButtonUp(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordButtonRelease(NiSceneMouse::SCENEMOUSE_MBUTTON);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnMButtonDown(UINT nFlags, CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->RecordButtonPress(NiSceneMouse::SCENEMOUSE_MBUTTON);
    
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnMButtonDblClk(UINT nFlags,
    CPoint point)
{ 
    if(m_pkMouse)
    {
        m_pkMouse->SetDeviceState(NiDevice::DOUBLE_CLICKED, 
            NiSceneMouse::SCENEMOUSE_MBUTTON);    
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnKeyUp(UINT nChar, UINT nRepCnt,
    UINT nFlags)
{ 
    if(m_pkKeyboard)
    {
        m_pkKeyboard->RecordKeyRelease(m_pkKeyboard
            ->NiTranslateKeyCode(nChar));
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnKeyDown(UINT nChar, UINT nRepCnt, 
                                              UINT nFlags)
{ 
    if(m_pkKeyboard)
    {
        if(m_pkKeyboard->KeyIsDown(NiSceneKeyboard::KEY_MENU))
            return;

        m_pkKeyboard->RecordKeyPress(m_pkKeyboard->NiTranslateKeyCode(nChar));
    }
}
//---------------------------------------------------------------------------
bool CNifRenderViewBufferedUIState::InitUserPrefs()
{
    CNifUserPreferences::Lock();
    CNifUserPreferences* pkPrefs = 
        CNifUserPreferences::AccessUserPreferences();

    if(!pkPrefs)
        return false;

    m_kTranslateSpeed = pkPrefs->GetTranslateSpeed();
    m_fYawSpeed = pkPrefs->GetYawSpeed();
    m_fPitchSpeed = pkPrefs->GetPitchSpeed();
    m_kKeyboardShortcuts = pkPrefs->GetKeyboardShortcuts();

    if(!m_pkKeyboard)
        return false;

    CNifKeyboardShortcuts::ShortcutFlag kFlag;
    UINT uiKey;
    
    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_MOVE_FORWARD;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eForwardKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_MOVE_BACK;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eBackwardKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_MOVE_LEFT;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eLeftKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_MOVE_RIGHT;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eRightKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_MOVE_UP;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eUpKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_MOVE_DOWN;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eDownKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_PITCH_UP;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_ePitchUpKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_PITCH_DOWN;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_ePitchDownKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_YAW_LEFT;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eYawLeftKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    kFlag = CNifKeyboardShortcuts::KBD_CAMERA_YAW_RIGHT;
    uiKey = m_kKeyboardShortcuts.GetKeyboardShortcut(kFlag);
    m_eYawRightKey = m_pkKeyboard->NiTranslateKeyCode(uiKey);

    CNifUserPreferences::UnLock();
    return true;
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::UpdateDevices()
{
    if(!m_pkKeyboard || !m_pkMouse)
        return;

    m_pkKeyboard->UpdateDevice();
    m_pkMouse->UpdateDevice();

    if(m_pkKeyboard->KeyIsDown(m_eForwardKey))
    {
        OnForwardKey(m_pkKeyboard->GetDeviceState(m_eForwardKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eBackwardKey))
    {
        OnBackwardKey(m_pkKeyboard->GetDeviceState(m_eBackwardKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eUpKey))
    {
        OnUpKey(m_pkKeyboard->GetDeviceState(m_eUpKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eDownKey))
    {
        OnDownKey(m_pkKeyboard->GetDeviceState(m_eDownKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eLeftKey))
    {
        OnLeftKey(m_pkKeyboard->GetDeviceState(m_eLeftKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eRightKey))
    {
        OnRightKey(m_pkKeyboard->GetDeviceState(m_eRightKey));
    }
    
    if(m_pkKeyboard->KeyIsDown(m_ePitchUpKey))
    {
        OnPitchUpKey(m_pkKeyboard->GetDeviceState(m_ePitchUpKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_ePitchDownKey))
    {
        OnPitchDownKey(m_pkKeyboard->GetDeviceState(m_ePitchDownKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eYawLeftKey))
    {
        OnYawLeftKey(m_pkKeyboard->GetDeviceState(m_eYawLeftKey));
    }

    if(m_pkKeyboard->KeyIsDown(m_eYawRightKey))
    {
        OnYawRightKey(m_pkKeyboard->GetDeviceState(m_eYawRightKey));
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::Update()
{
    if(m_bReInitialize)
        Initialize();
    CalculateTime();
    UpdateDevices();
    m_fWheelDelta = 0.0f;
}
//---------------------------------------------------------------------------
void CNifRenderViewBufferedUIState::OnKillFocus(CWnd* pNewWnd)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
        pkDoc->Lock();

    NiDevice::DeviceState* pkStateArray = NULL;
    if(m_pkKeyboard)
    {
        pkStateArray = m_pkKeyboard->GetDeviceStateArray();
        for(unsigned int ui = 0; pkStateArray && 
            ui < m_pkKeyboard->GetDeviceStateArraySize(); ui++)
        {
            pkStateArray[ui] = NiDevice::NOT_PRESSED;
        }
    }
    if(m_pkMouse)
    {
        pkStateArray = m_pkMouse->GetDeviceStateArray();
        for(unsigned int ui = 0; pkStateArray && 
            ui < m_pkMouse->GetDeviceStateArraySize(); ui++)
        {
            pkStateArray[ui] = NiDevice::NOT_PRESSED;
        }
    
    }

    if(pkDoc)
        pkDoc->UnLock();
}
//---------------------------------------------------------------------------

void CNifRenderViewBufferedUIState::SetAxisConstraint(UIAxisConstraint 
                                                      eConstraint) 
{
    CNifRenderViewUIState::SetAxisConstraint(eConstraint);
    m_eLastSetAxisConstraint = eConstraint;
}
        
    