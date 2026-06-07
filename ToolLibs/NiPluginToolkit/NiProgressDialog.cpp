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
#include "NiProgressDialog.h"
#include <commctrl.h> 
#include "resource.h"
#define MAX_TEXT_LENGTH 32
//---------------------------------------------------------------------------
NiProgressDialog::NiProgressDialog(NiString strTitle) :
    NiDialog(IDD_PROGRESS_DLG, 0, 0) 
{
    m_strTitle = strTitle;
    m_hInstance = gs_hNiPluginToolkitHandle;
    m_strLineOneText = "\0";
    m_strLineTwoText = "\0";
}

//---------------------------------------------------------------------------
void NiProgressDialog::SetLineOne(NiString strFullText)
{
    char acString[MAX_TEXT_LENGTH+1];
    if (strFullText.Length() > MAX_TEXT_LENGTH)
    {
        strFullText = strFullText.Left(MAX_TEXT_LENGTH - 3);
        strFullText += "...";
    }

    m_strLineOneText = strFullText;

    if (m_hWnd)
    {
        NiSprintf(acString, MAX_TEXT_LENGTH + 1, "%s", 
            (const char*) strFullText);  
        SetDlgItemText( m_hWnd,                 // handle of dialog box
                    IDC_PRIMARY_TEXT,
                                                // identifier of control
                    (LPCSTR) acString);         // text to set
    }
}
//---------------------------------------------------------------------------
void NiProgressDialog::SetLineTwo(NiString strFullText)
{
    char acString[MAX_TEXT_LENGTH+1];
    if (strFullText.Length() > MAX_TEXT_LENGTH)
    {
        strFullText = strFullText.Left(MAX_TEXT_LENGTH - 3);
        strFullText += "...";
    }

    m_strLineTwoText = strFullText;

    if (m_hWnd)
    {
        NiSprintf(acString, MAX_TEXT_LENGTH + 1, "%s", 
            (const char*) strFullText);  
        SetDlgItemText( m_hWnd,                 // handle of dialog box
                    IDC_SECONDARY_TEXT,
                                                // identifier of control
                    (LPCSTR) acString);         // text to set
    }
}

//---------------------------------------------------------------------------
void NiProgressDialog::StepIt()
{
    SendMessage(m_hWndPB, PBM_STEPIT, 0, 0);
}
//---------------------------------------------------------------------------
void NiProgressDialog::SetRangeSpan(unsigned int uiSpan)
{
    SendMessage(m_hWndPB, PBM_SETRANGE32, 0, uiSpan);
}
//---------------------------------------------------------------------------

void NiProgressDialog::SetPosition(unsigned int uiPos)
{
    SendMessage(m_hWndPB, PBM_SETPOS, uiPos, 0);
}

//---------------------------------------------------------------------------

void NiProgressDialog::InitDialog()
{
    m_hWndPB = GetDlgItem(m_hWnd, IDC_PROGRESS_BAR);
    SendMessage(m_hWndPB, PBM_SETSTEP, 1, 0);
    SetLineOne(m_strLineOneText);
    SetLineTwo(m_strLineTwoText);

    
    char acTitle[MAX_PATH];
    NiSprintf(acTitle, MAX_PATH, "%s", (const char*) m_strTitle);
    SetDlgItemText(     m_hWnd,              // handle of dialog box
                        IDC_PROGRESS_NAME,   // identifier of control
                        (LPCSTR)acTitle);
    InvalidateRect(m_hWnd, NULL, true);
    UpdateWindow(m_hWnd);
}
//---------------------------------------------------------------------------

BOOL NiProgressDialog::OnCommand(int, int, long)
{
    //UpdateWindow(m_hWnd);
    return FALSE;
    
}
//---------------------------------------------------------------------------

BOOL NiProgressDialog::OnMessage(unsigned int, int, 
                                 int, long)
{
   // UpdateWindow(m_hWnd);
    return FALSE;
}
//---------------------------------------------------------------------------

void NiProgressDialog::OnClose()
{
    m_hWnd = 0;
}
//---------------------------------------------------------------------------

void NiProgressDialog::OnDestroy()
{
    m_hWnd = 0;
}

//---------------------------------------------------------------------------
