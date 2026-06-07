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

//---------------------------------------------------------------------------
inline bool NiD3D10RenderStateManager::GetLeftRightSwap() const
{
    return m_bLeftRightSwap;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetLeftRightSwap(bool bSwap)
{
    m_bLeftRightSwap = bSwap;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetBlendStateDesc(
    D3D10_BLEND_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kCurrentBlendDesc;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetBlendFactor(
	const float afBlendFactor[4])
{
    m_afCurrentBlendFactor[0] = afBlendFactor[0];
    m_afCurrentBlendFactor[1] = afBlendFactor[1];
    m_afCurrentBlendFactor[2] = afBlendFactor[2];
    m_afCurrentBlendFactor[3] = afBlendFactor[3];
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetBlendFactor(
    float afBlendFactor[4]) const
{
    afBlendFactor[0] = m_afCurrentBlendFactor[0];
    afBlendFactor[1] = m_afCurrentBlendFactor[1];
    afBlendFactor[2] = m_afCurrentBlendFactor[2];
    afBlendFactor[3] = m_afCurrentBlendFactor[3];
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetSampleMask(NiUInt32 uiSampleMask)
{
    m_uiCurrentSampleMask = uiSampleMask;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetSampleMask(
	NiUInt32& uiSampleMask) const
{
    uiSampleMask = m_uiCurrentSampleMask;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDepthStencilStateDesc(
    D3D10_DEPTH_STENCIL_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kCurrentDepthStencilDesc;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetStencilRef(NiUInt32 uiStencilRef)
{
    m_uiCurrentStencilRef = uiStencilRef;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetStencilRef(
	NiUInt32& uiStencilRef) const
{
    uiStencilRef = m_uiCurrentStencilRef;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetRasterizerStateDesc(
    D3D10_RASTERIZER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kCurrentRasterizerDesc;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetSamplerDesc(
    NiGPUProgram::ProgramType eType, NiUInt32 uiSampler, 
    D3D10_SAMPLER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        kDesc = m_aakCurrentSamplerDescs[(NiUInt32)eType][uiSampler];
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetDefaultBlendStateDesc(
    const D3D10_BLEND_DESC& kDesc)
{
    m_kDefaultBlendDesc = kDesc;
    UpdateDefaultBlendStateObject();
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultBlendStateDesc(
    D3D10_BLEND_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kDefaultBlendDesc;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetDefaultBlendFactor(
    const float afBlendFactor[4])
{
    m_afDefaultBlendFactor[0] = afBlendFactor[0];
    m_afDefaultBlendFactor[1] = afBlendFactor[1];
    m_afDefaultBlendFactor[2] = afBlendFactor[2];
    m_afDefaultBlendFactor[3] = afBlendFactor[3];
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultBlendFactor(
    float afBlendFactor[4]) const
{
    afBlendFactor[0] = m_afDefaultBlendFactor[0];
    afBlendFactor[1] = m_afDefaultBlendFactor[1];
    afBlendFactor[2] = m_afDefaultBlendFactor[2];
    afBlendFactor[3] = m_afDefaultBlendFactor[3];
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetDefaultSampleMask(
    NiUInt32 uiSampleMask)
{
    m_uiDefaultSampleMask = uiSampleMask;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultSampleMask(
    NiUInt32& uiSampleMask) const
{
    uiSampleMask = m_uiDefaultSampleMask;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetDefaultDepthStencilStateDesc(
    const D3D10_DEPTH_STENCIL_DESC& kDesc)
{
    m_kDefaultDepthStencilDesc = kDesc;
    UpdateDefaultDepthStencilStateObject();
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultDepthStencilStateDesc(
    D3D10_DEPTH_STENCIL_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kDefaultDepthStencilDesc;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetDefaultStencilRef(
    NiUInt32 uiStenciRef)
{
    m_uiDefaultStencilRef = uiStenciRef;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultStencilRef(
    NiUInt32& uiStencilRef) const
{
    uiStencilRef = m_uiDefaultStencilRef;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::SetDefaultRasterizerStateDesc(
    const D3D10_RASTERIZER_DESC& kDesc)
{
    m_kDefaultRasterizerDesc = kDesc;
    UpdateDefaultRasterizerStateObject();
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultRasterizerStateDesc(
    D3D10_RASTERIZER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kDefaultRasterizerDesc;
}
//---------------------------------------------------------------------------
inline void NiD3D10RenderStateManager::GetDefaultSamplerDesc(
    NiGPUProgram::ProgramType eType, NiUInt32 uiSampler, 
    D3D10_SAMPLER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        kDesc = m_aakDefaultSamplerDescs[(NiUInt32)eType][uiSampler];
}
