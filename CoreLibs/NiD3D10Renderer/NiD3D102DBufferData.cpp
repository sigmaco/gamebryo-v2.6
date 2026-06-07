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

#include "NiD3D102DBufferData.h"
#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10PixelFormat.h"

#include <NiDepthStencilBuffer.h>

NiImplementRootRTTI(NiD3D102DBufferData);

//---------------------------------------------------------------------------
NiD3D102DBufferData::~NiD3D102DBufferData()
{
    if (m_pkTexture)
        m_pkTexture->Release();
}
//---------------------------------------------------------------------------
bool NiD3D102DBufferData::CanDisplayFrame()
{
    return false;
}
//---------------------------------------------------------------------------
HRESULT NiD3D102DBufferData::DisplayFrame(unsigned int, 
    bool)
{
    return E_NOTIMPL;
}
//---------------------------------------------------------------------------
ID3D10Texture2D* NiD3D102DBufferData::GetTexture2D() const
{
    return m_pkTexture;
}
//---------------------------------------------------------------------------
NiImplementRTTI(NiD3D10RenderTargetBufferData,NiD3D102DBufferData);
//---------------------------------------------------------------------------
NiD3D10RenderTargetBufferData::NiD3D10RenderTargetBufferData() :
    m_pkRenderTargetView(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10RenderTargetBufferData::~NiD3D10RenderTargetBufferData()
{
    if (m_pkRenderTargetView)
        m_pkRenderTargetView->Release();
}
//---------------------------------------------------------------------------
NiD3D10RenderTargetBufferData* NiD3D10RenderTargetBufferData::Create(
    ID3D10Texture2D* pkD3DTexture, Ni2DBuffer*& pkBuffer, 
    D3D10_RENDER_TARGET_VIEW_DESC* pkRTViewDesc)
{
    if (pkD3DTexture == NULL)
        return NULL;

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    assert (pkRenderer);
    ID3D10Device* pkDevice = pkRenderer->GetD3D10Device();
    assert (pkDevice);

    ID3D10RenderTargetView* pkRTView = NULL;
    HRESULT hr = pkDevice->CreateRenderTargetView(pkD3DTexture, pkRTViewDesc, 
        &pkRTView);
    if (FAILED(hr) || pkRTView == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
                "No error message from D3D10, but render target view is "
                "NULL.");
        }

        if (pkRTView)
            pkRTView->Release();

        return NULL;
    }

    NiD3D10RenderTargetBufferData* pkThis = NiNew
        NiD3D10RenderTargetBufferData;

    pkThis->m_pkRenderTargetView = pkRTView;
    pkThis->m_pkTexture = pkD3DTexture;
    pkThis->m_pkTexture->AddRef();

    D3D10_TEXTURE2D_DESC kDesc;
    pkD3DTexture->GetDesc(&kDesc);

    pkThis->m_pkPixelFormat = 
        NiD3D10PixelFormat::ObtainFromDXGIFormat(kDesc.Format);

    pkThis->m_eMSAAPref = Ni2DBuffer::GetMSAAPrefFromCountAndQuality(
        kDesc.SampleDesc.Count, kDesc.SampleDesc.Quality);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kDesc.Width, kDesc.Height, pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;

    return pkThis;
}

//---------------------------------------------------------------------------
NiImplementRTTI(NiD3D10SwapChainBufferData,NiD3D10RenderTargetBufferData);
//---------------------------------------------------------------------------
NiD3D10SwapChainBufferData::~NiD3D10SwapChainBufferData()
{
    if (m_pkSwapChain)
    {
        // Get out of fullscreen mode before releasing
        m_pkSwapChain->SetFullscreenState(false, NULL);
        m_pkSwapChain->Release();
    }
}

//---------------------------------------------------------------------------
NiD3D10SwapChainBufferData* NiD3D10SwapChainBufferData::Create(
    IDXGISwapChain* pkSwapChain, Ni2DBuffer*& pkBuffer)
{
    if (pkSwapChain == NULL)
        return NULL;

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    assert (pkRenderer);
    ID3D10Device* pkDevice = pkRenderer->GetD3D10Device();
    assert (pkDevice);

    ID3D10Texture2D* pkBackBuffer = NULL;
    HRESULT hr = pkSwapChain->GetBuffer(0, __uuidof(*pkBackBuffer),
        (LPVOID*)&pkBackBuffer);

    if (FAILED(hr) || pkBackBuffer == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_GET_BUFFER_FROM_SWAP_CHAIN_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_GET_BUFFER_FROM_SWAP_CHAIN_FAILED,
                "No error message from D3D10, but buffer is NULL.");
        }

        if (pkBackBuffer)
            pkBackBuffer->Release();

        return NULL;
    }

    ID3D10RenderTargetView* pkRTView = NULL;
    hr = pkDevice->CreateRenderTargetView(pkBackBuffer, NULL, 
        &pkRTView);
    if (FAILED(hr) || pkRTView == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
                "No error message from D3D10, but render target view is "
                "NULL.");
        }

        if (pkRTView)
            pkRTView->Release();
        if (pkBackBuffer)
            pkBackBuffer->Release();

        return NULL;
    }

    NiD3D10SwapChainBufferData* pkThis = NiNew
        NiD3D10SwapChainBufferData;

    pkThis->m_pkSwapChain = pkSwapChain;
    pkThis->m_pkRenderTargetView = pkRTView;
    pkThis->m_pkTexture = pkBackBuffer;

    D3D10_TEXTURE2D_DESC kDesc;
    pkBackBuffer->GetDesc(&kDesc);

    pkThis->m_pkPixelFormat = 
        NiD3D10PixelFormat::ObtainFromDXGIFormat(kDesc.Format);

    pkThis->m_eMSAAPref = Ni2DBuffer::GetMSAAPrefFromCountAndQuality(
        kDesc.SampleDesc.Count, kDesc.SampleDesc.Quality);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kDesc.Width, kDesc.Height, pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;

    return pkThis;
}
//---------------------------------------------------------------------------
bool NiD3D10SwapChainBufferData::ResizeSwapChain(unsigned int uiWidth, 
    unsigned int uiHeight)
{
    DXGI_SWAP_CHAIN_DESC kDesc;
    HRESULT hr = m_pkSwapChain->GetDesc(&kDesc);
    if (FAILED(hr))
    {
        return false;
    }

    m_pkRenderTargetView->Release();
    m_pkRenderTargetView = NULL;
    m_pkTexture->Release();
    m_pkTexture = NULL;

    hr = m_pkSwapChain->ResizeBuffers(kDesc.BufferCount, uiWidth, uiHeight, 
        kDesc.BufferDesc.Format, kDesc.Flags);
    if (FAILED(hr))
    {
        // Don't return - still need to recreate render target view.
    }

    // Recreate render target view
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    assert (pkRenderer);
    ID3D10Device* pkDevice = pkRenderer->GetD3D10Device();
    assert (pkDevice);

    ID3D10Texture2D* pkBackBuffer = NULL;
    hr = m_pkSwapChain->GetBuffer(0, __uuidof(*pkBackBuffer), 
        (LPVOID*)&pkBackBuffer);

    if (FAILED(hr) || pkBackBuffer == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_GET_BUFFER_FROM_SWAP_CHAIN_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_GET_BUFFER_FROM_SWAP_CHAIN_FAILED,
                "No error message from D3D10, but buffer is NULL.");
        }

        if (pkBackBuffer)
            pkBackBuffer->Release();

        return NULL;
    }

    ID3D10RenderTargetView* pkRTView = NULL;
    hr = pkDevice->CreateRenderTargetView(pkBackBuffer, NULL, 
        &pkRTView);
    if (FAILED(hr) || pkRTView == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RENDER_TARGET_VIEW_CREATION_FAILED,
                "No error message from D3D10, but render target view is "
                "NULL.");
        }

        if (pkRTView)
            pkRTView->Release();
        if (pkBackBuffer)
            pkBackBuffer->Release();

        return NULL;
    }

    m_pkRenderTargetView = pkRTView;
    m_pkTexture = pkBackBuffer;

    D3D10_TEXTURE2D_DESC kTextureDesc;
    m_pkTexture->GetDesc(&kTextureDesc);

    m_pkPixelFormat = NiD3D10PixelFormat::ObtainFromDXGIFormat(
        kTextureDesc.Format);

    m_pkBuffer->ResetDimensions(kTextureDesc.Width, kTextureDesc.Height);

    if (kTextureDesc.SampleDesc.Count == 1)
        m_eMSAAPref = Ni2DBuffer::MULTISAMPLE_NONE;
    else if (kTextureDesc.SampleDesc.Count < 4)
        m_eMSAAPref = Ni2DBuffer::MULTISAMPLE_2;
    else
        m_eMSAAPref = Ni2DBuffer::MULTISAMPLE_4;

    return true;
}
//---------------------------------------------------------------------------
IDXGISwapChain* NiD3D10SwapChainBufferData::GetSwapChain() const
{
    return m_pkSwapChain;
}
//---------------------------------------------------------------------------
bool NiD3D10SwapChainBufferData::CanDisplayFrame()
{
    return true;
}
//---------------------------------------------------------------------------
HRESULT NiD3D10SwapChainBufferData::DisplayFrame(unsigned int uiSyncInterval, 
    bool bPresentTest)
{
    if (m_pkSwapChain == NULL)
        return false;

    unsigned int uiFlags = (bPresentTest ? DXGI_PRESENT_TEST : 0);

    return m_pkSwapChain->Present(uiSyncInterval, uiFlags);
}
//---------------------------------------------------------------------------
NiImplementRTTI(NiD3D10DepthStencilBufferData,NiD3D102DBufferData);
//---------------------------------------------------------------------------
NiD3D10DepthStencilBufferData::NiD3D10DepthStencilBufferData() :
    m_pkDepthStencilView(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10DepthStencilBufferData::~NiD3D10DepthStencilBufferData()
{
    if (m_pkDepthStencilView)
        m_pkDepthStencilView->Release();
}
//---------------------------------------------------------------------------
ID3D10DepthStencilView* NiD3D10DepthStencilBufferData::GetDepthStencilView()
    const
{
    return m_pkDepthStencilView;
}
//---------------------------------------------------------------------------
ID3D10Texture2D* NiD3D10DepthStencilBufferData::GetDepthStencilBuffer() const
{
    return m_pkTexture;
}
//---------------------------------------------------------------------------
NiD3D10DepthStencilBufferData* NiD3D10DepthStencilBufferData::Create(
    ID3D10Texture2D* pkD3DTexture, NiDepthStencilBuffer*& pkBuffer, 
    D3D10_DEPTH_STENCIL_VIEW_DESC* pkDSViewDesc)
{    
    if (pkD3DTexture == NULL)
        return NULL;

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    assert (pkRenderer);
    ID3D10Device* pkDevice = pkRenderer->GetD3D10Device();
    assert (pkDevice);

    ID3D10DepthStencilView* pkDSView = NULL;
    HRESULT hr = pkDevice->CreateDepthStencilView(pkD3DTexture, pkDSViewDesc, 
        &pkDSView);
    if (FAILED(hr) || pkDSView == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEPTH_STENCIL_VIEW_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEPTH_STENCIL_VIEW_CREATION_FAILED,
                "No error message from D3D10, but depth stencil view is "
                "NULL.");
        }

        if (pkDSView)
            pkDSView->Release();

        return NULL;
    }

    NiD3D10DepthStencilBufferData* pkThis = NiNew
        NiD3D10DepthStencilBufferData;

    pkThis->m_pkDepthStencilView = pkDSView;
    pkThis->m_pkTexture = pkD3DTexture;
    pkThis->m_pkTexture->AddRef();

    D3D10_TEXTURE2D_DESC kDesc;
    pkD3DTexture->GetDesc(&kDesc);

    pkThis->m_pkPixelFormat = 
        NiD3D10PixelFormat::ObtainFromDXGIFormat(kDesc.Format);

    pkThis->m_eMSAAPref = Ni2DBuffer::GetMSAAPrefFromCountAndQuality(
        kDesc.SampleDesc.Count, kDesc.SampleDesc.Quality);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = NiDepthStencilBuffer::Create(kDesc.Width, kDesc.Height, 
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;

    return pkThis;
}
//---------------------------------------------------------------------------
