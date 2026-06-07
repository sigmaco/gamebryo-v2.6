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

#include "NiDialog.h"

//---------------------------------------------------------------------------
NiDialog::NiDialog(WORD wResourceId, NiModuleRef hInstance, 
    NiWindowRef hWndParent)
{
    m_wResourceId = wResourceId;
    m_hInstance = hInstance;
    m_hWndParent = hWndParent;
    m_hWnd = NULL;
}

//---------------------------------------------------------------------------

NiDialog::~NiDialog()
{
    if (m_hWnd != NULL)
        Destroy();
}

//---------------------------------------------------------------------------
void NiDialog::Create()
{
    // Ensure that the common control DLL is loaded and create a 
    // progress bar along the bottom of the client area of the 
    // parent window. Base the height of the progress bar on 
    // the height of a scroll bar arrow. 
    m_hWnd = CreateDialogParam(m_hInstance, MAKEINTRESOURCE(m_wResourceId), 
        m_hWndParent, &TheDialogProc, (LPARAM) this);

    if (m_hWnd == 0)
    {
        GetLastError();
        NIASSERT(m_hWnd != 0);
    }

/*    if(m_hWndParent == NULL)
    {
        m_hWndParent = GetWindow(m_hWnd,GW_OWNER);
        SetParent(m_hWnd, m_hWndParent);
    }
*/

    m_bIsModalDlg = false;
    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    UpdateWindow(m_hWnd);

}

//---------------------------------------------------------------------------
void NiDialog::Destroy()
{
    ShowWindow(m_hWnd, SW_HIDE);
    DestroyWindow(m_hWnd);
    m_hWnd = 0;
}

//---------------------------------------------------------------------------
NiWindowRef NiDialog::GetWindowRef()
{
    return m_hWnd;
}

//---------------------------------------------------------------------------
int NiDialog::DoModal()
{
    m_bIsModalDlg = true;
    int iResult =  (int)DialogBoxParam(m_hInstance, 
        MAKEINTRESOURCE(m_wResourceId), 
        m_hWndParent, &TheDialogProc, (LPARAM) this);

    if (iResult == -1)
    {
        GetLastError();
        NIASSERT(iResult != -1);
    }

    m_hWnd = 0;
    return iResult;
}

//---------------------------------------------------------------------------
void NiDialog::InitDialog()
{
}

//---------------------------------------------------------------------------
BOOL NiDialog::OnCommand(int, int, long)
{
    return FALSE;
}

//---------------------------------------------------------------------------
BOOL NiDialog::OnMessage(unsigned int, int, 
                         int, long)
{
    return FALSE;
}

//---------------------------------------------------------------------------
void NiDialog::OnClose()
{
}

//---------------------------------------------------------------------------
void NiDialog::OnDestroy()
{
}
//---------------------------------------------------------------------------
BOOL CALLBACK NiDialog::TheDialogProc(NiWindowRef hWnd, UINT message,
    WPARAM wParam,LPARAM lParam)
{
    int id = LOWORD(wParam);
    int code = HIWORD(wParam);
    LONG lWindow = GetWindowLong(hWnd, GWL_USERDATA);
    NiDialog* pkDialog = 
        reinterpret_cast<NiDialog*>(LongToPtr(lWindow));

    switch (message)
    { 
        case WM_INITDIALOG:
            pkDialog = (NiDialog*) lParam;
            pkDialog->m_hWnd = hWnd;
            SetWindowLong(hWnd, GWL_USERDATA, (long) lParam);
            pkDialog->InitDialog();
            UpdateWindow(hWnd);
            DefWindowProc(hWnd, message, wParam, lParam);
            return TRUE;
            break;
        case WM_CLOSE:
            NIASSERT(pkDialog);
            if(pkDialog)
                pkDialog->OnClose();
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
            break;
        case WM_DESTROY:
            NIASSERT(pkDialog);
            if(pkDialog)
                pkDialog->OnDestroy();
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
            break;
        case WM_COMMAND:
            if(pkDialog)
               return pkDialog->OnCommand(id, code, (long)lParam);
            break;
        default:
            if (pkDialog)
               return pkDialog->OnMessage(message, id, code, (long)lParam);
    }

    return (BOOL)DefWindowProc(hWnd, message, wParam, lParam);
}

