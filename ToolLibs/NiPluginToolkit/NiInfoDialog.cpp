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
#include "NiInfoDialog.h"
#include <commctrl.h> 
#include "resource.h"
#define MAX_TEXT_LENGTH 32
//---------------------------------------------------------------------------
NiInfoDialog::NiInfoDialog(NiString strTitle) :
    NiDialog(IDD_INFO_DLG, 0, 0) 
{
    m_strTitle = strTitle;
    m_hInstance = gs_hNiPluginToolkitHandle;
}

//---------------------------------------------------------------------------
int NiInfoDialog::DoModal()
{
    int iResult = NiDialog::DoModal();
    return iResult;
}

//---------------------------------------------------------------------------
void NiInfoDialog::AppendText(const char* pcString, bool bAutoDisplay)
{
    NiString strInfoText = pcString;
    // Unify the standards of the string
    strInfoText.Replace("\r\r\n", "\n");
    strInfoText.Replace("\n", "\r\r\n");
    m_strText += strInfoText;
    if (bAutoDisplay)
        DisplayText();
}

//---------------------------------------------------------------------------
void NiInfoDialog::SetText(const char* pcText, bool bAutoDisplay)
{
    NiString strInfoText = pcText;
    // Unify the standards of the string
    strInfoText.Replace("\r\r\n", "\n");
    strInfoText.Replace("\n", "\r\r\n");
    m_strText = strInfoText;
    if (bAutoDisplay)
        DisplayText();
}

//---------------------------------------------------------------------------
bool NiInfoDialog::IsEmpty()
{
    return m_strText.IsEmpty();
}
//---------------------------------------------------------------------------
NiString NiInfoDialog::GetText()
{
    return m_strText;
}

//---------------------------------------------------------------------------

void NiInfoDialog::InitDialog()
{

    SetWindowText(m_hWnd, (const char*) m_strTitle);

    DisplayText();
    InvalidateRect(m_hWnd, NULL, true);
    UpdateWindow(m_hWnd);
}

//---------------------------------------------------------------------------
void NiInfoDialog::DisplayText()
{
    if (m_hWnd != NULL)
    {
        SendDlgItemMessage(m_hWnd, IDC_INFO_EDIT, WM_SETTEXT, 0, 
            (LPARAM)((const char*)m_strText));
    }
}
//---------------------------------------------------------------------------

BOOL NiInfoDialog::OnCommand(int iWParamLow, int, long)
{
    if (iWParamLow == IDOK)
    {
            EndDialog(m_hWnd, IDOK);
            return FALSE;
    }

    if (iWParamLow == IDC_SAVE)
    {
        char acName[_MAX_PATH];
        acName[0] = 0;

        OPENFILENAME kFilename;
        kFilename.lStructSize = sizeof(OPENFILENAME);
        kFilename.hwndOwner = NULL;
        kFilename.lpstrFilter = "text files (*.txt)\0*.txt\0\0";
        kFilename.lpstrCustomFilter = NULL;
        kFilename.nFilterIndex = 0;
        kFilename.lpstrFile = acName;
        kFilename.nMaxFile = _MAX_PATH;
        kFilename.lpstrFileTitle = NULL;
        kFilename.lpstrInitialDir = NULL;
        kFilename.lpstrTitle = NULL;
        kFilename.Flags = OFN_EXPLORER;
        kFilename.lpstrDefExt = "txt";

        if (GetSaveFileName(&kFilename))
        {
            NiString kCopy = m_strText;
            kCopy.Replace("\r\r\n", "\n");

            FILE* pkFP;
#if defined(_MSC_VER) && _MSC_VER >= 1400
            fopen_s(&pkFP, kFilename.lpstrFile, "w");
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
            pkFP = fopen(kFilename.lpstrFile, "w");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

            fprintf(pkFP,"%s",((const char*)kCopy));
            fclose(pkFP);
        }
        return TRUE;
    }
    return FALSE;
    
}
//---------------------------------------------------------------------------

BOOL NiInfoDialog::OnMessage(unsigned int, int, 
                                 int, long)
{
   // UpdateWindow(m_hWnd);
    return FALSE;
}
//---------------------------------------------------------------------------

void NiInfoDialog::OnClose()
{
    m_hWnd = 0;
}
//---------------------------------------------------------------------------

void NiInfoDialog::OnDestroy()
{
    m_hWnd = 0;
}

//---------------------------------------------------------------------------
