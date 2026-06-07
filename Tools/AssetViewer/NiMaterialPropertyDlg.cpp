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

// NiMaterialPropertyDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiMaterialPropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiMaterialPropertyDlg dialog
//---------------------------------------------------------------------------
CNiMaterialPropertyDlg::CNiMaterialPropertyDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiMaterialPropertyDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiMaterialPropertyDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiMaterialPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiMaterialPropertyDlg)
    DDX_Control(pDX, IDC_NIMATPROP_SPECULAR_RED_EDIT, m_wndSpecularRedEdit);
    DDX_Control(pDX, IDC_NIMATPROP_SPECULAR_GREEN_EDIT,
        m_wndSpecularGreenEdit);
    DDX_Control(pDX, IDC_NIMATPROP_SPECULAR_BLUE_EDIT,
        m_wndSpecularBlueEdit);
    DDX_Control(pDX, IDC_NIMATPROP_SHININESS_EDIT, m_wndShineEdit);
    DDX_Control(pDX, IDC_NIMATPROP_EMISSIVE_RED_EDIT, m_wndEmissiveRedEdit);
    DDX_Control(pDX, IDC_NIMATPROP_EMISSIVE_GREEN_EDIT, 
        m_wndEmissiveGreenEdit);
    DDX_Control(pDX, IDC_NIMATPROP_EMISSIVE_BLUE_EDIT, 
        m_wndEmissiveBlueEdit);
    DDX_Control(pDX, IDC_NIMATPROP_DIFFUSE_GREEN_EDIT, 
        m_wndDiffuseGreenEdit);
    DDX_Control(pDX, IDC_NIMATPROP_DIFFUSE_RED_EDIT, m_wndDiffuseRedEdit);
    DDX_Control(pDX, IDC_NIMATPROP_DIFFUSE_BLUE_EDIT, m_wndDiffuseBlueEdit);
    DDX_Control(pDX, IDC_NIMATPROP_AMBIENT_RED_EDIT, m_wndAmbientRedEdit);
    DDX_Control(pDX, IDC_NIMATPROP_AMBIENT_GREEN_EDIT, 
        m_wndAmbientGreenEdit);
    DDX_Control(pDX, IDC_NIMATPROP_AMBIENT_BLUE_EDIT, m_wndAmbientBlueEdit);
    DDX_Control(pDX, IDC_NIMATPROP_ALPHA_EDIT, m_wndAlphaEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiMaterialPropertyDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiMaterialPropertyDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiMaterialPropertyDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiMaterialPropertyDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    if(!NiIsKindOf(NiMaterialProperty, m_pkObj))
        return FALSE;

    CRect kRect;
    CRect kStartRect;
    CRect kWinRect (0,0,0,0);
    CRect kTemp;

    ClientToScreen( kWinRect );
    
    m_wndAlphaEdit.GetClientRect(kStartRect);
    m_wndAlphaEdit.ClientToScreen(kStartRect);
    m_wndSpecularRedEdit.GetClientRect(kRect);
    m_wndSpecularRedEdit.ClientToScreen(kRect);

    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndSpecularColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);


    m_wndAmbientRedEdit.GetClientRect(kRect);
    m_wndAmbientRedEdit.ClientToScreen(kRect);
    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndAmbientColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);

    m_wndDiffuseRedEdit.GetClientRect(kRect);
    m_wndDiffuseRedEdit.ClientToScreen(kRect);
    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndDiffuseColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);

    m_wndEmissiveRedEdit.GetClientRect(kRect);
    m_wndEmissiveRedEdit.ClientToScreen(kRect);
    kTemp.top = kRect.top - kWinRect.top;
    kTemp.bottom = kRect.bottom - kWinRect.bottom;
    kTemp.left = kStartRect.left - kWinRect.left;
    kTemp.right = kStartRect.right - kWinRect.right;
    m_wndEmissiveColor.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_CENTER, 
        kTemp, this);

    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
bool CNiMaterialPropertyDlg::DoUpdate()
{
    if(!NiIsKindOf(NiMaterialProperty, m_pkObj))
        return false;

    NiMaterialProperty* pkProp = (NiMaterialProperty*)m_pkObj;
    char acString[256];
    
    NiColor kColor = pkProp->GetSpecularColor();
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndSpecularRedEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndSpecularGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndSpecularBlueEdit.SetWindowText(acString);
    m_wndSpecularColor.SetBackgroundColor(kColor);
    
    NiSprintf(acString, 256, "%.4f", pkProp->GetAlpha());
    m_wndAlphaEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", pkProp->GetShineness());
    m_wndShineEdit.SetWindowText(acString);

    kColor = pkProp->GetDiffuseColor();
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndDiffuseRedEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndDiffuseGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndDiffuseBlueEdit.SetWindowText(acString);
    m_wndDiffuseColor.SetBackgroundColor(kColor);
    
    kColor = pkProp->GetAmbientColor();
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndAmbientBlueEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndAmbientGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndAmbientRedEdit.SetWindowText(acString);
    m_wndAmbientColor.SetBackgroundColor(kColor);

    kColor = pkProp->GetEmittance();
    NiSprintf(acString, 256, "%.4f", kColor.b);
    m_wndEmissiveBlueEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.g);
    m_wndEmissiveGreenEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kColor.r);
    m_wndEmissiveRedEdit.SetWindowText(acString);
    m_wndEmissiveColor.SetBackgroundColor(kColor);

    return true;
}
//---------------------------------------------------------------------------
