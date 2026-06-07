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

#include "NiBinaryStream.h"
#include "NiDevImageConverter.h"
#include "NiPixelData.h"
#include "NiDDSReader.h"
#include "NiTextureCodec.h"
#include <NiFile.h>
#include <NiSystem.h>
#include <NiSourceCubeMap.h>
#if defined(_XENON)
    #include <XGraphics.h>
#endif  //#if defined(_XENON)

//---------------------------------------------------------------------------
NiDDSReader::NiDDSReader()
{
    m_uiHeight = 0;
    m_uiWidth = 0;

    m_kOriginalFormat = NiPixelFormat::DXT1;
}
//---------------------------------------------------------------------------
NiDDSReader::~NiDDSReader()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiDDSReader::ReadHeader(NiFile& kIst, unsigned int& uiWidth, 
    unsigned int& uiHeight, NiPixelFormat& kFormat, bool& bMipmap, 
    unsigned int& uiFaces)
{
    // Since the reader is usually owned by a static image converter class,
    // we lock for safety.
    m_kReadCriticalSection.Lock();

    unsigned int uiTemp;

    unsigned int uiMagicNumber;
    NiStreamLoadBinary(kIst, uiMagicNumber);
    if (uiMagicNumber != NI_DDS) // "DDS" is the header for all DDS files
    {
        // It's possible this is a still valid file, but just has the
        // reverse endianness.
        unsigned int uiSwapped = uiMagicNumber;
        NiEndian::Swap32((char*)&uiSwapped);
        if (uiSwapped == NI_DDS)
        {
            // swap and continue loading
            kIst.SetEndianSwap(true);
        }
        else
        {
            m_kReadCriticalSection.Unlock();
            return false;
        }
    }

    // surface description header
    unsigned int uiSize;
    NiStreamLoadBinary(kIst, uiSize); // size
    if (uiSize != 124)
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    // Value validity flags
    unsigned int uiFlags;
    NiStreamLoadBinary(kIst, uiFlags); // flags

    // We ignore the following flags:
    // DDSD_COMP_ALPHABITDEPTH 
    // DDSD_CKDESTBLT 
    // DDSD_CKDESTOVERLAY 
    // DDSD_CKSRCBLT 
    // DDSD_CKSRCOVERLAY 
    // DDSD_LPSURFACE ???
    // DDSD_REFRESHRATE 
    // DDSD_TEXTURESTAGE 
    // DDSD_ZBUFFERBITDEPTH 
    // DDSD_PITCH

    // We need the following flags
    // DDSD_CAPS 
    // DDSD_HEIGHT 
    // DDSD_PIXELFORMAT 
    // DDSD_WIDTH
    const unsigned int uiRequiredFlags = NI_DDSD_CAPS | NI_DDSD_HEIGHT | 
        NI_DDSD_PIXELFORMAT | NI_DDSD_WIDTH;

    if ((uiFlags & uiRequiredFlags) != uiRequiredFlags)
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    // We can handle the following flags
    // DDSD_MIPMAPCOUNT 
    
    // We do not want to see the following flags
    // DDSD_BACKBUFFERCOUNT - unless the count is 0

    NiStreamLoadBinary(kIst, m_uiHeight);
    NiStreamLoadBinary(kIst, m_uiWidth);

    // Data size
    NiStreamLoadBinary(kIst, uiTemp);

    // dwDepth
    NiStreamLoadBinary(kIst, uiTemp);

    // Cannot handle volume textures in file
    if ((uiFlags & NI_DDSD_DEPTH) && uiTemp)
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    // dwMipMapCount
    unsigned int uiMipmapCount;
    NiStreamLoadBinary(kIst, uiMipmapCount);

    // dwReserved
    kIst.Seek(4*11,NiFile::ms_iSeekCur);

    ////////////////
    // pixel format
    NiStreamLoadBinary(kIst, uiTemp); // dwSize

    unsigned int uiFormatFlags;
    NiStreamLoadBinary(kIst, uiFormatFlags); // dwFlags

    unsigned int uiCompressionCode;
    NiStreamLoadBinary(kIst, uiCompressionCode); // dwFourCC

    if ((uiFormatFlags & NI_FOURCC))
    {
        // can only handle DXT1, DXT3, or DXT5 files for now
        switch (uiCompressionCode)
        {
            case NI_DXTC1:
                m_kOriginalFormat = NiPixelFormat::DXT1;
                break;
            case NI_DXTC3:
                m_kOriginalFormat = NiPixelFormat::DXT3;
                break;
            case NI_DXTC5:
                m_kOriginalFormat = NiPixelFormat::DXT5;
                break;
            case NI_R16:
                m_kOriginalFormat = NiPixelFormat::R16;
                break;
            case NI_RG32:
                m_kOriginalFormat = NiPixelFormat::RG32;
                break;
            case NI_RGBA64:
                m_kOriginalFormat = NiPixelFormat::RGBA64;
                break;
            case NI_R32:
                m_kOriginalFormat = NiPixelFormat::R32;
                break;
            case NI_RG64:
                m_kOriginalFormat = NiPixelFormat::RG64;
                break;
            case NI_RGBA128:
                m_kOriginalFormat = NiPixelFormat::RGBA128;
                break;
            default:
                m_kReadCriticalSection.Unlock();
                return false;
                break;
        }

        kFormat = m_kOriginalFormat;
        // Ignore the following fields for compressed textures:
        //  dwRGBBitCount
        //  dwRBitMask
        //  dwGBitMask
        //  dwBBitMask
        //  dwRGBAlphaBitMask

        kIst.Seek(4*5,NiFile::ms_iSeekCur);
    }
    else if ((uiFormatFlags & NI_RGB))
    {
        unsigned int uiBitsPerPixel;
        NiStreamLoadBinary(kIst, uiBitsPerPixel); // dwRGBBitCount

        unsigned int uiRedMask;
        NiStreamLoadBinary(kIst, uiRedMask);    // dwRBitMask
        
        unsigned int uiGreenMask;
        NiStreamLoadBinary(kIst, uiGreenMask);    // dwGBitMask
        
        unsigned int uiBlueMask;
        NiStreamLoadBinary(kIst, uiBlueMask);    // dwBBitMask
        
        unsigned int uiAlphaMask;
        NiStreamLoadBinary(kIst, uiAlphaMask);    // dwRGBAlphaBitMask
        
        if (!(uiFormatFlags & NI_ALPHAPIXELS))
            uiAlphaMask = 0x00000000;

        if (!ValidateRGBABitmasks(uiRedMask, uiGreenMask, 
                uiBlueMask, uiAlphaMask, uiBitsPerPixel))
        {
            m_kReadCriticalSection.Unlock();
            return false;
        }

        kFormat = NiPixelFormat::CreateFromRGBAMasks(
            (unsigned char)uiBitsPerPixel, uiRedMask, uiGreenMask, uiBlueMask, 
            uiAlphaMask);

        // We need to tell the pixel format that it is big endian because 
        // we'll be swapping the pixels themselves. Otherwise, we end up 
        // with masks that are non-contiguous and VERY bad things 
        // happen on conversion.
        kFormat.SetLittleEndian(
            NiSystemDesc::GetSystemDesc().IsLittleEndian());

        m_kOriginalFormat = kFormat;
        kFormat = ComputeFinalFormat(kFormat);

    }
    else
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }
    
    
    ///////////////////////
    // surface description

    // dwCaps
    unsigned int uiCaps1;
    NiStreamLoadBinary(kIst, uiCaps1);

    // The following caps are allowed
    // DDSCAPS_COMPLEX - we can handle mipmaps
    // DDSCAPS_TEXTURE - we want to deal with textures only
    // DDSCAPS_MIPMAP - we can handle mipmaps

    if ((uiCaps1 & NI_DDSCAPS_COMPLEX) ||
        (uiCaps1 & NI_DDSCAPS_TEXTURE) ||
        (uiCaps1 & NI_DDSCAPS_MIPMAP))
    {
        // we can support these flags
    }
    else
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    // dwCaps2
    unsigned int uiCaps2;
    NiStreamLoadBinary(kIst, uiCaps2);

    // We cannot handle volume maps
    if ((uiCaps2 & NI_DDSCAPS2_VOLUME) != 0)
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    // We can handle cube maps, but they must all be defined
    if ((uiCaps2 & NI_DDSCAPS2_CUBEMAP) != 0 &&
        (((uiCaps2 & NI_DDSCAPS2_CUBEMAP_POSITIVEX) == 0) || 
         ((uiCaps2 & NI_DDSCAPS2_CUBEMAP_NEGATIVEX) == 0) || 
         ((uiCaps2 & NI_DDSCAPS2_CUBEMAP_POSITIVEY) == 0) || 
         ((uiCaps2 & NI_DDSCAPS2_CUBEMAP_NEGATIVEY) == 0) || 
         ((uiCaps2 & NI_DDSCAPS2_CUBEMAP_POSITIVEZ) == 0) || 
         ((uiCaps2 & NI_DDSCAPS2_CUBEMAP_NEGATIVEZ) == 0)))
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    if ((uiCaps2 & NI_DDSCAPS2_CUBEMAP) != 0)
    {
        uiFaces = 6;
    }
    else
    {
        uiFaces = 1;
    }

    // dwReserved[2]
    // dwReserved2
    kIst.Seek(4*3,NiFile::ms_iSeekCur);

    // file pointer now points to data start

    uiWidth = m_uiWidth;
    uiHeight = m_uiHeight;

    if ((uiCaps1 & NI_DDSCAPS_COMPLEX) && (uiCaps1 & NI_DDSCAPS_MIPMAP) &&
        (uiMipmapCount != 1))
    {
        bMipmap = true;
        m_uiMipmapLevels = uiMipmapCount;
    }
    else
    {
        bMipmap = false;
        m_uiMipmapLevels = 1;
    }

    m_kReadCriticalSection.Unlock();
    return true;
}
//---------------------------------------------------------------------------
bool NiDDSReader::CanReadFile(const char* pcFileExtension) const
{
    if (!NiStricmp(pcFileExtension, ".dds"))
        return true;
    else
        return false;
}

//---------------------------------------------------------------------------
bool NiDDSReader::ValidateRGBABitmasks(unsigned int, 
    unsigned int, unsigned int, 
    unsigned int, unsigned int uiBitsPerPixel)
{
    switch (uiBitsPerPixel)
    {
        case 32:
        case 24:
        case 16:
            {
                return true;
            }
        
    };

    return false;
}


//---------------------------------------------------------------------------
void NiDDSReader::Read24Bit(NiFile& kIst, NiPixelData* pkDest,
    NiPixelFormat kSrcFmt, NiPixelFormat kDestFmt, unsigned int uiMipMapIdx, 
    unsigned int uiFaceIdx)
{
    unsigned char* pucDest = pkDest->GetPixels(uiMipMapIdx, uiFaceIdx);
    unsigned int uiSize = (unsigned int)(pkDest->GetSizeInBytes(uiMipMapIdx, uiFaceIdx));

    NiStreamLoadBinary(kIst, pucDest, uiSize);

    NiDevImageConverter::PixelBits kSrcBits(kSrcFmt);

    unsigned int uiWidth = pkDest->GetWidth(uiMipMapIdx, uiFaceIdx);
    unsigned int uiHeight = pkDest->GetHeight(uiMipMapIdx, uiFaceIdx);
    if (kSrcFmt != kDestFmt)
    {
        if (kSrcBits.m_uiRM == 0x00ff0000)
        {
            const unsigned char* pucSrc = pucDest;
            // must expand data from BGR to RGB
            for (unsigned int y = 0; y < uiHeight; y++)
            {
                for (unsigned int x = 0; x < uiWidth; x++)
                {
                    unsigned char ucR = *(pucSrc+2);
                    unsigned char ucG = *(pucSrc+1);
                    unsigned char ucB = *pucSrc;
                    *(pucDest++) = ucR;
                    *(pucDest++) = ucG;
                    *(pucDest++) = ucB;

                    pucSrc+=3;
                }
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

//---------------------------------------------------------------------------
void NiDDSReader::Read32Bit(NiFile& kIst, NiPixelData* pkDest,
    NiPixelFormat kSrcFmt, NiPixelFormat kDestFmt, unsigned int uiMipMapIdx, 
    unsigned int uiFaceIdx)
{
    unsigned char* pucDest = pkDest->GetPixels(uiMipMapIdx, uiFaceIdx);
    unsigned int uiSize = (unsigned int)(pkDest->GetSizeInBytes(uiMipMapIdx, uiFaceIdx));

    NiStreamLoadBinary(kIst, pucDest, uiSize);

    NiDevImageConverter::PixelBits kSrcBits(kSrcFmt);

    unsigned int uiWidth = pkDest->GetWidth(uiMipMapIdx, uiFaceIdx);
    unsigned int uiHeight = pkDest->GetHeight(uiMipMapIdx, uiFaceIdx);
    if (kSrcFmt != kDestFmt)
    {
        unsigned int* puiSrc = (unsigned int*) pucDest;

        if (!NiSystemDesc::GetSystemDesc().IsLittleEndian())
        {
            NiEndian::Swap32((char*)puiSrc, uiSize / 4);
        }
   
        if (kSrcFmt.GetBits(NiPixelFormat::COMP_ALPHA) != 0)
        {
            for (unsigned int y = 0; y < uiHeight; y++)
            {
                for (unsigned int x = 0; x < uiWidth; x++)
                {
                    unsigned char R = NiDevImageConverter::UnpackRedChannel(
                        *puiSrc, kSrcBits);
                    unsigned char G = NiDevImageConverter::UnpackGreenChannel(
                        *puiSrc, kSrcBits);
                    unsigned char B = NiDevImageConverter::UnpackBlueChannel(
                        *puiSrc, kSrcBits);
                    unsigned char A = NiDevImageConverter::UnpackAlphaChannel(
                        *puiSrc, kSrcBits);
                    *(pucDest++) = R;
                    *(pucDest++) = G;
                    *(pucDest++) = B;
                    *(pucDest++) = A;

                    puiSrc++;
                }
            }
        }
        else
        {
            for (unsigned int y = 0; y < uiHeight; y++)
            {
                for (unsigned int x = 0; x < uiWidth; x++)
                {
                    unsigned char R = NiDevImageConverter::UnpackRedChannel(
                        *puiSrc, kSrcBits);
                    unsigned char G = NiDevImageConverter::UnpackGreenChannel(
                        *puiSrc, kSrcBits);
                    unsigned char B = NiDevImageConverter::UnpackBlueChannel(
                        *puiSrc, kSrcBits);
                    unsigned char A = 255;
                    *(pucDest++) = R;
                    *(pucDest++) = G;
                    *(pucDest++) = B;
                    *(pucDest++) = A;

                    puiSrc++;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiDDSReader::Read16Bit(NiFile& kIst, NiPixelData* pkDest,
    NiPixelFormat kSrcFmt, NiPixelFormat kDestFmt, unsigned int uiMipMapIdx,
    unsigned int uiFaceIdx)
{
    unsigned char pucSrc[512];
    unsigned char* pucDest = pkDest->GetPixels(uiMipMapIdx, uiFaceIdx);
    unsigned int uiSize = pkDest->GetWidth(uiMipMapIdx, uiFaceIdx) * 
        pkDest->GetHeight(uiMipMapIdx, uiFaceIdx) * 2;

    NiDevImageConverter::PixelBits kSrcBits(kSrcFmt);

    for (unsigned int uiBytesWritten = 0; uiBytesWritten < uiSize; )
    {
        unsigned int uiBuffSize = 512;
        if (uiBuffSize > uiSize - uiBytesWritten)
            uiBuffSize = uiSize - uiBytesWritten;

        NiStreamLoadBinary(kIst, pucSrc, uiBuffSize);

        if (!NiSystemDesc::GetSystemDesc().IsLittleEndian())
        {
            NiEndian::Swap16((char*)pucSrc, uiBuffSize / 2);
        }

        if (kDestFmt == NiPixelFormat::RGBA32)
        {
            const unsigned short* pusSrc = (const unsigned short*)pucSrc;
            for (unsigned int y = 0; y < uiBuffSize/2; y++)
            {
                *(pucDest++) = 
                    NiDevImageConverter::UnpackRedChannel(*pusSrc, kSrcBits);
                *(pucDest++) = 
                    NiDevImageConverter::UnpackGreenChannel(*pusSrc, kSrcBits);
                *(pucDest++) = 
                    NiDevImageConverter::UnpackBlueChannel(*pusSrc, kSrcBits);
                *(pucDest++) = 
                    NiDevImageConverter::UnpackAlphaChannel(*pusSrc, kSrcBits);
                pusSrc++;
            }
        }
        else if (kDestFmt == NiPixelFormat::RGB24)
        {
            const unsigned short* pusSrc = (const unsigned short*)pucSrc;
            for (unsigned int y = 0; y < uiBuffSize/2; y++)
            {
                *(pucDest++) = 
                    NiDevImageConverter::UnpackRedChannel(*pusSrc, kSrcBits);
                *(pucDest++) = 
                    NiDevImageConverter::UnpackGreenChannel(*pusSrc, kSrcBits);
                *(pucDest++) = 
                    NiDevImageConverter::UnpackBlueChannel(*pusSrc, kSrcBits);
                pusSrc++;
            }
        }
        else
        {
            NIASSERT(0);
            return;
        }
        uiBytesWritten += uiBuffSize;
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiDDSReader::ReadFile(NiFile& kIst, NiPixelData* pkOptDest)
{
    bool bMipmap;
    unsigned int uiW, uiH, uiMipmapLevels;
    NiPixelFormat kFmt;
    unsigned int uiNumFaces;

    // Since the reader is usually owned by a static image converter class,
    // we lock for safety.
    m_kReadCriticalSection.Lock();

    if (!ReadHeader(kIst, uiW, uiH, kFmt, bMipmap, uiNumFaces))
    {
        m_kReadCriticalSection.Unlock();
        return NULL;
    }

    NiPixelData* pkDest;

    if (pkOptDest && (pkOptDest->GetWidth() == m_uiWidth) && 
        (pkOptDest->GetHeight() == m_uiHeight) &&
        (pkOptDest->GetPixelFormat() == kFmt) &&
        (pkOptDest->GetNumMipmapLevels() == m_uiMipmapLevels) &&
        (pkOptDest->GetNumFaces() == uiNumFaces))
    {
        pkDest = pkOptDest;
    }
    else
    {
        pkDest = NiNew NiPixelData(m_uiWidth, m_uiHeight, kFmt, 
            m_uiMipmapLevels, uiNumFaces);
    }

    uiMipmapLevels = pkDest->GetNumMipmapLevels();

    if (kFmt == m_kOriginalFormat)
    {
        // The data for the mipmap levels follows directly, one mipmap level
        // after another
        unsigned int i;
        
        for (unsigned int uiFaceIdx = 0; uiFaceIdx < uiNumFaces; uiFaceIdx++)
        {
            unsigned int uiTrueFaceIdx = RemapFace(uiFaceIdx);
            for (i = 0; i < uiMipmapLevels; i++)
            {
                unsigned char* pucDest = pkDest->GetPixels(i, uiTrueFaceIdx);
                unsigned int uiSize = (unsigned int)(pkDest->GetSizeInBytes(i, uiTrueFaceIdx));

                NiStreamLoadBinary(kIst, pucDest, uiSize);
            }
        }
    }
    else 
    {
        for (unsigned int uiFaceIdx = 0; uiFaceIdx < uiNumFaces; uiFaceIdx++)
        {
            unsigned int uiTrueFaceIdx = RemapFace(uiFaceIdx);

            for (unsigned int i = 0; i < uiMipmapLevels; i++)
            {
                switch(m_kOriginalFormat.GetBitsPerPixel())
                {
                    case 32:
                        Read32Bit(kIst, pkDest, m_kOriginalFormat, kFmt, i,
                            uiTrueFaceIdx);
                        break;
                    case 16:
                        Read16Bit(kIst, pkDest, m_kOriginalFormat, kFmt, i,
                            uiTrueFaceIdx);
                        break;
                    case 24:
                        Read24Bit(kIst, pkDest, m_kOriginalFormat, kFmt, i,
                            uiTrueFaceIdx);
                        break;
                }
            }
        }
    }

    m_kReadCriticalSection.Unlock();
    return pkDest;
}
//---------------------------------------------------------------------------
unsigned int NiDDSReader::RemapFace(unsigned int uiDDSFace)
{
    switch(uiDDSFace)
    {
        default:
        case 0: // DDS positive x
            return (unsigned int) NiSourceCubeMap::FACE_POS_X;
        case 1: // DDS negative x
            return (unsigned int) NiSourceCubeMap::FACE_NEG_X;
        case 2: // DDS positive y
            return (unsigned int) NiSourceCubeMap::FACE_POS_Y;
        case 3: // DDS negative y
            return (unsigned int) NiSourceCubeMap::FACE_NEG_Y;
        case 4: // DDS positive z
            return (unsigned int) NiSourceCubeMap::FACE_POS_Z;
        case 5: // DDS negative z
            return (unsigned int) NiSourceCubeMap::FACE_NEG_Z;
    }

}
//---------------------------------------------------------------------------
NiPixelFormat NiDDSReader::ComputeFinalFormat(const NiPixelFormat kSrcFormat)
{
    switch(kSrcFormat.GetBitsPerPixel())
    {
        case 32:
            return NiPixelFormat::RGBA32;
            break;
        case 16:
            if (kSrcFormat.GetMask(NiPixelFormat::COMP_ALPHA) != 0)
                return NiPixelFormat::RGBA32;
            else
                return NiPixelFormat::RGB24;
            break;
        case 24:
            return NiPixelFormat::RGB24;
            break;
    }
    return kSrcFormat;
}
//---------------------------------------------------------------------------
    
