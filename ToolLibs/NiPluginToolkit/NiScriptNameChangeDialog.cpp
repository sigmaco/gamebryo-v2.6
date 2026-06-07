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
#include "NiScriptNameChangeDialog.h"
#include "NiScriptInfoDialogs.h"
#include "NiScriptTemplateManager.h"
#include "NiScriptReader.h"
#include "NiScriptWriter.h"
#include "resource.h"
#include <NiSystem.h>
#include "NiStringTokenizer.h"
#include "NiPluginManager.h"
//---------------------------------------------------------------------------
NiScriptNameChangeDialog::NiScriptNameChangeDialog(NiModuleRef hInstance, 
    NiWindowRef m_hWnd,const char* pcName, const char* pcTitle) :
    NiDialog(IDD_RENAME_SCRIPT,hInstance,m_hWnd)
{
    NIASSERT(pcName);
    NIASSERT(pcTitle);
    size_t stLength = strlen(pcName) + 1;
    m_pcName =  NiAlloc(char, stLength);
    NiSprintf(m_pcName, stLength, "%s", pcName);
    stLength = strlen(pcTitle) + 1;
    m_pcTitle = NiAlloc(char, stLength);
    NiSprintf(m_pcTitle, stLength, "%s", pcTitle);
}

//---------------------------------------------------------------------------
NiScriptNameChangeDialog::~NiScriptNameChangeDialog()
{
    NiFree(m_pcName);
    NiFree(m_pcTitle);
    m_pcName = 0;
    m_pcTitle = 0;
}
        
//---------------------------------------------------------------------------
int NiScriptNameChangeDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void NiScriptNameChangeDialog::InitDialog()
{
    SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM) m_pcTitle);
    SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, WM_SETTEXT, 
        0, (LPARAM) m_pcName);
    SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, WM_SETFOCUS,
        0, (LPARAM) m_hWnd);
    SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, EM_SETSEL,
        0, -1);
}

//---------------------------------------------------------------------------
BOOL NiScriptNameChangeDialog::OnCommand(int iWParamLow, int, 
                                         long)
{
    switch(iWParamLow)
    {
        case IDOK:
            NiFree(m_pcName);
            m_pcName = NiAlloc(char, MAX_PATH);
            SendDlgItemMessage(m_hWnd, IDC_RENAME_SCRIPT_EDIT, WM_GETTEXT, 
                MAX_PATH, (LPARAM)m_pcName);
            EndDialog(m_hWnd, IDOK);
            break;
        case IDCANCEL:
            EndDialog(m_hWnd, IDCANCEL);
            break;
        default:
            break;
    }

    return FALSE;
}
