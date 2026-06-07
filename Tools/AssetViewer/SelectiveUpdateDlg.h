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

// SelectiveUpdateDlg.h

#ifndef SELECTIVEUPDATEDLG_H
#define SELECTIVEUPDATEDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CSelectiveUpdateDlg : public CNiObjectDlg
{
// Construction
public:
    CSelectiveUpdateDlg(CWnd* pParent = NULL);   // standard constructor
    bool DoUpdate();
// Dialog Data
    //{{AFX_DATA(CSelectiveUpdateDlg)
    enum { IDD = IDD_UPDATE_FLAGS };
    CButton m_wndAppCulled;
    CButton m_wndTransformsCheck;
    CButton m_wndRigidCheck;
    CButton m_wndPropertyControllersCheck;
    CButton m_wndSelectiveUpdateCheck;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSelectiveUpdateDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CSelectiveUpdateDlg)
    afx_msg void OnAppculled();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef SELECTIVEUPDATEDLG_H
