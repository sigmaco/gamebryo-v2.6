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
#include "MultiNIFExportDialog.h"
#include "MultiNIFExportPluginDefines.h"
#include "resource.h"

//---------------------------------------------------------------------------
MultiNIFExportDialog::MultiNIFExportDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_EXPORT_DIALOG, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
MultiNIFExportDialog::~MultiNIFExportDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void MultiNIFExportDialog::InitDialog()
{

    if (m_spPluginInfoInitial->GetBool(
        MULTI_NIF_EXPROT_USE_OBJECT_NAME_AS_FILE_NAME))
    {
        CheckDlgButton(m_hWnd, IDC_USE_OBJECT_NAME_AS_FILE, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_USE_OBJECT_NAME_AS_FILE, BST_UNCHECKED);
    }



    if (m_spPluginInfoInitial->GetBool(MULTI_NIF_EXPORT_DELETE_AFTER_EXPORT))
    {
        CheckDlgButton(m_hWnd, IDC_DELETE_AFTER_EXPORT, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_DELETE_AFTER_EXPORT, BST_UNCHECKED);
    }


    if (m_spPluginInfoInitial->GetBool(
        MULTI_NIF_EXPORT_REPAIR_EMITTER_WITHOUT_PARTICLES))
    {
        CheckDlgButton(m_hWnd, IDC_REPAIR_PARTICLE_SYSTEMS, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_REPAIR_PARTICLE_SYSTEMS, BST_UNCHECKED);
    }


    SendMessage(GetDlgItem(m_hWnd, IDC_TRANSLATE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("World Translate"));

    SendMessage(GetDlgItem(m_hWnd, IDC_TRANSLATE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("Local Translate"));

    SendMessage(GetDlgItem(m_hWnd, IDC_TRANSLATE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("No Translate"));

    SendMessage(GetDlgItem(m_hWnd, IDC_TRANSLATE_TYPE), 
        CB_SETCURSEL, m_spPluginInfoInitial->GetInt(
        MULTI_NIF_EXPORT_TRANSLATE_TYPE), 0);



    SendMessage(GetDlgItem(m_hWnd, IDC_ROTATE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("World Rotation"));

    SendMessage(GetDlgItem(m_hWnd, IDC_ROTATE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("Local Rotation"));

    SendMessage(GetDlgItem(m_hWnd, IDC_ROTATE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("No Rotation"));

    SendMessage(GetDlgItem(m_hWnd, IDC_ROTATE_TYPE), 
        CB_SETCURSEL, m_spPluginInfoInitial->GetInt(
        MULTI_NIF_EXPORT_ROTATE_TYPE), 0);



    SendMessage(GetDlgItem(m_hWnd, IDC_SCALE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("World Scale"));

    SendMessage(GetDlgItem(m_hWnd, IDC_SCALE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("Local Scale"));

    SendMessage(GetDlgItem(m_hWnd, IDC_SCALE_TYPE), 
        CB_ADDSTRING, 0, PtrToLong("No Scale"));

    SendMessage(GetDlgItem(m_hWnd, IDC_SCALE_TYPE), 
        CB_SETCURSEL, m_spPluginInfoInitial->GetInt(
        MULTI_NIF_EXPORT_SCALE_TYPE), 0);

}
//---------------------------------------------------------------------------
BOOL MultiNIFExportDialog::OnCommand(int iWParamLow, int,
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
    // If you have any logic for updating any of the GUI, add the handlers
    // here. Return true for anything that you handle.
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr MultiNIFExportDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void MultiNIFExportDialog::HandleOK()
{
    // Now that the user has accepted the changes that were made, we must set
    // the resulting plug-in info.

    m_spPluginInfoResults->SetBool(
        MULTI_NIF_EXPROT_USE_OBJECT_NAME_AS_FILE_NAME, 
        IsDlgButtonChecked(
        m_hWnd, IDC_USE_OBJECT_NAME_AS_FILE) == BST_CHECKED);


    m_spPluginInfoResults->SetBool(MULTI_NIF_EXPORT_DELETE_AFTER_EXPORT, 
        IsDlgButtonChecked(m_hWnd, IDC_DELETE_AFTER_EXPORT) == BST_CHECKED);


    m_spPluginInfoResults->SetBool(
        MULTI_NIF_EXPORT_REPAIR_EMITTER_WITHOUT_PARTICLES, 
        IsDlgButtonChecked(
        m_hWnd, IDC_REPAIR_PARTICLE_SYSTEMS) == BST_CHECKED);


    m_spPluginInfoResults->SetInt(MULTI_NIF_EXPORT_TRANSLATE_TYPE,
        (int)SendMessage(GetDlgItem(m_hWnd, IDC_TRANSLATE_TYPE), 
            CB_GETCURSEL, 0, 0));

    m_spPluginInfoResults->SetInt(MULTI_NIF_EXPORT_ROTATE_TYPE,
        (int)SendMessage(GetDlgItem(m_hWnd, IDC_ROTATE_TYPE), 
            CB_GETCURSEL, 0, 0));

    m_spPluginInfoResults->SetInt(MULTI_NIF_EXPORT_SCALE_TYPE,
        (int)SendMessage(GetDlgItem(m_hWnd, IDC_SCALE_TYPE), 
            CB_GETCURSEL, 0, 0));

}
//---------------------------------------------------------------------------
