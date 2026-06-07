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

// NiBoundDlg.h

#ifndef NIBOUNDDLG_H
#define NIBOUNDDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiBoundDlg : public CNiObjectDlg
{
// Construction
public:
    CNiBoundDlg(CWnd* pParent = NULL);   // standard constructor

    virtual bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CNiBoundDlg)
    enum { IDD = IDD_WORLD_BOUNDS };
    CEdit   m_wndRadiusEdit;
    CEdit   m_wndCenterZEdit;
    CEdit   m_wndCenterYEdit;
    CEdit   m_wndCenterXEdit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiBoundDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiBoundDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIBOUNDDLG_H
