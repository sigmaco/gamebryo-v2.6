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

// NifPropertyWindow.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifPropertyWindow.h"
#include "NiObjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifPropertyWindow
//---------------------------------------------------------------------------
CNifPropertyWindow::CNifPropertyWindow()
{
    m_strMyClass = AfxRegisterWndClass(
        CS_DBLCLKS |
        CS_VREDRAW |
        CS_HREDRAW,
        (HCURSOR)::LoadCursor(NULL, IDC_ARROW),
        (HBRUSH)(COLOR_BACKGROUND+1),
        NULL);
    m_pkCreator = NULL;
    m_bHasHeartbeat = false;
    m_spObject = NULL;
}
//---------------------------------------------------------------------------
CNifPropertyWindow::~CNifPropertyWindow()
{
    m_spObject = NULL;
}
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifPropertyWindow, CWnd)
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifPropertyWindow, CWnd)
    //{{AFX_MSG_MAP(CNifPropertyWindow)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifPropertyWindow message handlers
//---------------------------------------------------------------------------
int CNifPropertyWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    int cx = lpCreateStruct->cx - 4;
    int cy = lpCreateStruct->cy - 4;
    int cox = 4;
    int coy = 4;

    m_kRollups.Create( WS_VISIBLE | WS_CHILD, CRect(cox,coy,cx,cy), this, 0);
    m_bHasHeartbeat = true;

    return 0;
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::OnDestroy() 
{
    m_pkCreator = NULL;
    m_bHasHeartbeat = false;
    m_spObject = NULL;
    CWnd::OnDestroy();
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::OnSize(UINT nType, int cx, int cy) 
{
    CWnd::OnSize(nType, cx, cy);
    WPARAM wParam = (WPARAM) nType;
    LPARAM lParam = MAKELPARAM(cx, cy);

    m_kRollups.SetWindowPos(NULL, 0,0, cx - 10, cy -10, SWP_NOMOVE |
        SWP_NOZORDER );

    m_kRollups.SendMessage(WM_SIZE, wParam, lParam);
}
//---------------------------------------------------------------------------
CRollupCtrl& CNifPropertyWindow::GetRollupControl()
{
    return m_kRollups;
}
//---------------------------------------------------------------------------
int CNifPropertyWindow::InsertPage(char* pcTitle, CNiObjectDlg* pwndDlg,
    bool bExpanded, int iIndex)
{
    int idx = m_kRollups.InsertPage(pcTitle, pwndDlg, iIndex);
    if(bExpanded)
        m_kRollups.ExpandPage(idx, TRUE);
    return idx;
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::UpdateActivePages()
{
    if(!HasHeartbeat())
        return;
    else if(PerformSuicideWatch())
        return;

    CNiObjectDlg* pkDialog = NULL;
    for(int idx = 0; idx < m_kRollups.GetPagesCount(); idx++)
    {
        RC_PAGEINFO* pkPageInfo = m_kRollups.GetPageInfo(idx);
        if(pkPageInfo != NULL && pkPageInfo->bExpanded &&
           pkPageInfo->pwndTemplate != NULL )
        {
            pkDialog = (CNiObjectDlg*)(pkPageInfo->pwndTemplate);
            if(pkDialog && pkDialog->NeedsUpdateAfterCreation())
                pkDialog->DoUpdate();
        }
    }
}
//---------------------------------------------------------------------------
CWnd* CNifPropertyWindow::GetParentForPages()
{
    return &m_kRollups;
}
//---------------------------------------------------------------------------
CString& CNifPropertyWindow::GetRegisteredClassName()
{
    return m_strMyClass;
}
//---------------------------------------------------------------------------
BOOL CNifPropertyWindow::OnEraseBkgnd(CDC* pDC) 
{
    CPen pen(PS_SOLID, 0, GetSysColor(COLOR_BTNFACE));
    CPen * pPen = pDC->SelectObject(&pen);
    CBrush brush(GetSysColor(COLOR_BTNFACE));
    CBrush *pBrush = pDC->SelectObject(&brush);
    
    CRect rect;
    GetClientRect(&rect);
    pDC->Rectangle(rect);
    pDC->SelectObject(pPen);
    pDC->SelectObject(pBrush);
    return TRUE;
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::ScrollToPage(int iPage)
{
   m_kRollups.ScrollToPage(iPage, TRUE);
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::SetCreator(CNiObjectPropertyWindowCreator* pkCreator)
{
    ASSERT(pkCreator);
    m_pkCreator = pkCreator;
}
//---------------------------------------------------------------------------
bool CNifPropertyWindow::HasHeartbeat()
{
    return m_bHasHeartbeat;
}
//---------------------------------------------------------------------------
void CNifPropertyWindow::SetNiObject(NiObject* pkObj)
{
    if(pkObj != NULL)
        m_spObject = pkObj;
}
//---------------------------------------------------------------------------
NiObject* CNifPropertyWindow::GetNiObject()
{
    return m_spObject;
}
//---------------------------------------------------------------------------
bool CNifPropertyWindow::PerformSuicideWatch()
{
    if(m_spObject && m_spObject->GetRefCount() == 1)
    {
        DestroyWindow();
        return true;
    }
    else if(!m_spObject)
    {
        if(m_bHasHeartbeat)
            DestroyWindow();
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
