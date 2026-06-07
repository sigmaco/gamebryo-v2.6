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
//---------------------------------------------------------------------------
//  NiZBufferProperty inline functions

//---------------------------------------------------------------------------
inline unsigned int NiTextureAddressSwizzler::IntegerLog2(unsigned int uiValue)
{
    NIASSERT(uiValue > 0);

    unsigned int uiResult = 0;
    while (uiValue > 1)
    {
       uiValue >>= 1;
       ++uiResult;
    }
    return uiResult;
} 
//----------------------------------------------------------------------------
// Converts from texel coordinate (u,v) to the texel offset in a swizzled
// coordinate system.
inline unsigned int NiTextureAddressSwizzler::CoordinateToMortonAddress(
    unsigned int uiU,
    unsigned int uiV,
    unsigned int uiLog2_Width,
    unsigned int uiLog2_Height)
{
    unsigned int uiOutputShift = 0;
    unsigned int uiResult = 0;

    while (uiLog2_Height + uiLog2_Width)
    {
        if (uiLog2_Width)
        {
            uiLog2_Width--;
            uiResult |= ((uiU & 1) << uiOutputShift);
            uiU >>= 1;
            uiOutputShift++;
        }

        if (uiLog2_Height)
        {
            uiLog2_Height--;
            uiResult |= ((uiV & 1) << uiOutputShift);
            uiV >>= 1;
            uiOutputShift++;
        }
    }

    return uiResult;
}
//----------------------------------------------------------------------------
// Converts from a linear address to a swizzled address (or vis versa)
inline unsigned int NiTextureAddressSwizzler::LinearAddressToMortonAddress(
    unsigned int uiSwizzledAddress,
    unsigned int uiLog2_Width,
    unsigned int uiLog2_Height)
{
    unsigned int uiV = uiSwizzledAddress >> uiLog2_Width;
    unsigned int uiU = uiSwizzledAddress & ((1 << uiLog2_Width) - 1);
    return CoordinateToMortonAddress(uiU, uiV, uiLog2_Width, uiLog2_Height);
}
//---------------------------------------------------------------------------
