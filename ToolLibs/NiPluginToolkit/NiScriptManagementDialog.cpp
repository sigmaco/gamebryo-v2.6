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
#include "NiScriptManagementDialog.h"
#include "NiScriptInfoDialogs.h"
#include "NiScriptTemplateManager.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "resource.h"
#include <NiSystem.h>
#include "NiStringTokenizer.h"
#include "NiPluginManager.h"
#include "NiScriptNameChangeDialog.h"
#include "NiScriptNewFromTemplateDialog.h"
#include <Commctrl.h>

//---------------------------------------------------------------------------
NiScriptManagementDialog::NiScriptManagementDialog(NiModuleRef hInstance, 
    NiScriptInfo* pkInfo, NiScriptInfoSet* pkSet, NiWindowRef hWndParent,
    const char* pcTypes) : NiDialog(IDD_SCRIPT_EDITOR, hInstance, hWndParent)
{
    m_spOriginalInfo = pkInfo;
    m_spScriptSet = pkSet;
    m_spInfo = pkInfo->Clone();
    m_eTabOption = ALL_PLUGINS;
    m_strTypes = pcTypes;
    m_idReturnState = -1;
    m_spRemovedScripts = NiNew NiScriptInfoSet();
}
//---------------------------------------------------------------------------
NiScriptManagementDialog::~NiScriptManagementDialog()
{
    m_spInfo = 0;
    m_spOriginalInfo = 0;
    m_spScriptSet = NULL;
    m_spRemovedScripts = NULL;
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::AddScriptsToComboBox(int iComboBoxID)
{
    NiWindowRef hWndCombo = GetDlgItem(m_hWnd, iComboBoxID);

    int iCurSel = (int)SendDlgItemMessage(m_hWnd, iComboBoxID, 
        CB_GETCURSEL, 0, 0);

    if (iCurSel == CB_ERR && m_spInfo != 0)
    {
        unsigned int uiIdx = 
            m_spScriptSet->GetScriptIndex(m_spInfo->GetName());
        if (uiIdx != NIPT_INVALID_INDEX)
            iCurSel = (int) uiIdx;
    }

    SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);
    for (unsigned int ui = 0; ui < m_spScriptSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = m_spScriptSet->GetScriptAt(ui);
        if (pkInfo)
        {   
            NiString strScriptName = pkInfo->GetName();
            if (!m_spScriptSet->IsTemplate(pkInfo))
            {
                strScriptName.Concatenate(" ");
            }
            else
            {
                strScriptName += "   (";
                NiString strFileName = pkInfo->GetFile();
                int i = strFileName.FindReverse('\\');
                strScriptName += strFileName.GetSubstring(i + 1, 
                    strFileName.Length());
                strScriptName +=  ")";
            }

            if (pkInfo->IsDirty())
                strScriptName += " *";

            SendMessage(hWndCombo, CB_ADDSTRING, 0, 
                (LPARAM) (const char*)strScriptName);
        }
    }

    if (iCurSel == CB_ERR)
        iCurSel = 0;

    SendMessage(hWndCombo, CB_SETCURSEL , (WPARAM) iCurSel, 0);
    
    if(!m_spOriginalInfo)
    {
        m_spOriginalInfo = m_spScriptSet->GetScriptAt(
            (unsigned int) iCurSel);
        m_spInfo = m_spOriginalInfo->Clone();
    }

    PopulatePluginsList(IDC_PLUGIN_EDIT_LIST, m_spInfo);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::PopulatePluginsList(int iListID, 
    NiScriptInfo* pkInfo)
{
    NiWindowRef hWndList = GetDlgItem(m_hWnd, iListID);
    
    int iReturn = (int)SendMessage(hWndList, LB_GETCURSEL, 0,0);
    if (iReturn == LB_ERR)
        iReturn = 0;

    SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
    for (unsigned int ui = 0; ui < pkInfo->GetPluginInfoCount(); ui++)
    {
        NiPluginInfo* pkPlugin = pkInfo->GetPluginInfoAt(ui);
        if (pkPlugin)
        {
            NiString strPluginInfo = MakePluginString(pkPlugin);
            SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM) 
                       (const char*) strPluginInfo);
        }
    }

    SendMessage(hWndList, LB_SETCURSEL, (WPARAM) iReturn,0);
}
//---------------------------------------------------------------------------
NiString NiScriptManagementDialog::MakePluginString(NiPluginInfo* pkInfo)
{
    return pkInfo->GetName();
    // + " (VERSION = \"" + pkInfo->GetVersion() + "\" TYPE = \"" + 
        //pkInfo->GetType() + "\")";
}
//---------------------------------------------------------------------------
bool NiScriptManagementDialog::SaveScript(NiScriptInfo* pkInfo, 
    bool bPromptForLocation)
{
    if (bPromptForLocation == true && pkInfo->GetFile() == "")
        bPromptForLocation = false;

    ReturnCode eCode = NiScriptInfoDialogs::DoScriptSaveDialog(pkInfo, 
        m_hWnd, bPromptForLocation);
    if (eCode == SUCCESS)
    {
        m_spScriptSet->SetTemplate(pkInfo, true);
        AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
    }

    return (eCode == SUCCESS);
}
//---------------------------------------------------------------------------
bool NiScriptManagementDialog::RenameScript(NiScriptInfo* pkInfo)
{
    
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;

    bool bContinue = false;

    int iReturn = IDOK;
    while (!bContinue)
    {
        NiString strTitle("Script Name Change");
        NiString strName(pkInfo->GetName());
        NiScriptNameChangeDialog kDialog(kModuleRef, m_hWnd, strName, 
            strTitle);
        iReturn = kDialog.DoModal();
        if (iReturn == IDOK)
        {
            NiString strName(kDialog.m_pcName);

            if (m_spScriptSet->GetScriptIndex(strName) != NIPT_INVALID_INDEX)
            {
                 NiMessageBox("A Script of the same name already exists! "
                    "Try again.", "Script Name Conflict");
            }
            else
            {
                pkInfo->SetName(strName);
                AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
                bContinue = true;
            }
        }
        else
        {
            break;
        }
    }

    return (iReturn == IDOK);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::EditPlugin(NiScriptInfo* pkInfo)
{
    NiPluginManager* pkPluginManager = NiPluginManager::GetInstance();
    NiWindowRef hWndList = GetDlgItem(m_hWnd, IDC_PLUGIN_EDIT_LIST);  
    
    int iReturn = (int)SendMessage(hWndList, LB_GETCURSEL, 0,0);
    if (iReturn == LB_ERR)
        return;

    NiPluginInfo* pkPluginInfo = pkInfo->GetPluginInfoAt(iReturn);

    if (!pkPluginInfo)
        return;

    NiPluginInfo* pkClonedOriginal = pkPluginInfo->Clone();
    bool bOriginalDirtiness = pkPluginInfo->IsDirty();
    pkPluginManager->DoManagementDialog(pkPluginInfo, m_hWnd);

    if (pkClonedOriginal->Equals(pkPluginInfo))
        pkPluginInfo->MarkAsDirty(bOriginalDirtiness);

    if (pkPluginInfo->IsDirty())
        AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::AddPlugin(NiScriptInfo* pkInfo)
{
    NiWindowRef hWndList = GetDlgItem(m_hWnd, IDC_KNOWN_PLUGINS_LIST); 
    
    int acSelections[512];
    int iNumItems = (int)SendMessage(hWndList, LB_GETSELITEMS, 512, 
        (LPARAM)&acSelections);

    if (iNumItems == LB_ERR)
        return;

    for(unsigned int ui = 0; ui < (unsigned int) iNumItems; ui++)
    {
        NiPlugin* pkPlugin = (NiPlugin*) SendMessage(hWndList, 
            LB_GETITEMDATA, acSelections[ui], 0);
        if (pkPlugin && pkPlugin->CanCreateDefaultInstance())
        {
            NiPluginInfo* pkPluginInfo = pkPlugin->GetDefaultPluginInfo();
            pkInfo->AddPluginInfo(pkPluginInfo);
            AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
        }
    }

    PopulatePluginsList(IDC_PLUGIN_EDIT_LIST, pkInfo);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::RemovePlugin(NiScriptInfo* pkInfo)
{
    NiWindowRef hWndList = GetDlgItem(m_hWnd, IDC_PLUGIN_EDIT_LIST);
   
    int iReturn = (int)SendMessage(hWndList, LB_GETCURSEL, 0, 0);

    if (iReturn == LB_ERR || pkInfo == NULL)
        return;

    pkInfo->RemovePluginInfoAt(iReturn);
    AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
    PopulatePluginsList(IDC_PLUGIN_EDIT_LIST, m_spInfo);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::MovePluginUp(NiScriptInfo* pkInfo)
{
    NiWindowRef hWndList = GetDlgItem(m_hWnd, IDC_PLUGIN_EDIT_LIST);
    
    int iReturn = (int)SendMessage(hWndList, LB_GETCURSEL, 0, 0);

    if (iReturn == LB_ERR || pkInfo == NULL)
        return;

    NiPluginInfo* pkPlugin = pkInfo->GetPluginInfoAt((unsigned int) iReturn);
    pkInfo->MovePluginInfoToPosition(pkPlugin, iReturn - 1);
    SendMessage(hWndList, LB_SETCURSEL, (WPARAM) iReturn - 1,0);
    AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
    PopulatePluginsList(IDC_PLUGIN_EDIT_LIST, pkInfo);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::MovePluginDown(NiScriptInfo* pkInfo)
{
    NIASSERT(pkInfo);
    NiWindowRef hWndList = GetDlgItem(m_hWnd, IDC_PLUGIN_EDIT_LIST);

    int iReturn = (int)SendMessage(hWndList, LB_GETCURSEL, 0, 0);

    if (iReturn == LB_ERR || pkInfo == NULL)
        return;

    NiPluginInfo* pkPlugin = pkInfo->GetPluginInfoAt((unsigned int) iReturn);
    pkInfo->MovePluginInfoToPosition(pkPlugin, (unsigned int) (iReturn + 1));
    SendMessage(hWndList, LB_SETCURSEL, (WPARAM) iReturn + 1,0);
    AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
    PopulatePluginsList(IDC_PLUGIN_EDIT_LIST, pkInfo);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::ScriptChange()
{
    NiPluginManager* pkPluginManager = NiPluginManager::GetInstance();
    int i = (int)SendDlgItemMessage(m_hWnd, IDC_SCRIPT_EDIT_COMBO, 
        CB_GETCURSEL, 0, 0);

    if (i == CB_ERR)
        return;

    NiScriptInfo* pkInfo = m_spScriptSet->GetScriptAt((unsigned int)i);
    NIASSERT(m_spInfo);

    if (!pkInfo)
    {
        MessageBox(m_hWnd, "The script you selected does not exist.",
            "Invalid Script", MB_OK);
    }
    else if (!pkPluginManager->VerifyScriptInfo(pkInfo))
    {
        MessageBox(m_hWnd, "The script you selected uses unknown plugins.\n"
            "Please edit the script to use known plugins.", "Invalid Script", 
            MB_OK);
    }
    m_spInfo = pkInfo;

    if (m_spInfo)
    {
        PopulatePluginsList(IDC_PLUGIN_EDIT_LIST, m_spInfo);
        AddTypesToComboBox(IDC_TYPE_COMBO);
    }
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::RemoveScript()
{
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    int iReturn = IDYES;

    iReturn = MessageBox(m_hWnd,
        "Are you sure you want to permanently delete this script?",
        "Delete Script", MB_YESNO);

    if (iReturn == IDYES)
    {
        if (m_spInfo->GetFile() != NULL)
        {    
            _unlink((const char*) m_spInfo->GetFile());
            m_spRemovedScripts->AddScript(m_spInfo);
        }
    }
    else
    {
        return;
    }

    pkManager->RemoveScript(m_spInfo->GetName());
    unsigned int ui = m_spScriptSet->GetScriptIndex(m_spInfo->GetName());
    m_spScriptSet->RemoveScript(m_spInfo->GetName());

    m_spInfo = m_spScriptSet->GetScriptAt(ui);
    while (!m_spInfo)
        m_spInfo = m_spScriptSet->GetScriptAt(--ui);
    
    // The original script has been deleted - there is no cancelling this
    // operation so the original script data must be updated.
    m_spOriginalInfo->CopyMembers(m_spInfo);

    SendDlgItemMessage(m_hWnd, IDC_SCRIPT_EDIT_COMBO, CB_SETCURSEL, 
        (WPARAM)ui, 0);

    AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::AddTypesToComboBox(int iComboBoxID)
{
    NiWindowRef hWndCombo = GetDlgItem(m_hWnd, iComboBoxID);
    int iSel = (int)SendDlgItemMessage(m_hWnd, iComboBoxID, 
        LB_GETCURSEL, 0, 0);
    NiString strCurType =  m_kScriptTypes.GetAt(0);

    if (m_spInfo && iSel != CB_ERR)
    {
        strCurType = m_kScriptTypes.GetAt(iSel);
    }

    SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);
    NiStringTokenizer kTok(m_strTypes);

    for (unsigned int ui = 0; ui < m_kScriptTypes.GetSize(); ui++)
    {
        NiString strTok = m_kScriptTypes.GetAt(ui);
        SendMessage(hWndCombo, CB_ADDSTRING, 0, 
            (LPARAM) (const char*) strTok);

        if (strTok.EqualsNoCase(m_spInfo->GetType()))
            strCurType = strTok;
    }

    SendMessage(hWndCombo, CB_SELECTSTRING , (WPARAM)-1, 
        (LPARAM)(const char*)strCurType);
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::AddScript()
{
    NiScriptInfo* pkInfo = 
        NiScriptInfoDialogs::DoScriptOpenDialog(m_hWnd, false);

    if (pkInfo)
    {
        NiString strType = pkInfo->GetType();
        bool bFound = false;
        for (unsigned int ui = 0; ui < m_kScriptTypes.GetSize(); ui++)
        {
            NiString strFoundType = m_kScriptTypes.GetAt(ui);
            if (strFoundType.EqualsNoCase(strType))
                bFound = true;
        }

        if (!bFound)
        {
            NiString strMessage = "The type of this script, \"" + strType +
                "\", does not match the types for this dialog.\n"
                "The script will not be added to the dialog.";
            NiMessageBox(strMessage, "Script Type Mismatch");
            return;
        }

        int iReturn = IDYES;
        NiScriptInfoPtr spExisting = m_spScriptSet->GetScript(
            pkInfo->GetName());
        
        if (spExisting != NULL)
        {
            NiString strMessage = "The name conflicts with a known script.\n"
                "Should we replace the old script?";
            iReturn = 
                MessageBox(m_hWnd, strMessage, "Load template?", MB_YESNO);
        }

        if (iReturn == IDYES)
        {
            m_spScriptSet->ReplaceScript(pkInfo);
            if (pkInfo->GetName() == m_spInfo->GetName())
            {
                m_spInfo = pkInfo;
            }
            AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
            SendDlgItemMessage(m_hWnd, IDC_SCRIPT_EDIT_COMBO, CB_SETCURSEL, 
                m_spScriptSet->GetScriptCount() -1, 0);
            ScriptChange();
        }
    }
}
//---------------------------------------------------------------------------
static unsigned int gs_uiNewScriptIndex = 0;
//---------------------------------------------------------------------------
void NiScriptManagementDialog::NewScriptFromTemplate()
{
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;

    bool bSuccess = false;
    while (!bSuccess)
    {
        gs_uiNewScriptIndex++;
        NiString strTitle("New Script");
        NiString strName("New Script " + 
            NiString::FromInt(gs_uiNewScriptIndex));

        int iReturn;
        NiScriptNewFromTemplateDialog kDialog(kModuleRef, m_hWnd, strName, 
            strTitle, m_spScriptSet);
        iReturn = kDialog.DoModal();
    
        if (iReturn == IDOK)
        {
            NiScriptInfo* pkInfo = kDialog.GenerateNewScript();
            if (pkInfo)
                pkInfo->SetType((const char*)m_kScriptTypes.GetAt(0));
            ReturnCode eReturn = m_spScriptSet->AddScript(pkInfo, false);
            
            if(eReturn == SUCCESS)
            {
                m_spInfo = pkInfo;
                AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
                SendDlgItemMessage(m_hWnd, IDC_SCRIPT_EDIT_COMBO, CB_SETCURSEL,
                    m_spScriptSet->GetScriptCount() -1, 0);
                ScriptChange();
                return;
            }
            else
            {
                NiMessageBox(
                    "A Script of the same name already exists! Try again.",
                    "Script Name Conflict");
                NiDelete pkInfo;
            }
        }
        else
            return;
    }
}
//---------------------------------------------------------------------------

void NiScriptManagementDialog::NewBlankScript()
{
    NiModuleRef kModuleRef = gs_hNiPluginToolkitHandle;

    bool bSuccess = false;
    while (!bSuccess)
    {
        gs_uiNewScriptIndex++;
        NiString strTitle("New Script");
        NiString strName("New Script " + 
            NiString::FromInt(gs_uiNewScriptIndex));

        int iReturn;
        NiScriptNameChangeDialog kDialog(kModuleRef, m_hWnd, strName, 
            strTitle);
        iReturn = kDialog.DoModal();
        
        if (iReturn == IDOK)
        {
            NiScriptInfo* pkInfo = 
                NiScriptTemplateManager::GetInstance()->CreateNewScript();
            pkInfo->SetName(NiString(kDialog.m_pcName));

            if (m_spInfo)
            {
                NiString strCurType = m_spInfo->GetType();
                pkInfo->SetType(strCurType);
            }
            else
            {
                pkInfo->SetType((const char*)m_kScriptTypes.GetAt(0));
            }

            pkInfo->MarkAsDirty(true);

            ReturnCode eReturn = m_spScriptSet->AddScript(pkInfo, false);
            
            if(eReturn == SUCCESS)
            {
                m_spInfo = pkInfo;
                AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
                SendDlgItemMessage(m_hWnd, IDC_SCRIPT_EDIT_COMBO, CB_SETCURSEL,
                    m_spScriptSet->GetScriptCount() -1, 0);
                ScriptChange();
                return;
            }
            else
            {
                NiMessageBox(
                    "A Script of the same name already exists! Try again.",
                    "Script Name Conflict");
                NiDelete pkInfo;
            }
        }
        else
            return;
    }
}
//---------------------------------------------------------------------------

void NiScriptManagementDialog::NewScript(bool bFromTemplate)
{
    if (bFromTemplate)
        NewScriptFromTemplate();
    else
        NewBlankScript();

}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::PromptForSave(NiScriptInfo* pkScript)
{
    if (pkScript->IsDirty() && m_spScriptSet->IsTemplate(pkScript))
    {
        char acString[512];
        NiSprintf(acString, 512, 
            "The template script \"%s\" has changed from the "
            "version on disk.\nDo you want to commit these changes "
            "into the template?"
            "\nNOTE: Choosing 'No' means that the changes will only "
            "be used for"
            " the active file.", (const char*) pkScript->GetName());
        int iReturn = MessageBox(m_hWnd, acString, 
            "Script Has Changed", MB_YESNO);
        if (iReturn == IDYES)
        {
            iReturn = MessageBox(NULL, 
                "Are you sure that you want to overwrite the old "
                "template?\n"
                "Clicking 'No' will force you to rename this template "
                "script.",
                "Template Overwrite Warning",
                MB_YESNO | MB_ICONQUESTION
                );

            bool bPrompt = false;

            if (iReturn == IDNO)
            {
                if (!RenameScript(pkScript))
                {
                    PromptForSave(pkScript);
                    return;
                }

                pkScript->SetFile("");
                bPrompt = true;
            }

            if (!SaveScript(pkScript, bPrompt))
            {
                PromptForSave(pkScript);
                return;
            }
        }
        else
        {
            NiString strPrepend = "Modified ";
            NiString strNewName = pkScript->GetName();
            NiString strOldLeft = strNewName.Left(strPrepend.Length());
            NiString strDigits = strNewName.Right(3);
            int i;
            bool bHasEnd = strDigits.ToInt(i);
            if (!strOldLeft.EqualsNoCase(strPrepend))
                strNewName = strPrepend + strNewName;

            if (bHasEnd)
            {
                strNewName = strNewName.Left(strNewName.Length() - 3);
                i++;
                if (i - 10 < 0)
                    strNewName = strNewName + "00";
                else if (i - 100 < 0)
                    strNewName = strNewName = "0";
                NIASSERT(i - 1000 < 0);
                strNewName = strNewName + NiString::FromInt(i);
            }
            else
            {
                strNewName = strNewName + " 000";
            }

            pkScript->SetName(strNewName);
            pkScript->SetFile("");
            pkScript->MarkAsDirty(false);
            m_spScriptSet->SetTemplate(pkScript, false);
        }

    }
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::PromptForSave()
{
    for (unsigned int ui = 0; ui < m_spScriptSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkScript = m_spScriptSet->GetScriptAt(ui);
        PromptForSave(pkScript);
    }

    PromptForSave(m_spInfo);

    AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
}

//---------------------------------------------------------------------------
void NiScriptManagementDialog::AddPluginTypesToTabControl(int iTabID)
{
   NiWindowRef hWndTab = GetDlgItem(m_hWnd, iTabID);
   TabCtrl_DeleteAllItems(hWndTab);

   int iCurSel = 0;
   for (int iItem = 0; iItem < NUM_TABS; iItem++)
   {
       LPTCITEM pitem = NiExternalNew TCITEM;
       pitem->mask = TCIF_TEXT;
       switch ((TabOption)iItem)
       {
           case ALL_PLUGINS:
                pitem->pszText = "All";
                pitem->cchTextMax = 3;
                if ((m_kScriptTypes.GetAt(0)).EqualsNoCase(pitem->pszText))
                    iCurSel = iItem;
                break;
            case IMPORT_PLUGINS:
                pitem->pszText = "Import";
                pitem->cchTextMax = 6;
                if ((m_kScriptTypes.GetAt(0)).EqualsNoCase(pitem->pszText))
                    iCurSel = iItem;
                break;
            case EXPORT_PLUGINS:
                pitem->pszText = "Export";
                pitem->cchTextMax = 6;              
                if ((m_kScriptTypes.GetAt(0)).EqualsNoCase(pitem->pszText))
                    iCurSel = iItem;
                break;
            case PROCESSOR_PLUGINS:
                pitem->pszText = "Process";
                pitem->cchTextMax = 7;              
                if ((m_kScriptTypes.GetAt(0)).EqualsNoCase(pitem->pszText))
                    iCurSel = iItem;
                break;
            case VIEWER_PLUGINS:
                pitem->pszText = "Viewer";
                pitem->cchTextMax = 6;              
                if ((m_kScriptTypes.GetAt(0)).EqualsNoCase(pitem->pszText))
                    iCurSel = iItem;
                break;
       }
       TabCtrl_InsertItem(hWndTab, iItem, pitem);
       NiExternalDelete pitem;
   }

   TabCtrl_SetCurSel(hWndTab, iCurSel);
   m_eTabOption = (TabOption) iCurSel;
    
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::AddPluginsToKnownPluginsList(int iListID)
{
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    NiWindowRef hWndList = GetDlgItem(m_hWnd, iListID);

    SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
    
    for (unsigned int ui = 0; ui < pkManager->GetTotalPluginCount();
        ui++)
    {
        NiPlugin* pkPlugin = pkManager->GetPluginAt(ui);
        if (pkPlugin)
        {
            if (!pkPlugin->CanCreateDefaultInstance())
                continue;

            NiString strId = pkPlugin->GetName();
            if (strId.IsEmpty())
            {
                strId.Concatenate(pkPlugin->GetRTTI()->GetName());
            }

            strId += "  v" + pkPlugin->GetVersion();
        
            if(pkPlugin->GetVersion().IsEmpty())
                strId += "????";
        
            bool bAdd = false;

            switch (m_eTabOption)
            {
                case ALL_PLUGINS:
                    if (!NiIsKindOfStringMatch("NiMFCPlugin", 
                        pkPlugin->GetRTTI()))
                        bAdd = true;
                    break;
                case IMPORT_PLUGINS:
                    if (NiIsKindOfStringMatch("NiImportPlugin", 
                        pkPlugin->GetRTTI()))
                        bAdd = true;
                    break;
                case EXPORT_PLUGINS:
                    if (NiIsKindOfStringMatch("NiExportPlugin", 
                        pkPlugin->GetRTTI()))
                        bAdd = true;
                    break;
                case PROCESSOR_PLUGINS:
                    if (!NiIsKindOfStringMatch("NiMFCPlugin", 
                            pkPlugin->GetRTTI()) &&
                        !NiIsKindOfStringMatch("NiImportPlugin", 
                            pkPlugin->GetRTTI()) &&
                        !NiIsKindOfStringMatch("NiExportPlugin", 
                            pkPlugin->GetRTTI()) &&
                        !NiIsKindOfStringMatch("NiViewerPlugin", 
                            pkPlugin->GetRTTI()))
                        bAdd = true;
                    break;
                case VIEWER_PLUGINS:
                    if (NiIsKindOfStringMatch("NiViewerPlugin", 
                        pkPlugin->GetRTTI()))
                        bAdd = true;
                    break;
            };
            if (bAdd)
            {
                int iId = (int)SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)
                           (const char*) strId);
                LPARAM lParam = reinterpret_cast<LPARAM>(pkPlugin);
                SendMessage(hWndList, LB_SETITEMDATA, iId, lParam);
            }
        }
    }

    SendMessage(hWndList, LB_SETCURSEL, (WPARAM)-1, 0);
}
//---------------------------------------------------------------------------

void NiScriptManagementDialog::ViewDescription()
{
    int acSelections[512];
    HWND hWndList = GetDlgItem(m_hWnd, IDC_KNOWN_PLUGINS_LIST);
    int iNumItems = (int)SendMessage(hWndList, LB_GETSELITEMS, 512, 
        (LPARAM)&acSelections);

    if (iNumItems == LB_ERR)
        return;

    for(unsigned int ui = 0; ui < (unsigned int) iNumItems; ui++)
    {
        NiPlugin* pkPlugin = (NiPlugin*)
            SendMessage(hWndList, LB_GETITEMDATA, acSelections[ui], 0);

        if (pkPlugin && !pkPlugin->GetLongDescription().IsEmpty())
        {
            MessageBox(hWndList, pkPlugin->GetLongDescription(), 
                pkPlugin->GetName(), MB_OK);
        }    
    }
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::VerifyStates()
{
    NiPluginManager* pkPluginManager = NiPluginManager::GetInstance();
    
    NiWindowRef hWndItem = GetDlgItem(m_hWnd, IDC_PLUGIN_EDIT_LIST);
    int iReturn = (int)SendMessage(hWndItem, LB_GETCURSEL, 0, 0);
    if ( iReturn == LB_ERR)
    {
        hWndItem = GetDlgItem(m_hWnd, IDC_EDIT_PLUGIN);
        EnableWindow(hWndItem, SW_HIDE);
        hWndItem = GetDlgItem(m_hWnd, IDC_REMOVE_PLUGIN);
        EnableWindow(hWndItem, SW_HIDE);
        hWndItem = GetDlgItem(m_hWnd, IDC_MOVE_PLUGIN_UP);
        EnableWindow(hWndItem, SW_HIDE);
        hWndItem = GetDlgItem(m_hWnd, IDC_MOVE_PLUGIN_DOWN);
        EnableWindow(hWndItem, SW_HIDE);

    }
    else
    {
        hWndItem = GetDlgItem(m_hWnd, IDC_REMOVE_PLUGIN);
        EnableWindow(hWndItem, SW_SHOW);

        hWndItem = GetDlgItem(m_hWnd, IDC_MOVE_PLUGIN_UP);
        
        if (iReturn == 0)
            EnableWindow(hWndItem, SW_HIDE);
        else
            EnableWindow(hWndItem, SW_SHOW);
        
        hWndItem = GetDlgItem(m_hWnd, IDC_MOVE_PLUGIN_DOWN);
        if (iReturn == (int)m_spInfo->GetPluginInfoCount() - 1)
            EnableWindow(hWndItem, SW_HIDE);
        else
            EnableWindow(hWndItem, SW_SHOW);
        
    }

    hWndItem = GetDlgItem(m_hWnd, IDC_KNOWN_PLUGINS_LIST);
    iReturn = (int)SendMessage(hWndItem, LB_GETSELCOUNT, 0, 0);
    if (iReturn == 0)
    {
        hWndItem = GetDlgItem(m_hWnd, IDC_ADD_PLUGIN);
        EnableWindow(hWndItem, SW_HIDE);
        hWndItem = GetDlgItem(m_hWnd, IDC_PLUGIN_DESCRIPTION);
        EnableWindow(hWndItem, SW_HIDE);
    }
    else
    {
        hWndItem = GetDlgItem(m_hWnd, IDC_ADD_PLUGIN);
        EnableWindow(hWndItem, SW_SHOW);
        hWndItem = GetDlgItem(m_hWnd, IDC_PLUGIN_DESCRIPTION);
        HWND hWndList = GetDlgItem(m_hWnd, IDC_KNOWN_PLUGINS_LIST);
        bool bShow = false;
        int acSelections[512];
        int iNumItems = (int)SendMessage(hWndList, LB_GETSELITEMS, 512, 
            (LPARAM)&acSelections);

        for(unsigned int ui = 0; ui < (unsigned int) iNumItems; ui++)
        {
            NiPlugin* pkPlugin = (NiPlugin*)
                SendMessage(hWndList, LB_GETITEMDATA, acSelections[ui], 0);
            if (pkPlugin && !pkPlugin->GetLongDescription().IsEmpty())
                bShow = true;
        }
    
        if (bShow)
            EnableWindow(hWndItem, SW_SHOW);
        else
            EnableWindow(hWndItem, SW_HIDE);
    }


    hWndItem = GetDlgItem(m_hWnd, IDC_PLUGIN_EDIT_LIST);

    iReturn = (int)SendMessage(hWndItem, LB_GETCURSEL, 0,0);
    hWndItem = GetDlgItem(m_hWnd, IDC_EDIT_PLUGIN);

    if (iReturn == LB_ERR)
    {
        EnableWindow(hWndItem, SW_HIDE);
    }
    else
    {
        NiPluginInfo* pkPluginInfo = m_spInfo->GetPluginInfoAt(iReturn);

        if (!pkPluginInfo)
        {
            EnableWindow(hWndItem, SW_HIDE);
        }
        else
        {
            bool bShow = pkPluginManager->HasManagementDialog(pkPluginInfo);

            if (bShow)
                EnableWindow(hWndItem, SW_SHOW);
            else
                EnableWindow(hWndItem, SW_HIDE);
        }
    }

}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::InitDialog()
{
    NiStringTokenizer kTok(m_strTypes);

    while (!kTok.IsEmpty())
    {
        NiString strType = kTok.GetNextToken("\n,\t");
        if (!strType.IsEmpty())
            m_kScriptTypes.Add(strType);
    }

    NiTObjectArray<NiScriptInfoPtr> kInvalidScripts;

    unsigned int ui = 0;
    for (; ui < m_spScriptSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = m_spScriptSet->GetScriptAt(ui);
        if (pkInfo)
        {
            NiString strType = pkInfo->GetType();
            bool bNotFoundMatch = true;

            for (unsigned int uj = 0; uj < m_kScriptTypes.GetSize() && 
                bNotFoundMatch; uj++)
            {
                NiString strTestType = m_kScriptTypes.GetAt(uj);
                if (strType.EqualsNoCase(strTestType))
                    bNotFoundMatch = false;
                    
            }

            if (bNotFoundMatch)
                kInvalidScripts.Add(pkInfo);
        }
    }

    for (ui = 0; ui < kInvalidScripts.GetSize(); ui++)
    {
        NiScriptInfoPtr spInfo = kInvalidScripts.GetAt(ui);
        if (spInfo)
        {
            m_spScriptSet->RemoveScript(spInfo->GetName());
        }
    }

    
    AddPluginTypesToTabControl(IDC_KNOWNPLUGINTAB);
    AddScriptsToComboBox(IDC_SCRIPT_EDIT_COMBO);
    AddPluginsToKnownPluginsList(IDC_KNOWN_PLUGINS_LIST);
    VerifyStates();
    AddTypesToComboBox(IDC_TYPE_COMBO);
}

//---------------------------------------------------------------------------
BOOL NiScriptManagementDialog::OnCommand(int iWParamLow, int iWParamHigh, 
                                         long)
{
    switch(iWParamLow)
    {
        case IDOK:
            PromptForSave();
            m_spScriptSet->ReplaceScript(m_spInfo);
            m_idReturnState = IDOK;
            EndDialog(m_hWnd, IDOK);
            break;
        case IDCANCEL:
            m_idReturnState = IDCANCEL;
            EndDialog(m_hWnd, IDCANCEL);
            break;
        case IDC_NEW_SCRIPT:
            NewScript(false);
            break;
        case IDC_NEW_FROM_TEMPLATE:
            NewScript(true);
            break;
        case IDC_ADD_SCRIPT:
            AddScript();
            VerifyStates();
            break;
        case IDC_REMOVE_SCRIPT:
            RemoveScript();
            VerifyStates();
            break;
        case IDC_SAVE_SCRIPT:
            SaveScript(m_spInfo, false);
            VerifyStates();
            break;
        case IDC_SAVE_SCRIPT_AS:
            SaveScript(m_spInfo, true);
            VerifyStates();
            break;
        case IDC_RENAME_SCRIPT:
            RenameScript(m_spInfo);
            VerifyStates();
            break;
        case IDC_EDIT_PLUGIN:
            EditPlugin(m_spInfo);
            VerifyStates();
            break;
        case IDC_ADD_PLUGIN:
            AddPlugin(m_spInfo);
            SendDlgItemMessage(m_hWnd, IDC_KNOWN_PLUGINS_LIST, 
                    LB_SETSEL, FALSE, -1);
            VerifyStates();
            break;
        case IDC_REMOVE_PLUGIN:
            RemovePlugin(m_spInfo);
            SendDlgItemMessage(m_hWnd, IDC_KNOWN_PLUGINS_LIST, 
                    LB_SETSEL, FALSE, -1);
            VerifyStates();
            break;
        case IDC_MOVE_PLUGIN_UP:
            MovePluginUp(m_spInfo);
            VerifyStates();
            break;
        case IDC_MOVE_PLUGIN_DOWN:
            MovePluginDown(m_spInfo);
            VerifyStates();
            break;
        case IDC_PLUGIN_EDIT_LIST:
            if (iWParamHigh == LBN_DBLCLK)
                EditPlugin(m_spInfo);
            if(iWParamHigh == LBN_SETFOCUS)
            {
                SendDlgItemMessage(m_hWnd, IDC_KNOWN_PLUGINS_LIST, 
                    LB_SETSEL, FALSE, -1);
            }
            VerifyStates();
            break;
        case IDC_KNOWN_PLUGINS_LIST:
            if (iWParamHigh == LBN_DBLCLK)
                AddPlugin(m_spInfo);
            if(iWParamHigh == LBN_SETFOCUS)
            {
                SendDlgItemMessage(m_hWnd, IDC_PLUGIN_EDIT_LIST, 
                    LB_SETCURSEL, (WPARAM)-1, 0);
            }
            VerifyStates();
            break;
        case IDC_SCRIPT_EDIT_COMBO:
            if(iWParamHigh == CBN_SELCHANGE )
            {
                ScriptChange();
            }
            VerifyStates();
            break;
        case IDC_TYPE_COMBO:
            if(iWParamHigh == CBN_SELCHANGE )
            {
                int iSel = (int)SendDlgItemMessage(m_hWnd, IDC_TYPE_COMBO, 
                    LB_GETCURSEL, 0, 0);

                if (m_spInfo && iSel != CB_ERR)
                {
                    m_spInfo->
                        SetType((const char*)m_kScriptTypes.GetAt(iSel));
                    ScriptChange();
                }
            }
            break;
        case IDC_PLUGIN_DESCRIPTION:
            ViewDescription();
            break;
        case IDC_KNOWNPLUGINTAB:
            if (iWParamHigh == TCN_SELCHANGING)
                return TRUE;
            if (iWParamHigh == TCN_SELCHANGE  )
            {
                TabChange();
            }
            break;
        default:
            break;
    }
    return FALSE;
 }
//---------------------------------------------------------------------------
BOOL NiScriptManagementDialog::OnMessage(unsigned int uiMessage, 
    int, int, long lParam)
{
    if (uiMessage == WM_NOTIFY)
    {
        LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(LongToPtr(lParam));
        if (pnmh && pnmh->code == TCN_SELCHANGE)
        {
            TabChange();
        }
    }
    return FALSE;
}

//---------------------------------------------------------------------------
void NiScriptManagementDialog::OnDestroy()
{
    if (m_idReturnState != IDCANCEL)
    {
        PromptForSave();
    }
}
//---------------------------------------------------------------------------

void NiScriptManagementDialog::OnClose()
{
    if (m_idReturnState != IDCANCEL)
    {
        PromptForSave();
    }
}
//---------------------------------------------------------------------------
void NiScriptManagementDialog::TabChange()
{
    NiWindowRef hWndTab = GetDlgItem(m_hWnd, IDC_KNOWNPLUGINTAB);
    int iControl = TabCtrl_GetCurSel(hWndTab);
    if (iControl == -1)
    {
        m_eTabOption = ALL_PLUGINS;
    }
    else
    {
        m_eTabOption = (TabOption) iControl;
    }

    AddPluginsToKnownPluginsList(IDC_KNOWN_PLUGINS_LIST);
    VerifyStates();

}
//---------------------------------------------------------------------------
