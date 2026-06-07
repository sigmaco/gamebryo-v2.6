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

// NifCollisionOptionsDlg.h

#ifndef NIFCOLLISIONOPTIONSDLG_H
#define NIFCOLLISIONOPTIONSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifCollisionOptionsDlg : public CDialog
{
// Construction
public:
    CNifCollisionOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CNifCollisionOptionsDlg)
    enum { IDD = IDD_COLLISION_OPTIONS };
    CEdit m_wndDynamicVelocity;
    float   m_fDynamicVelocity;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifCollisionOptionsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// Implementation
protected:
    bool m_bInitialized;
    // Generated message map functions
    //{{AFX_MSG(CNifCollisionOptionsDlg)
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnChangeDynamicVelocity(); 

    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFCOLLISIONOPTIONSDLG_H
