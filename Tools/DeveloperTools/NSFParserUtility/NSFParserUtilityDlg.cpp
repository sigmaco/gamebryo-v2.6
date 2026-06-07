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

// NSFParserUtilityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NSFParserUtility.h"
#include "NSFParserUtilityDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFParserUtilityDlg dialog
//---------------------------------------------------------------------------
NSFParserUtilityDlg* NSFParserUtilityDlg::ms_pkDlg = 0;
//---------------------------------------------------------------------------

NSFParserUtilityDlg::NSFParserUtilityDlg(CWnd* pParent /*=NULL*/)
    : CDialog(NSFParserUtilityDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(NSFParserUtilityDlg)
    m_strFileOrFolderString = _T("Folder Name:");
    m_bRecurse = FALSE;
    m_strFileOrFolder = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    ms_pkDlg = this;
}

void NSFParserUtilityDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(NSFParserUtilityDlg)
    DDX_Control(pDX, IDC_LIST_STATUS, m_CtrlList_Status);
    DDX_Control(pDX, IDC_EDIT_FILE_OR_FOLDER, m_CtrlEdit_FileOrFolder);
    DDX_Control(pDX, IDC_CHECK_RECURSE, m_CtrlBtn_Recurse);
    DDX_Control(pDX, IDC_BUTTON_SELECT, m_CtrlBtn_FileSelect);
    DDX_Control(pDX, IDC_BUTTON_CLEAR_LIST, m_CtrlBtn_ClearStatus);
    DDX_Control(pDX, IDC_STATIC_FILE_OR_FOLDER_NAME,
        m_CtrlStatic_FileOrFolderName);
    DDX_Text(pDX, IDC_STATIC_FILE_OR_FOLDER_NAME, m_strFileOrFolderString);
    DDX_Check(pDX, IDC_CHECK_RECURSE, m_bRecurse);
    DDX_Text(pDX, IDC_EDIT_FILE_OR_FOLDER, m_strFileOrFolder);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(NSFParserUtilityDlg, CDialog)
    //{{AFX_MSG_MAP(NSFParserUtilityDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_CLEAR_LIST, OnButtonClearList)
    ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
    ON_BN_CLICKED(IDC_CHECK_RECURSE, OnCheckRecurse)
    ON_EN_CHANGE(IDC_EDIT_FILE_OR_FOLDER, OnChangeEditFileOrFolder)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NSFParserUtilityDlg message handlers

BOOL NSFParserUtilityDlg::OnInitDialog()
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
    
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void NSFParserUtilityDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void NSFParserUtilityDlg::OnPaint() 
{
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR NSFParserUtilityDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void NSFParserUtilityDlg::OnButtonClearList() 
{
    m_CtrlList_Status.ResetContent();
}

void NSFParserUtilityDlg::OnButtonSelect() 
{
    CString strFilter = "NSF Files (*.nsf)|*.nsf|";

    CFileDialog kFileDlg(true, "*.nsf", 0, OFN_FILEMUSTEXIST, strFilter);

    if (kFileDlg.DoModal() == IDOK)
    {
        CString strTemp = kFileDlg.GetPathName();
        RemoveFileNameFromPath(strTemp);
        m_strFileOrFolder = strTemp;
        UpdateData(false);
    }

}

void NSFParserUtilityDlg::OnCheckRecurse() 
{
    
}

void NSFParserUtilityDlg::OnChangeEditFileOrFolder() 
{
    // A note from Visual Studio:
    // If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
}

void NSFParserUtilityDlg::OnCancel() 
{
    CDialog::OnCancel();
}

void NSFParserUtilityDlg::OnOK() 
{
    UpdateData(true);

    if (m_strFileOrFolder.IsEmpty())
    {
        AfxMessageBox("You must select a file/folder");
        return;
    }

    RunParserUtility();
}
//---------------------------------------------------------------------------
bool NSFParserUtilityDlg::RemoveFileNameFromPath(CString& strFullPath)
{
    // Parse off the file name and leave the directory
    int iIndex;

    // Make sure there is a file name in there...
    iIndex = strFullPath.ReverseFind('.');
    if (iIndex != -1)
    {
        iIndex = strFullPath.ReverseFind('\\');
        if (iIndex == -1)
            iIndex = strFullPath.ReverseFind('/');

        if (iIndex != -1)
            strFullPath.Delete(iIndex, (int)(strlen(strFullPath)) - iIndex);
    }
    return true;
}
//---------------------------------------------------------------------------
void NSFParserUtilityDlg::RunParserUtility(void)
{
    // Setup the error callback
    NiD3DShaderFactory::RegisterErrorCallback(ShaderErrorCallback);

    CString strStatus;

    // Parse just the whole directory
    strStatus.Format("Parsing directory...");
    m_CtrlList_Status.AddString(strStatus);
    strStatus.Format("    %s", m_strFileOrFolder);
    m_CtrlList_Status.AddString(strStatus);
    strStatus.Format("    RecurseSubFolders = %s", 
        m_bRecurse ? "ENABLED" : "DISABLED");
    m_CtrlList_Status.AddString(strStatus);

#if defined(_MSC_VER) && _MSC_VER >= 1400
    char acEnvPath[NI_MAX_PATH];
    unsigned int uiEnvPath = 0;
    if (getenv_s(&uiEnvPath, acEnvPath, NI_MAX_PATH, 
        "EGB_SHADER_LIBRARY_PATH") != 0 || uiEnvPath > NI_MAX_PATH)
    {
        acEnvPath[0] = '\0';
    }
    CString strParserName(acEnvPath);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    CString strParserName;

    strParserName = getenv("EGB_SHADER_LIBRARY_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400


#if _MSC_VER == 1310 //VC7.1
    strParserName += "\\Lib\\VC71\\";
#elif _MSC_VER == 1400 //VC8.0
    strParserName += "\\Lib\\VC80\\";
#elif _MSC_VER == 1500 //VC9.0
    strParserName += "\\Lib\\VC90\\";
#else
    #error Unsupported version of Visual Studio
#endif

#ifdef NIRELEASE
    strParserName += "NSFParserLib.np9";
#else 
#ifdef NISHIPPING
    strParserName += "NSFParserLib.sp9";
#else
    strParserName += "NSFParserLib.dp9";
#endif
#endif

    char* pszLibName = strParserName.LockBuffer();
    if (pszLibName)
    {
        char* pszDirectory = m_strFileOrFolder.LockBuffer();

        bool bRecurse;
        if (m_bRecurse)
            bRecurse = true;
        else
            bRecurse = false;
        unsigned int uiCount = NiD3DShaderFactory::LoadAndRunParserLibrary(
            pszLibName, pszDirectory, bRecurse);

        strStatus.Format("Parsed %d shader(s) from %s!", uiCount, pszLibName);
        m_CtrlList_Status.AddString(strStatus);

        m_strFileOrFolder.UnlockBuffer();
        strParserName.UnlockBuffer();
    }
}
//---------------------------------------------------------------------------
unsigned int NSFParserUtilityDlg::ShaderErrorCallback(const char* pacError, 
    NiD3DError, bool)
{
    AfxMessageBox(pacError);
    if (ms_pkDlg)
    {
        // We want to split the error lines before adding them to the list
        CString strTemp = pacError;
        CString strStatus;

        int iCount = 0;
        int iIndexSrc = 0;
        int iIndex = strTemp.Find("\n", 0);
        while (iIndex != -1)
        {
            int iLen = strTemp.GetLength();
            strStatus.Empty();
            while (iIndexSrc < iIndex)
                strStatus += strTemp.GetAt(iIndexSrc++);
            if (iCount != 0)
            {
                // Insert some spaces to indent
                strStatus.Insert(0, "    ");
                ms_pkDlg->m_CtrlList_Status.AddString(strStatus);
            }
            else
            {
                ms_pkDlg->m_CtrlList_Status.AddString(strStatus);
            }

            // Skip the \n
            strTemp = strTemp.Right(iLen - iIndexSrc - 1);
            iIndex = strTemp.Find("\n", 0);
            iIndexSrc = 0;
            iCount++;
        }
        // Put up the last one
        strStatus = strTemp;
        if (strStatus.GetLength() > 1)
            ms_pkDlg->m_CtrlList_Status.AddString(strStatus);
    }
    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pacError);

    return 0;
}
//---------------------------------------------------------------------------
