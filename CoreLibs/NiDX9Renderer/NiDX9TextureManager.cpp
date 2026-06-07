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

#include "NiDX9TextureManager.h"
#include "NiDX9Renderer.h"
#include "NiDX9TextureData.h"
#include "NiDX9SourceTextureData.h"
#include "NiDX9RenderedTextureData.h"
#include "NiDX9DynamicTextureData.h"

//---------------------------------------------------------------------------
NiDX9TextureManager::NiDX9TextureManager(NiDX9Renderer* pkRenderer)
{
    NIASSERT(pkRenderer);
    m_pkRenderer = pkRenderer;
    m_pkD3DDevice9 = m_pkRenderer->GetD3DDevice();
    D3D_POINTER_REFERENCE(m_pkD3DDevice9);
}
//---------------------------------------------------------------------------
NiDX9TextureManager::~NiDX9TextureManager()
{
    NiDX9Renderer::ReleaseDevice(m_pkD3DDevice9);
    m_pkD3DDevice9 = 0;
}
//---------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 NiDX9TextureManager::GetTexture(unsigned int, 
    unsigned int, unsigned int, unsigned int, D3DFORMAT, D3DPOOL)
{
    return NULL;
}
//---------------------------------------------------------------------------
LPDIRECT3DBASETEXTURE9 NiDX9TextureManager::PrepareTextureForRendering(
    NiTexture* pkNewTexIm, bool& bChanged, bool& bMipmap, bool& bNonPow2)
{
    bChanged = false;
    bMipmap = false;
    bNonPow2 = false;
            
    if (!pkNewTexIm)
        return NULL;
   
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    pkRenderer->LockSourceDataCriticalSection();
    // We now know that pkNewTexIm is valid - so, we get the texture data
    NiDX9TextureData* pkTexData = 
        (NiDX9TextureData*)pkNewTexIm->GetRendererData();

    if (!pkTexData)
    {
        NiSourceTexture* pkSourceTex = 
            NiDynamicCast(NiSourceTexture, pkNewTexIm);

        if (pkSourceTex)
        {
            pkTexData = NiDX9SourceTextureData::Create(pkSourceTex, 
                m_pkRenderer);
        }
        else
        {
            // By definition, a rendered texture must have been created for
            // a given renderer.  If we don't already have a texture data
            // object, we never will.
            pkRenderer->UnlockSourceDataCriticalSection();
            return NULL;
        }

        bChanged = true;
    }

    unsigned int uiWidth = pkTexData->GetWidth();
    unsigned int uiHeight = pkTexData->GetHeight();

    pkRenderer->UnlockSourceDataCriticalSection();

    if (!(NiIsPowerOf2(uiWidth) && NiIsPowerOf2(uiHeight)))
    {
        bNonPow2 = true;
    }

    // BEGIN MYTHIC TUNING
    if (pkTexData->IsSourceTexture())
    {
        NiDX9SourceTextureData* pkSourceTexData =
            (NiDX9SourceTextureData *)pkTexData;

        if (!pkTexData->GetD3DTexture())
        {
            pkSourceTexData->Update();
            bChanged = true;
        }
        else 
        {
            // skip any form of updating if the texture is static
            if (!((NiSourceTexture*)pkNewTexIm)->GetStatic())
            {
                pkSourceTexData->Update();
            }
        }
    }
    NIASSERT(pkTexData->GetLevels() == 1 ||
        (!pkTexData->IsRenderedTexture() && !pkTexData->IsDynamicTexture()));

    bMipmap = (pkTexData->GetLevels() > 1) ? true : false;

    return pkTexData->GetD3DTexture();
}
//---------------------------------------------------------------------------
bool NiDX9TextureManager::PrecacheTexture(NiTexture* pkIm)
{
    if (!pkIm)
        return false;
    
    // We now know that pkNewTexIm is valid - so, we get the texture data
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    pkRenderer->LockSourceDataCriticalSection();
    NiDX9TextureData* pkTexData = 
        (NiDX9TextureData*)pkIm->GetRendererData();

    if (!pkTexData)
    {
        NiSourceTexture* pkSourceTex = NiDynamicCast(NiSourceTexture, pkIm);

        if (pkSourceTex)
        {
            pkTexData = NiDX9SourceTextureData::Create(pkSourceTex, 
                m_pkRenderer);

            // Check to see if the texture data can be released
            if (NiSourceTexture::GetDestroyAppDataFlag() && 
                pkSourceTex->GetRendererData() && 
                pkSourceTex->GetStatic())
            {
                pkSourceTex->DestroyAppPixelData();
            }
        }
        else
        {
            NiSourceCubeMap* pkSourceCube = 
                NiDynamicCast(NiSourceCubeMap, pkIm);
            if (pkSourceCube)
            {
                pkTexData = NiDX9SourceCubeMapData::Create(pkSourceCube, 
                    m_pkRenderer);

                // Check to see if the texture data can be released
                if (NiSourceTexture::GetDestroyAppDataFlag() && 
                    pkSourceCube->GetRendererData() && 
                    pkSourceCube->GetStatic())
                {
                    pkSourceCube->DestroyAppPixelData();
                }
            }
            else
            {
                // By definition, a non-source texture must have been created 
                // for a given renderer.  If we don't already have a texture 
                // data object, we never will.
                pkRenderer->UnlockSourceDataCriticalSection();
                return false;
            }
        }
    }    
    pkRenderer->UnlockSourceDataCriticalSection();

    NiDX9RenderedTextureData* pkRendered = pkTexData->GetAsRenderedTexture();
    if (pkRendered) // renderer texture
    {
        if (pkRendered->GetD3DTexture())
            return true;
        else
            return false;
    }

    NiDX9DynamicTextureData* pkDynamicTexData
        = pkTexData->GetAsDynamicTexture();
    if (pkDynamicTexData) // renderer texture
    {
        if (pkDynamicTexData->GetD3DTexture())
            return true;
        else
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiDX9TextureManager::GetFormatFromTexture(NiTexture& kIm)
{
    NiDX9TextureData* pkTexData = (NiDX9TextureData*)kIm.GetRendererData();

    if (pkTexData)
    {
        return ((NiPixelFormat*)
            (pkTexData->GetPixelFormat()))->GetExtraData();
    }
    else
    {
        // must allocate data - should happen at most once for any image
        // this should only happen for source textures that were created
        // before the renderer was created
        NiSourceTexture* pkSource = NiDynamicCast(NiSourceTexture, (&kIm));
        if (pkSource)
        {
            if (m_pkRenderer->CreateSourceTextureRendererData(pkSource))
            {
                pkTexData = (NiDX9TextureData*)kIm.GetRendererData();
                if (pkTexData)
                {
                    return ((NiPixelFormat*)
                        (pkTexData->GetPixelFormat()))->GetExtraData();
                }
            }
        }

        return NiDX9Renderer::TEX_NUM;
    }
}
//---------------------------------------------------------------------------
