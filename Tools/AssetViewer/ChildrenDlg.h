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

// ChildrenDlg.h

#ifndef CHILDRENDLG_H
#define CHILDRENDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CChildrenDlg : public CNiObjectDlg
{
// Construction
public:
    CChildrenDlg(CWnd* pParent = NULL);   // standard constructor
    bool DoUpdate();

    void GatherRecursiveInfo(NiAVObject* pkObj, int& iDepthComplexity,
        int& iSubTreeItemCount);

// Dialog Data
    //{{AFX_DATA(CChildrenDlg)
    enum { IDD = IDD_CHILDREN };
    CEdit   m_wndSubTreeDepthComplexityEdit;
    CEdit   m_wndSubTreeObjectCountEdit;
    CEdit   m_wndChildCountEdit;
    CListCtrl   m_wndChildrenList;
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CChildrenDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CChildrenDlg)
    afx_msg void OnDblclkChildrenList(NMHDR* pNMHDR, LRESULT* pResult);
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef CHILDRENDLG_H
