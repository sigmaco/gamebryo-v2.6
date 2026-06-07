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
#include "NiPhysXRagdollDialog.h"
#include "NiPhysXRagdollPluginDefines.h"
#include "NiPhysXRagdoll_resource.h"

//---------------------------------------------------------------------------
NiPhysXRagdollDialog::NiPhysXRagdollDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_PHYSXRAGDOLL_OPTIONS, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
NiPhysXRagdollDialog::~NiPhysXRagdollDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void NiPhysXRagdollDialog::InitDialog()
{
    // Here is where the original plug-in info's values are read in and
    // the original state of the UI is set up.
    NiString strSkeletonName =
        m_spPluginInfoInitial->GetValue(PHYSX_RAGDOLL_SKELETON_NAME);
    NiString strRagdollName =
        m_spPluginInfoInitial->GetValue(PHYSX_RAGDOLL_RAGDOLL_NAME);
    NiString strPrefix =
        m_spPluginInfoInitial->GetValue(PHYSX_RAGDOLL_PREFIX);
    NiString strPostfix =
        m_spPluginInfoInitial->GetValue(PHYSX_RAGDOLL_POSTFIX);
    bool bKeepMeshes = m_spPluginInfoInitial->GetBool(PHYSX_RAGDOLL_KEEP);

    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SKELETON_NAME), strSkeletonName);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RAGDOLL_NAME), strRagdollName);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_PREFIX), strPrefix);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_POSTFIX), strPostfix);

    if (bKeepMeshes)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP, BST_UNCHECKED);
    }
}
//---------------------------------------------------------------------------
BOOL NiPhysXRagdollDialog::OnCommand(int iWParamLow, int,
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
    case IDC_OPT_BUTTON_RESETDEFAULTS:
        ResetDefaultDialogOptions();
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void NiPhysXRagdollDialog::ResetDefaultDialogOptions()
{
    // Resets options in dialog to defaults.
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SKELETON_NAME), "SceneNode");
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RAGDOLL_NAME), "ragdoll");
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_PREFIX), "");
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_POSTFIX), "RigidBody");
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP, BST_CHECKED);
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiPhysXRagdollDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiPhysXRagdollDialog::HandleOK()
{
    // Now that the user has accepted the changes that were made, we must set
    // the resulting plug-in info.

    char acSkeletonName[256];
    GetWindowText(
        GetDlgItem(m_hWnd, IDC_EDIT_SKELETON_NAME), acSkeletonName, 256);
    m_spPluginInfoResults->
        SetValue(PHYSX_RAGDOLL_SKELETON_NAME, acSkeletonName);

    char acRagdollName[256];
    GetWindowText(
        GetDlgItem(m_hWnd, IDC_EDIT_RAGDOLL_NAME), acRagdollName, 256);
    m_spPluginInfoResults->
        SetValue(PHYSX_RAGDOLL_RAGDOLL_NAME, acRagdollName);

    char acPrefix[256];
    GetWindowText(
        GetDlgItem(m_hWnd, IDC_EDIT_PREFIX), acPrefix, 256);
    m_spPluginInfoResults->
        SetValue(PHYSX_RAGDOLL_PREFIX, acPrefix);

    char acPostfix[256];
    GetWindowText(
        GetDlgItem(m_hWnd, IDC_EDIT_POSTFIX), acPostfix, 256);
    m_spPluginInfoResults->
        SetValue(PHYSX_RAGDOLL_POSTFIX, acPostfix);

    m_spPluginInfoResults->SetBool(PHYSX_RAGDOLL_KEEP,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_KEEP) == BST_CHECKED);
}
//---------------------------------------------------------------------------
