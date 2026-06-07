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

#ifndef NITEXTUREADDRESSSWIZZLER_H
#define NITEXTUREADDRESSSWIZZLER_H

#include <NiUniversalTypes.h>
#include "NiMath.h"

// This class assists in converting textures raster (linear) scan and morton
// (z-order) scan formats.  Individual pixels are not altered, only their
// order in memory.  This is only supported for textures that are a power of
// two in each dimension.
class NIMAIN_ENTRY NiTextureAddressSwizzler
{
public:
    // *** begin Emergent internal use only ***
    static inline unsigned int IntegerLog2(unsigned int uiValue);

    // Converts from texel coordinate (u,v) to the texel offset in a swizzled
    // coordinate system.
    // Width and height are passed in as log base 2 (if width=16, pass in 4)
    static inline unsigned int CoordinateToMortonAddress(
        unsigned int uiU,
        unsigned int uiV,
        unsigned int uiLog2_Width,
        unsigned int uiLog2_Height);

    // Converts from a linear address to a swizzled address (or vis versa)
    static inline unsigned int LinearAddressToMortonAddress(
        unsigned int uiSwizzledAddress,
        unsigned int uiLog2_Width,
        unsigned int uiLog2_Height);

    // Converts a texture in one scan order to the other order (it is an
    // invertible operation and does not modify individual pixels)
    static void ToggleScanOrder(char* NI_RESTRICT pcOutputTexture,
        const char* NI_RESTRICT pcInputTexture,
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned int uiBytesPerPixel,
        unsigned int uiLog2_BytesPerPixel);

    // Converts the scan order in one texture to another, with special
    // handling of fat pixel formats (8 bytes/pixel or 16 bytes/pixel), which
    // are treated as 4 byte/pixel formats and swizzled accordingly.
    // Individual components of texels are potentially not contiguous in
    // memory in this case.
    static bool ToggleScanOrderNV(char* NI_RESTRICT pcOutputTexture,
        const char* NI_RESTRICT pcInputTexture,
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned int uiBytesPerPixel);

    // *** end Emergent internal use only ***
};

#include "NiTextureAddressSwizzler.inl"

#endif

