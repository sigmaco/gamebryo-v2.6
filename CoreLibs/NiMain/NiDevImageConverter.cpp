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
#include "NiMainPCH.h"

#include "NiBMPReader.h"
#include "NiDDSReader.h"
#include "NiDevImageConverter.h"
#include "NiMath.h"
#include "NiNIFImageReader.h"
#include "NiPalette.h"
#include "NiPixelData.h"
#include "NiPixelFormat.h"
#include "NiSGIReader.h"
#include "NiTGAReader.h"
#include <NiFile.h>
#include <NiFilename.h>
#include <NiSystem.h>

NiImplementRTTI(NiDevImageConverter, NiImageConverter);

//---------------------------------------------------------------------------
NiDevImageConverter::NiDevImageConverter()
{
    m_kReaders.AddHead(NiNew NiBMPReader);
    m_kReaders.AddHead(NiNew NiDDSReader);
    m_kReaders.AddHead(NiNew NiSGIReader);
    m_kReaders.AddHead(NiNew NiTGAReader);
    m_kReaders.AddHead(NiNew NiNIFImageReader);
}
//---------------------------------------------------------------------------
NiDevImageConverter::~NiDevImageConverter()
{
    while (m_kReaders.GetSize())
        NiDelete m_kReaders.RemoveHead();
}
//---------------------------------------------------------------------------
void NiDevImageConverter::AddReader(NiImageReader *pkReader)
{
    m_kReaders.AddHead(pkReader);
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::IsLittleEndian(Platform ePlatform)
{
    switch (ePlatform)
    {
    case WII:
    case XENON:
    case PLAYSTATION3:
        return false;
    case ANY:
    case PLAYSTATION2:
    case XBOX:
    case D3D10:
    case DX9:
        return true;
    default:
        NIASSERT(!"NiDevImageConverter - Unknown platform");
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::CanReadImageFile(const char* pcFilename) const
{
    NiTListIterator pkIter = m_kReaders.GetHeadPos();

    char acStandardFilename[NI_MAX_PATH];
    NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilename);
    NiPath::Standardize(acStandardFilename);
    NiFilename kFileName(acStandardFilename);
    const char* pcExt = kFileName.GetExt();

    while (pkIter)
    {
        NiImageReader* pkReader = m_kReaders.GetNext(pkIter);

        if (pkReader->CanReadFile(pcExt))
        {
            NiFile* pkIstr = NiFile::GetFile(acStandardFilename, 
                NiFile::READ_ONLY);
            if ((!pkIstr) || (!(*pkIstr)))
            {
                NiDelete pkIstr;
                return false;
            }

            NiDelete pkIstr;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::ReadImageFileInfo(const char* pcFilename,
    NiPixelFormat& kFmt, bool& bMipmap, unsigned int& uiWidth,
    unsigned int& uiHeight, unsigned int& uiNumFaces)
{
    NiTListIterator pkIter = m_kReaders.GetHeadPos();

    char acStandardFilename[NI_MAX_PATH];
    NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilename);
    NiPath::Standardize(acStandardFilename);
    NiFilename kFileName(acStandardFilename);
    const char* pcExt = kFileName.GetExt();

    while (pkIter)
    {
        NiImageReader* pkReader = m_kReaders.GetNext(pkIter);

        if (pkReader->CanReadFile(pcExt))
        {
            NiFile* pkIstr = NiFile::GetFile(acStandardFilename, 
                NiFile::READ_ONLY);
            if ((!pkIstr) || (!(*pkIstr)))
            {
                NiDelete pkIstr;
                return false;
            }

            if (pkReader->ReadHeader(*pkIstr, uiWidth, uiHeight, kFmt,
                bMipmap, uiNumFaces))
            {
                NiDelete pkIstr;
                return true;
            }

            NiDelete pkIstr;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiPixelData* NiDevImageConverter::ReadImageFile(const char* pcFilename,
    NiPixelData* pkOptDest)
{
    NiTListIterator pkIter = m_kReaders.GetHeadPos();

    char acStandardFilename[NI_MAX_PATH];
    NiStrcpy(acStandardFilename, NI_MAX_PATH, pcFilename);
    NiPath::Standardize(acStandardFilename);
    NiFilename kFileName(acStandardFilename);
    const char* pcExt = kFileName.GetExt();

    while (pkIter)
    {
        NiImageReader* pkReader = m_kReaders.GetNext(pkIter);

        if (pkReader->CanReadFile(pcExt))
        {
            NiFile* pkIstr = NiFile::GetFile(acStandardFilename, 
                NiFile::READ_ONLY);
            if ((!pkIstr) || (!(*pkIstr)))
            {
                NiDelete pkIstr;
                return false;
            }

            NiPixelData* pkDest = pkReader->ReadFile(*pkIstr, pkOptDest);

            if (pkDest)
            {
                NiDelete pkIstr;
                return pkDest;
            }

            NiDelete pkIstr;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::CanConvertPixelData(const NiPixelFormat& kSrcFmt, 
    const NiPixelFormat& kDestFmt) const
{
    NiPixelFormat::Format eDestFmt = kDestFmt.GetFormat();
    unsigned int uiDestBPP = kDestFmt.GetBitsPerPixel();
    PixelBits kDestBits(kDestFmt);

    if (kSrcFmt == kDestFmt)
        return true;

    // cannot convert from any format into compressed format
    if (kDestFmt.GetCompressed())
        return false;

    // Explicit convert functions exist to convert to big endian Wii-specific
    // BGRA4443 or BGR565 formats, which do not require the source to be
    // endian swapped. This logic is duplicated in ConvertPixelData().
    if((kSrcFmt == NiPixelFormat::RGB24
        && kDestFmt == NiPixelFormat::BGR565_BE) ||
        
        ((kSrcFmt.GetBitsPerPixel() == 16 || kSrcFmt.GetBitsPerPixel() == 32)
        && kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA 
        && kDestFmt == NiPixelFormat::BGRA4443_BE))
    {
        return true;
    }

    // Check for 'trivial' reorganizations we can try (endian-mismatch,
    // swizzling)
    if (kSrcFmt.FunctionallyIdentical(kDestFmt,
            (unsigned int)(NiPixelFormat::CMP_DONTCARE_ENDIAN & 
            NiPixelFormat::CMP_DONTCARE_TILING)))
    {
        // cases:
        //    need swap                         => succeed
        //               swizzle                => succeed
        //               swizzle   cant swizzle => fail
        //    need swap, swizzle                => succeed
        //    need swap, swizzle   cant swizzle => fail
        //    don't need anything               => continue to more tests
        if (kSrcFmt.GetLittleEndian() != kDestFmt.GetLittleEndian())
        {
            // Only an endian swap?
            if (kSrcFmt.GetTiling() == kDestFmt.GetTiling())
                return true;
        }

        if (((kSrcFmt.GetTiling() == NiPixelFormat::TILE_NONE) &&
            (kDestFmt.GetTiling() == NiPixelFormat::TILE_NV_SWIZZLED)) ||
            (((kSrcFmt.GetTiling() == NiPixelFormat::TILE_NV_SWIZZLED) &&
            (kDestFmt.GetTiling() == NiPixelFormat::TILE_NONE))))
        {
            // a type of swap we can succeed at
            return true;
        }
        else
        {
            // varies by tiling but not in a way we can handle
            if (kSrcFmt.GetTiling() != kDestFmt.GetTiling())
                return false;
        }
    }

    // Remaining image conversion methods are only done between the same
    // endian type, except for the few supported methods
    if (kSrcFmt.GetLittleEndian() != kDestFmt.GetLittleEndian())
    {
        return false;
    }

    // When dealing with 24-bit data, only support B8G8R8 and R8G8B8
    if ((uiDestBPP == 24) && ((kDestBits.m_uiGM != 0x00ff00) || 
        ((kDestBits.m_uiRM != 0x0000ff) && (kDestBits.m_uiRM != 0xff0000))))
    {
        return false;
    }

    if ((kSrcFmt == NiPixelFormat::PAL4) ||
        (kSrcFmt == NiPixelFormat::PALA4))
    {
        // Currently, we only support convertion to 24,32 bit
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP != 24 && uiDestBPP != 32)
                return false;
        }
        else if ((kDestFmt != NiPixelFormat::PAL4) &&
            (kDestFmt != NiPixelFormat::PALA4))
        {
            return false;
        }
    }
    else if ((kSrcFmt == NiPixelFormat::PAL8) || 
        (kSrcFmt == NiPixelFormat::PALA8))
    {
        // Destination formats are PALA8, 16 bit RGB(A), 24 bit RGB
        // and 32 bit RGB(A)
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if ((uiDestBPP != 16) && (uiDestBPP != 24) && (uiDestBPP != 32))
                return false;
        }
        else if ((kDestFmt != NiPixelFormat::PAL8) && 
            (kDestFmt != NiPixelFormat::PALA8)) 
        {
            // We cannot convert to any formats other than PAL8 or PALA8
            return false;
        }
    }
    else if ((kSrcFmt == NiPixelFormat::RGB24) || 
        (kSrcFmt == NiPixelFormat::RGBA32) || kSrcFmt.GetCompressed())
    {
        // Note the term above allowing compressed formats - if a compressed
        // format it given, then it can be pre-converted into RGBA32
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA) ||
            (eDestFmt == NiPixelFormat::FORMAT_BUMP) || 
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA) ||
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA_WII))
        {
            if ((uiDestBPP != 16) && (uiDestBPP != 24) && (uiDestBPP != 32))
                return false;
        }
        else
        {
            return false;
        }
    }
    else if ((kSrcFmt == NiPixelFormat::BUMP16) || 
        (kSrcFmt == NiPixelFormat::BUMPLUMA32) ||
        (kSrcFmt == NiPixelFormat::BUMPLUMA32_WII))
    {
        // Note the term above allowing compressed formats - if a compressed
        // format it given, then it can be pre-converted into RGBA32
        if ((eDestFmt == NiPixelFormat::FORMAT_BUMP) || 
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA) ||
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA_WII))
        {
            if ((uiDestBPP != 16) && (uiDestBPP != 24) && (uiDestBPP != 32))
                return false;
        }
        else
        {
            return false;
        }
    }
    else if ((kSrcFmt == NiPixelFormat::RGBA16 ||
        (kSrcFmt == NiPixelFormat::BGRA16)))
    {
        if (uiDestBPP != 16)
            return false;
        
        // We only support RGBA5551 or BGRA5551
        // in any case, alpha and green should have the same mask.
        if (kDestBits.m_uiAM != 0x00008000 ||
            kDestBits.m_uiGM != 0x000003e0)
            return false;

        if (((kDestBits.m_uiRM != 0x0000001f) ||
             (kDestBits.m_uiBM != 0x00007c00)) &&
            ((kDestBits.m_uiRM != 0x00007c00) ||
             (kDestBits.m_uiBM != 0x0000001f)))
             return false;
    }
    else if (((kDestFmt == NiPixelFormat::BGRA8888) ||
         (kDestFmt == NiPixelFormat::ARGB8888) ||
         (kDestFmt == NiPixelFormat::RGBA32)) &&
        ((kSrcFmt == NiPixelFormat::BGRA8888) ||
         (kSrcFmt == NiPixelFormat::ARGB8888) ||
         (kSrcFmt == NiPixelFormat::RGBA32)))
    {
        return true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiPixelData* NiDevImageConverter::ConvertPixelData(const NiPixelData& kSrc,
    const NiPixelFormat& kDestFmt, NiPixelData* pkOptDest, bool bMipmap)
{
    NiPixelDataPtr spCompressionTmp;
    NiPixelDataPtr spEndianTmp;

    const NiPixelData* pkSrcTmp = &kSrc;
    const NiPixelFormat& kSrcFmt = kSrc.GetPixelFormat();
    
    // Check for endian-mismatch conversions we can try
    bool bCanEndianSwapSuccessfully =
        kSrcFmt.FunctionallyIdentical(kDestFmt,
            (unsigned int)(NiPixelFormat::CMP_DONTCARE_ENDIAN & 
            NiPixelFormat::CMP_DONTCARE_TILING));
    bool bNeedToEndianSwapSource = 
        (kSrcFmt.GetLittleEndian() != kDestFmt.GetLittleEndian());

    // Explicit convert functions exist to convert to big endian Wii-specific
    // BGRA4443 or BGR565 formats, which do not require the source to be
    // endian swapped. This logic is duplicated in CanConvertPixelData().
    if ((kSrcFmt == NiPixelFormat::RGB24 
            && kDestFmt == NiPixelFormat::BGR565_BE) ||
        ((kSrcFmt.GetBitsPerPixel() == 16 || kSrcFmt.GetBitsPerPixel() == 32)
            && kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA 
            && kDestFmt == NiPixelFormat::BGRA4443_BE))
    {
        bCanEndianSwapSuccessfully = true;
        bNeedToEndianSwapSource = kSrcFmt.ComponentCrossesByteBoundary();
    }

    if (bNeedToEndianSwapSource)
    {
        if (bCanEndianSwapSuccessfully)
        {
            if ((pkOptDest != NULL) && (&kSrc == pkOptDest))
            {
                // Swap in place (pkOptDest == pkSrc)
                if (!pkOptDest->ConvertToOppositeEndian())
                    return NULL;

                // No need to update pkSrcTmp, it points to the same place
            }
            else
            {
                // DO NOT return spEndianTemp, as it will be deleted on exit
                spEndianTmp = NiNew NiPixelData(kSrc);
                if (!spEndianTmp->ConvertToOppositeEndian())
                    return NULL;
     
                pkSrcTmp = spEndianTmp;
            }
        }
        else
        {
            NiOutputDebugString("NiDevImageConverter::Error>> "
                "Cannot convert between little and big endian pixel formats "
                "if they are not otherwise identical\n");
            return NULL;
        }
    }

    // If the source format is compressed, and the destination is not, then
    // we have to pre-convert to RGBA32.  This is another expansion step,
    // but will only be needed if the application compressed their source
    // textures and are running on a renderer/device that cannot use
    // compressed textures.
    if (kSrc.GetPixelFormat().GetCompressed() &&
        (kDestFmt.GetFormat() != kSrc.GetPixelFormat().GetFormat()))
    {
        // DO NOT return spCompressionTemp, as it will be deleted on exit
        spCompressionTmp = m_kCodec.ExpandCompressedData(kSrc);
        pkSrcTmp = spCompressionTmp;
    }

    NiPixelDataPtr spMipmap;

    const NiPixelData* pkPostMipmap = NULL;

    // Mipmap if needed - we cannot mipmap if the destination format is
    // a non-RGB(A) format other than a bump map, as the outcome of 
    // GenerateMipmapLevels will always be RGB(A), and we cannot convert 
    // from RGB(A) to a non-RGB(A) format other than a bump format.
    // Also, we do not generate mipmaps on-the-fly if either texture dimension
    // is non-power-of-two.
    if (bMipmap && (pkSrcTmp->GetNumMipmapLevels() <= 1) &&
        NiIsPowerOf2(pkSrcTmp->GetWidth()) &&
        NiIsPowerOf2(pkSrcTmp->GetHeight()) &&
        ((kDestFmt.GetFormat() == NiPixelFormat::FORMAT_RGB) ||
        (kDestFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA) ||
        (kDestFmt.GetFormat() == NiPixelFormat::FORMAT_BUMP) ||
        (kDestFmt.GetFormat() == NiPixelFormat::FORMAT_BUMPLUMA) ||
        (kDestFmt.GetFormat() == NiPixelFormat::FORMAT_BUMPLUMA_WII)))
    {
        // If the optional destination is flat, then don't pass it to the
        // mipmap operation.
        spMipmap = NiDevImageConverter::GenerateMipmapLevels(pkSrcTmp, 
            pkOptDest);

        if (spMipmap)
            pkPostMipmap = spMipmap;
        else
            pkPostMipmap = pkSrcTmp; 
    }
    else
    {
        pkPostMipmap = pkSrcTmp;
    }

    // If there is an optional destination, the use it unless:
    // 1) The optional destination's format does not match the specified 
    //    destination format
    // OR
    // 2) Mipmapping is required and the optional destination is flat
    if (pkOptDest && (pkOptDest->GetPixelFormat() == kDestFmt) 
        && (!bMipmap || (pkOptDest->GetNumMipmapLevels() > 1)))
    {
        // Source equals post-mipmap equals optional destination - just
        // return the pointer to the optional dest
        if (pkOptDest == pkPostMipmap)
        {
            return pkOptDest;
        }
        else
        {
            // This will fail if the destination does not match in any way
            if (ConvertPixelDataFormat(*pkOptDest, *pkPostMipmap, -1))
                return pkOptDest;
        }
    }
    
    NiPixelDataPtr spBumpmap;

    const NiPixelData* pkPostBumpmap = pkPostMipmap;

    // if the source data is RGB24 and the dest is BUMP or BUMP32, then we
    // convert the RGB24 to BUMP16 using forward differencing.  Alternatively,
    // if the source is RGBA32 and the dest is BUMP or BUMP32, then we
    // convert the RGBA32 to BUMPLUMA32 using forward differencing.  
    if ((kDestFmt.GetFormat() == NiPixelFormat::FORMAT_BUMP) || 
        (kDestFmt.GetFormat() == NiPixelFormat::FORMAT_BUMPLUMA) ||
        (kDestFmt.GetFormat() == NiPixelFormat::FORMAT_BUMPLUMA_WII))
    {
        spBumpmap = GenerateBumpMap(*pkPostMipmap, kDestFmt);

        if (spBumpmap)
            pkPostBumpmap = spBumpmap;
    }

    // allocate the required destination data
    NiPixelData* pkDest = NiNew NiPixelData(pkPostBumpmap->GetWidth(), 
        pkPostBumpmap->GetHeight(), kDestFmt, 
        pkPostBumpmap->GetNumMipmapLevels(), 
        pkPostBumpmap->GetNumFaces());

    if (ConvertPixelDataFormat(*pkDest, *pkPostBumpmap, -1))
    {
        return pkDest;
    }
    else
    {
        NiDelete pkDest;
        return NULL;
    }
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::ConvertPixelDataFormat(NiPixelData& kDest, 
    const NiPixelData& kSrc, int iMipmapLevel)
{
    const NiPixelFormat& kSrcFmt = kSrc.GetPixelFormat();
    const NiPixelFormat& kDestFmt = kDest.GetPixelFormat();

    // For now, we can only convert into RGB formats (not PAL or compressed
    // unless there is no conversion

    if ((kSrc.GetWidth() != kDest.GetWidth()) || 
        (kSrc.GetHeight() != kDest.GetHeight()))
        return false;

    unsigned int uiMaxFaces = kSrc.GetNumFaces();
    if (kSrc.GetNumFaces() != kDest.GetNumFaces())
        return false;

    // if iMipmapLevel is -1, then all mimpmap levels are copied
    // The source must have at least as many mipmap levels as the dest
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
        // dest
        if ((unsigned int)iMipmapLevel >= kSrc.GetNumMipmapLevels())
            return false;

        if ((unsigned int)iMipmapLevel >= kDest.GetNumMipmapLevels())
            return false;

        uiMinLevel = uiMaxLevel = (unsigned int)iMipmapLevel;
    }

    // can copy any format into the same format, even if we can't do
    // conversions involving such formats
    if (kSrcFmt == kDestFmt)
    {
        for (unsigned int uiFace = 0; uiFace < uiMaxFaces; uiFace++)
        {
            for (unsigned int uiMip = uiMinLevel; uiMip <= uiMaxLevel; uiMip++)
            {
                NiMemcpy(kDest.GetPixels(uiMip, uiFace),
                    kDest.GetSizeInBytes(uiMip, uiFace),
                    kSrc.GetPixels(uiMip, uiFace),
                    kSrc.GetSizeInBytes(uiMip, uiFace));
            }
        }

        if (kSrc.GetPalette())
        {
            // Copy the palette
            if (!kDest.GetPalette())
                kDest.SetPalette(NiNew NiPalette(*kSrc.GetPalette()));
            else
                *kDest.GetPalette() = *kSrc.GetPalette();
        }

        return true;
    }

    NiPixelFormat::Format eDestFmt = kDestFmt.GetFormat();
    NiPixelFormat::Format eSrcFmt = kSrcFmt.GetFormat();
    unsigned int uiDestBPP = kDestFmt.GetBitsPerPixel();
    unsigned int uiSrcBPP = kSrcFmt.GetBitsPerPixel();
    PixelBits kDestBits(kDestFmt);
    PixelBits kSrcBits(kSrcFmt);

    // cannot convert from any format into compressed format
    if ((eDestFmt == NiPixelFormat::FORMAT_DXT1) ||
        (eDestFmt == NiPixelFormat::FORMAT_DXT3) ||
        (eDestFmt == NiPixelFormat::FORMAT_DXT5))
    {
        return false;
    }

    // When dealing with 24-bit data, only support B8G8R8 and R8G8B8
    if ((uiDestBPP == 24) && ((kDestBits.m_uiGM != 0x00ff00) || 
        ((kDestBits.m_uiRM != 0x0000ff) && (kDestBits.m_uiRM != 0xff0000))))
    {
        return false;
    }

    ConverterFunc pfnConverter = NULL;

    if (kSrcFmt == NiPixelFormat::PAL4)
    {
        if (!kSrc.GetPalette())
            return false;

        // Destination formats are PALA4, 
        // and 32 bit RGB(A)
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP == 24)
                pfnConverter = &NiDevImageConverter::ConvertPAL4To24;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertPAL4To32;
            else
                return false;
        }
        else if (kDestFmt == NiPixelFormat::PALA4) 
        {
            // Copy the image data and the palette, putting 255's in the
            // alpha components of the palette entries
            for (unsigned int uiFace = 0; uiFace < uiMaxFaces; uiFace++)
            {
                for (unsigned int uiMip = uiMinLevel; uiMip <= uiMaxLevel;
                    uiMip++)
                {
                    NiMemcpy(kDest.GetPixels(uiMip, uiFace), 
                        kDest.GetSizeInBytes(uiMip, uiFace),
                        kSrc.GetPixels(uiMip, uiFace), 
                        kSrc.GetSizeInBytes(uiMip, uiFace));
                }
            }

            if (kSrc.GetPalette())
            {
                if (!kDest.GetPalette())
                    kDest.SetPalette(NiNew NiPalette(*kSrc.GetPalette()));
                else
                    *kDest.GetPalette() = *kSrc.GetPalette();

                NiPalette::PaletteEntry* pkEntries 
                    = kDest.GetPalette()->GetEntries();

                unsigned int uiEntries = kDest.GetPalette()->GetNumEntries();

                for (unsigned int i = 0; i < uiEntries; i++)
                {
                    pkEntries->m_ucAlpha = 255;
                }

                return true;
            }
            else 
            {
                return false;
            }
        }
        else
        {
            // The equal formats case above handled PAL4 to PAL4, and the
            // case just above this handled PAL4 to PALA4
            // So, this must be a non-4-bit palette format.  We can't
            // convert that
            return false;
        }
    }
    else if (kSrcFmt == NiPixelFormat::PALA4)
    {
        if (!kSrc.GetPalette())
            return false;

        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP == 24)
                pfnConverter = &NiDevImageConverter::ConvertPALA4To24;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertPALA4To32;
            else
                return false;
        }
        else if (kDestFmt == NiPixelFormat::PAL4) 
        {
            // Copy the image data and the palette, creating the palette with
            // no alpha
            for (unsigned int uiFace = 0; uiFace < uiMaxFaces; uiFace++)
            {
                for (unsigned int uiMip = uiMinLevel; uiMip <= uiMaxLevel;
                    uiMip++)
                {
                    NiMemcpy(kDest.GetPixels(uiMip, uiFace),
                        kDest.GetSizeInBytes(uiMip, uiFace),
                        kSrc.GetPixels(uiMip, uiFace), 
                        kSrc.GetSizeInBytes(uiMip, uiFace));
                }
            }

            if (kSrc.GetPalette())
            {
                kDest.SetPalette(NiNew NiPalette(false, 16,
                    kSrc.GetPalette()->GetEntries()));

                return true;
            }
            else
            {
                return false;
            }

        }
        else
        {
            // The equal formats case above handled PALA4 to PALA4, and the
            // case just above this handled PALA4 to PAL4
            // So, this must be a non-4-bit palette format.  We can't
            // convert that
            return false;
        }
    }
    else if (kSrcFmt == NiPixelFormat::PAL8)
    {
        if (!kSrc.GetPalette())
            return false;

        // Destination formats are PALA8, 16 bit RGB(A), 24 bit RGB
        // and 32 bit RGB(A)
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP == 16)
                pfnConverter = &NiDevImageConverter::ConvertPAL8To16;
            else if (uiDestBPP == 24)
                pfnConverter = &NiDevImageConverter::ConvertPAL8To24;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertPAL8To32;
            else
                return false;
        }
        else if (kDestFmt == NiPixelFormat::PALA8) 
        {
            // Copy the image data and the palette, putting 255's in the
            // alpha components of the palette entries
            for (unsigned int uiFace = 0; uiFace < uiMaxFaces; uiFace++)
            {
                for (unsigned int uiMip = uiMinLevel; uiMip <= uiMaxLevel;
                    uiMip++)
                {
                    NiMemcpy(kDest.GetPixels(uiMip, uiFace),
                        kDest.GetSizeInBytes(uiMip, uiFace),
                        kSrc.GetPixels(uiMip, uiFace), 
                        kSrc.GetSizeInBytes(uiMip, uiFace));
                }
            }

            if (kSrc.GetPalette())
            {
                if (!kDest.GetPalette())
                    kDest.SetPalette(NiNew NiPalette(*kSrc.GetPalette()));
                else
                    *kDest.GetPalette() = *kSrc.GetPalette();

                NiPalette::PaletteEntry* pkEntries 
                    = kDest.GetPalette()->GetEntries();

                unsigned int uiEntries = kDest.GetPalette()->GetNumEntries();

                for (unsigned int i = 0; i < uiEntries; i++)
                {
                    pkEntries->m_ucAlpha = 255;
                }

                return true;
            }
            else
            {
                return false;
            }

        }
        else
        {
            // The equal formats case above handled PAL8 to PAL8, and the
            // case just above this handled PAL8 to PALA8
            // So, this must be a non-8-bit palette format.  We can't
            // convert that
            return false;
        }
    }
    else if (kSrcFmt == NiPixelFormat::PALA8)
    {
        if (!kSrc.GetPalette())
            return false;

        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP == 16)
                pfnConverter = &NiDevImageConverter::ConvertPALA8To16;
            else if (uiDestBPP == 24)
                pfnConverter = &NiDevImageConverter::ConvertPALA8To24;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertPALA8To32;
            else
                return false;
        }
        else if (kDestFmt == NiPixelFormat::PAL8) 
        {
            // Copy the image data and the palette, creating the palette with
            // no alpha
            for (unsigned int uiFace = 0; uiFace < uiMaxFaces; uiFace++)
            {
                for (unsigned int uiMip = uiMinLevel; uiMip <= uiMaxLevel;
                    uiMip++)
                {
                    NiMemcpy(kDest.GetPixels(uiMip, uiFace),
                        kDest.GetSizeInBytes(uiMip, uiFace),
                        kSrc.GetPixels(uiMip, uiFace), 
                        kSrc.GetSizeInBytes(uiMip, uiFace));
                }
            }

            if (kSrc.GetPalette())
            {
                kDest.SetPalette(NiNew NiPalette(false, 256,
                    kSrc.GetPalette()->GetEntries()));

                return true;
            }
            else 
            {
                return false;
            }

        }
        else
        {
            // The equal formats case above handled PALA8 to PALA8, and the
            // case just above this handled PALA8 to PAL8
            // So, this must be a non-8-bit palette format.  We can't
            // convert that
            return false;
        }
    }
    else if (kSrcFmt == NiPixelFormat::RGB24)
    {
        // Destination formats are 16 bit RGB(A), 24 bit RGB(A) 
        // and 32 bit RGB(A)    
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP == 16)
            {
                // Special case conversion for Wii from little endian
                // RGB23 to big endian BGR565. NiDevImageConverter does
                // not normally allow cross endian conversion.
                pfnConverter = (kDestFmt == NiPixelFormat::BGR565_BE  ?
                    &NiDevImageConverter::ConvertRGB24ToBGR565_BE :
                    &NiDevImageConverter::ConvertRGB24To16);
            }
            else if (uiDestBPP == 24)
                pfnConverter = &NiDevImageConverter::ConvertRGB24To24;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertRGB24To32;
            else
                return false;
        }
        else
        {
            return false;
        }
    }
    else if (((kDestFmt == NiPixelFormat::BGRA8888) ||
         (kDestFmt == NiPixelFormat::ARGB8888) ||
         (kDestFmt == NiPixelFormat::RGBA32)) &&
        ((kSrcFmt == NiPixelFormat::BGRA8888) ||
         (kSrcFmt == NiPixelFormat::ARGB8888) ||
         (kSrcFmt == NiPixelFormat::RGBA32)))
    {
        pfnConverter = &NiDevImageConverter::ConvertRGBA32To32;
    }
    // Use this BGRA4443-aware function to convert since the Wii
    // requires the MSB of the BGRA4443 (alpha channel) to be 0
    else if (kSrcFmt.GetBitsPerPixel() == 32
        && kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA 
        && kDestFmt == NiPixelFormat::BGRA4443_BE)
    {
        pfnConverter 
            = &NiDevImageConverter::ConvertRGBA32ToBGRA4443;
    }
    else if (kSrcFmt == NiPixelFormat::RGBA32)
    {
        if ((eDestFmt == NiPixelFormat::FORMAT_RGB) || 
            (eDestFmt == NiPixelFormat::FORMAT_RGBA))
        {
            if (uiDestBPP == 16)
                pfnConverter = &NiDevImageConverter::ConvertRGBA32To16;
            else if (uiDestBPP == 24)
                pfnConverter = &NiDevImageConverter::ConvertRGBA32To24; 
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertRGBA32To32; 
            else
                return false;
        }
        else
        {
            return false;
        }
    }
    else if (kSrcFmt == NiPixelFormat::BUMP16)
    {
        if ((eDestFmt == NiPixelFormat::FORMAT_BUMP) || 
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA) ||
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA_WII))
        {
            if (uiDestBPP == 16)
                pfnConverter = &NiDevImageConverter::ConvertBUMP16To16;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertBUMP16To32; 
            else
                return false;
        }
        else
        {
            return false;
        }
    }
    else if ((kSrcFmt == NiPixelFormat::BUMPLUMA32) ||
        (kSrcFmt == NiPixelFormat::BUMPLUMA32_WII))
    {
        if ((eDestFmt == NiPixelFormat::FORMAT_BUMP) || 
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA) ||
            (eDestFmt == NiPixelFormat::FORMAT_BUMPLUMA_WII))
        {
            if (uiDestBPP == 16)
                pfnConverter = &NiDevImageConverter::ConvertBUMPLUMA32To16;
            else if (uiDestBPP == 32)
                pfnConverter = &NiDevImageConverter::ConvertBUMPLUMA32To32; 
            else
                return false;
        }
        else
        {
            return false;
        }
    }
    else if (kSrcFmt == NiPixelFormat::RGBA16 &&
        kDestFmt == NiPixelFormat::BGRA16)
    {
        pfnConverter 
            = &NiDevImageConverter::ConvertRGBA16ToBGRA16;
    }
    // Use this BGRA4443-aware function to convert since the Wii
    // requires the MSB of the BGRA4443 (alpha channel) to be 0
    else if (kSrcFmt.GetBitsPerPixel() == 16
        && kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA 
        && kDestFmt == NiPixelFormat::BGRA4443_BE)
    {
        pfnConverter 
            = &NiDevImageConverter::ConvertRGBA16ToBGRA4443;
    }
    else if (kSrcFmt == NiPixelFormat::BGRA16 &&
        kDestFmt == NiPixelFormat::RGBA16)
    {
        pfnConverter 
            = &NiDevImageConverter::ConvertBGRA16ToRGBA16;

    }
    else
    {

        // Was not a conversion from standard format to odd format
        // Perhaps it is a conversion from an odd format to a standard format
        // We only support conversions to RGB24 and RGBA32
        // Also, only 16, 24 and 32-bit sources are supported
        if ((eSrcFmt != NiPixelFormat::FORMAT_RGB) && 
            (eSrcFmt != NiPixelFormat::FORMAT_RGBA))
        {
            return false;
        }

        if (kDestFmt == NiPixelFormat::RGBA32)
        {
            if (uiSrcBPP == 16)
            {
                if (kSrcBits.m_uiAM)
                {
                    pfnConverter 
                        = &NiDevImageConverter::Convert16AlphaToRGBA32;
                }
                else
                {
                    pfnConverter 
                        = &NiDevImageConverter::Convert16NoAlphaToRGBA32;
                }
            }
            else if (uiSrcBPP == 24)
            {
                pfnConverter = &NiDevImageConverter::Convert24ToRGBA32;
            }
            else if (uiSrcBPP == 32)
            {
                if (kSrcBits.m_uiAM)
                {
                    pfnConverter 
                        = &NiDevImageConverter::Convert32AlphaToRGBA32;
                }
                else
                {
                    pfnConverter 
                        = &NiDevImageConverter::Convert32NoAlphaToRGBA32;
                }
            }
            else
                return false;
        }
        else if (kDestFmt == NiPixelFormat::RGB24)
        {
            if (uiSrcBPP == 16)
                pfnConverter = &NiDevImageConverter::Convert16ToRGB24;
            else if (uiSrcBPP == 24)
                pfnConverter = &NiDevImageConverter::Convert24ToRGB24;
            else if (uiSrcBPP == 32)
                pfnConverter = &NiDevImageConverter::Convert32ToRGB24;
            else
                return false;
        }
        // Any RGB16 non-alpha format to any other RGB16 non-alpha format
        else if ((uiSrcBPP == 16 && !kSrcBits.m_uiAM) &&
            (uiDestBPP == 16 && !kDestBits.m_uiAM))
        {
            pfnConverter = &NiDevImageConverter::ConvertRGB16To16;
        }
        else
        {
            return false;
        }
    }
    
    for (unsigned int uiFace = 0; uiFace < uiMaxFaces; uiFace++)
    {
        for (unsigned int uiMip = uiMinLevel; uiMip <= uiMaxLevel; uiMip++)
        {
            (*pfnConverter)(kDest.GetWidth(uiMip, uiFace), 
                kDest.GetHeight(uiMip, uiFace),
                (unsigned int)(kDest.GetSizeInBytes(uiMip, uiFace)),
                kDest.GetPixels(uiMip, uiFace), kSrc.GetPalette(), 
                kDestBits, kSrc.GetPixels(uiMip, uiFace), kSrcBits);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::IsASupportedMipmapGenerationFormat(
    const NiPixelFormat& kSrcFmt) const
{
    if (IsAnAlphaSupportedMipmapGenerationFormat(kSrcFmt) ||
        IsANonAlphaSupportedMipmapGenerationFormat(kSrcFmt))
        return true;
    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::IsAnAlphaSupportedMipmapGenerationFormat(
    const NiPixelFormat& kSrcFmt) const
{
    if ((kSrcFmt == NiPixelFormat::PALA4) ||
        (kSrcFmt == NiPixelFormat::PALA8) ||
        (kSrcFmt == NiPixelFormat::RGBA32))
        return true;
    return false;
}
//---------------------------------------------------------------------------
bool NiDevImageConverter::IsANonAlphaSupportedMipmapGenerationFormat(
    const NiPixelFormat& kSrcFmt) const
{
    if ((kSrcFmt == NiPixelFormat::PAL4) ||
        (kSrcFmt == NiPixelFormat::PAL8) || 
        (kSrcFmt == NiPixelFormat::RGB24))
        return true;
    return false;
}
//---------------------------------------------------------------------------
NiPixelData* NiDevImageConverter::GenerateMipmapLevels(
    const NiPixelData* pkSrc, NiPixelData* pkOptDest)
{
    const NiPixelFormat& kSrcFmt = pkSrc->GetPixelFormat();
    unsigned int uiFineRowStride;

    NiPixelFormat kDestFmt;

    if (IsANonAlphaSupportedMipmapGenerationFormat(kSrcFmt))
    {
        kDestFmt = NiPixelFormat::RGB24;
        uiFineRowStride = pkSrc->GetWidth()*3;
    }
    else if (IsAnAlphaSupportedMipmapGenerationFormat(kSrcFmt))
    {
        kDestFmt = NiPixelFormat::RGBA32;
        uiFineRowStride = pkSrc->GetWidth()*4;
    }
    else
        return NULL;

    NiPixelData* pkDest = 0;

    if (pkOptDest && (pkOptDest->GetPixelFormat() == kDestFmt) &&
        (pkOptDest->GetNumMipmapLevels() > 1) && 
        pkOptDest->GetNumFaces() == pkSrc->GetNumFaces())
    {
        pkDest = pkOptDest;
    }

    if (!pkDest)
    {
        pkDest = NiNew NiPixelData(pkSrc->GetWidth(), pkSrc->GetHeight(), 
            kDestFmt, 0, pkSrc->GetNumFaces());
    }

    unsigned int uiMipmapLevels = pkDest->GetNumMipmapLevels();

    // if the source and the dest are not the same object, then we need to
    // copy/reformat the base mipmap level to the destination
    if (pkSrc != pkDest)
    {
        if(!ConvertPixelDataFormat(*pkDest, *pkSrc))
            return NULL;
    }

    for (unsigned int uiFace = 0; uiFace < pkSrc->GetNumFaces(); uiFace++)
    {
        if (kDestFmt == NiPixelFormat::RGB24)
        {
            unsigned int i, j, k;
            for (k = 1; k < uiMipmapLevels; k++)
            {
                const unsigned char* pucFine =
                    pkDest->GetPixels(k - 1, uiFace);
                unsigned char* pucCoarse = pkDest->GetPixels(k, uiFace);

                unsigned int uiCoarseWidth = pkDest->GetWidth(k, uiFace);
                unsigned int uiCoarseHeight = pkDest->GetHeight(k, uiFace);
                unsigned int uiRowStride =
                    pkDest->GetWidth(k - 1, uiFace) * 3;

                if (pkDest->GetWidth(k - 1, uiFace) == 1)
                {
                    // if the fine level has a width of one, must special-case
                    for (j = 0; j < uiCoarseHeight; j++)
                    {
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[0] 
                            + (unsigned int)pucFine[3]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[1] +
                            (unsigned int)pucFine[4]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[2] + 
                            (unsigned int)pucFine[5]) >> 1);

                        // step down two pixels
                        pucFine += 6; 
                    }
                }
                else if (pkDest->GetHeight(k - 1, uiFace) == 1)
                {
                    // if the fine level has a height of one, must special-case
                    for (i = 0; i < uiCoarseWidth; i++)
                    {
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[0] 
                            + (unsigned int)pucFine[3]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[1] +
                            (unsigned int)pucFine[4]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[2] + 
                            (unsigned int)pucFine[5]) >> 1);

                        pucFine += 6; // step over two pixels
                    }
                }
                else
                {
                    for (j = 0; j < uiCoarseHeight; j++)
                    {
                        for (i = 0; i < uiCoarseWidth; i++)
                        {
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[0] 
                                + (unsigned int)pucFine[3] +
                                (unsigned int)pucFine[uiRowStride] +
                                (unsigned int)pucFine[uiRowStride + 3])
                                    >> 2);
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[1] +
                                (unsigned int)pucFine[4] +
                                (unsigned int)pucFine[uiRowStride + 1] +
                                (unsigned int)pucFine[uiRowStride + 4])
                                    >> 2);
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[2] + 
                                (unsigned int)pucFine[5] +
                                (unsigned int)pucFine[uiRowStride + 2] +
                                (unsigned int)pucFine[uiRowStride + 5])
                                    >> 2);

                            pucFine += 6; // step over two pixels
                        }
                
                        pucFine += uiRowStride; // an extra row
                    }
                }
            }
        }
        else if (kDestFmt == NiPixelFormat::RGBA32)
        {
            unsigned int i, j, k;
            for (k = 1; k < uiMipmapLevels; k++)
            {
                const unsigned char* pucFine =
                    pkDest->GetPixels(k - 1, uiFace);
                unsigned char* pucCoarse = pkDest->GetPixels(k, uiFace);

                unsigned int uiCoarseWidth = pkDest->GetWidth(k, uiFace);
                unsigned int uiCoarseHeight = pkDest->GetHeight(k, uiFace);
                unsigned int uiRowStride =
                    pkDest->GetWidth(k - 1, uiFace) * 4;

                if (pkDest->GetWidth(k - 1, uiFace) == 1)
                {
                    // if the fine level has a width of one, must special-case
                    for (j = 0; j < uiCoarseHeight; j++)
                    {
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[0] 
                            + (unsigned int)pucFine[4]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[1] +
                            (unsigned int)pucFine[5]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[2] + 
                            (unsigned int)pucFine[6]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[3] + 
                            (unsigned int)pucFine[7]) >> 1);

                        // step down two pixels
                        pucFine += 8; 
                    }
                }
                else if (pkDest->GetHeight(k - 1) == 1)
                {
                    // if the fine level has a height of one, must special-case
                    for (i = 0; i < uiCoarseWidth; i++)
                    {
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[0] 
                            + (unsigned int)pucFine[4]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[1] +
                            (unsigned int)pucFine[5]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[2] + 
                            (unsigned int)pucFine[6]) >> 1);
                        *(pucCoarse++) = static_cast<unsigned char>(((unsigned int)pucFine[3] + 
                            (unsigned int)pucFine[7]) >> 1);

                        pucFine += 8; // step over two pixels
                    }
                }
                else
                {
                    for (j = 0; j < uiCoarseHeight; j++)
                    {
                        for (i = 0; i < uiCoarseWidth; i++)
                        {
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[0] 
                                + (unsigned int)pucFine[4] +
                                (unsigned int)pucFine[uiRowStride] + 
                                (unsigned int)pucFine[uiRowStride + 4])
                                    >> 2);
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[1] +
                                (unsigned int)pucFine[5] +
                                (unsigned int)pucFine[uiRowStride + 1] + 
                                (unsigned int)pucFine[uiRowStride + 5])
                                    >> 2);
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[2] + 
                                (unsigned int)pucFine[6] +
                                (unsigned int)pucFine[uiRowStride + 2] + 
                                (unsigned int)pucFine[uiRowStride + 6])
                                    >> 2);
                            *(pucCoarse++) 
                                = static_cast<unsigned char>(((unsigned int)pucFine[3] + 
                                (unsigned int)pucFine[7] +
                                (unsigned int)pucFine[uiRowStride + 3] + 
                                (unsigned int)pucFine[uiRowStride + 7])
                                    >> 2);

                            pucFine += 8; // step over two pixels
                        }
                
                        pucFine += uiRowStride; // an extra row
                    }
                }
            }
        }
    }

    return pkDest;
}
//---------------------------------------------------------------------------
NiPixelData* NiDevImageConverter::GenerateMipmapPyramidFromFiles(
    const char* apcFilenames[], unsigned int uiNumLevels,
    const NiPixelData* pkSrc)
{
    if (!apcFilenames[0])
        return 0;

    NiPixelData* pkDest;
    NiPixelFormat kFmt;
    bool bMipmap;
    unsigned int uiWidth;
    unsigned int uiHeight;
    unsigned int uiNumFaces;

    if (ReadImageFileInfo(apcFilenames[0], kFmt, bMipmap, uiWidth, uiHeight,
        uiNumFaces))
    {
        if(uiNumFaces != 1)
            return 0;

        pkDest = NiNew NiPixelData(uiWidth, uiHeight, kFmt, uiNumLevels);

        if (!pkDest)
            return 0;

        for (unsigned int i = 0; i < uiNumLevels; i++)
        {
            if (!apcFilenames[i])
            {
                NiDelete pkDest;
                return 0;
            }

            NiPixelDataPtr spLevel;

            if (i == 0 && pkSrc)
                spLevel = (NiPixelData*)pkSrc;
            else
                spLevel = ReadImageFile(apcFilenames[i], NULL);

            if (!spLevel)
            {
                NiDelete pkDest;
                return 0;
            }

            size_t stDestSize = pkDest->GetSizeInBytes(i);

            if (stDestSize != spLevel->GetSizeInBytes())
            {
                NiDelete pkDest;
                return 0;
            }

            NiMemcpy(pkDest->GetPixels(i), spLevel->GetPixels(), stDestSize);

            if (!i && kFmt.GetPalettized())
            {
                // Use the palette from the base image
                pkDest->SetPalette(spLevel->GetPalette());
            }
        }
    }
    else
    {
        return 0;
    }

    return pkDest;
}
//---------------------------------------------------------------------------
NiPixelData* NiDevImageConverter::ConvertForPlatform(Platform ePlatform, 
    const NiTexture::FormatPrefs& kPrefs, const NiPixelData* pkSrc,
    const NiPixelFormat& kDesiredFormat)
{
    NIASSERT(pkSrc);
    const NiPixelFormat& kSrcFmt = pkSrc->GetPixelFormat();

    const NiPixelFormat* pkDestFmt = FindClosestPixelFormat(ePlatform, 
        kPrefs, kSrcFmt, kDesiredFormat);

    if (pkDestFmt == &kSrcFmt)
    {
        // don't convert, since pkSrc is already in correct format
        return (NiPixelData*)pkSrc;
    }

    if (pkDestFmt == NULL)
    {
        return NULL;
    }

    if (*pkDestFmt == pkSrc->GetPixelFormat())
        return (NiPixelData*)pkSrc;

    if (CanConvertPixelData(pkSrc->GetPixelFormat(),*pkDestFmt) == false)
    {
        return NULL;
    }

    // allocate the required destination data
    NiPixelData* pkDest = NiNew NiPixelData(pkSrc->GetWidth(), 
        pkSrc->GetHeight(), *pkDestFmt, pkSrc->GetNumMipmapLevels(),
        pkSrc->GetNumFaces());

    if (ConvertPixelDataFormat(*pkDest, *pkSrc, -1))
    {
        return pkDest;
    }
    else
    {
        NiDelete pkDest;
        return NULL;
    }
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDevImageConverter::FindClosestPixelFormat(
    Platform ePlatform, const NiTexture::FormatPrefs& kFmtPrefs, 
    const NiPixelFormat& kSrcFmt, const NiPixelFormat& kDesiredFmt) const
{
    const NiPixelFormat* pkDestFmt;
    switch (ePlatform)
    {
        case ANY:
            // Temporary code to prevent palettized bump maps on Win32
            // In the future, this code should exist in a separate
            // FindClosestPixelFormatDX9 function, but that will require 
            // changing the tools' interface.
            if ((kDesiredFmt.GetPalettized() || 
                (kDesiredFmt.GetBitsPerPixel() == 16)) &&
                (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP))
            {
                return &NiPixelFormat::RGBA32;
            }
            else
            {
                // return directly
                return &kDesiredFmt;
            }
        case DX9:
            pkDestFmt = FindClosestPixelFormatDX9(kFmtPrefs, 
                kSrcFmt, kDesiredFmt);
            break;
        case D3D10:
            pkDestFmt = FindClosestPixelFormatD3D10(kFmtPrefs, 
                kSrcFmt, kDesiredFmt);
            break;
        case XENON:
            pkDestFmt = FindClosestPixelFormatXenon(kFmtPrefs, 
                kSrcFmt, kDesiredFmt);
            break;
        case PLAYSTATION3:
            pkDestFmt = FindClosestPixelFormatPS3(kFmtPrefs, 
                kSrcFmt, kDesiredFmt);
            break;
        case WII:
            pkDestFmt = FindClosestPixelFormatWii(kFmtPrefs, 
                kSrcFmt, kDesiredFmt);
            break;
        default:
            pkDestFmt = NULL;   // Error return value.
            break;
    }
    return pkDestFmt;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDevImageConverter::FindClosestPixelFormatDX9(
    const NiTexture::FormatPrefs& kFmtPrefs,
    const NiPixelFormat&, const NiPixelFormat& kDesiredFmt) const
{
    // Examine the FormatPrefs.PixelLayout first.  It overrides the desired
    // file format.  Once the format preferences are honored, try to match the
    // desired format as much as possible.

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::SINGLE_COLOR_8)
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
            return &NiPixelFormat::L8;
        else
            return &NiPixelFormat::A8;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::SINGLE_COLOR_16)
        return &NiPixelFormat::R16;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::SINGLE_COLOR_32)
        return &NiPixelFormat::R32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::DOUBLE_COLOR_32)
        return &NiPixelFormat::RG32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::DOUBLE_COLOR_64)
        return &NiPixelFormat::RG64;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_128)
        return &NiPixelFormat::RGBA128;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_64)
        return &NiPixelFormat::RGBA64;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_32)
        return &NiPixelFormat::RGBA64;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
            return &NiPixelFormat::BGRX8888;
        else
            return &NiPixelFormat::BGRA8888;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::HIGH_COLOR_16)
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
            return &NiPixelFormat::BGRA5551;
        else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::BGRA4444;
        else
            return &NiPixelFormat::BGR565;
    }
    
    if ((kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_8) ||
        (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_4))
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::DXT5;
        else
            return &NiPixelFormat::DXT1;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
            return &NiPixelFormat::DXT1;
        else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
            return &NiPixelFormat::DXT3;
        else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
            return &NiPixelFormat::DXT5;
        else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::DXT5;
        else
            return &NiPixelFormat::DXT1;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
        return &NiPixelFormat::BUMPLUMA32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
    {
        if (kDesiredFmt.GetCompressed() || kDesiredFmt.GetPalettized())
        {
            if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
                return &NiPixelFormat::DXT1;
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
                return &NiPixelFormat::DXT3;
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
                return &NiPixelFormat::DXT5;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
                return &NiPixelFormat::DXT5;
            else
                return &NiPixelFormat::DXT1;
        }
        else if (kDesiredFmt.GetBitsPerPixel() == 16)
        {
            bool bDesiredAlpha = kDesiredFmt.GetPalettized() ? 
                kDesiredFmt.GetPaletteHasAlpha() : 
                kDesiredFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0;

            if (kDesiredFmt.GetCompressed())
                bDesiredAlpha = true;

            if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
                return &NiPixelFormat::BGRA5551;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
                return &NiPixelFormat::BGRA4444;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
                return &NiPixelFormat::BGR565;
            else if (bDesiredAlpha) // Desired format has alpha.
                return &NiPixelFormat::BGRA4444;
            else
                return &NiPixelFormat::BGR565;
        }
        else if (kDesiredFmt.GetBitsPerPixel() == 32)
        {
            if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
            {
                return &NiPixelFormat::BGRX8888;
            }
            else if ((kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
                || (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH))
            {
                return &NiPixelFormat::BGRA8888;
            }
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_RGB)
            {
                return &NiPixelFormat::BGRX8888;
            }
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA)
            {
                return &NiPixelFormat::BGRA8888;
            }

            // Fall through to return of desired format.
        }

        // Fall through to return of desired format.
    }

    return &kDesiredFmt;    // Return the originally desired format.
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDevImageConverter::FindClosestPixelFormatD3D10(
    const NiTexture::FormatPrefs& kFmtPrefs,
    const NiPixelFormat&, const NiPixelFormat& kDesiredFmt) const
{
    // Examine the FormatPrefs.PixelLayout first.  It overrides the desired
    // file format.  Once the format preferences are honored, try to match the
    // desired format as much as possible.

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::SINGLE_COLOR_8)
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
            return &NiPixelFormat::L8;
        else
            return &NiPixelFormat::A8;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::SINGLE_COLOR_16)
        return &NiPixelFormat::R16;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::SINGLE_COLOR_32)
        return &NiPixelFormat::R32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::DOUBLE_COLOR_32)
        return &NiPixelFormat::RG32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::DOUBLE_COLOR_64)
        return &NiPixelFormat::RG64;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_128)
        return &NiPixelFormat::RGBA128;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_64)
        return &NiPixelFormat::RGBA64;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_32)
        return &NiPixelFormat::RGBA32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
    {
        return &NiPixelFormat::RGBA32;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::HIGH_COLOR_16)
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
            return &NiPixelFormat::BGRA5551;
        else
            return &NiPixelFormat::BGR565;
    }
    
    if ((kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_8) ||
        (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_4))
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::DXT5;
        else
            return &NiPixelFormat::DXT1;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
            return &NiPixelFormat::DXT1;
        else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
            return &NiPixelFormat::DXT3;
        else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
            return &NiPixelFormat::DXT5;
        else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::DXT5;
        else
            return &NiPixelFormat::DXT1;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
        return &NiPixelFormat::BUMPLUMA32;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
    {
        if (kDesiredFmt.GetCompressed() || kDesiredFmt.GetPalettized())
        {
            if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
                return &NiPixelFormat::DXT1;
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
                return &NiPixelFormat::DXT3;
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
                return &NiPixelFormat::DXT5;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
                return &NiPixelFormat::DXT5;
            else
                return &NiPixelFormat::DXT1;
        }
        else if (kDesiredFmt.GetBitsPerPixel() == 16)
        {
            if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
                return &NiPixelFormat::BGRA5551;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
                return &NiPixelFormat::BGR565;
            else
                return &NiPixelFormat::BGR565;
        }
        else if (kDesiredFmt.GetBitsPerPixel() == 32)
        {
            return &NiPixelFormat::RGBA32;
        }

        // Fall through to return of desired format.
    }

    return &kDesiredFmt;    // Return the originally desired format.
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDevImageConverter::FindClosestPixelFormatXenon(
    const NiTexture::FormatPrefs& kFmtPrefs,
    const NiPixelFormat& kSrcFmt, const NiPixelFormat& kDesiredFmt) const
{
    bool bSrcAlpha = kSrcFmt.GetPalettized() ? 
        kSrcFmt.GetPaletteHasAlpha() : 
        kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0;

    if (kSrcFmt.GetCompressed())
        bSrcAlpha = true;

    // Logic for conversion for Xenon goes as follows.  Consider the format 
    // preferences first.  They override the desired file format since
    // the renderer will convert at load time anyway based off the format
    // preferences.  Once the format preferences are honored, let's try
    // to match the desired format as much as possible.  In the case of 
    // PIX_DEFAULT we honor the desired format.

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_128)
    {
        // 128-bit was requested.
        return &NiPixelFormat::RGBA128;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_64)
    {
        // 64-bit was requested.
        return &NiPixelFormat::RGBA64;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
    {
        // 32-bit was requested, and we can convert anything to 32
        return &NiPixelFormat::BGRA8888;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::HIGH_COLOR_16)
    {
        // Force 16-bit images to 4444.
        return bSrcAlpha ? &NiPixelFormat::BGRA4444 : &NiPixelFormat::BGR565;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_8 ||
        kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_4)
    {
        // Palettized images are not supported on Xenon. We will go to 
        // compressed since that offers similar space savings as palettized.
        return bSrcAlpha ? &NiPixelFormat::DXT5 : &NiPixelFormat::DXT1;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
            return &NiPixelFormat::DXT1;
        else if(kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
            return &NiPixelFormat::DXT3;
        else if(kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
            return &NiPixelFormat::DXT5;
        
        // It's not a specific compressed type, so we make our best guess.
        return bSrcAlpha ? &NiPixelFormat::DXT5 : &NiPixelFormat::DXT1;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
    {
        // We need 32-bit to handle the bump format.
        return &NiPixelFormat::BUMPLUMA32;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
    {
        if (kDesiredFmt.GetPalettized() || kDesiredFmt.GetCompressed())
        {
            if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
                return &NiPixelFormat::DXT1;
            else if(kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
                return &NiPixelFormat::DXT3;
            else if(kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
                return &NiPixelFormat::DXT5;
            
            // It's not a specific compressed type, so we make our best guess.
            return bSrcAlpha ? &NiPixelFormat::DXT5 : &NiPixelFormat::DXT1;
        }

        if (kDesiredFmt.GetBitsPerPixel() == 16)
        {
            return bSrcAlpha ? &NiPixelFormat::BGRA4444 : 
                &NiPixelFormat::BGR565;
        }

        if (kDesiredFmt.GetBitsPerPixel() == 64)
        {
            return &NiPixelFormat::RGBA64;
        }

        if (kDesiredFmt.GetBitsPerPixel() == 128)
        {
            return &NiPixelFormat::RGBA128;
        }

        // Fall through to 32-bit.
    }

    // Xenon cannot use true 24-bit images, so we'd have to go to RGBA32 on the
    // console anyway.  We might as well convert here to save the per-pixel
    // operations at runtime and reduce memory fragmentation.  It will cause
    // longer DVD load times.
    return &NiPixelFormat::BGRA8888;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDevImageConverter::FindClosestPixelFormatPS3(
    const NiTexture::FormatPrefs& kFmtPrefs,
    const NiPixelFormat& kSrcFmt, const NiPixelFormat& kDesiredFmt) const
{
    bool bSrcAlpha = kSrcFmt.GetPalettized() ? 
        kSrcFmt.GetPaletteHasAlpha() : 
        kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0;

    if (kSrcFmt.GetCompressed())
        bSrcAlpha = true;

    // Examine the FormatPrefs.PixelLayout first.  It overrides the desired
    // file format.  Once the format preferences are honored, try to match the
    // desired format as much as possible.

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_128)
        return &NiPixelFormat::RGBA128_BE;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_64)
        return &NiPixelFormat::RGBA64_BE;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::FLOAT_COLOR_32)
        return &NiPixelFormat::RGBA64_BE;
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::HIGH_COLOR_16)
        return (bSrcAlpha ? &NiPixelFormat::BGRA4444 : &NiPixelFormat::BGR565);
    
    if ((kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_8) ||
        (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PALETTIZED_4))
    {
        if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::DXT5;
        else
            return &NiPixelFormat::DXT1;
    }
    
    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::COMPRESSED)
    {
        if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
            return &NiPixelFormat::DXT1;
        else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
            return &NiPixelFormat::DXT3;
        else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
            return &NiPixelFormat::DXT5;
        else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
            return &NiPixelFormat::DXT5;
        else
            return &NiPixelFormat::DXT1;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::BUMPMAP)
    {
        // We need 32-bit to handle the bump format.
        return &NiPixelFormat::BUMPLUMA32;
    }

    if (kFmtPrefs.m_ePixelLayout == NiTexture::FormatPrefs::PIX_DEFAULT)
    {
        if (kDesiredFmt.GetCompressed() || kDesiredFmt.GetPalettized())
        {
            if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1)
                return &NiPixelFormat::DXT1;
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT3)
                return &NiPixelFormat::DXT3;
            else if (kDesiredFmt.GetFormat() == NiPixelFormat::FORMAT_DXT5)
                return &NiPixelFormat::DXT5;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
                return &NiPixelFormat::DXT5;
            else
                return &NiPixelFormat::DXT1;
        }
        else if (kDesiredFmt.GetBitsPerPixel() == 16)
        {
            bool bDesiredAlpha = kDesiredFmt.GetPalettized() ? 
                kDesiredFmt.GetPaletteHasAlpha() : 
                kDesiredFmt.GetBits(NiPixelFormat::COMP_ALPHA) > 0;

            if (kDesiredFmt.GetCompressed())
                bDesiredAlpha = true;

            if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::BINARY)
                return &NiPixelFormat::BGRA4444;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::SMOOTH)
                return &NiPixelFormat::BGRA4444;
            else if (kFmtPrefs.m_eAlphaFmt == NiTexture::FormatPrefs::NONE)
                return &NiPixelFormat::BGR565;
            else if (bDesiredAlpha) // Desired format has alpha.
                return &NiPixelFormat::BGRA4444;
            else
                return &NiPixelFormat::BGR565;
        }
        else if ((kDesiredFmt == NiPixelFormat::RGBA64) ||
            (kDesiredFmt == NiPixelFormat::RGBA64_BE))
        {
            return &NiPixelFormat::RGBA64_BE;
        }
        else if ((kDesiredFmt == NiPixelFormat::RGBA128) ||
            (kDesiredFmt == NiPixelFormat::RGBA128_BE))
        {
            return &NiPixelFormat::RGBA128_BE;
        }

        // Fall through to optimal default format.
    }

    // If the format hasn't been resolved by this point, resort to ARGB8888,
    // which is an optimal 32-bit format for PS3.  Converting at this point
    // will save the per-pixel operations at runtime and reduce memory
    // fragmentation.  It may or may not cause longer DVD load times.
    return &NiPixelFormat::ARGB8888;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiDevImageConverter::FindClosestPixelFormatWii(
    const NiTexture::FormatPrefs& kFmtPrefs, 
    const NiPixelFormat& kSrcFmt, const NiPixelFormat&) const
{
    const NiTexture::FormatPrefs::PixelLayout eLayout = 
        kFmtPrefs.m_ePixelLayout;

    // Check for an exact match with pixel layout request
    if ((kSrcFmt == NiPixelFormat::RGBA32 && 
        (eLayout == NiTexture::FormatPrefs::TRUE_COLOR_32 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)) ||

        ((kSrcFmt == NiPixelFormat::PAL8) &&
        (eLayout == NiTexture::FormatPrefs::PALETTIZED_8 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)) ||

        ((kSrcFmt == NiPixelFormat::PALA8) &&
        (eLayout == NiTexture::FormatPrefs::PALETTIZED_8 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)) ||

        ((kSrcFmt == NiPixelFormat::PAL4) &&
        (eLayout == NiTexture::FormatPrefs::PALETTIZED_4 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)) ||

        ((kSrcFmt == NiPixelFormat::PALA4) &&
        (eLayout == NiTexture::FormatPrefs::PALETTIZED_4 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)) ||

        ((kSrcFmt == NiPixelFormat::BGRA4443_BE ||
        kSrcFmt == NiPixelFormat::BGR565_BE ||
        kSrcFmt == NiPixelFormat::BUMP16) &&
        (eLayout == NiTexture::FormatPrefs::HIGH_COLOR_16 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)) ||

        ((kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_DXT1) &&
        (eLayout == NiTexture::FormatPrefs::COMPRESSED ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)))
    {
        return &kSrcFmt;
    }

    if ((kSrcFmt == NiPixelFormat::RGB24 && 
        (eLayout == NiTexture::FormatPrefs::TRUE_COLOR_32 ||
        eLayout == NiTexture::FormatPrefs::PIX_DEFAULT)))
    {
        return &NiPixelFormat::RGBA32;
    }

    if (kSrcFmt.GetBitsPerPixel() == 4)
    {
        return NULL;
    }

    // Otherwise classify source data's alpha 
    bool bHasAlpha = 
        ((kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA)!=0 ? true : false) |
        kSrcFmt.GetPaletteHasAlpha());

    if (kSrcFmt.GetCompressed())
    {
        return &NiPixelFormat::RGBA32;
    }

    if (eLayout == NiTexture::FormatPrefs::BUMPMAP)
    {
        return &NiPixelFormat::BUMPLUMA32_WII;
    }

    // Find a reasonable option for conversion
    if (bHasAlpha)
    {
        if (eLayout == NiTexture::FormatPrefs::HIGH_COLOR_16 ||
            (eLayout == NiTexture::FormatPrefs::PIX_DEFAULT &&
            kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_RGBA &&
            kSrcFmt.GetBitsPerPixel() == 16))
        {
            return &NiPixelFormat::BGRA4443_BE;
        }
        else // if (eLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
        {
            return &NiPixelFormat::RGBA32;
        }
    }
    else
    {
        if (eLayout == NiTexture::FormatPrefs::HIGH_COLOR_16 ||
            (eLayout == NiTexture::FormatPrefs::PIX_DEFAULT &&
            kSrcFmt.GetFormat() == NiPixelFormat::FORMAT_RGB &&
            kSrcFmt.GetBitsPerPixel() == 16))
        {
            return &NiPixelFormat::BGR565_BE;
        }
        else // if (eLayout == NiTexture::FormatPrefs::TRUE_COLOR_32)
        {
            // Default to RGBA32 since RGB24 isn't supportted on the Wii
            return &NiPixelFormat::RGBA32;
        }
    }
}

//---------------------------------------------------------------------------
NiPixelData* NiDevImageConverter::GenerateBumpMap(
    const NiPixelData& kSrc, const NiPixelFormat& kDestFmt)
{
    // This function stores up to 4 values in the bump map.  If the dest
    // ends up as BUMP16, then it stores 8 bits of dU followed by 8 bits
    // of dV.  If the foramt is BUMPLUMA32, then dU, dV, L, and H are stored
    // where L is the luma value and H is the original height map height.
    // On most platforms, these 8 bit values are stored as dU, dV, L, H.  On
    // Wii, they are stored as H, L, dU, dV to work correctly with the TEV.
    NiPixelData* pkDest = NULL;

    unsigned int uiWidth = kSrc.GetWidth();
    unsigned int uiHeight = kSrc.GetHeight();
    unsigned int uiLevels = kSrc.GetNumMipmapLevels();
    unsigned int uiSrcPixelStride;
    unsigned int uiDestPixelStride;

    if (kSrc.GetPixelFormat() == NiPixelFormat::RGBA32)
    {
        pkDest = NiNew NiPixelData(uiWidth, uiHeight, 
            (kDestFmt == NiPixelFormat::BUMPLUMA32_WII) ? 
            NiPixelFormat::BUMPLUMA32_WII : NiPixelFormat::BUMPLUMA32,
            uiLevels);

        unsigned int l, i, j;

        // copy luminance to the destination
        for (l = 0; l < uiLevels; l++)
        {
            unsigned char* pucDest = pkDest->GetPixels(l);
    
            uiWidth = kSrc.GetWidth(l);
            uiHeight = kSrc.GetHeight(l);
            
            const unsigned char* pucSrc = kSrc.GetPixels(l);

            for (j = 0; j < uiHeight; j++)
            {
                for (i = 0; i < uiWidth; i++)
                {
                    // Copy the luma from the alpha and the red value to be
                    // the height.  This assumes that the RGB is a true 
                    // greyscale height map and R == G == B.
                    pucDest[2] = pucSrc[3];
                    pucDest[3] = pucSrc[0];

                    pucSrc += 4;
                    pucDest += 4;
                }
            }
        }

        uiSrcPixelStride = 4;
        uiDestPixelStride = 4;
    }
    else if ((kSrc.GetPixelFormat() == NiPixelFormat::RGB24) &&
        ((kDestFmt == NiPixelFormat::BUMPLUMA32) || 
        (kDestFmt == NiPixelFormat::BUMPLUMA32_WII)))
    {
        NiOutputDebugString("Warning: Creating a BUMPLUMA32 image from\n"
            "RGB24.  All luma values will be 1.\n");

        pkDest = NiNew NiPixelData(uiWidth, uiHeight, 
            (kDestFmt == NiPixelFormat::BUMPLUMA32_WII) ? 
            NiPixelFormat::BUMPLUMA32_WII : NiPixelFormat::BUMPLUMA32,
            uiLevels);

        unsigned int l, i, j;

        // copy luminance to the destination
        for (l = 0; l < uiLevels; l++)
        {
            unsigned char* pucDest = pkDest->GetPixels(l);
    
            uiWidth = kSrc.GetWidth(l);
            uiHeight = kSrc.GetHeight(l);
            
            const unsigned char* pucSrc = kSrc.GetPixels(l);

            for (j = 0; j < uiHeight; j++)
            {
                for (i = 0; i < uiWidth; i++)
                {
                    // No alpha in src, so we hard code in 0xff
                    pucDest[2] = 0xff;
                    pucDest[3] = pucSrc[0];

                    pucSrc += 3;
                    pucDest += 4;
                }
            }
        }

        uiSrcPixelStride = 3;
        uiDestPixelStride = 4;
    }
    else if (kSrc.GetPixelFormat() == NiPixelFormat::RGB24)
    {
        pkDest = NiNew NiPixelData(uiWidth, uiHeight, NiPixelFormat::BUMP16, 
            uiLevels);

        uiSrcPixelStride = 3;
        uiDestPixelStride = 2;
    }
    else
    {
        return NULL;
    }
    
    unsigned int l, i, j;

    for (l = 0; l < uiLevels; l++)
    {
        unsigned char* pucDest = pkDest->GetPixels(l);

        uiWidth = kSrc.GetWidth(l);
        uiHeight = kSrc.GetHeight(l);
        
        const unsigned char* pucSrc = kSrc.GetPixels(l);
        const unsigned char* pucSrcU = pucSrc + uiSrcPixelStride;
        const unsigned char* pucSrcV = pucSrc + (uiWidth * uiSrcPixelStride);

        uiWidth--;
        uiHeight--;

        for (j = 0; j < uiHeight; j++)
        {
            for (i = 0; i < uiWidth; i++)
            {
                // using first-order forward diffs
                short s00 = *pucSrc;
                short s01 = *pucSrcU;
                short s10 = *pucSrcV;

                short sDU = s01 - s00;
                short sDV = s10 - s00;

                // the values above could end up being 9 bits (-255...255)
                // but we DO NOT shift them to the right one bit before
                // assigning them to the 8-bit map.  Rather, we choose to
                // simply drop the high-order bit.  As a result, if a 
                // height-map pixel and its neighbor differ by more than 
                // 127 levels in either direction (+/-), then the 
                // resulting bump pixel will be "noise".  However, a bump
                // height discontinuity of >127 is such a spike that noise
                // is probably a reasonable value.

                pucDest[0] = (unsigned char)sDU;
                pucDest[1] = (unsigned char)sDV;

                pucSrc += uiSrcPixelStride;
                pucSrcU += uiSrcPixelStride;
                pucSrcV += uiSrcPixelStride;
                pucDest += uiDestPixelStride;
            }

            // Special-case the row-end pixel (this cannot be the 
            // lower-right corner pixel, since we are inside the j loop)
            
            if (i)
            {
                // backward first difference
                pucDest[0] = *pucSrc - *(pucSrc - uiSrcPixelStride);
            }
            else
            {
                // single column - just use 0
                pucDest[0] = 0;
            }

            pucDest[1] = *pucSrcV - *pucSrc;

            pucSrc += uiSrcPixelStride;
            pucSrcU += uiSrcPixelStride;
            pucSrcV += uiSrcPixelStride;
            pucDest += uiDestPixelStride;
        }

        // special-case the final row (note uiWidth+1 to correct for 
        // decrement of the value above)
        pucSrcV = pucSrc - (uiWidth + 1) * uiSrcPixelStride;

        for (i = 0; i < uiWidth; i++)
        {
            // using first-order forward diffs
            short s00 = *pucSrc;
            short s01 = *pucSrcU;
            short s10 = *pucSrcV;

            short sDU = s01 - s00;

            // Must flip, as s10 is a backward diff
            short sDV = s00 - s10;

            // the values above could end up being 9 bits (-255...255)
            // but we DO NOT shift them to the right one bit before
            // assigning them to the 8-bit map.  Rather, we choose to
            // simply drop the high-order bit.  As a result, if a 
            // height-map pixel and its neighbor differ by more than 
            // 127 levels in either direction (+/-), then the 
            // resulting bump pixel will be "noise".  However, a bump
            // height discontinuity of >127 is such a spike that noise
            // is probably a reasonable value.

            pucDest[0] = (unsigned char)sDU;
            pucDest[1] = (unsigned char)sDV;

            pucSrc += uiSrcPixelStride;
            pucSrcU += uiSrcPixelStride;
            pucSrcV += uiSrcPixelStride;
            pucDest += uiDestPixelStride;
        }

        // Special-case the row-end pixel (this is the 
        // lower-right corner pixel)
        
        if (i)
        {
            // backward first difference
            pucDest[0] = *pucSrc - *(pucSrc - uiSrcPixelStride);

            if (j)
            {
                // backward first difference
                pucDest[1] = *pucSrc - *pucSrcV;

            }
            else
            {
                // single row - just use 0
                pucDest[1] = 0;
            }
        }
        else
        {
            // single column - just use 0
            pucDest[0] = 0;

            if (j)
            {
                // backward first difference
                pucDest[1] = *pucSrc - *pucSrcV;

            }
            else
            {
                // single row - just use 0
                pucDest[1] = 0;
            }
        }
    }

    if (pkDest->GetPixelFormat() == NiPixelFormat::BUMPLUMA32_WII)
    {
        ConvertBUMPLUMA32ToBUMPLUMA32_WII(pkDest);
    }

    return pkDest;
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertBUMPLUMA32ToBUMPLUMA32_WII(
    NiPixelData* pkDest)
{
    NIASSERT(pkDest);

    unsigned int l, i, j;
    unsigned int uiLevels, uiWidth, uiHeight;
    uiLevels = pkDest->GetNumMipmapLevels();
    for (l = 0; l < uiLevels; l++)
    {
        unsigned char* pucPixels = pkDest->GetPixels(l);
    
        uiWidth = pkDest->GetWidth(l);
        uiHeight = pkDest->GetHeight(l);
            
        for (j = 0; j < uiHeight; j++)
        {
            for (i = 0; i < uiWidth; i++)
            {
                // Take a BUMPLUMA32 pixel data object and convert the
                // pixels for Wii.  Instead of dU, dV, luma, height, we
                // need height, luma, dV, dU.  Also, the dV and dU need
                // to be modified since they represent signed 8-bit ints
                // and we need to make them biased, unsigned 8-bit ints
                
                // Get dV, modify and write to bits 16-23.  We don't care
                // if we blitz those bits since luma is always 255 on Wii
                unsigned char ucTemp = pucPixels[1];
                ucTemp += 128;
                
                pucPixels[2] = ucTemp;

                // now get dU into ucTemp
                ucTemp = pucPixels[0];

                // we have dU, so copy height to bits 0-7, and write 0xff to 
                // bits 8-15.
                pucPixels[0] = pucPixels[3];
                pucPixels[1] = 0xff;

                // convert dU and write to bits 24-31
                ucTemp += 128;
                pucPixels[3] = ucTemp;
                pucPixels += 4;
            }
        }
    }
}
//---------------------------------------------------------------------------
#define PackPixel(val, kBits, cComp) \
    (((((unsigned long)val)>> kBits. m_uc##cComp##Q) \
    << kBits. m_uc##cComp##S) & kBits. m_ui##cComp##M)

#define UnpackPixel(pS, kBits, cComp) \
    ((unsigned char)((((*pS) & kBits. m_ui##cComp##M) \
    >> kBits. m_uc##cComp##S) << kBits. m_uc##cComp##Q))

#define UnpackPixelDirect(val, kBits, cComp) \
    ((unsigned char)((((val) & kBits. m_ui##cComp##M) \
    >> kBits. m_uc##cComp##S) << kBits. m_uc##cComp##Q))
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGBA16ToBGRA16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette*, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    const unsigned short* pusSrc = (const unsigned short*)pucSrc;
    unsigned short* pusW = (unsigned short*)pucDest;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR, ucG, ucB, ucA;
            ucR = UnpackPixel(pusSrc, kSrcBits, R);
            ucG = UnpackPixel(pusSrc, kSrcBits, G);
            ucB = UnpackPixel(pusSrc, kSrcBits, B);
            ucA = UnpackPixel(pusSrc, kSrcBits, A);

            *pusW = (unsigned short)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(ucA, kDestBits, A));

            pusW++;
            pusSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGB16To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits)
{
    const unsigned short* pusSrc = (const unsigned short*)pucSrc;
    unsigned short* pusW = (unsigned short*)pucDest;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR, ucG, ucB;
            ucR = UnpackPixel(pusSrc, kSrcBits, R);
            ucG = UnpackPixel(pusSrc, kSrcBits, G);
            ucB = UnpackPixel(pusSrc, kSrcBits, B);

            *pusW = (unsigned short)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B));

            pusW++;
            pusSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGBA16ToBGRA4443(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette*, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    const unsigned short* pusSrc = (const unsigned short*)pucSrc;
    unsigned short* pusW = (unsigned short*)pucDest;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR, ucG, ucB, ucA;
            ucR = UnpackPixel(pusSrc, kSrcBits, R);
            ucG = UnpackPixel(pusSrc, kSrcBits, G);
            ucB = UnpackPixel(pusSrc, kSrcBits, B);
            ucA = UnpackPixel(pusSrc, kSrcBits, A);

            // Ensure that 1-bit alpha is handled correclty
            // since we need to force the most significant bit
            // to zero for the BGRA4443 format as defined by 
            // Nintendo. The PackPixel() macro will shift
            // the Alpha 5 right which will result in a 0
            // in the MSB of the alpha.
            if (ucA)
            {
                bool bOneBitAlpha = 
                    !(kSrcBits.m_uiAM & (kSrcBits.m_uiAM - 1));
                ucA = (bOneBitAlpha ? 0xE0 : (ucA & 0xE0));
            }

            *pusW = (unsigned short)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(ucA, kDestBits, A));

            pusW++;
            pusSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGBA32ToBGRA4443(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette*, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    unsigned short* pusW = (unsigned short*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR = *(pucSrc++);
            unsigned char ucG = *(pucSrc++);
            unsigned char ucB = *(pucSrc++);
            unsigned char ucA = *(pucSrc++);

            *pusW = (unsigned short)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(ucA, kDestBits, A));

            pusW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertBGRA16ToRGBA16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    // Function
    ConvertRGBA16ToBGRA16(uiWidth, uiHeight, uiDestSize, pucDest, pkSrcPal,
        kDestBits, pucSrc, kSrcBits);
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPAL4To24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    if (kDestBits.m_uiGM != 0x0000ff00)
        return;
    
    // Expand out and treat as ConvertPAL8To32
    unsigned char* pucExpanded = NiAlloc2(unsigned char, uiHeight * uiWidth,
        NiMemHint::TEXTURE);
    unsigned char* pucStart = pucExpanded;

    unsigned int i;

    if (uiHeight == uiWidth && uiHeight == 1)
    {
        *pucExpanded = pucSrc[0] >> 4;
    }
    else
    {
        // If width is odd, then assert, since that format is not supported.
        NIASSERT(!(uiWidth & 0x1));

        for (i=0; i < uiHeight*(uiWidth >> 1); i++)
        {
            *pucExpanded++ = pucSrc[i] >> 4;
            *pucExpanded++ = pucSrc[i] & 0x0f;
        }
    }

    pucExpanded = pucStart;


    if (kDestBits.m_uiRM == 0x000000ff)
    {
        // must expand data from palettized to RGB
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            // Set pointer to next row
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucExpanded].m_ucRed;
                *(pucDest++) = pkPal[*pucExpanded].m_ucGreen;
                *(pucDest++) = pkPal[*pucExpanded].m_ucBlue;

                pucExpanded++;
            }
        }
    }
    else if (kDestBits.m_uiRM == 0x00ff0000)
    {
        // must expand data from palettized to BGR
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucExpanded].m_ucBlue;
                *(pucDest++) = pkPal[*pucExpanded].m_ucGreen;
                *(pucDest++) = pkPal[*pucExpanded].m_ucRed;

                pucExpanded++;
            }
        }
    }

    NiFree(pucStart);
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPAL4To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits,
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    // Expand out and treat as ConvertPAL4To32
    unsigned char* pucExpanded = NiAlloc2(unsigned char, uiHeight * uiWidth,
        NiMemHint::TEXTURE);
    unsigned char* pucStart = pucExpanded;

    unsigned int i;

    if (uiHeight == uiWidth && uiHeight == 1)
    {
        *pucExpanded = pucSrc[0] >> 4;
    }
    else
    {
        // If width is odd, then assert, since that format is not supported.
        NIASSERT(!(uiWidth & 0x1));

        for (i=0; i < uiHeight*(uiWidth >> 1); i++)
        {
            *pucExpanded++ = pucSrc[i] >> 4;
            *pucExpanded++ = pucSrc[i] & 0x0f;
        }
    }

    pucExpanded = pucStart;

    unsigned long aulLut[16];
    for (i = 0; i < 16; i++)
    {
        aulLut[i] = (unsigned long)(
            PackPixel(pkPal[i].m_ucRed, kDestBits, R) |
            PackPixel(pkPal[i].m_ucGreen, kDestBits, G) |
            PackPixel(pkPal[i].m_ucBlue, kDestBits, B) |
            PackPixel(255, kDestBits, A));
    }

    unsigned int* puiDW = (unsigned int*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
            *(puiDW++) = aulLut[*(pucExpanded++)];
    }

    NiFree(pucStart);
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPALA4To24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette* pkSrcPal, const PixelBits& kDestBits,
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    if (kDestBits.m_uiGM != 0x0000ff00)
        return;
   
    // Expand out and treat as ConvertPAL8To32
    unsigned char* pucExpanded = NiAlloc2(unsigned char, uiHeight * uiWidth,
        NiMemHint::TEXTURE);
    unsigned char* pucStart = pucExpanded;

    unsigned int i;

    // If width is odd, then assert, since that format is not supported.
    NIASSERT(!(uiWidth & 0x1));

    for (i=0; i < uiHeight*(uiWidth >> 1); i++)
    {
        *pucExpanded++ = pucSrc[i] >> 4;
        *pucExpanded++ = pucSrc[i] & 0x0f;
    }

    pucExpanded = pucStart;

    bool bIsLittleEndian = NiSystemDesc::GetSystemDesc().IsLittleEndian();

    //Determine if red should be the first char packed. This mask is
    //0x000000FF on little endian machines and 0x00FF0000 on big 
    //endian machines.
    if ((bIsLittleEndian && kDestBits.m_uiRM == 0x000000FF) ||
        (!bIsLittleEndian && kDestBits.m_uiRM == 0x00FF0000))
    {
        // must expand data from palettized to RGB
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucExpanded].m_ucRed;
                *(pucDest++) = pkPal[*pucExpanded].m_ucGreen;
                *(pucDest++) = pkPal[*pucExpanded].m_ucBlue;

                pucExpanded++;
            }
        }
    }
    // Determine if we should pack red third. The mask is 0x00FF0000
    // on little endian machines and 0x000000FF on big endian machines.
    else if ((bIsLittleEndian && kDestBits.m_uiRM == 0x00FF0000) ||
        (!bIsLittleEndian && kDestBits.m_uiRM == 0x000000FF))
    {
        // must expand data from palettized to BGR
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucExpanded].m_ucBlue;
                *(pucDest++) = pkPal[*pucExpanded].m_ucGreen;
                *(pucDest++) = pkPal[*pucExpanded].m_ucRed;

                pucExpanded++;
            }
        }
    }

    NiFree(pucStart);
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPALA4To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    // Expand out and treat as ConvertPAL8To32
    unsigned char* pucExpanded = NiAlloc2(unsigned char, uiHeight * uiWidth,
        NiMemHint::TEXTURE);
    unsigned char* pucStart = pucExpanded;

    unsigned int i;

    // If width is odd, then assert, since that format is not supported.
    NIASSERT(!(uiWidth & 0x1));

    for (i=0; i < uiHeight*(uiWidth >> 1); i++)
    {
        *pucExpanded++ = pucSrc[i] >> 4;
        *pucExpanded++ = pucSrc[i] & 0x0f;
    }

    pucExpanded = pucStart;

    unsigned long aulLut[16];
    for (i = 0; i < 16; i++)
    {
        aulLut[i] = (unsigned long)(
            PackPixel(pkPal[i].m_ucRed, kDestBits, R) |
            PackPixel(pkPal[i].m_ucGreen, kDestBits, G) |
            PackPixel(pkPal[i].m_ucBlue, kDestBits, B) |
            PackPixel(pkPal[i].m_ucAlpha, kDestBits, A));
    }

    unsigned int* puiDW = (unsigned int*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
            *(puiDW++) = aulLut[*(pucExpanded++)];
    }

    NiFree(pucStart);
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPAL8To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    // must expand data from palettized to RGB
    unsigned short ausLut[256];
    for (unsigned int i = 0; i < 256; i++)
    {
        ausLut[i] = (unsigned short)(
            PackPixel(pkPal[i].m_ucRed, kDestBits, R) |
            PackPixel(pkPal[i].m_ucGreen, kDestBits, G) |
            PackPixel(pkPal[i].m_ucBlue, kDestBits, B) |
            PackPixel(255, kDestBits, A));
    }

    unsigned short* pusW = (unsigned short*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        // Set pointer to next row
        for (unsigned int x = 0; x < uiWidth; x++)
            *(pusW++) = ausLut[*(pucSrc++)];
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPAL8To24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    if (kDestBits.m_uiGM != 0x0000ff00)
        return;

    bool bIsLittleEndian = NiSystemDesc::GetSystemDesc().IsLittleEndian();

    //Determine if red should be the first char packed. This mask is
    //0x000000FF on little endian machines and 0x00FF0000 on big 
    //endian machines.
    if ((bIsLittleEndian && kDestBits.m_uiRM == 0x000000FF) ||
        (!bIsLittleEndian && kDestBits.m_uiRM == 0x00FF0000))
    {
        // must expand data from palettized to RGB
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            // Set pointer to next row
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucSrc].m_ucRed;
                *(pucDest++) = pkPal[*pucSrc].m_ucGreen;
                *(pucDest++) = pkPal[*pucSrc].m_ucBlue;

                pucSrc++;
            }
        }
    }
    // Determine if we should pack red third. The mask is 0x00FF0000
    // on little endian machines and 0x000000FF on big endian machines.
    else if ((bIsLittleEndian && kDestBits.m_uiRM == 0x00FF0000) ||
        (!bIsLittleEndian && kDestBits.m_uiRM == 0x000000FF))
    {
        // must expand data from palettized to BGR
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucSrc].m_ucBlue;
                *(pucDest++) = pkPal[*pucSrc].m_ucGreen;
                *(pucDest++) = pkPal[*pucSrc].m_ucRed;

                pucSrc++;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPAL8To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits,
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    unsigned long aulLut[256];
    for (unsigned int i = 0; i < 256; i++)
    {
        aulLut[i] = (unsigned long)(
            PackPixel(pkPal[i].m_ucRed, kDestBits, R) |
            PackPixel(pkPal[i].m_ucGreen, kDestBits, G) |
            PackPixel(pkPal[i].m_ucBlue, kDestBits, B) |
            PackPixel(255, kDestBits, A));
    }

    unsigned int* puiDW = (unsigned int*)pucDest;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
            *(puiDW++) = aulLut[*(pucSrc++)];
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPALA8To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest, 
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    // must expand data from palettized to RGB
    unsigned short ausLut[256];
    for (unsigned int i = 0; i < 256; i++)
    {
        ausLut[i] = (unsigned short)(
            PackPixel(pkPal[i].m_ucRed, kDestBits, R) |
            PackPixel(pkPal[i].m_ucGreen, kDestBits, G) |
            PackPixel(pkPal[i].m_ucBlue, kDestBits, B) |
            PackPixel(pkPal[i].m_ucAlpha, kDestBits, A));
    }

    unsigned short* pusW = (unsigned short*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
            *(pusW++) = ausLut[*(pucSrc++)];
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPALA8To24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    if (kDestBits.m_uiGM != 0x0000ff00)
        return;
    
    if (kDestBits.m_uiRM == 0x000000ff)
    {
        // must expand data from palettized to RGB
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucSrc].m_ucRed;
                *(pucDest++) = pkPal[*pucSrc].m_ucGreen;
                *(pucDest++) = pkPal[*pucSrc].m_ucBlue;

                pucSrc++;
            }
        }
    }
    else if (kDestBits.m_uiRM == 0x00ff0000)
    {
        // must expand data from palettized to BGR
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = pkPal[*pucSrc].m_ucBlue;
                *(pucDest++) = pkPal[*pucSrc].m_ucGreen;
                *(pucDest++) = pkPal[*pucSrc].m_ucRed;

                pucSrc++;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertPALA8To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette* pkSrcPal, const PixelBits& kDestBits, 
    const unsigned char* pucSrc, const PixelBits&) 
{
    NIASSERT(pkSrcPal);
    const NiPalette::PaletteEntry* pkPal = pkSrcPal->GetEntries();

    unsigned long aulLut[256];
    for (unsigned int i = 0; i < 256; i++)
    {
        aulLut[i] = (unsigned long)(
            PackPixel(pkPal[i].m_ucRed, kDestBits, R) |
            PackPixel(pkPal[i].m_ucGreen, kDestBits, G) |
            PackPixel(pkPal[i].m_ucBlue, kDestBits, B) |
            PackPixel(pkPal[i].m_ucAlpha, kDestBits, A));
    }

    unsigned int* puiDW = (unsigned int*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
            *(puiDW++) = aulLut[*(pucSrc++)];
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGB24To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits& kDestBits,
    const unsigned char* pucSrc, const PixelBits&) 
{
    unsigned short* pusW = (unsigned short*)pucDest;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR = *(pucSrc++);
            unsigned char ucG = *(pucSrc++);
            unsigned char ucB = *(pucSrc++);

            *pusW = (unsigned short)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(0xff, kDestBits, A));

            pusW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGB24ToBGR565_BE(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits&,
    const unsigned char* pucSrc, const PixelBits&) 
{
    unsigned short* pusW = (unsigned short*)pucDest;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR = *(pucSrc++);
            unsigned char ucG = *(pucSrc++);
            unsigned char ucB = *(pucSrc++);

            *pusW = ((unsigned short)(ucR & 0xf8) << 8) |
                ((unsigned short)(ucG & 0xfc) << 3) | 
                ((unsigned short)(ucB & 0xf8) >> 3);

            if (NiSystemDesc::GetSystemDesc().IsLittleEndian())
            {
                NiEndian::Swap16(reinterpret_cast<char*>(pusW));
            }
            pusW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGB24To24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned uiDestSize, unsigned char* pucDest,
    const NiPalette*, const PixelBits& kDestBits,
    const unsigned char* pucSrc, const PixelBits&) 
{
    // direct copy of 24-bit data
    if (kDestBits.m_uiGM != 0x00ff00)
        return;
    
    if (kDestBits.m_uiRM == 0x00ff0000)
    {
        // must expand data from RGB to BGR
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = *(pucSrc+2);
                *(pucDest++) = *(pucSrc+1);
                *(pucDest++) = *pucSrc;

                pucSrc+=3;
            }
        }
    }
    else if (kDestBits.m_uiRM == 0x000000ff)
    {
        NiMemcpy(pucDest, uiDestSize, pucSrc, 3 * uiWidth * uiHeight);
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGB24To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, 
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    unsigned int* puiDW = (unsigned int*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR = *(pucSrc++);
            unsigned char ucG = *(pucSrc++);
            unsigned char ucB = *(pucSrc++);

            *puiDW = (unsigned int)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(255, kDestBits, A));

            puiDW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGBA32To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, 
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    unsigned short* pusW = (unsigned short*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucR = *(pucSrc++);
            unsigned char ucG = *(pucSrc++);
            unsigned char ucB = *(pucSrc++);
            unsigned char ucA = *(pucSrc++);

            *pusW = (unsigned short)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(ucA, kDestBits, A));

            pusW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGBA32To24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, 
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    if (kDestBits.m_uiGM != 0x0000ff00)
        return;

    if (kDestBits.m_uiRM == 0x00ff0000)
    {
        // convert to BGR
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = *(pucSrc+2);
                *(pucDest++) = *(pucSrc+1);
                *(pucDest++) = *pucSrc;
                pucSrc += 4; // skip the alpha component
            }
        }
    }
    else if (kDestBits.m_uiRM == 0x000000ff)
    {
        // convert to RGB
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = *(pucSrc++);
                *(pucDest++) = *(pucSrc++);
                *(pucDest++) = *(pucSrc++);
                pucSrc++; // skip the alpha component
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertRGBA32To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, 
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits& kSrcBits) 
{
    unsigned int* puiDW = (unsigned int*)pucDest;
    unsigned int* puiSW = (unsigned int*)pucSrc;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            // Read and unpack a pixel
            unsigned int uiRGBA = *puiSW++;
            unsigned char ucR = UnpackPixelDirect(uiRGBA, kSrcBits, R);
            unsigned char ucG = UnpackPixelDirect(uiRGBA, kSrcBits, G);
            unsigned char ucB = UnpackPixelDirect(uiRGBA, kSrcBits, B);
            unsigned char ucA = UnpackPixelDirect(uiRGBA, kSrcBits, A);

            // Write a pixel
            *puiDW++ = (unsigned int)(
                PackPixel(ucR, kDestBits, R) |
                PackPixel(ucG, kDestBits, G) |
                PackPixel(ucB, kDestBits, B) |
                PackPixel(ucA, kDestBits, A));
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert16ToRGB24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int,
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits&, const unsigned char* pucSrc, 
    const PixelBits& kSrcBits) 
{
    const unsigned short* pusSrc = (const unsigned short*)pucSrc;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, R);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, G);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, B);

            pusSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert16AlphaToRGBA32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int,
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits&, const unsigned char* pucSrc, 
    const PixelBits& kSrcBits) 
{
    const unsigned short* pusSrc = (const unsigned short*)pucSrc;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, R);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, G);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, B);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, A);

            pusSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert16NoAlphaToRGBA32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int,
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits&, const unsigned char* pucSrc, 
    const PixelBits& kSrcBits) 
{
    const unsigned short* pusSrc = (const unsigned short*)pucSrc;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, R);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, G);
            *(pucDest++) = UnpackPixel(pusSrc, kSrcBits, B);
            *(pucDest++) = 255;

            pusSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert24ToRGB24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest, 
    const NiPalette*, const PixelBits&, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    if (kSrcBits.m_uiGM != 0x00ff00)
        return;
    
    if (kSrcBits.m_uiRM == 0x00ff0000)
    {
        // must expand data from BGR to RGB
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = *(pucSrc+2);
                *(pucDest++) = *(pucSrc+1);
                *(pucDest++) = *pucSrc;

                pucSrc+=3;
            }
        }
    }
    else if (kSrcBits.m_uiRM == 0x000000ff)
    {
        // RGB to RGB - direct copy
        NiMemcpy(pucDest, uiDestSize, pucSrc, 3 * uiWidth * uiHeight);
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert24ToRGBA32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits&, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    if (kSrcBits.m_uiGM != 0x0000ff00)
        return;
   
    if (kSrcBits.m_uiRM == 0x00ff0000)
    {
        // must expand data from BGR to RGBA
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = *(pucSrc+2);
                *(pucDest++) = *(pucSrc+1);
                *(pucDest++) = *pucSrc;
                *(pucDest++) = 255;

                pucSrc+=3;
            }
        }
    }
    else
    {
        // must expand data from RGB to RGBA
        for (unsigned int y = 0; y < uiHeight; y++)
        {
            for (unsigned int x = 0; x < uiWidth; x++)
            {
                *(pucDest++) = *(pucSrc++);
                *(pucDest++) = *(pucSrc++);
                *(pucDest++) = *(pucSrc++);
                *(pucDest++) = 255;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert32ToRGB24(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits&, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    const unsigned int* puiSrc = (const unsigned int*)pucSrc;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, R);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, G);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, B);

            puiSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert32AlphaToRGBA32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits&, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    const unsigned int* puiSrc = (const unsigned int*)pucSrc;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, R);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, G);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, B);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, A);

            puiSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::Convert32NoAlphaToRGBA32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, unsigned char* pucDest,
    const NiPalette*, const PixelBits&, 
    const unsigned char* pucSrc, const PixelBits& kSrcBits) 
{
    const unsigned int* puiSrc = (const unsigned int*)pucSrc;

    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, R);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, G);
            *(pucDest++) = UnpackPixel(puiSrc, kSrcBits, B);
            *(pucDest++) = 255;

            puiSrc++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertBUMPLUMA32To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int, 
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    unsigned int* puiDW = (unsigned int*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucU = *(pucSrc++);
            unsigned char ucV = *(pucSrc++);
            unsigned char ucL = *(pucSrc++);
            pucSrc++;

            *puiDW = (unsigned int)(
                PackPixel(ucU, kDestBits, R) |
                PackPixel(ucV, kDestBits, G) |
                PackPixel(ucL, kDestBits, A));

            puiDW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertBUMPLUMA32To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int,
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    unsigned short* puiW = (unsigned short*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucU = *(pucSrc++);
            unsigned char ucV = *(pucSrc++);
            unsigned char ucL = *(pucSrc++);
            pucSrc++;

            *puiW = (unsigned short)(
                PackPixel(ucU, kDestBits, R) |
                PackPixel(ucV, kDestBits, G) |
                PackPixel(ucL, kDestBits, A));

            puiW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertBUMP16To32(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int,
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    unsigned int* puiDW = (unsigned int*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucU = *(pucSrc++);
            unsigned char ucV = *(pucSrc++);

            *puiDW = (unsigned int)(
                PackPixel(ucU, kDestBits, R) |
                PackPixel(ucV, kDestBits, G) |
                PackPixel(0xffffffff, kDestBits, A));

            puiDW++;
        }
    }
}
//---------------------------------------------------------------------------
void NiDevImageConverter::ConvertBUMP16To16(unsigned int uiWidth, 
    unsigned int uiHeight, unsigned int,
    unsigned char* pucDest, const NiPalette*, 
    const PixelBits& kDestBits, const unsigned char* pucSrc, 
    const PixelBits&) 
{
    unsigned short* puiW = (unsigned short*)pucDest;
    for (unsigned int y = 0; y < uiHeight; y++)
    {
        for (unsigned int x = 0; x < uiWidth; x++)
        {
            unsigned char ucU = *(pucSrc++);
            unsigned char ucV = *(pucSrc++);

            *puiW = (unsigned short)(
                PackPixel(ucU, kDestBits, R) |
                PackPixel(ucV, kDestBits, G) |
                PackPixel(0xffffffff, kDestBits, A));

            puiW++;
        }
    }
}
//---------------------------------------------------------------------------
// Returns true if the source and destination formats are identical except
// that they have opposite endianness.
bool NiDevImageConverter::CanEndianSwap(const NiPixelFormat& kSrcFmt,
    const NiPixelFormat& kDstFmt)
{
    if (kSrcFmt.GetLittleEndian() == kDstFmt.GetLittleEndian())
    {
        return false;
    }

    return kSrcFmt.FunctionallyIdentical(kDstFmt, (unsigned int)
        NiPixelFormat::CMP_DONTCARE_ENDIAN);
}
//---------------------------------------------------------------------------
