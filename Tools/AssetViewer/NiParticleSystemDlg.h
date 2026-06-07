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

// NiParticleSystemDlg.h

#ifndef NIPARTICLESYSTEMDLG_H
#define NIPARTICLESYSTEMDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiParticleSystemDlg : public CNiObjectDlg
{
// Construction
public:
    CNiParticleSystemDlg(CWnd* pParent = NULL);   // standard constructor
    ~CNiParticleSystemDlg();

    bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CNiParticleSystemDlg)
    enum { IDD = IDD_NIPARTICLESYSTEM };
    CEdit       m_wndParticleCountMax;
    CEdit       m_wndParticleCountCurrent;
    CEdit       m_wndIsWorldSpace;
    CEdit       m_wndHasColors;
    CEdit       m_wndHasRotation;
    CEdit       m_wndHasRotationAxis;
    CListCtrl   m_wndEmittersList;
    CListCtrl   m_wndSpawnersList;
    CButton     m_wndDeathSpawnerButton;
    CListCtrl   m_wndForcesList;
    CListCtrl   m_wndCollidersList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiParticleSystemDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiParticleSystemDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkEmittersList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkSpawnersList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeathSpawnerButton();
    afx_msg void OnDblclkForcesList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkCollidersList(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIPARTICLESYSTEMDLG_H
