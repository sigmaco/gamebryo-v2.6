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

#ifndef NIDX9RENDERER_H
#define NIDX9RENDERER_H

#include "NiDX9Headers.h"
#include "NiDX9RendererLibType.h"

#include "NiDX9Error.h"
#include "NiDX9LightManager.h"
#include "NiDX9PixelFormat.h"
#include "NiDX9RendererSDM.h"
#include "NiDX9RenderState.h"
#include "NiDX92DBufferData.h"
#include "NiDX9DataStream.h"
#include "NiDX9ShadowWriteShader.h"
#include "NiDX9MeshMaterialBinding.h"
#include "NiD3DDefaultShader.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DShaderLibraryVersion.h"
#include "NiD3DUtils.h"

#include <NiUniversalTypes.h>
#include <NiAccumulator.h>
#include <NiBool.h>
#include <NiColor.h>
#include <NiDataStream.h>
#include <NiDataStreamFactory.h>
#include <NiTexture.h>
#include <NiDynamicEffect.h>
#include <NiDynamicEffectState.h>
#include <NiMesh.h>
#include <NiPropertyState.h>
#include <NiRect.h>
#include <NiRenderer.h>
#include <NiRenderTargetGroup.h>
#include <NiShaderDeclaration.h>
#include <NiTObjectList.h>
#include <NiTPointerList.h>

class NiAVObject;
class NiBound;
class NiPoint3;
class NiPoint2;
class NiPlane;
class NiCamera;
class NiFrustum;
class NiTriShapeData;
class NiTriStripsData;
class NiParticlesData;
class NiLinesData;
class NiTexture;
class NiSkinningMeshModifier;
NiSmartPointer(NiPixelData);
NiSmartPointer(NiDX92DBufferData);
NiSmartPointer(NiPersistentSrcTextureRendererData);

//  DX9 Specific classes
class NiDX9AdapterDesc;
class NiDX9DeviceDesc;
class NiDX9SystemDesc;
class NiDX9TextureManager;
class NiDX9TextureData;
class NiDX9SourceTextureData;
class NiDX9RenderedTextureData;
class NiDX9RenderedCubeMapData;
class NiDX9DynamicTextureData;
class NiDX9PersistentSrcTextureRendererData;

// DX9 ID Number (Must be unique to all renderers).
enum { NI_DX9_RENDER_ID = 8 };

typedef LPDIRECT3D9 (CALLBACK *NILPD3DCREATE) (unsigned int);

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4324)
#endif

class NIDX9RENDERER_ENTRY NiDX9Renderer : public NiRenderer
{
    NiDeclareRTTI;

public:
    // Flags for creation options.
    enum FlagType
    {
        USE_NOFLAGS                      = 0x00000000,
        USE_MANUALDEPTHSTENCIL           = 0x00000001,
        USE_FULLSCREEN                   = 0x00000004,
        USE_STENCIL                      = 0x00000008,
        USE_16BITBUFFERS                 = 0x00000010,
        USE_MULTITHREADED                = 0x00000020,
        USE_FPU_PRESERVE                 = 0x00000040
    };

    // Device Description
    enum DeviceDesc
    {
        DEVDESC_PURE,
        DEVDESC_HAL_HWVERTEX,
        DEVDESC_HAL_MIXEDVERTEX,
        DEVDESC_HAL_SWVERTEX,
        DEVDESC_REF,
        DEVDESC_REF_HWVERTEX,
        DEVDESC_REF_MIXEDVERTEX,
        DEVDESC_NUM
    };

    //  DX9 supported frame buffer formats
    enum FrameBufferFormat
    {
        FBFMT_UNKNOWN = 0,
        FBFMT_R8G8B8,
        FBFMT_A8R8G8B8,
        FBFMT_X8R8G8B8,
        FBFMT_R5G6B5,
        FBFMT_X1R5G5B5,
        FBFMT_A1R5G5B5,
        FBFMT_A4R4G4B4,
        FBFMT_R3G3B2,
        FBFMT_A8,
        FBFMT_A8R3G3B2,
        FBFMT_X4R4G4B4,
        FBFMT_R16F,
        FBFMT_G16R16F,
        FBFMT_A16B16G16R16F,
        FBFMT_R32F,
        FBFMT_G32R32F,
        FBFMT_A32B32G32R32F,
        FBFMT_NUM
    };

    //  DX9 supported depth/stencil buffer formats
    enum DepthStencilFormat
    {
        DSFMT_UNKNOWN      =   0,
        DSFMT_D16_LOCKABLE =  70,
        DSFMT_D32          =  71,
        DSFMT_D15S1        =  73,
        DSFMT_D24S8        =  75,
        DSFMT_D16          =  80,
        DSFMT_D24X8        =  77,
        DSFMT_D24X4S4      =  79,
    };

    // Presentation Interval
    enum PresentationInterval
    {
        PRESENT_INTERVAL_IMMEDIATE = 0,
        PRESENT_INTERVAL_ONE = 1,
        PRESENT_INTERVAL_TWO = 2,
        PRESENT_INTERVAL_THREE = 3,
        PRESENT_INTERVAL_FOUR = 4,
        PRESENT_INTERVAL_NUM
    };

    // Swap Effect
    enum SwapEffect
    {
        SWAPEFFECT_DEFAULT,
        SWAPEFFECT_DISCARD,
        SWAPEFFECT_FLIP,
        SWAPEFFECT_COPY,
        SWAPEFFECT_NUM
    };

    enum FramebufferMode
    {
        FBMODE_DEFAULT,
        FBMODE_LOCKABLE,
        FBMODE_MULTISAMPLES_2           = 0x00010000,
        FBMODE_MULTISAMPLES_3           = 0x00020000,
        FBMODE_MULTISAMPLES_4           = 0x00030000,
        FBMODE_MULTISAMPLES_5           = 0x00040000,
        FBMODE_MULTISAMPLES_6           = 0x00050000,
        FBMODE_MULTISAMPLES_7           = 0x00060000,
        FBMODE_MULTISAMPLES_8           = 0x00070000,
        FBMODE_MULTISAMPLES_9           = 0x00080000,
        FBMODE_MULTISAMPLES_10          = 0x00090000,
        FBMODE_MULTISAMPLES_11          = 0x000a0000,
        FBMODE_MULTISAMPLES_12          = 0x000b0000,
        FBMODE_MULTISAMPLES_13          = 0x000c0000,
        FBMODE_MULTISAMPLES_14          = 0x000d0000,
        FBMODE_MULTISAMPLES_15          = 0x000e0000,
        FBMODE_MULTISAMPLES_16          = 0x000f0000,
        FBMODE_MULTISAMPLES_NONMASKABLE = 0x80000000,
        FBMODE_QUALITY_MASK             = 0x0000FFFF,
        FBMODE_NUM = 18
    };

    // Refresh Rate
    enum RefreshRate
    {
        REFRESHRATE_DEFAULT   = 0
    };

    enum TexFormat
    {
        TEX_RGB555,
        TEX_RGB565,
        TEX_RGB888,
        TEX_RGBA5551,
        TEX_RGBA4444,
        TEX_RGBA8888,
        TEX_PAL8,
        TEX_PALA8,
        TEX_DXT1,
        TEX_DXT3,
        TEX_DXT5,
        TEX_BUMP88,
        TEX_BUMPLUMA556,
        TEX_BUMPLUMA888,
        TEX_L8,
        TEX_A8,
        TEX_R16F,
        TEX_RG32F,
        TEX_RGBA64F,
        TEX_R32F,
        TEX_RG64F,
        TEX_RGBA128F,
        TEX_DEFAULT,
        TEX_NUM         = TEX_DEFAULT
    };

    enum TexUsage
    {
        TEXUSE_TEX,
        TEXUSE_RENDERED_TEX,
        TEXUSE_CUBE,
        TEXUSE_RENDERED_CUBE,
        TEXUSE_NUM
    };

    enum FramebufferStatus
    {
        FBSTATUS_OK,
        FBSTATUS_NOT_FOUND,
        FBSTATUS_NEEDS_RESET
    };

    enum RecreateStatus
    {
        RECREATESTATUS_FAILED,
        RECREATESTATUS_RESTORED,
        RECREATESTATUS_OK,
    };

    enum
    {
        HW_BONE_LIMIT = 4
    };

    virtual ~NiDX9Renderer();

    // Renderer creation
    static NiDX9Renderer* Create(unsigned int uiWidth, unsigned int uiHeight,
        unsigned int uiUseFlags, NiWindowRef kWndDevice, 
        NiWindowRef kWndFocus, unsigned int uiAdapter = D3DADAPTER_DEFAULT, 
        DeviceDesc eDesc = DEVDESC_PURE,
        FrameBufferFormat eFBFormat = FBFMT_UNKNOWN, 
        DepthStencilFormat eDSFormat = DSFMT_UNKNOWN, 
        PresentationInterval ePresentationInterval = PRESENT_INTERVAL_ONE,
        SwapEffect eSwapEffect = SWAPEFFECT_DEFAULT,
        unsigned int uiFBMode = FBMODE_DEFAULT,
        unsigned int uiBackBufferCount = 1, 
        unsigned int uiRefreshRate = REFRESHRATE_DEFAULT);

    void GetCreationParameters(unsigned int& uiWidth, unsigned int& uiHeight,
        unsigned int& uiUseFlags, NiWindowRef& kWndDevice, 
        NiWindowRef& kWndFocus, unsigned int& uiAdapter, DeviceDesc& eDesc, 
        FrameBufferFormat& eFBFormat, DepthStencilFormat& eDSFormat, 
        PresentationInterval& ePresentationInterval, SwapEffect& eSwapEffect,
        unsigned int& uiFBMode, unsigned int& uiBackBufferCount, 
        unsigned int& uiRefreshRate) const;

    // Renderer recreation
    RecreateStatus Recreate(unsigned int uiWidth, unsigned int uiHeight,
        unsigned int uiUseFlags, NiWindowRef kWndDevice = 0, 
        FrameBufferFormat eFBFormat = FBFMT_UNKNOWN, 
        DepthStencilFormat eDSFormat = DSFMT_UNKNOWN, 
        PresentationInterval ePresentationInterval = PRESENT_INTERVAL_ONE,
        SwapEffect eSwapEffect = SWAPEFFECT_DEFAULT,
        unsigned int uiFBMode = FBMODE_DEFAULT,
        unsigned int uiBackBufferCount = 1, 
        unsigned int uiRefreshRate = REFRESHRATE_DEFAULT);

    RecreateStatus Recreate(unsigned int uiWidth, unsigned int uiHeight);

    bool Recreate();

    // Respond to D3D device reset
    inline unsigned int GetResetCounter() const;
    inline bool IsDeviceLost() const;

    // Callbacks for improved response to D3D device reset
    typedef bool (*RESETNOTIFYFUNC)(bool bBeforeReset, void* pvData);

    inline unsigned int AddResetNotificationFunc(RESETNOTIFYFUNC pfnNotify,
        void* pvData);
    inline bool RemoveResetNotificationFunc(RESETNOTIFYFUNC pfnNotify);
    inline bool RemoveResetNotificationFunc(unsigned int uiIndex);
    inline void RemoveAllResetNotificationFuncs();

    inline bool ChangeResetNotificationFuncData(RESETNOTIFYFUNC pfnNotify,
        void* pvData);
    inline bool ChangeResetNotificationFuncData(unsigned int uiIndex,
        void* pvData);

    inline unsigned int GetResetNotificationFuncCount() const;
    inline unsigned int GetResetNotificationFuncArrayCount() const;
    inline unsigned int FindResetNotificationFunc(RESETNOTIFYFUNC pfnNotify)
        const;
    RESETNOTIFYFUNC GetResetNotificationFunc(unsigned int uiIndex) const;

    typedef bool (*LOSTDEVICENOTIFYFUNC)(void* pvData);

    inline unsigned int AddLostDeviceNotificationFunc(
        LOSTDEVICENOTIFYFUNC pfnNotify,
        void* pvData);
    inline bool RemoveLostDeviceNotificationFunc(
        LOSTDEVICENOTIFYFUNC pfnNotify);
    inline bool RemoveLostDeviceNotificationFunc(unsigned int uiIndex);
    inline void RemoveAllLostDeviceNotificationFuncs();

    inline bool ChangeLostDeviceNotificationFuncData(
        LOSTDEVICENOTIFYFUNC pfnNotify,
        void* pvData);
    inline bool ChangeLostDeviceNotificationFuncData(unsigned int uiIndex,
        void* pvData);

    inline unsigned int GetLostDeviceNotificationFuncCount() const;
    inline unsigned int GetLostDeviceNotificationFuncArrayCount() const;
    inline unsigned int FindLostDeviceNotificationFunc(
        LOSTDEVICENOTIFYFUNC pfnNotify) const;
    LOSTDEVICENOTIFYFUNC GetLostDeviceNotificationFunc(unsigned int uiIndex)
        const;

    // Swap Chain RenderTarget management
    bool CreateSwapChainRenderTargetGroup(unsigned int uiUseFlags, 
        NiWindowRef kWndDevice, 
        FrameBufferFormat eFBFormat = FBFMT_UNKNOWN, 
        DepthStencilFormat eDSFormat = DSFMT_UNKNOWN, 
        PresentationInterval ePresentationInterval = PRESENT_INTERVAL_ONE,
        SwapEffect eSwapEffect = SWAPEFFECT_DEFAULT,
        unsigned int uiFBMode = FBMODE_DEFAULT,
        unsigned int uiBackBufferCount = 1, 
        unsigned int uiRefreshRate = REFRESHRATE_DEFAULT);
    void DestroySwapChainRenderTargetGroup(NiWindowRef kWndDevice);
    NiRenderTargetGroup* GetSwapChainRenderTargetGroup(
        NiWindowRef kWndDevice) 
        const;

    // Optimize decal maps in the default shader
    inline void UseQuickDecals(bool bQuickDecals);

    // Wrapping an NiTexture around an IDirect3DBaseTexture9
    NiTexturePtr CreateNiTextureFromD3DTexture(
        D3DBaseTexturePtr pkD3DTexture);
    bool RecreateNiTextureFromD3DTexture(NiTexture* pkTexture,
        D3DBaseTexturePtr pkD3DTexture);

    // Display formats to query upon renderer creation
    static void AddDisplayFormat(D3DFORMAT eFormat);
    static void RemoveDisplayFormat(D3DFORMAT eFormat);

    // DX9-specific pixel format query
    const NiPixelFormat* FindClosestPixelFormat(
        TexFormat eFormat, TexUsage eUsage) const;

    // Handle resources for vertex and pixel shader programs
    void PurgeVertexShader(NiD3DVertexShader* pkShader);
    void PurgePixelShader(NiD3DPixelShader* pkShader);

    // Handle resources for Gamebryo shaders
    void RegisterD3DShader(NiD3DShaderInterface* pkD3DShader);
    void ReleaseD3DShader(NiD3DShaderInterface* pkD3DShader);
    void PurgeD3DShader(NiD3DShaderInterface* pkD3DShader);

    void PurgeAllD3DShaders();

    // Access D3D interfaces
    inline LPDIRECT3DDEVICE9 GetD3DDevice() const;
    inline unsigned int GetAdapter() const;
    inline D3DDEVTYPE GetDevType() const;
    inline D3DPRESENT_PARAMETERS* GetPresentParams(
        const NiRenderTargetGroup* pkTarget = NULL) const;
    inline D3DFORMAT GetAdapterFormat() const;
    inline const D3DCAPS9* GetDeviceCaps() const;

    static LPDIRECT3D9 GetDirect3D();

    // Access Gamebryo representations of the device
    inline const NiDX9AdapterDesc* GetAdapterDesc() const;
    inline const NiDX9DeviceDesc* GetDeviceDesc() const;

    static const NiDX9SystemDesc* GetSystemDesc();

    // Access DX9 renderer manager classes
    inline NiDX9RenderState* GetRenderState() const;
    inline NiDX9TextureManager* GetTextureManager() const;
    inline NiDX9LightManager* GetLightManager() const;

    inline NiD3DShader* GetLegacyDefaultShader() const;

    // Query D3D device capabilities
    inline unsigned int GetMaxStreams() const;
    inline unsigned int GetPixelShaderVersion() const;
    inline unsigned int GetVertexShaderVersion() const;
    inline bool GetMipmapCubeMaps() const;
    inline bool IsDynamicTexturesCapable() const;
    bool GetMRTPostPixelShaderBlendingCapability() const;
    bool GetFormatPostPixelShaderBlendingCapability(D3DFORMAT eFormat) const;

    // Remove renderer-specific data from a scene graph to allow it
    // to safely stream
    static void PrepareForStreaming(NiObjectNET* pkRoot);
    static void RestoreAfterStreaming();

    // Translate between D3D and Gamebryo values
    static unsigned char GetD3DFormatSize(D3DFORMAT eFormat);
    static D3DFORMAT GetD3DFormat(FrameBufferFormat eFBFormat);
    static D3DFORMAT GetCompatibleAdapterFormat(FrameBufferFormat eFBFormat);
    static FrameBufferFormat GetNiFBFormat(D3DFORMAT eFormat);
    static D3DFORMAT GetD3DFormat(DepthStencilFormat eDSFormat);
    static DepthStencilFormat GetNiDSFormat(D3DFORMAT eFormat);
    static D3DMULTISAMPLE_TYPE GetMultiSampleType(unsigned int uiMode);
    static unsigned int GetMultiSampleQuality(unsigned int uiMode);
    static D3DSWAPEFFECT GetSwapEffect(SwapEffect eMode, 
        unsigned int uiFBMode, unsigned int uiUseFlags);
    static unsigned int GetRefreshRate(unsigned int uiRefreshRateHz);
    static unsigned int GetPresentInterval(
        PresentationInterval ePresentationInterval);
    static bool GetDX9PrimitiveFromNiMeshPrimitiveType(
        NiPrimitiveType::Type eMeshPrimType,
        D3DPRIMITIVETYPE& eOutputD3DPrimitiveType);

    // NiRenderer virtual overrides

    // String describing renderer
    virtual const char* GetDriverInfo() const;

    // Renderer capabilities
    virtual unsigned int GetFlags() const;

    // Renderer ID
    virtual NiSystemDesc::RendererID GetRendererID() const;

    // Render target clearing operations
    virtual void SetDepthClear(const float fZClear);
    virtual float GetDepthClear() const;
    virtual void SetBackgroundColor(const NiColor& kColor);
    virtual void SetBackgroundColor(const NiColorA& kColor);
    virtual void GetBackgroundColor(NiColorA& kColor) const;
    virtual void SetStencilClear(unsigned int uiClear);
    virtual unsigned int GetStencilClear() const;

    // Render target access and management
    virtual bool ValidateRenderTargetGroup(NiRenderTargetGroup* pkTarget);
    virtual bool IsDepthBufferCompatible(Ni2DBuffer* pkBuffer, 
        NiDepthStencilBuffer* pkDSBuffer);

    virtual NiRenderTargetGroup* GetDefaultRenderTargetGroup() const;
    virtual const NiRenderTargetGroup* GetCurrentRenderTargetGroup() const;

    virtual NiDepthStencilBuffer* GetDefaultDepthStencilBuffer() const;
    virtual Ni2DBuffer* GetDefaultBackBuffer() const;

    // Pixel format support queries
    virtual const NiPixelFormat* 
        FindClosestPixelFormat(NiTexture::FormatPrefs& kFmtPrefs) const;
    virtual const NiPixelFormat* FindClosestDepthStencilFormat(
        const NiPixelFormat* pkFrontBufferFormat, unsigned int uiDepthBPP,
        unsigned int uiStencilBPP) const;

    // Multiple render target queries
    virtual unsigned int GetMaxBuffersPerRenderTargetGroup() const;
    virtual bool GetIndependentBufferBitDepths() const;

    // Texture precaching
    virtual bool PrecacheTexture(NiTexture* pkIm);

    // Texture mipmap skip level
    virtual bool SetMipmapSkipLevel(unsigned int uiSkip);
    virtual unsigned int GetMipmapSkipLevel() const;

    // Geometry precaching
    virtual bool PrecacheMesh(
        NiRenderObject* pkMesh, 
        bool bReleaseSystemMemory = true);

    // Perform the precaches that the renderer has queued up
    virtual bool PerformPrecache();

    // Purge cached renderer data
    virtual void PurgeMaterial(NiMaterialProperty* pkMaterial);
    virtual void PurgeEffect(NiDynamicEffect* pLight);
    virtual bool PurgeTexture(NiTexture* pkTexture);

    virtual bool PurgeAllTextures(bool bPurgeLocked);

    // Screen shot
    virtual NiPixelData* TakeScreenShot(
        const NiRect<unsigned int>* pkScreenRect,
        const NiRenderTargetGroup* pkTarget = NULL);

    // Screenshot direct to file
    virtual bool SaveScreenShot(const char* pcFilename,
        EScreenshotFormat eFormat);

    // Ni2DBuffer interaction

    // Copy an arbitrary region from one 2D buffer to the specified
    // coordinates of a different 2D buffer. Note that the NiPixelFormats of
    // both Ni2DBuffers must match for this method. This is the fastest path
    // possible for copying pixel data. Also note that a NULL value for the
    // NiRect will use the dimensions of the Ni2DBuffer.
    virtual bool FastCopy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect = NULL,
        unsigned int uiDestX = 0, unsigned int uiDestY = 0);
    // Copy an arbitrary region from one 2D buffer to another arbitrary region
    // in a different 2D buffer. Note that the NiPixelFormats
    // of both Ni2DBuffers may differ for this method. Also note that a NULL 
    // value for either NiRect will use the dimensions of the Ni2DBuffer.
    // This path is slower than the previous FastCopy method
    virtual bool Copy(const Ni2DBuffer* pkSrc, Ni2DBuffer* pkDest, 
        const NiRect<unsigned int>* pkSrcRect,
        const NiRect<unsigned int>* pkDestRect,
        Ni2DBuffer::CopyFilterPreference ePref);

    // Reversed left-right rendering
    virtual bool GetLeftRightSwap() const;
    virtual bool SetLeftRightSwap(bool bSwap);

    // Global fog settings
    virtual float GetMaxFogValue() const;
    virtual void SetMaxFogValue(float fFogVal);

    // max anisotropy settings
    virtual void SetMaxAnisotropy(unsigned short usMaxAnisotropy);
    inline unsigned short GetHWMaxAnisotropy() const;

    // Default material
    virtual void UseLegacyPipelineAsDefaultMaterial();

    // Return the renderer
    static NiDX9Renderer* GetRenderer();

    // Return pointer to array of D3DXMACRO structures for specified file type
    const D3DXMACRO* GetD3DXMacroList(
        const char* pcFileType,
        const D3DXMACRO* pkUserMacros = NULL);

    // Return pointer to array of CG compiler command-line parameters for
    // specified file type, which specify macro definitions and optionally
    // user-specified parameters appended.
    const char** GetCGMacroList(
        const char* pcFileType,
        const char** ppcUserParams = NULL);

    // *** begin Emergent internal use only ***
    // Model state
    void SetModelTransform(const NiTransform& kXform, 
        bool bPushToDevice = true);
    void SetSkinnedModelTransforms(NiMesh* pkMesh, NiUInt32 uiSubmesh);

    // Vertex description
    enum
    {
        VERTEX_FLAG_DX_TEXTUREMASK              = 0xFF000000,
        VERTEX_FLAG_DX_TEXTURESHIFT             = 24,
        VERTEX_FLAG_DX_HASNORMALS               = 0x00800000,
        VERTEX_FLAG_DX_HASCOLORS                = 0x00400000,
        VERTEX_FLAG_DX_HASBINORMALSTANGENTS     = 0x00200000
    };

    // Pointer release wrappers for cross-platform compatibility
    static void ReleaseResource(D3DResourcePtr pkResource);
    static void ReleaseVBResource(D3DVertexBufferPtr pkResource);
    static void ReleaseIBResource(D3DIndexBufferPtr pkResource);
    static void ReleaseTextureResource(D3DTexturePtr pkResource);
    static void ReleaseSwapChain(LPDIRECT3DSWAPCHAIN9 pkSwapChain);
    static void ReleaseDevice(D3DDevicePtr pkDevice);
    static void ReleaseVertexDecl(NiD3DVertexDeclaration pkVertDecl);

    // Renderer specific data for textures and palettes
    virtual bool CreateSourceTextureRendererData(NiSourceTexture* pkTexture);
    virtual bool CreateRenderedTextureRendererData(
        NiRenderedTexture* pkTexture, 
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE);
    virtual bool CreateSourceCubeMapRendererData(
        NiSourceCubeMap* pkCubeMap);
    virtual bool CreateRenderedCubeMapRendererData(
        NiRenderedCubeMap* pkCubeMap);
    virtual bool CreateDynamicTextureRendererData(
        NiDynamicTexture* pkTexture);
    virtual void CreatePaletteRendererData(NiPalette* pkPalette);
    virtual bool CreateDepthStencilRendererData(
        NiDepthStencilBuffer* pkDSBuffer, const NiPixelFormat* pkFormat,
        Ni2DBuffer::MultiSamplePreference eMSAAPref = 
        Ni2DBuffer::MULTISAMPLE_NONE);

    virtual void RemoveRenderedCubeMapData(NiRenderedCubeMap* pkCubeMap);
    virtual void RemoveRenderedTextureData(NiRenderedTexture* pkTexture);
    virtual void RemoveDynamicTextureData(NiDynamicTexture* pkTexture);

    virtual void* LockDynamicTexture(const NiTexture::RendererData* pkRData,
        int& iPitch);
    virtual bool UnLockDynamicTexture(const NiTexture::RendererData* pkRData);

    virtual NiShader* GetFragmentShader(
        NiMaterialDescriptor* pkMaterialDescriptor);
    virtual NiShader* GetShadowWriteShader(
        NiMaterialDescriptor* pkMaterialDescriptor);
    virtual void SetRenderShadowCasterBackfaces(bool bRenderBackfaces);

    virtual void SetDefaultProgramCache(NiFragmentMaterial* pkMaterial, 
        bool bAutoWriteToDisk, bool bWriteDebugFile, bool bLoad,
        bool bNoNewProgramCreation, const char* pcWorkingDir);

    inline NiPixelFormat** GetTextureFormatArray(TexUsage eUse);
    inline const NiPixelFormat* GetDefaultTextureFormat(TexUsage eUse) const;
    inline const NiPixelData* GetDefaultTextureData(TexUsage eUse) const;

    TexFormat GetReplacementDataFormat() const;

    inline NiTexture* GetClipperTexture() const;

    inline const D3DMATRIX& GetD3DMat() const;
    inline const D3DMATRIX& GetD3DView() const;
    inline const D3DMATRIX& GetD3DProj() const;
    inline const D3DMATRIX& GetInvView() const;

    // This is to be considered 'internal only'
    inline const D3DXALIGNEDMATRIX* GetD3DWorld() const;

    // Shader version information
    inline void SetMinPixelShaderVersion(unsigned int uiMaj,
        unsigned int uiMin);
    inline void SetPixelShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetMinVertexShaderVersion(unsigned int uiMaj,
        unsigned int uiMin);
    inline void SetVertexShaderVersionRequest(unsigned int uiMaj, 
        unsigned int uiMin);
    inline void SetSystemUserVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetMinUserVersion(unsigned int uiMaj, unsigned int uiMin);
    inline void SetUserVersionRequest(unsigned int uiMaj, unsigned int uiMin);
    inline unsigned int GetMinPixelShaderVersion() const;
    inline unsigned int GetPixelShaderVersionRequest() const;
    inline unsigned int GetMinVertexShaderVersion() const;
    inline unsigned int GetVertexShaderVersionRequest() const;
    inline unsigned int GetMinUserVersion() const;
    inline unsigned int GetUserVersionRequest() const;

    // D3D Default Buffer Management
    inline void AddD3DDefaultBuffer(NiDX9DataStream* pkDataStream);
    inline void RemoveD3DDefaultBuffer(NiDX9DataStream* pkDataStream);

    /**
    @name Shared Vertex Buffers
    Used to share a larger vertex buffer between many NiDataStream objects
    */
    //@{

    /// Lock around access and modification of shared buffers,
    /// hold the lock until after all modifications to NiDX9BufferInfo.
    inline void AquireSharedBufferLock();

    /// Lock around access and modification of shared buffers,
    /// hold the lock until after all modifications to NiDX9BufferInfo.
    inline void ReleaseSharedBufferLock();

    /// Must have called AquireSharedBufferLock()
    inline void AddSharedVertexBuffer(
        unsigned int uiStride,
        NiDX9DataStream::NiDX9BufferInfo* pkBufferInfo);

    /// Must have called AquireSharedBufferLock()
    inline NiDX9DataStream::NiDX9BufferInfo* GetSharedVertexBuffer(
        unsigned int uiStride);

    /// Must have called AquireSharedBufferLock()
    inline bool RemoveSharedVertexBuffer(unsigned int uiStride);

    /// Must have called AquireSharedBufferLock()
    inline void AddSharedIndexBuffer(
        unsigned int uiStride,
        NiDX9DataStream::NiDX9BufferInfo* pkBufferInfo);

    /// Must have called AquireSharedBufferLock()
    inline NiDX9DataStream::NiDX9BufferInfo* GetSharedIndexBuffer(
        unsigned int uiStride);

    /// Must have called AquireSharedBufferLock()
    inline bool RemoveSharedIndexBuffer(unsigned int uiStride);

    protected:
        /// Protects m_kSharedDynamicVertexBuffers and 
        /// m_kSharedDynamicIndexBuffers
        NiCriticalSection m_kSharedBufferLock;

        NiTPointerMap<unsigned int, NiDX9DataStream::NiDX9BufferInfo*>
            m_kSharedDynamicVertexBuffers;
        NiTPointerMap<unsigned int, NiDX9DataStream::NiDX9BufferInfo*>
            m_kSharedDynamicIndexBuffers;
    public:
    //@}

    // Texture support information
    inline unsigned int GetMaxTextureWidth() const;
    inline unsigned int GetMaxTextureHeight() const;
    inline unsigned int GetMaxTextureAspectRatio() const;

    // Threading information
    inline unsigned long GetDeviceThreadID() const;

    inline const NiD3DShaderLibraryVersion* GetShaderLibraryVersion() const;

    static void _SDMShutdown();

    class  AddressSettings : public NiMemObject
    {
    public:
        D3DTEXTUREADDRESS m_eU;
        D3DTEXTUREADDRESS m_eV;
    };

    class FilterSettings : public NiMemObject
    {
    public:
        D3DTEXTUREFILTERTYPE m_eMin;
        D3DTEXTUREFILTERTYPE m_eMag;
        D3DTEXTUREFILTERTYPE m_eMip;
    };

    static AddressSettings GetD3DClampMode(
        NiTexturingProperty::ClampMode eClamp);
    static FilterSettings GetD3DFilterMode(
        NiTexturingProperty::FilterMode eFilter);

    static unsigned int GetProjectedTextureFlags();

    // Prepacking
    class PrePackMesh : public NiMemObject
    {
    public:
        NiMeshPtr m_spMesh;
        bool m_bReleaseSystemMemory;
    };

    class PrePackTexture : public NiMemObject
    {
    public:
        NiTexturePtr m_spTexture;
        NiPixelDataPtr m_spSrcPixelData;
    };

    // *** end Emergent internal use only ***

protected:

    //  Start-up and shutdown related functions...
    NiDX9Renderer();
    bool Initialize(unsigned int uiWidth, unsigned int uiHeight,
        unsigned int uiUseFlags, NiWindowRef kWndDevice, 
        NiWindowRef kWndFocus, unsigned int uiAdapter, DeviceDesc eDesc,
        FrameBufferFormat eFBFormat, DepthStencilFormat eDSFormat, 
        PresentationInterval ePresentationInterval, SwapEffect eSwapEffect,
        unsigned int uiFBMode, unsigned int uiBackBufferCount,
        unsigned int uiRefreshRate);

    bool InitializeRendererFromDevice(
        D3DPRESENT_PARAMETERS kD3DPresentParams);

    void InitToDefault();
    static bool IsD3D9Create();
    static int CreateDirect3D9();

    // LostDevice handler
    bool LostDeviceRestore();

    // 'Check' functions
    bool InitializePresentParams(NiWindowRef kWndDevice, unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiUseFlags, 
        unsigned int uiFBMode, 
        FrameBufferFormat eFBFormat, DepthStencilFormat eDSFormat,
        unsigned int uiBackBufferCount, SwapEffect eSwapEffect, 
        unsigned int uiRefreshRate, 
        PresentationInterval ePresentationInterval,
        D3DPRESENT_PARAMETERS& kD3DPresentParams) const;

    bool InitializeTextureDefaults();
    bool InitializeDeviceCaps(const D3DPRESENT_PARAMETERS& kPresentParams);

    // Renderer specific implementations
    virtual bool Do_BeginFrame();
    virtual bool Do_EndFrame();
    virtual bool Do_DisplayFrame();
    virtual void Do_ClearBuffer(const NiRect<float>* pkR,
        unsigned int uiMode);
    virtual void Do_SetCameraData(const NiPoint3& kWorldLoc,
        const NiPoint3& kWorldDir, const NiPoint3& kWorldUp, 
        const NiPoint3& kWorldRight, const NiFrustum& kFrustum, 
        const NiRect<float>& kPort);
    virtual void Do_SetScreenSpaceCameraData(
        const NiRect<float>* pkPort = NULL);
    virtual void Do_GetCameraData(NiPoint3& kWorldLoc, NiPoint3& kWorldDir,
        NiPoint3& kWorldUp, NiPoint3& kWorldRight, NiFrustum& kFrustum,
        NiRect<float>& kPort);
    virtual bool Do_BeginUsingRenderTargetGroup(
        NiRenderTargetGroup* pkTarget, 
        unsigned int uiClearMode);
    virtual bool Do_EndUsingRenderTargetGroup();

    virtual void Do_RenderMesh(NiMesh* pkMesh);

    // Conversion routines
    inline char* GetDevTypeString() const;
    inline char* GetBehaviorString() const;

    static bool ExpandDeviceType(DeviceDesc eDesc, D3DDEVTYPE& eDevType,
        unsigned int& uiBehaviorFlags);

    // Returns a shader for a mesh
    // falls back to default material, or even error shader, if needed.
    NiD3DShaderInterface* GetShaderAndVertexDecl(NiRenderObject* pkMesh, 
        NiDX9MeshMaterialBindingPtr& spVertexDecl);

    // Returns a shader for a mesh
    // falls back to default material if needed, but not error shader.
    NiD3DShaderInterface* GetShaderAndVertexDecl_NoErrorShader(
        NiRenderObject* pkMesh, NiDX9MeshMaterialBindingPtr& spVertexDecl);

    bool GenerateDefaultTextureData();

    void CreatePixelFormatArray(NiPixelFormat** ppkFormatArray,
        unsigned int uiUsage, D3DRESOURCETYPE eType);

    enum D3DResourceType
    {
        D3DRT_GENERIC,
        D3DRT_VERTEXBUFFER,
        D3DRT_INDEXBUFFER,
        D3DRT_TEXTURE,
        D3DRT_SWAPCHAIN,
        D3DRT_DEVICE,
        D3DRT_VERTEXDECL,

        D3DRT_COUNT
    };
    static unsigned int ReleaseIUnknown(
        IUnknown* pkD3DResource, 
        D3DResourceType eType);

    void ReleaseDeferredResources();

    static LPDIRECT3D9 ms_pkD3D9;
    LPDIRECT3DDEVICE9 m_pkD3DDevice9;
    D3DCAPS9 m_kD3DCaps9;

    NiWindowRef m_kWndDevice;
    NiWindowRef m_kWndFocus;
    mutable char m_acDriverDesc[512];
    unsigned int m_uiAdapter;
    D3DDEVTYPE m_eDevType;
    unsigned int m_uiBehaviorFlags;
    D3DFORMAT m_eAdapterFormat;
    
    const static NiDX9SystemDesc* ms_pkSystemDesc;
    const NiDX9AdapterDesc* m_pkAdapterDesc;
    const NiDX9DeviceDesc* m_pkDeviceDesc;

    unsigned int m_uiBackground;
    float m_fZClear;
    unsigned int m_uiStencilClear;
    unsigned int m_uiRendFlags;
    mutable char m_acBehavior[32];

    // Prepacking
    NiTObjectList<PrePackMesh> m_kPrePackMeshes;
    NiTObjectList<PrePackTexture> m_kPrePackTextures;

    // Local copies of camera vectors for points and texture transforms
    NiPoint3 m_kCamRight;
    NiPoint3 m_kCamUp;
    NiPoint3 m_kModelCamRight;
    NiPoint3 m_kModelCamUp;

    float m_fNearDepth, m_fDepthRange;
    
    D3DMATRIX m_kD3DIdentity;

    D3DVIEWPORT9 m_kD3DPort; // full framebuffer viewport

    unsigned int m_uiHWBones;
    unsigned int m_uiMaxStreams;
    unsigned int m_uiMaxPixelShaderVersion;
    unsigned int m_uiMaxVertexShaderVersion;
    bool m_bMipmapCubeMaps;
    bool m_bDynamicTexturesCapable;

    unsigned int m_uiResetCounter;
    bool m_bDeviceLost;

    enum
    {
        HW_MAX_ANISOTROPY = 16
    };

    unsigned short m_usHwMaxAnisotropy;

    // Pixel Format mappings
    NiPixelFormat* m_aapkTextureFormats[TEXUSE_NUM][TEX_NUM];
    NiPixelFormat* m_apkDefaultTextureFormat[TEXUSE_NUM];
    // Default texture data
    NiPixelDataPtr m_aspDefaultTextureData[TEXUSE_NUM];

    TexFormat m_eReplacementDataFormat;

    NiRenderTargetGroupPtr m_spDefaultRenderTargetGroup;
    NiRenderTargetGroup* m_pkCurrRenderTargetGroup;

    NiRenderTargetGroup* m_pkCurrOnscreenRenderTargetGroup;
    NiTPointerMap<NiWindowRef, NiRenderTargetGroupPtr> 
        m_kOnscreenRenderTargetGroups;
    unsigned int m_uiMaxNumRenderTargets;
    bool m_bIndependentBitDepths;
    bool m_bMRTPostPixelShaderBlending;

    NiPropertyStatePtr m_spScreenTexturePropertyState;

    // Managers
    NiDX9RenderState* m_pkRenderState;
    NiDX9TextureManager* m_pkTextureManager;
    NiDX9LightManager* m_pkLightManager;

    // Rendered textures and cube maps
    NiTPointerMap<NiRenderedTexture*, NiDX9RenderedTextureData*> 
        m_kRenderedTextures;
    NiTPointerMap<NiRenderedCubeMap*, NiDX9RenderedCubeMapData*> 
        m_kRenderedCubeMaps;

    // Dynamic textures
    NiTPointerMap<NiDynamicTexture*, NiDX9DynamicTextureData*> 
        m_kDynamicTextures;

    // Clipper texture
    NiTexturePtr m_spClipperImage;

    NiTPointerList<NiDX92DBufferDataPtr> m_kBuffersToUseAtDisplayFrame;

    // Shaders
    NiTPointerList<NiD3DShaderInterface*> m_kD3DShaders;
    NiD3DShaderLibraryVersion m_kShaderLibraryVersion;

    // Temporary members - used to speed up code
    D3DXALIGNEDMATRIX m_kD3DMat;
    D3DMATRIX m_kD3DView;
    D3DMATRIX m_kD3DProj;
    D3DMATRIX m_kInvView;
    // Screen texture members
    NiPoint2* m_pkScreenTextureVerts;
    NiColorA* m_pkScreenTextureColors;
    NiPoint2* m_pkScreenTextureTexCoords;
    unsigned short m_usNumScreenTextureVerts;
    unsigned short* m_pusScreenTextureIndices;
    unsigned int m_uiNumScreenTextureIndices;

    // Creation parameters
    unsigned int m_uiCreationWidth;
    unsigned int m_uiCreationHeight;
    unsigned int m_uiCreationUseFlags;
    NiWindowRef m_kCreationWndDevice;
    NiWindowRef m_kCreationWndFocus;
    unsigned int m_uiCreationAdapter;
    DeviceDesc m_eCreationDesc;
    FrameBufferFormat m_eCreationFBFormat;
    DepthStencilFormat m_eCreationDSFormat;
    PresentationInterval m_eCreationPresentationInterval;
    SwapEffect m_eCreationSwapEffect;
    unsigned int m_uiCreationFBMode;
    unsigned int m_uiCreationBackBufferCount;
    unsigned int m_uiCreationRefreshRate;

    // Device thread ID
    unsigned long m_ulDeviceThreadID;

    /// D3DPOOL_DEFAULT buffers list that must be recreated on lost device
    NiTPointerList<NiDX9DataStream*> m_kD3DDefaultBuffers;

    /// protects m_kD3DDefaultBuffers
    NiCriticalSection m_kD3DDefaultBuffersLock;

    // Default shader
    NiD3DDefaultShaderPtr m_spLegacyDefaultShader;
    NiMaterialPtr m_spLegacyDefaultShaderMaterial;

    // Reset notification functions
    NiTPrimitiveArray<RESETNOTIFYFUNC> m_kResetNotifyFuncs;
    NiTPrimitiveArray<void*> m_kResetNotifyFuncData;

    // LostDevice notification functions
    NiTPrimitiveArray<LOSTDEVICENOTIFYFUNC> m_kLostDeviceNotifyFuncs;
    NiTPrimitiveArray<void*> m_kLostDeviceNotifyFuncData;

    mutable NiTMap<D3DFORMAT, NiPixelFormat*> m_kDepthStencilFormats;

    static NiTPointerList<unsigned int> ms_kDisplayFormats;

    static HINSTANCE ms_hDXLib;
    static NILPD3DCREATE ms_pfnD3DCreate9;
    static NiCriticalSection ms_kD3DCriticalSection;

    // Deferred resource release 
    static NiCriticalSection ms_kDeferredReleaseLock;
    static NiTPointerList<IUnknown*> ms_akDeferredReleaseList[D3DRT_COUNT];

    // Static flags/values/conversion arrays

    // Map NiTexturingProperty::ClampModes to D3D clamping
    static AddressSettings 
        ms_akD3DAddressMapping[NiTexturingProperty::CLAMP_MAX_MODES];
    // Map NiTexturingProperty::FilterModes to D3D filtering
    static FilterSettings 
        ms_akD3DFilterMapping[NiTexturingProperty::FILTER_MAX_MODES];

    // Map D3DFORMAT to sizes
    enum 
    {
        D3DFORMATSIZECOUNT = 118
    };

    static unsigned char ms_aucD3DFormatSizes[D3DFORMATSIZECOUNT];

    static unsigned int ms_uiProjectedTextureFlags; //  Projected flags

    // Cached camera data.
    NiFrustum m_kCachedFrustum;
    NiRect<float> m_kCachedPort;

    // Buffers and their length for holding macro definition lists
    D3DXMACRO* m_pkD3DXMacroBuffer;
    NiUInt32 m_uiD3DXMacroBufferLen;
    const char** m_ppcCGMacroPtrBuffer;
    char* m_pcCGMacroBuffer;
    NiUInt32 m_uiCGMacroPtrBufferLen;
    NiUInt32 m_uiCGMacroBufferLen;
};

#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

typedef NiPointer<NiDX9Renderer> NiDX9RendererPtr;

// Assumed to be called from within an NiDX9Renderer object
#define NIASSERT_D3D9_DEVICE_THREAD \
    NIASSERT(((m_uiCreationUseFlags & USE_MULTITHREADED) != 0) || \
        m_ulDeviceThreadID == NiGetCurrentThreadId() || \
        !"Failing this assertion means a function was called by a thread "\
        "other than the thread on which the Renderer was created. "\
        "If this behavior is intentional, the Renderer must be created "\
        "with the USE_MULTITHREADED flag enabled.");

#include "NiDX9Renderer.inl"

#endif  //#ifndef NIDX9RENDERER_H
