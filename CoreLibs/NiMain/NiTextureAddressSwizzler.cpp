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

#include "NiTextureAddressSwizzler.h"

//----------------------------------------------------------------------------
void NiTextureAddressSwizzler::ToggleScanOrder(
    char* NI_RESTRICT pcOutputTexture,
    const char* NI_RESTRICT pcInputTexture,
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned int uiBytesPerPixel,
    unsigned int uiLog2_BytesPerPixel)
{
    NIASSERT(NiIsPowerOf2(uiWidth) && NiIsPowerOf2(uiHeight));

    const unsigned int uiLog2_Width  = IntegerLog2(uiWidth);
    const unsigned int uiLog2_Height = IntegerLog2(uiHeight);

    // For each pixel in a linear ordering, copy to the output
    unsigned int uiSrcIndex = 0;
    for (unsigned int y = 0; y < uiHeight; ++y)
    {
        for (unsigned int x = 0; x < uiWidth; ++x)
        {
            // Copy a single pixel from source (linear) to destination
            // (swizzled)
            unsigned int uiDestIndex = CoordinateToMortonAddress(
                x, y, uiLog2_Width, uiLog2_Height) << uiLog2_BytesPerPixel;

            memcpy(pcOutputTexture + uiDestIndex,
                pcInputTexture + uiSrcIndex, uiBytesPerPixel);

            uiSrcIndex += uiBytesPerPixel;
        }
    }
}
//----------------------------------------------------------------------------
bool NiTextureAddressSwizzler::ToggleScanOrderNV(
    char* NI_RESTRICT pcOutputTexture,
    const char* NI_RESTRICT pcInputTexture,
    unsigned int uiWidth,
    unsigned int uiHeight,
    unsigned int uiBytesPerPixel)
{
    // 1, 2, and 4 bytes/pixel textures are handled normally, but
    // 8 or 16 byte textures are treated as 2x or 4x wide 4 bytes/pixel
    unsigned int uiLog2_BytesPerPixel = 0;
    switch (uiBytesPerPixel)
    {
    case 1:
        uiLog2_BytesPerPixel = 0;
        break;
    case 2:
        uiLog2_BytesPerPixel = 1;
        break;
    case 4:
        uiLog2_BytesPerPixel = 2;
        break;
    case 8: 
        // Treated as a 2x wide 4 byte/pixel
        uiWidth <<= 1;
        uiBytesPerPixel >>= 1;
        uiLog2_BytesPerPixel = 2;
        break;
    case 16:
        // Treated as a 4x wide 4 byte/pixel
        uiWidth <<= 2;
        uiBytesPerPixel >>= 2;
        uiLog2_BytesPerPixel = 2;
        break;
    default:
        NIASSERT(false && "Invalid pixel format for NV swizzling");
        return false;
    }

    ToggleScanOrder(pcOutputTexture, pcInputTexture, uiWidth, uiHeight,
        uiBytesPerPixel, uiLog2_BytesPerPixel);

    return true;
}
//---------------------------------------------------------------------------
