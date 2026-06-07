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

// RemoveFileDlg.h

#ifndef REMOVEFILEDLG_H
#define REMOVEFILEDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRemoveFileDlg : public CDialog
{
// Construction
public:
    NiTStringMap<NiNode*>* m_pkNameToRoot;
    NiNode* m_pkNodeToRemove;
    CRemoveFileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CRemoveFileDlg)
    enum { IDD = IDD_REMOVE_FILE };
    CListBox    m_kFilesEdit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRemoveFileDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CRemoveFileDlg)
    afx_msg void OnDblclkListFiles();
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef REMOVEFILEDLG_H
