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

#include "NiD3D10PixelFormat.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10TextureData.h"

#include <NiImageConverter.h>

//---------------------------------------------------------------------------
NiD3D10TextureData::NiD3D10TextureData(NiTexture* pkTexture) : 
    NiTexture::RendererData(pkTexture),
    m_usLevels(0),
    m_usTextureType(0),
    m_pkResourceView(NULL),
    m_pkD3D10Texture(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10TextureData::~NiD3D10TextureData()
{
    if (m_pkResourceView)
        m_pkResourceView->Release();

    if (m_pkD3D10Texture)
        m_pkD3D10Texture->Release();
}
//---------------------------------------------------------------------------
void NiD3D10TextureData::ClearTextureData()
{
    NiTexture* pkTexture = NiTexture::GetListHead();

    while (pkTexture)
    {
        NiD3D10TextureData* pkData = (NiD3D10TextureData*)
            (pkTexture->GetRendererData());

        if (pkData)
        {
            pkTexture->SetRendererData(NULL);
            NiDelete pkData;
        }

        pkTexture = pkTexture->GetListNext();
    }
}
//---------------------------------------------------------------------------
void NiD3D10TextureData::SetResourceView(
    ID3D10ShaderResourceView* pkResourceView)
{
    if (pkResourceView == m_pkResourceView)
        return;

    if (pkResourceView)
        pkResourceView->AddRef();
    if (m_pkResourceView)
        m_pkResourceView->Release();
    m_pkResourceView = pkResourceView;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10TextureData::FindMatchingPixelFormat(
    const NiPixelFormat& kSrcFmt, const NiTexture::FormatPrefs& kPrefs,
    D3D10_FORMAT_SUPPORT eSupportTest)
{
    // * Must always select a pixel format that exists for the renderer
    // * We must select format pairs that can be converted between.  Use a 
    // less desirable format if the desirable format cannot supported by the 
    // current image converter
    // * In general, prefer matching the desires of the format prefs

    NiPixelFormat::Format eFmt = kSrcFmt.GetFormat();
    NiImageConverter* pkConvert = NiImageConverter::GetImageConverter();
    const NiPixelFormat* pkDestFmt = NULL;

    // D3D10 renderer doesn't deal with tiled platform-specific textures.
    if (kSrcFmt.GetTiling() != NiPixelFormat::TILE_NONE)
        return NULL;

    // D3D10 renderer doesn't deal with Big Endian platform-specific formats.
    if (!kSrcFmt.GetLittleEndian())
        return NULL;

    // Renderer must exist.
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL)
        return NULL;

    // Examine existing data format.
    // Assume that all channels are basically equivalent.
    NiPixelFormat::Component eComponent = NiPixelFormat::COMP_EMPTY;
    NiPixelFormat::Representation eRep = NiPixelFormat::REP_UNKNOWN;
    unsigned char ucBPC = 0;
    bool bSigned = false;

    if (kSrcFmt.GetComponent(0, eComponent, eRep, ucBPC, bSigned))
    {
        if (eRep == NiPixelFormat::REP_COMPRESSED)
        {
            // If it's compressed, leave it as such unless it's supported
            // or an RGBA format is requested
            if ((kPrefs.m_ePixelLayout != 
                NiTexture::FormatPrefs::HIGH_COLOR_16) &&
                (kPrefs.m_ePixelLayout != 
                NiTexture::FormatPrefs::TRUE_COLOR_32))
            {
                if (eFmt == NiPixelFormat::FORMAT_DXT1)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                else if (eFmt == NiPixelFormat::FORMAT_DXT3)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                else if (eFmt == NiPixelFormat::FORMAT_DXT5)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
            }
        }
        else if (eRep == NiPixelFormat::REP_FLOAT ||
            eRep == NiPixelFormat::REP_HALF)
        {
            NIASSERT((eRep == NiPixelFormat::REP_FLOAT && ucBPC == 32) ||
                eRep == NiPixelFormat::REP_HALF && ucBPC == 16);
            unsigned int uiNumComponents = kSrcFmt.GetNumComponents();

            if (ucBPC == 16)
            {
                // Fall back to increasing size, then increasing channels
                if (uiNumComponents < 2)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 3)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 4)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }
            else if (ucBPC == 32)
            {
                // Fall back to increasing channels, then decreasing size
                if (uiNumComponents < 2)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 3)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 4)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }
                if (uiNumComponents < 2)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 3)
                {
                    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT;
                    if (pkRenderer->DoesFormatSupportFlag(
                        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
                        eSupportTest) &&
                        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }
        }
        else if (eRep == NiPixelFormat::REP_INT)
        {
        }
    }

    // Default to RGBA case
    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
    {
        // Bump map -> signed normalized
        if ((eFmt == NiPixelFormat::FORMAT_BUMPLUMA) || 
            (eFmt == NiPixelFormat::FORMAT_RGBA))
        {
            // 4-component 32-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 4-component 64-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 2-component 32-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 2-component 16-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 2-component compressed
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC5_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        else
        {
            // 2-component 32-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 2-component 16-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 2-component compressed
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC5_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 4-component 32-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            // 4-component 64-bit
            pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_SNORM;
            if (pkRenderer->DoesFormatSupportFlag(
                (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        // No other signed, normalized formats support more than one channel - 
        // return NULL
        return NULL;
    }
    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (kPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
        {
            // Compressed formats
            if (kSrcFmt.GetSRGBSpace())
            {
                // SRGB space

                // BC1
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM_SRGB;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC2
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM_SRGB;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC3
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM_SRGB;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

            }
            else
            {
                // Linear RGB space

                // BC1
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC2
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC3
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

            }
        }
        else
        {
            // Compressed formats
            if (kSrcFmt.GetSRGBSpace())
            {
                // SRGB space

                // BC2
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM_SRGB;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC3
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM_SRGB;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC1
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM_SRGB;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

            }
            else
            {
                // Linear RGB space

                // BC2
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC3
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

                // BC1
                pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM;
                if (pkRenderer->DoesFormatSupportFlag(
                    (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
                    pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
                {
                    return pkDestFmt;
                }

            }
        }
    }

    // Default - RGBA, unsigned normalized values
    // D3D10 has no 16-bit or non-alpha formats, so the format prefs have
    // little relevance anymore.

    if (kSrcFmt.GetSRGBSpace())
    {
        // SRGB space

        // 4-component 32-bit 
        pkDestFmt = 
            &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM_SRGB;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), 
            eSupportTest) &&
            pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
        {
            return pkDestFmt;
        }

        // Fall through to linear space
    }

    // Linear space

    // 4-component 32-bit 
    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM;
    if (pkRenderer->DoesFormatSupportFlag(
        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
    {
        return pkDestFmt;
    }

    // 4-component 32-bit with 2-bit alpha
    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R10G10B10A2_UNORM;
    if (pkRenderer->DoesFormatSupportFlag(
        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
    {
        return pkDestFmt;
    }

    // 4-component 64-bit
    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_UNORM;
    if (pkRenderer->DoesFormatSupportFlag(
        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest) &&
        pkConvert->CanConvertPixelData(kSrcFmt, *pkDestFmt))
    {
        return pkDestFmt;
    }

    // Give up
    return NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10TextureData::FindMatchingPixelFormat(
    const NiTexture::FormatPrefs& kPrefs, D3D10_FORMAT_SUPPORT eSupportTest)
{
    // * Must always select a pixel format that exists for the renderer
    // * We must select format pairs that can be converted between.  Use a 
    // less desirable format if the desirable format cannot supported by the 
    // current image converter
    // * In general, prefer matching the desires of the format prefs

    const NiPixelFormat* pkDestFmt = NULL;

    // Renderer must exist.
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL)
        return NULL;

    switch (kPrefs.m_ePixelLayout)
    {
    case NiTexture::FormatPrefs::BUMPMAP:
        // 4-component 32-bit
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_SNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 64-bit
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_SNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 32-bit
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_SNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 16-bit
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8_SNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::SINGLE_COLOR_8:
        // Assume normalized integer
        // Start with smaller formats, then fall back to larger ones
        // Then fall back to more channels

        // 1-component 8-bit norm
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8_UNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 1-component 16-bit norm
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16_UNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 8-bit norm
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8_UNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 16-bit norm
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_UNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::SINGLE_COLOR_16:
        // Assume float
        // Start with smaller formats, then fall back to larger ones
        // Then fall back to more channels
        // 1-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 1-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::SINGLE_COLOR_32:
        // Assume float
        // Start with larger formats, then fall back to more channels
        // Then fall back to smaller formats
        // 1-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 1-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::DOUBLE_COLOR_32:
        // Assume float
        // Start with smaller formats, then fall back to larger ones
        // Then fall back to more channels
        // 2-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::DOUBLE_COLOR_64:
        // Assume float
        // Start with larger formats, then fall back to more channels
        // Then fall back to smaller formats
        // 2-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 2-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::FLOAT_COLOR_32:
    case NiTexture::FormatPrefs::FLOAT_COLOR_64:
        // Assume float
        // Start with smaller format, then fall back to larger one
        // 4-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;
    case NiTexture::FormatPrefs::FLOAT_COLOR_128:
        // Assume float
        // Start with larger format, then fall back to smaller one
        // 4-component 32-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // 4-component 16-bit float
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }

        // Fall back to 32-bit unsigned formats.
        break;

    case NiTexture::FormatPrefs::HIGH_COLOR_16:
    case NiTexture::FormatPrefs::PALETTIZED_8:
    case NiTexture::FormatPrefs::PALETTIZED_4:
    case NiTexture::FormatPrefs::COMPRESSED:
        // Not supported - fall through to 32-bit unsigned formats.
    case NiTexture::FormatPrefs::TRUE_COLOR_32:
    case NiTexture::FormatPrefs::PIX_DEFAULT:
        break;
    }

    // At this point, either the requested format is not supported or a
    // 32-bit unsigned format was requested.

    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BINARY)
    {
        // Try 10-10-10-2
        pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R10G10B10A2_UNORM;
        if (pkRenderer->DoesFormatSupportFlag(
            (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
        {
            return pkDestFmt;
        }
    }

    // 4-component 32-bit norm
    pkDestFmt = &NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM;
    if (pkRenderer->DoesFormatSupportFlag(
        (DXGI_FORMAT)pkDestFmt->GetRendererHint(), eSupportTest))
    {
        return pkDestFmt;
    }

    // Give up
    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10TextureData::InitializeFromD3D10Resource(
    ID3D10Resource* pkD3D10Texture, ID3D10ShaderResourceView* pkResourceView)
{
    if (pkD3D10Texture == NULL || pkResourceView == NULL)
        return false;

    D3D10_RESOURCE_DIMENSION eType = D3D10_RESOURCE_DIMENSION_UNKNOWN;
    pkD3D10Texture->GetType(&eType);
    NIASSERT(eType != D3D10_RESOURCE_DIMENSION_UNKNOWN);
    if (eType == D3D10_RESOURCE_DIMENSION_BUFFER)
        return false;

#if defined(NIDEBUG)
    D3D10_SHADER_RESOURCE_VIEW_DESC kViewDesc;
    pkResourceView->GetDesc(&kViewDesc);
    NIASSERT((eType == D3D10_RESOURCE_DIMENSION_TEXTURE1D && 
        (kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE1D ||
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE1DARRAY)) ||
        (eType == D3D10_RESOURCE_DIMENSION_TEXTURE2D && (
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE2D ||
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE2DARRAY ||
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE2DMS ||
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY ||
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURECUBE)) ||
        (eType == D3D10_RESOURCE_DIMENSION_TEXTURE3D && 
        kViewDesc.ViewDimension == D3D10_SRV_DIMENSION_TEXTURE3D));
#endif //#if defined(NIDEBUG)

    m_pkResourceView = pkResourceView;
    m_pkD3D10Texture = pkD3D10Texture;

    if (eType == D3D10_RESOURCE_DIMENSION_TEXTURE1D)
        m_usTextureType |= TEXTURETYPE_1D;
    else if (eType == D3D10_RESOURCE_DIMENSION_TEXTURE2D)
        m_usTextureType |= TEXTURETYPE_2D;
    else if (eType == D3D10_RESOURCE_DIMENSION_TEXTURE3D)
        m_usTextureType |= TEXTURETYPE_3D;

    return true;
}
//---------------------------------------------------------------------------
