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

#include <StdAfx.h>
#include "NiCreateABVDialog.h"
#include "NiCreateABV_resource.h"

//---------------------------------------------------------------------------
NiCreateABVDialog::NiCreateABVDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_OPT_CREATE_ABV, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();

}

//---------------------------------------------------------------------------
NiCreateABVDialog::~NiCreateABVDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
        
//---------------------------------------------------------------------------
int NiCreateABVDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void NiCreateABVDialog::InitDialog()
{
    // Load options.
    NiString strValue;
    NiString strTrue = NiString::FromBool(true);
    
    strValue = m_spPluginInfoResults->GetValue("RemoveProxyGeometry");
    CheckDlgButton(m_hWnd, IDC_CHECK_REMOVE_PROXY_MESH,
        (strValue == strTrue));

    strValue = m_spPluginInfoResults->GetValue("CreateABV");
    CheckDlgButton(m_hWnd, IDC_CHECK_CREATEABV,
        (strValue == strTrue));

    strValue = m_spPluginInfoInitial->GetValue("CreateWireframeABVProxy");
    CheckDlgButton(m_hWnd, IDC_CHECK_CREATE_WIREFRAME_ABVPROXY,
        (strValue == strTrue));
    
    strValue = m_spPluginInfoInitial->GetValue("CreateWireframeABV");
    CheckDlgButton(m_hWnd, IDC_CHECK_CREATE_WIREFRAME_ABV,
        (strValue == strTrue));

    strValue = m_spPluginInfoInitial->GetValue("CreateWireframeSelection");
    CheckDlgButton(m_hWnd, IDC_CHECK_CREATE_WIREFRAME_SELECTION,
        (strValue == strTrue));
}
//---------------------------------------------------------------------------
BOOL NiCreateABVDialog::OnCommand(int iWParamLow, int, 
                                         long)
{
    switch(iWParamLow)
    {
    case IDOK:
    {
            // Save options.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    }
    case IDCANCEL:
    {
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiCreateABVDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiCreateABVDialog::HandleOK()
{
    m_spPluginInfoResults->SetValue("RemoveProxyGeometry", 
        NiString::FromBool(IsDlgButtonChecked(m_hWnd, 
        IDC_CHECK_REMOVE_PROXY_MESH) == 1));

    m_spPluginInfoResults->SetValue("CreateABV", 
        NiString::FromBool(IsDlgButtonChecked(m_hWnd, 
        IDC_CHECK_CREATEABV) == 1));

    m_spPluginInfoResults->SetValue("CreateWireframeABVProxy",
        NiString::FromBool(IsDlgButtonChecked(m_hWnd, 
        IDC_CHECK_CREATE_WIREFRAME_ABVPROXY) == BST_CHECKED));

    m_spPluginInfoResults->SetValue("CreateWireframeABV",
        NiString::FromBool(IsDlgButtonChecked(m_hWnd, 
        IDC_CHECK_CREATE_WIREFRAME_ABV) == BST_CHECKED));

    m_spPluginInfoResults->SetValue("CreateWireframeSelection",
        NiString::FromBool(IsDlgButtonChecked(m_hWnd, 
        IDC_CHECK_CREATE_WIREFRAME_SELECTION) == BST_CHECKED));
}
//---------------------------------------------------------------------------
