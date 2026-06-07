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

#include "NiDX9DynamicTextureData.h"
#include "NiDX9Renderer.h"

//---------------------------------------------------------------------------
NiDX9DynamicTextureData* NiDX9DynamicTextureData::Create(
    NiDynamicTexture* pkTexture)
{
    if (pkTexture->GetTiled())
    {
        NiDX9Renderer::Warning("NiDX9DynamicTextureData Failed> "
            "Can not create a tiled dynamic texture.\n");
        return NULL;
    }

    NiDX9Renderer* pkRenderer = (NiDX9Renderer*)NiRenderer::GetRenderer();

    NiDX9DynamicTextureData* pkThis = NiNew NiDX9DynamicTextureData(
        pkTexture, pkRenderer);

    bool bDynTexturesCap = pkRenderer->IsDynamicTexturesCapable();

    const NiPixelFormat* pkFmt =
        pkThis->CreateSurf(pkTexture, bDynTexturesCap);
    if (!pkFmt)
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkTexture->SetRendererData(pkThis);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9DynamicTextureData::NiDX9DynamicTextureData(
    NiDynamicTexture* pkTexture, NiDX9Renderer* pkRenderer) : 
    NiDX9TextureData(pkTexture, pkRenderer), m_bTextureLocked(false),
    m_pkD3DLockableSysMemTexture(NULL)
{
    NIASSERT(pkTexture && pkRenderer);
    m_usTextureType |= TEXTURETYPE_DYNAMIC;
}
//---------------------------------------------------------------------------
NiDX9DynamicTextureData::~NiDX9DynamicTextureData()
{
    m_pkRenderer->RemoveDynamicTextureData((NiDynamicTexture*)m_pkTexture);
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9DynamicTextureData::CreateSurf(
    const NiDynamicTexture* pkTexture, bool bDynTexturesCap)
{
    if (pkTexture == 0)
        return NULL;

    m_uiWidth = pkTexture->GetWidth();
    m_uiHeight = pkTexture->GetHeight();
    m_usLevels = 1;

    // Need a format.  Determine the desired pixel format for the buffer.  For
    // now, just look at the alpha.
    const NiTexture::FormatPrefs kPrefs = pkTexture->GetFormatPreferences();
    NiPixelFormat** ppkDestFmts = m_pkRenderer->GetTextureFormatArray(
        NiDX9Renderer::TEXUSE_TEX);

    const NiPixelFormat* pkFmt = FindClosestPixelFormat(kPrefs,
        ppkDestFmts);

    if (pkFmt == NULL)
        return NULL;

    m_kPixelFormat = *pkFmt;

    D3DFORMAT eD3DFmt = (D3DFORMAT)pkFmt->GetRendererHint();

    LPDIRECT3DDEVICE9 pkD3DDevice9 = m_pkRenderer->GetD3DDevice();
    D3DTexturePtr pkD3DTexture = NULL;

    if (bDynTexturesCap)
    {
        // The video card supports dynamic textures (meaning lockable textures
        // in video memory), so allocate the texture in video card memory.
        HRESULT eD3DRet = pkD3DDevice9->CreateTexture(m_uiWidth, m_uiHeight, 1,
            D3DUSAGE_DYNAMIC, eD3DFmt, D3DPOOL_DEFAULT, &pkD3DTexture, NULL);
        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Warning("NiDX9DynamicTextureData::CreateSurf> "
            "Failed CreateImageSurface - %s", 
                NiDX9ErrorString((unsigned int)eD3DRet));
            m_pkD3DTexture = NULL;
            return NULL;
        }

        m_pkD3DTexture = pkD3DTexture;
        D3D_POINTER_CHECK(m_pkD3DTexture);
    }
    else
    {
        // The video card does not support dynamic textures, so allocate an
        // unlockable texture in video memory and a lockable texture in system
        // memory.
        // (Later, the system memory texture may be locked and modified; then
        // the video memory texture may be updated via
        // IDirect3DDevice9::UpdateTexture().)

        // Allocate the video memory.
        HRESULT eD3DRet = pkD3DDevice9->CreateTexture(m_uiWidth, m_uiHeight, 1,
            0, eD3DFmt, D3DPOOL_DEFAULT, &pkD3DTexture, NULL);
        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Warning("NiDX9DynamicTextureData::CreateSurf> "
            "Failed CreateImageSurface - %s", 
                NiDX9ErrorString((unsigned int)eD3DRet));
            m_pkD3DTexture = NULL;
            return NULL;
        }
        
        m_pkD3DTexture = pkD3DTexture;
        D3D_POINTER_CHECK(m_pkD3DTexture);
        pkD3DTexture = NULL;

        // Allocate the system memory.
        eD3DRet = pkD3DDevice9->CreateTexture(m_uiWidth, m_uiHeight, 1, 
            0, eD3DFmt, D3DPOOL_SYSTEMMEM, &pkD3DTexture, NULL);
        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Warning("NiDX9DynamicTextureData::CreateSurf> "
            "Failed CreateImageSurface - %s", 
                NiDX9ErrorString((unsigned int)eD3DRet));
            m_pkD3DLockableSysMemTexture = NULL;
            return NULL;
        }

        m_pkD3DLockableSysMemTexture = pkD3DTexture;
        D3D_POINTER_CHECK(m_pkD3DLockableSysMemTexture);
    }

    return pkFmt;
}
//---------------------------------------------------------------------------
