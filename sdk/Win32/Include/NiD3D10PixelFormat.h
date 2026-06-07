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

#ifndef NID3D10PIXELFORMAT_H
#define NID3D10PIXELFORMAT_H

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

#include <NiPixelFormat.h>
#include <NiDataStreamElement.h>

class NID3D10RENDERER_ENTRY NiD3D10PixelFormat
{
public:
    static DXGI_FORMAT DetermineDXGIFormat(const NiPixelFormat& kDesc);
    static DXGI_FORMAT DetermineDXGIFormat(
        NiDataStreamElement::Format eFormat);
    static void InitFromDXGIFormat(DXGI_FORMAT eDXGIFormat, 
        NiPixelFormat& kFormat);
    static const NiPixelFormat* ObtainFromDXGIFormat(DXGI_FORMAT eDXGIFormat);

    static unsigned int GetBitsPerPixel(DXGI_FORMAT eDXGIFormat);

    static const char* const GetFormatName(DXGI_FORMAT eDXGIFormat,
        bool bWithPrefix = true);
    
    static bool IsDXGIFormatSupported(DXGI_FORMAT eDXGIFormat);
    static bool DXGIFormatSupportsMipmaps(DXGI_FORMAT eDXGIFormat);
 
//---------------------------------------------------------------------------

    static NiPixelFormat NI_FORMAT_R32G32B32A32_TYPELESS;
    static NiPixelFormat NI_FORMAT_R32G32B32A32_FLOAT;
    static NiPixelFormat NI_FORMAT_R32G32B32A32_UINT;
    static NiPixelFormat NI_FORMAT_R32G32B32A32_SINT;
    static NiPixelFormat NI_FORMAT_R32G32B32_TYPELESS;
    static NiPixelFormat NI_FORMAT_R32G32B32_FLOAT;
    static NiPixelFormat NI_FORMAT_R32G32B32_UINT;
    static NiPixelFormat NI_FORMAT_R32G32B32_SINT;
    static NiPixelFormat NI_FORMAT_R16G16B16A16_TYPELESS;
    static NiPixelFormat NI_FORMAT_R16G16B16A16_FLOAT;
    static NiPixelFormat NI_FORMAT_R16G16B16A16_UNORM;
    static NiPixelFormat NI_FORMAT_R16G16B16A16_UINT;
    static NiPixelFormat NI_FORMAT_R16G16B16A16_SNORM;
    static NiPixelFormat NI_FORMAT_R16G16B16A16_SINT;
    static NiPixelFormat NI_FORMAT_R32G32_TYPELESS;
    static NiPixelFormat NI_FORMAT_R32G32_FLOAT;
    static NiPixelFormat NI_FORMAT_R32G32_UINT;
    static NiPixelFormat NI_FORMAT_R32G32_SINT;
    static NiPixelFormat NI_FORMAT_R32G8X24_TYPELESS;
    static NiPixelFormat NI_FORMAT_D32_FLOAT_S8X24_UINT;
    static NiPixelFormat NI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    static NiPixelFormat NI_FORMAT_X32_TYPELESS_G8X24_UINT;
    static NiPixelFormat NI_FORMAT_R10G10B10A2_TYPELESS;
    static NiPixelFormat NI_FORMAT_R10G10B10A2_UNORM;
    static NiPixelFormat NI_FORMAT_R10G10B10A2_UINT;
    static NiPixelFormat NI_FORMAT_R11G11B10_FLOAT;
    static NiPixelFormat NI_FORMAT_R8G8B8A8_TYPELESS;
    static NiPixelFormat NI_FORMAT_R8G8B8A8_UNORM;
    static NiPixelFormat NI_FORMAT_R8G8B8A8_UNORM_SRGB;
    static NiPixelFormat NI_FORMAT_R8G8B8A8_UINT;
    static NiPixelFormat NI_FORMAT_R8G8B8A8_SNORM;
    static NiPixelFormat NI_FORMAT_R8G8B8A8_SINT;
    static NiPixelFormat NI_FORMAT_R16G16_TYPELESS;
    static NiPixelFormat NI_FORMAT_R16G16_FLOAT;
    static NiPixelFormat NI_FORMAT_R16G16_UNORM;
    static NiPixelFormat NI_FORMAT_R16G16_UINT;
    static NiPixelFormat NI_FORMAT_R16G16_SNORM;
    static NiPixelFormat NI_FORMAT_R16G16_SINT;
    static NiPixelFormat NI_FORMAT_R32_TYPELESS;
    static NiPixelFormat NI_FORMAT_D32_FLOAT;
    static NiPixelFormat NI_FORMAT_R32_FLOAT;
    static NiPixelFormat NI_FORMAT_R32_UINT;
    static NiPixelFormat NI_FORMAT_R32_SINT;
    static NiPixelFormat NI_FORMAT_R24G8_TYPELESS;
    static NiPixelFormat NI_FORMAT_D24_UNORM_S8_UINT;
    static NiPixelFormat NI_FORMAT_R24_UNORM_X8_TYPELESS;
    static NiPixelFormat NI_FORMAT_X24_TYPELESS_G8_UINT;
    static NiPixelFormat NI_FORMAT_R8G8_TYPELESS;
    static NiPixelFormat NI_FORMAT_R8G8_UNORM;
    static NiPixelFormat NI_FORMAT_R8G8_UINT;
    static NiPixelFormat NI_FORMAT_R8G8_SNORM;
    static NiPixelFormat NI_FORMAT_R8G8_SINT;
    static NiPixelFormat NI_FORMAT_R16_TYPELESS;
    static NiPixelFormat NI_FORMAT_R16_FLOAT;
    static NiPixelFormat NI_FORMAT_D16_UNORM;
    static NiPixelFormat NI_FORMAT_R16_UNORM;
    static NiPixelFormat NI_FORMAT_R16_UINT;
    static NiPixelFormat NI_FORMAT_R16_SNORM;
    static NiPixelFormat NI_FORMAT_R16_SINT;
    static NiPixelFormat NI_FORMAT_R8_TYPELESS;
    static NiPixelFormat NI_FORMAT_R8_UNORM;
    static NiPixelFormat NI_FORMAT_R8_UINT;
    static NiPixelFormat NI_FORMAT_R8_SNORM;
    static NiPixelFormat NI_FORMAT_R8_SINT;
    static NiPixelFormat NI_FORMAT_A8_UNORM;
    static NiPixelFormat NI_FORMAT_R1_UNORM;
    static NiPixelFormat NI_FORMAT_R9G9B9E5_SHAREDEXP;
    static NiPixelFormat NI_FORMAT_R8G8_B8G8_UNORM;
    static NiPixelFormat NI_FORMAT_G8R8_G8B8_UNORM;
    static NiPixelFormat NI_FORMAT_BC1_TYPELESS;
    static NiPixelFormat NI_FORMAT_BC1_UNORM;
    static NiPixelFormat NI_FORMAT_BC1_UNORM_SRGB;
    static NiPixelFormat NI_FORMAT_BC2_TYPELESS;
    static NiPixelFormat NI_FORMAT_BC2_UNORM;
    static NiPixelFormat NI_FORMAT_BC2_UNORM_SRGB;
    static NiPixelFormat NI_FORMAT_BC3_TYPELESS;
    static NiPixelFormat NI_FORMAT_BC3_UNORM;
    static NiPixelFormat NI_FORMAT_BC3_UNORM_SRGB;
    static NiPixelFormat NI_FORMAT_BC4_TYPELESS;
    static NiPixelFormat NI_FORMAT_BC4_UNORM;
    static NiPixelFormat NI_FORMAT_BC4_SNORM;
    static NiPixelFormat NI_FORMAT_BC5_TYPELESS;
    static NiPixelFormat NI_FORMAT_BC5_UNORM;
    static NiPixelFormat NI_FORMAT_BC5_SNORM;
    static NiPixelFormat NI_FORMAT_B5G6R5_UNORM;
    static NiPixelFormat NI_FORMAT_B5G5R5A1_UNORM;
    static NiPixelFormat NI_FORMAT_B8G8R8A8_UNORM;
    static NiPixelFormat NI_FORMAT_B8G8R8X8_UNORM;
};    

#endif  //#ifndef NID3D10PIXELFORMAT_H
