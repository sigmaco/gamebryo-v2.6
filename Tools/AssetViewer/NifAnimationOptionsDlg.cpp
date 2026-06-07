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

// NifAnimationOptionsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifAnimationOptionsDlg.h"
#include "NifDoc.h"
#include "NifUserPreferences.h"
#include "NifTimeManager.h"
#include "NifAnimationInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifAnimationOptionsDlg dialog
//---------------------------------------------------------------------------
CNifAnimationOptionsDlg::CNifAnimationOptionsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNifAnimationOptionsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNifAnimationOptionsDlg)
    m_fEndTime = 0.0f;
    m_fPlaybackRate = 0.0f;
    m_fStartingTime = 0.0f;
    m_fTickRate = 0.0f;
    m_bInitialized = false;
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNifAnimationOptionsDlg)
    DDX_Control(pDX, IDC_TOTAL_KEYFRAME_COUNT, m_wndTotalKeyframeEdit);
    DDX_Control(pDX, IDC_TIME_SLIDER_TICK_RATE, m_wndTimeSliderTickRateEdit);
    DDX_Control(pDX, IDC_STARTING_TIME, m_wndStartingTimeEdit);
    DDX_Control(pDX, IDC_PLAYBACKRATE_SLIDER, m_wndPlaybackRateSlider);
    DDX_Control(pDX, IDC_PLAYBACK_RATE, m_wndPlaybackRateEdit);
    DDX_Control(pDX, IDC_LOWEST_KEYFRAME_TIME, m_wndLowestKeyframeTimeEdit);
    DDX_Control(pDX, IDC_KEYFRAME_CONTROLLER_COUNT, 
        m_wndKeyframeControllerCountEdit);
    DDX_Control(pDX, IDC_ENDING_TIME, m_wndEndingTimeEdit);
    DDX_Control(pDX, IDC_HIGHEST_KEYFRAME_TIME, m_wndHighestKeyframeTime);
    DDX_Text(pDX, IDC_ENDING_TIME, m_fEndTime);
    DDV_MinMaxFloat(pDX, m_fEndTime, 0.f, 999999.0f);
    DDX_Text(pDX, IDC_PLAYBACK_RATE, m_fPlaybackRate);
    DDV_MinMaxFloat(pDX, m_fPlaybackRate, 0.f, 400.f);
    DDX_Text(pDX, IDC_STARTING_TIME, m_fStartingTime);
    DDX_Text(pDX, IDC_TIME_SLIDER_TICK_RATE, m_fTickRate);
    DDV_MinMaxFloat(pDX, m_fTickRate, 0.f, 1000.f);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifAnimationOptionsDlg, CDialog)
    //{{AFX_MSG_MAP(CNifAnimationOptionsDlg)
    ON_EN_CHANGE(IDC_PLAYBACK_RATE, OnChangePlaybackRate)
    ON_EN_CHANGE(IDC_STARTING_TIME, OnChangeStartingTime)
    ON_EN_CHANGE(IDC_TIME_SLIDER_TICK_RATE, OnChangeTimeSliderTickRate)
    ON_EN_CHANGE(IDC_ENDING_TIME, OnChangeEndingTime)
    ON_WM_HSCROLL()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifAnimationOptionsDlg message handlers
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnOK() 
{
    CDialog::OnOK();
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->Lock();
        CNifAnimationInfo* pkInfo = pkDoc->GetAnimationInfo();
        CNifTimeManager* pkManager = pkDoc->GetTimeManager();
        if(!pkManager || !pkInfo)
            return;

        pkManager->SetEndTime(m_fEndTime);
        pkManager->SetStartTime(m_fStartingTime);
        pkManager->SetScaleFactor(m_fPlaybackRate/100.0f);

        if (pkManager->GetStartTime() == pkManager->GetEndTime())
        {
            pkManager->SetTimeMode(CNifTimeManager::CONTINUOUS);
        }

        CNifUserPreferences::Lock();
        CNifUserPreferences* pkPrefs = 
            CNifUserPreferences::AccessUserPreferences();
        
        pkPrefs->SetAnimationSliderSecondsPerTick(m_fTickRate);
        CNifUserPreferences::UnLock();
        pkDoc->UpdateAllViews(NULL, MAKELPARAM(NIF_TIMINGINFOCHANGED,NULL),
            NULL);
        pkDoc->UnLock();
    }


}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnCancel() 
{
    CDialog::OnCancel();
}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnChangePlaybackRate() 
{
    if(m_bInitialized)
    {
 
        int nMaxCount = 256;
        char acString[256];
    
        m_wndPlaybackRateEdit.GetWindowText(acString, nMaxCount);
        int iValue = atoi(acString);
        if(iValue < 0)
        {
            iValue = 0;
            NiSprintf(acString, 256,"%d", iValue);
            m_wndPlaybackRateEdit.SetWindowText(acString);
        }
        else if(iValue > 400)
        {
            iValue = 400;
            NiSprintf(acString, 256,"%d", iValue);
            m_wndPlaybackRateEdit.SetWindowText(acString);
        }
    
        m_wndPlaybackRateSlider.SetPos(iValue);
    }
}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnChangeStartingTime() 
{
    if(m_bInitialized)
    {
        int nMaxCount = 256;
        char acString[256];
    
        m_wndStartingTimeEdit.GetWindowText(acString, nMaxCount);
        double dValue = atof(acString);
        if(dValue > m_fLastEndTime)
        {
            dValue = m_fLastStartTime;
            NiSprintf(acString, 256, "%.3f", dValue);
            m_wndStartingTimeEdit.SetWindowText(acString);
        }

        m_fLastStartTime = (float) dValue;
    }   
}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnChangeTimeSliderTickRate() 
{
}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnChangeEndingTime() 
{
    if(m_bInitialized)
    {
        int nMaxCount = 256;
        char acString[256];
    
        m_wndEndingTimeEdit.GetWindowText(acString, nMaxCount);
        double dValue = atof(acString);
        if(dValue < m_fLastStartTime)
        {
            dValue = m_fLastEndTime;
            NiSprintf(acString, 256, "%.3f", dValue);
            m_wndEndingTimeEdit.SetWindowText(acString);
        }

        m_fLastEndTime = (float) dValue;
    }
    
}
//---------------------------------------------------------------------------
BOOL CNifAnimationOptionsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->Lock();
        CNifAnimationInfo* pkInfo = pkDoc->GetAnimationInfo();
        CNifTimeManager* pkManager = pkDoc->GetTimeManager();
        if(!pkManager || !pkInfo)
            return TRUE;

        char acString[256];
        //Init starting time
        m_fLastStartTime = pkManager->GetStartTime();
        NiSprintf(acString, 256, "%.3f", m_fLastStartTime);
        m_wndStartingTimeEdit.SetWindowText(acString);

        //Init ending time
        m_fLastEndTime = pkManager->GetEndTime();
        NiSprintf(acString, 256, "%.3f", m_fLastEndTime);
        m_wndEndingTimeEdit.SetWindowText(acString);

        //Init total keyframe count
        NiSprintf(acString, 256, "%d", pkInfo->GetNumberOfKeyframes());
        m_wndTotalKeyframeEdit.SetWindowText(acString);

        //Init playback rate
        NiSprintf(acString, 256, "%d", (int)(pkManager->GetScaleFactor() *
            100.0f));
        m_wndPlaybackRateEdit.SetWindowText(acString);
        m_wndPlaybackRateSlider.SetRange(0, 400, TRUE);
        m_wndPlaybackRateSlider.SetPos((int)
            (pkManager->GetScaleFactor()*100));
        m_wndPlaybackRateSlider.SetTicFreq(50);

        //Init time slider tick rate
        CNifUserPreferences::Lock();
        float fTickRate = CNifUserPreferences::
            AccessUserPreferences()->GetAnimationSliderSecondsPerTick();
        CNifUserPreferences::UnLock();

        NiSprintf(acString, 256,"%.4f", fTickRate);
        m_wndTimeSliderTickRateEdit.SetWindowText(acString);

        //Init lowest keyframe time
        NiSprintf(acString, 256, "%.3f", pkInfo->GetMinTime());
        m_wndLowestKeyframeTimeEdit.SetWindowText(acString);

        //Init highest keyframe time
        NiSprintf(acString, 256, "%.3f", pkInfo->GetMaxTime());
        m_wndHighestKeyframeTime.SetWindowText(acString);

        //Init controller count
        NiSprintf(acString, 256, "%d", pkInfo->GetNumberOfControllers());
        m_wndKeyframeControllerCountEdit.SetWindowText(acString);
                 
        m_bInitialized = true;
        pkDoc->UnLock();
    }
    
    return TRUE;
}
//---------------------------------------------------------------------------
void CNifAnimationOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos,
    CScrollBar* pScrollBar) 
{
    char acString[256];

    if (nSBCode != TB_ENDTRACK)
    {
        nPos = m_wndPlaybackRateSlider.GetPos();   
        NiSprintf(acString, 256, "%d", nPos);
        m_wndPlaybackRateEdit.SetWindowText(acString);
    }

    NiSprintf(acString, 256, "%d, %d\n", nSBCode, nPos);
    NiOutputDebugString(acString);
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
//---------------------------------------------------------------------------
int CNifAnimationOptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    lpCreateStruct->style &= WS_THICKFRAME | WS_MAXIMIZEBOX; 
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}
//---------------------------------------------------------------------------
BOOL CNifAnimationOptionsDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
    // Here we modify the construction params and call the base class

    // removes min/max boxes
    cs.style &= (WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    // makes dialog box unsizable
    cs.style &= WS_THICKFRAME;
    return CDialog::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------
