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

#ifndef NISETTINGSDIALOG_H
#define NISETTINGSDIALOG_H

#include <NiMemObject.h>

// Forward declaration
class NiBaseTabController;

class NiSettingsDialog: public NiRefObject
{
public:
    static bool IsFinishedOK();

    NiSettingsDialog();
    virtual ~NiSettingsDialog();

    // Add custom tab to dlg's tab control. Must be called before InitDialog
    bool AddTabController(NiBaseTabController* pkTabCtrl);

    // Initialize the dialog. Return code must be checked.
    bool InitDialog();

    // Show dialog. Must be called after InitDialog().
    bool ShowDialog();

    // Show messagebox with info about error, returned by GetLastError()
    static void ReportWinAPIError(DWORD dwErrorCode, char* pcErrorText);

protected:
    bool m_bViewAdvanced;
    NiWindowRef m_pDlgHandle;
    NiTPrimitiveArray<NiBaseTabController*> m_kTabArray;
    bool m_bInitialized;
    static bool ms_bDlgOK;

    // Create always present (Renderer) NiXXXTabController
    bool CreateInternalTabControllers(NiWindowRef pDlgHandle);
    // Initialize / delete tab controller(s)
    NiWindowRef InitTabController(unsigned int uiIdx);
    void DeleteTabControllers();

    // Center window on the screen
    void CenterWindow();

    // Init controls in dialog
    bool InitDialogControls();

    // Move control up or down for changing to basic / advanced size
    void RepositionControl(int iControl, int iDeltaHeight);
    void ChangeSize();

    // Functions to react on user input
    bool ChangeAdvanced();
    bool ChangeNoDialog();
    bool ChangeSaveSettings();
    void ActivateTab(unsigned int uiTabIdx);
    void DeactivateTab(unsigned int uiTabIdx);

private:
    // Message processing functions
    bool ProcessCommand(NiWindowRef pDlgHandle, WORD wID, WORD wNotifyCode);
    static BOOL CALLBACK SettingsWndProc(
        HWND pDlg,
        UINT uiMsg,
        WPARAM wParam,
        LPARAM lParam);
};

#include "NiSettingsDialog.inl"

#endif // NISETTINGSDIALOG_H