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

#include <commctrl.h>
#include "NiRendererTabController.h"
#include "NiApplicationResource.hrc"

// Static variables
bool NiSettingsDialog::ms_bDlgOK = false;

//---------------------------------------------------------------------------
// Tab controllers manipulating functions
//---------------------------------------------------------------------------
bool NiSettingsDialog::AddTabController(NiBaseTabController* pkTabCtrl)
{
    if (!pkTabCtrl)
        return false;

    m_kTabArray.Add(pkTabCtrl);

    return true;
}
//---------------------------------------------------------------------------
bool NiSettingsDialog::CreateInternalTabControllers(NiWindowRef)
{
    NiBaseTabController* pkTC = NiNew NiRendererTabController();
    if (pkTC)
        m_kTabArray.Add(pkTC);

    // Here can be created other classes that control other tabs (sound,
    // input, network). That classes must be childs of NiBaseTabController.

    // If array is not empty, return success
    return m_kTabArray.GetSize() > 0;
}
//---------------------------------------------------------------------------
NiWindowRef NiSettingsDialog::InitTabController(unsigned int uiIdx)
{
    NiBaseTabController* pkTabControl = m_kTabArray[uiIdx];
    if (pkTabControl == NULL)
        return NULL;

    // Create child dialog
    NiWindowRef pChildDlg = pkTabControl->InitDialog(m_pDlgHandle);
    if (pChildDlg == NULL)
        return NULL;

    LPSTR pcText = pkTabControl->GetCaption();

    // Create tab item structure and insert it
    TC_ITEM item;
    item.mask = TCIF_TEXT | TCIF_PARAM;
    item.pszText = pcText;
    item.iImage = -1;
    item.lParam = (LPARAM)pChildDlg;

    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SETTINGSTAB,
        TCM_INSERTITEM,
        uiIdx,
        (LPARAM)&item);

    // Reposition tab control dialog
    RECT kTabRect;
    POINT kPoint;
    NiWindowRef pTab = GetDlgItem(m_pDlgHandle, IDC_SETTINGSTAB);
    GetWindowRect(pTab, &kTabRect);
    TabCtrl_AdjustRect(pTab, FALSE, &kTabRect);
    kPoint.x = kTabRect.left;
    kPoint.y = kTabRect.top;
    ScreenToClient(m_pDlgHandle, &kPoint);
    SetWindowPos(pChildDlg, HWND_TOP, kPoint.x, kPoint.y, 0, 0, SWP_NOSIZE);

    return pChildDlg;
}
//---------------------------------------------------------------------------
void NiSettingsDialog::DeleteTabControllers()
{
    for (unsigned i = 0; i < m_kTabArray.GetSize(); i++)
    {
        NiDelete m_kTabArray[i];
    }
}
//---------------------------------------------------------------------------
// Functions to create dialog, show it and initialize its controls
//---------------------------------------------------------------------------
bool NiSettingsDialog::InitDialog()
{
    SetLastError(0);
    m_pDlgHandle = CreateDialog(
        NiApplication::GetInstanceReference(),
        MAKEINTRESOURCE(IDD_SETTINGSDIALOG),
        NULL,
        (DLGPROC)SettingsWndProc);

    if (m_pDlgHandle == NULL)
    {
        DWORD dwError = GetLastError();
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiSettingsDialog::InitDialog()\n"
            "Cannot create NiRendererTabController dialog.");

        return false;
    }

    // Save our pointer for wndproc

    // This function should take a LONG_PTR rather than a LONG; in fact, that's
    // the whole purpose of the function. Casting to a LONG is not the correct
    // thing to do here, but that's what seems to be required to eliminate
    // warnings. A more recent Windows SDK may fix this problem.
    SetLastError(0);
    SetWindowLongPtr(m_pDlgHandle, GWL_USERDATA, (LONG)(LONG_PTR)this);

    // Check for error - if class pointer was not saved correctly,
    // we'll probably receive a crash. So, exit with error code.
    DWORD dwError = GetLastError();
    if (dwError)
    {
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiSettingsDialog::InitDialog()\n"
            "Unable to save class pointer using SetWindowLongPtr().");
        EndDialog(m_pDlgHandle, 0);
        return false;
    }

    CenterWindow();

    if (!CreateInternalTabControllers(m_pDlgHandle))
    {
        NiMessageBox(
            "Error in NiSettingsDialog::InitDialog()\n"
            "Failed to create at least one Ni...TabController class.\n",
            "NiSettingsDialog Error");
        EndDialog(m_pDlgHandle, 0);
        return false;
    }

    if (!InitDialogControls())
    {
        EndDialog(m_pDlgHandle, 0);
        return false;
    };

    m_bInitialized = true;
    return true;
}

//---------------------------------------------------------------------------
bool NiSettingsDialog::InitDialogControls()
{
    // Clear tablist
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SETTINGSTAB,
        TCM_DELETEALLITEMS,
        0,
        0);

    // Init tabs and child dialogs, create tabcontrollers
    NiWindowRef pFirstTabDlg = NULL;
    for (unsigned int i = 0; i < m_kTabArray.GetSize(); i++)
    {
        NiWindowRef pChildDlg = InitTabController(i);

        if (pChildDlg == NULL)
            return false;

        if (pFirstTabDlg == NULL)
            pFirstTabDlg = pChildDlg;
    }

    // Activate 1st tab and show its child dialog
    ShowWindow(pFirstTabDlg, SW_SHOW);
    SendDlgItemMessage(m_pDlgHandle, IDC_SETTINGSTAB, TCM_SETCURSEL, 0, 0);

    // Initialize "Do not show dialog" and "Save settings" checkboxes
    bool bRendererDialog =
        NiRendererSettings::GetInstance()->m_bRendererDialog;
    bool bSaveSettings =
        NiRendererSettings::GetInstance()->m_bSaveSettings;

    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_NODIALOG_CHECK,
        BM_SETCHECK,
        bRendererDialog ? BST_UNCHECKED : BST_CHECKED,
        0);
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SAVESETTINGS_CHECK,
        BM_SETCHECK,
        !bRendererDialog || bSaveSettings ? BST_CHECKED : BST_UNCHECKED,
        0);

    if (!bRendererDialog)
    {
        EnableWindow(
            GetDlgItem(m_pDlgHandle, IDC_SAVESETTINGS_CHECK),
            FALSE);
    }

    // End status of dialog box - ok or cancel pressed
    ms_bDlgOK = false;

    return true;
}

//---------------------------------------------------------------------------
bool NiSettingsDialog::ShowDialog()
{
    // Exit if InitDialog() was not called or fails
    if (!m_bInitialized)
        return false;

    ChangeSize();
    ShowWindow(m_pDlgHandle, SW_SHOW);

    const NiWindowRef pWnd =
        NiApplication::ms_pkApplication->GetAppWindow()->GetWindowReference();

    const NiAcceleratorRef pAccel =
        NiApplication::ms_pkApplication->GetAcceleratorReference();

    // Message loop
    while (IsWindowVisible(m_pDlgHandle))
    {
        MSG kMsg;
        if (PeekMessage(&kMsg, NULL, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(pWnd, pAccel, &kMsg))
            {
                TranslateMessage(&kMsg);
                DispatchMessage(&kMsg);
            }
        }
    }

    return ms_bDlgOK;
}

//---------------------------------------------------------------------------
// Window and its controls repositioning / resizing functions
//---------------------------------------------------------------------------
void NiSettingsDialog::CenterWindow()
{
    // Make it centered
    RECT kParentRect;
    GetWindowRect(GetDesktopWindow(), &kParentRect);

    RECT kWndRect;
    GetWindowRect(m_pDlgHandle, &kWndRect);

    LONG lParentWidth = kParentRect.right - kParentRect.left;
    LONG lParentHeight = kParentRect.bottom - kParentRect.top;

    LONG lWndWidth = kWndRect.right - kWndRect.left;
    LONG lWndHeight = kWndRect.bottom - kWndRect.top;

    SetWindowPos(
        m_pDlgHandle,
        HWND_TOP,
        lParentWidth / 2 - lWndWidth / 2,
        lParentHeight / 2 - lWndHeight / 2,
        lWndWidth,
        lWndHeight,
        0);
}

//---------------------------------------------------------------------------
void NiSettingsDialog::RepositionControl(
    int iControl,
    int iDeltaHeight)
{
    // move control up or down by iDeltaHeight pixels
    NiWindowRef pWnd = GetDlgItem(m_pDlgHandle, iControl);
    RECT kRect;
    POINT kPoint;
    GetWindowRect(pWnd, &kRect);
    kPoint.x = kRect.left;
    kPoint.y = kRect.top;
    ScreenToClient(m_pDlgHandle, &kPoint);
    SetWindowPos(
        pWnd,
        HWND_TOP,
        kPoint.x,
        (int)kPoint.y + iDeltaHeight,
        0,
        0,
        SWP_NOSIZE);
}

//---------------------------------------------------------------------------
void NiSettingsDialog::ChangeSize()
{
    // Get active tab
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SETTINGSTAB,
        TCM_GETCURSEL,
        0,
        0);
    if (lResult == -1)
        return;

    // Get height of tab client area
    unsigned int uiIdx = PtrToUint(lResult);
    unsigned int uiTabHeight;
    if (m_bViewAdvanced)
        uiTabHeight = m_kTabArray[uiIdx]->SetAdvancedHeight();
    else
        uiTabHeight = m_kTabArray[uiIdx]->SetBasicHeight();

    RECT kTabRect;
    RECT kWindowRect;
    NiWindowRef pTab = GetDlgItem(m_pDlgHandle, IDC_SETTINGSTAB);
    GetWindowRect(pTab, &kTabRect);
    GetWindowRect(m_pDlgHandle, &kWindowRect);
    unsigned int uiOldTabHeight = kTabRect.bottom - kTabRect.top;

    // Convert client tab height to full tab height
    TabCtrl_AdjustRect(pTab, FALSE, &kTabRect);
    kTabRect.bottom = kTabRect.top + uiTabHeight;
    TabCtrl_AdjustRect(pTab, TRUE, &kTabRect);
    int iTabHeightDelta =
        (int)(kTabRect.bottom - kTabRect.top - uiOldTabHeight);

    // Adjust height of window
    kWindowRect.bottom += iTabHeightDelta;

    // Resize tab control
    SetWindowPos(
        pTab,
        HWND_TOP,
        0,
        0,
        kTabRect.right - kTabRect.left,
        kTabRect.bottom - kTabRect.top,
        SWP_NOMOVE);

    // Move controls under tab control by iTabHeightDelta
    RepositionControl(IDC_NODIALOG_CHECK, iTabHeightDelta);
    RepositionControl(IDC_SAVESETTINGS_CHECK, iTabHeightDelta);
    RepositionControl(IDOK, iTabHeightDelta);
    RepositionControl(IDCANCEL, iTabHeightDelta);
    RepositionControl(IDADVANCED, iTabHeightDelta);

    // Resize main window
    SetWindowPos(
        m_pDlgHandle,
        HWND_TOP,
        0,
        0,
        kWindowRect.right - kWindowRect.left,
        kWindowRect.bottom - kWindowRect.top,
        SWP_NOMOVE);

    CenterWindow();
}

//---------------------------------------------------------------------------
// Functions, that react on user input
//---------------------------------------------------------------------------
bool NiSettingsDialog::ChangeAdvanced()
{
    // Change advanced button text and request window size change
    char* pcText;
    m_bViewAdvanced = !m_bViewAdvanced;
    if (m_bViewAdvanced)
        pcText = "Advanced <<";
    else
        pcText = "Advanced >>";

    SetDlgItemText(m_pDlgHandle, IDADVANCED, pcText);

    ChangeSize();

    return true;
}

//---------------------------------------------------------------------------
bool NiSettingsDialog::ChangeNoDialog()
{
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_NODIALOG_CHECK,
        BM_GETCHECK,
        0,
        0);

    if (lResult != BST_CHECKED && lResult != BST_UNCHECKED)
        return false;

    bool bRendererDialog = lResult == BST_UNCHECKED ? true : false;
    NiRendererSettings::GetInstance()->m_bRendererDialog = bRendererDialog;
    bool bSaveSettings =
        NiRendererSettings::GetInstance()->m_bSaveSettings;

    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SAVESETTINGS_CHECK,
        BM_SETCHECK,
        !bRendererDialog || bSaveSettings ? BST_CHECKED : BST_UNCHECKED,
        0);

    BOOL bEnable = bRendererDialog ? TRUE : FALSE;
    EnableWindow(
        GetDlgItem(m_pDlgHandle, IDC_SAVESETTINGS_CHECK),
        bEnable);

    return true;
}

//---------------------------------------------------------------------------
bool NiSettingsDialog::ChangeSaveSettings()
{
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SAVESETTINGS_CHECK,
        BM_GETCHECK,
        0,
        0);

    if (lResult != BST_CHECKED && lResult != BST_UNCHECKED)
        return false;

    bool bSaveSettings = lResult == BST_CHECKED ? true : false;
    NiRendererSettings::GetInstance()->m_bSaveSettings =
        bSaveSettings;

    return true;
}

//---------------------------------------------------------------------------
void NiSettingsDialog::ActivateTab(unsigned int uiTabIdx)
{
    // Get child window HWND and show/hide it
    TC_ITEM kItem;
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SETTINGSTAB,
        TCM_GETITEM,
        uiTabIdx,
        (LPARAM)&kItem);

    ShowWindow((HWND)kItem.lParam, TRUE);

    ChangeSize();
}

//---------------------------------------------------------------------------
void NiSettingsDialog::DeactivateTab(unsigned int uiTabIdx)
{
    // Get child window HWND and show/hide it
    TC_ITEM kItem;
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SETTINGSTAB,
        TCM_GETITEM,
        uiTabIdx,
        (LPARAM)&kItem);

    ShowWindow((HWND)kItem.lParam, FALSE);
}

//---------------------------------------------------------------------------
// Window messages processing functions
//---------------------------------------------------------------------------
bool NiSettingsDialog::ProcessCommand(
    NiWindowRef,
    WORD wID,
    WORD)
{
    // Process renderer type change
    switch (wID)
    {
    case IDOK:
        // Process clicks on OK and Cancel
        ms_bDlgOK = true;

    case IDCANCEL:
        EndDialog(m_pDlgHandle, wID);
        return true;

    case IDADVANCED:
        return ChangeAdvanced();

    case IDC_NODIALOG_CHECK:
        return ChangeNoDialog();

    case IDC_SAVESETTINGS_CHECK:
        return ChangeSaveSettings();
    }

    return false;
}

//---------------------------------------------------------------------------
BOOL CALLBACK NiSettingsDialog::SettingsWndProc(
    HWND pDlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    // Get pointer to class to pass it messages
    LONG_PTR pkPtr = GetWindowLongPtr(pDlg, GWL_USERDATA);
    NiSettingsDialog* pkSetDlg =
        reinterpret_cast<NiSettingsDialog*>(pkPtr);

    WORD wID = LOWORD(wParam);
    WORD wNotifyCode = HIWORD(wParam);
    NiWindowRef pDlgHandle = (NiWindowRef)lParam;

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(pDlg, IDOK));
        return TRUE;

    case WM_COMMAND:
        if (!pkSetDlg)
            return FALSE;

        if (pkSetDlg->ProcessCommand(pDlgHandle, wID, wNotifyCode))
            return TRUE;

        break;

    case WM_NOTIFY:
        // Switch tabs
        if ((int)wParam != IDC_SETTINGSTAB)
            break;

        LPNMHDR pNMHDR = (LPNMHDR)lParam;
        if (pNMHDR->code != TCN_SELCHANGING && pNMHDR->code != TCN_SELCHANGE)
            break;

        // Tab id
        LRESULT lResult =
            SendDlgItemMessage(pDlg, IDC_SETTINGSTAB, TCM_GETCURSEL, 0, 0);
        if (lResult == -1)
            break;

        // Show or hide tab dialog?
        unsigned int uiTabIdx = PtrToUint(lResult);
        if (pNMHDR->code == TCN_SELCHANGING)
            pkSetDlg->DeactivateTab(uiTabIdx);
        else
            pkSetDlg->ActivateTab(uiTabIdx);

        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
// This function creates a messagebox, saying user that something goes wrong
//---------------------------------------------------------------------------
void NiSettingsDialog::ReportWinAPIError(DWORD dwErrorCode, char* pcErrorText)
{
    if (dwErrorCode == ERROR_RESOURCE_TYPE_NOT_FOUND)
    {
        const char* pcResourceError =
            "Error: Resource files are not linked in.\n"
            "Please link file NiApplicationResource.rc\n"
            "into your project. This file can be found in\n"
            "SDK\\Win32\\Resource.";

        NiMessageBox(pcResourceError, "Resources Error");
    }
    else
    {
        char pcWin32ErrorText[256];
        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwErrorCode,
            0,
            (LPTSTR)pcWin32ErrorText,
            256,
            NULL);
        char pcCompleteErrorText[512];
        NiSprintf(
            pcCompleteErrorText,
            512,
            "%s\nWindows reports error 0x%x:\n%s",
            pcErrorText,
            dwErrorCode,
            pcWin32ErrorText);

        NiMessageBox(pcCompleteErrorText, "Dialog creation error");
    }
}
//---------------------------------------------------------------------------
