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

#include "NiDX9RenderedTextureData.h"
#include "NiDX9Renderer.h"
#include "NiDX92DBufferData.h"
#include <NiRenderedTexture.h>

//---------------------------------------------------------------------------
NiDX9RenderedTextureData* NiDX9RenderedTextureData::Create(
    NiRenderedTexture* pkTexture, NiDX9Renderer* pkRenderer)
{
    NiDX9RenderedTextureData* pkThis = NiNew NiDX9RenderedTextureData(
        pkTexture, pkRenderer);

    const NiPixelFormat* pkFmt = pkThis->CreateSurf(pkTexture);
    if (!pkFmt)
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkTexture->SetRendererData(pkThis);

    D3DBaseTexturePtr pkD3DBaseTex = pkThis->GetD3DTexture();
    NIASSERT(pkD3DBaseTex != NULL);
    
    // Grab the Ni2DBuffer from the NiRenderedTexture. This will
    // hold the DX9 Surface that we are rendering to.
    Ni2DBuffer* pkBuffer = pkTexture->GetBuffer();
    NIASSERT(pkBuffer != NULL);
    
    D3DTexturePtr pkD3DTex = NULL;
    // To get the 1st mip surface of the texture, we must have a 
    // Direct3DTexture9. Note that the QueryInterface method bumps the 
    // refcount up one.
    HRESULT hr = pkD3DBaseTex->QueryInterface(IID_IDirect3DTexture9, 
        (LPVOID *)&pkD3DTex);

    if (FAILED(hr))
    {
        NiDX9Renderer::Warning("NiDX9RenderedTextureData::Create> "
           "Failed QueryInterface IID_IDirect3DTexture9 - %s", 
            NiDX9ErrorString((unsigned int)hr));
        NiDelete pkThis;
        return NULL;
    }
    
    // Create the 2D buffer data. This will manage the DX9 surface
    // and automatically fills in the Ni2DBuffer::RendererData for us
    // as a side effect.
    NiDX92DBufferData* pkBuffData = NiDX9TextureBufferData::Create(pkD3DTex, 
        pkBuffer);
    
    // Decrement the refcount from the QueryInterface call or else we leak.
    NiDX9Renderer::ReleaseResource(pkD3DTex);

    if (!pkBuffData)
    {
        NiDX9Renderer::Warning("NiDX9RenderedTextureData::Create> "
           "Failed NiDX92DBufferData::Create - %s", "");
        NiDelete pkThis;
        return NULL;
    }

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9RenderedTextureData::NiDX9RenderedTextureData(
    NiRenderedTexture* pkTexture, NiDX9Renderer* pkRenderer) : 
    NiDX9TextureData(pkTexture, pkRenderer)
{
    NIASSERT(pkTexture && pkRenderer);
    m_usTextureType |= TEXTURETYPE_RENDERED;
}
//---------------------------------------------------------------------------
NiDX9RenderedTextureData::~NiDX9RenderedTextureData()
{
    m_pkRenderer->RemoveRenderedTextureData((NiRenderedTexture*)m_pkTexture);
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9RenderedTextureData::CreateSurf(
    const NiRenderedTexture* pkTexture)
{
    if (pkTexture == 0)
        return NULL;

    m_uiWidth = pkTexture->GetWidth();
    m_uiHeight = pkTexture->GetHeight();
    m_usLevels = 1;

    //  Need a format...
    // Determine the desired pixel format for the buffer - for now, just look
    // at the alpha
    const NiTexture::FormatPrefs kPrefs = pkTexture->GetFormatPreferences();
    NiPixelFormat** ppkDestFmts = m_pkRenderer->GetTextureFormatArray(
        NiDX9Renderer::TEXUSE_RENDERED_TEX);

    const NiPixelFormat* pkFmt = FindClosestPixelFormat(kPrefs,
        ppkDestFmts);

    if (pkFmt == NULL)
        return NULL;

    m_kPixelFormat = *pkFmt;

    D3DFORMAT eD3DFmt = (D3DFORMAT)pkFmt->GetRendererHint();

    LPDIRECT3DDEVICE9 pkD3DDevice9 = m_pkRenderer->GetD3DDevice();
    D3DTexturePtr pkD3DTexture = NULL;

    HRESULT eD3DRet = pkD3DDevice9->CreateTexture(m_uiWidth, m_uiHeight, 1, 
        D3DUSAGE_RENDERTARGET, eD3DFmt, D3DPOOL_DEFAULT,
        &pkD3DTexture, NULL);
    
    if (FAILED(eD3DRet))
    {
       NiDX9Renderer::Warning("NiDX9RenderedTextureData::CreateSurf> "
           "Failed CreateImageSurface - %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        m_pkD3DTexture = NULL;
        return NULL;
    }
    m_pkD3DTexture = pkD3DTexture;

    D3D_POINTER_CHECK(m_pkD3DTexture);

    return pkFmt;
}
//---------------------------------------------------------------------------
