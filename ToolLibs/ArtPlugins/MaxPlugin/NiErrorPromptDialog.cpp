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
#include "NiErrorPromptDialog.h"
#include <commctrl.h> 

#define MAX_TEXT_LENGTH 32
//---------------------------------------------------------------------------
NiErrorPromptDialog::NiErrorPromptDialog() :
    NiDialog(IDD_PROMPTFORERRORSDLG, 0, 0) 
{
    m_hInstance = hInstance;
    m_bAskAgain = true;
}

//---------------------------------------------------------------------------
int NiErrorPromptDialog::DoModal()
{
    if (NiMAXOptions::GetBool(NI_LOG_ERRORS_PROMPT) == false)
    {
        bool bLastReturn = NiMAXOptions::GetBool(
            NI_LAST_ERROR_PROMPT_RETURN);
        if (bLastReturn == true)
            return IDYES;
        else
            return IDNO;
    }

    int iResult = NiDialog::DoModal();
    if (m_bAskAgain == false)
    {
        NiMAXOptions::SetBool(NI_LOG_ERRORS_PROMPT, false);
        if (iResult == IDYES)
            NiMAXOptions::SetBool(NI_LAST_ERROR_PROMPT_RETURN, true);
        else 
            NiMAXOptions::SetBool(NI_LAST_ERROR_PROMPT_RETURN, false);
        NiMAXOptions::WriteOptions(GetCOREInterface());
    }
    return iResult;
}


//---------------------------------------------------------------------------

void NiErrorPromptDialog::InitDialog()
{

    InvalidateRect(m_hWnd, NULL, true);
    CenterWindow(m_hWnd, GetWindow(m_hWnd, GW_OWNER));
    UpdateWindow(m_hWnd);

}

//---------------------------------------------------------------------------

BOOL NiErrorPromptDialog::OnCommand(int iWParamLow, int, 
    long)
{
    if (iWParamLow == IDYES)
    {
        EndDialog(m_hWnd, IDYES);
        return FALSE;
    }

    if (iWParamLow == IDNO)
    {
        EndDialog(m_hWnd, IDNO);
        return FALSE;
    }

    if (iWParamLow == IDC_ASK_AGAIN)
    {
        int iState = IsDlgButtonChecked(m_hWnd, IDC_ASK_AGAIN);
        switch (iState)
        {
            case BST_CHECKED:
                m_bAskAgain = false;
                break;
            case BST_INDETERMINATE:
                NIASSERT(iState != BST_INDETERMINATE);
                return false;
            case BST_UNCHECKED:
                m_bAskAgain = true;
                break;
        }
    }
    return FALSE;
    
}
//---------------------------------------------------------------------------

BOOL NiErrorPromptDialog::OnMessage(unsigned int, int, 
                                 int, long)
{
   // UpdateWindow(m_hWnd);
    return FALSE;
}
//---------------------------------------------------------------------------

void NiErrorPromptDialog::OnClose()
{
    m_hWnd = 0;
}
//---------------------------------------------------------------------------

void NiErrorPromptDialog::OnDestroy()
{
    m_hWnd = 0;
}

//---------------------------------------------------------------------------
