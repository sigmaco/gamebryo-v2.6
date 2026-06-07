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
#include "NiD3DRendererPCH.h"

#include "NiDX9PixelFormat.h"
#include "NiDX9Renderer.h"
#include "NiDX92DBufferData.h"
#include "NiDX9SystemDesc.h"

NiImplementRootRTTI(NiDX92DBufferData);

LPDIRECT3DSURFACE9 NiDX92DBufferData::ms_apkCurrentBuffer[4] = 
{
    NULL,
    NULL,
    NULL,
    NULL
};
LPDIRECT3DSURFACE9 NiDX92DBufferData::ms_pkCurrentDepthStencil = NULL;
unsigned int NiDX92DBufferData::ms_uiNumBuffers = 0;
bool NiDX92DBufferData::ms_bIndependentBitDepths = false;

//---------------------------------------------------------------------------
NiDX92DBufferData::NiDX92DBufferData() : m_pkSurface(NULL)
{
}
//---------------------------------------------------------------------------
NiDX92DBufferData::~NiDX92DBufferData()
{
    ReleaseRTReferences();
}
//---------------------------------------------------------------------------
void NiDX92DBufferData::GetMSAAD3DTypeAndQualityFromPref(
    Ni2DBuffer::MultiSamplePreference ePref, 
    D3DMULTISAMPLE_TYPE& eType, unsigned int& uiQuality)
{
    unsigned int uiCount = 0;
    uiQuality = ePref & 0x00FF;
    Ni2DBuffer::GetMSAACountAndQualityFromPref(ePref, uiCount, uiQuality);
    if (uiCount == 1)
        eType = D3DMULTISAMPLE_NONE;
    else if (uiCount > 16)
        eType = D3DMULTISAMPLE_NONMASKABLE;
    else
        eType = (D3DMULTISAMPLE_TYPE)uiCount;
}
//---------------------------------------------------------------------------
Ni2DBuffer::MultiSamplePreference 
    NiDX92DBufferData::GetMSAAPrefFromD3DTypeAndQuality(
    D3DMULTISAMPLE_TYPE eType, unsigned int uiQuality)
{
    unsigned int uiCount = 1;
    if (eType == D3DMULTISAMPLE_NONMASKABLE)
        uiCount = 0xFF;
    else if (eType == D3DMULTISAMPLE_NONE)
        uiCount = 1;
    else
        uiCount = (unsigned int)eType;
    return Ni2DBuffer::GetMSAAPrefFromCountAndQuality(uiCount, uiQuality);
}
//---------------------------------------------------------------------------
bool NiDX92DBufferData::SetRenderTarget(LPDIRECT3DDEVICE9 pkD3DDevice9,
    unsigned int uiTarget)
{
    // True validation should occur in the 
    // NiRenderer::ValidateRenderTargetGroup method.
    if (IsValid())
    {
        HRESULT eResult = S_OK;

        if (uiTarget > ms_uiNumBuffers)
            return false;

        // Set render target
        if (m_pkSurface != ms_apkCurrentBuffer[uiTarget])
        {
            // Go ahead and set the render target
            eResult = pkD3DDevice9->SetRenderTarget(uiTarget, 
                m_pkSurface);
            if (FAILED(eResult))
                return false;
            ms_apkCurrentBuffer[uiTarget] = m_pkSurface;
        }

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiDX92DBufferData::SetDepthTarget(LPDIRECT3DDEVICE9)
{
    // This should be implemented in the derived class, 
    // NiDX9DepthStencilBufferData
    return false;
}
//---------------------------------------------------------------------------
void NiDX92DBufferData::ClearRenderTarget(
    LPDIRECT3DDEVICE9 pkD3DDevice9, unsigned int uiTarget)
{
    if (uiTarget != 0 && ms_apkCurrentBuffer[uiTarget])
    {
        pkD3DDevice9->SetRenderTarget(uiTarget, NULL);
        ms_apkCurrentBuffer[uiTarget] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiDX92DBufferData::ReleaseReferences()
{
    ReleaseRTReferences();
}
//---------------------------------------------------------------------------
void NiDX92DBufferData::ReleaseRTReferences()
{
    if (m_pkSurface)
    {
        // This safely decrements the refcount.
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = 0;
    }

    NiDelete m_pkPixelFormat;
    m_pkPixelFormat = 0;
}
//---------------------------------------------------------------------------
bool NiDX92DBufferData::Shutdown()
{
    ReleaseReferences();
    return true;
}
//---------------------------------------------------------------------------
bool NiDX92DBufferData::Recreate(LPDIRECT3DDEVICE9)
{
    // Should be implemented, if necessary, by derived classes.
    return false;
}
//---------------------------------------------------------------------------
// NiDX9TextureBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9TextureBufferData,NiDX92DBufferData);

//---------------------------------------------------------------------------
NiDX9TextureBufferData::~NiDX9TextureBufferData()
{
    ReleaseTRTReferences();
}
//---------------------------------------------------------------------------
NiDX9TextureBufferData::NiDX9TextureBufferData()
{
    m_pkRenderTexture = NULL;
}
//---------------------------------------------------------------------------
NiDX9TextureBufferData* NiDX9TextureBufferData::Create(
    LPDIRECT3DTEXTURE9 pkD3DTexture, LPDIRECT3DDEVICE9, 
    Ni2DBuffer*& pkBuffer)
{
    NiDX9TextureBufferData* pkThis = NiNew NiDX9TextureBufferData;

    // We need our own refcount reference to the rendered texture
    // to safely manage the surface pointer.
    NIASSERT(pkD3DTexture);
    pkThis->m_pkRenderTexture = pkD3DTexture;
    D3D_POINTER_REFERENCE(pkThis->m_pkRenderTexture);

    // Our surface is actually the first miplevel of the rendered texture.
    HRESULT eD3dRet = pkD3DTexture->GetSurfaceLevel(0, 
        &pkThis->m_pkSurface);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9TextureBufferData::Create> FAILED"
            "- GetSurfaceLevel - %s", 
            NiDX9ErrorString((unsigned int)eD3dRet));

        NiDelete pkThis;
        return NULL;
    }

    // The description is required so that we can determine the size 
    // and pixel format.
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9TextureBufferData::Create> FAILED"
            "- m_pkSurface::GetDesc - %s", 
            NiDX9ErrorString((unsigned int)eD3dRet));
        NiDelete pkThis;
        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, kSurfDesc.Height,
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9TextureBufferData* NiDX9TextureBufferData::Create(
    LPDIRECT3DTEXTURE9 pkD3DTexture, Ni2DBuffer*& pkBuffer)
{
    NiDX9TextureBufferData* pkThis = NiNew NiDX9TextureBufferData;

    // We need our own refcount reference to the rendered texture
    // to safely manage the surface pointer.
    NIASSERT(pkD3DTexture);
    pkThis->m_pkRenderTexture = pkD3DTexture;
    D3D_POINTER_REFERENCE(pkThis->m_pkRenderTexture);

    // Our surface is actually the first miplevel of the rendered texture.
    HRESULT eD3dRet = pkD3DTexture->GetSurfaceLevel(0, 
        &pkThis->m_pkSurface);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9TextureBufferData::Create> FAILED"
            "- GetSurfaceLevel - %s", 
            NiDX9ErrorString((unsigned int)eD3dRet));

        NiDelete pkThis;

        return NULL;
    }

    // The description is required so that we can determine the size 
    // and pixel format.
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;

        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat = 
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, kSurfDesc.Height,
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9TextureBufferData* NiDX9TextureBufferData::CreateCubeFace(
    LPDIRECT3DCUBETEXTURE9 pkD3DCubeTexture, D3DCUBEMAP_FACES eFace, 
    NiDX9TextureBufferData*, LPDIRECT3DDEVICE9, 
    Ni2DBuffer*& pkBuffer)
{
    NiDX9TextureBufferData* pkThis = NiNew NiDX9TextureBufferData;

    // We need our own refcount reference to the rendered texture
    // to safely manage the surface pointer.
    NIASSERT(pkD3DCubeTexture);
    pkThis->m_pkRenderTexture = pkD3DCubeTexture;
    D3D_POINTER_REFERENCE(pkThis->m_pkRenderTexture);

    // Our surface is actually the first miplevel of the rendered cubemap.
    HRESULT eD3dRet = ((LPDIRECT3DCUBETEXTURE9)(pkThis->m_pkRenderTexture))
        ->GetCubeMapSurface((D3DCUBEMAP_FACES)eFace, 0, 
        &pkThis->m_pkSurface);
    
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9TextureBufferData::Create> FAILED"
            "- GetSurfaceLevel - %s", 
            NiDX9ErrorString((unsigned int)eD3dRet));
        pkThis->m_pkSurface = 0;

        NiDelete pkThis;

        return NULL;
    }

    // The description is required so that we can determine the size 
    // and pixel format.
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;

        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, kSurfDesc.Height,
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9TextureBufferData* NiDX9TextureBufferData::CreateCubeFace(
    LPDIRECT3DCUBETEXTURE9 pkD3DCubeTexture, D3DCUBEMAP_FACES eFace, 
    NiDX9TextureBufferData*, Ni2DBuffer*& pkBuffer)
{
     NiDX9TextureBufferData* pkThis = NiNew NiDX9TextureBufferData;

    // We need our own refcount reference to the rendered texture
    // to safely manage the surface pointer.
    NIASSERT(pkD3DCubeTexture);
    pkThis->m_pkRenderTexture = pkD3DCubeTexture;
    D3D_POINTER_REFERENCE(pkThis->m_pkRenderTexture);

    // Our surface is actually the first miplevel of the rendered cubemap.
    HRESULT eD3dRet = ((LPDIRECT3DCUBETEXTURE9)(pkThis->m_pkRenderTexture))
        ->GetCubeMapSurface((D3DCUBEMAP_FACES)eFace, 0, 
        &pkThis->m_pkSurface);
    
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9TextureBufferData::Create> FAILED"
            "- GetSurfaceLevel - %s", 
            NiDX9ErrorString((unsigned int)eD3dRet));
        pkThis->m_pkSurface = 0;

        NiDelete pkThis;

        return NULL;
    }

    // The description is required so that we can determine the size 
    // and pixel format.
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;

        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat = 
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, kSurfDesc.Height,
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
void NiDX9TextureBufferData::ReleaseReferences()
{
    ReleaseRTReferences();
    ReleaseTRTReferences();
}
//---------------------------------------------------------------------------
void NiDX9TextureBufferData::ReleaseTRTReferences()
{
    if (m_pkRenderTexture)
    {
        NiDX9Renderer::ReleaseTextureResource(
            (D3DTexturePtr)m_pkRenderTexture);
        m_pkRenderTexture = 0;
    }
}
//---------------------------------------------------------------------------
// NiDX9Direct3DBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9Direct3DBufferData ,
                NiDX92DBufferData);
//---------------------------------------------------------------------------
NiDX9Direct3DBufferData* NiDX9Direct3DBufferData::Create(
    LPDIRECT3DSURFACE9 pkD3DSurface, Ni2DBuffer*& pkBuffer)
{
    NiDX9Direct3DBufferData* pkThis = NiNew 
        NiDX9Direct3DBufferData();

    pkThis->m_pkSurface = pkD3DSurface;
    if (pkThis->m_pkSurface)
        pkThis->m_pkSurface->AddRef();

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, 
            kSurfDesc.Height, pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
void NiDX9Direct3DBufferData::PurgeBufferData()
{
    ReleaseRTReferences();
}
//---------------------------------------------------------------------------
bool NiDX9Direct3DBufferData::RecreateBufferData(
    LPDIRECT3DSURFACE9 pkD3DSurface)
{
    m_pkSurface = pkD3DSurface;
    if (m_pkSurface == NULL)
        return false;

    m_pkSurface->AddRef();

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;
        return false;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    m_pkPixelFormat = NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    return true;
}
//---------------------------------------------------------------------------
// NiDX9DepthStencilBuffer Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9DepthStencilBufferData, NiDX92DBufferData);
//---------------------------------------------------------------------------
NiDX9DepthStencilBufferData::~NiDX9DepthStencilBufferData()
{
    // If this is the currently set D/S buffer, un-set it on the device
    NiDX9Renderer* pkDX9Renderer = NiDX9Renderer::GetRenderer();
    if (pkDX9Renderer)
    {
        LPDIRECT3DDEVICE9 pkDevice = pkDX9Renderer->GetD3DDevice();
        NIASSERT(pkDevice);
        LPDIRECT3DSURFACE9 pkCurrentDS = NULL;
        HRESULT hr = pkDevice->GetDepthStencilSurface(&pkCurrentDS);
        if (SUCCEEDED(hr))
        {
            NIASSERT(pkCurrentDS);
            if (pkCurrentDS == m_pkSurface)
                SetNULLDepthStencilTarget(pkDX9Renderer->GetD3DDevice());
            pkCurrentDS->Release();
        }
    }
}
//---------------------------------------------------------------------------
bool NiDX9DepthStencilBufferData::SetDepthTarget(
    LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (!IsValid())
        return false;

    // Set depth stencil if first render target
    if (m_pkSurface != ms_pkCurrentDepthStencil)
    {
        HRESULT eResult = pkD3DDevice9->SetDepthStencilSurface(m_pkSurface);
        if (FAILED(eResult))
        {
            // Restore render target or no?
            return false;
        }
        ms_pkCurrentDepthStencil = m_pkSurface;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9DepthStencilBufferData::SetNULLDepthStencilTarget(
    LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    // Set depth stencil if first render target
    if (NULL != ms_pkCurrentDepthStencil)
    {
        HRESULT eResult = pkD3DDevice9->SetDepthStencilSurface(NULL);
        if (FAILED(eResult))
        {
            // Restore render target or no?
            return false;
        }
        ms_pkCurrentDepthStencil = NULL;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9DepthStencilBufferData::HasValidDepthBuffer()
{
    if (m_pkPixelFormat && m_pkSurface)
    {
        unsigned int uiBits = m_pkPixelFormat->GetBits(
            NiPixelFormat::COMP_DEPTH);
        if (uiBits > 0)
            return true;
        else
            return false;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiDX9DepthStencilBufferData::HasValidStencilBuffer()
{
    if (m_pkPixelFormat && m_pkSurface)
    {
        unsigned int uiBits = m_pkPixelFormat->GetBits(
            NiPixelFormat::COMP_STENCIL);
        if (uiBits > 0)
            return true;
        else
            return false;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
// NiDX9ImplicitDepthStencilBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9ImplicitDepthStencilBufferData,
    NiDX9DepthStencilBufferData);
//---------------------------------------------------------------------------
bool NiDX9ImplicitDepthStencilBufferData::Recreate(
    LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (IsValid())
        Shutdown();

    HRESULT eD3dRet = pkD3DDevice9->GetDepthStencilSurface(
        &m_pkSurface);

    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;
        return false;
    }
    
    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;

        return false;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    m_pkPixelFormat = NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    m_pkBuffer->ResetDimensions(kSurfDesc.Width, kSurfDesc.Height);

    return true;
}
//---------------------------------------------------------------------------
NiDX9ImplicitDepthStencilBufferData* 
NiDX9ImplicitDepthStencilBufferData::Create(LPDIRECT3DDEVICE9 pkD3DDevice9, 
    NiDepthStencilBuffer*& pkBuffer)
{
    NiDX9ImplicitDepthStencilBufferData* pkThis = NiNew 
        NiDX9ImplicitDepthStencilBufferData();

    HRESULT eD3dRet = pkD3DDevice9->GetDepthStencilSurface(
        &(pkThis->m_pkSurface));

    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = NiDepthStencilBuffer::Create(kSurfDesc.Width, 
            kSurfDesc.Height, pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    // The implicit depth buffer is the default depth/stencil surface.
    ms_pkCurrentDepthStencil = pkThis->m_pkSurface;

    return pkThis;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// NiDX9Direct3DDepthStencilBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9Direct3DDepthStencilBufferData,
                NiDX9DepthStencilBufferData);
//---------------------------------------------------------------------------
NiDX9Direct3DDepthStencilBufferData* 
    NiDX9Direct3DDepthStencilBufferData::Create(
    LPDIRECT3DSURFACE9 pkD3DDepthStencilSurface, 
    NiDepthStencilBuffer*& pkBuffer)
{
    NiDX9Direct3DDepthStencilBufferData* pkThis = NiNew 
        NiDX9Direct3DDepthStencilBufferData();

    pkThis->m_pkSurface = pkD3DDepthStencilSurface;
    if (pkThis->m_pkSurface)
        pkThis->m_pkSurface->AddRef();

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = NiDepthStencilBuffer::Create(kSurfDesc.Width, 
            kSurfDesc.Height, pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }

    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
void NiDX9Direct3DDepthStencilBufferData::PurgeBufferData()
{
    ReleaseRTReferences();
}
//---------------------------------------------------------------------------
bool NiDX9Direct3DDepthStencilBufferData::RecreateBufferData(
    LPDIRECT3DSURFACE9 pkD3DDepthStencilSurface)
{
    m_pkSurface = pkD3DDepthStencilSurface;
    if (m_pkSurface == NULL)
        return false;

    m_pkSurface->AddRef();

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;
        return false;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    m_pkPixelFormat = NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    return true;
}
//---------------------------------------------------------------------------
// NiDX9AdditionalDepthStencilBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9AdditionalDepthStencilBufferData,
    NiDX9DepthStencilBufferData);
//---------------------------------------------------------------------------
// Static variables
//---------------------------------------------------------------------------
NiCriticalSection 
NiDX9AdditionalDepthStencilBufferData::ms_kAddDSListCriticalSection;
NiTPointerList<NiDX9AdditionalDepthStencilBufferData*> 
    NiDX9AdditionalDepthStencilBufferData::ms_kAddDepthStencil;
//---------------------------------------------------------------------------
NiDX9AdditionalDepthStencilBufferData::
    NiDX9AdditionalDepthStencilBufferData() : m_eCreateFormat(D3DFMT_UNKNOWN)
{
    ms_kAddDSListCriticalSection.Lock();
    ms_kAddDepthStencil.AddHead(this);
    ms_kAddDSListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiDX9AdditionalDepthStencilBufferData::
    ~NiDX9AdditionalDepthStencilBufferData()
{
    ms_kAddDSListCriticalSection.Lock();
    ms_kAddDepthStencil.Remove(this);
    ms_kAddDSListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiDX9AdditionalDepthStencilBufferData::ShutdownAll()
{
    ms_kAddDSListCriticalSection.Lock();
    NiTListIterator kIter = ms_kAddDepthStencil.GetHeadPos();
    while (kIter)
    {
        NiDX9AdditionalDepthStencilBufferData* pkData = 
            ms_kAddDepthStencil.GetNext(kIter);
        NIASSERT(pkData);
        pkData->Shutdown();
    }
    ms_kAddDSListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiDX9AdditionalDepthStencilBufferData::RecreateAll(LPDIRECT3DDEVICE9 
    pkD3DDevice9)
{
    ms_kAddDSListCriticalSection.Lock();
    NiTListIterator kIter = ms_kAddDepthStencil.GetHeadPos();
    while (kIter)
    {
        NiDX9AdditionalDepthStencilBufferData* pkData = 
            ms_kAddDepthStencil.GetNext(kIter);
        NIASSERT(pkData);
        pkData->Recreate(pkD3DDevice9);
    }
    ms_kAddDSListCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
bool NiDX9AdditionalDepthStencilBufferData::Recreate(
    LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (IsValid())
        Shutdown();

    if (m_eCreateFormat == D3DFMT_UNKNOWN)
        return false;

    // Determine if the D3D format is compatible with the device.
    NiDX9Renderer* pkRenderer = (NiDX9Renderer*)NiDX9Renderer::GetRenderer();
    const NiRenderTargetGroup* pkDefaultRenderTargetGroup = 
        pkRenderer->GetDefaultRenderTargetGroup();
    NI_UNUSED_ARG(pkDefaultRenderTargetGroup);
    NIASSERT(pkDefaultRenderTargetGroup);

    HRESULT eD3DRet = pkRenderer->GetDirect3D()->CheckDeviceFormat( 
        pkRenderer->GetAdapter(), pkRenderer->GetDevType(), 
        pkRenderer->GetAdapterFormat(),
        D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, m_eCreateFormat);

    // The format is not compatible, the renderer data failed to create.
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9AdditionalDepthStencilBufferData::Recreate> FAILED %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return false;
    }

    BOOL bDiscard = false;

    unsigned int uiMultiSampleQuality = 0;
    D3DMULTISAMPLE_TYPE eMultisampleType = D3DMULTISAMPLE_NONE;
    GetMSAAD3DTypeAndQualityFromPref(m_eMSAAPref, 
        eMultisampleType, uiMultiSampleQuality);

    // Create the depth/stencil buffer.
    eD3DRet = pkD3DDevice9->CreateDepthStencilSurface(
        m_pkBuffer->GetWidth(), m_pkBuffer->GetHeight(), 
        m_eCreateFormat, eMultisampleType, uiMultiSampleQuality,
        bDiscard, &m_pkSurface, NULL);

    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9AdditionalDepthStencilBufferData::Recreate> FAILED %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;
        return false;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;
        return false;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    return true;
}
//---------------------------------------------------------------------------
NiDX9AdditionalDepthStencilBufferData* 
NiDX9AdditionalDepthStencilBufferData::Create(LPDIRECT3DDEVICE9 pkD3DDevice9, 
    NiDepthStencilBuffer*& pkBuffer, const NiPixelFormat* pkFormat,
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    // Check for a valid D3D representation
    if (pkFormat == NULL)
        return NULL;

    // Determine the D3D format from the pixel format
    D3DFORMAT eFormat = NiDX9PixelFormat::DetermineD3DFormat(*pkFormat);
    
    if (eFormat == D3DFMT_UNKNOWN)
        return NULL;

    // Determine if the D3D format is compatible with the device.
    NiDX9Renderer* pkRenderer = (NiDX9Renderer*)NiDX9Renderer::GetRenderer();
    const NiRenderTargetGroup* pkDefaultRenderTargetGroup = 
       pkRenderer->GetDefaultRenderTargetGroup();
    NI_UNUSED_ARG(pkDefaultRenderTargetGroup);
    NIASSERT(pkDefaultRenderTargetGroup);

    HRESULT eD3DRet = pkRenderer->GetDirect3D()->CheckDeviceFormat( 
        pkRenderer->GetAdapter(), pkRenderer->GetDevType(), 
        pkRenderer->GetAdapterFormat(),
        D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, eFormat);

    // The format is not compatible, the renderer data failed to create.
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9AdditionalDepthStencilBufferData::Create> FAILED %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return NULL;
    }

    NiDX9AdditionalDepthStencilBufferData* pkThis = NiNew 
        NiDX9AdditionalDepthStencilBufferData();

    NIASSERT(pkBuffer != NULL);

    unsigned int uiMultiSampleQuality = 0;
    D3DMULTISAMPLE_TYPE eMultisampleType = D3DMULTISAMPLE_NONE;
    GetMSAAD3DTypeAndQualityFromPref(eMSAAPref, 
        eMultisampleType, uiMultiSampleQuality);

    BOOL bDiscard = false;

    // Create the depth/stencil buffer.
    eD3DRet = pkD3DDevice9->CreateDepthStencilSurface(
        pkBuffer->GetWidth(), pkBuffer->GetHeight(), 
        eFormat, eMultisampleType, uiMultiSampleQuality,
        bDiscard, &pkThis->m_pkSurface, NULL);

    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Warning(
            "NiDX9AdditionalDepthStencilBufferData::Create> FAILED %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        
        NiDelete pkThis;
        return NULL;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    // Compute the NiRenderer representation of the D3D pixel format.
    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);
    pkThis->m_eCreateFormat = eFormat;

    pkBuffer->SetRendererData(pkThis);
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiDX9OnscreenBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9OnscreenBufferData,NiDX92DBufferData);

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiDX9ImplicitBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9ImplicitBufferData,NiDX9OnscreenBufferData);

//---------------------------------------------------------------------------
NiDX9ImplicitBufferData::NiDX9ImplicitBufferData()
{
    m_pkD3DDevice9 = NULL;
}
//---------------------------------------------------------------------------
NiDX9ImplicitBufferData* NiDX9ImplicitBufferData::Create(
    LPDIRECT3DDEVICE9 pkD3DDevice9, 
    const D3DPRESENT_PARAMETERS& kD3DPresentParams, Ni2DBuffer*& pkBuffer)
{
    NiDX9ImplicitBufferData* pkThis = NiNew NiDX9ImplicitBufferData;

    NiMemcpy(&pkThis->m_kD3DPresentParams, 
        sizeof(pkThis->m_kD3DPresentParams),&kD3DPresentParams,
        sizeof(kD3DPresentParams));

    pkThis->m_pkD3DDevice9 = pkD3DDevice9;
    D3D_POINTER_REFERENCE(pkThis->m_pkD3DDevice9);

    //  Grab the render target and depth-stencil target
    HRESULT eD3dRet = pkD3DDevice9->GetRenderTarget(0,
        &(pkThis->m_pkSurface));

    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    NiDX9Renderer::Message(
        "NiDX9Renderer::Create> Using %s backbuffer format\n",
        NiDX9SystemDesc::GetD3D9FormatString(kSurfDesc.Format));

    // Fill in multiple render target capabilities
    if (ms_uiNumBuffers == 0)
    {
        NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
        NiDX9Renderer* pkRenderer = (NiDX9Renderer*)NiRenderer::GetRenderer();

        ms_uiNumBuffers = pkRenderer->GetMaxBuffersPerRenderTargetGroup();
        ms_bIndependentBitDepths = 
            pkRenderer->GetIndependentBufferBitDepths();
    }

    // It is not an error if the target has no depth/stencil
//    if (FAILED(eD3dRet))
//    {
//
//    }

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, kSurfDesc.Height,
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9ImplicitBufferData::~NiDX9ImplicitBufferData()
{
    ReleaseIRTReferences();
}
//---------------------------------------------------------------------------
bool NiDX9ImplicitBufferData::DisplayFrame()
{
    HRESULT eResult = m_pkD3DDevice9->Present(NULL, NULL, NULL, NULL);
    return SUCCEEDED(eResult);
}
//---------------------------------------------------------------------------
bool NiDX9ImplicitBufferData::Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (IsValid())
        Shutdown();

    ms_apkCurrentBuffer[0] = NULL;
    ms_pkCurrentDepthStencil = NULL;

    // If we have an invalid object with a valid device
    if (m_pkD3DDevice9)
    {
        NiDX9Renderer::ReleaseDevice(m_pkD3DDevice9);
        m_pkD3DDevice9 = NULL;
    }

    m_pkD3DDevice9 = pkD3DDevice9;
    D3D_POINTER_REFERENCE(m_pkD3DDevice9);

    //  Grab the render target and depth-stencil target
    HRESULT eD3dRet = m_pkD3DDevice9->GetRenderTarget(0, &(m_pkSurface));
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;
        return false;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    eD3dRet = m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;

        return false;
    }

    m_pkBuffer->ResetDimensions(kSurfDesc.Width, kSurfDesc.Height);

    m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    NiDX9Renderer::Message(
        "NiDX9Renderer::Recreate> Using %s backbuffer format\n",
        NiDX9SystemDesc::GetD3D9FormatString(kSurfDesc.Format));

    return true;
}
//---------------------------------------------------------------------------
void NiDX9ImplicitBufferData::ReleaseReferences()
{
    ReleaseRTReferences();
    ReleaseIRTReferences();
}
//---------------------------------------------------------------------------
void NiDX9ImplicitBufferData::ReleaseIRTReferences()
{
    if (m_pkD3DDevice9)
    {
        NiDX9Renderer::ReleaseDevice(m_pkD3DDevice9);
        m_pkD3DDevice9 = 0;
    }
}

//---------------------------------------------------------------------------
// NiDX9SwapChainBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9SwapChainBufferData,NiDX9OnscreenBufferData);
//---------------------------------------------------------------------------

NiDX9SwapChainBufferData::NiDX9SwapChainBufferData()
{
    m_pkSwapChain = NULL;
}
//---------------------------------------------------------------------------
NiDX9SwapChainBufferData::~NiDX9SwapChainBufferData()
{
    ReleaseSCRTReferences();
}
//---------------------------------------------------------------------------
NiDX9SwapChainBufferData* NiDX9SwapChainBufferData::Create(
    LPDIRECT3DDEVICE9 pkD3DDevice9, 
    const D3DPRESENT_PARAMETERS& kD3DPresentParams, Ni2DBuffer*& pkBuffer)
{
    NiDX9SwapChainBufferData* pkThis = NiNew NiDX9SwapChainBufferData;

    NiMemcpy(&pkThis->m_kD3DPresentParams, 
        sizeof(pkThis->m_kD3DPresentParams), &kD3DPresentParams, 
        sizeof(kD3DPresentParams));

    if (pkD3DDevice9->CreateAdditionalSwapChain(
        &pkThis->m_kD3DPresentParams, &pkThis->m_pkSwapChain))
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkSurface = NULL;

    //  Grab the render target and depth-stencil target
    HRESULT eD3DRet = pkThis->m_pkSwapChain->GetBackBuffer(0, 
        D3DBACKBUFFER_TYPE_MONO, &pkThis->m_pkSurface);

    if (FAILED(eD3DRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    eD3DRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3DRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    NiDX9Renderer::Message(
        "NiDX9Renderer::Create> Using %s backbuffer format\n",
        NiDX9SystemDesc::GetD3D9FormatString(kSurfDesc.Format));

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = Ni2DBuffer::Create(kSurfDesc.Width, kSurfDesc.Height,
            pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    return pkThis;
}
//---------------------------------------------------------------------------
bool NiDX9SwapChainBufferData::DisplayFrame()
{
    HRESULT eResult = m_pkSwapChain->Present(NULL, NULL, NULL, NULL, 0);
    return SUCCEEDED(eResult);
}
//---------------------------------------------------------------------------
bool NiDX9SwapChainBufferData::Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (IsValid())
        Shutdown();

    ms_apkCurrentBuffer[0] = NULL;
    ms_pkCurrentDepthStencil = NULL;

    m_pkSwapChain = NULL;
    m_pkSurface = NULL;
    if (!pkD3DDevice9->CreateAdditionalSwapChain(
        &m_kD3DPresentParams, &m_pkSwapChain))
    {
        //  Grab the render target and depth-stencil target
        m_pkSwapChain->GetBackBuffer(0, 
            D3DBACKBUFFER_TYPE_MONO, &m_pkSurface);

        //  Get the surface desc...
        D3DSURFACE_DESC kSurfDesc;
        if (m_pkSurface->GetDesc(&kSurfDesc) == 0)
        {
            m_pkPixelFormat = 
                NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

            NiDX9Renderer::Message(
                "NiDX9Renderer::Recreate> Using %s backbuffer format\n",
                NiDX9SystemDesc::GetD3D9FormatString(kSurfDesc.Format));
        }
        else
        {
            NiDX9Renderer::ReleaseResource(m_pkSurface);
            m_pkSurface = 0;

            NiDX9Renderer::ReleaseSwapChain(m_pkSwapChain);
            m_pkSwapChain = 0;

            return false;
        }

        m_pkBuffer->ResetDimensions(kSurfDesc.Width, kSurfDesc.Height);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiDX9SwapChainBufferData::ReleaseReferences()
{
    ReleaseRTReferences();
    ReleaseSCRTReferences();
}
//---------------------------------------------------------------------------
void NiDX9SwapChainBufferData::ReleaseSCRTReferences()
{
    if (m_pkSwapChain)
    {
        NiDX9Renderer::ReleaseSwapChain(m_pkSwapChain);
        m_pkSwapChain = 0;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// NiDX9SwapChainDepthStencilBufferData Class Implementation
//---------------------------------------------------------------------------
NiImplementRTTI(NiDX9SwapChainDepthStencilBufferData,
    NiDX9DepthStencilBufferData);
//---------------------------------------------------------------------------
bool NiDX9SwapChainDepthStencilBufferData::Recreate(
    LPDIRECT3DDEVICE9 pkD3DDevice9)
{
    if (IsValid())
        Shutdown();

    NIASSERT(m_pkSwapChainBufferData);
    D3DPRESENT_PARAMETERS& kD3DPresentParams = 
        m_pkSwapChainBufferData->GetPresentParams();

    LPDIRECT3DSURFACE9 pkDepthStencil = NULL;
    pkD3DDevice9->CreateDepthStencilSurface(
        m_pkSwapChainBufferData->GetWidth(), 
        m_pkSwapChainBufferData->GetHeight(), 
        kD3DPresentParams.AutoDepthStencilFormat,
        kD3DPresentParams.MultiSampleType, 
        kD3DPresentParams.MultiSampleQuality, 
        ((kD3DPresentParams.Flags | 
        D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL) != 0), 
        &pkDepthStencil, NULL);

    m_pkSurface = pkDepthStencil;

    NiDX9Renderer::Message(
        "NiDX9Renderer::Recreate> Using %s depth/stencil format\n",
        NiDX9SystemDesc::GetD3D9FormatString(
        kD3DPresentParams.AutoDepthStencilFormat));
    
    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDX9Renderer::ReleaseResource(m_pkSurface);
        m_pkSurface = NULL;

        return false;
    }
    
    m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    m_pkBuffer->ResetDimensions(kSurfDesc.Width, kSurfDesc.Height);
    return true;
}
//---------------------------------------------------------------------------
NiDX9SwapChainDepthStencilBufferData* 
NiDX9SwapChainDepthStencilBufferData::Create(LPDIRECT3DDEVICE9 pkD3DDevice9, 
    NiDX9SwapChainBufferData* pkSwapChainBufferData, 
    NiDepthStencilBuffer*& pkBuffer)
{
    NIASSERT(pkSwapChainBufferData);
    NiDX9SwapChainDepthStencilBufferData* pkThis = NiNew 
        NiDX9SwapChainDepthStencilBufferData();

    pkThis->m_pkSwapChainBufferData = pkSwapChainBufferData;
    
    D3DPRESENT_PARAMETERS& kD3DPresentParams = 
        pkSwapChainBufferData->GetPresentParams();

    LPDIRECT3DSURFACE9 pkDepthStencil = NULL;
    if (SUCCEEDED(pkD3DDevice9->CreateDepthStencilSurface(
        pkSwapChainBufferData->GetWidth(), 
        pkSwapChainBufferData->GetHeight(), 
        kD3DPresentParams.AutoDepthStencilFormat,
        kD3DPresentParams.MultiSampleType, 
        kD3DPresentParams.MultiSampleQuality, 
        ((kD3DPresentParams.Flags | 
        D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL) != 0),
        &pkDepthStencil, NULL)))
    {
        pkThis->m_pkSurface = pkDepthStencil;
    }
    else
    {
        NiDelete pkThis;
        return NULL;
    }

    //  Get the surface desc...
    D3DSURFACE_DESC kSurfDesc;
    HRESULT eD3dRet = pkThis->m_pkSurface->GetDesc(&kSurfDesc);
    if (FAILED(eD3dRet))
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkPixelFormat =
        NiDX9PixelFormat::CreateFromD3DFormat(kSurfDesc.Format);

    if (pkBuffer == NULL)
    {
        // Create the buffer if it did not exist prior to this. Note
        // that it is assumed that the app will refcount this
        // buffer after this has been created otherwise it will leak.
        pkBuffer = NiDepthStencilBuffer::Create(kSurfDesc.Width, 
            kSurfDesc.Height, pkThis);
    }
    else
    {
        pkBuffer->SetRendererData(pkThis);
    }
    pkThis->m_pkBuffer = pkBuffer;
    pkThis->m_eMSAAPref = GetMSAAPrefFromD3DTypeAndQuality(
        kSurfDesc.MultiSampleType, kSurfDesc.MultiSampleQuality);

    NiDX9Renderer::Message(
        "NiDX9SwapChainDepthStencilBufferData::Create> Using %s depth/stencil"
        " format\n", NiDX9SystemDesc::GetD3D9FormatString(
        kD3DPresentParams.AutoDepthStencilFormat));

    return pkThis;
}
//---------------------------------------------------------------------------
