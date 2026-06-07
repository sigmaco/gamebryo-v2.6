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

#ifndef NID3D10RENDERSTATEMANAGER_H
#define NID3D10RENDERSTATEMANAGER_H

#include <NiMemObject.h>

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"
#include "NiD3D10DeviceState.h"

#include <NiAlphaProperty.h>
#include <NiGPUProgram.h>
#include <NiStencilProperty.h>
#include <NiTexturingProperty.h>
#include <NiZBufferProperty.h>

class NiD3D10RenderStateGroup;
class NiPropertyState;
class NiWireframeProperty;

class NID3D10RENDERER_ENTRY NiD3D10RenderStateManager : public NiMemObject
{
public:
    NiD3D10RenderStateManager(ID3D10Device* pkDevice, 
        NiD3D10DeviceState* pkDeviceState);
    virtual ~NiD3D10RenderStateManager();

    // Gamebryo override
    inline bool GetLeftRightSwap() const;
    inline void SetLeftRightSwap(bool bSwap);

    // Gamebryo properties
    bool SetProperties(const NiPropertyState* pkState);
    bool SetAlphaProperty(const NiAlphaProperty* pkNew);
    bool SetStencilProperty(const NiStencilProperty* pkNew);
    bool SetWireframeProperty(const NiWireframeProperty* pkNew);
    bool SetZBufferProperty(const NiZBufferProperty* pkNew);

    // Gamebryo render state group
    bool SetRenderStateGroup(const NiD3D10RenderStateGroup* pkRSGroup);

    enum BlendStateValidFlags
    {
        BSVALID_ALPHATOCOVERAGEENABLE   = 1 <<  0,
        BSVALID_BLENDENABLE_0           = 1 <<  1,
        BSVALID_BLENDENABLE_1           = 1 <<  2,
        BSVALID_BLENDENABLE_2           = 1 <<  3,
        BSVALID_BLENDENABLE_3           = 1 <<  4,
        BSVALID_BLENDENABLE_4           = 1 <<  5,
        BSVALID_BLENDENABLE_5           = 1 <<  6,
        BSVALID_BLENDENABLE_6           = 1 <<  7,
        BSVALID_BLENDENABLE_7           = 1 <<  8,
        BSVALID_SRCBLEND                = 1 <<  9,
        BSVALID_DESTBLEND               = 1 << 10,
        BSVALID_BLENDOP                 = 1 << 11,
        BSVALID_SRCBLENDALPHA           = 1 << 12,
        BSVALID_DESTBLENDALPHA          = 1 << 13,
        BSVALID_BLENDOPALPHA            = 1 << 14,
        BSVALID_RENDERTARGETWRITEMASK_0 = 1 << 15,
        BSVALID_RENDERTARGETWRITEMASK_1 = 1 << 16,
        BSVALID_RENDERTARGETWRITEMASK_2 = 1 << 17,
        BSVALID_RENDERTARGETWRITEMASK_3 = 1 << 18,
        BSVALID_RENDERTARGETWRITEMASK_4 = 1 << 19,
        BSVALID_RENDERTARGETWRITEMASK_5 = 1 << 20,
        BSVALID_RENDERTARGETWRITEMASK_6 = 1 << 21,
        BSVALID_RENDERTARGETWRITEMASK_7 = 1 << 22
    };

    enum DepthStencilStateValidFlags
    {
        DSSVALID_DEPTHENABLE                    = 1 <<  0,
        DSSVALID_DEPTHWRITEMASK                 = 1 <<  1,
        DSSVALID_DEPTHFUNC                      = 1 <<  2,
        DSSVALID_STENCILENABLE                  = 1 <<  3,
        DSSVALID_STENCILREADMASK                = 1 <<  4,
        DSSVALID_STENCILWRITEMASK               = 1 <<  5,
        DSSVALID_FRONTFACE_STENCILFAILOP        = 1 <<  6,
        DSSVALID_FRONTFACE_STENCILDEPTHFAILOP   = 1 <<  7,
        DSSVALID_FRONTFACE_STENCILPASSOP        = 1 <<  8,
        DSSVALID_FRONTFACE_STENCILFUNC          = 1 <<  9,
        DSSVALID_BACKFACE_STENCILFAILOP         = 1 << 10,
        DSSVALID_BACKFACE_STENCILDEPTHFAILOP    = 1 << 11,
        DSSVALID_BACKFACE_STENCILPASSOP         = 1 << 12,
        DSSVALID_BACKFACE_STENCILFUNC           = 1 << 13
    };

    enum RasterizerStateValidFlags
    {
        RSVALID_FILLMODE                = 1 <<  0,
        RSVALID_CULLMODE                = 1 <<  1,
        RSVALID_FRONTCOUNTERCLOCKWISE   = 1 <<  2,
        RSVALID_DEPTHBIAS               = 1 <<  3,
        RSVALID_DEPTHBIASCLAMP          = 1 <<  4,
        RSVALID_SLOPESCALEDDEPTHBIAS    = 1 <<  5,
        RSVALID_DEPTHCLIPENABLE         = 1 <<  6,
        RSVALID_SCISSORENABLE           = 1 <<  7,
        RSVALID_MULTISAMPLEENABLE       = 1 <<  8,
        RSVALID_ANTIALIASEDLINEENABLE   = 1 <<  9
    };

    enum SamplerValidFlags
    {
        SVALID_FILTER          = 1 <<  0,
        SVALID_ADDRESSU        = 1 <<  1,
        SVALID_ADDRESSV        = 1 <<  2,
        SVALID_ADDRESSW        = 1 <<  3,
        SVALID_MIPLODBIAS      = 1 <<  4,
        SVALID_MAXANISOTROPY   = 1 <<  5,
        SVALID_COMPARISONFUNC  = 1 <<  6,
        SVALID_BORDERCOLOR     = 1 <<  7,
        SVALID_MINLOD          = 1 <<  8,
        SVALID_MAXLOD          = 1 <<  9
    };

    // Render state currently being built
    void SetBlendStateDesc(
        const D3D10_BLEND_DESC& kDesc, 
        NiUInt32 uiValidFlags);
    inline void GetBlendStateDesc(D3D10_BLEND_DESC& kDesc) const;
    inline void SetBlendFactor(const float afBlendFactor[4]);
    inline void GetBlendFactor(float afBlendFactor[4]) const;
    inline void SetSampleMask(NiUInt32 uiSampleMask);
    inline void GetSampleMask(NiUInt32& uiSampleMask) const;

    void SetDepthStencilStateDesc(
        const D3D10_DEPTH_STENCIL_DESC& kDesc, 
        NiUInt32 uiValidFlags);
    inline void GetDepthStencilStateDesc(D3D10_DEPTH_STENCIL_DESC& kDesc) const;
    inline void SetStencilRef(NiUInt32 uiStencilRef);
    inline void GetStencilRef(NiUInt32& uiStencilRef) const;

    void SetRasterizerStateDesc(
        const D3D10_RASTERIZER_DESC& kDesc, 
        NiUInt32 uiValidFlags);
    inline void GetRasterizerStateDesc(D3D10_RASTERIZER_DESC& kDesc) const;
    void SetSamplerDesc(
        NiGPUProgram::ProgramType eType,
        NiUInt32 uiSampler, 
        const D3D10_SAMPLER_DESC& kDesc, 
        NiUInt32 uiValidFlags);
    inline void GetSamplerDesc(
        NiGPUProgram::ProgramType eType,
        NiUInt32 uiSampler, 
        D3D10_SAMPLER_DESC& kDesc) const;

    void ResetCurrentState();

    // Default values 
    inline void SetDefaultBlendStateDesc(const D3D10_BLEND_DESC& kDesc);
    inline void GetDefaultBlendStateDesc(D3D10_BLEND_DESC& kDesc) const;
    inline void SetDefaultBlendFactor(const float afBlendFactor[4]);
    inline void GetDefaultBlendFactor(float afBlendFactor[4]) const;
    inline void SetDefaultSampleMask(NiUInt32 uiSampleMask);
    inline void GetDefaultSampleMask(NiUInt32& uiSampleMask) const;

    inline void SetDefaultDepthStencilStateDesc(
        const D3D10_DEPTH_STENCIL_DESC& kDesc);
    inline void GetDefaultDepthStencilStateDesc(
        D3D10_DEPTH_STENCIL_DESC& kDesc) const;
    inline void SetDefaultStencilRef(NiUInt32 uiStenciRef);
    inline void GetDefaultStencilRef(NiUInt32& uiStencilRef) const;

    inline void SetDefaultRasterizerStateDesc(const D3D10_RASTERIZER_DESC& kDesc);
    inline void GetDefaultRasterizerStateDesc(D3D10_RASTERIZER_DESC& kDesc) const;

    void SetDefaultSamplerDesc(
        NiGPUProgram::ProgramType eType,
        NiUInt32 uiSampler, 
        const D3D10_SAMPLER_DESC& kDesc);
    void GetDefaultSamplerDesc(
        NiGPUProgram::ProgramType eType,
        NiUInt32 uiSampler, 
        D3D10_SAMPLER_DESC& kDesc) const;

    // Apply current render state to device
    void ApplyCurrentState(const D3D10_STATE_BLOCK_MASK* pkMask = NULL);

    void ApplyCurrentBlendState();
    void ApplyCurrentDepthStencilState();
    void ApplyCurrentRasterizerState();
    void ApplyCurrentSamplers(const D3D10_STATE_BLOCK_MASK* pkMask = NULL);

    void ApplyCurrentVertexSamplers(
        NiUInt32 uiSamplerStart = 0, 
        NiUInt32 uiSamplerCount = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT,
        const D3D10_STATE_BLOCK_MASK* pkMask = NULL);
    void ApplyCurrentGeometrySamplers(
        NiUInt32 uiSamplerStart = 0, 
        NiUInt32 uiSamplerCount = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT,
        const D3D10_STATE_BLOCK_MASK* pkMask = NULL);
    void ApplyCurrentPixelSamplers(
        NiUInt32 uiSamplerStart = 0, 
        NiUInt32 uiSamplerCount = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT,
        const D3D10_STATE_BLOCK_MASK* pkMask = NULL);
    void ApplyCurrentSamplers(NiGPUProgram::ProgramType eType, 
        NiUInt32 uiSamplerStart, NiUInt32 uiSamplerCount,
        const D3D10_STATE_BLOCK_MASK* pkMask = NULL);

    // Conversions
    static D3D10_BLEND ConvertGbBlendToD3D10Blend(
        NiAlphaProperty::AlphaFunction eFunction);
    static D3D10_COMPARISON_FUNC ConvertGbStencilFuncToD3D10Comparison(
        NiStencilProperty::TestFunc eFunction);
    static D3D10_STENCIL_OP ConvertGbStencilActionToD3D10StencilOp(
        NiStencilProperty::Action eAction);
    static D3D10_COMPARISON_FUNC ConvertGbDepthFuncToD3D10Comparison(
        NiZBufferProperty::TestFunction eFunction);
    static D3D10_FILTER ConvertGbFilterModeToD3D10Filter(
        NiTexturingProperty::FilterMode eFilterMode);
    static bool ConvertGbFilterModeToMipmapEnable(
        NiTexturingProperty::FilterMode eFilterMode);
    static D3D10_TEXTURE_ADDRESS_MODE ConvertGbClampModeToD3D10AddressU(
        NiTexturingProperty::ClampMode eClampMode);
    static D3D10_TEXTURE_ADDRESS_MODE ConvertGbClampModeToD3D10AddressV(
        NiTexturingProperty::ClampMode eClampMode);

protected:
    void InitDefaultValues();

    void UpdateDefaultBlendStateObject();
    void UpdateDefaultDepthStencilStateObject();
    void UpdateDefaultRasterizerStateObject();
    void UpdateDefaultSamplerObject(
        NiGPUProgram::ProgramType eType,
        NiUInt32 uiSampler);
    void UpdateDefaultSamplerObjects();

    ID3D10Device* m_pkDevice;
    NiD3D10DeviceState* m_pkDeviceState;

    // Default render state values
    D3D10_BLEND_DESC m_kDefaultBlendDesc;
    D3D10_DEPTH_STENCIL_DESC m_kDefaultDepthStencilDesc;
    D3D10_RASTERIZER_DESC m_kDefaultRasterizerDesc;
    D3D10_SAMPLER_DESC m_aakDefaultSamplerDescs[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    float m_afDefaultBlendFactor[4];
    NiUInt32 m_uiDefaultSampleMask;
    NiUInt32 m_uiDefaultStencilRef;

    // Render states currently under construction
    D3D10_BLEND_DESC m_kCurrentBlendDesc;
    D3D10_DEPTH_STENCIL_DESC m_kCurrentDepthStencilDesc;
    D3D10_RASTERIZER_DESC m_kCurrentRasterizerDesc;
    D3D10_SAMPLER_DESC m_aakCurrentSamplerDescs[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    float m_afCurrentBlendFactor[4];
    NiUInt32 m_uiCurrentSampleMask;
    NiUInt32 m_uiCurrentStencilRef;

    // Default state objects
    ID3D10BlendState* m_pkDefaultBlendState;
    ID3D10DepthStencilState* m_pkDefaultDepthStencilState;
    ID3D10RasterizerState* m_pkDefaultRasterizerState;
    ID3D10SamplerState* m_aapkDefaultSamplerStates[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    // Gamebryo override value
    bool m_bLeftRightSwap;

    bool m_bBlendStateDirty;
    bool m_bDepthStencilStateDirty;
    bool m_bRasterizerStateDirty;
    bool m_aabSamplersDirty[NiGPUProgram::PROGRAM_MAX]
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
};

#include "NiD3D10RenderStateManager.inl"

#endif // NID3D10RENDERSTATEMANAGER_H