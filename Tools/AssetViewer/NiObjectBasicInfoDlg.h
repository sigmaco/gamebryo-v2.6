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

// NiObjectBasicInfoDlg.h

#ifndef NIOBJECTBASICINFODLG_H
#define NIOBJECTBASICINFODLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiObjectBasicInfoDlg : public CNiObjectDlg
{
// Construction
public:
    CNiObjectBasicInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CNiObjectBasicInfoDlg)
    enum { IDD = IDD_BASIC_INFO };
    CButton m_wndParentButton;
    CEdit   m_wndRefCountEdit;
    CEdit   m_wndPointerEdit;
    CEdit   m_wndNameEdit;
    CEdit   m_wndClassEdit;
    //}}AFX_DATA

    virtual bool DoUpdate();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiObjectBasicInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiObjectBasicInfoDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnParentButton();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIOBJECTBASICINFODLG_H
