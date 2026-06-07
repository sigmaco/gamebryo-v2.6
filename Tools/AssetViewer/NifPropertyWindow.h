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

// NifPropertyWindow.h

#ifndef NIFPROPERTYWINDOW_H
#define NIFPROPERTYWINDOW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RollupCtrl.h"

class CNiObjectPropertyWindowCreator;
class CNiObjectDlg;

class CNifPropertyWindow : public CWnd
{
// Construction
public:
    CNifPropertyWindow();
    DECLARE_DYNCREATE(CNifPropertyWindow)

// Attributes
public:

// Operations
public:
    int InsertPage(char* pcTitle, CNiObjectDlg* pwndDlg, 
        bool bExpanded = false, int iIndex = -1);
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifPropertyWindow)
    //}}AFX_VIRTUAL
    void UpdateActivePages();
    void ScrollToPage(int iPage);
    CString& GetRegisteredClassName();
    // Implementation
public:
    virtual ~CNifPropertyWindow();
    CWnd* GetParentForPages();
    void SetCreator(CNiObjectPropertyWindowCreator* pkCreator);

    bool HasHeartbeat();
    NiObject* GetNiObject();
    void SetNiObject(NiObject* pkObj);
    bool PerformSuicideWatch(); //Returns true if suicide was committed due
                                //to NiObject having a refcount of 1 
                                //false if not

    // Generated message map functions
protected:
    CNiObjectPropertyWindowCreator* m_pkCreator;
    CString     m_strMyClass;
    CRollupCtrl& GetRollupControl();
    CRollupCtrl m_kRollups;
    NiObjectPtr m_spObject;

    bool m_bHasHeartbeat;
    //{{AFX_MSG(CNifPropertyWindow)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFPROPERTYWINDOW_H
