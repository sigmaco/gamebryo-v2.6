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

#include "NiSceneViewerConfigDialog.h"
#include "resource.h"
#include "ScriptParameterDefinitions.h"

//---------------------------------------------------------------------------
void SetCheck(HWND hwnd, int iControlID, bool bChecked)
{
    if (bChecked)
        SendDlgItemMessage(hwnd, iControlID, BM_SETCHECK, BST_CHECKED, 0);
    else
        SendDlgItemMessage(hwnd, iControlID, BM_SETCHECK, BST_UNCHECKED, 0);
}
//---------------------------------------------------------------------------
bool GetCheck(HWND hWnd, int iControlID)
{
    int iCheck = (int)SendDlgItemMessage(hWnd, iControlID, BM_GETCHECK, 0, 0);
    return (iCheck == BST_CHECKED);
}
//---------------------------------------------------------------------------
NiSceneViewerConfigDialog::NiSceneViewerConfigDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo,
    NiString strPluginDataPath, CSceneViewerDll* pkSceneViewerDLL) :
    NiDialog(IDD_SCENEVIEWER, hInstance, hWndParent)
{
    m_strPluginDataPath = strPluginDataPath;
    m_pkSceneViewerDLL = pkSceneViewerDLL;
    NIASSERT(m_pkSceneViewerDLL);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
    m_bInit = false;
}
//---------------------------------------------------------------------------
NiSceneViewerConfigDialog::~NiSceneViewerConfigDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
       
//---------------------------------------------------------------------------
int NiSceneViewerConfigDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void NiSceneViewerConfigDialog::InitDialog()
{
    if (!m_bInit)
    {
        // Initialize SHOWTREEVIEW check box.
        bool bCheck = false;
        bCheck = m_spPluginInfoResults->GetBool(PARAM_SHOWTREEVIEW);
        SetCheck(m_hWnd, IDC_SHOWTREEVIEW, bCheck);
        NIASSERT(NiIsKindOf(NiViewerPluginInfo, m_spPluginInfoResults));
        m_bInit = true;
    }

    // Initialize UIM combo box.
    NiString strValue;
    char acPathString[1024];
    char acFileString[1024];
    
    NiSprintf(acFileString, 1024, "%s", (const char*) m_strPluginDataPath);
    
    NiSprintf(acPathString, 1024, "%s%s", (const char*) m_strPluginDataPath,
        "*.uim");

    m_kUIMArray.RemoveAll();
    
    LPWIN32_FIND_DATA lpFindFileData = NiExternalNew WIN32_FIND_DATA;

    HANDLE hFindHandle = FindFirstFile(acPathString, lpFindFileData);
    HWND hwndCombo = GetDlgItem(m_hWnd, IDC_UIMAP_SELECTIONS );
    char acFilename[NI_MAX_PATH];
    SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0); 

    if (lpFindFileData->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
    {
        NiSprintf(acFilename, NI_MAX_PATH, "%s%s", acFileString,
            lpFindFileData->cFileName);
        AddFile(m_hWnd, acFilename);
    }

    while (FindNextFile(hFindHandle, lpFindFileData))
    {
        if (lpFindFileData->dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
        {
            NiSprintf(acFilename, NI_MAX_PATH, "%s%s", acFileString,
                lpFindFileData->cFileName);
            AddFile(m_hWnd, acFilename);
        }
    }

    m_iUISelection = 0;
    bool bFoundFileFlag = false;
    for (unsigned int ui = 0; ui < m_kUIMArray.GetSize() && 
        !bFoundFileFlag; ui++)
    {
        NiString strFilename = m_kUIMArray.GetAt(ui);
        if (strFilename.EqualsNoCase(m_strCustomUIMapFilename))
        {
            bFoundFileFlag = true;
            SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM) ui, 0);
            m_iUISelection = ui;
        }
    
    }

    if (!bFoundFileFlag)
    {
        SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM) 0, 0);
    }

    FindClose(hFindHandle);
    NiExternalDelete lpFindFileData;
}
//---------------------------------------------------------------------------
BOOL NiSceneViewerConfigDialog::OnCommand(int iWParamLow, int iWParamHigh, 
    long)
{
    switch(iWParamLow)
    {
        case IDOK:
        {
                // Save options.
            HandleOK();
            EndDialog(m_hWnd, IDOK);
            return TRUE;
        }
        case IDCANCEL:
        {
            EndDialog(m_hWnd, IDCANCEL);
            return TRUE;
        }
        case IDC_VIEW_UI_BUTTON:
        {
            char acPathString[1024];
            NiSprintf(acPathString, 1024, "%s",
                (const char*)m_strPluginDataPath );

            char acExecPath[1024];
            NiSprintf(acExecPath, 1024, "%s%s",
                (const char*) m_strPluginDataPath ,
                "ConfigurableUIMapEditor.exe");

            char acStartFile[1024];
            NiString strLocalName = m_kUIMArray.GetAt(m_iUISelection);
            NiSprintf(acStartFile, 1024, "%s", 
                (const char*) strLocalName);

            m_pkSceneViewerDLL->RunConfigurableUIMapEditor(acExecPath, 
                acPathString, acStartFile);
        }
        break;
        case IDC_CREATE_NEW_UIMAP_BUTTON:
        {
            char acPathString[1024];
            NiSprintf(acPathString, 1024, "%s",
                (const char*)m_strPluginDataPath );

            char acExecPath[1024];
            NiSprintf(acExecPath, 1024, "%s%s",
                (const char*)m_strPluginDataPath ,
                "ConfigurableUIMapEditor.exe");

            char* pcFile = NULL;
            char* pcFullFile = NULL;
            pcFullFile = m_pkSceneViewerDLL->RunConfigurableUIMapEditor(
                acExecPath, acPathString, pcFile);

            if (pcFullFile != NULL)
            {
                pcFile = strrchr(pcFullFile, '\\');
                if (pcFile == NULL)
                    pcFile = pcFullFile;

                //AddFile(hWnd, pcFile);
                InitDialog();
            }
        }
        break;
        case IDC_UIMAP_SELECTIONS:
            if (iWParamHigh == CBN_SELCHANGE )
            {
                m_iUISelection = (int)SendDlgItemMessage(m_hWnd, 
                    IDC_UIMAP_SELECTIONS, CB_GETCURSEL, 0, 0);
                m_strCustomUIMapFilename = m_kUIMArray.GetAt(m_iUISelection);
            }
        break;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiSceneViewerConfigDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiSceneViewerConfigDialog::HandleOK()
{
    // Update SHOWTREEVIEW parameter in script.
    m_spPluginInfoResults->SetBool(PARAM_SHOWTREEVIEW, GetCheck(m_hWnd,
        IDC_SHOWTREEVIEW));
    // Update renderer type in script.
    NIASSERT(NiIsKindOf(NiViewerPluginInfo, m_spPluginInfoResults));
    // Update UIMFILE parameter in script.
    char acUIMRelativePath[NI_MAX_PATH];
#ifdef NIDEBUG
    size_t stBytes = 
#endif
        NiPath::ConvertToRelative(acUIMRelativePath, NI_MAX_PATH,
        m_kUIMArray.GetAt(m_iUISelection), m_strPluginDataPath);
    NIASSERT(stBytes > 0);
    m_spPluginInfoResults->SetValue(PARAM_UIMFILE, acUIMRelativePath);
}
//---------------------------------------------------------------------------
void NiSceneViewerConfigDialog::AddFile(HWND handle, char* pcFilename)
{
    if (pcFilename == NULL)
        return;
    HWND hwndCombo = GetDlgItem(handle,IDC_UIMAP_SELECTIONS );

    char acUsername [1024];
    char* pcName = NULL;

    NiFile* pkFile = NiFile::GetFile(pcFilename, NiFile::READ_ONLY);
    if (*pkFile)
    {
        pkFile->GetLine(acUsername, 1024); // First line is file type info
        pkFile->GetLine(acUsername, 1024); // Second line is user name

        pcName = strrchr(acUsername, '\"');
    }

    if (pcName != NULL)
    {
        pcName[0] = '\0';
        pcName = strstr(acUsername, "\"")+1;
    }
    else
    {
        pcName = pcFilename;
    }

    m_kUIMArray.Add(NiStrdup(pcFilename));
    SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM) pcName);
    NiDelete pkFile;
}
//---------------------------------------------------------------------------
