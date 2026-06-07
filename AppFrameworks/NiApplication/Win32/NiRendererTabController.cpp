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
#include "NiRendererTabController.h"
#include "NiDX9RendererOptionsView.h"
#include "NiD3D10RendererOptionsView.h"
#include "NiApplicationResource.hrc"

//---------------------------------------------------------------------------
// Functions to create, initialize, delete NiXXXRendererOptionsView's
//---------------------------------------------------------------------------
bool NiRendererTabController::CreateOptionsViews()
{
    // Create OptionsView classes for DX9 and D3D10
    NiDX9RendererOptionsView* pkDX9ROV = NiNew NiDX9RendererOptionsView();

    if (pkDX9ROV)
    {
        if (InitOptionsView(pkDX9ROV))
            m_kViewArray.Add(pkDX9ROV);
        else
            NiDelete pkDX9ROV;
    }

    NiD3D10RendererOptionsView* pkD3D10ROV =
        NiNew NiD3D10RendererOptionsView();

    if (pkD3D10ROV)
    {
        if (InitOptionsView(pkD3D10ROV))
            m_kViewArray.Add(pkD3D10ROV);
        else
            NiDelete pkD3D10ROV;
    }

    // If array is not empty, return success
    return m_kViewArray.GetSize() > 0;
}

//---------------------------------------------------------------------------
bool NiRendererTabController::InitOptionsView(NiBaseRendererOptionsView* pkOV)
{
    NiWindowRef pDlg = pkOV->InitDialog(m_pDlgHandle);
    if (pDlg == NULL)
        return false;

    // Offset child dialog to be below renderer selection combo
    SetWindowPos(
        pDlg,
        HWND_TOP,
        0,
        m_uiChildDlgOffset,
        0,
        0,
        SWP_NOSIZE);

    return true;
}

//---------------------------------------------------------------------------
void NiRendererTabController::DeleteOptionsViews()
{
    // Delete all OptionsView classes
    for (unsigned i = 0; i < m_kViewArray.GetSize(); i++)
    {
        NiDelete m_kViewArray[i];
    }
}

//---------------------------------------------------------------------------
// Functions to create dialog and initialize controls
//---------------------------------------------------------------------------
NiWindowRef NiRendererTabController::InitDialog(NiWindowRef pParentWnd)
{
    LONG_PTR pkTemp = GetWindowLongPtr(pParentWnd, GWL_HINSTANCE);
    NiInstanceRef pInstance = (NiInstanceRef)pkTemp;

    // Create dialog
    m_pDlgHandle = CreateDialog(
        pInstance,
        MAKEINTRESOURCE(IDD_RENDERERTAB),
        pParentWnd,
        RendererTabWndProc);

    if (m_pDlgHandle == NULL)
    {
        DWORD dwError = GetLastError();
        NiSettingsDialog::ReportWinAPIError(
            dwError,
            "Error in NiRendererTabController::InitDialog()\n"
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
            "Error in NiRendererTabController::InitDialog()\n"
            "Unable to save class pointer using SetWindowLongPtr().");
        EndDialog(m_pDlgHandle, 0);
        return NULL;
    }

    NiWindowRef pRendererCombo =
        GetDlgItem(m_pDlgHandle, IDC_RENDERER_SEL_COMBO);

    // Save bottom point of renderer selection combo for child dlg repos
    RECT kRect;
    POINT kPoint;
    GetWindowRect(pRendererCombo, &kRect);
    kPoint.x = kRect.right;
    kPoint.y = kRect.bottom;
    ScreenToClient(m_pDlgHandle, &kPoint);
    m_uiChildDlgOffset = kPoint.y;

    if (!CreateOptionsViews())
    {
        NiMessageBox(
            "Error in NiRendererTabController::InitDialog()\n"
            "Failed to create at least one NiRendererOptionsView class.",
            "NiRendererTabController Error");
        EndDialog(m_pDlgHandle, 0);
        return NULL;
    }

    // Init and fill controls in this dialog
    InitDialogControls();

    return m_pDlgHandle;
}

//---------------------------------------------------------------------------
void NiRendererTabController::InitDialogControls()
{
    NIASSERT(m_pDlgHandle);

    // Create entries in renderer selection combo
    for (unsigned int i = 0; i < m_kViewArray.GetSize(); i++)
    {
        NiBaseRendererOptionsView* pkROV = m_kViewArray[i];

        // Insert OptionsView's name in renderer selection combobox
        LPSTR pcName = pkROV->GetName();
        SendDlgItemMessage(
            m_pDlgHandle,
            IDC_RENDERER_SEL_COMBO,
            CB_ADDSTRING,
            0,
            (LPARAM)pcName);

        if (pkROV->IsActive())
            m_uiCurrentViewIdx = i;
    }

    // Set default selection according to default renderer setting
    SendDlgItemMessage(
        m_pDlgHandle,
        IDC_RENDERER_SEL_COMBO,
        CB_SETCURSEL,
        m_uiCurrentViewIdx,
        0);

    m_kViewArray[m_uiCurrentViewIdx]->Activate();
}

//---------------------------------------------------------------------------
// Height adjusting functions for basic / advanced view
//---------------------------------------------------------------------------
unsigned int NiRendererTabController::SetBasicHeight()
{
    // Get maximum advanced OptionsView height
    unsigned int uiMaxHeight = 0;
    for (unsigned int i = 0; i < m_kViewArray.GetSize(); i++)
    {
        unsigned int uiHeight = m_kViewArray[i]->SetBasicHeight();
        if (uiHeight > uiMaxHeight)
            uiMaxHeight = uiHeight;
    }

    // Set size of control to size of OV dialog + rend sel combo
    RECT kRect;
    GetWindowRect(m_pDlgHandle, &kRect);
    SetWindowPos(
        m_pDlgHandle,
        HWND_TOP,
        0,
        0,
        kRect.right - kRect.left,
        uiMaxHeight + m_uiChildDlgOffset,
        SWP_NOMOVE);

    return uiMaxHeight + m_uiChildDlgOffset;
}

//---------------------------------------------------------------------------
unsigned int NiRendererTabController::SetAdvancedHeight()
{
    // Get maximum basic OptionsView height
    unsigned int uiMaxHeight = 0;
    for (unsigned int i = 0; i < m_kViewArray.GetSize(); i++)
    {
        unsigned int uiHeight = m_kViewArray[i]->SetAdvancedHeight();
        if (uiHeight > uiMaxHeight)
            uiMaxHeight = uiHeight;
    }

    // Set size of control to size of OV dialog + rend sel combo
    RECT kRect;
    GetWindowRect(m_pDlgHandle, &kRect);
    SetWindowPos(
        m_pDlgHandle,
        HWND_TOP,
        0,
        0,
        kRect.right - kRect.left,
        uiMaxHeight + m_uiChildDlgOffset,
        SWP_NOMOVE);

    return uiMaxHeight + m_uiChildDlgOffset;
}

//---------------------------------------------------------------------------
// Window messages processing functions
//---------------------------------------------------------------------------
bool NiRendererTabController::ProcessCommand(
    NiWindowRef,
    WORD wID,
    WORD wNotifyCode)
{
    // Process renderer type change
    if (wID == IDC_RENDERER_SEL_COMBO && wNotifyCode == CBN_SELCHANGE)
    {
        LRESULT lResult = SendDlgItemMessage(
            m_pDlgHandle,
            IDC_RENDERER_SEL_COMBO,
            CB_GETCURSEL,
            0,
            0);

        if (lResult == CB_ERR)
            return false;

        m_kViewArray[m_uiCurrentViewIdx]->Deactivate();
        m_uiCurrentViewIdx = PtrToUint(lResult);
        m_kViewArray[m_uiCurrentViewIdx]->Activate();

        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
BOOL CALLBACK NiRendererTabController::RendererTabWndProc(
    HWND pDlg,
    UINT uiMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    // Get pointer to class to pass it messages
    LONG_PTR pkPtr = GetWindowLongPtr(pDlg, GWL_USERDATA);
    NiRendererTabController* pkTabCtrl =
        reinterpret_cast<NiRendererTabController*>(pkPtr);

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        // Pass command to TabController and optionally then to OptionsView
        WORD wID = LOWORD(wParam);
        WORD wNotifyCode = HIWORD(wParam);
        NiWindowRef pDlgHandle = (NiWindowRef)lParam;

        if (!pkTabCtrl)
            return false;

        if (pkTabCtrl->ProcessCommand(pDlgHandle, wID, wNotifyCode))
            return TRUE;
    }

    return FALSE;
}
