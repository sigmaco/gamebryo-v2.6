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

#include "StdAfx.h"
#include "ApplyMeshProfileDialog.h"
#include "ApplyMeshProfilePluginDefines.h"
#include "ApplyMeshProfile_resource.h"

//---------------------------------------------------------------------------
ApplyMeshProfileDialog::ApplyMeshProfileDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_APPLYMESHPROFILE_DIALOG, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
ApplyMeshProfileDialog::~ApplyMeshProfileDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void ApplyMeshProfileDialog::InitDialog()
{
    // Here is where the original plug-in info's values are read in and
    // the original state of the UI is set up.
    NiString strProfileName =
        m_spPluginInfoInitial->GetValue(APPLY_MESH_PROFILE_NAME);

    SetWindowText(GetDlgItem(m_hWnd, IDC_PROFILE_NAME_EDIT), strProfileName);
}
//---------------------------------------------------------------------------
BOOL ApplyMeshProfileDialog::OnCommand(int iWParamLow, int,
    long)
{
    switch (iWParamLow)
    {
    case IDOK:
        // Handle the OK button.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        // Handle the cancel button.
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr ApplyMeshProfileDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void ApplyMeshProfileDialog::HandleOK()
{
    // Now that the user has accepted the changes that were made, we must set
    // the resulting plug-in info.

    char acProfileName[256];
    GetWindowText(
        GetDlgItem(m_hWnd, IDC_PROFILE_NAME_EDIT), acProfileName, 256);
    m_spPluginInfoResults->
        SetValue(APPLY_MESH_PROFILE_NAME, acProfileName);
}
//---------------------------------------------------------------------------
