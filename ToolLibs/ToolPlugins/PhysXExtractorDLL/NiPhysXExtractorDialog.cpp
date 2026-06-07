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
#include "NiPhysXExtractorDialog.h"
#include "NiPhysXExtractorPluginDefines.h"
#include "NiPhysXExtractor_resource.h"

//---------------------------------------------------------------------------
NiPhysXExtractorDialog::NiPhysXExtractorDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_PHYSXEXTRACTOR_OPTIONS, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
NiPhysXExtractorDialog::~NiPhysXExtractorDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void NiPhysXExtractorDialog::InitDialog()
{
    unsigned int uiIndex;
    char pcVal[256];

    // Here is where the original plug-in info's values are read in and
    // the original state of the UI is set up.
    
    NiString strSceneName =
        m_spPluginInfoInitial->GetValue(PHYSX_EXTRACTOR_SCENE_NAME);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SCENE_NAME), strSceneName);
        
    float fScale = m_spPluginInfoInitial->GetFloat(PHYSX_EXTRACTOR_SCALE);
    NiSprintf(pcVal, 256, "%0.6g", fScale);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SCALE), pcVal);

    bool bConvertPSys =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_CONVERTPSYS);
    if (bConvertPSys)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_CONVERT_PSYS, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_CONVERT_PSYS, BST_UNCHECKED);
    }

    bool bFluidScene =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_SCENE_FLUID);
    if (bFluidScene)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_FLUID, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_FLUID, BST_UNCHECKED);
    }

    bool bPSysInPhysXSpace =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_PSYS_SAME_SPACE);
    if (bPSysInPhysXSpace)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_PSYS_SPACE, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_PSYS_SPACE, BST_UNCHECKED);
    }

    bool bPSysUseRotate =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_PSYS_USE_ROT);
    if (bPSysUseRotate)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_PSYS_ROT, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_PSYS_ROT, BST_UNCHECKED);
    }

    float fConvexSkin =
        m_spPluginInfoInitial->GetFloat(PHYSX_EXTRACTOR_CONVEX_SKIN);
    NiString strConvexSkin = NiString::FromFloat(fConvexSkin);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_CONVEX_SKIN), strConvexSkin);

    bool bCookForHardware =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_HARDWARE_COOK);
    if (bCookForHardware)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_HARDWARE_COOK, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_HARDWARE_COOK, BST_UNCHECKED);
    }

    uiIndex =
        m_spPluginInfoInitial->GetParameterIndex(PHYSX_EXTRACTOR_INFLATE);
    if (uiIndex == NIPT_INVALID_INDEX)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_INFLATE, BST_UNCHECKED);
    }
    else
    {
        bool bInflate =
            m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_INFLATE);
        if (bInflate)
        {
            CheckDlgButton(m_hWnd, IDC_OPT_CHECK_INFLATE, BST_CHECKED);
        }
        else
        {
            CheckDlgButton(m_hWnd, IDC_OPT_CHECK_INFLATE, BST_UNCHECKED);
        }
    }
    
    uiIndex =
        m_spPluginInfoInitial->GetParameterIndex(PHYSX_EXTRACTOR_UNCOMPRESSED);
    if (uiIndex == NIPT_INVALID_INDEX)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_UNCOMPRESSEDNORMS, BST_UNCHECKED);
    }
    else
    {
        bool bInflate =
            m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_UNCOMPRESSED);
        if (bInflate)
        {
            CheckDlgButton(m_hWnd, IDC_OPT_CHECK_UNCOMPRESSEDNORMS,
                BST_CHECKED);
        }
        else
        {
            CheckDlgButton(m_hWnd, IDC_OPT_CHECK_UNCOMPRESSEDNORMS,
                BST_UNCHECKED);
        }
    }
    
    bool bKeepOnLoad =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_KEEP_ON_LOAD);
    if (bKeepOnLoad)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_MESHES, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_MESHES, BST_UNCHECKED);
    }

    int iMaterialBase =
        m_spPluginInfoInitial->GetInt(PHYSX_EXTRACTOR_MATERIAL_BASE);
    NiSprintf(pcVal, 256, "%d", iMaterialBase);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_MATERIAL_BASE), pcVal);

    bool bSequenceStates =
        m_spPluginInfoInitial->GetBool(PHYSX_EXTRACTOR_SEQUENCE_STATES);
    if (bSequenceStates)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SEQUENCE_STATES, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SEQUENCE_STATES, BST_UNCHECKED);
    }

    bKeepOnLoad = m_spPluginInfoInitial->GetBool(
        PHYSX_EXTRACTOR_STATE_KEEP_ALL);
    if (bKeepOnLoad)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_ALL_STATES, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_ALL_STATES, BST_UNCHECKED);
    }

    bKeepOnLoad = m_spPluginInfoInitial->GetBool(
        PHYSX_EXTRACTOR_STATE_KEEP_DEFAULT);
    if (bKeepOnLoad)
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_DEFAULT_STATE, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_DEFAULT_STATE,
            BST_UNCHECKED);
    }
}
//---------------------------------------------------------------------------
BOOL NiPhysXExtractorDialog::OnCommand(int iWParamLow, int,
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
void NiPhysXExtractorDialog::ResetDefaultDialogOptions()
{
    // Resets options in dialog to defaults.
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SCENE_NAME), "PhysX Scene");
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SCALE), "1.0");
    
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_CONVERT_PSYS, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_FLUID, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_PSYS_SPACE, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_PSYS_ROT, BST_UNCHECKED);

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_HARDWARE_COOK, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_UNCOMPRESSEDNORMS, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_INFLATE, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_MESHES, BST_UNCHECKED);
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_CONVEX_SKIN), "0.05");
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_MATERIAL_BASE), "1");

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SEQUENCE_STATES, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_ALL_STATES, BST_UNCHECKED);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_KEEP_DEFAULT_STATE, BST_CHECKED);
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiPhysXExtractorDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiPhysXExtractorDialog::HandleOK()
{
    // Now that the user has accepted the changes that were made, we must set
    // the resulting plug-in info.

    char acSceneName[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SCENE_NAME), acSceneName, 256);
    m_spPluginInfoResults->SetValue(PHYSX_EXTRACTOR_SCENE_NAME, acSceneName);

    char acScale[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SCALE), acScale, 256);
    float fScale = (float) atof(acScale);
    m_spPluginInfoResults->SetFloat(PHYSX_EXTRACTOR_SCALE, fScale);

    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_CONVERTPSYS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_CONVERT_PSYS) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_SCENE_FLUID,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_FLUID) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_PSYS_SAME_SPACE,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_PSYS_SPACE) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_PSYS_USE_ROT,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_PSYS_ROT) == BST_CHECKED);

    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_HARDWARE_COOK,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_HARDWARE_COOK) == 
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_INFLATE,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_INFLATE) == 
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_UNCOMPRESSED,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_UNCOMPRESSEDNORMS) == 
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_KEEP_ON_LOAD,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_KEEP_MESHES) == BST_CHECKED);

    char acConvexSkin[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_CONVEX_SKIN), acConvexSkin, 256);
    float fConvexSkin = (float) atof(acConvexSkin);
    m_spPluginInfoResults->SetFloat(PHYSX_EXTRACTOR_CONVEX_SKIN, fConvexSkin);

    char acMaterialBase[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_MATERIAL_BASE),
        acMaterialBase, 256);
    int iMaterialBase = (int) atoi(acMaterialBase);
    m_spPluginInfoResults->SetInt(PHYSX_EXTRACTOR_MATERIAL_BASE, 
        iMaterialBase);

    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_SEQUENCE_STATES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_SEQUENCE_STATES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_STATE_KEEP_ALL,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_KEEP_ALL_STATES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(PHYSX_EXTRACTOR_STATE_KEEP_DEFAULT,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_KEEP_DEFAULT_STATE) ==
        BST_CHECKED);
}
//---------------------------------------------------------------------------
