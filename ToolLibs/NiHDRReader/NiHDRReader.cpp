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

#include "NiHDRReader.h"
#include "NiPixelData.h"
#include "NiPixelFormat.h"
#include <NiFile.h>
#include <NiSystem.h>

#include <NiDirectXVersion.h>
#include <NiD3DDefines.h>

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

// NOTE:
// As of right now, NiHDRReader always exports RGBA64 textures.  This loses
// 3 bits of mantissa precision from full RGBA128 textures.  If for some
// reason it is desired to have NiHDRReader export RGBA128 textures,
// then changing all occurences of RGBA64 to RGBA128 in this file as well
// as using the ConvertRGBEToFloat instead of ConvertRGBE to half functions
// in the places that it is used should be sufficient.

//---------------------------------------------------------------------------
bool NiHDRReader::CanReadFile(const char* pcFileExtension) const
{
    if (!NiStricmp(pcFileExtension, ".hdr"))
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
NiPixelData* NiHDRReader::ReadFile(NiFile& kIst, NiPixelData* pkOptDest)
{
    unsigned int uiHeight;
    unsigned int uiWidth;
    bool bXLeftToRight;
    bool bYTopToBottom;
    bool bRowMajor;

    if (!ReadHeaderInternal(kIst, uiWidth, uiHeight, bXLeftToRight, 
        bYTopToBottom, bRowMajor))
    {
        return NULL;
    }

    NiPixelData* pkDest;

    if (pkOptDest && (pkOptDest->GetWidth() == uiWidth) && 
        (pkOptDest->GetHeight() == uiHeight) &&
        (pkOptDest->GetPixelFormat() == NiPixelFormat::RGBA64))
    {
        pkDest = pkOptDest;
    }
    else
    {
        pkDest = NiNew NiPixelData(uiWidth, uiHeight, 
            NiPixelFormat::RGBA64);
    }

    unsigned char* pucData = pkDest->GetPixels(0, 0);
    const unsigned int uiDPixelSize = pkDest->GetPixelStride();
    int iRowStride = uiDPixelSize * uiWidth;
    int iColStride = uiDPixelSize;

    // If bottom to top, then start at the last scanline and
    // change the direction of the row stride
    if (!bYTopToBottom)
    {
        pucData += iRowStride * (uiHeight - 1);
        iRowStride = -iRowStride;
    }

    // If right to left, start at the end of the current scanline.
    if (!bXLeftToRight)
    {
        pucData += iColStride * (uiWidth - 1);
        iColStride = -iColStride;
    }

    // Get the current position.
    unsigned int uiPosition = kIst.GetPosition();

    // Read first pixel of data to see if it is RLE data
    unsigned char aucFirstPixel[4];
    NiStreamLoadBinary(kIst, aucFirstPixel, sizeof(aucFirstPixel));

    // Set the file pointer back.
    kIst.Seek(uiPosition, NiFile::ms_iSeekSet);

    bool bSuccess;

    // RLE files have a 2(1 byte), 2(1 byte), scanline length (2 bytes,
    // upper first, lower second, must be less than 32,768).
    if (aucFirstPixel[0] != 2 || aucFirstPixel[1] != 2 || 
        aucFirstPixel[2] & 0x80)
    {
        bSuccess = ReadRawFile(kIst, aucFirstPixel, pucData, iRowStride, 
            iColStride, uiHeight, uiWidth, uiDPixelSize);
    }
    else
    {
        bSuccess = ReadRLEFile(kIst, aucFirstPixel, pucData, iRowStride,
            iColStride, uiHeight, uiWidth, uiDPixelSize);
    }

    if (bSuccess)
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
bool NiHDRReader::ReadRLEFile(NiFile& kIst, unsigned char [4],
    unsigned char* pucData, int iRowStride, int iColStride,
    unsigned int uiHeight, unsigned int uiWidth, unsigned int)
{
    // RGBE components are encoded independently.  Therefore,
    // we must read an entire scanline's r1-rn, g1-gn, b1-bn, then e1-en,
    // store them into the pucScanLine in r1,g1,b1,e1 order.  Then,
    // convert to (half)floats and store into the image.
    unsigned char* pucLineBuffer = NiAlloc(unsigned char, uiWidth*4);

    unsigned int uiRowsRead = 0;

    // Loop through all of the rows that the file claims to have
    while (uiRowsRead < uiHeight)
    {
        unsigned int uiColsRead = 0;

        unsigned char aucData[4];
        NiStreamLoadBinary(kIst, aucData, sizeof(aucData));

        if (aucData[0] != 2 || aucData[1] != 2 || aucData[2] & 0x80
            || (unsigned int)((aucData[2] << 8) + aucData[3]) != uiWidth)
        {
            // invalid scanline start
            return false;
        }

        // Loop through all the components
        const unsigned int uiSPixelSize = 4;
        for (unsigned int uiComp = 0; uiComp < uiSPixelSize; uiComp++)
        {
            // start location to interleave the read data
            unsigned char* pucComponentBuffer = pucLineBuffer + uiComp;

            uiColsRead = 0;

            // Loop through all the columns in this row.
            while (uiColsRead < uiWidth)
            {
                unsigned int uiDumpLength = 0;
                unsigned int uiRunLength = 0;

                // Read dump/run size
                unsigned char ucLen;
                NiStreamLoadBinary(kIst, ucLen);

                // bad dump length?
                if (0 == ucLen)
                    return false;
                else if (ucLen > 128)
                    uiRunLength = ucLen - 128;
                else
                    uiDumpLength = ucLen;

                // Explicitly read first pixel
                unsigned char ucRead;
                NiStreamLoadBinary(kIst, ucRead);
                *pucComponentBuffer = ucRead;
                pucComponentBuffer += uiSPixelSize;

                // Use length-1 for dump/runs because the first
                // pixel of the dump/run will get read explicitly.
               
                uiColsRead += (uiDumpLength + uiRunLength);
                // If a dump, read the rest of the pixels.
                while (uiDumpLength-- > 1)
                {
                    NiStreamLoadBinary(kIst, *pucComponentBuffer);
                    pucComponentBuffer += uiSPixelSize;
                }

                // If a run, copy the buffered pixel repeatedly for the
                // remaining times.
                while (uiRunLength-- > 1)
                {
                    *pucComponentBuffer = ucRead;
                    pucComponentBuffer += uiSPixelSize;
                }
            }
            NIASSERT(uiColsRead == uiWidth);
        }
        
        // Done reading an entire scanline; now convert to the right
        // format and store in the correct location.
        unsigned char* pucPixel = pucData;
        unsigned char* pucBufPointer = pucLineBuffer;
        for (unsigned int uiPixel = 0; uiPixel < uiWidth; uiPixel++)
        {
            ConvertRGBEToHalf(pucBufPointer, pucPixel);
            pucPixel += iColStride;
            pucBufPointer += uiSPixelSize;
        }
        pucData += iRowStride;
        uiRowsRead++;
    }
    NIASSERT(uiRowsRead == uiHeight);
    NiFree(pucLineBuffer);

    return true;
}
//---------------------------------------------------------------------------
bool NiHDRReader::ReadHeader(NiFile& kIst, 
    unsigned int& uiWidth, unsigned int& uiHeight, 
    NiPixelFormat& kFormat, bool& bMipmap, unsigned int& uiFaces)
{
    // Set static HDR conditions
    kFormat = NiPixelFormat::RGBA64;
    bMipmap = false;
    uiFaces = 1;

    // Read the header for the rest of the information
    bool bDummy;
    return ReadHeaderInternal(kIst, uiWidth, uiHeight, bDummy, bDummy, 
        bDummy);
}
//---------------------------------------------------------------------------
bool NiHDRReader::ReadHeaderInternal(NiFile& kIst, unsigned int& uiWidth, 
    unsigned int& uiHeight, bool& bXLeftToRight, bool& bYTopToBottom,
    bool& bRowMajor)
{
    const unsigned int BUFFER_SIZE = 256;
    const char* acFormat = "FORMAT=32-bit_rle_rgbe";

    // Start at the beginning of the file...
    kIst.Seek(0, 0);

    char acBuffer[BUFFER_SIZE];

    while (kIst.GetLine(acBuffer, BUFFER_SIZE) > 0)
    {
        // If the line starts with FORMAT but is not the right kind...
        if (strncmp(acFormat, acBuffer, 7) && 
            !strncmp(acFormat, acBuffer, strlen(acFormat)))
        {
            return false;
        }
        else if (strlen(acBuffer) == 0)
        {
            // Once the first blank line is hit, the end of the header has
            // been reached.
            break;
        }

        // ignore other lines
    }

    // Read the resolution line
    if (kIst.GetLine(acBuffer, BUFFER_SIZE) <= 0)
        return false;

    // Valid regular expressions: 
    //   col-major: m/[-+]X [0-9]+ [-+]Y [0-9]+/
    //   row-major: m/[-+]Y [0-9]+ [-+]X [0-9]+/
    // NOTE: only row-major hdr images are supported at this time.

    // Was the last direction read the X direction?
    bool bReadX = false;

    char* pcContext;
    char* pcTok = NiStrtok(acBuffer, " ", &pcContext);
    int iTokNum = 0;

    while (pcTok && iTokNum < 4)
    {
        if (0 == iTokNum || 2 == iTokNum)
        {
            // catch errors
            if (pcTok[0] != '-' && pcTok[0] != '+')
                return false;

            if ('X' == pcTok[1] && !bReadX)
            {
                bXLeftToRight = (pcTok[0] == '+');
                bReadX = true;
                if (0 == iTokNum)
                    bRowMajor = false;
            }
            else if ('Y' == pcTok[1] && (0 == iTokNum || bReadX))
            {
                bYTopToBottom = (pcTok[0] == '-');
                bReadX = false;
                if (0 == iTokNum)
                    bRowMajor = true;
            }
            else
            {
                // Invalid axis
                return false;
            }
        }
        else // if (1 == iTokNum || 3 == iTokNum)
        {
            unsigned int uiTemp = (unsigned int)atoi(pcTok);

            if (bReadX)
                uiWidth = uiTemp;
            else
                uiHeight = uiTemp;
        }

        iTokNum++;
        pcTok = NiStrtok(NULL, " ", &pcContext);
    }

    // sanity check in case the file ends in the middle of the line
    if (iTokNum != 4)
        return false;

    // As of right now, only row-major hdr reading is supported.  If a
    // file specifies that its pixels refer to destinations in a column
    // major way, then it will just be rotated 90 degrees but will still
    // be silently loaded.  Most exporters use -Y +X (row major, start
    // in upper left hand corner) so this should not be a "major" issue.
    bRowMajor = true;

    return true;
}
//---------------------------------------------------------------------------
void NiHDRReader::ConvertRGBEToHalf(unsigned char* pucRGBE, 
    unsigned char* pucDest)
{
    float afRGB[3];

    if (pucRGBE[3] == 0)
    {
        afRGB[0] = 0.0f;
        afRGB[1] = 0.0f;
        afRGB[2] = 0.0f;
    }
    else
    {
        // original RGBE spec says ldexp(1.0/256.0, exponent - 128)
        float fExponent = (float)ldexp(1.0, pucRGBE[3] - 136);

        afRGB[0] = ((unsigned int)pucRGBE[0]+0.5f)*fExponent;
        afRGB[1] = ((unsigned int)pucRGBE[1]+0.5f)*fExponent;
        afRGB[2] = ((unsigned int)pucRGBE[2]+0.5f)*fExponent;
    }

    D3DXFLOAT16* pkHalfDest = (D3DXFLOAT16*)pucDest;

    *(pkHalfDest++) = D3DXFLOAT16(afRGB[0]);
    *(pkHalfDest++) = D3DXFLOAT16(afRGB[1]);
    *(pkHalfDest++) = D3DXFLOAT16(afRGB[2]);
    *pkHalfDest = D3DXFLOAT16(1.0f);
}
//---------------------------------------------------------------------------
void NiHDRReader::ConvertRGBEToFloat(unsigned char* pucRGBE,
    unsigned char* pucDest)
{
    float* pfDest = (float*)pucDest;

    if (0 == pucRGBE[3])
    {
        pfDest[0] = 0.0f;
        pfDest[1] = 0.0f;
        pfDest[2] = 0.0f;
    }
    else
    {
        // original RGBE spec says ldexp(1.0/256.0, exponent - 128)
        float fExponent = (float)ldexp(1.0, pucRGBE[3] - 136);

        pfDest[0] = ((unsigned int)pucRGBE[0]+0.5f)*fExponent;
        pfDest[1] = ((unsigned int)pucRGBE[1]+0.5f)*fExponent;
        pfDest[2] = ((unsigned int)pucRGBE[2]+0.5f)*fExponent;
    }

    pfDest[3] = 1.0f;
}
//---------------------------------------------------------------------------
bool NiHDRReader::ReadRawFile(NiFile& kIst, unsigned char*, 
    unsigned char* pucData, int iRowStride, int iColStride, 
    unsigned int uiHeight, unsigned int uiWidth, unsigned int)
{
    // Raw files just have all the pixels listed contiguously with no
    // RLE encoding.

    for (unsigned int uiR = 0; uiR < uiHeight; uiR++)
    {
        // temporary pointer to walk through the row
        unsigned char* pucColPointer = pucData;

        for (unsigned int uiC = 0; uiC < uiWidth; uiC++)
        {
            // read pixel
            unsigned char aucData[4];
            NiStreamLoadBinary(kIst, aucData, sizeof(aucData));
            // convert and store pixel
            ConvertRGBEToHalf(aucData, pucColPointer);

            pucColPointer += iColStride;
        }

        pucData += iRowStride;
    }

    return true;
}
//---------------------------------------------------------------------------