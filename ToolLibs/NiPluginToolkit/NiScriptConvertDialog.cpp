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
#include "NiScriptConvertDialog.h"
#include "resource.h"

bool NiScriptConvertDialog::ms_bYesToAll = false;
bool NiScriptConvertDialog::ms_bDoNotModifyScript = false;

//---------------------------------------------------------------------------
NiScriptConvertDialog::NiScriptConvertDialog(NiString strTitle) :
NiDialog(IDD_CONVERT_SCRIPT, 0, 0) 
{
    m_strTitle = strTitle;
    m_hInstance = gs_hNiPluginToolkitHandle;
}

//---------------------------------------------------------------------------
bool NiScriptConvertDialog::GetModifyScript()
{
    return !ms_bDoNotModifyScript;
}

//---------------------------------------------------------------------------
bool NiScriptConvertDialog::GetYesToAll()
{
    return ms_bYesToAll;
}

//---------------------------------------------------------------------------
int NiScriptConvertDialog::DoModal()
{
    int iResult = NiDialog::DoModal();
    return iResult;
}

//---------------------------------------------------------------------------
void NiScriptConvertDialog::InitDialog()
{
    SetWindowText(m_hWnd, (const char*) m_strTitle);
    SendDlgItemMessage(m_hWnd, IDC_CONVERT_SCRIPT_ADD, BM_SETCHECK, 
        0, ms_bDoNotModifyScript);
}

//---------------------------------------------------------------------------
BOOL NiScriptConvertDialog::OnCommand(int iWParamLow, int iWParamHigh, 
                                      long)
{
    switch(iWParamLow)
    {
    case IDOK:
        EndDialog(m_hWnd, IDOK);
        break;
    case ID_YES_TO_ALL:
        ms_bYesToAll = true;
        EndDialog(m_hWnd, IDOK);
        break;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        break;
    case IDC_CONVERT_SCRIPT_ADD:
        if (iWParamHigh == BN_CLICKED)
            ms_bDoNotModifyScript = !ms_bDoNotModifyScript;
        break;
    default:
        break;
    }

    return FALSE;
}

//---------------------------------------------------------------------------

BOOL NiScriptConvertDialog::OnMessage(unsigned int, int, 
                             int, long)
{
    return FALSE;
}

//---------------------------------------------------------------------------

void NiScriptConvertDialog::OnClose()
{
    m_hWnd = 0;
}

//---------------------------------------------------------------------------

void NiScriptConvertDialog::OnDestroy()
{
    m_hWnd = 0;
}

//---------------------------------------------------------------------------
