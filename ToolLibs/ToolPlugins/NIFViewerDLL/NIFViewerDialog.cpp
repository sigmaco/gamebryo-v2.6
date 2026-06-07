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
#include "NIFViewerDialog.h"
#include "resource.h"

//---------------------------------------------------------------------------
NIFViewerDialog::NIFViewerDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_DIALOG_NIFVIEWER, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
void NIFViewerDialog::InitDialog()
{
    NIASSERT(NiIsKindOf(NiViewerPluginInfo, m_spPluginInfoInitial));
    NiViewerPluginInfo* pkViewerInfo = NiSmartPointerCast(NiViewerPluginInfo,
        m_spPluginInfoInitial);
    if (pkViewerInfo->GetRenderer() == NiSystemDesc::RENDERER_D3D10)
    {
        CheckDlgButton(m_hWnd, IDC_RADIO_DX9, BST_UNCHECKED);
        CheckDlgButton(m_hWnd, IDC_RADIO_D3D10, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_RADIO_DX9, BST_CHECKED);
        CheckDlgButton(m_hWnd, IDC_RADIO_D3D10, BST_UNCHECKED);
    }
}
//---------------------------------------------------------------------------
BOOL NIFViewerDialog::OnCommand(int iWParamLow, int,
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
NiPluginInfoPtr NIFViewerDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NIFViewerDialog::HandleOK()
{
    // Now that the user has accepted the changes that were made, we must set
    // the resulting plug-in info.

    NIASSERT(NiIsKindOf(NiViewerPluginInfo, m_spPluginInfoResults));
    NiViewerPluginInfo* pkViewerInfo = NiSmartPointerCast(NiViewerPluginInfo,
        m_spPluginInfoResults);
    if (IsDlgButtonChecked(m_hWnd, IDC_RADIO_D3D10) == BST_CHECKED)
    {
        pkViewerInfo->SetRenderer(NiSystemDesc::RENDERER_D3D10);
    }
    else
    {
        pkViewerInfo->SetRenderer(NiSystemDesc::RENDERER_DX9);
    }
}
//---------------------------------------------------------------------------
