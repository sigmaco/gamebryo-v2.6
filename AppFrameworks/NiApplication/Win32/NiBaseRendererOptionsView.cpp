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
#include "NiBaseRendererOptionsView.h"
#include "NiBaseRendererDesc.h"
#include "NiApplicationResource.hrc"

//---------------------------------------------------------------------------
// Functions to create dialog and initialize controls in it
//---------------------------------------------------------------------------
NiWindowRef NiBaseRendererOptionsView::InitDialog(NiWindowRef pParentWnd)
{
    LONG_PTR pkTemp = GetWindowLongPtr(pParentWnd, GWL_HINSTANCE);
    NiInstanceRef pInstance = (NiInstanceRef)pkTemp;

    // Create our dialog
    m_pDlgHandle = CreateDialog(
        pInstance,
        MAKEINTRESOURCE(IDD_BASEOPTIONSVIEW),
        pParentWnd,
        RendererOptionsViewWndProc);

    if (!m_pDlgHandle)
    {
        DWORD dwError = GetLastError();
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiBaseRendererOptionsView::InitDialog()\n"
            "Cannot create a dialog.");
        return NULL;
    }

    // Save our pointer for wndproc

    // This function should take a LONG_PTR rather than a LONG; in fact, that's
    // the whole purpose of the function. Casting to a LONG is not the correct
    // thing to do here, but that's what seems to be required to eliminate
    // warnings. A more recent Windows SDK may fix this problem.
    SetWindowLongPtr(m_pDlgHandle, GWL_USERDATA, (LONG)(LONG_PTR)this);

    // Check for error - if class pointer was not saved correctly,
    // we'll probably receive a crash. So, exit with error code.
    DWORD dwError = GetLastError();
    if (dwError)
    {
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiBaseRendererOptionsView::InitDialog()\n"
            "Unable to save class pointer using SetWindowLongPtr().");
        EndDialog(m_pDlgHandle, 0);
        return NULL;
    }

    // Calculate basic & advanced dialog height
    RECT kRect;
    RECT kAdvRect;
    GetWindowRect(m_pDlgHandle, &kRect);
    NiWindowRef pAdvGroup = GetDlgItem(m_pDlgHandle, IDC_ADVANCED_GROUP);
    GetWindowRect(pAdvGroup, &kAdvRect);
    m_uiAdvancedHeight = kAdvRect.bottom - kRect.top;
    m_uiBasicHeight = kAdvRect.top - kRect.top;

    // Fill controls and select defaults from stored settings
    InitDialogControls();

    // Select default aspect ratio
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SCRSIZE_ALL_RADIO,
        BM_SETCHECK,
        BST_CHECKED,
        0);

    return m_pDlgHandle;
}

//---------------------------------------------------------------------------
void NiBaseRendererOptionsView::InitDialogControls()
{
    // NVPrefHUD checkbox.
    bool bPerfHUDSupport = GetRendDesc()->GetNVPerfHUDSupport();
    bool bControlChecked = GetRendDesc()->GetDefaultNVPerfHUDSetting();
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_NVPERFHUD_CHECK,
        BM_SETCHECK,
        bControlChecked ? BST_CHECKED : BST_UNCHECKED,
        0);
    EnableWindow(
        GetDlgItem(m_pDlgHandle, IDC_NVPERFHUD_CHECK),
        bPerfHUDSupport);
    if (bPerfHUDSupport)
        ChangePerfHUD();

    // Fill list of adapters
    NiTObjectArray<NiFixedString> kList;
    GetRendDesc()->GetAdapterList(kList);
    FillComboboxFromArray(
        m_pDlgHandle,
        IDC_ADAPTER_COMBO,
        kList,
        GetRendDesc()->GetDefaultAdapter());

    // If NVPerfHUD enabled, disable adapter selection
    if (bPerfHUDSupport && bControlChecked)
        EnableWindow(GetDlgItem(m_pDlgHandle, IDC_ADAPTER_COMBO), FALSE);

    bControlChecked = GetRendDesc()->GetDefaultVSync();
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_VSYNC_CHECK,
        BM_SETCHECK,
        bControlChecked ? BST_CHECKED : BST_UNCHECKED,
        0);

    InitAdapterDependentControls();
}

//---------------------------------------------------------------------------
void NiBaseRendererOptionsView::InitAdapterDependentControls()
{
    bool bDesiredDevice = GetRendDesc()->GetDefaultDevice();

    if (GetRendDesc()->GetDefaultNVPerfHUDSetting())
    {
        // Disable device selection when NVPerfHUD requested
        EnableWindow(GetDlgItem(m_pDlgHandle, IDC_HAL_RADIO), FALSE);
        EnableWindow(GetDlgItem(m_pDlgHandle, IDC_REF_RADIO), FALSE);
    }
    else
    {
        BOOL bEnable = GetRendDesc()->IsDeviceAvailable(true) &&
            GetRendDesc()->IsDeviceAvailable(false);

        // Disable radiobox of another device if it is unavailable
        EnableWindow(
            GetDlgItem(
                m_pDlgHandle,
                bDesiredDevice ? IDC_HAL_RADIO : IDC_REF_RADIO),
            bEnable);

        // Enable radiobox for choosen device
        EnableWindow(
            GetDlgItem(
                m_pDlgHandle,
                bDesiredDevice ? IDC_REF_RADIO : IDC_HAL_RADIO),
            TRUE);

        // Check/uncheck appropriate radiobox for selected device type
        SendDlgItemMessage(
            m_pDlgHandle,
            IDC_HAL_RADIO,
            BM_SETCHECK,
            bDesiredDevice ? BST_UNCHECKED : BST_CHECKED,
            0);
        SendDlgItemMessage(
            m_pDlgHandle,
            IDC_REF_RADIO,
            BM_SETCHECK,
            bDesiredDevice ? BST_CHECKED : BST_UNCHECKED,
            0);
    }

    // Fill in resolution combobox
    NiTObjectArray<NiFixedString> kList;

    GetRendDesc()->GetResolutionList(kList);
    FillComboboxFromArray(
        m_pDlgHandle,
        IDC_RESOLUTION_COMBO,
        kList,
        GetRendDesc()->GetDefaultResolutionIdx());

    InitDeviceDependentControls();
}

//---------------------------------------------------------------------------
void NiBaseRendererOptionsView::InitDeviceDependentControls()
{
    // Enable/disable and set value of pure checkbox
    EnableWindow(
        GetDlgItem(m_pDlgHandle, IDC_PURE_CHECK),
        GetRendDesc()->IsPureDeviceAvailable());
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_PURE_CHECK,
        BM_SETCHECK,
        GetRendDesc()->GetDefaultPureDevice() ? BST_CHECKED : BST_UNCHECKED,
        0);

    // Enable / disable vertexprocessing options
    BOOL bEnable;
    if (GetRendDesc()->IsVetrexprocessingAvailable())
        bEnable = TRUE;
    else
        bEnable = FALSE;

    EnableWindow(GetDlgItem(m_pDlgHandle, IDC_HWVERTEX_RADIO), bEnable);
    EnableWindow(GetDlgItem(m_pDlgHandle, IDC_MIXVERTEX_RADIO), bEnable);
    EnableWindow(GetDlgItem(m_pDlgHandle, IDC_SWVERTEX_RADIO), bEnable);

    int iEnableHW = BST_UNCHECKED;
    int iEnableMix = BST_UNCHECKED;
    int iEnableSW = BST_UNCHECKED;

    switch (GetRendDesc()->GetDefaultVertexprocessing())
    {
    case NiRendererSettings::VERTEX_HARDWARE:
        iEnableHW = BST_CHECKED;
        break;

    case NiRendererSettings::VERTEX_MIXED:
        iEnableMix = BST_CHECKED;
        break;

    case NiRendererSettings::VERTEX_SOFTWARE:
        iEnableSW = BST_CHECKED;
        break;
    }

    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_HWVERTEX_RADIO,
        BM_SETCHECK,
        iEnableHW,
        0);
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_MIXVERTEX_RADIO,
        BM_SETCHECK,
        iEnableMix,
        0);
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_SWVERTEX_RADIO,
        BM_SETCHECK,
        iEnableSW,
        0);

    // Fill RT and DS formats lists
    NiTObjectArray<NiFixedString> kList;
    GetRendDesc()->GetRenderTargetFormatList(kList);
    FillComboboxFromArray(
        m_pDlgHandle,
        IDC_RTFORMAT_COMBO,
        kList,
        GetRendDesc()->GetDefaultRTFormat());

    kList.RemoveAll();
    GetRendDesc()->GetDepthSurfaceFormatList(kList);
    FillComboboxFromArray(
        m_pDlgHandle,
        IDC_DSFORMAT_COMBO,
        kList,
        GetRendDesc()->GetDefaultDSFormat());

    // Full screen checkbox
    bool bEnableFullscreen = GetRendDesc()->CanRenderWindowed();
    bool bControlChecked = GetRendDesc()->GetDefaultWindowedMode();
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_FULLSCREEN_CHECK,
        BM_SETCHECK,
        bControlChecked ? BST_CHECKED : BST_UNCHECKED,
        0);
    EnableWindow(
        GetDlgItem(m_pDlgHandle, IDC_FULLSCREEN_CHECK),
        bEnableFullscreen);

    // Fill multisamples list
    kList.RemoveAll();
    GetRendDesc()->GetMultisampleModeList(kList);
    FillComboboxFromArray(
        m_pDlgHandle,
        IDC_MULTISAMPLE_COMBO,
        kList,
        GetRendDesc()->GetDefaultMultisample());
}

//---------------------------------------------------------------------------
// Height adjusting functions for basic / advanced view
//---------------------------------------------------------------------------
unsigned int NiBaseRendererOptionsView::SetBasicHeight()
{
    RECT kRect;
    GetWindowRect(m_pDlgHandle, &kRect);
    SetWindowPos(
        m_pDlgHandle,
        HWND_TOP,
        0,
        0,
        kRect.right - kRect.left,
        m_uiBasicHeight,
        SWP_NOMOVE);
    return m_uiBasicHeight;
}

//---------------------------------------------------------------------------
unsigned int NiBaseRendererOptionsView::SetAdvancedHeight()
{
    RECT kRect;
    GetWindowRect(m_pDlgHandle, &kRect);
    SetWindowPos(
        m_pDlgHandle,
        HWND_TOP,
        0,
        0,
        kRect.right - kRect.left,
        m_uiAdvancedHeight,
        SWP_NOMOVE);
    return m_uiAdvancedHeight;
}

//---------------------------------------------------------------------------
// Helper function, that fills a combo box from array of strings
//---------------------------------------------------------------------------
void NiBaseRendererOptionsView::FillComboboxFromArray(
    NiWindowRef pWnd,
    int iControlId,
    NiTObjectArray<NiFixedString>& kList,
    unsigned int uiCurSel)
{
    // Reset combobox contents
    SendDlgItemMessage(
        pWnd,
        iControlId,
        CB_RESETCONTENT,
        0,
        0);

    unsigned int uiListLength = kList.GetSize();
    if (!uiListLength)
    {
        // Disable combobox if it is empty and exit
        SendDlgItemMessage(
            pWnd,
            iControlId,
            CB_ADDSTRING,
            0,
            (LPARAM)"(Empty)");
        SendDlgItemMessage(
            pWnd,
            iControlId,
            CB_SETCURSEL,
            0,
            0);
        EnableWindow(GetDlgItem(pWnd, iControlId), false);
        return;
    }

    // Enable combobox and fill it
    EnableWindow(GetDlgItem(pWnd, iControlId), true);

    for (unsigned int i = 0; i < uiListLength; i++)
    {
        const char* pcStr = kList[i];
        SendDlgItemMessage(
            pWnd,
            iControlId,
            CB_ADDSTRING,
            0,
            (LPARAM)pcStr);
    }

    // Set current selection
    if (uiCurSel >= kList.GetSize())
        uiCurSel = kList.GetSize() - 1;
    SendDlgItemMessage(
        pWnd,
        iControlId,
        CB_SETCURSEL,
        uiCurSel,
        0);

}

//---------------------------------------------------------------------------
// Functions to react on user input
//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangePerfHUD()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_NVPERFHUD_CHECK,
        BM_GETCHECK,
        0,
        0);

    if (lResult != BST_CHECKED && lResult != BST_UNCHECKED)
        return false;

    BOOL bPerfHud;
    // Enable or disable adapter selection and store nvperfhud state
    if (lResult == BST_CHECKED)
    {
        bPerfHud = FALSE;
        if (!GetRendDesc()->SelectPerfHUDAdapter())
        {
            // Error selecting perfhud adapter - clear the checkbox
            SendDlgItemMessage(
                m_pDlgHandle,
                IDC_NVPERFHUD_CHECK,
                BM_SETCHECK,
                BST_UNCHECKED,
                0);

            return true;
        }
        InitAdapterDependentControls();
    }
    else
    {
        bPerfHud = TRUE;
        ChangeAdapter();
    }

    EnableWindow(
        GetDlgItem(m_pDlgHandle, IDC_ADAPTER_COMBO),
        bPerfHud);

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeAdapter()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_ADAPTER_COMBO,
        CB_GETCURSEL,
        0,
        0);

    if (lResult != CB_ERR)
    {
        // Ask RenderDesc to change adapter and reenum dependent settings
        unsigned int uiAdapter = PtrToUint(lResult);
        GetRendDesc()->SelectAdapter(uiAdapter);
        // Refill controls, that depends on adapter choice
        InitAdapterDependentControls();
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeDeviceType(bool bRefDevice)
{
    int iControl;

    // Find control id, which must be unchecked
    if (bRefDevice)
        iControl = IDC_HAL_RADIO;
    else
        iControl = IDC_REF_RADIO;

    // Disable unchecked radiobox
    SendDlgItemMessage(
        m_pDlgHandle,
        iControl,
        BM_SETCHECK,
        BST_UNCHECKED, 0);

    // And, finally, save changes and refill device-dependent controls
    GetRendDesc()->SelectDevice(bRefDevice);

    InitDeviceDependentControls();

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangePureDevice()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_PURE_CHECK,
        BM_GETCHECK,
        0,
        0);

    if (lResult != BST_CHECKED && lResult != BST_UNCHECKED)
        return false;

    GetRendDesc()->SelectPureDevice(lResult == BST_CHECKED);

    InitDeviceDependentControls();

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeVertexprocessing(
    NiRendererSettings::Vertexprocessing eVertex)
{
    int iControl1 = 0;
    int iControl2 = 0;

    // Find id of controls, which must be unchecked
    switch (eVertex)
    {
    case NiRendererSettings::VERTEX_HARDWARE:
        iControl1 = IDC_MIXVERTEX_RADIO;
        iControl2 = IDC_SWVERTEX_RADIO;
        break;

    case NiRendererSettings::VERTEX_MIXED:
        iControl1 = IDC_HWVERTEX_RADIO;
        iControl2 = IDC_SWVERTEX_RADIO;
        break;

    case NiRendererSettings::VERTEX_SOFTWARE:
        iControl1 = IDC_HWVERTEX_RADIO;
        iControl2 = IDC_MIXVERTEX_RADIO;
        break;
    }
    NIASSERT(iControl1 != 0 && iControl2 != 0);

    // Disable unchecked radioboxes
    SendDlgItemMessage(
        m_pDlgHandle,
        iControl1,
        BM_SETCHECK,
        BST_UNCHECKED,
        0);
    SendDlgItemMessage(
        m_pDlgHandle,
        iControl2,
        BM_SETCHECK,
        BST_UNCHECKED,
        0);

    GetRendDesc()->SelectVertexprocessing(eVertex);

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeFullScreen()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_FULLSCREEN_CHECK,
        BM_GETCHECK,
        0,
        0);

    // Check for valid checkbox state
    if (lResult != BST_CHECKED && lResult != BST_UNCHECKED)
        return false;

    // Save changes
    if (lResult == BST_CHECKED)
        GetRendDesc()->SelectWindowedMode(true);
    else
        GetRendDesc()->SelectWindowedMode(false);

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeMultisample()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_MULTISAMPLE_COMBO,
        CB_GETCURSEL,
        0,
        0);

    if (lResult != CB_ERR)
    {
        unsigned int uiMS = PtrToUint(lResult);
        GetRendDesc()->SelectMultisample(uiMS);
    }

    return false;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeScreenSizeFilter(
    NiBaseRendererDesc::ResolutionFilters eSize)
{
    int iControl1 = 0;
    int iControl2 = 0;

    // Find id of controls, which must be unchecked
    switch (eSize)
    {
    case NiBaseRendererDesc::RESFILTER_ALL:
        iControl1 = IDC_SCRSIZE_NORMAL_RADIO;
        iControl2 = IDC_SCRSIZE_WIDE_RADIO;
        break;

    case NiBaseRendererDesc::RESFILTER_NORMAL:
        iControl1 = IDC_SCRSIZE_ALL_RADIO;
        iControl2 = IDC_SCRSIZE_WIDE_RADIO;
        break;

    case NiBaseRendererDesc::RESFILTER_WIDE:
        iControl1 = IDC_SCRSIZE_ALL_RADIO;
        iControl2 = IDC_SCRSIZE_NORMAL_RADIO;
        break;
    }
    NIASSERT(iControl1 != 0 && iControl2 != 0);

    // Disable unchecked radioboxes
    SendDlgItemMessage(
        m_pDlgHandle,
        iControl1,
        BM_SETCHECK,
        BST_UNCHECKED,
        0);
    SendDlgItemMessage(
        m_pDlgHandle,
        iControl2,
        BM_SETCHECK,
        BST_UNCHECKED,
        0);

    GetRendDesc()->SetResolutionFilter(eSize);

    InitAdapterDependentControls();

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeResolution()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_RESOLUTION_COMBO,
        CB_GETCURSEL,
        0,
        0);

    unsigned int uiRes = PtrToUint(lResult);
    GetRendDesc()->SelectResolution(uiRes);

    InitDeviceDependentControls();

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeRTFormat()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_RTFORMAT_COMBO,
        CB_GETCURSEL,
        0,
        0);

    if (lResult != CB_ERR)
    {
        // Ask RenderDesc to change adapter and reenum dependent settings
        unsigned int uiRTFormat = PtrToUint(lResult);
        GetRendDesc()->SelectRTFormat(uiRTFormat);

        InitDeviceDependentControls();

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeVSync()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_VSYNC_CHECK,
        BM_GETCHECK,
        0,
        0);

    // Check for valid checkbox state
    if (lResult != BST_CHECKED && lResult != BST_UNCHECKED)
        return false;

    // Save changes
    if (lResult == BST_CHECKED)
        GetRendDesc()->SelectVSync(true);
    else
        GetRendDesc()->SelectVSync(false);

    return true;
}

//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ChangeDSFormat()
{
    // Get selection
    LRESULT lResult = SendDlgItemMessage(
        m_pDlgHandle,
        IDC_DSFORMAT_COMBO,
        CB_GETCURSEL,
        0,
        0);

    if (lResult != CB_ERR)
    {
        // Ask RenderDesc to change adapter and reenume dependent settings
        unsigned int uiDSFormat = PtrToUint(lResult);
        GetRendDesc()->SelectDSFormat(uiDSFormat);

        InitDeviceDependentControls();

        return true;
    }

    return false;
}

//---------------------------------------------------------------------------
// Window messages processing functions
//---------------------------------------------------------------------------
bool NiBaseRendererOptionsView::ProcessCommand(
    NiWindowRef,
    WORD wID,
    WORD wNotifyCode)
{
    // Check which control is changed and call appropriate function
    switch (wID)
    {
    case IDC_NVPERFHUD_CHECK:
        return ChangePerfHUD();

    case IDC_ADAPTER_COMBO:
        if (wNotifyCode != CBN_SELCHANGE)
            break;
        return ChangeAdapter();

    case IDC_HAL_RADIO:
        return ChangeDeviceType(false);

    case IDC_REF_RADIO:
        return ChangeDeviceType(true);

    case IDC_PURE_CHECK:
        return ChangePureDevice();

    case IDC_HWVERTEX_RADIO:
        return ChangeVertexprocessing(NiRendererSettings::VERTEX_HARDWARE);

    case IDC_MIXVERTEX_RADIO:
        return ChangeVertexprocessing(NiRendererSettings::VERTEX_MIXED);

    case IDC_SWVERTEX_RADIO:
        return ChangeVertexprocessing(NiRendererSettings::VERTEX_SOFTWARE);

    case IDC_FULLSCREEN_CHECK:
        return ChangeFullScreen();

    case IDC_VSYNC_CHECK:
        return ChangeVSync();

    case IDC_SCRSIZE_ALL_RADIO:
        return ChangeScreenSizeFilter(NiBaseRendererDesc::RESFILTER_ALL);

    case IDC_SCRSIZE_NORMAL_RADIO:
        return ChangeScreenSizeFilter(NiBaseRendererDesc::RESFILTER_NORMAL);

    case IDC_SCRSIZE_WIDE_RADIO:
        return ChangeScreenSizeFilter(NiBaseRendererDesc::RESFILTER_WIDE);

    case IDC_RESOLUTION_COMBO:
        if (wNotifyCode != CBN_SELCHANGE)
            break;
        return ChangeResolution();

    case IDC_MULTISAMPLE_COMBO:
        if (wNotifyCode != CBN_SELCHANGE)
            break;
        return ChangeMultisample();

    case IDC_RTFORMAT_COMBO:
        if (wNotifyCode != CBN_SELCHANGE)
            break;
        return ChangeRTFormat();

    case IDC_DSFORMAT_COMBO:
        if (wNotifyCode != CBN_SELCHANGE)
            break;
        return ChangeDSFormat();
    }
    return false;
}

//---------------------------------------------------------------------------
BOOL CALLBACK NiBaseRendererOptionsView::RendererOptionsViewWndProc(
    HWND pDlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    // Get pointer to corresponding OptionsView to pass it messages.
    LONG_PTR pkPtr = GetWindowLongPtr(pDlg, GWL_USERDATA);
    NiBaseRendererOptionsView* pkOptionsView =
        reinterpret_cast<NiBaseRendererOptionsView*>(pkPtr);

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        // WndProc is static, and there may be two or even more different
        // OptionsViews with their own dialogs, so we must pass message to
        // corresponding OptionsView which controls given dialog.
        WORD wID = LOWORD(wParam);
        WORD wNotifyCode = HIWORD(wParam);
        NiWindowRef pDlgHandle = (NiWindowRef)lParam;

        if (!pkOptionsView)
            return FALSE;

        if (pkOptionsView->ProcessCommand(pDlgHandle, wID, wNotifyCode))
            return TRUE;

    }
    return FALSE;
}
