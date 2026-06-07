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

#include "NiDX9SourceTextureData.h"
#include "NiDX9SourceCubeMapData.h"
#include "NiDX9Renderer.h"

#include <NiFilename.h>
#include <NiImageConverter.h>
#include <NiSourceCubeMap.h>

unsigned int NiDX9SourceTextureData::ms_uiSkipLevels = 0;

//---------------------------------------------------------------------------
NiDX9SourceTextureData::NiDX9SourceTextureData(NiSourceTexture* pkTexture, 
    NiDX9Renderer* pkRenderer) : 
    NiDX9TextureData(pkTexture, pkRenderer),
    m_uiLevelsSkipped(0),
    m_uiSourceRevID(0),
    m_uiPalRevID(0),
    m_bReplacementData(false),
    m_bMipmap(false),
    m_uiFormattedSize(0)
{
    m_usTextureType |= TEXTURETYPE_SOURCE;
}
//---------------------------------------------------------------------------
NiDX9SourceTextureData::~NiDX9SourceTextureData()
{
    /* */
}
//---------------------------------------------------------------------------
NiDX9SourceTextureData* NiDX9SourceTextureData::Create(
    NiSourceTexture* pkTexture, NiDX9Renderer* pkRenderer)
{
    if (NiIsKindOf(NiSourceCubeMap, pkTexture))
    {
        return NiDX9SourceCubeMapData::Create((NiSourceCubeMap*)pkTexture, 
            pkRenderer);
    }

    // Using the new default texture formats.
    const NiDX9Renderer::TexUsage eTex = NiDX9Renderer::TEXUSE_TEX;
    NiPixelFormat** ppkPixFmts = pkRenderer->GetTextureFormatArray(eTex);

    NiDX9SourceTextureData* pkThis = NiNew NiDX9SourceTextureData(
        pkTexture, pkRenderer);

    const NiTexture::FormatPrefs& kPrefs = pkTexture->GetFormatPreferences();

    // It is safe to use regular pointers, since the image converter functions
    // never assign incoming or outgoing data to smart pointers.
    NiPixelData* pkSrcPixels = NULL;

    // If there's NiDX9PersistentSrcTextureRendererData, use it.
    NiDX9PersistentSrcTextureRendererData* pkPersistentSrcRendererData =
        (NiDX9PersistentSrcTextureRendererData*)
        pkTexture->GetPersistentSourceRendererData();
    bool bFoundFile = false;
    bool bCanLoadFile = false;
    if (!pkPersistentSrcRendererData)
    {
        pkSrcPixels = pkTexture->GetSourcePixelData();

        if (!pkSrcPixels)
        {
            // Import file into pixel data object
            const char* pcFilename = pkTexture->GetPlatformSpecificFilename();

            if (pcFilename)
            {
                if (pkTexture->GetLoadDirectToRendererHint())
                {
                    bool bSuccess = pkThis->LoadTextureFile(pcFilename, 
                        pkRenderer, kPrefs);

                    if (bSuccess)
                    {
                        pkThis->m_pkTexture->SetRendererData(pkThis);
                        return pkThis;
                    }
                }

                pkSrcPixels = NiImageConverter::GetImageConverter()->
                    ReadImageFile(pcFilename, NULL);

                if (pkSrcPixels)
                {
                    bFoundFile = true;
                    bCanLoadFile = false;
                }
                else
                {
                    // Try to figure out why the file didn't load

                    // Check for missing file
                    char acStandardFilename[NI_MAX_PATH];
                    NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilename);
                    NiPath::Standardize(acStandardFilename);
                    NiFile* pkIstr = NiFile::GetFile(acStandardFilename, 
                        NiFile::READ_ONLY);
                    if ((pkIstr) && ((*pkIstr)))
                    {
                        bFoundFile = true;
                    }
                    NiDelete pkIstr;

                    if (bFoundFile)
                    {
                        // Check for unsupported format
                        bCanLoadFile = NiImageConverter::GetImageConverter()->
                            CanReadImageFile(pcFilename);
                    }
                }
            }
        }
    }

    D3DFORMAT eD3DFmt = D3DFMT_UNKNOWN;
    const NiPixelFormat* pkDestFmt = NULL;

    // This smart pointer exists to allow pkSrcPixels to be deleted if it needs
    // to be.
    NiPixelDataPtr spSrcPixels = pkSrcPixels;

    if ((!pkPersistentSrcRendererData) && (!pkSrcPixels))
    {
        if (!bFoundFile)
        {
            pkSrcPixels = GetReplacementData(pkTexture, FILENOTFOUND);

            NiD3DRenderer::Error("Failed to find texture %s\n",
                pkTexture->GetPlatformSpecificFilename());
        }
        else if (!bCanLoadFile)
        {
            // File found, but format not supported.
            pkSrcPixels = 
                GetReplacementData(pkTexture, FILEFORMATNOTSUPPORTED);

            NiD3DRenderer::Error("Texture file %s found, but format is not "
                "supported\n",
                pkTexture->GetPlatformSpecificFilename());
        }
        else
        {
            // File found, but couldn't be read.
            pkSrcPixels = GetReplacementData(pkTexture, FILELOADFAILED);

            NiD3DRenderer::Error("Texture file %s found and supported, but "
                "could not be loaded\n",
                pkTexture->GetPlatformSpecificFilename());
        }

        pkThis->m_bReplacementData = true;
    }
    else if (pkPersistentSrcRendererData)
    {
        const NiPixelFormat& kSrcFmt =
            pkPersistentSrcRendererData->GetPixelFormat();

        if ((pkPersistentSrcRendererData->GetPixels() == NULL) ||
            (pkPersistentSrcRendererData->GetTargetPlatform() !=
            NiPersistentSrcTextureRendererData::NI_DX9))
        {
            pkSrcPixels = GetReplacementData(pkTexture, FAILEDCONVERT);
            pkThis->m_bReplacementData = true;

            NiD3DRenderer::Error("Texture %s contains persistent source "
                "texture data for a platform other than DX9; cannot "
                "convert.\n",
                (const char*)pkTexture->GetPlatformSpecificFilename());
        }
        else if (!(NiIsPowerOf2(pkPersistentSrcRendererData->GetWidth()) &&
            NiIsPowerOf2(pkPersistentSrcRendererData->GetHeight())) &&
            (pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0)
        {
            if (pkRenderer->GetFlags() &
                NiRenderer::CAPS_NONPOW2_CONDITIONAL_TEXT)
            {
                pkDestFmt = pkThis->FindMatchingPixelFormat(kSrcFmt, kPrefs,
                    ppkPixFmts);
                if (pkDestFmt == NULL)
                {
                    pkSrcPixels = GetReplacementData(pkTexture, FAILEDCONVERT);
                    pkThis->m_bReplacementData = true;
                }
            }
            else
            {
                // Case where power-of-two texture dimensions are required.
                pkSrcPixels = GetReplacementData(pkTexture, BADDIMENSIONS);
                pkThis->m_bReplacementData = true;
            }
        }
        else
        {
            pkDestFmt = pkThis->FindMatchingPixelFormat(kSrcFmt, kPrefs, 
                ppkPixFmts);
            if (pkDestFmt == NULL)
            {
                pkSrcPixels = GetReplacementData(pkTexture, FAILEDCONVERT);
                pkThis->m_bReplacementData = true;
            }
        }

        // Determine the D3D format from the pixel format.
        eD3DFmt = NiDX9PixelFormat::DetermineD3DFormat(kSrcFmt);
        if (eD3DFmt == D3DFMT_UNKNOWN)
        {
            pkSrcPixels = GetReplacementData(pkTexture, FAILEDCONVERT);
            pkThis->m_bReplacementData = true;
        }

        // Determine if the D3D format is compatible with the device.
        NiDX9Renderer* pkRenderer =
            (NiDX9Renderer*)NiDX9Renderer::GetRenderer();
        HRESULT eD3DRet = pkRenderer->GetDirect3D()->CheckDeviceFormat( 
            pkRenderer->GetAdapter(),
            pkRenderer->GetDevType(),
            pkRenderer->GetAdapterFormat(),
                                  // Unused, but valid display format required.
            0,
            D3DRTYPE_TEXTURE,
            eD3DFmt);
        if (FAILED(eD3DRet))
        {
            pkSrcPixels = GetReplacementData(pkTexture, FAILEDCONVERT);
            pkThis->m_bReplacementData = true;
        }

        // Check texture dimensions
        if (pkPersistentSrcRendererData->GetWidth() > 
            pkRenderer->GetMaxTextureWidth() ||
            pkPersistentSrcRendererData->GetHeight() > 
            pkRenderer->GetMaxTextureHeight() ||
            (pkRenderer->GetMaxTextureAspectRatio() > 0 &&
            ((pkPersistentSrcRendererData->GetWidth() / 
            pkPersistentSrcRendererData->GetHeight() > 
            pkRenderer->GetMaxTextureAspectRatio()) || 
            (pkPersistentSrcRendererData->GetHeight() / 
            pkPersistentSrcRendererData->GetWidth() > 
            pkRenderer->GetMaxTextureAspectRatio()))))
        {
            pkSrcPixels = GetReplacementData(pkTexture, BADDIMENSIONS);
            pkThis->m_bReplacementData = true;
        }
    }
    else    // if (pkSrcPixels)
    {
        const NiPixelFormat& kSrcFmt = pkSrcPixels->GetPixelFormat();

        if (!(NiIsPowerOf2(pkSrcPixels->GetWidth()) && 
            NiIsPowerOf2(pkSrcPixels->GetHeight())) &&
            (pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0)
        {
            if (pkRenderer->GetFlags() &
                NiRenderer::CAPS_NONPOW2_CONDITIONAL_TEXT)
            {
                // Case where non-power-of-two texture dimensions are
                // supported, but with some restrictions.
                if (kPrefs.m_ePixelLayout ==
                    NiTexture::FormatPrefs::COMPRESSED)
                {
                    NiTexture::FormatPrefs kPrefsUncompressed = kPrefs;

                    kPrefsUncompressed.m_ePixelLayout =
                        NiTexture::FormatPrefs::TRUE_COLOR_32;

                    pkDestFmt = pkThis->FindMatchingPixelFormat(kSrcFmt,
                        kPrefsUncompressed, ppkPixFmts);
                }
                else
                {
                    pkDestFmt = pkThis->FindMatchingPixelFormat(kSrcFmt,
                        kPrefs, ppkPixFmts);
                }

                if (pkDestFmt == NULL)
                {
                    pkSrcPixels =
                        GetReplacementData(pkTexture, FAILEDCONVERT);
                    pkThis->m_bReplacementData = true;
                }
            }
            else
            {
                // Case where power-of-two texture dimensions are required.
                pkSrcPixels = GetReplacementData(pkTexture, BADDIMENSIONS);
                pkThis->m_bReplacementData = true;
            }
        }
        else
        {
            pkDestFmt = pkThis->FindMatchingPixelFormat(kSrcFmt, kPrefs, 
                ppkPixFmts);

            if (pkDestFmt == NULL)
            {
                pkSrcPixels = GetReplacementData(pkTexture, FAILEDCONVERT);
                pkThis->m_bReplacementData = true;
            }
        }
        
        // Check texture dimensions
        if (pkSrcPixels->GetWidth() > pkRenderer->GetMaxTextureWidth() ||
            pkSrcPixels->GetHeight() > pkRenderer->GetMaxTextureHeight() ||
            (pkRenderer->GetMaxTextureAspectRatio() > 0 &&
            ((pkSrcPixels->GetWidth() / pkSrcPixels->GetHeight() > 
            pkRenderer->GetMaxTextureAspectRatio()) || 
            (pkSrcPixels->GetHeight() / pkSrcPixels->GetWidth() > 
            pkRenderer->GetMaxTextureAspectRatio()))))
        {
            pkSrcPixels = GetReplacementData(pkTexture, BADDIMENSIONS);
            pkThis->m_bReplacementData = true;
        }
    }

    if (pkThis->m_bReplacementData)
    {
        pkDestFmt = ppkPixFmts[pkRenderer->GetReplacementDataFormat()];

        pkPersistentSrcRendererData = NULL; // Sanity checks.
        NIASSERT(pkSrcPixels && pkDestFmt);
    }

    if (pkPersistentSrcRendererData)
    {
        // Update texture attributes from the NiDX9SourceTextureData.
        pkThis->m_kPixelFormat = pkPersistentSrcRendererData->GetPixelFormat();
        pkThis->m_uiWidth = pkPersistentSrcRendererData->GetWidth();
        pkThis->m_uiHeight = pkPersistentSrcRendererData->GetHeight();
        pkThis->m_usLevels = 
            (unsigned short)pkPersistentSrcRendererData->GetNumMipmapLevels();
        pkThis->m_bMipmap = ((pkThis->m_usLevels == 1) ? false : true);
        pkThis->m_bReplacementData = false;
        pkThis->m_pkRenderer = pkRenderer;
        pkThis->m_uiFormattedSize =
            (unsigned int) pkPersistentSrcRendererData->GetTotalSizeInBytes();

        // Attach palette if the surface is palettized
        pkThis->UpdatePalette(pkPersistentSrcRendererData->GetPalette());

        // Allocate texture and establish mipmap levels.
        if (!pkThis->m_pkD3DTexture)
        {
            pkThis->CreateSurfFromRendererData(pkPersistentSrcRendererData,
                eD3DFmt);
        }

        // Copy to texture.
        if (pkThis->m_pkD3DTexture) 
            pkThis->CopyRendererDataToSurface(pkPersistentSrcRendererData);
    }
    else
    {
        pkThis->m_uiSourceRevID = pkSrcPixels->GetRevisionID();
        pkThis->m_kPixelFormat = *pkDestFmt;
        pkThis->m_uiWidth = pkSrcPixels->GetWidth();
        pkThis->m_uiHeight = pkSrcPixels->GetHeight();

        pkThis->m_bMipmap = (!pkThis->m_bReplacementData &&
            ((kPrefs.m_eMipMapped == NiTexture::FormatPrefs::YES) ||
            ((kPrefs.m_eMipMapped == NiTexture::FormatPrefs::MIP_DEFAULT) && 
            NiTexture::GetMipmapByDefault()))) ? true : false;

        NiPixelData* pkFormatted = NULL;
        if (pkThis->m_bReplacementData)
        {
            pkFormatted = pkSrcPixels;  // Do not convert replacement data.
        }
        else
        {
            // Convert data format
            pkFormatted = 
                NiImageConverter::GetImageConverter()->ConvertPixelData(
                *pkSrcPixels, *pkDestFmt, pkSrcPixels, pkThis->m_bMipmap);
        }

        NIASSERT(pkFormatted);

        // Allocate texture
        if (!pkThis->m_pkD3DTexture)
            pkThis->CreateSurf(*pkFormatted);

        // Copy to texture
        if (pkThis->m_pkD3DTexture)
            pkThis->CopyDataToSurface(*pkFormatted);

        pkThis->m_usLevels = (unsigned short)pkFormatted->GetNumMipmapLevels();

        // Attach palette if the surface is palettized.
        pkThis->UpdatePalette(pkSrcPixels->GetPalette());

        pkThis->m_uiFormattedSize = 
            (unsigned int) pkFormatted->GetTotalSizeInBytes();

        // Some of the objects used locally may have zero refcounts.  Smart
        // pointers have been avoided in the function for speed.  Here, at the
        // end of the function, the pkFormatted pointer is assigned to a smart
        // pointer to force destruction, as needed.
        NiPixelDataPtr spDestructor = pkFormatted;
    }

    pkThis->m_pkTexture->SetRendererData(pkThis);

    if (NiSourceTexture::GetDestroyAppDataFlag())
        pkTexture->DestroyAppPixelData();

    return pkThis;
}
//---------------------------------------------------------------------------
bool NiDX9SourceTextureData::CreateSurf(const NiPixelData& kPixels)
{
    D3DFORMAT eD3DFmt = NiDX9PixelFormat::DetermineD3DFormat(m_kPixelFormat);

    m_uiWidth = kPixels.GetWidth();
    m_uiHeight = kPixels.GetHeight();

    if (!(NiIsPowerOf2(m_uiWidth) && NiIsPowerOf2(m_uiHeight)) &&
        (m_pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0)
    {
        m_usLevels = 1;
    }
    else
    {
        m_usLevels = (unsigned short)kPixels.GetNumMipmapLevels();
    }

    unsigned short usSkipLevels = (unsigned short)ms_uiSkipLevels;
    if (usSkipLevels > m_usLevels - 1)
        usSkipLevels = m_usLevels - 1;

    // Adjust (possibly non-power-of-two) dimensions by uiSkipLevels.
    for (unsigned int i = 0; i < usSkipLevels; i++)
    {
        if (m_uiWidth & 0x1)    // If odd, add 1 so shifted value rounds up.
            m_uiWidth++;
        m_uiWidth >>= 1;
        if (m_uiHeight & 0x1)   // If odd, add 1 so shifted value rounds up.
            m_uiHeight++;
        m_uiHeight >>= 1;
    }

    m_uiLevelsSkipped = usSkipLevels;

    NIASSERT(ms_uiSkipLevels == 0 || m_usLevels != 0);

    LPDIRECT3DDEVICE9 pD3DDevice9 = m_pkRenderer->GetD3DDevice();
    LPDIRECT3DTEXTURE9 pkD3DTexture = NULL;
    HRESULT eD3DRet = pD3DDevice9->CreateTexture(m_uiWidth, m_uiHeight,
        m_usLevels - m_uiLevelsSkipped, 0, eD3DFmt, D3DPOOL_MANAGED, 
        &pkD3DTexture, NULL);
    if (FAILED(eD3DRet) || pkD3DTexture == NULL)
    {
        NiDX9Renderer::Warning("NiDX9SourceTextureData::CreateSurf> Failed"
            " CreateImageSurface - %s\n", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        m_pkD3DTexture = NULL;
        return false;
    }

    m_pkD3DTexture = (D3DBaseTexturePtr)pkD3DTexture;

    return true;
}
//---------------------------------------------------------------------------
void NiDX9SourceTextureData::CopyDataToSurface(const NiPixelData& kPixels)
{
    if (m_pkD3DTexture == NULL ||
        m_pkD3DTexture->GetType() == D3DRTYPE_CUBETEXTURE ||
        m_pkD3DTexture->GetType() == D3DRTYPE_VOLUMETEXTURE)
    {
        return;
    }

    kPixels.GetPixels();
    m_usLevels = (unsigned short)kPixels.GetNumMipmapLevels();

    NIASSERT(m_pkD3DTexture);

    D3DTexturePtr pkD3DTexture = (D3DTexturePtr)m_pkD3DTexture;
    D3DSurfacePtr pkMipLevel;
    for (unsigned int i = 0; i < m_usLevels - m_uiLevelsSkipped; i++)
    {
        HRESULT eD3DRet = pkD3DTexture->GetSurfaceLevel(i, &pkMipLevel);
        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Error("NiDX9SourceTextureData::CopyDataToSurface>"
                " Failed GetSurfaceLevel (%d)- %s", i,
                NiDX9ErrorString((unsigned int)eD3DRet));
            return;
        }

        CopyDataToSurfaceLevel(kPixels, i + m_uiLevelsSkipped, pkMipLevel);

        NiD3DRenderer::ReleaseResource(pkMipLevel);
    }
}
//---------------------------------------------------------------------------
void NiDX9SourceTextureData::CopyDataToSurfaceLevel(const NiPixelData& kPixels,
    unsigned int uiLevel, D3DSurfacePtr pkMipLevel, unsigned int uiFace)
{
    D3DLOCKED_RECT kLockRect;
    D3DSURFACE_DESC kSurfDesc;

    HRESULT eD3DRet = pkMipLevel->GetDesc(&kSurfDesc);
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Error("NiDX9SourceTextureData::CopyDataToSurfaceLevel>"
            " Failed GetDesc - %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return;
    }

    eD3DRet = pkMipLevel->LockRect(&kLockRect, NULL, 0);
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Error("NiDX9SourceTextureData::CopyDataToSurfaceLevel>"
            " Failed GetSurfaceLevel - %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return;
    }

    unsigned int uiSrcRowSize = kPixels.GetWidth(uiLevel, uiFace) * 
        kPixels.GetPixelStride();
    unsigned int uiDestRowSize = (unsigned int)kLockRect.Pitch;
    unsigned int uiMinNumRows = 
        (kPixels.GetHeight(uiLevel) > kSurfDesc.Height ? 
        kSurfDesc.Height : kPixels.GetHeight(uiLevel, uiFace));

    if (kPixels.GetPixelFormat().GetCompressed())
    {
        // For compressed formats, the pitch = the number of bytes per
        // row of blocks
        // Each row of blocks is 4 rows of pixels
        NIASSERT(kSurfDesc.Format == D3DFMT_DXT1 ||
            kSurfDesc.Format == D3DFMT_DXT2 ||
            kSurfDesc.Format == D3DFMT_DXT3 ||
            kSurfDesc.Format == D3DFMT_DXT4 ||
            kSurfDesc.Format == D3DFMT_DXT5);
        uiDestRowSize /= 4;

        // kPixels.GetPixelStride = 0 for compressed formats; need to
        // recalculate and account for non-divisible-by-four pixel counts
        if (kSurfDesc.Format == D3DFMT_DXT1)
            uiSrcRowSize = ((kPixels.GetWidth(uiLevel) + 3) & 0xFFFFFFC) / 2;
        else
            uiSrcRowSize = (kPixels.GetWidth(uiLevel) + 3) & 0xFFFFFFC;

        // Account for non-divisible-by-four pixel counts;
        uiMinNumRows = (uiMinNumRows + 3) & 0xFFFFFFC;
    }

    if (uiDestRowSize != uiSrcRowSize)
    {
        unsigned int uiMinRowSize = (uiSrcRowSize > uiDestRowSize ? 
            uiDestRowSize : uiSrcRowSize); 

        const unsigned char* pucSrc = kPixels.GetPixels(uiLevel, uiFace);
        unsigned char* pucDest = (unsigned char*)(kLockRect.pBits);
        for (unsigned int y = 0; y < uiMinNumRows; y++)
        {
            NiMemcpy(pucDest, uiMinRowSize, pucSrc, uiMinRowSize);

            pucSrc += uiSrcRowSize;
            pucDest += kLockRect.Pitch;
        }
    }
    else
    {
        size_t stUsedSize =
            kPixels.GetSizeInBytes(uiLevel);
        NI_UNUSED_ARG(stUsedSize);
        unsigned int uiDataSize = uiSrcRowSize * uiMinNumRows;
        NIASSERT(stUsedSize >= uiDataSize);
        NiMemcpy((unsigned char*)(kLockRect.pBits), uiDataSize,
            kPixels.GetPixels(uiLevel, uiFace), uiDataSize);
    }

    pkMipLevel->UnlockRect();
}
//---------------------------------------------------------------------------
bool NiDX9SourceTextureData::CreateSurfFromRendererData(
    const NiDX9PersistentSrcTextureRendererData* pkPersistentSrcRendererData,
    D3DFORMAT eD3DFmt)
{
    if (!(NiIsPowerOf2(m_uiWidth) && NiIsPowerOf2(m_uiHeight)) &&
        (m_pkRenderer->GetFlags() & NiRenderer::CAPS_NONPOW2_TEXT) == 0)
    {
        m_usLevels = 1;
    }
    else
    {
        m_usLevels =
            (unsigned short)pkPersistentSrcRendererData->GetNumMipmapLevels();
    }
    m_bMipmap = ((m_usLevels == 1) ? false : true);

    unsigned short usSkipLevels = (unsigned short)ms_uiSkipLevels;
    if (usSkipLevels > m_usLevels - 1)
        usSkipLevels = m_usLevels - 1;

    // Adjust (possibly non-power-of-two) dimensions by uiSkipLevels.
    for (unsigned int i = 0; i < usSkipLevels; i++)
    {
        if (m_uiWidth & 0x1)    // If odd, add 1 so shifted value rounds up.
            m_uiWidth++;
        m_uiWidth >>= 1;
        if (m_uiHeight & 0x1)   // If odd, add 1 so shifted value rounds up.
            m_uiHeight++;
        m_uiHeight >>= 1;
    }

    m_uiLevelsSkipped = usSkipLevels;
    NIASSERT(ms_uiSkipLevels == 0 || m_usLevels != 0);

    LPDIRECT3DDEVICE9 pkD3DDevice9 = m_pkRenderer->GetD3DDevice();
    D3DTexturePtr pkD3DTexture = NULL;

    HRESULT eD3DRet = pkD3DDevice9->CreateTexture(
        m_uiWidth,  // [in] Width of top level of texture.
        m_uiHeight, // [in] Height of top level of texture.
        m_usLevels - m_uiLevelsSkipped, // [in] Number of levels in the texture
        0,  // [in] Usage:  0 = no usage value.
        eD3DFmt,    // [in] The format of all levels in the texture.
        D3DPOOL_MANAGED, // [in] Memory class into which texture should go.
        &pkD3DTexture,  // [out] Pointer to IDirect3DTexture9 interface,
                        // representing the created texture resource.
        NULL);  // [in] Reserved.
    if (FAILED(eD3DRet) || pkD3DTexture == NULL)
    {
        NiDX9Renderer::Warning(
            "NiDX9SourceTextureData::CreateSurfFromRendererData> "
            "FAILED to create texture of format: 0x%x, D3D error: %s", 
            eD3DFmt, NiDX9ErrorString((unsigned int)eD3DRet));
        m_pkD3DTexture = NULL;
        return false;
    }

    m_pkD3DTexture = (D3DBaseTexturePtr)pkD3DTexture;

    return true;
}
//---------------------------------------------------------------------------
void NiDX9SourceTextureData::CopyRendererDataToSurface(
    const NiDX9PersistentSrcTextureRendererData* pkPersistentSrcRendererData)
{
    if (m_pkD3DTexture == NULL ||
        m_pkD3DTexture->GetType() == D3DRTYPE_CUBETEXTURE ||
        m_pkD3DTexture->GetType() == D3DRTYPE_VOLUMETEXTURE)
    {
        return;
    }

    pkPersistentSrcRendererData->GetPixels();

    D3DTexturePtr pkD3DTexture = (D3DTexturePtr)m_pkD3DTexture;
    D3DSurfacePtr pkMipLevel;
    for (unsigned int i = 0; i < m_usLevels - m_uiLevelsSkipped; i++)
    {
        HRESULT eD3DRet = pkD3DTexture->GetSurfaceLevel(i, &pkMipLevel);
        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Error(
                "NiDX9SourceTextureData::CopyRendererDataToSurface>"
                " Failed GetSurfaceLevel (%d)- %s", i,
                NiDX9ErrorString((unsigned int)eD3DRet));
            return;
        }

        CopyRendererDataToSurfaceLevel(pkPersistentSrcRendererData,
            i + m_uiLevelsSkipped, pkMipLevel);

        NiD3DRenderer::ReleaseResource(pkMipLevel);
    }
}
//---------------------------------------------------------------------------
void NiDX9SourceTextureData::CopyRendererDataToSurfaceLevel(
    const NiDX9PersistentSrcTextureRendererData* pkPSTRD, unsigned int uiLevel,
    D3DSurfacePtr pkMipLevel, unsigned int uiFace)
{
    D3DLOCKED_RECT kLockRect;
    D3DSURFACE_DESC kSurfDesc;

    HRESULT eD3DRet = pkMipLevel->GetDesc(&kSurfDesc);
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Error(
            "NiDX9SourceTextureData::CopyRendererDataToSurfaceLevel>"
            " Failed GetDesc - %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return;
    }

    eD3DRet = pkMipLevel->LockRect(&kLockRect, NULL, 0);
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Error(
            "NiDX9SourceTextureData::CopyRendererDataToSurfaceLevel>"
            " Failed GetSurfaceLevel - %s", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return;
    }

    unsigned int uiSrcRowSize =
        pkPSTRD->GetWidth(uiLevel, uiFace) * pkPSTRD->GetPixelStride();
    unsigned int uiDestRowSize = (unsigned int)kLockRect.Pitch;
    unsigned int uiMinNumRows = 
        (pkPSTRD->GetHeight(uiLevel) > kSurfDesc.Height ? 
        kSurfDesc.Height : pkPSTRD->GetHeight(uiLevel, uiFace));

    if (pkPSTRD->GetPixelFormat().GetCompressed())
    {
        // For compressed formats, the pitch = the number of bytes per
        // row of blocks
        // Each row of blocks is 4 rows of pixels
        NIASSERT(kSurfDesc.Format == D3DFMT_DXT1 ||
            kSurfDesc.Format == D3DFMT_DXT2 ||
            kSurfDesc.Format == D3DFMT_DXT3 ||
            kSurfDesc.Format == D3DFMT_DXT4 ||
            kSurfDesc.Format == D3DFMT_DXT5);
        uiDestRowSize /= 4;

        // kPixels.GetPixelStride = 0 for compressed formats; need to
        // recalculate and account for non-divisible-by-four pixel counts
        if (kSurfDesc.Format == D3DFMT_DXT1)
            uiSrcRowSize = ((pkPSTRD->GetWidth(uiLevel) + 3) & 0xFFFFFFC) / 2;
        else
            uiSrcRowSize = (pkPSTRD->GetWidth(uiLevel) + 3) & 0xFFFFFFC;

        // Account for non-divisible-by-four pixel counts;
        uiMinNumRows = (uiMinNumRows + 3) & 0xFFFFFFC;
    }

    if (uiDestRowSize != uiSrcRowSize)
    {
        unsigned int uiMinRowSize = (uiSrcRowSize > uiDestRowSize ? 
            uiDestRowSize : uiSrcRowSize); 

        const unsigned char* pucSrc = pkPSTRD->GetPixels(uiLevel, uiFace);
        unsigned char* pucDest = (unsigned char*)(kLockRect.pBits);
        for (unsigned int y = 0; y < uiMinNumRows; y++)
        {
            NiMemcpy(pucDest, uiMinRowSize, pucSrc, uiMinRowSize);

            pucSrc += uiSrcRowSize;
            pucDest += kLockRect.Pitch;
        }
    }
    else
    {
        size_t stUsedSize = 
            pkPSTRD->GetUsedSizeInBytes(uiLevel);
        NI_UNUSED_ARG(stUsedSize);
        unsigned int uiDataSize = uiSrcRowSize * uiMinNumRows;
        NIASSERT(stUsedSize >= uiDataSize);
        NiMemcpy((unsigned char*)(kLockRect.pBits), uiDataSize,
            pkPSTRD->GetPixels(uiLevel, uiFace), uiDataSize);
    }

    pkMipLevel->UnlockRect();
}
//---------------------------------------------------------------------------
void NiDX9SourceTextureData::Update()
{
    // Textures using pixel replacement data needs not be updated
    if (m_bReplacementData)
        return;

    NiPixelData* pkSrcPixels = 
        ((NiSourceTexture*)m_pkTexture)->GetSourcePixelData();

    if (pkSrcPixels)
    {
        bool bPaletteExpand = UpdatePalette(pkSrcPixels->GetPalette());

        // The most common reason for this case below is a change to the
        // pixels in the pixel data object.  We may also have to enter this 
        // case when a palette changes and the destination format has no
        // palette (in which case the palette must be re-expanded into the
        // RGB(A) texture data, and expensive operation)
        if ((pkSrcPixels->GetRevisionID() != m_uiSourceRevID) 
            || bPaletteExpand)
        {
            // Convert data format
            NiPixelData* pkFormatted = 
                NiImageConverter::GetImageConverter()->ConvertPixelData(
                    *pkSrcPixels, m_kPixelFormat, pkSrcPixels, m_bMipmap);

            NIASSERT(pkFormatted);

            m_uiSourceRevID = pkSrcPixels->GetRevisionID();

            // Copy to DirectDraw Surface
            if (m_pkTexture)
                CopyDataToSurface(*pkFormatted);

            // Some of the objects used locally may have zero refcounts.  
            // We have avoided using smart pointers in the function for speed.
            // Here, at the end of the function, we assign them to smart 
            // pointers to force destruction as needed
            NiPixelDataPtr spDestructor = pkFormatted;
        }
    }
}
//---------------------------------------------------------------------------
bool NiDX9SourceTextureData::InitializeFromD3DTexture(
    D3DBaseTexturePtr pkD3DTexture)
{
    m_usLevels = (unsigned short) pkD3DTexture->GetLevelCount();
    m_bMipmap = (m_usLevels != 1);

    m_uiFormattedSize = 0;
    D3DRESOURCETYPE eType = pkD3DTexture->GetType();
    if (eType == D3DRTYPE_TEXTURE)
    {
        D3DTexturePtr pk2DTexture = (D3DTexturePtr)pkD3DTexture;

        D3DSURFACE_DESC kDesc;
        for (unsigned short i = 0; i < m_usLevels; i++)
        {
            HRESULT hr = pk2DTexture->GetLevelDesc(i, &kDesc);
            if (FAILED(hr))
                return false;

            if (i == 0)
            {
                m_uiWidth = kDesc.Width;
                m_uiHeight = kDesc.Height;

                // Currently do not support palettized D3D textures
                if (kDesc.Format == D3DFMT_A8P8 ||
                    kDesc.Format == D3DFMT_P8)
                {
                    return false;
                }

        
                m_kPixelFormat = NiPixelFormat();
                NiDX9PixelFormat::InitFromD3DFormat(
                    kDesc.Format, m_kPixelFormat);
            }

            m_uiFormattedSize += kDesc.Width * kDesc.Height * 
                m_kPixelFormat.GetBitsPerPixel() / 8;
        }
    }
    else if (eType == D3DRTYPE_CUBETEXTURE)
    {
        LPDIRECT3DCUBETEXTURE9 pkCubeTexture = 
            (LPDIRECT3DCUBETEXTURE9)pkD3DTexture;

        D3DSURFACE_DESC kDesc;
        for (unsigned short i = 0; i < m_usLevels; i++)
        {
            HRESULT hr = pkCubeTexture->GetLevelDesc(i, &kDesc);
            if (FAILED(hr))
                return false;

            if (i == 0)
            {
                m_uiWidth = kDesc.Width;
                m_uiHeight = kDesc.Height;
                // Currently do not support palettized D3D textures
                if (kDesc.Format == D3DFMT_A8P8 ||
                    kDesc.Format == D3DFMT_P8)
                {
                    return false;
                }

                m_kPixelFormat = NiPixelFormat();
                NiDX9PixelFormat::InitFromD3DFormat(
                    kDesc.Format, m_kPixelFormat);
            }

            m_uiFormattedSize += kDesc.Width * kDesc.Height * 
                m_kPixelFormat.GetBitsPerPixel() / 8;
        }

        m_uiFormattedSize *= 6;
    }
    else if (eType == D3DRTYPE_VOLUMETEXTURE)
    {
        LPDIRECT3DVOLUMETEXTURE9 pk3DTexture = 
            (LPDIRECT3DVOLUMETEXTURE9)pkD3DTexture;

        D3DVOLUME_DESC kDesc;
        for (unsigned short i = 0; i < m_usLevels; i++)
        {
            HRESULT hr = pk3DTexture->GetLevelDesc(i, &kDesc);
            if (FAILED(hr))
                return false;

            if (i == 0)
            {
                m_uiWidth = kDesc.Width;
                m_uiHeight = kDesc.Height;

                // Currently do not support palettized D3D textures
                if (kDesc.Format == D3DFMT_A8P8 ||
                    kDesc.Format == D3DFMT_P8)
                {
                    return false;
                }

                m_kPixelFormat = NiPixelFormat();
                NiDX9PixelFormat::InitFromD3DFormat(
                    kDesc.Format, m_kPixelFormat);
            }

            m_uiFormattedSize += kDesc.Width * kDesc.Height * kDesc.Depth *
                m_kPixelFormat.GetBitsPerPixel() / 8;
        }
    }

    m_pkD3DTexture = pkD3DTexture;

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9SourceTextureData::UpdatePalette(NiPalette* pkNewPalette)
{
    bool bPaletteExpand = false;

    // If the Pixel Data has a Palette, We need to check for:
    // 1) New palette attached to data
    // 2) Palette elements changed
    if (pkNewPalette)
    {
        unsigned int uiPalRevID = pkNewPalette->GetRevisionID();


        // We know that the dest is not palettized, so if the palette
        // entries have changed, then we need to re-expand the data
        if (m_uiPalRevID != uiPalRevID)
            bPaletteExpand = true;

        m_uiPalRevID = uiPalRevID;

        // We know that the dest is not palettized, so if the palette
        // itself has changed, then we need to re-expand the data
        if (m_spPalette != pkNewPalette)
        {
            m_spPalette = pkNewPalette;
            bPaletteExpand = true;
        }
    }

    return bPaletteExpand;
}
//---------------------------------------------------------------------------
bool NiDX9SourceTextureData::LoadTextureFile(const char* pcFilename,
    NiDX9Renderer* pkRenderer, const NiTexture::FormatPrefs& kPrefs)
{
    if (pkRenderer == NULL)
        return false;

    LPDIRECT3DDEVICE9 pkD3DDevice = pkRenderer->GetD3DDevice();
    if (pkD3DDevice == NULL)
        return false;

    // Find file
    char acStandardFilename[NI_MAX_PATH];
    NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilename);
    NiPath::Standardize(acStandardFilename);
    NiFilename kFileName(acStandardFilename);
    const char* pcExt = kFileName.GetExt();

    D3DXIMAGE_FILEFORMAT eFormat = D3DXIFF_FORCE_DWORD;

    if (NiStricmp(pcExt, ".bmp") == 0)
        eFormat = D3DXIFF_BMP;
    else if (NiStricmp(pcExt, ".tga") == 0)
        eFormat = D3DXIFF_TGA;
    else if (NiStricmp(pcExt, ".dds") == 0)
        eFormat = D3DXIFF_DDS;

    if (eFormat == D3DXIFF_FORCE_DWORD)
        return false;

    NiFile* pkIstr = NiFile::GetFile(acStandardFilename, NiFile::READ_ONLY);
    if ((!pkIstr) || (!(*pkIstr)))
    {
        NiDelete pkIstr;
        return false;
    }

    unsigned int uiBufferLength = pkIstr->GetFileSize();
    if (uiBufferLength == 0)
    {
        NiDelete pkIstr;
        return false;
    }

    // Read file into memory
    unsigned char* pucBuffer = NiAlloc2(unsigned char, uiBufferLength, NiMemHint::TEXTURE);
    pkIstr->Read(pucBuffer, uiBufferLength);

    NiDelete pkIstr;

    D3DXIMAGE_INFO kImageInfo;
    HRESULT hr = D3DXGetImageInfoFromFileInMemory((VOID*)pucBuffer, 
        uiBufferLength, &kImageInfo);

    if (FAILED(hr))
    {
        NiFree(pucBuffer);
        return false;
    }

    // Special case: any bump maps not in a signed format
    // must have data massaged in NiPixelData format
    if (kPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP &&
        kImageInfo.Format != D3DFMT_V8U8 &&
        kImageInfo.Format != D3DFMT_Q8W8V8U8 &&
        kImageInfo.Format != D3DFMT_V16U16 &&
        kImageInfo.Format != D3DFMT_Q16W16V16U16 &&
        kImageInfo.Format != D3DFMT_CxV8U8 &&
        kImageInfo.Format != D3DFMT_L6V5U5 &&
        kImageInfo.Format != D3DFMT_X8L8V8U8 &&
        kImageInfo.Format != D3DFMT_A2W10V10U10)
    {
        NiFree(pucBuffer);
        return false;
    }

    LPDIRECT3DBASETEXTURE9 pkD3DTexture = NULL;

    if (kImageInfo.ResourceType == D3DRTYPE_TEXTURE)
    {
        LPDIRECT3DTEXTURE9 pk2DTexture = NULL;
        hr = D3DXCreateTextureFromFileInMemory(pkD3DDevice,
            (VOID*)pucBuffer, uiBufferLength, &pk2DTexture);
        pkD3DTexture = (LPDIRECT3DBASETEXTURE9)pk2DTexture;
    }
    else if (kImageInfo.ResourceType == D3DRTYPE_CUBETEXTURE)
    {
        NIASSERT(eFormat == D3DXIFF_DDS);
        LPDIRECT3DCUBETEXTURE9 pkCubeTexture = NULL;
        hr = D3DXCreateCubeTextureFromFileInMemory(pkD3DDevice,
            (VOID*)pucBuffer, uiBufferLength, &pkCubeTexture);
        pkD3DTexture = (LPDIRECT3DBASETEXTURE9)pkCubeTexture;
    }
    else if (kImageInfo.ResourceType == D3DRTYPE_VOLUMETEXTURE)
    {
        NIASSERT(eFormat == D3DXIFF_DDS);
        LPDIRECT3DVOLUMETEXTURE9 pk3DTexture = NULL;
        hr = D3DXCreateVolumeTextureFromFileInMemory(pkD3DDevice,
            (VOID*)pucBuffer, uiBufferLength, &pk3DTexture);
        pkD3DTexture = (LPDIRECT3DBASETEXTURE9)pk3DTexture;
    }
    else
    {
        hr = D3DERR_INVALIDCALL;
    }

    NiFree(pucBuffer);
        
    if (FAILED(hr) || pkD3DTexture == NULL)
        return false;

    bool bSuccess = InitializeFromD3DTexture(pkD3DTexture);
    if (bSuccess == false)
    {
        NiDX9Renderer::ReleaseTextureResource((D3DTexturePtr)pkD3DTexture);
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
