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

#include "MaxImmerse.h"
#include "NiMAXExportSettingsDialog.h"
#include "MiResource.h"
#include <NiMeshProfileProcessor.h>
#include <NiScriptInfoDialogs.h>
#include <NiMeshProfileXMLParser.h>

//---------------------------------------------------------------------------
const char  
NiMAXExportSettingsDialog::ms_aPlatforms[NUM_DD_IDS][DD_STRING_LENGTH] =
{
    "Generic",
    "DX 9",
    "D3D 10",
    "Playstation 3",
    "Wii",
    "Xbox 360"
};

//---------------------------------------------------------------------------
NiMAXExportSettingsDialog::NiMAXExportSettingsDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent):
    NiDialog(IDD_NIF_EXPORT, hInstance, hWndParent)
    
{
    //Initialize the scene mesh profile name
    NiStrcpy(m_acSceneMeshProfile, 
        NI_MAX_PATH,
        NI_DEFAULTSCENEMESHPROFILENAME);
}
//---------------------------------------------------------------------------
NiMAXExportSettingsDialog::~NiMAXExportSettingsDialog()
{
}
//---------------------------------------------------------------------------
int NiMAXExportSettingsDialog::DoModal()
{
    m_spNewProcessScript = NiMAXOptions::GetProcessScript();
    m_spNewExportScript = NiMAXOptions::GetExportScript();

    m_spAvailableExportScripts = 
        NiMAXOptions::GenerateAvailableExportScripts();
    m_spAvailableProcessScripts = 
        NiMAXOptions::GenerateAvailableProcessScripts();
    m_bOptionsChanged = false;

    NiString sSceneMeshProfileName = 
        NiMAXOptions::GetValue(NI_SCENEMESHPROFILENAME);

    NiStrcpy(m_acSceneMeshProfile, NI_MAX_PATH,
        sSceneMeshProfileName);

    int iResult = NiDialog::DoModal();
    return iResult;
}
//---------------------------------------------------------------------------
void FillDropDownList(HWND hList, NiScriptInfoSet* pkSet, 
    NiScriptInfo* pkCurrentScript)
{
    NIASSERT(pkSet);
    NIASSERT(pkCurrentScript);

    SendMessage(hList, CB_RESETCONTENT , 0, 0);

    for (unsigned int ui = 0; ui < pkSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = pkSet->GetScriptAt(ui);
        if (pkInfo)
        {
            SendMessage(hList, CB_ADDSTRING, 0, (LPARAM) (const char*)
                pkInfo->GetName());
        }
    }

    SendMessage(hList, CB_SELECTSTRING, (WPARAM) -1, (LPARAM) (const char*) 
        pkCurrentScript->GetName());
}
//---------------------------------------------------------------------------
void FillMeshProfileList(HWND hList)
{
    SendMessage(hList, CB_RESETCONTENT , 0, 0);

    NiTPrimitiveSet<const char*> kProfiles;
    NiMeshProfileProcessor::GetAvailableProfiles(kProfiles);

    for (unsigned int ui = 0; ui < kProfiles.GetSize(); ui++)
    {        
        SendMessage(hList, CB_ADDSTRING, 0, (LPARAM) (const char*)
            kProfiles.GetAt(ui));
    }

    SendMessage(hList, CB_SELECTSTRING, (WPARAM) -1, (LPARAM) (const char*) 
        NiMAXOptions::GetValue(NI_SCENEMESHPROFILENAME));    
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::InitDialog()
{    
    CHECK_MEMORY();
    // Script options
    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT), 
        m_spAvailableExportScripts, m_spNewExportScript);
    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT), 
        m_spAvailableProcessScripts, m_spNewProcessScript);

    FillMeshProfileList(GetDlgItem(m_hWnd, IDC_SELECTED_MESH_PROFILE));

    CheckDlgButton(m_hWnd, IDC_USE_TEMPLATE_SCRIPTS_CHECK, 
        NiMAXOptions::GetBool(NI_USE_TEMPLATE_FOR_SCRIPT));

    // export options
    CheckDlgButton(m_hWnd, IDC_CAM, 
        NiMAXOptions::GetBool(NI_CONVERT_CAMERAS));
    CheckDlgButton(m_hWnd, IDC_LIGHTS, 
        NiMAXOptions::GetBool(NI_CONVERT_LIGHTS));

    // animation options
    CheckDlgButton(m_hWnd, IDC_APP_TIME,  
        NiMAXOptions::GetBool(NI_USE_APPTIME));
    CheckDlgButton(m_hWnd, IDC_EXPORT_CSASKEYFRAME, 
        NiMAXOptions::GetBool(NI_CONVERT_CSASKEYFRAMES));
    SetDlgItemText(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT, 
        NiMAXOptions::GetValue(NI_SKINNING_THRESHOLD));

    // texture options
    NiString strTextureExport = NiMAXOptions::GetValue(NI_TEXTURES);
    int iID = IDC_TEX_MAINNIF;
    if (strTextureExport == NI_SEPERATE_NIFS)
        iID = IDC_TEX_SEPARATE;
    else if(strTextureExport == NI_ORIGINAL_IMAGES)
        iID = IDC_TEX_ORIG;
    else if (strTextureExport == NI_ORIGINAL_IMAGES_VRAM)
        iID = IDC_TEX_ORIG_VRAM;
    else
        iID = IDC_TEX_MAINNIF;
    CheckTextureExportRadio(iID);
    
    // Platform specific settings
    NiSystemDesc::RendererID ePlatform = 
        (NiSystemDesc::RendererID) NiMAXOptions::GetInt(NI_EXPORT_PLATFORM);
    HWND hPlatform = GetDlgItem(m_hWnd, IDC_PLATFORM);
    SendMessage(hPlatform, CB_RESETCONTENT, 0L, 0L);

    SendMessage(hPlatform, CB_ADDSTRING, 0L,
        (LPARAM)ms_aPlatforms[ID_DD_GENERIC] );
    SendMessage(hPlatform, CB_ADDSTRING, 0L,
        (LPARAM)ms_aPlatforms[ID_DD_DX9]);
    SendMessage(hPlatform, CB_ADDSTRING, 0L,
        (LPARAM)ms_aPlatforms[ID_DD_D3D10]);
    SendMessage(hPlatform, CB_ADDSTRING, 0L,
        (LPARAM)ms_aPlatforms[ID_DD_PS3]);
    SendMessage(hPlatform, CB_ADDSTRING, 0L,
        (LPARAM)ms_aPlatforms[ID_DD_WII]);
    SendMessage(hPlatform, CB_ADDSTRING, 0L,
        (LPARAM)ms_aPlatforms[ID_DD_XENON]);

    int iCurSel = 0;
    switch (ePlatform)
    {
        case NiSystemDesc::RENDERER_XENON:
            iCurSel = ID_DD_XENON;
            break;
        case NiSystemDesc::RENDERER_PS3:
            iCurSel = ID_DD_PS3;
            break;
        case NiSystemDesc::RENDERER_DX9:
            iCurSel = ID_DD_DX9;
            break;
        case NiSystemDesc::RENDERER_WII:
            iCurSel = ID_DD_WII;
            break;
        case NiSystemDesc::RENDERER_D3D10:
            iCurSel = ID_DD_D3D10;
            break;
        default:    // RENDER_GENERIC or other value.
            iCurSel = ID_DD_GENERIC;
            break;
    }

    SendMessage(hPlatform, CB_SETCURSEL, (WPARAM) iCurSel, 0);
    OnPlatform();
    OnTextureExport();

    CheckDlgButton(m_hWnd, IDC_PER_TEXTURE_OVERRIDE, 
        NiMAXOptions::GetBool(NI_PER_TEXTURE_OVERRIDE));
    SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, 
        NiMAXOptions::GetValue(NI_IMAGE_FILE_DIR));
    CheckDlgButton(m_hWnd, IDC_ALLOW_NPOT_TEXTURES,
        NiMAXOptions::GetBool(NI_ALLOW_NPOT_TEXTURES));
    
    NiTexture::FormatPrefs::PixelLayout eLayout = 
        (NiTexture::FormatPrefs::PixelLayout) 
        (NiMAXOptions::GetInt(NI_PIXEL_LAYOUT));
      
    int iUISelection = IDC_TEX_QUAL_32;
    switch(eLayout)
    {
        case NiTexture::FormatPrefs::PALETTIZED_8 :
            iUISelection = IDC_TEX_QUAL_PAL;
            break;
        case NiTexture::FormatPrefs::HIGH_COLOR_16 :
            iUISelection = IDC_TEX_QUAL_16;
            break;
        case NiTexture::FormatPrefs::TRUE_COLOR_32 :
            iUISelection = IDC_TEX_QUAL_32;
            break;
        case NiTexture::FormatPrefs::COMPRESSED :
            iUISelection = IDC_TEX_QUAL_COMP;
            break;
        case NiTexture::FormatPrefs::BUMPMAP :
            iUISelection = IDC_TEX_QUAL_32;
            break;
        case NiTexture::FormatPrefs::PALETTIZED_4 :
            iUISelection = IDC_TEX_QUAL_PAL;
            break;
        case NiTexture::FormatPrefs::PIX_DEFAULT :
            iUISelection = IDC_TEX_QUAL_32;
            break;
    }

    CheckPixelLayoutRadio(iUISelection);
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
NiSystemDesc::RendererID NiMAXExportSettingsDialog::GetPlatform()
{
    CHECK_MEMORY();
    int iSel = (unsigned int) SendDlgItemMessage(m_hWnd, IDC_PLATFORM, 
        CB_GETCURSEL, 0, 0);
    switch (iSel)
    {
        case ID_DD_GENERIC:
            return NiSystemDesc::RENDERER_GENERIC;
        case ID_DD_XENON:
            return NiSystemDesc::RENDERER_XENON;
        case ID_DD_PS3:
            return NiSystemDesc::RENDERER_PS3;
        case ID_DD_DX9:
            return NiSystemDesc::RENDERER_DX9;
        case ID_DD_WII:
            return NiSystemDesc::RENDERER_WII;
        case ID_DD_D3D10:
            return NiSystemDesc::RENDERER_D3D10;
        default:
            return NiSystemDesc::RENDERER_GENERIC;
    };
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::RefreshImageFormatOptions()
{
    CHECK_MEMORY();
    
    bool bOrigImages = IsChecked(IDC_TEX_ORIG) || 
        IsChecked(IDC_TEX_ORIG_VRAM);
    EnableImageFormatOptions(m_hWnd, !bOrigImages);
    if (bOrigImages)
        return;

    // enable/disable content quality options
    // no content quality conversion for generic platform
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_COMP), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_PAL), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_16), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_32), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_PER_TEXTURE_OVERRIDE), true);
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnPlatform()
{
    CHECK_MEMORY();
    NiSystemDesc::RendererID ePlatform = GetPlatform();
    RefreshImageFormatOptions();

    // enable/disable content quality options
    switch (ePlatform)
    {
    case NiSystemDesc::RENDERER_XENON:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "Xbox360");
        break;
    case NiSystemDesc::RENDERER_PS3:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "PS3");
        break;
    case NiSystemDesc::RENDERER_WII:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "Wii");
        break;
    case NiSystemDesc::RENDERER_DX9:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "DX9");
        break;
    case NiSystemDesc::RENDERER_D3D10:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "D3D10");
        break;
    default:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "");
        break;
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::EnableImageFormatOptions(HWND m_hWnd, 
    bool bEnable)
{
    CHECK_MEMORY();
    NiString strTextureExport = GetTextureExport();
    if (strTextureExport == NI_SEPERATE_NIFS)
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), true);
    else
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    
    if (bEnable)
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_COMP),   true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_16),     true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_32),     true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_PAL),    true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_PER_TEXTURE_OVERRIDE), true);
    }
    else
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_COMP),   false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_16),     false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_32),     false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_PAL),    false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_PER_TEXTURE_OVERRIDE), false);
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnTextureExport()
{
    CHECK_MEMORY();
    NiString strTextureExport = GetTextureExport();
    if (strTextureExport == NI_SEPERATE_NIFS)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), true);
    }
    else if (strTextureExport == NI_ORIGINAL_IMAGES)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    }
    else if (strTextureExport == NI_ORIGINAL_IMAGES_VRAM)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    }
    else if (strTextureExport == NI_KEEP_IN_MAIN_NIF)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    }
    else
    {
        NIASSERT(0);
    }
    CHECK_MEMORY();
}
            
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSkinPercentageEdit()
{   
    CHECK_MEMORY();
    char acString[32];
#ifdef NIDEBUG
    int iReturn = 
#endif
        GetDlgItemText(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT,
        acString, 32);
    NIASSERT(iReturn != 0);
    NiString strPercent(acString);

    if (strPercent != NiMAXOptions::GetValue(NI_SKINNING_THRESHOLD))
    {
        m_bOptionsChanged = true;
    }

    int iSkinPercent = 0;
    bool bChanged = false;
    if (strPercent.ToInt(iSkinPercent))
    {
        if (iSkinPercent < 1)
        {
            iSkinPercent = 1;
            bChanged = true;
        }
        if (iSkinPercent > 50)
        {
            iSkinPercent = 50;
            bChanged = true;
        }
    }
    else
    {
        bChanged = true;
        iSkinPercent = NiMAXOptions::GetInt(NI_SKINNING_THRESHOLD);
    }

    if (bChanged)
    {
        HWND hwnd = GetDlgItem(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT);
        NiSprintf(acString, 32, "%d", iSkinPercent);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) acString);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnEditExportScriptButton()
{
    CHECK_MEMORY();
    NiScriptInfoSet* pkSet = m_spAvailableExportScripts;
    NIASSERT(pkSet);
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    NiScriptInfoPtr spScript = 
        NiScriptInfoDialogs::DoManagementDialog(m_spNewExportScript, 
        pkSet, m_hWnd, "EXPORT");

    NiScriptInfoSet& kInfoSet = pkManager->GetScriptInfoSet();
    
    if (spScript)
        m_spNewExportScript = spScript;

    if (kInfoSet.IsTemplate(spScript) && !pkSet->IsTemplate(spScript))
    {   
        CheckDlgButton(m_hWnd, IDC_USE_TEMPLATE_SCRIPTS_CHECK, FALSE);
    }

    if (spScript && pkSet && 
        pkSet->GetScriptIndex(spScript->GetName()) == NIPT_INVALID_INDEX)
    {
        pkSet->AddScript(spScript);
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT, CB_ADDSTRING ,
            0, (LPARAM) (const char*) spScript->GetName());
    }

    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT), 
        m_spAvailableExportScripts, 
        m_spNewExportScript);

    m_bOptionsChanged = true;
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnEditProcessScriptButton()
{
    CHECK_MEMORY();
    NiScriptInfoSet* pkSet = m_spAvailableProcessScripts;
    NIASSERT(pkSet);
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();

    NiScriptInfoPtr spScript = 
        NiScriptInfoDialogs::DoManagementDialog(m_spNewProcessScript, 
        pkSet, m_hWnd, "PROCESS");

    NiScriptInfoSet& kInfoSet = pkManager->GetScriptInfoSet();
    if (spScript)
        m_spNewProcessScript = spScript;

    if (kInfoSet.IsTemplate(spScript) && !pkSet->IsTemplate(spScript))
    {   
        CheckDlgButton(m_hWnd, IDC_USE_TEMPLATE_SCRIPTS_CHECK, FALSE);
    }

    if (spScript && pkSet &&
        pkSet->GetScriptIndex(spScript->GetName()) == NIPT_INVALID_INDEX)
    {
        pkSet->AddScript(spScript);
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT, 
            CB_ADDSTRING , 0, (LPARAM) (const char*) spScript->GetName());
    }
    
    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT), 
        m_spAvailableProcessScripts, 
        m_spNewProcessScript);

    m_bOptionsChanged = true;
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSelectedExportScript()
{
   CHECK_MEMORY();
   int iSelIndex = (unsigned int) SendDlgItemMessage(m_hWnd,
        IDC_SELECTED_EXPORT_SCRIPT, CB_GETCURSEL, 0, 0);
    if (iSelIndex != CB_ERR)
    {
        char acScriptName[NI_MAX_PATH];
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT,
            CB_GETLBTEXT, (WPARAM) iSelIndex, (LPARAM) (LPCSTR)
            &acScriptName[0]);

        NiScriptInfoSet* pkSet = 
            m_spAvailableExportScripts;
        NIASSERT(pkSet);
        NiScriptInfo* pkSelScript = pkSet->GetScript(
            acScriptName);

        if (pkSelScript)
        {
            m_spNewExportScript = pkSelScript;
            m_bOptionsChanged = true;
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSelectedProcessScript()
{                
    CHECK_MEMORY();
    int iSelIndex = (unsigned int) SendDlgItemMessage(m_hWnd,
        IDC_SELECTED_PROCESS_SCRIPT, CB_GETCURSEL, 0, 0);
    if (iSelIndex != CB_ERR)
    {
        char acScriptName[NI_MAX_PATH];
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT,
            CB_GETLBTEXT, (WPARAM) iSelIndex, (LPARAM) (LPCSTR)
            &acScriptName[0]);

        NiScriptInfoSet* pkSet = 
            m_spAvailableProcessScripts;
        NIASSERT(pkSet);
        NiScriptInfo* pkSelScript = pkSet->GetScript(
            acScriptName);

        if (pkSelScript)
        {
            m_spNewProcessScript = pkSelScript;
            m_bOptionsChanged = true;
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSelectedMeshProfile()
{
   CHECK_MEMORY();
   int iSelIndex = (unsigned int) SendDlgItemMessage(m_hWnd,
        IDC_SELECTED_MESH_PROFILE, CB_GETCURSEL, 0, 0);
    if (iSelIndex != CB_ERR)
    {
        char acMeshProfile[NI_MAX_PATH];
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_MESH_PROFILE,
            CB_GETLBTEXT, (WPARAM) iSelIndex, (LPARAM) (LPCSTR)
            &acMeshProfile[0]);

        if (acMeshProfile)
        {
            NiStrcpy(m_acSceneMeshProfile, NI_MAX_PATH, acMeshProfile);
            m_bOptionsChanged = true;
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnReloadProfilesButton()
{
    CHECK_MEMORY();
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    NiMeshProfileProcessor::DestroyMeshProfileProcessor();

    NiMeshProfileProcessor::CreateMeshProfileProcessor();
    NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor();

     // Output the status and the errors
    NiUInt32 uiCount = 
        NiMeshProfileProcessor::GetErrorHandler().GetMessageCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiFixedString theMessage = 
            NiMeshProfileProcessor::GetErrorHandler().GetMessageA(ui);
        switch (NiMeshProfileProcessor::GetErrorHandler().GetMessageType(ui))
        {
        case NiMeshProfileErrorHandler::MT_ANNOTATION:
            {
            mprintf(theMessage); 
            NiOutputDebugString(theMessage);
            }
            break;
        case NiMeshProfileErrorHandler::MT_WARNING:
            mprintf(theMessage); 
            NILOGWARNING(theMessage);
            break;
        case NiMeshProfileErrorHandler::MT_ERROR:
            mprintf(theMessage); 
            NILOGERROR(theMessage);
            break;
        }
    }

    FillMeshProfileList(GetDlgItem(m_hWnd, IDC_SELECTED_MESH_PROFILE));

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
BOOL NiMAXExportSettingsDialog::OnCommand(int iWParamLow, int iWParamHigh,
    long)
{
    CHECK_MEMORY();
    switch(iWParamLow)
    {
        case IDOK:
            OnOK();
            EndDialog(m_hWnd, IDOK);
            break;
        case IDCANCEL:
            EndDialog(m_hWnd, IDCANCEL);
            break;
        case IDC_PLATFORM:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                m_bOptionsChanged = true;
                OnPlatform();
            }
            return TRUE;
        case IDC_SKIN_PERCENTAGE_EDIT:
            if (iWParamHigh == EN_CHANGE)
            {
                OnSkinPercentageEdit();
            }
            return TRUE;
        case IDC_CAM:
        case IDC_LIGHTS:
        case IDC_TEX_QUAL_32:
        case IDC_TEX_QUAL_COMP:
        case IDC_TEX_QUAL_16:
        case IDC_TEX_QUAL_PAL:
        case IDC_KF_ONE:
        case IDC_KF_SEPARATE:
        case IDC_APP_TIME:
        case IDC_EXPORT_CSASKEYFRAME:
        case IDC_USE_TEMPLATE_SCRIPTS_CHECK:
        case IDC_PER_TEXTURE_OVERRIDE:
            m_bOptionsChanged = true;
            break;
        case IDC_TEX_MAINNIF:
        case IDC_TEX_SEPARATE:
        case IDC_TEX_ORIG:
        case IDC_TEX_ORIG_VRAM:
            m_bOptionsChanged = true;
            OnTextureExport();
            break;
        case IDC_EDIT_PROCESS_SCRIPT_BUTTON:
            OnEditProcessScriptButton();
            m_bOptionsChanged = true;
            break;
        case IDC_EDIT_EXPORT_SCRIPT_BUTTON:
            OnEditExportScriptButton();
            m_bOptionsChanged = true;
            break;
        case IDC_SELECTED_PROCESS_SCRIPT:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                OnSelectedProcessScript();
                m_bOptionsChanged = true;
            }
            break;
        case IDC_SELECTED_EXPORT_SCRIPT:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                OnSelectedExportScript();
                m_bOptionsChanged = true;
            }
            break;
        case IDC_SELECTED_MESH_PROFILE:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                OnSelectedMeshProfile();
                m_bOptionsChanged = true;
            }
            break;
        case IDC_RELOAD_PROFILES_BUTTON:
            OnReloadProfilesButton();
            m_bOptionsChanged = true;
            break;
        default:
            break;
    }
            
    return FALSE;
}

//---------------------------------------------------------------------------
bool NiMAXExportSettingsDialog::IsChecked(int iResource)
{
    CHECK_MEMORY();
    int iState = IsDlgButtonChecked(m_hWnd, iResource);
    switch (iState)
    {
        case BST_CHECKED:
            return true; 
        case BST_INDETERMINATE:
            NIASSERT(iState != BST_INDETERMINATE);
            return false;
        case BST_UNCHECKED:
            return false;
    }
    return false;
}
//---------------------------------------------------------------------------
NiString NiMAXExportSettingsDialog::GetTextureExport()
{
    CHECK_MEMORY();
    NiString strTextureExport = NI_SEPERATE_NIFS;
    bool bSeperateNifs = IsChecked(IDC_TEX_SEPARATE);
    bool bOriginalTextures = IsChecked(IDC_TEX_ORIG);
    bool bOriginalTexturesVRAM = IsChecked(IDC_TEX_ORIG_VRAM);
    bool bMainNif = IsChecked(IDC_TEX_MAINNIF);

    if (bSeperateNifs)
    {    
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bMainNif);
        NIASSERT(!bOriginalTexturesVRAM);
        strTextureExport = NI_SEPERATE_NIFS;
    }
    else if(bOriginalTextures)
    {    
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bMainNif);
        NIASSERT(!bOriginalTexturesVRAM);
        strTextureExport = NI_ORIGINAL_IMAGES;
    }
    else if (bOriginalTexturesVRAM)
    {    
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bMainNif);
        strTextureExport = NI_ORIGINAL_IMAGES_VRAM;
    }
    else if (bMainNif)
    {    
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bOriginalTexturesVRAM);
        strTextureExport = NI_KEEP_IN_MAIN_NIF;
    }
    else
    {
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bMainNif);
        NIASSERT(0);
    }

    return strTextureExport;
}

//---------------------------------------------------------------------------
NiTexture::FormatPrefs::PixelLayout 
NiMAXExportSettingsDialog::GetPixelLayout()
{
    CHECK_MEMORY();
    NiTexture::FormatPrefs::PixelLayout eLayout = 
        NiTexture::FormatPrefs::PIX_DEFAULT;

    bool bTrueColor = IsChecked(IDC_TEX_QUAL_32); 
    bool bHighColor = IsChecked(IDC_TEX_QUAL_16);
    bool bCompressed = IsChecked(IDC_TEX_QUAL_COMP);
    bool bPaletted = IsChecked(IDC_TEX_QUAL_PAL);

    if (bTrueColor)
    {
        NIASSERT(!bHighColor);
        NIASSERT(!bCompressed);
        NIASSERT(!bPaletted);
        return NiTexture::FormatPrefs::PIX_DEFAULT;
    }
    if (bHighColor)
    {
        NIASSERT(!bTrueColor);
        NIASSERT(!bCompressed);
        NIASSERT(!bPaletted);
        return NiTexture::FormatPrefs::HIGH_COLOR_16;
    }
    if (bCompressed)
    {
        NIASSERT(!bTrueColor);
        NIASSERT(!bHighColor);
        NIASSERT(!bPaletted);
        return NiTexture::FormatPrefs::COMPRESSED;
    }
    if (bPaletted)
    {
        NIASSERT(!bTrueColor);
        NIASSERT(!bHighColor);
        NIASSERT(!bCompressed);
        return NiTexture::FormatPrefs::PALETTIZED_8;
    }

    return eLayout;
}
        
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnOK()
{
    CHECK_MEMORY();
    bool bChecked = false;

    // Script options
    {
        bChecked = IsChecked(IDC_USE_TEMPLATE_SCRIPTS_CHECK);
        NiMAXOptions::SetBool(NI_USE_TEMPLATE_FOR_SCRIPT, bChecked);

        NIASSERT(m_spNewProcessScript);
        NiMAXOptions::SetProcessScript(m_spNewProcessScript);
        
        NIASSERT(m_spNewExportScript);
        NiMAXOptions::SetExportScript(m_spNewExportScript);
    }

    //scene mesh profile
    {
        NiMAXOptions::SetValue(NI_SCENEMESHPROFILENAME, m_acSceneMeshProfile);
    }

    // Export options
    {
        bChecked = IsChecked(IDC_CAM);
        NiMAXOptions::SetBool(NI_CONVERT_CAMERAS, bChecked);

        bChecked = IsChecked(IDC_LIGHTS);
        NiMAXOptions::SetBool(NI_CONVERT_LIGHTS, bChecked);
    }

    // Animation options
    {
        bChecked = IsChecked(IDC_APP_TIME);
        NiMAXOptions::SetBool(NI_USE_APPTIME, bChecked);

        bChecked = IsChecked(IDC_EXPORT_CSASKEYFRAME);
        NiMAXOptions::SetBool(NI_CONVERT_CSASKEYFRAMES, bChecked);

        char acString[32];
#ifdef NIDEBUG
        int iReturn = 
#endif
            GetDlgItemText(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT,
            acString, 32);
        NIASSERT(iReturn != 0);
        NiString strPercent(acString);
        int iSkinPercent = 0;
        if (strPercent.ToInt(iSkinPercent))
            NiMAXOptions::SetInt(NI_SKINNING_THRESHOLD, iSkinPercent);
    }

    
    // Texturing options
    {
        NiString strTextureExport = GetTextureExport();
        NiMAXOptions::SetValue(NI_TEXTURES, strTextureExport);
    }

    // Platform SPecific options
    NiSystemDesc::RendererID eRenderer = GetPlatform();
    NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, eRenderer);

    char acImageFileDir[MAX_PATH];
    GetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, 
        (LPTSTR) acImageFileDir, MAX_PATH + 1);

    NiMAXOptions::SetValue(NI_IMAGE_FILE_DIR, acImageFileDir);

    NiTexture::FormatPrefs::PixelLayout ePixelLayout = GetPixelLayout();
    NiMAXOptions::SetInt(NI_PIXEL_LAYOUT, ePixelLayout);

    bChecked = IsChecked(IDC_PER_TEXTURE_OVERRIDE);
    NiMAXOptions::SetBool(NI_PER_TEXTURE_OVERRIDE, bChecked);
    bChecked = IsChecked(IDC_ALLOW_NPOT_TEXTURES);
    NiMAXOptions::SetBool(NI_ALLOW_NPOT_TEXTURES, bChecked);

    if (m_bOptionsChanged)
        SetSaveRequiredFlag(TRUE);
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
BOOL  NiMAXExportSettingsDialog::OnMessage(unsigned int uiMessage, 
    int iWParamLow, int iWParamHigh, long lParam)
{
    return NiDialog::OnMessage(uiMessage, iWParamLow, iWParamHigh, lParam);
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::CheckTextureExportRadio(int iSelection)
{
    CHECK_MEMORY();
    int iState = BST_UNCHECKED;
    CheckDlgButton(m_hWnd, IDC_TEX_SEPARATE, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_ORIG, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_MAINNIF, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_ORIG_VRAM, iState);

    CheckDlgButton(m_hWnd, iSelection, BST_CHECKED);    
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::CheckPixelLayoutRadio(int iSelection)
{
    CHECK_MEMORY();
    int iState = BST_UNCHECKED;
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_PAL, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_16, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_32, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_COMP, iState);

    CheckDlgButton(m_hWnd, iSelection, BST_CHECKED);
}
