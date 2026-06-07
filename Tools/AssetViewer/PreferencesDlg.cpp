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

// PreferencesDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "PreferencesDlg.h"
#include "NifUserPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CPreferencesDlg dialog
//---------------------------------------------------------------------------
CPreferencesDlg::CPreferencesDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CPreferencesDlg::IDD, pParent)
    , m_iCameraControlMode(0)
{
    //{{AFX_DATA_INIT(CPreferencesDlg)
    m_bDefaultLights = FALSE;
    m_bPromptOnDiscard = FALSE;
    m_uiYawSpeed = 0;
    m_uiPitchSpeed = 0;
    m_uiTranslateSpeedX = 0;
    m_uiTranslateSpeedY = 0;
    m_uiTranslateSpeedZ = 0;
    m_bHighlightSelected = FALSE;
    m_bShowABVs = FALSE;
    //}}AFX_DATA_INIT

    m_bCameraControlModeChanged = false;
}
//---------------------------------------------------------------------------
void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPreferencesDlg)
    DDX_Check(pDX, IDC_CHECK_DEFAULT_LIGHTS, m_bDefaultLights);
    DDX_Check(pDX, IDC_CHECK_PROMPT_ON_DISCARD, m_bPromptOnDiscard);
    DDX_Text(pDX, IDC_EDIT_YAW, m_uiYawSpeed);
    DDX_Text(pDX, IDC_EDIT_PITCH, m_uiPitchSpeed);
    DDX_Text(pDX, IDC_EDIT_TRANSLATE_X, m_uiTranslateSpeedX);
    DDX_Text(pDX, IDC_EDIT_TRANSLATE_Y, m_uiTranslateSpeedY);
    DDX_Text(pDX, IDC_EDIT_TRANSLATE_Z, m_uiTranslateSpeedZ);
    DDX_Check(pDX, IDC_CHECK_HIGHLIGHT_SELECTED, m_bHighlightSelected);
    DDX_Check(pDX, IDC_CHECK_SHOW_ABVS, m_bShowABVs);
    DDX_CBIndex(pDX, IDC_COMBO_CAMERACONTROLMODE, m_iCameraControlMode);
    //}}AFX_DATA_MAP

    NIASSERT(
        m_iCameraControlMode >= 0 &&
        m_iCameraControlMode < CNifUserPreferences::CCMODE_COUNT);
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
    //{{AFX_MSG_MAP(CPreferencesDlg)
    ON_BN_CLICKED(IDC_BUTTON_CHOOSE_COLOR, OnButtonChooseColor)
    ON_BN_CLICKED(IDC_BUTTON_KEYBOARD_SHORTCUTS, OnButtonKeyboardShortcuts)
    ON_CBN_SELCHANGE(IDC_COMBO_CAMERACONTROLMODE, OnCameraControlModeChanged)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CPreferencesDlg message handlers
//---------------------------------------------------------------------------
BOOL CPreferencesDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    m_wndBackgroundColor.Attach(GetDlgItem(IDC_STATIC_BACKGROUND_COLOR)
        ->m_hWnd);
    m_wndBackgroundColor.SetBackgroundColor(m_kBackgroundColor);

    return TRUE;
}
//---------------------------------------------------------------------------
void CPreferencesDlg::OnOK() 
{
    CDialog::OnOK();
}
//---------------------------------------------------------------------------
void CPreferencesDlg::PostNcDestroy() 
{
    m_wndBackgroundColor.Detach();

    CDialog::PostNcDestroy();
}
//---------------------------------------------------------------------------
void CPreferencesDlg::OnButtonChooseColor() 
{
    COLORREF color = RGB(
        (int) (m_kBackgroundColor.r * 255),
        (int) (m_kBackgroundColor.g * 255),
        (int) (m_kBackgroundColor.b * 255));

    CColorDialog dlg(color, CC_ANYCOLOR | CC_FULLOPEN);
    if (dlg.DoModal() == IDOK)
    {
        color = dlg.GetColor();
        m_kBackgroundColor = NiColor(
            (float) GetRValue(color) / 255,
            (float) GetGValue(color) / 255,
            (float) GetBValue(color) / 255);
    }

    m_wndBackgroundColor.SetBackgroundColor(m_kBackgroundColor);
}
//---------------------------------------------------------------------------
void CPreferencesDlg::OnButtonKeyboardShortcuts() 
{
    // Code for changing keyboard shortcuts can go here if needed
}
//---------------------------------------------------------------------------
void CPreferencesDlg::OnCameraControlModeChanged() 
{
    m_bCameraControlModeChanged = true;
}
//---------------------------------------------------------------------------