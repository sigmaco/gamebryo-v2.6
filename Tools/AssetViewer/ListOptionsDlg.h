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

// ListOptionsDlg.h

#ifndef LISTOPTIONSDLG_H
#define LISTOPTIONSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CListOptionsDlg : public CDialog
{
// Construction
public:
    CListOptionsDlg(CWnd* pParent = NULL);   // standard constructor
    BOOL m_bCustom;

// Dialog Data
    //{{AFX_DATA(CListOptionsDlg)
    enum { IDD = IDD_LIST_OPTIONS };
    BOOL    m_bCameras;
    BOOL    m_bGeometry;
    BOOL    m_bLights;
    BOOL    m_bNodes;
    BOOL    m_bDynamicEffects;
    BOOL    m_bProperties;
    BOOL    m_bTimeControllers;
    CString m_strCustom;
    BOOL    m_bHidden;
    BOOL    m_bVisible;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListOptionsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

// Implementation
protected:
    CButton m_rbStandard;
    CButton m_rbCustom;

    // Generated message map functions
    //{{AFX_MSG(CListOptionsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnRadioButtonClicked();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif // #ifndef LISTOPTIONSDLG_H
