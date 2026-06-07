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

// PreferencesDlg.h

#ifndef PREFERENCESDLG_H
#define PREFERENCESDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorStatic.h"

class CPreferencesDlg : public CDialog
{
// Construction
public:
    NiColor m_kBackgroundColor;

    CPreferencesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CPreferencesDlg)
    enum { IDD = IDD_PREFERENCES };
    BOOL    m_bDefaultLights;
    BOOL    m_bPromptOnDiscard;
    UINT    m_uiYawSpeed;
    UINT    m_uiPitchSpeed;
    UINT    m_uiTranslateSpeedX;
    UINT    m_uiTranslateSpeedY;
    UINT    m_uiTranslateSpeedZ;
    BOOL    m_bHighlightSelected;
    BOOL    m_bShowABVs;
    //}}AFX_DATA

    int m_iCameraControlMode;
    bool m_bCameraControlModeChanged;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPreferencesDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

// Implementation
protected:
    CColorStatic m_wndBackgroundColor;

    // Generated message map functions
    //{{AFX_MSG(CPreferencesDlg)
    afx_msg void OnButtonChooseColor();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnButtonKeyboardShortcuts();
    afx_msg void OnCameraControlModeChanged();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef PREFERENCESDLG_H
