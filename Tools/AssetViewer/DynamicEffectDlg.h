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

// DynamicEffectDlg.h

#ifndef DYNAMICEFFECTDLG_H
#define DYNAMICEFFECTDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CDynamicEffectDlg : public CNiObjectDlg
{
// Construction
public:
    CDynamicEffectDlg(CWnd* pParent = NULL);   // standard constructor
    bool DoUpdate();

// Dialog Data
    //{{AFX_DATA(CDynamicEffectDlg)
    enum { IDD = IDD_NIDYNAMIC_EFFECT };
    CComboBox   m_wndEffectTypeCombo;
    CListCtrl   m_wndAffectedNodeList;
    CButton m_wndSwitchCheck;
    CButton m_wndShadowGeneratorButton;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDynamicEffectDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDynamicEffectDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkNidynamiceffectAffectednodeList(NMHDR* pNMHDR, 
        LRESULT* pResult);
    afx_msg void OnBnClickedButtonShadowgenerator();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef DYNAMICEFFECTDLG_H
