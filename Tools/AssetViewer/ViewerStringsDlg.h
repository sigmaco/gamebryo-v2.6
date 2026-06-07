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

// ViewerStringsDlg.h

#ifndef VIEWERSTRINGSDLG_H
#define VIEWERSTRINGSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CViewerStringsDlg : public CNiObjectDlg
{
    
// Construction
public:
    CViewerStringsDlg(CWnd* pParent = NULL);   // standard constructor

    enum ShowType { FULL, LIMITED};
    void SetShowType(ShowType eShowType) { m_eShowType = eShowType; };

    virtual bool DoUpdate();
// Dialog Data
    //{{AFX_DATA(CViewerStringsDlg)
    enum { IDD = IDD_VIEWER_STRINGS };
    CListBox    m_wndViewerStringsList;
    //}}AFX_DATA
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CViewerStringsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    ShowType m_eShowType;

    void SetStrings();
    NiTPrimitiveArray<char*> m_kStringArray;
    // Generated message map functions
    //{{AFX_MSG(CViewerStringsDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef VIEWERSTRINGSDLG_H
