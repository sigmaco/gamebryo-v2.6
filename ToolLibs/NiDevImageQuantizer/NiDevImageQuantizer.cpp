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

#include <NiPixelData.h>
#include "NiDevImageQuantizer.h"
#include "QuantizeUtil.h"
#include <NiString.h>

#include <math.h>

#define DIRECTDRAW_VERSION 0x0700
#include <ddraw.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

#include <D3DX9.h>
#if defined(NIDEBUG)
    // In February 2005 SDK update, D3DX9dt.lib no longer exists
    #if D3DX_SDK_VERSION >= 24
        #pragma comment(lib, "D3DX9d.lib")
    #else //#if D3DX_SDK_VERSION >= 24
        #pragma comment(lib, "D3DX9dt.lib")
    #endif //#if D3DX_SDK_VERSION >= 24
#else //#if defined(NIDEBUG)
    #pragma comment(lib, "D3DX9.lib")
#endif //#if defined(NIDEBUG)

NiImplementRTTI(NiDevImageQuantizer,NiDevImageConverter);

//---------------------------------------------------------------------------
typedef HRESULT (WINAPI * NILPDIRECTDRAWCREATEEXA)
    ( GUID FAR* lpGuid, LPVOID* lplpDD, REFIID iid, IUnknown FAR* pUnkOuter);
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::Compress(NiPixelData& kDest, 
    const NiPixelData& kSrc, int iMipmapLevel)
{
    NIASSERT(kSrc.GetPixelFormat() == m_kRGBAFormat);

    if ((kSrc.GetWidth() != kDest.GetWidth()) || 
        (kSrc.GetHeight() != kDest.GetHeight()))
        return false;

    unsigned int uiFaceCount = kSrc.GetNumFaces();
    NIASSERT(uiFaceCount == kDest.GetNumFaces());

    // If iMipmapLevel is -1, then all mimpmap levels are copied.  The source
    // must have at least as many mipmap levels as the dest.
    unsigned int uiMinLevel, uiMaxLevel;
    if (iMipmapLevel == -1)
    {
        if (kSrc.GetNumMipmapLevels() < kDest.GetNumMipmapLevels())
            return false;

        uiMinLevel = 0;
        uiMaxLevel = kDest.GetNumMipmapLevels() - 1;
    }
    else
    {
        // If a mipmap level is specified, it must exist in the source and
        // dest.
        if ((unsigned int)iMipmapLevel >= kSrc.GetNumMipmapLevels())
            return false;

        if ((unsigned int)iMipmapLevel >= kDest.GetNumMipmapLevels())
            return false;

        uiMinLevel = uiMaxLevel = (unsigned int)iMipmapLevel;
    }

    unsigned long dwFourCC;

    const NiPixelFormat& kDestFmt = kDest.GetPixelFormat();

    switch(kDestFmt.GetFormat())
    {
        case NiPixelFormat::FORMAT_DXT1:
            dwFourCC = FOURCC_DXT1;
            break;
        case NiPixelFormat::FORMAT_DXT3:
            dwFourCC = FOURCC_DXT3;
            break;
        case NiPixelFormat::FORMAT_DXT5:
            dwFourCC = FOURCC_DXT5;
            break;
        default:
            return false;
    }

    DDSURFACEDESC2 kDDSDRGBA;
    DDSURFACEDESC2 kDDSDComp;
    
    memset(&kDDSDRGBA, 0, sizeof(kDDSDRGBA));
    kDDSDRGBA.dwSize = sizeof(kDDSDRGBA);
    kDDSDRGBA.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
    kDDSDRGBA.dwFlags
        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    kDDSDRGBA.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    kDDSDRGBA.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    kDDSDRGBA.ddpfPixelFormat.dwRGBBitCount = 32;
    kDDSDRGBA.ddpfPixelFormat.dwFourCC = 0;
    kDDSDRGBA.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
    kDDSDRGBA.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
    kDDSDRGBA.ddpfPixelFormat.dwBBitMask = 0x000000ff;
    kDDSDRGBA.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;

    memset(&kDDSDComp, 0, sizeof(kDDSDRGBA));
    kDDSDComp.dwSize = sizeof(kDDSDRGBA);
    kDDSDComp.dwFlags =
        DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    kDDSDComp.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
    memset(&kDDSDComp.ddpfPixelFormat, 0, sizeof(kDDSDComp.ddpfPixelFormat));
    kDDSDComp.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    kDDSDComp.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
    kDDSDComp.ddpfPixelFormat.dwFourCC = dwFourCC;

    DDSURFACEDESC2 kDDSD2;
    memset(&kDDSD2, 0, sizeof(DDSURFACEDESC2));
    kDDSD2.dwSize = sizeof(DDSURFACEDESC2);

    // For each mipmap level,
    // 1) Copy the source pixel data into the RGBA surface.
    // 2) Use BLT to compress the RGBA surface into a DXT surface.
    // 3) Copy the data into the destination pixel data.
    for (unsigned int uiFace = 0; uiFace < uiFaceCount; uiFace++)
    {
        for (unsigned int i = uiMinLevel; i <= uiMaxLevel; i++)
        {
            // The width and height of the resulting compressed surface must 
            // be a multiple of 4.  So, we round the surface sizes to the 
            // next highest multiple of 4.  The BLT source and destination 
            // must have the same dimensions.  As a result, we need to be 
            // careful when copying into the source surface, and only copy 
            // the subset of the surface pixels that are in the source pixel 
            // data.
            kDDSDRGBA.dwWidth = kDDSDComp.dwWidth 
                = ((kSrc.GetWidth(i) + 3) & ~0x3);
            kDDSDRGBA.dwHeight = kDDSDComp.dwHeight 
                = ((kSrc.GetHeight(i) + 3) & ~0x3);

            // Create the RGBA surface of the correct size.
            LPDIRECTDRAWSURFACE7 pkRGBASurf = NULL;
            if (m_pkDD7->CreateSurface(&kDDSDRGBA, &pkRGBASurf, NULL))
                return false;

            // Create the compressed surface of the correct size.
            LPDIRECTDRAWSURFACE7 pkCompSurf = NULL;
            if (m_pkDD7->CreateSurface(&kDDSDComp, &pkCompSurf, NULL))
            {
                pkRGBASurf->Release();
                return false;
            }

            // *1* Copy RGBA pixels into RGBA surface.
            pkRGBASurf->Lock(NULL, &kDDSD2, DDLOCK_SURFACEMEMORYPTR, 0);

            unsigned int uiSrcRowStride 
                = kSrc.GetWidth(i) * kSrc.GetPixelStride();

            unsigned int uiDestRowOverBytes = 0;
            if ((4 * kDDSD2.dwWidth) > uiSrcRowStride)
                uiDestRowOverBytes = (4 * kDDSD2.dwWidth) - uiSrcRowStride;

            const unsigned char* pucSrc = kSrc.GetPixels(i, uiFace);
            unsigned int j;
            for (j = 0; j < kSrc.GetHeight(i); j++)
            {
                unsigned char *pucDest 
                    = ((unsigned char *)(kDDSD2.lpSurface)) + j * 
                    kDDSD2.lPitch;

                // Pixel formats should match exactly - just memcpy rows, 
                // but be sure to clear out the texels that are in the 
                // surface but not in the source (when the surface is 
                // bigger than the source).
                NiMemcpy(pucDest, pucSrc, uiSrcRowStride);

                // Skip extra columns in dest, clearing them.
                if (uiDestRowOverBytes)
                {
                    pucDest += uiSrcRowStride;

                    memset(pucDest, 0, uiDestRowOverBytes);
                }

                pucSrc += uiSrcRowStride;
            }

            // Skip extra rows in dest, clearing them.
            for (j = kSrc.GetHeight(i); j <  kDDSD2.dwHeight; j++)
            {
                memset(((unsigned char *)(kDDSD2.lpSurface)) + j * 
                    kDDSD2.lPitch, 0, 4 * kDDSD2.dwWidth);
            }

            pkRGBASurf->Unlock(NULL);

            // *2* Compress the RGBA surface.
            if (pkCompSurf->Blt(NULL, pkRGBASurf, NULL, DDBLT_WAIT, NULL))
            {
                pkRGBASurf->Release();
                pkCompSurf->Release();
                return false;
            }

            // *3* Copy the compressed data into the destination pixel data.
            pkCompSurf->Lock(NULL, &kDDSD2, DDLOCK_SURFACEMEMORYPTR, 0);

            NIASSERT(!(kDDSD2.dwFlags & DDSD_PITCH));

            // Copy the compressed, linear data from the surface to the pixel
            // data object.
            unsigned int uiDestSize = (NiUInt32)kDest.GetSizeInBytes(i);

            NiMemcpy(kDest.GetPixels(i, uiFace), 
                (unsigned char*)(kDDSD2.lpSurface), uiDestSize);

            pkCompSurf->Unlock(NULL);

            pkRGBASurf->Release();
            pkCompSurf->Release();
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::RGBA128toRGBA64(NiPixelData& kDest,
    const NiPixelData& kSrc)
{
    for (unsigned int uiFace = 0; uiFace < kSrc.GetNumFaces(); uiFace++)
    {
        for (unsigned int uiMipMap = 0; uiMipMap < kSrc.GetNumMipmapLevels();
            uiMipMap++)
        {
            const float* pfSrcPix =
                (const float*) kSrc.GetPixels(uiMipMap, uiFace);
            D3DXFLOAT16* pkDestPix =
                (D3DXFLOAT16*) kDest.GetPixels(uiMipMap, uiFace);


            unsigned int uiElementCount = kSrc.GetWidth(uiMipMap, uiFace) * 
                kSrc.GetHeight(uiMipMap, uiFace) * 4;

            D3DXFloat32To16Array(pkDestPix, pfSrcPix, uiElementCount);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::RGBA64toRGBA128(NiPixelData& kDest,
    const NiPixelData& kSrc)
{
    for (unsigned int uiFace = 0; uiFace < kSrc.GetNumFaces(); uiFace++)
    {
        for (unsigned int uiMipMap = 0; uiMipMap < kSrc.GetNumMipmapLevels();
            uiMipMap++)
        {
            FLOAT* pfDestPix =
                (FLOAT*) kDest.GetPixels(uiMipMap, uiFace);
            D3DXFLOAT16* pkSrcPix =
                (D3DXFLOAT16*) kSrc.GetPixels(uiMipMap, uiFace);

            unsigned int uiElementCount = kSrc.GetWidth(uiMipMap, uiFace) * 
                kSrc.GetHeight(uiMipMap, uiFace) * 4; 

            D3DXFloat16To32Array(pfDestPix, pkSrcPix, uiElementCount);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
NiDevImageQuantizer* NiDevImageQuantizer::Create()
{
    NILPDIRECTDRAWCREATEEXA pfnDirectDrawCreateEx = NULL;

    HINSTANCE hInst = LoadLibrary("DDRAW.DLL");
    HMODULE hDDrawDLL = GetModuleHandle("DDRAW.DLL");
    if (hDDrawDLL)
    {
        // Find the DDraw enumeration function, and call it.  This function is
        // statically linked into DX6 and above, but not on NT4.0.  So, we
        // need to query it.
        pfnDirectDrawCreateEx = (NILPDIRECTDRAWCREATEEXA)
            GetProcAddress( hDDrawDLL, "DirectDrawCreateEx" );

        if (!pfnDirectDrawCreateEx)
            return NULL;
    }
    else
    {
        return NULL;
    }

    LPDIRECTDRAW7 pkDD7 = NULL;
    if(pfnDirectDrawCreateEx(NULL, (void**)&pkDD7, IID_IDirectDraw7, NULL))
        return NULL;

    if (pkDD7->SetCooperativeLevel(NULL, DDSCL_NORMAL))
        return NULL;

    NiDevImageQuantizer* pkThis = NiNew NiDevImageQuantizer;

    pkThis->m_pkDD7 = pkDD7;

    pkThis->m_pkDDrawDLL = hInst;

    return pkThis;
}
//---------------------------------------------------------------------------
NiDevImageQuantizer::NiDevImageQuantizer()
: m_kRGBAFormat(NiPixelFormat::BGRA8888)
{
    m_pkDD7 = NULL;
    m_pkDDrawDLL = 0;
    m_pQuantUtil = NiExternalNew QuantizeUtil;
}
//---------------------------------------------------------------------------
NiDevImageQuantizer::~NiDevImageQuantizer()
{
    if (m_pkDD7)
        m_pkDD7->Release();

    FreeLibrary((HINSTANCE)m_pkDDrawDLL);
    NiExternalDelete m_pQuantUtil;
}
//---------------------------------------------------------------------------
void NiDevImageQuantizer::SetNumberOfColorBits( unsigned int uiColorBits )
{
    NIASSERT(m_pQuantUtil);
    
    if (m_pQuantUtil)
        m_pQuantUtil->SetNumberOfColorBits( uiColorBits );
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::CanConvertPixelData(
    const NiPixelFormat& kSrcFmt, const NiPixelFormat& kDestFmt)
{
    if (NiDevImageConverter::CanConvertPixelData(kSrcFmt, kDestFmt))
        return true;

    // One more case can be handled - compressing textures.  If the source can
    // be converted to the given RGBA format, then this converter can compress
    // that format.
    if (kDestFmt.GetCompressed() &&
        NiDevImageConverter::CanConvertPixelData(kSrcFmt, m_kRGBAFormat))
        return true;

    // One more case can be handled - palettizing textures.  If the source can
    // be converted to the given RGBA format, then this converter can compress
    // that format.
    if (kDestFmt.GetPalettized() && 
        NiDevImageConverter::CanConvertPixelData(kSrcFmt, 
        NiPixelFormat::RGBA32))
    {
        return true;
    }

    if (kSrcFmt == NiPixelFormat::RGBA128 && 
        kDestFmt == NiPixelFormat::RGBA64)
    {
        return true;
    }

    if (kSrcFmt == NiPixelFormat::RGBA64 && 
        kDestFmt == NiPixelFormat::RGBA128)
    {
        return true;
    }

    return false; 
}
//---------------------------------------------------------------------------
NiPixelData* NiDevImageQuantizer::ConvertPixelData(
    const NiPixelData& kSrc, const NiPixelFormat& kDestFmt, 
    NiPixelData* pkOptDest, bool bMipmap)
{
    if (kSrc.GetPixelFormat() == NiPixelFormat::RGBA128 && 
        kDestFmt == NiPixelFormat::RGBA64)
    {
        unsigned int uiMipLevels = kSrc.GetNumMipmapLevels();
        NiPixelData* pkDest = NiNew NiPixelData(kSrc.GetWidth(), 
            kSrc.GetHeight(), kDestFmt, uiMipLevels, kSrc.GetNumFaces());
        bool bSuccess = RGBA128toRGBA64(*pkDest,kSrc);
        if (!bSuccess)
        {
            NiDelete pkDest;
            return NULL;
        }
        else
        {
            return pkDest;
        }
    }
    else if (kSrc.GetPixelFormat() == NiPixelFormat::RGBA64 && 
        kDestFmt == NiPixelFormat::RGBA128)
    {
        unsigned int uiMipLevels = kSrc.GetNumMipmapLevels();
        NiPixelData* pkDest = NiNew NiPixelData(kSrc.GetWidth(), 
            kSrc.GetHeight(), kDestFmt, uiMipLevels, kSrc.GetNumFaces());
        bool bSuccess = RGBA64toRGBA128(*pkDest,kSrc);
        if (!bSuccess)
        {
            NiDelete pkDest;
            return NULL;
        }
        else
        {
            return pkDest;
        }
    }
    else if (kDestFmt.GetCompressed() && (kDestFmt != kSrc.GetPixelFormat()))
    {
        // if the destination is compressed and the source is not the same
        // compressed format, then this converter is the only hope - we need 
        // not even try the base class
        
        NiPixelDataPtr spRGBATemp = NULL;

        if (NiDevImageConverter::CanConvertPixelData(kSrc.GetPixelFormat(), 
            m_kRGBAFormat))
        {
            // convert the data to the intermediate RGBA format that the 
            // compressor requries
            spRGBATemp = NiDevImageConverter::ConvertPixelData(
                kSrc, m_kRGBAFormat, NULL, bMipmap);            
        }
        else
        {
            // convert the data to the intermediate RGBA format that the 
            // compressor requries
            NiPixelDataPtr spNewSrc = ConvertPixelData(kSrc,
                NiPixelFormat::RGBA32, NULL, bMipmap);
            NIASSERT(spNewSrc);
            spRGBATemp = NiDevImageConverter::ConvertPixelData(
                *(spNewSrc), m_kRGBAFormat, NULL, bMipmap);
        }

        NiPixelData* pkLocalDest = NULL;
        NiPixelData* pkDest = NULL;

        if (pkOptDest && (pkOptDest->GetWidth() == spRGBATemp->GetWidth()) &&
             (pkOptDest->GetHeight() == spRGBATemp->GetHeight()) &&
             (pkOptDest->GetNumMipmapLevels() 
                == spRGBATemp->GetNumMipmapLevels()) &&
             (pkOptDest->GetPixelFormat() == kDestFmt))
        {
            // Use the supplied (matching) optional destination.
            pkDest = pkOptDest;
        }
        else
        {
            // Create the destination data object.
            pkDest = NiNew NiPixelData(kSrc.GetWidth(), kSrc.GetHeight(), 
                kDestFmt, kSrc.GetNumMipmapLevels(), kSrc.GetNumFaces());
            pkLocalDest = pkDest;
        }

        NIASSERT(pkDest);

        // Compress the data.
        if (spRGBATemp && Compress(*pkDest, *spRGBATemp, -1))
        {
            return pkDest;
        }
        else
        {
            // Delete the temporary destination (if it was alocated locally).
            NiPixelDataPtr spDelete = pkLocalDest;
            return NULL;
        }
    }
    else
    {
        return NiDevImageConverter::ConvertPixelData(kSrc, kDestFmt, 
            pkOptDest, bMipmap);
    }
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::ConvertPixelDataFormat(NiPixelData& kDest, 
    const NiPixelData& kSrc, int iMipmapLevel)
{
    NiPixelFormat::Format eDestFmt = kDest.GetPixelFormat().GetFormat();
    NiPixelFormat::Format eSrcFmt = kSrc.GetPixelFormat().GetFormat();

    if ((eDestFmt == NiPixelFormat::FORMAT_PAL ||
        eDestFmt == NiPixelFormat::FORMAT_PALALPHA) && 
        (eSrcFmt == NiPixelFormat::FORMAT_RGBA || 
        eSrcFmt == NiPixelFormat::FORMAT_RGB || 
        CanConvertPixelData(kSrc.GetPixelFormat(), NiPixelFormat::RGBA32)))
    {
        unsigned int uiDestBPP = kDest.GetPixelFormat().GetBitsPerPixel();
        unsigned int uiSrcBPP = kSrc.GetPixelFormat().GetBitsPerPixel();

        if (uiDestBPP == 4 || uiDestBPP == 8)
        { 
            if (uiSrcBPP == 32)
            {
                SetNumberOfColorBits(uiDestBPP);
                return Quantize(kDest, kSrc, iMipmapLevel);
            }
            else
            {
                // We must now blow out to 32-bit.
                bool bMipMap = kSrc.GetNumMipmapLevels() > 1;
                NiPixelDataPtr spNewSrc = ConvertPixelData(kSrc,
                    NiPixelFormat::RGBA32, NULL, bMipMap);
                NIASSERT(spNewSrc);
                
                SetNumberOfColorBits(uiDestBPP);
                return Quantize(kDest, *spNewSrc, iMipmapLevel);
            }
        }
    }
    else if (kDest.GetPixelFormat().GetCompressed() && 
        (kDest.GetPixelFormat() != kSrc.GetPixelFormat()))
    {
        return Compress(kDest, kSrc, iMipmapLevel);
    }
    

    return NiDevImageConverter::ConvertPixelDataFormat(kDest, kSrc, 
            iMipmapLevel);
    
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::Quantize(NiPixelData& kDest, 
    const NiPixelData& kSrc, int iMipmapLevel)
{
    unsigned int uiPalSize = m_pQuantUtil->GetNumberOfPaletteColors();

    // kSrcFmt & m_kRGBAFormat may have different color 
    if ((kSrc.GetWidth() != kDest.GetWidth()) || 
        (kSrc.GetHeight() != kDest.GetHeight()))
        return false;

    // If iMipmapLevel is -1, then all mimpmap levels are copied.  The source
    // must have at least as many mipmap levels as the dest.
    unsigned int uiMinLevel, uiMaxLevel;
    if (iMipmapLevel == -1)
    {
        if (kSrc.GetNumMipmapLevels() < kDest.GetNumMipmapLevels())
            return false;

        uiMinLevel = 0;
        uiMaxLevel = kDest.GetNumMipmapLevels() - 1;
    }
    else
    {
        // If a mipmap level is specified, it must exist in the source and
        // dest.
        if ((unsigned int)iMipmapLevel >= kSrc.GetNumMipmapLevels())
            return false;

        if ((unsigned int)iMipmapLevel >= kDest.GetNumMipmapLevels())
            return false;

        uiMinLevel = uiMaxLevel = (unsigned int)iMipmapLevel;
    }

    // Step 1 - Transfer src into quantizer structure.

    unsigned long ulWidth = (unsigned long)kSrc.GetTotalSizeInBytes() /
        kSrc.GetPixelStride();
    unsigned long ulHeight = 1;
    unsigned long ulPixelSize = kSrc.GetPixelStride();
    const unsigned char* pucPixels = kSrc.GetPixels();
    unsigned long ulBytes = ulPixelSize;


    // QuantizeUtil expects pixels to have RGB values first and the last value
    // reserved for alpha.  Quantizer doesn't care if values are read as RGB,
    // BGR, etc., as quantization will yield like-formatted palettes.

    ImgInfo MyImgInfo;

    MyImgInfo.pucSaveIndexData = NULL;
    MyImgInfo.ulWidth = ulWidth;
    MyImgInfo.ulHeight = ulHeight;
    MyImgInfo.ulRowBytes = ulWidth * ulBytes;
    MyImgInfo.pucRGBAData = pucPixels;  
    MyImgInfo.pucIndexedData = NULL;

    MyImgInfo.ppucRowPointers = NiAlloc(const unsigned char*, ulHeight);
    
    unsigned long ulI;
    for (ulI=0; ulI< ulHeight; ulI++)
    {
        MyImgInfo.ppucRowPointers[ulI] = 
            pucPixels + ulI * ulWidth * ulBytes;
    }

    MyImgInfo.iInterlaced = 0;
    MyImgInfo.iChannels = ulBytes;
    MyImgInfo.iSampleDepth = 0;
    MyImgInfo.iNumPalette = 0;      
    MyImgInfo.iNumTrans = 0;    

    memset( MyImgInfo.Palette, 0 , sizeof(RGBAPixel) * uiPalSize );

    // Step 2 - Quantize and get structure back.
    m_pQuantUtil->Quantize(&MyImgInfo);

    // Step 3 - Transfer structure to NiPixelData kDest.  Later app could use
    // for TGA-writer, etc.
    // Pixel formats should match exactly - just memcpy rows, but be sure to
    // clear out the texels that are in the surface but not in the source
    // (when the surface is bigger than the source).
    unsigned char* pucDestPixels = kDest.GetPixels(0);

    if (uiPalSize == 16)
    {
        // This is 4-bit, we pack in the info...

        // Check for odd size
        unsigned int uiByteSize = ulWidth * ulHeight;
        if (uiByteSize %2)
        {
            uiByteSize++;
        }

        unsigned char* pkPackedPixels = NiAlloc(unsigned char, uiByteSize);
        for (unsigned int i=0; i<ulWidth*ulHeight; i++)
        {
            if (i%2 == 0)   // Even.
                pkPackedPixels[i>>1] = MyImgInfo.pucSaveIndexData[i];
            else    // Odd.
                pkPackedPixels[i>>1] |= MyImgInfo.pucSaveIndexData[i] << 4;
        }

        NiMemcpy(pucDestPixels, pkPackedPixels, ulWidth * ulHeight >> 1);

        NiFree(pkPackedPixels);
    }
    else
    {
        // Assume 8-bit.
        NiMemcpy(pucDestPixels, MyImgInfo.pucSaveIndexData,
            ulWidth * ulHeight);
    }

    // Copy in colormap.  We allocate the palette to 256 regardless of the
    // uiPalSize since NiPalette always assumes 256.
    NiPalette::PaletteEntry* pkPalEntry = 
        NiNew NiPalette::PaletteEntry[256];

    for ( ulI = 0; ulI < uiPalSize; ulI++ )
    {
        RGBAPixel* pkQuantPal = &(MyImgInfo.Palette[ulI]);
        NiPalette::PaletteEntry* pkPixPal = &(pkPalEntry[ulI]);
        
        pkPixPal->m_ucRed   = pkQuantPal->ucRed;
        pkPixPal->m_ucGreen = pkQuantPal->ucGreen;
        pkPixPal->m_ucBlue  = pkQuantPal->ucBlue;
        pkPixPal->m_ucAlpha = pkQuantPal->ucAlpha;
    }

    NiPalette* pkPal = NiNew NiPalette(true, uiPalSize, pkPalEntry);
    kDest.SetPalette( pkPal );

    // Since the palette creates it's own copy of the palette entry,
    // we must delete the one we previously allocated.
    NiDelete [] pkPalEntry;

    NiFree(MyImgInfo.ppucRowPointers);
    MyImgInfo.ppucRowPointers = NULL;
    return true;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::IsAnAlphaSupportedMipmapGenerationFormat(
    const NiPixelFormat& kSrcFmt)
{
    if (NiDevImageConverter::IsAnAlphaSupportedMipmapGenerationFormat(
        kSrcFmt))
        return true;

    // Quantizer also supports RGBA16 and BGRA16.
    if ((kSrcFmt == NiPixelFormat::RGBA16) ||
        (kSrcFmt == NiPixelFormat::BGRA16))
        return true;

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::IsANonAlphaSupportedMipmapGenerationFormat(
    const NiPixelFormat& kSrcFmt)
{
    if (NiDevImageConverter::IsANonAlphaSupportedMipmapGenerationFormat(
        kSrcFmt))
        return true;

    // Quantizer does not add any additional support yet.
    return false;
}
//---------------------------------------------------------------------------
NiPixelDataPtr NiDevImageQuantizer::GenerateMipmapLevels(
    NiPixelDataPtr spSrc, NiPixelDataPtr spOptDest)
{
    unsigned int uiComputedMipmapLevelCount = 
        NiPixelData::ComputeMipmapLevels(spSrc->GetWidth(),
        spSrc->GetHeight());

    if ((spSrc != NULL) &&
        (spSrc->GetNumMipmapLevels() == uiComputedMipmapLevelCount))
    {
        spOptDest = (NiPixelData*)spSrc;
        return spOptDest;
    }

    // Can have a mipmap with non-power-of-two dimensions if it is supplied
    // with the texture.  Mipmaps for non-power-of-two dimensions will not be
    // generated automatically.
    if (!(NiIsPowerOf2(spSrc->GetWidth()) &&
        NiIsPowerOf2(spSrc->GetHeight())))
    {
        spOptDest = (NiPixelData*)spSrc;
        return spOptDest;
    }

    const NiPixelFormat& kSrcFmt = spSrc->GetPixelFormat();
    NiPixelFormat kDestFmt;

    kDestFmt = kSrcFmt;
    NiPixelDataPtr spDest;

    if ((kSrcFmt == NiPixelFormat::RGBA16) ||
        (kSrcFmt == NiPixelFormat::BGRA16))
    {
        if (kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0)
        {
            spSrc = ConvertPixelData(*spSrc, NiPixelFormat::RGBA32, NULL,
                false);
        }
        else
        {
            spSrc = ConvertPixelData(*spSrc, NiPixelFormat::RGB24, NULL,
                false);
        }
    }
    else if (kSrcFmt.GetCompressed())
    {
        spSrc = ConvertPixelData(*spSrc, NiPixelFormat::RGBA32, NULL, false);
    }

    if (kSrcFmt == NiPixelFormat::RGBA128)
        spDest = GenerateMipmapLevelsRGBA128(spSrc, spOptDest);
    else if (kSrcFmt == NiPixelFormat::RGBA64)
        spDest = GenerateMipmapLevelsRGBA64(spSrc, spOptDest);
    else
        spDest = NiDevImageConverter::GenerateMipmapLevels(spSrc, spOptDest);

    NIASSERT(spDest->GetNumMipmapLevels() == uiComputedMipmapLevelCount);

    if (spDest && spDest->GetPixelFormat() != kDestFmt)
        spDest = ConvertPixelData(*spDest, kDestFmt, spOptDest, true);

    return spDest;
}
//---------------------------------------------------------------------------
NiPixelDataPtr NiDevImageQuantizer::GenerateMipmapLevelsRGBA64(
    NiPixelDataPtr spSrc, NiPixelDataPtr spOptDest)
{
    NiPixelDataPtr spDest;

    unsigned int uiNumMipMaps = NiPixelData::ComputeMipmapLevels(
        spSrc->GetWidth(), spSrc->GetHeight());

    spDest = NiNew NiPixelData(spSrc->GetWidth(), spSrc->GetHeight(),
        spSrc->GetPixelFormat(), uiNumMipMaps ,spSrc->GetNumFaces());

    NiPixelData& kDest = *(spDest);
    unsigned int uiMipMap = 0;

    // Copy the zero-th mipmap from the src to the destination.
    for (unsigned int uiFace = 0; uiFace < spSrc->GetNumFaces(); uiFace++)
    {
        const unsigned char* pucSrcPixels = 
            spSrc->GetPixels(uiMipMap, uiFace);
        unsigned char* pucDestPixels = 
            spDest->GetPixels(uiMipMap, uiFace);

        unsigned int uiSizeInBytes =
            (NiUInt32)spSrc->GetSizeInBytes(uiMipMap, uiFace);
        unsigned int uiDestSize = 
            (NiUInt32)spDest->GetSizeInBytes(uiMipMap, uiFace);

        NiMemcpy(pucDestPixels, uiDestSize, pucSrcPixels, uiSizeInBytes);
    }

    // Perform a simple box filter on the rest of the levels of the mipmap.
    for (unsigned int uiFace = 0; uiFace < spDest->GetNumFaces(); uiFace++)
    {
        unsigned int x, y, k;

        for (k = 1; k < uiNumMipMaps; k++)
        {
            unsigned int uiSrcWidth = 
                spDest->GetWidth(k-1, uiFace);
            unsigned int uiSrcHeight = 
                spDest->GetHeight(k-1, uiFace);
            NI_UNUSED_ARG(uiSrcHeight);
            NI_UNUSED_ARG(uiSrcWidth);

            unsigned int uiDestWidth = spDest->GetWidth(k, uiFace);
            unsigned int uiDestHeight = spDest->GetHeight(k, uiFace);

            NIASSERT((uiDestWidth == uiSrcWidth / 2) || 
                (uiDestWidth == 1));
            NIASSERT((uiDestHeight == uiSrcHeight / 2) || 
                (uiDestHeight == 1));

            for (y = 0; y < uiDestHeight; y++) 
            {
                for (x = 0; x < uiDestWidth; x++) 
                {
                    D3DXFLOAT16* pfSrcUpLeft = (D3DXFLOAT16*)
                        (kDest(x*2, y*2, k-1, uiFace));
                    D3DXFLOAT16* pfSrcUpRight = (D3DXFLOAT16*)
                        (kDest(x*2+1, y*2, k-1, uiFace));
                    D3DXFLOAT16* pfSrcDownLeft = (D3DXFLOAT16*)
                        (kDest(x*2, y*2+1, k-1, uiFace));
                    D3DXFLOAT16* pfSrcDownRight = (D3DXFLOAT16*)
                        (kDest(x*2+1, y*2+1, k-1, uiFace));

                    D3DXFLOAT16* pfDest = (D3DXFLOAT16*)
                        (kDest(x, y, k, uiFace));
    
                    for (unsigned int ui = 0; ui < 4; ui++)
                    {
                        pfDest[ui] = ((FLOAT)pfSrcUpLeft[ui] + 
                            (FLOAT)pfSrcUpRight[ui] +
                            (FLOAT)pfSrcDownLeft[ui] + 
                            (FLOAT)pfSrcDownRight[ui])/4.0f;
                    }
                }
            }
        }
    }

    return spDest;
}
//---------------------------------------------------------------------------
NiPixelDataPtr NiDevImageQuantizer::GenerateMipmapLevelsRGBA128(
    NiPixelDataPtr spSrc, NiPixelDataPtr spOptDest)
{
    NiPixelDataPtr spDest;

    unsigned int uiNumMipMaps = NiPixelData::ComputeMipmapLevels(
        spSrc->GetWidth(), spSrc->GetHeight());
    
    spDest = NiNew NiPixelData(spSrc->GetWidth(), spSrc->GetHeight(),
        spSrc->GetPixelFormat(), uiNumMipMaps ,spSrc->GetNumFaces());

    NiPixelData& kDest = *(spDest);
    unsigned int uiMipMap = 0;
    
    // Copy the zero-th mipmap from the src to the destination.
    for (unsigned int uiFace = 0; uiFace < spSrc->GetNumFaces(); uiFace++)
    {
        const unsigned char* pucSrcPixels = 
            spSrc->GetPixels(uiMipMap, uiFace);
        unsigned char* pucDestPixels = 
            spDest->GetPixels(uiMipMap, uiFace);

        unsigned int uiSizeInBytes = 
            (NiUInt32)spSrc->GetSizeInBytes(uiMipMap, uiFace);
        unsigned int uiDestSize = 
            (NiUInt32)spDest->GetSizeInBytes(uiMipMap, uiFace);

        NiMemcpy(pucDestPixels, uiDestSize, pucSrcPixels, uiSizeInBytes);
    }

    // Perform a simple box filter on the rest of the levels of the mipmap.
    for (unsigned int uiFace = 0; uiFace < spDest->GetNumFaces(); uiFace++)
    {
        unsigned int x, y, k;

        for (k = 1; k < uiNumMipMaps; k++)
        {
            unsigned int uiSrcWidth = 
                spDest->GetWidth(k-1, uiFace);
            unsigned int uiSrcHeight = 
                spDest->GetHeight(k-1, uiFace);
            NI_UNUSED_ARG(uiSrcHeight);
            NI_UNUSED_ARG(uiSrcWidth);

            unsigned int uiDestWidth = spDest->GetWidth(k, uiFace);
            unsigned int uiDestHeight = spDest->GetHeight(k, uiFace);

            NIASSERT((uiDestWidth == uiSrcWidth / 2) || 
                (uiDestWidth == 1));
            NIASSERT((uiDestHeight == uiSrcHeight / 2) || 
                (uiDestHeight == 1));

            for (y = 0; y < uiDestHeight; y++) 
            {
                for (x = 0; x < uiDestWidth; x++) 
                {
                    float* pfSrcUpLeft = (float*)
                        (kDest(x*2, y*2, k-1, uiFace));
                    float* pfSrcUpRight = (float*)
                        (kDest(x*2+1, y*2, k-1, uiFace));
                    float* pfSrcDownLeft = (float*)
                        (kDest(x*2, y*2+1, k-1, uiFace));
                    float* pfSrcDownRight = (float*)
                        (kDest(x*2+1, y*2+1, k-1, uiFace));

                    float* pfDest = (float*)(kDest(x, y, k, uiFace));
   
                    for (unsigned int ui = 0; ui < 4; ui++)
                    {
                        pfDest[ui] = (pfSrcUpLeft[ui] + 
                            pfSrcUpRight[ui] +
                            pfSrcDownLeft[ui] + 
                            pfSrcDownRight[ui])/4.0f;
                    }
                }
            }
        }
    }
    
    return spDest;
}
//---------------------------------------------------------------------------
unsigned int NiDevImageQuantizer::CalculateMinWiiTextureSizeDivideBy(
    unsigned int uiWidth, unsigned int uiHeight)
{
    unsigned int uiSizeDivideBy = 1;

    if ((uiWidth > MAX_WII_TEXTURE_DIMENSION || 
        uiHeight > MAX_WII_TEXTURE_DIMENSION))
    {
        // If the DIVIDE BY exceeds 1024, there is probably some other
        // problem since one of the texture's dimensions is greater
        // than MAX_WII_TEXTURE_DIMENSION*MAX_DIVIDE_BY.
        // Return 0 to indicate an error.
        uiSizeDivideBy = 0;
        for(unsigned int i=1; i<=MAX_DIVIDE_BY; i<<=1)
        {
            if ((uiWidth/i) <= MAX_WII_TEXTURE_DIMENSION &&
                (uiHeight/i) <= MAX_WII_TEXTURE_DIMENSION)
            {
                uiSizeDivideBy = i;
                break;
            }
        }
    }

    return uiSizeDivideBy;
}
//---------------------------------------------------------------------------
NiPixelDataPtr NiDevImageQuantizer::ProcessPixelData(
    NiPixelDataPtr spPixelData, NiTexture::FormatPrefs kPrefs, bool bMipMap, 
    bool, NiPixelFormat kDestFormat, 
    unsigned int uiSizeDivideBy, NiDevImageConverter::Platform ePlatform, 
    const char* pcFilename, char* pcErrorBuffer, 
    unsigned int uiErrorBufferLength)
{
    bool bRequiresConversion = false;
    NiDevImageQuantizer* pkConverter = 
            (NiDevImageQuantizer*) NiImageConverter::GetImageConverter();
    NIASSERT(pkConverter);
        
    if (!spPixelData)
    {
        NiSprintf(pcErrorBuffer, uiErrorBufferLength, 
            "Error: Texture \"%s\" pixel data could not be found.",
            pcFilename);

        return NULL;
    }

    // Wii can only support textures with height and width less than 1024
    if (ePlatform == NiDevImageConverter::WII)
    {      
        unsigned int uiWiiMinSizeDivideBy = 
            CalculateMinWiiTextureSizeDivideBy( spPixelData->GetWidth(), 
                spPixelData->GetHeight());
        if (uiWiiMinSizeDivideBy == 0)
        {
            NiSprintf(pcErrorBuffer, uiErrorBufferLength, 
                "Error: Texture \"%s\" dimension (%u x %u) is too large "
                "for Wii (greater than %u). Recalculated \"divide by\" "
                "factor to resize texture exceeds maximum (%u)."
                "Resize texture or set "
                "Wii-specific texture divide by value and try again.",
                pcFilename,
                spPixelData->GetWidth(), spPixelData->GetHeight(),
                MAX_WII_TEXTURE_DIMENSION,
                MAX_DIVIDE_BY);

            return NULL;
        }
        else if (uiWiiMinSizeDivideBy > uiSizeDivideBy)
        {
            NiSprintf(pcErrorBuffer, uiErrorBufferLength, 
                "Warning: Texture \"%s\" dimension (%u x %u) is too large "
                "for Wii (greater than %u). Setting texture size "
                "\"divide by\" to %u to resize texture to %u x %u.",
                pcFilename,
                spPixelData->GetWidth(), spPixelData->GetHeight(),
                MAX_WII_TEXTURE_DIMENSION,
                uiWiiMinSizeDivideBy,
                spPixelData->GetWidth()/uiWiiMinSizeDivideBy,
                spPixelData->GetHeight()/uiWiiMinSizeDivideBy);

            uiSizeDivideBy = uiWiiMinSizeDivideBy;
        }
    }

    NiPixelFormat kOriginalFormat = spPixelData->GetPixelFormat();
    if (kOriginalFormat.GetFormat() == NiPixelFormat::FORMAT_RGBA && 
        kDestFormat.GetFormat() == NiPixelFormat::FORMAT_PAL)
    {
        kDestFormat = NiPixelFormat::PALA8;
    }

    if (kDestFormat != kOriginalFormat)
    {
        bRequiresConversion = true;
    }

    if (uiSizeDivideBy != 1)
        bRequiresConversion = true;

    if (bMipMap == true && spPixelData->GetNumMipmapLevels() == 1)
        bRequiresConversion = true;

    const NiPixelFormat* pkFinalFormat = pkConverter->FindClosestPixelFormat(
        ePlatform, kPrefs, kOriginalFormat, kDestFormat);

    if (*pkFinalFormat != kDestFormat)
    {
        kDestFormat = *pkFinalFormat;
        bRequiresConversion = true;
    }

    if (spPixelData && bRequiresConversion)
    {
        //Resize the image as necessary.
        if (uiSizeDivideBy != 1 || bMipMap)
        {
            spPixelData = 
                pkConverter->GenerateMipmapLevels(spPixelData, NULL);
            
            // We can't reduce because there are not mip map levels.
            if (spPixelData->GetNumMipmapLevels() == 1)
                uiSizeDivideBy = 1;

            if (spPixelData->GetNumMipmapLevels() != 1 && uiSizeDivideBy != 1)
            {
                unsigned int uiWhichMipMapLevel = 0;
                if (uiSizeDivideBy == 1)
                    uiWhichMipMapLevel = 0;
                else if (uiSizeDivideBy == 2)
                    uiWhichMipMapLevel = 1;
                else if (uiSizeDivideBy == 4)
                    uiWhichMipMapLevel = 2;
                else if (uiSizeDivideBy == 8)
                    uiWhichMipMapLevel = 3;
                else if (uiSizeDivideBy == 16)
                    uiWhichMipMapLevel = 4;
                else if (uiSizeDivideBy == 32)
                    uiWhichMipMapLevel = 5;
                else if (uiSizeDivideBy == 64)
                    uiWhichMipMapLevel = 6;
                else
                {
                    NiSprintf(pcErrorBuffer, uiErrorBufferLength, 
                        "Warning: Texture \"%s\" divide by setting is "
                        "greater than %u or is not "
                        "a power of two: %u. Defaulting to MIP map level 0.",
                        pcFilename, MAX_DIVIDE_BY, uiSizeDivideBy);

                    uiWhichMipMapLevel = 0;

                    NIASSERT(!"uiSizeDivideBy must be a power of two!");
                }

                unsigned int uiWidth = 
                    spPixelData->GetWidth(uiWhichMipMapLevel);
                unsigned int uiHeight= 
                    spPixelData->GetHeight(uiWhichMipMapLevel);
                unsigned int uiMipMapLevels = 
                    spPixelData->GetNumMipmapLevels() - uiWhichMipMapLevel;
                
                NiPixelDataPtr spTempData = NiNew NiPixelData(uiWidth, 
                    uiHeight, spPixelData->GetPixelFormat(), uiMipMapLevels,
                    spPixelData->GetNumFaces());

                if (spPixelData->GetPalette() != NULL)
                    spTempData->SetPalette(spPixelData->GetPalette());

                for (unsigned int uiFace = 0; 
                    uiFace < spPixelData->GetNumFaces(); uiFace++)
                {
                    for (unsigned int ui = 0, uj = uiWhichMipMapLevel; 
                        ui < uiMipMapLevels; ui++, uj++)
                    {
                        unsigned char* pcSrcPixels = 
                            spPixelData->GetPixels(uj, uiFace);
                        unsigned char* pcDestPixels = 
                            spTempData->GetPixels(ui, uiFace);

                        unsigned int uiSrcSize = 
                            (NiUInt32)spPixelData->GetSizeInBytes(uj, uiFace);
                        NIASSERT(uiSrcSize == spTempData->GetSizeInBytes(ui,
                            uiFace));

                        NiMemcpy(pcDestPixels, pcSrcPixels, uiSrcSize);
                    }
                }

                spPixelData = spTempData;
            }
        }
        
        //Compress or convert the image as necessary...
        if (kDestFormat != kOriginalFormat)
        {
            if (!pkConverter->CanConvertPixelData(
                kOriginalFormat, kDestFormat))
            {
                char* pcOriginalFormat = 
                    GetImageFormatString(kOriginalFormat);
                char* pcDestFormat = GetImageFormatString(kDestFormat);
                NiSprintf(pcErrorBuffer, uiErrorBufferLength,
                    "Error: Cannot convert texture \"%s\" from a "
                    "%s format to a %s format. Try recreating the original "
                    "image as a 32 bit R8G8B8A8 image.", 
                    pcFilename,
                    pcOriginalFormat, pcDestFormat);

                NiFree(pcOriginalFormat);
                NiFree(pcDestFormat);
                return NULL;
            }
            spPixelData = pkConverter->ConvertPixelData(*spPixelData, 
                kDestFormat, NULL, bMipMap);
            NIASSERT(spPixelData);
            NIASSERT(spPixelData->GetPixelFormat() == kDestFormat);
        }
    }

    return spPixelData;
}
//---------------------------------------------------------------------------
char* NiDevImageQuantizer::GetImageFormatString(NiPixelFormat kFormat)
{
    char* pcString = NiAlloc(char, 256);
    NiSprintf(pcString, 256, "%d bpp ", (int) kFormat.GetBitsPerPixel());
    
    NiPixelFormat::Component aeWhichOrder[4];
    memset(aeWhichOrder, 0, 4 * sizeof(NiPixelFormat::Component));

    unsigned int uiCount = 0;
    for (unsigned int ui = 0; ui < 4; ui++)
    {
        NiPixelFormat::Component eComp0 = (NiPixelFormat::Component) ui;
        unsigned int uiMask0 = kFormat.GetMask(eComp0);

        unsigned int uiIndex = uiCount;
        for (unsigned int uj = 0; uj < uiCount; uj++)
        {
            NiPixelFormat::Component eComp1 = aeWhichOrder[uj];
            unsigned int uiMask1 = kFormat.GetMask(eComp1);

            if (uiMask0 < uiMask1)
            {
                for (int ix = uiCount; ix > (int)uj; ix--)
                {
                    aeWhichOrder[ix] = aeWhichOrder[ix-1];
                }
                uiIndex = uj;
                break;
            }
        }

        aeWhichOrder[uiIndex] = eComp0;
        uiCount++;
    }
    
    NiString kFormatString;
    NiPixelFormat::Format kFormatType = kFormat.GetFormat();
    if (kFormatType == NiPixelFormat::FORMAT_DXT1)
    {
        kFormatString = "DXT1";
    }
    else if (kFormatType == NiPixelFormat::FORMAT_DXT3)
    {
        kFormatString = "DXT3";
    }
    else if (kFormatType == NiPixelFormat::FORMAT_DXT5)
    {
        kFormatString = "DXT5";
    }
    else if (kFormatType == NiPixelFormat::FORMAT_PAL ||
        kFormatType == NiPixelFormat::FORMAT_PALALPHA)
    {
        kFormatString = pcString;
        kFormatString += "Palettized";
    }
    else
    {
        kFormatString = pcString;
        for (int iIndex = 0; iIndex < 4; iIndex++)
        {
            NiPixelFormat::Component eComp = aeWhichOrder[iIndex];
            unsigned int uiMask = kFormat.GetMask(eComp); 
            char cChannel = '\0';
            if (eComp == NiPixelFormat::COMP_RED)
                cChannel = 'R';
            else if (eComp == NiPixelFormat::COMP_GREEN)
                cChannel = 'G';
            else if (eComp == NiPixelFormat::COMP_BLUE)
                cChannel = 'B';
            else if (eComp == NiPixelFormat::COMP_ALPHA)
                cChannel = 'A';

            unsigned int uiMaskSize = CalcMaskSize(uiMask);
            if (uiMaskSize > 0)
            {
                kFormatString += NiString(cChannel) + 
                    NiString::FromInt(uiMaskSize);
            }
        }
    }

    NiFree(pcString);

    return kFormatString.MakeExternalCopy();
}
//---------------------------------------------------------------------------
unsigned int NiDevImageQuantizer::CalcMaskSize(unsigned int uiMask)
{
    unsigned int uiTotal = 0;
    while (uiMask != 0)
    {
        uiTotal += (uiMask & 1);
        uiMask = uiMask >> 1;
    }
    return uiTotal;
    
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::ImageHasAlpha(const NiPixelData* pkPixelData)
{
    if (!pkPixelData)
    {
        return false;
    }

    if (pkPixelData->GetPixelFormat().GetCompressed())
    {
        return CompressedImageHasAlpha(pkPixelData);
    }
    else if (pkPixelData->GetPixelFormat().GetPalettized() &&
        pkPixelData->GetPixelFormat().GetPaletteHasAlpha())
    {
        return PALImageHasAlpha(pkPixelData);
    }
    else
    {
        return UncompressedImageHasAlpha(pkPixelData);
    }
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::CompressedImageHasAlpha(
    const NiPixelData* pkPixelData)
{
    unsigned int uiBlockSize;
    bool (*pfBlockHasAlpha)(const unsigned char*);

    // Select appropriate block size and decoding method based
    // on the DXT algorithm
    switch(pkPixelData->GetPixelFormat().GetFormat())
    {
    case NiPixelFormat::FORMAT_DXT1:
        uiBlockSize = 8; //bytes
        pfBlockHasAlpha = DXT1BlockHasAlpha;
    break;

    case NiPixelFormat::FORMAT_DXT3:
        uiBlockSize = 16; //bytes
        pfBlockHasAlpha = DXT3BlockHasAlpha;
    break;

    case NiPixelFormat::FORMAT_DXT5:
        uiBlockSize = 16; //bytes
        pfBlockHasAlpha = DXT5BlockHasAlpha;
    break;

    default:
        NIASSERT(!"Unknown compressed image format.");
        // Return true as a fallback since the DXT formats *can* 
        // support alpha.
        return true;
    }

    unsigned int uiWidth = pkPixelData->GetWidth();
    unsigned int uiHeight = pkPixelData->GetHeight();    

    unsigned int uiCalculatedImageSize = static_cast<unsigned int>(
        (ceil(uiWidth/4.0f) * ceil(uiHeight/4.0f) * uiBlockSize));

    // Examine each texel to determine if any have non-opaque alpha values
    for (unsigned int uiFace = 0; uiFace < pkPixelData->GetNumFaces();
        ++uiFace)
    {
        unsigned int uiActualImageSize = 
            (unsigned int)pkPixelData->GetSizeInBytes(0, uiFace);
        
        // Does DXT image size does not match what we expect?
        NIASSERT(uiCalculatedImageSize == uiActualImageSize);
        if (uiCalculatedImageSize != uiActualImageSize)
        {
            // Since this image doesn't match what we expect, it isn't safe
            // to try and decode the format (we may overrun memory).
            // Return true as a fallback since the DXT formats *can*
            // support alpha.
            return true;
        }

        for(unsigned int uiY = 0; uiY < pkPixelData->GetHeight(); uiY += 4)
        {
            for(unsigned int uiX = 0; uiX < pkPixelData->GetWidth(); uiX += 4)
            {
                unsigned int uiOffset = static_cast<unsigned int>(
                    (uiBlockSize * (ceil(uiWidth/4.0f) * floor(uiY/4.0f)
                    + floor(uiX/4.0f))));
                const unsigned char* pucBlock = 
                    pkPixelData->GetPixels(0, uiFace) + uiOffset;

                if (pfBlockHasAlpha(pucBlock))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::DXT1BlockHasAlpha(const unsigned char* pucBlock)
{
    unsigned short usColor0 = *(unsigned short*)(pucBlock);
    unsigned short usColor1 = *(unsigned short*)(pucBlock+2);
    if (usColor0 <= usColor1)
    {
        unsigned long uiIndices = *(unsigned long*)(pucBlock+4);
        for( int iShift=0; iShift<32; iShift += 2)
        {
            if (((uiIndices>>iShift) & 0x03) == 0x03)
            {
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::DXT3BlockHasAlpha(const unsigned char* pucBlock)
{
    unsigned long usAlphaComponents0 = *(unsigned long*)(pucBlock);
    unsigned long usAlphaComponents1 = *(unsigned long*)(pucBlock+4);
    if (usAlphaComponents0 != 0xffffffff || usAlphaComponents1 != 0xffffffff)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::DXT5BlockHasAlpha(const unsigned char* pucBlock)
{
    unsigned char ucOpaqueIndex;
    unsigned char usAlpha0 = *(unsigned char*)(pucBlock);
    unsigned char usAlpha1 = *(unsigned char*)(pucBlock+1);

    if (usAlpha0 == 0xFF)
    {
        ucOpaqueIndex = 0x0;
    }
    else if (usAlpha0 <= usAlpha1)
    {
        ucOpaqueIndex = 0x7;
    }
    else
    {
        // All indices have transparency associated with them
        // thus some texels in this block must have alpha
        return true;
    }

    unsigned long uiIndicesLo = *(unsigned long*)(pucBlock+4);
    for( int iShift=0; iShift<30; iShift += 3)
    {
        if ((uiIndicesLo>>iShift & 0x07) != ucOpaqueIndex)
        {
            return true;
        }

    }

    unsigned long uiIndicesHi = *(unsigned short*)(pucBlock+2);
    uiIndicesHi <<= 2;
    uiIndicesHi |= ((uiIndicesLo & 0xc0000000)>>30);

    for( int iShift=0; iShift<18; iShift += 3)
    {
        if ((uiIndicesHi>>iShift & 0x07) != ucOpaqueIndex)
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::PALImageHasAlpha(const NiPixelData* pkPixelData)
{
    const NiPalette::PaletteEntry* pkPaletteEntry = 
        pkPixelData->GetPalette()->GetEntries();

    unsigned int uiNumPaletteEntries = 
        pkPixelData->GetPalette()->GetNumEntries();
    NI_UNUSED_ARG(uiNumPaletteEntries);

    if (pkPixelData->GetPixelFormat().GetBitsPerPixel() != 8)
    {
        NIASSERT(!"Can only delect alpha texels in PALA8 "
            "textures");
        // Return true as a fallback since the other PAL formats *can*
        // support alpha.
        return true;
    }

    // Examine each texel to determine if any have non-opaque alpha values
    for (unsigned int uiFace = 0; uiFace < pkPixelData->GetNumFaces();
        ++uiFace)
    {
        for(unsigned int uiY = 0; uiY < pkPixelData->GetHeight(); ++uiY)
        {
            for(unsigned int uiX = 0; uiX < pkPixelData->GetWidth(); ++uiX)
            {
                unsigned char ucIndex = 
                    *reinterpret_cast<const unsigned char*>(
                    ((*pkPixelData)(uiX, uiY, 0, uiFace)));

                // Look up texel color in the palette
                NIASSERT(ucIndex < uiNumPaletteEntries);
                if (pkPaletteEntry[ucIndex].m_ucAlpha < 255)
                {
                    return true;
                }
            }  
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::UncompressedImageHasAlpha(
    const NiPixelData* pkPixelData)
{
    // This this image have an alpha component?
    unsigned int uiAlphaMask = 
        pkPixelData->GetPixelFormat().GetMask(NiPixelFormat::COMP_ALPHA);
    if (!uiAlphaMask)
    {
        return false;
    }

    unsigned int uiAlphaShift = 
        pkPixelData->GetPixelFormat().GetShift(NiPixelFormat::COMP_ALPHA);

    unsigned int uiAlphaBits = 
        pkPixelData->GetPixelFormat().GetBits(NiPixelFormat::COMP_ALPHA);

    // Calculate the max value that can be stored in the Alpha component
    // This represents opaque. 2^uiAlphaBits - 1
    unsigned int uiOpaqueValue = (1<<uiAlphaBits)-1;

    // Examine each texel to determine if any have non-opaque alpha values
    for (unsigned int uiFace = 0; uiFace < pkPixelData->GetNumFaces();
        ++uiFace)
    {
        for(unsigned int uiY = 0; uiY < pkPixelData->GetHeight(); ++uiY)
        {
            for(unsigned int uiX = 0; uiX < pkPixelData->GetWidth(); ++uiX)
            {
                unsigned int uiColor;
                switch(pkPixelData->GetPixelFormat().GetBitsPerPixel())
                {
                case 16:
                    uiColor = *reinterpret_cast<const unsigned short*>(
                        ((*pkPixelData)(uiX, uiY, 0, uiFace)));
                break;

                case 32:
                    uiColor = *reinterpret_cast<const unsigned long*>(
                        ((*pkPixelData)(uiX, uiY, 0, uiFace)));
                    break;

                default:
                    NIASSERT(!"Can only detect alpha in textures with 16  "
                        "or 32 bits per pixel");
                    // Return true as a fallback since the we do have
                    // an alpha component which *could* contain non-
                    // opaque values.
                    return true;
                }

                unsigned int uiAlpha = (uiColor & uiAlphaMask) 
                    >> uiAlphaShift;
                if (uiAlpha != uiOpaqueValue)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageQuantizer::ConvertIfNecessaryForAlphaSwitch(
    NiPixelFormat& eFormat)
{
    NiPixelFormat eOrigFormat = eFormat;
    if (eFormat == NiPixelFormat::RGBA16)
        eFormat = NiPixelFormat::RGB24;
    if (eFormat == NiPixelFormat::RGBA32)
        eFormat = NiPixelFormat::RGB24;
    if (eFormat == NiPixelFormat::PALA8)
        eFormat = NiPixelFormat::RGB24;
    if (eFormat == NiPixelFormat::PALA4)
        eFormat = NiPixelFormat::RGB24;
    if (eFormat.GetCompressed())
        eFormat = NiPixelFormat::RGB24;
    if (eFormat == eOrigFormat)
        return false;
    else
        return true;
}
//---------------------------------------------------------------------------
