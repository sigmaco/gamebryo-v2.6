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

// NifAnimationOptionsDlg.h

#ifndef NIFANIMATIONOPTIONSDLG_H
#define NIFANIMATIONOPTIONSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifAnimationOptionsDlg : public CDialog
{
// Construction
public:
    CNifAnimationOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CNifAnimationOptionsDlg)
    enum { IDD = IDD_ANIMATION_OPTIONS };
    CEdit   m_wndTotalKeyframeEdit;
    CEdit   m_wndTimeSliderTickRateEdit;
    CEdit   m_wndStartingTimeEdit;
    CSliderCtrl m_wndPlaybackRateSlider;
    CEdit   m_wndPlaybackRateEdit;
    CEdit   m_wndLowestKeyframeTimeEdit;
    CEdit   m_wndKeyframeControllerCountEdit;
    CEdit   m_wndEndingTimeEdit;
    CEdit   m_wndHighestKeyframeTime;
    float   m_fEndTime;
    float   m_fPlaybackRate;
    float   m_fStartingTime;
    float   m_fTickRate;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifAnimationOptionsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// Implementation
protected:
    float m_fLastStartTime;
    float m_fLastEndTime;
    bool m_bInitialized;
    // Generated message map functions
    //{{AFX_MSG(CNifAnimationOptionsDlg)
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnChangePlaybackRate();
    afx_msg void OnChangeStartingTime();
    afx_msg void OnChangeTimeSliderTickRate();
    afx_msg void OnChangeEndingTime();
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFANIMATIONOPTIONSDLG_H
