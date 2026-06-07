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

// AssetViewer.h

#ifndef ASSETVIEWER_H
#define ASSETVIEWER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CAssetViewerApp : public CWinApp
{
public:
    CAssetViewerApp();

    // Functions for setting registry values.
    void SetSettingsParamFloat(CString strParam, float fValue);
    float GetSettingsParamFloat(CString strParam);
    void SetSettingsParamString(CString strParam, CString strValue);
    CString GetSettingsParamString(CString strParam);
    void SetSettingsParamInt(CString strParam, int iValue);
    int GetSettingsParamInt(CString strParam);

    CString GetApplicationDirectory();
protected:
    CString m_kApplicationDirectory;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAssetViewerApp)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CAssetViewerApp)
    afx_msg void OnAppAbout();
    afx_msg void OnFileNew();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

extern CAssetViewerApp theApp;

#include "AssetViewer.inl"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef ASSETVIEWER_H
