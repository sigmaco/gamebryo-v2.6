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
#include "NiD3DRendererPCH.h"

#include "NiD3DDefaultShader.h"
#include "NiD3DGPUProgramCache.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DUtility.h"
#include "NiD3DUtils.h"
#include "NiDX92DBufferData.h"
#include "NiDX9DataStream.h"
#include "NiDX9DataStreamFactory.h"
#include "NiDX9Direct3DTexture.h"
#include "NiDX9Direct3DTextureData.h"
#include "NiDX9DynamicTextureData.h"
#include "NiDX9ErrorShader.h"
#include "NiDX9FragmentShader.h"
#include "NiDX9LightManager.h"
#include "NiDX9PixelFormat.h"
#include "NiDX9PersistentSrcTextureRendererData.h"
#include "NiDX9RenderedCubeMapData.h"
#include "NiDX9RenderedTextureData.h"
#include "NiDX9Renderer.h"
#include "NiDX9ShadowWriteShader.h"
#include "NiDX9SourceCubeMapData.h"
#include "NiDX9SourceTextureData.h"
#include "NiDX9SystemDesc.h"
#include "NiDX9TextureData.h"
#include "NiDX9TextureManager.h"
#include <NiAVObject.h>
#include <NiBound.h>
#include <NiCamera.h>
#include <NiDirectionalShadowWriteMaterial.h>
#include <NiDynamicTexture.h>
#include <NiFilename.h>
#include <NiLight.h>
#include <NiRenderObject.h>
#include <NiParticles.h>
#include <NiPlane.h>
#include <NiPoint2.h>
#include <NiPoint3.h>
#include <NiPointShadowWriteMaterial.h>
#include <NiRenderedCubeMap.h>
#include <NiRenderedTexture.h>
#include <NiShaderTimeController.h>
#include <NiShadowGenerator.h>
#include <NiShadowManager.h>
#include <NiSingleShaderMaterial.h>
#include <NiSkinningMeshModifier.h>
#include <NiSourceCubeMap.h>
#include <NiSpotShadowWriteMaterial.h>
#include <NiSystem.h>
#include <NiTexturingProperty.h>
#include <NiToolDataStream.h>
#include <NiTriShape.h>
#include <NiTriShapeData.h>
#include <NiTriStrips.h>
#include <NiTriStripsData.h>
#include <NiMesh.h>

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiDX9);
//---------------------------------------------------------------------------

NiImplementRTTI(NiDX9Renderer,NiRenderer);

HINSTANCE NiDX9Renderer::ms_hDXLib = NULL;
LPDIRECT3D9 NiDX9Renderer::ms_pkD3D9 = NULL;
NiCriticalSection NiDX9Renderer::ms_kD3DCriticalSection;

NILPD3DCREATE NiDX9Renderer::ms_pfnD3DCreate9 = NULL;
// Map NiTexturingProperty::ClampModes to D3D clamping
NiDX9Renderer::AddressSettings NiDX9Renderer::ms_akD3DAddressMapping[
    NiTexturingProperty::CLAMP_MAX_MODES];
// Map NiTexturingProperty::FilterModes to D3D filtering
NiDX9Renderer::FilterSettings NiDX9Renderer::ms_akD3DFilterMapping[
    NiTexturingProperty::FILTER_MAX_MODES];
unsigned char NiDX9Renderer::ms_aucD3DFormatSizes[
    NiDX9Renderer::D3DFORMATSIZECOUNT];

// Projected flags
unsigned int NiDX9Renderer::ms_uiProjectedTextureFlags = D3DTTFF_DISABLE;

NiTPointerList<unsigned int> NiDX9Renderer::ms_kDisplayFormats;

static NiDX9RendererSDM NiDX9RendererSDMObject;

const NiDX9SystemDesc* NiDX9Renderer::ms_pkSystemDesc = NULL;

NiCriticalSection NiDX9Renderer::ms_kDeferredReleaseLock;
NiTPointerList<IUnknown*> NiDX9Renderer::ms_akDeferredReleaseList[
    NiDX9Renderer::D3DRT_COUNT];

#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))
//  These are to do reference count checking during debug development
//  char buffer defined in NiDX9Renderer.cpp
char g_szIFDebugBuff[1024];
int g_iDeviceRefCount = 0;
LPDIRECT3DDEVICE9 g_pkD3DDevice = NULL;
#endif  //#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))

NiAllocatorDeclareStatics(
    NiDX9Renderer::PrePackMesh, 
    sizeof(NiDX9Renderer::PrePackMesh) * 16);

NiAllocatorDeclareStatics(
    NiDX9Renderer::PrePackTexture, 
    sizeof(NiDX9Renderer::PrePackTexture) * 16);

//---------------------------------------------------------------------------
void NiDX9Renderer::InitToDefault()
{
    m_pkD3DDevice9 = NULL;
    memset(&m_kD3DCaps9, 0, sizeof(m_kD3DCaps9));
    m_kWndDevice = 0;
    m_acDriverDesc[0] = '\0';
    m_uiAdapter = D3DADAPTER_DEFAULT;
    m_eDevType = D3DDEVTYPE_HAL;
    m_kWndFocus = 0;
    m_uiBehaviorFlags = 0;
    m_eAdapterFormat = D3DFMT_UNKNOWN;

    m_uiHWBones = 0;
    m_uiMaxStreams = 0;
    m_uiMaxPixelShaderVersion = 0;
    m_uiMaxVertexShaderVersion = 0;
    m_bMipmapCubeMaps = false;
    m_bDynamicTexturesCapable = false;

    m_uiMaxNumRenderTargets = 0;
    m_bIndependentBitDepths = false;
    m_bMRTPostPixelShaderBlending = false;

    m_uiResetCounter = 0;
    m_bDeviceLost = false;

    m_uiBackground = D3DCOLOR_XRGB(128, 128, 128);
    m_fZClear = 1.0;
    m_uiStencilClear = 0;
    m_uiRendFlags = 0;
    m_acBehavior[0] = '\0';
    m_usHwMaxAnisotropy = HW_MAX_ANISOTROPY;
    m_usMaxAnisotropy = HW_MAX_ANISOTROPY;

    m_kCamRight = NiPoint3::UNIT_X;
    m_kCamUp = NiPoint3::UNIT_Y;
    m_kModelCamRight = NiPoint3::UNIT_X;
    m_kModelCamUp = NiPoint3::UNIT_Y;

    m_fNearDepth = 0.1f;
    m_fDepthRange = 100.0f;

    memset(&m_kD3DIdentity, 0, sizeof(D3DMATRIX));
    m_kD3DIdentity._11 = m_kD3DIdentity._22 = 
        m_kD3DIdentity._33 = m_kD3DIdentity._44 = 1.0f;

    memset(&m_kD3DPort, 0, sizeof(D3DVIEWPORT9));
    m_kD3DPort.X = 0;
    m_kD3DPort.Y = 0;
    m_kD3DPort.Width = 1; // placeholder
    m_kD3DPort.Height = 1; // placeholder
    m_kD3DPort.MinZ = 0.0f;     
    m_kD3DPort.MaxZ = 1.0f;

    unsigned int i, j;
    for (j = 0; j < TEXUSE_NUM; j++)
    {
        for (i = 0; i < TEX_NUM; i++)
            m_aapkTextureFormats[j][i] = NULL;

        m_apkDefaultTextureFormat[j] = NULL;
        m_aspDefaultTextureData[j] = 0;
    }
    
    m_eReplacementDataFormat = TEX_DEFAULT;

    m_pkCurrRenderTargetGroup = NULL;
    m_pkCurrOnscreenRenderTargetGroup = NULL;

    m_pkTextureManager = NULL;
    m_pkRenderState = NULL;
    m_pkLightManager = NULL;

    m_kD3DMat._14 = 0.0f;
    m_kD3DMat._24 = 0.0f;
    m_kD3DMat._34 = 0.0f;
    m_kD3DMat._44 = 1.0f;

    m_ulDeviceThreadID = 0;

    memset(ms_aucD3DFormatSizes, 0, sizeof(ms_aucD3DFormatSizes));
    ms_aucD3DFormatSizes[D3DFMT_R8G8B8] = 24;
    ms_aucD3DFormatSizes[D3DFMT_A8R8G8B8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_X8R8G8B8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_R5G6B5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_X1R5G5B5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A1R5G5B5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A4R4G4B4] = 16;
    ms_aucD3DFormatSizes[D3DFMT_R3G3B2] = 8;
    ms_aucD3DFormatSizes[D3DFMT_A8] = 8;
    ms_aucD3DFormatSizes[D3DFMT_A8R3G3B2] = 16;
    ms_aucD3DFormatSizes[D3DFMT_X4R4G4B4] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A2B10G10R10] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A8B8G8R8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_X8B8G8R8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_G16R16] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A2R10G10B10] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A16B16G16R16] = 64;
    ms_aucD3DFormatSizes[D3DFMT_A8P8] = 16;
    ms_aucD3DFormatSizes[D3DFMT_P8] = 8;
    ms_aucD3DFormatSizes[D3DFMT_L8] = 8;
    ms_aucD3DFormatSizes[D3DFMT_A8L8] = 16;
    ms_aucD3DFormatSizes[D3DFMT_A4L4] = 8;
    ms_aucD3DFormatSizes[D3DFMT_V8U8] = 16;
    ms_aucD3DFormatSizes[D3DFMT_L6V5U5] = 16;
    ms_aucD3DFormatSizes[D3DFMT_X8L8V8U8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_Q8W8V8U8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_V16U16] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A2W10V10U10] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D16_LOCKABLE] = 16;
    ms_aucD3DFormatSizes[D3DFMT_D32] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D15S1] = 16;
    ms_aucD3DFormatSizes[D3DFMT_D24S8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D24X8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D24X4S4] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D16] = 16;
    ms_aucD3DFormatSizes[D3DFMT_D32F_LOCKABLE] = 32;
    ms_aucD3DFormatSizes[D3DFMT_D24FS8] = 32;
    ms_aucD3DFormatSizes[D3DFMT_L16] = 16;
    ms_aucD3DFormatSizes[D3DFMT_INDEX16] = 16;
    ms_aucD3DFormatSizes[D3DFMT_INDEX32] = 32;
    ms_aucD3DFormatSizes[D3DFMT_Q16W16V16U16] = 64;
    ms_aucD3DFormatSizes[D3DFMT_R16F] = 16;
    ms_aucD3DFormatSizes[D3DFMT_G16R16F] = 32;
    ms_aucD3DFormatSizes[D3DFMT_A16B16G16R16F] = 64;
    ms_aucD3DFormatSizes[D3DFMT_R32F] = 32;
    ms_aucD3DFormatSizes[D3DFMT_G32R32F] = 64;
    ms_aucD3DFormatSizes[D3DFMT_A32B32G32R32F] = 128;
    ms_aucD3DFormatSizes[D3DFMT_CxV8U8] = 16;

    // setup the safe zone to the inner 98% of the display
    m_kDisplaySafeZone.m_top =  0.01f;
    m_kDisplaySafeZone.m_left =  0.01f;
    m_kDisplaySafeZone.m_right =  0.99f;
    m_kDisplaySafeZone.m_bottom =  0.99f;

    // Initialize cached frustum.
    m_kCachedFrustum.m_fLeft = -0.5f;
    m_kCachedFrustum.m_fRight = 0.5f;
    m_kCachedFrustum.m_fTop = 0.5f;
    m_kCachedFrustum.m_fBottom = -0.5f;
    m_kCachedFrustum.m_fNear = 1.0f;
    m_kCachedFrustum.m_fFar = 2.0f;
    m_kCachedFrustum.m_bOrtho = false;

    // Initialize cached port.
    m_kCachedPort.m_left = 0.0f;
    m_kCachedPort.m_right = 1.0f;
    m_kCachedPort.m_top = 1.0f;
    m_kCachedPort.m_bottom = 0.0f;

    // Initialize shader macros buffers
    m_pkD3DXMacroBuffer = NULL;
    m_uiD3DXMacroBufferLen = 0;
    m_ppcCGMacroPtrBuffer = NULL;
    m_pcCGMacroBuffer = NULL;
    m_uiCGMacroPtrBufferLen = 0;
    m_uiCGMacroBufferLen = 0;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::IsD3D9Create()
{
    // This function assumes the critical section has already been locked.
    if (ms_pfnD3DCreate9)
        return true;

    ms_hDXLib = LoadLibrary("D3D9.DLL");

    if (ms_hDXLib)
    {
        // Find the D3D Create function, and call it
        // This function is statically linked into DX9 and above, but
        // not on NT4.0.  So, we need to query it
        ms_pfnD3DCreate9 = (NILPD3DCREATE)
            GetProcAddress(ms_hDXLib, "Direct3DCreate9");

        if (ms_pfnD3DCreate9)
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int NiDX9Renderer::CreateDirect3D9()
{
    int iRet = 0;
    ms_kD3DCriticalSection.Lock();

    if (ms_pkD3D9 == 0)
    {
        if (IsD3D9Create())
        {
            ms_pkD3D9 = (*ms_pfnD3DCreate9)(D3D_SDK_VERSION);
            if (ms_pkD3D9 == 0)
            {
                Message("Create D3D9 instance...FAILED\n");
                iRet = -1;
            }
            else
            {
                Message("Create D3D9 instance...SUCCESSFUL\n");
            }
        }
        else
        {
            Message("Create D3D9 instance...FAILED\n");
            iRet = -1;
        }
    }

    ms_kD3DCriticalSection.Unlock();
    return iRet;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::_SDMShutdown()
{
    ms_kD3DCriticalSection.Lock();

    if (ms_pkD3D9)
    {
        D3D_POINTER_RELEASE(ms_pkD3D9);
        ms_pkD3D9 = 0;
    }

    NiDelete ms_pkSystemDesc;
    ms_pkSystemDesc = NULL;

    if (ms_hDXLib)
        FreeLibrary(ms_hDXLib);
    ms_pfnD3DCreate9 = 0;
    ms_kDisplayFormats.RemoveAll();

    ms_kD3DCriticalSection.Unlock();

    NiAllocatorShutdown(NiDX9Renderer::PrePackMesh);
    NiAllocatorShutdown(NiDX9Renderer::PrePackTexture);
}
//---------------------------------------------------------------------------
NiDX9Renderer::NiDX9Renderer() :
    m_kDepthStencilFormats(7)
{
    InitToDefault();
}
//---------------------------------------------------------------------------
NiDX9Renderer::~NiDX9Renderer()
{
//CODEBLOCK(1) - DO NOT DELETE THIS LINE

    if (m_ulDeviceThreadID != NiGetCurrentThreadId())
    {
        Error("Releasing NiDX9Renderer from a different thread than the "
            "one used to create the renderer! "
            "This will cause failures to release D3D resouces and could lead "
            "to a crash.");
    }

    if (m_pkD3DDevice9)
    {
        // Set ALL stage texture to 0
        unsigned int ui;
        if (m_pkRenderState)
        {
            for (ui = 0; ui < NiD3DPass::ms_uiMaxTextureBlendStages; ui++)
                m_pkRenderState->SetTexture(ui,0);
        }

        // Set ALL streams to 0
        for (ui = 0; ui < m_uiMaxStreams; ui++)
            m_pkD3DDevice9->SetStreamSource(ui, 0, 0, 0);
        m_pkD3DDevice9->SetIndices(0);
    }

    for (unsigned int j = 0; j < TEXUSE_NUM; j++)
    {
        for (unsigned int i = 0; i < TEX_NUM; i++)
            NiDelete m_aapkTextureFormats[j][i];

        m_aspDefaultTextureData[j] = 0;
    }

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

    NiMaterial::UnloadShadersForAllMaterials();
    if (m_spLegacyDefaultShaderMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spLegacyDefaultShaderMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial, 
            m_spLegacyDefaultShaderMaterial)->SetCachedShader(NULL);
    }
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

    // Purge the D3DShaders
    PurgeAllD3DShaders();
    // Shut down the shader system
    NiD3DShaderInterface::ShutdownShaderSystem();

    NiDX9AdditionalDepthStencilBufferData::ShutdownAll();
    m_kRenderedTextures.RemoveAll();
    m_kRenderedCubeMaps.RemoveAll();
    m_kDynamicTextures.RemoveAll();
    
    NiDX9TextureData::ClearTextureData(this);

    // loop for each onscreen buffer
    NiTMapIterator kIter = m_kOnscreenRenderTargetGroups.GetFirstPos();
    while (kIter)
    {
        NiWindowRef kHWND;
        NiRenderTargetGroupPtr spRenderTargetGroup;

        m_kOnscreenRenderTargetGroups.GetNext(kIter, kHWND, 
            spRenderTargetGroup);
        if (spRenderTargetGroup)
        {
            for (unsigned int ui = 0; 
                ui < spRenderTargetGroup->GetBufferCount();
                ui++)
            {
                Ni2DBuffer* pkBuffer = spRenderTargetGroup->GetBuffer(ui);
                if (pkBuffer)
                {
                    pkBuffer->SetRendererData(NULL);
                }
            }

            Ni2DBuffer* pkBuffer = 
                spRenderTargetGroup->GetDepthStencilBuffer();
            if (pkBuffer)
            {
                pkBuffer->SetRendererData(NULL);
            }
        }
    }

    m_kOnscreenRenderTargetGroups.RemoveAll();

    // Clear out the m_kDepthStencilFormats map
    kIter = m_kDepthStencilFormats.GetFirstPos();
    while (kIter)
    {
        D3DFORMAT eFormat;
        NiPixelFormat* pkPixelFormat = NULL;
        m_kDepthStencilFormats.GetNext(kIter, eFormat, pkPixelFormat);
        NiDelete pkPixelFormat;
    }

    NiDelete m_pkRenderState;
    m_pkRenderState = NULL;

    NiDelete m_pkTextureManager;
    m_pkTextureManager = NULL;

    NiDelete m_pkLightManager;
    m_pkLightManager = NULL;

    m_spErrorShader = 0;

    ReleaseDeferredResources();

    if (m_pkD3DDevice9)
    {
        ReleaseDevice(m_pkD3DDevice9);
        m_pkD3DDevice9 = 0;
    }

    RemoveAllResetNotificationFuncs();
    RemoveAllLostDeviceNotificationFuncs();

    // Configure the streaming system with platform specific loaders

    NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
    NiPersistentSrcTextureRendererData::ResetStreamingFunctions();

    NiDelete NiDataStream::GetFactory();
    NiDataStream::SetFactory(NULL);

    NiFree(m_pkD3DXMacroBuffer);
    NiFree(m_ppcCGMacroPtrBuffer);
    NiFree(m_pcCGMacroBuffer);
}
//---------------------------------------------------------------------------
NiDX9Renderer* NiDX9Renderer::Create(
    unsigned int uiWidth, 
    unsigned int uiHeight,
    unsigned int uiUseFlags, 
    NiWindowRef kWndDevice, 
    NiWindowRef kWndFocus, 
    unsigned int uiAdapter, 
    DeviceDesc eDesc, 
    FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, 
    PresentationInterval ePresentationInterval, 
    SwapEffect eSwapEffect, 
    unsigned int uiFBMode, 
    unsigned int uiBackBufferCount, 
    unsigned int uiRefreshRate)
{
    // Create the Direct3D9 interface
    if (CreateDirect3D9() == -1)
    {
        SetLastErrorString("Creation failed: Could not initialize DirectX9");
        Error("NiDX9Renderer::Create> Invalid device ID specified "
            "- FAILING\n");
        return NULL;
    }

//CODEBLOCK(2) - DO NOT DELETE THIS LINE

    // Create an 'empty' instance of the renderer
    NiDX9Renderer* pkR = NiNew NiDX9Renderer;
    NIASSERT(pkR);

    ms_kD3DCriticalSection.Lock();

    bool bSuccess = pkR->Initialize(uiWidth, uiHeight, uiUseFlags, kWndDevice, 
        kWndFocus, uiAdapter, eDesc, eFBFormat, eDSFormat, 
        ePresentationInterval, eSwapEffect, uiFBMode, uiBackBufferCount, 
        uiRefreshRate);

    ms_kD3DCriticalSection.Unlock();

    if (bSuccess == false)
    {
        NiDelete pkR;
        return NULL;
    }

    // Configure the streaming system with platform specific loaders

    // Registration of the DX9-specific
    // NiDX9PersistentSrcTextureRendererData::LoadBinary() that overrides
    // NiPersistentSrcTextureRendererData::LoadBinary();
    NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
    NiStream::RegisterLoader("NiPersistentSrcTextureRendererData", 
        NiDX9PersistentSrcTextureRendererData::CreateObject);

    return pkR;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Initialize(
    unsigned int uiWidth, 
    unsigned int uiHeight, 
    unsigned int uiUseFlags, 
    NiWindowRef kWndDevice, 
    NiWindowRef kWndFocus, 
    unsigned int uiAdapter, 
    DeviceDesc eDesc, 
    FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, 
    PresentationInterval ePresentationInterval, 
    SwapEffect eSwapEffect, 
    unsigned int uiFBMode, 
    unsigned int uiBackBufferCount, 
    unsigned int uiRefreshRate)
{
    m_uiCreationWidth = uiWidth;
    m_uiCreationHeight = uiHeight;
    m_uiCreationUseFlags = uiUseFlags;
    m_kCreationWndDevice = kWndDevice;
    m_kCreationWndFocus = kWndFocus;
    m_uiCreationAdapter = uiAdapter;
    m_eCreationDesc = eDesc;
    m_eCreationFBFormat = eFBFormat;
    m_eCreationDSFormat = eDSFormat;
    m_eCreationPresentationInterval = ePresentationInterval;
    m_eCreationSwapEffect = eSwapEffect;
    m_uiCreationFBMode = uiFBMode;
    m_uiCreationBackBufferCount = uiBackBufferCount;
    m_uiCreationRefreshRate = uiRefreshRate;

    // Unlock so GetSystemDesc can pick up the lock - shouldn't make a
    // difference if the process gets interrupted here.
    ms_kD3DCriticalSection.Unlock();
    const NiDX9SystemDesc* pkSystemDesc = 
        GetSystemDesc();
    NI_UNUSED_ARG(pkSystemDesc);
    ms_kD3DCriticalSection.Lock();
    NIASSERT(pkSystemDesc);

    //  Adapter
    m_uiAdapter = uiAdapter;

    m_kWndDevice = kWndDevice;
    m_kWndFocus = kWndFocus;

    if (!ExpandDeviceType(eDesc, m_eDevType, m_uiBehaviorFlags))
    {
        SetLastErrorString("Creation failed: Invalid 3D device type");
        Error("NiDX9Renderer::Initialize> Invalid device ID specified - "
            "FAILING\n");

        return false;
    }

    if (uiUseFlags & USE_FPU_PRESERVE)
        m_uiBehaviorFlags |= D3DCREATE_FPU_PRESERVE;

    if (uiUseFlags & USE_MULTITHREADED)
        m_uiBehaviorFlags |= D3DCREATE_MULTITHREADED;

    m_pkAdapterDesc = ms_pkSystemDesc->GetAdapter(m_uiAdapter);

    if (!m_pkAdapterDesc)
    {
        SetLastErrorString("Creation failed: Invalid Adapter");
        Error("NiDX9Renderer::Initialize> Invalid Adapter specified - "
            "FAILING\n");

        return false;
    }

    m_pkDeviceDesc = m_pkAdapterDesc->GetDevice(m_eDevType);

    if (!m_pkDeviceDesc)
    {
        SetLastErrorString("Creation failed: Invalid 3D device type");
        Error("NiDX9Renderer::Initialize> Invalid device ID specified - "
            "FAILING\n");

        return false;
    }

    HRESULT eD3dRet;

    D3DPRESENT_PARAMETERS kD3DPresentParams;
    memset(&kD3DPresentParams, 0, sizeof(kD3DPresentParams));

    for ( ; ; )
    {
        Message("NiDX9Renderer::Initialize> Attempting to create "
            "rendering device\n");

        if (!ms_pkSystemDesc->GetAdapter(m_uiAdapter)->GetDevice(m_eDevType))
        {
            SetLastErrorString("Creation failed: Requested device not valid");
            Warning("NiDX9Renderer::Initialize> No device of specified "
                "type \n");
        }
        else
        {
            if (InitializePresentParams(m_kWndDevice, uiWidth, uiHeight, 
                uiUseFlags, uiFBMode, eFBFormat, eDSFormat, uiBackBufferCount, 
                eSwapEffect, uiRefreshRate, ePresentationInterval, 
                kD3DPresentParams))
            {
                // Cache the backbuffer count (Validate may change it)
                uiBackBufferCount = kD3DPresentParams.BackBufferCount;

                //  Create the device
                eD3dRet = ms_pkD3D9->CreateDevice(m_uiAdapter, m_eDevType, 
                    m_kWndFocus, m_uiBehaviorFlags, &kD3DPresentParams, 
                    &m_pkD3DDevice9);
                
                m_ulDeviceThreadID = NiGetCurrentThreadId();

                if (SUCCEEDED(eD3dRet))
                    break;

                m_bDeviceLost = false;

                // If the call failed and D3D changed the backbuffer count 
                // for us in the creation call, then we should try again with
                // the new (valid) backbuffer count
                if (uiBackBufferCount != kD3DPresentParams.BackBufferCount)
                {
                    Warning("NiDX9Renderer::Initialize> Specified number of "
                        "backbuffers was invalid - trying fewer "
                        "backbuffers\n");

                    eD3dRet = ms_pkD3D9->CreateDevice(m_uiAdapter, m_eDevType,
                        m_kWndFocus, m_uiBehaviorFlags, &kD3DPresentParams, 
                        &m_pkD3DDevice9);

                    if (SUCCEEDED(eD3dRet))
                        break;
                }
            }
        }

        if (eDesc == DEVDESC_REF)
        {
            SetLastErrorString("Creation failed: Could not create reference "
                "device");
            Error("NiDX9Renderer::Initialize> Could not create requested "
                "reference device - FAILING\n");

            return false;
        }

        eDesc = (DeviceDesc)(((unsigned int)eDesc) + 1);
        
        // We do _not_ fall back to the reference device automatically -
        // an app must request it specifically
        if (eDesc == DEVDESC_REF)
        {
            // do not change error string - earlier functions set better
            // error messages
            Error("NiDX9Renderer::Initialize> Could not create hardware "
                "device of any type - FAILING\n");

            return false;
        }

        Warning("NiDX9Renderer::Initialize> Could not create device of "
            "desired type - trying next-lower device type\n");

        if (!ExpandDeviceType(eDesc, m_eDevType, m_uiBehaviorFlags))
        {
            SetLastErrorString("Creation failed: Invalid 3D device type");
            Error("NiDX9Renderer::Initialize> Invalid device ID specified - "
                "FAILING\n");

            return false;
        }
    }

    if (!m_pkD3DDevice9)
    {
        SetLastErrorString("Creation failed: Could not create hardware "
            "device");
        Error("NiDX9Renderer::Initialize> Could not create device of any "
            "type - FAILING\n");

        return false;
    }

#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))
    g_pkD3DDevice = m_pkD3DDevice9;
#endif  //#if (defined(NIDEBUG) && defined(DEBUG_INTERFACE_POINTERS))

    //  Create the texture manager
    m_pkTextureManager = NiNew NiDX9TextureManager(this);
    NIASSERT(m_pkTextureManager);

    if (InitializeRendererFromDevice(kD3DPresentParams) == false)
        return false;

    m_kOnscreenRenderTargetGroups.SetAt(kWndDevice,
        m_spDefaultRenderTargetGroup);

    // Lights initialization
    m_pkLightManager = 
        NiNew NiDX9LightManager(m_pkRenderState, m_pkD3DDevice9);

    // Camera initialization
    m_pkCurrRenderTargetGroup = m_spDefaultRenderTargetGroup;
    NiFrustum kFrust;
    kFrust.m_fLeft = kFrust.m_fBottom = -1.0f;
    kFrust.m_fRight = kFrust.m_fTop = 1.0f;
    kFrust.m_fNear = 0.1f;
    kFrust.m_fFar = 1.0f;

    Do_SetCameraData(NiPoint3::ZERO, NiPoint3::UNIT_X, NiPoint3::UNIT_Y, 
        NiPoint3::UNIT_Z, kFrust, NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f));
    m_pkCurrRenderTargetGroup = NULL;
    m_pkCurrOnscreenRenderTargetGroup = NULL;

    NIASSERT(NiD3DShaderFactory::GetD3DShaderFactory());
    NiD3DShaderFactory::GetD3DShaderFactory()->SetAsActiveFactory();

    m_spLegacyDefaultShader = NiNew NiD3DDefaultShader();
    NIASSERT(m_spLegacyDefaultShader != NULL);
    m_spLegacyDefaultShader->Initialize();
    m_spLegacyDefaultShaderMaterial = NiSingleShaderMaterial::Create(
        m_spLegacyDefaultShader, true);
    if (m_spLegacyDefaultShaderMaterial && NiIsKindOf(NiSingleShaderMaterial,
        m_spLegacyDefaultShaderMaterial))
    {
        NiSmartPointerCast(NiSingleShaderMaterial,
            m_spLegacyDefaultShaderMaterial)->
            SetCachedShader(m_spLegacyDefaultShader);
    }

    NiShader* pkErrorShader = NiNew NiDX9ErrorShader;
    pkErrorShader->Initialize();

    SetErrorShader(pkErrorShader);

    // Set this class as the NiDataStream Factory
    NiDX9DataStreamFactory* pkDataStreamFactory = 
        NiNew NiDX9DataStreamFactory();
    NiDataStream::SetFactory(pkDataStreamFactory);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializeRendererFromDevice(
    D3DPRESENT_PARAMETERS kD3DPresentParams)
{
    if (!InitializeDeviceCaps(kD3DPresentParams))
    {
        // do not set error string - InitializeDeviceCaps will set it
        return false;
    }

    if (m_spDefaultRenderTargetGroup == NULL)
    {
        // Add info to the current onscreen render target
        Ni2DBuffer* pkBackBuffer = NULL;
        NiDepthStencilBuffer* pkDepthBuffer = NULL;
        NiDX9ImplicitBufferData::Create(
            m_pkD3DDevice9, kD3DPresentParams, pkBackBuffer);
        NiDX9ImplicitDepthStencilBufferData::Create(
            m_pkD3DDevice9, pkDepthBuffer);

        m_spDefaultRenderTargetGroup = NiRenderTargetGroup::Create(1, this);
        m_spDefaultRenderTargetGroup->AttachBuffer(pkBackBuffer, 0);
        m_spDefaultRenderTargetGroup->AttachDepthStencilBuffer(pkDepthBuffer);
    }

    m_eAdapterFormat = GetCompatibleAdapterFormat(GetNiFBFormat((D3DFORMAT)
        m_spDefaultRenderTargetGroup->GetPixelFormat(0)->GetRendererHint()));

    if (!InitializeTextureDefaults())
    {
        // do not set error string - InitializeTextureDefaults will set it
        Error("NiDX9Renderer::Initialize> Could not initialize texture "
            "defaults - FAILING\n");

        return false;
    }

    // Create render state manager
    if (m_pkRenderState)
    {
        m_pkRenderState->Reset(m_kD3DCaps9, true);
    }
    else
    {
        m_pkRenderState = NiDX9RenderState::Create(this, m_kD3DCaps9, true);

        // Initialize the shader system
        NiD3DShaderInterface::InitializeShaderSystem(this);
    }

    // Inform the shadowing system of the shader model versions used by the 
    // current hardware.
    if (NiShadowManager::GetShadowManager())
    {
        NiShadowManager::ValidateShaderVersions(
            (unsigned short)D3DSHADER_VERSION_MAJOR(
                m_uiMaxVertexShaderVersion),
            (unsigned short)D3DSHADER_VERSION_MINOR(
                m_uiMaxVertexShaderVersion),
            0, 0,
            (unsigned short)D3DSHADER_VERSION_MAJOR(
                m_uiMaxPixelShaderVersion),
            (unsigned short)D3DSHADER_VERSION_MINOR(
                m_uiMaxPixelShaderVersion));
    }

    if (kD3DPresentParams.MultiSampleType != D3DMULTISAMPLE_NONE)
    {
        m_pkRenderState->SetAntiAlias(true);
    }

    NiD3DShaderFactory* pkFactory = 
        (NiD3DShaderFactory*)NiShaderFactory::GetInstance();

    if (m_uiMaxPixelShaderVersion >=
        pkFactory->CreatePixelShaderVersion(2, 0) &&
        m_uiMaxVertexShaderVersion >=
        pkFactory->CreateVertexShaderVersion(2, 0))
    {
        NiStandardMaterial* pkStandardMaterial = NiDynamicCast(
            NiStandardMaterial, m_spInitialDefaultMaterial);
        if (pkStandardMaterial)
        {
            ((NiRenderer*)this)->SetDefaultProgramCache(pkStandardMaterial);
        }

        if ( NiShadowManager::GetShadowManager())
        {
            NiDirectionalShadowWriteMaterial* pkDirShadowWriteMaterial = 
                (NiDirectionalShadowWriteMaterial*)NiShadowManager::
                GetShadowWriteMaterial(NiStandardMaterial::LIGHT_DIR);

            if (pkDirShadowWriteMaterial)
            {
                ((NiRenderer*)this)->SetDefaultProgramCache(
                    pkDirShadowWriteMaterial);
            }

            NiPointShadowWriteMaterial* pkPointShadowWriteMaterial = 
                (NiPointShadowWriteMaterial*)NiShadowManager::
                GetShadowWriteMaterial(NiStandardMaterial::LIGHT_POINT);

            if (pkPointShadowWriteMaterial)
            {
                ((NiRenderer*)this)->SetDefaultProgramCache(
                    pkPointShadowWriteMaterial);
            }

            NiSpotShadowWriteMaterial* pkSpotShadowWriteMaterial = 
                (NiSpotShadowWriteMaterial*)NiShadowManager::
                GetShadowWriteMaterial(NiStandardMaterial::LIGHT_SPOT);

            if (pkSpotShadowWriteMaterial)
            {
                ((NiRenderer*)this)->SetDefaultProgramCache(
                    pkSpotShadowWriteMaterial);
            }
        }
    }
    else
    {
        m_spInitialDefaultMaterial = m_spLegacyDefaultShaderMaterial;
    }

    m_spCurrentDefaultMaterial = m_spInitialDefaultMaterial;

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetDefaultProgramCache(
    NiFragmentMaterial* pkMaterial, 
    bool bAutoWriteToDisk, 
    bool bWriteDebugFile, 
    bool bLoad,
    bool bNoNewProgramCreation, 
    const char* pcWorkingDir)
{
    const char* pcVSProfile = 
        D3DXGetVertexShaderProfile(m_pkD3DDevice9);
    const char* pcPSProfile = 
        D3DXGetPixelShaderProfile(m_pkD3DDevice9);

    if (pcVSProfile == NULL || pcPSProfile == NULL)
        return;

    // Check for ps_3_0/vs_3_0 mismatch - technically it's allowed in some
    // circumstances with specific restrictions, but we don't follow those
    // restrictions in NiStandardMaterial, so instead we just fall back
    if (strcmp(pcVSProfile, "vs_3_0") == 0 && 
        strcmp(pcPSProfile, "ps_3_0") != 0)
    {
        pcVSProfile = "vs_2_a";
    }

    NIASSERT(
        (strcmp(pcVSProfile, "vs_3_0") == 0 && 
        strcmp(pcPSProfile, "ps_3_0") == 0) || 
        (strcmp(pcVSProfile, "vs_3_0") != 0 && 
        strcmp(pcPSProfile, "ps_3_0") != 0));

    if (pcWorkingDir == NULL)
        pcWorkingDir = NiMaterial::GetDefaultWorkingDirectory();

    NiGPUProgram::ProgramType eType = NiGPUProgram::PROGRAM_VERTEX;
    NiD3DGPUProgramCache* pkCache = 
        NiNew NiD3DGPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcVSProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);

    eType = NiGPUProgram::PROGRAM_PIXEL;
    pkCache = 
        NiNew NiD3DGPUProgramCache(
        pkMaterial->GetProgramVersion(eType),
        pcWorkingDir, eType, pcPSProfile, 
        pkMaterial->GetName(), bAutoWriteToDisk,
        bWriteDebugFile, bNoNewProgramCreation, bLoad);
    pkMaterial->SetProgramCache(pkCache, eType);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Recreate()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Wait for all floodgate tasks to complete. This is needed since there may
    // be active floodgate task that need to read/write to data streams. In 
    // order to avoid serious problems with attempting to read/write to a data
    // stream as its being recreated we wait for floodgate to complete all its 
    // active tasks before recreating the data streams.
    NiStreamProcessor* pkStreamProcessor = NiStreamProcessor::Get();
    if (pkStreamProcessor)
    {
        float fStartTime = NiGetCurrentTimeInSec();
        while(pkStreamProcessor->GetActiveWorkflowCount() != 0)
        {
            // Wait no longer than two seconds
            if (NiGetCurrentTimeInSec() - fStartTime > 2.0f)
            {
                NILOG("Error: Floodgate taking longer than two second to "
                    "complete its set of currently active task. Now attempting"
                    " to restore the D3D device while Floodgate is active\n");
                break;
            }
        }
    }

    // Acquire locks for resources being recreated
    m_kD3DDefaultBuffersLock.Lock();
    LockSourceDataCriticalSection();
    LockRenderer();

    // Release non-managed resources
    m_pkLightManager->ReleaseLights();

    NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 1);
    NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 2);
    NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 3);

    NiTListIterator pkListIter = m_kD3DDefaultBuffers.GetHeadPos();
    while (pkListIter)
    {
        NiDX9DataStream* pkDataStream = 
            m_kD3DDefaultBuffers.GetNext(pkListIter);
        pkDataStream->LostDevice();

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkDataStream->IncRefCount();
    }

    NiTMapIterator pkIter = m_kOnscreenRenderTargetGroups.GetFirstPos();
    while (pkIter)
    {
        NiWindowRef kHWND;
        NiRenderTargetGroupPtr spRenderTargetGroup;
        Ni2DBuffer* pkBuffer;
        NiDX9OnscreenBufferData* pkTarget;
        NiDX9DepthStencilBufferData* pkDSTarget;

        m_kOnscreenRenderTargetGroups.GetNext(pkIter, kHWND, 
            spRenderTargetGroup);
        NIASSERT(spRenderTargetGroup);

        pkBuffer = spRenderTargetGroup->GetBuffer(0);
        NIASSERT(pkBuffer);

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkBuffer->IncRefCount();

        pkTarget = NiDynamicCast(NiDX9OnscreenBufferData, 
            (NiDX92DBufferData*)pkBuffer->GetRendererData());

        NIASSERT(pkTarget);
        pkTarget->Shutdown();

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkTarget->IncRefCount();

        pkBuffer = spRenderTargetGroup->GetDepthStencilBuffer();
        if (pkBuffer)
        {
            pkDSTarget = NiDynamicCast(NiDX9DepthStencilBufferData, 
                (NiDX92DBufferData*)pkBuffer->GetRendererData());

            if (pkDSTarget)
            {
                pkDSTarget->Shutdown();

                // Increment the ref count to prevent the object from being
                // deleted in the callbacks
                pkDSTarget->IncRefCount();
            }
        }
    }

    NiTMapIterator kPos = m_kRenderedTextures.GetFirstPos();
    while (kPos)
    {
        NiRenderedTexture* pkRenderedTex;
        NiDX9RenderedTextureData* pkTexData;
        m_kRenderedTextures.GetNext(kPos, pkRenderedTex, pkTexData);

        PurgeTexture(pkRenderedTex);

        m_kRenderedTextures.SetAt(pkRenderedTex, 0);

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkRenderedTex->IncRefCount();
    }

    kPos = m_kRenderedCubeMaps.GetFirstPos();
    while (kPos)
    {
        NiRenderedCubeMap* pkRenderedCubeMap;
        NiDX9RenderedCubeMapData* pkCubeMapData;
        m_kRenderedCubeMaps.GetNext(kPos, pkRenderedCubeMap, pkCubeMapData);
        
        PurgeTexture(pkRenderedCubeMap);

        m_kRenderedCubeMaps.SetAt(pkRenderedCubeMap, 0);

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkRenderedCubeMap->IncRefCount();
    }

    kPos = m_kDynamicTextures.GetFirstPos();
    while (kPos)
    {
        NiDynamicTexture* pkDynTxtr;
        NiDX9DynamicTextureData* pkDynTxtrData;
        m_kDynamicTextures.GetNext(kPos, pkDynTxtr, pkDynTxtrData);

        pkDynTxtr->SetRendererData(NULL);
        NiDelete pkDynTxtrData;

        m_kDynamicTextures.SetAt(pkDynTxtr, 0);

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkDynTxtr->IncRefCount();
    }

    NiDX9AdditionalDepthStencilBufferData::ShutdownAll();

    NiTListIterator kShaderPos = m_kD3DShaders.GetHeadPos();
    while (kShaderPos)
    {
        NiD3DShaderInterface* pkD3DShader = 
            m_kD3DShaders.GetNext(kShaderPos);
        pkD3DShader->HandleLostDevice();

        // Increment the ref count to prevent the object from being
        // deleted in the callbacks
        pkD3DShader->IncRefCount();
    }

    // Notify any application-set functions about device reset
    unsigned int uiFuncCount = m_kResetNotifyFuncs.GetSize();
    unsigned int i = 0;
    for (; i < uiFuncCount; i++)
    {
        RESETNOTIFYFUNC pfnFunc = m_kResetNotifyFuncs.GetAt(i);
        void* pvData = m_kResetNotifyFuncData.GetAt(i);
        if (pfnFunc)
        {
            bool bResult = (*pfnFunc)(true, pvData);

            if (bResult == false)
            {
                Error("NiDX9Renderer::Recreate> Reset notification function "
                    "failed before device reset - FAILING\n");
                return false;
            }
        }
    }

    const Ni2DBuffer* pkBuffer = m_spDefaultRenderTargetGroup->GetBuffer(0);
    NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        pkBuffer->GetRendererData());
    NIASSERT(pkOnscreenBuffData);

    HRESULT eD3dRet = m_pkD3DDevice9->Reset((D3DPRESENT_PARAMETERS*)
        &pkOnscreenBuffData->GetPresentParams());
    
    if (FAILED(eD3dRet))
        return false;

    m_uiResetCounter++;
    m_bDeviceLost = false;

    // Re-create NiDX9Renderer settings

    // Recreate all of the onscreen buffers - be careful when dealing with
    // the current onscreen buffer, as it will already have been created
    // again when the device
    pkIter = m_kOnscreenRenderTargetGroups.GetFirstPos();
    while (pkIter)
    {
        NiWindowRef kHWND;
        NiRenderTargetGroupPtr spRenderTargetGroup;
        Ni2DBuffer* pkBuffer;
        NiDX9OnscreenBufferData* pkTarget;
        NiDX9DepthStencilBufferData* pkDSTarget;

        m_kOnscreenRenderTargetGroups.GetNext(pkIter, kHWND, 
            spRenderTargetGroup);
        NIASSERT(spRenderTargetGroup);

        pkBuffer = spRenderTargetGroup->GetBuffer(0);
        NIASSERT(pkBuffer);

        pkTarget = NiDynamicCast(NiDX9OnscreenBufferData,
            (NiDX92DBufferData*)pkBuffer->GetRendererData());

        NIASSERT(pkTarget);

        // This call may fail if the window is iconified, etc
        pkTarget->Recreate(m_pkD3DDevice9);

        pkDSTarget = NiDynamicCast(NiDX9DepthStencilBufferData,
            (NiDX92DBufferData*) 
            spRenderTargetGroup->GetDepthStencilBufferRendererData());
        if (pkDSTarget)
        {
            pkDSTarget->Recreate(m_pkD3DDevice9);

            // Decrement the ref count to that was previously incremented
            pkDSTarget->DecRefCount();
        }

        // Decrement the ref count to that was previously incremented
        pkBuffer->DecRefCount();

        // Decrement the ref count to that was previously incremented
        pkTarget->DecRefCount();
    }

    // Reset all settings
    InitializeRendererFromDevice(pkOnscreenBuffData->GetPresentParams());
    m_pkLightManager->InitRenderModes();

    kPos = m_kRenderedTextures.GetFirstPos();
    while (kPos)
    {
        NiRenderedTexture* pkRenderedTex;
        NiDX9RenderedTextureData* pkTexData;
        m_kRenderedTextures.GetNext(kPos, pkRenderedTex, pkTexData);

        CreateRenderedTextureRendererData(pkRenderedTex);

        // Decrement the ref count to that was previously incremented
        pkRenderedTex->DecRefCount();
    }

    kPos = m_kRenderedCubeMaps.GetFirstPos();
    while (kPos)
    {
        NiRenderedCubeMap* pkRenderedCubeMap;
        NiDX9RenderedCubeMapData* pkCubeMapData;
        m_kRenderedCubeMaps.GetNext(kPos, pkRenderedCubeMap, pkCubeMapData);

        CreateRenderedCubeMapRendererData(pkRenderedCubeMap);

        // Decrement the ref count to that was previously incremented
        pkRenderedCubeMap->DecRefCount();
    }

    kPos = m_kDynamicTextures.GetFirstPos();
    while (kPos)
    {
        NiDynamicTexture* pkDynTxtr;
        NiDX9DynamicTextureData* pkDynTxtrData;
        m_kDynamicTextures.GetNext(kPos, pkDynTxtr, pkDynTxtrData);

        CreateDynamicTextureRendererData(pkDynTxtr);

        // Decrement the ref count to that was previously incremented
        pkDynTxtr->DecRefCount();
    }

    NiDX9AdditionalDepthStencilBufferData::RecreateAll(m_pkD3DDevice9);

    pkListIter = m_kD3DDefaultBuffers.GetHeadPos();
    while (pkListIter)
    {
        NiDX9DataStream* pkDataStream = 
            m_kD3DDefaultBuffers.GetNext(pkListIter);
        pkDataStream->Recreate();

        // Decrement the ref count to that was previously incremented
        pkDataStream->DecRefCount();
    }

    kShaderPos = m_kD3DShaders.GetHeadPos();
    while (kShaderPos)
    {
        NiD3DShaderInterface* pkD3DShader = 
            m_kD3DShaders.GetNext(kShaderPos);
        pkD3DShader->HandleResetDevice();

        // Decrement the ref count to that was previously incremented
        pkD3DShader->DecRefCount();
    }

    // Notify any application-set functions about device reset
    for (i = 0; i < uiFuncCount; i++)
    {
        RESETNOTIFYFUNC pfnFunc = m_kResetNotifyFuncs.GetAt(i);
        void* pvData = m_kResetNotifyFuncData.GetAt(i);
        if (pfnFunc)
        {
            bool bResult = (*pfnFunc)(false, pvData);

            if (bResult == false)
            {
                Error("NiDX9Renderer::Recreate> Reset notification function "
                    "failed after device reset - FAILING\n");
                return false;
            }
        }
    }

    // Notify the shadow manager about the device reset
    if (NiShadowManager::GetShadowManager())
    {
        NiShadowManager::RendererRecreated();
    }

    // Release locks
    UnlockRenderer();
    UnlockSourceDataCriticalSection();
    m_kD3DDefaultBuffersLock.Unlock();
    
//CODEBLOCK(3) - DO NOT DELETE THIS LINE

    return true;
}
//---------------------------------------------------------------------------
NiDX9Renderer::RecreateStatus NiDX9Renderer::Recreate(
    unsigned int uiWidth, 
    unsigned int uiHeight,
    unsigned int uiUseFlags, 
    NiWindowRef kWndDevice, 
    FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, 
    PresentationInterval ePresentationInterval, 
    SwapEffect eSwapEffect, 
    unsigned int uiFBMode, 
    unsigned int uiBackBufferCount, 
    unsigned int uiRefreshRate)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    const Ni2DBuffer* pkBuffer = m_spDefaultRenderTargetGroup->GetBuffer(0);
    NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        pkBuffer->GetRendererData());
    NIASSERT(pkOnscreenBuffData);

    D3DPRESENT_PARAMETERS& kPrimaryPresent = 
        pkOnscreenBuffData->GetPresentParams();
    D3DPRESENT_PARAMETERS kTempBackup;
    NiMemcpy(&kTempBackup, &kPrimaryPresent, sizeof(kTempBackup));

    // Get present parameters
    if (!InitializePresentParams(kWndDevice, uiWidth, uiHeight, uiUseFlags, 
        uiFBMode, eFBFormat, eDSFormat, uiBackBufferCount, eSwapEffect, 
        uiRefreshRate, ePresentationInterval, kPrimaryPresent))
    {
        NiMemcpy(&kPrimaryPresent, sizeof(kPrimaryPresent), &kTempBackup,
            sizeof(kTempBackup));
        return RECREATESTATUS_FAILED;
    }

    if (kWndDevice)
    {
        kPrimaryPresent.hDeviceWindow = kWndDevice;
    }

    if (!Recreate())
    {
        Warning("NiDX9Renderer::Recreate> Could not reset device to "
            "desired parameters - trying to recover original values\n");
        NiMemcpy(&kPrimaryPresent, sizeof(kPrimaryPresent), &kTempBackup,
            sizeof(kTempBackup));
        if (!Recreate())
        {
            Error("NiDX9Renderer::Recreate> Could not recover original "
                "presentation parameters\n");

            return RECREATESTATUS_FAILED;
        }
        else
        {
            return RECREATESTATUS_RESTORED;
        }
    }

    // Update creation parameters
    m_uiCreationWidth = uiWidth;
    m_uiCreationHeight = uiHeight;
    m_uiCreationUseFlags = uiUseFlags;
    m_kCreationWndDevice = kWndDevice;
    m_eCreationFBFormat = eFBFormat;
    m_eCreationDSFormat = eDSFormat;
    m_eCreationPresentationInterval = ePresentationInterval;
    m_eCreationSwapEffect = eSwapEffect;
    m_uiCreationFBMode = uiFBMode;
    m_uiCreationBackBufferCount = uiBackBufferCount;
    m_uiCreationRefreshRate = uiRefreshRate;

    return RECREATESTATUS_OK;
}
//---------------------------------------------------------------------------
NiDX9Renderer::RecreateStatus NiDX9Renderer::Recreate(unsigned int uiWidth, 
    unsigned int uiHeight)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    const Ni2DBuffer* pkBuffer = m_spDefaultRenderTargetGroup->GetBuffer(0);
    NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        pkBuffer->GetRendererData());
    NIASSERT(pkOnscreenBuffData);

    D3DPRESENT_PARAMETERS& kPrimaryPresent = 
        pkOnscreenBuffData->GetPresentParams();
    D3DPRESENT_PARAMETERS kTempBackup;
    NiMemcpy(&kTempBackup, sizeof(kTempBackup), &kPrimaryPresent, 
        sizeof(kPrimaryPresent));

    // Change just width and height
    kPrimaryPresent.BackBufferWidth = uiWidth;
    kPrimaryPresent.BackBufferHeight = uiHeight;

    if (!Recreate())
    {
        Warning("NiDX9Renderer::Recreate> Could not reset device to "
            "desired parameters - trying to recover original values\n");
        NiMemcpy(&kPrimaryPresent, sizeof(kPrimaryPresent), &kTempBackup, 
            sizeof(kTempBackup));
        if (!Recreate())
        {
            Error("NiDX9Renderer::Recreate> Could not recover original "
                "presentation parameters\n");

            return RECREATESTATUS_FAILED;
        }
        else
        {
            return RECREATESTATUS_RESTORED;
        }
    }

    // Update creation width/height
    m_uiCreationWidth = uiWidth;
    m_uiCreationHeight = uiHeight;

    return RECREATESTATUS_OK;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::GetCreationParameters(
    unsigned int& uiWidth, 
    unsigned int& uiHeight, 
    unsigned int& uiUseFlags, NiWindowRef& kWndDevice, 
    NiWindowRef& kWndFocus, 
    unsigned int& uiAdapter, DeviceDesc& eDesc, 
    FrameBufferFormat& eFBFormat, 
    DepthStencilFormat& eDSFormat, 
    PresentationInterval& ePresentationInterval, 
    SwapEffect& eSwapEffect,
    unsigned int& uiFBMode, 
    unsigned int& uiBackBufferCount, 
    unsigned int& uiRefreshRate) const
{
    uiWidth = m_uiCreationWidth;
    uiHeight = m_uiCreationHeight;
    uiUseFlags = m_uiCreationUseFlags;
    kWndDevice = m_kCreationWndDevice;
    kWndFocus = m_kCreationWndFocus;
    uiAdapter = m_uiCreationAdapter;
    eDesc = m_eCreationDesc;
    eFBFormat = m_eCreationFBFormat;
    eDSFormat = m_eCreationDSFormat;
    ePresentationInterval = m_eCreationPresentationInterval;
    eSwapEffect = m_eCreationSwapEffect;
    uiFBMode = m_uiCreationFBMode;
    uiBackBufferCount = m_uiCreationBackBufferCount;
    uiRefreshRate = m_uiCreationRefreshRate;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateSwapChainRenderTargetGroup(
    unsigned int uiUseFlags,
    NiWindowRef kWndDevice, 
    FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, 
    PresentationInterval ePresentationInterval,
    SwapEffect eSwapEffect, 
    unsigned int uiFBMode, 
    unsigned int uiBackBufferCount, 
    unsigned int uiRefreshRate)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Does the framebuffer already exist?
    if (!kWndDevice)
    {
        return true;
    }
    else
    {
        NiRenderTargetGroupPtr spRenderTargetGroup;
        
        // Does the framebuffer exist?
        if (m_kOnscreenRenderTargetGroups.GetAt(kWndDevice, 
            spRenderTargetGroup))
        {
            return true;
        }
    }

    D3DPRESENT_PARAMETERS kD3DPresentParams;

    if (!InitializePresentParams(kWndDevice, 0, 0, uiUseFlags, 
        uiFBMode, eFBFormat, eDSFormat, uiBackBufferCount, 
        eSwapEffect, uiRefreshRate, ePresentationInterval,
        kD3DPresentParams))
    {
        return false;
    }

    Ni2DBuffer* pkNewBuffer = NULL;
    NiDX9SwapChainBufferData* pkNewTarget = 
        NiDX9SwapChainBufferData::Create(m_pkD3DDevice9, 
        kD3DPresentParams, pkNewBuffer);

    if (pkNewTarget)
    {
        NiRenderTargetGroup* pkRenderTargetGroup = 
            NiRenderTargetGroup::Create(1, this);
        pkRenderTargetGroup->AttachBuffer(pkNewBuffer, 0);
        m_kOnscreenRenderTargetGroups.SetAt(kWndDevice, pkRenderTargetGroup);

        NiDepthStencilBuffer* pkDSBuffer = NULL;
        NiDX9SwapChainDepthStencilBufferData::Create(m_pkD3DDevice9, 
            pkNewTarget, pkDSBuffer);

        if (pkDSBuffer)
        {
            pkRenderTargetGroup->AttachDepthStencilBuffer(pkDSBuffer);
        }
    
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::DestroySwapChainRenderTargetGroup(NiWindowRef kWndDevice)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Does the framebuffer exist?
    if (!kWndDevice)
    {
        return;
    }
    else
    {
        NiRenderTargetGroupPtr spRenderTargetGroup = NULL;

        // Does the framebuffer exist?
        if (!m_kOnscreenRenderTargetGroups.GetAt(kWndDevice, 
            spRenderTargetGroup))
        {
            return;
        }

        // can only delete non-primary buffers that are not current targets
        if (spRenderTargetGroup != m_pkCurrOnscreenRenderTargetGroup)
        {
            if (spRenderTargetGroup != m_spDefaultRenderTargetGroup)
                m_kOnscreenRenderTargetGroups.RemoveAt(kWndDevice);
            spRenderTargetGroup = NULL;
        }
        else
        {
            Error("NiDX9Renderer::DestroyWindowedFramebuffer - destroying "
                "swap chain that is a current render target\n");
        }
    }
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiDX9Renderer::GetSwapChainRenderTargetGroup(NiWindowRef 
    kWndDevice) const
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!kWndDevice)
    {
        return NULL;
    }
    else
    {
        NiRenderTargetGroupPtr spRenderTargetGroup = NULL;

        // Does the framebuffer exist?
        if (m_kOnscreenRenderTargetGroups.GetAt(kWndDevice, 
            spRenderTargetGroup))
        {
            return spRenderTargetGroup;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializePresentParams(NiWindowRef kWndDevice,
    unsigned int uiWidth, 
    unsigned int uiHeight, 
    unsigned int uiUseFlags, 
    unsigned int uiFBMode, 
    FrameBufferFormat eFBFormat, 
    DepthStencilFormat eDSFormat, 
    unsigned int uiBackBufferCount, 
    SwapEffect eSwapEffect, 
    unsigned int uiRefreshRate, 
    PresentationInterval ePresentationInterval, 
    D3DPRESENT_PARAMETERS& kD3DPresentParams) const
{
    // Assumes that the following members are valid at call time:
    // m_pkDeviceInfo
    // m_uiAdapter, m_eDevType
    // m_kWndDevice

    bool bWindowed = (uiUseFlags & USE_FULLSCREEN) ? false : true;

    const D3DCAPS9& kD3dCaps9 = m_pkDeviceDesc->GetCaps();

    unsigned int uiBitDepth = 32;
    if (uiUseFlags & USE_16BITBUFFERS)
        uiBitDepth = 16;

    // If eFBFormat is UNKNOWN, then the renderer must come up with the best
    // format, based on supported formats in the hardware
    if (eFBFormat == FBFMT_UNKNOWN)
    {
        eFBFormat = m_pkDeviceDesc->GetNearestFrameBufferFormat(bWindowed, 
            uiBitDepth);
    }

    if (eFBFormat == FBFMT_UNKNOWN)
    {
        SetLastErrorString("Creation failed: Could not find desired "
            "framebuffer format");
        Warning("NiDX9Renderer::Initialize> No matching "
            "framebuffer format available\n");

        return false;
    }

    D3DFORMAT eD3DFBFormat = GetD3DFormat(eFBFormat);
    D3DFORMAT eAdapterFormat = GetCompatibleAdapterFormat(eFBFormat);

    // If eDSFormat is UNKNOWN, then the renderer must come up with the best
    // format, based on supported formats in the hardware
    if (eDSFormat == DSFMT_UNKNOWN)
    {
        eDSFormat = m_pkDeviceDesc->GetNearestDepthStencilFormat(
            eAdapterFormat, eD3DFBFormat, 
            uiBitDepth, (uiUseFlags & USE_STENCIL) ? 8 : 0);
    }

    if (eDSFormat == DSFMT_UNKNOWN)
    {
        SetLastErrorString("Creation failed: Could not find desired "
            "depth/stencil format");
        Warning("NiDX9Renderer::Initialize> No matching "
            "depth/stencil format available\n");

        return false;
    }

    memset((void*)&kD3DPresentParams, 0, sizeof(kD3DPresentParams));

    kD3DPresentParams.BackBufferWidth = uiWidth;
    kD3DPresentParams.BackBufferHeight = uiHeight;

    kD3DPresentParams.BackBufferFormat = eD3DFBFormat;

    kD3DPresentParams.MultiSampleType = GetMultiSampleType(uiFBMode);
    kD3DPresentParams.MultiSampleQuality = GetMultiSampleQuality(uiFBMode);

    kD3DPresentParams.AutoDepthStencilFormat = GetD3DFormat(eDSFormat);
    
    kD3DPresentParams.EnableAutoDepthStencil = 
        (uiUseFlags & USE_MANUALDEPTHSTENCIL) ? false : true;
    
    kD3DPresentParams.BackBufferCount = uiBackBufferCount;

    kD3DPresentParams.Windowed = bWindowed;

    kD3DPresentParams.SwapEffect = GetSwapEffect(eSwapEffect, uiFBMode, 
        uiUseFlags);
    kD3DPresentParams.Flags = (uiFBMode == FBMODE_LOCKABLE) 
        ? D3DPRESENTFLAG_LOCKABLE_BACKBUFFER : 0;

    kD3DPresentParams.hDeviceWindow = kWndDevice;

    kD3DPresentParams.FullScreen_RefreshRateInHz = 
        GetRefreshRate(uiRefreshRate);
    kD3DPresentParams.PresentationInterval = 
        GetPresentInterval(ePresentationInterval);

    ///////////////////////
    // Multisample Validate
    DWORD uiQualityLevels;
    HRESULT eD3dRet = ms_pkD3D9->CheckDeviceMultiSampleType(m_uiAdapter,
        m_eDevType, kD3DPresentParams.BackBufferFormat,
        kD3DPresentParams.Windowed, kD3DPresentParams.MultiSampleType, 
        &uiQualityLevels);

    // If the selected mode can't be supported, default to NONE
    if (FAILED(eD3dRet))
    {
        Warning("NiDX9Renderer::Initialize> Could not support "
            "requested antialiasing mode - disabling\n");
        kD3DPresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
    }
    else if (uiQualityLevels <= kD3DPresentParams.MultiSampleQuality)
    {
        kD3DPresentParams.MultiSampleQuality = uiQualityLevels - 1;
    }

    ///////////////////////////////////
    // Backbuffer number validate
    // This cannot be fully validated until we create the device, but we'll
    // avoid at least one iteration by validating the basic range
    if (kD3DPresentParams.BackBufferCount == 0)
        kD3DPresentParams.BackBufferCount = 1;
    else if (kD3DPresentParams.BackBufferCount > 3)
        kD3DPresentParams.BackBufferCount = 3;
    
    unsigned int uiRequestedRefresh 
        = kD3DPresentParams.FullScreen_RefreshRateInHz;

    ///////////////////////////////////
    // Fullscreen display mode validate (including refresh rate)
    // Iterate over all modes, looking for a match - will return a
    // valid refresh rate if everything but the refresh match
    if (kD3DPresentParams.Windowed)
    {
        kD3DPresentParams.FullScreen_RefreshRateInHz = 0;
    }
    else
    {
        if (!m_pkAdapterDesc->ValidateFullscreenMode(
            GetCompatibleAdapterFormat(GetNiFBFormat(
            kD3DPresentParams.BackBufferFormat)),
            kD3DPresentParams.BackBufferWidth,
            kD3DPresentParams.BackBufferHeight, 
            kD3DPresentParams.FullScreen_RefreshRateInHz))
        {
            SetLastErrorString("Creation failed: Could not match desired "
                "fullscreen mode");
            Warning("NiDX9Renderer::Initialize> Could not find "
                "matching fullscreen display mode\n");

            return false;
        }

        if (uiRequestedRefresh 
            != kD3DPresentParams.FullScreen_RefreshRateInHz)
        {
            Warning("NiDX9Renderer::Initialize> Could not match "
                "requested fullscreen refresh rate - using closest\n");
        }
    }

    ////////////////////////////
    // Present Interval Validate
    if (kD3DPresentParams.Windowed)
    {
        if (kD3DPresentParams.PresentationInterval != 
            D3DPRESENT_INTERVAL_DEFAULT &&
            kD3DPresentParams.PresentationInterval != 
            D3DPRESENT_INTERVAL_IMMEDIATE &&
            kD3DPresentParams.PresentationInterval != 
            D3DPRESENT_INTERVAL_ONE)
        {
            kD3DPresentParams.PresentationInterval = 
                D3DPRESENT_INTERVAL_IMMEDIATE;
        }
    }
    else if (!(kD3DPresentParams.PresentationInterval &
        kD3dCaps9.PresentationIntervals))
    {
        kD3DPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializeTextureDefaults() 
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Texture initialization.  Dynamic textures use Source Texture formats.
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_TEX], 0, 
        D3DRTYPE_TEXTURE);
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_RENDERED_TEX], 
        D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE);
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_CUBE], 0, 
        D3DRTYPE_CUBETEXTURE);
    CreatePixelFormatArray(m_aapkTextureFormats[TEXUSE_RENDERED_CUBE], 
        D3DUSAGE_RENDERTARGET, D3DRTYPE_CUBETEXTURE);
    GenerateDefaultTextureData();

    // Create default "clipper" texture
    NiPixelFormat* pkDefault = m_apkDefaultTextureFormat[TEXUSE_TEX];

    NiPixelDataPtr spRaw = NiNew NiPixelData(2, 1, *pkDefault);
    unsigned char* pkData = spRaw->GetPixels();
    if (pkDefault->GetBitsPerPixel() == 16)
    {
        pkData[0] = pkData[1] = 0;
        pkData[2] = pkData[3] = 255;
    }
    else if (pkDefault->GetBitsPerPixel() == 32)
    {
        pkData[0] = pkData[1] = pkData[2] = pkData[3] = 0;
        pkData[4] = pkData[5] = pkData[6] = pkData[7] = 255;
    }

    NiTexture::FormatPrefs kPrefs;
    kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
    kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
    m_spClipperImage = NiSourceTexture::Create(spRaw, kPrefs);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::InitializeDeviceCaps(
    const D3DPRESENT_PARAMETERS&) 
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Device capability initialization
    HRESULT hr = m_pkD3DDevice9->GetDeviceCaps(&m_kD3DCaps9);

    if (FAILED(hr))
    {
        SetLastErrorString("Creation failed: Could not query device caps");
        Error("NiDX9Renderer::Initialize> Could not query device caps "
            "- FAILING\n");
     
        return false;
    }

    // The device must be able to handle 2-texture, 2-stage multitexture for
    // now, as we assume it.
    if ((m_kD3DCaps9.MaxTextureBlendStages < 2) || 
        (m_kD3DCaps9.MaxSimultaneousTextures < 2))
    {
        SetLastErrorString("Creation failed: Device does not support "
            "single-pass multi-texture");
        Error("NiDX9Renderer::Initialize> Device does not support single-pass "
            "multi-texture - FAILING\n");
     
        return false;
    }

    m_uiRendFlags = 0;

    if ((m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_POW2) == 0)
        m_uiRendFlags |= CAPS_NONPOW2_TEXT;
    else if (m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
        m_uiRendFlags |= CAPS_NONPOW2_CONDITIONAL_TEXT;

    if ((m_kD3DCaps9.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) ||
       (m_kD3DCaps9.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC))
    {
        m_uiRendFlags |= CAPS_ANISO_FILTERING;
        m_usHwMaxAnisotropy = static_cast<unsigned short>(m_kD3DCaps9.MaxAnisotropy);
        m_usMaxAnisotropy = m_usHwMaxAnisotropy; 
    }

    m_uiHWBones = m_kD3DCaps9.MaxVertexBlendMatrices;

    if (m_uiHWBones >= HW_BONE_LIMIT)
        m_uiRendFlags |= CAPS_HARDWARESKINNING;

    if (m_kD3DCaps9.VertexShaderVersion >= D3DVS_VERSION(3, 0))
    {
        m_uiRendFlags |= CAPS_HARDWAREINSTANCING;
    }
    else if (m_kD3DCaps9.VertexShaderVersion >= D3DVS_VERSION(2, 0))
    {
        // Some ATI VS2.0 cards have instancing support, but there is no
        // DX9 Caps bit to check for this functionality. Instead the work
        // around below allows us to check for and enable hardware instancing 
        // support.
        if( SUCCEEDED(ms_pkD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
            D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, 
            (D3DFORMAT)MAKEFOURCC('I','N','S','T')) ) )
        { 
            // Enable instancing 
            m_pkD3DDevice9->SetRenderState(D3DRS_POINTSIZE, 
                MAKEFOURCC('I','N','S','T')); 

            m_uiRendFlags |= CAPS_HARDWAREINSTANCING;

            // No need to set D3DRS_POINTSIZE back to its default state, 
            // since the render state will be initialized
            // when the shader system is initialized.
        } 
    }

    m_uiMaxStreams = m_kD3DCaps9.MaxStreams;
    m_uiMaxPixelShaderVersion = m_kD3DCaps9.PixelShaderVersion;
    m_uiMaxVertexShaderVersion = m_kD3DCaps9.VertexShaderVersion;
    m_bMipmapCubeMaps = 
        (m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP) != 0;
    m_bDynamicTexturesCapable =
        (m_kD3DCaps9.Caps2 & D3DCAPS2_DYNAMICTEXTURES) != 0;
    m_uiMaxNumRenderTargets = m_kD3DCaps9.NumSimultaneousRTs;
    m_bIndependentBitDepths = (m_kD3DCaps9.PrimitiveMiscCaps & 
        D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) != 0;
    m_bMRTPostPixelShaderBlending = (m_kD3DCaps9.PrimitiveMiscCaps & 
        D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) != 0;

    // Setup the default shader version information
    m_kShaderLibraryVersion.SetSystemPixelShaderVersion(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetMinPixelShaderVersion(0, 0);
    m_kShaderLibraryVersion.SetPixelShaderVersionRequest(
        m_uiMaxPixelShaderVersion);
    m_kShaderLibraryVersion.SetSystemVertexShaderVersion(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetMinVertexShaderVersion(0, 0);
    m_kShaderLibraryVersion.SetVertexShaderVersionRequest(
        m_uiMaxVertexShaderVersion);
    m_kShaderLibraryVersion.SetSystemUserVersion(0, 0);
    m_kShaderLibraryVersion.SetMinUserVersion(0, 0);
    m_kShaderLibraryVersion.SetUserVersionRequest(0, 0);
    m_kShaderLibraryVersion.SetPlatform(NiShader::NISHADER_DX9);

    NiD3DPass::SetMaxTextureBlendStages(
        m_kD3DCaps9.MaxTextureBlendStages);
    NiD3DPass::SetMaxSimultaneousTextures(
        m_kD3DCaps9.MaxSimultaneousTextures);

    if (m_uiMaxPixelShaderVersion >= D3DPS_VERSION(2, 0))
        NiD3DPass::SetMaxSamplers(16);
    else
        NiD3DPass::SetMaxSamplers(m_kD3DCaps9.MaxTextureBlendStages);

    // Address mappings
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_WRAP_T].m_eU = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_WRAP_T].m_eV = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_CLAMP_T].m_eU = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::WRAP_S_CLAMP_T].m_eV = 
        D3DTADDRESS_CLAMP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_WRAP_T].m_eU = 
        D3DTADDRESS_CLAMP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_WRAP_T].m_eV = 
        D3DTADDRESS_WRAP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_CLAMP_T].m_eU = 
        D3DTADDRESS_CLAMP;
    ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_S_CLAMP_T].m_eV = 
        D3DTADDRESS_CLAMP;

    // FILTER_NEAREST
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_NEAREST].m_eMag = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_NEAREST].m_eMin = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_NEAREST].m_eMip = 
        D3DTEXF_NONE;
    //  FILTER_BILERP
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_BILERP].m_eMag = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_BILERP].m_eMin = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_BILERP].m_eMip = 
        D3DTEXF_NONE;
    //  FILTER_TRILERP
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_TRILERP].m_eMag = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_TRILERP].m_eMin = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_TRILERP].m_eMip = 
        D3DTEXF_LINEAR;
    //  FILTER_NEAREST_MIPNEAREST
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPNEAREST].m_eMag = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPNEAREST].m_eMin = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPNEAREST].m_eMip = 
        D3DTEXF_POINT;
    //  FILTER_NEAREST_MIPLERP
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPLERP].m_eMag = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPLERP].m_eMin = 
        D3DTEXF_POINT;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_NEAREST_MIPLERP].m_eMip = 
        D3DTEXF_LINEAR;
    //  FILTER_BILERP_MIPNEAREST
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_BILERP_MIPNEAREST].m_eMag = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_BILERP_MIPNEAREST].m_eMin = 
        D3DTEXF_LINEAR;
    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_BILERP_MIPNEAREST].m_eMip = 
        D3DTEXF_POINT;

    //  FILTER_ANISOTROPIC
    //  check on support anisotropic filtering
    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_ANISOTROPIC].m_eMag = 
        (m_kD3DCaps9.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) ? 
        D3DTEXF_ANISOTROPIC : 
        D3DTEXF_LINEAR;

    ms_akD3DFilterMapping[NiTexturingProperty::FILTER_ANISOTROPIC].m_eMin = 
        (m_kD3DCaps9.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) ?
        D3DTEXF_ANISOTROPIC : 
        D3DTEXF_LINEAR;

    ms_akD3DFilterMapping[
        NiTexturingProperty::FILTER_ANISOTROPIC].m_eMip = 
        D3DTEXF_LINEAR;

    // Projected
    if (m_kD3DCaps9.TextureCaps & D3DPTEXTURECAPS_PROJECTED)
    {
        ms_uiProjectedTextureFlags = D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
    }
    else
    {
        ms_uiProjectedTextureFlags = D3DTTFF_COUNT2;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::ExpandDeviceType(DeviceDesc eDesc, D3DDEVTYPE& eDevType,
    unsigned int& uiBehaviorFlags)
{
    // Remove VP/pure flags from uiBehaviorFlags
    uiBehaviorFlags &= ~(D3DCREATE_HARDWARE_VERTEXPROCESSING | 
        D3DCREATE_MIXED_VERTEXPROCESSING |
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE);

    switch (eDesc)
    {
        case DEVDESC_PURE:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING |
                D3DCREATE_PUREDEVICE;
            break;
        case DEVDESC_HAL_HWVERTEX:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
            break;
        case DEVDESC_HAL_MIXEDVERTEX:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
            break;
        case DEVDESC_HAL_SWVERTEX:
            eDevType = D3DDEVTYPE_HAL;
            uiBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            break;
        case DEVDESC_REF_HWVERTEX:
            eDevType = D3DDEVTYPE_REF;
            uiBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
            break;
        case DEVDESC_REF_MIXEDVERTEX:
            eDevType = D3DDEVTYPE_REF;
            uiBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
            break;
        case DEVDESC_REF:
            eDevType = D3DDEVTYPE_REF;
            uiBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            break;
        default:
            return false;
    };

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GenerateDefaultTextureData()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    const unsigned int uiDefaultFormatNum = 6;
    TexFormat aeFormats[uiDefaultFormatNum] =
    {
        TEX_RGB565,
        TEX_RGB555,
        TEX_RGB888,
        TEX_RGBA4444,
        TEX_RGBA5551,
        TEX_RGBA8888
    };

    unsigned int uiFmtIndex, uiUsage;
    for (uiUsage = 0; uiUsage < TEXUSE_NUM; uiUsage++)
    {
        m_apkDefaultTextureFormat[uiUsage] = NULL;

        for (uiFmtIndex = 0; uiFmtIndex < uiDefaultFormatNum; uiFmtIndex++)
        {
            TexFormat eFmt = aeFormats[uiFmtIndex];

            if (m_aapkTextureFormats[uiUsage][eFmt])
            {
                m_apkDefaultTextureFormat[uiUsage] = 
                    m_aapkTextureFormats[uiUsage][eFmt];
                break;
            }
        }

        if (m_apkDefaultTextureFormat[uiUsage])
        {
            m_aspDefaultTextureData[uiUsage] = NiNew NiPixelData(4, 4, 
                *m_apkDefaultTextureFormat[uiUsage], 1);

            unsigned char* pkPixels = 
                m_aspDefaultTextureData[uiUsage]->GetPixels();
            memset(pkPixels, 0xff, 
                4 * 4 * m_aspDefaultTextureData[uiUsage]->GetPixelStride());
        }
    }

    // Must find a nonrendered, flat texture format (other types such
    // as cubes and rendered textures are optional)
    if (!m_apkDefaultTextureFormat[TEXUSE_TEX])
        return false;

    NiTexture::RendererData::SetTextureReplacementFormat(
        *m_apkDefaultTextureFormat[TEXUSE_TEX]);
    m_eReplacementDataFormat = (TexFormat)
        m_apkDefaultTextureFormat[TEXUSE_TEX]->GetExtraData();

    return true;
}
//---------------------------------------------------------------------------
const char* NiDX9Renderer::GetDriverInfo() const
{
    D3DADAPTER_IDENTIFIER9 kD3DAdapterIdentifier9;

    m_acDriverDesc[0] = 0;
    memset((void*)&kD3DAdapterIdentifier9, 0, sizeof(kD3DAdapterIdentifier9));

    if (SUCCEEDED(ms_pkD3D9->GetAdapterIdentifier(m_uiAdapter,
        0, &kD3DAdapterIdentifier9)))
    {
        NiSprintf(m_acDriverDesc, 512, "%s (%s-%s)", 
            kD3DAdapterIdentifier9.Description, GetDevTypeString(), 
            GetBehaviorString());
    }

    return m_acDriverDesc;
}
//---------------------------------------------------------------------------
NiSystemDesc::RendererID NiDX9Renderer::GetRendererID() const
{
    return NiSystemDesc::RENDERER_DX9;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9Renderer::FindClosestPixelFormat(
    NiTexture::FormatPrefs& kFmtPrefs) const
{
    return NiDX9TextureData::FindClosestPixelFormat(kFmtPrefs,
        m_aapkTextureFormats[TEXUSE_TEX]);
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9Renderer::FindClosestPixelFormat(
    TexFormat eFormat, TexUsage eUsage) const
{
    NIASSERT(m_aapkTextureFormats != NULL);
    NIASSERT(TEX_NUM > eFormat);
    NIASSERT(TEXUSE_NUM > eUsage);
    return m_aapkTextureFormats[eUsage][eFormat];
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9Renderer::FindClosestDepthStencilFormat(
    const NiPixelFormat* pkFrontBufferFormat, unsigned int uiDepthBPP,
    unsigned int uiStencilBPP) const
{
    NIASSERT(pkFrontBufferFormat);

    NiDX9OnscreenBufferData* pkData = NiDynamicCast(
        NiDX9OnscreenBufferData, (NiDX92DBufferData*)
        m_spDefaultRenderTargetGroup->GetBufferRendererData(0));
    NIASSERT(pkData);

    D3DFORMAT eD3DFBFormat = NiDX9PixelFormat::DetermineD3DFormat(
        *pkFrontBufferFormat);
    NiDX9Renderer::DepthStencilFormat eDSFormat = 
        m_pkDeviceDesc->GetNearestDepthStencilFormat(
        pkData->GetPresentParams().BackBufferFormat, 
        eD3DFBFormat, uiDepthBPP, uiStencilBPP);

    NIASSERT(eDSFormat != DSFMT_UNKNOWN);

    D3DFORMAT eD3DDSFormat = NiDX9Renderer::GetD3DFormat(eDSFormat);
    NIASSERT(eD3DDSFormat != D3DFMT_UNKNOWN);

    NiPixelFormat* pkTemp = NULL;
    if (!m_kDepthStencilFormats.GetAt(eD3DDSFormat, pkTemp) || pkTemp == NULL)
    {
        pkTemp = NiDX9PixelFormat::CreateFromD3DFormat(eD3DDSFormat);
        m_kDepthStencilFormats.SetAt(eD3DDSFormat, pkTemp);
    }

    return pkTemp;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetMRTPostPixelShaderBlendingCapability() const
{
    return m_bMRTPostPixelShaderBlending;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetFormatPostPixelShaderBlendingCapability(
    D3DFORMAT eFormat) const
{
    HRESULT hr = ms_pkD3D9->CheckDeviceFormat(m_uiAdapter, m_eDevType,
        m_eAdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
        D3DRTYPE_TEXTURE, eFormat);
    return SUCCEEDED(hr);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_BeginUsingRenderTargetGroup(
    NiRenderTargetGroup* pkTarget, 
    unsigned int uiClearMode)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!m_bDeviceLost)
    {
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(RENDER_TARGET_CHANGES, 1);

        NIASSERT(ValidateRenderTargetGroup(pkTarget));

        // Clear existing render targets
        NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 1);
        NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 2);
        NiDX92DBufferData::ClearRenderTarget(m_pkD3DDevice9, 3);
        
        for (unsigned int ui = 0; ui < pkTarget->GetBufferCount(); ui++)
        {
            NiDX92DBufferData* pkBuffData = NiDynamicCast(NiDX92DBufferData, 
                (NiDX92DBufferData*)pkTarget->GetBufferRendererData(ui));
            if (pkBuffData)
            {
                bool bSet = pkBuffData->SetRenderTarget(m_pkD3DDevice9, ui);
                if (!bSet)
                {
                    NIVERIFY(Do_BeginUsingRenderTargetGroup(
                            m_spDefaultRenderTargetGroup,
                            uiClearMode));

                    Warning("NiDX9Renderer::BeginUsingRenderTargetGroup> "
                        "Failed - %s", "Setting an output target failed."
                        " Resetting to default target.");
                    return false;
                }
            }
        }

        NiDX9DepthStencilBufferData* pkDepthData = NiDynamicCast(
            NiDX9DepthStencilBufferData, (NiDX92DBufferData*)
            pkTarget->GetDepthStencilBufferRendererData());

        if (pkDepthData)
        {
            bool bSet = pkDepthData->SetDepthTarget(m_pkD3DDevice9);
            if (!bSet)
            {
                NIVERIFY(Do_BeginUsingRenderTargetGroup(
                    m_spDefaultRenderTargetGroup, uiClearMode));

                Warning("NiDX9Renderer::BeginUsingRenderTargetGroup> "
                    "Failed - %s", "Setting the depth/stencil buffer failed."
                    " Resetting to default target.");
                return false;
            }
        }
        else
        {
            if (!NiDX9DepthStencilBufferData::SetNULLDepthStencilTarget(
                m_pkD3DDevice9))
            {

                NIVERIFY(Do_BeginUsingRenderTargetGroup(
                    m_spDefaultRenderTargetGroup, uiClearMode));


                Warning("NiDX9Renderer::BeginUsingRenderTargetGroup> "
                    "Failed - %s", "Setting the depth/stencil buffer to NULL"
                    " failed. Resetting to default target.");
                return false;
            }
        }

        NiDX9OnscreenBufferData* pkOnscreenBuffData = NiDynamicCast(
            NiDX9OnscreenBufferData, (NiDX92DBufferData*)
            pkTarget->GetBufferRendererData(0));

        m_pkCurrRenderTargetGroup = pkTarget;
        if (pkOnscreenBuffData)
            m_pkCurrOnscreenRenderTargetGroup = pkTarget;

        Do_ClearBuffer(NULL, uiClearMode);
    }

    // Return true even if the device is lost.
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_EndUsingRenderTargetGroup()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!m_bDeviceLost)
    {
        NIASSERT(m_pkCurrRenderTargetGroup);

        for (unsigned int ui = 0; 
            ui < m_pkCurrRenderTargetGroup->GetBufferCount(); ui++)
        {

            NiDX92DBufferData* pkBuffData = NiDynamicCast(
                NiDX92DBufferData, (NiDX92DBufferData*)
                m_pkCurrRenderTargetGroup->GetBufferRendererData(ui));
            NIASSERT(pkBuffData);

            // Store for DisplayFrame
            if (pkBuffData->CanDisplayFrame())
            {
                // Only store once
                if (!m_kBuffersToUseAtDisplayFrame.FindPos(pkBuffData))
                    m_kBuffersToUseAtDisplayFrame.AddTail(pkBuffData);
            }
        }
        
        m_pkCurrRenderTargetGroup = NULL;
        m_pkCurrOnscreenRenderTargetGroup = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::ValidateRenderTargetGroup(
    NiRenderTargetGroup* pkTarget)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    unsigned int uiBuffers = pkTarget->GetBufferCount();
    if (uiBuffers > GetMaxBuffersPerRenderTargetGroup())
        return false;

    for (unsigned int ui = 0; ui < uiBuffers; ui++)
    {
        NiDX92DBufferData* pkBuffer = (NiDX92DBufferData*)
            pkTarget->GetBufferRendererData(ui);

        if (!pkBuffer || !pkBuffer->IsValid())
            return false;

        for (unsigned int uj = 0; uj < ui; uj++)
        {
            NiDX92DBufferData* pkCompBuffer = (NiDX92DBufferData*)
                pkTarget->GetBufferRendererData(uj);

            LPDIRECT3DSURFACE9 pkCompare = 
                pkCompBuffer->GetSurface();

            D3DSURFACE_DESC kDesc;
            HRESULT hr = pkCompare->GetDesc(&kDesc);
            if (FAILED(hr))
                return false;

            // Antialiasing not supported
            if (kDesc.MultiSampleType != D3DMULTISAMPLE_NONE)
                return false;

            // Render target must be same width and height
            if (pkBuffer->GetWidth() != kDesc.Width || 
                pkBuffer->GetHeight() != kDesc.Height)
            {
                return false;
            }

            // Render target may need to be the same texture format, or
            // possibly a format with the same bit depth
            if (!GetIndependentBufferBitDepths())
            {
                unsigned char ucCompareBitDepth = 
                    NiDX9Renderer::GetD3DFormatSize(kDesc.Format);

                unsigned char ucBufferBitDepth = 
                    pkBuffer->GetPixelFormat()->GetBitsPerPixel();

                if (ucCompareBitDepth != ucBufferBitDepth)
                    return false;
            }
        }
    }

    NiDepthStencilBuffer* pkDSBuffer = pkTarget->GetDepthStencilBuffer();
    if (pkDSBuffer)
    {
        for (unsigned int ui = 0; ui < uiBuffers; ui++)
        {
            if (!IsDepthBufferCompatible(pkTarget->GetBuffer(ui), pkDSBuffer))
                return false;
        }
    }
    
    NiDX9DepthStencilBufferData* pkDSData = NiDynamicCast(
        NiDX9DepthStencilBufferData, (NiDX92DBufferData*)
        pkTarget->GetDepthStencilBufferRendererData());

    if (pkDSData != NULL)
    {
        if (!pkDSData->IsValid())
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::IsDepthBufferCompatible(Ni2DBuffer* pkBuffer,
    NiDepthStencilBuffer* pkDSBuffer)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Depth buffer may be NULL, but color buffer may not
    if (pkBuffer == NULL)
        return false;
    if (pkDSBuffer == NULL)
        return true;

    // Render target must be smaller than Depth Stencil
    if ((pkBuffer->GetWidth() > pkDSBuffer->GetWidth()) || 
        pkBuffer->GetHeight() > pkDSBuffer->GetHeight())
    {
        return false;
    }

    // Test targets are compatible together
    NiDX92DBufferData* pkColorData = 
        (NiDX92DBufferData*)pkBuffer->GetRendererData();
    NIASSERT(pkColorData);

    NiDX92DBufferData* pkDepthData =
        (NiDX92DBufferData*)pkDSBuffer->GetRendererData();
    NIASSERT(pkDepthData);

    D3DSURFACE_DESC kBufferDesc;
    D3DSURFACE_DESC kDepthDesc;
    HRESULT hr;

    hr = pkColorData->GetSurface()->GetDesc(&kBufferDesc);
    NIASSERT(SUCCEEDED(hr));

    hr = pkDepthData->GetSurface()->GetDesc(&kDepthDesc);
    NIASSERT(SUCCEEDED(hr));

    if ((kBufferDesc.MultiSampleQuality != kDepthDesc.MultiSampleQuality) ||
        (kBufferDesc.MultiSampleType != kDepthDesc.MultiSampleType))
        return false;

    hr = ms_pkD3D9->CheckDepthStencilMatch(GetAdapter(),
        GetDevType(), m_eAdapterFormat,
        kBufferDesc.Format, kDepthDesc.Format);
    if (FAILED(hr))
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiDX9Renderer::GetMaxBuffersPerRenderTargetGroup() const
{
    return m_uiMaxNumRenderTargets;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetIndependentBufferBitDepths() const
{
    return m_bIndependentBitDepths;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiDX9Renderer::GetDefaultRenderTargetGroup() const
{   
    return m_spDefaultRenderTargetGroup;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_ClearBuffer(const NiRect<float>* pkR, 
    unsigned int uiMode)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (uiMode == CLEAR_NONE)
        return;

    if (!m_bDeviceLost)
    {
        NIASSERT(m_pkCurrRenderTargetGroup);

        D3DRECT kClear;
        DWORD dwFlags = 0;

        //  If a rect is passed in, use it
        unsigned int uiBuffW = m_pkCurrRenderTargetGroup->GetWidth(0);
        unsigned int uiBuffH = m_pkCurrRenderTargetGroup->GetHeight(0);
        if (pkR)
        {
            float fW = (float)uiBuffW;
            float fH = (float)uiBuffH;
            // Clear the Back buffer
            kClear.x1 = (long)(fW * pkR->m_left);
            kClear.y1 = (long)(fH * (1.0f - pkR->m_top));
            kClear.x2 = (long)(fW * pkR->m_right); // Full window
            kClear.y2 = (long)(fH * (1.0f - pkR->m_bottom));
        }
        else
        {
            kClear.x1 = 0;
            kClear.y1 = 0;
            kClear.x2 = (long)uiBuffW;
            kClear.y2 = (long)uiBuffH;
        }

        dwFlags |= ((uiMode & NiRenderer::CLEAR_BACKBUFFER) ? 
            D3DCLEAR_TARGET : 0);

        NiDX9DepthStencilBufferData* pkDSData = NiDynamicCast(
            NiDX9DepthStencilBufferData, (NiDX92DBufferData*)
            m_pkCurrRenderTargetGroup->GetDepthStencilBufferRendererData());

        if (pkDSData)
        {
            NIASSERT(((uiMode & NiRenderer::CLEAR_ZBUFFER) == 0) ||
                pkDSData->HasValidDepthBuffer());

            dwFlags |= (((uiMode & NiRenderer::CLEAR_ZBUFFER) && 
                pkDSData->HasValidDepthBuffer()) ? D3DCLEAR_ZBUFFER : 0);
        
            dwFlags |= (((uiMode & NiRenderer::CLEAR_STENCIL) && 
                pkDSData->HasValidStencilBuffer()) ? D3DCLEAR_STENCIL : 0);
        }

        m_kD3DPort.Width = m_pkCurrRenderTargetGroup->GetWidth(0);
        m_kD3DPort.Height = m_pkCurrRenderTargetGroup->GetHeight(0);
        m_pkD3DDevice9->SetViewport(&m_kD3DPort);

        HRESULT hrD3DDeviceClear = 
            m_pkD3DDevice9->Clear(1, &kClear, dwFlags, 
            m_uiBackground, m_fZClear, m_uiStencilClear);
        NI_UNUSED_ARG(hrD3DDeviceClear);
        NIASSERT(!FAILED(hrD3DDeviceClear));
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiDX9Renderer::TakeScreenShot(
    const NiRect<unsigned int>* pkScreenRect, 
    const NiRenderTargetGroup* pkTarget)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (m_bDeviceLost)
        return NULL;

    if (pkTarget == NULL)
        pkTarget = m_spDefaultRenderTargetGroup;

    NIASSERT(pkTarget);
    NIASSERT(pkTarget->IsValid());

    RECT* pkRect = NULL;
    RECT kRect;

    unsigned int uiFBWidth = pkTarget->GetWidth(0);
    unsigned int uiFBHeight = pkTarget->GetHeight(0);
    unsigned int uiSSWidth;     // the width of the screenshot pixeldata
    unsigned int uiSSHeight;    // the height of the screenshot pixeldata

    if (pkScreenRect)
    {
        kRect.left = pkScreenRect->m_left;
        kRect.right = pkScreenRect->m_right;
        kRect.top = pkScreenRect->m_top;
        kRect.bottom = pkScreenRect->m_bottom;

        if ((kRect.left >= (float)uiFBWidth) || 
            (kRect.bottom >= (float)uiFBHeight))
            return NULL;
        if (kRect.right > (float)uiFBWidth)
            kRect.right = uiFBWidth;
        if (kRect.bottom > (float)uiFBHeight)
            kRect.bottom = uiFBHeight;

        uiSSWidth = kRect.right - kRect.left;
        uiSSHeight = kRect.bottom - kRect.top;

        pkRect = &kRect;
    }
    else
    {
        uiSSWidth = uiFBWidth;
        uiSSHeight = uiFBHeight;
    }


    NiDX92DBufferData* pkBuffData = NiDynamicCast(
        NiDX92DBufferData, (NiDX92DBufferData*)
        pkTarget->GetBufferRendererData(0));
    NIASSERT(pkBuffData);

    LPDIRECT3DSURFACE9 pkShotSurf = pkBuffData->GetSurface();

    // Get the surface description. Make sure it's a 32-bit format
    D3DSURFACE_DESC kDesc;
    pkShotSurf->GetDesc(&kDesc);

    // Lock the surface
    D3DLOCKED_RECT kLock;

    if (FAILED(pkShotSurf->LockRect(&kLock, pkRect, D3DLOCK_READONLY)))
        return NULL;

    // Create an NiPixelData object that matches the backbuffer in format and
    // size
    NiPixelData* pkPixelData = NiNew NiPixelData(uiSSWidth, uiSSHeight, 
        *(pkTarget->GetPixelFormat(0)), 1);

    if (pkPixelData == NULL)
    {
        pkShotSurf->UnlockRect();
        return NULL;
    }

    unsigned int uiRowSize 
        = pkPixelData->GetWidth(0) * pkPixelData->GetPixelStride();
    unsigned char* pucDest = pkPixelData->GetPixels(0);
    const unsigned char* pucSrc = (unsigned char*)kLock.pBits;

    for (unsigned int i = 0; i < uiSSHeight; i++)
    {
        NiMemcpy(pucDest, pucSrc, uiRowSize);

        pucDest += uiRowSize;
        pucSrc += kLock.Pitch;
    }

    // Cleanup and return
    pkShotSurf->UnlockRect();

    return pkPixelData;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::SaveScreenShot(const char* pcFilename,
    EScreenshotFormat eFormat)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    D3DXIMAGE_FILEFORMAT eFormatD3D;
    switch (eFormat)
    {
    case FORMAT_PNG:
        eFormatD3D = D3DXIFF_PNG;
        break;
    case FORMAT_JPEG:
        eFormatD3D = D3DXIFF_JPG;
        break;
    default:
        Error("NiDX9Renderer::SaveScreenShot> "
            "Unsupported image format %d\n", eFormat);
        return false;
    }

    char acImagePath[NI_MAX_PATH];

    // Make sure the path to the screenshot directory exists
    NiFilename kDirName(pcFilename);
    kDirName.SetFilename("");
    kDirName.SetExt("");
    kDirName.GetFullPath(acImagePath, NI_MAX_PATH);
    NiPath::Standardize(acImagePath);

    if ((strlen(acImagePath) > 0) && 
        !NiFile::CreateDirectoryRecursive(acImagePath)) {
        Error("NiDX9Renderer::SaveScreenShot> "
            "Could not create directory (\"%s\")\n", acImagePath);
        return false;
    }

    // Get the full filename
    NiFilename kName(pcFilename);
    kName.GetFullPath(acImagePath, NI_MAX_PATH);
    NiPath::Standardize(acImagePath);

    // Acquire the backbuffer
    LPDIRECT3DDEVICE9 pkDevice = GetD3DDevice();
    LPDIRECT3DSURFACE9 pkDestSurface = NULL;
    HRESULT hr = pkDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
        &pkDestSurface);
    if (SUCCEEDED(hr))
    {
        // Save the image to a file
        HRESULT hr2 = D3DXSaveSurfaceToFile(acImagePath,
            eFormatD3D, pkDestSurface, NULL, NULL);

        ReleaseResource(pkDestSurface);

        return SUCCEEDED(hr2);
    }
    else
    {
        // Failed to acquire the back buffer surface
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::FastCopy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    unsigned int uiDestX , unsigned int uiDestY)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (m_bDeviceLost)
        return false;

    NIASSERT(pkSrc != NULL);
    NIASSERT(pkDest != NULL);

    NiDX92DBufferData* pkSrcRendData = (NiDX92DBufferData*)
        pkSrc->GetRendererData();
    NiDX92DBufferData* pkDestRendData = (NiDX92DBufferData*)
        pkDest->GetRendererData();

    if (pkSrcRendData == NULL || pkDestRendData == NULL)
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed - %s", "No RendererData found");
        return false;
    }

    if (*(pkSrcRendData->GetPixelFormat()) != 
        *(pkDestRendData->GetPixelFormat()))
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed - %s", "Pixel formats do not match");
        return false;
    }
    
    LPDIRECT3DSURFACE9 pkSourceSurface = pkSrcRendData->GetSurface();
    LPDIRECT3DSURFACE9 pkDestSurface = pkDestRendData->GetSurface();

    if (pkSourceSurface == NULL || pkDestSurface == NULL)
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed - %s", "NULL Surface found");
        return false;
    }

    RECT kD3DSrcRect;
    if (pkSrcRect != NULL)
    {
        kD3DSrcRect.left    = pkSrcRect->m_left;
        kD3DSrcRect.right   = pkSrcRect->m_right;
        kD3DSrcRect.top     = pkSrcRect->m_top;
        kD3DSrcRect.bottom  = pkSrcRect->m_bottom;
    }
    else
    {
        // remove warning
        kD3DSrcRect.left    = 0;
        kD3DSrcRect.right   = 0;
        kD3DSrcRect.top     = 0;
        kD3DSrcRect.bottom  = 0;
    }


    NIASSERT(m_pkD3DDevice9);
    HRESULT hr;
    if (pkSrcRect != NULL)
    {
        RECT kD3DDestRect;
        kD3DDestRect.left   = uiDestX;
        kD3DDestRect.right  = uiDestX + kD3DSrcRect.right - kD3DSrcRect.left;
        kD3DDestRect.top    = uiDestY;
        kD3DDestRect.bottom = uiDestY + kD3DSrcRect.bottom - kD3DSrcRect.top;

        hr = m_pkD3DDevice9->StretchRect(pkSourceSurface,
            &kD3DSrcRect, pkDestSurface, &kD3DDestRect,
            D3DTEXF_NONE);
    }
    else
    {
        hr = m_pkD3DDevice9->StretchRect(pkSourceSurface,
            NULL, pkDestSurface, NULL, D3DTEXF_NONE);
    }

    if (FAILED(hr))
    {
        Warning("NiDX9Renderer::FastCopy> "
           "Failed StretchRect - %s", 
            NiDX9ErrorString((unsigned int)hr));
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Copy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
    const NiRect<unsigned int>* pkSrcRect,
    const NiRect<unsigned int>* pkDestRect,
    Ni2DBuffer::CopyFilterPreference ePref)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (m_bDeviceLost)
        return false;

    NIASSERT(pkSrc != NULL);
    NIASSERT(pkDest != NULL);

    NiDX92DBufferData* pkSrcRendData = (NiDX92DBufferData*)
        pkSrc->GetRendererData();
    NiDX92DBufferData* pkDestRendData = (NiDX92DBufferData*)
        pkDest->GetRendererData();

    if (pkSrcRendData == NULL || pkDestRendData == NULL)
    {
        Warning("NiDX9Renderer::Copy> "
           "Failed - %s", "No RendererData found");
        return false;
    }
    
    LPDIRECT3DSURFACE9 pkSourceSurface = pkSrcRendData->GetSurface();
    LPDIRECT3DSURFACE9 pkDestSurface = pkDestRendData->GetSurface();

    if (pkSourceSurface == NULL || pkDestSurface == NULL)
    {
        Warning("NiDX9Renderer::Copy> "
           "Failed - %s", "NULL Surface found");
        return false;
    }

    RECT kD3DSrcRect;
    if (pkSrcRect != NULL)
    {
        kD3DSrcRect.left    = pkSrcRect->m_left;
        kD3DSrcRect.right   = pkSrcRect->m_right;
        kD3DSrcRect.top     = pkSrcRect->m_top;
        kD3DSrcRect.bottom  = pkSrcRect->m_bottom;
    }
    else
    {
        kD3DSrcRect.left    = 0;
        kD3DSrcRect.right   = pkSrc->GetWidth();
        kD3DSrcRect.top     = 0;
        kD3DSrcRect.bottom  = pkSrc->GetHeight();
    }

    RECT kD3DDestRect;
    if (pkDestRect != NULL)
    {
        kD3DDestRect.left    = pkDestRect->m_left;
        kD3DDestRect.right   = pkDestRect->m_right;
        kD3DDestRect.top     = pkDestRect->m_top;
        kD3DDestRect.bottom  = pkDestRect->m_bottom;
    }
    else
    {
        kD3DDestRect.left    = 0;
        kD3DDestRect.right   = pkDest->GetWidth();
        kD3DDestRect.top     = 0;
        kD3DDestRect.bottom  = pkDest->GetHeight();
    }

    D3DTEXTUREFILTERTYPE eFilterType;
    switch (ePref)
    {
        default:
        case Ni2DBuffer::COPY_FILTER_NONE:
            eFilterType = D3DTEXF_NONE;
            break;
        case Ni2DBuffer::COPY_FILTER_POINT:
            eFilterType = D3DTEXF_POINT;
            break;
        case Ni2DBuffer::COPY_FILTER_LINEAR:
            eFilterType = D3DTEXF_LINEAR;
            break;
    }

    NIASSERT(m_pkD3DDevice9);
    HRESULT hr = m_pkD3DDevice9->StretchRect(pkSourceSurface,
        &kD3DSrcRect, pkDestSurface, &kD3DDestRect,
        eFilterType);

    if (FAILED(hr))
    {
        Warning("NiDX9Renderer::Copy> "
           "Failed StretchRect - %s", 
            NiDX9ErrorString((unsigned int)hr));
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiDX9Renderer::GetDefaultDepthStencilBuffer() const
{
    return m_spDefaultRenderTargetGroup->GetDepthStencilBuffer();
}
//---------------------------------------------------------------------------
Ni2DBuffer* NiDX9Renderer::GetDefaultBackBuffer() const
{
    return m_spDefaultRenderTargetGroup->GetBuffer(0);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PurgeAllTextures(bool)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiDX9TextureData::ClearTextureData(this);
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PurgeTexture(NiTexture* pkIm)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiDX9TextureData* pkData = (NiDX9TextureData*)
        (pkIm->GetRendererData());

    if (pkData)
    {
        NiDX9RenderedTextureData* pkRTData = pkData->GetAsRenderedTexture();

        if (pkRTData)
        {
            if (NiIsKindOf(NiRenderedCubeMap, pkIm))
            {
                NiRenderedCubeMap* pkRenderedCubeMap = 
                    (NiRenderedCubeMap*)pkIm;
                for (unsigned int i = 0; i < NiRenderedCubeMap::FACE_NUM; i++)
                {
                    Ni2DBuffer* pkBuffer = pkRenderedCubeMap->GetFaceBuffer(
                        (NiRenderedCubeMap::FaceID)i);
                    pkBuffer->SetRendererData(NULL);
                }
            }
            else
            {
                NiRenderedTexture* pkRenderedTex = (NiRenderedTexture*)pkIm;
                Ni2DBuffer* pkBuffer = pkRenderedTex->GetBuffer();
                pkBuffer->SetRendererData(NULL);
            }
        }

        pkIm->SetRendererData(NULL);
        NiDelete pkData;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::SetMipmapSkipLevel(unsigned int uiSkip)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    return NiDX9SourceTextureData::SetMipmapSkipLevel(uiSkip);
}
//---------------------------------------------------------------------------
unsigned int NiDX9Renderer::GetMipmapSkipLevel() const
{
    NIASSERT_D3D9_DEVICE_THREAD;

    return NiDX9SourceTextureData::GetMipmapSkipLevel();
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetMaxAnisotropy(unsigned short usMaxAnisotropy)
{
    m_usMaxAnisotropy = static_cast<unsigned short>(
        NiClamp(usMaxAnisotropy, 1, GetHWMaxAnisotropy()));
}
//---------------------------------------------------------------------------
unsigned short NiDX9Renderer::GetHWMaxAnisotropy() const
{
    return m_usHwMaxAnisotropy;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::PrecacheMesh(
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
bool NiDX9Renderer::PrecacheTexture(NiTexture* pkIm)
{
    if (pkIm == NULL)
        return false;

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
bool NiDX9Renderer::PerformPrecache()
{
    // Can only perform precache from device thread.
    if (((m_uiCreationUseFlags & USE_MULTITHREADED) == 0) &&
        m_ulDeviceThreadID != NiGetCurrentThreadId())
    {
        return false;
    }

    NIASSERT_D3D9_DEVICE_THREAD;

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
                NiIsKindOf(NiDX9DataStream, pkDataStream));

            // Can only precache static, GPU-readable streams
            if ((pkDataStream->GetAccessMask() & 
                NiDataStream::ACCESS_CPU_WRITE_STATIC) == 0 ||
                (pkDataStream->GetAccessMask() & 
                NiDataStream::ACCESS_GPU_READ) == 0)
            {
                continue;
            }

            NiDX9DataStream* pkDX9DataStream = (NiDX9DataStream*)pkDataStream;

            pkDX9DataStream->UpdateD3DBuffers(bReleaseSystemMemory);
        }

        pkPrePack->m_spMesh = NULL;
    }
    m_kPrePackMeshes.RemoveAll();

    kIter = m_kPrePackTextures.GetHeadPos();
    while (kIter)
    {
        PrePackTexture* pkPrePack = m_kPrePackTextures.GetNext(kIter);

        NiTexture* pkTexture = pkPrePack->m_spTexture;

        m_pkTextureManager->PrecacheTexture(pkTexture);

        pkPrePack->m_spTexture = NULL;
        pkPrePack->m_spSrcPixelData = NULL;
    }
    m_kPrePackTextures.RemoveAll();
    UnlockPrecacheCriticalSection();

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_BeginFrame()
{
    NIASSERT_D3D9_DEVICE_THREAD;
       
    if (LostDeviceRestore())
    {
        PerformPrecache();
        ReleaseDeferredResources();

        m_spLegacyDefaultShader->NextFrame();
        m_pkLightManager->NextFrame();

        // Reset any states we are tracking frame-by-frame
        // BeginScene
        if (FAILED(m_pkD3DDevice9->BeginScene()))
        {
#ifdef NIDEBUG
            Warning("NiDX9Renderer::Do_BeginFrame> FAILED "
                "- probably alt-tabbed away from app");
#endif
            return false;
        }

        while (m_kBuffersToUseAtDisplayFrame.GetSize())
            m_kBuffersToUseAtDisplayFrame.RemoveHead();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_EndFrame()
{
    NIASSERT_D3D9_DEVICE_THREAD;

//CODEBLOCK(4) - DO NOT DELETE THIS LINE

    if (!m_bDeviceLost)
    {
        if (FAILED(m_pkD3DDevice9->EndScene()))
        {
#ifdef NIDEBUG
            Warning("NiDX9Renderer::Do_EndFrame> FAILED - Lost Device?");
#endif
            // Do not re-open the scene if this call fails
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::Do_DisplayFrame()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!m_bDeviceLost)
    {
        while (m_kBuffersToUseAtDisplayFrame.GetSize())
        {
            NiDX92DBufferDataPtr spBuffer = 
                m_kBuffersToUseAtDisplayFrame.RemoveHead();

            spBuffer->DisplayFrame();
        }
    }

    NIMETRICS_DX9RENDERER_RECORDMETRICS();

    return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_SetCameraData(const NiPoint3& kLoc,
    const NiPoint3& kDir, const NiPoint3& kUp, const NiPoint3& kRight,
    const NiFrustum& kFrustum, const NiRect<float>& kPort)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!m_bDeviceLost)
    {
        // View matrix update
        m_kD3DView._11 = kRight.x;    
        m_kD3DView._12 = kUp.x;    
        m_kD3DView._13 = kDir.x;
        m_kD3DView._14 = 0.0f;
        m_kD3DView._21 = kRight.y; 
        m_kD3DView._22 = kUp.y;    
        m_kD3DView._23 = kDir.y;
        m_kD3DView._24 = 0.0f;
        m_kD3DView._31 = kRight.z; 
        m_kD3DView._32 = kUp.z;    
        m_kD3DView._33 = kDir.z;
        m_kD3DView._34 = 0.0f;
        m_kD3DView._41 = -(kRight * kLoc);
        m_kD3DView._42 = -(kUp * kLoc);
        m_kD3DView._43 = -(kDir * kLoc);
        m_kD3DView._44 = 1.0f;

        m_kInvView._11 = kRight.x;
        m_kInvView._12 = kRight.y;
        m_kInvView._13 = kRight.z;
        m_kInvView._14 = 0.0f;
        m_kInvView._21 = kUp.x;
        m_kInvView._22 = kUp.y;
        m_kInvView._23 = kUp.z;
        m_kInvView._24 = 0.0f;
        m_kInvView._31 = kDir.x;
        m_kInvView._32 = kDir.y;
        m_kInvView._33 = kDir.z;
        m_kInvView._34 = 0.0f;
        m_kInvView._41 = kLoc.x;
        m_kInvView._42 = kLoc.y;
        m_kInvView._43 = kLoc.z;
        m_kInvView._44 = 1.0f;

        m_pkD3DDevice9->SetTransform(D3DTS_VIEW, &m_kD3DView);

        m_kModelCamRight = kRight;
        m_kCamRight = kRight;
        m_kModelCamUp = kUp;
        m_kCamUp = kUp;

        m_fNearDepth = kFrustum.m_fNear;
        m_fDepthRange = kFrustum.m_fFar - kFrustum.m_fNear;

        // Projection matrix update
        float fRmL = kFrustum.m_fRight - kFrustum.m_fLeft;
        float fRpL = kFrustum.m_fRight + kFrustum.m_fLeft;
        float fTmB = kFrustum.m_fTop - kFrustum.m_fBottom;
        float fTpB = kFrustum.m_fTop + kFrustum.m_fBottom;
        float fInvFmN = 1.0f / m_fDepthRange;

        if (kFrustum.m_bOrtho)
        {
            if (m_pkRenderState->GetLeftHanded())
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
            if (m_pkRenderState->GetLeftHanded())
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

        m_pkD3DDevice9->SetTransform(D3DTS_PROJECTION, &m_kD3DProj);

        // Viewport update
        NIASSERT(m_pkCurrRenderTargetGroup);
        float fWidth = (float)m_pkCurrRenderTargetGroup->GetWidth(0);
        float fHeight = (float)m_pkCurrRenderTargetGroup->GetHeight(0);

        D3DVIEWPORT9 kD3DPort;
 
        kD3DPort.X = (unsigned int)(kPort.m_left * fWidth);
        kD3DPort.Y = (unsigned int)((1.0f - kPort.m_top) * fHeight);
        kD3DPort.Width = 
            (unsigned int)((kPort.m_right - kPort.m_left) * fWidth);
        kD3DPort.Height = 
            (unsigned int)((kPort.m_top - kPort.m_bottom) * fHeight);
        kD3DPort.MinZ = 0.0f;     
        kD3DPort.MaxZ = 1.0f;

        m_pkD3DDevice9->SetViewport(&kD3DPort);

        // Fog information to the camera
        NIASSERT(m_pkRenderState);
        m_pkRenderState->SetCameraNearAndFar(kFrustum.m_fNear, 
            kFrustum.m_fFar);

        // Set cached camera data.
        m_kCachedFrustum = kFrustum;
        m_kCachedPort = kPort;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_SetScreenSpaceCameraData(const NiRect<float>* pkPort)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!m_bDeviceLost)
    {
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
        //      Left:   -1/2 + 1/(2*w)
        //      Right:  1/2 + 1/(2*w)
        //      Top:    1/2 - 1/(2*h)
        //      Bottom: -1/2 - 1/(2*h)
        //      Near:   1
        //      Far:    10000

        NIASSERT(m_pkCurrRenderTargetGroup);
        unsigned int uiWidth = m_pkCurrRenderTargetGroup->GetWidth(0);
        unsigned int uiHeight = m_pkCurrRenderTargetGroup->GetHeight(0);
        float fWidth = (float)uiWidth;
        float fHeight = (float)uiHeight;

        // View matrix update
        m_kD3DView._11 = 1.0f;    
        m_kD3DView._12 = 0.0f;    
        m_kD3DView._13 = 0.0f;
        m_kD3DView._14 = 0.0f;
        m_kD3DView._21 = 0.0f; 
        m_kD3DView._22 = -1.0;    
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

        m_pkD3DDevice9->SetTransform(D3DTS_VIEW, &m_kD3DView);

        m_kModelCamRight = NiPoint3(1.0f, 0.0f, 0.0f);
        m_kCamRight = m_kModelCamRight;
        m_kModelCamUp = NiPoint3(0.0f, -1.0f, 0.0f);
        m_kCamUp = m_kModelCamUp;

        m_fNearDepth = 1.0f;
        m_fDepthRange = 9999.0f;
        const float fNearDepthDivDepthRange = 1.0f / 9999.0f;

        // Projection matrix update
        m_kD3DProj._11 = 2.0f;
        m_kD3DProj._21 = 0.0f;
        m_kD3DProj._31 = 0.0f; 
        m_kD3DProj._41 = -1.0f / fWidth;
        m_kD3DProj._12 = 0.0f;
        m_kD3DProj._22 = 2.0f;
        m_kD3DProj._32 = 0.0f; 
        m_kD3DProj._42 = 1.0f / fHeight;
        m_kD3DProj._13 = 0.0f;
        m_kD3DProj._23 = 0.0f;
        m_kD3DProj._33 = fNearDepthDivDepthRange; 
        m_kD3DProj._43 = -fNearDepthDivDepthRange; 
        m_kD3DProj._14 = 0.0f;
        m_kD3DProj._24 = 0.0f;
        m_kD3DProj._34 = 0.0f;
        m_kD3DProj._44 = 1.0f;               

        m_pkD3DDevice9->SetTransform(D3DTS_PROJECTION, &m_kD3DProj);

        // Viewport update
        D3DVIEWPORT9 kD3DPort;

        if (pkPort)
        {
            kD3DPort.X = (unsigned int)(pkPort->m_left * fWidth);
            kD3DPort.Y = (unsigned int)((1.0f - pkPort->m_top) * fHeight);
            kD3DPort.Width = 
                (unsigned int)((pkPort->m_right - pkPort->m_left) * fWidth);
            kD3DPort.Height = 
                (unsigned int)((pkPort->m_top - pkPort->m_bottom) * fHeight);

            // Set cached port.
            m_kCachedPort = *pkPort;
        }
        else
        {
            kD3DPort.X = 0;
            kD3DPort.Y = 0;
            kD3DPort.Width = uiWidth;
            kD3DPort.Height = uiHeight;

            // Set cached port.
            m_kCachedPort = NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f);
        }

        kD3DPort.MinZ = 0.0f;     
        kD3DPort.MaxZ = 1.0f;

        m_pkD3DDevice9->SetViewport(&kD3DPort);

        // Fog information to the camera
        NIASSERT(m_pkRenderState);
        m_pkRenderState->SetCameraNearAndFar(1.0f, 10000.0f);

        // Set cached frustum.
        const float fDoubleWidth = 2.0f * fWidth;
        const float fDoubleHeight = 2.0f * fHeight;
        m_kCachedFrustum.m_fLeft = -0.5f + 1.0f / fDoubleWidth;
        m_kCachedFrustum.m_fRight = 0.5f + 1.0f / fDoubleWidth;
        m_kCachedFrustum.m_fTop = 0.5f - 1.0f / fDoubleHeight;
        m_kCachedFrustum.m_fBottom = -0.5f - 1.0f / fDoubleHeight;
        m_kCachedFrustum.m_fNear = 1.0f;
        m_kCachedFrustum.m_fFar = 10000.0f;
        m_kCachedFrustum.m_bOrtho = true;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_GetCameraData(NiPoint3& kWorldLoc,
    NiPoint3& kWorldDir, NiPoint3& kWorldUp, NiPoint3& kWorldRight,
    NiFrustum& kFrustum, NiRect<float>& kPort)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (!m_bDeviceLost)
    {
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

        kFrustum = m_kCachedFrustum;
        kPort = m_kCachedPort;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetModelTransform(const NiTransform& kXform,
    bool bPushToDevice)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiD3DUtility::GetD3DFromNi(m_kD3DMat, kXform);

    if (bPushToDevice)
        m_pkD3DDevice9->SetTransform(D3DTS_WORLD, &m_kD3DMat);

    m_pkRenderState->SetModelTransform(kXform);

    NiMatrix3 kRotScale = kXform.m_Rotate*kXform.m_fScale;
    m_kModelCamRight = m_kCamRight*kRotScale;
    m_kModelCamUp = m_kCamUp*kRotScale;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::SetSkinnedModelTransforms(NiMesh* pkMesh,
    NiUInt32 uiSubmesh)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiSkinningMeshModifier* pkSkin = 
        NiGetModifier(NiSkinningMeshModifier, pkMesh);
    NIASSERT(pkSkin);
    if (!pkSkin)
        return;

    // Get the per-partition bone matrix palette
    NiDataStreamRef* pkBonePaletteStreamRef =
        pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE());
    NIASSERT(pkBonePaletteStreamRef != NULL);
    if (!pkBonePaletteStreamRef)
        return;

    const NiDataStream::Region& kRegion =
        pkBonePaletteStreamRef->GetRegionForSubmesh(uiSubmesh);
    NiUInt32 uiRegionIdx = 
        pkBonePaletteStreamRef->GetRegionIndexForSubmesh(uiSubmesh);

    NiDataStream* pkBonePaletteStream = 
        pkBonePaletteStreamRef->GetDataStream();

#if defined(NIDEBUG)
    // The palette indices are assumed to be 16 bit unsigned ints,
    // packed in their own stream
    const NiDataStreamElement& kElem =
        pkBonePaletteStream->GetElementDescAt(0);
    NIASSERT(kElem.GetFormat() == NiDataStreamElement::F_UINT16_1);
    NIASSERT(pkBonePaletteStream->GetStride() == sizeof(NiUInt16));
    NIASSERT(kElem.GetOffset() == 0);
#endif
    
    NiUInt32 uiBoneCount = kRegion.GetRange();
    if (uiBoneCount > NiDX9Renderer::HW_BONE_LIMIT)
        return;

    // Get a pointer to the current region's bone palette
    const NiUInt16* puiBonePalette =
        (NiUInt16*)pkBonePaletteStream->LockRegion(uiRegionIdx, 
        NiDataStream::LOCK_READ);
    NIASSERT(puiBonePalette);

    NiMatrix3x4* pkBoneMatrices = pkSkin->GetBoneMatrices();
    NIASSERT(pkBoneMatrices);

    for (unsigned int ui = 0; ui < uiBoneCount; ui++)
    {
        NIASSERT(puiBonePalette[ui] < pkSkin->GetBoneCount());

        NiMatrix3x4& kMat = 
            pkBoneMatrices[puiBonePalette[ui]];

        D3DXMATRIX kD3DMat;
        kD3DMat._11 = kMat.m_kEntry[0][0];
        kD3DMat._21 = kMat.m_kEntry[0][1];
        kD3DMat._31 = kMat.m_kEntry[0][2];
        kD3DMat._41 = kMat.m_kEntry[0][3];
        kD3DMat._12 = kMat.m_kEntry[1][0];
        kD3DMat._22 = kMat.m_kEntry[1][1];
        kD3DMat._32 = kMat.m_kEntry[1][2];
        kD3DMat._42 = kMat.m_kEntry[1][3];
        kD3DMat._13 = kMat.m_kEntry[2][0];
        kD3DMat._23 = kMat.m_kEntry[2][1];
        kD3DMat._33 = kMat.m_kEntry[2][2];
        kD3DMat._43 = kMat.m_kEntry[2][3];
        kD3DMat._14 = 0.0f;
        kD3DMat._24 = 0.0f;
        kD3DMat._34 = 0.0f;
        kD3DMat._44 = 1.0f;

        m_pkD3DDevice9->SetTransform(D3DTS_WORLDMATRIX(ui),
            &kD3DMat);
    }
    pkBonePaletteStream->Unlock(NiDataStream::LOCK_READ);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeEffect(NiDynamicEffect* pkEffect)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiLight* pkLight = NiDynamicCast(NiLight, pkEffect);

    if (pkLight)
        m_pkLightManager->RemoveLight(*pkLight);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeMaterial(NiMaterialProperty* pkMaterial)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    m_pkRenderState->PurgeMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeVertexShader(NiD3DVertexShader* pkShader)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    m_pkRenderState->SetVertexShader(NULL);
    LPDIRECT3DVERTEXSHADER9 pkShaderHandle = pkShader->GetShaderHandle();
    if (pkShaderHandle)
        pkShaderHandle->Release();
    pkShader->SetShaderHandle(NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgePixelShader(NiD3DPixelShader* pkShader)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    m_pkRenderState->SetPixelShader(NULL);
    LPDIRECT3DPIXELSHADER9 pkShaderHandle = pkShader->GetShaderHandle();
    if (pkShaderHandle)
        pkShaderHandle->Release();
    pkShader->SetShaderHandle(NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RegisterD3DShader(NiD3DShaderInterface* pkD3DShader)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    m_kD3DShaders.AddHead(pkD3DShader);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::ReleaseD3DShader(NiD3DShaderInterface* pkD3DShader)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    pkD3DShader->SetD3DRenderer(0);

    m_kD3DShaders.Remove(pkD3DShader);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeD3DShader(NiD3DShaderInterface* pkD3DShader)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    pkD3DShader->DestroyRendererData();
    pkD3DShader->SetD3DRenderer(0);

    m_kD3DShaders.Remove(pkD3DShader);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PurgeAllD3DShaders()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    while (!m_kD3DShaders.IsEmpty())
        PurgeD3DShader(m_kD3DShaders.GetHead());
}
//---------------------------------------------------------------------------
void NiDX9Renderer::PrepareForStreaming(NiObjectNET* pkRoot)
{
    NiShaderTimeController::RemoveBeforeStreaming(pkRoot);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RestoreAfterStreaming()
{
    NiShaderTimeController::AddAfterStreaming();
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::LostDeviceRestore()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    HRESULT eD3dRet = m_pkD3DDevice9->TestCooperativeLevel();
    if (eD3dRet == D3DERR_DEVICENOTRESET)
    {
        bool bSuccess = Recreate();
        return bSuccess;
    }
    else if (eD3dRet == D3DERR_DEVICELOST)
    {
        if (m_bDeviceLost == false)
        {
            m_bDeviceLost = true;

            unsigned int uiFuncCount = m_kLostDeviceNotifyFuncs.GetSize();
            for (unsigned int i = 0; i < uiFuncCount; i++)
            {
                LOSTDEVICENOTIFYFUNC pfnFunc = 
                    m_kLostDeviceNotifyFuncs.GetAt(i);
                void* pvData = m_kLostDeviceNotifyFuncData.GetAt(i);
                if (pfnFunc)
                {
                    bool bResult = (*pfnFunc)(pvData);

                    if (bResult == false)
                    {
                        Error("NiDX9Renderer::LostDeviceRestore> "
                            "Lost device notification function failed\n");
                        return false;
                    }
                }
            }
        }
        return false;
    }
    else
    {
        NIASSERT(SUCCEEDED(eD3dRet));
        return true;
    }
}
//---------------------------------------------------------------------------
unsigned int NiDX9Renderer::ReleaseIUnknown(IUnknown* pkD3DResource, 
    D3DResourceType eType)
{
    if (pkD3DResource == NULL)
        return 0;

    NIASSERT(eType < NiDX9Renderer::D3DRT_COUNT);

    NiDX9Renderer* pkRenderer = NiDX9Renderer::GetRenderer();
    if (pkRenderer && pkRenderer->GetDeviceThreadID() == GetCurrentThreadId())
    {
        return pkD3DResource->Release();
    }
    else
    {
        if (!pkRenderer)
        {
            NILOG("ReleaseIUnknown without a renderer.\n");
        }

        // Add to a queue of resources that will be released
        ms_kDeferredReleaseLock.Lock();
        ms_akDeferredReleaseList[eType].AddTail(pkD3DResource);
        ms_kDeferredReleaseLock.Unlock();

        // Assume that at least one reference must exist at this point, so
        // just return 1 to indicate that the memory has not yet been freed.
        return 1;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::ReleaseDeferredResources()
{
    if (m_ulDeviceThreadID != GetCurrentThreadId())
        return;

    ms_kDeferredReleaseLock.Lock();
    for (unsigned int i = 0; i < D3DRT_COUNT; i++)
    {
        NiTListIterator kIter = ms_akDeferredReleaseList[i].GetHeadPos();
        while (kIter)
        {
            IUnknown* pkD3DResource = 
                ms_akDeferredReleaseList[i].GetNext(kIter);

            // Call the original release function so any necessary tracking is 
            // done. Since we know this is the device thread, these calls
            // will release the resource.
            switch (i)
            {
            case D3DRT_GENERIC:
                ReleaseResource((D3DResourcePtr)pkD3DResource);
                break;
            case D3DRT_VERTEXBUFFER:
                ReleaseVBResource((D3DVertexBufferPtr)pkD3DResource);
                break;
            case D3DRT_INDEXBUFFER:
                ReleaseIBResource((D3DIndexBufferPtr)pkD3DResource);
                break;
            case D3DRT_TEXTURE:
                ReleaseTextureResource((D3DTexturePtr)pkD3DResource);
                break;
            case D3DRT_SWAPCHAIN:
                ReleaseSwapChain((LPDIRECT3DSWAPCHAIN9)pkD3DResource);
                break;
            case D3DRT_DEVICE:
                ReleaseDevice((D3DDevicePtr)pkD3DResource);
                break;
            case D3DRT_VERTEXDECL:
                ReleaseVertexDecl((NiD3DVertexDeclaration)pkD3DResource);
                break;
            default:
                NIASSERT(!"Invalid resource type!");
            }
        }
        ms_akDeferredReleaseList[i].RemoveAll();
    }
    ms_kDeferredReleaseLock.Unlock();
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateSourceTextureRendererData(
    NiSourceTexture* pkTexture)
{
    // Not a function that requires it be on the device thread

    if (pkTexture == NULL)
        return false;

    if (m_ulDeviceThreadID == GetCurrentThreadId())
    {
        // Device thread - load texture immediately
        m_pkTextureManager->PrecacheTexture(pkTexture);
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
bool NiDX9Renderer::CreateRenderedTextureRendererData(
    NiRenderedTexture* pkTexture, Ni2DBuffer::MultiSamplePreference)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiDX9RenderedTextureData* pkData = 
        (NiDX9RenderedTextureData*)pkTexture->GetRendererData();
    if (pkData)
        return true;

    pkData = NiDX9RenderedTextureData::Create(pkTexture, this); 
    if (!pkData)
        return false;

    m_kRenderedTextures.SetAt(pkTexture, pkData);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateSourceCubeMapRendererData(
    NiSourceCubeMap* pkCubeMap)
{
    // Not a function that requires it be on the device thread

    if (pkCubeMap == NULL)
        return false;

    if (m_ulDeviceThreadID == GetCurrentThreadId())
    {
        // Device thread - load texture immediately
        m_pkTextureManager->PrecacheTexture(pkCubeMap);
    }
    else
    {
        // Non-device thread - delay texture loading

        // Load texture file so queries will work - this won't create a D3D
        // resource.
        if (!pkCubeMap->GetLoadDirectToRendererHint())
            pkCubeMap->LoadPixelDataFromFile();

        LockPrecacheCriticalSection();

        PrePackTexture* pkPrePack = m_kPrePackTextures.AddNewTail();
        pkPrePack->m_spTexture = pkCubeMap;
        pkPrePack->m_spSrcPixelData = pkCubeMap->GetSourcePixelData();

        UnlockPrecacheCriticalSection();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateRenderedCubeMapRendererData(
    NiRenderedCubeMap* pkCubeMap)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiDX9RenderedCubeMapData* pkData = 
        (NiDX9RenderedCubeMapData*)pkCubeMap->GetRendererData();
    if (pkData)
        return true;

    pkData = NiDX9RenderedCubeMapData::Create(pkCubeMap, this); 
    if (!pkData)
        return false;

    m_kRenderedCubeMaps.SetAt(pkCubeMap, pkData);

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateDynamicTextureRendererData(
    NiDynamicTexture* pkTexture)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiDX9DynamicTextureData* pkDynTxtrData = NULL;

    NiDX9TextureData* pkTxtrData =
        (NiDX9TextureData*)(pkTexture->GetRendererData());
    if (pkTxtrData)
    {
        pkDynTxtrData = pkTxtrData->GetAsDynamicTexture();
        if (pkDynTxtrData)
            return true;
    }

    pkDynTxtrData = NiDX9DynamicTextureData::Create(pkTexture);
    if (!pkDynTxtrData)
        return false;

    m_kDynamicTextures.SetAt(pkTexture, pkDynTxtrData);

    return true;
}
//---------------------------------------------------------------------------
void* NiDX9Renderer::LockDynamicTexture(
    const NiTexture::RendererData* pkRData, int& iPitch)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    // Discard the entire pixel buffer.  Caller will refill with pixels.
    
    iPitch = 0; // Initialize in case there's an error return.

    NiDX9TextureData* pkTxtrData = (NiDX9TextureData*)(pkRData);
    if (!pkTxtrData)
        return NULL;

    // Call GetAsDynamicTexture() so app won't crash in case this function
    // gets called using a non-dynamic texture, accidentally.
    NiDX9DynamicTextureData* pkDynTxtrData = pkTxtrData->GetAsDynamicTexture();
    if (!pkDynTxtrData)
        return NULL;

    // Sanity check - error return if dynamic texture is already locked.
    if (pkDynTxtrData->IsLocked())
        return NULL;

    D3DTexturePtr pkD3DTexture = NULL;
    DWORD dwLockingFlags = 0;
    if (IsDynamicTexturesCapable())
    {   // Get the video memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetD3DTexture());
        dwLockingFlags = D3DLOCK_DISCARD;
    }
    else
    {   // Get the system memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetSysMemTexture());
    }

    if (!pkD3DTexture)
        return NULL;

    // Lock the bits of the texture, whether video or system memory.
    D3DLOCKED_RECT kLockedRect;
    HRESULT eD3dRet = pkD3DTexture->LockRect(0, &kLockedRect, NULL,
        dwLockingFlags);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning("NiDX9Renderer::LockDynamicTexture Failed> "
            "LockRect - %s", NiDX9ErrorString((unsigned int)eD3dRet));
        return NULL;
    }

    iPitch = kLockedRect.Pitch;
    void* pvBits = (void*)(kLockedRect.pBits);
    pkDynTxtrData->SetLocked(true);

    return pvBits;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::UnLockDynamicTexture(
    const NiTexture::RendererData* pkRData)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NiDX9TextureData* pkTxtrData = (NiDX9TextureData*)(pkRData);
    if (!pkTxtrData)
        return false;

    // Call GetAsDynamicTexture() so app won't crash in case this function
    // gets called using a non-dynamic texture, accidentally.
    NiDX9DynamicTextureData* pkDynTxtrData =
        pkTxtrData->GetAsDynamicTexture();
    if (!pkDynTxtrData)
        return false;

    // Sanity check - error return if dynamic texture is already unlocked.
    if (!pkDynTxtrData->IsLocked())
        return false;

    D3DTexturePtr pkD3DTexture = NULL;
    if (IsDynamicTexturesCapable())
    {   // Get the video memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetD3DTexture());
    }
    else
    {   // Get the system memory texture.
        pkD3DTexture = (D3DTexturePtr)(pkDynTxtrData->GetSysMemTexture());
    }
    if (!pkD3DTexture)
        return false;

    HRESULT eD3dRet = pkD3DTexture->UnlockRect(0);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9Renderer::UnLockDynamicTexture Failed> "
            "UnlockRect - %s", NiDX9ErrorString((unsigned int)eD3dRet));
        return false;
    }

    if (!IsDynamicTexturesCapable())
    {
        // Get the video memory texture.
        D3DTexturePtr pkD3DVidMemTexture =
            (D3DTexturePtr)(pkDynTxtrData->GetD3DTexture());
        if (!pkD3DVidMemTexture)
            return false;

        // Update the video memory texture.
        eD3dRet =
            m_pkD3DDevice9->UpdateTexture(pkD3DTexture, pkD3DVidMemTexture);
        if (FAILED(eD3dRet))
        {
            NiDX9Renderer::Warning(
                "NiDX9Renderer::UnLockDynamicTexture Failed> "
                "UpdateTexture - %s",
                NiDX9ErrorString((unsigned int)eD3dRet));
            return false;
        }
    }

    pkDynTxtrData->SetLocked(false);

    return true;
}
//---------------------------------------------------------------------------
NiShader* NiDX9Renderer::GetFragmentShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiDX9FragmentShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
NiShader* NiDX9Renderer::GetShadowWriteShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    return NiNew NiDX9ShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::CreatePaletteRendererData(NiPalette*)
{
    // Palettes not supported directly due to poor support in DX9 (one pal
    // per device, rather than one per texture stage)
    /* */
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::CreateDepthStencilRendererData(
    NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat, 
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (pkDSBuffer == NULL)
        return false;
    
    NiDX9AdditionalDepthStencilBufferData* pkRendererData = 
        NiDX9AdditionalDepthStencilBufferData::Create(
        m_pkD3DDevice9, pkDSBuffer, pkFormat, eMSAAPref);

    if (pkRendererData == NULL)
        return false;
    else
        return true;
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RemoveRenderedCubeMapData(NiRenderedCubeMap* pkCubeMap)
{
    pkCubeMap->SetRendererData(NULL);
    m_kRenderedCubeMaps.RemoveAt(pkCubeMap);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RemoveRenderedTextureData(NiRenderedTexture* pkTexture)
{
    pkTexture->SetRendererData(NULL);
    m_kRenderedTextures.RemoveAt(pkTexture);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::RemoveDynamicTextureData(NiDynamicTexture* pkTexture)
{
    pkTexture->SetRendererData(NULL);
    m_kDynamicTextures.RemoveAt(pkTexture);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::CreatePixelFormatArray(NiPixelFormat** ppkFormatArray,
    unsigned int, D3DRESOURCETYPE eType)
{
    NIASSERT(m_spDefaultRenderTargetGroup);

    NiPixelFormat* pkPixFmt = NULL;
    for (unsigned int ui = 0; ui < TEX_NUM; ui++)
    {
        switch (ui)
        {
        case TEX_RGB555:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGR555);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGB565:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGR565);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA5551:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRA5551);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA4444:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRA4444);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA8888:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRA8888);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_PAL8:
        case TEX_PALA8:
            {
                // We do not support palettes in HW on DX9, as the DX9 
                // support for them is not good - only one per device,
                // rather than one per texture stage
                pkPixFmt = NULL;
            }
            break;
        case TEX_DXT1:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::DXT1);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_DXT3:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::DXT3);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_DXT5:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::DXT5);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_BUMP88:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BUMP16);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_BUMPLUMA556:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BUMPLUMA556);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_BUMPLUMA888:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BUMPLUMA32);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGB888:
        case TEX_DEFAULT:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::BGRX8888);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_L8:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::I8);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_A8:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::A8);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_R16F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::R16);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RG32F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RG32);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA64F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RGBA64);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_R32F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::R32);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RG64F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RG64);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        case TEX_RGBA128F:
            {
                pkPixFmt = NiNew NiPixelFormat(NiPixelFormat::RGBA128);
                NIASSERT(pkPixFmt);
                pkPixFmt->SetRendererHint(
                    NiDX9PixelFormat::DetermineD3DFormat(*pkPixFmt));
                pkPixFmt->SetExtraData(ui);
            }
            break;
        }

        if (pkPixFmt)
        {
            NiDelete ppkFormatArray[ui];

            if (ms_pkD3D9->CheckDeviceFormat(m_uiAdapter, m_eDevType,
                m_eAdapterFormat, 0, eType, 
                (D3DFORMAT)pkPixFmt->GetRendererHint()))
            {
                NiDelete pkPixFmt;
                pkPixFmt = NULL;
            }

            ppkFormatArray[ui] = pkPixFmt;
        }
    }
}
//---------------------------------------------------------------------------
unsigned char NiDX9Renderer::GetD3DFormatSize(D3DFORMAT eFormat)
{
    unsigned int uiValue = (unsigned int)eFormat;
    if (uiValue >= D3DFORMATSIZECOUNT)
        return 0;
    else
        return ms_aucD3DFormatSizes[uiValue];
}
//---------------------------------------------------------------------------
const NiDX9SystemDesc* NiDX9Renderer::GetSystemDesc()
{
    ms_kD3DCriticalSection.Lock();
    if (ms_pkSystemDesc)
    {
        ms_kD3DCriticalSection.Unlock();
        return ms_pkSystemDesc;
    }

    // Unlock so CreateDirect3D9 can pick up the lock - shouldn't make a
    // difference if the process gets interrupted here.
    ms_kD3DCriticalSection.Unlock();
    CreateDirect3D9();
    ms_kD3DCriticalSection.Lock();

    NIASSERT(ms_pkD3D9);
    ms_pkSystemDesc = NiNew NiDX9SystemDesc(ms_pkD3D9, ms_kDisplayFormats);
    ms_kD3DCriticalSection.Unlock();

    return ms_pkSystemDesc;
}
//---------------------------------------------------------------------------
NiTexturePtr NiDX9Renderer::CreateNiTextureFromD3DTexture(
    D3DBaseTexturePtr pkD3DTexture)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (pkD3DTexture == NULL)
        return NULL;

    NiDX9Direct3DTexturePtr spTexture = NiDX9Direct3DTexture::Create(this);
    NIASSERT(spTexture && "Failed to create NiDX9Direct3DTexture!");

    NiDX9Direct3DTextureData::Create(spTexture, this, pkD3DTexture);

    return NiSmartPointerCast(NiTexture, spTexture);
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::RecreateNiTextureFromD3DTexture(NiTexture* pkTexture,
    D3DBaseTexturePtr pkD3DTexture)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    if (pkTexture == NULL || pkD3DTexture == NULL ||
        !NiIsKindOf(NiDX9Direct3DTexture, pkTexture))
    {
        return false;
    }

    NiDX9Direct3DTextureData* pkData = NiDX9Direct3DTextureData::Create(
        (NiDX9Direct3DTexture*)pkTexture, this, pkD3DTexture);

    return (pkData != NULL);
}
//---------------------------------------------------------------------------
void NiDX9Renderer::UseLegacyPipelineAsDefaultMaterial()
{
    NIASSERT_D3D9_DEVICE_THREAD;

    m_spCurrentDefaultMaterial = m_spLegacyDefaultShaderMaterial;
}
//---------------------------------------------------------------------------
bool NiDX9Renderer::GetDX9PrimitiveFromNiMeshPrimitiveType(
    NiPrimitiveType::Type eMeshPrimType,
    D3DPRIMITIVETYPE& eOutputD3DPrimitiveType)
{
    eOutputD3DPrimitiveType = (D3DPRIMITIVETYPE)0;
    switch(eMeshPrimType)
    {
    case NiPrimitiveType::PRIMITIVE_TRIANGLES: 
        eOutputD3DPrimitiveType = D3DPT_TRIANGLELIST;
        return true;
    case NiPrimitiveType::PRIMITIVE_TRISTRIPS: 
        eOutputD3DPrimitiveType = D3DPT_TRIANGLESTRIP;
        return true;
    case NiPrimitiveType::PRIMITIVE_LINES: 
        eOutputD3DPrimitiveType = D3DPT_LINELIST;
        return true;
    case NiPrimitiveType::PRIMITIVE_LINESTRIPS: 
        eOutputD3DPrimitiveType = D3DPT_LINESTRIP;
        return true;
    case NiPrimitiveType::PRIMITIVE_QUADS: 
        NIASSERT(0 && "PRIMITIVE_QUADS not supported by DX9");
        return false;
    case NiPrimitiveType::PRIMITIVE_POINTS: 
        eOutputD3DPrimitiveType = D3DPT_POINTLIST;
        return true;
    default:
        NIASSERT(0 && "Unknown primitive type");
        return false;
    }
}
//---------------------------------------------------------------------------
void NiDX9Renderer::Do_RenderMesh(NiMesh* pkMesh)
{
    NIASSERT_D3D9_DEVICE_THREAD;

    NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_MESH);
    NIASSERT(LockCount());

    if (m_bDeviceLost)
        return;

    NiTransform kWorld = pkMesh->GetWorldTransform();
    NiBound kWorldBound = pkMesh->GetWorldBound();

    NiDX9MeshMaterialBindingPtr spVertexDeclaration;
    NiD3DShaderInterface* pkShader = GetShaderAndVertexDecl(pkMesh, 
        spVertexDeclaration);
    NIASSERT(pkShader);
    NIASSERT(m_pkCurrProp);

    if (spVertexDeclaration == NULL || 
        spVertexDeclaration->GetD3DDeclaration() == NULL)
    {
        // Can't render without a valid vertex declaration
        return;
    }
    
    NiRenderCallContext kRCC;
    kRCC.m_pkMesh = pkMesh;
    kRCC.m_pkWorldBound = &kWorldBound;
    kRCC.m_pkEffects = m_pkCurrEffects;
    kRCC.m_pkState = m_pkCurrProp;
    kRCC.m_pkWorld = &kWorld;
    kRCC.m_pkMeshMaterialBinding = 
        NiSmartPointerCast(NiVertexDeclarationCache, spVertexDeclaration); 

    // Preprocess the pipeline
    unsigned int uiRet = pkShader->PreProcessPipeline(kRCC);

    if (uiRet != 0)
    {
        NIASSERT(!"PreProcess failed - skipping render");
        return;
    }

    // Update the pipeline
    uiRet = pkShader->UpdatePipeline(kRCC);

    // Convert primitive type to geometry type
    D3DPRIMITIVETYPE eGeomType;
    if (!GetDX9PrimitiveFromNiMeshPrimitiveType(
        pkMesh->GetPrimitiveType(),
        eGeomType))
    {
        return;
    }
    NIASSERT(eGeomType);

    // Get the first vertex buffer
    if (spVertexDeclaration->m_kStreamsToSet.GetSize() == 0)
    {
        NiDX9Renderer::Warning(
            "NiDX9Renderer::Do_RenderMesh> "
            "No vertex streams found to render with on"
            "'%s' object, pointer: %x, skipping render.",
            pkMesh->GetName(),
            pkMesh);
        return;
    }

    bool bIndexed = spVertexDeclaration->m_pkIndexStreamRef != NULL;

    const NiDataStreamRef* pkStreamRef = NULL;
    if (bIndexed)
    {
        pkStreamRef = spVertexDeclaration->m_pkIndexStreamRef;
    }
    else
    {
        pkStreamRef = pkMesh->GetStreamRefAt(
            spVertexDeclaration->m_kStreamsToSet.GetAt(0));
    }

    // Set vertex declaration on device
    m_pkD3DDevice9->SetVertexDeclaration(
        spVertexDeclaration->GetD3DDeclaration());

    unsigned int uiMetricDPCalls = 0;

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

        // Set the transformations
        uiRet = pkShader->SetupTransformations(kRCC);

        // Iterate over submesh regions
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshCount; ++uiSubmesh)
        {
            kRCC.m_uiSubmesh = uiSubmesh;

            NiDataStream::Region kRegion = pkStreamRef->GetRegionForSubmesh(
                uiSubmesh);
            unsigned int uiElementCount = pkStreamRef->GetCount(uiSubmesh);
            unsigned int uiPrimitiveCount = 
                pkMesh->GetPrimitiveCountFromElementCount(uiElementCount);

            if (uiPrimitiveCount == 0)
            {
                NiDX9Renderer::Warning(
                    "NiDX9Renderer::Do_RenderMesh> "
                    "Zero primitive count in submesh %d of NiRenderObject "
                    "'%s' pointer: %x, skipping render.",
                    uiSubmesh,
                    pkMesh->GetName(),
                    pkMesh);
            }
            else
            {
                // Set the shader programs
                // This is to give the shader final 'override' authority
                uiRet = pkShader->SetupShaderPrograms(kRCC);

                m_pkRenderState->CommitShaderConstants();

                // Setup the current submesh
                pkShader->PreRenderSubmesh(kRCC);

                NiUInt32 uiNumVertices;
                if (bIndexed)
                {
                    uiNumVertices = pkMesh->GetVertexCount(uiSubmesh);

                    m_pkD3DDevice9->DrawIndexedPrimitive(
                        eGeomType, // Type 
                        0, // BaseVertexIndex
                        0, // MinIndex
                        uiNumVertices, // NumVertices 
                        kRegion.GetStartIndex(), // StartIndex 
                        uiPrimitiveCount); // PrimitiveCount 
                }
                else
                {
                    uiNumVertices = kRegion.GetRange();

                    m_pkD3DDevice9->DrawPrimitive(
                        eGeomType, // Type
                        0, // StartIndex (offset applied at SetStreamSource)
                        uiPrimitiveCount); // PrimitiveCount 
                }

                uiMetricDPCalls++;
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVES, 
                    uiPrimitiveCount);
                NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
                    uiNumVertices);

                pkShader->PostRenderSubmesh(kRCC);
            }
        }

        // Perform any post-rendering steps
        uiRet = pkShader->PostRender(kRCC);

        // Inform the shader to move to the next pass
        uiRemainingPasses = pkShader->NextPass();        
        kRCC.m_uiPass++;
    }

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_SUBMESH_COUNT, uiMetricDPCalls);

    // PostProcess the pipeline
    pkShader->PostProcessPipeline(kRCC);
}
//---------------------------------------------------------------------------
NiD3DShaderInterface* NiDX9Renderer::GetShaderAndVertexDecl(
    NiRenderObject* pkMesh, NiDX9MeshMaterialBindingPtr& spVertexDecl)
{
    NiD3DShaderInterface* pkShader = 
        GetShaderAndVertexDecl_NoErrorShader(pkMesh, spVertexDecl);

    // If no shader is found, use error shader
    if (pkShader == NULL)
    {
        Error("No shader found for object \"%s\" pointer: %x\n"
            "Using Error Shader!\n", pkMesh->GetName(), pkMesh);
        NIASSERT(NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
        pkShader = (NiD3DShaderInterface*)GetErrorShader();

        spVertexDecl = NiDX9MeshMaterialBinding::Create(
            NiVerifyStaticCast(NiMesh, pkMesh), 
            pkShader->GetSemanticAdapterTable(),
            false);
    }

    NIASSERT(pkShader);
    return pkShader;
}
//---------------------------------------------------------------------------
NiD3DShaderInterface* NiDX9Renderer::GetShaderAndVertexDecl_NoErrorShader(
    NiRenderObject* pkMesh, NiDX9MeshMaterialBindingPtr& spVertexDecl)
{
    NIASSERT(pkMesh);

    bool bNoActiveMaterial_UseDefault = pkMesh->GetActiveMaterial() == NULL;
    if (bNoActiveMaterial_UseDefault)
        pkMesh->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);

    NiD3DShaderInterface* pkShader = NiVerifyStaticCast(NiD3DShaderInterface, 
        pkMesh->GetShaderFromMaterial());

    // Grab vertex declaration cache
    const NiMaterialInstance* pkMatInst = pkMesh->GetActiveMaterialInstance();
    spVertexDecl =
        (NiDX9MeshMaterialBinding*)pkMatInst->GetVertexDeclarationCache();

    // Restore original material if necessary
    if (bNoActiveMaterial_UseDefault)
        pkMesh->SetActiveMaterial(NULL);

    return pkShader;
}
//---------------------------------------------------------------------------
const D3DXMACRO* NiDX9Renderer::GetD3DXMacroList(
    const char* pcFileType,
    const D3DXMACRO* pkUserMacros)
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
    if (!m_pkD3DXMacroBuffer || m_uiD3DXMacroBufferLen <
        (kAllMacros.GetCount() + 1) * sizeof(D3DXMACRO))
    {
        if (m_pkD3DXMacroBuffer)
            NiFree(m_pkD3DXMacroBuffer);

        // Allocate buffer, that is 20% larger, than required to avoid
        // reallocations when few macros are added or changed
        m_uiD3DXMacroBufferLen =
            (NiUInt32)((kAllMacros.GetCount() + 1) * 1.2f);
        m_pkD3DXMacroBuffer = NiAlloc(D3DXMACRO, m_uiD3DXMacroBufferLen);
        m_uiD3DXMacroBufferLen *= sizeof(D3DXMACRO);
    }

    NiTMapIterator kIter = kAllMacros.GetFirstPos();
    NiUInt32 uiIdx = 0;
    while (kIter)
    {
        NiFixedString kName;
        NiFixedString kValue;
        kAllMacros.GetNext(kIter, kName, kValue);

        (m_pkD3DXMacroBuffer + uiIdx)->Name = kName;
        (m_pkD3DXMacroBuffer + uiIdx)->Definition = kValue;
        uiIdx++;
    }
    // NULL-terminate a list
    (m_pkD3DXMacroBuffer + uiIdx)->Name = NULL;
    (m_pkD3DXMacroBuffer + uiIdx)->Definition = NULL;

    return m_pkD3DXMacroBuffer;
}
//---------------------------------------------------------------------------
const char** NiDX9Renderer::GetCGMacroList(
    const char* pcFileType,
    const char** ppcUserParams)
{
    if (!GetGlobalMacroCount() && !GetMacroCount(pcFileType))
    {
        // Exit if there are no global or file type-specific macros defined
        return ppcUserParams;
    }

    ShaderData kUserMacro;
    NiTPointerList<const char*> kUserParams;
    NiTFixedStringMap<NiFixedString> kAllMacros;

    // Fill kUserData array with user-supplied macro definitions
    // This will speed up searching for duplicate macro names,
    // because NiTFixedStringMap uses hash and only compares pointers.
    if (ppcUserParams)
    {
        while (*ppcUserParams)
        {
            // Here we will iterate through user-supplied list of parameters
            // and extract macro definitions to kUserData array. All other
            // compiler options will be added to kUserParams array.
            NiString kParam(*ppcUserParams);

            kParam.TrimLeft(' ');
            NiUInt32 uiValPos = kParam.Find('=');

            // If string is in form "-D<name>=<value>", extract macro
            if (kParam[0] == '-' && kParam[1] == 'D' &&
                uiValPos != NiString::INVALID_INDEX && uiValPos > 2)
            {
                NiString kName(kParam.GetSubstring(2, uiValPos));
                NiString kValue(
                    kParam.GetSubstring(uiValPos + 1, kParam.Length()));

                // Delete leading / trailing spaces and quotes
                kName.TrimLeft(' ');
                kName.TrimLeft('\"');
                kName.TrimRight(' ');
                kName.TrimRight('\"');
                kValue.TrimLeft(' ');
                kValue.TrimLeft('\"');
                kValue.TrimRight(' ');
                kValue.TrimRight('\"');

                kUserMacro.AddMacro(NiFixedString(kName),
                    NiFixedString(kValue));
            }
            else
            {
                // Looks like this string is not a macro definition
                kUserParams.AddTail(*ppcUserParams);
            }

            ppcUserParams++;
        }
    }

    // Search for duplicate macro names and merge all macro containing structs
    BuildMacroList(pcFileType, &kUserMacro, kAllMacros);

    // Calculate the buffer size and number of pointers
    NiUInt32 uiBufSize = 0;
    NiUInt32 uiPtrCount = 0;
    NiTMapIterator kMIter = kAllMacros.GetFirstPos();
    while(kMIter)
    {
        NiFixedString kName;
        NiFixedString kValue;
        kAllMacros.GetNext(kMIter, kName, kValue);
        // Length of "-D"<n>"="<v>"\0" is name length + value length + 8 chars
        uiBufSize = 
            (NiUInt32)(uiBufSize + kName.GetLength() + kValue.GetLength() + 8);
        uiPtrCount++;
    }
    NiTListIterator kLIter = kUserParams.GetHeadPos();
    while (kLIter)
    {
        uiBufSize =
            (NiUInt32)(uiBufSize + strlen(kUserParams.GetNext(kLIter)) + 1);
        uiPtrCount++;
    }
    uiPtrCount++;

    // If buffer is not created or its size is too small, [re]allocate it
    if (!m_pcCGMacroBuffer || m_uiCGMacroBufferLen < uiBufSize)
    {
        if (m_pcCGMacroBuffer)
            NiFree(m_pcCGMacroBuffer);

        // Allocate buffer, that is 20% larger, than required to avoid
        // reallocations when few macros are added or changed
        m_uiCGMacroBufferLen = (NiUInt32)(uiBufSize * 1.2f);
        m_pcCGMacroBuffer = NiAlloc(char, m_uiCGMacroBufferLen);
    }
    if (!m_ppcCGMacroPtrBuffer || m_uiCGMacroPtrBufferLen <
        uiPtrCount * sizeof(char*))
    {
        if (m_ppcCGMacroPtrBuffer)
            NiFree(m_ppcCGMacroPtrBuffer);

        // Allocate buffer, that is 20% larger, than required to avoid
        // reallocations when few macros are added or changed
        m_uiCGMacroPtrBufferLen =
            (NiUInt32)(uiPtrCount * sizeof(char*) * 1.2f);
        m_ppcCGMacroPtrBuffer = NiAlloc(const char*, m_uiCGMacroPtrBufferLen);
    }

    // Build macro definition options list
    char* pcBuffer = m_pcCGMacroBuffer;
    const char** ppcPtr = m_ppcCGMacroPtrBuffer;
    kMIter = kAllMacros.GetFirstPos();
    while(kMIter)
    {
        NiFixedString kName;
        NiFixedString kValue;

        // Get name and value of macro definition
        kAllMacros.GetNext(kMIter, kName, kValue);

        // Store pointer to new compiler parameter string
        *ppcPtr = pcBuffer;

        // Fill buffer with compiler parameter string, increment pointers
        NiUInt32 uiSizeLeft = m_uiCGMacroBufferLen -
            (pcBuffer - m_pcCGMacroBuffer);

        // If macro value is empty, just define it with -D<macro>
        if (kValue.GetLength())
        {
            NiSprintf(pcBuffer, uiSizeLeft, "-D\"%s\"=\"%s\"", 
                (const char*)kName, (const char*)kValue);
            pcBuffer += kName.GetLength() + kValue.GetLength() + 8;
        }
        else
        {
            NiSprintf(pcBuffer, uiSizeLeft, "-D\"%s\"", (const char*)kName);
            pcBuffer += kName.GetLength() + 5;
        }
        ppcPtr++;
    }

    // Append user-supplied compiler options list
    kLIter = kUserParams.GetHeadPos();
    while (kLIter)
    {
        // Get string and store it in buffer
        NiFixedString kStr = kUserParams.GetNext(kLIter);
        NiUInt32 uiSizeLeft = m_uiCGMacroBufferLen -
            (pcBuffer - m_pcCGMacroBuffer);
        NiStrcpy(pcBuffer, uiSizeLeft, kStr);

        // Store pointer to new compiler parameter string, increment pointers
        *ppcPtr = pcBuffer;
        pcBuffer += kStr.GetLength() + 1;
        ppcPtr++;
    }

    // NULL-terminate pointer list
    *ppcPtr = 0;

    // Return pointer to array of pointers to compiler options strings
    return m_ppcCGMacroPtrBuffer;
}
//---------------------------------------------------------------------------
