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

#include <NiOS.h>

// status window and menu support
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "NiAppWindow.h"
#include "NiApplication.h"

char NiAppWindow::ms_acWindowClassName[] = "Gamebryo Application";

//---------------------------------------------------------------------------
NiAppWindow::NiAppWindow(const char* pcWindowCaption, unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int uiNumStatusPanes, 
    unsigned long ulWindowStyle)
{
    // text for window caption
    if (pcWindowCaption && pcWindowCaption[0])
    {
        size_t stLen = strlen(pcWindowCaption) + 1;
        m_pcWindowCaption = NiAlloc(char, stLen);
        NiStrcpy(m_pcWindowCaption, stLen, pcWindowCaption);
    }
    else
    {
        size_t stLen = strlen(ms_acWindowClassName) + 1;
        m_pcWindowCaption = NiAlloc(char, stLen);
        NiStrcpy(m_pcWindowCaption, stLen, ms_acWindowClassName);
    }

    // window references
    m_pWnd = 0;
    m_pRenderWnd = 0;
    m_ppStatusWnd = 0;
    
    // client window dimensions
    m_uiWidth = m_uiParentWidth = uiWidth;
    m_uiHeight = m_uiParentHeight = uiHeight;

    // client window style
    m_ulWindowStyle = ulWindowStyle;

    // number of panes in status bar
    m_uiNumStatusPanes = uiNumStatusPanes;
    m_bStatusPanes = false;
}
//---------------------------------------------------------------------------
NiAppWindow::~NiAppWindow()
{
    if (GetStatusPanesExist())
    {
        if (IsWindow(m_pWnd))
        {
            StatusWindowDestroy();
        }
        if (m_ppStatusWnd)
            NiFree(m_ppStatusWnd);
    }
    NiFree(m_pcWindowCaption);

    m_pWnd = 0;
    m_pRenderWnd = 0;
}
//---------------------------------------------------------------------------
void NiAppWindow::SetWindowCaption(char* pcCaption)
{
    NiFree(m_pcWindowCaption);
    const size_t stStrLength = strlen(pcCaption) + 1;
    m_pcWindowCaption = NiAlloc(char, stStrLength);
    NiStrcpy(m_pcWindowCaption, stStrLength, pcCaption);

    // Call the appropriate function to set the window title
    SetWindowText(m_pWnd, m_pcWindowCaption);
}
//---------------------------------------------------------------------------
void NiAppWindow::StatusWindowCreate()
{

    // Make sure we can create a status window
    if (NiApplication::ms_pkApplication->GetFullscreen())
        m_uiNumStatusPanes = 0;

    if (m_uiNumStatusPanes == 0 || m_bStatusPanes)
        return;

    RECT kStatusRect;
    RECT kRect;
    int iWidth;
    int iHeight;
    INITCOMMONCONTROLSEX kICC;

    kICC.dwSize = sizeof(INITCOMMONCONTROLSEX);
    kICC.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&kICC);
    
    SetStatusWindowReference(CreateWindow(
        STATUSCLASSNAME, 
        "", 
        WS_CHILD | WS_VISIBLE, 
        0, 0, 0, 0, 
        m_pWnd, 
        NULL, 
        NiApplication::GetInstanceReference(), 
        NULL));
    
    // increase parent window height by status bar height
    GetWindowRect(GetStatusWindowReference(), &kStatusRect);
    GetWindowRect(GetWindowReference(), &kRect);

    m_bStatusPanes = true;    

    iWidth = kRect.right - kRect.left;
    iHeight = kRect.bottom - kRect.top + kStatusRect.bottom 
        - kStatusRect.top;
    MoveWindow(GetWindowReference(), kRect.left, kRect.top, iWidth, iHeight,
        TRUE);
}
//---------------------------------------------------------------------------
void NiAppWindow::WriteToStatusWindow(const unsigned int uiPane, char* pcMsg) 
    const
{
    if (uiPane >= m_uiNumStatusPanes || !m_bStatusPanes)
        return;
    SendMessage(GetStatusWindowReference(), SB_SETTEXT, (WPARAM) uiPane, 
        (LPARAM) pcMsg);
    SendMessage(GetStatusWindowReference(), WM_PAINT, 0, 0); 
}
//---------------------------------------------------------------------------
void NiAppWindow::StatusWindowDestroy()
{
    if (m_uiNumStatusPanes == 0 || !m_bStatusPanes)
        return;

    RECT kStatusRect;
    RECT kRect;
    int iWidth;
    int iHeight;

    GetWindowRect(GetStatusWindowReference(), &kStatusRect);
    GetWindowRect(GetWindowReference(), &kRect);

    DestroyWindow(GetStatusWindowReference());
    SetStatusWindowReference(0);
    
    m_bStatusPanes = false;

    // decrease parent window height by status bar height
    iWidth = kRect.right - kRect.left;
    iHeight = kRect.bottom - kRect.top - 
        (kStatusRect.bottom - kStatusRect.top);
    MoveWindow(GetWindowReference(), kRect.left, kRect.top, iWidth, iHeight, 
        TRUE);
}
//---------------------------------------------------------------------------
NiWindowRef NiAppWindow::CreateMainWindow(int iWinMode, NiWindowRef hWnd)
{
    NiWindowRef pWnd = NULL;
    // require the renderer window to have the specified client area
    RECT kRect = { 0, 0, GetWidth(), GetHeight() };
    AdjustWindowRect(&kRect, GetWindowStyle(), 
        NiApplication::ms_pkApplication->GetMenuID());

    unsigned int uiFlags = WS_CHILD;

    // Create window
    if (NiApplication::ms_pkApplication->GetFullscreen())
    {
        // Create window
        pWnd = CreateWindow
        (
            GetWindowClassName(), 
            GetWindowCaption(),         
            WS_POPUP,       
            0,                      // initial x position
            0,                      // initial y position
            kRect.right - kRect.left, // initial width
            kRect.bottom - kRect.top, // initial height
            hWnd,                   // parent window handle
            NULL,                   // window menu handle
            NiApplication::GetInstanceReference(), // program instance handle
            NULL                    // creation parameters
       );
    }
    else
    {
        uiFlags |= WS_VISIBLE;

        pWnd = CreateWindow
        (
            GetWindowClassName(), 
            GetWindowCaption(),         
            GetWindowStyle(),       
            0,                      // initial x position
            0,                      // initial y position
            kRect.right - kRect.left, // initial width
            kRect.bottom - kRect.top, // initial height
            hWnd,                   // parent window handle
            NULL,                   // window menu handle
            NiApplication::GetInstanceReference(), // program instance handle
            NULL                    // creation parameters
       );
    }
    SetWindowReference(pWnd);

    // create status window (if requested)
    if (GetNumStatusPanes())
        StatusWindowCreate();

    // Create render window
    NiWindowRef pRenderWnd = CreateWindow
    (
    
        GetWindowClassName(), 
        NULL, 
        uiFlags, 
        0,                      // initial x position
        0,                      // initial y position
        GetWidth(),             // initial width
        GetHeight(),            // initial height
        pWnd,                   // parent window handle
        NULL,                   // window menu handle
        NiApplication::GetInstanceReference(), // program instance handle
        NULL                    // creation parameters
   );

    SetRenderWindowReference(pRenderWnd);

    ShowWindow(GetWindowReference(), iWinMode);
    UpdateWindow(GetWindowReference());

    return pWnd;
}
//---------------------------------------------------------------------------

