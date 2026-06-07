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

#ifndef NID3D102DBUFFERDATA_H
#define NID3D102DBUFFERDATA_H

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

#include <Ni2DBuffer.h>

class NiDepthStencilBuffer;

//---------------------------------------------------------------------------
// Abstract base class designed to represent all rendering targets
class NID3D10RENDERER_ENTRY NiD3D102DBufferData : 
    public Ni2DBuffer::RendererData
{
    NiDeclareRootRTTI(NiD3D102DBufferData);
public:
    virtual ~NiD3D102DBufferData();

    // *** begin Emergent internal use only ***
    virtual bool CanDisplayFrame();
    virtual HRESULT DisplayFrame(unsigned int uiSyncInterval, 
        bool bPresentTest = false);

    ID3D10Texture2D* GetTexture2D() const;
    // *** end Emergent internal use only ***

protected:
    inline NiD3D102DBufferData();

    ID3D10Texture2D* m_pkTexture;
};
NiSmartPointer(NiD3D102DBufferData);

//---------------------------------------------------------------------------
// Class representing textures to which the system can render
class NID3D10RENDERER_ENTRY NiD3D10RenderTargetBufferData : 
    public NiD3D102DBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiD3D10RenderTargetBufferData();

    // *** begin Emergent internal use only ***

    // IMPORTANT NOTE: This function requires that a Ni2DBuffer* reference is
    // passed in. Since D3D10 requires that you create a texture prior to 
    // creating
    // the Ni2DBuffer, the Ni2DBuffer and its renderer data are populated by 
    // the Create call. Note that if  pkBuffer is NULL, a new Ni2DBuffer will
    // be created. If this new Ni2DBUffer it is not stored in a smart pointer 
    // when the function returns, it will be LEAKED by the application.
    
    static NiD3D10RenderTargetBufferData* Create(ID3D10Texture2D* pkD3DTexture,
        Ni2DBuffer*& pkBuffer, 
        D3D10_RENDER_TARGET_VIEW_DESC* pkRTViewDesc = NULL);

    inline ID3D10RenderTargetView* GetRenderTargetView() const;
    inline ID3D10Texture2D* GetRenderTargetBuffer() const;

    // *** end Emergent internal use only ***

protected:
    NiD3D10RenderTargetBufferData();

    ID3D10RenderTargetView* m_pkRenderTargetView;
};
//NiSmartPointer(NiD3D10RenderTargetBufferData);

//---------------------------------------------------------------------------
// Class representing a render target that was created after the device,
// generally as a secondary window.  Such render targets are created from
// swap chain objects, and are thus called SwapChain render targets
class NID3D10RENDERER_ENTRY NiD3D10SwapChainBufferData : 
    public NiD3D10RenderTargetBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiD3D10SwapChainBufferData();

    // *** begin Emergent internal use only ***
    virtual bool CanDisplayFrame();
    virtual HRESULT DisplayFrame(unsigned int uiSyncInterval, 
        bool bPresentTest = false);

    static NiD3D10SwapChainBufferData* Create(IDXGISwapChain* pkSwapChain,
        Ni2DBuffer*& pkBuffer);

    bool ResizeSwapChain(unsigned int uiWidth, unsigned int uiHeight);

    IDXGISwapChain* GetSwapChain() const;
    // *** end Emergent internal use only ***

protected:
    NiD3D10SwapChainBufferData();

    IDXGISwapChain* m_pkSwapChain;
};
//NiSmartPointer(NiD3D10SwapChainBufferData);

//---------------------------------------------------------------------------
// Class representing a depth/stencil buffer
class NID3D10RENDERER_ENTRY NiD3D10DepthStencilBufferData : 
    public NiD3D102DBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiD3D10DepthStencilBufferData();

    // *** begin Emergent internal use only ***

    static NiD3D10DepthStencilBufferData* Create(
        ID3D10Texture2D* pkD3DTexture, NiDepthStencilBuffer*& pkBuffer, 
        D3D10_DEPTH_STENCIL_VIEW_DESC* pkDSViewDesc = NULL);

    ID3D10DepthStencilView* GetDepthStencilView() const;
    ID3D10Texture2D* GetDepthStencilBuffer() const;

    // *** end Emergent internal use only ***
protected:
    NiD3D10DepthStencilBufferData();

    ID3D10DepthStencilView* m_pkDepthStencilView;
};
//NiSmartPointer(NiD3D10DepthStencilBufferData);
            

#include "NiD3D102DBufferData.inl"

//---------------------------------------------------------------------------
#endif // NID3D102DBUFFERDATA_H
