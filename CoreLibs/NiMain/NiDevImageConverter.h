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
#ifndef NIDEVIMAGECONVERTER_H
#define NIDEVIMAGECONVERTER_H

#include "NiImageConverter.h"
#include "NiTexture.h"
#include "NiTextureCodec.h"
#include "NiTPointerList.h"

class NiImageReader;
class NiPalette;

class NIMAIN_ENTRY NiDevImageConverter : public NiImageConverter
{
public:
    NiDeclareRTTI;

    NiDevImageConverter();
    ~NiDevImageConverter();

    virtual bool CanReadImageFile(const char* pcFilename) const;

    virtual bool ReadImageFileInfo(const char* pcFilename,
        NiPixelFormat &kFmt, bool &bMipmap, unsigned int &uiWidth,
        unsigned int &uiHeight, unsigned int& uiFaces);

    virtual NiPixelData* ReadImageFile(const char* pcFilename,
        NiPixelData* pkOptDest);

    virtual bool CanConvertPixelData(const NiPixelFormat& kSrcFmt, 
        const NiPixelFormat& kDestFmt) const;

    virtual NiPixelData* ConvertPixelData(const NiPixelData& kSrc, 
        const NiPixelFormat& kDestFmt, NiPixelData* pkOptDest, bool bMipmap);

    virtual bool IsASupportedMipmapGenerationFormat(const NiPixelFormat&
        kSrcFmt) const;

    virtual bool IsAnAlphaSupportedMipmapGenerationFormat(const 
        NiPixelFormat& kSrcFmt) const;

    virtual bool IsANonAlphaSupportedMipmapGenerationFormat(const 
        NiPixelFormat& kSrcFmt) const;

    virtual NiPixelData* GenerateMipmapLevels(const NiPixelData* pkSrc,
        NiPixelData* pkOptDest);

    // All image files must be of same format.  Base image (largest) must
    // be specified first with each additional image being halved in size.
    // Zero will be returned otherwise.
    virtual NiPixelData* GenerateMipmapPyramidFromFiles(
        const char* apcFilenames[], unsigned int uiNumLevels,
        const NiPixelData* pkSrc = NULL);

    virtual bool ConvertPixelDataFormat(NiPixelData& kDest, 
        const NiPixelData& kSrc, int iMipmapLevel = 0);

    virtual NiPixelData* GenerateBumpMap(const NiPixelData& kSrc, 
        const NiPixelFormat& kDestFmt);

    virtual void AddReader(NiImageReader* pkReader);

    /*** begin Emergent internal use only ***/
    enum Platform 
    {
        ANY,            // generic PAL8, RGB24, and RGBA32 images
        PLAYSTATION2,
        XBOX,
        WII,
        XENON,
        PLAYSTATION3,
        DX9,
        D3D10,
        NUM_PLATFORMS
    };
    static bool IsLittleEndian(Platform ePlatform);

    virtual const NiPixelFormat* FindClosestPixelFormat(Platform ePlatform,
        const NiTexture::FormatPrefs& kFmtPrefs, const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDesiredFmt) const;

    virtual NiPixelData* ConvertForPlatform(Platform ePlatform, 
        const NiTexture::FormatPrefs& kPrefs, const NiPixelData* pkSrc,
        const NiPixelFormat& kDesiredFormat);

    virtual NiTPointerList<NiImageReader*>& GetImageReaderList();
    /*** begin Emergent internal use only ***/

    class NIMAIN_ENTRY PixelBits
    {
    public:
        inline PixelBits() {/* */};
        PixelBits(const NiPixelFormat& kFmt);
        unsigned int m_uiRM, m_uiGM, m_uiBM, m_uiAM;
        unsigned char m_ucRS, m_ucGS, m_ucBS, m_ucAS;
        unsigned char m_ucRQ, m_ucGQ, m_ucBQ, m_ucAQ;
    };

    typedef void (*ConverterFunc)
        (unsigned int uiWidth, unsigned int uiHeight, 
        unsigned int uiDestSize,
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc, 
        const PixelBits& kSrcBits); 

    static void ConvertRGBA16ToBGRA16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc, 
        const PixelBits& kSrcBits); 
    static void ConvertBGRA16ToRGBA16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits);
    static void ConvertRGB16To16(unsigned int uiWidth, 
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits);
    static void ConvertRGBA16ToBGRA4443(unsigned int uiWidth, 
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits);
    static void ConvertRGBA32ToBGRA4443(unsigned int uiWidth, 
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc, 
        const PixelBits& kSrcBits);
    static void ConvertPAL4To24(unsigned int uiWidth, 
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits);
    static void ConvertPAL4To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal,
        const PixelBits& kDestBits, const unsigned char* pucSrc, 
        const PixelBits& kSrcBits); 
    static void ConvertPALA4To24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize,
        unsigned char* pucDest, const NiPalette* pkSrcPal,
        const PixelBits& kDestBits, const unsigned char* pucSrc, 
        const PixelBits& kSrcBits);
    static void ConvertPALA4To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize,
        unsigned char* pucDest, const NiPalette* pkSrcPal,
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits); 

    static void ConvertPAL8To16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, 
        unsigned char* pucDest, const NiPalette* pkSrcPal, 
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits); 
    static void ConvertPAL8To24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize,
        unsigned char* pucDest, const NiPalette* pkSrcPal,
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits); 
    static void ConvertPAL8To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize,
        unsigned char* pucDest, const NiPalette* pkSrcPal,
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits); 
    static void ConvertPALA8To16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize,
        unsigned char* pucDest, const NiPalette* pkSrcPal,
        const PixelBits& kDestBits, const unsigned char* pucSrc,
        const PixelBits& kSrcBits); 
    static void ConvertPALA8To24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertPALA8To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGB24To16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGB24ToBGR565_BE(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGB24To24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGB24To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGBA32To16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGBA32To24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits); 
    static void ConvertRGBA32To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    
    static void Convert16ToRGB24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert16AlphaToRGBA32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert16NoAlphaToRGBA32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert24ToRGB24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert24ToRGBA32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert32ToRGB24(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert32AlphaToRGBA32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void Convert32NoAlphaToRGBA32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void ConvertBUMPLUMA32To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void ConvertBUMPLUMA32To16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void ConvertBUMP16To32(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);
    static void ConvertBUMP16To16(unsigned int uiWidth,
        unsigned int uiHeight, unsigned int uiDestSize, unsigned char* pucDest,
        const NiPalette* pkSrcPal, const PixelBits& kDestBits,
        const unsigned char* pucSrc, const PixelBits& kSrcBits);

    static void ConvertBUMPLUMA32ToBUMPLUMA32_WII(NiPixelData* pkDest);

    static unsigned char UnpackRedChannel(unsigned int uiSrc,
        PixelBits& kBits);
    static unsigned char UnpackGreenChannel(unsigned int uiSrc,
        PixelBits& kBits);
    static unsigned char UnpackBlueChannel(unsigned int uiSrc,
        PixelBits& kBits);
    static unsigned char UnpackAlphaChannel(unsigned int uiSrc,
        PixelBits& kBits);

    /*** end Emergent internal use only ***/

protected:
    virtual const NiPixelFormat* FindClosestPixelFormatD3D10(
        const NiTexture::FormatPrefs& kFmtPrefs, const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDesiredFmt) const;

    virtual const NiPixelFormat* FindClosestPixelFormatDX9(
        const NiTexture::FormatPrefs& kFmtPrefs, const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDesiredFmt) const;
    
    virtual const NiPixelFormat* FindClosestPixelFormatXenon(
        const NiTexture::FormatPrefs& kFmtPrefs, const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDesiredFmt) const;
    
    virtual const NiPixelFormat* FindClosestPixelFormatPS3(
        const NiTexture::FormatPrefs& kFmtPrefs, const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDesiredFmt) const;

    virtual const NiPixelFormat* FindClosestPixelFormatWii(
        const NiTexture::FormatPrefs& kFmtPrefs, const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDesiredFmt) const;

    static bool CanEndianSwap(const NiPixelFormat& kSrcFmt,
        const NiPixelFormat& kDstFmt);

    NiTextureCodec m_kCodec;

    NiTPointerList<NiImageReader*> m_kReaders;
};

//---------------------------------------------------------------------------
#include "NiDevImageConverter.inl"
//---------------------------------------------------------------------------
#endif
