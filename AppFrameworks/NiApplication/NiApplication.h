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

#ifndef NIAPPLICATION_H
#define NIAPPLICATION_H

// Gamebryo main engine
#include "NiMain.h"

// Many NiApplication subclasses use NiOutputDebugString and other
// system-related calls
#include <NiSystem.h>

// Needed to render screen elements
#include <NiMeshLib.h>

// command line parsing
#include "NiCommand.h"

// application-level classes
#include "NiAppWindow.h"

// input devices
#include "NiInputSystem.h"
#include "NiTurret.h"

// frame rate display in the hardware-accelerated window
#include "NiFrameRate.h"

// graph visualization for common statistics
#include <NiVisualTrackerRenderClick.h>
#include <NiCommonGraphCallbackObjects.h>

#include <NiUniversalTypes.h>
#include <NiVersion.h>

#if defined(WIN32)
#include <NiDirectXVersion.h>
#endif //#if defined(WIN32)

#if defined(_PS3)
#include <NiPS3Renderer.h>
#endif

class NiApplication : public NiMemObject
{
//----- base application layer ----------------------------------------------
public:
    // Default width and height for the window.
    static const unsigned int DEFAULT_WIDTH;
    static const unsigned int DEFAULT_HEIGHT;

    // the unique application instance
    static NiApplication* ms_pkApplication;

    // Create must be implemented in the NiApplication-derived class.
    static NiApplication* Create();

    NiApplication
    (
        const char* pcWindowCaption,            // Caption of window.
        unsigned int uiWidth = DEFAULT_WIDTH,   // Client window width.
        unsigned int uiHeight = DEFAULT_HEIGHT, // Client window height.
        bool bUseFrameSystem = false,           // Indicates whether or not to
                                                // use an NiRenderFrame to
                                                // render scene.
        unsigned int uiMenuID = 0,              // Menu resource ID.
        unsigned int uiNumStatusPanes = 0,      // Number of panes in status
                                                // window.
        unsigned int uiBitDepth = 0             // Color depth of screen.
    );
    virtual ~NiApplication();

#ifdef _PS3
    enum DisplayMode
    {
#define NIENUMMAP(enumerant, displaymode, verticalresolution) \
    DisplayMode_##enumerant
#include "NiPSGLDisplayModes.inl"
#undef NIENUMMAP
        , NUM_DISPLAYMODES
    };

    enum MultisamplingMode
    {
#define NIENUMMAP(enumerant, psglmultisamplingmode) \
    MultisamplingMode_##enumerant
#include "NiPSGLMultisamplingModes.inl"
#undef NIENUMMAP
        NUM_MULTISAMPLINGMODES
    };
#endif

    // command line parsing
    static void SetCommandLine(char* pcCommandLine);
    static void SetCommandLine(int iArgc, char** ppcArgv);
    static NiCommand* GetCommand();
    static void CommandSelectRenderer();

    // Media Path
    static void SetMediaPath(const char* pcPath);
    static const char* GetMediaPath();  

    // Each call to this function will return the _same_ storage -
    // each call overwrites the previously returned string, so applications
    // must take care to copy the return immediately
    static const char* ConvertMediaFilename(const char* pcFilename);

    // Window methods
    inline NiWindowRef CreateMainWindow(int iWinMode, 
        NiWindowRef hWnd = NULL) const;

    // selection of renderer and attributes
    inline void SetFullscreen(bool bOn);
    inline bool GetFullscreen() const;
    inline void SetStencil(bool bOn);
    inline bool GetStencil() const;
    inline void SetRendererDialog(bool bOn);
    inline bool GetRendererDialog() const;
    inline void SetMultiThread(bool bMultithread);
    inline bool GetMultiThread() const;
#if defined(WIN32)
    inline void SetD3D10Renderer(bool bD3D10Renderer);
    inline bool GetD3D10Renderer() const;
    inline void SetRefRast(bool bRefRast);
    inline bool GetRefRast() const;
    inline void SetPerfHUD(bool bPerfHUD);
    inline bool GetPerfHUD() const;
    inline void SetSWVertex(bool bSWVertex);
    inline bool GetSWVertex() const;
#endif //#if defined(WIN32)

    // Frame rate
    virtual void EnableFrameRate(bool bEnable);
    inline float GetMaxFrameRate();
    void SetMaxFrameRate(float fMax);

    // VisualTrackers
    void SetShowAllTrackers(bool bShow);

    // This is called by WinMain after ShowWindow and UpdateWindow, but before
    // the idle loop.  The window handle is valid at this point, so any
    // NiApplication-derived class may safely use ms_hWnd in this routine.
    // The routine should be used for allocating resources and initializing
    // any data values.
    virtual bool Initialize();

    // Terminate is called after the idle loop exits. The application
    // should free up its resources in this routine.
    virtual void Terminate();

    virtual void CreateMenuBar();

    inline NiWindowRef GetWindowReference() const;
    inline NiWindowRef GetRenderWindowReference() const;
    static void SetInstanceReference(NiInstanceRef pInstance);
    static NiInstanceRef GetInstanceReference();
    static void SetAcceleratorReference(NiAcceleratorRef pAccel);
    static NiAcceleratorRef GetAcceleratorReference();

    inline NiAppWindow* GetAppWindow() const;
    inline unsigned int GetMenuID() const;    

    // Control cursor visibility
    void HidePointer();
    void ShowPointer();

    void MainLoop();
    void QuitApplication(void);

    // Events
    // Not all events apply to all platforms; the corresponding event handlers
    // will be simply not be called on those platforms.  Look in the event 
    // handler appropriate to your system to see how the respective events are 
    // processed before the virtual handlers are called.
    
    inline NiInputSystem* GetInputSystem();

    // timing
    inline float GetCurrentTime();
    inline float GetLastTime();
    inline float GetAccumTime();
    inline float GetFrameTime();

    // updating the input system
    virtual void UpdateInput();

    // process the collected input. You would override this function to 
    // implement poll the devices states or check for action maps.
    virtual void ProcessInput();

    // check default keyboard/gamepad to show or hide the visual trackers.
    // override this to use your own keystroke or button settings.
    virtual void ProcessVisualTrackerInput();
    
    // idle processing
    virtual void OnIdle(void); // glut requires its display function to have
                               // an explicit "void"; parameter must match
    virtual void ResetFrameTimings();
    virtual void BeginFrame();

    // the begin/end wrappers are used to calculate the timings
    // used by the visual tracker.  If you do your own processing,
    // you should call the appropriate begin/end pair to add your timings
    // to those for the default update/cull/render process.  
    // Do not nest begin/end pairs, though, as that will assert.
    virtual void BeginUpdate();
    virtual void UpdateFrame();
    virtual void EndUpdate();

    virtual void BeginCull();
    virtual void CullFrame();
    virtual void EndCull();

    virtual void BeginRender();
    virtual void RenderFrame();
    virtual void EndRender();

    virtual void RenderScreenItems();
    virtual void RenderVisualTrackers();
    virtual void EndFrame();

    virtual void OnDisplayFrame();
    virtual void DisplayFrame();

    virtual void UpdateVisualTrackers();
    virtual void UpdateMetrics();


#if defined(WIN32)
    static LRESULT CALLBACK WinProc(HWND hWnd, UINT uiMsg, WPARAM wParam,
        LPARAM lParam);
    
    // windows messages
    virtual bool OnWindowResize(int iWidth, int iHeight, 
        unsigned int uiSizeType, NiWindowRef pWnd);
    virtual bool OnWindowDestroy(NiWindowRef pWnd, bool bOption);
    virtual bool OnDefault(NiEventRef pEventRecord);
    virtual bool IsExclusiveMouse() const;

    static unsigned int DebugOutMessageBoxFunc(const char* pcText,
        const char* pcCaption, void* pvExtraData);
#endif //#if defined(WIN32)

protected:
    // Types of interesting per-frame events
    enum ETrackerEvent
    {
        TRACKER_BEGIN_UPDATE = 0,
        TRACKER_END_UPDATE = 1,
        TRACKER_BEGIN_CULL = 2,
        TRACKER_END_CULL = 3,
        TRACKER_BEGIN_RENDER = 4,
        TRACKER_END_RENDER = 5,
        TRACKER_DISPLAY_FRAME = 6
    };
    void TrackedEvent(ETrackerEvent eEvent);

    virtual bool CreateScene();
    virtual bool CreateCamera();
    virtual bool CreateFrame();
    virtual bool CreateRenderer();
    virtual bool CreateInputSystem();
    virtual bool CreateVisualTrackers();
    virtual NiInputSystem::CreateParams* GetInputSystemCreateParams();
    virtual void ConfigureInputDevices();
    virtual bool MeasureTime();

    virtual void CheckScreenshot();
    virtual void SaveScreenshot();
    virtual void GetScreenshotFilename(char* pcBuffer,
        unsigned int pcBufferSize, const char* pcSuffix);

    static bool ShadowRenderStepPre(NiRenderStep* pkCurrentStep,
        void* pvCallbackData);

    NiCamera* FindFirstCamera(NiNode* pkNode);
    void AdjustCameraAspectRatio(NiCamera* pkCamera);

    inline NiTPointerList<NiMeshScreenElementsPtr>& GetScreenElements();
    inline NiTPointerList<NiVisualTrackerPtr>& GetVisualTrackers();

    // basic objects
    NiNodePtr m_spScene;
    NiCameraPtr m_spCamera;
    NiRenderFramePtr m_spFrame;
    Ni3DRenderViewPtr m_spMainRenderView;
    NiMesh2DRenderViewPtr m_spMeshScreenElementsRenderView;
    NiVisualTrackerRenderClickPtr m_spVisualTrackerRenderClick;
    NiAccumulatorProcessorPtr m_spAccumulatorProcessor;
    NiRendererPtr m_spRenderer;
    NiInputSystemPtr m_spInputSystem;

    // Application-related classes
    NiAppWindow* m_pkAppWindow;
    static NiInstanceRef ms_pInstance;
    static NiAcceleratorRef ms_pAccel;
    unsigned int m_uiMenuID;

    // renderer creation and properties
    bool m_bFullscreen;
    bool m_bStencil;
    bool m_bRendererDialog;
    bool m_bMultiThread;
    unsigned int m_uiBitDepth;
    NiFrameRate* m_pkFrameRate;
    bool m_bFrameRateEnabled;

    // frame rate counting and display
    float m_fCurrentTime; // Time reported by system
    float m_fLastTime; // Last time reported by system
    float m_fAccumTime; // Time elapsed since application start
    float m_fFrameTime; // Time elapsed since previous frame
    int m_iClicks;
    float m_fFrameRate;

    unsigned int m_uiNumObjectsDrawn;
    float m_fCullTime;
    float m_fRenderTime;
    float m_fUpdateTime;
    float m_fBeginUpdate;
    float m_fBeginCull;
    float m_fBeginRender;

    // Frame rate limitation
    float m_fMinFramePeriod;
    float m_fLastFrame;

    // Application control over visual tracker graph maximums
    float m_fVTPerformanceMax;
    float m_fVTMemoryMax;
    float m_fVTTimeMax;

    // command line
    static NiCommand* ms_pkCommand;

    // media path
    static char ms_acMediaPath[NI_MAX_PATH];
    static char ms_acTempMediaFilename[NI_MAX_PATH];

#if defined(_XENON)
    //  App-specific setup flags
    unsigned int m_uiSetupFlags;
#endif //#if defined(_XENON)

#if defined(WIN32) && !defined(_XENON)
    virtual bool Process();

    // Runtime-setting flags
    bool m_bD3D10Renderer;
    bool m_bRefRast;
    bool m_bSWVertex;
    bool m_bNVPerfHUD;
    bool m_bExclusiveMouse;
#else   //#if defined(WIN32) && !defined(_XENON)
    bool m_bQuitApp;
#endif  //#if defined(WIN32) && !defined(_XENON)

#if defined(_PS3)
    // Enter a render-loop that displays a message, flips the display, and
    // waits for the user to quit.  This should be used only in exceptional
    // circumstances.
    void WaitForUserToExit(const char* pcMessage);

    // Will be called when the footswitch is pressed and GAMEBRYO_PROFILE
    // is defined.  By default, it takes a screenshot
    virtual void OnFootSwitchPressed();

    // Renderer init options
    NiPS3Renderer::InitOptions m_kPS3GLInitParameters;
#endif

    // culling (separate from display)
    NiVisibleArray m_kVisible;
    NiCullingProcessPtr m_spCuller;

    // Frame rendering system object names.
    NiFixedString m_kRenderFrameName;
    NiFixedString m_kShadowRenderStepName;
    NiFixedString m_kMainRenderStepName;
    NiFixedString m_kMainRenderClickName;
    NiFixedString m_kMainRenderViewName;
    NiFixedString m_kScreenSpaceRenderStepName;
    NiFixedString m_kMeshScreenElementsRenderClickName;
    NiFixedString m_kMeshScreenElementsRenderViewName;
    NiFixedString m_kVisualTrackerRenderClickName;

    // visual trackers
    bool m_bShowAllTrackers;

    // Frame renderering system
    const bool m_bUseFrameSystem;

    // Fixed time system
    bool m_bUseFixedTime;
    float m_fFixedTimeIncrement;
    float m_fAppEndTime;

    // Screenshot saving system
    bool m_bDumpShotAtFixedInterval;
    float m_fDumpInterval;
    float m_fLastDumpTime;
    unsigned int m_uiScreenShotId;

    static NiRenderer::EScreenshotFormat ms_eScreenshotFormat;
private:
    NiApplication & operator=( const NiApplication & );
};

#include "NiApplication.inl"

#endif // NIAPPLICATION_H
