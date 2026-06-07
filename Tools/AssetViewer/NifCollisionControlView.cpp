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

// NifCollisionControlView.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifCollisionControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// Construction/Destruction
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE (CNifCollisionControlView, CScrollView)
//---------------------------------------------------------------------------
CNifCollisionControlView::CNifCollisionControlView()
{

}
//---------------------------------------------------------------------------
CNifCollisionControlView::~CNifCollisionControlView()
{

}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifCollisionControlView, CScrollView)
    //{{AFX_MSG_MAP(CNifCollisionControlView)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CNifCollisionControlView drawing
//---------------------------------------------------------------------------
void CNifCollisionControlView::OnDraw(CDC* pDC)
{
    // Draw code can go here if needed
}
//---------------------------------------------------------------------------
// CNifCollisionControlView diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CNifCollisionControlView::AssertValid() const
{
    CScrollView::AssertValid();
}
//---------------------------------------------------------------------------
void CNifCollisionControlView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}
#endif //_DEBUG
//---------------------------------------------------------------------------
// CNifCollisionControlView message handlers
//---------------------------------------------------------------------------
int CNifCollisionControlView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CScrollView::OnCreate(lpCreateStruct) == -1)
        return -1;
    m_wndDialog.Create(IDD_COLLISION_CONTROLS, this);
    SetScrollSizes(MM_TEXT, CSize(20,20));
    ResizeParentToFit(FALSE);
    return 0;
}
//---------------------------------------------------------------------------
BOOL CNifCollisionControlView::OnEraseBkgnd(CDC* pDC) 
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
void CNifCollisionControlView::OnInitialUpdate() 
{
    //ResizeParentToFit(FALSE);
    CScrollView::OnInitialUpdate();   
    
    CRect rect;
    m_wndDialog.GetClientRect(&rect);

    SIZE size = {rect.Width(), rect.Height() };

    SetScrollSizes(MM_TEXT, size);
}
//---------------------------------------------------------------------------
BOOL CNifCollisionControlView::PreCreateWindow(CREATESTRUCT& cs) 
{
    // removes min/max boxes
    cs.style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    // makes dialog box unsizable
    cs.style &= ~WS_THICKFRAME;
    return CScrollView::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------
void CNifCollisionControlView::OnSize(UINT nType, int cx, int cy) 
{
    if(m_wndDialog.m_hWnd == NULL)
    {
        CScrollView::OnSize(nType, cx, cy);
        return;
    }

    CRect rectChild;
    m_wndDialog.GetClientRect(&rectChild);

    CRect rectParent;
    GetClientRect(&rectParent);

    if(rectParent.Width() < rectChild.Width() || 
       rectParent.Height() < rectChild.Height())
    {
        EnableScrollBar(SB_BOTH);
        EnableScrollBarCtrl(SB_HORZ , TRUE );
        EnableScrollBarCtrl(SB_VERT , TRUE );
    }
    else
    {
        EnableScrollBarCtrl(SB_HORZ , FALSE );
        EnableScrollBarCtrl(SB_VERT , FALSE);
    }

    CRect newRect;
    int iWidth = rectParent.Width() - rectChild.Width();
    int iHeight = rectParent.Height() - rectChild.Height();
    
    if(iWidth < 0)
        iWidth = 0;

    if(iHeight < 0)
        iHeight = 0;

    newRect.bottom = rectChild.bottom + iHeight/2;
    newRect.left = rectChild.left + iWidth/2;
    newRect.right = rectChild.right + iWidth/2;
    newRect.top = rectChild.top + iHeight/2;

    m_wndDialog.MoveWindow(&newRect);

    CScrollView::OnSize(nType, cx, cy);    
}
//---------------------------------------------------------------------------
void CNifCollisionControlView::OnUpdate(CView* pSender, LPARAM lHint,
    CObject* pHint) 
{
    m_wndDialog.OnUpdate(pSender, lHint, pHint);
}
//---------------------------------------------------------------------------
