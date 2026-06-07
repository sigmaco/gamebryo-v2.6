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

#include "NiPalette.h"
#include "NiPixelData.h"
#include "NiPixelFormat.h"
#include <NiFile.h>
#include <NiSystem.h>

// These are included by copying them from the Windows95
// headers.  This is safe because even if Windows95 changes,
// the file format should not.  This removes dependence upon
// the Windows headers for BMP file importation

//---------------------------------------------------------------------------
NiBMPReader::NiBMPReader()
{
    m_uiWidth = 0;
    m_uiHeight = 0;
    m_kFormat = NiPixelFormat::RGB24;
    m_usBPP = 0;
    m_uiClrUsed = 0;
    m_uiOffBits = 0;
    m_bFlipVert = false;
    m_bRLE = false;
}
//---------------------------------------------------------------------------
NiBMPReader::~NiBMPReader()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiBMPReader::ReadHeader(NiFile& kIst, unsigned int& uiWidth, 
    unsigned int& uiHeight, NiPixelFormat& kFormat, bool& bMipmap, 
    unsigned int& uiFaces)
{
    // Set bmp file as little endian
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kIst.SetEndianSwap(!bPlatformLittle);

    uiFaces = 1;
    // temporary data to be used for dummy reads of file values that the
    // importer can ignore
    int iTempInt;
    unsigned int uiTempUnsignedInt;
    unsigned short usTempUnsignedShort;

    // Bitmap File Header
    //     WORD    bfType;
    //     DWORD   bfSize;
    //     WORD    bfReserved1;
    //     WORD    bfReserved2;
    //     DWORD   bfOffBits;

    NiStreamLoadBinary(kIst, usTempUnsignedShort); // bfType
    // Check the file header for the correct format tag
    if (usTempUnsignedShort != 0x4d42)
        return false;

    // Since the reader is usually owned by a static image converter class,
    // we lock for safety.
    m_kReadCriticalSection.Lock();

    NiStreamLoadBinary(kIst, uiTempUnsignedInt); // bfSize (UNUSED)
    NiStreamLoadBinary(kIst, usTempUnsignedShort); // bfReserved1 (UNUSED)
    NiStreamLoadBinary(kIst, usTempUnsignedShort); // bfReserved2 (UNUSED)
    NiStreamLoadBinary(kIst, m_uiOffBits); // bfOffBits
    
    // Bitmap Info Header
    //     DWORD      biSize;
    //     LONG       biWidth;
    //     LONG       biHeight;
    //     WORD       biPlanes;
    //     WORD       biBitCount;
    //     DWORD      biCompression;
    //     DWORD      biSizeImage;
    //     LONG       biXPelsPerMeter;
    //     LONG       biYPelsPerMeter;
    //     DWORD      biClrUsed;
    //     DWORD      biClrImportant;

    unsigned int uiHeaderSize;
    NiStreamLoadBinary(kIst, uiHeaderSize); // biSize
    
    int iWidth, iHeight;
    NiStreamLoadBinary(kIst, iWidth); // biWidth
    NiStreamLoadBinary(kIst, iHeight); // biHeight
    NiStreamLoadBinary(kIst, usTempUnsignedShort); // biPlanes (UNUSED)
    NiStreamLoadBinary(kIst, m_usBPP); // biBitCount

    unsigned int uiCompression;    
    NiStreamLoadBinary(kIst, uiCompression); // biCompression

    NiStreamLoadBinary(kIst, uiTempUnsignedInt); // biSizeImage
    NiStreamLoadBinary(kIst, iTempInt); // biXPelsPerMeter (UNUSED)
    NiStreamLoadBinary(kIst, iTempInt); // biYPelsPerMeter (UNUSED)
    NiStreamLoadBinary(kIst, m_uiClrUsed); // biClrUsed
    NiStreamLoadBinary(kIst, uiTempUnsignedInt); // biClrImportant (UNUSED)

    // Check the file version by checking the core header size
    if (uiHeaderSize == sizeof(WINDOWS_BMPCOREHEADER_SIZE))
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    // Determine the destination pixel format of the imported image
    // This importer only supports 4, 8, 24, and 32-bit images.
    switch (m_usBPP)
    {
        case 4:
            m_kFormat = NiPixelFormat::PAL4;
            break;
        case 8:
            m_kFormat = NiPixelFormat::PAL8;
            break;
        case 24:
            m_kFormat = NiPixelFormat::RGB24;
            break;
        case 32:
            m_kFormat = NiPixelFormat::RGBA32;
            break;
        default:
            m_kReadCriticalSection.Unlock();
            return false;
    };

    // Importer does not support RLE formats
    if ((uiCompression == WINDOWS_BI_RLE4) || 
        (uiCompression == WINDOWS_BI_RLE8))
    {
        m_kReadCriticalSection.Unlock();
        return false;
    }

    m_uiWidth = (unsigned int)iWidth;

    // If the height is negative, then the image must be flipped vertically
    m_bFlipVert = (iHeight >= 0) ? true : false;

    m_uiHeight = (iHeight > 0) 
        ? (unsigned int)iHeight : (unsigned int)(-iHeight);

    // If the colormap size is not specified, then it is based on the number
    // of bits per pixel
    if ((m_uiClrUsed == 0) && (m_usBPP != 24) && (m_usBPP != 32))
        m_uiClrUsed = 1 << m_usBPP;

    uiWidth = m_uiWidth;
    uiHeight = m_uiHeight;
    kFormat = m_kFormat;
    bMipmap = true;

    m_kReadCriticalSection.Unlock();
    return true;
}
//---------------------------------------------------------------------------
bool NiBMPReader::CanReadFile(const char* pcFileExtension) const
{
    if (!NiStricmp(pcFileExtension, ".bmp"))
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
NiPixelData* NiBMPReader::ReadFile(NiFile& kIst, NiPixelData* pkOptDest)
{
    bool bMipmap;
    unsigned int uiW, uiH;
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

    if (m_kFormat == NiPixelFormat::PAL8 && m_uiClrUsed > 256 ||
        m_kFormat == NiPixelFormat::PAL4 && m_uiClrUsed > 16)
    {
        m_kReadCriticalSection.Unlock();
        return NULL;
    }


    NiPixelData* pkDest;

    if (pkOptDest && (pkOptDest->GetWidth() == m_uiWidth) && 
        (pkOptDest->GetHeight() == m_uiHeight) &&
        (pkOptDest->GetPixelFormat() == m_kFormat))
    {
        pkDest = pkOptDest;
    }
    else
    {
        pkDest = NiNew NiPixelData(m_uiWidth, m_uiHeight, m_kFormat);
    }

    // read in color map (B G R Res) - ignore if not palettized image
    if (m_kFormat == NiPixelFormat::PAL8 || m_kFormat == NiPixelFormat::PAL4)
    {
        NIASSERT(m_uiClrUsed);
        NIASSERT(m_usBPP <= 8);

        // assumes that palette has at most 256 entries
        WINDOWS_RGBQUAD akSrcPal[256];
        NiPalette::PaletteEntry* pkPal = (NiPalette::PaletteEntry*)akSrcPal;

        NiStreamLoadBinary(kIst, (char *)akSrcPal, 
            m_uiClrUsed * sizeof(WINDOWS_RGBQUAD));

        // Flip the red and blue to match raw data format RGB
        for (unsigned int i = 0; i < m_uiClrUsed; i++)
        {
            unsigned char ucTmp = pkPal->m_ucRed;
            pkPal->m_ucRed = pkPal->m_ucBlue;
            pkPal->m_ucBlue = ucTmp;
            pkPal->m_ucAlpha = 255;
            
            pkPal++;
        }

        pkDest->SetPalette(NiNew NiPalette(false, 256,
            (NiPalette::PaletteEntry*) akSrcPal));
    }
    else
    {
        // seek past colormap - we do not need it
        kIst.Seek(m_uiClrUsed * sizeof(WINDOWS_RGBQUAD), NiFile::ms_iSeekCur); 
    }

    // Skip over any extra data to the set of pixels
    int iSkipAmt = (m_uiOffBits - WINDOWS_BMPTOTALHEADER_SIZE
        - m_uiClrUsed * sizeof(WINDOWS_RGBQUAD));

    if (iSkipAmt > 0)
      kIst.Seek(iSkipAmt, NiFile::ms_iSeekCur); 

    // The image size is the width of a row (padded to 4 byte alignment) times
    // the number of rows in the image
    unsigned int uiSizeImage 
        = ((((m_uiWidth * m_usBPP) + 31) & ~31) >> 3) * m_uiHeight;

    unsigned int uiDestRowStride 
        = pkDest->GetPixelStride() * pkDest->GetWidth();

    // must expand 4 bit per pixel images into 8 BPP
    if (m_usBPP == 4)
    {
        // pad out to an even number of pixels per row
        unsigned int uiInRowStride = (m_uiWidth >> 1) + (m_uiWidth & 0x1);

        uiDestRowStride = uiInRowStride;
        
        // pad input stride
        unsigned int uiInRowPad = (uiInRowStride & 0x3) 
            ? 4 - (uiInRowStride & 0x3) : 0;

        unsigned char *pucInput = NiAlloc(unsigned char,
            uiInRowStride+uiInRowPad);

        unsigned char* pucScanLine; 
        int iRowStep;
        if (m_bFlipVert)
        {
            // flip scanlines while reading
        
            // points to start of final scanline in image
            pucScanLine = pkDest->GetPixels(0) 
                + uiDestRowStride * (pkDest->GetHeight() - 1);

            iRowStep = -(int)(uiDestRowStride);
        }
        else
        {
            // read pixel data directly into image 
        
            // points to start of first scanline in image
            pucScanLine = pkDest->GetPixels(0); 

            iRowStep = uiDestRowStride;
        }

        if (m_kFormat == NiPixelFormat::PAL4)
        {
            if (uiInRowPad)
            {
                for (unsigned int i = 0; i < m_uiHeight; i++, pucScanLine += iRowStep)
                {
                    NiStreamLoadBinary(kIst, pucScanLine, uiInRowStride);
                    unsigned char aucDummy[4];
                    NiStreamLoadBinary(kIst, aucDummy, uiInRowPad);
                }
            }
            else
            {
                for (unsigned int i = 0; i < m_uiHeight; i++, pucScanLine += iRowStep)
                {
                    NiStreamLoadBinary(kIst, pucScanLine, uiInRowStride);
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < m_uiHeight; i++, pucScanLine += iRowStep)
            {
                NiStreamLoadBinary(kIst, pucInput, uiInRowStride+uiInRowPad);
                
                unsigned char *pucPixel = pucScanLine;
                unsigned char *pucInputPixel = pucInput;

                // decode pixels in pairs
                for (unsigned int j=0; j < uiInRowStride; j++)
                {
                    *(pucPixel++) = (*pucInputPixel) >> 4;
                    *(pucPixel++) = (*pucInputPixel) &  0x0f;
                    pucInputPixel++;
                }
            }
        }

        NiFree(pucInput);
    }
    else // 8, 24 or 32-bit data
    {
        if (m_bFlipVert)
        {
            // flip scanlines while reading
        
            // pad input stride
            // points to start of final scanline in image
            unsigned char* pucScanLine = pkDest->GetPixels(0) 
                + uiDestRowStride*(m_uiHeight - 1);
            unsigned int i;

            if (uiDestRowStride & 0x3) 
            {
                unsigned int uiInRowPad = 4 - (uiDestRowStride & 0x3);
                unsigned char aucDummy[4];

                for (i = 0; i < m_uiHeight; i++)
                {
                    NiStreamLoadBinary(kIst, pucScanLine, uiDestRowStride);
                    NiStreamLoadBinary(kIst, aucDummy, uiInRowPad);
                    pucScanLine -= uiDestRowStride;
                }
            }
            else
            {
                for (i = 0; i < m_uiHeight; i++)
                {
                    NiStreamLoadBinary(kIst, pucScanLine, uiDestRowStride);
                    pucScanLine -= uiDestRowStride;
                }
            }
        }
        else
        {
            // read pixel data directly into image 

            // flip scanlines while reading
        
            // pad input stride
            if (uiDestRowStride & 0x3)
            {
                unsigned int uiInRowPad = 4 - (uiDestRowStride & 0x3);

                // points to start of final scanline in image
                unsigned char* pucScanLine = pkDest->GetPixels(0);

                // Pad will always be fewer than four bytes
                unsigned char aucDummy[4];
                unsigned int i;

                for (i = 0; i < m_uiHeight; i++, 
                    pucScanLine += uiDestRowStride)
                {
                    NiStreamLoadBinary(kIst, pucScanLine, uiDestRowStride);
                    NiStreamLoadBinary(kIst, aucDummy, uiInRowPad);
                }
            }
            else
            {
                NiStreamLoadBinary(kIst, pkDest->GetPixels(0), uiSizeImage);
            }
        }
    }

    // If RGB(A), then flip R and B to match raw data format
    if ((m_kFormat == NiPixelFormat::RGB24) 
        || (m_kFormat == NiPixelFormat::RGBA32))
    {
        unsigned int uiStride = pkDest->GetPixelStride();
        unsigned int uiSize = m_uiWidth * m_uiHeight;
        unsigned char* pucTmp = pkDest->GetPixels(0);

        for (unsigned int i = 0; i < uiSize; i++, pucTmp += uiStride)
        {
            unsigned char ucSwap = pucTmp[0];
            pucTmp[0] = pucTmp[2];
            pucTmp[2] = ucSwap;
        }
    }
    
    m_kReadCriticalSection.Unlock();
    return pkDest;
}
//---------------------------------------------------------------------------
