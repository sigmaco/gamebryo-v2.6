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
inline unsigned int NiDX9Renderer::GetFlags() const
{
    return m_uiRendFlags;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::GetLeftRightSwap() const
{ 
    NIASSERT(m_pkRenderState);
    return m_pkRenderState->GetLeftHanded(); 
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::SetLeftRightSwap(bool bSwap)
{
    NIASSERT(m_pkRenderState);
    m_pkRenderState->SetLeftHanded(bSwap); 
    return true;
}
//---------------------------------------------------------------------------
inline const NiRenderTargetGroup* 
    NiDX9Renderer::GetCurrentRenderTargetGroup() const
{
    return m_pkCurrRenderTargetGroup;
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetDepthClear(const float fZClear)
{
    m_fZClear = fZClear;
}
//---------------------------------------------------------------------------
inline float NiDX9Renderer::GetDepthClear() const
{
    return m_fZClear;
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetBackgroundColor(const NiColor& kColor)
{
    unsigned char ucR = (unsigned char)(kColor.r * 255.0f);
    unsigned char ucG = (unsigned char)(kColor.g * 255.0f);
    unsigned char ucB = (unsigned char)(kColor.b * 255.0f);
    m_uiBackground = D3DCOLOR_XRGB(ucR, ucG, ucB);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetBackgroundColor(const NiColorA& kColor)
{
    unsigned char ucR = (unsigned char)(kColor.r * 255.0f);
    unsigned char ucG = (unsigned char)(kColor.g * 255.0f);
    unsigned char ucB = (unsigned char)(kColor.b * 255.0f);
    unsigned char ucA = (unsigned char)(kColor.a * 255.0f);
    m_uiBackground = D3DCOLOR_ARGB(ucA, ucR, ucG, ucB);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::GetBackgroundColor(NiColorA& kColor) const
{
    const float fConstant = 1.0f / 255.0f;
    kColor.a = ((m_uiBackground >> 24) & 0xff) * fConstant;
    kColor.r = ((m_uiBackground >> 16) & 0xff) * fConstant;
    kColor.g = ((m_uiBackground >> 8) & 0xff) * fConstant;
    kColor.b = (m_uiBackground & 0xff) * fConstant;
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetStencilClear(unsigned int uiClear)
{
    m_uiStencilClear = uiClear;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetStencilClear() const
{
    return m_uiStencilClear;
}
//---------------------------------------------------------------------------
inline float NiDX9Renderer::GetMaxFogValue() const
{
    NIASSERT(m_pkRenderState);
    return m_pkRenderState->GetMaxFogValue();
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetMaxFogValue(float fFogVal)
{
    NIASSERT(m_pkRenderState);
    m_pkRenderState->SetMaxFogValue(fFogVal);
}
//---------------------------------------------------------------------------
inline NiDX9Renderer* NiDX9Renderer::GetRenderer()
{
    if (ms_pkRenderer && 
        ms_pkRenderer->GetRendererID() == NiSystemDesc::RENDERER_DX9)
    {
        return (NiDX9Renderer*)ms_pkRenderer;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::UseQuickDecals(bool bQuickDecals)
{
    NIASSERT(m_spLegacyDefaultShader);
    m_spLegacyDefaultShader->UseQuickDecals(bQuickDecals);
}
//---------------------------------------------------------------------------
inline LPDIRECT3DDEVICE9 NiDX9Renderer::GetD3DDevice() const
{
    return m_pkD3DDevice9;
}
//---------------------------------------------------------------------------
inline NiDX9RenderState* NiDX9Renderer::GetRenderState() const
{
    return m_pkRenderState;
}
//---------------------------------------------------------------------------
inline NiDX9TextureManager* NiDX9Renderer::GetTextureManager() const
{
    return m_pkTextureManager;
}
//---------------------------------------------------------------------------
inline NiDX9LightManager* NiDX9Renderer::GetLightManager() const
{
    return m_pkLightManager;
}
//---------------------------------------------------------------------------
inline NiD3DShader* NiDX9Renderer::GetLegacyDefaultShader() const
{
    return m_spLegacyDefaultShader;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMaxStreams() const
{
    return m_uiMaxStreams;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetPixelShaderVersion() const
{
    return m_uiMaxPixelShaderVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetVertexShaderVersion() const
{
    return m_uiMaxVertexShaderVersion;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::GetMipmapCubeMaps() const
{
    return m_bMipmapCubeMaps;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::IsDynamicTexturesCapable() const
{
    return m_bDynamicTexturesCapable;
}
//---------------------------------------------------------------------------
inline char* NiDX9Renderer::GetDevTypeString() const
{
    switch (m_eDevType)
    {
    case D3DDEVTYPE_HAL:
        return "HAL";
    case D3DDEVTYPE_REF:
        return "REF";
    case D3DDEVTYPE_SW:
        return "SW";
    }

    return "???";
}
//---------------------------------------------------------------------------
inline char* NiDX9Renderer::GetBehaviorString() const
{
    m_acBehavior[0] = '\0';

    if (m_uiBehaviorFlags & D3DCREATE_PUREDEVICE)
        NiStrcat(m_acBehavior, 32, "PURE");
    if (m_uiBehaviorFlags & D3DCREATE_MULTITHREADED)
        NiStrcat(m_acBehavior, 32, " MT");
    if (m_uiBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
        NiStrcat(m_acBehavior, 32, " HWVP");
    if (m_uiBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)
        NiStrcat(m_acBehavior, 32, " MIXVP");
    if (m_uiBehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING)
        NiStrcat(m_acBehavior, 32, " SWVP");

    return m_acBehavior;
}
//---------------------------------------------------------------------------
inline D3DFORMAT NiDX9Renderer::GetD3DFormat(
    NiDX9Renderer::FrameBufferFormat eFBFormat)
{
    switch (eFBFormat)
    {
    case FBFMT_UNKNOWN:
        return D3DFMT_UNKNOWN;
    case FBFMT_R8G8B8:
        return D3DFMT_R8G8B8;
    case FBFMT_A8R8G8B8:
        return D3DFMT_A8R8G8B8;
    case FBFMT_X8R8G8B8:
        return D3DFMT_X8R8G8B8;
    case FBFMT_R5G6B5:
        return D3DFMT_R5G6B5;
    case FBFMT_X1R5G5B5:
        return D3DFMT_X1R5G5B5;
    case FBFMT_A1R5G5B5:
        return D3DFMT_A1R5G5B5;
    case FBFMT_A4R4G4B4:
        return D3DFMT_A4R4G4B4;
    case FBFMT_R3G3B2:
        return D3DFMT_R3G3B2;
    case FBFMT_A8:
        return D3DFMT_A8;
    case FBFMT_A8R3G3B2:
        return D3DFMT_A8R3G3B2;
    case FBFMT_X4R4G4B4:
        return D3DFMT_X4R4G4B4;
    case FBFMT_R16F:
        return D3DFMT_R16F;
    case FBFMT_G16R16F:
        return D3DFMT_G16R16F;
    case FBFMT_A16B16G16R16F:
        return D3DFMT_A16B16G16R16F;
    case FBFMT_R32F:
        return D3DFMT_R32F;
    case FBFMT_G32R32F:
        return D3DFMT_G32R32F;
    case FBFMT_A32B32G32R32F:
        return D3DFMT_A32B32G32R32F;
    }

    return D3DFMT_UNKNOWN;
}
//---------------------------------------------------------------------------
inline D3DFORMAT NiDX9Renderer::GetCompatibleAdapterFormat(
    NiDX9Renderer::FrameBufferFormat eFBFormat)
{
    switch (eFBFormat)
    {
    case FBFMT_UNKNOWN:
    case FBFMT_A8:
    case FBFMT_A8R3G3B2:
        return D3DFMT_UNKNOWN;
    case FBFMT_R8G8B8:
        return D3DFMT_R8G8B8;
    case FBFMT_A8R8G8B8:
    case FBFMT_X8R8G8B8:
        return D3DFMT_X8R8G8B8;
    case FBFMT_R5G6B5:
        return D3DFMT_R5G6B5;
    case FBFMT_X1R5G5B5:
    case FBFMT_A1R5G5B5:
        return D3DFMT_X1R5G5B5;
    case FBFMT_X4R4G4B4:
    case FBFMT_A4R4G4B4:
        return D3DFMT_X4R4G4B4;
    case FBFMT_R3G3B2:
        return D3DFMT_R3G3B2;
    }

    return D3DFMT_UNKNOWN;
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::FrameBufferFormat NiDX9Renderer::GetNiFBFormat(
    D3DFORMAT eFormat)
{
    switch (eFormat)
    {
    case D3DFMT_UNKNOWN:
        return FBFMT_UNKNOWN;
    case D3DFMT_R8G8B8:
        return FBFMT_R8G8B8;
    case D3DFMT_A8R8G8B8:
        return FBFMT_A8R8G8B8;
    case D3DFMT_X8R8G8B8:
        return FBFMT_X8R8G8B8;
    case D3DFMT_R5G6B5:
        return FBFMT_R5G6B5;
    case D3DFMT_X1R5G5B5:
        return FBFMT_X1R5G5B5;
    case D3DFMT_A1R5G5B5:
        return FBFMT_A1R5G5B5;
    case D3DFMT_A4R4G4B4:
        return FBFMT_A4R4G4B4;
    case D3DFMT_R3G3B2:
        return FBFMT_R3G3B2;
    case D3DFMT_A8:
        return FBFMT_A8;
    case D3DFMT_A8R3G3B2:
        return FBFMT_A8R3G3B2;
    case D3DFMT_X4R4G4B4:
        return FBFMT_X4R4G4B4;
    case D3DFMT_R16F:
        return FBFMT_R16F;
    case D3DFMT_G16R16F:
        return FBFMT_G16R16F;
    case D3DFMT_A16B16G16R16F:
        return FBFMT_A16B16G16R16F;
    case D3DFMT_R32F:
        return FBFMT_R32F;
    case D3DFMT_G32R32F:
        return FBFMT_G32R32F;
    case D3DFMT_A32B32G32R32F:
        return FBFMT_A32B32G32R32F;
    }

    return FBFMT_UNKNOWN;
}
//---------------------------------------------------------------------------
inline D3DFORMAT NiDX9Renderer::GetD3DFormat(DepthStencilFormat eDSFormat)
{
    //  For now, this is a straight conversion.
    //  This function is provided for future changes
    return (D3DFORMAT)eDSFormat;
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::DepthStencilFormat NiDX9Renderer::GetNiDSFormat(
    D3DFORMAT eFormat)
{
    //  For now, this is a straight conversion.
    //  This function is provided for future changes
    return (NiDX9Renderer::DepthStencilFormat)eFormat;
}
//---------------------------------------------------------------------------
inline D3DMULTISAMPLE_TYPE NiDX9Renderer::GetMultiSampleType(
    unsigned int uiMode)
{
    if (uiMode == FBMODE_DEFAULT || uiMode == FBMODE_LOCKABLE)
        return D3DMULTISAMPLE_NONE;

    uiMode &= ~FBMODE_QUALITY_MASK;

    switch (uiMode)
    {
    case FBMODE_MULTISAMPLES_2:
        return D3DMULTISAMPLE_2_SAMPLES;
    case FBMODE_MULTISAMPLES_3:
        return D3DMULTISAMPLE_3_SAMPLES;
    case FBMODE_MULTISAMPLES_4:
        return D3DMULTISAMPLE_4_SAMPLES;
    case FBMODE_MULTISAMPLES_5:
        return D3DMULTISAMPLE_5_SAMPLES;
    case FBMODE_MULTISAMPLES_6:
        return D3DMULTISAMPLE_6_SAMPLES;
    case FBMODE_MULTISAMPLES_7:
        return D3DMULTISAMPLE_7_SAMPLES;
    case FBMODE_MULTISAMPLES_8:
        return D3DMULTISAMPLE_8_SAMPLES;
    case FBMODE_MULTISAMPLES_9:
        return D3DMULTISAMPLE_9_SAMPLES;
    case FBMODE_MULTISAMPLES_10:
        return D3DMULTISAMPLE_10_SAMPLES;
    case FBMODE_MULTISAMPLES_11:
        return D3DMULTISAMPLE_11_SAMPLES;
    case FBMODE_MULTISAMPLES_12:
        return D3DMULTISAMPLE_12_SAMPLES;
    case FBMODE_MULTISAMPLES_13:
        return D3DMULTISAMPLE_13_SAMPLES;
    case FBMODE_MULTISAMPLES_14:
        return D3DMULTISAMPLE_14_SAMPLES;
    case FBMODE_MULTISAMPLES_15:
        return D3DMULTISAMPLE_15_SAMPLES;
    case FBMODE_MULTISAMPLES_16:
        return D3DMULTISAMPLE_16_SAMPLES;
    case FBMODE_MULTISAMPLES_NONMASKABLE:
        return D3DMULTISAMPLE_NONMASKABLE;
    default:
        NIASSERT(false);
    }

    return D3DMULTISAMPLE_NONE;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMultiSampleQuality(unsigned int uiMode)
{
    if (uiMode == FBMODE_DEFAULT || uiMode == FBMODE_LOCKABLE)
        return 0;

    return (uiMode & FBMODE_QUALITY_MASK);
}
//---------------------------------------------------------------------------
inline D3DSWAPEFFECT NiDX9Renderer::GetSwapEffect(
    SwapEffect eMode, unsigned int uiFBMode, unsigned int)
{
    // Must use discard with a multisampled framebuffer
    if ((uiFBMode != FBMODE_DEFAULT) && (uiFBMode != FBMODE_LOCKABLE))
        return D3DSWAPEFFECT_DISCARD;

    switch (eMode)
    {
    case SWAPEFFECT_DISCARD:
        return D3DSWAPEFFECT_DISCARD;
    case SWAPEFFECT_FLIP:
        return D3DSWAPEFFECT_FLIP;
    case SWAPEFFECT_COPY:
        return D3DSWAPEFFECT_COPY;
    default:
        return D3DSWAPEFFECT_DISCARD;
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetRefreshRate(
    unsigned int uiRefreshRateHz)
{
    switch (uiRefreshRateHz)
    {
    case REFRESHRATE_DEFAULT:
        return D3DPRESENT_RATE_DEFAULT;
    }

    return uiRefreshRateHz;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetPresentInterval(
    NiDX9Renderer::PresentationInterval ePresentationInterval)
{
    switch (ePresentationInterval)
    {
    case PRESENT_INTERVAL_IMMEDIATE:
        return D3DPRESENT_INTERVAL_IMMEDIATE;
    case PRESENT_INTERVAL_ONE:
        return D3DPRESENT_INTERVAL_ONE;
    case PRESENT_INTERVAL_TWO:
        return D3DPRESENT_INTERVAL_TWO;
    case PRESENT_INTERVAL_THREE:
        return D3DPRESENT_INTERVAL_THREE;
    case PRESENT_INTERVAL_FOUR:
        return D3DPRESENT_INTERVAL_FOUR;
    }

    return D3DPRESENT_INTERVAL_DEFAULT;
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::AddressSettings NiDX9Renderer::GetD3DClampMode(
    NiTexturingProperty::ClampMode eClamp)
{
    return ms_akD3DAddressMapping[eClamp];
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::FilterSettings NiDX9Renderer::GetD3DFilterMode(
    NiTexturingProperty::FilterMode eFilter)
{
    return ms_akD3DFilterMapping[eFilter];
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetAdapter() const
{
    return m_uiAdapter;
}
//---------------------------------------------------------------------------
inline D3DDEVTYPE NiDX9Renderer::GetDevType() const
{
    return m_eDevType;
}
//---------------------------------------------------------------------------
inline D3DPRESENT_PARAMETERS* NiDX9Renderer::GetPresentParams(
    const NiRenderTargetGroup* pkTarget) const
{
    if (pkTarget == NULL)
        pkTarget = m_spDefaultRenderTargetGroup;
    const Ni2DBuffer* pkBuffer = pkTarget->GetBuffer(0);
    NiDX9OnscreenBufferData* pkData = NiDynamicCast(NiDX9OnscreenBufferData,
        (NiDX92DBufferData*)pkBuffer->GetRendererData());
    NIASSERT(pkData);
    return &pkData->GetPresentParams();
}
//---------------------------------------------------------------------------
inline D3DFORMAT NiDX9Renderer::GetAdapterFormat() const
{
    return m_eAdapterFormat;
}
//---------------------------------------------------------------------------
inline NiPixelFormat** NiDX9Renderer::GetTextureFormatArray(
    TexUsage eUse)
{
    return m_aapkTextureFormats[eUse];
}
//---------------------------------------------------------------------------
inline const NiPixelFormat* NiDX9Renderer::GetDefaultTextureFormat(
    TexUsage eUse) const
{
    return m_apkDefaultTextureFormat[eUse];
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::TexFormat NiDX9Renderer::GetReplacementDataFormat() 
    const
{
    return m_eReplacementDataFormat;
}
//---------------------------------------------------------------------------
inline const NiPixelData* NiDX9Renderer::GetDefaultTextureData(TexUsage eUse)
    const
{
    return m_aspDefaultTextureData[eUse];
}
//---------------------------------------------------------------------------
inline NiTexture* NiDX9Renderer::GetClipperTexture() const
{
    return m_spClipperImage;
}
//---------------------------------------------------------------------------
inline LPDIRECT3D9 NiDX9Renderer::GetDirect3D()
{
    return ms_pkD3D9;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetProjectedTextureFlags()
{
    return ms_uiProjectedTextureFlags;
}
//---------------------------------------------------------------------------
inline const D3DCAPS9* NiDX9Renderer::GetDeviceCaps() const
{
    return &m_kD3DCaps9;
}
//---------------------------------------------------------------------------
inline const NiDX9AdapterDesc* NiDX9Renderer::GetAdapterDesc() const
{
    return m_pkAdapterDesc;
}
//---------------------------------------------------------------------------
inline const NiDX9DeviceDesc* NiDX9Renderer::GetDeviceDesc() const
{
    return m_pkDeviceDesc;
}
//---------------------------------------------------------------------------
inline const D3DMATRIX& NiDX9Renderer::GetD3DMat() const
{
    return m_kD3DMat;
}
//---------------------------------------------------------------------------
inline const D3DMATRIX& NiDX9Renderer::GetD3DView() const
{
    return m_kD3DView;
}
//---------------------------------------------------------------------------
inline const D3DMATRIX& NiDX9Renderer::GetD3DProj() const
{
    return m_kD3DProj;
}
//---------------------------------------------------------------------------
inline const D3DMATRIX& NiDX9Renderer::GetInvView() const
{
    return m_kInvView;
}
//---------------------------------------------------------------------------
inline const D3DXALIGNEDMATRIX* NiDX9Renderer::GetD3DWorld() const
{
    return &m_kD3DMat;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetResetCounter() const
{
    return m_uiResetCounter;
}
//---------------------------------------------------------------------------
// Shader version information
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetMinPixelShaderVersion(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetMinPixelShaderVersion(
        D3DPS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetPixelShaderVersionRequest(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetPixelShaderVersionRequest(
        D3DPS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetMinVertexShaderVersion(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetMinVertexShaderVersion(
        D3DVS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetVertexShaderVersionRequest(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetVertexShaderVersionRequest(
        D3DVS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetSystemUserVersion(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetSystemUserVersion(
        D3DVS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetMinUserVersion(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetMinUserVersion(D3DVS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetUserVersionRequest(unsigned int uiMaj, 
    unsigned int uiMin)
{
    m_kShaderLibraryVersion.SetUserVersionRequest(
        D3DVS_VERSION(uiMaj, uiMin));
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMinPixelShaderVersion() const
{
    return m_kShaderLibraryVersion.GetMinPixelShaderVersion();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetPixelShaderVersionRequest() const
{
    return m_kShaderLibraryVersion.GetPixelShaderVersionRequest();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMinVertexShaderVersion() const
{
    return m_kShaderLibraryVersion.GetMinVertexShaderVersion();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetVertexShaderVersionRequest() const
{
    return m_kShaderLibraryVersion.GetVertexShaderVersionRequest();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMinUserVersion() const
{
    return m_kShaderLibraryVersion.GetMinUserVersion();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetUserVersionRequest() const
{
    return m_kShaderLibraryVersion.GetUserVersionRequest();
}
//---------------------------------------------------------------------------
inline const NiD3DShaderLibraryVersion* 
    NiDX9Renderer::GetShaderLibraryVersion() const
{
    return &m_kShaderLibraryVersion;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMaxTextureWidth() const
{
    return m_kD3DCaps9.MaxTextureWidth;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMaxTextureHeight() const
{
    return m_kD3DCaps9.MaxTextureHeight;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetMaxTextureAspectRatio() const
{
    return m_kD3DCaps9.MaxTextureAspectRatio;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::IsDeviceLost() const
{
    return m_bDeviceLost;
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::AddDisplayFormat(D3DFORMAT eFormat)
{
    if (ms_kDisplayFormats.FindPos((unsigned int)eFormat))
        return;
    ms_kDisplayFormats.AddTail((unsigned int)eFormat);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::RemoveDisplayFormat(D3DFORMAT eFormat)
{
    ms_kDisplayFormats.Remove((unsigned int)eFormat);
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::AddResetNotificationFunc(
    RESETNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiReturn = m_kResetNotifyFuncs.Add(pfnNotify);
    m_kResetNotifyFuncData.SetAtGrow(uiReturn, pvData);

    return uiReturn;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::RemoveResetNotificationFunc(
    RESETNOTIFYFUNC pfnNotify)
{
    unsigned int uiIndex = FindResetNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    m_kResetNotifyFuncs.RemoveAt(uiIndex);
    m_kResetNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::RemoveResetNotificationFunc(unsigned int uiIndex)
{
    if (m_kResetNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kResetNotifyFuncs.RemoveAt(uiIndex);
    m_kResetNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::RemoveAllResetNotificationFuncs()
{
    m_kResetNotifyFuncs.RemoveAll();
    m_kResetNotifyFuncData.RemoveAll();
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::ChangeResetNotificationFuncData(
    RESETNOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiIndex = FindResetNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    NIASSERT(m_kResetNotifyFuncData.GetSize() > uiIndex);
    m_kResetNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::ChangeResetNotificationFuncData(
    unsigned int uiIndex, void* pvData)
{
    if (m_kResetNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    NIASSERT(m_kResetNotifyFuncData.GetSize() > uiIndex);
    m_kResetNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetResetNotificationFuncCount() const
{
    return m_kResetNotifyFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetResetNotificationFuncArrayCount() const
{
    return m_kResetNotifyFuncs.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::FindResetNotificationFunc(
    RESETNOTIFYFUNC pfnNotify) const
{
    for (unsigned int i = 0; i < m_kResetNotifyFuncs.GetSize(); i++)
    {
        if (m_kResetNotifyFuncs.GetAt(i) == pfnNotify)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::RESETNOTIFYFUNC NiDX9Renderer::GetResetNotificationFunc(
    unsigned int uiIndex) const
{
    return m_kResetNotifyFuncs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::AddLostDeviceNotificationFunc(
    LOSTDEVICENOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiReturn = m_kLostDeviceNotifyFuncs.Add(pfnNotify);
    m_kLostDeviceNotifyFuncData.SetAtGrow(uiReturn, pvData);

    return uiReturn;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::RemoveLostDeviceNotificationFunc(
    LOSTDEVICENOTIFYFUNC pfnNotify)
{
    unsigned int uiIndex = FindLostDeviceNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    m_kLostDeviceNotifyFuncs.RemoveAt(uiIndex);
    m_kLostDeviceNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::RemoveLostDeviceNotificationFunc(
    unsigned int uiIndex)
{
    if (m_kLostDeviceNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    m_kLostDeviceNotifyFuncs.RemoveAt(uiIndex);
    m_kLostDeviceNotifyFuncData.RemoveAt(uiIndex);
    return true;
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::RemoveAllLostDeviceNotificationFuncs()
{
    m_kLostDeviceNotifyFuncs.RemoveAll();
    m_kLostDeviceNotifyFuncData.RemoveAll();
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::ChangeLostDeviceNotificationFuncData(
    LOSTDEVICENOTIFYFUNC pfnNotify, void* pvData)
{
    unsigned int uiIndex = FindLostDeviceNotificationFunc(pfnNotify);
    if (uiIndex == UINT_MAX)
        return false;

    NIASSERT(m_kLostDeviceNotifyFuncData.GetSize() > uiIndex);
    m_kLostDeviceNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::ChangeLostDeviceNotificationFuncData(
    unsigned int uiIndex, void* pvData)
{
    if (m_kLostDeviceNotifyFuncs.GetAt(uiIndex) == 0)
        return false;

    NIASSERT(m_kLostDeviceNotifyFuncData.GetSize() > uiIndex);
    m_kLostDeviceNotifyFuncData.SetAt(uiIndex, pvData);
    return true;
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetLostDeviceNotificationFuncCount() const
{
    return m_kLostDeviceNotifyFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::GetLostDeviceNotificationFuncArrayCount() 
    const
{
    return m_kLostDeviceNotifyFuncs.GetSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiDX9Renderer::FindLostDeviceNotificationFunc(
    LOSTDEVICENOTIFYFUNC pfnNotify) const
{
    for (unsigned int i = 0; i < m_kLostDeviceNotifyFuncs.GetSize(); i++)
    {
        if (m_kLostDeviceNotifyFuncs.GetAt(i) == pfnNotify)
            return i;
    }
    return UINT_MAX;
}
//---------------------------------------------------------------------------
inline NiDX9Renderer::LOSTDEVICENOTIFYFUNC
    NiDX9Renderer::GetLostDeviceNotificationFunc(unsigned int uiIndex) const
{
    return m_kLostDeviceNotifyFuncs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseResource(D3DResourcePtr pkResource)
{
    ReleaseIUnknown(pkResource, D3DRT_GENERIC);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseVBResource(D3DVertexBufferPtr pkResource)
{
#if NIMETRICS
    D3DVERTEXBUFFER_DESC kDesc;
    pkResource->GetDesc(&kDesc);
#endif

#if NIMETRICS
    ULONG ulRet =
#endif
        ReleaseIUnknown(pkResource, D3DRT_VERTEXBUFFER);

#if NIMETRICS
    // check reference count to see if buffer memory was released
    if (!ulRet)
    {
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_BUFFER_SIZE, 
            -(int)kDesc.Size);
    }
#endif
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseIBResource(D3DIndexBufferPtr pkResource)
{
#if NIMETRICS
    D3DINDEXBUFFER_DESC kDesc;
    pkResource->GetDesc(&kDesc);
#endif

#if NIMETRICS
    ULONG ulRet = 
#endif
        ReleaseIUnknown(pkResource, D3DRT_INDEXBUFFER);

#if NIMETRICS
    // check reference count to see if buffer memory was released
    if (!ulRet)
    {
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(INDEX_BUFFER_SIZE,
            -(int)kDesc.Size);
    }
#endif
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseTextureResource(D3DTexturePtr pkResource)
{
    ReleaseIUnknown(pkResource, D3DRT_TEXTURE);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseSwapChain(LPDIRECT3DSWAPCHAIN9 pkSwapChain)
{
    ReleaseIUnknown(pkSwapChain, D3DRT_SWAPCHAIN);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseDevice(D3DDevicePtr pkDevice)
{
    ReleaseIUnknown(pkDevice, D3DRT_DEVICE);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseVertexDecl(
    NiD3DVertexDeclaration pkVertDecl)
{
    ReleaseIUnknown(pkVertDecl, D3DRT_VERTEXDECL);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::SetRenderShadowCasterBackfaces(
    bool bRenderBackfaces)
{
    NiDX9ShadowWriteShader::SetRenderBackfaces(bRenderBackfaces);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::AddD3DDefaultBuffer(
    NiDX9DataStream* pkDataStream)
{
    m_kD3DDefaultBuffersLock.Lock();
    m_kD3DDefaultBuffers.AddTail(pkDataStream);
    m_kD3DDefaultBuffersLock.Unlock();
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::RemoveD3DDefaultBuffer(
    NiDX9DataStream* pkDataStream)
{
    m_kD3DDefaultBuffersLock.Lock();
    m_kD3DDefaultBuffers.Remove(pkDataStream);
    m_kD3DDefaultBuffersLock.Unlock();
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::AquireSharedBufferLock()
{
    m_kSharedBufferLock.Lock();
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::ReleaseSharedBufferLock()
{
    m_kSharedBufferLock.Unlock();
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::AddSharedVertexBuffer(
    unsigned int uiStride,
    NiDX9DataStream::NiDX9BufferInfo* pkBufferInfo)
{
    m_kSharedDynamicVertexBuffers.SetAt(uiStride, pkBufferInfo);
}
//---------------------------------------------------------------------------
inline NiDX9DataStream::NiDX9BufferInfo* NiDX9Renderer::GetSharedVertexBuffer(
    unsigned int uiStride)
{
    NiDX9DataStream::NiDX9BufferInfo* pkRetBuffer = NULL;
    m_kSharedDynamicVertexBuffers.GetAt(uiStride, pkRetBuffer);

    return pkRetBuffer;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::RemoveSharedVertexBuffer(unsigned int uiStride)
{
    return m_kSharedDynamicVertexBuffers.RemoveAt(uiStride);
}
//---------------------------------------------------------------------------
inline void NiDX9Renderer::AddSharedIndexBuffer(
    unsigned int uiStride,
    NiDX9DataStream::NiDX9BufferInfo* pkBufferInfo)
{
    m_kSharedDynamicIndexBuffers.SetAt(uiStride, pkBufferInfo);
}
//---------------------------------------------------------------------------
inline NiDX9DataStream::NiDX9BufferInfo* NiDX9Renderer::GetSharedIndexBuffer(
    unsigned int uiStride)
{
    NiDX9DataStream::NiDX9BufferInfo* pkRetBuffer = NULL;
    m_kSharedDynamicIndexBuffers.GetAt(uiStride, pkRetBuffer);

    return pkRetBuffer;
}
//---------------------------------------------------------------------------
inline bool NiDX9Renderer::RemoveSharedIndexBuffer(unsigned int uiStride)
{
    return m_kSharedDynamicIndexBuffers.RemoveAt(uiStride);
}
//---------------------------------------------------------------------------
inline unsigned long NiDX9Renderer::GetDeviceThreadID() const
{
    return m_ulDeviceThreadID;
}
//---------------------------------------------------------------------------
