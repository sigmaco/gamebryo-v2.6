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

#include "NiDX9TextureData.h"
#include "NiDX9Renderer.h"

#include <NiImageConverter.h>

//---------------------------------------------------------------------------
NiDX9TextureData::NiDX9TextureData(NiTexture* pkTexture,    
    NiDX9Renderer* pkRenderer) : 
    NiTexture::RendererData(pkTexture),
    m_pkRenderer(pkRenderer),
    m_pkD3DTexture(0),
    m_usLevels(0),
    m_usTextureType(0)
{
    /* */
}
//---------------------------------------------------------------------------
NiDX9TextureData::~NiDX9TextureData()
{
    if (m_pkD3DTexture)
    {
        NIASSERT(m_pkRenderer && m_pkRenderer->GetRenderState());
        m_pkRenderer->GetRenderState()->ClearTexture(m_pkD3DTexture);

        NiDX9Renderer::ReleaseTextureResource((D3DTexturePtr)m_pkD3DTexture);

        m_pkD3DTexture = 0;
    }
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9TextureData::FindClosestPixelFormat(
    const NiTexture::FormatPrefs& kPrefs, NiPixelFormat* const* ppkDestFmts)
{
    NiTexture::FormatPrefs::PixelLayout eLayout = kPrefs.m_ePixelLayout;
    NiTexture::FormatPrefs::AlphaFormat eAlpha = kPrefs.m_eAlphaFmt;

    if (eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
        eLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
    if (eAlpha == NiTexture::FormatPrefs::ALPHA_DEFAULT)
        eAlpha = NiTexture::FormatPrefs::NONE;
    
    if ((eLayout == NiTexture::FormatPrefs::PALETTIZED_8 ||
        eLayout == NiTexture::FormatPrefs::PALETTIZED_4) &&
        (eAlpha == NiTexture::FormatPrefs::NONE))
    {
        if (ppkDestFmts[NiDX9Renderer::TEX_PAL8])
            return ppkDestFmts[NiDX9Renderer::TEX_PAL8];
    }
    else if ((eLayout == NiTexture::FormatPrefs::PALETTIZED_8 ||
        eLayout == NiTexture::FormatPrefs::PALETTIZED_4) &&
        ((eAlpha == NiTexture::FormatPrefs::BINARY) ||
        (eAlpha == NiTexture::FormatPrefs::SMOOTH)))
    {
        if (ppkDestFmts[NiDX9Renderer::TEX_PALA8])
            return ppkDestFmts[NiDX9Renderer::TEX_PALA8];
    }

    switch (eLayout)
    {
        // If the above code didn't handle the palettized case, we have to
        // resort to using the RGB cases
        case NiTexture::FormatPrefs::PALETTIZED_4:
        case NiTexture::FormatPrefs::PALETTIZED_8:
        // For now, treat compressed as 16-bit
        case NiTexture::FormatPrefs::COMPRESSED:
        {
            switch (eAlpha)
            {
                case NiTexture::FormatPrefs::NONE:
                case NiTexture::FormatPrefs::BINARY:
                {    
                    if (ppkDestFmts[NiDX9Renderer::TEX_DXT1])
                        return ppkDestFmts[NiDX9Renderer::TEX_DXT1];
                    break;
                }
                case NiTexture::FormatPrefs::SMOOTH:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_DXT3])
                        return ppkDestFmts[NiDX9Renderer::TEX_DXT3];
                    break;
                }
            }
            // NO BREAK - fall to below
        }
        case NiTexture::FormatPrefs::HIGH_COLOR_16:
        {
            switch (eAlpha)
            {
                case NiTexture::FormatPrefs::NONE:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGB565])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGB565];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGB555])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGB555];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGB888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGB888];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA5551])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                    break;
                }
                case NiTexture::FormatPrefs::BINARY:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA5551])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                    break;
                }
                case NiTexture::FormatPrefs::SMOOTH:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                    break;
                }
            }
            break;
        }
        case NiTexture::FormatPrefs::BUMPMAP:
        {
            if (ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA888])
                return ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA888];
            if (eAlpha == NiTexture::FormatPrefs::NONE)
            {
                if (ppkDestFmts[NiDX9Renderer::TEX_BUMP88])
                    return ppkDestFmts[NiDX9Renderer::TEX_BUMP88];
                if (ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556])
                    return ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556];
            }
            else
            {
                if (ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556])
                    return ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556];
                if (ppkDestFmts[NiDX9Renderer::TEX_BUMP88])
                    return ppkDestFmts[NiDX9Renderer::TEX_BUMP88];
            }
            // NO BREAK - fall to below
        }
        case NiTexture::FormatPrefs::TRUE_COLOR_32:
        {
            switch (eAlpha)
            {
                case NiTexture::FormatPrefs::NONE:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGB888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGB888];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGB565])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGB565];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGB555])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGB555];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA5551])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                    break;
                }
                case NiTexture::FormatPrefs::BINARY:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA5551])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                    break;
                }
                case NiTexture::FormatPrefs::SMOOTH:
                {
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                    if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                        return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                    break;
                }
            }
            break;
        }
        case NiTexture::FormatPrefs::SINGLE_COLOR_8:
        {
            // Fall back to 16 bits if necessary
            if (eAlpha == NiTexture::FormatPrefs::NONE)
            {
                if (ppkDestFmts[NiDX9Renderer::TEX_L8])
                    return ppkDestFmts[NiDX9Renderer::TEX_L8];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGB565])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGB565];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGB555])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGB555];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGB888])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGB888];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGBA5551])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
            }
            else
            {
                if (ppkDestFmts[NiDX9Renderer::TEX_A8])
                    return ppkDestFmts[NiDX9Renderer::TEX_A8];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGBA5551])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGBA4444])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
                if (ppkDestFmts[NiDX9Renderer::TEX_RGBA8888])
                    return ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
            }
            break;
        }
        case NiTexture::FormatPrefs::SINGLE_COLOR_16:
        {
            // Fall back to larger formats, then to more channels
            if (ppkDestFmts[NiDX9Renderer::TEX_R16F])
                return ppkDestFmts[NiDX9Renderer::TEX_R16F];
            if (ppkDestFmts[NiDX9Renderer::TEX_R32F])
                return ppkDestFmts[NiDX9Renderer::TEX_R32F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RG32F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG32F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RG64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA128F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
            break;
        }
        case NiTexture::FormatPrefs::SINGLE_COLOR_32:
        {
            // Fall back to more channels, then to smaller formats
            if (ppkDestFmts[NiDX9Renderer::TEX_R32F])
                return ppkDestFmts[NiDX9Renderer::TEX_R32F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RG64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA128F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
            if (ppkDestFmts[NiDX9Renderer::TEX_R16F])
                return ppkDestFmts[NiDX9Renderer::TEX_R16F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RG32F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG32F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
            break;
        }
        case NiTexture::FormatPrefs::DOUBLE_COLOR_32:
        {
            // Fall back to larger formats, then to more channels
            if (ppkDestFmts[NiDX9Renderer::TEX_RG32F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG32F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RG64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA128F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
            break;
        }
        case NiTexture::FormatPrefs::DOUBLE_COLOR_64:
        {
            // Fall back to more channels, then to smaller formats
            if (ppkDestFmts[NiDX9Renderer::TEX_RG64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA128F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RG32F])
                return ppkDestFmts[NiDX9Renderer::TEX_RG32F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
            break;
        }
        case NiTexture::FormatPrefs::FLOAT_COLOR_32:
        case NiTexture::FormatPrefs::FLOAT_COLOR_64:
        {
            // Fall back to larger formats
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA128F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
            break;
        }
        case NiTexture::FormatPrefs::FLOAT_COLOR_128:
        {
            // Fall back to smaller formats
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA128F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
            if (ppkDestFmts[NiDX9Renderer::TEX_RGBA64F])
                return ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
            break;
        }
        default:
            NIASSERT(0); // Must add case above.
    }

    return NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDX9TextureData::FindMatchingPixelFormat(
    const NiPixelFormat& kSrcFmt, const NiTexture::FormatPrefs& kPrefs,
    NiPixelFormat** ppkDestFmts)
{
    // * Must always select a pixel format that exists for the renderer
    // * We must select format pairs that can be converted between.  Use a 
    // less desirable format if the desirable format cannot supported by the 
    // current image converter
    // * In general, prefer matching the desires of the format prefs

    NiPixelFormat::Format eFmt = kSrcFmt.GetFormat();
    NiImageConverter* pkConvert = NiImageConverter::GetImageConverter();
    const NiPixelFormat* pkDestFmt = NULL;

    // DX9 renderer doesn't deal with Xenon-tiled platform-specific textures.
    if (kSrcFmt.GetTiling() != NiPixelFormat::TILE_NONE)
        return NULL;

    // DX9 renderer doesn't deal with Big Endian platform-specific formats.
    if (!kSrcFmt.GetLittleEndian())
        return NULL;

    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
    {
        if ((eFmt == NiPixelFormat::FORMAT_BUMPLUMA) || 
            (eFmt == NiPixelFormat::FORMAT_RGBA))
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA888];
            if (pkDestFmt && pkConvert->CanConvertPixelData(
                kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556];
            if (pkDestFmt && pkConvert->CanConvertPixelData(
                kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMP88];
            if (pkDestFmt && pkConvert->CanConvertPixelData(
                kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        else
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMP88];
            if (pkDestFmt && pkConvert->CanConvertPixelData(
                kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA888];
            if (pkDestFmt && pkConvert->CanConvertPixelData(
                kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }

            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556];
            if (pkDestFmt && pkConvert->CanConvertPixelData(
                kSrcFmt, *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        return NULL;
    }

    switch (eFmt)
    {
    case NiPixelFormat::FORMAT_PAL:
    case NiPixelFormat::FORMAT_RGB:
        {
            if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
            {
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT1];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT3];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT5];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }

            bool bTrueColor = true;

            // Prefer true color formats for true color and high color
            // formats for high color, unless requested explicitly otherwise
            if ((bTrueColor && (kPrefs.m_ePixelLayout ==
                NiTexture::FormatPrefs::HIGH_COLOR_16)) ||
                (!bTrueColor && (kPrefs.m_ePixelLayout !=
                NiTexture::FormatPrefs::TRUE_COLOR_32)))
            {
                // Prefer 16 bit over 32 bit
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGB565];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGB555];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGB888];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }
            else
            {
                // Prefer 32 bit over 16 bit
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGB888];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGB565];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGB555];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }

            // Last-ditch efforts to find _some_ working format
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }

            return NULL;
        }

    case NiPixelFormat::FORMAT_PALALPHA:
    case NiPixelFormat::FORMAT_RGBA:
        // fall down below to RGBA case
        break;

    case NiPixelFormat::FORMAT_DXT1:
        if ((kPrefs.m_ePixelLayout != 
            NiTexture::FormatPrefs::HIGH_COLOR_16) &&
            (kPrefs.m_ePixelLayout !=
            NiTexture::FormatPrefs::TRUE_COLOR_32))
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT1];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        // No DXT1 support - fall down to RGBA
        break;

    case NiPixelFormat::FORMAT_DXT3:
        if ((kPrefs.m_ePixelLayout != 
            NiTexture::FormatPrefs::HIGH_COLOR_16) &&
            (kPrefs.m_ePixelLayout !=
            NiTexture::FormatPrefs::TRUE_COLOR_32))
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT3];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        // No DXT3 support - fall down to RGBA
        break;

    case NiPixelFormat::FORMAT_DXT5:
        if ((kPrefs.m_ePixelLayout != 
            NiTexture::FormatPrefs::HIGH_COLOR_16) &&
            (kPrefs.m_ePixelLayout !=
            NiTexture::FormatPrefs::TRUE_COLOR_32))
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT5];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        // No DXT5 support - fall down to RGBA
        break;

    // Bump maps
    case NiPixelFormat::FORMAT_BUMP:
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMP88];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA888];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }
        return NULL;

    case NiPixelFormat::FORMAT_BUMPLUMA:
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA556];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMPLUMA888];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_BUMP88];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }
        return NULL;

    case NiPixelFormat::FORMAT_ONE_CHANNEL:
        if (kPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_A8];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        else
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_L8];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        // Fall down to floating point/RGBA
        break;

    case NiPixelFormat::FORMAT_TWO_CHANNEL:
    case NiPixelFormat::FORMAT_THREE_CHANNEL:
    case NiPixelFormat::FORMAT_FOUR_CHANNEL:
        // Fall down to floating point/RGBA
        break;

    default:
        return NULL;
    }

    // Check for floating point format
    NiPixelFormat::Component eComponent = NiPixelFormat::COMP_EMPTY;
    NiPixelFormat::Representation eRep = NiPixelFormat::REP_UNKNOWN;
    unsigned char ucBPC = 0;
    bool bSigned = false;

    if (kSrcFmt.GetComponent(0, eComponent, eRep, ucBPC, bSigned))
    {
        if (eRep == NiPixelFormat::REP_FLOAT ||
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
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_R16F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_R32F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 3)
                {
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RG32F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RG64F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }
            else if (ucBPC == 32)
            {
                // Fall back to increasing channels, then decreasing size
                if (uiNumComponents < 2)
                {
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_R32F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 3)
                {
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RG64F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA128F];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
                if (uiNumComponents < 2)
                {
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_R16F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                if (uiNumComponents < 3)
                {
                    pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RG32F];
                    if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                        *pkDestFmt))
                    {
                        return pkDestFmt;
                    }
                }
                pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA64F];
                if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                    *pkDestFmt))
                {
                    return pkDestFmt;
                }
            }
        }
    }

    // Default to RGBA case
    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (kPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT1];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT3];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT5];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        else
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT3];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT5];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_DXT1];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
    }

    bool bTrueColor = true;

    // Prefer true color formats for true color and high color
    // formats for high color, unless requested explicitly otherwise
    if ((bTrueColor && (kPrefs.m_ePixelLayout ==
        NiTexture::FormatPrefs::HIGH_COLOR_16)) ||
        (!bTrueColor && (kPrefs.m_ePixelLayout !=
        NiTexture::FormatPrefs::TRUE_COLOR_32)))
    {
        // Prefer 16 bit over 32 bit
        if (kPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        else
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
    }
    else
    {
        // Prefer 32 bit over 16 bit
        pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA8888];
        if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
            *pkDestFmt))
        {
            return pkDestFmt;
        }

        if (kPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
        else
        {
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA4444];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
            pkDestFmt = ppkDestFmts[NiDX9Renderer::TEX_RGBA5551];
            if (pkDestFmt && pkConvert->CanConvertPixelData(kSrcFmt, 
                *pkDestFmt))
            {
                return pkDestFmt;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiDX9TextureData::ClearTextureData(NiDX9Renderer*)
{
    NiTexture* pkTexture = NiTexture::GetListHead();

    while (pkTexture)
    {
        NiDX9TextureData* pkData = (NiDX9TextureData*)
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
bool NiDX9TextureData::InitializeFromD3DTexture(
    D3DBaseTexturePtr pkD3DTexture)
{
    m_usLevels = (unsigned short)pkD3DTexture->GetLevelCount();

    D3DRESOURCETYPE eType = pkD3DTexture->GetType();
    if (eType == D3DRTYPE_TEXTURE)
    {
        D3DTexturePtr pk2DTexture = (D3DTexturePtr)pkD3DTexture;

        D3DSURFACE_DESC kDesc;
        HRESULT hr = pk2DTexture->GetLevelDesc(0, &kDesc);
        if (FAILED(hr))
            return false;

        m_uiWidth = kDesc.Width;
        m_uiHeight = kDesc.Height;

        // Currently do not support palettized D3D textures
        if (kDesc.Format == D3DFMT_A8P8 || kDesc.Format == D3DFMT_P8)
            return false;

        NiDX9PixelFormat::InitFromD3DFormat(kDesc.Format, 
            m_kPixelFormat);
    }
    else if (eType == D3DRTYPE_CUBETEXTURE)
    {
        LPDIRECT3DCUBETEXTURE9 pkCubeTexture = 
            (LPDIRECT3DCUBETEXTURE9)pkD3DTexture;

        D3DSURFACE_DESC kDesc;
        HRESULT hr = pkCubeTexture->GetLevelDesc(0, &kDesc);
        if (FAILED(hr))
            return false;

        m_uiWidth = kDesc.Width;
        m_uiHeight = kDesc.Height;

        // Currently do not support palettized D3D textures
        if (kDesc.Format == D3DFMT_A8P8 || kDesc.Format == D3DFMT_P8)
            return false;

        NiDX9PixelFormat::InitFromD3DFormat(kDesc.Format, 
            m_kPixelFormat);
    }
    else if (eType == D3DRTYPE_VOLUMETEXTURE)
    {
        LPDIRECT3DVOLUMETEXTURE9 pk3DTexture = 
            (LPDIRECT3DVOLUMETEXTURE9)pkD3DTexture;

        D3DVOLUME_DESC kDesc;
        HRESULT hr = pk3DTexture->GetLevelDesc(0, &kDesc);
        if (FAILED(hr))
            return false;

        m_uiWidth = kDesc.Width;
        m_uiHeight = kDesc.Height;

        // Currently do not support palettized D3D textures
        if (kDesc.Format == D3DFMT_A8P8 || kDesc.Format == D3DFMT_P8)
            return false;

        NiDX9PixelFormat::InitFromD3DFormat(kDesc.Format, 
            m_kPixelFormat);
    }

    return true;
}
//---------------------------------------------------------------------------
