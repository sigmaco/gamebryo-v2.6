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
inline D3D10_DRIVER_TYPE NiD3D10Renderer::GetD3D10DriverType(DriverType eType)
    const
{
    return (D3D10_DRIVER_TYPE)eType;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Renderer::GetD3D10CreateDeviceFlags(
    unsigned int uiCreateDeviceFlags) const
{
    return uiCreateDeviceFlags;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Renderer::GetD3D10WindowAssociationFlags(
    unsigned int uiWindowAssociationFlags) const
{
    return uiWindowAssociationFlags;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::SetRenderShadowCasterBackfaces(
    bool bRenderBackfaces)
{
    NiD3D10ShadowWriteShader::SetRenderBackfaces(bRenderBackfaces);
}
//---------------------------------------------------------------------------
inline const NiD3D10ShaderLibraryVersion* 
    NiD3D10Renderer::GetShaderLibraryVersion() const
{
    return &m_kShaderLibraryVersion;
}
//---------------------------------------------------------------------------
inline unsigned long NiD3D10Renderer::GetDeviceThreadID() const
{
    return m_ulDeviceThreadID;
}
//---------------------------------------------------------------------------
inline bool NiD3D10Renderer::SupportsMMX()
{
    return m_bSupportsMMX;
}
//---------------------------------------------------------------------------
inline bool NiD3D10Renderer::SupportsSSE()
{
    return m_bSupportsSSE;
}
//---------------------------------------------------------------------------
inline bool NiD3D10Renderer::SupportsSSE2()
{
    return m_bSupportsSSE2;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::
    GetCreationParameters(CreationParameters& kCreate) const
{
    kCreate = m_kInitialCreateParameters;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::RemoveAllDeviceRemovedNotificationFuncs()
{
    m_kDeviceRemovedNotifyFuncs.RemoveAll();
    m_kDeviceRemovedNotifyFuncData.RemoveAll();
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Renderer::
        GetDeviceRemovedNotificationFuncCount() const
{
    return m_kDeviceRemovedNotifyFuncs.GetEffectiveSize();
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Renderer::
        GetDeviceRemovedNotificationFuncArrayCount() const
{
    return m_kDeviceRemovedNotifyFuncs.GetSize();
}
//---------------------------------------------------------------------------
inline NiD3D10Renderer::DEVICEREMOVEDNOTIFYFUNC 
    NiD3D10Renderer::GetDeviceRemovedNotificationFunc(unsigned int uiIndex)
    const
{
    return m_kDeviceRemovedNotifyFuncs.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
inline bool NiD3D10Renderer::DoesFormatSupportFlag(DXGI_FORMAT eFormat, 
    unsigned int uiFlags) const
{
    return (m_auiFormatSupport[(unsigned int)eFormat] & uiFlags) != 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Renderer::GetFormatSupportFlags(
    DXGI_FORMAT eFormat) const
{
    return m_auiFormatSupport[(unsigned int)eFormat];
}
//---------------------------------------------------------------------------
inline unsigned int NiD3D10Renderer::GetSyncInterval() const
{
    return m_uiSyncInterval;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::SetSyncInterval(unsigned int uiSyncInterval)
{
    m_uiSyncInterval = uiSyncInterval;
}
//---------------------------------------------------------------------------
inline NiD3D10DeviceState* NiD3D10Renderer::GetDeviceState() const
{
    return m_pkDeviceState;
}
//---------------------------------------------------------------------------
inline NiD3D10RenderStateManager* NiD3D10Renderer::
    GetRenderStateManager() const
{
    return m_pkRenderStateManager;
}
//---------------------------------------------------------------------------
inline NiD3D10ResourceManager* NiD3D10Renderer::GetResourceManager() const
{
    return m_pkResourceManager;
}
//---------------------------------------------------------------------------
inline NiD3D10ShaderConstantManager* NiD3D10Renderer::
    GetShaderConstantManager() const
{
    return m_pkShaderConstantManager;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::InvalidateDeviceState()
{
    if (m_pkDeviceState)
        m_pkDeviceState->InvalidateDeviceState();
}
//---------------------------------------------------------------------------
inline ID3D10Device* NiD3D10Renderer::GetD3D10Device() const
{
    return m_pkD3D10Device;
}
//---------------------------------------------------------------------------
inline bool NiD3D10Renderer::ReleaseResources()
{
    m_spDefaultRenderTargetGroup = NULL;
    return true;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::RegisterD3D10Shader(
    NiD3D10ShaderInterface* pkD3D10Shader)
{
    m_kD3D10Shaders.AddHead(pkD3D10Shader);
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::ReleaseD3D10Shader(
    NiD3D10ShaderInterface* pkD3D10Shader)
{
    m_kD3D10Shaders.Remove(pkD3D10Shader);
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::PurgeD3D10Shader(
    NiD3D10ShaderInterface* pkD3D10Shader)
{
    pkD3D10Shader->DestroyRendererData();

    m_kD3D10Shaders.Remove(pkD3D10Shader);
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::PurgeAllD3D10Shaders()
{
    while (!m_kD3D10Shaders.IsEmpty())
        PurgeD3D10Shader(m_kD3D10Shaders.GetHead());
}
//---------------------------------------------------------------------------
inline const D3DXMATRIXA16& NiD3D10Renderer::GetProjectionMatrix() const
{
    return m_kD3DProj;
}
//---------------------------------------------------------------------------
inline const D3DXMATRIXA16& NiD3D10Renderer::GetViewMatrix() const
{
    return m_kD3DView;
}
//---------------------------------------------------------------------------
inline const D3DXMATRIXA16& NiD3D10Renderer::GetInverseViewMatrix() const
{
    return m_kInvView;
}
//---------------------------------------------------------------------------
inline const D3DXMATRIXA16& NiD3D10Renderer::GetWorldMatrix() const
{
    return m_kD3DModel;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::GetCameraNearAndFar(float& fNear, 
    float& fFar) const
{
    fNear = m_kCurrentFrustum.m_fNear;
    fFar = m_kCurrentFrustum.m_fFar;
}
//---------------------------------------------------------------------------
inline void NiD3D10Renderer::SetModelTransform(const NiTransform& kWorld)
{
    NiD3D10Utility::GetD3DFromNi(m_kD3DModel, kWorld);
}
//---------------------------------------------------------------------------
