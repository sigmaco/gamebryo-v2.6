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

// NifRenderViewUIManager.h

#ifndef NIFRENDERVIEWUIMANAGER_H
#define NIFRENDERVIEWUIMANAGER_H

#include "NifRenderViewUIState.h"

class CNifRenderView;
class CNifRenderViewUICommand;
class NiSceneKeyboard;
class NiSceneMouse;

class CNifRenderViewUIManager
{
public:
    CNifRenderViewUIManager(CNifRenderView* pkRenderView);
    virtual ~CNifRenderViewUIManager();

    virtual CNifRenderViewUIState* GetCurrentViewState();
    virtual void SetCurrentViewState(CNifRenderViewUIState* pkController);
    virtual void SetCurrentAxisMode(
        CNifRenderViewUIState::UIAxisConstraint eMode);
    virtual CNifRenderViewUIState::UIAxisConstraint GetCurrentAxisMode();
    virtual bool IsAxisModeAllowed(
        CNifRenderViewUIState::UIAxisConstraint eMode);

    virtual void Update();
    virtual void QueueCommand(CNifRenderViewUICommand* pkCommand);

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
    virtual void OnKillFocus(CWnd* pNewWnd);
    virtual void OnUpdate(UINT Msg);

protected:  
    void ExecuteCommands();
    NiTPrimitiveQueue<CNifRenderViewUICommand*> m_kCommands;

    CNifRenderViewUIState* m_pkState;
    CNifRenderView* m_pkRenderView;  
    CCriticalSection m_kCriticalSection;

    NiSceneKeyboard* m_pkKeyboard;
    NiSceneMouse* m_pkMouse;
};

#endif  // #ifndef NIFRENDERVIEWUIMANAGER_H
