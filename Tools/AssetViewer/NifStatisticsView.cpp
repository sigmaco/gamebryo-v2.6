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

// NifStatisticsView.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifStatisticsView.h"
#include "NifDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNifStatisticsView* CNifStatisticsView::m_pkThis = NULL;

//---------------------------------------------------------------------------
// CNifStatisticsView
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifStatisticsView, CView)
//---------------------------------------------------------------------------
CNifStatisticsView::CNifStatisticsView()
{
    m_pkThis = this;
    m_bListExists = false;
    m_pkFont = new CFont;
    m_pkFont->CreatePointFont(80, "MS Sans Serif");
}
//---------------------------------------------------------------------------
CNifStatisticsView::~CNifStatisticsView()
{
    m_pkThis = NULL;
    delete m_pkFont;
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifStatisticsView, CView)
    //{{AFX_MSG_MAP(CNifStatisticsView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifStatisticsView drawing
//---------------------------------------------------------------------------
void CNifStatisticsView::OnDraw(CDC* pDC)
{
    // Draw code can go here if needed
}
//---------------------------------------------------------------------------
// CNifStatisticsView diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CNifStatisticsView::AssertValid() const
{
    CView::AssertValid();
}
//---------------------------------------------------------------------------
void CNifStatisticsView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
//---------------------------------------------------------------------------
CNifDoc* CNifStatisticsView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNifDoc)));
    return (CNifDoc*)m_pDocument;
}
#endif //_DEBUG
//---------------------------------------------------------------------------
// CNifStatisticsView message handlers
//---------------------------------------------------------------------------
int CNifStatisticsView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_kStatisticsList.Create(WS_CHILD | WS_VISIBLE | LBS_MULTICOLUMN |
        LBS_NOINTEGRALHEIGHT | LBS_USETABSTOPS,
        CRect(0, 0, 100, 100), this, IDC_LIST_STATISTICS);
    m_kStatisticsList.SetColumnWidth(175);
    m_kStatisticsList.SetFont(m_pkFont);
    m_bListExists = true;

    // Note: STATISTICS_TIMER_INCREMENT found in NifGlobals.h
    m_uiTimerId = (unsigned int)SetTimer(STATISTICS_TIMER_ID, STATISTICS_TIMER_INCREMENT,
        StatisticsTimerProc);

    return 0;
}
//---------------------------------------------------------------------------
void CNifStatisticsView::OnDestroy() 
{
    CView::OnDestroy();

    KillTimer(m_uiTimerId);
}
//---------------------------------------------------------------------------
void CNifStatisticsView::OnSize(UINT nType, int cx, int cy) 
{
    CView::OnSize(nType, cx, cy);

    if (m_bListExists)
    {
        m_kStatisticsList.MoveWindow(0, 0, cx, cy);
    }
}
//---------------------------------------------------------------------------
void CALLBACK EXPORT CNifStatisticsView::StatisticsTimerProc(
    HWND hWnd,      // handle of CWnd that called SetTimer
    UINT nMsg,      // WM_TIMER
    UINT nIDEvent,  // timer identification
    DWORD dwTime)   // system time
{
    if (m_pkThis)
    {
        m_pkThis->UpdateStatistics();
    }
}
//---------------------------------------------------------------------------
void CNifStatisticsView::UpdateStatistics()
{
    CNifStatisticsManager::Lock();
    CNifStatisticsManager* pkStats = 
        CNifStatisticsManager::AccessStatisticsManager();

    float fFrameRate = pkStats->GetFrameRate();
    float fUpdateTime = pkStats->GetUpdateTime();
    float fCullTime = pkStats->GetCullTime();
    float fRenderTime = pkStats->GetRenderTime();
    float fSwapTime = pkStats->GetSwapTime();
    unsigned int uiNumObjects = pkStats->GetNumObjects();
    unsigned int uiNumTris = pkStats->GetNumTriangles();
    unsigned int uiNumVerts = pkStats->GetNumVertices();
    unsigned int uiNumObjsDrawn = pkStats->GetNumObjectsDrawnPerFrame();

    CNifStatisticsManager::UnLock();

    m_kStatisticsList.ResetContent();
    CString strStats;
    strStats.Format("FPS:\t\t%.1f", fFrameRate);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Total Objects:\t%d", uiNumObjects);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Total Triangles:\t%d", uiNumTris);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Total Vertices:\t%d", uiNumVerts);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Objects Drawn:\t%d", uiNumObjsDrawn);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Update Time:\t%.2f ms", fUpdateTime * 1000.0f);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Cull Time:\t\t%.2f ms", fCullTime * 1000.0f);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Render Time:\t%.2f ms", fRenderTime * 1000.0f);
    m_kStatisticsList.AddString(strStats);
    strStats.Format("Swap Time:\t%.2f ms", fSwapTime * 1000.0f);
    m_kStatisticsList.AddString(strStats);
}
//---------------------------------------------------------------------------
void CNifStatisticsView::OnUpdate(CView* pSender, LPARAM lHint,
    CObject* pHint) 
{
    // Overloaded this method to help eliminate some of the flickering
}
//---------------------------------------------------------------------------
