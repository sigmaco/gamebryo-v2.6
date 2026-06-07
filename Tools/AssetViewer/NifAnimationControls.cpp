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

// NifAnimationControls.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifAnimationControls.h"
#include "NifAnimationOptionsDlg.h"
#include "NifDoc.h"    
#include "NifUserPreferences.h"
#include "NifTimeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifAnimationControls property page
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifAnimationControls, CDialog)
//---------------------------------------------------------------------------
CNifAnimationControls::CNifAnimationControls() :
    CDialog(CNifAnimationControls::IDD)
{
    //{{AFX_DATA_INIT(CNifAnimationControls)
    m_intTimeSliderValue = 0;
    m_fCurrentTime = 0.0f;
    //}}AFX_DATA_INIT
    m_bIsPlaying = false;
    m_bIsLooping = true;
    m_kImageIds[PLAY] =  IDB_PLAY;
    m_kImageIds[PAUSE] = IDB_PAUSE;
    m_kImageIds[STOP] = IDB_STOP;
    m_kImageIds[FIRST_FRAME] = IDB_FIRST_FRAME;
    m_kImageIds[LAST_FRAME] = IDB_LAST_FRAME;
    m_kImageIds[NEXT_FRAME] = IDB_NEXT_FRAME;
    m_kImageIds[PREVIOUS_FRAME] = IDB_PREVIOUS_FRAME;
    m_kImageIds[ANIMATIONS_LOOP] = IDB_ANIM_LOOPU ;
    m_kImageIds[ANIMATIONS_CONTINUOUS] = IDB_ANIM_LOOPD;
    m_kImageIds[ANIMATION_OPTIONS] = IDB_ANIM_OPTIONS;
    m_iFrameRate = 60;
    m_bUserHasAdjustedSlider = false;

}
//---------------------------------------------------------------------------
CNifAnimationControls::~CNifAnimationControls()
{
}
//---------------------------------------------------------------------------
void CNifAnimationControls::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNifAnimationControls)
    DDX_Control(pDX, IDC_START_TIME, m_wndStartTimeEdit);
    DDX_Control(pDX, IDC_END_TIME, m_wndEndTimeEdit);
    DDX_Control(pDX, IDC_CURRENT_TIME, m_kCurrentTimeEdit);
    DDX_Control(pDX, IDC_STOP, m_wndStopButton);
    DDX_Control(pDX, IDC_PREVIOUS_FRAME, m_wndPreviousFrameButton);
    DDX_Control(pDX, IDC_PLAY, m_wndPlayButton);
    DDX_Control(pDX, IDC_NEXT_FRAME, m_wndNextFrameButton);
    DDX_Control(pDX, IDC_LAST_FRAME, m_wndLastFrameButton);
    DDX_Control(pDX, IDC_FIRST_FRAME, m_wndFirstFrameButton);
    DDX_Control(pDX, IDC_ANIMATION_OPTIONS, m_wndOptionsButton);
    DDX_Control(pDX, IDC_ANIM_LOOP, m_wndAnimationLoopButton);
    DDX_Control(pDX, IDC_TIME_SLIDER, m_wndTimeSlider);
    DDX_Slider(pDX, IDC_TIME_SLIDER, m_intTimeSliderValue);
    DDX_Text(pDX, IDC_CURRENT_TIME, m_fCurrentTime);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifAnimationControls, CDialog)
    //{{AFX_MSG_MAP(CNifAnimationControls)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_PLAY, OnPlay)
    ON_BN_CLICKED(IDC_PREVIOUS_FRAME, OnPreviousFrame)
    ON_BN_CLICKED(IDC_STOP, OnStop)
    ON_BN_CLICKED(IDC_NEXT_FRAME, OnNextFrame)
    ON_BN_CLICKED(IDC_LAST_FRAME, OnLastFrame)
    ON_BN_CLICKED(IDC_FIRST_FRAME, OnFirstFrame)
    ON_BN_CLICKED(IDC_ANIMATION_OPTIONS, OnAnimationOptions)
    ON_BN_CLICKED(IDC_ANIM_LOOP, OnAnimLoop)
    ON_WM_HSCROLL()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifAnimationControls message handlers
//---------------------------------------------------------------------------
int CNifAnimationControls::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;


    for(unsigned int ui = PLAY; ui < NUM_IMAGES; ui++)
    {
        m_akBitmaps[ui].LoadBitmap(m_kImageIds[ui]);
    }

    return 0;
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnUpdate(CView* pSender, LPARAM lHint,
    CObject* pHint)
{
    WORD wLoWord = LOWORD(lHint);
    CNifDoc* pkDoc = CNifDoc::GetDocument();
        
    if (wLoWord == NIF_TIMINGINFOCHANGED)
    {
        RefreshValues();
    }
    else if(wLoWord == NIF_TIMECHANGED && pkDoc)
    {
        float fTime = pkDoc->GetLastTimeUpdated();
        char acValue [256];
        NiSprintf(acValue, 256,"%f", fTime);

        if(m_kCurrentTimeEdit.m_hWnd != NULL)
            m_kCurrentTimeEdit.SetWindowText(acValue);
        int iCurrentTime = ConvertTimeToSliderRange(fTime);
        if(m_wndTimeSlider.m_hWnd != NULL)
            m_wndTimeSlider.SetPos(iCurrentTime);
    }
}
//---------------------------------------------------------------------------
BOOL CNifAnimationControls::OnInitDialog() 
{
    m_fCurrentTime = 0.0f;
    CDialog::OnInitDialog();
    m_wndStopButton.SetBitmap(m_akBitmaps[STOP]);
    m_wndPreviousFrameButton.SetBitmap(m_akBitmaps[PREVIOUS_FRAME]);

    if(!IsPlaying())
        m_wndPlayButton.SetBitmap(m_akBitmaps[PLAY]);
    else
        m_wndPlayButton.SetBitmap(m_akBitmaps[PAUSE]);

    m_wndNextFrameButton.SetBitmap(m_akBitmaps[NEXT_FRAME]);
    m_wndLastFrameButton.SetBitmap(m_akBitmaps[LAST_FRAME]);
    m_wndFirstFrameButton.SetBitmap(m_akBitmaps[FIRST_FRAME]);
    m_wndOptionsButton.SetBitmap(m_akBitmaps[ANIMATION_OPTIONS]);

    if(IsLooping())
    {
        m_wndAnimationLoopButton.SetBitmap(m_akBitmaps[ANIMATIONS_LOOP]);
    }
    else
    {
        m_wndAnimationLoopButton.SetBitmap(
            m_akBitmaps[ANIMATIONS_CONTINUOUS]);
    }

    RefreshValues();
    
    return TRUE;
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnPlay() 
{
    SetPlayState(!IsPlaying());
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnPreviousFrame() 
{
    AdjustTime(PREVIOUS);
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnStop() 
{
    SetPlayState(false);
    OnFirstFrame();
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnNextFrame() 
{
    AdjustTime(NEXT);
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnLastFrame() 
{
    AdjustTime(LAST);
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnFirstFrame() 
{
    AdjustTime(FIRST);
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnAnimationOptions() 
{
    CNifAnimationOptionsDlg kDialog;
    kDialog.DoModal();
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnAnimLoop() 
{
    SetLoopState(!IsLooping());
}
//---------------------------------------------------------------------------
bool CNifAnimationControls::IsPlaying()
{
    return m_bIsPlaying;
}
//---------------------------------------------------------------------------
bool CNifAnimationControls::IsLooping()
{
    return m_bIsLooping;
}
//---------------------------------------------------------------------------
void CNifAnimationControls::SetPlayState(bool bPlay)
{
    m_bIsPlaying = bPlay;
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(!pkDoc)
        return;

    pkDoc->Lock();
    CNifTimeManager* pkTimeManager = pkDoc->GetTimeManager();

    if(!IsPlaying() && pkTimeManager)
    {
        pkTimeManager->Disable();
        m_wndPlayButton.SetBitmap(m_akBitmaps[PLAY]);
    }
    else if(pkTimeManager)
    {
        pkTimeManager->Enable();
        m_wndPlayButton.SetBitmap(m_akBitmaps[PAUSE]);
    }
    pkDoc->UnLock();

}
//---------------------------------------------------------------------------
void CNifAnimationControls::SetLoopState(bool bLoop)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(!pkDoc)
        return;

    pkDoc->Lock();
    CNifTimeManager* pkTimeManager = pkDoc->GetTimeManager();
    
    m_bIsLooping = bLoop;
    if(IsLooping())
    {
        pkTimeManager->SetTimeMode(CNifTimeManager::LOOP);
        m_wndAnimationLoopButton.SetBitmap(m_akBitmaps[ANIMATIONS_LOOP]);
    }
    else
    {
        pkTimeManager->SetTimeMode(CNifTimeManager::CONTINUOUS);
        m_wndAnimationLoopButton.SetBitmap(m_akBitmaps[ANIMATIONS_CONTINUOUS]);
    }

    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnHScroll(UINT nSBCode, UINT nPos, 
                                      CScrollBar* pScrollBar) 
{
    if(nSBCode != TB_ENDTRACK)
    {
        AdjustTime(SLIDE);
    }
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnDestroy() 
{
    CDialog::OnDestroy();
}
//---------------------------------------------------------------------------
void CNifAnimationControls::AdjustTime(TimeAdjustType eControlID)
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(!pkDoc)
        return;

    pkDoc->Lock();
    CNifTimeManager* pkTimeManager = pkDoc->GetTimeManager();

    float fLastTime = pkDoc->GetLastTimeUpdated();
    switch(eControlID)
    {
        case FIRST:
            pkTimeManager->SetCurrentTime(pkTimeManager->GetStartTime());
            break;
        case PREVIOUS:
            pkTimeManager->SetCurrentTime(fLastTime -
                1.0f / (float) m_iFrameRate);
            break;
        case NEXT:
            pkTimeManager->SetCurrentTime(fLastTime +
                1.0f / (float) m_iFrameRate);
            break;
        case LAST:
            pkTimeManager->SetCurrentTime(pkTimeManager->GetEndTime());
            break;
        case SLIDE:
            {
                int iPos = m_wndTimeSlider.GetPos();
                //NiOutputDebugString("AdjustTime: ");
                m_fCurrentTime = ConvertSliderValueToTime(iPos);
                
                if(fLastTime != m_fCurrentTime)
                {
                    pkTimeManager->SetCurrentTime(m_fCurrentTime);
                    pkDoc->UpdateScene();
                }
            }
            break;
        default:
            break;
    }

    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifAnimationControls::OnClose() 
{

    CDialog::OnClose();
}
//---------------------------------------------------------------------------
BOOL CNifAnimationControls::DestroyWindow() 
{
    return CDialog::DestroyWindow();
}
//---------------------------------------------------------------------------
void CNifAnimationControls::RefreshValues()
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(!pkDoc) 
        return;

    pkDoc->Lock();
    CNifTimeManager* pkTimeManager = pkDoc->GetTimeManager();

    CNifUserPreferences::Lock();
    float fTicksPerSec = CNifUserPreferences::
        AccessUserPreferences()->GetAnimationSliderSecondsPerTick();
    CNifUserPreferences::UnLock();

    if(!pkTimeManager)
    {
        pkDoc->UnLock();
        return;
    }
    m_fStart = pkTimeManager->GetStartTime();
    m_fEnd = pkTimeManager->GetEndTime();

    m_fNumSeconds = m_fEnd - m_fStart;
    m_iSliderSpan = (int)(m_fNumSeconds*m_iFrameRate);
    int iLastTime = ConvertTimeToSliderRange(pkDoc->GetLastTimeUpdated());
    
    m_wndTimeSlider.SetRange(0, m_iSliderSpan, TRUE);

    m_wndTimeSlider.SetPos(iLastTime);

    int iTickFreq = (int) (fTicksPerSec*m_iFrameRate);
    m_wndTimeSlider.SetTicFreq(iTickFreq);

    char acString[256];
    NiSprintf(acString, 256, "%.3f", m_fStart);
    m_wndStartTimeEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.3f", m_fEnd);
    m_wndEndTimeEdit.SetWindowText(acString);

    if(pkTimeManager->GetTimeMode() == CNifTimeManager::CONTINUOUS)
    {
        m_bIsLooping = false;
        m_wndAnimationLoopButton.SetBitmap(
            m_akBitmaps[ANIMATIONS_CONTINUOUS]);
    }
    else if (pkTimeManager->GetTimeMode() == CNifTimeManager::LOOP)
    {
        m_bIsLooping = true;
        m_wndAnimationLoopButton.SetBitmap(m_akBitmaps[ANIMATIONS_LOOP]);
    }

    if(pkTimeManager->IsEnabled())
    {
        m_bIsPlaying = true;
        m_wndPlayButton.SetBitmap(m_akBitmaps[PAUSE]);
    }
    else 
    {
        m_bIsPlaying = false;
        m_wndPlayButton.SetBitmap(m_akBitmaps[PLAY]);
    }
    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
int CNifAnimationControls::ConvertTimeToSliderRange(float fTime)
{
    float fInSpan = fTime - m_fStart;
    fInSpan *= 60.0f;
    return (int) fInSpan;
}
//---------------------------------------------------------------------------
float CNifAnimationControls::ConvertSliderValueToTime(int iValue)
{
    float fTime = ((float)iValue) / ((float)m_iFrameRate);
    fTime += m_fStart;

    return fTime;
}
//---------------------------------------------------------------------------
