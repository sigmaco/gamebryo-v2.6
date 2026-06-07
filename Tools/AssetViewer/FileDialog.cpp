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

// FileDialog.cpp

#include "stdafx.h"
#include "FileDialog.h"

#define MAX_FILES 32

//---------------------------------------------------------------------------
CString FileDialog::GetFilename(HWND hWnd, const char* pcTitle,
    const char* pcFilter, const char* pcDefExt)
{
    char acFilename[_MAX_PATH];
    acFilename[0] = NULL;
    CString strFilename;

    OPENFILENAME ofn;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = pcFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = &acFilename[0];
    ofn.nMaxFile = _MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = pcTitle;
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
        OFN_HIDEREADONLY;
    ofn.lpstrDefExt = NULL;

    if (GetOpenFileName(&ofn))
    {
        strFilename = acFilename;
    }

    return strFilename;
}
//---------------------------------------------------------------------------
bool FileDialog::GetMultipleFilenames(CStringArray& astrFilenames,
    HWND hWnd, const char* pcTitle, const char* pcFilter,
    const char* pcDefExt)
{
    char acFilenames[_MAX_PATH * MAX_FILES];
    acFilenames[0] = NULL;

    OPENFILENAME ofn;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = pcFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = &acFilenames[0];
    ofn.nMaxFile = _MAX_PATH * MAX_FILES;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = pcTitle;
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
        OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;
    ofn.lpstrDefExt = NULL;

    if (!GetOpenFileName(&ofn))
    {
        return false;
    }

    NIASSERT(acFilenames[0]);
    if (ofn.nFileExtension != 0)
    {
        astrFilenames.Add(acFilenames);
    }
    else
    {
        CString strPath = CString(acFilenames) + "\\";
        char* pcFilename = strchr(acFilenames, '\0');
        pcFilename += 1;
        while (pcFilename[0] != '\0')
        {
            astrFilenames.Add(strPath + pcFilename);
            pcFilename = strchr(pcFilename, '\0');
            pcFilename += 1;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
CString FileDialog::GetFolderName(HWND hParentWnd, const char* pcTitle,
    const char* pcInitialFolder)
{
    CString strSelectedFolder;
    char acFolderPath[MAX_PATH];

    BROWSEINFO kBrowseInfo;
    kBrowseInfo.hwndOwner = hParentWnd;
    kBrowseInfo.pidlRoot = NULL;
    kBrowseInfo.pszDisplayName = acFolderPath;
    kBrowseInfo.lpszTitle = pcTitle;
    kBrowseInfo.ulFlags = BIF_NEWDIALOGSTYLE;
    kBrowseInfo.lpfn = &BrowseCallbackProc;
    kBrowseInfo.lParam = (LPARAM) pcInitialFolder;

    LPITEMIDLIST pidl = SHBrowseForFolder(&kBrowseInfo);
    if (pidl != NULL)
    {
        BOOL bSuccess = SHGetPathFromIDList(pidl, acFolderPath);
        if (bSuccess)
        {
            strSelectedFolder = acFolderPath;
        }
    }

    LPMALLOC pMalloc;
    HRESULT hRes = SHGetMalloc(&pMalloc);
    if (hRes == NOERROR)
    {
        pMalloc->Free(pidl);
    }

    return strSelectedFolder;
}
//---------------------------------------------------------------------------
int CALLBACK FileDialog::BrowseCallbackProc(HWND hwnd, UINT uMsg,
    LPARAM lParam, LPARAM lpData)
{
    switch (uMsg)
    {
        case BFFM_INITIALIZED:
            // Set selection to initial folder.
            SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
            break;
    }

    return 0;
}
//---------------------------------------------------------------------------
