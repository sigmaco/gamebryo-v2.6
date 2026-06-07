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

#ifndef NIWIN32SCENEVIEWERWINDOW_H
#define NIWIN32SCENEVIEWERWINDOW_H

#include "NiSceneViewerWindow.h"

class NiWin32SceneViewerWindow : public NiSceneViewerWindow
{
public:    
    NiWin32SceneViewerWindow(
        const char* pcWindowCaption,       // caption of window
        unsigned int uiWidth,              // client window width
        unsigned int uiHeight,             // client window height
        unsigned int uiMenuID = 0,         // menu resource ID
        unsigned int uiNumStatusPanes = 0, // number of panes in status window
        unsigned int uiBitDepth = 0 ,       // kColor depth of screen
        long ulWindowStyle =                // client window style
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
    );
    inline void SetWindowClassName(const char* pcClassName);
    bool IsCloseToWindowBorder(int iX, int iY);
    static NiSceneViewerWindow* Create(unsigned int uiWidth,
        unsigned int uiHeight);

    long MainLoopTick();
    ~NiWin32SceneViewerWindow();
    NiWindowRef CreateMainWindow(int iWinMode, NiWindowRef hWnd = NULL);
    void OnIdle();

    bool Terminate();
    
    static const char* GetWindowClassName();
    inline unsigned long GetWindowStyle();
    inline void SetWindowStyle(unsigned long ulWindowStyle);

    // window caption
    inline void SetWindowCaption(char* pcCaption);
    inline const char* GetWindowCaption() inline const;
    
    // window painting
    virtual bool OnWindowDraw(NiContextRef pkDC);
    virtual bool OnWindowErase(NiContextRef pkDC);

    // window maintenance
    virtual bool OnWindowCreate(NiCreateStructRef pkCreateStructRef);
    virtual bool OnWindowResize(int iWidth, int iHeight, 
        unsigned int uiSizeType, NiWindowRef pkWnd);
    virtual bool OnWindowDestroy(NiWindowRef pkWnd, bool bOption);
    virtual bool OnWindowMove(int iXPos, int iYPos, NiWindowRef pkWnd);

    inline unsigned int GetMenuID() const;

    static LRESULT CALLBACK WinProc(HWND hWnd, UINT uiMsg, WPARAM wParam, 
        LPARAM lParam);

    // command processing
    // input/system processing (returns true to continue, false to exit)
    bool Process(long* plRetVal);

    // default processing
    bool OnDefault(NiEventRef pEventRecord);
protected:
    virtual bool CreateRenderer();
    
    static char* ms_pcWindowClassName;

    unsigned int m_uiMenuID;

    static bool ms_bDirect3DCreated;

    char* m_pcWindowCaption;

    unsigned long m_ulWindowStyle;
};

NiSmartPointer(NiWin32SceneViewerWindow);

static void  AddSceneViewerWindow(NiWindowRef hWnd, 
    NiWin32SceneViewerWindow* pkWindow);
static NiWin32SceneViewerWindow* GetSceneViewerWindow(NiWindowRef hWnd);

#include "NiWin32SceneViewerWindow.inl"
#endif
