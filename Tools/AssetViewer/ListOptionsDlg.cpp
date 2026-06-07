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

// ListOptionsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ListOptionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CListOptionsDlg dialog
//---------------------------------------------------------------------------
CListOptionsDlg::CListOptionsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CListOptionsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CListOptionsDlg)
    m_bCameras = FALSE;
    m_bGeometry = FALSE;
    m_bLights = FALSE;
    m_bNodes = FALSE;
    m_bDynamicEffects = FALSE;
    m_bProperties = FALSE;
    m_bTimeControllers = FALSE;
    m_strCustom = _T("");
    m_bHidden = FALSE;
    m_bVisible = FALSE;
    //}}AFX_DATA_INIT
    m_bCustom = FALSE;
}
//---------------------------------------------------------------------------
void CListOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CListOptionsDlg)
    DDX_Check(pDX, IDC_CHECK_CAMERAS, m_bCameras);
    DDX_Check(pDX, IDC_CHECK_GEOMETRY, m_bGeometry);
    DDX_Check(pDX, IDC_CHECK_LIGHTS, m_bLights);
    DDX_Check(pDX, IDC_CHECK_NODES, m_bNodes);
    DDX_Check(pDX, IDC_CHECK_DYNAMIC_EFFECTS, m_bDynamicEffects);
    DDX_Check(pDX, IDC_CHECK_PROPERTIES, m_bProperties);
    DDX_Check(pDX, IDC_CHECK_TIME_CONTROLLERS, m_bTimeControllers);
    DDX_Text(pDX, IDC_EDIT_CUSTOM, m_strCustom);
    DDX_Check(pDX, IDC_CHECK_HIDDEN, m_bHidden);
    DDX_Check(pDX, IDC_CHECK_VISIBLE, m_bVisible);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CListOptionsDlg, CDialog)
    //{{AFX_MSG_MAP(CListOptionsDlg)
    ON_BN_CLICKED(IDC_RADIO_STANDARD, OnRadioButtonClicked)
    ON_BN_CLICKED(IDC_RADIO_CUSTOM, OnRadioButtonClicked)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CListOptionsDlg message handlers
//---------------------------------------------------------------------------
BOOL CListOptionsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    m_rbStandard.Attach(GetDlgItem(IDC_RADIO_STANDARD)->m_hWnd);
    m_rbCustom.Attach(GetDlgItem(IDC_RADIO_CUSTOM)->m_hWnd);
    if (m_bCustom)
    {
        m_rbStandard.SetCheck(BST_UNCHECKED);
        m_rbCustom.SetCheck(BST_CHECKED);
    }
    else
    {
        m_rbStandard.SetCheck(BST_CHECKED);
        m_rbCustom.SetCheck(BST_UNCHECKED);
    }
    OnRadioButtonClicked();

    return TRUE;
}
//---------------------------------------------------------------------------
void CListOptionsDlg::PostNcDestroy() 
{
    m_rbStandard.Detach();
    m_rbCustom.Detach();

    CDialog::PostNcDestroy();
}
//---------------------------------------------------------------------------
void CListOptionsDlg::OnOK() 
{
    if (m_rbCustom.GetCheck() == BST_CHECKED)
    {
        m_bCustom = TRUE;
    }
    else
    {
        m_bCustom = FALSE;
    }

    CDialog::OnOK();
}
//---------------------------------------------------------------------------
void CListOptionsDlg::OnRadioButtonClicked()
{
    if (m_rbCustom.GetCheck() == BST_CHECKED)
    {
        GetDlgItem(IDC_CHECK_NODES)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_LIGHTS)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_CAMERAS)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_GEOMETRY)->EnableWindow(FALSE);
        GetDlgItem(IDC_EDIT_CUSTOM)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_CHECK_NODES)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_LIGHTS)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_CAMERAS)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_GEOMETRY)->EnableWindow(TRUE);
        GetDlgItem(IDC_EDIT_CUSTOM)->EnableWindow(FALSE);
    }
}
//---------------------------------------------------------------------------
