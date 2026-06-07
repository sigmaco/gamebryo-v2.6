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

// CollisionDlg.h

#ifndef COLLISIONDLG_H
#define COLLISIONDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CCollisionDlg : public CNiObjectDlg
{
// Construction
public:
    CCollisionDlg(CWnd* pParent = NULL);   // standard constructor
    virtual bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CCollisionDlg)
    enum { IDD = IDD_COLLISION };
    CComboBox   m_wndBVLocalCombo;
    CComboBox   m_wndBVWorldCombo;
    CComboBox   m_wndPropagateModeCombo;
    CEdit   m_wndVelocityZWorldEdit;
    CEdit   m_wndVelocityYWorldEdit;
    CEdit   m_wndVelocityXWorldEdit;
    CEdit   m_wndVelocityZEdit;
    CEdit   m_wndVelocityYEdit;
    CEdit   m_wndVelocityXEdit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCollisionDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CCollisionDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef COLLISIONDLG_H
