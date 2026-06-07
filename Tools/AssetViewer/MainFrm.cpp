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

// MainFrm.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "MainFrm.h"
#include "NifTreeView.h"
#include "NifListView.h"
#include "NifUserPreferences.h"
#include "NifAnimationControlView.h"
#include "NifAnimationOptionsDlg.h"
#include "NifCollisionControlView.h"
#include "NifPropertyWindowManager.h"
#include "NifStatisticsView.h"
#include "NifTimeManager.h"
#include <process.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString gs_strHelpPath = "..\\..\\..\\..\\Documentation\\HTML\\"
    "Gamebryo.chm";
static CString gs_strHelpLocation = "::/Tool_Manuals/Asset_Viewer/"
    "Asset_Viewer_Basics/Introduction_to_the_Asset_Viewer.htm";

//---------------------------------------------------------------------------
// CMainFrame
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_CAMNAV_TOOLBAR, OnViewCamNavToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_CAMNAV_TOOLBAR, OnUpdateViewCamNavToolbar)
    ON_COMMAND(ID_VIEW_SCENEGRAPHTREE, OnViewSceneGraphTree)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SCENEGRAPHTREE, OnUpdateViewSceneGraphTree)
    ON_COMMAND(ID_VIEW_ANIMATIONCONTROLS, OnViewAnimationControls)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ANIMATIONCONTROLS, 
        OnUpdateViewAnimationControls)
    ON_COMMAND(ID_VIEW_COLLISIONCONTROLS, OnViewCollisionControls)
    ON_UPDATE_COMMAND_UI(ID_VIEW_COLLISIONCONTROLS,
        OnUpdateViewCollisionControls)
    ON_COMMAND(ID_ANIMATION_OPTIONS, OnAnimationOptions)
    ON_WM_DESTROY()
    ON_COMMAND(ID_VIEW_DOCKING_VIEWS_TOOLBAR, OnViewDockingViewsToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DOCKING_VIEWS_TOOLBAR, 
        OnUpdateViewDockingViewsToolbar)
    ON_COMMAND(ID_PLAY_TOOLBAR, OnPlayToolbar)
    ON_UPDATE_COMMAND_UI(ID_PLAY_TOOLBAR, OnUpdatePlayToolbar)
    ON_COMMAND(ID_PREVIOUSFRAME_TOOLBAR, OnPreviousframeToolbar)
    ON_COMMAND(ID_STOP_TOOLBAR, OnStopToolbar)
    ON_COMMAND(ID_NEXTFRAME_TOOLBAR, OnNextframeToolbar)
    ON_COMMAND(ID_LASTFRAME_TOOLBAR, OnLastframeToolbar)
    ON_COMMAND(ID_FIRSTFRAME_TOOLBAR, OnFirstframeToolbar)
    ON_COMMAND(ID_VIEW_MINIANIMATION_TOOLBAR, OnViewMinianimationToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_MINIANIMATION_TOOLBAR,
        OnUpdateViewMinianimationToolbar)
    ON_UPDATE_COMMAND_UI(ID_LOOP_TOOLBAR, OnUpdateLoopToolbar)
    ON_COMMAND(ID_LOOP_TOOLBAR, OnLoopToolbar)
    ON_COMMAND(ID_VIEW_STATISTICS, OnViewStatistics)
    ON_UPDATE_COMMAND_UI(ID_VIEW_STATISTICS, OnUpdateViewStatistics)
    ON_WM_SHOWWINDOW()
    ON_COMMAND(ID_ASSETVIEWER_HELP, OnAssetViewerHelp)
    ON_COMMAND(ID_VIEW_SCENEGRAPHLIST, OnViewSceneGraphList)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SCENEGRAPHLIST, OnUpdateViewSceneGraphList)
    //}}AFX_MSG_MAP
    ON_UPDATE_COMMAND_UI(ID_ASSETVIEWER_HELP, OnUpdateAssetViewerHelp)
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};
//---------------------------------------------------------------------------
// CMainFrame construction/destruction
//---------------------------------------------------------------------------
CMainFrame::CMainFrame()
{
}
//---------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}
//---------------------------------------------------------------------------
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create status bar.
    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    // Create standard toolbar.
    if (!m_wndStdToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD |
        WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_SIZE_DYNAMIC) || !m_wndStdToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }
  
    // Create camera navigation toolbar.
    if (!m_wndCamNavToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD |
        WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_SIZE_DYNAMIC, CRect(0,0,0,0), ID_BAR_CAMERANAV) ||
        !m_wndCamNavToolBar.LoadToolBar(IDR_CAMERA_NAV))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    // Create views toolbar.
    if (!m_wndViewsToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD |
        WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_SIZE_DYNAMIC, CRect(0,0,0,0), ID_BAR_VIEWS) ||
        !m_wndViewsToolBar.LoadToolBar(IDR_VIEWS_BAR))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    // Create mini-animation toolbar.
    if (!m_wndAnimationToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD |
        WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_SIZE_DYNAMIC, CRect(0,0,0,0), ID_BAR_MINIANIMATIONCONTROLS) ||
        !m_wndAnimationToolBar.LoadToolBar(IDR_ANIMATION_TOOLBAR))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    // Create tree view docking window.
    if (!m_wndTreeView.Create(this, RUNTIME_CLASS (CNifTreeView),
        (CCreateContext*) (lpCreateStruct->lpCreateParams),
        _T("Scene Graph Tree"), WS_CHILD | WS_VISIBLE | CBRS_TOP |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_BAR_TREEVIEW))
    {
        TRACE0("Failed to create Tree View\n");
        return -1;      // fail to create
    }

    // Create list view docking window.
    if (!m_wndListView.Create(this, RUNTIME_CLASS(CNifListView),
        (CCreateContext*) lpCreateStruct->lpCreateParams,
        _T("Scene Graph List"), WS_CHILD | WS_VISIBLE | CBRS_TOP |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, ID_BAR_LISTVIEW))
    {
        TRACE0("Failed to create List View\n");
        return -1;      // fail to create
    }

    // Create collision controls view docking window.
    if (!m_wndCollisionView.Create(this, RUNTIME_CLASS(
        CNifCollisionControlView), (CCreateContext*)
        (lpCreateStruct->lpCreateParams), _T("Collision"),
        WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_SIZE_DYNAMIC, ID_BAR_COLLISIONCONTROLS))
    {
        TRACE0("Failed to create Collision View\n");
        return -1;      // fail to create
    }

    // Create animation controls view docking window.
    if (!m_wndAnimationView.Create(this, RUNTIME_CLASS(
        CNifAnimationControlView), (CCreateContext*)
        (lpCreateStruct->lpCreateParams), _T("Animation"),
        WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY |
        CBRS_SIZE_DYNAMIC, ID_BAR_ANIMATIONCONTROLS))
    {
        TRACE0("Failed to create Animation View\n");
        return -1;      // fail to create
    }

    // Create performance statistics view docking window.
    if (!m_wndStatisticsView.Create(this, RUNTIME_CLASS(
        CNifStatisticsView), (CCreateContext*)lpCreateStruct->lpCreateParams,
        _T("Performance Statistics"), WS_CHILD | WS_VISIBLE | CBRS_TOP |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
        ID_BAR_STATISTICSVIEW))
    {
        TRACE0("Failed to create Statistics View\n");
        return -1;      // fail to create
    }

    // Enable docking for frame window.
    EnableDocking(CBRS_ALIGN_ANY);

    CRect rect;

    // Enable docking for toolbars.
    m_wndStdToolBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndStdToolBar, AFX_IDW_DOCKBAR_TOP);
    RecalcLayout();
    m_wndStdToolBar.GetWindowRect(rect);

    rect.OffsetRect(1, 0);
    m_wndCamNavToolBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndCamNavToolBar, AFX_IDW_DOCKBAR_TOP, rect);
    RecalcLayout();
    m_wndCamNavToolBar.GetWindowRect(rect);

    rect.OffsetRect(1, 0);
    m_wndAnimationToolBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndAnimationToolBar, AFX_IDW_DOCKBAR_TOP, rect);
    RecalcLayout();
    m_wndAnimationToolBar.GetWindowRect(rect);

    rect.OffsetRect(1, 0);
    m_wndViewsToolBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndViewsToolBar, AFX_IDW_DOCKBAR_TOP, rect);

    // Enable docking for tree view.
    m_wndTreeView.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndTreeView, AFX_IDW_DOCKBAR_RIGHT);
    RecalcLayout();
    m_wndTreeView.GetWindowRect(rect);

    // Enable docking for list view.
    rect.OffsetRect(0, 1);
    m_wndListView.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndListView, AFX_IDW_DOCKBAR_RIGHT, rect);
    RecalcLayout();
    m_wndListView.GetWindowRect(rect);

    // Enable docking for statistics view.
    rect.OffsetRect(0, 1);
    m_wndStatisticsView.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndStatisticsView, AFX_IDW_DOCKBAR_RIGHT, rect);

    // Enable docking for collision controls view
    m_wndCollisionView.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    DockControlBar(&m_wndCollisionView, AFX_IDW_DOCKBAR_LEFT);
    ShowControlBar(&m_wndCollisionView, FALSE, FALSE);

    // Enable docking for animation controls view
    m_wndAnimationView.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
    DockControlBar(&m_wndAnimationView, AFX_IDW_DOCKBAR_BOTTOM);
    ShowControlBar(&m_wndAnimationView, FALSE, FALSE);

#ifdef _SCB_REPLACE_MINIFRAME
    // Change floating frame class.
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif  // #ifdef _SCB_REPLACE_MINIFRAME

    // Load bar state for all bars in application.
    CString strProfile = _T("BarState");
    if (VerifyBarState(strProfile))
    {
        CSizingControlBar::GlobalLoadState(this, strProfile);
        LoadBarState(strProfile);
    }

    // Note: PROPERTY_PANEL_TIMER_INCREMENT found in NifGlobals.h
    m_uiTimerId = (unsigned int)SetTimer(PROPERTY_PANEL_TIMER_ID,
        PROPERTY_PANEL_TIMER_INCREMENT, PropertyWindowTimerProc);

    return 0;
}
//---------------------------------------------------------------------------
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;

    // Modify the Window class or styles here by modifying
    // the CREATESTRUCT cs

    cs.style &= ~FWS_ADDTOTITLE;
    cs.style &= ~FWS_PREFIXTITLE;

    return TRUE;
}
//---------------------------------------------------------------------------
// CMainFrame diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}
#endif //_DEBUG
//---------------------------------------------------------------------------
// CMainFrame message handlers
//---------------------------------------------------------------------------
// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
//---------------------------------------------------------------------------
BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = (int)pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = PtrToUint(pInfo->m_arrBarID[j]);
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}
//---------------------------------------------------------------------------
// Toggle functions for docking views and toolbars.
//---------------------------------------------------------------------------
void CMainFrame::OnViewSceneGraphTree() 
{
    BOOL bShow = !m_wndTreeView.IsVisible();
    ShowControlBar(&m_wndTreeView, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewSceneGraphTree(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndTreeView.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewSceneGraphList() 
{
    BOOL bShow = !m_wndListView.IsVisible();
    ShowControlBar(&m_wndListView, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewSceneGraphList(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndListView.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewCollisionControls() 
{
    BOOL bShow = !m_wndCollisionView.IsVisible();
    ShowControlBar(&m_wndCollisionView, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewCollisionControls(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndCollisionView.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewAnimationControls() 
{
    BOOL bShow = !m_wndAnimationView.IsVisible();
    ShowControlBar(&m_wndAnimationView, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewAnimationControls(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndAnimationView.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewCamNavToolbar() 
{
    BOOL bShow = !m_wndCamNavToolBar.IsVisible();
    ShowControlBar(&m_wndCamNavToolBar, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewCamNavToolbar(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndCamNavToolBar.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewDockingViewsToolbar() 
{
    BOOL bShow = !m_wndViewsToolBar.IsVisible();
    ShowControlBar(&m_wndViewsToolBar, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewDockingViewsToolbar(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndViewsToolBar.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewStatistics() 
{
    BOOL bShow = !m_wndStatisticsView.IsVisible();
    ShowControlBar(&m_wndStatisticsView, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewStatistics(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndStatisticsView.IsVisible());
}
//---------------------------------------------------------------------------
// Options dialog.
//---------------------------------------------------------------------------
void CMainFrame::OnAnimationOptions() 
{
    CNifAnimationOptionsDlg kDialog;
    kDialog.DoModal();
}
//---------------------------------------------------------------------------
// Window messages.
//---------------------------------------------------------------------------
BOOL CMainFrame::DestroyWindow() 
{
    // Save bar state for all bars in application.
    CString strProfile = _T("BarState");
    CSizingControlBar::GlobalSaveState(this, strProfile);
    SaveBarState(strProfile);

    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileBinary("MainFrame", "WP", (LPBYTE)&wp,
        sizeof(wp));

    return CFrameWnd::DestroyWindow();
}
//---------------------------------------------------------------------------
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CFrameWnd::OnShowWindow(bShow, nStatus);
    
    static bool bOnce = true;

    if(bShow && !IsWindowVisible() && bOnce)
    {
        bOnce = false;

        WINDOWPLACEMENT *lwp;
        UINT nl;

        if(AfxGetApp()->GetProfileBinary("MainFrame", "WP", (LPBYTE*)&lwp,
            &nl))
        {
            if(lwp->showCmd == SW_SHOWMINIMIZED)
                lwp->showCmd = SW_SHOWNORMAL;
            SetWindowPlacement(lwp);
            delete[] lwp;
        }
    }
}
//---------------------------------------------------------------------------
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
    CDocument* pkDoc = GetActiveDocument();
    if(pMsg->message == NIF_BROADCASTUPDATE && pkDoc)
        pkDoc->UpdateAllViews(NULL, pMsg->lParam, NULL);
    
    return CFrameWnd::PreTranslateMessage(pMsg);
}
//---------------------------------------------------------------------------
void CMainFrame::OnDestroy() 
{
    KillTimer(m_uiTimerId);
    CNifPropertyWindowManager::DestroyPropertyWindowManager();
    CFrameWnd::OnDestroy();
}
//---------------------------------------------------------------------------
void CALLBACK EXPORT CMainFrame::PropertyWindowTimerProc(
    HWND hWnd,      // handle of CWnd that called SetTimer
    UINT nMsg,      // WM_TIMER
    UINT nIDEvent,  // timer identification
    DWORD dwTime)   // system time
{
    CNifPropertyWindowManager* pkManager = 
        CNifPropertyWindowManager::GetPropertyWindowManager();
    pkManager->UpdateAllVisiblePropertyWindows();
}
//---------------------------------------------------------------------------
void CMainFrame::OnPlayToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        if(pkDoc->GetTimeManager()->IsEnabled())
            pkDoc->GetTimeManager()->Disable();
        else
            pkDoc->GetTimeManager()->Enable();
    }
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdatePlayToolbar(CCmdUI* pCmdUI) 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        if(pkDoc->GetTimeManager()->IsEnabled())
            pCmdUI->SetCheck(true);
        else
            pCmdUI->SetCheck(false);
    }
    else
        pCmdUI->SetCheck(false);
}
//---------------------------------------------------------------------------
void CMainFrame::OnPreviousframeToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        float fLastTime = pkDoc->GetLastTimeUpdated();
        float fFrameRate = 60.0f;
        pkDoc->GetTimeManager()->SetCurrentTime(fLastTime - 1.0f/fFrameRate);
    }
}
//---------------------------------------------------------------------------
void CMainFrame::OnStopToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->GetTimeManager()->Disable();
        OnFirstframeToolbar();
    }   
}
//---------------------------------------------------------------------------
void CMainFrame::OnNextframeToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        float fLastTime = pkDoc->GetLastTimeUpdated();
        float fFrameRate = 60.0f;
        pkDoc->GetTimeManager()->SetCurrentTime(fLastTime + 1.0f/fFrameRate);
    }
}
//---------------------------------------------------------------------------
void CMainFrame::OnLastframeToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->GetTimeManager()->SetCurrentTime(pkDoc->GetTimeManager()->
            GetEndTime());
    }
}
//---------------------------------------------------------------------------
void CMainFrame::OnFirstframeToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->GetTimeManager()->SetCurrentTime(pkDoc->GetTimeManager()->
            GetStartTime());
    }
}
//---------------------------------------------------------------------------
void CMainFrame::OnViewMinianimationToolbar() 
{
    BOOL bShow = !m_wndAnimationToolBar.IsVisible();
    ShowControlBar(&m_wndAnimationToolBar, bShow, FALSE);
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateViewMinianimationToolbar(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_wndAnimationToolBar.IsVisible());
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateLoopToolbar(CCmdUI* pCmdUI) 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        if(pkDoc->GetTimeManager()->GetTimeMode() == CNifTimeManager::LOOP)
            pCmdUI->SetCheck(true);
        else
            pCmdUI->SetCheck(false);
    }
    else
        pCmdUI->SetCheck(false);
}
//---------------------------------------------------------------------------
void CMainFrame::OnLoopToolbar() 
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        if(pkDoc->GetTimeManager()->GetTimeMode() == CNifTimeManager::LOOP)
            pkDoc->GetTimeManager()->SetTimeMode(
            CNifTimeManager::CONTINUOUS);
        else
            pkDoc->GetTimeManager()->SetTimeMode(CNifTimeManager::LOOP);
    }   
}
//---------------------------------------------------------------------------
BOOL CALLBACK EnumWindowsProc(
  HWND hwnd,      // handle to parent window
  LPARAM lParam   // application-defined value
)
{
    CMainFrame* pkMainFrame = (CMainFrame*) lParam;
    
    HWND hParent = (HWND) LongToPtr(GetWindowLong(hwnd, GWL_HWNDPARENT));
    HWND hParent2 = pkMainFrame->m_hWnd;

    if(hParent == hParent2 && hwnd != hParent2)
    {
        CWnd* pkWnd = CWnd::FromHandle(hwnd);
        pkWnd->SetActiveWindow();
        pkWnd->SetForegroundWindow();
        pkWnd->SetWindowText( "Test" );
    }

    return TRUE;
}
//---------------------------------------------------------------------------
void CMainFrame::OnAssetViewerHelp() 
{
    const char* pcErrorMessage = "Unable to find the Gamebryo help file.";
    const char* pcErrorTitle = "Asset Viewer Help Error";

    // Get application path.
    CAssetViewerApp* pkTheApp = (CAssetViewerApp*) AfxGetApp();
    CString strAppPath = pkTheApp->GetApplicationDirectory();
    if(strAppPath.IsEmpty())
    {
        MessageBox(pcErrorMessage, pcErrorTitle, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    CString strCommandLine = "hh.exe " + strAppPath + gs_strHelpPath +
        gs_strHelpLocation;
    if (WinExec(strCommandLine, SW_SHOWNORMAL) <= 31)
    {
        MessageBox(pcErrorMessage, pcErrorTitle, MB_OK | MB_ICONEXCLAMATION);
    }
}
//---------------------------------------------------------------------------
void CMainFrame::OnUpdateAssetViewerHelp(CCmdUI *pCmdUI)
{
    // Get application path.
    CAssetViewerApp* pkTheApp = (CAssetViewerApp*) AfxGetApp();
    CString strAppPath = pkTheApp->GetApplicationDirectory();
    if(strAppPath.IsEmpty())
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    // Build help path.
    if (NiFile::Access(strAppPath + gs_strHelpPath, NiFile::READ_ONLY)
        == false)
    {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);
}
//---------------------------------------------------------------------------
