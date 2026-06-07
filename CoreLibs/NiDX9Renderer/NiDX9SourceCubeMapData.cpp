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

#include "NiDX9SourceCubeMapData.h"
#include "NiDX9Renderer.h"

#include <NiImageConverter.h>

//---------------------------------------------------------------------------
NiDX9SourceCubeMapData* NiDX9SourceCubeMapData::Create(
    NiSourceCubeMap* pkTexture, NiDX9Renderer* pkRenderer)
{
    // Using the new default texture formats.
    const NiDX9Renderer::TexUsage eTex = NiDX9Renderer::TEXUSE_CUBE;
    NiPixelFormat** ppkPixFmts = pkRenderer->GetTextureFormatArray(eTex);

    // Quick-out if no cube map formats supported
    unsigned int i;
    for (i = 0; i < NiDX9Renderer::TEX_NUM; i++)
    {
        if (ppkPixFmts[i] != NULL)
            break;
    }
    if (i == NiDX9Renderer::TEX_NUM)
    {
        NiDX9Renderer::Warning("NiDX9SourceCubeMapData::Create> "
           "No Cube Map texture formats supported\n");
        return NULL;
    }

    NiDX9SourceCubeMapData* pkThis = NiNew NiDX9SourceCubeMapData(
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

        if (pkSrcPixels == NULL)
        {
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

                // Import file into pixel data object
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

    D3DFORMAT eD3DFmt;
    const NiPixelFormat* pkDestFmt = NULL;

    if ((!pkPersistentSrcRendererData) && (!pkSrcPixels))
    {
        if (!bFoundFile)
        {
            pkSrcPixels = GetReplacementData(pkTexture, CUBEMAPFILENOTFOUND);

            NiD3DRenderer::Error("Failed to find texture %s\n",
                pkTexture->GetPlatformSpecificFilename());
        }
        else if (!bCanLoadFile)
        {
            // File found, but format not supported.
            pkSrcPixels = 
                GetReplacementData(pkTexture, CUBEMAPFILEFORMATNOTSUPPORTED);

            NiD3DRenderer::Error("Texture file %s found, but format is not "
                "supported\n",
                pkTexture->GetPlatformSpecificFilename());
        }
        else
        {
            // File found, but couldn't be read.
            pkSrcPixels = GetReplacementData(pkTexture, CUBEMAPFILELOADFAILED);

            NiD3DRenderer::Error("Texture file %s found and supported, but "
                "could not be loaded\n",
                pkTexture->GetPlatformSpecificFilename());
        }

        pkThis->m_bReplacementData = true;
    }
    else if (pkPersistentSrcRendererData)
    {
        NiPixelFormat& kSrcFmt = pkPersistentSrcRendererData->GetPixelFormat();

        if ((pkPersistentSrcRendererData->GetWidth() !=
            pkPersistentSrcRendererData->GetHeight()) ||
            !NiIsPowerOf2(pkPersistentSrcRendererData->GetWidth()))
        {   // Check width == height & power of two
            pkSrcPixels = GetReplacementData(pkTexture, CUBEMAPBADDIMENSIONS);
            pkThis->m_bReplacementData = true;
        }
        else
        {
            pkDestFmt = pkThis->FindMatchingPixelFormat(kSrcFmt, kPrefs, 
                ppkPixFmts);
            if (pkDestFmt == NULL)
            {
                pkSrcPixels = GetReplacementData(pkTexture,
                    CUBEMAPFAILEDCONVERT);
                pkThis->m_bReplacementData = true;
            }
        }

        // Check texture dimensions
        if (pkPersistentSrcRendererData->GetWidth() > 
            pkRenderer->GetMaxTextureWidth() ||
            pkPersistentSrcRendererData->GetHeight() > 
            pkRenderer->GetMaxTextureHeight())
        {
            pkSrcPixels = GetReplacementData(pkTexture, CUBEMAPBADDIMENSIONS);
            pkThis->m_bReplacementData = true;
        }

        if (!pkThis->m_bReplacementData)
        {
            // Determine the D3D format from the pixel format.
            eD3DFmt = NiDX9PixelFormat::DetermineD3DFormat(kSrcFmt);
            if (eD3DFmt == D3DFMT_UNKNOWN)
            {
                pkSrcPixels =
                    GetReplacementData(pkTexture, CUBEMAPFAILEDCONVERT);
                pkThis->m_bReplacementData = true;
            }
            else
            {
                // Determine if the D3D format is compatible with the device.
                NiDX9Renderer* pkRenderer =
                    (NiDX9Renderer*)NiDX9Renderer::GetRenderer();
                HRESULT eD3DRet = pkRenderer->GetDirect3D()->CheckDeviceFormat(
                    pkRenderer->GetAdapter(),
                    pkRenderer->GetDevType(),
                    pkRenderer->GetAdapterFormat(), // Unused, but valid
                                                    // display format required.
                    0,
                    D3DRTYPE_TEXTURE,
                    eD3DFmt);
                if (FAILED(eD3DRet))
                {
                    pkSrcPixels = GetReplacementData(pkTexture,
                        CUBEMAPFAILEDCONVERT);
                    pkThis->m_bReplacementData = true;
                }
                else
                {
                    kSrcFmt.SetRendererHint(eD3DFmt);
                }
            }
        }
    }
    else    // if (pkSrcPixels)
    {
        const NiPixelFormat& kSrcFmt = pkSrcPixels->GetPixelFormat();

        // Check width == height & power of two (required for cube maps).
        if ((pkSrcPixels->GetWidth() != pkSrcPixels->GetHeight()) ||
            !NiIsPowerOf2(pkSrcPixels->GetWidth()))
        {
            pkSrcPixels = GetReplacementData(pkTexture, CUBEMAPBADDIMENSIONS);
            pkThis->m_bReplacementData = true;
        }
        else
        {
            pkDestFmt =
                pkThis->FindMatchingPixelFormat(kSrcFmt, kPrefs, ppkPixFmts);

            // Can't use palettized source cube maps.
            if (!pkDestFmt || kSrcFmt.GetPalettized())
            {
                pkSrcPixels =
                    GetReplacementData(pkTexture, CUBEMAPFAILEDCONVERT);
                pkThis->m_bReplacementData = true;
            }
        }

        // Check texture dimensions
        if (pkSrcPixels->GetWidth() > pkRenderer->GetMaxTextureWidth() ||
            pkSrcPixels->GetHeight() > pkRenderer->GetMaxTextureHeight())
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
        // Update texture attributes in the NiDX9SourceTextureData.
        pkThis->m_kPixelFormat = pkPersistentSrcRendererData->GetPixelFormat();
        pkThis->m_uiWidth = pkPersistentSrcRendererData->GetWidth();
        pkThis->m_uiHeight = pkPersistentSrcRendererData->GetHeight();
        pkThis->m_usLevels = static_cast<unsigned short>(
            pkRenderer->GetMipmapCubeMaps() ? 
            pkPersistentSrcRendererData->GetNumMipmapLevels() : 1);
        pkThis->m_bMipmap = ((pkThis->m_usLevels == 1) ? false : true);
        pkThis->m_bReplacementData = false;
        pkThis->m_pkRenderer = pkRenderer;

        // Attach palette if the surface is palettized.  Cube maps only support
        // a single palette shared by all faces.  Since all faces of the
        // NiPersistentSrcRendererData object share a single palette, use the
        // NiPersistentSrcRendererData palette.
        pkThis->UpdatePalette(pkPersistentSrcRendererData->GetPalette());

        pkThis->m_uiFormattedSize =
           (unsigned int) pkPersistentSrcRendererData->GetTotalSizeInBytes();

        // Allocate cube map texture
        bool bCreated = pkThis->CreateCubeMap();
        if (!bCreated || !pkThis->m_pkD3DTexture)
        {
            NiDelete pkThis;
            return NULL;
        }

        for (i = 0; i < NiSourceCubeMap::FACE_NUM; i++ )
        {
            // Copy to texture
            pkThis->CopyRendererDataToCubeMapFace(pkPersistentSrcRendererData,
                (NiSourceCubeMap::FaceID)i);
        }
    }
    else
    {
        NiPixelData* pkFormatted = NULL;
        if (pkThis->m_bReplacementData)
        {
            pkFormatted = pkSrcPixels;  // Do not convert replacement data.
        }
        else
        {
            // Convert data format
            pkFormatted = NiImageConverter::GetImageConverter()->
                ConvertPixelData(*pkSrcPixels, *pkDestFmt, pkSrcPixels,
                pkThis->m_bMipmap);
        }

        NIASSERT(pkFormatted);
        if (!pkFormatted)
        {
            pkFormatted = GetReplacementData(pkTexture, CUBEMAPFAILEDCONVERT);
            pkDestFmt = ppkPixFmts[pkRenderer->GetReplacementDataFormat()];
            pkPersistentSrcRendererData = NULL;
        }

        pkThis->m_uiSourceRevID = pkSrcPixels->GetRevisionID();
        pkThis->m_kPixelFormat = *pkDestFmt;
        pkThis->m_uiWidth = pkThis->m_uiHeight = pkFormatted->GetWidth();

        if (!pkThis->m_bReplacementData &&
            (kPrefs.m_eMipMapped == NiTexture::FormatPrefs::YES) ||
            ((kPrefs.m_eMipMapped == NiTexture::FormatPrefs::MIP_DEFAULT) && 
            NiTexture::GetMipmapByDefault()) &&
            pkRenderer->GetMipmapCubeMaps())
        {
            pkThis->m_bMipmap = true;
        }
        else
        {
            pkThis->m_bMipmap = false;
        }

        pkThis->m_usLevels = static_cast<unsigned short>(
            pkRenderer->GetMipmapCubeMaps() ? 
            pkFormatted->GetNumMipmapLevels() : 1);

        // Allocate cube map texture
        bool bCreated = pkThis->CreateCubeMap();
        if (!bCreated || !pkThis->m_pkD3DTexture)
        {
            NiDelete pkThis;
            return NULL;
        }

        for (i = 0; i < NiSourceCubeMap::FACE_NUM; i++ )
        {
            // Copy to texture
            pkThis->CopyDataToCubeMapFace(*(pkFormatted),
                (NiSourceCubeMap::FaceID)i);
        }

        // Attach palette if the surface is palettized.  Cube maps only support
        // a single palette shared by all faces.  Since all faces of the
        // NiPixelData object share a single palette, use the NiPixelData
        // palette.
        pkThis->UpdatePalette(pkSrcPixels->GetPalette());

        pkThis->m_uiFormattedSize = 
            (unsigned int) pkFormatted->GetTotalSizeInBytes();

        // Some of the objects used locally may have zero refcounts.  We have 
        // avoided using smart pointers in the function for speed.  Here, at
        // the end of the function, we assign them to smart pointers to force
        // destruction, as needed.
        NiPixelDataPtr spDestructor = pkFormatted;
    }

    pkThis->m_pkTexture->SetRendererData(pkThis);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9SourceCubeMapData::~NiDX9SourceCubeMapData()
{
}
//---------------------------------------------------------------------------
NiDX9SourceCubeMapData::NiDX9SourceCubeMapData(NiSourceCubeMap* pkTexture, 
    NiDX9Renderer* pkRenderer) :
    NiDX9SourceTextureData(pkTexture, pkRenderer)
{
    m_uiSourceRevID= 0;
}
//---------------------------------------------------------------------------
bool NiDX9SourceCubeMapData::CreateCubeMap()
{
    D3DFORMAT eD3DFmt = (D3DFORMAT)m_kPixelFormat.GetRendererHint();

    LPDIRECT3DDEVICE9 pkD3DDevice9 = m_pkRenderer->GetD3DDevice();

    LPDIRECT3DCUBETEXTURE9 pkD3DCubeTex = NULL;
    HRESULT eD3DRet = pkD3DDevice9->CreateCubeTexture(m_uiWidth, m_usLevels,
        0, eD3DFmt, D3DPOOL_MANAGED, &pkD3DCubeTex, NULL);
    if (FAILED(eD3DRet))
    {
        NiDX9Renderer::Warning("NiDX9SourceCubeMapData::CreateCubeMap> "
           "Failed CreateCubeTexture - %s\n", 
            NiDX9ErrorString((unsigned int)eD3DRet));
        return false;
    }
    m_pkD3DTexture = pkD3DCubeTex;
    
    return true;
}
//---------------------------------------------------------------------------
void NiDX9SourceCubeMapData::CopyDataToCubeMapFace(
    const NiPixelData& kPixels, NiSourceCubeMap::FaceID eFace)
{
    kPixels.GetPixels(0, eFace);

    NIASSERT(m_pkD3DTexture);
    LPDIRECT3DCUBETEXTURE9 pkCubeMap = (LPDIRECT3DCUBETEXTURE9)m_pkD3DTexture;

    for (unsigned short i = 0; i < m_usLevels; i++)
    {
        D3DSurfacePtr pkMipLevel;
        HRESULT eD3DRet = pkCubeMap->GetCubeMapSurface(
            (D3DCUBEMAP_FACES)eFace, i, &pkMipLevel);

        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Error("NiDX9SourceCubeMapData::CopyDataToSurface>"
                " Failed GetSurfaceLevel (%d)- %s", i,
                NiDX9ErrorString((unsigned int)eD3DRet));
            return;
        }

        CopyDataToSurfaceLevel(kPixels, i, pkMipLevel, (unsigned int)eFace);

        D3D_POINTER_RELEASE(pkMipLevel);
    }
}
//---------------------------------------------------------------------------
void NiDX9SourceCubeMapData::CopyRendererDataToCubeMapFace(const
    NiDX9PersistentSrcTextureRendererData* pkPersistentSrcTextureRendererData,
    NiSourceCubeMap::FaceID eFace)
{
    pkPersistentSrcTextureRendererData->GetPixels(0, eFace);

    NIASSERT(m_pkD3DTexture);
    LPDIRECT3DCUBETEXTURE9 pkCubeMap = (LPDIRECT3DCUBETEXTURE9)m_pkD3DTexture;

    for (unsigned short i = 0; i < m_usLevels; i++)
    {
        D3DSurfacePtr pkMipLevel;
        HRESULT eD3DRet = pkCubeMap->GetCubeMapSurface(
            (D3DCUBEMAP_FACES)eFace, i, &pkMipLevel);

        if (FAILED(eD3DRet))
        {
            NiDX9Renderer::Error("NiDX9SourceCubeMapData::CopyDataToSurface>"
                " Failed GetSurfaceLevel (%d)- %s", i,
                NiDX9ErrorString((unsigned int)eD3DRet));
            return;
        }

        CopyRendererDataToSurfaceLevel(pkPersistentSrcTextureRendererData, i,
            pkMipLevel, (unsigned int)eFace);

        D3D_POINTER_RELEASE(pkMipLevel);
    }
}
//---------------------------------------------------------------------------
void NiDX9SourceCubeMapData::Update()
{
    // Textures using pixel replacement data needs not be updated
    if (m_bReplacementData)
        return;

    NiSourceCubeMap* pkTex = (NiSourceCubeMap*)m_pkTexture;

    bool bPaletteExpand = false;
    
    NiPixelData* pkSrcPixels = pkTex->GetSourcePixelData();
    if (pkSrcPixels)
    {
        if (UpdatePalette(pkSrcPixels->GetPalette()))
            bPaletteExpand = true;
    }
    
    if (pkSrcPixels)
    {
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

            // Copy to cube face
            if (m_pkD3DTexture)
            {
                for (unsigned int ui = 0; ui < pkFormatted->GetNumFaces(); 
                    ui++)
                {
                    CopyDataToCubeMapFace(*pkFormatted, 
                        (NiSourceCubeMap::FaceID)ui);
                }
            }

            // Some of the objects used locally may have zero refcounts.  
            // We have avoided using smart pointers in the function for speed.
            // Here, at the end of the function, we assign them to smart 
            // pointers to force destruction as needed
            NiPixelDataPtr spDestructor = pkFormatted;
        }
    }

}
//---------------------------------------------------------------------------
