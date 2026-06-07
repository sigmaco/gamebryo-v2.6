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
#include "NiStripifyGeometryDialog.h"
#include "NiStripifyGeometry_resource.h"
#include "NiStripifyDLLDefines.h"

//---------------------------------------------------------------------------
NiStripifyGeometryDialog::NiStripifyGeometryDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_OPT_STRIPIFY_OPTIONS, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
NiStripifyGeometryDialog::~NiStripifyGeometryDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
int NiStripifyGeometryDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void NiStripifyGeometryDialog::InitDialog()
{
    NiUInt32 uiCacheSize = 0;

    if (m_spPluginInfoInitial->GetBool(STRIP_PC_STRIPIFY_VALUE))
    {
        CheckDlgButton(m_hWnd, IDC_WIN32_STRIPIFY_CHECK, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_WIN32_STRIPIFY_CHECK, BST_UNCHECKED);
    }

    uiCacheSize = m_spPluginInfoInitial->GetInt(STRIP_PC_CACHE_VALUE);
    NiString strPCCacheSize;
    strPCCacheSize.Format("%d", uiCacheSize);
    SetWindowText(GetDlgItem(m_hWnd, IDC_WIN32_CACHE_EDIT), strPCCacheSize);

    if (m_spPluginInfoInitial->GetBool(STRIP_PS3_STRIPIFY_VALUE))
    {
        CheckDlgButton(m_hWnd, IDC_PS3_STRIPIFY_CHECK, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_PS3_STRIPIFY_CHECK, BST_UNCHECKED);
    }

    uiCacheSize = m_spPluginInfoInitial->GetInt(STRIP_PS3_CACHE_VALUE);
    NiString strPS3CacheSize;
    strPS3CacheSize.Format("%d", uiCacheSize);
    SetWindowText(GetDlgItem(m_hWnd, IDC_PS3_CACHE_EDIT), strPS3CacheSize);

    if (m_spPluginInfoInitial->GetBool(STRIP_WII_STRIPIFY_VALUE))
    {
        CheckDlgButton(m_hWnd, IDC_WII_STRIPIFY_CHECK, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_WII_STRIPIFY_CHECK, BST_UNCHECKED);
    }

    uiCacheSize = m_spPluginInfoInitial->GetInt(STRIP_WII_CACHE_VALUE);
    NiString strWiiCacheSize;
    strWiiCacheSize.Format("%d", uiCacheSize);
    SetWindowText(GetDlgItem(m_hWnd, IDC_WII_CACHE_EDIT), strWiiCacheSize);

    if (m_spPluginInfoInitial->GetBool(STRIP_XENON_STRIPIFY_VALUE))
    {
        CheckDlgButton(m_hWnd, IDC_XENON_STRIPIFY_CHECK, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_XENON_STRIPIFY_CHECK, BST_UNCHECKED);
    }

    uiCacheSize = m_spPluginInfoInitial->GetInt(STRIP_XENON_CACHE_VALUE);
    NiString strXenonCacheSize;
    strXenonCacheSize.Format("%d", uiCacheSize);
    SetWindowText(GetDlgItem(m_hWnd, IDC_XENON_CACHE_EDIT), strXenonCacheSize);
}
//---------------------------------------------------------------------------
BOOL NiStripifyGeometryDialog::OnCommand(int iWParamLow, int, 
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
NiPluginInfoPtr NiStripifyGeometryDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiStripifyGeometryDialog::HandleOK()
{
    char acCacheSize[256];
    NiUInt32 uiCacheSize = 0;

    m_spPluginInfoResults->SetBool(STRIP_PC_STRIPIFY_VALUE,
        IsDlgButtonChecked(m_hWnd, IDC_WIN32_STRIPIFY_CHECK) == BST_CHECKED);

    GetWindowText(GetDlgItem(m_hWnd, IDC_WIN32_CACHE_EDIT), acCacheSize, 256);
    uiCacheSize = (NiUInt32)atoi(acCacheSize);
    m_spPluginInfoResults->SetInt(STRIP_PC_CACHE_VALUE, uiCacheSize);
        
    m_spPluginInfoResults->SetBool(STRIP_PS3_STRIPIFY_VALUE,
        IsDlgButtonChecked(m_hWnd, IDC_PS3_STRIPIFY_CHECK) == BST_CHECKED);

    GetWindowText(GetDlgItem(m_hWnd, IDC_PS3_CACHE_EDIT), acCacheSize, 256);
    uiCacheSize = (NiUInt32)atoi(acCacheSize);
    m_spPluginInfoResults->SetInt(STRIP_PS3_CACHE_VALUE, uiCacheSize);

    m_spPluginInfoResults->SetBool(STRIP_WII_STRIPIFY_VALUE,
        IsDlgButtonChecked(m_hWnd, IDC_WII_STRIPIFY_CHECK) == BST_CHECKED);

    GetWindowText(GetDlgItem(m_hWnd, IDC_WII_CACHE_EDIT), acCacheSize, 256);
    uiCacheSize = (NiUInt32)atoi(acCacheSize);
    m_spPluginInfoResults->SetInt(STRIP_WII_CACHE_VALUE, uiCacheSize);

    m_spPluginInfoResults->SetBool(STRIP_XENON_STRIPIFY_VALUE,
        IsDlgButtonChecked(m_hWnd, IDC_XENON_STRIPIFY_CHECK) == BST_CHECKED);

    GetWindowText(GetDlgItem(m_hWnd, IDC_XENON_CACHE_EDIT), acCacheSize, 256);
    uiCacheSize = (NiUInt32)atoi(acCacheSize);
    m_spPluginInfoResults->SetInt(STRIP_XENON_CACHE_VALUE, uiCacheSize);
}
//---------------------------------------------------------------------------
