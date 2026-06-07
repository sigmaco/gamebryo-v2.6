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
#include "NiControllerExtractorDialog.h"
#include "resource.h"
#include "ScriptParameterDefinitions.h"
#include <windowsx.h>
#include <commctrl.h>

static const char* gs_pcFlipControllerType = "NiFlipController;";
const char* NiControllerExtractorDialog::ms_pcPSResetControllerType =
    "NiPSResetOnLoopCtlr;";

//---------------------------------------------------------------------------
NiControllerExtractorDialog::NiControllerExtractorDialog(
    NiModuleRef hInstance, NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo)
    : NiDialog(IDD_OPTIONS, hInstance, hWndParent)
{
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
NiControllerExtractorDialog::~NiControllerExtractorDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::InitDialog()
{
    // Load options.
    if (m_spPluginInfoInitial->GetBool(PARAM_BOOL_SINGLEKF))
    {
        CheckDlgButton(m_hWnd, IDC_RADIO_SINGLE_KF, BST_CHECKED);
        CheckDlgButton(m_hWnd, IDC_RADIO_MULTIPLE_KF, BST_UNCHECKED);
        EnableFileNameOptions(FALSE);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_RADIO_SINGLE_KF, BST_UNCHECKED);
        CheckDlgButton(m_hWnd, IDC_RADIO_MULTIPLE_KF, BST_CHECKED);
        EnableFileNameOptions(TRUE);
    }

    if (m_spPluginInfoInitial->GetBool(PARAM_BOOL_EXPORTEDFILENAME))
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXPORTED_FILE_NAME, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXPORTED_FILE_NAME, BST_UNCHECKED);
    }

    if (m_spPluginInfoInitial->GetBool(PARAM_BOOL_CHARACTERNAME))
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CHARACTER_NAME, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CHARACTER_NAME, BST_UNCHECKED);
    }

    if (m_spPluginInfoInitial->GetBool(PARAM_BOOL_UPPERCASE))
    {
        CheckDlgButton(m_hWnd, IDC_RADIO_UPPER_CASE, BST_CHECKED);
        CheckDlgButton(m_hWnd, IDC_RADIO_LOWER_CASE, BST_UNCHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_RADIO_UPPER_CASE, BST_UNCHECKED);
        CheckDlgButton(m_hWnd, IDC_RADIO_LOWER_CASE, BST_CHECKED);
    }

    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR),
        m_spPluginInfoInitial->GetValue(PARAM_STRING_SEPARATOR));

    if (m_spPluginInfoInitial->GetBool(PARAM_BOOL_CREATEKFM))
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CREATE_KFM, BST_CHECKED);
        EnableKFMOptions(TRUE);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CREATE_KFM, BST_UNCHECKED);
        EnableKFMOptions(FALSE);
    }

    if (m_spPluginInfoInitial->GetBool(PARAM_BOOL_CHARACTERNAMEINKFMFILE))
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CHARACTER_NAME_IN_KFM_FILE,
            BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_CHARACTER_NAME_IN_KFM_FILE,
            BST_UNCHECKED);
    }

    if (!m_spPluginInfoInitial->GetBool(PARAM_BOOL_ALWAYSCOMPRESS))
    {
        CheckDlgButton(m_hWnd, IDC_COMPRESSION_ALWAYSCHECK,
            BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_COMPRESSION_ALWAYSCHECK,
            BST_UNCHECKED);
    }
   
    NiKFMTool::TransitionType eSyncTransType = (NiKFMTool::TransitionType)
        m_spPluginInfoInitial->GetInt(PARAM_INT_SYNCTRANSTYPE);
    SetSyncTransType(eSyncTransType);

    NiKFMTool::TransitionType eNonSyncTransType = (NiKFMTool::TransitionType)
        m_spPluginInfoInitial->GetInt(PARAM_INT_NONSYNCTRANSTYPE);
    SetNonSyncTransType(eNonSyncTransType);

    float fSyncTransDuration = m_spPluginInfoInitial->GetFloat(
        PARAM_FLOAT_SYNCTRANSDURATION);
    NiString strSyncTransDuration;
    strSyncTransDuration.Format("%d", (unsigned int) floor(fSyncTransDuration
        * 1000.0f));
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SYNC_TRANS_DURATION),
        strSyncTransDuration);

    float fNonSyncTransDuration = m_spPluginInfoInitial->GetFloat(
        PARAM_FLOAT_NONSYNCTRANSDURATION);
    NiString strNonSyncTransDuration;
    strNonSyncTransDuration.Format("%d", (unsigned int) floor(
        fNonSyncTransDuration * 1000.0f));
    SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_NONSYNC_TRANS_DURATION),
        strNonSyncTransDuration);

    // Reset the examples so the user can see what stuff will look like
    ResetExampleFileNames();

    NiString strExcludedControllerTypes = m_spPluginInfoInitial->GetValue(
        PARAM_STRING_EXCLUDEDCONTROLLERTYPES);
    char acExcludedControllerTypes[NI_MAX_PATH];
    NiSprintf(acExcludedControllerTypes, NI_MAX_PATH,
        strExcludedControllerTypes);
    bool bExcludeFlipControllers = false;
    bool bExcludePSResetControllers = false;
    bool bExcludeCustomControllers = false;
    const char* pcDelim = ";";
    char* pcContext = NULL;
    char* pcType = NiStrtok(acExcludedControllerTypes, pcDelim, &pcContext);
    while (pcType)
    {
        if (strstr(gs_pcFlipControllerType, pcType))
        {
            bExcludeFlipControllers = true;
        }
        else if (strstr(ms_pcPSResetControllerType, pcType))
        {
            bExcludePSResetControllers = true;
        }
        else
        {
            bExcludeCustomControllers = true;
        }

        pcType = NiStrtok(NULL, pcDelim, &pcContext);
    }
    strExcludedControllerTypes.Replace(gs_pcFlipControllerType, "");
    strExcludedControllerTypes.Replace(ms_pcPSResetControllerType, "");
    if (bExcludeFlipControllers)
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXCLUDEFLIPCONTROLLERS, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXCLUDEFLIPCONTROLLERS,
            BST_UNCHECKED);
    }
    if (bExcludePSResetControllers)
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXCLUDEPSRESET, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXCLUDEPSRESET, BST_UNCHECKED);
    }
    if (bExcludeCustomControllers)
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXCLUDECUSTOMCONTROLLERS,
            BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_CHECK_EXCLUDECUSTOMCONTROLLERS,
            BST_UNCHECKED);
    }
    EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_EXCLUDEDCONTROLLERTYPES),
        bExcludeCustomControllers);
    SetDlgItemText(m_hWnd, IDC_EDIT_EXCLUDEDCONTROLLERTYPES,
        strExcludedControllerTypes);
}
//---------------------------------------------------------------------------
BOOL NiControllerExtractorDialog::OnMessage(unsigned int uiMessage,
    int, int, long)
{
    if (uiMessage == WM_HSCROLL)
    {
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL NiControllerExtractorDialog::OnCommand(int iWParamLow, int, 
    long)
{
    switch (iWParamLow)
    {
    case IDOK:
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    case IDC_CHECK_EXPORTED_FILE_NAME:
    case IDC_CHECK_CHARACTER_NAME:
    case IDC_RADIO_UPPER_CASE:
    case IDC_RADIO_LOWER_CASE:
        ResetExampleFileNames();
        return TRUE;
    case IDC_EDIT_SEPARATOR:
        VerifyCorrectSeparator();
        ResetExampleFileNames();
        return TRUE;
    case IDC_RADIO_SINGLE_KF:
        EnableFileNameOptions(FALSE);
        ResetExampleFileNames();
        return TRUE;
    case IDC_RADIO_MULTIPLE_KF:
        EnableFileNameOptions(TRUE);
        ResetExampleFileNames();
        return TRUE;
    case IDC_CHECK_CREATE_KFM:
        EnableKFMOptions(IsDlgButtonChecked(m_hWnd, IDC_CHECK_CREATE_KFM)
            == BST_CHECKED);
        return TRUE;
    case IDC_COMPRESSION_ALWAYSCHECK :
        return TRUE;
    case IDC_CHECK_EXCLUDECUSTOMCONTROLLERS:
        EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_EXCLUDEDCONTROLLERTYPES),
            IsDlgButtonChecked(m_hWnd, IDC_CHECK_EXCLUDECUSTOMCONTROLLERS) ==
            BST_CHECKED);
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiControllerExtractorDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::HandleOK()
{
    m_spPluginInfoResults->SetBool(PARAM_BOOL_SINGLEKF, IsDlgButtonChecked(
        m_hWnd, IDC_RADIO_SINGLE_KF) == BST_CHECKED);

    m_spPluginInfoResults->SetBool(PARAM_BOOL_EXPORTEDFILENAME,
        IsDlgButtonChecked(m_hWnd, IDC_CHECK_EXPORTED_FILE_NAME) ==
        BST_CHECKED);

    m_spPluginInfoResults->SetBool(PARAM_BOOL_CHARACTERNAME,
        IsDlgButtonChecked(m_hWnd, IDC_CHECK_CHARACTER_NAME) == BST_CHECKED);

    m_spPluginInfoResults->SetBool(PARAM_BOOL_UPPERCASE, IsDlgButtonChecked(
        m_hWnd, IDC_RADIO_UPPER_CASE) == BST_CHECKED);

    char acSeparator[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR), acSeparator, 256);
    m_spPluginInfoResults->SetValue(PARAM_STRING_SEPARATOR, acSeparator);

    m_spPluginInfoResults->SetBool(PARAM_BOOL_CREATEKFM, IsDlgButtonChecked(
        m_hWnd, IDC_CHECK_CREATE_KFM) == BST_CHECKED);

    m_spPluginInfoResults->SetBool(PARAM_BOOL_CHARACTERNAMEINKFMFILE,
        IsDlgButtonChecked(m_hWnd, IDC_CHECK_CHARACTER_NAME_IN_KFM_FILE) ==
        BST_CHECKED);

    m_spPluginInfoResults->SetInt(PARAM_INT_SYNCTRANSTYPE,
        GetSyncTransType());

    m_spPluginInfoResults->SetInt(PARAM_INT_NONSYNCTRANSTYPE,
        GetNonSyncTransType());

    char acSyncTransDuration[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SYNC_TRANS_DURATION),
        acSyncTransDuration, 256);
    float fSyncTransDuration = (float) atof(acSyncTransDuration) / 1000.0f;
    if (fSyncTransDuration == 0.0f)
    {
        fSyncTransDuration = 0.001f;
    }
    m_spPluginInfoResults->SetFloat(PARAM_FLOAT_SYNCTRANSDURATION,
        fSyncTransDuration);

    char acNonSyncTransDuration[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_NONSYNC_TRANS_DURATION),
        acNonSyncTransDuration, 256);
    float fNonSyncTransDuration = (float) atof(acNonSyncTransDuration) /
        1000.0f;
    if (fNonSyncTransDuration == 0.0f)
    {
        fNonSyncTransDuration = 0.001f;
    }
    m_spPluginInfoResults->SetFloat(PARAM_FLOAT_NONSYNCTRANSDURATION,
        fNonSyncTransDuration);

    m_spPluginInfoResults->SetBool(PARAM_BOOL_ALWAYSCOMPRESS, 
        IsDlgButtonChecked(m_hWnd, IDC_COMPRESSION_ALWAYSCHECK)
        != BST_CHECKED);

    NiString strExcludedControllerTypes;
    if (IsDlgButtonChecked(m_hWnd, IDC_CHECK_EXCLUDEFLIPCONTROLLERS) ==
        BST_CHECKED)
    {
        strExcludedControllerTypes = gs_pcFlipControllerType;
    }
    if (IsDlgButtonChecked(m_hWnd, IDC_CHECK_EXCLUDEPSRESET) == BST_CHECKED)
    {
        strExcludedControllerTypes.Concatenate(ms_pcPSResetControllerType);
    }
    if (IsDlgButtonChecked(m_hWnd, IDC_CHECK_EXCLUDECUSTOMCONTROLLERS) ==
        BST_CHECKED)
    {
        char acExcludedControllerTypes[NI_MAX_PATH];
        GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_EXCLUDEDCONTROLLERTYPES),
            acExcludedControllerTypes, NI_MAX_PATH);

        const char* pcDelim = ";";
        char* pcContext = NULL;
        char* pcType = NiStrtok(acExcludedControllerTypes, pcDelim,
            &pcContext);
        NiString strType;
        while (pcType)
        {
            strType = pcType;
            strType.TrimLeft();
            strType.TrimRight();
            strType.Format("%s%s", strType, pcDelim);
            if (!strExcludedControllerTypes.Contains(strType))
            {
                strExcludedControllerTypes.Concatenate(strType);
            }

            pcType = NiStrtok(NULL, pcDelim, &pcContext);
        }
    }
    m_spPluginInfoResults->SetValue(PARAM_STRING_EXCLUDEDCONTROLLERTYPES,
        strExcludedControllerTypes);
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::VerifyCorrectSeparator()
{
    char acSeparator[256];
    GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR), acSeparator, 256);
    if (strlen(acSeparator) > 1)
    {
        acSeparator[1] = '\0';
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR), acSeparator);
    }
    SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR), EM_SETSEL, 0, -1);
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::EnableFileNameOptions(BOOL bEnabled)
{
    EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_EXPORTED_FILE_NAME), bEnabled);
    EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_CHARACTER_NAME), bEnabled);
    EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR), bEnabled);
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::ResetExampleFileNames()
{
    NiString strKFFileName;
    if (IsDlgButtonChecked(m_hWnd, IDC_RADIO_SINGLE_KF) == BST_CHECKED)
    {
        strKFFileName = "ExportedFile.";
    }
    else
    {
        char acSeparator[256];
        GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_SEPARATOR), acSeparator,
            256);
        NiString strSeparator = acSeparator;

        if (IsDlgButtonChecked(m_hWnd, IDC_CHECK_EXPORTED_FILE_NAME) ==
            BST_CHECKED)
        {
            strKFFileName = "ExportedFile" + strSeparator;
        }
        if (IsDlgButtonChecked(m_hWnd, IDC_CHECK_CHARACTER_NAME) ==
            BST_CHECKED)
        {
            strKFFileName += "CharacterName" + strSeparator;
        }
        strKFFileName += "LayerName" + strSeparator + "SequenceName.";
    }

    if (IsDlgButtonChecked(m_hWnd, IDC_RADIO_UPPER_CASE) == BST_CHECKED)
    {
        strKFFileName += "KF";
    }
    else
    {
        strKFFileName += "kf";
    }

    SetWindowText(GetDlgItem(m_hWnd, IDC_STATIC_KF_FILENAME), strKFFileName);
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::EnableKFMOptions(BOOL bEnabled)
{
    EnableWindow(GetDlgItem(m_hWnd, IDC_CHECK_CHARACTER_NAME_IN_KFM_FILE),
        bEnabled);

    EnableWindow(GetDlgItem(m_hWnd, IDC_COMBO_SYNC_TRANS_TYPE), bEnabled);
    EnableWindow(GetDlgItem(m_hWnd, IDC_COMBO_NONSYNC_TRANS_TYPE), bEnabled);

    EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_SYNC_TRANS_DURATION), bEnabled);
    EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_NONSYNC_TRANS_DURATION),
        bEnabled);
}
//---------------------------------------------------------------------------
NiKFMTool::TransitionType NiControllerExtractorDialog::GetSyncTransType()
{
    int iIndex = ComboBox_GetCurSel(GetDlgItem(m_hWnd,
        IDC_COMBO_SYNC_TRANS_TYPE));
    NIASSERT(iIndex != CB_ERR);
    return (NiKFMTool::TransitionType) ComboBox_GetItemData(GetDlgItem(m_hWnd,
        IDC_COMBO_SYNC_TRANS_TYPE), iIndex);
}
//---------------------------------------------------------------------------
NiKFMTool::TransitionType NiControllerExtractorDialog::GetNonSyncTransType()
{
    int iIndex = ComboBox_GetCurSel(GetDlgItem(m_hWnd,
        IDC_COMBO_NONSYNC_TRANS_TYPE));
    NIASSERT(iIndex != CB_ERR);
    return (NiKFMTool::TransitionType) ComboBox_GetItemData(GetDlgItem(m_hWnd,
        IDC_COMBO_NONSYNC_TRANS_TYPE), iIndex);
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::SetSyncTransType(
    NiKFMTool::TransitionType eType)
{
    // Populate combo box.
    int iIndex = ComboBox_AddString(GetDlgItem(m_hWnd,
        IDC_COMBO_SYNC_TRANS_TYPE), TransTypeToString(NiKFMTool::TYPE_MORPH));
    ComboBox_SetItemData(GetDlgItem(m_hWnd, IDC_COMBO_SYNC_TRANS_TYPE),
        iIndex, NiKFMTool::TYPE_MORPH);

    // Select appropriate item in combo box.
    iIndex = ComboBox_SelectString(GetDlgItem(m_hWnd,
        IDC_COMBO_SYNC_TRANS_TYPE), -1, TransTypeToString(eType));
    NIASSERT(iIndex != CB_ERR);
}
//---------------------------------------------------------------------------
void NiControllerExtractorDialog::SetNonSyncTransType(
    NiKFMTool::TransitionType eType)
{
    // Populate combo box.
    int iIndex = ComboBox_AddString(GetDlgItem(m_hWnd,
        IDC_COMBO_NONSYNC_TRANS_TYPE), TransTypeToString(
        NiKFMTool::TYPE_BLEND));
    ComboBox_SetItemData(GetDlgItem(m_hWnd, IDC_COMBO_NONSYNC_TRANS_TYPE),
        iIndex, NiKFMTool::TYPE_BLEND);
    iIndex = ComboBox_AddString(GetDlgItem(m_hWnd,
        IDC_COMBO_NONSYNC_TRANS_TYPE), TransTypeToString(
        NiKFMTool::TYPE_CROSSFADE));
    ComboBox_SetItemData(GetDlgItem(m_hWnd, IDC_COMBO_NONSYNC_TRANS_TYPE),
        iIndex, NiKFMTool::TYPE_CROSSFADE);

    // Select appropriate item in combo box.
    iIndex = ComboBox_SelectString(GetDlgItem(m_hWnd,
        IDC_COMBO_NONSYNC_TRANS_TYPE), -1, TransTypeToString(eType));
    NIASSERT(iIndex != CB_ERR);
}
//---------------------------------------------------------------------------
NiString NiControllerExtractorDialog::TransTypeToString(
    NiKFMTool::TransitionType eType)
{
    switch (eType)
    {
        case NiKFMTool::TYPE_BLEND:
            return "Blend";
        case NiKFMTool::TYPE_MORPH:
            return "Morph";
        case NiKFMTool::TYPE_CROSSFADE:
            return "CrossFade";
        default:
            NIASSERT(false);
            return "";
    }
}
//---------------------------------------------------------------------------
