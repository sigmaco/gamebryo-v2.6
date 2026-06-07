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

// NifCollisionControls.h

#ifndef NIFCOLLISIONCONTROLS_H
#define NIFCOLLISIONCONTROLS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifCollisionControls : public CDialog
{
    DECLARE_DYNCREATE(CNifCollisionControls)

// Construction
public:
    CNifCollisionControls();
    ~CNifCollisionControls();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    void RefreshValues();

    char m_acPath[MAX_PATH];

// Dialog Data
    //{{AFX_DATA(CNifCollisionControls)
    enum { IDD = IDD_COLLISION_CONTROLS };
    CListBox    m_kCollideeList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CNifCollisionControls)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual void OnOK() {};
    virtual void OnCancel(){};

    // Generated message map functions
    //{{AFX_MSG(CNifCollisionControls)
    afx_msg void OnAddCollider();
    afx_msg void OnSetCollideePath();
    afx_msg void OnRemoveCollisionLab();
    afx_msg void OnCollisionLabOptions();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnDestroy();
    afx_msg void OnClose();
    afx_msg void OnDblclkListCollideeFiles();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFANIMATIONCONTROLS_H
