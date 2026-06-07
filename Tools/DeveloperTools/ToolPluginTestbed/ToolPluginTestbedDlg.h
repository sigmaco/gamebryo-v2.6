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

#ifndef TOOLPLUGINTESTBEDDLG_H
#define TOOLPLUGINTESTBEDDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CToolPluginTestbedDlg : public CDialog
{
// Construction
public:
    CToolPluginTestbedDlg(CWnd* pParent = NULL);

protected:
    class TestbedInfo
    {
    public:
        TestbedInfo() {m_bViewSelected = false;}

        NiScriptInfoPtr m_spProcessScript;
        NiScriptInfoPtr m_spViewerScript;
        NiScriptInfoPtr m_spExportScript;

        NiString m_strScriptDir;
        NiString m_strImportFile;
        NiString m_strExportFile;
        bool m_bViewSelected;
    };

    TestbedInfo* m_pkInfo;
    CString m_strLastProcessScript;
    CString m_strLastViewerScript;
    CString m_strLastExportScript;

    void LoadLastUsedSettings();
    void SaveLastUsedSettings();
    void SyncDialogToInfo();
    void BuildProcessCombo();
    void BuildExportCombo();
    void BuildViewCombo();
    void RunScriptManagerDlg(NiScriptInfoPtr& spNewScript, NiString strType);

public:
// Dialog Data
    //{{AFX_DATA(CToolPluginTestbedDlg)
    enum { IDD = IDD_TOOLPLUGINTESTBED_DIALOG };
    CButton m_wndViewOptionsBtn;
    CButton m_wndRunButton;
    CButton m_wndProcessOptionsBtn;
    CButton m_wndExportOptionsBtn;
    CButton m_wndExportButton;
    CButton m_wndScriptDirSel;
    CEdit   m_wndExportFilename;
    CButton m_wndExportRadio;
    CComboBox   m_wndViewCombo;
    CEdit   m_wndScriptDirectoryEdit;
    CComboBox   m_wndProcessCombo;
    CEdit   m_wndImportPluginEdit;
    CButton m_wndImportOptionsButton;
    CEdit   m_wndImportFilename;
    CButton m_wndImportButton;
    CComboBox   m_wndExportCombo;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolPluginTestbedDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CToolPluginTestbedDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnScriptDirSel();
    afx_msg void OnImportButton();
    afx_msg void OnSelchangeProcessCombo();
    afx_msg void OnProcessOptionsBtn();
    afx_msg void OnSelchangeViewCombo();
    afx_msg void OnViewOptionsBtn();
    afx_msg void OnViewRadio();
    afx_msg void OnExportRadio();
    afx_msg void OnExportButton();
    afx_msg void OnSelchangeExportCombo();
    afx_msg void OnExportOptionsBtn();
    afx_msg void OnRun();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef TOOLPLUGINTESTBEDDLG_H
