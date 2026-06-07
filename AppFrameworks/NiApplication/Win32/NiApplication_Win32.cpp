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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiApplicationPCH.h"

#include "NiApplication.h"
#include "NiApplicationMetrics.h"
#include "NiDX9RendererDesc.h"
#include "NiD3D10RendererDesc.h"
#include <NiFragmentMaterial.h>
#include <NiMeshCullingProcess.h>
#include <NiTNodeTraversal.h>
#include <NiDI8InputSystem.h>
#include <NiDebug.h>
#include <commctrl.h>

// Win32 NiApplication used to automatically insert references to 
// NiSystem.lib and NiMain.lib via a #pragma comment directive.
// These libraries are still required by any NiApplication-derived 
// application, but they must now be linked in explicitly by the app.
// Use of the #pragma NiSystem.lib reference is incompatible with 
// NiSystem's global operator new overload, which is enabled
// if NI_USE_MEMORY_MANAGEMENT is defined.

#ifdef NIVISUALTRACKER_IMPORT
    #pragma comment(lib, "NiVisualTracker" NI_DLL_SUFFIX ".lib")
#else   // #ifdef NIVisualTracker_IMPORT
    #pragma comment(lib, "NiVisualTracker.lib")
#endif  // #ifdef NIVisualTracker_IMPORT

#if NIMETRICS
    #ifdef NIMETRICSOUTPUT_IMPORT
        #pragma comment(lib, "NiMetricsOutput" NI_DLL_SUFFIX ".lib")
    #else // #ifdef NIMETRICSOUTPUT_IMPORT
        #pragma comment(lib, "NiMetricsOutput.lib")
    #endif // #ifdef NIMETRICSOUTPUT_IMPORT
#endif // #if NIMETRICS

const unsigned int NiApplication::DEFAULT_WIDTH = 1024;
const unsigned int NiApplication::DEFAULT_HEIGHT = 768;

char NiApplication::ms_acMediaPath[NI_MAX_PATH] = "./../../Data/";
char NiApplication::ms_acTempMediaFilename[NI_MAX_PATH];

NiRenderer::EScreenshotFormat NiApplication::ms_eScreenshotFormat = 
    NiRenderer::FORMAT_PNG;

//---------------------------------------------------------------------------
NiApplication::NiApplication(const char* pcWindowCaption, 
    unsigned int uiWidth, unsigned int uiHeight, bool bUseFrameSystem,
    unsigned int uiMenuID, unsigned int uiNumStatusPanes,
    unsigned int uiBitDepth) :
    m_kVisible(1024, 1024), 
    m_bUseFrameSystem(bUseFrameSystem), 
    m_bUseFixedTime(false), 
    m_fFixedTimeIncrement(1.0f / 60.0f), 
    m_fAppEndTime(NI_INFINITY), 
    m_bDumpShotAtFixedInterval(false), 
    m_fDumpInterval(-NI_INFINITY), 
    m_fLastDumpTime(-NI_INFINITY), 
    m_uiScreenShotId(0)
{
    m_pkAppWindow = NiNew NiAppWindow(pcWindowCaption, uiWidth, uiHeight, 
        uiNumStatusPanes);

    m_uiMenuID = uiMenuID;
    m_uiBitDepth = uiBitDepth;

    // performance measurements (display in status pane 0 if enabled)
    m_fLastTime = -1.0f;
    m_fAccumTime = 0.0f;
    m_fCurrentTime = 0.0f;
    m_fFrameTime = 0.0f;
    m_iClicks = 0;
    m_fFrameRate = 0.0f;

    m_fMinFramePeriod = 1.0f / 100.0f;
    m_fLastFrame = 0.0f;

    m_uiNumObjectsDrawn = 0;
    m_fCullTime = 0.0f;
    m_fRenderTime = 0.0f;
    m_fUpdateTime = 0.0f;
    m_fBeginUpdate = 0.0f;
    m_fBeginCull = 0.0f;
    m_fBeginRender = 0.0f;

    // Renderer defaults, change these in the NiApplication-derived class
    // constructor from command line information or other.
    if (m_uiBitDepth == 0)
        m_bFullscreen = false;
    else
        m_bFullscreen = true;
    m_bStencil = false;
    m_bRendererDialog = true;

    // Default is no multithread-capable renderers.
    m_bMultiThread = false;

    m_pkFrameRate = NULL;
    m_bFrameRateEnabled = false;

    // Set the renderer
    OSVERSIONINFO kInfo;
    kInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&kInfo);

    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
    NiTexture::SetMipmapByDefault(true);

    // the unique application instance
    ms_pkApplication = this;

    // Initialize frame rendering system object names.
    m_kRenderFrameName = "NiApplication Render Frame";
    m_kShadowRenderStepName = "NiApplication Shadow Render Step";
    m_kMainRenderStepName = "NiApplication Main Scene Render Step";
    m_kMainRenderClickName = "NiApplication Main Scene Render Click";
    m_kMainRenderViewName = "NiApplication Main Scene Render View";
    m_kScreenSpaceRenderStepName = "NiApplication Screen Space Render Step";
    m_kMeshScreenElementsRenderClickName = "NiApplication Mesh Screen "
        "Elements Render Click";
    m_kMeshScreenElementsRenderViewName = "NiApplication Mesh Screen "
        "Elements Render View";
    m_kVisualTrackerRenderClickName = "NiApplication Visual Tracker Render "
        "Click";

    m_bD3D10Renderer = false;
    m_bRefRast = false;
    m_bSWVertex = false;
    m_bExclusiveMouse = false;
    m_bNVPerfHUD = false;

    // Configure the visual trackers
    m_bShowAllTrackers = false;

    m_fVTPerformanceMax = 100.0f;
    m_fVTMemoryMax = 256000.0f;
    m_fVTTimeMax = 100.0f;

    // Create the mesh screen elements render view.
    m_spMeshScreenElementsRenderView = NiNew NiMesh2DRenderView;
    m_spMeshScreenElementsRenderView->SetName(
        m_kMeshScreenElementsRenderViewName);

    // Create the visual tracker render click.
    m_spVisualTrackerRenderClick = NiNew NiVisualTrackerRenderClick;
    m_spVisualTrackerRenderClick->SetName(m_kVisualTrackerRenderClickName);

    // Create the culling process.
    m_spCuller = NiNew NiMeshCullingProcess(&m_kVisible, NULL);
}
//---------------------------------------------------------------------------
NiApplication::~NiApplication()
{
    // Ensure image converter that was used during application is deleted now
    // to free any NiObject derived members contained in NiStreams of image
    // readers.  Otherwise NiApplication will claim object leaks.
    NiImageConverter::SetImageConverter(NiNew NiImageConverter);

    NiDelete ms_pkCommand;

    NiDelete m_pkAppWindow;

    NiDelete NiRendererSettings::GetInstance();

    ms_pkApplication = 0;
    ms_pkCommand = 0;
}
//---------------------------------------------------------------------------
static NiUInt32 DefineTestAppWorkerThreadCount()
{
    // This function is only used for internal testing.
    return 1;
}
//---------------------------------------------------------------------------
void NiApplication::CommandSelectRenderer()
{
    // process renderer-specific command line options
    // Now command line is parsed by NiRendererSettings::ParseCommandLine

    if (ms_pkCommand)
    {
        // Automated testing checks
        if (ms_pkCommand->Boolean("test") || 
            ms_pkApplication->m_bDumpShotAtFixedInterval)
        {
            NiInitTestEnvironment();

            // Run using fixed time for 10 simulated seconds
            ms_pkApplication->m_bUseFixedTime = true;
            ms_pkApplication->m_fFixedTimeIncrement = 1.0f / 60.0f;
            ms_pkApplication->m_fAppEndTime = 10.0f;

            // Dump a screenshot every 5 simulated seconds
            ms_pkApplication->m_bDumpShotAtFixedInterval = true;
            ms_pkApplication->m_fDumpInterval = 5.0f;

            // Set Floodgate to run in serial, for better image consistency
            // when using rand().  Because Floodgate has already been
            // initialized with some number of worker threads, restart it.
            NiStreamProcessor::Get()->SetDefineWorkerThreadCountFunc(
                &DefineTestAppWorkerThreadCount);
            NiFloodgateSDM::Shutdown();
            NiFloodgateSDM::Init();

            // Since in "test" mode, skip "Select" dialog.
            ms_pkApplication->m_bRendererDialog = false;

            // Make sure that assertions do not stall the 
            // automated test cycle
            NiAssertFail::ms_pfnNiAssertFailProc = 
                NiAssertFail::SimpleAssertFail;

            // Seed the random number generator to a constant value.
            NiSrand(42);

            // Disable blocking message boxes
            NiMessageBoxUtilities::SetMessageBoxFunction(
                &DebugOutMessageBoxFunc);
        }

        // Update NiRendererSettings with app defaults for compatibility
        NiRendererSettings::SyncFromNiApplication();
    }
}
//---------------------------------------------------------------------------
bool NiApplication::CreateCamera()
{
    m_spCamera = NiNew NiCamera;
    NIASSERT(m_spCamera);

    float fAspectRatio = 1.0f;
    if (m_spRenderer)
    {
        Ni2DBuffer* pkBackbuffer = m_spRenderer->GetDefaultBackBuffer();
        fAspectRatio = (float)pkBackbuffer->GetWidth() / 
            (float)pkBackbuffer->GetHeight();
    }
    else
    {
        fAspectRatio = (float)m_pkAppWindow->GetWidth() / 
            (float)m_pkAppWindow->GetHeight();
    }

    // Setup the camera frustum and viewport
    float fVerticalFieldOfViewDegrees = 20.0f;
    float fVerticalFieldOfViewRad =
        NI_PI / 180.0f * fVerticalFieldOfViewDegrees;
    float fViewPlaneHalfHeight = tanf(fVerticalFieldOfViewRad * 0.5f);
    float fViewPlaneHalfWidth = fViewPlaneHalfHeight * fAspectRatio;

    NiFrustum kFrustum = NiFrustum(
        -fViewPlaneHalfWidth, fViewPlaneHalfWidth, 
        fViewPlaneHalfHeight, -fViewPlaneHalfHeight,
        1.0f, 1000.0f);
    NiRect<float> kPort(0.0f, 1.0f, 1.0f, 0.0f);
    m_spCamera->SetViewFrustum(kFrustum);
    m_spCamera->SetViewPort(kPort);

    return true;
}
//---------------------------------------------------------------------------
void NiApplication::AdjustCameraAspectRatio(NiCamera* pkCamera)
{
    NIASSERT(m_spRenderer);
    if (!m_spRenderer)
        return;

    // Correct camera's aspect ratio
    Ni2DBuffer* pkBackbuffer = m_spRenderer->GetDefaultBackBuffer();
    pkCamera->AdjustAspectRatio(
        pkBackbuffer->GetWidth() / (float) pkBackbuffer->GetHeight());
}
//---------------------------------------------------------------------------
bool NiApplication::CreateRenderer()
{
    const char* pcWorkingDir = ConvertMediaFilename("Shaders\\Generated");
    NiMaterial::SetDefaultWorkingDirectory(pcWorkingDir);

    // Set defaults in NiRendererSettings from NiApplication
    NiRendererSettings::SyncFromNiApplication();

    // Load settings and parse commandline
    NiRendererSettings::LoadSettings();
    NiRendererSettings::ParseCommandLine();

    // Try to create renderer. If creation fails, ask user to correct settings
    // and show settings dialog again. If user press Cancel, exit.
    while (!m_spRenderer)
    {
        if (NiRendererSettings::GetInstance()->m_bRendererDialog)
        {
            // Create an instance of settings dialog
            NiSettingsDialog* pkDialog = NiNew NiSettingsDialog();

            // Exit if dialog fail to initialize. It will show a cause of error
            if (!pkDialog->InitDialog())
                return false;

            // Exit if user have pressed a cancel button
            if (!pkDialog->ShowDialog())
                return false;

            // Delete dialog instance
            NiDelete pkDialog;
        }

        NiRendererSettings::SyncToNiApplication();

        // Focus window handle
        NiWindowRef pFocusWnd =
            NiApplication::ms_pkApplication->GetWindowReference();

        // Resize it to new resolution
        RECT kRect = 
        { 
            0, 
            0, 
            NiRendererSettings::GetInstance()->m_uiScreenWidth, 
            NiRendererSettings::GetInstance()->m_uiScreenHeight
        };
        
        AdjustWindowRect(&kRect, 
            NiApplication::ms_pkApplication->GetAppWindow()->GetWindowStyle(), 
            NiApplication::ms_pkApplication->GetMenuID());

        SetWindowPos(
            pFocusWnd,
            HWND_TOP,
            0,
            0,
            kRect.right - kRect.left,
            kRect.bottom - kRect.top,
            SWP_NOMOVE);

        NiWindowRef pRenderWnd;

        // Get appropriate window handle for CreateRenderer
        if (NiRendererSettings::GetInstance()->m_bFullscreen)
        {
            pRenderWnd = NiApplication::ms_pkApplication->GetWindowReference();
        }
        else
        {
            pRenderWnd =
                NiApplication::ms_pkApplication->GetRenderWindowReference();
        }

        // Call appropriate renderer creation function
        if (NiRendererSettings::GetInstance()->m_bD3D10Renderer)
        {
            m_spRenderer = NiD3D10RendererDesc::CreateD3D10Renderer(pFocusWnd);
        }
        else
        {
            m_spRenderer =
                NiDX9RendererDesc::CreateDX9Renderer(pRenderWnd, pFocusWnd);
        }

        if (!m_spRenderer)
        {
            NiRendererSettings::GetInstance()->m_bRendererDialog = true;
            NiMessageBox(
                "Unable to create renderer.\nPlease correct settings\n"
                "and try again.",
                "Renderer creation error");
        }
    }

    // Update NiApplication settings from NiRendererSettings
    // This variables are protected in NiApplication and have no accessors,
    // so this piece of code cannot be moved to SyncToNiApplication()
    ms_pkApplication->m_bD3D10Renderer =
        NiRendererSettings::GetInstance()->m_bD3D10Renderer;
    ms_pkApplication->m_bRefRast =
        NiRendererSettings::GetInstance()->m_bRefRast;
    ms_pkApplication->m_bNVPerfHUD =
        NiRendererSettings::GetInstance()->m_bNVPerfHUD;
    if (NiRendererSettings::GetInstance()->m_eVertexProcessing ==
        NiRendererSettings::VERTEX_SOFTWARE)
    {
        ms_pkApplication->m_bSWVertex = true;
    }
    else
    {
        ms_pkApplication->m_bSWVertex = false;
    }

    // Save settings if asked and renderer creation succeedeed
    if (NiRendererSettings::GetInstance()->m_bSaveSettings ||
        !NiRendererSettings::GetInstance()->m_bRendererDialog)
    {
        NiRendererSettings::SaveSettings();
    }

    m_spRenderer->SetBackgroundColor(NiColor(0.5f, 0.5f, 0.5f));

    if (ms_pkCommand->Boolean("ffp"))
    {
        m_spRenderer->UseLegacyPipelineAsDefaultMaterial();

        char acBuffer[1024];
        NiStrcpy(acBuffer, sizeof(acBuffer), 
            GetAppWindow()->GetWindowCaption());
        NiStrcat(acBuffer, sizeof(acBuffer), " (FFP)");
        GetAppWindow()->SetWindowCaption(acBuffer);
    }

    // If full screen, send messages to debug output window 
    if (m_bFullscreen)
        NiMessageBoxUtilities::SetMessageBoxFunction(&DebugOutMessageBoxFunc);

    return true;
}
//---------------------------------------------------------------------------
bool NiApplication::CreateInputSystem()
{
    // Create and initialize parameters for the input system
    NiInputSystem::CreateParams* pkParams = GetInputSystemCreateParams();

    // Create the input system
    m_spInputSystem = NiInputSystem::Create(pkParams);
    NiDelete pkParams;
    if (!m_spInputSystem)
    {
        NiMessageBox("CreateInputSystem: Creation failed.", 
            "NiApplication Error");
        return false;
    }

    // The creation of the input system automatically starts an enumeration
    // of the devices. 
    NiInputErr eErr = m_spInputSystem->CheckEnumerationStatus();
    switch (eErr)
    {
    case NIIERR_ENUM_NOTRUNNING:
        NIASSERT(!"EnumerateDevices failed?");
        return false;
    case NIIERR_ENUM_FAILED:
        NIASSERT(!"CheckEnumerationStatus> FAILED!");
        return false;
    case NIIERR_ENUM_COMPLETE:
    case NIIERR_ENUM_NOTCOMPLETE:
    default:
        break;
    }

    // On Win32, assume there is a mouse and keyboard
    if (!m_spInputSystem->OpenMouse() || !m_spInputSystem->OpenKeyboard())
    {
        NiMessageBox("CreateInputSystem: Mouse or keyboard failed to open.", 
            "NiApplication Error");
        return false;
    }

    // Gamepad may not exist, but attempt to open any
    for (unsigned int iGamePad = 0; 
        iGamePad < NiInputSystem::MAX_GAMEPADS;
        iGamePad++
        )
    {
        m_spInputSystem->OpenGamePad(iGamePad,0);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiApplication::CreateVisualTrackers()
{
    const float fLeftBorder = 0.05f;
    const float fTopBorder = 0.025f;
    const float fRegionHeight = 0.25f;
    const float fRegionWidth = 0.90f;

    NiRect<float> kWindowRect1;
    kWindowRect1.m_left   = fLeftBorder;
    kWindowRect1.m_right  = kWindowRect1.m_left + fRegionWidth;
    kWindowRect1.m_top    = 0.15f;
    kWindowRect1.m_bottom = kWindowRect1.m_top + fRegionHeight;

    NiRect<float> kWindowRect2;
    kWindowRect2.m_left   = fLeftBorder;
    kWindowRect2.m_right  = kWindowRect2.m_left + fRegionWidth;
    kWindowRect2.m_top    = kWindowRect1.m_bottom + fTopBorder;
    kWindowRect2.m_bottom = kWindowRect2.m_top + fRegionHeight;

    NiRect<float> kWindowRect3;
    kWindowRect3.m_left   = fLeftBorder;
    kWindowRect3.m_right  = kWindowRect3.m_left + fRegionWidth;
    kWindowRect3.m_top    = kWindowRect2.m_bottom + fTopBorder;
    kWindowRect3.m_bottom = kWindowRect3.m_top + fRegionHeight;

    NiColor blue(0.0f, 0.0f, 1.0f);
    NiColor yellow(1.0f, 1.0f, 0.0f);
    NiColor red(1.0f, 0.0f, 0.0f);

    NiVisualTracker* pkTracker = NULL;

    // First Tracker (FPS vs Objects drawn)
    pkTracker = NiNew NiVisualTracker(m_fVTPerformanceMax, 0, kWindowRect1, 
        "Performance", true, 2);
    pkTracker->SetShow(false);

    pkTracker->AddGraph(NiNew FrameRateUpdate(&m_fFrameRate), 
        "Frame-Rate", red, 100, 0.1f, true);
    if (m_bUseFrameSystem)
    {
        pkTracker->AddGraph(NiNew GenericUnsignedIntUpdate(1.0f,
            &m_uiNumObjectsDrawn), "Vis Objects", yellow, 100, 0.1f, true);
    }
    else
    {
        pkTracker->AddGraph(NiNew VisibleArrayUpdate(m_kVisible),
            "Vis Objects", yellow, 100, 0.1f, true);
    }

    GetVisualTrackers().AddTail(pkTracker);

    // Second Tracker (Timing)
    pkTracker =  NiNew NiVisualTracker(m_fVTTimeMax, 0, kWindowRect2, 
            "Time (ms)", true, 3);
    pkTracker->SetShow(false);

    pkTracker->AddGraph(NiNew GenericFloatUpdate(0.001f, &m_fUpdateTime), 
        "Update", red, 100, 0.1f, true);
    pkTracker->AddGraph(NiNew GenericFloatUpdate(0.001f, &m_fCullTime), 
        "Cull", yellow, 100, 0.1f, true);
    pkTracker->AddGraph(NiNew GenericFloatUpdate(0.001f, &m_fRenderTime), 
        "Render", blue, 100, 0.1f, true);

    GetVisualTrackers().AddTail(pkTracker);

#ifdef NI_MEMORY_DEBUGGER
    // Third Tracker (memory)
    NiMemTracker *pkMemTracker = NiMemTracker::Get();
    if (pkMemTracker)
    {
        pkTracker = NiNew NiVisualTracker(m_fVTMemoryMax, 0, kWindowRect3, 
            "Memory (kb)", true, 3);
        pkTracker->SetShow(false);

        pkTracker->AddGraph(NiNew MemHighWaterMarkUpdate(
            1024.0f, pkMemTracker), "High Watermark",
            red, 100, 0.1f, true);
        pkTracker->AddGraph(NiNew MemCurrentUpdate(
            1024.0f, pkMemTracker), "Current",
            yellow, 100, 0.1f, true);
        pkTracker->AddGraph(NiNew MemCurrentAllocCountUpdate(
            1.0f, pkMemTracker), "Num Allocs",
            blue, 100, 0.1f, true);

        GetVisualTrackers().AddTail(pkTracker);
    }
#endif

    return true;
}
//---------------------------------------------------------------------------
// This function is platform-specific...
// It is provided here to allow applications to setup the creation parameters
// for the input system to their liking, but still allow the NiApplication
// framework to create the input system.
//---------------------------------------------------------------------------
NiInputSystem::CreateParams* NiApplication::GetInputSystemCreateParams()
{
    NiDI8InputSystem::DI8CreateParams* pkParams = 
        NiNew NiDI8InputSystem::DI8CreateParams();
    NIASSERT(pkParams);

    pkParams->SetKeyboardUsage(static_cast<unsigned int>(
        NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE));

    unsigned int uiMouseFlags = NiInputSystem::FOREGROUND;
    if (m_bExclusiveMouse)
    {
        uiMouseFlags |= NiInputSystem::EXCLUSIVE;
    }
    else
    {
        uiMouseFlags |= NiInputSystem::NONEXCLUSIVE;
    }

    pkParams->SetMouseUsage(uiMouseFlags);
    pkParams->SetGamePadCount(2);
    pkParams->SetAxisRange(-100, +100);
    pkParams->SetOwnerInstance(GetInstanceReference());
    pkParams->SetOwnerWindow(GetWindowReference());

    return pkParams;
}
//---------------------------------------------------------------------------
void NiApplication::SetMaxFrameRate(float fMax)
{
    // convert from rate to period - if a bogus max framerate is passed in,
    // disable framerate limitation
    if(fMax < 1e-5f)
        m_fMinFramePeriod = 1e-5f;    
    else
        m_fMinFramePeriod = 1.0f / fMax;    
}
//---------------------------------------------------------------------------
bool NiApplication::Initialize()
{
    // The ShadowManager needs to be initialized before the renderer is
    // created.
    if (m_bUseFrameSystem)
    {
        NiShadowManager::Initialize();
    }

    if (!CreateRenderer())
        return false;

    if (!CreateCamera())
        return false;

    if (!CreateInputSystem())
        return false;

    NIMETRICS_APPLICATION_TIMER(NiMetricsClockTimer, kTimer, 
        CREATE_SCENE_TIME);
    NIMETRICS_APPLICATION_STARTTIMER(kTimer);
    if (!CreateScene())
    {
        NIMETRICS_APPLICATION_ENDTIMER(kTimer);
        return false;
    }
    NIMETRICS_APPLICATION_ENDTIMER(kTimer);

    if (m_bUseFrameSystem && !CreateFrame())
        return false;

    if (m_spScene)
    {
        m_spScene->Update(0.0f);
        m_spScene->UpdateProperties();
        m_spScene->UpdateEffects();
    }

    if (m_spCamera)
    {
        m_spCamera->Update(0.0f);
    }

    if (m_spRenderer != NULL)
    {
        if (!CreateVisualTrackers())
            return false;
    }

    NIMETRICS_APPLICATION_EVENT(INITIALIZED, 1.0f);

    return true;
}
//---------------------------------------------------------------------------
void NiApplication::HidePointer()
{
    ShowCursor(false);
}
//---------------------------------------------------------------------------
void NiApplication::ShowPointer()
{
    ShowCursor(true);
}
//---------------------------------------------------------------------------
void NiApplication::SetMediaPath(const char* pcPath)
{
    NiStrcpy(ms_acMediaPath, NI_MAX_PATH, pcPath);
}
//---------------------------------------------------------------------------
void NiApplication::ProcessInput()
{
    ProcessVisualTrackerInput();
    // Minimally, we provide a way to quit if the NiApplication derived app 
    // does not override with it's own ProcessInput.
    NiInputKeyboard* pkKeyboard = GetInputSystem()->GetKeyboard();
    if (pkKeyboard)
    {
        if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_ESCAPE))
        {
            QuitApplication();
        }
        else if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_F2))
        {
            // We have overriden Initialize so that we can configure our
            // input devices.
            ConfigureInputDevices();
        }
    }

    // Allow ANY gamepad to exit the app
    NiInputGamePad* pkGamePad;
    for (unsigned int uiPort = 0; uiPort < NiInputSystem::MAX_GAMEPADS; 
        uiPort++)
    {
        pkGamePad = m_spInputSystem->GetGamePad(uiPort);
        if (pkGamePad)
        {
            if (pkGamePad->ButtonIsDown(NiInputGamePad::NIGP_START) &&
                pkGamePad->ButtonIsDown(NiInputGamePad::NIGP_SELECT))
            {
                QuitApplication();
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiApplication::QuitApplication()
{
    PostMessage(GetAppWindow()->GetWindowReference(), WM_DESTROY, 0, 0);
}
//---------------------------------------------------------------------------
void NiApplication::MainLoop()
{
    for( ; ; )
    {
        if (!Process())
            break;
    }
}
//---------------------------------------------------------------------------
LRESULT CALLBACK NiApplication::WinProc(HWND hWnd, UINT uiMsg, 
    WPARAM wParam, LPARAM lParam)
{
    NiApplication* pkTheApp = NiApplication::ms_pkApplication;

    if (!pkTheApp)
        return DefWindowProc(hWnd, uiMsg, wParam, lParam);

    switch (uiMsg) 
    {
        case WM_DESTROY:
        {
            pkTheApp->OnWindowDestroy(NULL, false);
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE:
        {
            int iWidth = int(LOWORD(lParam));
            int iHeight = int(HIWORD(lParam));
            unsigned int uiSizeType = (unsigned int)(wParam);
            if (pkTheApp->OnWindowResize(iWidth, iHeight, uiSizeType, NULL))
                return 0;
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
// OnWindowResize() handles WM_SIZE
//   iWidth = width
//   iHeight = height
//   uiSizeType = fwSizeType
//   pWnd = unused
bool NiApplication::OnWindowResize(int iWidth, int iHeight, 
    unsigned int uiSizeType, NiWindowRef)
{
    m_pkAppWindow->SetParentWidth(iWidth);
    m_pkAppWindow->SetParentHeight(iHeight);

    // let application know of new client dimensions
    RECT kRect;
    unsigned int uiNumPanes = m_pkAppWindow->GetNumStatusPanes();
    if (uiNumPanes && m_pkAppWindow->GetStatusPanesExist())
    {
        // resize the status window
        HWND hStatusWnd = m_pkAppWindow->GetStatusWindowReference();

        WPARAM wParam = uiSizeType;
        LPARAM lParam = MAKELPARAM(iWidth, iHeight);
        SendMessage(hStatusWnd, WM_SIZE, wParam, lParam);

        // specify number of piPanes and repaint
        GetClientRect(m_pkAppWindow->GetWindowReference(), &kRect);
        int* piPanes = NiAlloc(int, uiNumPanes);
        for (unsigned int i = 0; i < uiNumPanes-1; i++)
            piPanes[i] = (i+1)*kRect.right/uiNumPanes;
        piPanes[uiNumPanes-1] = kRect.right;

        wParam = WPARAM(uiNumPanes);
        lParam = LPARAM(piPanes);
        SendMessage(hStatusWnd, SB_SETPARTS, wParam, lParam);

        wParam = 0;
        lParam = 0;
        SendMessage(hStatusWnd, WM_PAINT, wParam, lParam);

        NiFree(piPanes);
    }

    HWND hRendWnd = m_pkAppWindow->GetRenderWindowReference();

    if (hRendWnd)
    {
        // resize the rendering window 
        RECT kMainRect;
        GetClientRect(GetWindowReference(), &kMainRect);
        int iClientWidth = kMainRect.right - kMainRect.left;
        int iClientHeight = kMainRect.bottom - kMainRect.top;

        // adjust the size to correct for a status window, if one exists
        if (m_pkAppWindow->GetStatusPanesExist())
        {
            RECT kStatusRect;
            GetWindowRect(m_pkAppWindow->GetStatusWindowReference(), 
                &kStatusRect);

            iClientHeight -= kStatusRect.bottom - kStatusRect.top;
        }

        MoveWindow(hRendWnd, 0, 0, iClientWidth, iClientHeight, FALSE);

        // redraw the status window
        if (m_pkAppWindow->GetStatusPanesExist())
        {
            SendMessage(m_pkAppWindow->GetStatusWindowReference(), WM_PAINT, 
                0, 0);
        }

        // get the new size of the rendering window
        GetWindowRect(hRendWnd, &kRect);
        m_pkAppWindow->SetWidth(kRect.right - kRect.left);
        m_pkAppWindow->SetHeight(kRect.bottom - kRect.top);
    }

    return true;
}
//---------------------------------------------------------------------------
// OnWindowDestroy() handles WM_DESTROY
//   pWnd = unused
//   bOption = unused
bool NiApplication::OnWindowDestroy(NiWindowRef, bool)
{
    return false;
}
//---------------------------------------------------------------------------
// Check message loop, which in turn processes input and system calls.
// returns true to continue, false to exit
bool NiApplication::Process()
{
    MSG kMsg;
    if (PeekMessage(&kMsg, NULL, 0, 0, PM_REMOVE))
    {
        if (kMsg.message == WM_QUIT)
            return false;
        
        if (!TranslateAccelerator(NiApplication::ms_pkApplication->
            GetAppWindow()->GetWindowReference(), 
            NiApplication::ms_pkApplication->GetAcceleratorReference(), 
            &kMsg))
        {
            TranslateMessage(&kMsg);
            DispatchMessage(&kMsg);
        }
    }
    else
    {
        NiApplication::ms_pkApplication->OnIdle();
    }

    return true;
}

//---------------------------------------------------------------------------
// default message processing
bool NiApplication::OnDefault(NiEventRef pkEventRecord)
{
    // Trap the ALT key, allowing the application to continue operation, 
    // without bringing up the main menu
    if (m_uiMenuID == 0 && pkEventRecord->uiMsg == WM_SYSKEYUP && 
        (pkEventRecord->wParam == VK_MENU || pkEventRecord->wParam == VK_F10))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiApplication::UpdateMetrics()
{
    NIMETRICS_MAIN_RECORDMETRICS();

    NIMETRICS_APPLICATION_ADDVALUE(FRAMERATE, m_fFrameRate);
    NIMETRICS_APPLICATION_ADDVALUE(CULL_TIME, m_fCullTime);
    NIMETRICS_APPLICATION_ADDVALUE(UPDATE_TIME, m_fUpdateTime);
    NIMETRICS_APPLICATION_ADDVALUE(RENDER_TIME, m_fRenderTime);

#ifdef NI_MEMORY_DEBUGGER
    NiMemTracker* pkT = NiMemTracker::Get();
    NIMETRICS_APPLICATION_ADDVALUE(ACTIVE_MEMORY, pkT->m_stActiveMemory);
    NIMETRICS_APPLICATION_ADDVALUE(ACCUMULATED_MEMORY,
        pkT->m_stAccumulatedMemory);
    NIMETRICS_APPLICATION_ADDVALUE(ACTIVE_ALLOCATIONS, 
        pkT->m_stActiveAllocationCount);
    NIMETRICS_APPLICATION_ADDVALUE(ACCUMULATED_ALLOCATIONS,
        pkT->m_stAccumulatedAllocationCount);
    NIMETRICS_APPLICATION_ADDVALUE(ACTIVE_TRACKER_OVERHEAD, 
        pkT->m_stActiveTrackerOverhead);
    NIMETRICS_APPLICATION_ADDVALUE(ACCUMULATED_TRACKER_OVERHEAD,
        pkT->m_stAccumulatedTrackerOverhead);
    NIMETRICS_APPLICATION_ADDVALUE(UNUSED_BUT_ALLOCATED_MEMORY, 
        pkT->m_stUnusedButAllocatedMemory);
#endif // #ifdef NI_MEMORY_DEBUGGER

    NIMETRICS_UPDATE();
}
//---------------------------------------------------------------------------
void NiApplication::TrackedEvent(ETrackerEvent)
{
}
//--------------------------------------------------------------------------
unsigned int NiApplication::DebugOutMessageBoxFunc(const char* pcText,
    const char*, void*)
{
    char acBuffer[1024];
    NiSnprintf(acBuffer, sizeof(acBuffer), NI_TRUNCATE, 
        "MessageBox: %s\n", pcText);
    NiOutputDebugString(acBuffer);

    return 0;
}
//--------------------------------------------------------------------------

