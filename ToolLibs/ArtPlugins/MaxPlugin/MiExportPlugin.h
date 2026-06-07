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

#ifndef MIEXPORTPLUGIN_H
#define MIEXPORTPLUGIN_H

#include "MiLibTypes.h"
#pragma warning(push)
// unreferenced formal parameter
#pragma warning(disable: 4100)
// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
// nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4238)
// nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4239)
// 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4245)
// '' : assignment operator could not be generated
#pragma warning(disable: 4512)
#include "max.h"
#pragma warning(pop)

#include <NiString.h>
#include <NiScriptInfoSet.h>
#include "NiMAXOptions.h"

class MiExportPlugin : public SceneExport 
{
public:
                    MiExportPlugin();
                    ~MiExportPlugin();
    int             ExtCount();          // Number of extensions supported
    const TCHAR *   Ext(int n);          // Extension #n (i.e. "3DS")
    const TCHAR *   LongDesc();          // Long ASCII description 
    const TCHAR *   ShortDesc();         // Short ASCII description
    const TCHAR *   AuthorName();        // ASCII Author name
    const TCHAR *   CopyrightMessage();  // ASCII Copyright message
    const TCHAR *   OtherMessage1();     // Other message #1
    const TCHAR *   OtherMessage2();     // Other message #2
    unsigned int    Version();           // Version number * 100 (v3.01 = 301)
    void            ShowAbout(HWND hWnd);// Show DLL's "About..." box
    virtual BOOL SupportsOptions(int ext, DWORD options);


    int             DoExport(const TCHAR *pName,
                             ExpInterface *pEI,
                             Interface *pIntf, 
                             BOOL bSuppressPrompts=FALSE, 
                             DWORD options=0);

    static int      Export(const TCHAR *pName,
                             ExpInterface *pEI,
                             Interface *pIntf, 
                             BOOL bSuppressPrompts=FALSE, 
                             DWORD options=0);
    static int     DoNiExportDialog(Interface* pkIntf);

    bool m_bOptionsChanged;
    NiScriptInfoPtr m_spNewProcessScript;
    NiScriptInfoPtr m_spNewExportScript;

    static BOOL CALLBACK ExportOptionsDlgProc(HWND hDlg, UINT message,
        WPARAM wParam, LPARAM lParam);

    static NiScriptInfoSetPtr ms_spAvailableExportScripts;
    static NiScriptInfoSetPtr ms_spAvailableProcessScripts;

private:
    NiTPrimitiveArray<char*> m_kFileTypeStrings;


};

//---------------------------------------------------------------------------
#define MI_OUTPUT_CLASS_ID  Class_ID(0x446905ef, 0)
//---------------------------------------------------------------------------
extern ClassDesc* GetMAXExportClassDesc();
//---------------------------------------------------------------------------

#endif
