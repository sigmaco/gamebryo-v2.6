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

// NifAnimationControls.h

#ifndef NIFANIMATIONCONTROLS_H
#define NIFANIMATIONCONTROLS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifAnimationControls : public CDialog
{
    DECLARE_DYNCREATE(CNifAnimationControls)

// Construction
public:
    CNifAnimationControls();
    ~CNifAnimationControls();
    bool IsPlaying();
    bool IsLooping();
    void SetPlayState(bool bPlay);
    void SetLoopState(bool bLoop);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    void RefreshValues();

// Dialog Data
    //{{AFX_DATA(CNifAnimationControls)
    enum { IDD = IDD_ANIMATION_CONTROLS };
    CEdit   m_wndStartTimeEdit;
    CEdit   m_wndEndTimeEdit;
    CEdit   m_kCurrentTimeEdit;
    CButton m_wndStopButton;
    CButton m_wndPreviousFrameButton;
    CButton m_wndPlayButton;
    CButton m_wndNextFrameButton;
    CButton m_wndLastFrameButton;
    CButton m_wndFirstFrameButton;
    CButton m_wndOptionsButton;
    CButton m_wndAnimationLoopButton;
    CSliderCtrl m_wndTimeSlider;
    int     m_intTimeSliderValue;
    float   m_fCurrentTime;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CNifAnimationControls)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    enum 
    {
        PLAY = 0,
        PAUSE,
        STOP,
        FIRST_FRAME,
        LAST_FRAME,
        NEXT_FRAME,
        PREVIOUS_FRAME,
        ANIMATIONS_LOOP,
        ANIMATIONS_CONTINUOUS,
        ANIMATION_OPTIONS,
        NUM_IMAGES
    };

    enum TimeAdjustType
    {
        FIRST = 0,
        PREVIOUS,
        NEXT,
        LAST,
        SLIDE
    };

    virtual void OnOK() {};
    virtual void OnCancel(){};
    void AdjustTime(TimeAdjustType eControlID);

    bool m_bIsPlaying;
    bool m_bIsLooping;
    UINT m_kImageIds[NUM_IMAGES];
    CBitmap m_akBitmaps[NUM_IMAGES];

    float m_fNumSeconds;
    int m_iFrameRate;
    int m_iSliderSpan;
    float m_fStart;
    float m_fEnd;
    bool m_bUserHasAdjustedSlider;
    int ConvertTimeToSliderRange(float fTime);
    float ConvertSliderValueToTime(int iValue);

    // Generated message map functions
    //{{AFX_MSG(CNifAnimationControls)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual BOOL OnInitDialog();
    afx_msg void OnPlay();
    afx_msg void OnPreviousFrame();
    afx_msg void OnStop();
    afx_msg void OnNextFrame();
    afx_msg void OnLastFrame();
    afx_msg void OnFirstFrame();
    afx_msg void OnAnimationOptions();
    afx_msg void OnAnimLoop();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnDestroy();
    afx_msg void OnClose();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFANIMATIONCONTROLS_H
