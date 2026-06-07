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

// NifRenderViewUIManager.cpp

#include "stdafx.h"
#include "NifRenderViewUIManager.h"
#include "NifRenderViewUIState.h"
#include "NiSceneKeyboard.h"
#include "NiSceneMouse.h"
#include "NifRenderViewUICommand.h"

//---------------------------------------------------------------------------
CNifRenderViewUIManager::CNifRenderViewUIManager(CNifRenderView* pkRenderView)
{
    m_pkRenderView = pkRenderView;
    m_pkState = NULL;
    m_pkKeyboard = NiNew NiSceneKeyboard();
    m_pkMouse = NiNew NiSceneMouse();
}
//---------------------------------------------------------------------------
CNifRenderViewUIManager::~CNifRenderViewUIManager()
{
    if(m_pkState)
        delete m_pkState;
    delete m_pkMouse;
    delete m_pkKeyboard;
}
//---------------------------------------------------------------------------
CNifRenderViewUIState* CNifRenderViewUIManager::GetCurrentViewState()
{ 
    return m_pkState;
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::SetCurrentViewState(
    CNifRenderViewUIState* pkState)
{
    CNifRenderViewUIState::UIAxisConstraint eMode = GetCurrentAxisMode();
    delete m_pkState;
    m_pkState = pkState;

    if(m_pkState)
    {
        m_pkState->SetMouse(m_pkMouse);
        m_pkState->SetKeyboard(m_pkKeyboard);
        m_pkState->Initialize();
        m_pkState->SetAxisConstraint(eMode);
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::Update()
{
    if(m_pkState)
        m_pkState->Update();
    ExecuteCommands();
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::QueueCommand(CNifRenderViewUICommand* pkCommand)
{ 
    if(pkCommand)
    {
        m_kCriticalSection.Lock();
        m_kCommands.Add(pkCommand);
        m_kCriticalSection.Unlock();
    }
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnRButtonDblClk(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnRButtonDblClk(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnRButtonDown(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnRButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnRButtonUp(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnRButtonUp(nFlags, point);
}
//---------------------------------------------------------------------------
BOOL CNifRenderViewUIManager::OnMouseWheel(UINT nFlags, short zDelta,
    CPoint pt)
{
    if(m_pkState)
        return m_pkState->OnMouseWheel(nFlags, zDelta, pt);
    else
        return FALSE;
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnMouseMove(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnMouseMove(nFlags,point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnLButtonUp(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnLButtonUp(nFlags, point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnLButtonDown(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnLButtonDown(nFlags, point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnLButtonDblClk(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnLButtonDblClk(nFlags, point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnMButtonUp(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnMButtonUp(nFlags, point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnMButtonDown(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnMButtonDown(nFlags, point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnMButtonDblClk(UINT nFlags, CPoint point)
{ 
    if(m_pkState)
        m_pkState->OnMButtonDblClk(nFlags, point);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{ 
    if(m_pkState)
        m_pkState->OnKeyUp(nChar, nRepCnt, nFlags);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{ 
    if(m_pkState)
        m_pkState->OnKeyDown(nChar, nRepCnt, nFlags);

}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnKillFocus(CWnd* pNewWnd)
{
    if (m_pkState)
        m_pkState->OnKillFocus(pNewWnd);
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::ExecuteCommands()
{ 
    m_kCriticalSection.Lock();
    CNifRenderViewUICommand* pkCommand = NULL;
    while(!m_kCommands.IsEmpty())
    {
        pkCommand = m_kCommands.Remove();
        if(pkCommand)
        {
            if(m_pkState && 
               m_pkState->CanExecuteCommand(pkCommand->GetIDString()))
                pkCommand->Execute(m_pkRenderView);
            else if(!m_pkState)
                pkCommand->Execute(m_pkRenderView);
            delete pkCommand;
            pkCommand = NULL;
        }
    }
    m_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::OnUpdate(UINT Msg)
{
    m_kCriticalSection.Lock();
    if(m_pkState)
        m_pkState->OnUpdate(Msg);
    m_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void CNifRenderViewUIManager::SetCurrentAxisMode(CNifRenderViewUIState::
                                        UIAxisConstraint eMode)
{
    if(eMode < CNifRenderViewUIState::CONSTRAINT_COUNT && m_pkState)
    {
        m_pkState->SetAxisConstraint(eMode);
    }
}
//---------------------------------------------------------------------------

CNifRenderViewUIState::UIAxisConstraint 
CNifRenderViewUIManager::GetCurrentAxisMode()
{
    if(m_pkState)
        return m_pkState->GetAxisConstraint();
    else
        return CNifRenderViewUIState::RIGHT_UP_AXIS;
}
//---------------------------------------------------------------------------
bool 
CNifRenderViewUIManager::IsAxisModeAllowed(CNifRenderViewUIState::
                                           UIAxisConstraint eMode)
{
    if(m_pkState)
        return m_pkState->IsAxisConstraintAllowed(eMode);
    else
        return false;
}
//---------------------------------------------------------------------------
