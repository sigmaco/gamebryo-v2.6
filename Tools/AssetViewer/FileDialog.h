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

// FileDialog.h

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

class FileDialog
{
public:
    static CString GetFilename(HWND hWnd, const char* pcTitle = NULL,
        const char* pcFilter = "All Files (*.*)\0*.*\0\0",
        const char* pcDefExt = NULL);
    static bool GetMultipleFilenames(CStringArray& astrFilenames, HWND hWnd,
        const char* pcTitle = NULL,
        const char* pcFilter = "All Files (*.*)\0*.*\0\0",
        const char* pcDefExt = NULL);
    static CString GetFolderName(HWND hParentWnd, const char* pcTitle = NULL,
        const char* pcInitialFolder = NULL);

protected:
    static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg,
        LPARAM lParam, LPARAM lpData);
};

#endif  // #ifndef FILEDIALOG_H
