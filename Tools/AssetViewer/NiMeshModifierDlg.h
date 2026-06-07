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

// NiMeshModifierDlg.h

#ifndef NIMESHMODIFIERMODIFIERDLG_H
#define NIMESHMODIFIERMODIFIERDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiMeshModifierDlg : public CNiObjectDlg
{
// Construction
public:
    CNiMeshModifierDlg(CWnd* pParent = NULL);   // standard constructor
    ~CNiMeshModifierDlg();

    bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CNiMeshModifierDlg)
    enum { IDD = IDD_NIMESHMODIFIER };
    CEdit       m_wndModifierName;
    CEdit       m_wndSubmitSynchNames;
    CEdit       m_wndCompleteSynchNames;
    CListBox    m_wndViewerStringsList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiMeshModifierDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiMeshModifierDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIMESHMODIFIERMODIFIERDLG_H
