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

#ifndef DISCARDOPENFILESDLG_H
#define DISCARDOPENFILESDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DiscardOpenFilesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiscardOpenFilesDlg dialog

class CDiscardOpenFilesDlg : public CDialog
{
// Construction
public:
    CDiscardOpenFilesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CDiscardOpenFilesDlg)
    enum { IDD = IDD_DISCARD_OPEN_FILES };
    BOOL    m_bDontPromptAgain;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDiscardOpenFilesDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDiscardOpenFilesDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif // #ifndef DISCARDOPENFILESDLG_H
