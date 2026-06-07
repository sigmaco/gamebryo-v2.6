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

#include "stdafx.h"
#include "MeshInstancingDialog.h"
#include "MeshInstancing_resource.h"
#include "MeshInstancingDLLDefines.h"

//---------------------------------------------------------------------------
MeshInstancingDialog::MeshInstancingDialog(
    NiModuleRef hInstance, NiWindowRef hWndParent, 
    NiPluginInfo* pkPluginInfo) : 
    NiDialog(IDD_OPT_MESH_INSTANCING_OPTIONS, hInstance, 
        hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();

}
//---------------------------------------------------------------------------
MeshInstancingDialog::~MeshInstancingDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
int MeshInstancingDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void MeshInstancingDialog::InitDialog()
{  
    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_NUM_INSTANCED_OBJECTS), m_spPluginInfoResults
        ->GetValue(OPT_MESH_INSTANCING_NUM_INSTANCED_OBJECTS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_DX9_NUM_INSTANCED_VERTS), m_spPluginInfoResults
        ->GetValue(OPT_MESH_INSTANCING_DX9_NUM_INSTANCED_VERTS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_D3D10_NUM_INSTANCED_VERTS), m_spPluginInfoResults
        ->GetValue(OPT_MESH_INSTANCING_D3D10_NUM_INSTANCED_VERTS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_XENON_NUM_INSTANCED_VERTS), m_spPluginInfoResults
        ->GetValue(OPT_MESH_INSTANCING_XENON_NUM_INSTANCED_VERTS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_PS3_NUM_INSTANCED_VERTS), m_spPluginInfoResults
        ->GetValue(OPT_MESH_INSTANCING_PS3_NUM_INSTANCED_VERTS));
}
//---------------------------------------------------------------------------
BOOL MeshInstancingDialog::OnCommand(int iWParamLow, int,
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
NiPluginInfoPtr MeshInstancingDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void MeshInstancingDialog::HandleOK()
{
    char acBuffer[64];
    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_NUM_INSTANCED_OBJECTS), 
        acBuffer, sizeof(acBuffer));
    int iValue = (int)atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_MESH_INSTANCING_NUM_INSTANCED_OBJECTS,
        iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_DX9_NUM_INSTANCED_VERTS), 
        acBuffer, sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_MESH_INSTANCING_DX9_NUM_INSTANCED_VERTS,
        iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_D3D10_NUM_INSTANCED_VERTS), 
        acBuffer, sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(
        OPT_MESH_INSTANCING_D3D10_NUM_INSTANCED_VERTS, iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_XENON_NUM_INSTANCED_VERTS), 
        acBuffer, sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(
        OPT_MESH_INSTANCING_XENON_NUM_INSTANCED_VERTS, iValue);

    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_PS3_NUM_INSTANCED_VERTS),
        acBuffer, sizeof(acBuffer));
    iValue = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_MESH_INSTANCING_PS3_NUM_INSTANCED_VERTS,
        iValue);
}
//---------------------------------------------------------------------------
