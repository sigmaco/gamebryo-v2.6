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

#include "NiDX9RenderedCubeMapData.h"
#include "NiDX9Renderer.h"
#include "NiDX92DBufferData.h"

//---------------------------------------------------------------------------
NiDX9RenderedCubeMapData* NiDX9RenderedCubeMapData::Create(
    NiRenderedCubeMap* pkTexture, NiDX9Renderer* pkRenderer)
{
    NiDX9RenderedCubeMapData* pkThis = NiNew NiDX9RenderedCubeMapData(
        pkTexture, pkRenderer);

    const NiPixelFormat* pkFmt = pkThis->CreateSurf(pkTexture);

    if (!pkFmt)
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkTexture->SetRendererData(pkThis);

    NiDX9TextureBufferData* pkFirstBuffData = NULL;

    // For each face of the cube map, we need to retrieve a D3D Surface..
    // This D3D Surface is what we render to.
    for (unsigned int ui = 0; ui < NiRenderedCubeMap::FACE_NUM; ui++)
    {
        D3DBaseTexturePtr spD3DBaseTex =
            pkThis->GetD3DTexture();
        NI_UNUSED_ARG(spD3DBaseTex);
        NIASSERT(spD3DBaseTex != NULL);
        
        // Get the Ni2DBuffer for the current face. It's renderer
        // data will eventually contain the surface pointer.
        Ni2DBuffer* pkBuffer = pkTexture->GetFaceBuffer(
            (NiRenderedCubeMap::FaceID)ui);
        NIASSERT(pkBuffer != NULL);

        // Create the 2D buffer data. This will manage the DX9 surface
        // and automatically fills in the Ni2DBuffer::RendererData for us
        // as a side effect. We assume that the D3DBaseTexture pointer
        // is in reality a Direct3DCubeTexture9. This should be a safe
        // assumption.
        NiDX9TextureBufferData* pkBuffData = 
            NiDX9TextureBufferData::CreateCubeFace(
            (LPDIRECT3DCUBETEXTURE9)pkThis->GetD3DTexture(), 
            (D3DCUBEMAP_FACES)ui, pkFirstBuffData, 
            pkRenderer->GetD3DDevice(), pkBuffer);
               
        if (!pkBuffData)
        {
            NiDX9Renderer::Warning("NiDX9RenderedCubeMapData::Create> "
                "NiDX9TextureBufferData::CreateCubeFace - %s", "");
            NiDelete pkThis;
            return NULL;
        }

        if (pkFirstBuffData == NULL)
            pkFirstBuffData = pkBuffData;
    }

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9RenderedCubeMapData::NiDX9RenderedCubeMapData(
    NiRenderedCubeMap* pkTexture, NiDX9Renderer* pkRenderer) :
    NiDX9RenderedTextureData(pkTexture, pkRenderer)
{
}
//---------------------------------------------------------------------------
NiDX9RenderedCubeMapData::~NiDX9RenderedCubeMapData()
{
    m_pkRenderer->RemoveRenderedCubeMapData((NiRenderedCubeMap*)m_pkTexture);
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9RenderedCubeMapData::CreateSurf(
    const NiRenderedCubeMap* pkTexture)
{
    if (pkTexture == 0)
        return NULL;

    m_uiWidth = m_uiHeight = pkTexture->GetWidth();
    m_usLevels = 1;

    //  Need a format...
    // Determine the desired pixel format for the buffer - for now, just look
    // at the alpha
    const NiTexture::FormatPrefs kPrefs = pkTexture->GetFormatPreferences();
    NiPixelFormat** ppkDestFmts = m_pkRenderer->GetTextureFormatArray(
        NiDX9Renderer::TEXUSE_RENDERED_CUBE);
    const NiPixelFormat* pkFmt = FindClosestPixelFormat(kPrefs,
        ppkDestFmts);

    if (pkFmt == NULL)
        return NULL;

    D3DFORMAT eD3DFmt = (D3DFORMAT)pkFmt->GetRendererHint();

    LPDIRECT3DDEVICE9 pkD3DDevice9 = m_pkRenderer->GetD3DDevice();
    LPDIRECT3DCUBETEXTURE9 pkD3DCubeMap = NULL;

    HRESULT eD3DRet = pkD3DDevice9->CreateCubeTexture(m_uiWidth, 1,
        D3DUSAGE_RENDERTARGET, eD3DFmt, D3DPOOL_DEFAULT, &pkD3DCubeMap,
        NULL);
    if (FAILED(eD3DRet))
    {
       NiDX9Renderer::Warning("NiDX9RenderedCubeMapData::CreateSurf> "
           "Failed CreateImageSurface - %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        m_pkD3DTexture = NULL;
        return NULL;
    }
    m_pkD3DTexture = pkD3DCubeMap;
    
    return pkFmt;
}
//---------------------------------------------------------------------------
