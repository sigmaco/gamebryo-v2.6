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

// NifStatisticsView.h

#ifndef NIFSTATISTICSVIEW_H
#define NIFSTATISTICSVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifDoc;

class CNifStatisticsView : public CView
{
protected:
    CNifStatisticsView();   // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CNifStatisticsView)

// Attributes
public:

// Operations
public:
    CNifDoc* GetDocument();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifStatisticsView)
    protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CNifStatisticsView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    CFont* m_pkFont;

    static void CALLBACK EXPORT StatisticsTimerProc(
        HWND hWnd,      // handle of CWnd that called SetTimer
        UINT nMsg,      // WM_TIMER
        UINT nIDEvent,  // timer identification
        DWORD dwTime);  // system time
    unsigned int m_uiTimerId;
    void UpdateStatistics();

    static CNifStatisticsView* m_pkThis;

    // Generated message map functions
protected:
    CListBox m_kStatisticsList;
    bool m_bListExists;
    //{{AFX_MSG(CNifStatisticsView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NifStatisticsView.cpp
inline CNifDoc* CNifStatisticsView::GetDocument()
   { return (CNifDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFSTATISTICSVIEW_H
