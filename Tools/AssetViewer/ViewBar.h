// ViewBar.h: interface for the CViewBar class.
//
//////////////////////////////////////////////////////////////////////

#ifndef VIEWBAR_H
#define VIEWBAR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sizecbar.h"
#include "scbarg.h"
#include "scbarcf.h"

// You must #define this for viewbar to compile properly
#define TViewBarBase CSizingControlBarCF

class CViewBar : public TViewBarBase
{
    DECLARE_DYNAMIC(CViewBar);
public:
    CViewBar();
    virtual ~CViewBar();
    virtual BOOL Create(
        CWnd* pParentWnd,           // mandatory
        CRuntimeClass *pViewClass,  // mandatory
        CCreateContext *pContext = NULL,
        LPCTSTR lpszWindowName = NULL,
        DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
        UINT nID = AFX_IDW_PANE_FIRST);

protected:
    CFrameWnd *m_pFrameWnd;
    CCreateContext m_Context;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CViewBar)
    public:
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CViewBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif
