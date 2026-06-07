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

// NifRenderViewUIState.h

#ifndef NIFRENDERVIEWUISTATE_H
#define NIFRENDERVIEWUISTATE_H

class CNifRenderView;
class NiSceneKeyboard;
class NiSceneMouse;

class CNifRenderViewUIState
{
public:

    enum UIAxisConstraint 
    {
        RIGHT_AXIS = 0,
        UP_AXIS,
        FORWARD_AXIS,
        FORWARD_RIGHT_AXIS,
        RIGHT_UP_AXIS,
        FORWARD_UP_AXIS,
        CONSTRAINT_COUNT
    };

    virtual ~CNifRenderViewUIState() {m_pkView = 
        NULL;m_eCurrentAxisConstraint = RIGHT_UP_AXIS;};
    virtual void Update()=0;
    virtual void Initialize(){};
    virtual bool CanExecuteCommand(char* pcCommandID){return true;};
    virtual void OnUpdate(UINT Msg) {};
    virtual void OnRButtonDblClk(UINT nFlags, CPoint point){ };
    virtual void OnRButtonDown(UINT nFlags, CPoint point){ };
    virtual void OnRButtonUp(UINT nFlags, CPoint point){ };
    virtual BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt){ 
        return FALSE;};
    virtual void OnMouseMove(UINT nFlags, CPoint point){ };
    virtual void OnLButtonUp(UINT nFlags, CPoint point){ };
    virtual void OnLButtonDown(UINT nFlags, CPoint point){ };
    virtual void OnLButtonDblClk(UINT nFlags, CPoint point){ };
    virtual void OnMButtonUp(UINT nFlags, CPoint point){ };
    virtual void OnMButtonDown(UINT nFlags, CPoint point){ };
    virtual void OnMButtonDblClk(UINT nFlags, CPoint point){ };
    virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){ };
    virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){ };
    virtual void OnKillFocus(CWnd* pNewWnd){ };
    virtual void SetKeyboard(NiSceneKeyboard* pkKeyboard){};
    virtual void SetMouse(NiSceneMouse* pkMouse){};
    virtual void SetAxisConstraint(UIAxisConstraint eConstraint) 
        {m_eCurrentAxisConstraint = eConstraint;};
    virtual UIAxisConstraint GetAxisConstraint() 
        {return m_eCurrentAxisConstraint;};
    virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint)
        {return false;}

protected:
    CNifRenderViewUIState(CNifRenderView* pkView) {m_pkView = pkView;};
    CNifRenderView* m_pkView;

    UIAxisConstraint m_eCurrentAxisConstraint;
};

#endif  // #ifndef NIFRENDERVIEWUISTATE_H
