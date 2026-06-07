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

// NiTimeControllerDlg.h

#ifndef NITIMECONTROLLERDLG_H
#define NITIMECONTROLLERDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiTimeControllerDlg : public CNiObjectDlg
{
// Construction
public:
    CNiTimeControllerDlg(CWnd* pParent = NULL);   // standard constructor
    bool DoUpdate();

// Dialog Data
    //{{AFX_DATA(CNiTimeControllerDlg)
    enum { IDD = IDD_NITIMECONTROLLER };
    CEdit   m_wndTotalKeyCountEdit;
    CButton m_wndNextButton;
    CButton m_wndTargetButton;
    CEdit   m_wndPhaseEdit;
    CButton m_wndIsTransformControllerRadio;
    CEdit   m_wndFrequencyEdit;
    CEdit   m_wndEndKeyTimeEdit;
    CComboBox   m_wndCycleTypeCombo;
    CEdit   m_wndBeginKeyTimeEdit;
    CComboBox   m_wndAnimTypeCombo;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiTimeControllerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    int GetInfo(NiTimeController* pkController);
    int GetInterpInfo(NiInterpController* pkController);
    // Generated message map functions
    //{{AFX_MSG(CNiTimeControllerDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnNitimecontrollerTargetButton();
    afx_msg void OnNitimecontrollerNextButton();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NITIMECONTROLLERDLG_H
