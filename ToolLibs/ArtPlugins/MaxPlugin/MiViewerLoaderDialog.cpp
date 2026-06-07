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
#include "MiViewerLoaderDialog.h"
#include "MiResource.h"
#include <commctrl.h> 
#include "NiMAX.h"
//---------------------------------------------------------------------------
// Our static message handler
// In order to avoid making everything static, I keep a ptr to the actual
// instance of the texture export settings dialog window
static BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM, 
    LPARAM)
{
    switch (iMsg)
    {
        case WM_INITDIALOG:
            UpdateWindow(hDlg);
            return TRUE;

        case WM_COMMAND:
            /*switch(LOWORD(wParam))
            {
                default:
                   break;
            }*/
            UpdateWindow(hDlg);
            break;
    }
    return FALSE;
}



//---------------------------------------------------------------------------
MiViewerLoaderDialog::MiViewerLoaderDialog(HINSTANCE hParent, char* pcTitle)
{
    
    // Ensure that the common control DLL is loaded and create a 
    // progress bar along the bottom of the client area of the 
    // parent window. Base the height of the progress bar on 
    // the height of a scroll bar arrow. 
    InitCommonControls(); 
    HWND hWndParent = NULL;
    hWnd = CreateDialog(hParent,        // handle to application instance
                        MAKEINTRESOURCE(IDD_VIEWER_LOADING_PANEL), 
                                       // identifies dialog box template name 
                        hWndParent,    // handle to owner window
                        DlgProc        // pointer to dialog box procedure
                );

    if(hWndParent == NULL)
    {
        hWndParent = GetWindow(hWnd,GW_OWNER);
        SetParent(hWnd, hWndParent);
    }
 
    CenterWindow(hWnd, GetWindow(hWnd, GW_OWNER));
    ShowWindow(hWnd, SW_SHOWNORMAL);
    hwndPB = GetDlgItem(hWnd, IDC_VIEWER_LOAD_PROGRESS);
    SendMessage(hwndPB, PBM_SETSTEP, 1, 0);
    SetDlgItemText(     hWnd,                       // handle of dialog box
                        IDC_VIEWER_LOAD_MESSAGE,    // identifier of control
                        "\0" );                     // text to set
    SetDlgItemText(     hWnd,                       // handle of dialog box
                        IDC_VIEWER_STATIC_STRING,   // identifier of control
                        (LPCSTR)pcTitle);
    
}
//---------------------------------------------------------------------------
MiViewerLoaderDialog::~MiViewerLoaderDialog()
{
    if(hWnd != 0)
        EndWindow();
}
//---------------------------------------------------------------------------
void MiViewerLoaderDialog::EndWindow()
{
    ShowWindow(hWnd, SW_HIDE);
    DestroyWindow(hWnd);
    hWnd = 0;
}
//---------------------------------------------------------------------------
void MiViewerLoaderDialog::UpdateLoadingString(char* pcString)
{
    SetDlgItemText( hWnd,                       // handle of dialog box
                    IDC_VIEWER_LOAD_MESSAGE,    // identifier of control
                    (LPCSTR) pcString);         // text to set
}
//---------------------------------------------------------------------------
void MiViewerLoaderDialog::UpdateSpecificString(char* pcString)
{
    SetDlgItemText( hWnd,                       // handle of dialog box
                    IDC_VIEWER_LOAD_MESSAGE_SPECIFIC,
                                                // identifier of control
                    (LPCSTR) pcString);         // text to set
}
//---------------------------------------------------------------------------
void MiViewerLoaderDialog::StepIt()
{
    SendMessage(hwndPB, PBM_STEPIT, 0, 0);
}
//---------------------------------------------------------------------------
void MiViewerLoaderDialog::SetRangeSpan(unsigned int uiSpan)
{
    SendMessage(hwndPB, PBM_SETRANGE32, 0, uiSpan);
}
//---------------------------------------------------------------------------
void MiViewerLoaderDialog::SetPosition(unsigned int uiPos)
{
    SendMessage(hwndPB, PBM_SETPOS, 0, uiPos);
}