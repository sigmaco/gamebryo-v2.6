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

#include "MaxImmerse.h"
#include "MiDlgProcInteraction.h"
#include <NiFile.h>
#include <NiScriptInfoDialogs.h>
#include <Shlobj.h>
#include "MiResource.h"

//---------------------------------------------------------------------------
void MiDlgProcInteraction::InitDialog(HWND handle)
{
    NiString strNwName = NiMAXOptions::GetNetworkScriptDirectory();
    SetDlgItemText(handle, IDC_NETWORK_PATH_EDIT, (const char*) strNwName);

    CheckDlgButton(handle, IDC_422_BEHAVIOR_CHECK,  
        NiMAXOptions::GetBool(NI_USEOLDBEHAVIOR));

    CheckDlgButton(handle, IDC_AREA_SUPERSPRAY_CHECK,  
        NiMAXOptions::GetBool(NI_USEAREASUPERSPRAY));

    CheckDlgButton(handle, IDC_OLD_SPINTIME_CHECK,  
        NiMAXOptions::GetBool(NI_USEOLDSPINTIME));
    
    CheckDlgButton(handle, IDC_OLD_NORMALS_CHECK,
        NiMAXOptions::GetBool(NI_USEOLDNORMALS));

    if (NiMAXOptions::GetBool(NI_WRITE_ERRORS_TO_FILE))
    {
        CheckDlgButton(handle, IDC_WRITE_ERRORS_TO_LOG_RADIO, true);
    }
    else if (NiMAXOptions::GetBool(NI_LOG_ERRORS_PROMPT))
    {
        CheckDlgButton(handle, IDC_PROMPT_ERRORS_RADIO, true);
    }
    else if (NiMAXOptions::GetBool(NI_LAST_ERROR_PROMPT_RETURN))
    {
        CheckDlgButton(handle, IDC_ALWAYS_SHOW_ERRORS_RADIO, true);
    }
    else
    {
        CheckDlgButton(handle, IDC_NEVER_SHOW_ERRORS_RADIO, true);
    }
}
//---------------------------------------------------------------------------
bool MiDlgProcInteraction::IsChecked(HWND hWnd, int iResource)
{
    CHECK_MEMORY();
    int iState = IsDlgButtonChecked(hWnd, iResource);
    switch (iState)
    {
        case BST_CHECKED:
            return true; 
        case BST_INDETERMINATE:
            NIASSERT(iState != BST_INDETERMINATE);
            return false;
        case BST_UNCHECKED:
            return false;
    }
    return false;
}

//---------------------------------------------------------------------------
LPITEMIDLIST GetShellPath(NiString strPath)
{
   LPITEMIDLIST  pidl = NULL;
   LPSHELLFOLDER pDesktopFolder;
   char          szPath[MAX_PATH];
   OLECHAR       olePath[MAX_PATH];
   ULONG         chEaten;
   ULONG         dwAttributes;
   HRESULT       hr;

   //
   // Get the path to the file we need to convert.
   //
   NiSprintf(szPath, MAX_PATH, "%s", (const char*)strPath);

   //
   // Get a pointer to the Desktop's IShellFolder interface.
   //
   if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
   {
       //
       // IShellFolder::ParseDisplayName requires the file name be in
       // Unicode.
       //
       MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPath, -1,
                           olePath, MAX_PATH);

       //
       // Convert the path to an ITEMIDLIST.
       //
       hr = pDesktopFolder->ParseDisplayName(NULL,
                                                     NULL,
                                                     olePath,
                                                     &chEaten,
                                                     &pidl,
                                                     &dwAttributes);
       if (FAILED(hr))
       {
           pidl = NULL;
       }

       //
       // pidl now contains a pointer to an ITEMIDLIST for .\readme.txt.
       // This ITEMIDLIST needs to be freed using the IMalloc allocator
       // returned from SHGetMalloc().
       //

       //release the desktop folder object
       pDesktopFolder->Release();
   }
   return pidl;

}
//---------------------------------------------------------------------------
BOOL MiDlgProcInteraction::DlgProc(TimeValue, IParamMap*, HWND hWnd,
    UINT msg, WPARAM wParam, LPARAM) 
{
    CHECK_MEMORY();
    bool bChecked = false;

    switch (msg) 
    {
        case WM_INITDIALOG:
            InitDialog(hWnd);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) 
            {
                case IDC_PROMPT_ERRORS_RADIO:
                    bChecked = IsChecked(hWnd, IDC_PROMPT_ERRORS_RADIO);
                    if (bChecked)
                    {
                        NiMAXOptions::g_kOptionsCS.Lock();
                        NiMAXOptions::ReadOptions(GetCOREInterface());
                        NiMAXOptions::SetBool(NI_WRITE_ERRORS_TO_FILE, false);
                        NiMAXOptions::SetBool(NI_LOG_ERRORS_PROMPT, true);
                        NiMAXOptions::WriteOptions(GetCOREInterface());
                        NiMAXOptions::g_kOptionsCS.Unlock();
                    }
                    break;
                case IDC_ALWAYS_SHOW_ERRORS_RADIO:
                    bChecked = IsChecked(hWnd, IDC_ALWAYS_SHOW_ERRORS_RADIO);
                    if (bChecked)
                    {
                        NiMAXOptions::g_kOptionsCS.Lock();
                        NiMAXOptions::ReadOptions(GetCOREInterface());
                        NiMAXOptions::SetBool(NI_WRITE_ERRORS_TO_FILE, false);
                        NiMAXOptions::SetBool(NI_LOG_ERRORS_PROMPT, false);
                        NiMAXOptions::SetBool(NI_LAST_ERROR_PROMPT_RETURN, 
                            true);
                        NiMAXOptions::WriteOptions(GetCOREInterface());
                        NiMAXOptions::g_kOptionsCS.Unlock();
                    }
                    break;
                case IDC_NEVER_SHOW_ERRORS_RADIO:
                    bChecked = IsChecked(hWnd, IDC_NEVER_SHOW_ERRORS_RADIO);
                    if (bChecked)
                    {
                        NiMAXOptions::g_kOptionsCS.Lock();
                        NiMAXOptions::ReadOptions(GetCOREInterface());
                        NiMAXOptions::SetBool(NI_WRITE_ERRORS_TO_FILE, false);
                        NiMAXOptions::SetBool(NI_LOG_ERRORS_PROMPT, false);
                        NiMAXOptions::SetBool(NI_LAST_ERROR_PROMPT_RETURN, 
                            false);
                        NiMAXOptions::WriteOptions(GetCOREInterface());
                        NiMAXOptions::g_kOptionsCS.Unlock();
                    }
                    break;
                case IDC_WRITE_ERRORS_TO_LOG_RADIO:
                    bChecked = IsChecked(hWnd, IDC_WRITE_ERRORS_TO_LOG_RADIO);
                    if (bChecked)
                    {
                        NiMAXOptions::g_kOptionsCS.Lock();
                        NiMAXOptions::ReadOptions(GetCOREInterface());
                        NiMAXOptions::SetBool(NI_WRITE_ERRORS_TO_FILE, true);
                        NiMAXOptions::WriteOptions(GetCOREInterface());
                        NiMAXOptions::g_kOptionsCS.Unlock();
                    }
                    break;
                case IDC_CS_AS_KEYFRAME:
                    m_pUtil->m_pInteractionPMap->Invalidate();
                    break;
                case IDC_422_BEHAVIOR_CHECK:
                    bChecked = IsChecked(hWnd, IDC_422_BEHAVIOR_CHECK);
                    NiMAXOptions::g_kOptionsCS.Lock();
                    NiMAXOptions::ReadOptions(GetCOREInterface());
                    NiMAXOptions::SetBool(NI_USEOLDBEHAVIOR, bChecked);
                    NiMAXOptions::WriteOptions(GetCOREInterface());
                    NiMAXOptions::g_kOptionsCS.Unlock();
                    break;
                case IDC_AREA_SUPERSPRAY_CHECK:
                    bChecked = IsChecked(hWnd, IDC_AREA_SUPERSPRAY_CHECK);
                    NiMAXOptions::g_kOptionsCS.Lock();
                    NiMAXOptions::ReadOptions(GetCOREInterface());
                    NiMAXOptions::SetBool(NI_USEAREASUPERSPRAY, bChecked);
                    NiMAXOptions::WriteOptions(GetCOREInterface());
                    NiMAXOptions::g_kOptionsCS.Unlock();
                    break;
                case IDC_OLD_SPINTIME_CHECK:
                    bChecked = IsChecked(hWnd, IDC_OLD_SPINTIME_CHECK);
                    NiMAXOptions::g_kOptionsCS.Lock();
                    NiMAXOptions::ReadOptions(GetCOREInterface());
                    NiMAXOptions::SetBool(NI_USEOLDSPINTIME, bChecked);
                    NiMAXOptions::WriteOptions(GetCOREInterface());
                    NiMAXOptions::g_kOptionsCS.Unlock();
                    break;
                case IDC_OLD_NORMALS_CHECK:
                    bChecked = IsChecked(hWnd, IDC_OLD_NORMALS_CHECK);
                    NiMAXOptions::g_kOptionsCS.Lock();
                    NiMAXOptions::ReadOptions(GetCOREInterface());
                    NiMAXOptions::SetBool(NI_USEOLDNORMALS, bChecked);
                    NiMAXOptions::WriteOptions(GetCOREInterface());
                    NiMAXOptions::g_kOptionsCS.Unlock();
                    break;
                case IDC_EDIT_PROCESS_SCRIPT:
                {
                    if (BN_CLICKED == HIWORD(wParam))
                    {
                        NiMAXOptions::g_kOptionsCS.Lock();

                        NiMAXOptions::ReadOptions(GetCOREInterface());

                        NiScriptInfoSetPtr spSet = 
                            NiMAXOptions::GenerateAvailableProcessScripts();
                        NiScriptTemplateManager* pkManager = 
                            NiScriptTemplateManager::GetInstance();
                        NiScriptInfoPtr spScript = 
                            NiScriptInfoDialogs::DoManagementDialog(
                            NiMAXOptions::GetProcessScript(), 
                            spSet, hWnd, "PROCESS");

                        NiScriptInfoSet& kInfoSet = 
                            pkManager->GetScriptInfoSet();
                
                        if (kInfoSet.IsTemplate(spScript) && 
                            !spSet->IsTemplate(spScript))
                        {   
                            NiMAXOptions::SetBool(
                                NI_USE_TEMPLATE_FOR_SCRIPT, false);
                        }

                        if (spScript)
                            NiMAXOptions::SetProcessScript(spScript);

                        NiMAXOptions::WriteOptions(GetCOREInterface());
                        NiMAXOptions::g_kOptionsCS.Unlock();
                    }

                }
                break;

                case IDC_EDIT_EXPORT_SCRIPT:
                {
                    if (BN_CLICKED == HIWORD(wParam))
                    {
                        NiMAXOptions::g_kOptionsCS.Lock();

                        NiMAXOptions::ReadOptions(GetCOREInterface());

                        NiScriptInfoSetPtr spSet = 
                            NiMAXOptions::GenerateAvailableExportScripts();
                        NiScriptTemplateManager* pkManager = 
                            NiScriptTemplateManager::GetInstance();
                        NiScriptInfoPtr spScript = 
                            NiScriptInfoDialogs::DoManagementDialog(
                            NiMAXOptions::GetExportScript(), 
                            spSet, hWnd, "EXPORT");

                        NiScriptInfoSet& kInfoSet = 
                            pkManager->GetScriptInfoSet();
                
                        if (kInfoSet.IsTemplate(spScript) && 
                            !spSet->IsTemplate(spScript))
                        {   
                            NiMAXOptions::SetBool(NI_USE_TEMPLATE_FOR_SCRIPT,
                                false);
                        }

                        if (spScript)
                            NiMAXOptions::SetExportScript(spScript);

                        NiMAXOptions::WriteOptions(GetCOREInterface());
                        NiMAXOptions::g_kOptionsCS.Unlock();
                    }

                }
                break;

                case IDC_NETWORK_PATH_EDIT:
                break;

                case IDC_NETWORK_SCRIPT_BUTTON:
                {
                    NiMAXOptions::g_kOptionsCS.Lock();
                    NiMAXOptions::ReadOptions(GetCOREInterface());
                    NiString strCurDir = 
                        NiMAXOptions::GetNetworkScriptDirectory();
                    char acString[MAX_PATH+1];

                    LPITEMIDLIST pOldPath = NULL;//GetShellPath(strCurDir);
                    BROWSEINFO kBrowseInfo;
                    kBrowseInfo.hwndOwner = hWnd;
                    kBrowseInfo.pidlRoot = pOldPath;
                    kBrowseInfo.pszDisplayName = acString;
                    kBrowseInfo.lpszTitle = "Please select the location"
                        " to load all script files from...";
                    kBrowseInfo.ulFlags  = BIF_EDITBOX ;
                    kBrowseInfo.lpfn  = 0;
                    LPITEMIDLIST pidl = SHBrowseForFolder(&kBrowseInfo);

                    if (pidl != NULL)
                    {
                        BOOL bSuccess = SHGetPathFromIDList(pidl, acString);
                        if (bSuccess && !strCurDir.EqualsNoCase(acString))
                        {
                            NiMAXOptions::SetValue(NI_NETWORK_SCRIPT_PATH,
                                acString);
                            SetDlgItemText(hWnd, IDC_NETWORK_PATH_EDIT,
                                acString);
                            MessageBox(hWnd, "You have altered the script"
                                " path used by this\n"
                                "installation of the Gamebryo Max Plugin."
                                "\n\n"
                                "You will need to restart your copy of Max"
                                " in order\n"
                                "to use this path",
                                "Script Path Has Changed", 
                                MB_OK | MB_ICONINFORMATION);
                            NiMAXOptions::WriteOptions(GetCOREInterface());
                        }
                    }

                    LPMALLOC pMalloc;
                    HRESULT hRes = SHGetMalloc(&pMalloc);
                    if (hRes == NOERROR)
                    {
                        pMalloc->Free(pidl);
                        pMalloc->Free(pOldPath);
                    }

                    NiMAXOptions::g_kOptionsCS.Unlock();
                }
                break;
            }
            break;
        default:
            return FALSE;
    }
    CHECK_MEMORY();
    return FALSE; 
}

//---------------------------------------------------------------------------
