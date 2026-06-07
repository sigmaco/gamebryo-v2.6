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

#ifndef NID3D10RENDERSTATEGROUP_H
#define NID3D10RENDERSTATEGROUP_H

#include <NiRefObject.h>

#include "NiD3D10RendererLibType.h"

#include "NiD3D10Headers.h"

#include <NiGPUProgram.h>

class NID3D10RENDERER_ENTRY NiD3D10RenderStateGroup : public NiRefObject
{
public:
    NiD3D10RenderStateGroup();
    ~NiD3D10RenderStateGroup();

    // Setting individual values
    void SetBSAlphaToCoverageEnable(bool bAlphaToCoverageEnable);
    bool GetBSAlphaToCoverageEnable(bool& bAlphaToCoverageEnable) const;
    void RemoveBSAlphaToCoverageEnable();

    void SetBSBlendEnable(unsigned int uiRenderTarget, bool bBlendEnable);
    bool GetBSBlendEnable(unsigned int uiRenderTarget, bool& bBlendEnable) 
        const;
    void RemoveBSBlendEnable(unsigned int uiRenderTarget);

    void SetBSSrcBlend(D3D10_BLEND eBlend);
    bool GetBSSrcBlend(D3D10_BLEND& eBlend) const;
    void RemoveBSSrcBlend();

    void SetBSDestBlend(D3D10_BLEND eBlend);
    bool GetBSDestBlend(D3D10_BLEND& eBlend) const;
    void RemoveBSDestBlend();

    void SetBSBlendOp(D3D10_BLEND_OP eBlendOp);
    bool GetBSBlendOp(D3D10_BLEND_OP& eBlendOp) const;
    void RemoveBSBlendOp();

    void SetBSSrcBlendAlpha(D3D10_BLEND eBlend);
    bool GetBSSrcBlendAlpha(D3D10_BLEND& eBlend) const;
    void RemoveBSSrcBlendAlpha();

    void SetBSDestBlendAlpha(D3D10_BLEND eBlend);
    bool GetBSDestBlendAlpha(D3D10_BLEND& eBlend) const;
    void RemoveBSDestBlendAlpha();

    void SetBSBlendOpAlpha(D3D10_BLEND_OP eBlendOp);
    bool GetBSBlendOpAlpha(D3D10_BLEND_OP& eBlendOp) const;
    void RemoveBSBlendOpAlpha();

    void SetBSRenderTargetWriteMask(unsigned int uiRenderTarget, 
        unsigned char ucWriteMask);
    bool GetBSRenderTargetWriteMask(unsigned int uiRenderTarget, 
        unsigned char& ucWriteMask) const;
    void RemoveBSRenderTargetWriteMask(unsigned int uiRenderTarget);

    void SetBlendFactor(const float afBlendFactor[4]);
    bool GetBlendFactor(float afBlendFactor[4]) const;
    void RemoveBlendFactor();

    void SetSampleMask(unsigned int uiSampleMask);
    bool GetSampleMask(unsigned int& uiSampleMask) const;
    void RemoveSampleMask();

    void SetDSSDepthEnable(bool bDepthEnable);
    bool GetDSSDepthEnable(bool& bDepthEnable) const;
    void RemoveDSSDepthEnable();

    void SetDSSDepthWriteMask(D3D10_DEPTH_WRITE_MASK eDepthWriteMask);
    bool GetDSSDepthWriteMask(D3D10_DEPTH_WRITE_MASK& eDepthWriteMask) const;
    void RemoveDSSDepthWriteMask();

    void SetDSSDepthFunc(D3D10_COMPARISON_FUNC eDepthFunc);
    bool GetDSSDepthFunc(D3D10_COMPARISON_FUNC& eDepthFunc) const;
    void RemoveDSSDepthFunc();

    void SetDSSStencilEnable(bool bStencilEnable);
    bool GetDSSStencilEnable(bool& bStencilEnable) const;
    void RemoveDSSStencilEnable();

    void SetDSSStencilReadMask(unsigned int uiStencilReadMask);
    bool GetDSSStencilReadMask(unsigned int& uiStencilReadMask) const;
    void RemoveDSSStencilReadMask();

    void SetDSSStencilWriteMask(unsigned int uiStencilWriteMask);
    bool GetDSSStencilWriteMask(unsigned int& uiStencilWriteMask) const;
    void RemoveDSSStencilWriteMask();

    void SetDSSFrontFaceStencilFailOp(D3D10_STENCIL_OP eStencilOp);
    bool GetDSSFrontFaceStencilFailOp(D3D10_STENCIL_OP& eStencilOp) const;
    void RemoveDSSFrontFaceStencilFailOp();

    void SetDSSFrontFaceStencilDepthFailOp(D3D10_STENCIL_OP eStencilOp);
    bool GetDSSFrontFaceStencilDepthFailOp(D3D10_STENCIL_OP& eStencilOp) const;
    void RemoveDSSFrontFaceStencilDepthFailOp();

    void SetDSSFrontFaceStencilPassOp(D3D10_STENCIL_OP eStencilOp);
    bool GetDSSFrontFaceStencilPassOp(D3D10_STENCIL_OP& eStencilOp) const;
    void RemoveDSSFrontFaceStencilPassOp();

    void SetDSSFrontFaceStencilFunc(D3D10_COMPARISON_FUNC eStencilFunc);
    bool GetDSSFrontFaceStencilFunc(D3D10_COMPARISON_FUNC& eStencilFunc) const;
    void RemoveDSSFrontFaceStencilFunc();

    void SetDSSBackFaceStencilFailOp(D3D10_STENCIL_OP eStencilOp);
    bool GetDSSBackFaceStencilFailOp(D3D10_STENCIL_OP& eStencilOp) const;
    void RemoveDSSBackFaceStencilFailOp();

    void SetDSSBackFaceStencilDepthFailOp(D3D10_STENCIL_OP eStencilOp);
    bool GetDSSBackFaceStencilDepthFailOp(D3D10_STENCIL_OP& eStencilOp) const;
    void RemoveDSSBackFaceStencilDepthFailOp();

    void SetDSSBackFaceStencilPassOp(D3D10_STENCIL_OP eStencilOp);
    bool GetDSSBackFaceStencilPassOp(D3D10_STENCIL_OP& eStencilOp) const;
    void RemoveDSSBackFaceStencilPassOp();

    void SetDSSBackFaceStencilFunc(D3D10_COMPARISON_FUNC eStencilFunc);
    bool GetDSSBackFaceStencilFunc(D3D10_COMPARISON_FUNC& eStencilFunc) const;
    void RemoveDSSBackFaceStencilFunc();

    void SetStencilRef(unsigned int uiStencilRef);
    bool GetStencilRef(unsigned int& uiStencilRef) const;
    void RemoveStencilRef();

    void SetRSFillMode(D3D10_FILL_MODE eFillMode);
    bool GetRSFillMode(D3D10_FILL_MODE& eFillMode) const;
    void RemoveRSFillMode();

    void SetRSCullMode(D3D10_CULL_MODE eCullMode);
    bool GetRSCullMode(D3D10_CULL_MODE& eCullMode) const;
    void RemoveRSCullMode();

    void SetRSFrontCounterClockwise(bool bFrontCounterClockwise);
    bool GetRSFrontCounterClockwise(bool& bFrontCounterClockwise) const;
    void RemoveRSFrontCounterClockwise();

    void SetRSDepthBias(int iDepthBias);
    bool GetRSDepthBias(int& iDepthBias) const;
    void RemoveRSDepthBias();

    void SetRSDepthBiasClamp(float fDepthBiasClamp);
    bool GetRSDepthBiasClamp(float& fDepthBiasClamp) const;
    void RemoveRSDepthBiasClamp();

    void SetRSSlopeScaledDepthBias(float fSlopeScaledDepthBias);
    bool GetRSSlopeScaledDepthBias(float& fSlopeScaledDepthBias) const;
    void RemoveRSSlopeScaledDepthBias();

    void SetRSDepthClipEnable(bool bDepthClipEnable);
    bool GetRSDepthClipEnable(bool& bDepthClipEnable) const;
    void RemoveRSDepthClipEnable();

    void SetRSScissorEnable(bool bScissorEnable);
    bool GetRSScissorEnable(bool& bScissorEnable) const;
    void RemoveRSScissorEnable();

    void SetRSMultisampleEnable(bool bMultisampleEnable);
    bool GetRSMultisampleEnable(bool& bMultisampleEnable) const;
    void RemoveRSMultisampleEnable();

    void SetRSAntialiasedLineEnable(bool bAntialiasedLineEnable);
    bool GetRSAntialiasedLineEnable(bool& bAntialiasedLineEnable) const;
    void RemoveRSAntialiasedLineEnable();

    void SetSamplerFilter(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_FILTER eFilter);
    bool GetSamplerFilter(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_FILTER& eFilter) const;
    void RemoveSamplerFilter(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerAddressU(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_TEXTURE_ADDRESS_MODE eAddressU);
    bool GetSamplerAddressU(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_TEXTURE_ADDRESS_MODE& eAddressU) const;
    void RemoveSamplerAddressU(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerAddressV(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_TEXTURE_ADDRESS_MODE eAddressV);
    bool GetSamplerAddressV(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_TEXTURE_ADDRESS_MODE& eAddressV) const;
    void RemoveSamplerAddressV(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerAddressW(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_TEXTURE_ADDRESS_MODE eAddressW);
    bool GetSamplerAddressW(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_TEXTURE_ADDRESS_MODE& eAddressW) const;
    void RemoveSamplerAddressW(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerMipLODBias(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float fMipLODBias);
    bool GetSamplerMipLODBias(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float& fMipLODBias) const;
    void RemoveSamplerMipLODBias(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerMaxAnisotropy(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, unsigned int uiMaxAnisotropy);
    bool GetSamplerMaxAnisotropy(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, unsigned int& uiMaxAnisotropy) const;
    void RemoveSamplerMaxAnisotropy(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerComparisonFunc(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_COMPARISON_FUNC eComparisonFunc);
    bool GetSamplerComparisonFunc(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_COMPARISON_FUNC& eComparisonFunc) const;
    void RemoveSamplerComparisonFunc(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerBorderColor(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, const float afBorderColor[4]);
    bool GetSamplerBorderColor(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float afBorderColor[4]) const;
    void RemoveSamplerBorderColor(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerMinLOD(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float fMinLOD);
    bool GetSamplerMinLOD(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float& fMinLOD) const;
    void RemoveSamplerMinLOD(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    void SetSamplerMaxLOD(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float fMaxLOD);
    bool GetSamplerMaxLOD(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, float& fMaxLOD) const;
    void RemoveSamplerMaxLOD(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler);

    // Allow samplers to be reordered to match shader programs
    void SetSamplerRemapping(NiGPUProgram::ProgramType eType,
        unsigned char* pucSamplerRemapping);
    const unsigned char* GetSamplerRemapping(NiGPUProgram::ProgramType eType)
        const;
    void RemoveSamplerRemapping(NiGPUProgram::ProgramType eType);

    // Setting descriptors
    void SetBlendStateDesc(const D3D10_BLEND_DESC& kDesc, 
        unsigned int uiValidFlags);
    void GetBlendStateDesc(D3D10_BLEND_DESC& kDesc, 
        unsigned int& uiValidFlags) const;
    void SetDepthStencilStateDesc(const D3D10_DEPTH_STENCIL_DESC& kDesc, 
        unsigned int uiValidFlags);
    void GetDepthStencilStateDesc(D3D10_DEPTH_STENCIL_DESC& kDesc, 
        unsigned int& uiValidFlags) const;
    void SetRasterizerStateDesc(const D3D10_RASTERIZER_DESC& kDesc, 
        unsigned int uiValidFlags);
    void GetRasterizerStateDesc(D3D10_RASTERIZER_DESC& kDesc, 
        unsigned int& uiValidFlags) const;
    void SetSamplerDesc(NiGPUProgram::ProgramType eType,
        unsigned int uiSampler, const D3D10_SAMPLER_DESC& kDesc, 
        unsigned int uiValidFlags);
    void GetSamplerDesc(NiGPUProgram::ProgramType eType, 
        unsigned int uiSampler, D3D10_SAMPLER_DESC& kDesc, 
        unsigned int& uiValidFlags) const;

    void ResetRenderStates();

protected:
    D3D10_BLEND_DESC m_kBlendDesc;
    D3D10_DEPTH_STENCIL_DESC m_kDepthStencilDesc;
    D3D10_RASTERIZER_DESC m_kRasterizerDesc;
    D3D10_SAMPLER_DESC m_aakSamplerDescs[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    float m_afBlendFactor[4];
    unsigned int m_uiSampleMask;
    unsigned int m_uiStencilRef;

    unsigned int m_uiBlendValidFlags;
    unsigned int m_uiDepthStencilValidFlags;
    unsigned int m_uiRasterizerValidFlags;
    unsigned int m_aauiSamplerValidFlags[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    unsigned char m_aaucSamplerRemapping[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    bool m_bBlendFactorValid;
    bool m_bSampleMaskValid;
    bool m_bStencilRefValid;
};

NiSmartPointer(NiD3D10RenderStateGroup);

#endif // NID3D10RENDERSTATEGROUP_H