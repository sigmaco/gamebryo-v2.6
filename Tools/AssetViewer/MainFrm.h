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

// MainFrm.h

#ifndef MAINFRM_H
#define MAINFRM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViewBar.h"

class CMainFrame : public CFrameWnd
{
    
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
    public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL DestroyWindow();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndStdToolBar;
    CToolBar    m_wndCamNavToolBar;
    CToolBar    m_wndViewsToolBar;
    CToolBar    m_wndAnimationToolBar;

    static void CALLBACK EXPORT PropertyWindowTimerProc(
        HWND hWnd,      // handle of CWnd that called SetTimer
        UINT nMsg,      // WM_TIMER
        UINT nIDEvent,  // timer identification
        DWORD dwTime);  // system time
    unsigned int m_uiTimerId;

protected:  // control bar IDs
    enum
    {
        ID_BAR_CAMERANAV = AFX_IDW_CONTROLBAR_FIRST + 33,
        ID_BAR_VIEWS,
        ID_BAR_TREEVIEW,
        ID_BAR_ANIMATIONCONTROLS,
        ID_BAR_MINIANIMATIONCONTROLS,
        ID_BAR_STATISTICSVIEW,
        ID_BAR_LISTVIEW,
        ID_BAR_COLLISIONCONTROLS
    };

// Generated message map functions
protected:
    BOOL VerifyBarState(LPCTSTR lpszProfileName);
    CViewBar m_wndTreeView;
    CViewBar m_wndListView;
    CViewBar m_wndCollisionView;
    CViewBar m_wndAnimationView;
    CViewBar m_wndStatisticsView;

    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnViewCamNavToolbar();
    afx_msg void OnUpdateViewCamNavToolbar(CCmdUI* pCmdUI);
    afx_msg void OnViewSceneGraphTree();
    afx_msg void OnUpdateViewSceneGraphTree(CCmdUI* pCmdUI);
    afx_msg void OnViewAnimationControls();
    afx_msg void OnUpdateViewCollisionControls(CCmdUI* pCmdUI);
    afx_msg void OnViewCollisionControls();   
    afx_msg void OnUpdateViewAnimationControls(CCmdUI* pCmdUI);
    afx_msg void OnAnimationOptions();
    afx_msg void OnDestroy();
    afx_msg void OnViewDockingViewsToolbar();
    afx_msg void OnUpdateViewDockingViewsToolbar(CCmdUI* pCmdUI);
    afx_msg void OnPlayToolbar();
    afx_msg void OnUpdatePlayToolbar(CCmdUI* pCmdUI);
    afx_msg void OnPreviousframeToolbar();
    afx_msg void OnStopToolbar();
    afx_msg void OnNextframeToolbar();
    afx_msg void OnLastframeToolbar();
    afx_msg void OnFirstframeToolbar();
    afx_msg void OnViewMinianimationToolbar();
    afx_msg void OnUpdateViewMinianimationToolbar(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLoopToolbar(CCmdUI* pCmdUI);
    afx_msg void OnLoopToolbar();
    afx_msg void OnViewStatistics();
    afx_msg void OnUpdateViewStatistics(CCmdUI* pCmdUI);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnAssetViewerHelp();
    afx_msg void OnViewSceneGraphList();
    afx_msg void OnUpdateViewSceneGraphList(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnUpdateAssetViewerHelp(CCmdUI *pCmdUI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef MAINFRM_H
