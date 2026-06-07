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
#include "NiD3DRendererPCH.h"

#include "NiDX9PixelFormat.h"

//---------------------------------------------------------------------------
D3DFORMAT NiDX9PixelFormat::DetermineD3DFormat(const NiPixelFormat& kDesc)
{
    D3DFORMAT eD3DFmt = (D3DFORMAT)kDesc.GetRendererHint();
    NiPixelFormat::Format eFormat = kDesc.GetFormat();
    unsigned char ucBPP = kDesc.GetBitsPerPixel();

    if (eD3DFmt != NiPixelFormat::INVALID_RENDERER_HINT)
        return eD3DFmt;

    eD3DFmt = D3DFMT_UNKNOWN;
    switch (eFormat)
    {
    case NiPixelFormat::FORMAT_RGB:
        switch (ucBPP)
        {
        case 8:
            if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0xE0)
                eD3DFmt = D3DFMT_R3G3B2;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x00)
                eD3DFmt = D3DFMT_L8;
            break;
        case 16:
            if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0xF800)
                eD3DFmt = D3DFMT_R5G6B5;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x7C00)
                eD3DFmt = D3DFMT_X1R5G5B5;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x0F00)
                eD3DFmt = D3DFMT_X4R4G4B4;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0)
                eD3DFmt = D3DFMT_L16;
            break;
        case 24:
            eD3DFmt = D3DFMT_R8G8B8;
            break;
        case 32:
            if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x00FF0000)
                eD3DFmt = D3DFMT_X8R8G8B8;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x000000FF)
                eD3DFmt = D3DFMT_X8B8G8R8;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0xFFFF0000)
                eD3DFmt = D3DFMT_G16R16;

            break;
        default:
            eD3DFmt = D3DFMT_UNKNOWN;
            break;
        }
        break;
    case NiPixelFormat::FORMAT_RGBA:
        switch (ucBPP)
        {
        case 8:
            if (kDesc.GetMask(NiPixelFormat::COMP_LUMA) == 0)
                eD3DFmt = D3DFMT_A8;
            else if (kDesc.GetMask(NiPixelFormat::COMP_LUMA) == 0x0F)
                eD3DFmt = D3DFMT_A4L4;
            break;
        case 16:
            if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x7C00)
                eD3DFmt = D3DFMT_A1R5G5B5;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x0F00)
                eD3DFmt = D3DFMT_A4R4G4B4;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x00E0)
                eD3DFmt = D3DFMT_A8R3G3B2;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0)
                eD3DFmt = D3DFMT_A8L8;
            break;
        case 24:
            eD3DFmt = D3DFMT_UNKNOWN;
            break;
        case 32:
            if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x00FF0000)
                eD3DFmt = D3DFMT_A8R8G8B8;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x000003FF)
                eD3DFmt = D3DFMT_A2B10G10R10;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x000000FF)
                eD3DFmt = D3DFMT_A8B8G8R8;
            else if (kDesc.GetMask(NiPixelFormat::COMP_RED) == 0x3FF00000)
                eD3DFmt = D3DFMT_A2R10G10B10;
            break;
        case 64:
            eD3DFmt = D3DFMT_A16B16G16R16F;
            break;
        case 128:
            eD3DFmt = D3DFMT_A32B32G32R32F;
            break;
        default:
            eD3DFmt = D3DFMT_UNKNOWN;
            break;
        }
        break;
    case NiPixelFormat::FORMAT_PAL:
        //  Make sure there is a palette
        if (kDesc.GetPalettized() == false)
        {
            NiOutputDebugString("NiDX9PixelFormat> PAL type w/ "
                "unpalettized format!");
        }
        eD3DFmt = D3DFMT_P8;
        break;
    case NiPixelFormat::FORMAT_PALALPHA:
        eD3DFmt = D3DFMT_UNKNOWN;
        break;
    case NiPixelFormat::FORMAT_DXT1:
        eD3DFmt = D3DFMT_DXT1;
        break;
    case NiPixelFormat::FORMAT_DXT3:
        eD3DFmt = D3DFMT_DXT3;
        break;
    case NiPixelFormat::FORMAT_DXT5:
        eD3DFmt = D3DFMT_DXT5;
        break;
    case NiPixelFormat::FORMAT_BUMP:
        eD3DFmt = D3DFMT_V8U8;
        break;
    case NiPixelFormat::FORMAT_BUMPLUMA:
        if (ucBPP == 16)
            eD3DFmt = D3DFMT_L6V5U5;
        else if (ucBPP == 32)
            eD3DFmt = D3DFMT_X8L8V8U8;
        break;
    case NiPixelFormat::FORMAT_RENDERERSPECIFIC:
        eD3DFmt = D3DFMT_UNKNOWN;
        break;
    case NiPixelFormat::FORMAT_ONE_CHANNEL:
        switch(ucBPP)
        {
            case 8:
                if (kDesc.GetMask(NiPixelFormat::COMP_ALPHA) == 0)
                    eD3DFmt = D3DFMT_L8;
                else
                    eD3DFmt = D3DFMT_A8;
                break;
            case 16:
                eD3DFmt = D3DFMT_R16F;
                break;
            case 32:
                eD3DFmt = D3DFMT_R32F;
                break;
        }
        break;
    case NiPixelFormat::FORMAT_TWO_CHANNEL:
        switch(ucBPP)
        {
            case 16:
                eD3DFmt = D3DFMT_A8L8;
                break;
            case 32:
                eD3DFmt = D3DFMT_G16R16F;
                break;
            case 64:
                eD3DFmt = D3DFMT_G32R32F;
                break;
        }
        break;
    case NiPixelFormat::FORMAT_THREE_CHANNEL:
        eD3DFmt = D3DFMT_UNKNOWN;
        break;
    case NiPixelFormat::FORMAT_FOUR_CHANNEL:
        switch(ucBPP)
        {
            case 32:
                eD3DFmt = D3DFMT_A8R8G8B8;
                break;
            case 64:
                eD3DFmt = D3DFMT_A16B16G16R16F;
                break;
            case 128:
                eD3DFmt = D3DFMT_A32B32G32R32F;
                break;
        }
        break;
    case NiPixelFormat::FORMAT_DEPTH_STENCIL:
        switch(ucBPP)
        {
            case 32:
            {
                switch(kDesc.GetBits(NiPixelFormat::COMP_STENCIL))
                {
                default:
                case 0:
                    if (kDesc.GetBits(NiPixelFormat::COMP_DEPTH) == 24)
                        return D3DFMT_D24X8;
                    return D3DFMT_D32;
                case 4:
                    return D3DFMT_D24X4S4;
                case 8:
                    return D3DFMT_D24S8;
                }
            }
            break;
            case 16:
            {
                if (kDesc.GetBits(NiPixelFormat::COMP_STENCIL) == 1)
                    return D3DFMT_D15S1;
                else 
                    return D3DFMT_D16;
            }
            break;
        }
        break;
    }

    return eD3DFmt;
}
//---------------------------------------------------------------------------
void NiDX9PixelFormat::InitFromD3DFormat(D3DFORMAT eD3DFmt, 
    NiPixelFormat& kFormat)
{
    switch (eD3DFmt)
    {
        case D3DFMT_R8G8B8:
            kFormat = NiPixelFormat::BGR888;
            break;
        case D3DFMT_A8R8G8B8:
            kFormat = NiPixelFormat::BGRA8888;
            break;
        case D3DFMT_X8R8G8B8:
            kFormat = NiPixelFormat::BGRX8888;
            break;
        case D3DFMT_R5G6B5:
            kFormat = NiPixelFormat::BGR565;
            break;
        case D3DFMT_X1R5G5B5:
            kFormat = NiPixelFormat::BGRX5551;
            break;
        case D3DFMT_A1R5G5B5:
            kFormat = NiPixelFormat::BGRA5551;
            break;
        case D3DFMT_A4R4G4B4:
            kFormat = NiPixelFormat::BGRA4444;
            break;
        case D3DFMT_R3G3B2:
            kFormat = NiPixelFormat::BGR233;
            break;
        case D3DFMT_A8:
            kFormat = NiPixelFormat::A8;
            break;
        case D3DFMT_A8R3G3B2:
            kFormat = NiPixelFormat::BGRA2338;
            break;
        case D3DFMT_X4R4G4B4:
            kFormat = NiPixelFormat::BGRX4444;
            break;
        case D3DFMT_A2B10G10R10:
            kFormat = NiPixelFormat::RGBA1010102;
            break;
        case D3DFMT_A8B8G8R8:
            kFormat = NiPixelFormat::RGBA32;
            break;
        case D3DFMT_X8B8G8R8:
            kFormat = NiPixelFormat::RGBX8888;
            break;
        case D3DFMT_G16R16:
            kFormat = NiPixelFormat::GR1616;
            break;
        case D3DFMT_A2R10G10B10:
            kFormat = NiPixelFormat::BGRA1010102;
            break;
        case D3DFMT_A16B16G16R16:
            kFormat = NiPixelFormat::RGBA64;
            break;
        case D3DFMT_A8P8:
            kFormat = NiPixelFormat::PA88;
            break;
        case D3DFMT_P8:
            kFormat = NiPixelFormat::PAL8;
            break;
        case D3DFMT_L8:
            kFormat = NiPixelFormat::L8;
            break;
        case D3DFMT_L16:
            kFormat = NiPixelFormat::L16;
            break;
        case D3DFMT_A8L8:
            kFormat = NiPixelFormat::LA88;
            break;
        case D3DFMT_A4L4:
            kFormat = NiPixelFormat::LA44;
            break;
        case D3DFMT_V8U8:
            kFormat = NiPixelFormat::BUMP16;
            break;
        case D3DFMT_Q8W8V8U8:
            kFormat = NiPixelFormat::BUMPWQ8888;
            break;
        case D3DFMT_V16U16:
            kFormat = NiPixelFormat::BUMP1616;
            break;
        case D3DFMT_Q16W16V16U16:
            kFormat = NiPixelFormat::RENDERERSPECIFIC64;
            break;
        case D3DFMT_CxV8U8:
            kFormat = NiPixelFormat::RENDERERSPECIFIC16;
            break;
        case D3DFMT_L6V5U5:
            kFormat = NiPixelFormat::BUMPLUMA556;
            break;
        case D3DFMT_X8L8V8U8:
            kFormat = NiPixelFormat::BUMPLUMAX8888;
            break;
        case D3DFMT_A2W10V10U10:
            kFormat = NiPixelFormat::BUMPW101111;
            break;
        case D3DFMT_DXT1:
            kFormat = NiPixelFormat::DXT1;
            break;
        case D3DFMT_DXT3:
            kFormat = NiPixelFormat::DXT3;
            break;
        case D3DFMT_DXT5:
            kFormat = NiPixelFormat::DXT5;
            break;
        case D3DFMT_D16_LOCKABLE:
            kFormat = NiPixelFormat::DEPTH16;
            break;
        case D3DFMT_D32:
            kFormat = NiPixelFormat::DEPTH32;
            break;
        case D3DFMT_D15S1:
            kFormat = NiPixelFormat::STENCILDEPTH115;
            break;
        case D3DFMT_D24S8:
            kFormat = NiPixelFormat::STENCILDEPTH824;
            break;
        case D3DFMT_D24X8:
            kFormat = NiPixelFormat::XDEPTH824;
            break;
        case D3DFMT_D24X4S4:
            kFormat = NiPixelFormat::STENCILXDEPTH4424;
            break;
        case D3DFMT_D32F_LOCKABLE:
            kFormat = NiPixelFormat::RENDERERSPECIFIC32;
            break;
        case D3DFMT_D24FS8:
            kFormat = NiPixelFormat::RENDERERSPECIFIC32;
            break;
        case D3DFMT_D16:
            kFormat = NiPixelFormat::DEPTH16;
            break;
        case D3DFMT_R16F:
            kFormat = NiPixelFormat::R16;
            break;
        case D3DFMT_G16R16F:
            kFormat = NiPixelFormat::RG32;
            break;
        case D3DFMT_A16B16G16R16F:
            kFormat = NiPixelFormat::RGBA64;
            break;
        case D3DFMT_R32F:
            kFormat = NiPixelFormat::R32;
            break;
        case D3DFMT_G32R32F:
            kFormat = NiPixelFormat::RG64;
            break;
        case D3DFMT_A32B32G32R32F:
            kFormat = NiPixelFormat::RGBA128;
            break;
        case D3DFMT_MULTI2_ARGB8:
        case D3DFMT_G8R8_G8B8:
        case D3DFMT_R8G8_B8G8:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
        case D3DFMT_UYVY:
        case D3DFMT_YUY2:
        default:
            kFormat = NiPixelFormat::RENDERERSPECIFICCOMPRESSED;
            break;
    }

    kFormat.SetRendererHint(eD3DFmt);
    kFormat.SetExtraData(0);
}
//---------------------------------------------------------------------------
NiPixelFormat* NiDX9PixelFormat::CreateFromD3DFormat(D3DFORMAT eD3DFmt)
{
    NiPixelFormat* pkFormat = NiNew NiPixelFormat();
    InitFromD3DFormat(eD3DFmt, *pkFormat);
    return pkFormat;
}
//---------------------------------------------------------------------------
