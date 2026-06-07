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

// NifRenderView.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiTDataStreamFactoryProxy.h"
#include "NiDX9DataStreamFactory.h"
#include "NiD3D10DataStreamFactory.h"
#include "NifRenderView.h"
#include "NifDoc.h"
#include "NifRenderViewUICommand.h"
#include "NifRenderViewCommands.h"
#include "NifUserPreferences.h"
#include "NifRenderViewCameraTranslationState.h"
#include "NifRenderViewCameraTumbleState.h"
#include "NifRenderViewCameraRotateState.h"
#include "NifRenderViewCameraZoomState.h"
#include "NifRenderViewCamera3dsmaxState.h"
#include "NifRenderViewCameraMayaState.h"
#include "NifRenderViewCameraLockedState.h"
#include "NifCameraList.h"
#include "NifRenderViewUIManager.h"
#include "NiMaterialHelpers.h"
#include "NifRenderViewCollisionLabTranslationState.h"
#include <NiMaterialToolkit.h>
#include <NiMemoryDefines.h>
#include <NiVersion.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifRenderView
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifRenderView, CView)
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifRenderView, CView)
    //{{AFX_MSG_MAP(CNifRenderView)
    ON_WM_RBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MBUTTONUP()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONDBLCLK()
    ON_WM_KEYUP()
    ON_WM_KEYDOWN()
    ON_WM_SIZE()
    ON_WM_SYSKEYUP()
    ON_WM_SYSKEYDOWN()
    ON_WM_DESTROY()
    ON_COMMAND_RANGE(ID_CAMERA_BEGIN, ID_CAMERA_END, OnCameraMenuSelection)
    ON_COMMAND(ID_CAMERA_FOCUS, OnCameraFocus)
    ON_COMMAND(ID_CAMERA_NEXT, OnCameraNext)
    ON_COMMAND(ID_CAMERA_PREVIOUS, OnCameraPrevious)
    ON_UPDATE_COMMAND_UI(ID_OVERDRAW, OnUpdateOverdraw)
    ON_COMMAND(ID_CAMERA_ROTATE, OnCameraRotate)
    ON_COMMAND(ID_CAMERA_TRANSLATE, OnCameraTranslate)
    ON_COMMAND(ID_CAMERA_TUMBLE, OnCameraTumble)
    ON_COMMAND(ID_COLLISION_LAB_TRANSLATE, OnCollisionLabTranslate)
    ON_COMMAND(ID_ZOOM_EXTENTS_ALL, OnZoomExtentsAll)
    ON_COMMAND(ID_CAMERA_RESET, OnCameraReset)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_ROTATE, OnUpdateCameraRotate)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_TRANSLATE, OnUpdateCameraTranslate)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_TUMBLE, OnUpdateCameraTumble)
    ON_UPDATE_COMMAND_UI(ID_COLLISION_LAB_TRANSLATE,
        OnUpdateCollisionLabTranslate)
    ON_COMMAND(ID_OVERDRAW, OnOverdraw)
    ON_UPDATE_COMMAND_UI(ID_OVERDRAW, OnUpdateOverdraw)
    ON_COMMAND(ID_WIREFRAME, OnWireframe)
    ON_UPDATE_COMMAND_UI(ID_WIREFRAME, OnUpdateWireframe)
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_COMMAND(ID_CAMERA_ZOOM, OnCameraZoom)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_ZOOM, OnUpdateCameraZoom)
    ON_COMMAND(ID_XAXIS_CONSTRAINT, OnXaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_XAXIS_CONSTRAINT, OnUpdateXaxisConstraint)
    ON_COMMAND(ID_XZAXIS_CONSTRAINT, OnXzaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_XZAXIS_CONSTRAINT, OnUpdateXzaxisConstraint)
    ON_COMMAND(ID_YAXIS_CONSTRAINT, OnYaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_YAXIS_CONSTRAINT, OnUpdateYaxisConstraint)
    ON_COMMAND(ID_ZAXIS_CONSTRAINT, OnZaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_ZAXIS_CONSTRAINT, OnUpdateZaxisConstraint)
    ON_COMMAND(ID_XYAXIS_CONSTRAINT, OnXyaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_XYAXIS_CONSTRAINT, OnUpdateXyaxisConstraint)
    ON_COMMAND(ID_YZAXIS_CONSTRAINT, OnYzaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_YZAXIS_CONSTRAINT, OnUpdateYzaxisConstraint)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_NEXT, OnUpdateCameraNext)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_PREVIOUS, OnUpdateCameraPrevious)
    ON_UPDATE_COMMAND_UI(ID_CAMERA_RESET, OnUpdateCameraReset)
    ON_UPDATE_COMMAND_UI(ID_ZOOM_EXTENTS_ALL, OnUpdateZoomExtentsAll)
    ON_COMMAND(ID_OBJECT_SELECTION, OnObjectSelection)
    ON_UPDATE_COMMAND_UI(ID_OBJECT_SELECTION, OnUpdateObjectSelection)
    ON_COMMAND(ID_LODDOWN_BUTTON, OnLoddownButton)
    ON_UPDATE_COMMAND_UI(ID_LODOVERRIDE_BUTTON, OnUpdateLodoverrideButton)
    ON_COMMAND(ID_LODOVERRIDE_BUTTON, OnLodoverrideButton)
    ON_COMMAND(ID_LODUP_BUTTON, OnLodupButton)
    ON_UPDATE_COMMAND_UI(ID_LODDOWN_BUTTON, OnUpdateLoddownButton)
    ON_UPDATE_COMMAND_UI(ID_LODUP_BUTTON, OnUpdateLodupButton)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_RELOAD_SHADERS, OnReloadShaders)
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifRenderView construction/destruction
//---------------------------------------------------------------------------
CNifRenderView::CNifRenderView() : 
     m_kVisible(1024, 1024),
     m_kCuller(&m_kVisible, NULL)
{
    NiDevImageConverter* pkConverter = NiTCreate<NiDevImageConverter>();   
    NiImageConverter::SetImageConverter(pkConverter);
    m_hThread = NULL;
    m_bSceneChanged = true;
    m_bBackgroundColorChanged = true;
    m_spCamera = NULL;
    m_spRenderer = NULL;
    m_bRendererCreationFailed = false;
    m_bNeedRecreateRenderer = false;
    m_hTopLevelHwnd = NULL;
    m_pkUIManager = new CNifRenderViewUIManager(this);
    m_kCameraIndex.uiCameraIndex = 0;
    m_kCameraIndex.uiRootIndex = 0;
    m_bNewDocument = false;
    m_eCurrentUIMode = OBJECT_SELECTION;
    m_bOverdraw = false;
    m_bWireframe = false;
    m_bOrbitCamera = true;
    m_pkCameraMenu = NULL;
    m_hMenu = NULL;
    // NOTE: sleep(0) starves the main (MFC) thread on Vista.
    m_uiSleepTime = 1;
    m_fLastTime = -1.0f;

    m_spFrame = NULL;
    m_spCuller = NULL;
    m_spSceneView = NULL;
    m_spSceneRenderClick = NULL;
    m_spAlphaProcessor = NULL;
    m_spElementView = NULL;
    m_spLabelRenderClick = NULL;
    m_spDebugGeometryClick = NULL;
    m_spRenderStep = NULL;
    m_kShadowRenderStepName = "NiApplication Shadow Render Step";
    m_spShadowRenderStep = NULL;
}

//---------------------------------------------------------------------------
CNifRenderView::~CNifRenderView()
{
    m_spCamera = NULL;
    delete m_pkUIManager;
    m_kCameraIndex.uiCameraIndex = 0;
    m_kCameraIndex.uiRootIndex = 0;
}
//---------------------------------------------------------------------------
BOOL CNifRenderView::PreCreateWindow(CREATESTRUCT& cs)
{
    // if we need to modify the style of this form, we can do it here
    return CView::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------
// CNifRenderView drawing
//---------------------------------------------------------------------------
void CNifRenderView::OnDraw(CDC* pDC)
{
}
//---------------------------------------------------------------------------
// CNifRenderView diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CNifRenderView::AssertValid() const
{
    CView::AssertValid();
}
//---------------------------------------------------------------------------
void CNifRenderView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
//---------------------------------------------------------------------------
CNifDoc* CNifRenderView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNifDoc)));
    return (CNifDoc*)m_pDocument;
}
#endif //_DEBUG
//---------------------------------------------------------------------------
CNifRenderViewUIManager* CNifRenderView::GetUIManager()
{
    return m_pkUIManager;
}
//---------------------------------------------------------------------------
// CNifRenderView message handlers
//---------------------------------------------------------------------------
void CNifRenderView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnRButtonDblClk(nFlags,point);

    CView::OnRButtonDblClk(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnRButtonDown(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnRButtonDown(nFlags, point);
    CView::OnRButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnRButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnRButtonUp( nFlags, point);
    CView::OnRButtonUp(nFlags, point);
}
//---------------------------------------------------------------------------
BOOL CNifRenderView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if(m_pkUIManager)
        m_pkUIManager->OnMouseWheel(nFlags, zDelta, pt);
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnMouseMove(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnMouseMove(nFlags, point);

    CView::OnMouseMove(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnLButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnLButtonUp(nFlags, point);

    CView::OnLButtonUp(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnLButtonDown(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnLButtonDown(nFlags,point);

    CView::OnLButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnLButtonDblClk(nFlags, point);

    CView::OnLButtonDblClk(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnMButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnMButtonUp(nFlags, point);

    CView::OnMButtonUp(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnMButtonDown(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnMButtonDown(nFlags,point);

    CView::OnMButtonDown(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    if(m_pkUIManager)
        m_pkUIManager->OnMButtonDblClk(nFlags, point);

    CView::OnMButtonDblClk(nFlags, point);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(m_pkUIManager)
        m_pkUIManager->OnKeyUp(nChar, nRepCnt, nFlags);

    CView::OnKeyUp(nChar, nRepCnt, nFlags);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(m_pkUIManager)
        m_pkUIManager->OnKeyDown(nChar, nRepCnt, nFlags);


    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnSysKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(m_pkUIManager)
        m_pkUIManager->OnKeyUp(nChar, nRepCnt, nFlags);

    CView::OnSysKeyUp(nChar, nRepCnt, nFlags);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(m_pkUIManager)
        m_pkUIManager->OnKeyDown(nChar, nRepCnt, nFlags);
    CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnSetFocus(CWnd* pOldWnd)
{
    CView::OnSetFocus(pOldWnd);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnKillFocus(CWnd* pNewWnd)
{
    if (m_pkUIManager)
        m_pkUIManager->OnKillFocus(pNewWnd);

    CView::OnKillFocus(pNewWnd);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    // Get screen metrics for scaling.
    CClientDC dc(this);
    m_kScreenBounds.x = (float) dc.GetDeviceCaps(HORZRES);
    m_kScreenBounds.y = (float ) dc.GetDeviceCaps(VERTRES);

    m_bNeedRecreateRenderer = true;

    // Reset the current camera frustum to fit the screen size
    CNifRenderViewUICommand* pkCommand = new
        CNifRenderViewResetFrustumCommand();

    if(m_pkUIManager)
        m_pkUIManager->QueueCommand(pkCommand);
}
//---------------------------------------------------------------------------
bool CNifRenderView::ReInitializeScene()
{
    CNifDoc* pkDoc = GetDocument();
    m_kRendererCS.Lock();
    if(pkDoc && m_spRenderer)
    {
        m_bSceneChanged = false;
        SetCurrentCameraIndices(m_kCameraIndex);
        if(pkDoc->GetNumberOfRoots() == 2 && m_bNewDocument)
        {
            if(m_pkUIManager)
            {
                OnZoomExtentsAll();
                m_bNewDocument = false;
            }
        }
        else
        {
            if (m_pkUIManager)
            {
                CNifRenderViewUICommand* pkCommand = new
                    CNifRenderViewResetFrustumCommand();
                m_pkUIManager->QueueCommand(pkCommand);
            }
        }

        if(m_pkUIManager)
        {
            CNifRenderViewUIState* pkState =
                m_pkUIManager->GetCurrentViewState();
            if(pkState)
                pkState->Initialize();
        }
    }
    m_kRendererCS.Unlock();
    return true;
}
//---------------------------------------------------------------------------
NiPoint2 CNifRenderView::GetScreenBounds()
{
    return m_kScreenBounds;
}
//---------------------------------------------------------------------------
void CNifRenderView::ReCreateRenderer()
{
    // Do not recreate renderer if the main mouse button is down. This is
    // usually the case when the window border is being dragged. This is to
    // prevent recreation of the renderer multiple times during one window
    // drag operation.
    int iMouseButton = VK_LBUTTON;
    if (GetSystemMetrics(SM_SWAPBUTTON))
    {
        iMouseButton = VK_RBUTTON;
    }
    if (GetAsyncKeyState(iMouseButton) & 0x8000)
    {
        return;
    }

    m_kRendererCS.Lock();
    CRect rect;
    GetClientRect(rect);

    if (rect.right > 0 && rect.bottom > 0)
    {
        if (g_bD3D10)
        {
            NiD3D10Renderer* pkD3D10Renderer = NiSmartPointerCast(
                NiD3D10Renderer, m_spRenderer);
            NIASSERT(pkD3D10Renderer);
            pkD3D10Renderer->ResizeBuffers(0, 0);
        }
        else
        {
            NiDX9Renderer* pkDX9Renderer = NiSmartPointerCast(
                NiDX9Renderer, m_spRenderer);
            NIASSERT(pkDX9Renderer);
            pkDX9Renderer->Recreate(0, 0, (NiDX9Renderer::FlagType)
                (NiDX9Renderer::USE_MULTITHREADED | 
                NiDX9Renderer::USE_STENCIL),
                m_hWnd);
        }
    }

    UpdateScreenConsole();

    m_bNeedRecreateRenderer = false;
    m_kRendererCS.Unlock();
}
//---------------------------------------------------------------------------
void CNifRenderView::CreateRenderer()
{
    m_kRendererCS.Lock();

    // Do not purge source texture data when loading
    NiSourceTexture::SetDestroyAppDataFlag(false);

    // Create a Direct3D renderer.
    bool bD3D10Failed = false;
    m_spRenderer = NULL;
    if (g_bD3D10)
    {
        NiD3D10Renderer::CreationParameters kParams(m_hWnd);
        kParams.m_uiCreateFlags &= 
            ~NiD3D10Renderer::CREATE_DEVICE_SINGLETHREADED;
        NiD3D10RendererPtr spD3D10Renderer;
        bool bSuccess = NiD3D10Renderer::Create(kParams, spD3D10Renderer);
        if (bSuccess)
        {
            m_spRenderer = spD3D10Renderer;
        }
        else
        {
            // D3D10 renderer creation failed. Try to use DX9 renderer.
            // A message box is displayed at the end of this function notifying
            // the user that DX9 was used instead.
            bD3D10Failed = true;
            g_bD3D10 = false;
        }
    }
    if (!g_bD3D10)
    {
        m_spRenderer = NiDX9Renderer::Create(0, 0, (NiDX9Renderer::FlagType)
            (NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL), 
            m_hWnd, m_hTopLevelHwnd);
    }

    if (!m_spRenderer)
    {
        m_kRendererCS.Unlock();
        m_bRendererCreationFailed = true;
        CNifDoc* pDoc = GetDocument();
        if (pDoc)
        {
            pDoc->Lock();
            pDoc->SetRendererCreationFailed(true);
            pDoc->UnLock();
        }
        return;
    }

    NiDelete NiDataStream::GetFactory();
    if (g_bD3D10)
    {
        NiDataStream::SetFactory(
            NiTCreate<NiTDataStreamFactoryProxy<NiD3D10DataStreamFactory> >());
    }
    else
    {
        NiDataStream::SetFactory(
            NiTCreate<NiTDataStreamFactoryProxy<NiDX9DataStreamFactory> >());
    }

    CNifDoc* pDoc = GetDocument();

    pDoc->Lock();

    // Load Cg Shader Library, if it exists and we are running DX9.
    // Only attempt to load the Cg Shader Library if Cg is installed.
    m_hNiCgShaderLib = NULL;
    if (!g_bD3D10)
    {
        HMODULE hCgCheck = LoadLibrary("CgD3D9.dll");
        if (hCgCheck)
        {
            FreeLibrary(hCgCheck);

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
    }

    // Load other shaders
#if defined(_MSC_VER) && _MSC_VER >= 1400
    char acTemp[NI_MAX_PATH];
    size_t stBufferLength = 0;
    if (getenv_s(&stBufferLength, NULL, 0, "EGB_SHADER_LIBRARY_PATH") == 0 &&
        stBufferLength > 0)
    {
        getenv_s(&stBufferLength, acTemp, NI_MAX_PATH, 
            "EGB_SHADER_LIBRARY_PATH");
    }
    else
    {
        acTemp[0] = '\0';
    }
    CString strShaderPath(acTemp);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    CString strShaderPath = getenv("EGB_SHADER_LIBRARY_PATH");
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (strShaderPath.IsEmpty())
    {
        ::MessageBox(NULL, "The environment variable EGB_SHADER_LIBRARY_PATH "
            "must\nbe defined for this application to properly execute.",
            "Missing Environment Variable", MB_OK | MB_ICONERROR);
    }
    else
    {
        NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
        pkToolkit->LoadFromDLL(strShaderPath);

        CString strShaderDir = strShaderPath + "\\Data\\";
        if (g_bD3D10)
        {
            strShaderDir += "D3D10\\";
        }
        else
        {
            strShaderDir += "DX9\\";
        }
        pkToolkit->SetMaterialDirectory(strShaderDir);
    }

    NiMaterialHelpers::RegisterMaterials(pDoc->GetSceneGraph(), m_spRenderer);

    if (!m_spCameraLabel)
    {
        CreateScreenConsole();
    }

    // Background color for renderer
    CNifUserPreferences::Lock();
    NiColor kColor =
        CNifUserPreferences::AccessUserPreferences()->GetBackgroundColor();
    m_spRenderer->SetBackgroundColor(kColor);
    CNifUserPreferences::UnLock();

    ReInitializeScene();

    pDoc->SetRendererCreated(true);
    pDoc->UnLock();
    m_kRendererCS.Unlock();

    if (bD3D10Failed)
    {
        MessageBox("A D3D10 renderer could not be created. Using DX9 "
            "instead.", "D3D10 Renderer Creation Failure", MB_OK |
            MB_ICONEXCLAMATION);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::DestroyRenderer()
{
    m_kRendererCS.Lock();
    m_spCamera = NULL;

    if (m_hNiCgShaderLib)
        FreeLibrary(m_hNiCgShaderLib);

    m_spFrame = NULL;
    m_spCuller = NULL;
    m_spSceneView = NULL;
    m_spSceneRenderClick = NULL;
    m_spAlphaProcessor = NULL;
    m_spElementView = NULL;
    m_spLabelRenderClick = NULL;
    m_spRenderStep = NULL;

    m_spCameraLabel = NULL;
    m_spFont = NULL;

    m_spShadowRenderStep = NULL;
    m_spDebugGeometryClick = NULL;

    // The shadow manager needs to be shutdown after the scene has been 
    // destroyed, but before the renderer is destroyed.
    NiShadowManager::Shutdown();

    m_spRenderer = NULL;

    m_kRendererCS.Unlock();
}
//---------------------------------------------------------------------------
void CNifRenderView::CreateFrame()
{
    CNifDoc* pkDoc = GetDocument();
    if (!pkDoc->GetSceneGraph())
        return;

    // delete previous instance  
    if (m_spFrame)
        m_spFrame = 0;

    // retrieve the document camera
    NiCamera* pkCamera = pkDoc->GetCamera(m_kCameraIndex);

    // create a new culling process for Ni3DRenderView
    m_spCuller = NiTCreate2<NiMeshCullingProcess>(&m_kVisible, 
        (NiSPWorkflowManager*)NULL);

    // Initialize shadow click generator and active it.
    NiShadowManager::SetActiveShadowClickGenerator(
        "NiDefaultShadowClickGenerator");

    // Initialize shadow manager parameters.
    NiShadowManager::SetSceneCamera(pkCamera);
    NiShadowManager::SetCullingProcess(m_spCuller);

    // Create shadow render step.
    if (!m_spShadowRenderStep)
    {
        m_spShadowRenderStep =
            NiTCreate<NiDefaultClickRenderStep>();
        m_spShadowRenderStep->SetName(m_kShadowRenderStepName);
        m_spShadowRenderStep->SetPreProcessingCallbackFunc(
            &CNifRenderView::ShadowRenderStepPre);
    }

    // create the scene graph RenderView
    m_spSceneView = NiTCreate2<Ni3DRenderView>(pkCamera, m_spCuller);
    m_spSceneView->AppendScene(pkDoc->GetSceneGraph());

    // create main scene graph render click.
    m_spSceneRenderClick = NiTCreate<NiViewRenderClick>();
    m_spSceneRenderClick->AppendRenderView(m_spSceneView);
    m_spSceneRenderClick->SetViewport(m_spCamera->GetViewPort());
    m_spSceneRenderClick->SetClearAllBuffers(true);

    // add an alpha processor (accumulator)
    m_spAlphaProcessor = NiTCreate<NiAlphaSortProcessor>();
    m_spSceneRenderClick->SetProcessor(m_spAlphaProcessor);

    // create debug geometry render click
    m_spDebugGeometryClick = NiTCreate<NiDebugGeometryClick>();
    m_spDebugGeometryClick->SetName("DebugGeometryClick");
    m_spDebugGeometryClick->SetCamera(m_spCamera);
    m_spDebugGeometryClick->SetClearAllBuffers(false);
    m_spDebugGeometryClick->ClearScene();
    m_spDebugGeometryClick->ProcessScene(pkDoc->GetSceneGraph(),
        NiDebugGeometryClick::ALL_MASK & ~NiDebugGeometryClick::CAMERA_MASK);
    CNifCameraList* pkCamList = pkDoc->GetCameraList();
    if (pkCamList)
    {
        // Add cameras
        unsigned int uiNumCameras = pkCamList->NumCameras();

        NiCameraInfoIndex kIndex;
        kIndex.uiCameraIndex = 0;
        kIndex.uiRootIndex = 0;

        for(unsigned int i = 0; i < uiNumCameras; i++)
        {
            NiCamera* pkCamera = pkCamList->GetCamera(kIndex);
            pkCamera->Update(0.0f);
            m_spDebugGeometryClick->ProcessScene(pkCamera);

            kIndex = pkCamList->GetNextCameraInfoIndex(kIndex);
        }
    }

    // create screen elements RenderView
    m_spElementView = NiTCreate<NiMesh2DRenderView>();
    NiMeshScreenElementsArray& kScreenElements =
        pkDoc->GetScreenElementsArray();
    const unsigned int uiSESize = kScreenElements.GetSize();
    for (unsigned int i = 0; i < uiSESize; i++)
    {
        NiMeshScreenElementsPtr pkElement = kScreenElements.GetAt(i);
        if (pkElement)
            m_spElementView->AppendScreenElement(pkElement);
    }

    // create a RenderClick for the Ni2DString
    m_spLabelRenderClick = NiTCreate<Ni2DStringRenderClick>();
    m_spLabelRenderClick->Append2DString(m_spCameraLabel);

    // create the render step
    m_spRenderStep = NiTCreate<NiDefaultClickRenderStep>();
    m_spRenderStep->AppendRenderClick(m_spSceneRenderClick);
    m_spRenderStep->AppendRenderClick(m_spDebugGeometryClick);
    m_spRenderStep->AppendRenderClick(m_spLabelRenderClick);

    // Create render frame
    m_spFrame = NiTCreate<NiRenderFrame>();
    m_spFrame->AppendRenderStep(m_spShadowRenderStep);
    m_spFrame->AppendRenderStep(m_spRenderStep);
}
//---------------------------------------------------------------------------
bool CNifRenderView::ShadowRenderStepPre(NiRenderStep* pkCurrentStep,
    void* pvCallbackData)
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
void CNifRenderView::ClearClickSwap (NiCamera* pkCamera)
{
    m_kRendererCS.Lock();
    if (m_spRenderer)
    {
        CNifDoc* pkDoc = GetDocument();

        // set the background color
        if (m_bBackgroundColorChanged && pkDoc)
        {
            CNifUserPreferences::Lock();
            NiColor kColor = CNifUserPreferences::AccessUserPreferences()
                ->GetBackgroundColor();
            m_spRenderer->SetBackgroundColor(kColor);
            CNifUserPreferences::UnLock();
            m_bBackgroundColorChanged = false;
        }

        if (pkCamera)
        {
            CNifStatisticsManager::Lock();
            CNifStatisticsManager* pkStats =
                CNifStatisticsManager::AccessStatisticsManager();

            if (!m_spFrame)
                CreateFrame();

            // Make sure that the CreateFrame function actually worked
            if (!m_spFrame)
            {
                m_kRendererCS.Unlock();

                // We don't want to continue if we can not create the frame
                m_bContinue = false;
                return;
            }

            NiCamera* pkCamera = pkDoc->GetCamera(m_kCameraIndex);
            m_spSceneView->SetCamera(pkCamera);

            m_spFrame->Draw();

            pkStats->StartSwapTimer();
            m_spFrame->Display();
            pkStats->StopSwapTimer();

            unsigned int uiNumObjectsDrawn;
            float fCullTime, fRenderTime;
            m_spFrame->GatherStatistics(uiNumObjectsDrawn, fCullTime,
                fRenderTime);
            pkStats->SetNumObjectsDrawnPerFrame(uiNumObjectsDrawn);
            pkStats->SetCullTime(fCullTime);
            pkStats->SetRenderTime(fRenderTime);

            CNifStatisticsManager::UnLock();
        }
    }
    m_kRendererCS.Unlock();
}
//---------------------------------------------------------------------------
void CNifRenderView::OnIdle()
{
    CNifDoc* pDoc = GetDocument();
    if(pDoc == NULL)
        return;

    // Limit frame rate to g_uiMaxFrameRate.
    float fCurrentTime = NiGetCurrentTimeInSec();
    if (m_fLastTime == -1.0f)
    {
        m_fLastTime = fCurrentTime;
    }
    if (fCurrentTime - m_fLastTime < g_fMinFrameTime)
    {
        return;
    }
    m_fLastTime = fCurrentTime;

    pDoc->Lock();
    bool bForceUpdate = false;

    if (pDoc->m_kCollisionLabInfo.GetUpdate())
    {
        bForceUpdate = true;
        pDoc->m_kCollisionLabInfo.SetUpdate(false);
    }
    pDoc->UnLock();

    if(!m_spRenderer)
    {
        // The ShadowManager needs to be initialized before the renderer is 
        // created 
        if (!NiShadowManager::GetShadowManager())
            NiShadowManager::Initialize();

        CreateRenderer();
        m_bSceneChanged = true;
    }

    if(m_spRenderer && m_bNeedRecreateRenderer)
        ReCreateRenderer();

    pDoc->Lock();

    if (m_spCamera == NULL || m_spRenderer == NULL)
    {
        NiOutputDebugString("**CNifRenderView**\n"
            "   No camera or no renderer!\n");

        pDoc->UnLock();

        if (m_bRendererCreationFailed)
        {
            ::MessageBox(NULL, "Renderer creation failed. Please restart "
                "application.", "Renderer Creation Failed", MB_OK |
                MB_ICONERROR);
            m_bContinue = false;
        }
        return;
    }

    if (m_bSceneChanged)
    {
        bool bResult = ReInitializeScene();
        ASSERT(bResult);
        bForceUpdate = true;
        CreateFrame();
    }

    if (m_pkUIManager)
    {
        m_pkUIManager->Update();
    }

    CNifStatisticsManager::Lock();
    CNifStatisticsManager* pkStats =
        CNifStatisticsManager::AccessStatisticsManager();

    pkStats->StartUpdateTimer();
    float fTime = pDoc->UpdateScene(bForceUpdate);

    if(GetCurrentCameraIndices().uiRootIndex == 0)
    {
        NiNode* pkRoot = pDoc->GetSceneGraph();
        if (pkRoot != NULL)
        {
            NiBound kBound = pkRoot->GetWorldBound();
            m_spCamera->FitNearAndFarToBound(kBound);
            if (m_spDebugGeometryClick)
                m_spDebugGeometryClick->ExtendCameraNearAndFar(m_spCamera);
        }
    }

    m_spCamera->Update(fTime);
    NiMesh::CompleteSceneModifiers(m_spCamera);
    pkStats->StopUpdateTimer();

    ClearClickSwap(m_spCamera);

    pkStats->SampleFrameRate();
    CNifStatisticsManager::UnLock();

    pDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifRenderView::OnInitialUpdate()
{
    CView::OnInitialUpdate();
    m_bNewDocument = true;

    m_hTopLevelHwnd = GetTopLevelParent()->GetSafeHwnd();

    if(m_hThread == NULL)
    {
        m_bContinue = true;
        CWinThread* pThread = AfxBeginThread(ThreadFunc, LPVOID (this),
            THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
        ::DuplicateHandle(GetCurrentProcess(), pThread->m_hThread,
            GetCurrentProcess(), &m_hThread, 0, FALSE,
            DUPLICATE_SAME_ACCESS);
        pThread->ResumeThread();
        SetCurrentUIMode(m_eCurrentUIMode);
    }
}
//---------------------------------------------------------------------------
BOOL CNifRenderView::DestroyWindow()
{
    return CView::DestroyWindow();
}
//---------------------------------------------------------------------------
NiRenderer* CNifRenderView::GetRenderer()
{
    return m_spRenderer;
}
//---------------------------------------------------------------------------
UINT CNifRenderView::ThreadFunc (LPVOID pParam)
{
    NiOutputDebugString("**THREAD**\n"
        "   Starting CNifRenderView::ThreadFunc\n");

    CNifRenderView* pThis = (CNifRenderView*) pParam;

    if(pThis == NULL)
    {
        NiOutputDebugString("**THREAD**\n"
        "   Ending CNifRenderView::ThreadFunc\n"
        "   Status: BAD (INVALID pTHIS)\n");
        return (UINT) -1;
    }

    while(!pThis->IsWindowVisible())
        Sleep(10);

    while(pThis->m_bContinue)
    {
        pThis->OnIdle();
        Sleep(pThis->m_uiSleepTime);
    }

    pThis->DestroyRenderer();

    NiOutputDebugString("**THREAD**\n"
        "   Ending CNifRenderView::ThreadFunc\n"
        "   Status: OKAY\n");

    return 0;
}
//---------------------------------------------------------------------------
void CNifRenderView::OnDestroy()
{
    m_bContinue = false;
    if (m_hThread != NULL)
    {
        ::WaitForSingleObject(m_hThread, INFINITE);
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    CView::OnDestroy();
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    WORD wLoWord = LOWORD(lHint);

    if(m_pkUIManager)
        m_pkUIManager->OnUpdate(wLoWord);

    switch (wLoWord)
    {
        case NIF_DESTROYSCENE:
//            NiOutputDebugString("CNifRenderView acknowledges "
//                "scene destruction\n");
            CNifRenderViewOverdrawIndicatorCommand::CleanUp();
            m_pkCameraMenu = NULL;
            m_bNewDocument = true;
            break;
        case NIF_CREATESCENE:
            m_bNewDocument = true;
            m_kCameraIndex.uiCameraIndex  = 0;
            m_kCameraIndex.uiRootIndex = 0;
            if(m_bOverdraw)
            {
                OnOverdraw();
                m_bOverdraw = true;
            }
            if(m_bWireframe)
            {
                OnWireframe();
                m_bWireframe = true;
            }
            BuildCameraMenu();
            m_bSceneChanged = true;
            break;
        case NIF_UPDATECAMERASMENU:
            BuildCameraMenu();
            break;
        case NIF_ADDNIF:
            BuildCameraMenu();
            m_bSceneChanged = true;
            break;
        case NIF_REMOVENIF:
            {
                CNifDoc* pkDoc = GetDocument();
                if(pkDoc)
                {
                    pkDoc->Lock();
                    if(pkDoc->GetCameraInfo(m_kCameraIndex) == NULL)
                    {
                        m_kCameraIndex.uiCameraIndex  = 0;
                        m_kCameraIndex.uiRootIndex = 0;
                    }
                    pkDoc->UnLock();
                }

                BuildCameraMenu();
                m_bSceneChanged = true;
            }
            break;
        case NIF_SCENECHANGED:
            m_bSceneChanged = true;
            break;
        case NIF_USERPREFERENCESCHANGED:
            m_bBackgroundColorChanged = true;
            break;
        case NIF_CAMERAMODECHANGED:
            if (OBJECT_SELECTION == m_eCurrentUIMode)
            {
                SetCurrentUIMode(OBJECT_SELECTION, true);
            }
            break;
        default:
            break;
    }
}
//---------------------------------------------------------------------------
BOOL CNifRenderView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
    AFX_CMDHANDLERINFO* pHandlerInfo)
{
    return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
//---------------------------------------------------------------------------
BOOL CNifRenderView::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return CView::OnCommand(wParam, lParam);
}
//---------------------------------------------------------------------------
NiCameraInfoIndex CNifRenderView::GetCurrentCameraIndices()
{
    return m_kCameraIndex;
}
//---------------------------------------------------------------------------
void CNifRenderView::SetCurrentCameraIndices(NiCameraInfoIndex kIndex)
{
    CNifDoc* pkDoc = GetDocument();
    if(pkDoc)
    {
        pkDoc->Lock();
        NiCamera* pkCamera = pkDoc->GetCamera(kIndex);

        if(pkCamera)
        {
            int iOldIndex =  GetMenuIndexForCamera(m_kCameraIndex);
            m_kCameraIndex = kIndex;
            int iNewIndex =  GetMenuIndexForCamera(kIndex);
            if(iOldIndex != -1 && iNewIndex != -1 && m_pkCameraMenu)
            {
                ::CheckMenuItem(m_hMenu, iOldIndex, MF_BYPOSITION |
                                MF_UNCHECKED);
                ::CheckMenuItem(m_hMenu, iNewIndex, MF_BYPOSITION |
                                MF_CHECKED);
            }

            m_spCamera = pkCamera;
            if (m_spDebugGeometryClick)
                m_spDebugGeometryClick->SetCamera(m_spCamera);

            UpdateScreenConsole();

            // Inform the shadowing system that the main scene camera has 
            // changed.
            if (NiShadowManager::GetShadowManager())
                NiShadowManager::SetSceneCamera(pkCamera);

            SetCurrentUIMode(OBJECT_SELECTION, true);

            if(m_pkUIManager)
                m_pkUIManager->OnUpdate(NIF_CURRENTCAMERACHANGED);
        }

        pkDoc->UnLock();
    }

}
//---------------------------------------------------------------------------
bool CNifRenderView::IsCurrentCameraDefault()
{
    bool bDefault = false;

    CNifDoc* pkDoc = GetDocument();
    if(pkDoc)
    {
        pkDoc->Lock();
        NiCameraInfo* pkInfo = pkDoc->GetCameraInfo(m_kCameraIndex);

        if (pkInfo)
        {
            bDefault = pkInfo->m_bIsDefault;
        }

        pkDoc->UnLock();
    }

    return bDefault;
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraFocus()
{
    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewCameraFocusCommand();

        m_pkUIManager->QueueCommand(pkCommand);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraNext()
{
    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewCameraNextCommand();

        m_pkUIManager->QueueCommand(pkCommand);

        pkCommand = new
            CNifRenderViewResetFrustumCommand();

        m_pkUIManager->QueueCommand(pkCommand);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraPrevious()
{
    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewCameraPreviousCommand();

        m_pkUIManager->QueueCommand(pkCommand);

        pkCommand = new
            CNifRenderViewResetFrustumCommand();

        m_pkUIManager->QueueCommand(pkCommand);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::SetCurrentUIMode(
    PersistentUIMode eMode,
    bool bForceUpdate/* = false*/)
{
    CNifDoc* pkDoc = GetDocument();
    pkDoc->Lock();

    if(eMode >= 0 && eMode < NUM_UIMODES && m_pkUIManager)
    {
        if(!bForceUpdate &&
            eMode == m_eCurrentUIMode &&
            m_pkUIManager->GetCurrentViewState())
        {
            pkDoc->UnLock();
            return;
        }

        switch(eMode)
        {
            case COLLISION_LAB_TRANSLATE:
            {
                CNifRenderViewCollisionLabTranslationState* pkState =
                    new CNifRenderViewCollisionLabTranslationState(this);
                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;

                break;
            }
            case CAMERA_TRANSLATE:
            {
                CNifRenderViewCameraTranslationState* pkState =
                    new CNifRenderViewCameraTranslationState(this);
                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;
                break;
            }
            case CAMERA_ROTATE:
            {
                CNifRenderViewCameraRotateState* pkState =
                    new CNifRenderViewCameraRotateState(this);
                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;
                break;
            }
            case CAMERA_ZOOM:
            {
                CNifRenderViewCameraZoomState* pkState =
                    new CNifRenderViewCameraZoomState(this);
                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;
                break;
            }
            case CAMERA_TUMBLE:
            {
                CNifRenderViewCameraTumbleState* pkState =
                    new CNifRenderViewCameraTumbleState(this);
                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;
                break;
            }
            case OBJECT_SELECTION:
            {
                CNifUserPreferences::Lock();
                CNifUserPreferences* pkPrefs =
                    CNifUserPreferences::AccessUserPreferences();

                CNifUserPreferences::CameraControlMode eCameraMode =
                    pkPrefs->GetCameraControlMode();

                pkPrefs = NULL;
                CNifUserPreferences::UnLock();

                CNifRenderViewUIState* pkState = NULL;
                if (CNifUserPreferences::CCMODE_3DSMAX == eCameraMode)
                {
                    pkState = new CNifRenderViewCamera3dsmaxState(this);
                }
                else if (CNifUserPreferences::CCMODE_MAYA == eCameraMode)
                {
                    pkState = new CNifRenderViewCameraMayaState(this);
                }

                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;

                break;
            }
            case CAMERA_LOCKED:
            {
                CNifRenderViewUIState* pkState =
                     new CNifRenderViewCameraLockedState(this);
                m_pkUIManager->SetCurrentViewState(pkState);
                m_eCurrentUIMode = eMode;
                break;
            }
            default:
                NIASSERT(false);
                break;
        }
    }
    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
CNifRenderView::PersistentUIMode CNifRenderView::GetCurrentUIMode() const
{
    return m_eCurrentUIMode;
}
//---------------------------------------------------------------------------
void CNifRenderView::SetCurrentAxisMode(CNifRenderViewUIState::
    UIAxisConstraint eMode)
{
    if(m_pkUIManager)
    {
        m_pkUIManager->SetCurrentAxisMode(eMode);
    }
}
//---------------------------------------------------------------------------
CNifRenderViewUIState::UIAxisConstraint CNifRenderView::GetCurrentAxisMode()
{
    if(m_pkUIManager)
        return m_pkUIManager->GetCurrentAxisMode();
    else
        return CNifRenderViewUIState::CONSTRAINT_COUNT;
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraRotate()
{
    SetCurrentUIMode(CAMERA_ROTATE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCollisionLabTranslate()
{
    SetCurrentUIMode(COLLISION_LAB_TRANSLATE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraTranslate()
{
    SetCurrentUIMode(CAMERA_TRANSLATE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraTumble()
{
    SetCurrentUIMode(CAMERA_TUMBLE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraZoom()
{
    SetCurrentUIMode(CAMERA_ZOOM);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnObjectSelection()
{
    SetCurrentUIMode(OBJECT_SELECTION);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateObjectSelection(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(OBJECT_SELECTION == m_eCurrentUIMode);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraZoom(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(CAMERA_ZOOM == m_eCurrentUIMode);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraRotate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(CAMERA_ROTATE == m_eCurrentUIMode);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraTranslate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(CAMERA_TRANSLATE == m_eCurrentUIMode);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraTumble(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(CAMERA_TUMBLE == m_eCurrentUIMode);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCollisionLabTranslate(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck(COLLISION_LAB_TRANSLATE == m_eCurrentUIMode);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnZoomExtentsAll()
{
    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewResetFrustumCommand();

        m_pkUIManager->QueueCommand(pkCommand);
        pkCommand = new
            CNifRenderViewZoomExtentsCommand(
                CNifRenderViewZoomExtentsCommand::EXTENTS_ALL);

        m_pkUIManager->QueueCommand(pkCommand);


    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraReset()
{

    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewCameraResetCommand();

        m_pkUIManager->QueueCommand(pkCommand);

        pkCommand = new
            CNifRenderViewResetFrustumCommand();

        m_pkUIManager->QueueCommand(pkCommand);

    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnOverdraw()
{
    if(m_pkUIManager)
    {
        m_bOverdraw = !m_bOverdraw;
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewOverdrawIndicatorCommand();

        m_pkUIManager->QueueCommand(pkCommand);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateOverdraw(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bOverdraw);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnWireframe()
{
    if(m_pkUIManager)
    {
        m_bWireframe = !m_bWireframe;
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewWireframeCommand();

        m_pkUIManager->QueueCommand(pkCommand);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateWireframe(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bWireframe);
}
//---------------------------------------------------------------------------
void CNifRenderView::CreateScreenConsole()
{
    if (!m_spCameraLabel)
    {
        CAssetViewerApp* pkTheApp = (CAssetViewerApp*) AfxGetApp();
        CString kAppDir = pkTheApp->GetApplicationDirectory();
        CString kFontDir = kAppDir + "\\..\\CourierNew10.NFF";

        m_spFont = NiFont::Create(m_spRenderer, kFontDir);

        NiColorA kColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_spCameraLabel = NiTCreate4<Ni2DString>(
            m_spFont, 
            NiFontString::COLORED, 64, "");
        
        m_spCameraLabel->SetColor(kColor);
        m_spCameraLabel->SetPosition(9, 0);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::UpdateScreenConsole()
{
    if (m_spCameraLabel && m_spCamera)
    {
        char acString[256];
        NiSprintf(acString, 256, "%s", m_spCamera->GetName());

        m_spCameraLabel->SetText(acString);
        m_spCameraLabel->UpdateScreenElements(true);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::BuildCameraMenu()
{

    CNifDoc * pkDoc = GetDocument();
    if(!pkDoc)
        return;

    pkDoc->Lock();

    CNifCameraList* pkCamList = pkDoc->GetCameraList();
    if(!pkCamList)
    {
        pkDoc->UnLock();
        return;
    }

    AttachCameraMenu();
    EmptyCameraMenu();
    unsigned int uiNumCameras = pkCamList->NumCameras();
    m_uiMaxCameras = uiNumCameras;

    unsigned int uiInsertIndex = 0;
    NiCameraInfoIndex kIndex;
    kIndex.uiCameraIndex = 0;
    kIndex.uiRootIndex = 0;
    int iUnnamedCameras = 0;

    NiCameraInfo* pkInfo = NULL;
    char acString[256];

    for(;uiInsertIndex < uiNumCameras; uiInsertIndex++)
    {
        pkInfo = pkCamList->GetCameraInfo(kIndex);

        if(pkInfo && pkInfo->m_spCam)
        {
            if(pkInfo->m_spCam->GetName())
                NiSprintf(acString, 256, "%s", pkInfo->m_spCam->GetName());
            else
                NiSprintf(acString, 256, "Unnamed Camera %d",
                    iUnnamedCameras++);

            m_pkCameraMenu->InsertMenu(uiInsertIndex, MF_STRING ,
                ID_CAMERA_BEGIN + uiInsertIndex, acString);
            char acString[256];
            NiSprintf(acString, 256, "********\nInserting menu item at idx: "
                "%d\n********\n", uiInsertIndex);
            NiOutputDebugString(acString);

            if(kIndex.uiCameraIndex == this->m_kCameraIndex.uiCameraIndex &&
               kIndex.uiRootIndex == this->m_kCameraIndex.uiRootIndex)
            {
                m_pkCameraMenu->CheckMenuItem(uiInsertIndex, MF_BYPOSITION |
                    MF_CHECKED);
            }
        }

        kIndex = pkCamList->GetNextCameraInfoIndex(kIndex);
    }

    pkDoc->UnLock();
}
//---------------------------------------------------------------------------
void CNifRenderView::AttachCameraMenu()
{
    CWnd* pkMainWnd = AfxGetApp()->GetMainWnd();
    if(pkMainWnd)
    {
        m_pkCameraMenu = NULL;
        CMenu* pkMenu = pkMainWnd->GetMenu();
        if(!pkMenu)
            return;

        CString kString;
        for(unsigned int ui = 0; ui < pkMenu->GetMenuItemCount(); ui++)
        {
            pkMenu->GetMenuString(ui, kString, MF_BYPOSITION);
            if(kString.CompareNoCase("&Cameras") == 0)
            {
                m_pkCameraMenu = pkMenu->GetSubMenu(ui);
                m_hMenu = m_pkCameraMenu->GetSafeHmenu();
                return;
            }
        }
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::EmptyCameraMenu()
{
    unsigned int uiMenuCount = m_pkCameraMenu->GetMenuItemCount();
    char acString[256];
    NiSprintf(acString, 256, "********\nRemoving %d Menu items\n********\n",
        uiMenuCount);
    NiOutputDebugString(acString);
    m_pkCameraMenu->RemoveMenu(ID_CAMERAS_PLACEHOLDERCAMERA, MF_BYCOMMAND);
    for(unsigned int ui= 0; ui < uiMenuCount; ui++)
    {
        UINT uiRet = m_pkCameraMenu->RemoveMenu(ID_CAMERA_BEGIN + ui,
            MF_BYCOMMAND);
    }
}
//---------------------------------------------------------------------------
int CNifRenderView::GetMenuIndexForCamera(NiCameraInfoIndex kIndex)
{
    CNifDoc * pkDoc = GetDocument();
    if(!pkDoc)
        return -1;

    pkDoc->Lock();

    CNifCameraList* pkCamList = pkDoc->GetCameraList();
    if(!pkCamList)
    {
        pkDoc->UnLock();
        return -1;
    }

    unsigned int uiMaxCameras = pkCamList->NumCameras();
    NiCameraInfoIndex kTempIndex;
    kTempIndex.uiCameraIndex = 0;
    kTempIndex.uiRootIndex = 0;

    NiCameraInfo* pkInfo = NULL;

    for(unsigned int ui = 0;ui < uiMaxCameras; ui++)
    {
        if(kTempIndex.uiCameraIndex == kIndex.uiCameraIndex &&
           kTempIndex.uiRootIndex == kIndex.uiRootIndex)
        {
            pkDoc->UnLock();
            return ui;
        }
        kTempIndex = pkCamList->GetNextCameraInfoIndex(kTempIndex);
    }

    pkDoc->UnLock();
    return -1;
}
//---------------------------------------------------------------------------
void CNifRenderView::OnCameraMenuSelection(UINT nID)
{
    unsigned int uiWhichIndex = nID - ID_CAMERA_BEGIN;
    CNifDoc * pkDoc = GetDocument();

    if(!pkDoc)
        return;

    pkDoc->Lock();

    CNifCameraList* pkCamList = pkDoc->GetCameraList();
    if(!pkCamList)
    {
        pkDoc->UnLock();
        return;
    }

    unsigned int uiMaxCameras = pkCamList->NumCameras();
    NiCameraInfoIndex kTempIndex;
    kTempIndex.uiCameraIndex = 0;
    kTempIndex.uiRootIndex = 0;

    NiCameraInfo* pkInfo = NULL;

    for(unsigned int ui = 0;ui < uiMaxCameras; ui++)
    {
        if(uiWhichIndex == ui)
        {
            SetCurrentCameraIndices(kTempIndex);

            // Ensure that the camera has the proper
            // aspect ratio given the windows dimensions
            if (m_pkUIManager)
            {
                CNifRenderViewUICommand* pkCommand = new
                    CNifRenderViewResetFrustumCommand();
                m_pkUIManager->QueueCommand(pkCommand);
            }

            pkDoc->UnLock();
            return;
        }
        kTempIndex = pkCamList->GetNextCameraInfoIndex(kTempIndex);
    }

    pkDoc->UnLock();

}
//---------------------------------------------------------------------------
void CNifRenderView::OnXaxisConstraint()
{
    SetCurrentAxisMode(CNifRenderViewUIState::RIGHT_AXIS);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateXaxisConstraint(CCmdUI* pCmdUI)
{
    if (m_pkUIManager &&
        m_pkUIManager->IsAxisModeAllowed(CNifRenderViewUIState::RIGHT_AXIS))
    {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_pkUIManager->GetCurrentAxisMode()
            == CNifRenderViewUIState::RIGHT_AXIS);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnYaxisConstraint()
{
    SetCurrentAxisMode(CNifRenderViewUIState::UP_AXIS);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateYaxisConstraint(CCmdUI* pCmdUI)
{
    if (m_pkUIManager &&
        m_pkUIManager->IsAxisModeAllowed(CNifRenderViewUIState::UP_AXIS))
    {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_pkUIManager->GetCurrentAxisMode()
            == CNifRenderViewUIState::UP_AXIS);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnZaxisConstraint()
{
    SetCurrentAxisMode(CNifRenderViewUIState::FORWARD_AXIS);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateZaxisConstraint(CCmdUI* pCmdUI)
{
    if (m_pkUIManager &&
        m_pkUIManager->IsAxisModeAllowed(
        CNifRenderViewUIState::FORWARD_AXIS))
    {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_pkUIManager->GetCurrentAxisMode()
            == CNifRenderViewUIState::FORWARD_AXIS);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnXyaxisConstraint()
{
    SetCurrentAxisMode(CNifRenderViewUIState::RIGHT_UP_AXIS);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateXyaxisConstraint(CCmdUI* pCmdUI)
{
    if (m_pkUIManager &&
        m_pkUIManager->IsAxisModeAllowed(CNifRenderViewUIState::
            RIGHT_UP_AXIS))
    {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_pkUIManager->GetCurrentAxisMode()
            == CNifRenderViewUIState::RIGHT_UP_AXIS);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnXzaxisConstraint()
{
    SetCurrentAxisMode(CNifRenderViewUIState::FORWARD_RIGHT_AXIS);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateXzaxisConstraint(CCmdUI* pCmdUI)
{
    if (m_pkUIManager &&
        m_pkUIManager->IsAxisModeAllowed(CNifRenderViewUIState::
            FORWARD_RIGHT_AXIS))
    {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_pkUIManager->GetCurrentAxisMode()
            == CNifRenderViewUIState::FORWARD_RIGHT_AXIS);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnYzaxisConstraint()
{
    SetCurrentAxisMode(CNifRenderViewUIState::FORWARD_UP_AXIS);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateYzaxisConstraint(CCmdUI* pCmdUI)
{
    if (m_pkUIManager &&
        m_pkUIManager->IsAxisModeAllowed(
            CNifRenderViewUIState::FORWARD_UP_AXIS))
    {
        pCmdUI->Enable(TRUE);
        pCmdUI->SetCheck(m_pkUIManager->GetCurrentAxisMode()
            == CNifRenderViewUIState::FORWARD_UP_AXIS);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraNext(CCmdUI* pCmdUI)
{
    if(m_pkUIManager && m_pkUIManager->GetCurrentViewState())
    {
        if(m_pkUIManager->GetCurrentViewState()->
           CanExecuteCommand("CameraNext"))
        {
            pCmdUI->Enable(TRUE);
            return;
        }
    }
    pCmdUI->Enable(FALSE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraPrevious(CCmdUI* pCmdUI)
{
    if(m_pkUIManager && m_pkUIManager->GetCurrentViewState())
    {
        if(m_pkUIManager->GetCurrentViewState()->
           CanExecuteCommand("CameraPrevious"))
        {
            pCmdUI->Enable(TRUE);
            return;
        }
    }
    pCmdUI->Enable(FALSE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateCameraReset(CCmdUI* pCmdUI)
{
    if(m_pkUIManager && m_pkUIManager->GetCurrentViewState())
    {
        if(m_pkUIManager->GetCurrentViewState()->
           CanExecuteCommand("CameraReset"))
        {
            pCmdUI->Enable(TRUE);
            return;
        }
    }
    pCmdUI->Enable(FALSE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateZoomExtentsAll(CCmdUI* pCmdUI)
{
    if(m_pkUIManager && m_pkUIManager->GetCurrentViewState())
    {
        if(m_pkUIManager->GetCurrentViewState()->
            CanExecuteCommand("CameraZoomExtents"))
        {
            pCmdUI->Enable(TRUE);
            return;
        }
    }
    pCmdUI->Enable(FALSE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnReloadShaders()
{
    ((CFrameWnd*) AfxGetMainWnd())->SetMessageText("Reloading shaders...");
    HCURSOR hOldCursor =
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

    CNifDoc* pkDoc = GetDocument();
    pkDoc->Lock();

    NiNode* pkScene = pkDoc->GetSceneGraph();
    NiMaterialHelpers::UnRegisterMaterials(pkScene, m_spRenderer, true);
    NiMaterialToolkit::UnloadShaders();

    // Because the scene may need to be repacked, the renderer data must
    // be purged.  This means that AssetViewer cannot throw any geometry or
    // texture data away during precache!
    if (pkScene)
        m_spRenderer->PurgeAllRendererData(pkScene);

    NiMaterialToolkit::ReloadShaders();
    NiMaterialHelpers::RegisterMaterials(pkScene, m_spRenderer);

    pkDoc->UnLock();

    SetCursor(hOldCursor);
    ((CFrameWnd*) AfxGetMainWnd())->SetMessageText(AFX_IDS_IDLEMESSAGE);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateLodoverrideButton(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CNifRenderViewLODAdjustCommand::IsAdjustEnabled());
}
//---------------------------------------------------------------------------
void CNifRenderView::OnLodoverrideButton()
{
    bool bValue = !CNifRenderViewLODAdjustCommand::IsAdjustEnabled();
    CNifRenderViewLODAdjustCommand::ToggleAdjust(bValue);
}
//---------------------------------------------------------------------------
void CNifRenderView::OnLodupButton()
{
    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewLODAdjustCommand(1);

        m_pkUIManager->QueueCommand(pkCommand);
    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnLoddownButton()
{
    if(m_pkUIManager)
    {
        CNifRenderViewUICommand* pkCommand = new
            CNifRenderViewLODAdjustCommand(-1);

        m_pkUIManager->QueueCommand(pkCommand);

    }
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateLoddownButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CNifRenderViewLODAdjustCommand::IsDecrementEnabled());
}
//---------------------------------------------------------------------------
void CNifRenderView::OnUpdateLodupButton(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(CNifRenderViewLODAdjustCommand::IsIncrementEnabled());
}
//---------------------------------------------------------------------------
