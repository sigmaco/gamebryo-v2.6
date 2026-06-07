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

// Precompiled Header
#include "NiApplicationPCH.h"

#include "NiApplication.h"
#include <NiFile.h>
#include <NiSystem.h>

//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR cmdline, int iWinMode)
{
    // for determining if there are 'object leaks'
    unsigned int uiInitialCount = NiRefObject::GetTotalObjectCount();

    NiInit();

    // If cmdline.txt file exists in the work directory, read it and 
    // set it as the command line
    NiFile* pkCmdlineFile = NULL;
    if (!cmdline || strlen(cmdline) == 0)
    {
        pkCmdlineFile = NiFile::GetFile("./cmdline.txt", NiFile::READ_ONLY);
    }

    if (pkCmdlineFile && *pkCmdlineFile)
    {
        // artificial 1024-character limit
        char* pcCmdline = NiAlloc(char, 1024);
        int iLength = 0;
        char c;
        // Read until limit is reached or end of file or end of string
        while (iLength < 1023) 
        {
            if (!(pkCmdlineFile->Read(&c, 1) || c == '\0'))
                break;

            // Factor out newlines and carriage returns
            // Replace with ' ' in case newline seperates valid parameters
            if (c == '\n' || c == '\r')
                c = ' ';
            pcCmdline[iLength++] = c;
        }
        pcCmdline[iLength] = '\0';

        // let the application know about the command line
        NiApplication::SetCommandLine(pcCmdline);

        NiFree(pcCmdline);
    }
    else
    {
        // let the application know about the command line
        NiApplication::SetCommandLine(cmdline);
    }

    NiDelete pkCmdlineFile;

    // create the application
    NiApplication* pkTheApp = NiApplication::Create();
    if (!pkTheApp)
    {
        NiMessageBox("Unable to create application", 
            NiAppWindow::GetWindowClassName());
        NiShutdown();    
        return 0;
    }

    // Set reference
    NiApplication::SetInstanceReference(hI);

    // process the command line for renderer selection
    NiApplication::CommandSelectRenderer();

    // Now, we are ready to initialize the window and the main loop.

    // register the window class
    WNDCLASS kWC;
    kWC.style         = CS_HREDRAW | CS_VREDRAW;
    kWC.lpfnWndProc   = NiApplication::WinProc;
    kWC.cbClsExtra    = 0;
    kWC.cbWndExtra    = 0;
    kWC.hInstance     = hI;
    kWC.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    kWC.hCursor       = LoadCursor(NULL, IDC_ARROW);
    kWC.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    kWC.lpszClassName = NiAppWindow::GetWindowClassName();

    // register menu (if requested)
    if (pkTheApp->GetMenuID())
        kWC.lpszMenuName = MAKEINTRESOURCE(pkTheApp->GetMenuID());
    else
        kWC.lpszMenuName = 0;

    RegisterClass(&kWC);

    // create the application window
    pkTheApp->CreateMainWindow(iWinMode);

    // allow the application to initialize before starting the message pump
    if (pkTheApp->Initialize())
    {
        pkTheApp->MainLoop();
    }

    pkTheApp->Terminate();

    // Ensure that the shadow manager has been destroyed.
    NIASSERT(!NiShadowManager::GetShadowManager());

    NiDelete pkTheApp;

    NiShutdown();    

    // for determining if there are 'object leaks'
    unsigned int uiFinalCount = NiRefObject::GetTotalObjectCount();
    char acMsg[256];
    NiSprintf(acMsg, 256,
        "\nGamebryo NiRefObject counts:  initial = %u, final = %u. ", 
        uiInitialCount, uiFinalCount);
    NiOutputDebugString(acMsg);
    if (uiFinalCount > uiInitialCount)
    {
        unsigned int uiDiff = uiFinalCount - uiInitialCount;
        NiSprintf(acMsg, 256, "Application is leaking %u objects\n\n", uiDiff);
        NiOutputDebugString(acMsg);
    }
    else
    {
        NiOutputDebugString("Application has no object leaks.\n\n");
    }
    
    return 0;
}
//---------------------------------------------------------------------------
