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

// NiFontCreator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NiFontCreator.h"
#include "NiFontCreatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <NiLicense.h>

NiEmbedGamebryoLicenseCode;


/////////////////////////////////////////////////////////////////////////////
// NiFontCreatorApp

BEGIN_MESSAGE_MAP(NiFontCreatorApp, CWinApp)
    //{{AFX_MSG_MAP(NiFontCreatorApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NiFontCreatorApp construction

NiFontCreatorApp::NiFontCreatorApp()
{
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only NiFontCreatorApp object

NiFontCreatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// NiFontCreatorApp initialization

BOOL NiFontCreatorApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    NiInit();

    NiSystemDesc::GetSystemDesc().SetToolMode(true);
    NiSystemDesc::GetSystemDesc().SetToolModeRendererID(
        NiSystemDesc::RENDERER_GENERIC);

    NiFontCreatorDlg dlg;
    m_pMainWnd = &dlg;

    // Open NiFontCreator dialog
    dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
//---------------------------------------------------------------------------
int NiFontCreatorApp::ExitInstance() 
{
    NiShutdown();

    return CWinApp::ExitInstance();
}
//---------------------------------------------------------------------------
