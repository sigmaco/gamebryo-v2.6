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
#include "MiDlgProcAbout.h"
#include "NiVersion.h"
#ifndef NO_CSTUDIO
#include "Phyexp.h"
#endif

//---------------------------------------------------------------------------
BOOL MiDlgProcAbout::DlgProc(TimeValue, IParamMap*, HWND hWnd,
    UINT msg, WPARAM, LPARAM) 
{
    switch (msg) 
    {
    case WM_CREATE:
    case WM_INITDIALOG:
        {
            CHECK_MEMORY();
            char acMaxVersion[256];
            NiSprintf(acMaxVersion, 256, "%.2f", (float)MAX_RELEASE / 1000.0f);
            char acCharStudioVersion[256];
#ifndef NO_CSTUDIO
            int iVer = CURRENT_VERSION;
            if (iVer > 100)
            {
                NiSprintf(acCharStudioVersion, 256, "%.2f", 
                    (float) CURRENT_VERSION / 100.0f);
            }
            else if (iVer > 10)
            {
                NiSprintf(acCharStudioVersion, 256, "%.2f", 
                    (float) CURRENT_VERSION / 10.0f);
            }
            else
            {
                NiSprintf(acCharStudioVersion, 256, "%.2f", 
                    (float) CURRENT_VERSION);
            }
#else
            NiSprintf(acCharStudioVersion, 256, "NONE");
#endif
            HWND hWndEdit = GetDlgItem(hWnd, IDC_MAXIMMERSE_ABOUT_EDIT);
            char acStr[512];
            NiSprintf(acStr, 512,
#ifdef NIDEBUG
                           "Gamebryo Max Plugin Dbg v %s\r\n"
#else                      
                           "Gamebryo Max Plugin v %s\r\n"
#endif
                           "\r\nBuilt on:\r\n"
                           "Gamebryo v %s\r\n"
                           "3DS Max v %s\r\n"
                           "Character Studio v %s\r\n"
                           "\r\nBuild Date: %s\r\n",                        
                           MAXIMMERSEVERSION,
                           GAMEBRYO_SDK_VERSION_STRING,
                           acMaxVersion,
                           acCharStudioVersion,
                           MAXIMMERSEBUILDSTRING);
            SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM) acStr);
            CHECK_MEMORY();
        }
        break;
    case WM_COMMAND:
        /* switch (LOWORD(wParam)) 
        {
            default:
                break;
        }
        */
        break;
        
    default:
        return FALSE;
    }
    return FALSE; 
}
//---------------------------------------------------------------------------
