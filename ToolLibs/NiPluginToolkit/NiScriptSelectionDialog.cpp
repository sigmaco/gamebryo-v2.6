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

#include "NiScriptSelectionDialog.h"
#include "NiScriptInfoDialogs.h"
#include "NiScriptTemplateManager.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "resource.h"
#include <NiSystem.h>
#include "NiStringTokenizer.h"
#include "NiPluginManager.h"

//---------------------------------------------------------------------------
NiScriptSelectionDialog::NiScriptSelectionDialog(NiModuleRef hInstance, 
    NiScriptInfo* pkInfo, NiScriptInfoSet* pkSet, NiWindowRef hWndParent,
    const char* pcTypes) : NiDialog(IDD_SCRIPT_SELECTOR, hInstance,hWndParent)
{
    ms_spScriptSet = pkSet;
    ms_spInfo = pkInfo;
    m_strTypes = pcTypes;
}

//---------------------------------------------------------------------------
NiScriptSelectionDialog::~NiScriptSelectionDialog()
{
    ms_spInfo = NULL;
    ms_spScriptSet = NULL;
}

//---------------------------------------------------------------------------
int NiScriptSelectionDialog::DoModal()
{
    int iResult = NiDialog::DoModal();
    return iResult;
}

//---------------------------------------------------------------------------
void NiScriptSelectionDialog::AddScriptsToComboBox(int iComboBoxID)
{
    NiWindowRef hWndCombo = GetDlgItem(m_hWnd, iComboBoxID);

    int iCurSel = (int)SendDlgItemMessage(m_hWnd, iComboBoxID, CB_GETCURSEL, 0, 0);

    if (iCurSel == CB_ERR && ms_spInfo != 0)
    {
        unsigned int uiIdx = 
            ms_spScriptSet->GetScriptIndex(ms_spInfo->GetName());
        if (uiIdx != NIPT_INVALID_INDEX)
            iCurSel = (int) uiIdx;
    }

    SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);
    for (unsigned int ui = 0; ui < ms_spScriptSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = ms_spScriptSet->GetScriptAt(ui);
        if (pkInfo)
        {   
            SendMessage(hWndCombo, CB_ADDSTRING, 0, 
                (LPARAM) (const char*)pkInfo->GetName());
        }
    }

    if (iCurSel == CB_ERR)
        iCurSel = 0;

    SendMessage(hWndCombo, CB_SETCURSEL , (WPARAM) iCurSel, 0);
    ms_spInfo = ms_spScriptSet->GetScriptAt(
        (unsigned int) iCurSel);


    if (ms_spInfo)
    {
        PopulatePluginsList(IDC_PLUGIN_SELECT_LIST, ms_spInfo);
    }

}

//---------------------------------------------------------------------------
void NiScriptSelectionDialog::PopulatePluginsList(int iListID, 
    NiScriptInfo* pkInfo)
{
    NiWindowRef hWndList = GetDlgItem(m_hWnd, iListID);
    
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
    
}
//---------------------------------------------------------------------------
NiString NiScriptSelectionDialog::MakePluginString(NiPluginInfo* pkInfo)
{
    NiPluginManager* pkManager = NiPluginManager::GetInstance();
    NiPlugin* pkPlugin = pkManager->GetPluginByClass((const char*)
        pkInfo->GetClassName());
    if (pkPlugin && !pkPlugin->GetShortDescription().IsEmpty())
        return pkInfo->GetName() + " - " + pkPlugin->GetShortDescription();
    else
        return pkInfo->GetName();
}
//---------------------------------------------------------------------------
void NiScriptSelectionDialog::InitDialog()
{
    NiStringTokenizer kTok(m_strTypes);
    NiStringArray kScriptTypes;

    while (!kTok.IsEmpty())
    {
        NiString strType = kTok.GetNextToken("\n,\t");
        if (!strType.IsEmpty())
            kScriptTypes.Add(strType);
    }

    NiTObjectArray<NiScriptInfoPtr> kInvalidScripts;

    unsigned int ui = 0;
    for (; ui < ms_spScriptSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = ms_spScriptSet->GetScriptAt(ui);
        if (pkInfo)
        {
            NiString strType = pkInfo->GetType();
            bool bNotFoundMatch = true;

            for (unsigned int uj = 0; uj < kScriptTypes.GetSize() && 
                bNotFoundMatch; uj++)
            {
                NiString strTestType = kScriptTypes.GetAt(uj);
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
            ms_spScriptSet->RemoveScript(spInfo->GetName());
        }
    }

    AddScriptsToComboBox(IDC_SCRIPT_SELECT_COMBO);
}

//---------------------------------------------------------------------------
BOOL NiScriptSelectionDialog::OnCommand(int iWParamLow, int iWParamHigh, 
    long)
{
    switch(iWParamLow)
    {
        case IDOK:
            EndDialog(m_hWnd, IDOK);
            break;
        case IDCANCEL:
            ms_spInfo = NULL;
            EndDialog(m_hWnd, IDCANCEL);
            break;
        case IDC_SCRIPT_EDITOR:
            {
                NiScriptInfoPtr spInfo = 
                    NiScriptInfoDialogs::DoManagementDialog(ms_spInfo, 
                    ms_spScriptSet, m_hWnd, m_strTypes);
                
                AddScriptsToComboBox(IDC_SCRIPT_SELECT_COMBO);
                if (spInfo)
                {
                    int iCurSel = 
                        ms_spScriptSet->GetScriptIndex(spInfo->GetName());
                    SendDlgItemMessage(m_hWnd, IDC_SCRIPT_SELECT_COMBO,
                        CB_SETCURSEL , (WPARAM) iCurSel, 0);
                    ms_spInfo = spInfo;
                    PopulatePluginsList(IDC_PLUGIN_SELECT_LIST, ms_spInfo);
                }
            }
            break;
        case IDC_SCRIPT_SELECT_COMBO:
            if(iWParamHigh == CBN_SELCHANGE )
            {
                int i = (int)SendDlgItemMessage(m_hWnd, IDC_SCRIPT_SELECT_COMBO, 
                    CB_GETCURSEL, 0, 0);
                if (i == CB_ERR)
                    break;

                NiScriptInfo* pkInfo = ms_spScriptSet->GetScriptAt(
                    (unsigned int) i);

                if (pkInfo && NiPluginManager::GetInstance()->
                    VerifyScriptInfo(pkInfo))
                {
                    ms_spInfo = pkInfo;
                }
                else
                {
                    MessageBox(m_hWnd, 
                        "The script you selected uses unknown plugins.\n"
                        "Switching to previous script.", "Invalid Script", 
                        MB_OK);
                    int iIdx = CB_ERR;
                    if (ms_spInfo)
                    {
                        iIdx = (int) ms_spScriptSet->
                            GetScriptIndex(ms_spInfo->GetName());
                    }
                    SendDlgItemMessage(m_hWnd, IDC_SCRIPT_SELECT_COMBO, 
                        CB_SETCURSEL, iIdx, 0);
                }

                if (ms_spInfo)
                {
                    PopulatePluginsList(IDC_PLUGIN_SELECT_LIST, ms_spInfo);
                }
            }
            break;
        default:
            break;
    }
            
    return FALSE;
}
