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
#include "MRenderer.h"
#include "MSharedData.h"

using namespace NiManagedToolInterface;
using namespace System::Threading;

#define USE_SHADER_SYSTEM

MColor::MColor(float fr, float fg, float fb)
{
    r = fr;
    g = fg;
    b = fb;
}
//---------------------------------------------------------------------------
MRenderer::MRenderer() : m_bD3D10(false), m_bRendererCreated(false),
    m_bNeedsRecreate(false)
{
    ActiveCamera = NiExternalNew MCamera();
    m_pkRenderer = NULL;
    m_pkMainRenderTarget = NULL;
    m_pkStatManager = NULL;
    m_strAppStartupPath = NULL;
    m_pkConsoleLabel = NULL;
    m_pkColor = NiExternalNew MColor(1.0f, 0.0f, 0.0f);
    m_pcActiveMode = NiExternalNew char[512];
    NiSprintf(m_pcActiveMode, 512, "Sequence");
    m_bFrameRateEnabled = true;
    m_bActiveModeEnabled = true;
    m_bLODStatsEnabled = true;
    NiD3DUtility::SetLogEnabled(false);

    m_pkVisible = NiNew NiVisibleArray(1024, 1024);
    m_pkCuller = NiNew NiMeshCullingProcess(m_pkVisible, NULL);

    m_pkRenderFrame = NULL;
    m_pkRenderStep = NULL;
    m_pkSceneView = NULL;
    m_pkShadowRenderStep = NULL;
}
//---------------------------------------------------------------------------
MRenderer::~MRenderer()
{
}
//---------------------------------------------------------------------------
void MRenderer::Lock()
{
    if (m_uiLockCount == 0)
    {
        Monitor::Enter(this);
    }
    m_uiLockCount++;
}
//---------------------------------------------------------------------------
void MRenderer::Unlock()
{
    m_uiLockCount--;
    if (m_uiLockCount == 0)
    {
        Monitor::Exit(this);
    }
}
//---------------------------------------------------------------------------
#pragma warning (disable:4312)
bool MRenderer::Create(IntPtr hWnd, IntPtr hRootWnd)
{
    if (m_pkRenderer != NULL)
        return false;

    m_hRendererWnd = (HWND) hWnd.ToInt32();
    m_hRootWnd =  (HWND) hRootWnd.ToInt32();

    // Do not purge source texture data when loading
    NiSourceTexture::SetDestroyAppDataFlag(false);
    
    // The ShadowManager needs to be initialized before the renderer is 
    // created 
    if (!NiShadowManager::GetShadowManager())
        NiShadowManager::Initialize();

    try 
    {
        // Try catch is just for extra precaution in case something
        // goes wrong in the creation of the Direct3D renderer.
        if (m_bD3D10)
        {
            NiD3D10Renderer::CreationParameters kParams(m_hRendererWnd);
            kParams.m_bAssociateWithWindow = false;
            NiD3D10RendererPtr spD3D10Renderer;
            bool bSuccess = NiD3D10Renderer::Create(kParams, spD3D10Renderer);
            if (bSuccess)
            {
                m_pkRenderer = spD3D10Renderer;
                m_pkRenderer->IncRefCount();

                m_pkMainRenderTarget =
                    spD3D10Renderer->GetSwapChainRenderTargetGroup(
                    m_hRendererWnd);
                MAssert(m_pkMainRenderTarget != NULL, "Swap chain render "
                    "target group not found!");
            }
            else
            {
                ::MessageBox(m_hRendererWnd, "A D3D10 renderer could not be "
                    "created. Using DX9 instead.", "D3D10 Renderer Creation "
                    "Failure", MB_OK | MB_ICONEXCLAMATION);
                m_bD3D10 = false;
            }
        }
        if (!m_bD3D10)
        {
            // Create DX9 renderer.
            m_pkRenderer = NiDX9Renderer::Create(0, 0,
                NiDX9Renderer::USE_MULTITHREADED, m_hRootWnd, m_hRootWnd);
            if (m_pkRenderer)
            {
                m_pkRenderer->IncRefCount();

                // Create swap chain render target group for render window.
                NiDX9Renderer* pkDX9Renderer = (NiDX9Renderer*) m_pkRenderer;
                if (pkDX9Renderer->CreateSwapChainRenderTargetGroup(
                    NiDX9Renderer::USE_MULTITHREADED, m_hRendererWnd))
                {
                    m_pkMainRenderTarget =
                        pkDX9Renderer->GetSwapChainRenderTargetGroup(
                        m_hRendererWnd);
                    MAssert(m_pkMainRenderTarget != NULL, "Swap chain render "
                        "target group not found!");
                }
                else
                {
                    m_pkRenderer->DecRefCount();
                    m_pkRenderer = NULL;
                }
            }
        }
        
        if (!m_pkRenderer)
        {
            return false;
        }
    }
    catch (Exception*)
    {
        return false;
    }

     // Add an alpha accumulator to the the renderer.
    NiAlphaAccumulator* pkAlpha = NiNew NiAlphaAccumulator;
    m_pkRenderer->SetSorter(pkAlpha);

    // Background color for renderer
    NiColor kColor;
    kColor.r = m_pkColor->r;
    kColor.g = m_pkColor->g;
    kColor.b = m_pkColor->b;
    m_pkRenderer->SetBackgroundColor(kColor);

    CreateShaderSystem();

    if (!m_bD3D10)
    {
        HMODULE hCgCheck = LoadLibrary("CgD3D9.dll");
        if (hCgCheck)
        {
            FreeLibrary(hCgCheck);

            // Load Cg Shader Library, if it exists
            const char* const pcCgShaderLibName = "NiCgShaderLib"
                NI_DLL_SUFFIX
                ".dll";
            
            m_hNiCgShaderLib = LoadLibrary(pcCgShaderLibName);
            if (m_hNiCgShaderLib)
            {
                unsigned int (*pfnGetCompilerVersionFunc)(void) =
                    (unsigned int (*)(void))GetProcAddress(m_hNiCgShaderLib, 
                    "GetCompilerVersion");
                if (pfnGetCompilerVersionFunc)
                {
                    unsigned int uiPluginVersion = pfnGetCompilerVersionFunc();
                    if (uiPluginVersion != (_MSC_VER))
                    {
                        FreeLibrary(m_hNiCgShaderLib);
                    }
                }
            }
        }
        else
        {
            if (GetLastError() == ERROR_PROC_NOT_FOUND)
            {
                NiMessageBox("Warning: You have an installation of Cg,"
                    " but it appears to be the wrong version.  You may be"
                    " running into this problem because of a conflict"
                    " between two different versions.  Check the section"
                    " on Installation and Setup in the documentation for more"
                    " information", "Conflicting Cg Versions");
            }
        }
    }

    ReloadShaders();
    CreateScreenConsole();
    EnableFrameRate(m_bFrameRateEnabled);
    EnableLODStats(m_bLODStatsEnabled);
    EnableActiveMode(m_bActiveModeEnabled);

    if (m_pkRendererCreatedDelegate != NULL)
    {
        m_pkRendererCreatedDelegate();
    }

    m_bRendererCreated = true;
    return true;
}
//---------------------------------------------------------------------------
bool MRenderer::ReCreate(IntPtr hRendererWndPtr)
{
    // We must have a renderer first.
    if (!m_pkRenderer)
    {
        return false;
    }

    m_bNeedsRecreate = true;

    HWND hNewRendererWnd = (HWND) hRendererWndPtr.ToInt32();

    NiRenderTargetGroupPtr spOldRenderTarget = m_pkMainRenderTarget;

    Lock();

    if (m_bD3D10)
    {
        NiD3D10Renderer* pkD3D10Renderer = (NiD3D10Renderer*) m_pkRenderer;
        NIASSERT(pkD3D10Renderer);

        NiD3D10Renderer::CreationParameters kParams;
        pkD3D10Renderer->GetCreationParameters(kParams);
        if (hNewRendererWnd != m_hRendererWnd)
        {
            NiD3D10Renderer::CreationParameters kNewParams(hNewRendererWnd);

            if (!pkD3D10Renderer->CreateSwapChainRenderTargetGroup(
                kNewParams.m_kSwapChain))
            {
                Unlock();
                return false;
            }
            m_pkMainRenderTarget = 
                pkD3D10Renderer->GetSwapChainRenderTargetGroup(
                hNewRendererWnd);
            MAssert(m_pkMainRenderTarget != NULL, "Swap chain render target "
                "group not found!");
            pkD3D10Renderer->DestroySwapChainRenderTargetGroup(m_hRendererWnd);
            pkD3D10Renderer->SetDefaultSwapChainRenderTargetGroup(
                hNewRendererWnd);
        }

        pkD3D10Renderer->ResizeBuffers(0, 0, hNewRendererWnd);
    }
    else
    {
        NIASSERT(NiIsKindOf(NiDX9Renderer, m_pkRenderer));
        NiDX9Renderer* pkDX9Renderer = (NiDX9Renderer*) m_pkRenderer;
        pkDX9Renderer->DestroySwapChainRenderTargetGroup(m_hRendererWnd);
        if (!pkDX9Renderer->CreateSwapChainRenderTargetGroup(
            NiDX9Renderer::USE_MULTITHREADED, m_hRendererWnd))
        {
            Unlock();
            return false;
        }
        m_pkMainRenderTarget = pkDX9Renderer->GetSwapChainRenderTargetGroup(
            hNewRendererWnd);
        MAssert(m_pkMainRenderTarget != NULL, "Swap chain render target group "
            "not found!");
    }

    // Update the render target for all render clicks.
    if (m_pkRenderFrame && spOldRenderTarget != m_pkMainRenderTarget)
    {
        const NiTPointerList<NiRenderStepPtr>& kSteps =
            m_pkRenderFrame->GetRenderSteps();
        NiTListIterator kStepIter = kSteps.GetHeadPos();
        while (kStepIter)
        {
            NiRenderStep* pkStep = kSteps.GetNext(kStepIter);
            NiDefaultClickRenderStep* pkClickStep = NiDynamicCast(
                NiDefaultClickRenderStep, pkStep);
            if (pkClickStep)
            {
                const NiTPointerList<NiRenderClickPtr>& kClicks =
                    pkClickStep->GetRenderClickList();
                NiTListIterator kClickIter = kClicks.GetHeadPos();
                while (kClickIter)
                {
                    NiRenderClick* pkClick = kClicks.GetNext(kClickIter);
                    pkClick->SetRenderTargetGroup(m_pkMainRenderTarget);
                }
            }
        }
    }

    if (m_pkRendererReCreatedDelegate != NULL)
    {
        m_pkRendererReCreatedDelegate();
    }
    Unlock();

    m_bNeedsRecreate = false;
    
    return true;
}
#pragma warning (default:4312)
//---------------------------------------------------------------------------
void MRenderer::Destroy()
{
    Lock();

    if (m_pkConsoleLabel)
    {
        EnableFrameRate(false);
        EnableLODStats(false);
        EnableActiveMode(false);

        m_pkConsoleLabel->DecRefCount();
        m_pkConsoleFont->DecRefCount();
    }

    if (ActiveCamera)
        ActiveCamera->DeleteContents();
    ActiveCamera = NULL;
    
    if (m_hNiCgShaderLib)
        FreeLibrary(m_hNiCgShaderLib);

    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (pkData)
    {
        for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
        {
            NiAVObject* pkObject = pkData->GetScene(ui);
            if (pkObject)
            {
                m_pkRenderer->PurgeAllRendererData(pkObject);
            }
        }
    }

    NiActorManager* pkManager = pkData->GetActorManager();
    if (pkManager && pkManager->GetNIFRoot())
    {
        m_pkRenderer->PurgeAllRendererData(pkManager->GetNIFRoot());
    }
    pkData->Unlock();

    NiDelete m_pkRenderFrame;
    NiDelete m_pkVisible;

    NiMaterialToolkit::DestroyToolkit();

    // The shadow manager needs to be shutdown after the scene has been 
    // destroyed, but before the renderer is destroyed.
    NiShadowManager::Shutdown();

    if (m_pkRenderer)
    {
        m_pkRenderer->DecRefCount();
        m_pkRenderer = NULL;
    }

    m_pkStatManager = NULL;
    m_pkRenderer = NULL;
    ActiveCamera = NULL;
    m_strAppStartupPath = NULL;

    Unlock();
}
//---------------------------------------------------------------------------
void MRenderer::DeleteContents()
{
    Destroy();
}
//---------------------------------------------------------------------------
void MRenderer::ClearClickSwap()
{
    if (!m_bRendererCreated || m_bNeedsRecreate)
    {
        return;
    }

    Lock();
    if (m_pkRenderer && ActiveCamera)
    {
        NiCamera* pkCamera = ActiveCamera->GetCamera();
        if (pkCamera)
        {
            MSharedData* pkData = MSharedData::Instance;
            pkData->Lock();

            UpdateScreenConsole();
            
            m_pkStatManager->StartClearTimer();
            Clear(pkCamera);
            m_pkStatManager->StopClearTimer();

            m_pkStatManager->StartClickTimer();
            Click(pkCamera);
            m_pkStatManager->StopClickTimer();
            
            m_pkStatManager->StartSwapTimer();
            SwapBuffers(pkCamera);
            m_pkStatManager->StopSwapTimer();

            m_pkStatManager->SampleFrameRate();
            pkData->Unlock();
        }
    }
    Unlock();
}
//---------------------------------------------------------------------------
void MRenderer::Clear(NiCamera*)
{
    NIASSERT(m_uiLockCount > 0);
}
//---------------------------------------------------------------------------
void MRenderer::Click(NiCamera* pkCamera)
{
    if (!m_pkRenderFrame)
    {
        CreateFrame();
    }

    // Store the current sorter
    NiAccumulatorPtr spOriginalSorter = m_pkRenderer->GetSorter();

    // reset the camera
    m_pkSceneView->SetCamera(pkCamera);

    // Render the frame.
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    m_pkRenderFrame->Draw();
    pkData->Unlock();

    // restore the original sorter (in case we NULLed it out)
    m_pkRenderer->SetSorter(spOriginalSorter);
}
//---------------------------------------------------------------------------
void MRenderer::SwapBuffers(NiCamera*)
{
    NIASSERT(m_uiLockCount > 0);
    if (m_pkRenderer)
        m_pkRenderer->DisplayFrame();
}
//---------------------------------------------------------------------------
void MRenderer::CreateShaderSystem()
{
    NiMaterialToolkit* pkMaterialKit = NiMaterialToolkit::CreateToolkit();

#if _MSC_VER >= 1400
    size_t iSize = 0;
    getenv_s(&iSize, NULL, 0, "EGB_SHADER_LIBRARY_PATH");
    NIASSERT(iSize > 0);

    char* pcMaterialPath = NiExternalNew char[iSize];

    if (pcMaterialPath)
    {
        getenv_s(&iSize, pcMaterialPath, iSize, "EGB_SHADER_LIBRARY_PATH");
    }
#else
    char* pcMaterialPath = getenv("EGB_SHADER_LIBRARY_PATH");
#endif

#ifdef USE_SHADER_SYSTEM

    if (pcMaterialPath == NULL)
    {
        const char* pcWarning = "The environment variable "
            "EGB_SHADER_LIBRARY_PATH must\nbe defined for this application"
            " to properly execute.";
        MessageBox(NULL, 
            pcWarning,
            "Missing Environment Variable", MB_OK | MB_ICONERROR);
        MLogger::LogWarning(pcWarning);
    }
    else
    {
        pkMaterialKit->LoadFromDLL(pcMaterialPath);
        char acMaterialDir[2048];
        NiSprintf(acMaterialDir, 2048, "%s%s", pcMaterialPath, "\\Data\\");
        if (m_bD3D10)
        {
            NiStrcat(acMaterialDir, 2048, "D3D10\\");
        }
        else
        {
            NiStrcat(acMaterialDir, 2048, "DX9\\");
        }
        NiMaterialToolkit::GetToolkit()->SetMaterialDirectory(acMaterialDir);

        MLogger::LogGeneral("Loaded Material Library");
    }
#endif
}
//---------------------------------------------------------------------------
void MRenderer::ReloadShaders()
{
#ifdef USE_SHADER_SYSTEM
    Lock();
    MLogger::LogGeneral("Reloading Materials");
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();

    // Unregister shaders in all of the scenes
    for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
    {
        NiAVObject* pkScene = pkData->GetScene(ui);
        if (pkScene)
        {
            NiMaterialHelpers::UnRegisterMaterials(pkScene, 
                m_pkRenderer, true);
        }
    }
    NiMaterialToolkit::UnloadShaders();
    // Purge all existing renderer data for the scene
    for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
    {
        NiAVObject* pkScene = pkData->GetScene(ui);
        if (pkScene)
        {
            m_pkRenderer->PurgeAllRendererData(pkScene);
        }
    }

    NiMaterialToolkit::ReloadShaders();

    // Re-register all existing shaders.
    bool bSet = false;
    for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
    {
        NiAVObject* pkScene = pkData->GetScene(ui);
        if (pkScene)
        {
            NiMaterialHelpers::RegisterMaterials(pkScene, m_pkRenderer);
            bSet = true;
        }
    }

    // If we don't have a scene graph, we still need to 
    // register the shader library directories. 
    if (!bSet)
    {
        NiMaterialToolkit::UpdateMaterialDirectory();
    }

    pkData->Unlock();
    Unlock();
#endif
}
//---------------------------------------------------------------------------
void MRenderer::Init(String* strAppStartupPath, 
    MStatisticsManager* pkStatManager)
{
    m_strAppStartupPath = strAppStartupPath;
    m_pkStatManager = pkStatManager;

    __hook(&MSharedData::OnSceneAdded, MSharedData::Instance,
        &MRenderer::OnSceneAdded);
    __hook(&MSharedData::OnSceneRemoved, MSharedData::Instance,
        &MRenderer::OnSceneRemoved);
    __hook(&MSharedData::OnSceneChanged, MSharedData::Instance,
        &MRenderer::OnSceneChanged);
}
//---------------------------------------------------------------------------
void MRenderer::Shutdown()
{
    __unhook(&MSharedData::OnSceneAdded, MSharedData::Instance,
        &MRenderer::OnSceneAdded);
    __unhook(&MSharedData::OnSceneRemoved, MSharedData::Instance,
        &MRenderer::OnSceneRemoved);
    __unhook(&MSharedData::OnSceneChanged, MSharedData::Instance,
        &MRenderer::OnSceneChanged);
}
//---------------------------------------------------------------------------
void MRenderer::EnableFrameRate(bool bEnable)
{
    NIASSERT(m_pkConsoleLabel != NULL);
    m_bFrameRateEnabled = bEnable;
}
//---------------------------------------------------------------------------
void MRenderer::EnableLODStats(bool bEnable)
{
    NIASSERT(m_pkConsoleLabel != NULL);
    m_bLODStatsEnabled = bEnable;
}
//---------------------------------------------------------------------------
void MRenderer::EnableActiveMode(bool bEnable)
{
    NIASSERT(m_pkConsoleLabel != NULL);
    m_bActiveModeEnabled = bEnable;
}
//---------------------------------------------------------------------------
void MRenderer::SetActiveModeString(String* strActiveMode)
{
    const char* pcMode = MStringToCharPointer(strActiveMode);
    NiSprintf(m_pcActiveMode, 512, "%s", pcMode);
    MFreeCharPointer(pcMode);
}
//---------------------------------------------------------------------------
void MRenderer::CreateScreenConsole()
{
    if(!m_pkConsoleLabel)
    {
        const char* pcPath = MStringToCharPointer(String::Concat(
            m_strAppStartupPath, "..\\..\\CourierNew10.NFF"));

        const char* pcEmptyString = MStringToCharPointer(String::Empty);

        m_pkConsoleFont = NiFont::Create(m_pkRenderer, pcPath);
        m_pkConsoleFont->IncRefCount();

        m_pkConsoleLabel = NiNew Ni2DString(m_pkConsoleFont, 
            NiFontString::COLORED, 64, pcEmptyString);
        m_pkConsoleLabel->IncRefCount();
        m_pkConsoleLabel->SetRenderTargetGroup(m_pkMainRenderTarget);
        m_pkConsoleLabel->SetPosition(9, 9);
    }
}
//---------------------------------------------------------------------------
void MRenderer::UpdateScreenConsole()
{
    if(m_pkConsoleLabel && m_pkConsoleRenderClick)
    {
        // Peformance Stats
        char stats0[256];
        char stats1[256];
        char stats2[256];

        if (m_bActiveModeEnabled)
        {
            NiSprintf(stats0, 256, "Display Mode: %s\r\n", m_pcActiveMode);
        }
        else
        {
            stats0[0] = '\0';
        }

        float fFrameRate = m_pkStatManager->GetFrameRate();
        if (m_bFrameRateEnabled)
        {
            NiSprintf(stats1, 256, "FPS: %.0f\r\n", fFrameRate);
        }
        else
        {
            stats1[0] = '\0';
        }

        if (m_bLODStatsEnabled)
        {
            MSharedData* pkSharedData = MSharedData::Instance;
            pkSharedData->Lock();

            NiActorManager* pkActorManager = 
                pkSharedData->GetActorManager();
            if (!pkActorManager)
            {
                stats2[0] = '\0';
            }
            else
            {
                NiSkinningLODController* pkBoneLOD = 
                    pkActorManager->GetSkinningLODController();
                if (pkBoneLOD && pkBoneLOD->GetNumberOfBoneLODs() > 0)
                {
                    NiSprintf(stats2, 256, "LOD: %d of %d\r\n", 
                        pkBoneLOD->GetBoneLOD(), 
                        pkBoneLOD->GetNumberOfBoneLODs() - 1);
                }
                else
                {
                    stats2[0] = '\0';
                }
            }
            pkSharedData->Unlock();
        }
        else
        {
            stats2[0] = '\0';
        }

        const char* pcStatsString = MStringToCharPointer(String::Concat(
            stats0, stats1, stats2));
        m_pkConsoleLabel->SetText(pcStatsString);
        if (m_pkConsoleRenderClick->GetRenderTargetGroup() !=
            m_pkMainRenderTarget ||
            m_pkConsoleLabel->GetRenderTargetGroup() !=
            m_pkMainRenderTarget)
        {
            m_pkConsoleRenderClick->SetRenderTargetGroup(m_pkMainRenderTarget);
            m_pkConsoleLabel->SetRenderTargetGroup(m_pkMainRenderTarget);
            m_pkConsoleLabel->UpdateScreenElements(true);
        }
        else
        {
            m_pkConsoleLabel->UpdateScreenElements();
        }
    }
    else
    {
        const char* pcEmptyString = MStringToCharPointer(String::Empty);
        m_pkConsoleLabel->SetText(pcEmptyString);
    }
}
//---------------------------------------------------------------------------
HWND MRenderer::GetRenderWindowHandle()
{
    return m_hRendererWnd;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
void MRenderer::add_OnRendererCreatedEvent(RendererCreatedEvent* pkEvent)
{
    m_pkRendererCreatedDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::remove_OnRendererCreatedEvent(RendererCreatedEvent* pkEvent)
{
    m_pkRendererCreatedDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::raise_OnRendererCreatedEvent()
{
    if (m_pkRendererCreatedDelegate)
    {
        m_pkRendererCreatedDelegate();
    }
}
//---------------------------------------------------------------------------
void MRenderer::add_OnRendererReCreatedEvent(RendererReCreatedEvent* pkEvent)
{
    m_pkRendererReCreatedDelegate += pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::remove_OnRendererReCreatedEvent(
    RendererReCreatedEvent* pkEvent)
{
    m_pkRendererReCreatedDelegate -= pkEvent;
}
//---------------------------------------------------------------------------
void MRenderer::raise_OnRendererReCreatedEvent()
{
    if (m_pkRendererReCreatedDelegate != NULL)
    {
        m_pkRendererReCreatedDelegate();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Properties
//---------------------------------------------------------------------------
bool MRenderer::get_Active()
{
    return m_pkRenderer != NULL;
}
//---------------------------------------------------------------------------
int MRenderer::get_Width()
{
    if (m_pkRenderer)
    {
        return m_pkMainRenderTarget->GetWidth(0);
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
int MRenderer::get_Height()
{
    if (m_pkRenderer)
    {
        return m_pkMainRenderTarget->GetHeight(0);
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
MColor* MRenderer::get_Color()
{
    return m_pkColor;
}
//---------------------------------------------------------------------------
void MRenderer::set_Color(MColor* pkColor)
{
    m_pkColor = pkColor;

    if (m_pkRenderer)
    {
        NiColor kColor;
        kColor.r = pkColor->r;
        kColor.g = pkColor->g;
        kColor.b = pkColor->b;
        Lock();
        m_pkRenderer->SetBackgroundColor(kColor);
        Unlock();
    }
}
//---------------------------------------------------------------------------
bool MRenderer::get_ShowFrameRate()
{
    return m_bFrameRateEnabled;
}
//---------------------------------------------------------------------------
void MRenderer::set_ShowFrameRate(bool bEnable)
{
    EnableFrameRate(bEnable);
}
//---------------------------------------------------------------------------
bool MRenderer::get_ShowLODStats()
{
    return m_bLODStatsEnabled;
}
//---------------------------------------------------------------------------
void MRenderer::set_ShowLODStats(bool bEnable)
{
    EnableLODStats(bEnable);
}
//---------------------------------------------------------------------------
bool MRenderer::get_ShowActiveMode()
{
    return m_bActiveModeEnabled;
}
//---------------------------------------------------------------------------
void MRenderer::set_ShowActiveMode(bool bEnable)
{
    EnableActiveMode(bEnable);
}
//---------------------------------------------------------------------------
#pragma warning (disable:4311)
Int32 MRenderer::get_RenderWindowHandle()
{
    return (Int32)(m_hRendererWnd);
}
#pragma warning (default:4311)
//---------------------------------------------------------------------------
#pragma warning (disable:4312)
void MRenderer::set_RenderWindowHandle(Int32 handle)
{
    m_hRendererWnd = (HWND)(handle);
}
#pragma warning (default:4312)
//---------------------------------------------------------------------------
bool MRenderer::get_D3D10()
{
    return m_bD3D10;
}
//---------------------------------------------------------------------------
void MRenderer::UseD3D10()
{
    if (!m_pkRenderer)
    {
        m_bD3D10 = true;
    }
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* MRenderer::GetMainRenderTarget()
{
    return m_pkMainRenderTarget;
}
//---------------------------------------------------------------------------
void MRenderer::CreateFrame()
{
    if (!m_pkRenderer)
    {
        return;
    }

    // retrieve the document camera
    NiCamera* pkCamera = ActiveCamera->GetCamera();

    // create the render frame
    bool bNewFrame = false;
    if (m_pkRenderFrame) 
    {
        bNewFrame = true;
        NiDelete m_pkRenderFrame;
        m_pkCuller = NiNew NiMeshCullingProcess(m_pkVisible, NULL);
    }
    m_pkRenderFrame = NiNew NiRenderFrame();

    // Initialize shadow click generator and active it.
    NiShadowManager::SetActiveShadowClickGenerator(
        "NiDefaultShadowClickGenerator");

    // Initialize shadow manager parameters.
    NiShadowManager::SetSceneCamera(pkCamera);
    NiShadowManager::SetCullingProcess(m_pkCuller);

    // Create shadow render step.
    if (!m_pkShadowRenderStep || bNewFrame)
    {
        m_pkShadowRenderStep = NiNew NiDefaultClickRenderStep();
        m_pkShadowRenderStep->SetPreProcessingCallbackFunc(
            MRendererCallbacks::ShadowRenderStepPre);
    }

    // create the render step
    m_pkRenderStep = NiNew NiDefaultClickRenderStep();

    // create the multiple scene graph render clicks
    MSharedData* pkData = MSharedData::Instance;
    pkData->Lock();
    if (pkData)
    {
        // create the RenderView
        m_pkSceneView = NiNew Ni3DRenderView(pkCamera, m_pkCuller);

        for (unsigned int ui = 0; ui < pkData->GetSceneArrayCount(); ui++)
        {
            NiAVObject* pkObject = pkData->GetScene(ui);
            
            if (!pkObject)
            {
                continue;
            }

            try
            {
                m_pkSceneView->AppendScene(pkObject);
            }
            catch (Exception*)
            {
                char acString[1024];
                NiSprintf(acString, 1024, 
                    "Exception in CullShow. SceneID: %d", ui);
                MLogger::LogDebug(acString);
            }
        }
        NiViewRenderClick* pkRenderClick = NiNew NiViewRenderClick();
        pkRenderClick->SetRenderTargetGroup(m_pkMainRenderTarget);
        pkRenderClick->AppendRenderView(m_pkSceneView);
        pkRenderClick->SetViewport(pkCamera->GetViewPort());
        pkRenderClick->SetClearAllBuffers(true);

        // add an alpha processor (accumulator)
        NiAccumulatorProcessor* pkSorter = NiNew 
            NiAccumulatorProcessor(m_pkRenderer->GetSorter());
        pkRenderClick->SetProcessor(pkSorter);

        m_pkRenderStep->AppendRenderClick(pkRenderClick);
    }

    // make sure there is a screen console
    if (!m_pkConsoleLabel)
    {
        CreateScreenConsole();
    }


    // create a RenderClick for the Ni2DString
    m_pkConsoleRenderClick = NiNew Ni2DStringRenderClick();
    m_pkConsoleRenderClick->SetRenderTargetGroup(m_pkMainRenderTarget);
    m_pkConsoleRenderClick->Append2DString(m_pkConsoleLabel);
    m_pkRenderStep->AppendRenderClick(m_pkConsoleRenderClick);

    // add the overall step to the frame
    m_pkRenderFrame->AppendRenderStep(m_pkShadowRenderStep);
    m_pkRenderFrame->AppendRenderStep(m_pkRenderStep);
    
    pkData->Unlock();
}
//---------------------------------------------------------------------------
bool MRendererCallbacks::ShadowRenderStepPre(NiRenderStep* pkCurrentStep,
    void*)
{
    // Get the list of render clicks from the shadow manager.
    const NiTPointerList<NiRenderClick*>& kShadowClicks =
        NiShadowManager::GenerateRenderClicks();

    // Replace the render clicks in the shadow render step with those provided
    // by the shadow manager.
    NIASSERT(NiIsKindOf(NiDefaultClickRenderStep, pkCurrentStep));
    NiDefaultClickRenderStep* pkClickRenderStep = (NiDefaultClickRenderStep*)
        pkCurrentStep;
    pkClickRenderStep->GetRenderClickList().RemoveAll();
    NiTListIterator kIter = kShadowClicks.GetHeadPos();
    while (kIter)
    {
        pkClickRenderStep->AppendRenderClick(kShadowClicks.GetNext(kIter));
    }

    return true;
}
//---------------------------------------------------------------------------
void MRenderer::OnSceneAdded(unsigned int, NiAVObject* pkScene)
{
    if (m_pkSceneView && pkScene)
    {
        m_pkSceneView->AppendScene(pkScene);
    }
}
//---------------------------------------------------------------------------
void MRenderer::OnSceneRemoved(unsigned int, NiAVObject* pkScene)
{
    if (m_pkSceneView && pkScene)
    {
        m_pkSceneView->RemoveScene(pkScene);
    }
}
//---------------------------------------------------------------------------
void MRenderer::OnSceneChanged(unsigned int, NiAVObject* pkOldScene,
    NiAVObject* pkNewScene)
{
    if (m_pkSceneView)
    {
        if (pkOldScene)
        {
            m_pkSceneView->RemoveScene(pkOldScene);
        }
        if (pkNewScene)
        {
            m_pkSceneView->AppendScene(pkNewScene);
        }
    }
}
//---------------------------------------------------------------------------
