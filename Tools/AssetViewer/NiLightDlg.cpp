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

// NiLightDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiLightDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiLightDlg dialog
//---------------------------------------------------------------------------
CNiLightDlg::CNiLightDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiLightDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiLightDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiLightDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiLightDlg)
    DDX_Control(pDX, IDC_NILIGHT_SPECULAR_COLOR_RED_EDIT, 
        m_wndSpecularColorRedEdit);
    DDX_Control(pDX, IDC_NILIGHT_SPECULAR_COLOR_GREEN_EDIT, 
        m_wndSpecularColorGreenEdit);
    DDX_Control(pDX, IDC_NILIGHT_SPECULAR_COLOR_BLUE_EDIT, 
        m_wndSpecularColorBlueEdit);
    DDX_Control(pDX, IDC_NILIGHT_DIMMER_EDIT, m_wndDimmerEdit);
    DDX_Control(pDX, IDC_NILIGHT_DIFFUSE_COLOR_RED_EDIT, 
        m_wndDiffuseColorRedEdit);
    DDX_Control(pDX, IDC_NILIGHT_DIFFUSE_COLOR_GREEN_EDIT, 
        m_wndDiffuseColorGreenEdit);
    DDX_Control(pDX, IDC_NILIGHT_DIFFUSE_COLOR_BLUE_EDIT, 
        m_wndDiffuseColorBlueEdit);
    DDX_Control(pDX, IDC_NILIGHT_AMBIENT_COLOR_BLUE_EDIT, 
        m_wndAmbientColorBlueEdit);
    DDX_Control(pDX, IDC_NILIGHT_AMBIENT_COLOR_GREEN_EDIT, 
        m_wndAmbientColorGreenEdit);
    DDX_Control(pDX, IDC_NILIGHT_AMBIENT_COLOR_RED_EDIT, 
        m_wndAmbientColorRedEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiLightDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiLightDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiLightDlg message handlers
//---------------------------------------------------------------------------
bool CNiLightDlg::DoUpdate()
{
    if(!NiIsKindOf(NiLight, m_pkObj))
        return false;

    NiLight* pkLight = (NiLight*)m_pkObj;
    char acString[256];
    
    NiColor kColor = pkLight->GetSpecularColor();
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndSpecularColorRedEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndSpecularColorGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndSpecularColorBlueEdit.SetWindowText(acString);
    m_wndSpecularColor.SetBackgroundColor(kColor);
    
    NiSprintf(acString, 256, "%.4f", pkLight->GetDimmer());
    m_wndDimmerEdit.SetWindowText(acString);

    kColor = pkLight->GetDiffuseColor();
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndDiffuseColorRedEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndDiffuseColorGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndDiffuseColorBlueEdit.SetWindowText(acString);
    m_wndDiffuseColor.SetBackgroundColor(kColor);
    
    kColor = pkLight->GetAmbientColor();
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndAmbientColorBlueEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndAmbientColorGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndAmbientColorRedEdit.SetWindowText(acString);
    m_wndAmbientColor.SetBackgroundColor(kColor);

    return true;
    
}
//---------------------------------------------------------------------------
BOOL CNiLightDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(NiIsKindOf(NiLight, m_pkObj));
    CRect kRect;
    CRect kStartRect;
    CRect kWinRect (0,0,0,0);
    CRect kTemp;

    ClientToScreen( kWinRect );
    
    m_wndDimmerEdit.GetClientRect(kStartRect);
    m_wndDimmerEdit.ClientToScreen(kStartRect);
    m_wndSpecularColorRedEdit.GetClientRect(kRect);
    m_wndSpecularColorRedEdit.ClientToScreen(kRect);

    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndSpecularColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);


    m_wndAmbientColorRedEdit.GetClientRect(kRect);
    m_wndAmbientColorRedEdit.ClientToScreen(kRect);
    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndAmbientColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);

    m_wndDiffuseColorRedEdit.GetClientRect(kRect);
    m_wndDiffuseColorRedEdit.ClientToScreen(kRect);
    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndDiffuseColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);

    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
