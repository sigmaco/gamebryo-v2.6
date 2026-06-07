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

// ChooseKFMDlg.h

#ifndef CHOOSEKFMDLG_H
#define CHOOSEKFMDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChooseKFMDlg : public CDialog
{
// Construction
public:
    NiTSet<NiNode*>* m_pkKFMRoots;
    NiNode* m_pkRoot;
    CChooseKFMDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CChooseKFMDlg)
    enum { IDD = IDD_CHOOSE_KFM };
    CListBox    m_kKFMFilesList;
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CChooseKFMDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CChooseKFMDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnDblclkListKfmFiles();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef CHOOSEKFMDLG_H
