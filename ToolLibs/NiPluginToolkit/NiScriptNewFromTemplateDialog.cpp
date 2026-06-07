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
#include "NiScriptNewFromTemplateDialog.h"
#include "NiScriptInfoDialogs.h"
#include "NiScriptTemplateManager.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "resource.h"
#include <NiSystem.h>
#include "NiStringTokenizer.h"
#include "NiPluginManager.h"
//---------------------------------------------------------------------------
NiScriptNewFromTemplateDialog::NiScriptNewFromTemplateDialog(
    NiModuleRef hInstance, NiWindowRef m_hWnd, const char* pcName, 
    const char* pcTitle, NiScriptInfoSetPtr pkSet) :
    NiDialog(IDD_NEW_FROM_TEMPLATE,hInstance,m_hWnd)
{
    NIASSERT(pcName);
    NIASSERT(pcTitle);
    size_t stLength = strlen(pcName) + 1;
    m_pcName =  NiAlloc(char, stLength);
    NiSprintf(m_pcName, stLength, "%s", pcName);
    stLength = strlen(pcTitle) + 1;
    m_pcTitle = NiAlloc(char, stLength);
    NiSprintf(m_pcTitle, stLength, "%s", pcTitle);
    m_spScriptSet = pkSet;
}

//---------------------------------------------------------------------------
NiScriptNewFromTemplateDialog::~NiScriptNewFromTemplateDialog()
{
    NiFree(m_pcName);
    NiFree(m_pcTitle);
    m_pcName = 0;
    m_pcTitle = 0;
    m_spScriptSet = NULL;
}
        
//---------------------------------------------------------------------------
int NiScriptNewFromTemplateDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void NiScriptNewFromTemplateDialog::AddScriptsToComboBox(int iComboBoxID)
{
    NiWindowRef hWndCombo = GetDlgItem(m_hWnd, iComboBoxID);

    int iCurSel = (int)SendDlgItemMessage(m_hWnd, iComboBoxID, CB_GETCURSEL, 0, 0);

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
                strScriptName += 
                    strFileName.GetSubstring(i + 1, strFileName.Length());
                strScriptName +=  ")";
            }
            SendMessage(hWndCombo, CB_ADDSTRING, 0, 
                (LPARAM) (const char*)strScriptName);
        }
    }

    if (iCurSel == CB_ERR)
        iCurSel = 0;

    SendMessage(hWndCombo, CB_SETCURSEL , (WPARAM) iCurSel, 0);
}
//---------------------------------------------------------------------------
void NiScriptNewFromTemplateDialog::InitDialog()
{
    SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) m_pcTitle);
    SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, WM_SETTEXT, 
        0, (LPARAM) m_pcName);
    SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, WM_SETFOCUS,
        0, (LPARAM) m_hWnd);
    SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, EM_SETSEL,
        0, -1);
    m_uiWhichScript = 0;
    AddScriptsToComboBox(IDC_SELECT_SCRIPT_TEMPLATE_COMBO);
}

//---------------------------------------------------------------------------
BOOL NiScriptNewFromTemplateDialog::OnCommand(int iWParamLow, int, 
    long)
{
    switch(iWParamLow)
    {
        case IDOK:
            {
                NiFree(m_pcName);
                m_pcName = NiAlloc(char, MAX_PATH);
                SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, WM_GETTEXT, 
                    MAX_PATH, (LPARAM)m_pcName);
                int iCurSel = (int)SendDlgItemMessage(m_hWnd, 
                    IDC_SELECT_SCRIPT_TEMPLATE_COMBO, CB_GETCURSEL, 0, 0);
                if (iCurSel == CB_ERR)
                    m_uiWhichScript = 0;
                else
                    m_uiWhichScript = iCurSel;
                EndDialog(m_hWnd, IDOK);
            }
            break;
        case IDCANCEL:
            EndDialog(m_hWnd, IDCANCEL);
            break;
        default:
            break;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiScriptInfo* NiScriptNewFromTemplateDialog::GenerateNewScript()
{
    NiScriptInfo* pkOriginalScript = 
        m_spScriptSet->GetScriptAt(m_uiWhichScript);
    NIASSERT(pkOriginalScript);
    NiScriptInfo* pkNewScript =  pkOriginalScript->Clone();
    pkNewScript->SetName(m_pcName);
    pkNewScript->SetFile("");
    pkNewScript->MarkAsDirty(true);
    return pkNewScript;
}
//---------------------------------------------------------------------------
