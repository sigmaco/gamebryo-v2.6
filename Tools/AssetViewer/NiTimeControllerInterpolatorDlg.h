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

// NiTimeControllerInterpolatorsDlg.h

#ifndef NITIMECONTROLLERINTERPOLATORSDLG_H
#define NITIMECONTROLLERINTERPOLATORSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiTimeControllerInterpolatorsDlg : public CNiObjectDlg
{
// Construction
public:
    // standard constructor
    CNiTimeControllerInterpolatorsDlg(CWnd* pParent = NULL); 
    bool DoUpdate();
// Dialog Data
    //{{AFX_DATA(CNiTimeControllerInterpolatorsDlg)
    enum { IDD = IDD_NIINTERPOLATORS };
    CListCtrl   m_wndInterpolatorsList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiTimeControllerInterpolatorsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiTimeControllerInterpolatorsDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkNiInterpolatorList(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NITimeControllerInterpolatorSDLG_H
