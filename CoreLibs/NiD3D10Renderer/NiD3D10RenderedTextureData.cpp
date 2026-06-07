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
#include "NiD3D10PixelFormat.h"
#include "NiD3D10RenderedTextureData.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ResourceManager.h"

#include <Ni2DBuffer.h>
#include <NiRenderedCubeMap.h>
#include <NiRenderedTexture.h>

//---------------------------------------------------------------------------
NiD3D10RenderedTextureData::NiD3D10RenderedTextureData(
    NiRenderedTexture* pkTexture) : 
    NiD3D10TextureData(pkTexture),
    m_usNumTextures(1)
{
    m_usTextureType |= TEXTURETYPE_SOURCE;
    if (NiIsKindOf(NiRenderedCubeMap, pkTexture))
    {
        m_usNumTextures = 6;
        m_usTextureType |= TEXTURETYPE_CUBE;
    }
}
//---------------------------------------------------------------------------
NiD3D10RenderedTextureData::~NiD3D10RenderedTextureData()
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10RenderedTextureData* NiD3D10RenderedTextureData::Create(
    NiRenderedTexture* pkTexture, Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    NiD3D10RenderedTextureData* pkThis = NiNew NiD3D10RenderedTextureData(
        pkTexture);

    bool bSuccess = pkThis->PrepareTexture(eMSAAPref);

    NIASSERT(bSuccess == false || pkTexture->GetRendererData() == pkThis);

    if (bSuccess)
    {
        return pkThis;
    }
    else
    {
        NiDelete pkThis;
        return NULL;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10RenderedTextureData::PrepareTexture(
    Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    if (m_pkTexture == NULL)
        return false;

    NIASSERT(NiIsKindOf(NiRenderedTexture, m_pkTexture));
    NiRenderedTexture* pkTexture = (NiRenderedTexture*)m_pkTexture;

    m_uiWidth = pkTexture->GetWidth();
    m_uiHeight = pkTexture->GetHeight();
    m_usLevels = 1;

    NIASSERT(m_usNumTextures == (IsCubeMap() ? 6 : 1));

    const NiTexture::FormatPrefs kPrefs = pkTexture->GetFormatPreferences();
    bool bCube = IsCubeMap();

    const NiPixelFormat* pkFmt = FindMatchingPixelFormat(kPrefs, bCube ? 
        D3D10_FORMAT_SUPPORT_TEXTURECUBE : D3D10_FORMAT_SUPPORT_TEXTURE2D);

    m_kPixelFormat = *pkFmt;

    DXGI_FORMAT eFormat = 
        NiD3D10PixelFormat::DetermineDXGIFormat(m_kPixelFormat);

    unsigned int uiMSAACount = 1;
    unsigned int uiMSAAQuality = 0;
    Ni2DBuffer::GetMSAACountAndQualityFromPref(eMSAAPref, 
        uiMSAACount, uiMSAAQuality);
    NIASSERT(uiMSAACount != 0);

    D3D10_USAGE eUsage = D3D10_USAGE_DEFAULT;
    unsigned int uiCPUAccessFlags = 0;

    unsigned int uiBindFlags = 
        D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;

    NIASSERT(m_usLevels != 0 && 
        m_usLevels < D3D10_MAX_TEXTURE_DIMENSION_2_TO_EXP);

    unsigned int uiMiscFlags = (bCube ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0);

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer && pkRenderer->GetResourceManager() &&
        pkRenderer->GetD3D10Device());

    ID3D10Texture2D* pkD3D10Texture = 
        pkRenderer->GetResourceManager()->CreateTexture2D(
        m_uiWidth, m_uiHeight, m_usLevels, m_usNumTextures, eFormat, 
        uiMSAACount, uiMSAAQuality, eUsage, uiBindFlags, uiCPUAccessFlags, 
        uiMiscFlags, NULL);

    if (pkD3D10Texture == NULL)
    {
        return false;
    }

    D3D10_SHADER_RESOURCE_VIEW_DESC kDesc;
    kDesc.Format = eFormat;
    if (bCube)
    {
        NIASSERT(m_usNumTextures == 6);
        kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
        kDesc.TextureCube.MostDetailedMip = 0;
        kDesc.TextureCube.MipLevels = m_usLevels;
    }
    else
    {
        if (m_usNumTextures == 1)
        {
            if (uiMSAACount == 1)
            {
                kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
                kDesc.Texture2D.MostDetailedMip = 0;
                kDesc.Texture2D.MipLevels = m_usLevels;
            }
            else
            {
                kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DMS;
                // No other parameters
            }
        }
        else
        {
            if (uiMSAACount == 1)
            {
                kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
                kDesc.Texture2DArray.MostDetailedMip = 0;
                kDesc.Texture2DArray.MipLevels = m_usLevels;
                kDesc.Texture2DArray.FirstArraySlice = 0;
                kDesc.Texture2DArray.ArraySize = m_usNumTextures;
            }
            else
            {
                kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY;
                kDesc.Texture2DMSArray.FirstArraySlice = 0;
                kDesc.Texture2DMSArray.ArraySize = m_usNumTextures;
            }
        }
    }

    ID3D10ShaderResourceView* pkResourceView = NULL;
    HRESULT hr = pkRenderer->GetD3D10Device()->CreateShaderResourceView(
        pkD3D10Texture, &kDesc, &pkResourceView);
    if (FAILED(hr) || pkResourceView == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::
                NiD3D10ERROR_SHADER_RESOURCE_VIEW_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::
                NiD3D10ERROR_SHADER_RESOURCE_VIEW_CREATION_FAILED,
                "No error message from D3D10, but resource view is NULL.");
        }

        if (pkResourceView)
        {
            pkResourceView->Release();
        }
    }

    // Create the 2D buffer data. This will manage the D3D10 surface
    // and automatically fills in the Ni2DBuffer::RendererData for us
    // as a side effect.
    if (pkTexture->GetBuffer())
    {
        NIASSERT(IsCubeMap() == false);
        Ni2DBuffer* pkBuffer = pkTexture->GetBuffer();
        NiD3D102DBufferData* pkBuffData = 
            NiD3D10RenderTargetBufferData::Create(pkD3D10Texture, 
            pkBuffer);

        if (!pkBuffData)
        {
            NiD3D10Renderer::Warning("NiD3D10RenderedTextureData::Create> "
                "Failed NiD3D10RenderTargetBufferData::Create - %s", "");
            return false;
        }

    }

    if (IsCubeMap())
    {
        NIASSERT(NiIsKindOf(NiRenderedCubeMap, pkTexture));
        NiRenderedCubeMap* pkRenderedCubeMap = (NiRenderedCubeMap*)pkTexture;

        D3D10_RENDER_TARGET_VIEW_DESC kRTVDesc;
        kRTVDesc.Format = eFormat;
        if (uiMSAACount == 1)
        {
            kRTVDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DARRAY;
            kRTVDesc.Texture2DArray.MipSlice = 0;
            kRTVDesc.Texture2DArray.FirstArraySlice = 0;
            kRTVDesc.Texture2DArray.ArraySize = 1;
        }
        else
        {
            kRTVDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY;
            kRTVDesc.Texture2DMSArray.FirstArraySlice = 0;
            kRTVDesc.Texture2DMSArray.ArraySize = 1;
        }

        for (unsigned int i = 0; i < NiRenderedCubeMap::FACE_NUM; i++)
        {
            Ni2DBuffer* pkBuffer = pkRenderedCubeMap->GetFaceBuffer(
                (NiRenderedCubeMap::FaceID)i);
            NIASSERT(pkBuffer != NULL);

            if (uiMSAACount == 1)
            {
                kRTVDesc.Texture2DArray.FirstArraySlice = i;
            }
            else
            {
                kRTVDesc.Texture2DMSArray.FirstArraySlice = i;
            }

            NiD3D102DBufferData* pkBuffData = 
                NiD3D10RenderTargetBufferData::Create(pkD3D10Texture, 
                pkBuffer, &kRTVDesc);

            if (!pkBuffData)
            {
                NiD3D10Renderer::Warning("NiD3D10RenderedTextureData::Create> "
                    "Failed NiD3D10RenderTargetBufferData::Create - %s", "");
                return false;
            }
        }
    }

    if (InitializeFromD3D10Resource(pkD3D10Texture, pkResourceView))
    {
        pkTexture->SetRendererData(this);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
