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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Renderer.h"

#include "NiD3D102DBufferData.h"
#include "NiD3D10DataStreamFactory.h"
#include "NiD3D10DeviceState.h"
#include "NiD3D10Direct3DTexture.h"
#include "NiD3D10Direct3DTextureData.h"
#include "NiD3D10Error.h"
#include "NiD3D10ErrorShader.h"
#include "NiD3D10FragmentShader.h"
#include "NiD3D10GPUProgramCache.h"
#include "NiD3D10MeshMaterialBinding.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10PersistentSrcTextureRendererData.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10RenderedTextureData.h"
#include "NiD3D10RendererSDM.h"
#include "NiD3D10ResourceManager.h"
#include "NiD3D10ShaderConstantManager.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10ShaderInterface.h"
#include "NiD3D10ShaderProgramFactory.h"
#include "NiD3D10SourceTextureData.h"
#include "NiD3D10Utility.h"

#include <NiCubeMapDepthStencilBuffer.h>
#include <NiDirectionalShadowWriteMaterial.h>
#include <NiFilename.h>
#include <NiFragmentMaterial.h>
#include <NiMesh.h>
#include <NiPointShadowWriteMaterial.h>
#include <NiRenderCallContext.h>
#include <NiRenderedCubeMap.h>
#include <NiRenderedTexture.h>
#include <NiShadowGenerator.h>
#include <NiShadowManager.h>
#include <NiSingleShaderMaterial.h>
#include <NiSourceCubeMap.h>
#include <NiSourceTexture.h>
#include <NiSpotShadowWriteMaterial.h>
#include <NiStandardMaterial.h>
#include <NiVersion.h>
#include <NiVertexColorProperty.h>

// for _chdir
#include <direct.h>

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiD3D10);
//---------------------------------------------------------------------------

HINSTANCE NiD3D10Renderer::ms_hD3D10 = NULL;

NILPD3D10CREATEBLOB NiD3D10Renderer::ms_pfnD3D10CreateBlob = NULL;
NILPD3D10CREATEDEVICE NiD3D10Renderer::ms_pfnD3D10CreateDevice = NULL;
NILPD3D10CREATEDEVICEANDSWAPCHAIN 
    NiD3D10Renderer::ms_pfnD3D10CreateDeviceAndSwapChain = NULL;
NILPD3D10CREATESTATEBLOCK NiD3D10Renderer::ms_pfnD3D10CreateStateBlock = NULL;
NILPD3D10GETINPUTANDOUTPUTSIGNATUREBLOB 
    NiD3D10Renderer::ms_pfnD3D10GetInputAndOutputSignatureBlob = NULL;
NILPD3D10GETINPUTSIGNATUREBLOB 
    NiD3D10Renderer::ms_pfnD3D10GetInputSignatureBlob = NULL;
NILPD3D10GETOUTPUTSIGNATUREBLOB 
    NiD3D10Renderer::ms_pfnD3D10GetOutputSignatureBlob = NULL;
NILPD3D10STATEBLOCKMASKDIFFERENCE 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskDifference = NULL;
NILPD3D10STATEBLOCKMASKDISABLEALL 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskDisableAll = NULL;
NILPD3D10STATEBLOCKMASKDISABLECAPTURE 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskDisableCapture = NULL;
NILPD3D10STATEBLOCKMASKENABLEALL 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskEnableAll = NULL;
NILPD3D10STATEBLOCKMASKENABLECAPTURE 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskEnableCapture = NULL;
NILPD3D10STATEBLOCKMASKGETSETTING 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskGetSetting = NULL;
NILPD3D10STATEBLOCKMASKINTERSECT 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskIntersect = NULL;
NILPD3D10STATEBLOCKMASKUNION 
    NiD3D10Renderer::ms_pfnD3D10StateBlockMaskUnion = NULL;

NILPD3D10COMPILESHADER NiD3D10Renderer::ms_pfnD3D10CompileShader = NULL;
NILPD3D10DISASSEMBLESHADER NiD3D10Renderer::ms_pfnD3D10DisassembleShader = 
    NULL;
NILPD3D10GETGEOMETRYSHADERPROFILE 
    NiD3D10Renderer::ms_pfnD3D10GetGeometryShaderProfile = NULL;
NILPD3D10GETPIXELSHADERPROFILE 
    NiD3D10Renderer::ms_pfnD3D10GetPixelShaderProfile = NULL;
NILPD3D10GETVERTEXSHADERPROFILE 
    NiD3D10Renderer::ms_pfnD3D10GetVertexShaderProfile = NULL;
NILPD3D10PREPROCESSSHADER 
    NiD3D10Renderer::ms_pfnD3D10PreprocessShader = NULL;
NILPD3D10REFLECTSHADER 
    NiD3D10Renderer::ms_pfnD3D10ReflectShader = NULL;

NILPD3D10COMPILEEFFECTFROMMEMORY 
    NiD3D10Renderer::ms_pfnD3D10CompileEffectFromMemory = NULL;
NILPD3D10CREATEEFFECTFROMMEMORY 
    NiD3D10Renderer::ms_pfnD3D10CreateEffectFromMemory = NULL;
NILPD3D10CREATEEFFECTPOOLFROMMEMORY 
    NiD3D10Renderer::ms_pfnD3D10CreateEffectPoolFromMemory = NULL;
NILPD3D10DISASSEMBLEEFFECT NiD3D10Renderer::ms_pfnD3D10DisassembleEffect = 
    NULL;

NiCriticalSection NiD3D10Renderer::ms_kD3D10LibraryCriticalSection;

static NiD3D10RendererSDM NiD3D10RendererSDMObject;

NiImplementRTTI(NiD3D10Renderer,NiRenderer);

NiAllocatorDeclareStatics(
    NiD3D10Renderer::PrePackMesh, 
    sizeof(NiD3D10Renderer::PrePackMesh) * 16);

NiAllocatorDeclareStatics(
    NiD3D10Renderer::PrePackTexture, 
    sizeof(NiD3D10Renderer::PrePackTexture) * 16);

//---------------------------------------------------------------------------
NiD3D10Renderer::CreationParameters::CreationParameters() :
    m_uiAdapterIndex(0),
    m_uiOutputIndex(0),
    m_eDriverType(DRIVER_HARDWARE),
#if defined(NIDEBUG)
    m_uiCreateFlags(CREATE_DEVICE_SINGLETHREADED | 
        CREATE_DEVICE_DEBUG),
#else //#if defined(NIDEBUG)
    m_uiCreateFlags(CREATE_DEVICE_SINGLETHREADED),
#endif //#if defined(NIDEBUG)
    m_bCreateSwapChain(false),
    m_bCreateDepthStencilBuffer(true),
    m_bAssociateWithWindow(false),
    m_uiWindowAssociationFlags(0),
    m_eDepthStencilFormat(DXGI_FORMAT_UNKNOWN)
{
    m_kSwapChain.BufferDesc.Width = 640;
    m_kSwapChain.BufferDesc.Height= 480;
    m_kSwapChain.BufferDesc.RefreshRate.Numerator = 60;
    m_kSwapChain.BufferDesc.RefreshRate.Denominator = 1;
    m_kSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_kSwapChain.BufferDesc.ScanlineOrdering = 
        DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    m_kSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_kSwapChain.SampleDesc.Count = 1;
    m_kSwapChain.SampleDesc.Quality = 0;
    m_kSwapChain.BufferUsage = DXGI_USAGE_BACK_BUFFER | 
        DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_kSwapChain.BufferCount = 1;
    m_kSwapChain.OutputWindow = NULL;
    m_kSwapChain.Windowed = false;
    m_kSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    m_kSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
}
//---------------------------------------------------------------------------
NiD3D10Renderer::CreationParameters::CreationParameters(HWND hWnd) :
    m_uiAdapterIndex(0),
    m_uiOutputIndex(0),
    m_eDriverType(DRIVER_HARDWARE),
#if defined(NIDEBUG)
    m_uiCreateFlags(CREATE_DEVICE_SINGLETHREADED | 
        CREATE_DEVICE_DEBUG),
#else //#if defined(NIDEBUG)
    m_uiCreateFlags(CREATE_DEVICE_SINGLETHREADED),
#endif //#if defined(NIDEBUG)
    m_bCreateSwapChain(true),
    m_bCreateDepthStencilBuffer(true),
    m_bAssociateWithWindow(true),
    m_uiWindowAssociationFlags(0),
    m_eDepthStencilFormat(DXGI_FORMAT_UNKNOWN)
{
    m_kSwapChain.BufferDesc.Width = 640;
    m_kSwapChain.BufferDesc.Height = 480;
    m_kSwapChain.BufferDesc.RefreshRate.Numerator = 60;
    m_kSwapChain.BufferDesc.RefreshRate.Denominator = 1;
    m_kSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_kSwapChain.BufferDesc.ScanlineOrdering = 
        DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    m_kSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_kSwapChain.SampleDesc.Count = 1;
    m_kSwapChain.SampleDesc.Quality = 0;
    m_kSwapChain.BufferUsage = DXGI_USAGE_BACK_BUFFER | 
        DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_kSwapChain.BufferCount = 1;
    m_kSwapChain.OutputWindow = NULL;
    m_kSwapChain.Windowed = false;
    m_kSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    m_kSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    if (hWnd)
    {
        m_kSwapChain.OutputWindow = hWnd;
        m_kSwapChain.Windowed = true;
        RECT kRect;
        ::GetClientRect(hWnd, &kRect);

        m_kSwapChain.BufferDesc.Width = kRect.right - kRect.left;
        m_kSwapChain.BufferDesc.Height = kRect.bottom - kRect.top;
    }
}
//---------------------------------------------------------------------------
NiD3D10Renderer::NiD3D10Renderer() :
    m_ulDeviceThreadID(0),
    m_pkD3D10Device(NULL),
    m_bInitialized(false),
    m_pkCurrentRenderTargetGroup(NULL),
    m_fDepthClear(1.0f),
    m_ucStencilClear(0),
    m_bLeftRightSwap(false),
    m_kCurrentFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
    m_kCurrentViewPort(0.0f, 1.0f, 1.0f, 0.0f),
    m_uiSyncInterval(1),
    m_fMaxFogFactor(0.0f),
    m_fMaxFogValue(1.0f),
    m_pkDataStreamFactory(NULL),
    m_pkDeviceState(NULL),
    m_pkRenderStateManager(NULL),
    m_pkResourceManager(NULL),
    m_pkShaderConstantManager(NULL),
    m_uiMaxVertexShaderVersion(0),
    m_uiMaxGeometryShaderVersion(0),
    m_uiMaxPixelShaderVersion(0),
    m_pkTempVertices(NULL),
    m_pkTempColors(NULL),
    m_pkTempTexCoords(NULL),
    m_uiTempArraySize(0),
    m_bDeviceOccluded(false),
    m_bDeviceRemoved(false),
    m_pkD3D10MacroBuffer(NULL),
    m_uiD3D10MacroBufferLen(0) ,
    m_bSupportsMMX(false),
    m_bSupportsSSE(false),
    m_bSupportsSSE2(false)
{
    m_uiAdapterIndex = (unsigned int)NULL_ADAPTER;
    m_usMaxAnisotropy = HW_MAX_ANISOTROPY;

    memset(m_afBackgroundColor, 0, sizeof(m_afBackgroundColor));
    memset(m_auiFormatSupport, 0, sizeof(m_auiFormatSupport));

    D3DXMatrixIdentity(&m_kD3DView);
    D3DXMatrixIdentity(&m_kInvView);
    D3DXMatrixIdentity(&m_kD3DProj);
    D3DXMatrixIdentity(&m_kD3DModel);
}
//---------------------------------------------------------------------------
NiD3D10Renderer::~NiD3D10Renderer()
{
//CODEBLOCK(1) - DO NOT DELETE THIS LINE

    NiMaterial::UnloadShadersForAllMaterials();

    if (m_spInitialDefaultMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spInitialDefaultMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial, 
            m_spInitialDefaultMaterial)->SetCachedShader(NULL);
    }
    if (m_spCurrentDefaultMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spCurrentDefaultMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial, 
            m_spCurrentDefaultMaterial)->SetCachedShader(NULL);
    }

    m_spBatchMaterial = 0;

    NiDelete[] m_pkTempVertices;
    NiDelete[] m_pkTempColors;
    NiDelete[] m_pkTempTexCoords;

    // Purge swap chains
    NiTListIterator kIter = m_kSwapChainRenderTargetGroups.GetFirstPos();
    while (kIter)
    {
        HWND hWnd;
        NiRenderTargetGroupPtr spRenderTargetGroup;

        m_kSwapChainRenderTargetGroups.GetNext(kIter, hWnd, 
            spRenderTargetGroup);
        if (spRenderTargetGroup)
        {
            for (unsigned int i = 0; 
                i < spRenderTargetGroup->GetBufferCount(); i++)
            {
                Ni2DBuffer* pkBuffer = spRenderTargetGroup->GetBuffer(i);
                if (pkBuffer)
                    pkBuffer->SetRendererData(NULL);
            }

            Ni2DBuffer* pkBuffer = 
                spRenderTargetGroup->GetDepthStencilBuffer();
            if (pkBuffer)
                pkBuffer->SetRendererData(NULL);
        }
    }
    m_kSwapChainRenderTargetGroups.RemoveAll();

    NiTListIterator kPrepackIter = m_kPrePackMeshes.GetHeadPos();
    while (kPrepackIter)
    {
        PrePackMesh* pkObject = m_kPrePackMeshes.GetNext(kPrepackIter);
        pkObject->m_spMesh = NULL;
    }
    m_kPrePackMeshes.RemoveAll();

    kPrepackIter = m_kPrePackTextures.GetHeadPos();
    while (kPrepackIter)
    {
        PrePackTexture* pkObject = m_kPrePackTextures.GetNext(kPrepackIter);
        pkObject->m_spTexture = NULL;
        pkObject->m_spSrcPixelData = NULL;
    }
    m_kPrePackTextures.RemoveAll();

    // Purge textures
    PurgeAllTextures(true);

    // Purge the D3D10Shaders
    PurgeAllD3D10Shaders();

    // Release all D3D10Shaders loaded by the shader factory.
    NiD3D10ShaderFactory* pkShaderFactory = 
        NiD3D10ShaderFactory::GetD3D10ShaderFactory();
    if (pkShaderFactory)
    {
        pkShaderFactory->RemoveAllShaders();
        pkShaderFactory->RemoveAllLibraries();
    }

    ReleaseManagers();
    ReleaseResources();
    ReleaseDevice();

    NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
    NiPersistentSrcTextureRendererData::ResetStreamingFunctions();

    NiFree(m_pkD3D10MacroBuffer);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::_SDMShutdown()
{
    ReleaseD3D10();

    NiAllocatorShutdown(NiD3D10Renderer::PrePackMesh);
    NiAllocatorShutdown(NiD3D10Renderer::PrePackTexture);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Create(CreationParameters& kCreate, 
    NiD3D10RendererPtr& spRenderer)
{
    spRenderer = NULL;

//CODEBLOCK(2) - DO NOT DELETE THIS LINE

    spRenderer = NiNew NiD3D10Renderer;
    NIASSERT(spRenderer);
    bool bSuccess = spRenderer->Initialize(kCreate);
    
    if (bSuccess == false)
    {
        spRenderer = NULL;
        return false;
    }

    NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
    NiStream::RegisterLoader("NiPersistentSrcTextureRendererData", 
        NiD3D10PersistentSrcTextureRendererData::CreateObject);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ResizeBuffers(unsigned int uiWidth, 
    unsigned int uiHeight, HWND hOutputWnd)
{
    NiRenderTargetGroup* pkRTG = m_spDefaultRenderTargetGroup;
    if (hOutputWnd)
        pkRTG = GetSwapChainRenderTargetGroup(hOutputWnd);

    if (pkRTG == NULL)
        return false;

    NiD3D10SwapChainBufferData* pkSwapChainBufferData =
        (NiD3D10SwapChainBufferData*)pkRTG->GetBufferRendererData(0);

    if (pkSwapChainBufferData == NULL)
        return false;
    
    // Release depth/stencil buffer
    bool bDSBuffer = pkRTG->HasDepthStencil();
    const NiPixelFormat* pkDSFormat = pkRTG->GetDepthStencilPixelFormat();

    pkRTG->AttachDepthStencilBuffer(NULL);

    bool bSuccess = pkSwapChainBufferData->ResizeSwapChain(uiWidth, uiHeight);
    if (!bSuccess)
    {
        // Don't return here - still need to recreate DS buffer.
    }

    // Recreate depth/stencil buffer
    if (bDSBuffer)
    {
        NiDepthStencilBufferPtr spDepthBuffer = NiDepthStencilBuffer::Create(
            pkSwapChainBufferData->GetWidth(), 
            pkSwapChainBufferData->GetHeight(), this, *pkDSFormat,
            pkSwapChainBufferData->GetMSAAPref());
        pkRTG->AttachDepthStencilBuffer(spDepthBuffer);
    }

//CODEBLOCK(3) - DO NOT DELETE THIS LINE

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Initialize(CreationParameters& kCreate)
{
    if (m_bInitialized)
    {
        NiD3D10Error::ReportWarning("Attempting to initialize renderer "
            "that is already initialized.");
        return false;
    }

    if (CreateDevice(kCreate))
    {
        CreateManagers();

        if (kCreate.m_bCreateSwapChain == false)
        {
            m_bInitialized = true;
        }
        else
        {
            IDXGISwapChain* pkSwapChain = 
                CreateSwapChain(kCreate.m_kSwapChain, kCreate.m_uiOutputIndex);
            if (pkSwapChain != NULL)
            {
                NiRenderTargetGroup* pkRTGroup = 
                    CreateRenderTargetGroupFromSwapChain(pkSwapChain, 
                    kCreate.m_bCreateDepthStencilBuffer,
                    kCreate.m_eDepthStencilFormat);
                if (pkRTGroup)
                {
                    m_spDefaultRenderTargetGroup = pkRTGroup;

                    m_kSwapChainRenderTargetGroups.SetAt(
                        kCreate.m_kSwapChain.OutputWindow, pkRTGroup);

                    m_bInitialized = true;

                }
                else
                {
                    NiD3D10Error::ReportWarning("Initialization failed "
                        "because NiRenderTargetGroup for default swap chain "
                        "could not be created; destroying D3D10 Device.");
                }
            }
            else
            {
                NiD3D10Error::ReportWarning("Initialization failed because "
                    "default swap chain could not be created; "
                    "destroying D3D10 Device.");
            }
        }
    }

    if (m_bInitialized == false)
    {
        // Initialization failed
        ReleaseDevice();

        return false;
    }

    if (kCreate.m_bAssociateWithWindow)
    {
        IDXGIFactory* pkFactory = m_spSystemDesc->GetFactory();
        NIASSERT(pkFactory);

        pkFactory->MakeWindowAssociation(
            kCreate.m_kSwapChain.OutputWindow,
            GetD3D10WindowAssociationFlags(
            kCreate.m_uiWindowAssociationFlags));
    }

    // Initialize shader system
    NIASSERT(NiD3D10ShaderFactory::GetD3D10ShaderFactory());
    NIASSERT(NiD3D10ShaderProgramFactory::GetInstance());

    NiD3D10ShaderFactory* pkShaderFactory = 
        NiD3D10ShaderFactory::GetD3D10ShaderFactory();

    pkShaderFactory->SetAsActiveFactory();

    // Setup the default shader version information
    m_uiMaxVertexShaderVersion = 
        pkShaderFactory->CreateVertexShaderVersion(4, 0);
    m_uiMaxGeometryShaderVersion = 
        pkShaderFactory->CreateGeometryShaderVersion(4, 0);
    m_uiMaxPixelShaderVersion = 
        pkShaderFactory->CreatePixelShaderVersion(4, 0);

    // Inform the shadowing system of the shader model versions used by the 
    // current hardware.
    if (NiShadowManager::GetShadowManager())
    {
        NiShadowManager::ValidateShaderVersions(
            (unsigned short)pkShaderFactory->GetMajorVertexShaderVersion(
            m_uiMaxVertexShaderVersion),
            (unsigned short)pkShaderFactory->GetMinorVertexShaderVersion(
            m_uiMaxVertexShaderVersion),
            (unsigned short)pkShaderFactory->GetMajorVertexShaderVersion(
            m_uiMaxGeometryShaderVersion), 
            (unsigned short)pkShaderFactory->GetMinorVertexShaderVersion(
            m_uiMaxGeometryShaderVersion),
            (unsigned short)pkShaderFactory->GetMajorVertexShaderVersion(
            m_uiMaxPixelShaderVersion),
            (unsigned short)pkShaderFactory->GetMinorVertexShaderVersion(
            m_uiMaxPixelShaderVersion));
    }

    NiStandardMaterial* pkStandardMaterial = NiDynamicCast(
        NiStandardMaterial, m_spInitialDefaultMaterial);
    if (pkStandardMaterial)
    {
        NiRenderer::SetDefaultProgramCache(pkStandardMaterial);
    }
    m_spCurrentDefaultMaterial = m_spInitialDefaultMaterial;

    // Shadow manager
    if (NiShadowManager::GetShadowManager())
    {
        NiDirectionalShadowWriteMaterial* pkDirShadowWriteMaterial = 
            (NiDirectionalShadowWriteMaterial*)NiShadowManager::
            GetShadowWriteMaterial(NiStandardMaterial::LIGHT_DIR);

        if (pkDirShadowWriteMaterial)
        {
            NiRenderer::SetDefaultProgramCache(pkDirShadowWriteMaterial);
        }

        NiPointShadowWriteMaterial* pkPointShadowWriteMaterial = 
            (NiPointShadowWriteMaterial*)NiShadowManager::
            GetShadowWriteMaterial(NiStandardMaterial::LIGHT_POINT);

        if (pkPointShadowWriteMaterial)
        {
            NiRenderer::SetDefaultProgramCache(pkPointShadowWriteMaterial);
        }

        NiSpotShadowWriteMaterial* pkSpotShadowWriteMaterial = 
            (NiSpotShadowWriteMaterial*)NiShadowManager::
            GetShadowWriteMaterial(NiStandardMaterial::LIGHT_SPOT);

        if (pkSpotShadowWriteMaterial)
        {
            NiRenderer::SetDefaultProgramCache(pkSpotShadowWriteMaterial);
        }
    }

    m_kShaderLibraryVersion.SetSystemVertexShaderVersion(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetMinVertexShaderVersion(1, 1);
    m_kShaderLibraryVersion.SetVertexShaderVersionRequest(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetSystemGeometryShaderVersion(
        m_uiMaxGeometryShaderVersion);
    m_kShaderLibraryVersion.SetMinGeometryShaderVersion(4, 0);
    m_kShaderLibraryVersion.SetGeometryShaderVersionRequest(
        m_uiMaxGeometryShaderVersion);
    m_kShaderLibraryVersion.SetSystemPixelShaderVersion(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetMinPixelShaderVersion(1, 1);
    m_kShaderLibraryVersion.SetPixelShaderVersionRequest(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetSystemUserVersion(0, 0);
    m_kShaderLibraryVersion.SetMinUserVersion(0, 0);
    m_kShaderLibraryVersion.SetUserVersionRequest(0, 0);
    m_kShaderLibraryVersion.SetPlatform(NiShader::NISHADER_D3D10);
    
    NiD3D10ShaderFactory::GetD3D10ShaderFactory();

    NiShader* pkErrorShader = NiNew NiD3D10ErrorShader;
    pkErrorShader->Initialize();

    SetErrorShader(pkErrorShader);

    // Initialize the safe zone to the inner 98% of the display
    m_kDisplaySafeZone.m_top =  0.01f;
    m_kDisplaySafeZone.m_left =  0.01f;
    m_kDisplaySafeZone.m_right =  0.99f;
    m_kDisplaySafeZone.m_bottom =  0.99f;

    m_bDeviceOccluded = false;
    m_bDeviceRemoved = false;

    return m_bInitialized;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateSwapChainRenderTargetGroup(
    DXGI_SWAP_CHAIN_DESC& kSwapChainDesc, unsigned int uiOutputIndex, 
    bool bCreateDepthStencilBuffer, DXGI_FORMAT eDepthStencilFormat)
{
    // Is there an output window?
    if (kSwapChainDesc.OutputWindow == NULL)
    {
        return false;
    }

    // Does a swap chain already exist for this window?
    NiRenderTargetGroupPtr spRenderTargetGroup;
    if (m_kSwapChainRenderTargetGroups.GetAt(kSwapChainDesc.OutputWindow, 
        spRenderTargetGroup))
    {
        return false;
    }

    IDXGISwapChain* pkSwapChain = 
        CreateSwapChain(kSwapChainDesc, uiOutputIndex);
    if (pkSwapChain != NULL)
    {
        NiRenderTargetGroup* pkRTGroup = 
            CreateRenderTargetGroupFromSwapChain(pkSwapChain, 
            bCreateDepthStencilBuffer, eDepthStencilFormat);

        m_kSwapChainRenderTargetGroups.SetAt(
            kSwapChainDesc.OutputWindow, pkRTGroup);

        return true;
    }


    return false;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::DestroySwapChainRenderTargetGroup(HWND hWnd)
{
    // Does the framebuffer exist?
    if (!hWnd)
    {
        return;
    }

    NiRenderTargetGroupPtr spRenderTargetGroup = NULL;

    // Does the framebuffer exist?
    if (!m_kSwapChainRenderTargetGroups.GetAt(hWnd, spRenderTargetGroup))
    {
        return;
    }

    // can only delete non-primary buffers that are not current targets
    if (spRenderTargetGroup != m_pkCurrentRenderTargetGroup)
    {
        m_kSwapChainRenderTargetGroups.RemoveAt(hWnd);

        // Check to see if this is the default render target group
        if (spRenderTargetGroup == m_spDefaultRenderTargetGroup)
        {
            // Try to find another render target group to use as the default
            NiRenderTargetGroupPtr spNewDefaultRenderTargetGroup;
            if (!m_kSwapChainRenderTargetGroups.IsEmpty())
            {
                NiTMapIterator kIter = 
                    m_kSwapChainRenderTargetGroups.GetFirstPos();
                HWND hNewWnd = NULL;
                m_kSwapChainRenderTargetGroups.GetNext(kIter, hNewWnd, 
                    spNewDefaultRenderTargetGroup);
            }

            m_spDefaultRenderTargetGroup = spNewDefaultRenderTargetGroup;
        }

        spRenderTargetGroup = NULL;
    }
    else
    {
        /* */
    }
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiD3D10Renderer::GetSwapChainRenderTargetGroup(HWND hWnd)
    const
{
    if (!hWnd)
    {
        return NULL;
    }

    NiRenderTargetGroupPtr spRenderTargetGroup;

    // Does the framebuffer exist?
    if (m_kSwapChainRenderTargetGroups.GetAt(hWnd, spRenderTargetGroup))
        return spRenderTargetGroup;

    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SetDefaultSwapChainRenderTargetGroup(HWND hWnd)
{
    NiRenderTargetGroup* pkRTG = GetSwapChainRenderTargetGroup(hWnd);
    if (pkRTG == NULL)
        return false;

    m_spDefaultRenderTargetGroup = pkRTG;
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::EnsureD3D10Loaded()
{
    ms_kD3D10LibraryCriticalSection.Lock();

    bool bSuccess = true;
    if (ms_hD3D10 == NULL)
        bSuccess = LoadD3D10();

    ms_kD3D10LibraryCriticalSection.Unlock();

    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::LoadD3D10()
{
    ms_kD3D10LibraryCriticalSection.Lock();

    if (ms_hD3D10 != NULL)
    {
        ms_kD3D10LibraryCriticalSection.Unlock();
        return true;
    }

    ms_hD3D10 = LoadLibrary("D3D10.dll");

    if (ms_hD3D10 == NULL)
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NID3D10ERROR_D3D10_LIB_MISSING);
        ms_kD3D10LibraryCriticalSection.Unlock();
        return false;
    }

    ms_pfnD3D10CreateBlob = (NILPD3D10CREATEBLOB)
        GetProcAddress(ms_hD3D10, "D3D10CreateBlob");
    ms_pfnD3D10CreateDevice = (NILPD3D10CREATEDEVICE)
        GetProcAddress(ms_hD3D10, "D3D10CreateDevice");
    ms_pfnD3D10CreateDeviceAndSwapChain = (NILPD3D10CREATEDEVICEANDSWAPCHAIN)
        GetProcAddress(ms_hD3D10, "D3D10CreateDeviceAndSwapChain");
    ms_pfnD3D10CreateStateBlock = (NILPD3D10CREATESTATEBLOCK)
        GetProcAddress(ms_hD3D10, "D3D10CreateStateBlock");
    ms_pfnD3D10GetInputAndOutputSignatureBlob = 
        (NILPD3D10GETINPUTANDOUTPUTSIGNATUREBLOB)
        GetProcAddress(ms_hD3D10, "D3D10GetInputAndOutputSignatureBlob");
    ms_pfnD3D10GetInputSignatureBlob = (NILPD3D10GETINPUTSIGNATUREBLOB)
        GetProcAddress(ms_hD3D10, "D3D10GetInputSignatureBlob");
    ms_pfnD3D10GetOutputSignatureBlob = (NILPD3D10GETOUTPUTSIGNATUREBLOB)
        GetProcAddress(ms_hD3D10, "D3D10GetOutputSignatureBlob");
    ms_pfnD3D10StateBlockMaskDifference = (NILPD3D10STATEBLOCKMASKDIFFERENCE)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskDifference");
    ms_pfnD3D10StateBlockMaskDisableAll = (NILPD3D10STATEBLOCKMASKDISABLEALL)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskDisableAll");
    ms_pfnD3D10StateBlockMaskDisableCapture = 
        (NILPD3D10STATEBLOCKMASKDISABLECAPTURE)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskDisableCapture");
    ms_pfnD3D10StateBlockMaskEnableAll = (NILPD3D10STATEBLOCKMASKENABLEALL)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskEnableAll");
    ms_pfnD3D10StateBlockMaskEnableCapture = 
        (NILPD3D10STATEBLOCKMASKENABLECAPTURE)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskEnableCapture");
    ms_pfnD3D10StateBlockMaskGetSetting = (NILPD3D10STATEBLOCKMASKGETSETTING)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskGetSetting");
    ms_pfnD3D10StateBlockMaskIntersect = (NILPD3D10STATEBLOCKMASKINTERSECT)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskIntersect");
    ms_pfnD3D10StateBlockMaskUnion = (NILPD3D10STATEBLOCKMASKUNION)
        GetProcAddress(ms_hD3D10, "D3D10StateBlockMaskUnion");

    ms_pfnD3D10CompileShader = (NILPD3D10COMPILESHADER)
        GetProcAddress(ms_hD3D10, "D3D10CompileShader");
    ms_pfnD3D10DisassembleShader = (NILPD3D10DISASSEMBLESHADER)
        GetProcAddress(ms_hD3D10, "D3D10DisassembleShader");
    ms_pfnD3D10GetGeometryShaderProfile = (NILPD3D10GETGEOMETRYSHADERPROFILE)
        GetProcAddress(ms_hD3D10, "D3D10GetGeometryShaderProfile");
    ms_pfnD3D10GetPixelShaderProfile = (NILPD3D10GETPIXELSHADERPROFILE)
        GetProcAddress(ms_hD3D10, "D3D10GetPixelShaderProfile");
    ms_pfnD3D10GetVertexShaderProfile = (NILPD3D10GETVERTEXSHADERPROFILE)
        GetProcAddress(ms_hD3D10, "D3D10GetVertexShaderProfile");
    ms_pfnD3D10PreprocessShader = (NILPD3D10PREPROCESSSHADER)
        GetProcAddress(ms_hD3D10, "D3D10PreprocessShader");
    ms_pfnD3D10ReflectShader = (NILPD3D10REFLECTSHADER)
        GetProcAddress(ms_hD3D10, "D3D10ReflectShader");

    ms_pfnD3D10CompileEffectFromMemory = (NILPD3D10COMPILEEFFECTFROMMEMORY)
        GetProcAddress(ms_hD3D10, "D3D10CompileEffectFromMemory");
    ms_pfnD3D10CreateEffectFromMemory = (NILPD3D10CREATEEFFECTFROMMEMORY)
        GetProcAddress(ms_hD3D10, "D3D10CreateEffectFromMemory");
    ms_pfnD3D10CreateEffectPoolFromMemory = 
        (NILPD3D10CREATEEFFECTPOOLFROMMEMORY)
        GetProcAddress(ms_hD3D10, "D3D10CreateEffectPoolFromMemory");
    ms_pfnD3D10DisassembleEffect = (NILPD3D10DISASSEMBLEEFFECT)
        GetProcAddress(ms_hD3D10, "D3D10DisassembleEffect");

    if (ms_pfnD3D10CreateBlob == NULL ||
        ms_pfnD3D10CreateDevice == NULL ||
        ms_pfnD3D10CreateDeviceAndSwapChain == NULL ||
        ms_pfnD3D10CreateStateBlock == NULL ||
        ms_pfnD3D10GetInputAndOutputSignatureBlob == NULL ||
        ms_pfnD3D10GetInputSignatureBlob == NULL ||
        ms_pfnD3D10GetOutputSignatureBlob == NULL ||
        ms_pfnD3D10StateBlockMaskDifference == NULL ||
        ms_pfnD3D10StateBlockMaskDisableAll == NULL ||
        ms_pfnD3D10StateBlockMaskDisableCapture == NULL ||
        ms_pfnD3D10StateBlockMaskEnableAll == NULL ||
        ms_pfnD3D10StateBlockMaskEnableCapture == NULL ||
        ms_pfnD3D10StateBlockMaskGetSetting == NULL ||
        ms_pfnD3D10StateBlockMaskIntersect == NULL ||
        ms_pfnD3D10StateBlockMaskUnion == NULL ||
        ms_pfnD3D10CompileShader == NULL ||
        ms_pfnD3D10DisassembleShader == NULL ||
        ms_pfnD3D10GetGeometryShaderProfile == NULL ||
        ms_pfnD3D10GetPixelShaderProfile == NULL ||
        ms_pfnD3D10GetVertexShaderProfile == NULL ||
        ms_pfnD3D10PreprocessShader == NULL ||
        ms_pfnD3D10ReflectShader == NULL ||
        ms_pfnD3D10CompileEffectFromMemory == NULL ||
        ms_pfnD3D10CreateEffectFromMemory == NULL ||
        ms_pfnD3D10CreateEffectPoolFromMemory == NULL ||
        ms_pfnD3D10DisassembleEffect == NULL)
    {
        NiD3D10Error::ReportError(NiD3D10Error::NID3D10ERROR_D3D10_LIB_MISSING,
            "Library loaded but some procedure addresses not found; "
            "releasing library.");

        ReleaseD3D10();
        ms_kD3D10LibraryCriticalSection.Unlock();

        return false;
    }

    ms_kD3D10LibraryCriticalSection.Unlock();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ReleaseD3D10()
{
    ms_kD3D10LibraryCriticalSection.Lock();

    if (ms_hD3D10)
    {
        FreeLibrary(ms_hD3D10);
        ms_hD3D10 = NULL;
        ms_pfnD3D10CreateBlob = NULL;
        ms_pfnD3D10CreateDevice = NULL;
        ms_pfnD3D10CreateDeviceAndSwapChain = NULL;
        ms_pfnD3D10CreateStateBlock = NULL;
        ms_pfnD3D10GetInputAndOutputSignatureBlob = NULL;
        ms_pfnD3D10GetInputSignatureBlob = NULL;
        ms_pfnD3D10GetOutputSignatureBlob = NULL;
        ms_pfnD3D10StateBlockMaskDifference = NULL;
        ms_pfnD3D10StateBlockMaskDisableAll = NULL;
        ms_pfnD3D10StateBlockMaskDisableCapture = NULL;
        ms_pfnD3D10StateBlockMaskEnableAll = NULL;
        ms_pfnD3D10StateBlockMaskEnableCapture = NULL;
        ms_pfnD3D10StateBlockMaskGetSetting = NULL;
        ms_pfnD3D10StateBlockMaskIntersect = NULL;
        ms_pfnD3D10StateBlockMaskUnion = NULL;
        ms_pfnD3D10CompileShader = NULL;
        ms_pfnD3D10DisassembleShader = NULL;
        ms_pfnD3D10GetGeometryShaderProfile = NULL;
        ms_pfnD3D10GetPixelShaderProfile = NULL;
        ms_pfnD3D10GetVertexShaderProfile = NULL;
        ms_pfnD3D10PreprocessShader = NULL;
        ms_pfnD3D10ReflectShader = NULL;
        ms_pfnD3D10CompileEffectFromMemory = NULL;
        ms_pfnD3D10CreateEffectFromMemory = NULL;
        ms_pfnD3D10CreateEffectPoolFromMemory = NULL;
        ms_pfnD3D10DisassembleEffect = NULL;
    }

    ms_kD3D10LibraryCriticalSection.Unlock();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDevice(CreationParameters& kCreate)
{
    // SwitchToRef not compatible with multisampled resources - disable if
    // MSAA enabled.
    if ((kCreate.m_kSwapChain.SampleDesc.Count != 1 || 
        kCreate.m_kSwapChain.SampleDesc.Quality != 0) &&
        (kCreate.m_uiCreateFlags & CREATE_DEVICE_SWITCH_TO_REF) != 0)
    {
        NiD3D10Error::ReportWarning("SwitchToRef device not compatible with "
            "multisampled resources; "
            "disabling request for SwitchToRef device.");
        kCreate.m_uiCreateFlags &= ~CREATE_DEVICE_SWITCH_TO_REF;
    }

    m_kInitialCreateParameters = kCreate;

    NiD3D10SystemDesc::GetSystemDesc(m_spSystemDesc);
    if (m_spSystemDesc->IsEnumerationValid() == false)
    {
        if (m_spSystemDesc->Enumerate() == false)
        {
            NiD3D10Error::ReportWarning("System enumeration failed; "
                "device cannot be created.");
            return false;
        }
    }

    //get instruction set support
    m_bSupportsMMX = NiSystemDesc::GetSystemDesc().MMX_Supported();
    m_bSupportsSSE = NiSystemDesc::GetSystemDesc().SSE_Supported();
    m_bSupportsSSE2 = NiSystemDesc::GetSystemDesc().SSE2_Supported();

    unsigned int uiAdapterID = kCreate.m_uiAdapterIndex;
    if (uiAdapterID == NULL_ADAPTER)
        kCreate.m_eDriverType = DRIVER_REFERENCE;
    else if (uiAdapterID >= m_spSystemDesc->GetAdapterCount())
        uiAdapterID = 0;

    const NiD3D10AdapterDesc* pkAdapterDesc = 
        m_spSystemDesc->GetAdapterDesc(uiAdapterID);
    NIASSERT(pkAdapterDesc);

    IDXGIAdapter* pkAdapter = NULL;
    if (kCreate.m_eDriverType == DRIVER_REFERENCE && 
        !pkAdapterDesc->IsPerfHUD())
    {
        uiAdapterID = (unsigned int) NULL_ADAPTER;
    }
    else
    {
        pkAdapter = pkAdapterDesc->GetAdapter();
        NIASSERT(pkAdapter);
    }

    ID3D10Device* pkD3DDevice = NULL;
    HRESULT hr = S_OK;
    ms_kD3D10LibraryCriticalSection.Lock();
    if (EnsureD3D10Loaded())
    {
        hr = D3D10CreateDevice(pkAdapter, 
            NiD3D10Renderer::GetD3D10DriverType(kCreate.m_eDriverType), 
            NULL, kCreate.m_uiCreateFlags, D3D10_SDK_VERSION, &pkD3DDevice);
    }
    else
    {
        NiD3D10Error::ReportWarning("D3D10 library failed loading; "
            "device cannot be created.");
        ms_kD3D10LibraryCriticalSection.Unlock();
        return false;
    }
    m_ulDeviceThreadID = NiGetCurrentThreadId();

    ms_kD3D10LibraryCriticalSection.Unlock();

    if (FAILED(hr) || pkD3DDevice == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                "No error message from D3D10, but device is NULL.");
        }

        if (pkD3DDevice)
            pkD3DDevice->Release();

        return false;
    }

    m_pkD3D10Device = pkD3DDevice;
    m_uiAdapterIndex = uiAdapterID;

    StoreDeviceSettings();

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::StoreDeviceSettings()
{
    NIASSERT(m_pkD3D10Device);
    // (DXGI_FORMAT)0 == DXGI_FORMAT_UNKNOWN
    unsigned int* puiIterator = &(m_auiFormatSupport[1]);
    for (unsigned int i = 1; i < DXGI_FORMAT_COUNT; i++)
    {
        DXGI_FORMAT eFormat = (DXGI_FORMAT)i;
        // These formats are deprecated.
        if (eFormat == DXGI_FORMAT_B5G6R5_UNORM ||
            eFormat == DXGI_FORMAT_B5G5R5A1_UNORM ||
            eFormat == DXGI_FORMAT_B8G8R8A8_UNORM ||
            eFormat == DXGI_FORMAT_B8G8R8X8_UNORM)
        {
            continue;
        }

        HRESULT hr = m_pkD3D10Device->CheckFormatSupport(eFormat, puiIterator);
        if (FAILED(hr))
        {
            NiD3D10Error::ReportWarning("ID3D10Device::CheckFormatSupport "
                "failed on format %s; error HRESULT = 0x%08X.", 
                NiD3D10PixelFormat::GetFormatName(eFormat, false),
                (unsigned int)hr);
        }
        puiIterator++;
    }

    // Set replacement texture format
    // This is D3D10 - it's safe to assume that R8G8B8A8 is supported.
    NIASSERT((m_auiFormatSupport[DXGI_FORMAT_R8G8B8A8_UNORM] & 
        D3D10_FORMAT_SUPPORT_TEXTURE2D) != 0);
    NiTexture::RendererData::SetTextureReplacementFormat(
        NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::CreateManagers()
{
    m_pkDataStreamFactory = NiNew NiD3D10DataStreamFactory;

    m_pkDeviceState = NiNew NiD3D10DeviceState(m_pkD3D10Device);
    m_pkRenderStateManager = NiNew NiD3D10RenderStateManager(m_pkD3D10Device,
        m_pkDeviceState);
    m_pkResourceManager = NiNew NiD3D10ResourceManager(m_pkD3D10Device);
    m_pkShaderConstantManager = 
        NiNew NiD3D10ShaderConstantManager(m_pkDeviceState);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::ReleaseManagers()
{
    NiDelete m_pkDataStreamFactory;

    NiDelete m_pkDeviceState;
    NiDelete m_pkRenderStateManager;
    NiDelete m_pkResourceManager;
    NiDelete m_pkShaderConstantManager;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::AddOccludedNotificationFunc(
    OCCLUDEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiReturn = m_kOccludedNotifyFuncs.Add(pfnNotify);
    m_kOccludedNotifyFuncData.SetAtGrow(uiReturn, pvData);

    return uiReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveOccludedNotificationFunc(
    OCCLUDEDNOTIFYFUNC pfnNotify)
{
    unsigned int uiIndex = FindOccludedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    m_kOccludedNotifyFuncs.RemoveAt(uiIndex);
    m_kOccludedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveOccludedNotificationFunc(unsigned int uiIndex)
{
    if (m_kOccludedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kOccludedNotifyFuncs.RemoveAt(uiIndex);
    m_kOccludedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveAllOccludedNotificationFuncs()
{
    m_kOccludedNotifyFuncs.RemoveAll();
    m_kOccludedNotifyFuncData.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeOccludedNotificationFuncData(
    OCCLUDEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiIndex = FindOccludedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    NIASSERT(m_kOccludedNotifyFuncData.GetSize() > uiIndex);
    m_kOccludedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeOccludedNotificationFuncData(unsigned int uiIndex, 
    void* pvData)
{
    if (m_kOccludedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    NIASSERT(m_kOccludedNotifyFuncData.GetSize() > uiIndex);
    m_kOccludedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetOccludedNotificationFuncCount() const
{
    return m_kOccludedNotifyFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetOccludedNotificationFuncArrayCount() const
{
    return m_kOccludedNotifyFuncs.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::FindOccludedNotificationFunc(
    OCCLUDEDNOTIFYFUNC pfnNotify) const
{
    for (unsigned int i = 0; i < m_kOccludedNotifyFuncs.GetSize(); i++)
    {
        if (m_kOccludedNotifyFuncs.GetAt(i) == pfnNotify)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
NiD3D10Renderer::OCCLUDEDNOTIFYFUNC 
    NiD3D10Renderer::GetOccludedNotificationFunc(unsigned int uiIndex) const
{
    return m_kOccludedNotifyFuncs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::AddDeviceRemovedNotificationFunc(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiReturn = m_kDeviceRemovedNotifyFuncs.Add(pfnNotify);
    m_kDeviceRemovedNotifyFuncData.SetAtGrow(uiReturn, pvData);

    return uiReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveDeviceRemovedNotificationFunc(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify)
{
    unsigned int uiIndex = FindDeviceRemovedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    m_kDeviceRemovedNotifyFuncs.RemoveAt(uiIndex);
    m_kDeviceRemovedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::RemoveDeviceRemovedNotificationFunc(
    unsigned int uiIndex)
{
    if (m_kDeviceRemovedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kDeviceRemovedNotifyFuncs.RemoveAt(uiIndex);
    m_kDeviceRemovedNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeDeviceRemovedNotificationFuncData(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiIndex = FindDeviceRemovedNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    NIASSERT(m_kDeviceRemovedNotifyFuncData.GetSize() > uiIndex);
    m_kDeviceRemovedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ChangeDeviceRemovedNotificationFuncData(
    unsigned int uiIndex, void* pvData)
{
    if (m_kDeviceRemovedNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    NIASSERT(m_kDeviceRemovedNotifyFuncData.GetSize() > uiIndex);
    m_kDeviceRemovedNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::FindDeviceRemovedNotificationFunc(
    DEVICEREMOVEDNOTIFYFUNC pfnNotify) const
{
    for (unsigned int i = 0; i < m_kDeviceRemovedNotifyFuncs.GetSize(); i++)
    {
        if (m_kDeviceRemovedNotifyFuncs.GetAt(i) == pfnNotify)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
IDXGISwapChain* NiD3D10Renderer::CreateSwapChain(
    DXGI_SWAP_CHAIN_DESC& kSwapChainDesc, unsigned int uiOutputIndex)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call CreateSwapChain without "
            "a valid device.");
        return NULL;
    }
    NIASSERT(m_spSystemDesc);

    NIASSERT(m_uiAdapterIndex == NULL_ADAPTER ||
        m_uiAdapterIndex < m_spSystemDesc->GetAdapterCount());

    DXGI_SWAP_CHAIN_DESC kActualSwapChainDesc = kSwapChainDesc;
    if (m_uiAdapterIndex != NULL_ADAPTER)
    {
        const NiD3D10AdapterDesc* pkAdapterDesc = 
            m_spSystemDesc->GetAdapterDesc(m_uiAdapterIndex);
        NIASSERT(pkAdapterDesc);

        unsigned int uiOutputID = uiOutputIndex;
        if (uiOutputID >= pkAdapterDesc->GetOutputCount())
            uiOutputID = 0;
        const NiD3D10OutputDesc* pkOutputDesc = 
            pkAdapterDesc->GetOutputDesc(uiOutputID);
        NIASSERT(pkOutputDesc);
        IDXGIOutput* pkOutput = pkOutputDesc->GetOutput();
        NIASSERT(pkOutput);

        pkOutput->FindClosestMatchingMode(&(kSwapChainDesc.BufferDesc), 
            &(kActualSwapChainDesc.BufferDesc), m_pkD3D10Device);
    }

    IDXGIFactory* pkFactory = m_spSystemDesc->GetFactory();
    NIASSERT(pkFactory);

    IDXGISwapChain* pkSwapChain = NULL;
    HRESULT hr = pkFactory->CreateSwapChain(m_pkD3D10Device, 
        &kActualSwapChainDesc, &pkSwapChain);

    if (FAILED(hr) || pkSwapChain == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_SWAP_CHAIN_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_SWAP_CHAIN_CREATION_FAILED,
                "No error message from D3D10, but swap chain is NULL.");
        }

        if (pkSwapChain)
            pkSwapChain->Release();

        return NULL;
    }

    return pkSwapChain;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiD3D10Renderer::CreateRenderTargetGroupFromSwapChain(
    IDXGISwapChain* pkSwapChain, bool bCreateDepthStencilBuffer,
    DXGI_FORMAT eDepthStencilFormat)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call "
            "CreateRenderTargetGroupFromSwapChain without a valid device.");
        return NULL;
    }

    if (pkSwapChain == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call "
            "CreateRenderTargetGroupFromSwapChain without a valid "
            "swap chain.");
        return NULL;
    }

    Ni2DBuffer* pkBackBuffer = NULL;
    NiD3D10SwapChainBufferData::Create(pkSwapChain, pkBackBuffer);
    Ni2DBufferPtr spBackBuffer = pkBackBuffer;

    NiDepthStencilBufferPtr spDepthBuffer;
    if (bCreateDepthStencilBuffer)
    {
        if (DoesFormatSupportFlag(eDepthStencilFormat,
            D3D10_FORMAT_SUPPORT_DEPTH_STENCIL))
        {
            const NiPixelFormat* pkFormat = 
                NiD3D10PixelFormat::ObtainFromDXGIFormat(eDepthStencilFormat);
            NIASSERT(pkFormat);
            spDepthBuffer = NiDepthStencilBuffer::Create(
                pkBackBuffer->GetWidth(), 
                pkBackBuffer->GetHeight(),
                this, *pkFormat,
                pkBackBuffer->GetMSAAPref());
        }
        else
        {
            spDepthBuffer = NiDepthStencilBuffer::CreateCompatible(
                pkBackBuffer, this);
            if (spDepthBuffer == NULL)
            {
                NiD3D10Error::ReportWarning("Compatible depth/stencil buffer "
                    "could not be created; render target group will have "
                    "no depth/stencil buffer.");
            }
        }
    }

    NiRenderTargetGroup* pkRTGroup = NiRenderTargetGroup::Create(pkBackBuffer,
        this, spDepthBuffer);

    return pkRTGroup;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ReleaseDevice()
{
    if (m_pkD3D10Device)
    {
        m_pkD3D10Device->Release();
        m_pkD3D10Device = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
ID3D10Device* NiD3D10Renderer::CreateTempDevice(IDXGIAdapter* pkAdapter, 
    D3D10_DRIVER_TYPE eType, unsigned int uiFlags)
{
    ID3D10Device* pkDevice = NULL;           
    HRESULT hr = S_OK;
    ms_kD3D10LibraryCriticalSection.Lock();
    if (EnsureD3D10Loaded())
    {
        // Check for the NVPerfHUD device.
        bool bIsPerfHUD = false;
        if (pkAdapter)
        {
            DXGI_ADAPTER_DESC kDesc;
            hr = pkAdapter->GetDesc(&kDesc);
            if (SUCCEEDED(hr))
            {
                bIsPerfHUD = 
                    (wcsstr(kDesc.Description, L"PerfHUD") != NULL);
            }
        }
        // If we have NVPerfHUD or a hardware 
        // device type, create temp device.
        if (eType == D3D10_DRIVER_TYPE_REFERENCE && !bIsPerfHUD)
            pkAdapter = NULL;

        hr = D3D10CreateDevice(pkAdapter, eType, NULL, uiFlags, 
            D3D10_SDK_VERSION, &pkDevice);
        if (FAILED(hr) || pkDevice == NULL)
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                    "Error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_DEVICE_CREATION_FAILED,
                    "No error message from D3D10, but device is NULL.");
            }

            if (pkDevice)
                pkDevice->Release();
        }
    }
    else
    {
        NiD3D10Error::ReportWarning("D3D10 library failed to be loaded; "
            "device cannot be created.");
    }
    ms_kD3D10LibraryCriticalSection.Unlock();
    return pkDevice;
}
//---------------------------------------------------------------------------
const char* NiD3D10Renderer::GetDriverInfo() const
{
    const NiD3D10AdapterDesc* pkAdapterDesc = 
        m_spSystemDesc->GetAdapterDesc(m_uiAdapterIndex);
    NIASSERT(pkAdapterDesc);

    const DXGI_ADAPTER_DESC* pkD3D10AdapterDesc = pkAdapterDesc->GetDesc();
    NIASSERT(pkD3D10AdapterDesc);

    int iResult = WideCharToMultiByte(
        CP_ACP, 
        0, 
        pkD3D10AdapterDesc->Description,
        -1, 
        m_acDriverDesc, 
        sizeof(m_acDriverDesc),
        NULL, 
        NULL);
    NIASSERT(iResult < sizeof(m_acDriverDesc));

    if (iResult > 0)
    {
        const char* pcDriverType = NULL;
        switch (m_kInitialCreateParameters.m_eDriverType)
        {
        case DRIVER_HARDWARE:
            pcDriverType = "Hardware driver";
            break;
        case DRIVER_REFERENCE:
            pcDriverType = "Software driver";
            break;
        case DRIVER_NULL:
            pcDriverType = "Null driver";
            break;
        default:
            pcDriverType = "Unknown driver";
            break;
        }

        NiSprintf(
            m_acDriverDesc + iResult - 1, 
            sizeof(m_acDriverDesc) - iResult,
            " (%s)", 
            pcDriverType);
    }

    return m_acDriverDesc;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetFlags() const
{
    return CAPS_HARDWARESKINNING |
        CAPS_NONPOW2_TEXT |
        CAPS_AA_RENDERED_TEXTURES |
        CAPS_HARDWAREINSTANCING |
        CAPS_ANISO_FILTERING;
}
//---------------------------------------------------------------------------
NiSystemDesc::RendererID NiD3D10Renderer::GetRendererID() const
{
    return NiSystemDesc::RENDERER_D3D10;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetDepthClear(const float fZClear)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    m_fDepthClear = fZClear;
}
//---------------------------------------------------------------------------
float NiD3D10Renderer::GetDepthClear() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_fDepthClear;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetBackgroundColor(const NiColor& kColor)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    m_afBackgroundColor[0] = kColor.r;
    m_afBackgroundColor[1] = kColor.g;
    m_afBackgroundColor[2] = kColor.b;
    m_afBackgroundColor[3] = 1.0f;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetBackgroundColor(const NiColorA& kColor)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    m_afBackgroundColor[0] = kColor.r;
    m_afBackgroundColor[1] = kColor.g;
    m_afBackgroundColor[2] = kColor.b;
    m_afBackgroundColor[3] = kColor.a;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::GetBackgroundColor(NiColorA& kColor) const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    kColor.r = m_afBackgroundColor[0];
    kColor.g = m_afBackgroundColor[1];
    kColor.b = m_afBackgroundColor[2];
    kColor.a = m_afBackgroundColor[3];
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetStencilClear(unsigned int uiClear)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    m_ucStencilClear = (unsigned char)(uiClear & 0x000000FF);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetStencilClear() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_ucStencilClear;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::ValidateRenderTargetGroup(NiRenderTargetGroup* pkTarget)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (pkTarget == NULL)
        return false;

    D3D10_RTV_DIMENSION eResourceType = D3D10_RTV_DIMENSION_UNKNOWN;
    unsigned int uiElementOffset = UINT_MAX;
    unsigned int uiElementWidth = UINT_MAX;
    unsigned int uiMipSlice = UINT_MAX;
    unsigned int uiArraySize = UINT_MAX;
    unsigned int uiBufferWidth = UINT_MAX;
    unsigned int uiBufferHeight = UINT_MAX;
    bool bCubeMap = false;

    Ni2DBuffer* pkFirstBuffer = NULL;

    for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
    {
        // Note that finding *no* render target in the render target group
        // is still valid.
        NiD3D102DBufferData* pk2DBufferData = 
            (NiD3D102DBufferData*)pkTarget->GetBufferRendererData(i);
        if (pk2DBufferData == NULL)
            continue;

        if (pkFirstBuffer == NULL)
            pkFirstBuffer = pkTarget->GetBuffer(i);

        NiD3D10RenderTargetBufferData* pkRTBufferData = 
            NiVerifyStaticCast(NiD3D10RenderTargetBufferData, pk2DBufferData);

        ID3D10RenderTargetView* pkRTView = 
            pkRTBufferData->GetRenderTargetView();

        if (pkRTView == NULL)
            continue;

        D3D10_RENDER_TARGET_VIEW_DESC kDesc;
        pkRTView->GetDesc(&kDesc);

        // Get 2D texture description
        ID3D10Texture2D* pkRTTexture = pkRTBufferData->GetRenderTargetBuffer();

        if (pkRTTexture == NULL)
            continue;

        D3D10_TEXTURE2D_DESC kRTTextureDesc;
        pkRTTexture->GetDesc(&kRTTextureDesc);

        // Check for un-set resource type
        if (eResourceType == D3D10_RTV_DIMENSION_UNKNOWN)
        {
            eResourceType = kDesc.ViewDimension;
            bCubeMap = (0 !=
                (kRTTextureDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE));
        }

        // Check for mismatched cube map flag
        if (bCubeMap != (
            (kRTTextureDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE) != 0))
        {
            return false;
        }

        NIASSERT(eResourceType != D3D10_RTV_DIMENSION_UNKNOWN);

        // Check for mismatched resource data
        if (eResourceType == D3D10_RTV_DIMENSION_BUFFER)
        {
            if (uiElementOffset == UINT_MAX)
            {
                NIASSERT(uiElementWidth == UINT_MAX);
                uiElementOffset = kDesc.Buffer.ElementOffset;
                uiElementWidth = kDesc.Buffer.ElementWidth;
            }
            else if (kDesc.ViewDimension != D3D10_RTV_DIMENSION_BUFFER ||
                uiElementOffset != kDesc.Buffer.ElementOffset ||
                uiElementWidth != kDesc.Buffer.ElementWidth)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE1D)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1D)
            {
                // OK
                uiTempMipSlice = kDesc.Texture1D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
            {
                if (uiMipSlice == UINT_MAX ||
                    kDesc.Texture1DArray.ArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture1DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
                uiMipSlice = uiTempMipSlice;
            else if (uiMipSlice != uiTempMipSlice)
                return false;

            NIASSERT(kDesc.Texture1D.MipSlice == 0);

            // For 1D resources, the height must be 1.
            NIASSERT(pk2DBufferData->GetHeight() == 1);
            if (uiBufferWidth == UINT_MAX)
            {
                uiBufferWidth = pk2DBufferData->GetWidth();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1D)
            {
                if (uiArraySize == UINT_MAX || uiArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture1D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
            {
                if (uiArraySize == UINT_MAX ||
                    kDesc.Texture1DArray.ArraySize == uiArraySize)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture1DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
            {
                NIASSERT(uiArraySize == UINT_MAX);
                uiMipSlice = uiTempMipSlice;
                uiArraySize = kDesc.Texture1DArray.ArraySize;
            }
            else if (uiMipSlice != uiTempMipSlice)
            {
                return false;
            }

            NIASSERT(kDesc.Texture1DArray.MipSlice == 0);

            // For 1D resources, the height must be 1.
            NIASSERT(pk2DBufferData->GetHeight() == 1);
            if (uiBufferWidth == UINT_MAX)
            {
                uiBufferWidth = pk2DBufferData->GetWidth();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2D)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2D)
            {
                // OK
                uiTempMipSlice = kDesc.Texture2D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
            {
                if (uiMipSlice == UINT_MAX ||
                    kDesc.Texture2DArray.ArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture2DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
                uiMipSlice = uiTempMipSlice;
            else if (uiMipSlice != uiTempMipSlice)
                return false;

            NIASSERT(kDesc.Texture2D.MipSlice == 0);

            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
        {
            unsigned int uiTempMipSlice = UINT_MAX;
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2D)
            {
                if (uiArraySize == UINT_MAX || uiArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture2D.MipSlice;
            }
            else if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
            {
                if (uiArraySize == UINT_MAX ||
                    kDesc.Texture2DArray.ArraySize == uiArraySize)
                {
                    // OK
                }
                else
                {
                    return false;
                }
                uiTempMipSlice = kDesc.Texture2DArray.MipSlice;
            }
            else
            {
                return false;
            }

            if (uiMipSlice == UINT_MAX)
            {
                NIASSERT(uiArraySize == UINT_MAX);
                uiMipSlice = uiTempMipSlice;
                uiArraySize = kDesc.Texture2DArray.ArraySize;
            }
            else if (uiMipSlice != uiTempMipSlice)
            {
                return false;
            }

            NIASSERT(kDesc.Texture2DArray.MipSlice == 0);

            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }
        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2DMS)
        {
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMS)
            {
                // OK
            }
            else if (kDesc.ViewDimension == 
                D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
            {
                if (kDesc.Texture2DMSArray.ArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            // D3D10 doesn't support mipmapped multisampled textures.
            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }

        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
        {
            if (kDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMS)
            {
                if (uiArraySize == UINT_MAX || uiArraySize == 1)
                {
                    // OK
                }
                else
                {
                    return false;
                }
            }
            else if (kDesc.ViewDimension == 
                D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
            {
                if (uiArraySize == UINT_MAX ||
                    kDesc.Texture2DMSArray.ArraySize == uiArraySize)
                {
                    // OK
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }

            if (uiArraySize == UINT_MAX)
            {
                uiArraySize = kDesc.Texture2DArray.ArraySize;
            }

            // D3D10 doesn't support mipmapped multisampled textures.
            if (uiBufferWidth == UINT_MAX)
            {
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }

        }
        else if (eResourceType == D3D10_RTV_DIMENSION_TEXTURE3D)
        {
            if (uiMipSlice == UINT_MAX)
            {
                NIASSERT(uiArraySize == UINT_MAX);
                uiMipSlice = kDesc.Texture3D.MipSlice;
                uiArraySize = kDesc.Texture3D.WSize;
            }
            else if (kDesc.ViewDimension != D3D10_RTV_DIMENSION_TEXTURE3D ||
                uiMipSlice != kDesc.Texture3D.MipSlice ||
                uiArraySize != kDesc.Texture3D.WSize)
            {
                return false;
            }

            NIASSERT(kDesc.Texture3D.MipSlice == 0);

            if (uiBufferWidth == UINT_MAX)
            {
                // Third dimension is checked in WSize above.
                NIASSERT(uiBufferHeight == UINT_MAX);
                uiBufferWidth = pk2DBufferData->GetWidth();
                uiBufferHeight = pk2DBufferData->GetHeight();
            }
            else if (pk2DBufferData->GetWidth() != uiBufferWidth ||
                pk2DBufferData->GetHeight() != uiBufferHeight)
            {
                return false;
            }

        }

        // Check for duplicate views
        for (unsigned int j = 0; j < i; j++)
        {
            NiD3D102DBufferData* pkOther2DBufferData = 
                (NiD3D102DBufferData*)pkTarget->GetBufferRendererData(j);
            if (pkOther2DBufferData == NULL)
                continue;

            NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, 
                pkOther2DBufferData));

            NiD3D10RenderTargetBufferData* pkOtherRTBufferData = 
                (NiD3D10RenderTargetBufferData*)pkOther2DBufferData;

            ID3D10RenderTargetView* pkOtherRTView = 
                pkOtherRTBufferData->GetRenderTargetView();

            if (pkOtherRTView == pkRTView)
                return false;
        }
    }

    // Check depth stencil buffer
    // Assume that the depth stencil buffer only needs to be tested against
    // one active color buffer.
    NiDepthStencilBuffer* pkDSBuffer = pkTarget->GetDepthStencilBuffer();
    if (pkDSBuffer)
    {
        if (IsDepthBufferCompatible(pkFirstBuffer, pkDSBuffer) == false)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::IsDepthBufferCompatible(Ni2DBuffer* pkBuffer, 
    NiDepthStencilBuffer* pkDSBuffer)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    // Note that finding *no* render target in the render target group
    // is still valid.
    if (pkBuffer == NULL || pkDSBuffer == NULL)
        return true;

    // Get 2D buffer description
    NiD3D102DBufferData* pk2DBufferData = 
        (NiD3D102DBufferData*)pkBuffer->GetRendererData();
    if (pk2DBufferData == NULL)
        return false;

    NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, pk2DBufferData));

    NiD3D10RenderTargetBufferData* pkRTBufferData = 
        (NiD3D10RenderTargetBufferData*)pk2DBufferData;

    ID3D10RenderTargetView* pkRTView = pkRTBufferData->GetRenderTargetView();

    if (pkRTView == NULL)
        return false;

    D3D10_RENDER_TARGET_VIEW_DESC kRTDesc;
    pkRTView->GetDesc(&kRTDesc);

    // Get 2D texture description
    ID3D10Texture2D* pkRTTexture = pkRTBufferData->GetRenderTargetBuffer();

    if (pkRTTexture == NULL)
        return false;

    D3D10_TEXTURE2D_DESC kRTTextureDesc;
    pkRTTexture->GetDesc(&kRTTextureDesc);

    // Get depth/stencil buffer description
    pk2DBufferData = (NiD3D102DBufferData*)pkDSBuffer->GetRendererData();
    if (pk2DBufferData == NULL)
        return false;

    NiD3D10DepthStencilBufferData* pkDSBufferData = 
        NiVerifyStaticCast(NiD3D10DepthStencilBufferData, pk2DBufferData);

    ID3D10DepthStencilView* pkDSView = pkDSBufferData->GetDepthStencilView();

    if (pkDSView == NULL)
        return false;

    D3D10_DEPTH_STENCIL_VIEW_DESC kDSDesc;
    pkDSView->GetDesc(&kDSDesc);

    // Get depth/stencil texture description
    ID3D10Texture2D* pkDSTexture = pkDSBufferData->GetDepthStencilBuffer();

    if (pkDSTexture == NULL)
        return false;

    D3D10_TEXTURE2D_DESC kDSTextureDesc;
    pkDSTexture->GetDesc(&kDSTextureDesc);

    // Check for cube maps - both must be cube maps (or not cube maps)
    if ((kRTTextureDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE) !=
        (kDSTextureDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE))
    {
        return false;
    }

    // Check for mismatched resource types and data.
    // Remember that an array of 1 texture is the same as just that texture.
    if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1D)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1D)
        {
            // OK
            uiDSMipSlice = kDSDesc.Texture1D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1DARRAY)
        {
            if (kDSDesc.Texture1DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture1DArray.MipSlice;
        }
        else
        {
            return false;
        }
        NIASSERT(kRTDesc.Texture1D.MipSlice == 0 && uiDSMipSlice == 0);

        // For 1D resources, the height must be 1.
        NIASSERT(pkRTBufferData->GetHeight() == 1 &&
            pkDSBufferData->GetHeight() == 1);
        if (pkRTBufferData->GetWidth() != pkDSBufferData->GetWidth())
            return false;
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1D)
        {
            if (kRTDesc.Texture1DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture1D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE1DARRAY)
        {
            if (kDSDesc.Texture1DArray.ArraySize == 
                kRTDesc.Texture1DArray.ArraySize)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture1DArray.MipSlice;
        }
        else
        {
            return false;
        }

        NIASSERT(kRTDesc.Texture1D.MipSlice == 0 && uiDSMipSlice == 0);

        // For 1D resources, the height must be 1.
        NIASSERT(pkRTBufferData->GetHeight() == 1 &&
            pkDSBufferData->GetHeight() == 1);
        if (pkRTBufferData->GetWidth() != pkDSBufferData->GetWidth())
            return false;
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2D)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2D)
        {
            // OK
            uiDSMipSlice = kDSDesc.Texture2D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DARRAY)
        {
            if (kDSDesc.Texture2DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture2DArray.MipSlice;
        }
        else
        {
            return false;
        }
        NIASSERT(kRTDesc.Texture2D.MipSlice == 0 && uiDSMipSlice == 0);

        if (pkRTBufferData->GetWidth() != pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() != pkDSBufferData->GetHeight())
        {
            return false;
        }

    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
    {
        unsigned int uiDSMipSlice = UINT_MAX;
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2D)
        {
            if (kRTDesc.Texture2DArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture2D.MipSlice;
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DARRAY)
        {
            if (kDSDesc.Texture2DArray.ArraySize == 
                kRTDesc.Texture2DArray.ArraySize)
            {
                // OK
            }
            else
            {
                return false;
            }
            uiDSMipSlice = kDSDesc.Texture2DArray.MipSlice;
        }
        else
        {
            return false;
        }

        NIASSERT(kRTDesc.Texture2D.MipSlice == 0 && uiDSMipSlice == 0);

        if (pkRTBufferData->GetWidth() != pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() != pkDSBufferData->GetHeight())
        {
            return false;
        }
    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMS)
    {

        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMS)
        {
            // OK
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY)
        {
            if (kDSDesc.Texture2DMSArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        // D3D10 doesn't support mipmapped multisampled textures.
        if (pkRTBufferData->GetWidth() != pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() != pkDSBufferData->GetHeight())
        {
            return false;
        }

    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
    {
        if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMS)
        {
            if (kRTDesc.Texture2DMSArray.ArraySize == 1)
            {
                // OK
            }
            else
            {
                return false;
            }
        }
        else if (kDSDesc.ViewDimension == D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY)
        {
            if (kDSDesc.Texture2DMSArray.ArraySize == 
                kRTDesc.Texture2DMSArray.ArraySize)
            {
                // OK
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        // D3D10 doesn't support mipmapped multisampled textures.
        if (pkRTBufferData->GetWidth() != pkDSBufferData->GetWidth() ||
            pkRTBufferData->GetHeight() != pkDSBufferData->GetHeight())
        {
            return false;
        }

    }
    else if (kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_BUFFER ||
        kRTDesc.ViewDimension == D3D10_RTV_DIMENSION_TEXTURE3D)
    {
        // Any other resource types other than those listed here are
        // not valid for the depth/stencil buffer, though 
        // D3D10_RTV_DIMENSION_UNKNOWN indicates that no render target
        // exists, so the depth/stencil buffer can be whatever it
        // wants to be.
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiD3D10Renderer::GetDefaultRenderTargetGroup() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_spDefaultRenderTargetGroup;
}
//---------------------------------------------------------------------------
const NiRenderTargetGroup* NiD3D10Renderer::GetCurrentRenderTargetGroup() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_pkCurrentRenderTargetGroup;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiD3D10Renderer::GetDefaultDepthStencilBuffer() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_spDefaultRenderTargetGroup)
        return m_spDefaultRenderTargetGroup->GetDepthStencilBuffer();
    return NULL;
}
//---------------------------------------------------------------------------
Ni2DBuffer* NiD3D10Renderer::GetDefaultBackBuffer() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_spDefaultRenderTargetGroup)
        return m_spDefaultRenderTargetGroup->GetBuffer(0);
    return NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10Renderer::FindClosestPixelFormat(
    NiTexture::FormatPrefs& kPrefs) const
{
    NiTexture::FormatPrefs::PixelLayout eLayout = kPrefs.m_ePixelLayout;
    NiTexture::FormatPrefs::AlphaFormat eAlpha = kPrefs.m_eAlphaFmt;

    if (eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
        eLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    if (eAlpha == NiTexture::FormatPrefs::ALPHA_DEFAULT)
        eAlpha = NiTexture::FormatPrefs::NONE;

    switch (eLayout)
    {
    case NiTexture::FormatPrefs::HIGH_COLOR_16: // 16-bit not supported
    case NiTexture::FormatPrefs::TRUE_COLOR_32:
        if (eAlpha == NiTexture::FormatPrefs::BINARY)
            return &NiD3D10PixelFormat::NI_FORMAT_R10G10B10A2_UNORM;
        else
            return &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM;
    case NiTexture::FormatPrefs::PALETTIZED_8: // Treat request for palettes
    case NiTexture::FormatPrefs::PALETTIZED_4: // as request for compression
    case NiTexture::FormatPrefs::COMPRESSED:
        if (eAlpha == NiTexture::FormatPrefs::BINARY ||
            eAlpha == NiTexture::FormatPrefs::NONE)
        {
            return &NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM;
        }
        else
        {
            return &NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM;
        }
    case NiTexture::FormatPrefs::BUMPMAP:
        return &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_SNORM;
    case NiTexture::FormatPrefs::SINGLE_COLOR_8:
        return &NiD3D10PixelFormat::NI_FORMAT_R8_UNORM;
    case NiTexture::FormatPrefs::SINGLE_COLOR_16:
        return &NiD3D10PixelFormat::NI_FORMAT_R16_UNORM;
    case NiTexture::FormatPrefs::SINGLE_COLOR_32:
        return &NiD3D10PixelFormat::NI_FORMAT_R32_UINT;
    case NiTexture::FormatPrefs::DOUBLE_COLOR_32:
        return &NiD3D10PixelFormat::NI_FORMAT_R16G16_UNORM;
    case NiTexture::FormatPrefs::DOUBLE_COLOR_64:
        return &NiD3D10PixelFormat::NI_FORMAT_R32G32_UINT;
    case NiTexture::FormatPrefs::FLOAT_COLOR_32:
        return &NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT;
    case NiTexture::FormatPrefs::FLOAT_COLOR_64:
        return &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
    case NiTexture::FormatPrefs::FLOAT_COLOR_128:
        return &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
    }

    return NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10Renderer::FindClosestDepthStencilFormat(
    const NiPixelFormat*, unsigned int uiDepthBPP,
    unsigned int uiStencilBPP) const
{
    // Depth stencil format is independent of front buffer format;
    NiPixelFormat* apkFormats[] = 
    {
        &NiD3D10PixelFormat::NI_FORMAT_D32_FLOAT_S8X24_UINT,
        &NiD3D10PixelFormat::NI_FORMAT_D32_FLOAT,
        &NiD3D10PixelFormat::NI_FORMAT_D24_UNORM_S8_UINT,
        &NiD3D10PixelFormat::NI_FORMAT_D16_UNORM
    };
    const unsigned int uiFormatCount = 
        sizeof(apkFormats) / sizeof(apkFormats[0]);

    // Array for order of attempting various formats
    unsigned int auiFormatPreference[uiFormatCount];
    unsigned int i = 0;
    for (; i < uiFormatCount; i++)
        auiFormatPreference[i] = i;

    if (uiStencilBPP > 0)
    {
        if (uiDepthBPP <= 24)
        {
            auiFormatPreference[0] = 2;
            auiFormatPreference[1] = 0;
            auiFormatPreference[2] = 3;
            auiFormatPreference[3] = 1;
        }
        else
        {
            auiFormatPreference[0] = 0;
            auiFormatPreference[1] = 2;
            auiFormatPreference[2] = 1;
            auiFormatPreference[3] = 3;
        }
    }
    else
    {
        if (uiDepthBPP <= 16)
        {
            auiFormatPreference[0] = 3;
            auiFormatPreference[1] = 2;
            auiFormatPreference[2] = 1;
            auiFormatPreference[3] = 0;
        }
        else if (uiDepthBPP <= 24)
        {
            auiFormatPreference[0] = 2;
            auiFormatPreference[1] = 1;
            auiFormatPreference[2] = 0;
            auiFormatPreference[3] = 3;
        }
        else
        {
            auiFormatPreference[0] = 1;
            auiFormatPreference[1] = 0;
            auiFormatPreference[2] = 2;
            auiFormatPreference[3] = 3;
        }
    }
    
    for (i = 0; i < uiFormatCount; i++)
    {
        NiPixelFormat* pkFormat = apkFormats[auiFormatPreference[i]];
        if (DoesFormatSupportFlag((DXGI_FORMAT)pkFormat->GetRendererHint(), 
            D3D10_FORMAT_SUPPORT_DEPTH_STENCIL))
        {
            return pkFormat;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetMaxBuffersPerRenderTargetGroup() const
{
    return D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::GetIndependentBufferBitDepths() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PrecacheMesh(
    NiRenderObject* pkMesh, 
    bool bReleaseSystemMemory)
{
    if (pkMesh == NULL || !NiIsKindOf(NiMesh, pkMesh))
        return false;

    LockPrecacheCriticalSection();

    PrePackMesh* pkPrePack = m_kPrePackMeshes.AddNewTail();
    pkPrePack->m_spMesh = (NiMesh*)pkMesh;
    pkPrePack->m_bReleaseSystemMemory = bReleaseSystemMemory;

    UnlockPrecacheCriticalSection();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PrecacheTexture(NiTexture* pkIm)
{
    if (NiIsKindOf(NiSourceTexture, pkIm))
    {
        return CreateSourceTextureRendererData((NiSourceTexture*)pkIm);
    }
    else
    {
        return (pkIm->GetRendererData() != NULL);
    }
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PerformPrecache()
{
    // Can only perform precache from device thread.
    if (((m_kInitialCreateParameters.m_uiCreateFlags &
        CREATE_DEVICE_SINGLETHREADED) != 0) &&
        m_ulDeviceThreadID != NiGetCurrentThreadId())
    {
        return false;
    }

    NIASSERT_D3D10_DEVICE_THREAD;

    LockPrecacheCriticalSection();
    NiTListIterator kIter = m_kPrePackMeshes.GetHeadPos();
    while (kIter)
    {
        PrePackMesh* pkPrePack = m_kPrePackMeshes.GetNext(kIter);

        NiMesh* pkMesh = pkPrePack->m_spMesh;
        const bool bReleaseSystemMemory = pkPrePack->m_bReleaseSystemMemory;

        /// Gets the number of streamrefs attached to this mesh.
        const NiUInt32 uiStreamRefCount = pkMesh->GetStreamRefCount();
        for (NiUInt32 i = 0; i < uiStreamRefCount; i++)
        {
            NiDataStreamRef* pkDataStreamRef = pkMesh->GetStreamRefAt(i);
            NIASSERT(pkDataStreamRef);
            NiDataStream* pkDataStream = pkDataStreamRef->GetDataStream();
            NIASSERT(pkDataStream && 
                NiIsKindOf(NiD3D10DataStream, pkDataStream));

            // Can only precache static, GPU-readable streams
            if ((pkDataStream->GetAccessMask() & 
                NiDataStream::ACCESS_CPU_WRITE_STATIC) == 0 ||
                (pkDataStream->GetAccessMask() & 
                NiDataStream::ACCESS_GPU_READ) == 0)
            {
                continue;
            }

            NiD3D10DataStream* pkD3D10DataStream = 
                (NiD3D10DataStream*)pkDataStream;

            pkD3D10DataStream->UpdateD3D10Buffers(bReleaseSystemMemory);
        }

        pkPrePack->m_spMesh = NULL;
    }
    m_kPrePackMeshes.RemoveAll();

    kIter = m_kPrePackTextures.GetHeadPos();
    while (kIter)
    {
        PrePackTexture* pkPrePack = m_kPrePackTextures.GetNext(kIter);

        NiTexture* pkTexture = pkPrePack->m_spTexture;

        NiD3D10TextureData* pkTexData = 
            (NiD3D10TextureData*)pkTexture->GetRendererData();
        if (pkTexData == NULL)
        {
            NiSourceTexture* pkSourceTex = 
                NiDynamicCast(NiSourceTexture, pkTexture);
            if (pkSourceTex)
            {
                pkTexData = NiD3D10SourceTextureData::Create(pkSourceTex);

                // Check to see if the texture data can be released
                if (NiSourceTexture::GetDestroyAppDataFlag() && 
                    pkSourceTex->GetRendererData() && 
                    pkSourceTex->GetStatic())
                {
                    pkSourceTex->DestroyAppPixelData();
                }
            }
            else
            {
                // All other texture types must have renderer data created
                // during texture construction. If we don't already have a 
                // texture data object by now, we never will.
            }
        }

        pkPrePack->m_spTexture = NULL;
        pkPrePack->m_spSrcPixelData = NULL;
    }
    m_kPrePackTextures.RemoveAll();

    UnlockPrecacheCriticalSection();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SetMipmapSkipLevel(unsigned int uiSkip)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NIASSERT(uiSkip <= USHRT_MAX);
    return NiD3D10SourceTextureData::SetMipmapSkipLevel(
        (unsigned short)uiSkip);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Renderer::GetMipmapSkipLevel() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return NiD3D10SourceTextureData::GetMipmapSkipLevel();
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeMaterial(NiMaterialProperty*)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    // No renderer data stored on materials
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::PurgeEffect(NiDynamicEffect*)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    // No renderer data stored on dynamic effects
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PurgeTexture(NiTexture* pkTexture)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NiD3D10TextureData* pkData = (NiD3D10TextureData*)
        (pkTexture->GetRendererData());

    if (pkData)
    {
        bool bIsRenderedTexture = pkData->IsRenderedTexture();

        if (bIsRenderedTexture)
        {
            if (NiIsKindOf(NiRenderedCubeMap, pkTexture))
            {
                NiRenderedCubeMap* pkRenderedCubeMap = 
                    (NiRenderedCubeMap*)pkTexture;
                for (unsigned int i = 0; i < NiRenderedCubeMap::FACE_NUM; i++)
                {
                    Ni2DBuffer* pkBuffer = pkRenderedCubeMap->GetFaceBuffer(
                        (NiRenderedCubeMap::FaceID)i);
                    pkBuffer->SetRendererData(NULL);
                }
            }
            else
            {
                NiRenderedTexture* pkRenderedTex = 
                    (NiRenderedTexture*)pkTexture;
                Ni2DBuffer* pkBuffer = pkRenderedTex->GetBuffer();
                pkBuffer->SetRendererData(NULL);
            }
        }

        pkTexture->SetRendererData(NULL);
        NiDelete pkData;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::PurgeAllTextures(bool)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NiD3D10TextureData::ClearTextureData();
    return true;
}
//---------------------------------------------------------------------------
NiPixelData* NiD3D10Renderer::TakeScreenShot(
    const NiRect<unsigned int>* pkScreenRect,
    const NiRenderTargetGroup* pkTarget)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (pkTarget == NULL)
        pkTarget = m_spDefaultRenderTargetGroup;

    ID3D10Resource* pkScreenShot = GetBackBufferResource(pkTarget, 0);

#if defined NIDEBUG
    D3D10_RESOURCE_DIMENSION eDim;
    pkScreenShot->GetType(&eDim);
    NIASSERT(eDim == D3D10_RESOURCE_DIMENSION_TEXTURE2D);
#endif // #if defined NIDEBUG

    ID3D10Texture2D* pkScreenShot2D = (ID3D10Texture2D*)pkScreenShot;

    D3D10_TEXTURE2D_DESC kDesc;
    pkScreenShot2D->GetDesc(&kDesc);

    // Calculate dimensions to be copied
    D3D10_BOX kBox;
    bool bCopySubregion = false;
    unsigned int uiSSWidth;
    unsigned int uiSSHeight;
    if (pkScreenRect)
    {
        if (pkScreenRect->m_right > pkScreenRect->m_left)
        {
            kBox.left = pkScreenRect->m_left;
            kBox.right = pkScreenRect->m_right;
        }
        else
        {
            kBox.right = pkScreenRect->m_left;
            kBox.left = pkScreenRect->m_right;
        }

        if (pkScreenRect->m_bottom > pkScreenRect->m_top)
        {
            kBox.top = pkScreenRect->m_top;
            kBox.bottom = pkScreenRect->m_bottom;
        }
        else
        {
            kBox.bottom = pkScreenRect->m_top;
            kBox.top = pkScreenRect->m_bottom;
        }

        kBox.front = 0;
        kBox.back = 1;

        if ((kBox.left >= kDesc.Width) || 
            (kBox.bottom >= kDesc.Height))
        {
            return NULL;
        }

        if (kBox.right > kDesc.Width)
            kBox.right = kDesc.Width;
        if (kBox.bottom > kDesc.Height)
            kBox.bottom = kDesc.Height;

        uiSSWidth = kBox.right - kBox.left;
        uiSSHeight = kBox.bottom - kBox.top;

        if (uiSSWidth < kDesc.Width || uiSSHeight < kDesc.Height)
            bCopySubregion = true;
    }
    else
    {
        uiSSWidth = kDesc.Width;
        uiSSHeight = kDesc.Height;
    }

    // If multisampled, must resolve to a single-sampled resource
    ID3D10Texture2D* pkSingleSampled = NULL;
    if (kDesc.SampleDesc.Count != 1 || kDesc.SampleDesc.Quality != 0)
    {
        pkSingleSampled = m_pkResourceManager->CreateTexture2D(
            kDesc.Width,
            kDesc.Height, 
            kDesc.MipLevels, 
            kDesc.ArraySize, 
            kDesc.Format, 
            1, 
            0,
            D3D10_USAGE_DEFAULT, 
            0, 
            0, 
            0);

        if (pkSingleSampled == NULL)
            return NULL;

        m_pkD3D10Device->ResolveSubresource(
            pkSingleSampled, 
            0, 
            pkScreenShot2D, 
            0, 
            kDesc.Format);
    }
    else
    {
        pkSingleSampled = pkScreenShot2D;
        pkSingleSampled->AddRef();
    };

    // Bring back to system RAM
    ID3D10Texture2D* pkStagingTexture = m_pkResourceManager->CreateTexture2D(
        uiSSWidth,
        uiSSHeight, 
        1, 
        kDesc.ArraySize, 
        kDesc.Format, 
        1, 
        0,
        D3D10_USAGE_STAGING, 
        0, 
        D3D10_CPU_ACCESS_READ, 
        0);

    if (pkStagingTexture == NULL)
    {
        pkSingleSampled->Release();
        return NULL;
    }

    if (bCopySubregion)
    {
        m_pkD3D10Device->CopySubresourceRegion(
            pkStagingTexture, 
            0, 
            0,
            0,
            0, 
            pkSingleSampled, 
            0,
            &kBox);
    }
    else
    {
        m_pkD3D10Device->CopyResource(pkStagingTexture, pkSingleSampled);
    }

    // Done with single-sampled resource
    pkSingleSampled->Release();

    // Fill in NiPixelData
    // Create an NiPixelData object that matches the backbuffer in format and
    // size
    NiPixelData* pkPixelData = NiNew NiPixelData(uiSSWidth, uiSSHeight, 
        *(pkTarget->GetPixelFormat(0)), 1);

    if (pkPixelData == NULL)
    {
        pkStagingTexture->Release();
        return NULL;
    }

    unsigned int uiRowSize 
        = pkPixelData->GetWidth(0) * pkPixelData->GetPixelStride();
    unsigned char* pucDest = pkPixelData->GetPixels(0);
    
    D3D10_MAPPED_TEXTURE2D kMappedTexture;
    HRESULT hr = pkStagingTexture->Map(0, D3D10_MAP_READ, 0, &kMappedTexture);
    if (FAILED(hr))
    {
        NiDelete pkPixelData;
        pkStagingTexture->Release();
        return NULL;
    }

    const unsigned char* pucSrc = (unsigned char*)kMappedTexture.pData;

    for (unsigned int i = 0; i < uiSSHeight; i++)
    {
        NiMemcpy(pucDest, pucSrc, uiRowSize);

        pucDest += uiRowSize;
        pucSrc += kMappedTexture.RowPitch;
    }

    pkStagingTexture->Unmap(0);

    pkStagingTexture->Release();

    return pkPixelData;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SaveScreenShot(const char* pcFilename,
    EScreenshotFormat eFormat)
{
    D3DX10_IMAGE_FILE_FORMAT eFormatD3D10;
    switch (eFormat)
    {
    case FORMAT_PNG:
        eFormatD3D10 = D3DX10_IFF_PNG;
        break;
    case FORMAT_JPEG:
        eFormatD3D10 = D3DX10_IFF_JPG;
        break;
    default:
        Error(
            "%s> Unsupported image format %d\n", 
            __FUNCTION__,
            eFormat);
        return false;
    }

    NiRenderTargetGroup* pkTarget = m_pkCurrentRenderTargetGroup;
    if (pkTarget == NULL)
        pkTarget = m_spDefaultRenderTargetGroup;

    ID3D10Resource* pkScreenShot = GetBackBufferResource(pkTarget, 0);

    // Huge hack to get around the fact that D3DX10SaveTextureToFile can only
    // save to the current directory

    // First get the full path to the destination file
    char acAbsolutePath[NI_MAX_PATH];

    NIASSERT(pcFilename);
    if (NiPath::IsRelative(pcFilename))
    {
        NiPath::ConvertToAbsolute(acAbsolutePath, NI_MAX_PATH, pcFilename,
            NULL);
    }
    else
    {
        NiStrcpy(acAbsolutePath, NI_MAX_PATH, pcFilename);
    }

    // Split off the directory from the file
    NiFilename kFilename(acAbsolutePath);
    char acDestPath[NI_MAX_PATH];
    char acDestFile[NI_MAX_PATH];
    NiSprintf(
        acDestPath, 
        NI_MAX_PATH, 
        "%s%s", 
        kFilename.GetDrive(), 
        kFilename.GetDir());
    NiSprintf(
        acDestFile,
        NI_MAX_PATH,
        "%s%s",
        kFilename.GetFilename(),
        kFilename.GetExt());

    // Save off the current working directory
    char acCurrentPath[NI_MAX_PATH];
    NIVERIFY(NiPath::GetCurrentWorkingDirectory(acCurrentPath, NI_MAX_PATH));

    // Ensure the new current directory exists.
    _mkdir(acDestPath);

    // Set the new current directory
    _chdir(acDestPath);

    // Save the file
    HRESULT hr = D3DX10SaveTextureToFile(
        pkScreenShot, 
        eFormatD3D10, 
        acDestFile);

    // Restore the original working directory
    _chdir(acCurrentPath);

    return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
ID3D10Texture2D* NiD3D10Renderer::GetBackBufferResource(
    const NiRenderTargetGroup* pkTarget,
    NiUInt32)
{
    NIASSERT(pkTarget && pkTarget->IsValid());

    NiD3D10RenderTargetBufferData* pkBuffData = 
        NiVerifyStaticCast(NiD3D10RenderTargetBufferData, 
        (NiD3D102DBufferData*)pkTarget->GetBufferRendererData(0));

    NIASSERT(pkBuffData);

    ID3D10Texture2D* pkTexture2D = pkBuffData->GetRenderTargetBuffer();

    return pkTexture2D;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::FastCopy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    unsigned int uiDestX, unsigned int uiDestY)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NIASSERT(pkSrc != NULL);
    NIASSERT(pkDest != NULL);

    NiD3D102DBufferData* pkSrcRendData = (NiD3D102DBufferData*)
        pkSrc->GetRendererData();
    NiD3D102DBufferData* pkDestRendData = (NiD3D102DBufferData*)
        pkDest->GetRendererData();

    if (pkSrcRendData == NULL || pkDestRendData == NULL)
    {
        Warning("%s> Failed - %s", 
            __FUNCTION__,
            "No RendererData found");
        return false;
    }

    if (*(pkSrcRendData->GetPixelFormat()) != 
        *(pkDestRendData->GetPixelFormat()))
    {
        Warning("%s> Failed - %s", 
            __FUNCTION__,
            "Pixel formats do not match");
        return false;
    }

    ID3D10Texture2D* pkSourceSurface = pkSrcRendData->GetTexture2D();
    ID3D10Texture2D* pkDestSurface = pkDestRendData->GetTexture2D();

    if (pkSourceSurface == NULL || pkDestSurface == NULL)
    {
        Warning("%s> Failed - %s", 
            __FUNCTION__,
            "NULL Surface found");
        return false;
    }

    // Calculate dimensions to be copied
    if (pkSrcRect)
    {
        D3D10_BOX kBox;
        kBox.left = pkSrcRect->m_left;
        kBox.right = pkSrcRect->m_right;
        kBox.top = pkSrcRect->m_top;
        kBox.bottom = pkSrcRect->m_bottom;
        kBox.front = 0;
        kBox.back = 1;

        m_pkD3D10Device->CopySubresourceRegion(
            pkDestSurface, 
            0, 
            uiDestX,
            uiDestY,
            0, 
            pkSourceSurface, 
            0,
            &kBox);
    }
    else
    {
        m_pkD3D10Device->CopyResource(pkDestSurface, pkSourceSurface);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Copy(const Ni2DBuffer* pkSrc, 
    Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    const NiRect<unsigned int>* pkDestRect,
    Ni2DBuffer::CopyFilterPreference ePref)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NIASSERT(pkSrc != NULL);
    NIASSERT(pkDest != NULL);

    NiD3D102DBufferData* pkSrcRendData = (NiD3D102DBufferData*)
        pkSrc->GetRendererData();
    NiD3D102DBufferData* pkDestRendData = (NiD3D102DBufferData*)
        pkDest->GetRendererData();

    if (pkSrcRendData == NULL || pkDestRendData == NULL)
    {
        Warning("%s> Failed - %s", 
            __FUNCTION__,
            "No RendererData found");
        return false;
    }

    if (*(pkSrcRendData->GetPixelFormat()) != 
        *(pkDestRendData->GetPixelFormat()))
    {
        Warning("%s> Failed - %s", 
            __FUNCTION__,
            "Pixel formats do not match");
        return false;
    }

    ID3D10Texture2D* pkSourceSurface = pkSrcRendData->GetTexture2D();
    ID3D10Texture2D* pkDestSurface = pkDestRendData->GetTexture2D();

    D3DX10_FILTER_FLAG eFilterType;
    switch (ePref)
    {
    default:
    case Ni2DBuffer::COPY_FILTER_NONE:
        eFilterType = D3DX10_FILTER_NONE;
        break;
    case Ni2DBuffer::COPY_FILTER_POINT:
        eFilterType = D3DX10_FILTER_POINT;
        break;
    case Ni2DBuffer::COPY_FILTER_LINEAR:
        eFilterType = D3DX10_FILTER_LINEAR;
        break;
    }

    D3DX10_TEXTURE_LOAD_INFO kLoadInfo;
    kLoadInfo.pSrcBox = NULL;
    kLoadInfo.pDstBox = NULL;
    kLoadInfo.SrcFirstMip = 0;
    kLoadInfo.DstFirstMip = 0;
    kLoadInfo.NumMips = 1; // Only doing one mip level
    kLoadInfo.SrcFirstElement = 0;
    kLoadInfo.DstFirstElement = 0;
    kLoadInfo.NumElements = 0;
    kLoadInfo.Filter = eFilterType;
    kLoadInfo.MipFilter = eFilterType;

    D3D10_BOX kSrcBox;
    if (pkSrcRect)
    {
        kSrcBox.left = pkSrcRect->m_left;
        kSrcBox.right = pkSrcRect->m_right;
        kSrcBox.top = pkSrcRect->m_top;
        kSrcBox.bottom = pkSrcRect->m_bottom;
        kSrcBox.front = 0;
        kSrcBox.back = 1;

        kLoadInfo.pSrcBox = &kSrcBox;
    }

    D3D10_BOX kDestBox;
    if (pkDestRect)
    {
        kDestBox.left = pkDestRect->m_left;
        kDestBox.right = pkDestRect->m_right;
        kDestBox.top = pkDestRect->m_top;
        kDestBox.bottom = pkDestRect->m_bottom;
        kDestBox.front = 0;
        kDestBox.back = 1;

        kLoadInfo.pDstBox = &kDestBox;
    }

    HRESULT hr = D3DX10LoadTextureFromTexture(
        pkSourceSurface, 
        &kLoadInfo, 
        pkDestSurface);

    if (FAILED(hr))
    {
        Warning("%s> "
            "Failed D3DX10LoadTextureFromTexture - %x", 
            __FUNCTION__,
            hr);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiD3D10Renderer::CreateCompatibleDepthStencilBuffer(
    Ni2DBuffer* pkBuffer) const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (!pkBuffer)
        return NULL;

    const NiPixelFormat* pkFormat = 
        NiRenderer::FindClosestDepthStencilFormat(pkBuffer->GetPixelFormat());

    if (pkFormat)
    {
        NiD3D102DBufferData* pkRendererData = 
            (NiD3D102DBufferData*)pkBuffer->GetRendererData();
        if (pkRendererData)
        {
            NiD3D10RenderTargetBufferData* pkRTBufferData = 
                NiVerifyStaticCast(NiD3D10RenderTargetBufferData, 
                pkRendererData);
            ID3D10Texture2D* pkTexture = 
                pkRTBufferData->GetRenderTargetBuffer();
            NIASSERT(pkTexture);

            D3D10_TEXTURE2D_DESC kTextureDesc;
            pkTexture->GetDesc(&kTextureDesc);

            if ((kTextureDesc.MiscFlags & D3D10_RESOURCE_MISC_TEXTURECUBE) != 
                0)
            {
                NiCubeMapDepthStencilBuffer* pkDSBuffer = 
                    NiCubeMapDepthStencilBuffer::Create(
                    pkBuffer->GetWidth(), 
                    pkBuffer->GetHeight(),
                    NiRenderer::GetRenderer(), //"this" not used b/c of const
                    *pkFormat,
                    pkBuffer->GetMSAAPref());
                return pkDSBuffer;
            }
        }

        NiDepthStencilBuffer* pkDSBuffer = NiDepthStencilBuffer::Create(
            pkBuffer->GetWidth(), 
            pkBuffer->GetHeight(),
            NiRenderer::GetRenderer(), //"this" not used b/c of const
            *pkFormat,
            pkBuffer->GetMSAAPref());
        return pkDSBuffer;
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::GetLeftRightSwap() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_bLeftRightSwap;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::SetLeftRightSwap(bool bSwap)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    m_bLeftRightSwap = bSwap;
    NIASSERT(m_pkRenderStateManager);
    m_pkRenderStateManager->SetLeftRightSwap(bSwap);

    return true;
}
//---------------------------------------------------------------------------
float NiD3D10Renderer::GetMaxFogValue() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_fMaxFogValue;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetMaxFogValue(float fFogVal)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (fFogVal <= 0.0f)
        fFogVal = 1e-5f;

    m_fMaxFogValue = fFogVal;
    m_fMaxFogFactor = 1.0f / fFogVal - 1.0f;
}
//---------------------------------------------------------------------------
float NiD3D10Renderer::GetMaxFogFactor() const
{
    NIASSERT_D3D10_DEVICE_THREAD;

    return m_fMaxFogFactor;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetMaxAnisotropy(unsigned short usMaxAnisotropy)
{
    m_usMaxAnisotropy = static_cast<unsigned short>(
        NiClamp(usMaxAnisotropy, 1, HW_MAX_ANISOTROPY));
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::UseLegacyPipelineAsDefaultMaterial()
{
    // Legacy pipeline not supported in D3D10
}
//---------------------------------------------------------------------------
NiD3D10Renderer* NiD3D10Renderer::GetRenderer()
{
    if (ms_pkRenderer && 
        ms_pkRenderer->GetRendererID() == NiSystemDesc::RENDERER_D3D10)
    {
        return (NiD3D10Renderer*)ms_pkRenderer;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiTexturePtr NiD3D10Renderer::CreateNiTextureFromD3D10Texture(
    ID3D10Resource* pkD3D10Texture, 
    ID3D10ShaderResourceView* pkResourceView)
{
    if (pkD3D10Texture == NULL || pkResourceView == NULL)
        return NULL;

    NiD3D10Direct3DTexturePtr spTexture = NiD3D10Direct3DTexture::Create(this);
    NIASSERT(spTexture && "Failed to create NiD3D10Direct3DTexture!");

    NiD3D10Direct3DTextureData::Create(spTexture, pkD3D10Texture, 
        pkResourceView);

    return NiSmartPointerCast(NiTexture, spTexture);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateSourceTextureRendererData(
    NiSourceTexture* pkTexture)
{
    // Not a function that requires it be on the device thread

    if (pkTexture == NULL)
        return false;

    if (m_ulDeviceThreadID == GetCurrentThreadId())
    {
        // Device thread - load texture immediately
        NiD3D10TextureData* pkTexData = 
            (NiD3D10TextureData*)pkTexture->GetRendererData();
        if (pkTexData == NULL)
        {
            pkTexData = (NiD3D10TextureData*)
                NiD3D10SourceTextureData::Create(pkTexture);
        }

        // Check to see if the texture data can be released
        if (NiSourceTexture::GetDestroyAppDataFlag() && 
            pkTexture->GetRendererData() && 
            pkTexture->GetStatic())
        {
            pkTexture->DestroyAppPixelData();
        }
    }
    else
    {
        // Non-device thread - delay texture loading

        // Load texture file so queries will work - this won't create a D3D
        // resource.
        if (!pkTexture->GetLoadDirectToRendererHint())
            pkTexture->LoadPixelDataFromFile();

        LockPrecacheCriticalSection();

        PrePackTexture* pkPrePack = m_kPrePackTextures.AddNewTail();
        pkPrePack->m_spTexture = pkTexture;
        pkPrePack->m_spSrcPixelData = pkTexture->GetSourcePixelData();

        UnlockPrecacheCriticalSection();
    }
    return true; 
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateRenderedTextureRendererData(
    NiRenderedTexture* pkTexture, 
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NiD3D10TextureData* pkData = 
        (NiD3D10TextureData*)pkTexture->GetRendererData();
    if (pkData)
        return true;

    pkData = NiD3D10RenderedTextureData::Create(pkTexture, eMSAAPref); 
    if (!pkData)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateSourceCubeMapRendererData(
    NiSourceCubeMap* pkCubeMap)
{
    // Cube maps go through standard 2D texture path.
    return CreateSourceTextureRendererData(pkCubeMap);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateRenderedCubeMapRendererData(
    NiRenderedCubeMap* pkCubeMap)
{
    // Cube maps go through standard 2D texture path.
    return CreateRenderedTextureRendererData(pkCubeMap);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDynamicTextureRendererData(
    NiDynamicTexture* pkTexture)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    NiD3D10DynamicTextureData* pkDynTxtrData = NULL;

    NiD3D10TextureData* pkTxtrData =
        (NiD3D10TextureData*)(pkTexture->GetRendererData());
    if (pkTxtrData)
    {
        pkDynTxtrData = pkTxtrData->GetAsDynamicTexture();
        if (pkDynTxtrData)
            return true;
    }

    pkDynTxtrData = NiD3D10DynamicTextureData::Create(pkTexture);
    if (!pkDynTxtrData)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::CreatePaletteRendererData(NiPalette*)
{
    // Palettes not supported in D3D10.
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDepthStencilRendererData(
    NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat,
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    return CreateDepthStencilRendererData(pkDSBuffer, pkFormat, eMSAAPref, 
        false);
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::CreateDepthStencilRendererData(
    NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat,
    Ni2DBuffer::MultiSamplePreference eMSAAPref, bool bCubeMap)
{
    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call "
            "CreateDepthStencilRendererData without a valid device.");
        return false;
    }

    if (pkDSBuffer == NULL)
        return false;

    DXGI_FORMAT eFormat;
    if (pkFormat)
        eFormat = NiD3D10PixelFormat::DetermineDXGIFormat(*pkFormat);
    else
        eFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    unsigned int uiMSAACount;
    unsigned int uiMSAAQuality;

    Ni2DBuffer::GetMSAACountAndQualityFromPref(eMSAAPref,
        uiMSAACount, uiMSAAQuality);

    D3D10_DEPTH_STENCIL_VIEW_DESC kDSVDesc;
    kDSVDesc.Format = eFormat;

    ID3D10Texture2D* pkDSTexture = NULL;
    if (bCubeMap)
    {
        pkDSTexture = m_pkResourceManager->CreateTexture2D(
            pkDSBuffer->GetWidth(),
            pkDSBuffer->GetHeight(), 
            1, 
            6, 
            eFormat, 
            uiMSAACount, 
            uiMSAAQuality,
            D3D10_USAGE_DEFAULT, 
            D3D10_BIND_DEPTH_STENCIL, 
            0, 
            D3D10_RESOURCE_MISC_TEXTURECUBE);
    
        if (uiMSAACount == 1)
        {
            kDSVDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DARRAY;
            kDSVDesc.Texture2DArray.MipSlice = 0;
            kDSVDesc.Texture2DArray.FirstArraySlice = 0;
            kDSVDesc.Texture2DArray.ArraySize = 1;
        }
        else
        {
            kDSVDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY;
            kDSVDesc.Texture2DMSArray.FirstArraySlice = 0;
            kDSVDesc.Texture2DMSArray.ArraySize = 1;
        }
    }
    else
    {
        pkDSTexture = m_pkResourceManager->CreateTexture2D(
            pkDSBuffer->GetWidth(),
            pkDSBuffer->GetHeight(), 
            1, 
            1, 
            eFormat, 
            uiMSAACount, 
            uiMSAAQuality,
            D3D10_USAGE_DEFAULT, 
            D3D10_BIND_DEPTH_STENCIL, 
            0, 
            0);

        if (uiMSAACount == 1)
        {
            kDSVDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
            kDSVDesc.Texture2D.MipSlice = 0;
        }
        else
        {
            kDSVDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
        }
    }

    if (pkDSTexture == NULL)
    {
        // Resource creation would have thrown error message
        NiD3D10Error::ReportWarning("CreateDepthStencilRendererData "
            "failed because depth/stencil texture could not be created.");
        return false;
    }



    NiD3D10DepthStencilBufferData* pkRendererData = 
        NiD3D10DepthStencilBufferData::Create(pkDSTexture, pkDSBuffer, 
        &kDSVDesc);

    // Reference to DS texture kept in NiD3D10DepthStencilBufferData
    pkDSTexture->Release();

    if (pkRendererData == NULL)
    {
        NiD3D10Error::ReportWarning("CreateDepthStencilRendererData "
            "failed because NiD3D10DepthStencilBufferData could not "
            "be created; destroying depth/stencil texture.");
        return false;
    }
    else
    {
        return true;
    }
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveRenderedCubeMapData(NiRenderedCubeMap* pkTexture)
{
    pkTexture->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveRenderedTextureData(NiRenderedTexture* pkTexture)
{
    pkTexture->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::RemoveDynamicTextureData(NiDynamicTexture* pkTexture)
{
    pkTexture->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
void* NiD3D10Renderer::LockDynamicTexture(
    const NiTexture::RendererData* pkRData,
    int& iPitch)
{
    void* pvMem = NULL;
    iPitch = 0; // Initialize in case there's an error return.

    NiD3D10TextureData* pkTxtrData = (NiD3D10TextureData*)(pkRData);
    if (!pkTxtrData)
        return NULL;

    // Call GetAsDynamicTexture() so app won't crash in case this function
    // gets called using a non-dynamic texture, accidentally.
    NiD3D10DynamicTextureData* pkDynTxtrData = 
        pkTxtrData->GetAsDynamicTexture();
    if (!pkDynTxtrData)
        return NULL;

    // Sanity check - error return if dynamic texture is already locked.
    if (pkDynTxtrData->IsLocked())
        return NULL;

    if (!pkDynTxtrData->GetD3DTexture())
    {
        NiTexture *pkDynamicTexture = 
            ((NiTexture::RendererData*) pkDynTxtrData )->GetTexture();
        if (!CreateDynamicTextureRendererData((NiDynamicTexture*)
            pkDynamicTexture))
        {
            NIASSERT(false);
        }
    }
    if (pkDynTxtrData)
    {
        pvMem = pkDynTxtrData->Lock(iPitch);
    }

    pkDynTxtrData->SetLocked(true);
    
    return pvMem;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::UnLockDynamicTexture(
    const NiTexture::RendererData* pkRData)
{
    NiD3D10TextureData* pkTxtrData = (NiD3D10TextureData*)(pkRData);
    if (!pkTxtrData)
        return false;

    NiD3D10DynamicTextureData *pkTexture = pkTxtrData->GetAsDynamicTexture();
    if (pkTexture && pkTexture->IsLocked())
    {
        bool bRet = pkTexture->UnLock();
        if(bRet)
            pkTexture->SetLocked(false);
        return bRet;
    }
    return false;
}
//---------------------------------------------------------------------------
NiShader* NiD3D10Renderer::GetFragmentShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiD3D10FragmentShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
NiShader* NiD3D10Renderer::GetShadowWriteShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiD3D10ShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::SetDefaultProgramCache(NiFragmentMaterial* pkMaterial, 
    bool bAutoWriteToDisk, bool bWriteDebugFile, bool bLoad,
    bool bNoNewProgramCreation, const char* pcWorkingDir)
{
    NIASSERT(m_pkD3D10Device)
    const char* pcProfile = D3D10GetVertexShaderProfile(m_pkD3D10Device);
    NiGPUProgram::ProgramType eType = NiGPUProgram::PROGRAM_VERTEX;

    if (pcWorkingDir == NULL)
        pcWorkingDir = NiMaterial::GetDefaultWorkingDirectory();

    NiD3D10GPUProgramCache* pkCache = 
        NiNew NiD3D10GPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);

    pcProfile = D3D10GetGeometryShaderProfile(m_pkD3D10Device);
    eType = NiGPUProgram::PROGRAM_GEOMETRY;
    pkCache = 
        NiNew NiD3D10GPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);

    pcProfile = D3D10GetPixelShaderProfile(m_pkD3D10Device);
    eType = NiGPUProgram::PROGRAM_PIXEL;
    pkCache = 
        NiNew NiD3D10GPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);
}

//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_BeginFrame()
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_bDeviceRemoved)
    {
        return false;
    }
    else if (m_bDeviceOccluded)
    {
        // Check device state only when the device is already occluded.
        if (m_spDefaultRenderTargetGroup == NULL)
            return false;
        NIASSERT(m_spDefaultRenderTargetGroup->GetBuffer(0));
        Ni2DBuffer::RendererData* pkBackBufferData = 
            m_spDefaultRenderTargetGroup->GetBufferRendererData(0);
        HandleDisplayFrameResult(
            ((NiD3D102DBufferData*)pkBackBufferData)->DisplayFrame(0, true));
    }

    while (m_kBuffersToUseAtDisplayFrame.GetSize())
        m_kBuffersToUseAtDisplayFrame.RemoveHead();

    PerformPrecache();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_EndFrame()
{
    NIASSERT_D3D10_DEVICE_THREAD;

//CODEBLOCK(4) - DO NOT DELETE THIS LINE

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_DisplayFrame()
{
    NIASSERT_D3D10_DEVICE_THREAD;

    while (m_kBuffersToUseAtDisplayFrame.GetSize())
    {
        NiD3D102DBufferDataPtr spBuffer = 
            m_kBuffersToUseAtDisplayFrame.RemoveHead();

        HRESULT hr = spBuffer->DisplayFrame(m_uiSyncInterval);
        if (HandleDisplayFrameResult(hr) == false)
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::HandleDisplayFrameResult(HRESULT hr)
{
    if (hr == DXGI_STATUS_OCCLUDED)
    {
        if (m_bDeviceOccluded == false)
        {
            // Signal application that it is now occluded
            const unsigned int uiFuncCount = m_kOccludedNotifyFuncs.GetSize();
            for (unsigned int i = 0; i < uiFuncCount; i++)
            {
                OCCLUDEDNOTIFYFUNC pfnFunc = m_kOccludedNotifyFuncs.GetAt(i);
                void* pvData = m_kOccludedNotifyFuncData.GetAt(i);
                if (pfnFunc)
                {
                    bool bResult = (*pfnFunc)(true, pvData);

                    if (bResult == false)
                    {
                        return false;
                    }
                }
            }

            m_bDeviceOccluded = true;
        }
    }
    else if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            hr = m_pkD3D10Device->GetDeviceRemovedReason();
            if (hr == DXGI_ERROR_DEVICE_REMOVED)
            {
                NiD3D10Error::ReportWarning(
                    "ID3D10Device::GetDeviceRemovedReason reports the "
                    "device has been removed. The D3D10 device must be "
                    "shut down and recreated, though the original "
                    "adapter may be missing.");
            }
            else if (hr == DXGI_ERROR_DEVICE_HUNG)
            {
                NiD3D10Error::ReportWarning(
                    "ID3D10Device::GetDeviceRemovedReason reports the "
                    "device has hung. The D3D10 device must be shut down "
                    "and recreated. The application can continue as "
                    "usual, but it is recommended that the graphics usage "
                    "be scaled back.");
            }
            else if (hr == DXGI_ERROR_DEVICE_RESET)
            {
                NiD3D10Error::ReportWarning(
                    "ID3D10Device::GetDeviceRemovedReason reports the "
                    "device has crashed and been reset by another "
                    "application. The D3D10 device must be shut down and "
                    "recreated.");
            }
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DISPLAY_SWAP_CHAIN_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        if (m_bDeviceRemoved == false)
        {
            const unsigned int uiFuncCount = 
                m_kDeviceRemovedNotifyFuncs.GetSize();
            for (unsigned int i = 0; i < uiFuncCount; i++)
            {
                DEVICEREMOVEDNOTIFYFUNC pfnFunc = 
                    m_kDeviceRemovedNotifyFuncs.GetAt(i);
                void* pvData = m_kDeviceRemovedNotifyFuncData.GetAt(i);
                if (pfnFunc)
                {
                    bool bResult = (*pfnFunc)(pvData);

                    if (bResult == false)
                    {
                        return false;
                    }
                }
            }
            m_bDeviceRemoved = true;
        }
        return false;
    }
    else if (m_bDeviceOccluded)
    {
        // Signal application that it is no longer occluded
        const unsigned int uiFuncCount = 
            m_kOccludedNotifyFuncs.GetSize();
        for (unsigned int i = 0; i < uiFuncCount; i++)
        {
            OCCLUDEDNOTIFYFUNC pfnFunc = m_kOccludedNotifyFuncs.GetAt(i);
            void* pvData = m_kOccludedNotifyFuncData.GetAt(i);
            if (pfnFunc)
            {
                bool bResult = (*pfnFunc)(false, pvData);

                if (bResult == false)
                {
                    return false;
                }
            }
        }
        m_bDeviceOccluded = false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_ClearBuffer(const NiRect<float>*, 
    unsigned int uiMode)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call ClearBuffer without "
            "a valid device.");
        return;
    }

    // This should be verified in ClearBuffer
    NIASSERT(m_pkCurrentRenderTargetGroup);

    if ((uiMode & NiRenderer::CLEAR_BACKBUFFER) != 0)
    {
        for (unsigned int i = 0; 
            i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
        {
            Ni2DBuffer* pk2DBuffer = 
                m_pkCurrentRenderTargetGroup->GetBuffer(i);
            if (pk2DBuffer == NULL)
                continue;

            NiD3D102DBufferData* pk2DBufferData = 
                (NiD3D102DBufferData*)pk2DBuffer->GetRendererData();
            NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, 
                pk2DBufferData));

            NiD3D10RenderTargetBufferData* pkRTBufferData = 
                (NiD3D10RenderTargetBufferData*)pk2DBufferData;

            ID3D10RenderTargetView* pkRTView = 
                pkRTBufferData->GetRenderTargetView();
            if (pkRTView)
            {
                m_pkD3D10Device->ClearRenderTargetView(pkRTView, 
                    m_afBackgroundColor);
            }
        }
    }


    unsigned int uiDSClearModes = 0;
    if ((uiMode & NiRenderer::CLEAR_ZBUFFER) != 0)
        uiDSClearModes |= D3D10_CLEAR_DEPTH;
    if ((uiMode & NiRenderer::CLEAR_STENCIL) != 0)
        uiDSClearModes |= D3D10_CLEAR_STENCIL;

    if (uiDSClearModes != 0)
    {
        NiDepthStencilBuffer* pkDSBuffer = 
            m_pkCurrentRenderTargetGroup->GetDepthStencilBuffer();
        if (pkDSBuffer)
        {
            NiD3D102DBufferData* pk2DBufferData = 
                (NiD3D102DBufferData*)pkDSBuffer->GetRendererData();
            NIASSERT(NiIsKindOf(NiD3D10DepthStencilBufferData, 
                pk2DBufferData));

            NiD3D10DepthStencilBufferData* pkDSBufferData = 
                (NiD3D10DepthStencilBufferData*)pk2DBufferData;

            ID3D10DepthStencilView* pkDSView = 
                pkDSBufferData->GetDepthStencilView();
            if (pkDSView)
            {
                m_pkD3D10Device->ClearDepthStencilView(pkDSView,
                    uiDSClearModes, m_fDepthClear, m_ucStencilClear);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_SetCameraData(const NiPoint3& kWorldLoc,
    const NiPoint3& kWorldDir, const NiPoint3& kWorldUp, 
    const NiPoint3& kWorldRight, const NiFrustum& kFrustum, 
    const NiRect<float>& kPort)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    // This should be verified in SetCameraData
    NIASSERT(m_pkCurrentRenderTargetGroup);

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call SetCameraData without "
            "a valid device.");
        return;
    }

    // View matrix update
    m_kD3DView._11 = kWorldRight.x;    
    m_kD3DView._12 = kWorldUp.x;    
    m_kD3DView._13 = kWorldDir.x;
    m_kD3DView._14 = 0.0f;
    m_kD3DView._21 = kWorldRight.y; 
    m_kD3DView._22 = kWorldUp.y;    
    m_kD3DView._23 = kWorldDir.y;
    m_kD3DView._24 = 0.0f;
    m_kD3DView._31 = kWorldRight.z; 
    m_kD3DView._32 = kWorldUp.z;    
    m_kD3DView._33 = kWorldDir.z;
    m_kD3DView._34 = 0.0f;
    m_kD3DView._41 = -(kWorldRight * kWorldLoc);
    m_kD3DView._42 = -(kWorldUp * kWorldLoc);
    m_kD3DView._43 = -(kWorldDir * kWorldLoc);
    m_kD3DView._44 = 1.0f;

    m_kInvView._11 = kWorldRight.x;
    m_kInvView._12 = kWorldRight.y;
    m_kInvView._13 = kWorldRight.z;
    m_kInvView._14 = 0.0f;
    m_kInvView._21 = kWorldUp.x;
    m_kInvView._22 = kWorldUp.y;
    m_kInvView._23 = kWorldUp.z;
    m_kInvView._24 = 0.0f;
    m_kInvView._31 = kWorldDir.x;
    m_kInvView._32 = kWorldDir.y;
    m_kInvView._33 = kWorldDir.z;
    m_kInvView._34 = 0.0f;
    m_kInvView._41 = kWorldLoc.x;
    m_kInvView._42 = kWorldLoc.y;
    m_kInvView._43 = kWorldLoc.z;
    m_kInvView._44 = 1.0f;

    float fDepthRange = kFrustum.m_fFar - kFrustum.m_fNear;

    // Projection matrix update
    float fRmL = kFrustum.m_fRight - kFrustum.m_fLeft;
    float fRpL = kFrustum.m_fRight + kFrustum.m_fLeft;
    float fTmB = kFrustum.m_fTop - kFrustum.m_fBottom;
    float fTpB = kFrustum.m_fTop + kFrustum.m_fBottom;
    float fInvFmN = 1.0f / fDepthRange;

    if (kFrustum.m_bOrtho)
    {
        if (m_bLeftRightSwap)
        {
            m_kD3DProj._11 = -2.0f / fRmL;
            m_kD3DProj._21 = 0.0f;
            m_kD3DProj._31 = 0.0f;      
            m_kD3DProj._41 = fRpL / fRmL; 
        }
        else
        {
            m_kD3DProj._11 = 2.0f / fRmL;
            m_kD3DProj._21 = 0.0f;
            m_kD3DProj._31 = 0.0f; 
            m_kD3DProj._41 = -fRpL / fRmL; 
        }

        m_kD3DProj._12 = 0.0f;
        m_kD3DProj._22 = 2.0f / fTmB;
        m_kD3DProj._32 = 0.0f; 
        m_kD3DProj._42 = -fTpB / fTmB;
        m_kD3DProj._13 = 0.0f;
        m_kD3DProj._23 = 0.0f;
        m_kD3DProj._33 = fInvFmN; 
        m_kD3DProj._43 = -(kFrustum.m_fNear * fInvFmN); 

        // A "w-friendly" projection matrix to make fog, w-buffering work
        m_kD3DProj._14 = 0.0f;
        m_kD3DProj._24 = 0.0f;
        m_kD3DProj._34 = 0.0f;
        m_kD3DProj._44 = 1.0f;               
    }
    else
    {
        if (m_bLeftRightSwap)
        {
            m_kD3DProj._11 = -2.0f / fRmL;
            m_kD3DProj._21 = 0.0f;
            m_kD3DProj._31 = fRpL / fRmL;
            m_kD3DProj._41 = 0.0f;
        }
        else
        {
            m_kD3DProj._11 = 2.0f / fRmL;
            m_kD3DProj._21 = 0.0f;
            m_kD3DProj._31 = -fRpL / fRmL;
            m_kD3DProj._41 = 0.0f;
        }
        m_kD3DProj._12 = 0.0f;
        m_kD3DProj._22 = 2.0f / fTmB;
        m_kD3DProj._32 = -fTpB / fTmB;
        m_kD3DProj._42 = 0.0f;
        m_kD3DProj._13 = 0.0f;
        m_kD3DProj._23 = 0.0f;
        m_kD3DProj._33 = kFrustum.m_fFar * fInvFmN;
        m_kD3DProj._43 = -(kFrustum.m_fNear * kFrustum.m_fFar * fInvFmN);

        // A "w-friendly" projection matrix to make fog, w-buffering work
        m_kD3DProj._14 = 0.0f;
        m_kD3DProj._24 = 0.0f;
        m_kD3DProj._34 = 1.0f;
        m_kD3DProj._44 = 0.0f;
    }

    // Viewport update

    D3D10_VIEWPORT kViewPort;
    float fWidth = (float)m_pkCurrentRenderTargetGroup->GetWidth(0);
    float fHeight = (float)m_pkCurrentRenderTargetGroup->GetHeight(0);

    kViewPort.TopLeftX = (unsigned int)(kPort.m_left * fWidth);
    kViewPort.TopLeftY = (unsigned int)((1.0f - kPort.m_top) * fHeight);
    kViewPort.Width = 
        (unsigned int)((kPort.m_right - kPort.m_left) * fWidth);
    kViewPort.Height = 
        (unsigned int)((kPort.m_top - kPort.m_bottom) * fHeight);
    kViewPort.MinDepth = 0.0f;     
    kViewPort.MaxDepth = 1.0f;

    m_pkD3D10Device->RSSetViewports(1, &kViewPort);

    m_kCurrentFrustum = kFrustum;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_SetScreenSpaceCameraData(const NiRect<float>* pkPort)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    // This should be verified in SetScreenSpaceCameraData
    NIASSERT(m_pkCurrentRenderTargetGroup);

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call SetScreenSpaceCameraData "
            "without a valid device.");
        return;
    }

    // Screen elements are in normalized display coordinates, (x,y), where 
    // 0 <= x <= 1 
    // 0 <= y <= 1
    // 0 <= z <= 9999
    // The screen buffer width is w and the screen height is h.
    // The camera parameters are:
    // Camera location: (1/2, 1/2, -1)
    // Camera right:    (1,  0, 0)
    // Camera up:       (0, -1, 0)
    // Camera dir:      (0,  0, 1)
    // Camera frustum:
    //      Left:   -1/2
    //      Right:  1/2
    //      Top:    1/2
    //      Bottom: -1/2
    //      Near:   1
    //      Far:    10000

    // View matrix update
    m_kD3DView._11 = 1.0f;    
    m_kD3DView._12 = 0.0f;    
    m_kD3DView._13 = 0.0f;
    m_kD3DView._14 = 0.0f;
    m_kD3DView._21 = 0.0f; 
    m_kD3DView._22 = -1.0f;    
    m_kD3DView._23 = 0.0f;
    m_kD3DView._24 = 0.0f;
    m_kD3DView._31 = 0.0f; 
    m_kD3DView._32 = 0.0f;    
    m_kD3DView._33 = 1.0f;
    m_kD3DView._34 = 0.0f;
    m_kD3DView._41 = -0.5f;
    m_kD3DView._42 = 0.5f;
    m_kD3DView._43 = 1.0f;
    m_kD3DView._44 = 1.0f;

    m_kInvView._11 = 1.0f;
    m_kInvView._12 = 0.0f;
    m_kInvView._13 = 0.0f;
    m_kInvView._14 = 0.0f;
    m_kInvView._21 = 0.0f;
    m_kInvView._22 = -1.0f;
    m_kInvView._23 = 0.0f;
    m_kInvView._24 = 0.0f;
    m_kInvView._31 = 0.0f;
    m_kInvView._32 = 0.0f;
    m_kInvView._33 = 1.0f;
    m_kInvView._34 = 0.0f;
    m_kInvView._41 = 0.5f;
    m_kInvView._42 = 0.5f;
    m_kInvView._43 = -1.0f;
    m_kInvView._44 = 1.0f;

    const float fNearDepthDivDepthRange = 1.0f / 9999.0f;

    if (m_bLeftRightSwap)
    {
        m_kD3DProj._11 = -2.0f;
    }
    else
    {
        m_kD3DProj._11 = 2.0f;
    }
    m_kD3DProj._21 = 0.0f;
    m_kD3DProj._31 = 0.0f; 
    m_kD3DProj._41 = 0.0f; 
    m_kD3DProj._12 = 0.0f;
    m_kD3DProj._22 = 2.0f;
    m_kD3DProj._32 = 0.0f; 
    m_kD3DProj._42 = 0.0f;
    m_kD3DProj._13 = 0.0f;
    m_kD3DProj._23 = 0.0f;
    m_kD3DProj._33 = fNearDepthDivDepthRange; 
    m_kD3DProj._43 = -fNearDepthDivDepthRange; 
    m_kD3DProj._14 = 0.0f;
    m_kD3DProj._24 = 0.0f;
    m_kD3DProj._34 = 0.0f;
    m_kD3DProj._44 = 1.0f;               

    // Viewport update
    D3D10_VIEWPORT kViewPort;

    unsigned int uiWidth = m_pkCurrentRenderTargetGroup->GetWidth(0);
    unsigned int uiHeight = m_pkCurrentRenderTargetGroup->GetHeight(0);

    if (pkPort)
    {
        float fWidth = (float)uiWidth;
        float fHeight = (float)uiHeight;
        kViewPort.TopLeftX = (unsigned int)(pkPort->m_left * fWidth);
        kViewPort.TopLeftY = (unsigned int)((1.0f - pkPort->m_top) * fHeight);
        kViewPort.Width = 
            (unsigned int)((pkPort->m_right - pkPort->m_left) * fWidth);
        kViewPort.Height = 
            (unsigned int)((pkPort->m_top - pkPort->m_bottom) * fHeight);

        // Set cached port.
        m_kCurrentViewPort = *pkPort;
    }
    else
    {
        kViewPort.TopLeftX = 0;
        kViewPort.TopLeftY = 0;
        kViewPort.Width = uiWidth;
        kViewPort.Height = uiHeight;

        // Set cached port.
        m_kCurrentViewPort = NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f);
    }

    kViewPort.MinDepth = 0.0f;     
    kViewPort.MaxDepth = 1.0f;

    m_pkD3D10Device->RSSetViewports(1, &kViewPort);

    // Set cached frustum.
    m_kCurrentFrustum.m_fLeft = -0.5f;
    m_kCurrentFrustum.m_fRight = 0.5f;
    m_kCurrentFrustum.m_fTop = 0.5f;
    m_kCurrentFrustum.m_fBottom = -0.5f;
    m_kCurrentFrustum.m_fNear = 1.0f;
    m_kCurrentFrustum.m_fFar = 10000.0f;
    m_kCurrentFrustum.m_bOrtho = true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_GetCameraData(NiPoint3& kWorldLoc, 
    NiPoint3& kWorldDir, NiPoint3& kWorldUp, NiPoint3& kWorldRight, 
    NiFrustum& kFrustum, NiRect<float>& kPort)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    kWorldRight.x = m_kInvView._11;
    kWorldRight.y = m_kInvView._12;
    kWorldRight.z = m_kInvView._13;

    kWorldUp.x = m_kInvView._21;
    kWorldUp.y = m_kInvView._22;
    kWorldUp.z = m_kInvView._23;

    kWorldDir.x = m_kInvView._31;
    kWorldDir.y = m_kInvView._32;
    kWorldDir.z = m_kInvView._33;

    kWorldLoc.x = m_kInvView._41;
    kWorldLoc.y = m_kInvView._42;
    kWorldLoc.z = m_kInvView._43;

    kFrustum = m_kCurrentFrustum;
    kPort = m_kCurrentViewPort;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_BeginUsingRenderTargetGroup(
    NiRenderTargetGroup* pkTarget, unsigned int uiClearMode)
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call BeginUsingRenderTargetGroup "
            "without a valid device.");
        return false;
    }

    // This should have been checked in BeginUsingRenderTargetGroup
    NIASSERT(pkTarget);

    NIASSERT(ValidateRenderTargetGroup(pkTarget));

    ID3D10RenderTargetView* apkRTViews[D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT];
    memset(apkRTViews, 0, sizeof(apkRTViews));

    for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
    {
        Ni2DBuffer* pk2DBuffer = pkTarget->GetBuffer(i);
        if (pk2DBuffer == NULL)
            continue;

        NiD3D102DBufferData* pk2DBufferData = 
            (NiD3D102DBufferData*)pk2DBuffer->GetRendererData();
        NIASSERT(NiIsKindOf(NiD3D10RenderTargetBufferData, 
            pk2DBufferData));

        NiD3D10RenderTargetBufferData* pkRTBufferData = 
            (NiD3D10RenderTargetBufferData*)pk2DBufferData;

        apkRTViews[i] = pkRTBufferData->GetRenderTargetView();
    }

    ID3D10DepthStencilView* pkDSView = NULL;
    NiDepthStencilBuffer* pkDSBuffer = pkTarget->GetDepthStencilBuffer();
    if (pkDSBuffer)
    {
        NiD3D102DBufferData* pk2DBufferData = 
            (NiD3D102DBufferData*)pkDSBuffer->GetRendererData();
        NIASSERT(NiIsKindOf(NiD3D10DepthStencilBufferData, 
            pk2DBufferData));

        NiD3D10DepthStencilBufferData* pkDSBufferData = 
            (NiD3D10DepthStencilBufferData*)pk2DBufferData;

        pkDSView = pkDSBufferData->GetDepthStencilView();
    }

    m_pkD3D10Device->OMSetRenderTargets(D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT,
        apkRTViews, pkDSView);

    m_pkCurrentRenderTargetGroup = pkTarget;

    Do_ClearBuffer(NULL, uiClearMode);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Renderer::Do_EndUsingRenderTargetGroup()
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call EndUsingRenderTargetGroup "
            "without a valid device.");
        return false;
    }

    // This should have been checked in EndUsingRenderTargetGroup
    NIASSERT(m_pkCurrentRenderTargetGroup);

    for (unsigned int i = 0; i < 
        m_pkCurrentRenderTargetGroup->GetBufferCount(); i++)
    {
        NiD3D102DBufferData* pkBuffData = (NiD3D102DBufferData*)
            m_pkCurrentRenderTargetGroup->GetBufferRendererData(i);

        if (pkBuffData == NULL)
            continue;

        // Store for DisplayFrame
        if (pkBuffData->CanDisplayFrame())
        {
            // Only store once
            if (!m_kBuffersToUseAtDisplayFrame.FindPos(pkBuffData))
                m_kBuffersToUseAtDisplayFrame.AddTail(pkBuffData);
        }
    }

    m_pkCurrentRenderTargetGroup = NULL;
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Renderer::Do_RenderMesh(NiMesh* pkMesh) 
{
    NIASSERT_D3D10_DEVICE_THREAD;

    if (m_pkD3D10Device == NULL)
    {
        NiD3D10Error::ReportWarning("Can't call RenderMesh without "
            "a valid device.");
        return;
    }

    NIASSERT(pkMesh);

    LockRenderer();

    // GetShaderAndVertexDecl will automatically increment the ref count
    // on the NiD3D10VertexDeclaration so it can be decremented at the end
    // of this function.
    NiD3D10MeshMaterialBindingPtr spMMB;
    NiD3D10ShaderInterface* pkShader = GetShaderAndVertexDecl(pkMesh, 
        spMMB);
    NIASSERT(pkShader);
    NIASSERT(m_pkCurrProp);

    if (spMMB == NULL)
    {
        // Can't render without a valid vertex declaration
        return;
    }

    NiRenderCallContext kRCC;
    kRCC.m_pkMesh = pkMesh;
    kRCC.m_pkWorldBound = &pkMesh->GetWorldBound();
    kRCC.m_pkEffects = m_pkCurrEffects;
    kRCC.m_pkState = m_pkCurrProp;
    kRCC.m_pkWorld = &pkMesh->GetWorldTransform();
    kRCC.m_pkMeshMaterialBinding = 
        NiSmartPointerCast(NiVertexDeclarationCache, spMMB);

    // Preprocess the pipeline
    unsigned int uiRet = pkShader->PreProcessPipeline(kRCC);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(kRCC);

    // Get the index buffer, if there is one
    bool bDrawAuto = pkMesh->GetInputDataIsFromStreamOut();
    bool bIndexed;

    const NiDataStreamRef* pkStreamRef = pkMesh->GetFirstStreamRef(
        NiDataStream::USAGE_VERTEX_INDEX, NiDataStream::ACCESS_GPU_READ);

    if (bDrawAuto)
    {
        bIndexed = false;
    }
    else if (pkStreamRef)
    {
        bIndexed = true;
    }
    else
    {
        // No index buffer
        bIndexed = false;
        pkStreamRef = pkMesh->GetFirstUsageVertexPerVertexStreamRef();

        if (pkStreamRef == NULL)
        {
            NiD3D10Error::ReportWarning("No vertex streams found to "
                "render with on '%s' object, pointer: %x; skipping render.",
                pkMesh->GetName(), 
                pkMesh);
            return;
        }
    }

    // Query for instancing
    NiUInt32 uiInstances = (pkMesh->GetInstanced() ? 
        NiInstancingUtilities::GetVisibleInstanceCount(pkMesh) : 1);

    // Get active submesh count
    NiUInt32 uiSubmeshCount = 0;
    if (pkMesh->GetInstanced())
        uiSubmeshCount = NiInstancingUtilities::GetVisibleSubmeshCount(pkMesh);
    else
        uiSubmeshCount = pkMesh->GetSubmeshCount();

    // Iterate over passes
    unsigned int uiRemainingPasses = pkShader->FirstPass();
    while (uiRemainingPasses != 0)
    {
        // Setup the rendering pass
        uiRet = pkShader->SetupRenderingPass(kRCC);

        if (bDrawAuto && uiSubmeshCount > 1)
        {
            NiD3D10Error::ReportWarning("DrawAuto does not yet work with "
                "submeshes.\nNiRenderObject '%s', pointer: %x; ",
                pkMesh->GetName(), pkMesh);
            continue;
        }

        // Iterate over submesh regions
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshCount; ++uiSubmesh)
        {
            kRCC.m_uiSubmesh = uiSubmesh;

            unsigned int uiElementCount = 0; 
            unsigned int uiPrimitiveCount = 0;
            if (!bDrawAuto)
            {
                uiElementCount = pkStreamRef->GetCount(uiSubmesh);
                uiPrimitiveCount = 
                    pkMesh->GetPrimitiveCountFromElementCount(uiElementCount);
            }

            if (uiPrimitiveCount == 0 && !bDrawAuto)
            {
                NiD3D10Error::ReportWarning("Zero primitive count in "
                    "submesh %d of NiRenderObject '%s' pointer: %x; "
                    "skipping render.",
                    uiSubmesh,
                    pkMesh->GetName(),
                    pkMesh);
            }
            else
            {
                // Set the transformations
                uiRet = pkShader->SetupTransformations(kRCC);

                // Set the shader programs
                // This is to give the shader final 'override' authority
                uiRet = pkShader->SetupShaderPrograms(kRCC);
                if (uiRet != 0)
                    continue;

                // Setup the current submesh
                uiRet = pkShader->PreRenderSubmesh(kRCC);
                if (uiRet != 0)
                    continue;

                // Apply accumulated state, but only for first submesh
                if (uiSubmesh == 0)
                {
                    m_pkRenderStateManager->ApplyCurrentState(
                        pkShader->GetStateBlockMask(kRCC));
                }

                // Apply constants
                m_pkShaderConstantManager->ApplyShaderConstants();

                // When drawing, don't use index offsets - byte offsets are 
                // specified when setting the streams in 
                // NiShader::PreRenderSubmesh

                if (bDrawAuto)
                {
                    if (uiInstances > 1)
                    {
                        NiD3D10Error::ReportWarning(
                            "Instancing is not supported with"
                            " DrawAuto!\nobject \"%s\" pointer: %x\n",
                            pkMesh->GetName(), pkMesh);
                    }
                    else if (bIndexed) 
                    {
                        NiD3D10Error::ReportWarning(
                            "DrawAuto does not support indexed draw!\n"
                            " object \"%s\" pointer: %x\n",
                            pkMesh->GetName(), pkMesh);
                    }
                    else
                    {
                        m_pkD3D10Device->DrawAuto();
                    }
                }
                else if (bIndexed)
                {
                    if (uiInstances > 1)
                    {
                        m_pkD3D10Device->DrawIndexedInstanced(
                            uiElementCount,
                            uiInstances, 
                            0,  // StartIndexLocation
                            0,  // BaseVertexLocation
                            0); // StartInstanceLocation
                    }
                    else
                    {
                        m_pkD3D10Device->DrawIndexed(
                            uiElementCount,
                            0,  // StartIndexLocation
                            0); // BaseVertexLocation
                    }
                }
                else
                {
                    if (uiInstances > 1)
                    {
                        m_pkD3D10Device->DrawInstanced(
                            uiElementCount,
                            uiInstances, 
                            0,  // StartVertexLocation
                            0); // StartInstanceLocation
                    }
                    else
                    {
                        m_pkD3D10Device->Draw(
                            uiElementCount,
                            0); // StartVertexLocation
                    }
                }

                pkShader->PostRenderSubmesh(kRCC);
            }

            // Perform any post-rendering steps
            uiRet = pkShader->PostRender(kRCC);
        }

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();        
        kRCC.m_uiPass++;
    }

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(kRCC);

    UnlockRenderer();
}
//---------------------------------------------------------------------------
NiD3D10ShaderInterface* NiD3D10Renderer::GetShaderAndVertexDecl(
    NiRenderObject* pkMesh, NiD3D10MeshMaterialBindingPtr& spMMB)
{
    NiD3D10ShaderInterface* pkShader = 
        GetShaderAndVertexDecl_NoErrorShader(pkMesh, spMMB);

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("No shader found for object \"%s\" pointer: %x\n"
            "Using Error Shader!\n", pkMesh->GetName(), pkMesh);
        NIASSERT(NiIsKindOf(NiD3D10ShaderInterface, GetErrorShader()));
        pkShader = (NiD3D10ShaderInterface*)GetErrorShader();

        spMMB = NiD3D10MeshMaterialBinding::Create(pkMesh, 
            pkShader->GetSemanticAdapterTable());
    }

    NIASSERT(pkShader);
    return pkShader;
}
//---------------------------------------------------------------------------
NiD3D10ShaderInterface* NiD3D10Renderer::GetShaderAndVertexDecl_NoErrorShader(
    NiRenderObject* pkMesh, NiD3D10MeshMaterialBindingPtr& spMMB)
{
    NIASSERT(pkMesh);

    bool bNoActiveMaterial_UseDefault = pkMesh->GetActiveMaterial() == NULL;
    if (bNoActiveMaterial_UseDefault)
        pkMesh->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);

    NiD3D10ShaderInterface* pkShader = NiVerifyStaticCast(
        NiD3D10ShaderInterface, pkMesh->GetShaderFromMaterial());

    // Grab vertex declaration cache
    const NiMaterialInstance* pkMatInst = pkMesh->GetActiveMaterialInstance();
    spMMB = 
        (NiD3D10MeshMaterialBinding*)pkMatInst->GetVertexDeclarationCache();

    // Restore original material if necessary
    if (bNoActiveMaterial_UseDefault)
    {
        pkMesh->SetActiveMaterial(NULL);
    }

    return pkShader;
}

//---------------------------------------------------------------------------
D3D10_PRIMITIVE_TOPOLOGY NiD3D10Renderer::GetD3D10TopologyFromPrimitiveType(
    NiPrimitiveType::Type eType, bool bAdjacency)
{
    switch (eType)
    {
    case NiPrimitiveType::PRIMITIVE_TRIANGLES:
        return (bAdjacency ? 
            D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ : 
            D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    case NiPrimitiveType::PRIMITIVE_TRISTRIPS:
        return (bAdjacency ? 
            D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ : 
            D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    case NiPrimitiveType::PRIMITIVE_LINES:
        return (bAdjacency ? 
            D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ : 
            D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    case NiPrimitiveType::PRIMITIVE_LINESTRIPS:
        return (bAdjacency ? 
            D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ : 
            D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
    case NiPrimitiveType::PRIMITIVE_POINTS:
        return D3D10_PRIMITIVE_TOPOLOGY_POINTLIST;
    }
    return D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateBlob(SIZE_T NumBytes, 
    LPD3D10BLOB* ppBuffer)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateBlob(NumBytes, ppBuffer);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateDevice(IDXGIAdapter* pAdapter, 
    D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, 
    UINT SDKVersion, ID3D10Device** ppDevice)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateDevice(pAdapter, DriverType, Software, Flags,
        SDKVersion, ppDevice);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, 
    D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, 
    UINT SDKVersion, DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, 
    IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateDeviceAndSwapChain(pAdapter, DriverType, Software,
        Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateStateBlock(ID3D10Device* pDevice, 
    D3D10_STATE_BLOCK_MASK* pStateBlockMask, ID3D10StateBlock** ppStateBlock)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateStateBlock(pDevice, pStateBlockMask, ppStateBlock);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10GetInputAndOutputSignatureBlob(
    CONST void* pShaderBytecode, SIZE_T BytecodeLength, 
    ID3D10Blob** ppSignatureBlob)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10GetInputAndOutputSignatureBlob(pShaderBytecode, 
        BytecodeLength, ppSignatureBlob);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10GetInputSignatureBlob(
    CONST void* pShaderBytecode, SIZE_T BytecodeLength, 
    ID3D10Blob** ppSignatureBlob)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10GetInputSignatureBlob(pShaderBytecode, BytecodeLength,
        ppSignatureBlob);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10GetOutputSignatureBlob(
    CONST void* pShaderBytecode, SIZE_T BytecodeLength, 
    ID3D10Blob** ppSignatureBlob)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10GetOutputSignatureBlob(pShaderBytecode, BytecodeLength,
        ppSignatureBlob);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskDifference(
    D3D10_STATE_BLOCK_MASK* pA, D3D10_STATE_BLOCK_MASK* pB, 
    D3D10_STATE_BLOCK_MASK* pResult)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskDifference(pA, pB, pResult);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskDisableAll(
    D3D10_STATE_BLOCK_MASK* pMask)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskDisableAll(pMask);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskDisableCapture(
    D3D10_STATE_BLOCK_MASK* pMask, D3D10_DEVICE_STATE_TYPES StateType, 
    UINT RangeStart, UINT RangeLength)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskDisableCapture(pMask, StateType, 
        RangeStart, RangeLength);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskEnableAll(
    D3D10_STATE_BLOCK_MASK* pMask)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskEnableAll(pMask);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskEnableCapture(
    D3D10_STATE_BLOCK_MASK* pMask, D3D10_DEVICE_STATE_TYPES StateType, 
    UINT RangeStart, UINT RangeLength)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskEnableCapture(pMask, StateType, RangeStart,
        RangeLength);
}
//---------------------------------------------------------------------------
BOOL NiD3D10Renderer::D3D10StateBlockMaskGetSetting(
    D3D10_STATE_BLOCK_MASK* pMask, D3D10_DEVICE_STATE_TYPES StateType, 
    UINT Entry)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskGetSetting(pMask, StateType, Entry);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskIntersect(
    D3D10_STATE_BLOCK_MASK* pA, D3D10_STATE_BLOCK_MASK* pB, 
    D3D10_STATE_BLOCK_MASK* pResult)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskIntersect(pA, pB, pResult);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10StateBlockMaskUnion(D3D10_STATE_BLOCK_MASK* pA, 
    D3D10_STATE_BLOCK_MASK* pB, D3D10_STATE_BLOCK_MASK* pResult)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10StateBlockMaskUnion(pA, pB, pResult);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen,
    LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, 
    LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags,
    ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CompileShader(pSrcData, SrcDataLen, pFileName, pDefines,
        pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10DisassembleShader(CONST UINT* pShader, 
    SIZE_T Size, BOOL EnableColorCode, LPCSTR pComments, 
    ID3D10Blob** ppDisassembly)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10DisassembleShader(pShader, Size, EnableColorCode, 
        pComments, ppDisassembly);
}
//---------------------------------------------------------------------------
LPCSTR NiD3D10Renderer::D3D10GetGeometryShaderProfile(ID3D10Device* pDevice)
{
    if (ms_hD3D10 == NULL)
        return NULL;

    return ms_pfnD3D10GetGeometryShaderProfile(pDevice);
}
//---------------------------------------------------------------------------
LPCSTR NiD3D10Renderer::D3D10GetPixelShaderProfile(ID3D10Device* pDevice)
{
    if (ms_hD3D10 == NULL)
        return NULL;

    return ms_pfnD3D10GetPixelShaderProfile(pDevice);
}
//---------------------------------------------------------------------------
LPCSTR NiD3D10Renderer::D3D10GetVertexShaderProfile(ID3D10Device* pDevice)
{
    if (ms_hD3D10 == NULL)
        return NULL;

    return ms_pfnD3D10GetVertexShaderProfile(pDevice);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10PreprocessShader(LPCSTR pSrcData, 
    SIZE_T SrcDataSize, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, 
    LPD3D10INCLUDE pInclude, ID3D10Blob** ppShaderText, 
    ID3D10Blob** ppErrorMsgs)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10PreprocessShader(pSrcData, SrcDataSize, pFileName, 
        pDefines, pInclude, ppShaderText, ppErrorMsgs);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10ReflectShader(CONST void* pShaderBytecode, 
    SIZE_T BytecodeLength, ID3D10ShaderReflection** ppReflector)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10ReflectShader(pShaderBytecode, BytecodeLength, 
        ppReflector);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CompileEffectFromMemory(void* pData, 
    SIZE_T DataLength, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO* pDefines,
    ID3D10Include* pInclude, UINT HLSLFlags, UINT FXFlags, 
    ID3D10Blob** ppCompiledEffect, ID3D10Blob** ppErrors)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CompileEffectFromMemory(pData, DataLength, pSrcFileName,
        pDefines, pInclude, HLSLFlags, FXFlags, ppCompiledEffect, ppErrors);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateEffectFromMemory(void* pData, 
    SIZE_T DataLength, UINT FXFlags, ID3D10Device* pDevice, 
    ID3D10EffectPool* pEffectPool, ID3D10Effect** ppEffect)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateEffectFromMemory(pData, DataLength, FXFlags, 
        pDevice, pEffectPool, ppEffect);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10CreateEffectPoolFromMemory(void* pData, 
    SIZE_T DataLength, UINT FXFlags, ID3D10Device* pDevice, 
    ID3D10EffectPool** ppEffectPool)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10CreateEffectPoolFromMemory(pData, DataLength, FXFlags, 
        pDevice, ppEffectPool);
}
//---------------------------------------------------------------------------
HRESULT NiD3D10Renderer::D3D10DisassembleEffect(ID3D10Effect* pEffect, 
    BOOL EnableColorCode, ID3D10Blob** ppDisassembly)
{
    if (ms_hD3D10 == NULL)
        return E_INVALIDARG;

    return ms_pfnD3D10DisassembleEffect(pEffect, EnableColorCode, 
        ppDisassembly);
}
//---------------------------------------------------------------------------
const D3D10_SHADER_MACRO* NiD3D10Renderer::GetD3D10MacroList(
    const char* pcFileType,
    const D3D10_SHADER_MACRO* pkUserMacros)
{
    if (!GetGlobalMacroCount() && !GetMacroCount(pcFileType))
    {
        // Exit if there are no global or file type-specific macros defined
        return pkUserMacros;
    }

    ShaderData kUserMacros;
    NiTFixedStringMap<NiFixedString> kAllMacros;

    // Fill kUserData array with user-supplied macro definitions
    // This will speed up searching for duplicate macro names,
    // because NiTFixedStringMap uses hash and only compares pointers.
    if (pkUserMacros)
    {
        while (pkUserMacros->Name && pkUserMacros->Definition)
        {
            kUserMacros.AddMacro(
                NiFixedString(pkUserMacros->Name),
                NiFixedString(pkUserMacros->Definition));
            pkUserMacros++;
        }
    }

    // Search for duplicate macro names and merge all macro containing structs
    BuildMacroList(pcFileType, &kUserMacros, kAllMacros);

    // If buffer is not created or its size is too small, [re]allocate it
    if (!m_pkD3D10MacroBuffer || m_uiD3D10MacroBufferLen <
        (kAllMacros.GetCount() + 1) * sizeof(D3D10_SHADER_MACRO))
    {
        if (m_pkD3D10MacroBuffer)
            NiFree(m_pkD3D10MacroBuffer);

        // Allocate buffer, that is 20% larger, than required to avoid
        // reallocations when few macros are added or changed
        m_uiD3D10MacroBufferLen =
            (NiUInt32)((kAllMacros.GetCount() + 1) * 1.2f);
        m_pkD3D10MacroBuffer = NiAlloc(D3D10_SHADER_MACRO,
            m_uiD3D10MacroBufferLen);
        m_uiD3D10MacroBufferLen *= sizeof(D3D10_SHADER_MACRO);
    }

    NiTMapIterator kIter = kAllMacros.GetFirstPos();
    NiUInt32 uiIdx = 0;
    while (kIter)
    {
        NiFixedString kName;
        NiFixedString kValue;
        kAllMacros.GetNext(kIter, kName, kValue);

        (m_pkD3D10MacroBuffer + uiIdx)->Name = kName;
        (m_pkD3D10MacroBuffer + uiIdx)->Definition = kValue;
        uiIdx++;
    }
    // NULL-terminate a list
    (m_pkD3D10MacroBuffer + uiIdx)->Name = NULL;
    (m_pkD3D10MacroBuffer + uiIdx)->Definition = NULL;

    return m_pkD3D10MacroBuffer;
}
//---------------------------------------------------------------------------
