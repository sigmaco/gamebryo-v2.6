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

#include "stdafx.h"
#include "ToolPluginTestbed.h"
#include "ToolPluginTestbedDlg.h"
#include <NiStandardSharedData.h>
#include <NiMeshProfileProcessor.h>

#define TPT_SETTINGS "Settings"
#define TPT_STRING_NOT_FOUND "NOTFOUND"
#define TPT_INT_NOT_FOUND -1
#define TPT_SCRIPT_DIRECTORY "Script Directory"
#define TPT_IMPORT_FILE "Import File"
#define TPT_EXPORT_FILE "Export File"
#define TPT_PROCESS_SCRIPT "Process Script"
#define TPT_VIEWER_SCRIPT "Viewer Script"
#define TPT_EXPORT_SCRIPT "Export Script"
#define TPT_VIEW_SELECTED "View Selected"

//---------------------------------------------------------------------------
// CAboutDlg dialog used for App About
//---------------------------------------------------------------------------
class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
//---------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CToolPluginTestbedDlg dialog
//---------------------------------------------------------------------------
CToolPluginTestbedDlg::CToolPluginTestbedDlg(CWnd* pParent)
    : CDialog(CToolPluginTestbedDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CToolPluginTestbedDlg)
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CToolPluginTestbedDlg)
    DDX_Control(pDX, IDC_VIEW_OPTIONS_BTN, m_wndViewOptionsBtn);
    DDX_Control(pDX, IDC_RUN, m_wndRunButton);
    DDX_Control(pDX, IDC_PROCESS_OPTIONS_BTN, m_wndProcessOptionsBtn);
    DDX_Control(pDX, IDC_EXPORT_OPTIONS_BTN, m_wndExportOptionsBtn);
    DDX_Control(pDX, IDC_EXPORT_BUTTON, m_wndExportButton);
    DDX_Control(pDX, IDC_SCRIPT_DIR_SEL, m_wndScriptDirSel);
    DDX_Control(pDX, IDC_EXPORT_FILENAME, m_wndExportFilename);
    DDX_Control(pDX, IDC_VIEW_COMBO, m_wndViewCombo);
    DDX_Control(pDX, IDC_SCRIPT_DIRECTORY, m_wndScriptDirectoryEdit);
    DDX_Control(pDX, IDC_PROCESS_COMBO, m_wndProcessCombo);
    DDX_Control(pDX, IDC_IMPORT_FILENAME, m_wndImportFilename);
    DDX_Control(pDX, IDC_IMPORT_BUTTON, m_wndImportButton);
    DDX_Control(pDX, IDC_EXPORT_COMBO, m_wndExportCombo);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CToolPluginTestbedDlg, CDialog)
    //{{AFX_MSG_MAP(CToolPluginTestbedDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_SCRIPT_DIR_SEL, OnScriptDirSel)
    ON_BN_CLICKED(IDC_IMPORT_BUTTON, OnImportButton)
    ON_CBN_SELCHANGE(IDC_PROCESS_COMBO, OnSelchangeProcessCombo)
    ON_BN_CLICKED(IDC_PROCESS_OPTIONS_BTN, OnProcessOptionsBtn)
    ON_CBN_SELCHANGE(IDC_VIEW_COMBO, OnSelchangeViewCombo)
    ON_BN_CLICKED(IDC_VIEW_OPTIONS_BTN, OnViewOptionsBtn)
    ON_BN_CLICKED(IDC_VIEW_RADIO, OnViewRadio)
    ON_BN_CLICKED(IDC_EXPORT_RADIO, OnExportRadio)
    ON_BN_CLICKED(IDC_EXPORT_BUTTON, OnExportButton)
    ON_CBN_SELCHANGE(IDC_EXPORT_COMBO, OnSelchangeExportCombo)
    ON_BN_CLICKED(IDC_EXPORT_OPTIONS_BTN, OnExportOptionsBtn)
    ON_BN_CLICKED(IDC_RUN, OnRun)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CToolPluginTestbedDlg message handlers
//---------------------------------------------------------------------------
BOOL CToolPluginTestbedDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    
    m_pkInfo = NiExternalNew TestbedInfo;
    LoadLastUsedSettings();
    SyncDialogToInfo();

    return TRUE;  // return TRUE unless you set the focus to a control
}
//---------------------------------------------------------------------------
BOOL CToolPluginTestbedDlg::DestroyWindow() 
{
    SaveLastUsedSettings();
    NiExternalDelete m_pkInfo;

    return CDialog::DestroyWindow();
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnPaint() 
{
    // If you add a minimize button to your dialog, you will need the code
    // below to draw the icon.  For MFC applications using the document/view
    // model, this is automatically done for you by the framework.
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}
//---------------------------------------------------------------------------
HCURSOR CToolPluginTestbedDlg::OnQueryDragIcon()
{
    // The system calls this to obtain the cursor to display while the user
    // drags the minimized window.

    return (HCURSOR) m_hIcon;
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::LoadLastUsedSettings()
{
    CString strProfileString;
    UINT uiProfileInt;

    // Load last script directory from registry.
    strProfileString = AfxGetApp()->GetProfileString(TPT_SETTINGS,
        TPT_SCRIPT_DIRECTORY, TPT_STRING_NOT_FOUND);
    if (strProfileString != TPT_STRING_NOT_FOUND)
    {
        NiScriptTemplateManager::GetInstance()->RemoveAllScripts();
        if (NiScriptTemplateManager::GetInstance()->AddScriptDirectory(
            (const char*) strProfileString, true, true) == SUCCESS)
        {
            m_pkInfo->m_strScriptDir = strProfileString;
        }
        else
        {
            AfxGetApp()->WriteProfileString(TPT_SETTINGS,
                TPT_SCRIPT_DIRECTORY, NULL);
        }
    }

    // Load last import filename from registry.
    strProfileString = AfxGetApp()->GetProfileString(TPT_SETTINGS,
        TPT_IMPORT_FILE, TPT_STRING_NOT_FOUND);
    if (strProfileString != TPT_STRING_NOT_FOUND)
    {
        m_pkInfo->m_strImportFile = strProfileString;
    }

    // Load last export filename from registry.
    strProfileString = AfxGetApp()->GetProfileString(TPT_SETTINGS,
        TPT_EXPORT_FILE, TPT_STRING_NOT_FOUND);
    if (strProfileString != TPT_STRING_NOT_FOUND)
    {
        m_pkInfo->m_strExportFile = strProfileString;
    }

    // Load last process script name from registry.
    strProfileString = AfxGetApp()->GetProfileString(TPT_SETTINGS,
        TPT_PROCESS_SCRIPT, TPT_STRING_NOT_FOUND);
    if (strProfileString != TPT_STRING_NOT_FOUND)
    {
        m_strLastProcessScript = strProfileString;
    }

    // Load last viewer script name from registry.
    strProfileString = AfxGetApp()->GetProfileString(TPT_SETTINGS,
        TPT_VIEWER_SCRIPT, TPT_STRING_NOT_FOUND);
    if (strProfileString != TPT_STRING_NOT_FOUND)
    {
        m_strLastViewerScript = strProfileString;
    }

    // Load last export script name from registry.
    strProfileString = AfxGetApp()->GetProfileString(TPT_SETTINGS,
        TPT_EXPORT_SCRIPT, TPT_STRING_NOT_FOUND);
    if (strProfileString != TPT_STRING_NOT_FOUND)
    {
        m_strLastExportScript = strProfileString;
    }

    // Load last view selected setting from registry.
    uiProfileInt = AfxGetApp()->GetProfileInt(TPT_SETTINGS,
        TPT_VIEW_SELECTED, TPT_INT_NOT_FOUND);
    if (uiProfileInt != TPT_INT_NOT_FOUND)
    {
        m_pkInfo->m_bViewSelected = (uiProfileInt == 1 ? true : false);
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::SaveLastUsedSettings()
{
    const char* pcProfileString;

    // Save last script directory to registry.
    if (!m_pkInfo->m_strScriptDir.IsEmpty())
    {
        pcProfileString = m_pkInfo->m_strScriptDir;
    }
    else
    {
        pcProfileString = NULL;
    }
    AfxGetApp()->WriteProfileString(TPT_SETTINGS, TPT_SCRIPT_DIRECTORY,
        pcProfileString);

    // Save last import filename to registry.
    if (!m_pkInfo->m_strImportFile.IsEmpty())
    {
        pcProfileString = m_pkInfo->m_strImportFile;
    }
    else
    {
        pcProfileString = NULL;
    }
    AfxGetApp()->WriteProfileString(TPT_SETTINGS, TPT_IMPORT_FILE,
        pcProfileString);

    // Save last export filename to registry.
    if (!m_pkInfo->m_strExportFile.IsEmpty())
    {
        pcProfileString = m_pkInfo->m_strExportFile;
    }
    else
    {
        pcProfileString = NULL;
    }
    AfxGetApp()->WriteProfileString(TPT_SETTINGS, TPT_EXPORT_FILE,
        pcProfileString);

    // Save last process script name to registry.
    if (m_pkInfo->m_spProcessScript)
    {
        pcProfileString = m_pkInfo->m_spProcessScript->GetName();
    }
    else
    {
        pcProfileString = NULL;
    }
    AfxGetApp()->WriteProfileString(TPT_SETTINGS, TPT_PROCESS_SCRIPT,
        pcProfileString);

    // Save last viewer script name to registry.
    if (m_pkInfo->m_spViewerScript)
    {
        pcProfileString = m_pkInfo->m_spViewerScript->GetName();
    }
    else
    {
        pcProfileString = NULL;
    }
    AfxGetApp()->WriteProfileString(TPT_SETTINGS, TPT_VIEWER_SCRIPT,
        pcProfileString);

    // Save last export script name to registry.
    if (m_pkInfo->m_spExportScript)
    {
        pcProfileString = m_pkInfo->m_spExportScript->GetName();
    }
    else
    {
        pcProfileString = NULL;
    }
    AfxGetApp()->WriteProfileString(TPT_SETTINGS, TPT_EXPORT_SCRIPT,
        pcProfileString);

    // Save last view selected setting to registry.
    AfxGetApp()->WriteProfileInt(TPT_SETTINGS, TPT_VIEW_SELECTED,
        m_pkInfo->m_bViewSelected);
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::SyncDialogToInfo()
{
    // Script directory...
    if (!m_pkInfo->m_strScriptDir.IsEmpty())
    {
        m_wndScriptDirectoryEdit.SetWindowText(m_pkInfo->m_strScriptDir);

        m_wndImportButton.EnableWindow(TRUE);
    }
    else
    {
        m_wndScriptDirectoryEdit.SetWindowText("");

        m_wndImportButton.EnableWindow(FALSE);
    }

    // Step 1...
    if (!m_pkInfo->m_strImportFile.IsEmpty())
    {
        m_wndImportFilename.SetWindowText(m_pkInfo->m_strImportFile);

        m_wndProcessCombo.EnableWindow(TRUE);
        m_wndProcessOptionsBtn.EnableWindow(TRUE);
        BuildProcessCombo();
    }
    else
    {
        m_wndImportFilename.SetWindowText("");

        m_wndProcessCombo.EnableWindow(FALSE);
        m_wndProcessOptionsBtn.EnableWindow(FALSE);
        m_wndProcessCombo.ResetContent();
    }

    // Step 2...
    if (m_pkInfo->m_spProcessScript)
    {
        m_wndProcessCombo.SelectString(-1, m_pkInfo->m_spProcessScript
            ->GetName());

        m_wndViewCombo.EnableWindow(TRUE);
        m_wndViewOptionsBtn.EnableWindow(TRUE);
        BuildViewCombo();
        m_wndExportButton.EnableWindow(TRUE);
        m_wndExportCombo.EnableWindow(TRUE);
        m_wndExportOptionsBtn.EnableWindow(TRUE);
        BuildExportCombo();
    }
    else
    {
        m_wndProcessCombo.SetCurSel(-1);

        m_wndViewCombo.EnableWindow(FALSE);
        m_wndViewOptionsBtn.EnableWindow(FALSE);
        m_wndViewCombo.ResetContent();
        m_wndExportButton.EnableWindow(FALSE);
        m_wndExportCombo.EnableWindow(FALSE);
        m_wndExportOptionsBtn.EnableWindow(FALSE);
        m_wndExportCombo.ResetContent();
    }

    // Step 3...
    if (m_pkInfo->m_bViewSelected)
    {
        CheckRadioButton(IDC_VIEW_RADIO, IDC_EXPORT_RADIO, IDC_VIEW_RADIO);
    }
    else
    {
        CheckRadioButton(IDC_VIEW_RADIO, IDC_EXPORT_RADIO, IDC_EXPORT_RADIO);
    }

    if (m_pkInfo->m_spViewerScript)
    {
        m_wndViewCombo.SelectString(-1,
            m_pkInfo->m_spViewerScript->GetName());
    }
    else
    {
        m_wndViewCombo.SetCurSel(-1);
    }

    m_wndExportFilename.SetWindowText(m_pkInfo->m_strExportFile);

    if (m_pkInfo->m_spExportScript)
    {
        m_wndExportCombo.SelectString(-1,
            m_pkInfo->m_spExportScript->GetName());
    }
    else
    {
        m_wndExportCombo.SetCurSel(-1);
    }

    // Run...
    if ((m_pkInfo->m_spViewerScript && m_pkInfo->m_bViewSelected) ||
        (!m_pkInfo->m_strExportFile.IsEmpty() && m_pkInfo->m_spExportScript &&
            !m_pkInfo->m_bViewSelected))
    {
        m_wndRunButton.EnableWindow(true);
    }
    else
    {
        m_wndRunButton.EnableWindow(false);
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::BuildProcessCombo()
{
    m_wndProcessCombo.ResetContent();
    NiScriptTemplateManager* pkManager = NiScriptTemplateManager::
        GetInstance();
    NIASSERT(pkManager);

    NiScriptInfo* pkSelectedScript = NULL;
    NiScriptInfo* pkFirstScript = NULL;

    for (unsigned int ui = 0; ui < pkManager->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkScript = pkManager->GetScriptAt(ui);
        if (pkScript && pkScript->GetType().EqualsNoCase("PROCESS"))
        {
            if (!pkFirstScript)
            {
                pkFirstScript = pkScript;
            }

            if (!pkSelectedScript && !m_strLastProcessScript.IsEmpty() &&
                pkScript->GetName() == (const char*) m_strLastProcessScript)
            {
                pkSelectedScript = pkScript;
            }

            int iIndex = m_wndProcessCombo.AddString(pkScript->GetName());
            m_wndProcessCombo.SetItemData(iIndex, 
                (DWORD) PtrToUlong(pkScript));
        }
    }

    if (!pkSelectedScript)
    {
        pkSelectedScript = pkFirstScript;
    }

    if (!m_pkInfo->m_spProcessScript)
    {
        m_pkInfo->m_spProcessScript = pkSelectedScript;
        m_strLastProcessScript.Empty();
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::BuildViewCombo()
{
    m_wndViewCombo.ResetContent();
    NiScriptTemplateManager* pkManager = NiScriptTemplateManager::
        GetInstance();
    NIASSERT(pkManager);

    NiScriptInfo* pkSelectedScript = NULL;
    NiScriptInfo* pkFirstScript = NULL;

    for (unsigned int ui = 0; ui < pkManager->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkScript = pkManager->GetScriptAt(ui);
        if (pkScript && pkScript->GetType().EqualsNoCase("VIEWER"))
        {
            if (!pkFirstScript)
            {
                pkFirstScript = pkScript;
            }

            if (!pkSelectedScript && !m_strLastViewerScript.IsEmpty() &&
                pkScript->GetName() == (const char*) m_strLastViewerScript)
            {
                pkSelectedScript = pkScript;
            }

            int iIndex = m_wndViewCombo.AddString(pkScript->GetName());
            m_wndViewCombo.SetItemData(iIndex,
                (DWORD) PtrToUlong(pkScript));
        }
    }

    if (!pkSelectedScript)
    {
        pkSelectedScript = pkFirstScript;
    }

    if (!m_pkInfo->m_spViewerScript)
    {
        m_pkInfo->m_spViewerScript = pkSelectedScript;
        m_strLastViewerScript.Empty();
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::BuildExportCombo()
{
    m_wndExportCombo.ResetContent();
    NiScriptTemplateManager* pkManager = NiScriptTemplateManager::
        GetInstance();
    NIASSERT(pkManager);

    NiScriptInfo* pkSelectedScript = NULL;
    NiScriptInfo* pkFirstScript = NULL;

    for (unsigned int ui = 0; ui < pkManager->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkScript = pkManager->GetScriptAt(ui);
        if (pkScript && pkScript->GetType().EqualsNoCase("EXPORT"))
        {
            if (!pkFirstScript)
            {
                pkFirstScript = pkScript;
            }

            if (!pkSelectedScript && !m_strLastExportScript.IsEmpty() &&
                pkScript->GetName() == (const char*) m_strLastExportScript)
            {
                pkSelectedScript = pkScript;
            }

            int iIndex = m_wndExportCombo.AddString(pkScript->GetName());
            m_wndExportCombo.SetItemData(iIndex, 
                (DWORD) PtrToUlong(pkScript));
        }
    }

    if (!pkSelectedScript)
    {
        pkSelectedScript = pkFirstScript;
    }

    if (!m_pkInfo->m_spExportScript)
    {
        m_pkInfo->m_spExportScript = pkSelectedScript;
        m_strLastExportScript.Empty();
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::RunScriptManagerDlg(NiScriptInfoPtr& spNewScript,
    NiString strType)
{
    NiModuleRef hModule = GetModuleHandle(NIPLUGINTOOLKIT_MODULE_NAME);
    
    NiScriptInfoSet& kMasterSet = NiScriptTemplateManager::GetInstance()
        ->GetScriptInfoSet();
    NiScriptInfoSetPtr spClonedSet = kMasterSet.Clone();

    NiScriptInfoPtr spOriginalScript = spClonedSet->GetScript(
        spNewScript->GetName());
    NIASSERT(spOriginalScript);

    NiScriptManagementDialog kDialog(hModule, spOriginalScript, spClonedSet,
        m_hWnd, strType);
    if (kDialog.DoModal() == IDOK)
    {
        for (unsigned int ui = 0; ui < spClonedSet->GetScriptCount(); ui++)
        {
            NiScriptInfo* pkNewScript = spClonedSet->GetScriptAt(ui);
            kMasterSet.ReplaceScript(pkNewScript);
        }
        spNewScript = kDialog.m_spInfo;

        if (strType.EqualsNoCase("PROCESS"))
        {
            BuildProcessCombo();
        }
        else if (strType.EqualsNoCase("VIEWER"))
        {
            BuildViewCombo();
        }
        else if (strType.EqualsNoCase("EXPORT"))
        {
            BuildExportCombo();
        }
        else
        {
            // Unknown type.
            NIASSERT(false);
        }
        SyncDialogToInfo();
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnScriptDirSel() 
{
    bool bDone = false;
    while (!bDone)
    {
        char acFolderPath[MAX_PATH];

        BROWSEINFO kBrowseInfo;
        kBrowseInfo.hwndOwner = m_hWnd;
        kBrowseInfo.pidlRoot = NULL;
        kBrowseInfo.pszDisplayName = acFolderPath;
        kBrowseInfo.lpszTitle = "Please select the location from which to "
            "load all script files:";
        kBrowseInfo.ulFlags = 0;
        kBrowseInfo.lpfn = NULL;

        LPITEMIDLIST pidl = SHBrowseForFolder(&kBrowseInfo);
        if (pidl != NULL)
        {
            BOOL bSuccess = SHGetPathFromIDList(pidl, acFolderPath);
            if (bSuccess && !m_pkInfo->m_strScriptDir.EqualsNoCase(
                acFolderPath))
            {
                NiScriptTemplateManager::GetInstance()->RemoveAllScripts();
                if (NiScriptTemplateManager::GetInstance()
                    ->AddScriptDirectory(acFolderPath, true) == SUCCESS)
                {
                    m_pkInfo->m_strScriptDir = acFolderPath;
                    SyncDialogToInfo();
                    bDone = true;
                }
                else
                {
                    MessageBox("An error occurred while loading scripts from "
                        "the selected location.\nPlease choose a location "
                        "that contains valid NiPluginToolkit script files.",
                        "Error Loading Scripts", MB_OK | MB_ICONERROR);
                }
            }
        }
        else
        {
            bDone = true;
        }

        LPMALLOC pMalloc;
        HRESULT hRes = SHGetMalloc(&pMalloc);
        if (hRes == NOERROR)
        {
            pMalloc->Free(pidl);
        }
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnImportButton() 
{
    NiString strPath, strFile, strExtension;
    if (NiPluginHelpers::DoImportFileDialog(strPath, strFile, strExtension,
        m_hWnd))
    {
        m_pkInfo->m_strImportFile = strPath + strFile + strExtension;
        SyncDialogToInfo();
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnSelchangeProcessCombo() 
{
    m_pkInfo->m_spProcessScript = (NiScriptInfo*) m_wndProcessCombo.
        GetItemData(m_wndProcessCombo.GetCurSel());
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnProcessOptionsBtn() 
{
    RunScriptManagerDlg(m_pkInfo->m_spProcessScript, "PROCESS");
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnViewRadio() 
{
    m_pkInfo->m_bViewSelected = true;
    SyncDialogToInfo();
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnExportRadio() 
{
    m_pkInfo->m_bViewSelected = false;
    SyncDialogToInfo();
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnSelchangeViewCombo() 
{
    m_pkInfo->m_spViewerScript = (NiScriptInfo*) m_wndViewCombo.GetItemData(
        m_wndViewCombo.GetCurSel());
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnViewOptionsBtn() 
{
    RunScriptManagerDlg(m_pkInfo->m_spViewerScript, "VIEWER");
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnExportButton() 
{
    NiString strPath, strFile, strExtension;
    if (NiPluginHelpers::DoExportFileDialog(strPath, strFile, strExtension,
        m_hWnd))
    {
        m_pkInfo->m_strExportFile = strPath + strFile + strExtension;
        SyncDialogToInfo();
    }
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnSelchangeExportCombo() 
{
    m_pkInfo->m_spExportScript = (NiScriptInfo*) m_wndExportCombo.GetItemData(
        m_wndExportCombo.GetCurSel());
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnExportOptionsBtn() 
{
    RunScriptManagerDlg(m_pkInfo->m_spExportScript, "EXPORT");
}
//---------------------------------------------------------------------------
void CToolPluginTestbedDlg::OnRun() 
{
    NiBatchExecutionResultPtr spResult;

    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();

    pkDataList->Lock();
    {
        pkDataList->DeleteAll();

        NiSceneGraphSharedData* pkSceneGraphSharedData = NiTCreate<
            NiSceneGraphSharedData>();

        NiRendererSharedData* pkRendererSharedData = NiTCreate<
            NiRendererSharedData>();
        pkRendererSharedData->SetBackgroundColor(NiColor(0.0f, 0.0f, 1.0f));
        pkRendererSharedData->SetWindowSize(NiPoint2(640.0f, 480.0f));
        pkRendererSharedData->SetFullscreen(false);
        
        NiTimerSharedData* pkTimerSharedData = NiTCreate<NiTimerSharedData>();
        pkTimerSharedData->SetStartTime(0.0f);
        pkTimerSharedData->SetEndTime(100.0f);
        pkTimerSharedData->SetTimeMode(NiTimerSharedData::LOOP);
        
        pkDataList->Insert(pkSceneGraphSharedData);
        pkDataList->Insert(pkRendererSharedData);
        pkDataList->Insert(pkTimerSharedData);

        NiExporterOptionsSharedDataPtr spEOSharedData =
            NiNew NiExporterOptionsSharedData();
        spEOSharedData->SetWriteResultsToLog(false);
        spEOSharedData->SetViewerRun(m_pkInfo->m_bViewSelected);
        spEOSharedData->SetOptimizeMeshForViewer(m_pkInfo->m_bViewSelected);
        pkDataList->Insert(spEOSharedData);
    }
    pkDataList->Unlock();

    // Import script.
    NiString strResult = NiPluginHelpers::ImportAndExecute(
        NiImportPluginInfo::REPLACE,
        m_pkInfo->m_strImportFile);

    if (strResult.IsEmpty())
    {
        MessageBox("The import script failed to execute successfully.\n"
            "No further scripts will be run at this time.", "Import Script "
            "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Process script.
    spResult = NiFramework::GetFramework().ExecuteScript(
        m_pkInfo->m_spProcessScript);
    if (!spResult->WasSuccessful())
    {
        MessageBox("The process script failed to execute successfully.\n"
            "No further scripts will be run at this time.", "Process Script "
            "Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (m_pkInfo->m_bViewSelected)
    {
        // Viewer script.
        spResult = NiFramework::GetFramework().ExecuteScript(
            m_pkInfo->m_spViewerScript);
        if (!spResult->WasSuccessful())
        {
            MessageBox("The viewer script failed to execute successfully.\n"
                "No further scripts will be run at this time.", "Viewer "
                "Script Error", MB_OK | MB_ICONERROR);
            return;
        }
    }
    else
    {
        // Export script.
        NiScriptInfoDialogs::CompileImportExportInfo(
            m_pkInfo->m_strExportFile, m_pkInfo->m_spExportScript, false,
            false);
        spResult = NiFramework::GetFramework().ExecuteScript(
            m_pkInfo->m_spExportScript);
        if (!spResult->WasSuccessful())
        {
            MessageBox("The export script failed to execute successfully.\n"
                "No further scripts will be run at this time.", "Export "
                "Script Error", MB_OK | MB_ICONERROR);
            return;
        }
    }

    pkDataList->Lock();
    pkDataList->DeleteAll();
    pkDataList->Unlock();

    MessageBox("All scripts executed successfully.", "Script Execution "
        "Success", MB_OK | MB_ICONINFORMATION);
}
//---------------------------------------------------------------------------

