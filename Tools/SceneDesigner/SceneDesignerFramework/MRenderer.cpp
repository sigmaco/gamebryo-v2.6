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
#include "SceneDesignerFrameworkPCH.h"

#include "MRenderer.h"
#include <NiMaterialToolkit.h>
#include <NiMeshCullingProcess.h>
#include <NiD3DUtility.h>
#include "MFramework.h"
#include "MUtility.h"
#include "MViewportManager.h"
#include "ServiceProvider.h"
#include "MLayer.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

//---------------------------------------------------------------------------
void MRenderer::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MRenderer();
    }
}
//---------------------------------------------------------------------------
void MRenderer::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MRenderer::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MRenderer* MRenderer::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MRenderer::MRenderer() : m_pkRenderer(NULL), m_pkMainRenderTarget(NULL),
    m_hRendererWnd(0), m_pkVisibleArray(NULL), m_pkCullingProcess(NULL),
    m_hNiCgShaderLib(0), m_pkTexture1(NULL), m_pkTexture2(NULL),
    m_pkTarget1(NULL), m_pkTarget2(NULL), m_pkScreenQuad(NULL),
    m_pkScreenQuadTexProp(NULL), m_pkScreenQuadAlphaProp(NULL),
    m_pkScreenQuadVertexColorProp(NULL), m_pkScreenQuadStencilProp(NULL),
    m_pkScreenQuadShaderMaterial(NULL), m_pkHighlightColor(NULL),
    m_bD3D10(false), m_bNeedsRecreate(false)
{
    m_mBackgroundColor = Color::FromArgb(255, 128, 128, 128);

    m_pmRenderingContext = new MRenderingContext(
        NiNew NiEntityRenderingContext());

    m_pkVisibleArray = NiNew NiVisibleArray(1024, 1024);
    m_pkCullingProcess = NiNew NiMeshCullingProcess(m_pkVisibleArray, NULL);

    m_pmRenderingContext->GetRenderingContext()->m_pkCullingProcess =
        m_pkCullingProcess;

    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter());

    // Turn off debug output messages from shader system for speed of
    // debugging purposes.
    //NiD3DUtility::SetLogEnabled(false);
    //NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_0, false);
    //NiLogger::SetOutputToDebugWindow(NIMESSAGE_GENERAL_1, false);

}
//---------------------------------------------------------------------------
void MRenderer::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        m_pmRenderingContext->Dispose();
    }

    if (m_hNiCgShaderLib)
    {
        FreeLibrary(m_hNiCgShaderLib);
    }

    NiDelete m_pkHighlightColor;
    NiDelete m_pkCullingProcess;
    NiDelete m_pkVisibleArray;

    MDisposeRefObject(m_pkTexture1);
    MDisposeRefObject(m_pkTexture2);
    MDisposeRefObject(m_pkTarget1);
    MDisposeRefObject(m_pkTarget2);
    MDisposeRefObject(m_pkScreenQuad);
    MDisposeRefObject(m_pkScreenQuadTexProp);
    MDisposeRefObject(m_pkScreenQuadAlphaProp);
    MDisposeRefObject(m_pkScreenQuadVertexColorProp);
    MDisposeRefObject(m_pkScreenQuadStencilProp);
    MDisposeRefObject(m_pkScreenQuadShaderMaterial);

    NiMaterialToolkit::DestroyToolkit();

    MDisposeRefObject(m_pkRenderer);
}
//---------------------------------------------------------------------------
Color MRenderer::get_BackgroundColor()
{
    MVerifyValidInstance;

    return m_mBackgroundColor;
}
//---------------------------------------------------------------------------
void MRenderer::set_BackgroundColor(Color mBackgroundColor)
{
    MVerifyValidInstance;

    m_mBackgroundColor = mBackgroundColor;
    if (m_pkRenderer != NULL)
    {
        NiColor kBackgroundColor(MUtility::RGBToFloat(mBackgroundColor.R),
            MUtility::RGBToFloat(mBackgroundColor.G),
            MUtility::RGBToFloat(mBackgroundColor.B));
        m_pkRenderer->SetBackgroundColor(kBackgroundColor);
    }
}
//---------------------------------------------------------------------------
int MRenderer::get_Width()
{
    MVerifyValidInstance;

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
    MVerifyValidInstance;

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
MRenderingContext* MRenderer::get_RenderingContext()
{
    MVerifyValidInstance;

    return m_pmRenderingContext;
}
//---------------------------------------------------------------------------
bool MRenderer::get_D3D10()
{
    MVerifyValidInstance;

    return m_bD3D10;
}
//---------------------------------------------------------------------------
void MRenderer::UseD3D10()
{
    MVerifyValidInstance;

    if (!m_pkRenderer)
    {
        m_bD3D10 = true;
    }
}
//---------------------------------------------------------------------------
bool MRenderer::Create(IntPtr hTopLevelWndPtr, IntPtr hRendererWndPtr)
{
    MVerifyValidInstance;

    if (m_pkRenderer != NULL)
    {
        return false;
    }

    HWND hTopLevelWnd = (HWND) IntToPtr(hTopLevelWndPtr.ToInt32());
    m_hRendererWnd = (HWND) IntToPtr(hRendererWndPtr.ToInt32());

    RECT rect;
    NIVERIFY(::GetClientRect(m_hRendererWnd, &rect));
    if ((rect.right - rect.left) <= 0 || (rect.bottom - rect.top) <= 0)
    {
        return false;
    }

    try 
    {
        // Try catch is just for extra precaution in case something
        // goes wrong in the creation of the Direct3D renderer.

        if (m_bD3D10)
        {
            // Create D3D10 renderer.
            NiD3D10Renderer::CreationParameters kParams(m_hRendererWnd);
            kParams.m_bAssociateWithWindow = false;
            NiD3D10RendererPtr spD3D10Renderer;
            bool bSuccess = NiD3D10Renderer::Create(kParams, spD3D10Renderer);
            if (bSuccess)
            {
                m_pkRenderer = spD3D10Renderer;
                MInitRefObject(m_pkRenderer);

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
                NiDX9Renderer::USE_STENCIL | 
                NiDX9Renderer::USE_MANUALDEPTHSTENCIL, 
                hTopLevelWnd, hTopLevelWnd);
            if (m_pkRenderer)
            {
                MInitRefObject(m_pkRenderer);

                // Create swap chain render target group for render window.
                NiDX9Renderer* pkDX9Renderer = (NiDX9Renderer*) m_pkRenderer;
                if (pkDX9Renderer->CreateSwapChainRenderTargetGroup(
                    NiDX9Renderer::USE_STENCIL, m_hRendererWnd))
                {
                    m_pkMainRenderTarget =
                        pkDX9Renderer->GetSwapChainRenderTargetGroup(
                        m_hRendererWnd);
                    MAssert(m_pkMainRenderTarget != NULL, "Swap chain render "
                        "target group not found!");
                }
                else
                {
                    MDisposeRefObject(m_pkRenderer);
                    m_pkRenderer = NULL;
                }

                pkDX9Renderer->AddLostDeviceNotificationFunc(
                    RendererInfo::OnDeviceLost, NULL);
                pkDX9Renderer->AddResetNotificationFunc(
                    RendererInfo::OnDeviceReset, NULL);
            }
        }

        if (m_pkRenderer == NULL)
        {
            return false;
        }


        // NiPick requires CPU read access to NiDataStreams.
        // The following code enables CPU read access for any 
        // NiDataStream that are created in the process of loading.
        NIASSERT(NiDataStream::GetFactory());
        NiDataStream::GetFactory()->SetCallback(
            NiDataStreamFactory::ForceCPUReadAccessCallback);

        m_pmRenderingContext->GetRenderingContext()->m_pkRenderer =
            m_pkRenderer;
    }
    catch (Exception*)
    {
        return false;
    }

    // Background color for renderer
    RegisterForBackgroundColorSetting();

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

    float fAspectRatio = (float) Width / Height;
    for (unsigned int ui = 0; ui < MViewportManager::Instance->ViewportCount;
        ui++)
    {
        MViewport* pmViewport = MViewportManager::Instance->GetViewport(ui);
        MCameraManager::Instance->SetAspectRatioOnStandardCameras(pmViewport,
            fAspectRatio);
        pmViewport->CreateScreenConsole();
    }

    CreateRenderedTextures();
    CreateScreenQuad();

    return true;
}
//---------------------------------------------------------------------------
bool MRenderer::Recreate(IntPtr hRendererWndPtr)
{
    MVerifyValidInstance;

    // We must have a renderer first.
    if (m_pkRenderer == NULL)
    {
        return false;
    }

    m_bNeedsRecreate = true;

    HWND hNewRendererWnd = (HWND) IntToPtr(hRendererWndPtr.ToInt32());

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
            NiDX9Renderer::USE_STENCIL, hNewRendererWnd))
        {
            return false;
        }
        m_pkMainRenderTarget = pkDX9Renderer->GetSwapChainRenderTargetGroup(
            hNewRendererWnd);
        MAssert(m_pkMainRenderTarget != NULL, "Swap chain render target group "
            "not found!");

        RendererInfo::m_bDeviceReset = false;
    }

    m_hRendererWnd = hNewRendererWnd;

    float fAspectRatio = (float) Width / Height;
    for (unsigned int ui = 0; ui < MViewportManager::Instance->ViewportCount;
        ui++)
    {
        MViewport* pmViewport = MViewportManager::Instance->GetViewport(ui);
        MCameraManager::Instance->SetAspectRatioOnStandardCameras(pmViewport,
            fAspectRatio);
        pmViewport->UpdateScreenConsole();
    }

    MViewportManager::Instance->OnResize();

    CreateRenderedTextures();

    m_bNeedsRecreate = false;

    return true;
}
//---------------------------------------------------------------------------
void MRenderer::Render()
{
    MVerifyValidInstance;

    if (m_pkRenderer == NULL || m_bNeedsRecreate)
    {
        return;
    }

    // Allow the viewports to do any pre-processing
    MViewportManager::Instance->PreRender();

    // Begin the rendering frame.
    m_pkRenderer->BeginFrame();

    // Handle a lost DX9 device.
    if (RendererInfo::m_bDeviceLost)
    {
        m_pkRenderer->EndFrame();
        m_pkRenderer->DisplayFrame();
        return;
    }
    else if (RendererInfo::m_bDeviceReset)
    {
        Recreate(m_hRendererWnd);
    }

    // Render unselected entities to the first rendered texture.
    m_pkRenderer->BeginUsingRenderTargetGroup(m_pkTarget1,
        NiRenderer::CLEAR_ALL);    
    MViewportManager::Instance->RenderUnselectedEntities();
    m_pkRenderer->EndUsingRenderTargetGroup();

    // Render selected entities to the second rendered texture.
    if (SelectionService->NumSelectedEntities > 0)
    {
        // Clear second rendered texture using a shader that uses the colors
        // from the unselected entities texture but with an alpha value of
        // 0.0 for each pixel.
        NiColorA kOldClearColor;
        m_pkRenderer->GetBackgroundColor(kOldClearColor);
        m_pkRenderer->SetBackgroundColor(NiColorA(0.0f, 0.0f, 0.0f, 0.0f));
        m_pkRenderer->BeginUsingRenderTargetGroup(m_pkTarget2,
            NiRenderer::CLEAR_BACKBUFFER);
        m_pkRenderer->SetBackgroundColor(kOldClearColor);
        m_pkRenderer->SetScreenSpaceCameraData();
        m_pkScreenQuadTexProp->SetBaseTexture(m_pkTexture1);
        const NiMaterial* pkOldMaterial = m_pkScreenQuad->GetActiveMaterial();
        m_pkScreenQuadAlphaProp->SetAlphaBlending(false);
        m_pkScreenQuad->SetActiveMaterial(m_pkScreenQuadShaderMaterial);
        m_pkScreenQuad->SetMaterialNeedsUpdate(true);
        m_pkScreenQuad->RenderImmediate(m_pkRenderer);
        m_pkScreenQuad->SetActiveMaterial(pkOldMaterial);

        // Render selected entities to cleared second texture.
        MViewportManager::Instance->RenderSelectedEntities();
        m_pkRenderer->EndUsingRenderTargetGroup();
    }

    // Switch to main render target group.
    m_pkRenderer->BeginUsingRenderTargetGroup(m_pkMainRenderTarget,
        NiRenderer::CLEAR_ALL);
    m_pkRenderer->SetScreenSpaceCameraData();

    // Render screen quad with first texture.
    m_pkScreenQuadTexProp->SetBaseTexture(m_pkTexture1);
    m_pkScreenQuadAlphaProp->SetAlphaBlending(false);
    m_pkScreenQuad->SetMaterialNeedsUpdate(true);
    m_pkScreenQuad->RenderImmediate(m_pkRenderer);

    // Render screen quad with second texture.
    if (SelectionService->NumSelectedEntities > 0)
    {
        // Render quad with standard alpha blending and alpha testing.
        m_pkScreenQuadTexProp->SetBaseTexture(m_pkTexture2);
        m_pkScreenQuadStencilProp->SetStencilOn(true);
        m_pkScreenQuadStencilProp->SetStencilFunction(
            NiStencilProperty::TEST_ALWAYS);
        m_pkScreenQuadStencilProp->SetStencilReference(1);
        m_pkScreenQuadStencilProp->SetStencilPassAction(
            NiStencilProperty::ACTION_REPLACE);
        m_pkScreenQuadAlphaProp->SetAlphaBlending(true);
        m_pkScreenQuadAlphaProp->SetSrcBlendMode(
            NiAlphaProperty::ALPHA_SRCALPHA);
        m_pkScreenQuadAlphaProp->SetDestBlendMode(
            NiAlphaProperty::ALPHA_INVSRCALPHA);
        m_pkScreenQuadAlphaProp->SetAlphaTesting(true);
        m_pkScreenQuad->SetMaterialNeedsUpdate(true);
        m_pkScreenQuad->RenderImmediate(m_pkRenderer);
        m_pkScreenQuadAlphaProp->SetAlphaTesting(false);

        // Render quad with vertex colors using the stencil buffer to
        // highlight the selected entities.
        m_pkScreenQuadTexProp->SetBaseTexture(NULL);
        NiTexturingProperty::ApplyMode eOldApplyMode =
            m_pkScreenQuadTexProp->GetApplyMode();
        m_pkScreenQuadTexProp->SetApplyMode(
            NiTexturingProperty::APPLY_MODULATE);
        m_pkScreenQuadStencilProp->SetStencilFunction(
            NiStencilProperty::TEST_EQUAL);
        m_pkScreenQuadStencilProp->SetStencilReference(1);
        m_pkScreenQuadStencilProp->SetStencilPassAction(
            NiStencilProperty::ACTION_KEEP);
        NiVertexColorProperty::SourceVertexMode eOldSourceMode =
            m_pkScreenQuadVertexColorProp->GetSourceMode();
        NiVertexColorProperty::LightingMode eOldLightingMode =
            m_pkScreenQuadVertexColorProp->GetLightingMode();
        m_pkScreenQuadVertexColorProp->SetSourceMode(
            NiVertexColorProperty::SOURCE_EMISSIVE);
        m_pkScreenQuadVertexColorProp->SetLightingMode(
            NiVertexColorProperty::LIGHTING_E);
        m_pkScreenQuad->SetMaterialNeedsUpdate(true);
        m_pkScreenQuad->RenderImmediate(m_pkRenderer);
        m_pkScreenQuadVertexColorProp->SetLightingMode(eOldLightingMode);
        m_pkScreenQuadVertexColorProp->SetSourceMode(eOldSourceMode);
        m_pkScreenQuadTexProp->SetApplyMode(eOldApplyMode);
        m_pkScreenQuadStencilProp->SetStencilOn(false);
    }

    // Render gizmo and screen elements for each viewport.
    MViewportManager::Instance->RenderGizmo();
    MViewportManager::Instance->RenderScreenElements();

    // End the rendering frame.
    m_pkRenderer->EndUsingRenderTargetGroup();
    m_pkRenderer->EndFrame();

    // Display the frame.
    m_pkRenderer->DisplayFrame();
}
//---------------------------------------------------------------------------
void MRenderer::CreateShaderSystem()
{
    NiMaterialToolkit* pkShaderKit = NiMaterialToolkit::CreateToolkit();

#if _MSC_VER >= 1400
    size_t iSize = 0;
    getenv_s(&iSize, NULL, 0, "EGB_SHADER_LIBRARY_PATH");
    NIASSERT(iSize > 0);

    char* pcShaderPath = NiExternalNew char[iSize];

    if (pcShaderPath)
    {
        getenv_s(&iSize, pcShaderPath, iSize, "EGB_SHADER_LIBRARY_PATH");
    }
#else
    char* pcShaderPath = getenv("EGB_SHADER_LIBRARY_PATH");
#endif

    if (pcShaderPath == NULL)
    {
        const char* pcWarning = "The environment variable "
            "EGB_SHADER_LIBRARY_PATH must\nbe defined for this application"
            " to properly execute.";
        ::MessageBox(NULL, pcWarning, "Missing Environment Variable",
            MB_OK | MB_ICONERROR);
        MessageService->AddMessage(MessageChannelType::Errors, pcWarning);
    }
    else
    {
        // Load shader libraries.
        pkShaderKit->LoadFromDLL(pcShaderPath);

        // Add Gamebryo shader path.
        char acShaderDir[2048];
        NiSprintf(acShaderDir, 2048, "%s%s", pcShaderPath, "\\Data\\");
        if (m_bD3D10)
        {
            NiStrcat(acShaderDir, 2048, "D3D10\\");
        }
        else
        {
            NiStrcat(acShaderDir, 2048, "DX9\\");
        }
        NiMaterialToolkit::GetToolkit()->SetMaterialDirectory(acShaderDir);
    }
}
//---------------------------------------------------------------------------
void MRenderer::ReloadShaders()
{
    // Unregister shaders in the main scene.
    MScene* pmScene = MFramework::Instance->Scene;
    MEntity* amEntities[] = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                NiMaterialHelpers::UnRegisterMaterials(pkSceneRoot, 
                    m_pkRenderer, true);
            }
        }
    }
    NiMaterialToolkit::UnloadShaders();

    // Purge all existing renderer data for the main scene.
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                m_pkRenderer->PurgeAllRendererData(pkSceneRoot);
            }
        }
    }

    // Reload screen texture pixel data to avoid crash during rendering.
    for (unsigned int ui = 0; ui < MViewportManager::Instance->ViewportCount;
        ui++)
    {
        MViewportManager::Instance->GetViewport(ui)
            ->ReloadScreenConsolePixelData();
    }

    NiMaterialToolkit::ReloadShaders();

    // Re-register all existing shaders for main scene.
    bool bSet = false;
    pmScene = MFramework::Instance->Scene;
    amEntities = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                NiMaterialHelpers::RegisterMaterials(pkSceneRoot,
                    m_pkRenderer);
                bSet = true;
            }
        }
    }

    // Re-register all existing shaders for the tool scene.
    pmScene = MFramework::Instance->Scene;
    amEntities = pmScene->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        unsigned int uiSceneRootCount = pmEntity->GetSceneRootPointerCount();
        for (unsigned int ui = 0; ui < uiSceneRootCount; ui++)
        {
            NiAVObject* pkSceneRoot = pmEntity->GetSceneRootPointer(ui);
            if (pkSceneRoot)
            {
                NiMaterialHelpers::RegisterMaterials(pkSceneRoot,
                    m_pkRenderer);
                bSet = true;
            }
        }
    }

    // If we don't have any entities, we still need to register the shader
    // library directories.
    if (!bSet)
    {
        NiMaterialToolkit::UpdateMaterialDirectory();
    }
}
//---------------------------------------------------------------------------
void MRenderer::CreateRenderedTextures()
{
    MVerifyValidInstance;

    MAssert(m_pkRenderer != NULL, "Renderer not created!");

    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;

    MDisposeRefObject(m_pkTexture1);
    m_pkTexture1 = NiRenderedTexture::Create(Width, Height, m_pkRenderer,
        kPrefs);
    MInitRefObject(m_pkTexture1);

    MDisposeRefObject(m_pkTexture2);
    m_pkTexture2 = NiRenderedTexture::Create(Width, Height, m_pkRenderer,
        kPrefs);
    MInitRefObject(m_pkTexture2);

    MDisposeRefObject(m_pkTarget1);
    m_pkTarget1 = NiRenderTargetGroup::Create(m_pkTexture1->GetBuffer(),
        m_pkRenderer, true, true);
    MInitRefObject(m_pkTarget1);

    NiDepthStencilBuffer* pkDSB = m_pkTarget1->GetDepthStencilBuffer();
    NIASSERT(pkDSB);

    MDisposeRefObject(m_pkTarget2);
    m_pkTarget2 = NiRenderTargetGroup::Create(m_pkTexture2->GetBuffer(),
        m_pkRenderer, pkDSB);
    MInitRefObject(m_pkTarget2);
}
//---------------------------------------------------------------------------
void MRenderer::CreateScreenQuad()
{
    MVerifyValidInstance;

    if (m_pkScreenQuad)
    {
        return;
    }

    MAssert(m_pkRenderer != NULL, "Renderer not created!");

    // Create screen quad.
    m_pkScreenQuad = NiNew NiMeshScreenElements(false, true, 1);
    MInitRefObject(m_pkScreenQuad);

    // Add single polygon.
    int iPolygon = m_pkScreenQuad->Insert(4);
    MAssert(iPolygon == 0, "Unexpected screen quad polygon index!");
    m_pkScreenQuad->SetRectangle(0, 0.0f, 0.0f, 1.0f, 1.0f);
    m_pkScreenQuad->UpdateBound();
    m_pkScreenQuad->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
    SetScreenQuadVertexColors();

    // Create and add texturing property.
    m_pkScreenQuadTexProp = NiNew NiTexturingProperty();
    MInitRefObject(m_pkScreenQuadTexProp);
    m_pkScreenQuadTexProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    m_pkScreenQuadTexProp->SetBaseMap(NiNew NiTexturingProperty::Map());
    m_pkScreenQuadTexProp->SetBaseFilterMode(
        NiTexturingProperty::FILTER_NEAREST);
    m_pkScreenQuadTexProp->SetBaseClampMode(
        NiTexturingProperty::CLAMP_S_CLAMP_T);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadTexProp);

    // Create and add alpha property.
    m_pkScreenQuadAlphaProp = NiNew NiAlphaProperty();
    MInitRefObject(m_pkScreenQuadAlphaProp);
    m_pkScreenQuadAlphaProp->SetAlphaBlending(false);
    m_pkScreenQuadAlphaProp->SetAlphaTesting(false);
    m_pkScreenQuadAlphaProp->SetTestMode(NiAlphaProperty::TEST_GREATER);
    m_pkScreenQuadAlphaProp->SetTestRef(0);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadAlphaProp);

    // Create and add vertex color property.
    m_pkScreenQuadVertexColorProp = NiNew NiVertexColorProperty();
    MInitRefObject(m_pkScreenQuadVertexColorProp);
    m_pkScreenQuadVertexColorProp->SetSourceMode(
        NiVertexColorProperty::SOURCE_IGNORE);
    m_pkScreenQuadVertexColorProp->SetLightingMode(
        NiVertexColorProperty::LIGHTING_E_A_D);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadVertexColorProp);

    // Create and add stencil property.
    m_pkScreenQuadStencilProp = NiNew NiStencilProperty();
    MInitRefObject(m_pkScreenQuadStencilProp);
    m_pkScreenQuadStencilProp->SetStencilOn(false);
    m_pkScreenQuad->AttachProperty(m_pkScreenQuadStencilProp);

    // Create and add z-buffer property.
    NiZBufferProperty* pkZBufferProp = NiNew NiZBufferProperty();
    pkZBufferProp->SetZBufferTest(false);
    pkZBufferProp->SetZBufferWrite(false);
    m_pkScreenQuad->AttachProperty(pkZBufferProp);

    // Create screen quad shader material.
    m_pkScreenQuadShaderMaterial = NiSingleShaderMaterial::Create(
        "TexColorNoAlpha");
    MInitRefObject(m_pkScreenQuadShaderMaterial);
    // Don't set it as active yet
    m_pkScreenQuad->ApplyMaterial(m_pkScreenQuadShaderMaterial);

    // Perform initial update.
    m_pkScreenQuad->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_pkScreenQuad);
    m_pkScreenQuad->UpdateProperties();
    m_pkScreenQuad->UpdateEffects();
    m_pkScreenQuad->UpdateNodeBound();
}
//---------------------------------------------------------------------------
void MRenderer::SetScreenQuadVertexColors()
{
    MVerifyValidInstance;

    if (m_pkScreenQuad)
    {
        if (!m_pkHighlightColor)
        {
            RegisterForHighlightColorSetting();
            RegisterForTransparencySetting();
        }

        m_pkScreenQuad->SetColors(0, *m_pkHighlightColor, *m_pkHighlightColor,
            *m_pkHighlightColor, *m_pkHighlightColor);
    }
}
//---------------------------------------------------------------------------
void MRenderer::RegisterForHighlightColorSetting()
{
    MVerifyValidInstance;

    if (!m_pkHighlightColor)
    {
        m_pkHighlightColor = NiNew NiColorA();
    }

    SettingsService->RegisterSettingsObject(ms_strHighlightColorSettingName,
        __box(Color::Yellow), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strHighlightColorSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MRenderer::OnHighlightColorChanged));
    OnHighlightColorChanged(NULL, NULL);

    OptionsService->AddOption(ms_strHighlightColorOptionName,
        SettingsCategory::PerUser, ms_strHighlightColorSettingName);
    OptionsService->SetHelpDescription(ms_strHighlightColorOptionName,
        "The color with which selected entities are highlighted.");
}
//---------------------------------------------------------------------------
void MRenderer::OnHighlightColorChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box Color* pmColor = dynamic_cast<__box Color*>(
        SettingsService->GetSettingsObject(ms_strHighlightColorSettingName,
        SettingsCategory::PerUser));
    if (pmColor != NULL)
    {
        m_pkHighlightColor->r = MUtility::RGBToFloat((*pmColor).R);
        m_pkHighlightColor->g = MUtility::RGBToFloat((*pmColor).G);
        m_pkHighlightColor->b = MUtility::RGBToFloat((*pmColor).B);
        SetScreenQuadVertexColors();
    }
}
//---------------------------------------------------------------------------
void MRenderer::RegisterForTransparencySetting()
{
    MVerifyValidInstance;

    if (!m_pkHighlightColor)
    {
        m_pkHighlightColor = NiNew NiColorA();
    }

    SettingsService->RegisterSettingsObject(ms_strTransparencySettingName,
        __box(0.25f), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strTransparencySettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MRenderer::OnTransparencyChanged));
    OnTransparencyChanged(NULL, NULL);

    OptionsService->AddOption(ms_strTransparencyOptionName,
        SettingsCategory::PerUser, ms_strTransparencySettingName);
    OptionsService->SetHelpDescription(ms_strTransparencyOptionName,
        "The transparency of the selection highlighting. This should be a "
        "number between 0 and 1, where 0 is transparent and 1 is opaque.");
}
//---------------------------------------------------------------------------
void MRenderer::OnTransparencyChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box float* pmTransparency = dynamic_cast<__box float*>(
        SettingsService->GetSettingsObject(ms_strTransparencySettingName,
        SettingsCategory::PerUser));
    if (pmTransparency != NULL)
    {
        m_pkHighlightColor->a = *pmTransparency;
        SetScreenQuadVertexColors();
    }
}
//---------------------------------------------------------------------------
void MRenderer::RegisterForBackgroundColorSetting()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(ms_strBackgroundColorSettingName,
        __box(this->BackgroundColor), SettingsCategory::PerUser);
    SettingsService->SetChangedSettingHandler(ms_strBackgroundColorSettingName,
        SettingsCategory::PerUser, new SettingChangedHandler(this,
        &MRenderer::OnBackgroundColorChanged));
    OnBackgroundColorChanged(NULL, NULL);

    OptionsService->AddOption(ms_strBackgroundColorOptionName,
        SettingsCategory::PerUser, ms_strBackgroundColorSettingName);
    OptionsService->SetHelpDescription(ms_strBackgroundColorOptionName,
        "The background color that is drawn in the viewports.");
}
//---------------------------------------------------------------------------
void MRenderer::OnBackgroundColorChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    __box Color* pmColor = dynamic_cast<__box Color*>(
        SettingsService->GetSettingsObject(ms_strBackgroundColorSettingName,
        SettingsCategory::PerUser));
    if (pmColor != NULL)
    {
        this->BackgroundColor = *pmColor;
    }
}
//---------------------------------------------------------------------------
IRenderingModeService* MRenderer::get_RenderingModeService()
{
    if (ms_pmRenderingModeService == NULL)
    {
        ms_pmRenderingModeService = MGetService(IRenderingModeService);
        MAssert(ms_pmRenderingModeService != NULL, "Rendering mode service "
            "not found!");
    }
    return ms_pmRenderingModeService;
}
//---------------------------------------------------------------------------
IInteractionModeService* MRenderer::get_InteractionModeService()
{
    if (ms_pmInteractionModeService == NULL)
    {
        ms_pmInteractionModeService = MGetService(IInteractionModeService);
        MAssert(ms_pmInteractionModeService != NULL, "Interaction mode "
            "service not found!");
    }
    return ms_pmInteractionModeService;
}
//---------------------------------------------------------------------------
IMessageService* MRenderer::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service "
            "not found!");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
ISelectionService* MRenderer::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service "
            "not found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
ISettingsService* MRenderer::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MRenderer::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
// Unmanaged RendererInfo class that receives DX9 callbacks.
//---------------------------------------------------------------------------
#pragma unmanaged
//---------------------------------------------------------------------------
bool RendererInfo::m_bDeviceLost = false;
bool RendererInfo::m_bDeviceReset = false;
//---------------------------------------------------------------------------
bool RendererInfo::OnDeviceLost(void*)
{
    m_bDeviceLost = true;
    return true;
}
//---------------------------------------------------------------------------
bool RendererInfo::OnDeviceReset(bool bBeforeReset, void*)
{
    if (!bBeforeReset)
    {
        m_bDeviceReset = true;
        m_bDeviceLost = false;
    }
    return true;
}
//---------------------------------------------------------------------------
#pragma managed
//---------------------------------------------------------------------------
