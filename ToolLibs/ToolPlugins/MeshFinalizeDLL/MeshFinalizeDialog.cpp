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
#include "MeshFinalizeDialog.h"
#include "MeshFinalize_resource.h"

//-----------------------------------------------------------------------------------------------
MeshFinalizeDialog::MeshFinalizeDialog(WORD wResourceId, NiModuleRef hInstance, 
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(wResourceId, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();

}
//-----------------------------------------------------------------------------------------------
MeshFinalizeDialog::~MeshFinalizeDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//-----------------------------------------------------------------------------------------------
int MeshFinalizeDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizeDialog::InitDialog()
{
    // If this is the no-option version of the dialog then there's nothing to initialize
    if (m_wResourceId == IDD_OPT_MESHFINALIZE)
    {
        return;
    }

    if (m_spPluginInfoInitial->GetValue(WII_INDEX_STREAM_REMOVAL_KEY) == "0")
        CheckDlgButton(m_hWnd, IDC_INDEX_STREAM_REMOVAL_CHECK, BST_UNCHECKED);
    else
        CheckDlgButton(m_hWnd, IDC_INDEX_STREAM_REMOVAL_CHECK, BST_CHECKED);

    if (m_spPluginInfoInitial->GetValue(WII_CONVERT_SKINNING_STREAMS_KEY) == "0")
        CheckDlgButton(m_hWnd, IDC_CONVERT_SKINNING_STREAMS_CHECK, BST_UNCHECKED);
    else
        CheckDlgButton(m_hWnd, IDC_CONVERT_SKINNING_STREAMS_CHECK, BST_CHECKED);

    if (m_spPluginInfoInitial->GetValue(WII_CREATE_DISPLAY_LIST_STREAM_KEY) == "0")
        CheckDlgButton(m_hWnd, IDC_CREATE_DISPLAY_LIST_STREAMS_CHECK, BST_UNCHECKED);
    else
        CheckDlgButton(m_hWnd, IDC_CREATE_DISPLAY_LIST_STREAMS_CHECK, BST_CHECKED);
}
//-----------------------------------------------------------------------------------------------
BOOL MeshFinalizeDialog::OnCommand(int iWParamLow, int, 
    long)
{
    switch(iWParamLow)
    {
    case IDOK:
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------
NiPluginInfoPtr MeshFinalizeDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//-----------------------------------------------------------------------------------------------
void MeshFinalizeDialog::HandleOK()
{
    // If this is the no-option version of the dialog then there's nothing to initialize
    if (m_wResourceId == IDD_OPT_MESHFINALIZE)
    {
        return;
    }

    m_spPluginInfoResults->SetValue(WII_INDEX_STREAM_REMOVAL_KEY,
        (IsDlgButtonChecked(m_hWnd, IDC_INDEX_STREAM_REMOVAL_CHECK) == BST_CHECKED) ? 
        "1" : "0");

    m_spPluginInfoResults->SetValue(WII_CONVERT_SKINNING_STREAMS_KEY,
        (IsDlgButtonChecked(m_hWnd, IDC_CONVERT_SKINNING_STREAMS_CHECK) == BST_CHECKED) ? 
        "1" : "0");

    m_spPluginInfoResults->SetValue(WII_CREATE_DISPLAY_LIST_STREAM_KEY,
        (IsDlgButtonChecked(m_hWnd, IDC_CREATE_DISPLAY_LIST_STREAMS_CHECK) == BST_CHECKED) ? 
        "1" : "0");
}
//-----------------------------------------------------------------------------------------------
