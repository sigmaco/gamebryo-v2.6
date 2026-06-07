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

// NiVertexColorPropertyDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiVertexColorPropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiVertexColorPropertyDlg dialog
//---------------------------------------------------------------------------
CNiVertexColorPropertyDlg::CNiVertexColorPropertyDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiVertexColorPropertyDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiVertexColorPropertyDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiVertexColorPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiVertexColorPropertyDlg)
    DDX_Control(pDX, IDC_NIVERTEXCOLORPROPERTY_VERTEXMODE_COMBO, 
        m_wndVertexModeCombo);
    DDX_Control(pDX, IDC_NIVERTEXCOLORPROPERTY_LIGHTINGMODE_COMBO, 
        m_wndLightingModeCombo);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiVertexColorPropertyDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiVertexColorPropertyDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiVertexColorPropertyDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiVertexColorPropertyDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    if(!NiIsKindOf(NiVertexColorProperty, m_pkObj))
        return FALSE;
    m_wndVertexModeCombo.AddString("SOURCE_IGNORE");
    m_wndVertexModeCombo.AddString("SOURCE_EMISSIVE");
    m_wndVertexModeCombo.AddString("SOURCE_AMB_DIFF");

    m_wndLightingModeCombo.AddString("LIGHTING_E");
    m_wndLightingModeCombo.AddString("LIGHTING_E_A_D");
    
    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
bool CNiVertexColorPropertyDlg::DoUpdate()
{
    if(!NiIsKindOf(NiVertexColorProperty, m_pkObj))
        return false;

    NiVertexColorProperty* pkProp = (NiVertexColorProperty*)m_pkObj;
    m_wndLightingModeCombo.SetCurSel((int)pkProp->GetLightingMode());
    m_wndVertexModeCombo.SetCurSel((int)pkProp->GetSourceMode());

    return true;
}
//---------------------------------------------------------------------------
