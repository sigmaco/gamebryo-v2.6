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

// AssetViewer.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "MainFrm.h"
#include "NifDoc.h"
#include "NifRenderView.h"
#include "NifUserPreferences.h"
#include "DiscardOpenFilesDlg.h"
#include "AssociateFileTypes.h"
#include <NiMaterialToolkit.h>
#include <NiVersion.h>
#include <NiSystem.h>
#include "NifPropertyWindowFactory.h"
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>
#include <NiGeometryConverter.h>

// Included for registry manipulation
#include <windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CAssetViewerApp
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAssetViewerApp, CWinApp)
    //{{AFX_MSG_MAP(CAssetViewerApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CAssetViewerApp construction
//---------------------------------------------------------------------------
CAssetViewerApp::CAssetViewerApp()
{
    // Place all significant initialization in InitInstance
    NiGeometryConverter::SetAutoGeometryConversion(true);
}
//---------------------------------------------------------------------------
// The one and only CAssetViewerApp object
//---------------------------------------------------------------------------
CAssetViewerApp theApp;

//---------------------------------------------------------------------------
// Utility function for recursively copying one key to another.  Assumes
// that hOldKey and hNewKey have been opened successfully.
//
// Returns true if no errors, false otherwise.
//---------------------------------------------------------------------------
inline bool RegCopyTreeEx(HKEY hOldKey, int, HKEY hNewKey)
{
    DWORD dwIndex = 0;
    char acNameBuffer[1024];
    char acClassBuffer[256];

    bool bDone = false;

    // Loop through subkeys of hOldKey and recurse through them
    while(!bDone)
    {
        DWORD dwNameSize = (DWORD)sizeof(acNameBuffer);
        DWORD dwClassSize = (DWORD)sizeof(acClassBuffer);

        LONG lRet = RegEnumKeyEx(hOldKey, dwIndex++, acNameBuffer,
            &dwNameSize, 0, acClassBuffer, &dwClassSize,
            NULL);

        switch (lRet)
        {
        case ERROR_SUCCESS:
            {
                // copy child keys recursively
                HKEY hOldSubKey;
                HKEY hNewSubKey;

                LONG lRetOld = RegOpenKeyEx(hOldKey, (LPCTSTR)acNameBuffer,
                    0, KEY_ALL_ACCESS, &hOldSubKey);
                LONG lRetNew = RegCreateKeyEx(hNewKey, (LPCTSTR)acNameBuffer,
                    0, acClassBuffer, 0, KEY_ALL_ACCESS, NULL, &hNewSubKey,
                    NULL);
 
                if (lRetOld == ERROR_SUCCESS && lRetNew == ERROR_SUCCESS)
                {
                    if (!RegCopyTreeEx(hOldSubKey, 0, hNewSubKey))
                        return false;
                }
                else
                {
                    return false;
                }
                break;
            }

        case ERROR_NO_MORE_ITEMS:
            // all done
            bDone = true;
            break;

        case ERROR_MORE_DATA:
            // shouldn't happen
            NIASSERT("!Static array not big enough.");
            return false;

        default:
            // unhandled error
            return false;
        }
    }

    // Now copy all the values.
    dwIndex = 0;
    bDone = false;
    BYTE aDataBuffer[1024];

    while (!bDone)
    {
        DWORD dwNameLen = sizeof(acNameBuffer);
        DWORD dwDataLen = sizeof(aDataBuffer);
        DWORD dwType;

        LONG lRet = RegEnumValue(hOldKey, dwIndex++, acNameBuffer,
            &dwNameLen, 0, &dwType, aDataBuffer, &dwDataLen);

        switch (lRet)
        {
        case ERROR_SUCCESS:
            {
                // copy key
                LONG lRet = RegSetValueEx(hNewKey, acNameBuffer, 0,
                    dwType, aDataBuffer, dwDataLen);

                if (lRet != ERROR_SUCCESS)
                    return false;
            }
            break;

        case ERROR_NO_MORE_ITEMS:
            bDone = true;
            break;

        default:
            // unhandled error
            return false;
        }
    }

    // success
    return true;
}
//---------------------------------------------------------------------------
// Utility function for recursively copying one registry key to another.
// Copies hOldRoot/pcOldKey into hNewRoot/pcNewKey if and only if
// hNewRoot/pcNewKey does not exist and hOldRoot/pcOldKey does.
//---------------------------------------------------------------------------
inline void CopyIfNotExist(HKEY hOldRoot, const char* pcOldKey,
    HKEY hNewRoot, const char* pcNewKey)
{
    // Determine if the new settings exist.
    HKEY hNewKey;
    LONG lRet = RegOpenKeyEx(hNewRoot, (LPCTSTR) pcNewKey, 0, KEY_ALL_ACCESS,
        &hNewKey);

    // if new not found
    if (lRet != ERROR_SUCCESS)
    {
        HKEY hOldKey;

        lRet = RegOpenKeyEx(hOldRoot, (LPCTSTR)pcOldKey,
            0, KEY_ALL_ACCESS, &hOldKey);

        // If old found...
        if (lRet == ERROR_SUCCESS)
        {
            // ...then copy tree.
            DWORD dwDisposition;
            lRet = RegCreateKeyEx(hNewRoot, (LPCTSTR)pcNewKey,
                0, NULL, 0, KEY_ALL_ACCESS, NULL, &hNewKey,
                &dwDisposition);
            NIASSERT(lRet == ERROR_SUCCESS &&
                dwDisposition == REG_CREATED_NEW_KEY);
            RegCopyTreeEx(hOldKey, 0, hNewKey);

            RegCloseKey(hNewKey);
            RegCloseKey(hOldKey);
        }
    }
    else
    {
        RegCloseKey(hNewKey);
    }
}

//---------------------------------------------------------------------------
// CAssetViewerApp initialization
//---------------------------------------------------------------------------
BOOL CAssetViewerApp::InitInstance()
{
    NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
        NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false)
#else
        NiExternalNew NiStandardAllocator()
#endif
        );

    NiInit(pkInitOptions);

    CNifPropertyWindowFactory::Init();

    // Turn default mipmapping on
    NiTexture::SetMipmapByDefault(true);

    NiMaterialToolkit* pkShaderKit = NiMaterialToolkit::CreateToolkit();
/*
    // Emergent/AssetViewer used to be NDL/SceneViewer.  We will try to 
    // preserve registry settings across this change.
    CopyIfNotExist(HKEY_CURRENT_USER, 
        "Software\\NDL\\Gamebryo Scene Viewer", HKEY_CURRENT_USER,
        "Software\\Emergent Game Technologies\\Gamebryo Asset Viewer");
    CopyIfNotExist(HKEY_CURRENT_USER, 
        "Software\\Emergent\\Gamebryo Asset Viewer", HKEY_CURRENT_USER,
        "Software\\Emergent Game Technologies\\Gamebryo Asset Viewer");

    // Set the registry key under which our settings are stored.
    SetRegistryKey(_T("Emergent Game Technologies"));
*/
    const char* pcAppFilename = __targv[0];

    char acString[256];
    NiSprintf(acString, 256, "%s", pcAppFilename);
    char* pcEnd = strrchr(acString, '\\');
    if (pcEnd)
    {
        pcEnd++;
        pcEnd[0] = '\0';
    }
    else
    {
        char acCurDirectory[NI_MAX_PATH];
        GetCurrentDirectory(NI_MAX_PATH, acCurDirectory);
        NiSprintf(acString, 256, "%s\\", acCurDirectory);
    }
    m_kApplicationDirectory = _T(acString);

    NiSprintf(acString, 256, "%sAssetViewer.ini", m_kApplicationDirectory);
    size_t stLength = strlen(acString) + 1;
    m_pszProfileName = NiExternalAlloc(char, stLength);
    NiStrcpy((char *)m_pszProfileName, stLength, acString);

    LoadStdProfileSettings(10); // Load standard INI file options
                                //(including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CNifDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CNifRenderView));
    AddDocTemplate(pDocTemplate);

    // Enable DDE Execute open
    EnableShellOpen();

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Detect D3D10 command-line switch here.
    CString strCmdLine = m_lpCmdLine;
    strCmdLine.MakeUpper();
    if (strCmdLine.Find("-D3D10") > -1)
    {
        g_bD3D10 = true;
    }

    // Associate .NIF file with AssetViewer in Windows shell
    // If user started AssetViewer with D3D10 option, include
    // that option in the shell association.
    const char* pcOptions = g_bD3D10 ? "-D3D10" : ""; 

    char acIcon[NI_MAX_PATH];
    NiSprintf( acIcon, NI_MAX_PATH, "%s,1", pcAppFilename);

    AssociateFileTypes::Associate(".nif", pcAppFilename, pcOptions,
        "NIF.Document", "Gamebryo NIF File", acIcon);


    // Dispatch commands specified on the command line
    if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
    {
        // Ensure that everything is properly initialized...
        // NewFile does this.
        CCommandLineInfo cmdNewFile;
        if (!ProcessShellCommand(cmdNewFile))
            return FALSE;
    }

    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // Enable drag/drop open
    m_pMainWnd->DragAcceptFiles();

    // Enable rich edit controls.
    AfxInitRichEdit();

    return TRUE;
}
//---------------------------------------------------------------------------
int CAssetViewerApp::ExitInstance() 
{
    CNifStatisticsManager::Destroy();
    CNifUserPreferences::Destroy();
    CNifPropertyWindowFactory::Shutdown();
    NiMaterialToolkit::DestroyToolkit();
    
    const NiInitOptions* pkInitOptions = NiStaticDataManager
        ::GetInitOptions();
    NiShutdown();
    NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
    NiExternalDelete pkInitOptions;
    NiExternalDelete pkAllocator;

    return CWinApp::ExitInstance();
}
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

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
        // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
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
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// App command to run the dialog
void CAssetViewerApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
//---------------------------------------------------------------------------
void CAssetViewerApp::OnFileNew() 
{
    CNifUserPreferences::Lock();
    bool bPromptOnDiscard = CNifUserPreferences::AccessUserPreferences()
        ->GetPromptOnDiscard();
    CNifUserPreferences::UnLock();

    if (g_bNeedPromptOnDiscard && bPromptOnDiscard)
    {
        CDiscardOpenFilesDlg dlg;
        if (dlg.DoModal() == IDOK)
        {
            if (dlg.m_bDontPromptAgain)
            {
                CNifUserPreferences::Lock();
                CNifUserPreferences::AccessUserPreferences()
                    ->SetPromptOnDiscard(false);
                CNifUserPreferences::UnLock();
            }
        }
        else
        {
            return;
        }
    }

    CWinApp::OnFileNew();
}
//---------------------------------------------------------------------------
BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CString strAboutText = CString(AfxGetAppName()) + "\n"
        "For use with Gamebryo %GAMEBRYO_SDK_VERSION_STRING%\n"
        "\n"
        "Direct3D Version: %D3D_VERSION%\n"
        "\n"
        "Copyright (c) 1996-2008 Emergent Game Technologies.\n"
        "All Rights Reserved\n"
        "\n"
        "This software is supplied under the terms of a license agreement or "
        "nondisclosure agreement with Emergent Game Technologies and may not "
        "be copied or disclosed except in accordance with the terms of that "
        "agreement.\n"
        "\n"
        "Emergent Game Technologies, Chapel Hill, North Carolina 27517\n"
        "http://www.emergent.net\n";

    CString strGamebryoVersion;
    strGamebryoVersion.Format("%s (%s)", GAMEBRYO_SDK_VERSION_STRING,
        GAMEBRYO_BUILD_DATE_STRING);
    CString strD3DVersion = g_bD3D10 ? "D3D10" : "DX9";

    strAboutText.Replace("%GAMEBRYO_SDK_VERSION_STRING%", strGamebryoVersion);
    strAboutText.Replace("%D3D_VERSION%", strD3DVersion);

    GetDlgItem(IDC_RICHEDIT_ABOUTTEXT)->SetWindowText(strAboutText);

    return TRUE;
}
//---------------------------------------------------------------------------

void CAboutDlg::OnBnClickedOk()
{
    // Control notification handler code goes here
    OnOK();
}
