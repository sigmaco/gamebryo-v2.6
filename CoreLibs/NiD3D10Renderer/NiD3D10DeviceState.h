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

#ifndef NID3D10DEVICESTATE_H
#define NID3D10DEVICESTATE_H

#include <NiMemObject.h>

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

class NID3D10RENDERER_ENTRY NiD3D10DeviceState : public NiMemObject
{
public:
    NiD3D10DeviceState(ID3D10Device* pkDevice);
    virtual ~NiD3D10DeviceState();

    // Render states
    void OMSetBlendState(ID3D10BlendState* pkBlendState, 
        const float afBlendFactor[4], unsigned int uiSampleMask);
    void OMGetBlendState(ID3D10BlendState** ppkBlendState, 
        float afBlendFactor[4], unsigned int* puiSampleMask) const;
    void OMClearBlendState();

    void OMSetDepthStencilState(ID3D10DepthStencilState* pkDepthStencilState,
        unsigned int uiStencilRef);
    void OMGetDepthStencilState(ID3D10DepthStencilState** ppkDepthStencilState,
        unsigned int* puiStencilRef) const;
    void OMClearDepthStencilState();

    void RSSetState(ID3D10RasterizerState* pkRasterizerState);
    void RSGetState(ID3D10RasterizerState** ppkRasterizerState) const;
    void RSClearState();

    // Samplers
    void VSSetSamplers(unsigned int uiStartSlot, unsigned int uiNumSamplers,
        ID3D10SamplerState*const* ppkSamplers);
    void VSGetSamplers(unsigned int uiStartSlot, unsigned int uiNumSamplers,
        ID3D10SamplerState** ppkSamplers) const;
    void VSClearSamplers();

    void GSSetSamplers(unsigned int uiStartSlot, unsigned int uiNumSamplers,
        ID3D10SamplerState*const* ppkSamplers);
    void GSGetSamplers(unsigned int uiStartSlot, unsigned int uiNumSamplers,
        ID3D10SamplerState** ppkSamplers) const;
    void GSClearSamplers();

    void PSSetSamplers(unsigned int uiStartSlot, unsigned int uiNumSamplers,
        ID3D10SamplerState*const* ppkSamplers);
    void PSGetSamplers(unsigned int uiStartSlot, unsigned int uiNumSamplers,
        ID3D10SamplerState** ppkSamplers) const;
    void PSClearSamplers();

    // Textures
    void VSSetShaderResources(unsigned int uiStartSlot, 
        unsigned int uiNumViews, 
        ID3D10ShaderResourceView*const* ppkResourceViews);
    void VSGetShaderResources(unsigned int uiStartSlot, 
        unsigned int uiNumViews, 
        ID3D10ShaderResourceView** ppkResourceViews) const;
    void VSClearShaderResources();

    void GSSetShaderResources(unsigned int uiStartSlot, 
        unsigned int uiNumViews, 
        ID3D10ShaderResourceView*const* ppkResourceViews);
    void GSGetShaderResources(unsigned int uiStartSlot, 
        unsigned int uiNumViews, 
        ID3D10ShaderResourceView** ppkResourceViews) const;
    void GSClearShaderResources();

    void PSSetShaderResources(unsigned int uiStartSlot, 
        unsigned int uiNumViews, 
        ID3D10ShaderResourceView*const* ppkResourceViews);
    void PSGetShaderResources(unsigned int uiStartSlot, 
        unsigned int uiNumViews, 
        ID3D10ShaderResourceView** ppkResourceViews) const;
    void PSClearShaderResources();

    // Shader constant buffers
    void VSSetConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, ID3D10Buffer*const* ppkConstantBuffers);
    void VSGetConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, ID3D10Buffer** ppkConstantBuffers) const;
    void VSClearConstantBuffers();

    void GSSetConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, ID3D10Buffer*const* ppkConstantBuffers);
    void GSGetConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, ID3D10Buffer** ppkConstantBuffers) const;
    void GSClearConstantBuffers();

    void PSSetConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, ID3D10Buffer*const* ppkConstantBuffers);
    void PSGetConstantBuffers(unsigned int uiStartSlot, 
        unsigned int uiNumBuffers, ID3D10Buffer** ppkConstantBuffers) const;
    void PSClearConstantBuffers();

    // Shader programs
    void VSSetShader(ID3D10VertexShader* pkVertexShader);
    inline void VSGetShader(ID3D10VertexShader** ppkVertexShader) const;
    inline ID3D10VertexShader* VSGetShader() const;
    void VSClearShader();

    void GSSetShader(ID3D10GeometryShader* pkGeometryShader);
    inline void GSGetShader(ID3D10GeometryShader** pkGeometryShader) const;  
    inline ID3D10GeometryShader* GSGetShader() const;
    void GSClearShader();

    void PSSetShader(ID3D10PixelShader* pkPixelShader);
    inline void PSGetShader(ID3D10PixelShader** pkPixelShader) const;   
    inline ID3D10PixelShader* PSGetShader() const;
    void PSClearShader();

    // Invalidate
    void InvalidateDeviceState();

protected:
    ID3D10Device* m_pkDevice;

    ID3D10BlendState* m_pkBlendState;
    ID3D10DepthStencilState* m_pkDepthStencilState;
    ID3D10RasterizerState* m_pkRasterizerState;
    ID3D10SamplerState* m_apkVertexSamplers
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    ID3D10SamplerState* m_apkGeometrySamplers
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    ID3D10SamplerState* m_apkPixelSamplers
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    float m_afBlendFactor[4];
    unsigned int m_uiSampleMask;
    unsigned int m_uiStencilRef;

    ID3D10ShaderResourceView* m_apkVertexTextures
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    ID3D10ShaderResourceView* m_apkGeometryTextures
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    ID3D10ShaderResourceView* m_apkPixelTextures
        [D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    ID3D10Buffer* m_apkVertexConstantBuffers
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
    ID3D10Buffer* m_apkGeometryConstantBuffers
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];
    ID3D10Buffer* m_apkPixelConstantBuffers
        [D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT];

    ID3D10VertexShader* m_pkVertexShader;
    ID3D10GeometryShader* m_pkGeometryShader;
    ID3D10PixelShader* m_pkPixelShader;

    bool m_bBlendStateUnChanged;
    bool m_bDepthStencilStateUnChanged;
    bool m_bVSResourcesUnChanged;
    bool m_bGSResourcesUnChanged;
    bool m_bPSResourcesUnChanged;
};

#include "NiD3D10DeviceState.inl"

#endif // NID3D10DEVICESTATE_H