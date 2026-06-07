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

#pragma once

#include "MCamera.h"
#include "MStatisticsManager.h"

#pragma unmanaged
#include "NiCullingProcess.h"
#include "NiRenderFrame.h"
#include "Ni2DString.h"
#include "Ni2DStringRenderClick.h"
#include "Ni3DRenderView.h"
#pragma managed

namespace NiManagedToolInterface
{
    public __delegate void RendererCreatedEvent();
    public __delegate void RendererReCreatedEvent();

    public __gc class MColor
    {
    public:
        MColor(float fr, float fg, float fb);
        float r; 
        float g; 
        float b; 
    };

    // This class exists because NiRenderClick will not accept a
    // function pointer to a managed class for a callback. While it is
    // possible to create a pointer using Delegates, this features only
    // appears to be properly supported in versions of .NET greater
    // than 7.1.
    class MRendererCallbacks
    {
    public:
        static bool ShadowRenderStepPre(NiRenderStep* pkCurrentStep, 
            void* pvCallbackData);
    };

    public __gc class MRenderer
    {
    public:
        MRenderer();
        ~MRenderer();

        // Events
        __event void add_OnRendererCreatedEvent(
            RendererCreatedEvent* pkEvent);
        __event void remove_OnRendererCreatedEvent(
            RendererCreatedEvent* pkEvent);
        __event void add_OnRendererReCreatedEvent(
            RendererReCreatedEvent* pkEvent);
        __event void remove_OnRendererReCreatedEvent(
            RendererReCreatedEvent* pkEvent);
        
        // Properties
        __property bool get_Active();
        __property int get_Width();
        __property int get_Height();
        __property MColor* get_Color();
        __property void set_Color(MColor* pkColor);
        __property bool get_ShowFrameRate();
        __property void set_ShowFrameRate(bool bShow);
        __property bool get_ShowLODStats();
        __property void set_ShowLODStats(bool bShow);
        __property bool get_ShowActiveMode();
        __property void set_ShowActiveMode(bool bShow);
        __property Int32 get_RenderWindowHandle();
        __property void set_RenderWindowHandle(Int32 handle);
        __property bool get_D3D10();

        bool Create(IntPtr hWnd, IntPtr hRootWnd);
        bool ReCreate(IntPtr hRendererWndPtr);
        void Destroy();
        void DeleteContents();

        void ClearClickSwap();
        void ReloadShaders();

        void Init(String* strAppStartupPath, 
            MStatisticsManager* pkStatManager);
        void Shutdown();
        MCamera* ActiveCamera;

        void Lock();
        void Unlock();

        HWND GetRenderWindowHandle();
        void SetActiveModeString(String* strActiveMode);

        void UseD3D10();
        
        NiRenderTargetGroup* GetMainRenderTarget(); 

        // frame rendering components
        void CreateFrame();

    protected:
        // Events.
        __event void raise_OnRendererCreatedEvent();
        __event void raise_OnRendererReCreatedEvent();

        // Delegates.
        RendererCreatedEvent* m_pkRendererCreatedDelegate;
        RendererReCreatedEvent* m_pkRendererReCreatedDelegate;

        void OnSceneAdded(unsigned int uiIndex, NiAVObject* pkScene);
        void OnSceneRemoved(unsigned int uiIndex, NiAVObject* pkScene);
        void OnSceneChanged(unsigned int uiIndex, NiAVObject* pkOldScene,
            NiAVObject* pkNewScene);

        HWND m_hRendererWnd;
        HWND m_hRootWnd;
        HMODULE m_hNiCgShaderLib;
        
        MStatisticsManager* m_pkStatManager;
        NiRenderer* m_pkRenderer;
        NiRenderTargetGroup* m_pkMainRenderTarget;
        Ni2DString* m_pkConsoleLabel;
        Ni2DStringRenderClick* m_pkConsoleRenderClick;
        NiFont* m_pkConsoleFont;
        bool m_bFrameRateEnabled;
        bool m_bLODStatsEnabled;
        bool m_bActiveModeEnabled;
        String* m_strAppStartupPath;
        char* m_pcActiveMode;

        MColor* m_pkColor;
        
        unsigned int m_uiLockCount;

        NiVisibleArray* m_pkVisible;
        NiCullingProcess* m_pkCuller;

        bool m_bD3D10;
        bool m_bRendererCreated;
        bool m_bNeedsRecreate;

        void Clear(NiCamera* pkCamera);
        void Click(NiCamera* pkCamera);
        void SwapBuffers(NiCamera* pkCamera);

        void EnableFrameRate(bool bEnable);
        void EnableActiveMode(bool bEnable);
        void EnableLODStats(bool bEnable);

        void CreateScreenConsole();
        void UpdateScreenConsole();
        void CreateShaderSystem();

        // Frame rendering components
        NiRenderFrame* m_pkRenderFrame;
        NiDefaultClickRenderStep* m_pkRenderStep;
        Ni3DRenderView* m_pkSceneView;
        NiDefaultClickRenderStep* m_pkShadowRenderStep;
    };
}
