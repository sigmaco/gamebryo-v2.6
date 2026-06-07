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

#include "stdafx.h"  
#include "NiWin32SceneViewerWindow.h"

#include "NiSharedDataList.h"
#include "NiFramework.h"
#include "NiSceneGraphSharedData.h"

#include <NiOS.h>
#include <NiD3DShaderFactory.h>
#include <NiMaterialToolkit.h>
#include <NiSystem.h>
#include "NiSceneMouse.h"
#include "NiSceneKeyboard.h"
#include <NiTList.h>
#define WM_MOUSEWHEEL                   0x020A

NiInstanceRef NiSceneViewerWindow::ms_pkInstance = 0;
NiAcceleratorRef NiSceneViewerWindow::ms_pkAccel = 0;
char* NiWin32SceneViewerWindow::ms_pcWindowClassName = NULL;
bool NiWin32SceneViewerWindow::ms_bDirect3DCreated = false;

//---------------------------------------------------------------------------
NiSceneViewerWindow* NiWin32SceneViewerWindow::Create(unsigned int uiWidth, 
    unsigned int uiHeight)
{
    NiWin32SceneViewerWindow* pkRetWindow =  NiNew NiWin32SceneViewerWindow(
        "SceneViewerDX9", uiWidth , uiHeight, 0, 0);
    return pkRetWindow;
}
//---------------------------------------------------------------------------
NiWin32SceneViewerWindow::NiWin32SceneViewerWindow(
    const char* pcWindowCaption,    // caption of window
    unsigned int uiWidth,            // client window width
    unsigned int uiHeight,            // client window height
    unsigned int,                    // menu resource ID
    unsigned int,                    // number of panes in status window
    unsigned int uiBitDepth,        // kColor depth of screen
    long ulWindowStyle)                // client window style
{
    // text for window caption
    m_pkSceneViewer = NULL;

    if (ms_pcWindowClassName == NULL)
    {
        ms_pcWindowClassName = NiAlloc(char, 256);
        NiStrcpy(ms_pcWindowClassName, 256, "SceneViewerDX9");
    }

    if (pcWindowCaption && pcWindowCaption[0])
    {
        size_t stLen = strlen(pcWindowCaption) + 1;
        m_pcWindowCaption = NiAlloc(char, stLen);
        NiStrcpy(m_pcWindowCaption, stLen, pcWindowCaption);
    }
    else
    {
        size_t stLen = strlen(ms_pcWindowClassName) + 1;
        m_pcWindowCaption = NiAlloc(char, stLen);
        NiStrcpy(m_pcWindowCaption, stLen, ms_pcWindowClassName);
    }

    // window references
    m_pkWnd = 0;
    m_pkRenderWnd = 0;
    
    // client window dimensions
    m_uiWidth = m_uiParentWidth = uiWidth;
    m_uiHeight = m_uiParentHeight = uiHeight;

    // client window style
    m_ulWindowStyle = ulWindowStyle;

    m_uiBitDepth = uiBitDepth;

    // Renderer defaults, change these in the NiWin32SceneViewerWindow-derived
    // class constructor from command line information or other.
    if (m_uiBitDepth == 0)
        m_bFullscreen = false;
    else
        m_bFullscreen = true;
    m_bHardware = true;
    m_bStencil = false;
    m_bMultitexture = true;
    m_kBackground = NiColor::BLACK;
    m_spDefaultCamNode = NULL;
    m_bD3D10 = false;

    // Set the renderer
    OSVERSIONINFO kInfo;
    kInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&kInfo);

    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
    NiTexture::SetMipmapByDefault(true);
    
    // Set up input device(s)
    m_pkKeyboard = NULL;
    m_pkMouse = NULL;
    m_pkKeyboard = NiNew NiSceneKeyboard;
    m_pkMouse = NiNew NiSceneMouse;
    m_pkMouse->SetKeyboard(m_pkKeyboard);
    m_pkMouse->SetFullscreenMode(m_bFullscreen);

    m_pkSceneEventHandler = NiNew NiSceneEventHandler();
    m_pkSceneEventHandler->SetKeyboard(m_pkKeyboard);
    m_pkSceneEventHandler->SetMouse(m_pkMouse);

    NiSceneCommandRegistry::Init();
    
    // the unique application instance
    m_uiMenuID = 0;
    m_bIsAlive = false;

    
}
//---------------------------------------------------------------------------
NiWin32SceneViewerWindow::~NiWin32SceneViewerWindow()
{
    // Ensure image converter that was used during application is deleted now
    // to free any NiObject derived members contained in NiStreams of image
    // readers.  Otherwise NiWin32SceneViewerWindow will claim object leaks.
    NiImageConverter::SetImageConverter(NULL);
    NiOverdrawIndicatorCommand::CleanUp();
    if (m_pkKeyboard)
        NiDelete m_pkKeyboard;
    if (m_pkMouse)
        NiDelete m_pkMouse;
    m_pkMouse = NULL;
    m_pkKeyboard = NULL;
    
    NiDelete m_pkSceneViewer;
    m_pkSceneViewer = NULL;
    NiDelete m_pkSceneEventHandler;
    m_pkSceneEventHandler = NULL;

    NiFree(m_pcWindowCaption);
    m_pcWindowCaption = NULL;
    m_spDefaultCamNode = NULL;

    // Close the scene
    CloseScene();

    m_bIsAlive = false;
    NiFree(ms_pcWindowClassName);
    ms_pcWindowClassName = NULL;

    NiSceneCommandRegistry::Shutdown();

    // Reload shaders without a renderer so the materials are still available
    // to the artists.
    NiMaterialToolkit::UnloadShaders();

    // Re-enable DX9 version of shader libraries to be consistent
    NiD3DShaderFactory* pkD3DFactory = 
        NiD3DShaderFactory::GetD3DShaderFactory();

    if (!pkD3DFactory->IsActiveFactory())
        pkD3DFactory->SetAsActiveFactory();

    NiMaterialToolkit::ReloadShaders();
}
//---------------------------------------------------------------------------
NiWindowRef NiWin32SceneViewerWindow::CreateMainWindow(
    int iWinMode, NiWindowRef hWnd)
{
    NiWindowRef pkWnd = NULL;
    // require the renderer window to have the specified client area
    RECT kRect = { 0, 0, GetWidth(), GetHeight() };
    AdjustWindowRect(&kRect, GetWindowStyle(), GetMenuID());

    unsigned int uiFlags = WS_CHILD;

    // Create window
    if (hWnd != NULL)
    {
        pkWnd = hWnd;
        uiFlags |= WS_VISIBLE;
        SetWindowReference(pkWnd);
        SetRenderWindowReference(pkWnd);
        AddSceneViewerWindow(GetWindowReference(), this);
        AddSceneViewerWindow(GetRenderWindowReference(), this);
        ShowWindow(GetWindowReference(), iWinMode);
        UpdateWindow(GetWindowReference());
        return pkWnd;
    }
    else
    {
        uiFlags |= WS_VISIBLE;

        pkWnd = CreateWindow
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
            GetInstanceReference(), // program instance handle
            NULL                    // creation parameters
       );
    }
    SetWindowReference(pkWnd);

    // Create render window
    NiWindowRef pkRenderWnd = CreateWindow
    (
        GetWindowClassName(), 
        NULL, 
        uiFlags, 
        0,                      // initial x position
        0,                      // initial y position
        GetWidth(),             // initial width
        GetHeight(),            // initial height
        pkWnd,                   // parent window handle
        NULL,                   // window menu handle
        GetInstanceReference(), // program instance handle
        NULL                    // creation parameters
   );

    SetRenderWindowReference(pkRenderWnd);
    AddSceneViewerWindow(GetWindowReference(), this);
    AddSceneViewerWindow(GetRenderWindowReference(), this);
    ShowWindow(GetWindowReference(), iWinMode);
    UpdateWindow(GetWindowReference());

    return pkWnd;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK NiWin32SceneViewerWindow::WinProc(HWND hWnd, UINT uiMsg, 
    WPARAM wParam, LPARAM lParam)
{
    NiWin32SceneViewerWindow* pkTheApp = GetSceneViewerWindow(hWnd);

    if (!pkTheApp)
        return DefWindowProc(hWnd, uiMsg, wParam, lParam);

    switch (uiMsg) 
    {
        case WM_CREATE:
        {
            LPCREATESTRUCT lpCS = LPCREATESTRUCT(lParam);
            if (pkTheApp->OnWindowCreate(lpCS))
                return 0;
            else
                return -1;
        }
        case WM_DESTROY:
        {
            pkTheApp->OnWindowDestroy(NULL, false);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT kPS;
            HDC hDC = BeginPaint(hWnd, &kPS);
            if (pkTheApp->OnWindowDraw(hDC))
            {
                EndPaint(hWnd, &kPS);
                return 0;
            }
            EndPaint(hWnd, &kPS);
            break;
        }
        case WM_ERASEBKGND:
        {
            HDC hDC = HDC(hWnd);
            if (pkTheApp->OnWindowErase(hDC))
                return 0;
            break;
        }
        case WM_MOVE:
        {
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);

            if (pkTheApp->OnWindowMove(lpPosition->x, lpPosition->y, hWnd))
            {
                NiExternalDelete lpPosition;
                return 0;
            }
            NiExternalDelete lpPosition;
            break;
        }
        case WM_SIZE:
        {
            int iWidth = int(LOWORD(lParam));
            int iHeight = int(HIWORD(lParam));
            unsigned int uiSizeType = (unsigned int)(wParam);
            DefWindowProc(hWnd, uiMsg, wParam, lParam);
            if (pkTheApp->OnWindowResize(iWidth, iHeight, uiSizeType, hWnd))
                return 0;
            break;
        }
        case WM_COMMAND:
        {
            WORD wNotifyCode = HIWORD(wParam);
            WORD wID = LOWORD(wParam);
            HWND hWndCtl = HWND(lParam);
            if (pkTheApp->OnCommand(wNotifyCode, wID, hWndCtl))
                return 0;
            break;
        }
        case WM_SYSCHAR:
        {
            NiSceneKeyboard* pkKeyboard = pkTheApp->GetKeyboard();
            unsigned char ucCharCode = unsigned char(wParam);
            if (iscntrl(ucCharCode))
            {
                ucCharCode = (unsigned char)pkKeyboard->
                    NiTranslateKeyCode(ucCharCode);
            }
            break;
        }
        case WM_SYSKEYDOWN:
        {
            NiSceneKeyboard* pkKeyboard = pkTheApp->GetKeyboard();
            NiSceneKeyboard::KeyCode eNiCharCode = 
                pkKeyboard->NiTranslateKeyCode(unsigned char(wParam));
            pkKeyboard->RecordKeyPress(eNiCharCode);
           break;
        }
        case WM_SYSKEYUP:
        {
            NiSceneKeyboard* pkKeyboard = pkTheApp->GetKeyboard();
            NiSceneKeyboard::KeyCode eNiCharCode = 
                pkKeyboard->NiTranslateKeyCode(unsigned char(wParam));
            pkKeyboard->RecordKeyRelease(eNiCharCode);
            break;
        }
        case WM_CHAR:
        {
            break;
        }
        case WM_KEYDOWN:
        {
            NiSceneKeyboard* pkKeyboard = pkTheApp->GetKeyboard();
            NiSceneKeyboard::KeyCode eNiCharCode = 
                pkKeyboard->NiTranslateKeyCode(unsigned char(wParam));
            pkKeyboard->RecordKeyPress(eNiCharCode);
            break;
        }
        case WM_KEYUP:
        {
            if (wParam == VK_ESCAPE)
                SendMessage(hWnd,WM_CLOSE, 0, 0);
            NiSceneKeyboard* pkKeyboard = pkTheApp->GetKeyboard();
            
            NiSceneKeyboard::KeyCode eNiCharCode = 
                pkKeyboard->NiTranslateKeyCode(unsigned char(wParam));
            
            pkKeyboard->RecordKeyRelease(eNiCharCode);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            SetCapture(hWnd);
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            pkMouse->RecordButtonPress(NiSceneMouse::SCENEMOUSE_LBUTTON);
            /*long m_lModifiers = pkMouse->GetModifiers((unsigned int)wParam);
            NiEventRecord kEvent = {hWnd, uiMsg, wParam, lParam};
            m_lModifiers |= ((long)(pkTheApp->GetMouseModifiers(&kEvent)));

            if (pkTheApp->OnMousePress(lpPosition->x, lpPosition->y, 
                NiSceneMouse::MOUSE_LBUTTON, m_lModifiers))
            {
                NiExternalDelete lpPosition;
                return 0;
            }
            else 
                NiExternalDelete lpPosition;
            */
            NiExternalDelete lpPosition;
            break;
        }
        case WM_LBUTTONUP:
        {
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            
//            long m_lModifiers = pkMouse->GetModifiers((unsigned int)wParam);
//            NiEventRecord kEvent = {hWnd, uiMsg, wParam, lParam};
//            m_lModifiers |= ((long)(pkTheApp->GetMouseModifiers(&kEvent)));
            ReleaseCapture();
            pkMouse->RecordButtonRelease(NiSceneMouse::SCENEMOUSE_LBUTTON);
/*            if (pkTheApp->OnMouseRelease(lpPosition->x, lpPosition->y,
                NiSceneMouse::MOUSE_LBUTTON, m_lModifiers))
            {
                NiExternalDelete lpPosition;
                return 0;
            }
            else 
                NiExternalDelete lpPosition;
            */
            NiExternalDelete lpPosition;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            SetCapture(hWnd);
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            pkMouse->RecordButtonPress(NiSceneMouse::SCENEMOUSE_RBUTTON);

            NiExternalDelete lpPosition;
            break;
        }
        case WM_RBUTTONUP:
        {
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            ReleaseCapture();
            pkMouse->RecordButtonRelease(NiSceneMouse::SCENEMOUSE_RBUTTON);
            NiExternalDelete lpPosition;
            break;
        }
        case WM_MBUTTONDOWN:
        {
            SetCapture(hWnd);
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            pkMouse->RecordButtonPress(NiSceneMouse::SCENEMOUSE_MBUTTON);
            NiExternalDelete lpPosition;
            break;
        }
        case WM_MBUTTONUP:
        {
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            ClientToScreen(hWnd, lpPosition);
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            ReleaseCapture();
            pkMouse->RecordButtonRelease(NiSceneMouse::SCENEMOUSE_MBUTTON);
            NiExternalDelete lpPosition;
            break;
        }
        case WM_MOUSEMOVE:
        {
            LPPOINT lpPosition = NiExternalNew POINT;
            lpPosition->x = SHORT(lParam);
            lpPosition->y = LONG(lParam) >> 16;
            int iX = lpPosition->x;
            int iY = lpPosition->y;
            ClientToScreen(hWnd, lpPosition);

            /*char* pcLine = NiAlloc(char, 256);
            NiSprintf(pcLine, "(%d, %d)\n", lpPosition->x, lpPosition->y);
            NiOutputDebugString(pcLine);
            NiFree(pcLine);
            */
            pkTheApp->GetMouse()->RecordMouseMove(
                lpPosition->x, lpPosition->y);
            //Determine if any buttons are currently down
            //If not, we need to reset the last positions of the mouse when
            //we come close to crossing the window borders
            if (GetCapture() != hWnd)
            {
                if (pkTheApp->IsCloseToWindowBorder(iX, iY))
                {
                    pkTheApp->GetMouse()->ResetPreviousMouseLocation();
                }
            }
            
            NiExternalDelete lpPosition;
            break;
        }
        case WM_MOUSEWHEEL:
        {
            short zDelta = (short) HIWORD(wParam);    // wheel rotation
            NiSceneMouse* pkMouse = pkTheApp->GetMouse();
            pkMouse->RecordWheelMoved(zDelta);
            break;
        }
        default:
        {
            NiEventRecord kEvent = {hWnd, uiMsg, wParam, lParam};
            if (pkTheApp->OnDefault(&kEvent))
                return 0;
            break;
        }
    }

    return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// window painting
//---------------------------------------------------------------------------
// OnWindowDraw()
// Handles WM_PAINT events
//    pDrawContext = HDC
bool NiWin32SceneViewerWindow::OnWindowDraw(NiContextRef)
{
    return false;
}
//---------------------------------------------------------------------------

// OnWindowErase()
// Handles WM_ERASEBKGND events
//    pDrawContext = HDC
bool NiWin32SceneViewerWindow::OnWindowErase(NiContextRef)
{
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// window maintenance
//---------------------------------------------------------------------------
// OnWindowCreate()
// Handles WM_CREATE events
//   pCreateStruceRef = LPCREATESTRUCT lpCS
bool NiWin32SceneViewerWindow::OnWindowCreate(
    NiCreateStructRef)
{
    return true;
}
//---------------------------------------------------------------------------

// OnWindowResize()
// Handles WM_SIZE
//   iWidth = width
//   iHeight = height
//   uiSizeType = fwSizeType
//   pkWnd = unused
bool NiWin32SceneViewerWindow::OnWindowResize(int iWidth, int iHeight, 
    unsigned int, NiWindowRef)
{
    SetParentWidth(iWidth);
    SetParentHeight(iHeight);

    // let application know of new client dimensions
    HWND hRendWnd = GetRenderWindowReference();

    if (hRendWnd)
    {
        SetWidth(iWidth);
        SetHeight(iHeight);

        if (m_pkSceneViewer)
            m_pkSceneViewer->Lock(NiSceneViewer::RENDERER_LOCK);
        NiOutputDebugString("BEGIN Recreating Renderer\n");

        if (m_spRenderer)
        {
            if (m_bD3D10)
            {
                NIASSERT(NiIsKindOf(NiD3D10Renderer, m_spRenderer));
                NiD3D10Renderer* pkD3D10Renderer = NiSmartPointerCast(
                    NiD3D10Renderer, m_spRenderer);
                pkD3D10Renderer->ResizeBuffers(0, 0);
            }
            else
            {
                NIASSERT(NiIsKindOf(NiDX9Renderer, m_spRenderer));
                NiDX9Renderer* pkDX9Renderer = NiSmartPointerCast(
                    NiDX9Renderer, m_spRenderer);
                pkDX9Renderer->Recreate(0, 0);
            }
        }

        if (m_pkSceneViewer)
        {
            unsigned int uiCam = m_pkSceneViewer->GetCurrentCameraID();

            for (unsigned int ui = 0; 
                ui < m_pkSceneViewer->GetCameraCount(); ui++)
            {
                NiCamera* pkCamera = m_pkSceneViewer->GetCamera(ui);
                CalcCameraFrustum(pkCamera);
            }
            m_pkSceneViewer->SelectCamera(uiCam);
        }
        NiOutputDebugString("END Rencreating renderer\n");

        if (m_pkSceneViewer)
            m_pkSceneViewer->Unlock(NiSceneViewer::RENDERER_LOCK);
    }

    return true;
}
//---------------------------------------------------------------------------

// OnWindowDestroy()
// Handles WM_DESTROY
//   pkWnd = unused
//   bOption = unused
bool NiWin32SceneViewerWindow::OnWindowDestroy(NiWindowRef, bool)
{
    // In order to clean up after itself properly, the OpenGL renderer must
    // be destroyed before its window is destroyed.
    m_bIsAlive = false;
    return true;
}
//---------------------------------------------------------------------------

// OnWindowMove()
// Handles WM_MOVE
//   iXPos = X dist
//   iYPos = Y dist
//   pkWnd = unused
bool NiWin32SceneViewerWindow::OnWindowMove(int iXPos, int iYPos, 
    NiWindowRef)
{
    /*LPRECT lpRect = NiExternalNew RECT;
    GetWindowRect(pkWnd, lpRect);
    iXPos = (lpRect->left + lpRect->right)/2;
    iYPos = (lpRect->top + lpRect->bottom)/2;*/
    iXPos = GetParentWidth()/2 + iXPos;
    iYPos = GetParentHeight()/2 + iYPos;
    if (m_pkSceneEventHandler && m_pkSceneEventHandler->GetUI())
        m_pkSceneEventHandler->GetUI()->SetViewportCenter(iXPos,iYPos);
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// system and input processing
//---------------------------------------------------------------------------
//  Process()
//  Checks message loop, which in turn processes input and system calls
//  Returns true to continue, false to exit
bool NiWin32SceneViewerWindow::Process(long* plRetVal)
{
    MSG kMsg;
    kMsg.wParam = 0;
    if (!m_bIsAlive)
        return false;
    if (PeekMessage(&kMsg, NULL, 0, 0, PM_REMOVE))
    {
        if (kMsg.message == WM_QUIT || kMsg.message == WM_DESTROY)
            return false;
        
        if (!TranslateAccelerator(GetWindowReference(), 
            GetAcceleratorReference(), &kMsg))
        {
            TranslateMessage(&kMsg);
            DispatchMessage(&kMsg);
        }
    }
    else
    {
        OnIdle();
    }
    *plRetVal = (long)kMsg.wParam;
    return true;
}

//---------------------------------------------------------------------------
// default processing
//---------------------------------------------------------------------------
// OnDefault()
// Handles all other events
//   pEventRecord = event record
bool NiWin32SceneViewerWindow::OnDefault(NiEventRef)
{
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// idle processing
//---------------------------------------------------------------------------
// OnIdle()
// Handles idle time
void NiWin32SceneViewerWindow::OnIdle(void)
{
    m_pkSceneViewer->MainLoop();
}
//---------------------------------------------------------------------------
long NiWin32SceneViewerWindow::MainLoopTick()
{
    long lResult = NiSceneViewerWindow::MainLoopTick();
    return lResult;
}
//---------------------------------------------------------------------------
bool NiWin32SceneViewerWindow::CreateRenderer()
{
    // The application calls SetRenderer to get appropriate renderer.  The
    // default value is NI_DX9.

    m_spRenderer = NULL;

    ms_bDirect3DCreated = true;

    if (m_bD3D10)
    {
        NiD3D10Renderer::CreationParameters kParams(
            GetRenderWindowReference());
        if (m_bFullscreen)
        {
            kParams.m_kSwapChain.Windowed = false;
        }
        NiD3D10RendererPtr spD3D10Renderer;
        bool bSuccess = NiD3D10Renderer::Create(kParams, spD3D10Renderer);
        if (bSuccess)
        {
            m_spRenderer = spD3D10Renderer;
        }
        else
        {
            NiMessageBox("A D3D10 renderer could not be created. Using DX9 "
                "instead.", "D3D10 Renderer Creation Failure");
            m_bD3D10 = false;
        }
    }
    if (!m_bD3D10)
    {
        unsigned int uiFlags = NiDX9Renderer::USE_MULTITHREADED;
        if (m_bStencil)
        {
            uiFlags |= NiDX9Renderer::USE_STENCIL;
        }

        if (m_uiBitDepth == 16 || m_uiBitDepth == 32)
        {
            uiFlags |= NiDX9Renderer::USE_FULLSCREEN;

            if (m_uiBitDepth == 16)
            {
                uiFlags |= NiDX9Renderer::USE_16BITBUFFERS;
            }

            m_spRenderer = NiDX9Renderer::Create(GetWidth(), GetHeight(), 
                (NiDX9Renderer::FlagType) uiFlags, GetWindowReference(),
                GetWindowReference());
        }
        else
        {
            if (m_bFullscreen)
            {
                uiFlags |= NiDX9Renderer::USE_FULLSCREEN;
    
                m_spRenderer = NiDX9Renderer::Create(GetWidth(),
                    GetHeight(), (NiDX9Renderer::FlagType) uiFlags, 
                    GetWindowReference(), GetWindowReference());
            }
            else
            {
                m_spRenderer = NiDX9Renderer::Create(0, 0, 
                    (NiDX9Renderer::FlagType) uiFlags, 
                    GetRenderWindowReference(),
                    GetRenderWindowReference());
            }
        }
    }

    if (m_spRenderer)
    {
        m_bHardware = true;
    }
    else
    {
        NiMessageBox("The Renderer was not created!\n"
            "Your hardware may be incompatible with this mode.", 
            "Renderer Creation Failure");
        DestroyWindow(GetWindowReference());
        return false;
    }

    m_spRenderer->SetBackgroundColor(m_kBackground);

    return true;
}
//---------------------------------------------------------------------------
bool NiWin32SceneViewerWindow::IsCloseToWindowBorder(int iX, int iY)
{
    bool bReturn = false;
    if (iX < 20 || iX > (int)(m_uiWidth) - 20 ||
        iY < 20 || iY > (int)(m_uiHeight) - 20)
    {
        bReturn = true;
    }

/*    int b = (int) bReturn;
    char* pcLine = NiAlloc(char, 256);
    NiSprintf(pcLine, 256, "(%d,%d) = %d\n", iX, iY, b);
    NiOutputDebugString(pcLine);
    NiDelete pcLine;*/
    return bReturn;
}
//---------------------------------------------------------------------------
void AddSceneViewerWindow(NiWindowRef hWnd, NiWin32SceneViewerWindow* pkWindow)
{
    SetWindowLong(hWnd, GWL_USERDATA, PtrToLong(pkWindow));
}
//---------------------------------------------------------------------------
NiWin32SceneViewerWindow* GetSceneViewerWindow(NiWindowRef hWnd)
{
    NiWin32SceneViewerWindow* pkRetWindow = NULL;
    long lValue = GetWindowLong(hWnd, GWL_USERDATA);
    if (lValue == 0)
        return NULL;

    pkRetWindow = (NiWin32SceneViewerWindow*) LongToPtr(lValue);
    return pkRetWindow;
};
//---------------------------------------------------------------------------
bool NiWin32SceneViewerWindow::Terminate()
{
    return true;
}
//---------------------------------------------------------------------------
