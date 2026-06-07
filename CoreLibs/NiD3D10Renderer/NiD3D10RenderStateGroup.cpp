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

#include "NiD3D10RenderStateGroup.h"
#include "NiD3D10RenderStateManager.h"

//---------------------------------------------------------------------------
NiD3D10RenderStateGroup::NiD3D10RenderStateGroup() :
    m_uiBlendValidFlags(0),
    m_uiDepthStencilValidFlags(0),
    m_uiRasterizerValidFlags(0),
    m_bBlendFactorValid(false),
    m_bSampleMaskValid(false),
    m_bStencilRefValid(false)
{
    ResetRenderStates();
}
//---------------------------------------------------------------------------
NiD3D10RenderStateGroup::~NiD3D10RenderStateGroup()
{
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSAlphaToCoverageEnable(
    bool bAlphaToCoverageEnable)
{
    m_kBlendDesc.AlphaToCoverageEnable = bAlphaToCoverageEnable;
    m_uiBlendValidFlags |= 
        NiD3D10RenderStateManager::BSVALID_ALPHATOCOVERAGEENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSAlphaToCoverageEnable(
    bool& bAlphaToCoverageEnable) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_ALPHATOCOVERAGEENABLE) != 0)
    {
        bAlphaToCoverageEnable = (m_kBlendDesc.AlphaToCoverageEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSAlphaToCoverageEnable()
{
    m_uiBlendValidFlags &= 
        ~NiD3D10RenderStateManager::BSVALID_ALPHATOCOVERAGEENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSBlendEnable(unsigned int uiRenderTarget, 
    bool bBlendEnable)
{
    if (uiRenderTarget < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT)
    {
        m_kBlendDesc.BlendEnable[uiRenderTarget] = bBlendEnable;
        m_uiBlendValidFlags |= 
            (NiD3D10RenderStateManager::BSVALID_BLENDENABLE_0 << 
            uiRenderTarget);
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSBlendEnable(unsigned int uiRenderTarget, 
    bool& bBlendEnable) const
{
    if (uiRenderTarget < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT &&
        (m_uiBlendValidFlags & 
        (NiD3D10RenderStateManager::BSVALID_BLENDENABLE_0 << uiRenderTarget))
        != 0)
    {
        bBlendEnable = (m_kBlendDesc.BlendEnable[uiRenderTarget] != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSBlendEnable(unsigned int uiRenderTarget)
{
    if (uiRenderTarget < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT)
    {
        m_uiBlendValidFlags &=
            ~(NiD3D10RenderStateManager::BSVALID_BLENDENABLE_0 << 
            uiRenderTarget);
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSSrcBlend(D3D10_BLEND eBlend)
{
    m_kBlendDesc.SrcBlend = eBlend;
    m_uiBlendValidFlags |= NiD3D10RenderStateManager::BSVALID_SRCBLEND;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSSrcBlend(D3D10_BLEND& eBlend) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_SRCBLEND) != 0)
    {
        eBlend = m_kBlendDesc.SrcBlend;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSSrcBlend()
{
    m_uiBlendValidFlags &= ~NiD3D10RenderStateManager::BSVALID_SRCBLEND;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSDestBlend(D3D10_BLEND eBlend)
{
    m_kBlendDesc.DestBlend = eBlend;
    m_uiBlendValidFlags |= NiD3D10RenderStateManager::BSVALID_DESTBLEND;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSDestBlend(D3D10_BLEND& eBlend) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_DESTBLEND) != 0)
    {
        eBlend = m_kBlendDesc.DestBlend;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSDestBlend()
{
    m_uiBlendValidFlags &= ~NiD3D10RenderStateManager::BSVALID_DESTBLEND;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSBlendOp(D3D10_BLEND_OP eBlendOp)
{
    m_kBlendDesc.BlendOp = eBlendOp;
    m_uiBlendValidFlags |= NiD3D10RenderStateManager::BSVALID_BLENDOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSBlendOp(D3D10_BLEND_OP& eBlendOp) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_BLENDOP) != 0)
    {
        eBlendOp = m_kBlendDesc.BlendOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSBlendOp()
{
    m_uiBlendValidFlags &= ~NiD3D10RenderStateManager::BSVALID_BLENDOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSSrcBlendAlpha(D3D10_BLEND eBlend)
{
    m_kBlendDesc.SrcBlendAlpha = eBlend;
    m_uiBlendValidFlags |= NiD3D10RenderStateManager::BSVALID_SRCBLENDALPHA;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSSrcBlendAlpha(D3D10_BLEND& eBlend) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_SRCBLENDALPHA) != 0)
    {
        eBlend = m_kBlendDesc.SrcBlendAlpha;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSSrcBlendAlpha()
{
    m_uiBlendValidFlags &= ~NiD3D10RenderStateManager::BSVALID_SRCBLENDALPHA;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSDestBlendAlpha(D3D10_BLEND eBlend)
{
    m_kBlendDesc.DestBlendAlpha = eBlend;
    m_uiBlendValidFlags |= NiD3D10RenderStateManager::BSVALID_DESTBLENDALPHA;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSDestBlendAlpha(D3D10_BLEND& eBlend) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_DESTBLENDALPHA) != 0)
    {
        eBlend = m_kBlendDesc.DestBlendAlpha;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSDestBlendAlpha()
{
    m_uiBlendValidFlags &= ~NiD3D10RenderStateManager::BSVALID_DESTBLENDALPHA;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSBlendOpAlpha(D3D10_BLEND_OP eBlendOp)
{
    m_kBlendDesc.BlendOpAlpha = eBlendOp;
    m_uiBlendValidFlags |= NiD3D10RenderStateManager::BSVALID_BLENDOPALPHA;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSBlendOpAlpha(D3D10_BLEND_OP& eBlendOp) const
{
    if ((m_uiBlendValidFlags & 
        NiD3D10RenderStateManager::BSVALID_BLENDOPALPHA) != 0)
    {
        eBlendOp = m_kBlendDesc.BlendOpAlpha;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSBlendOpAlpha()
{
    m_uiBlendValidFlags &= ~NiD3D10RenderStateManager::BSVALID_BLENDOPALPHA;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBSRenderTargetWriteMask(
    unsigned int uiRenderTarget, unsigned char ucWriteMask)
{
    if (uiRenderTarget < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT)
    {
        m_kBlendDesc.RenderTargetWriteMask[uiRenderTarget] = ucWriteMask;
        m_uiBlendValidFlags |= 
            (NiD3D10RenderStateManager::BSVALID_RENDERTARGETWRITEMASK_0 << 
            uiRenderTarget);
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBSRenderTargetWriteMask(
    unsigned int uiRenderTarget, unsigned char& ucWriteMask) const
{
    if (uiRenderTarget < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT &&
        (m_uiBlendValidFlags & 
        (NiD3D10RenderStateManager::BSVALID_RENDERTARGETWRITEMASK_0 << 
        uiRenderTarget)) != 0)
    {
        ucWriteMask = m_kBlendDesc.RenderTargetWriteMask[uiRenderTarget];
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBSRenderTargetWriteMask(
    unsigned int uiRenderTarget)
{
    if (uiRenderTarget < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT)
    {
        m_uiBlendValidFlags &=
            ~(NiD3D10RenderStateManager::BSVALID_RENDERTARGETWRITEMASK_0 << 
            uiRenderTarget);
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBlendFactor(const float afBlendFactor[4])
{
    for (unsigned int i = 0; i < 4; i++)
        m_afBlendFactor[i] = afBlendFactor[i];
    m_bBlendFactorValid = true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetBlendFactor(float afBlendFactor[4]) const
{
    if (m_bBlendFactorValid)
    {
        for (unsigned int i = 0; i < 4; i++)
            afBlendFactor[i] = m_afBlendFactor[i];
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveBlendFactor()
{
    m_bBlendFactorValid = false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSampleMask(unsigned int uiSampleMask)
{
    m_uiSampleMask = uiSampleMask;
    m_bSampleMaskValid = true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSampleMask(unsigned int& uiSampleMask) const
{
    if (m_bSampleMaskValid)
    {
        uiSampleMask = m_uiSampleMask;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSampleMask()
{
    m_bSampleMaskValid = false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSDepthEnable(bool bDepthEnable)
{
    m_kDepthStencilDesc.DepthEnable = bDepthEnable;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_DEPTHENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSDepthEnable(bool& bDepthEnable) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_DEPTHENABLE) != 0)
    {
        bDepthEnable = (m_kDepthStencilDesc.DepthEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSDepthEnable()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_DEPTHENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSDepthWriteMask(
    D3D10_DEPTH_WRITE_MASK eDepthWriteMask)
{
    m_kDepthStencilDesc.DepthWriteMask = eDepthWriteMask;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_DEPTHWRITEMASK;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSDepthWriteMask(
    D3D10_DEPTH_WRITE_MASK& eDepthWriteMask) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_DEPTHWRITEMASK) != 0)
    {
        eDepthWriteMask = m_kDepthStencilDesc.DepthWriteMask;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSDepthWriteMask()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_DEPTHWRITEMASK;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSDepthFunc(D3D10_COMPARISON_FUNC eDepthFunc)
{
    m_kDepthStencilDesc.DepthFunc = eDepthFunc;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_DEPTHFUNC;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSDepthFunc(
    D3D10_COMPARISON_FUNC& eDepthFunc) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_DEPTHFUNC) != 0)
    {
        eDepthFunc = m_kDepthStencilDesc.DepthFunc;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSDepthFunc()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_DEPTHFUNC;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSStencilEnable(bool bStencilEnable)
{
    m_kDepthStencilDesc.StencilEnable = bStencilEnable;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_STENCILENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSStencilEnable(bool& bStencilEnable) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_STENCILENABLE) != 0)
    {
        bStencilEnable = (m_kDepthStencilDesc.StencilEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSStencilEnable()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_STENCILENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSStencilReadMask(
    unsigned int uiStencilReadMask)
{
    m_kDepthStencilDesc.StencilReadMask = (UINT8)uiStencilReadMask;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_STENCILREADMASK;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSStencilReadMask(
    unsigned int& uiStencilReadMask) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_STENCILREADMASK) != 0)
    {
        uiStencilReadMask = m_kDepthStencilDesc.StencilReadMask;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSStencilReadMask()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_STENCILREADMASK;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSStencilWriteMask(
    unsigned int uiStencilWriteMask)
{
    m_kDepthStencilDesc.StencilWriteMask = (UINT8)uiStencilWriteMask;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_STENCILWRITEMASK;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSStencilWriteMask(
    unsigned int& uiStencilWriteMask) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_STENCILWRITEMASK) != 0)
    {
        uiStencilWriteMask = m_kDepthStencilDesc.StencilWriteMask;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSStencilWriteMask()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_STENCILWRITEMASK;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSFrontFaceStencilFailOp(
    D3D10_STENCIL_OP eStencilOp)
{
    m_kDepthStencilDesc.FrontFace.StencilFailOp = eStencilOp;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILFAILOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSFrontFaceStencilFailOp(
    D3D10_STENCIL_OP& eStencilOp) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILFAILOP) != 0)
    {
        eStencilOp = m_kDepthStencilDesc.FrontFace.StencilFailOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSFrontFaceStencilFailOp()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILFAILOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSFrontFaceStencilDepthFailOp(
    D3D10_STENCIL_OP eStencilOp)
{
    m_kDepthStencilDesc.FrontFace.StencilDepthFailOp = eStencilOp;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILDEPTHFAILOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSFrontFaceStencilDepthFailOp(
    D3D10_STENCIL_OP& eStencilOp) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILDEPTHFAILOP) != 0)
    {
        eStencilOp = m_kDepthStencilDesc.FrontFace.StencilDepthFailOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSFrontFaceStencilDepthFailOp()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILDEPTHFAILOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSFrontFaceStencilPassOp(
    D3D10_STENCIL_OP eStencilOp)
{
    m_kDepthStencilDesc.FrontFace.StencilPassOp = eStencilOp;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILPASSOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSFrontFaceStencilPassOp(
    D3D10_STENCIL_OP& eStencilOp) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILPASSOP) != 0)
    {
        eStencilOp = m_kDepthStencilDesc.FrontFace.StencilPassOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSFrontFaceStencilPassOp()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILPASSOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSFrontFaceStencilFunc(
    D3D10_COMPARISON_FUNC eStencilFunc)
{
    m_kDepthStencilDesc.FrontFace.StencilFunc = eStencilFunc;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILFUNC;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSFrontFaceStencilFunc(
    D3D10_COMPARISON_FUNC& eStencilFunc) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILFUNC) != 0)
    {
        eStencilFunc = m_kDepthStencilDesc.FrontFace.StencilFunc;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSFrontFaceStencilFunc()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_FRONTFACE_STENCILFUNC;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSBackFaceStencilFailOp(
    D3D10_STENCIL_OP eStencilOp)
{
    m_kDepthStencilDesc.BackFace.StencilFailOp = eStencilOp;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILFAILOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSBackFaceStencilFailOp(
    D3D10_STENCIL_OP& eStencilOp) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILFAILOP) != 0)
    {
        eStencilOp = m_kDepthStencilDesc.BackFace.StencilFailOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSBackFaceStencilFailOp()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILFAILOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSBackFaceStencilDepthFailOp(
    D3D10_STENCIL_OP eStencilOp)
{
    m_kDepthStencilDesc.BackFace.StencilDepthFailOp = eStencilOp;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILDEPTHFAILOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSBackFaceStencilDepthFailOp(
    D3D10_STENCIL_OP& eStencilOp) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILDEPTHFAILOP) != 0)
    {
        eStencilOp = m_kDepthStencilDesc.BackFace.StencilDepthFailOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSBackFaceStencilDepthFailOp()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILDEPTHFAILOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSBackFaceStencilPassOp(
    D3D10_STENCIL_OP eStencilOp)
{
    m_kDepthStencilDesc.BackFace.StencilPassOp = eStencilOp;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILPASSOP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSBackFaceStencilPassOp(
    D3D10_STENCIL_OP& eStencilOp) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILPASSOP) != 0)
    {
        eStencilOp = m_kDepthStencilDesc.BackFace.StencilPassOp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSBackFaceStencilPassOp()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILPASSOP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDSSBackFaceStencilFunc(
    D3D10_COMPARISON_FUNC eStencilFunc)
{
    m_kDepthStencilDesc.BackFace.StencilFunc = eStencilFunc;
    m_uiDepthStencilValidFlags |= 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILFUNC;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetDSSBackFaceStencilFunc(
    D3D10_COMPARISON_FUNC& eStencilFunc) const
{
    if ((m_uiDepthStencilValidFlags & 
        NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILFUNC) != 0)
    {
        eStencilFunc = m_kDepthStencilDesc.BackFace.StencilFunc;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveDSSBackFaceStencilFunc()
{
    m_uiDepthStencilValidFlags &= 
        ~NiD3D10RenderStateManager::DSSVALID_BACKFACE_STENCILFUNC;
}

//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetStencilRef(unsigned int uiStencilRef)
{
    m_uiStencilRef = uiStencilRef;
    m_bStencilRefValid = true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetStencilRef(unsigned int& uiStencilRef) const
{
    if (m_bStencilRefValid)
    {
        uiStencilRef = m_uiStencilRef;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveStencilRef()
{
    m_bStencilRefValid = false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSFillMode(D3D10_FILL_MODE eFillMode)
{
    m_kRasterizerDesc.FillMode = eFillMode;
    m_uiRasterizerValidFlags |= NiD3D10RenderStateManager::RSVALID_FILLMODE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSFillMode(D3D10_FILL_MODE& eFillMode) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_FILLMODE) != 0)
    {
        eFillMode = m_kRasterizerDesc.FillMode;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSFillMode()
{
    m_uiRasterizerValidFlags &= ~NiD3D10RenderStateManager::RSVALID_FILLMODE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSCullMode(D3D10_CULL_MODE eCullMode)
{
    m_kRasterizerDesc.CullMode = eCullMode;
    m_uiRasterizerValidFlags |= NiD3D10RenderStateManager::RSVALID_CULLMODE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSCullMode(D3D10_CULL_MODE& eCullMode) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_CULLMODE) != 0)
    {
        eCullMode = m_kRasterizerDesc.CullMode;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSCullMode()
{
    m_uiRasterizerValidFlags &= ~NiD3D10RenderStateManager::RSVALID_CULLMODE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSFrontCounterClockwise(
    bool bFrontCounterClockwise)
{
    m_kRasterizerDesc.FrontCounterClockwise = bFrontCounterClockwise;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_FRONTCOUNTERCLOCKWISE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSFrontCounterClockwise(
    bool& bFrontCounterClockwise) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_FRONTCOUNTERCLOCKWISE) != 0)
    {
        bFrontCounterClockwise = 
            (m_kRasterizerDesc.FrontCounterClockwise != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSFrontCounterClockwise()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_FRONTCOUNTERCLOCKWISE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSDepthBias(int iDepthBias)
{
    m_kRasterizerDesc.DepthBias = iDepthBias;
    m_uiRasterizerValidFlags |= NiD3D10RenderStateManager::RSVALID_DEPTHBIAS;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSDepthBias(int& iDepthBias) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_DEPTHBIAS) != 0)
    {
        iDepthBias = m_kRasterizerDesc.DepthBias;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSDepthBias()
{
    m_uiRasterizerValidFlags &= ~NiD3D10RenderStateManager::RSVALID_DEPTHBIAS;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSDepthBiasClamp(float fDepthBiasClamp)
{
    m_kRasterizerDesc.DepthBiasClamp = fDepthBiasClamp;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_DEPTHBIASCLAMP;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSDepthBiasClamp(float& fDepthBiasClamp) 
    const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_DEPTHBIASCLAMP) != 0)
    {
        fDepthBiasClamp = m_kRasterizerDesc.DepthBiasClamp;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSDepthBiasClamp()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_DEPTHBIASCLAMP;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSSlopeScaledDepthBias(
    float fSlopeScaledDepthBias)
{
    m_kRasterizerDesc.SlopeScaledDepthBias = fSlopeScaledDepthBias;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_SLOPESCALEDDEPTHBIAS;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSSlopeScaledDepthBias(
    float& fSlopeScaledDepthBias) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_SLOPESCALEDDEPTHBIAS) != 0)
    {
        fSlopeScaledDepthBias = m_kRasterizerDesc.SlopeScaledDepthBias;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSSlopeScaledDepthBias()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_SLOPESCALEDDEPTHBIAS;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSDepthClipEnable(bool bDepthClipEnable)
{
    m_kRasterizerDesc.DepthClipEnable = bDepthClipEnable;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_DEPTHCLIPENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSDepthClipEnable(bool& bDepthClipEnable) 
    const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_DEPTHCLIPENABLE) != 0)
    {
        bDepthClipEnable = (m_kRasterizerDesc.DepthClipEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSDepthClipEnable()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_DEPTHCLIPENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSScissorEnable(bool bScissorEnable)
{
    m_kRasterizerDesc.ScissorEnable = bScissorEnable;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_SCISSORENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSScissorEnable(bool& bScissorEnable) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_SCISSORENABLE) != 0)
    {
        bScissorEnable = (m_kRasterizerDesc.ScissorEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSScissorEnable()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_SCISSORENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSMultisampleEnable(bool bMultisampleEnable)
{
    m_kRasterizerDesc.MultisampleEnable= bMultisampleEnable;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_MULTISAMPLEENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSMultisampleEnable(bool& bMultisampleEnable) 
    const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_MULTISAMPLEENABLE) != 0)
    {
        bMultisampleEnable = (m_kRasterizerDesc.MultisampleEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSMultisampleEnable()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_MULTISAMPLEENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRSAntialiasedLineEnable(
    bool bAntialiasedLineEnable)
{
    m_kRasterizerDesc.AntialiasedLineEnable = bAntialiasedLineEnable;
    m_uiRasterizerValidFlags |= 
        NiD3D10RenderStateManager::RSVALID_ANTIALIASEDLINEENABLE;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetRSAntialiasedLineEnable(
    bool& bAntialiasedLineEnable) const
{
    if ((m_uiRasterizerValidFlags & 
        NiD3D10RenderStateManager::RSVALID_ANTIALIASEDLINEENABLE) != 0)
    {
        bAntialiasedLineEnable = 
            (m_kRasterizerDesc.AntialiasedLineEnable != 0);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveRSAntialiasedLineEnable()
{
    m_uiRasterizerValidFlags &= 
        ~NiD3D10RenderStateManager::RSVALID_ANTIALIASEDLINEENABLE;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerFilter(NiGPUProgram::ProgramType eType,
    unsigned int uiSampler, D3D10_FILTER eFilter)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].Filter = eFilter;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_FILTER;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerFilter(NiGPUProgram::ProgramType eType,
    unsigned int uiSampler, D3D10_FILTER& eFilter) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_FILTER) != 0)
    {
        eFilter = m_aakSamplerDescs[(unsigned int)eType][uiSampler].Filter;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerFilter(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_FILTER;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerAddressU(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_TEXTURE_ADDRESS_MODE eAddressU)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].AddressU = eAddressU;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_ADDRESSU;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerAddressU(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_TEXTURE_ADDRESS_MODE& eAddressU) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_ADDRESSU) != 0)
    {
        eAddressU = m_aakSamplerDescs[(unsigned int)eType][uiSampler].AddressU;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerAddressU(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_ADDRESSU;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerAddressV(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_TEXTURE_ADDRESS_MODE eAddressV)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].AddressV = eAddressV;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_ADDRESSV;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerAddressV(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_TEXTURE_ADDRESS_MODE& eAddressV) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_ADDRESSV) != 0)
    {
        eAddressV = m_aakSamplerDescs[(unsigned int)eType][uiSampler].AddressV;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerAddressV(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_ADDRESSV;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerAddressW(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_TEXTURE_ADDRESS_MODE eAddressW)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].AddressW = eAddressW;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_ADDRESSW;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerAddressW(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_TEXTURE_ADDRESS_MODE& eAddressW) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_ADDRESSW) != 0)
    {
        eAddressW = m_aakSamplerDescs[(unsigned int)eType][uiSampler].AddressW;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerAddressW(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_ADDRESSW;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerMipLODBias(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, float fMipLODBias)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].MipLODBias = 
            fMipLODBias;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_MIPLODBIAS;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerMipLODBias(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    float& fMipLODBias) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_MIPLODBIAS) != 0)
    {
        fMipLODBias = 
            m_aakSamplerDescs[(unsigned int)eType][uiSampler].MipLODBias;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerMipLODBias(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_MIPLODBIAS;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerMaxAnisotropy(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    unsigned int uiMaxAnisotropy)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].MaxAnisotropy = 
            uiMaxAnisotropy;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_MAXANISOTROPY;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerMaxAnisotropy(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    unsigned int& uiMaxAnisotropy) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_MAXANISOTROPY) != 0)
    {
        uiMaxAnisotropy = 
            m_aakSamplerDescs[(unsigned int)eType][uiSampler].MaxAnisotropy;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerMaxAnisotropy(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_MAXANISOTROPY;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerComparisonFunc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_COMPARISON_FUNC eComparisonFunc)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].ComparisonFunc = 
            eComparisonFunc;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_COMPARISONFUNC;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerComparisonFunc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_COMPARISON_FUNC& eComparisonFunc) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_COMPARISONFUNC) != 0)
    {
        eComparisonFunc = 
            m_aakSamplerDescs[(unsigned int)eType][uiSampler].ComparisonFunc;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerComparisonFunc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_COMPARISONFUNC;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerBorderColor(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    const float afBorderColor[4])
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            m_aakSamplerDescs[(unsigned int)eType][uiSampler].BorderColor[i] = 
                afBorderColor[i];
        }
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_BORDERCOLOR;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerBorderColor(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    float afBorderColor[4]) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_BORDERCOLOR) != 0)
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            afBorderColor[i] = 
                m_aakSamplerDescs[(unsigned int)eType][uiSampler].
                BorderColor[i];
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerBorderColor(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_BORDERCOLOR;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerMinLOD(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, float fMinLOD)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].MinLOD = fMinLOD;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_MINLOD;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerMinLOD(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, float& fMinLOD) 
    const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_MINLOD) != 0)
    {
        fMinLOD = m_aakSamplerDescs[(unsigned int)eType][uiSampler].MinLOD;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerMinLOD(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_MINLOD;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerMaxLOD(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, float fMaxLOD)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler].MaxLOD = fMaxLOD;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] |= 
            NiD3D10RenderStateManager::SVALID_MAXLOD;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateGroup::GetSamplerMaxLOD(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, float& fMaxLOD) 
    const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT &&
        (m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] & 
        NiD3D10RenderStateManager::SVALID_MAXLOD) != 0)
    {
        fMaxLOD = m_aakSamplerDescs[(unsigned int)eType][uiSampler].MaxLOD;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerMaxLOD(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] &=
            ~NiD3D10RenderStateManager::SVALID_MAXLOD;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerRemapping(
    NiGPUProgram::ProgramType eType, unsigned char* pucSamplerRemapping)
{
    if (pucSamplerRemapping)
    {
        memcpy(m_aaucSamplerRemapping[(unsigned int)eType], 
            pucSamplerRemapping, 
            sizeof(m_aaucSamplerRemapping[(unsigned int)eType]));
    }
    else
    {
        RemoveSamplerRemapping(eType);
    }
}
//---------------------------------------------------------------------------
const unsigned char* NiD3D10RenderStateGroup::GetSamplerRemapping(
    NiGPUProgram::ProgramType eType) const
{
    return m_aaucSamplerRemapping[(unsigned int)eType];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::RemoveSamplerRemapping(
    NiGPUProgram::ProgramType eType)
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
        m_aaucSamplerRemapping[(unsigned int)eType][i] = (unsigned char)i;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetBlendStateDesc(
    const D3D10_BLEND_DESC& kDesc, unsigned int uiValidFlags)
{
    m_kBlendDesc = kDesc;
    m_uiBlendValidFlags = uiValidFlags;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::GetBlendStateDesc(
    D3D10_BLEND_DESC& kDesc, unsigned int& uiValidFlags) const
{
    kDesc = m_kBlendDesc;
    uiValidFlags = m_uiBlendValidFlags;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetDepthStencilStateDesc(
    const D3D10_DEPTH_STENCIL_DESC& kDesc, unsigned int uiValidFlags)
{
    m_kDepthStencilDesc = kDesc;
    m_uiDepthStencilValidFlags = uiValidFlags;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::GetDepthStencilStateDesc(
    D3D10_DEPTH_STENCIL_DESC& kDesc, unsigned int& uiValidFlags) const
{
    kDesc = m_kDepthStencilDesc;
    uiValidFlags = m_uiDepthStencilValidFlags;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetRasterizerStateDesc(
    const D3D10_RASTERIZER_DESC& kDesc, unsigned int uiValidFlags)
{
    m_kRasterizerDesc = kDesc;
    m_uiRasterizerValidFlags = uiValidFlags;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::GetRasterizerStateDesc(
    D3D10_RASTERIZER_DESC& kDesc, unsigned int& uiValidFlags) const
{
    kDesc = m_kRasterizerDesc;
    uiValidFlags = m_uiRasterizerValidFlags;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::SetSamplerDesc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    const D3D10_SAMPLER_DESC& kDesc, unsigned int uiValidFlags)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        m_aakSamplerDescs[(unsigned int)eType][uiSampler] = kDesc;
        m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler] = uiValidFlags;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::GetSamplerDesc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_SAMPLER_DESC& kDesc, unsigned int& uiValidFlags) const
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
    {
        kDesc = m_aakSamplerDescs[(unsigned int)eType][uiSampler];
        uiValidFlags = m_aauiSamplerValidFlags[(unsigned int)eType][uiSampler];
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateGroup::ResetRenderStates()
{
    m_uiBlendValidFlags = 0;
    m_uiDepthStencilValidFlags = 0;
    m_uiRasterizerValidFlags = 0;
    m_bBlendFactorValid = false;
    m_bSampleMaskValid = false;
    m_bStencilRefValid = false;

    for (unsigned int i = 0; i < NiGPUProgram::PROGRAM_MAX; i++)
    {
        memset(m_aauiSamplerValidFlags[i], 0, 
            sizeof(m_aauiSamplerValidFlags[i]));
        RemoveSamplerRemapping((NiGPUProgram::ProgramType)i);
    }    
}
//---------------------------------------------------------------------------
