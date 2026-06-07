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

#include "NiPNGReader.h"
#include "NiBinaryStream.h"
#include "NiPalette.h"
#include "NiPixelData.h"

#include "png.h"

#include <NiFile.h>
#include <NiSystem.h>

#ifndef png_jmpbuf
#define png_jmpbuf(pPng) ((pPng)->jmpbuf)
#endif

#define BIG_ENDIAN_UNSIGNED_INT( buf ) \
            ((unsigned int)(*((unsigned char*)&buf)) << 24) + \
            ((unsigned int)(*((unsigned char*)&buf + 1)) << 16) + \
            ((unsigned int)(*((unsigned char*)&buf + 2)) << 8) + \
             (unsigned int)(*((unsigned char*)&buf + 3))


void ConvertToStrCode( unsigned int uiType, unsigned char *ucType)
{
    ucType[0] = (unsigned char)(uiType & 0x000000ff); 
    ucType[1] = (unsigned char)((uiType & 0x0000ff00) >> 8);
    ucType[2] = (unsigned char)((uiType & 0x00ff0000) >> 16);
    ucType[3] = (unsigned char)((uiType & 0xff000000) >> 24);
    ucType[4] = NULL;
}

#define CONVERT_TO_STR_CODE( uiType, acType ) \
    ConvertToStrCode( uiType, (unsigned char*)acType )

//---------------------------------------------------------------------------
NiPNGReader::NiPNGReader()
{
    // Initialize all data, although most of this will be reinitialized in
    // the reading code (ReadHeader)
    m_ucIDLength = 0;
    m_ucImgType = 0;
    m_usMinPalIndex = 0;
    m_usPalLength = 0;
    m_ucCoSize = 0;
    m_usXOrigin = 0;
    m_usYOrigin = 0;
    m_usWidth = 0;
    m_usHeight = 0;
    m_ucPixelSize = 0;
    m_ucAttBits = 0;

    m_uiBytesPerPixel = 0;

    m_bAlpha = false;
    m_bColormapped = false;
    m_bFlipVert = false;
    m_kFormat = NiPixelFormat::RGB24;

    m_uiRawDataMax = 0;
    m_pucRawData = NULL;
    m_uiColorMapMax = 0;
    m_pkColorMap = NULL;

    m_pfnUnpacker = NULL;
}
//---------------------------------------------------------------------------
NiPNGReader::~NiPNGReader()
{
    NiFree(m_pucRawData);
    NiDelete[] m_pkColorMap;
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack4BitSourceRowPal(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 4-bit PAL
    // We divide the width in half to reflect the 2 pixels per byte.
    for (unsigned int i = 0; i < (unsigned int)(m_usWidth >> 1); i++)
    {
        unsigned char ucEntryValue = (unsigned char)(pucSrc[0] - m_usMinPalIndex);
        pucSrc++;

        *pucDest++ = ucEntryValue;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack8BitSourceRowPal(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 8-bit PAL
    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        unsigned char ucEntryValue = (unsigned char)(pucSrc[0] - m_usMinPalIndex);
        pucSrc++;

        *pucDest++ = ucEntryValue;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack8BitSourceRowGray(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 24-bit RGB
    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        // decode the 8-bit grayscale data into the destination 
        // R, G, and B values
        pucDest[0] = pucDest[1] = pucDest[2] = *pucSrc; 

        pucDest += 3;
        pucSrc++;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack16BitSourceRowRGB(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 24-bit RGB

    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        // decode the 16-bit data into the destination R, G and B values
        // We cannot just treat each pixel as a short, as this would not
        // work across platforms
        *(pucDest++) = ( pucSrc[1] & 0x7C ) << 1;
        *(pucDest++) = (( pucSrc[1] & 0x03 ) << 6 ) +
            (( pucSrc[0] & 0xE0 ) >> 2 );
        *(pucDest++) = ( pucSrc[0] & 0x1F ) << 3;

        pucSrc += 2;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack16BitSourceRowIndexed(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 24-bit RGB

    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        // decode the 16-bit data into the destination
        const NiPalette::PaletteEntry& kEntry 
            = m_pkColorMap[(pucSrc[1]<<8) + pucSrc[0] - m_usMinPalIndex];

        *(pucDest++) = kEntry.m_ucRed; 
        *(pucDest++) = kEntry.m_ucGreen; 
        *(pucDest++) = kEntry.m_ucBlue; 

        pucSrc += 2;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack16BitSourceRowIndexedAlpha(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 24-bit RGBA

    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        // decode the 16-bit data into the destination
        const NiPalette::PaletteEntry& kEntry 
            = m_pkColorMap[(pucSrc[1]<<8) + pucSrc[0] - m_usMinPalIndex];

        *(pucDest++) = kEntry.m_ucRed; 
        *(pucDest++) = kEntry.m_ucGreen; 
        *(pucDest++) = kEntry.m_ucBlue; 
        *(pucDest++) = kEntry.m_ucAlpha; 

        pucSrc += 2;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack24BitSourceRow(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 24-bit RGB

    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        *(pucDest++) = *(pucSrc+2);
        *(pucDest++) = *(pucSrc+1);
        *(pucDest++) = *(pucSrc);

        pucSrc += 3;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack32BitFrom24BitSourceRow(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 32-bit RGBA

    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        *(pucDest++) = *(pucSrc+0); // Red
        *(pucDest++) = *(pucSrc+1); // Green
        *(pucDest++) = *(pucSrc+2); // Blue
        *(pucDest++) = 255; //*(pucSrc+3);

        pucSrc += 3;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::Unpack32BitFrom32BitSourceRow(unsigned char* pucSrc, 
    unsigned char* pucDest)
{
    // Assume destination is 32-bit RGBA

    for (unsigned int i = 0; i < m_usWidth; i++)
    {
        *(pucDest++) = *(pucSrc+0); // Red
        *(pucDest++) = *(pucSrc+1); // Green
        *(pucDest++) = *(pucSrc+2); // Blue
        *(pucDest++) = *(pucSrc+3);

        pucSrc += 4;
    }
}
//---------------------------------------------------------------------------
void NiPNGReader::GetColormap(NiFile& kIst)
{
    if (m_uiColorMapMax < m_usPalLength)
    {
        m_uiColorMapMax = m_usPalLength;

        NiDelete[] m_pkColorMap;
        m_pkColorMap = NiNew NiPalette::PaletteEntry[m_uiColorMapMax];
    }

    if (m_uiRawDataMax < (unsigned int)(m_usPalLength << 2))
    {
        m_uiRawDataMax = m_usPalLength << 2;

        NiFree(m_pucRawData);
        m_pucRawData = NiAlloc(unsigned char, m_uiRawDataMax);
    }

    NiPalette::PaletteEntry* pkDest = m_pkColorMap;

    // Read appropriate number of bytes, break into rgb & put in map.
    switch (m_ucCoSize)
    {
        case 8:             // Greyscale, read and triplicate.
        {
            unsigned char* pucRaw = m_pucRawData;

            NiStreamLoadBinary(kIst, (char *)m_pucRawData, 
                m_usPalLength);

            for (unsigned int i = 0; i < m_usPalLength; i++)
            {
                pkDest->m_ucRed = pkDest->m_ucGreen = pkDest->m_ucBlue
                    = *pucRaw;
                pkDest->m_ucAlpha = 255;

                pkDest++;
                pucRaw++;
            }
        }
        break;

        case 15:            // 5 bits each of red green and blue.
        case 16:            // Watch for byte order.
        {
            unsigned char* pucRaw = m_pucRawData;

            // cannot treat these directly as unsigned shorts, or else
            // the code will not be little/big endian compatible
            NiStreamLoadBinary(kIst, (char *)m_pucRawData, 
                m_usPalLength*2);

            for (unsigned int i = 0; i < m_usPalLength; i++)
            {
                pkDest->m_ucRed = ( pucRaw[1] & 0x7C ) << 1;
                pkDest->m_ucGreen = (( pucRaw[1] & 0x03 ) << 6 ) 
                    + (( pucRaw[0] & 0xE0 ) >> 2 );
                pkDest->m_ucBlue = ( pucRaw[0] & 0x1F ) << 3;
                pkDest->m_ucAlpha = 255;

                pkDest++;
                pucRaw += 2;
            }
        }
        break;

        case 24:            // 8 bits each of blue green and red.
        {
            unsigned char* pucRaw = m_pucRawData;

            NiStreamLoadBinary(kIst, (char*)m_pucRawData, 
                m_usPalLength*3);

            for (unsigned int i = 0; i < m_usPalLength; i++)
            {
                pkDest->m_ucBlue = *(pucRaw++);
                pkDest->m_ucGreen = *(pucRaw++);
                pkDest->m_ucRed = *(pucRaw++);
                pkDest->m_ucAlpha = 255;

                pkDest++;
            }
        }
        break;

        case 32:
        {
            unsigned char* pucRaw = m_pucRawData;

            NiStreamLoadBinary(kIst, (char *)m_pucRawData, 
                m_usPalLength*4);

            for (unsigned int i = 0; i < m_usPalLength; i++)
            {
                pkDest->m_ucBlue = *(pucRaw++);
                pkDest->m_ucGreen = *(pucRaw++);
                pkDest->m_ucRed = *(pucRaw++);
                pkDest->m_ucAlpha = *(pucRaw++);

                pkDest++;
            }
        }
        break;

        default:
            break;
    };
}
//---------------------------------------------------------------------------
bool NiPNGReader::ReadHeader(NiFile& kIst, unsigned int& uiWidth, 
    unsigned int& uiHeight, NiPixelFormat& kFormat, bool& bMipmap,
    unsigned int& uiNumFaces)
{
    uiNumFaces = 1;
    NiStreamLoadBinary(kIst, m_aucHeader[0]); // 137 = \211 
    NiStreamLoadBinary(kIst, m_aucHeader[1]); //  80 = 'P'
    NiStreamLoadBinary(kIst, m_aucHeader[2]); //  78 = 'N'
    NiStreamLoadBinary(kIst, m_aucHeader[3]); //  71 = 'G'
    NiStreamLoadBinary(kIst, m_aucHeader[4]); //  13 = \r
    NiStreamLoadBinary(kIst, m_aucHeader[5]); //  10 = \n
    NiStreamLoadBinary(kIst, m_aucHeader[6]); //  26 = \032
    NiStreamLoadBinary(kIst, m_aucHeader[7]); //  10 = \n

    // Keep loading chunks until no more chunks or
    // until we find a data chunk
    // We'll ignore many chunks such as gAMA, cHRM, iCPP, etc.

    NiStreamLoadBinary(kIst, m_uiLength); // Chunk length
    m_uiLength = BIG_ENDIAN_UNSIGNED_INT( m_uiLength );
    NiStreamLoadBinary(kIst, m_uiType);   // Chunk type
    CONVERT_TO_STR_CODE( m_uiType, m_acType );

    unsigned char uCM,uFM,uIM;
    unsigned char ucSampleSize = 0;
    unsigned int uiCRC;

    if (strcmp(m_acType,"IHDR")==0)
    {
        NIASSERT( m_uiLength == 13 ); // IHDR lengths are 13

        NiStreamLoadBinary(kIst, uiWidth);
        uiWidth = BIG_ENDIAN_UNSIGNED_INT( uiWidth );

        NiStreamLoadBinary(kIst, uiHeight);
        uiHeight = BIG_ENDIAN_UNSIGNED_INT( uiHeight );

        NiStreamLoadBinary(kIst, ucSampleSize);

        NiStreamLoadBinary(kIst, m_ucImgType);
        NiStreamLoadBinary(kIst, uCM);
        NiStreamLoadBinary(kIst, uFM);
        NiStreamLoadBinary(kIst, uIM);

        NiStreamLoadBinary(kIst, uiCRC);

        // Width:              4 bytes
        // Height:             4 bytes
        // Bit depth:          1 byte
        // Color type:         1 byte
        // Compression method: 1 byte
        // Filter method:      1 byte
        // Interlace method:   1 byte
    }

    switch(m_ucImgType)
    {
        
        case 0: // 1,2,4,8,16  Each pixel is a grayscale sample.
        case 2: // 8,16        Each pixel is an R,G,B triple.
            m_ucPixelSize = ucSampleSize * 3;
            m_bColormapped = false;
            break;
        case 3: // 1,2,4,8     Each pixel is a palette index;
                //              a PLTE chunk must appear.
            m_ucPixelSize = ucSampleSize;
            m_bColormapped = true;
            break;
        case 4: // 8,16        Each pixel is a grayscale sample,
                //              followed by an alpha sample.
            m_ucPixelSize = ucSampleSize * 4;
            m_bColormapped = false;
            break;
        case 6: // 8,16 bits   R,G,B,A
            m_ucPixelSize = ucSampleSize * 4;
            m_bAlpha = true;
            m_bColormapped = false;
            break;
        default: // UNKNOWN
            NIASSERT(0);
            break;
    }

    switch (m_ucPixelSize)
    {
        case 4:
            m_uiBytesPerPixel = 0;
            m_pfnUnpacker = &NiPNGReader::Unpack4BitSourceRowPal;
            break;
        case 8:
            m_uiBytesPerPixel = 1;
            m_pfnUnpacker = &NiPNGReader::Unpack8BitSourceRowPal;
            break;
        case 15:
        case 16:
            NIASSERT(0);
            m_uiBytesPerPixel = 2;
            break;
        case 24:
            m_uiBytesPerPixel = 3;
            m_pfnUnpacker = &NiPNGReader::Unpack32BitFrom24BitSourceRow;
            break;
        case 32:
            m_uiBytesPerPixel = 4;
            m_pfnUnpacker = &NiPNGReader::Unpack32BitFrom32BitSourceRow;
            break;
    };

    m_bAlpha = true; // always true in this version.

    if (m_bColormapped)
    {
        if (ucSampleSize == 4)
            m_kFormat = NiPixelFormat::PALA4;
        else
        {
            NIASSERT(ucSampleSize == 8);

            m_kFormat = NiPixelFormat::PALA8;        
        }
    }
    else
    {
        m_kFormat = NiPixelFormat::RGBA32;
    }

    m_usWidth = (unsigned short)uiWidth;
    m_usHeight = (unsigned short)uiHeight;
    kFormat = m_kFormat;
    bMipmap = false;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPNGReader::CanReadFile(const char* pcFileExtension) const
{
    if (!NiStricmp(pcFileExtension, ".png")) 
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiPixelData* NiPNGReader::ReadFile(NiFile &kIst, NiPixelData* pkOptDest)
{
    bool bMipmap;
    unsigned int uiW, uiH;
    NiPixelFormat kFmt;
    unsigned int uiNumFaces;

    if (!ReadHeader(kIst, uiW, uiH, kFmt, bMipmap, uiNumFaces))
        return NULL;

    return ReadBody(kIst, pkOptDest);
}
//---------------------------------------------------------------------------
static void NiPNGReaderUserErrorCB(png_structp,
    png_const_charp)
{
    return;
}
//---------------------------------------------------------------------------
static void NiPNGReaderUserWarningCB(png_structp, 
    png_const_charp)
{
    return;
}
//---------------------------------------------------------------------------
void NiPNGReaderReadCB(png_structp png_ptr, png_bytep data, 
    png_uint_32 length)
{
    void* read_io_ptr = png_get_io_ptr(png_ptr);

    NiFile* pFile = (NiFile*)read_io_ptr;

    pFile->Read(data, length);

    return;
}
//---------------------------------------------------------------------------
NiPixelData*  NiPNGReader::ReadBody(NiFile &kIst, NiPixelData* pkOptDest)
{
    // ----- SETUP -----
    // Reset kIst since PNG docs say not to read
    // more than 8 bytes and we have.
    kIst.Seek( 0, SEEK_SET );

    png_structp pPng = NULL;
    png_infop pInfo = NULL;

    // No Error, Warning handlers supplied
    int iVar; // Dummy variable for passing into png func.
    pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
        &iVar,
        NiPNGReaderUserErrorCB, 
        NiPNGReaderUserWarningCB );
    

    if (pPng == NULL)
        return false;

    pInfo = png_create_info_struct(pPng);

    if (pInfo == NULL)
    {
        png_destroy_read_struct(&pPng, NULL, NULL);
        return false;
    }
#if defined(WIN32) || defined(_XENON)
#pragma warning(disable:4611)
#endif
    if (setjmp(png_jmpbuf((pPng))))
    {
        png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
        return false;
    }
#if defined(WIN32) || defined(_XENON)
#pragma warning(default:4611)
#endif

    // ----- READ PNG -----

    png_set_read_fn( pPng, (void*)&kIst, (png_rw_ptr)NiPNGReaderReadCB );

    png_set_sig_bytes(pPng, 0);

    png_read_png(pPng, pInfo, NULL/*pTransforms*/, NULL);

    // ----- TRANSLATE -----
    if (pInfo->palette)
    {
        m_bColormapped = true;

        m_usPalLength = pInfo->num_palette;

        if (m_uiColorMapMax < pInfo->num_palette)
        {
            m_uiColorMapMax = pInfo->num_palette;
            NiDelete [] m_pkColorMap;
            m_pkColorMap = NiNew NiPalette::PaletteEntry[m_usPalLength];
        }

        NiPalette::PaletteEntry* pkDest = m_pkColorMap;

        for (unsigned int i = 0; i < m_usPalLength; i++)
        {
            pkDest->m_ucBlue = pInfo->palette[i].blue; 
            pkDest->m_ucGreen = pInfo->palette[i].green;
            pkDest->m_ucRed = pInfo->palette[i].red; 
            pkDest->m_ucAlpha = 255;          
            pkDest++;
        }

        pkDest = m_pkColorMap;
        if (pInfo->trans)
        {
            // Fill in the correct alpha values if alpha is given.
            for (unsigned int i = 0; i < pInfo->num_trans; i++)
            {
                pkDest->m_ucAlpha = pInfo->trans[i];
                pkDest++;
            }
        }
    }

    NiPixelData* pkDest;

    if (pkOptDest && (pkOptDest->GetWidth() == m_usWidth) && 
        (pkOptDest->GetHeight() == m_usHeight) &&
        (pkOptDest->GetPixelFormat() == m_kFormat))
    {
        pkDest = pkOptDest;
    }
    else
    {
         pkDest = NiNew NiPixelData(m_usWidth, m_usHeight, m_kFormat);
    }

    // If already have a color map, assign it.
    if (m_bColormapped)
    {
        pkDest->SetPalette(NiNew NiPalette(m_bAlpha, m_usPalLength, 
            m_pkColorMap));
    }

    unsigned char* pucDestPixels = pkDest->GetPixels();

    int iRowOffset = pkDest->GetWidth() * pkDest->GetPixelStride();

    if (iRowOffset == 0)
    {
        // iRowOffset is 0 in 4 bit case...
        // Handling this oddity here.
        iRowOffset = pkDest->GetWidth() >> 1;
    }

    if (m_bFlipVert)
    {
        pucDestPixels += iRowOffset * (m_usHeight - 1);
        iRowOffset = -iRowOffset;
    }

    for (unsigned int uiRow = 0; uiRow < m_usHeight; uiRow++)
    {
        // Load a row of raw pixels into temporary storage directly 
        // from the image file
        m_pucRawData = pInfo->row_pointers[uiRow];

        // Unpack the 8,15,16,24, or 32 bit indexed or true color pixels
        (this->*m_pfnUnpacker)(m_pucRawData, pucDestPixels);

        pucDestPixels += iRowOffset;
    }

    m_pucRawData = NULL;

    png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);

    return pkDest;
}
//---------------------------------------------------------------------------
