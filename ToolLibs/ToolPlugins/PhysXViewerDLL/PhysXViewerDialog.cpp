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
#include "PhysXViewerDialog.h"
#include "resource.h"
#include "PhysXViewerDefines.h"

//---------------------------------------------------------------------------
PhysXViewerDialog::PhysXViewerDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) : NiDialog(
    IDD_PHYSXVIEWER_OPTIONS, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
PhysXViewerDialog::~PhysXViewerDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void PhysXViewerDialog::InitDialog()
{
    CheckDlgButton(m_hWnd, IDC_CHECK_DEBUGOUTPUT,
        m_spPluginInfoResults->GetBool(OPT_DEBUGOUTPUT));

    bool bGroundPlane = m_spPluginInfoResults->GetBool(OPT_GROUNDPLANE);
    CheckDlgButton(m_hWnd, IDC_CHECK_GROUNDPLANE, bGroundPlane);
    
    EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_GROUNDHEIGHT), bGroundPlane);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GROUNDHEIGHT),
        m_spPluginInfoResults->GetValue(OPT_GROUNDHEIGHT));

    bool bFullScreen = m_spPluginInfoResults->GetBool(OPT_FULLSCREEN);
    CheckDlgButton(m_hWnd, IDC_RADIO_FULLSCREEN, bFullScreen);
    CheckDlgButton(m_hWnd, IDC_RADIO_WINDOWED, !bFullScreen);

    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RESOLUTIONWIDTH),
        m_spPluginInfoResults->GetValue(OPT_RESOLUTIONWIDTH));

    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RESOLUTIONHEIGHT),
        m_spPluginInfoResults->GetValue(OPT_RESOLUTIONHEIGHT));

    unsigned int uiIndex;
    uiIndex = m_spPluginInfoResults->GetParameterIndex(OPT_GRAVITY_X);
    if (uiIndex == NIPT_INVALID_INDEX)
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_X), "0.0");
    }
    else
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_X),
            m_spPluginInfoResults->GetValue(OPT_GRAVITY_X));
    }
    uiIndex = m_spPluginInfoResults->GetParameterIndex(OPT_GRAVITY_Y);
    if (uiIndex == NIPT_INVALID_INDEX)
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_Y), "0.0");
    }
    else
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_Y),
            m_spPluginInfoResults->GetValue(OPT_GRAVITY_Y));
    }
    uiIndex = m_spPluginInfoResults->GetParameterIndex(OPT_GRAVITY_Z);
    if (uiIndex == NIPT_INVALID_INDEX)
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_Z), "-9.8");
    }
    else
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_Z),
            m_spPluginInfoResults->GetValue(OPT_GRAVITY_Z));
    }

    NIASSERT(NiIsKindOf(NiViewerPluginInfo, m_spPluginInfoResults));
    NiViewerPluginInfo* pkViewerInfo = NiSmartPointerCast(NiViewerPluginInfo,
        m_spPluginInfoResults);
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
BOOL PhysXViewerDialog::OnCommand(int iWParamLow,
    int, long)
{
    switch(iWParamLow)
    {
    case IDOK:
        if (HandleOK())
        {
            EndDialog(m_hWnd, IDOK);
        }
        return TRUE;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    case IDC_CHECK_GROUNDPLANE:
        EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_GROUNDHEIGHT),
            IsDlgButtonChecked(m_hWnd, IDC_CHECK_GROUNDPLANE) == BST_CHECKED);
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr PhysXViewerDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
bool PhysXViewerDialog::HandleOK()
{
    const unsigned int uiBufferSize = 100;
    char acBuffer[uiBufferSize];

    m_spPluginInfoResults->SetBool(OPT_DEBUGOUTPUT, IsDlgButtonChecked(m_hWnd,
        IDC_CHECK_DEBUGOUTPUT) == BST_CHECKED);

    m_spPluginInfoResults->SetBool(OPT_GROUNDPLANE, IsDlgButtonChecked(m_hWnd,
        IDC_CHECK_GROUNDPLANE) == BST_CHECKED);

    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GROUNDHEIGHT), &acBuffer[0],
        uiBufferSize);
    float fGroundHeight = (float) atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_GROUNDHEIGHT, fGroundHeight);

    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_X), &acBuffer[0],
        uiBufferSize);
    float fGravityX = (float) atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_GRAVITY_X, fGravityX);

    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_Y), &acBuffer[0],
        uiBufferSize);
    float fGravityY = (float) atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_GRAVITY_Y, fGravityY);

    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_GRAVITY_Z), &acBuffer[0],
        uiBufferSize);
    float fGravityZ = (float) atof(acBuffer);
    m_spPluginInfoResults->SetFloat(OPT_GRAVITY_Z, fGravityZ);

    m_spPluginInfoResults->SetBool(OPT_FULLSCREEN, IsDlgButtonChecked(m_hWnd,
        IDC_RADIO_FULLSCREEN) == BST_CHECKED);

    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RESOLUTIONWIDTH), &acBuffer[0],
        uiBufferSize);
    int iResolutionWidth = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_RESOLUTIONWIDTH, iResolutionWidth);

    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_RESOLUTIONHEIGHT), &acBuffer[0],
        uiBufferSize);
    int iResolutionHeight = atoi(acBuffer);
    m_spPluginInfoResults->SetInt(OPT_RESOLUTIONHEIGHT, iResolutionHeight);

    if (iResolutionWidth <= 0 || iResolutionHeight <= 0)
    {
        MessageBox(m_hWnd, "The resolution width and height must be greater "
            "than zero.", "Invalid Resolution Values", MB_OK | MB_ICONERROR);
        return false;
    }

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

    return true;
}
//---------------------------------------------------------------------------
