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

#include "NiApplicationPCH.h"
#include "NiD3D10RendererOptionsView.h"
#include "NiD3D10RendererDesc.h"
#include "NiApplicationResource.hrc"

//---------------------------------------------------------------------------
// Functions to create dialog and initialize controls in it
//---------------------------------------------------------------------------
NiWindowRef NiD3D10RendererOptionsView::InitDialog(NiWindowRef pParentWnd)
{
    if (!m_pkD3D10RendererDesc || !m_pkD3D10RendererDesc->Initialize())
        return NULL;

    NiWindowRef pWnd = NiBaseRendererOptionsView::InitDialog(pParentWnd);
    if (pWnd == NULL)
        return NULL;

    LONG_PTR pkTemp = GetWindowLongPtr(pParentWnd, GWL_HINSTANCE);
    NiInstanceRef pInstance = (NiInstanceRef)pkTemp;

    m_pD3D10DlgHandle = CreateDialog(
        pInstance,
        MAKEINTRESOURCE(IDD_D3D10OPTIONSVIEW),
        pWnd,
        D3D10RendererOptionsViewWndProc);

    if (!m_pD3D10DlgHandle)
    {
        DWORD dwError = GetLastError();
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiD3D10RendererOptionsView::InitDialog()\n"
            "Cannot create a dialog.");
        return NULL;
    }

    // Save our pointer for wndproc

    // This function should take a LONG_PTR rather than a LONG; in fact, that's
    // the whole purpose of the function. Casting to a LONG is not the correct
    // thing to do here, but that's what seems to be required to eliminate
    // warnings. A more recent Windows SDK may fix this problem.
    SetWindowLongPtr(m_pD3D10DlgHandle, GWL_USERDATA, (LONG)(LONG_PTR)this);

    // Check for error - if class pointer was not saved correctly,
    // we'll probably receive a crash. So, exit with error code.
    DWORD dwError = GetLastError();
    if (dwError)
    {
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiD3D10RendererOptionsView::InitDialog()\n"
            "Unable to save class pointer using SetWindowLong().");
        EndDialog(m_pD3D10DlgHandle, 0);
        return NULL;
    }

    // Reposition D3D10 specific options dialog to the bottom of parent
    SetWindowPos(
        m_pD3D10DlgHandle,
        HWND_TOP,
        0,
        m_uiAdvancedHeight,
        0,
        0,
        SWP_NOSIZE);

    // Update advanced height by D3D10 specific options dialog height
    RECT kRect;
    GetWindowRect(m_pD3D10DlgHandle, &kRect);
    m_uiAdvancedHeight += kRect.bottom - kRect.top;

    ShowWindow(m_pD3D10DlgHandle, SW_SHOW);

    InitD3D10DialogControls();

    return pWnd;
}

//---------------------------------------------------------------------------
void NiD3D10RendererOptionsView::InitD3D10DialogControls()
{
    NIASSERT(m_pkD3D10RendererDesc);

    NiTObjectArray<NiFixedString> kList;

    m_pkD3D10RendererDesc->GetDisplayList(kList);
    FillComboboxFromArray(
        m_pD3D10DlgHandle,
        IDC_DISPLAY_COMBO,
        kList,
        m_pkD3D10RendererDesc->GetDefaultDisplay());
}

//---------------------------------------------------------------------------
void NiD3D10RendererOptionsView::InitAdapterDependentControls()
{
    InitD3D10DialogControls();
    NiBaseRendererOptionsView::InitAdapterDependentControls();
}

//---------------------------------------------------------------------------
// Functions to react on user input
//---------------------------------------------------------------------------
bool NiD3D10RendererOptionsView::ChangeDisplay()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pD3D10DlgHandle,
        IDC_RTFORMAT_COMBO,
        CB_GETCURSEL,
        0,
        0);

    if (lResult != CB_ERR)
    {
        // Ask RenderDesc to change adapter and reenum dependent settings
        unsigned int uiDisplay = PtrToUint(lResult);
        m_pkD3D10RendererDesc->SelectDisplay(uiDisplay);
        // Refill controls, that depends on adapter choice
        NiBaseRendererOptionsView::InitAdapterDependentControls();
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
// Window messages processing functions
//---------------------------------------------------------------------------
bool NiD3D10RendererOptionsView::ProcessD3D10Command(
    NiWindowRef,
    WORD wID,
    WORD wNotifyCode)
{
    switch (wID)
    {
    case IDC_DISPLAY_COMBO:
        if (wNotifyCode != CBN_SELCHANGE)
            break;
        return ChangeDisplay();
    }
    return false;
}

//---------------------------------------------------------------------------
BOOL CALLBACK NiD3D10RendererOptionsView::D3D10RendererOptionsViewWndProc(
    HWND pDlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    // Get pointer to class to pass it messages
    LONG_PTR pkPtr = GetWindowLongPtr(pDlg, GWL_USERDATA);
    NiD3D10RendererOptionsView* pkOptionsView =
        reinterpret_cast<NiD3D10RendererOptionsView*>(pkPtr);

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        // Pass command to TabController and optionally then to OptionsView
        WORD wID = LOWORD(wParam);
        WORD wNotifyCode = HIWORD(wParam);
        NiWindowRef pDlgHandle = (NiWindowRef)lParam;

        if (!pkOptionsView)
            return FALSE;

        if (pkOptionsView->ProcessD3D10Command(pDlgHandle, wID, wNotifyCode))
            return TRUE;
    }

    return FALSE;
}
