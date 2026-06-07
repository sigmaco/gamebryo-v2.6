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

// NifRenderViewBufferedUIState.h

#ifndef NIFRENDERVIEWBUFFEREDUISTATE_H
#define NIFRENDERVIEWBUFFEREDUISTATE_H

#include "NifRenderViewUIState.h"
#include "NiSceneKeyboard.h"
#include "NifKeyboardShortcuts.h"

class NiSceneMouse;
class CNifRenderView;

class CNifRenderViewBufferedUIState : public CNifRenderViewUIState
{
    public:
        CNifRenderViewBufferedUIState(CNifRenderView* pkView);
        virtual ~CNifRenderViewBufferedUIState();
        virtual void Update();
        virtual void Initialize();
        virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
        virtual void OnRButtonDown(UINT nFlags, CPoint point);
        virtual void OnRButtonUp(UINT nFlags, CPoint point);
        virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        virtual void OnMouseMove(UINT nFlags, CPoint point);
        virtual void OnLButtonUp(UINT nFlags, CPoint point);
        virtual void OnLButtonDown(UINT nFlags, CPoint point);
        virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
        virtual void OnMButtonUp(UINT nFlags, CPoint point);
        virtual void OnMButtonDown(UINT nFlags, CPoint point);
        virtual void OnMButtonDblClk(UINT nFlags, CPoint point);
        virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
        virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        virtual void SetKeyboard(NiSceneKeyboard* pkKeyboard);
        virtual void SetMouse(NiSceneMouse* pkMouse);
        virtual void OnKillFocus(CWnd* pNewWnd);
        virtual void SetAxisConstraint(UIAxisConstraint eConstraint);


    protected:
        NiSceneMouse* m_pkMouse;
        NiSceneKeyboard* m_pkKeyboard;
        CPoint m_kLastPoint;

        UIAxisConstraint m_eLastSetAxisConstraint;

        virtual void UpdateDevices();
        virtual bool InitUserPrefs();
        virtual void CalculateTime();

        virtual void OnForwardKey(NiDevice::DeviceState eState) {};
        virtual void OnBackwardKey(NiDevice::DeviceState eState){};
        virtual void OnLeftKey(NiDevice::DeviceState eState){};
        virtual void OnRightKey(NiDevice::DeviceState eState){};
        virtual void OnUpKey(NiDevice::DeviceState eState){};
        virtual void OnDownKey(NiDevice::DeviceState eState){};
        virtual void OnPitchUpKey(NiDevice::DeviceState eState){};
        virtual void OnPitchDownKey(NiDevice::DeviceState eState){};
        virtual void OnYawLeftKey(NiDevice::DeviceState eState){};
        virtual void OnYawRightKey(NiDevice::DeviceState eState){};

        // Keyboard shortcuts
        NiSceneKeyboard::KeyCode m_eForwardKey;
        NiSceneKeyboard::KeyCode m_eBackwardKey;
        NiSceneKeyboard::KeyCode m_eLeftKey;
        NiSceneKeyboard::KeyCode m_eRightKey;
        NiSceneKeyboard::KeyCode m_eUpKey;
        NiSceneKeyboard::KeyCode m_eDownKey;
        NiSceneKeyboard::KeyCode m_ePitchUpKey;
        NiSceneKeyboard::KeyCode m_ePitchDownKey;
        NiSceneKeyboard::KeyCode m_eYawLeftKey;
        NiSceneKeyboard::KeyCode m_eYawRightKey;

        NiPoint3 m_kTranslateSpeed;
        CNifKeyboardShortcuts m_kKeyboardShortcuts;
        bool m_bActive;
        float m_fYawSpeed;
        float m_fPitchSpeed;

        float m_fLastTime;
        float m_fAccumTime;
        float m_fDeltaTime;
        bool m_bReInitialize;

        float m_fWheelDelta;
};      

#endif  // #ifndef NIFRENDERVIEWBUFFEREDUISTATE_H
