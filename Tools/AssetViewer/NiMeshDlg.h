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

// NiMeshDlg.h

#ifndef NIMESHDLG_H
#define NIMESHDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"
#include "afxwin.h"

class CNiMeshDlg : public CNiObjectDlg
{
// Construction
public:
    CNiMeshDlg(CWnd* pParent = NULL);   // standard constructor
    ~CNiMeshDlg();

    bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CNiMeshDlg)
    enum { IDD = IDD_NIMESH };
    CEdit       m_wndShaderName;
    CButton     m_wndMeshDataButton;
    CEdit       m_wndModelBoundRadiusEdit;
    CEdit       m_wndModelBoundCenterZEdit;
    CEdit       m_wndModelBoundCenterYEdit;
    CEdit       m_wndModelBoundCenterXEdit;
    CEdit       m_wndSubmeshCountEdit;
    CListCtrl   m_wndMeshModifierList;
    CListCtrl   m_wndDataStreamList;
    CTreeCtrl   m_wndTreeControl;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiMeshDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    CImageList* m_pilImages;
    void* m_pCachedTreeCtrl;    // Used to remember which data stream group
                                // was used to build the tree control.

    // Generated message map functions
    //{{AFX_MSG(CNiMeshDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkNiMeshModifierList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkNiDataStreamGroupList(NMHDR *pNMHDR, 
        LRESULT *pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    CEdit m_wndPrimitiveType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIMESHDLG_H
