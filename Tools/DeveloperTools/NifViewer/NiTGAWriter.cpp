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

#include "NiTGAWriter.h"

#include <NiBinaryStream.h>
#include <NiPixelData.h>

//---------------------------------------------------------------------------
bool NiTGAWriter::Dump(const NiPixelData* pkPixelData,
    NiBinaryStream& kStream)
{
    // TGA files are always little endian
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kStream.SetEndianSwap(!bPlatformLittle);

    const NiPixelFormat &kFormat = pkPixelData->GetPixelFormat();
    unsigned int uiBitsPerPixel = kFormat.GetBitsPerPixel();

    if (uiBitsPerPixel != 32 && uiBitsPerPixel != 24 && uiBitsPerPixel != 16)
        return false;

    unsigned char ucIDLen = 0;
    NiStreamSaveBinary(kStream, ucIDLen);

    unsigned char ucColorMapType = 0;
    NiStreamSaveBinary(kStream, ucColorMapType);

    unsigned char ucImageType = 2;
    NiStreamSaveBinary(kStream, ucImageType);

    unsigned char aucColorMap[5];
    NiBinaryStreamSave(kStream, aucColorMap, 5);

    unsigned short usX = 0;
    NiStreamSaveBinary(kStream, usX);

    unsigned short usY = 0;
    NiStreamSaveBinary(kStream, usY);

    unsigned short usWidth = (unsigned short)pkPixelData->GetWidth();
    NiStreamSaveBinary(kStream, usWidth);

    unsigned short usHeight = (unsigned short)pkPixelData->GetHeight();
    NiStreamSaveBinary(kStream, usHeight);

    unsigned char ucPixelSize;
    unsigned char ucImageDescriptor;

    if (uiBitsPerPixel == 32)
    {
        ucPixelSize = 32;
        ucImageDescriptor = 0x28;
    }
    else if (uiBitsPerPixel == 24)
    {
        ucPixelSize = 24;
        ucImageDescriptor = 0x20;
    }
    else
    {
        NIASSERT(uiBitsPerPixel == 16);
        ucPixelSize = 16;
        ucImageDescriptor = 0x21;
    }

    NiStreamSaveBinary(kStream, ucPixelSize);
    NiStreamSaveBinary(kStream, ucImageDescriptor);

    // Write the pixels

    // Write() seems to be slow otherwise, even though it does not do swapping
    kStream.SetEndianSwap(false);

    const unsigned char* pucPixels = pkPixelData->GetPixels();
    unsigned int uiPixels = usHeight * usWidth;
    unsigned char aucShift[4];
    unsigned int auiMask[4];

    aucShift[0] = kFormat.GetShift(NiPixelFormat::COMP_RED);
    aucShift[1] = kFormat.GetShift(NiPixelFormat::COMP_GREEN);
    aucShift[2] = kFormat.GetShift(NiPixelFormat::COMP_BLUE);
    aucShift[3] = kFormat.GetShift(NiPixelFormat::COMP_ALPHA);

    auiMask[0] = kFormat.GetMask(NiPixelFormat::COMP_RED);
    auiMask[1] = kFormat.GetMask(NiPixelFormat::COMP_GREEN);
    auiMask[2] = kFormat.GetMask(NiPixelFormat::COMP_BLUE);
    auiMask[3] = kFormat.GetMask(NiPixelFormat::COMP_ALPHA);

    if (ucPixelSize == 16)
    {
        NIASSERT(pkPixelData->GetPixelStride() == 2);
        unsigned char aucBits[4];
        aucBits[0] = kFormat.GetBits(NiPixelFormat::COMP_RED);
        aucBits[1] = kFormat.GetBits(NiPixelFormat::COMP_GREEN);
        aucBits[2] = kFormat.GetBits(NiPixelFormat::COMP_BLUE);
        aucBits[3] = kFormat.GetBits(NiPixelFormat::COMP_ALPHA);

        for (unsigned int i = 0; i < uiPixels; i++)
        {
#if defined(_PS3) || defined(_XENON) || defined(_WII) 
            unsigned short usPixel = pucPixels[1] | (pucPixels[0] << 8);
#elif defined(WIN32)
            unsigned short usPixel = pucPixels[0] | (pucPixels[1] << 8);
#else
#error Unsupported platform
#endif
            pucPixels += 2;

            // Zeroes are shifted into 5-bit RGB values if the pixel data has
            // less than 5 bits.

            unsigned char aucColor[4];

            aucColor[0] = (unsigned char)(
                ((usPixel & auiMask[2]) >> aucShift[2]) << (5 - aucBits[2]));
            aucColor[1] = (unsigned char)(
                ((usPixel & auiMask[1]) >> aucShift[1]) << (5 - aucBits[1]));
            aucColor[2] = (unsigned char)(
                ((usPixel & auiMask[0]) >> aucShift[0]) << (5 - aucBits[0]));

            // 1-bit alpha
            aucColor[3] = (unsigned char)(
                ((usPixel & auiMask[3]) >> aucShift[3]) >> (aucBits[3] - 1));

            unsigned char aucByte[2];

            aucByte[0] = aucColor[0] | (aucColor[1] << 5); // gggbbbbb
            aucByte[1] = (aucColor[3] << 7) | (aucColor[2] << 2) |
                (aucColor[1] >> 3); // arrrrrgg

            kStream.Write(aucByte, 2);
        }
    }
    else if (ucPixelSize == 24)
    {
        unsigned int uiStride = pkPixelData->GetPixelStride();
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_RED) == 8);
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_GREEN) == 8);
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_BLUE) == 8);
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_ALPHA) == 0);

        for (unsigned int i = 0; i < uiPixels; i++)
        {
            unsigned int uiPixel = 0;

#if defined(_PS3) || defined(_XENON) || defined(_WII)
            unsigned uiShift = uiStride*8;
            for (unsigned int c = 0; c < uiStride; c++)
            {
                uiShift -= 8;
                uiPixel = uiPixel | *pucPixels++ << uiShift;
            }
#elif defined(WIN32)
            unsigned uiShift = 0;
            for (unsigned int c = 0; c < uiStride; c++)
            {
                uiPixel = uiPixel | *pucPixels++ << uiShift;
                uiShift += 8;
            }
#else
#error Unsupported platform
#endif

            // Assemble bytes in order defined by TGA: BGR or BGRA
            unsigned char aucColor[3];

            aucColor[0] = (unsigned char)((uiPixel & auiMask[2]) >> aucShift[2]);
            aucColor[1] = (unsigned char)((uiPixel & auiMask[1]) >> aucShift[1]);
            aucColor[2] = (unsigned char)((uiPixel & auiMask[0]) >> aucShift[0]);
            
            kStream.Write(aucColor, 3);
        }
    }
    else if (ucPixelSize == 32)
    {
        unsigned int uiStride = pkPixelData->GetPixelStride();
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_RED) == 8);
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_GREEN) == 8);
        NIASSERT(kFormat.GetBits(NiPixelFormat::COMP_BLUE) == 8);

        const int iForceAlphaTo1 =
            (kFormat.GetBits(NiPixelFormat::COMP_ALPHA) == 8) ? 0x00 : 0xFF;

        for (unsigned int i = 0; i < uiPixels; i++)
        {
            unsigned int uiPixel = 0;

#if defined(_PS3) || defined(_XENON) || defined(_WII)
            unsigned uiShift = uiStride*8;
            for (unsigned int c = 0; c < uiStride; c++)
            {
                uiShift -= 8;
                uiPixel = uiPixel | *pucPixels++ << uiShift;
            }
#elif defined(WIN32)
            unsigned uiShift = 0;
            for (unsigned int c = 0; c < uiStride; c++)
            {
                uiPixel = uiPixel | *pucPixels++ << uiShift;
                uiShift += 8;
            }
#else
#error Unsupported platform
#endif

            // Assemble bytes in order defined by TGA: BGR or BGRA
            unsigned char aucColor[4];

            aucColor[0] = (unsigned char)((uiPixel & auiMask[2]) >> aucShift[2]);
            aucColor[1] = (unsigned char)((uiPixel & auiMask[1]) >> aucShift[1]);
            aucColor[2] = (unsigned char)((uiPixel & auiMask[0]) >> aucShift[0]);
            aucColor[3] = (unsigned char)(
                ((uiPixel & auiMask[3]) >> aucShift[3]) | iForceAlphaTo1);

            kStream.Write(aucColor, 4);
        }
    }

    return true;
}
