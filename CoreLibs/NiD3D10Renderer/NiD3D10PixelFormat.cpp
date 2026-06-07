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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10PixelFormat.h"

NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_TYPELESS(
    NiPixelFormat::FORMAT_RGBA, 128, NiPixelFormat::TILE_NONE, true, 
    DXGI_FORMAT_R32G32B32A32_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_UNKNOWN, 32, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_FLOAT(
    NiPixelFormat::FORMAT_RGBA, 128, NiPixelFormat::TILE_NONE, true, 
    DXGI_FORMAT_R32G32B32A32_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_FLOAT,   32, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_UINT(
    NiPixelFormat::FORMAT_RGBA, 128, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32B32A32_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,     32, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32A32_SINT(
    NiPixelFormat::FORMAT_RGBA, 128, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32B32A32_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,     32, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 96, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32B32_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32_FLOAT(
    NiPixelFormat::FORMAT_RGB, 96, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32B32_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32_UINT(
    NiPixelFormat::FORMAT_RGB, 96, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32B32_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32B32_SINT(
    NiPixelFormat::FORMAT_RGB, 96, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32B32_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_TYPELESS(
    NiPixelFormat::FORMAT_RGBA, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16B16A16_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 16, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN, 16, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_UNKNOWN, 16, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_UNKNOWN, 16, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_FLOAT(
    NiPixelFormat::FORMAT_RGBA, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16B16A16_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_HALF,    16, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_HALF,    16, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_HALF,    16, true,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_HALF,    16, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_UNORM(
    NiPixelFormat::FORMAT_RGBA, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16B16A16_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,   16, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,   16, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,   16, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_UINT(
    NiPixelFormat::FORMAT_RGBA, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16B16A16_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     16, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     16, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     16, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,     16, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_SNORM(
    NiPixelFormat::FORMAT_BUMPLUMA, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16B16A16_SNORM, false, 
    NiPixelFormat::COMP_OFFSET_U,   NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_OFFSET_V,   NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_LUMA,       NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_HEIGHT,     NiPixelFormat::REP_NORM_INT,   16, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16B16A16_SINT(
    NiPixelFormat::FORMAT_RGBA, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16B16A16_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     16, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     16, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     16, true,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,     16, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32_FLOAT(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32_UINT(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G32_SINT(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G32_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32G8X24_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32G8X24_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN, 24, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_D32_FLOAT_S8X24_UINT(
    NiPixelFormat::FORMAT_DEPTH_STENCIL, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT, false, 
    NiPixelFormat::COMP_DEPTH,  NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_STENCIL,NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN, 24, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT_X8X24_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN, 24, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_X32_TYPELESS_G8X24_UINT(
    NiPixelFormat::FORMAT_RGB, 64, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT, false, 
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN, 24, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R10G10B10A2_TYPELESS(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R10G10B10A2_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 10, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN, 10, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_UNKNOWN, 10, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_UNKNOWN,  2, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R10G10B10A2_UNORM(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R10G10B10A2_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,   10, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,   10, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,   10, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,    2, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R10G10B10A2_UINT(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R10G10B10A2_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     10, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     10, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,     10, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,      2, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R11G11B10_FLOAT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R11G11B10_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_FLOAT,   11, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_FLOAT,   11, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_FLOAT,   10, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_TYPELESS(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8B8A8_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_UNKNOWN,  8, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8B8A8_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,    8, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UNORM_SRGB(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, true, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,    8, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_UINT(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8B8A8_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,      8, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_SNORM(
    NiPixelFormat::FORMAT_BUMPLUMA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8B8A8_SNORM, false, 
    NiPixelFormat::COMP_OFFSET_U,   NiPixelFormat::REP_NORM_INT,    8, true,
    NiPixelFormat::COMP_OFFSET_V,   NiPixelFormat::REP_NORM_INT,    8, true,
    NiPixelFormat::COMP_LUMA,       NiPixelFormat::REP_NORM_INT,    8, true,
    NiPixelFormat::COMP_HEIGHT,     NiPixelFormat::REP_NORM_INT,    8, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8B8A8_SINT(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8B8A8_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,      8, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,      8, true,
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_INT,      8, true,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_INT,      8, true);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 16, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN, 16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16_FLOAT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_HALF,    16, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_HALF,    16, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16_UNORM(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,   16, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,   16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16_UINT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     16, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16_SNORM(
    NiPixelFormat::FORMAT_BUMP, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16_SNORM, false, 
    NiPixelFormat::COMP_OFFSET_U,   NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_OFFSET_V,   NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16G16_SINT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16G16_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     16, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,     16, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 32, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_D32_FLOAT(
    NiPixelFormat::FORMAT_DEPTH_STENCIL, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_D32_FLOAT, false, 
    NiPixelFormat::COMP_DEPTH,  NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32_FLOAT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_FLOAT,   32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32_UINT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R32_SINT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R32_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     32, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R24G8_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R24G8_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 24, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN,  8, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_D24_UNORM_S8_UINT(
    NiPixelFormat::FORMAT_DEPTH_STENCIL, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_D24_UNORM_S8_UINT, false, 
    NiPixelFormat::COMP_DEPTH,  NiPixelFormat::REP_NORM_INT,   24, false,
    NiPixelFormat::COMP_STENCIL,NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R24_UNORM_X8_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,   24, false,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_X24_TYPELESS_G8_UINT(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT, false, 
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN, 24, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8_UNORM(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8_UINT(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8_SNORM(
    NiPixelFormat::FORMAT_BUMP, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8_SNORM, false, 
    NiPixelFormat::COMP_OFFSET_U,   NiPixelFormat::REP_NORM_INT,    8, true,
    NiPixelFormat::COMP_OFFSET_V,   NiPixelFormat::REP_NORM_INT,    8, true,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8_SINT(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,      8, true,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_INT,      8, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN, 16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16_FLOAT(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16_FLOAT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_HALF,    16, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_D16_UNORM(
    NiPixelFormat::FORMAT_DEPTH_STENCIL, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_D16_UNORM, false, 
    NiPixelFormat::COMP_DEPTH,  NiPixelFormat::REP_NORM_INT,   16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16_UNORM(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,   16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16_UINT(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     16, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16_SNORM(
    NiPixelFormat::FORMAT_BUMP, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16_SNORM, false, 
    NiPixelFormat::COMP_OFFSET_U,   NiPixelFormat::REP_NORM_INT,   16, true,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R16_SINT(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R16_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,     16, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8_TYPELESS(
    NiPixelFormat::FORMAT_RGB, 8, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8_TYPELESS, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_UNKNOWN,  8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8_UNORM(
    NiPixelFormat::FORMAT_RGB, 8, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8_UINT(
    NiPixelFormat::FORMAT_RGB, 8, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8_UINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,      8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8_SNORM(
    NiPixelFormat::FORMAT_BUMP, 8, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8_SNORM, false, 
    NiPixelFormat::COMP_OFFSET_U,   NiPixelFormat::REP_NORM_INT,    8, true,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,      NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8_SINT(
    NiPixelFormat::FORMAT_RGB, 8, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8_SINT, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_INT,      8, true,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_A8_UNORM(
    NiPixelFormat::FORMAT_RGBA, 8, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_A8_UNORM, false, 
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R1_UNORM(
    NiPixelFormat::FORMAT_RGB, 1, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R1_UNORM, false, 
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    1, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R9G9B9E5_SHAREDEXP(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_R8G8_B8G8_UNORM(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_R8G8_B8G8_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_G8R8_G8B8_UNORM(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_G8R8_G8B8_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC1_TYPELESS(
    NiPixelFormat::FORMAT_DXT1, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC1_TYPELESS, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM(
    NiPixelFormat::FORMAT_DXT1, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC1_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC1_UNORM_SRGB(
    NiPixelFormat::FORMAT_DXT1, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC1_UNORM_SRGB, true, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC2_TYPELESS(
    NiPixelFormat::FORMAT_DXT3, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC2_TYPELESS, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM(
    NiPixelFormat::FORMAT_DXT3, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC2_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC2_UNORM_SRGB(
    NiPixelFormat::FORMAT_DXT3, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC2_UNORM_SRGB, true, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC3_TYPELESS(
    NiPixelFormat::FORMAT_DXT5, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC3_TYPELESS, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM(
    NiPixelFormat::FORMAT_DXT5, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC3_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC3_UNORM_SRGB(
    NiPixelFormat::FORMAT_DXT5, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC3_UNORM_SRGB, true, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC4_TYPELESS(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC4_TYPELESS, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC4_UNORM(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC4_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC4_SNORM(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC4_SNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC5_TYPELESS(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC5_TYPELESS, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC5_UNORM(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC5_UNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_BC5_SNORM(
    NiPixelFormat::FORMAT_RENDERERSPECIFIC, 0, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_BC5_SNORM, false, 
    NiPixelFormat::COMP_COMPRESSED,NiPixelFormat::REP_COMPRESSED, 0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_B5G6R5_UNORM(
    NiPixelFormat::FORMAT_RGB, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_B5G6R5_UNORM, false, 
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,    5, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    6, false,
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    5, false,
    NiPixelFormat::COMP_EMPTY,  NiPixelFormat::REP_UNKNOWN,  0, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_B5G5R5A1_UNORM(
    NiPixelFormat::FORMAT_RGBA, 16, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_B5G5R5A1_UNORM, false, 
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,    5, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    5, false,
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    5, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,    1, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_B8G8R8A8_UNORM(
    NiPixelFormat::FORMAT_RGBA, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_B8G8R8A8_UNORM, false, 
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_ALPHA,  NiPixelFormat::REP_NORM_INT,    8, false);
NiPixelFormat NiD3D10PixelFormat::NI_FORMAT_B8G8R8X8_UNORM(
    NiPixelFormat::FORMAT_RGB, 32, NiPixelFormat::TILE_NONE, true,
    DXGI_FORMAT_B8G8R8X8_UNORM, false, 
    NiPixelFormat::COMP_BLUE,   NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_GREEN,  NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_RED,    NiPixelFormat::REP_NORM_INT,    8, false,
    NiPixelFormat::COMP_PADDING,NiPixelFormat::REP_UNKNOWN,  8, false);

//---------------------------------------------------------------------------
DXGI_FORMAT NiD3D10PixelFormat::DetermineDXGIFormat(
    const NiPixelFormat& kDesc)
{
    DXGI_FORMAT eDXGIFormat = (DXGI_FORMAT)kDesc.GetRendererHint();
    if (eDXGIFormat != NiPixelFormat::INVALID_RENDERER_HINT)
        return eDXGIFormat;

    NiPixelFormat::Format eFormat = kDesc.GetFormat();
    unsigned char ucBPP = kDesc.GetBitsPerPixel();
    unsigned int uiComponentCount = kDesc.GetNumComponents();

    NiPixelFormat::Component eComponent = NiPixelFormat::COMP_EMPTY;
    NiPixelFormat::Representation eRep = NiPixelFormat::REP_UNKNOWN;
    unsigned char ucBits = 0;
    bool bSigned = false;
    if (!kDesc.GetComponent(0, eComponent, eRep, ucBits, bSigned))
        return DXGI_FORMAT_UNKNOWN;

    eDXGIFormat = DXGI_FORMAT_UNKNOWN;
    switch (eFormat)
    {
    case NiPixelFormat::FORMAT_ONE_CHANNEL:
    case NiPixelFormat::FORMAT_TWO_CHANNEL:
    case NiPixelFormat::FORMAT_THREE_CHANNEL:
    case NiPixelFormat::FORMAT_FOUR_CHANNEL:
    case NiPixelFormat::FORMAT_RGB:
    case NiPixelFormat::FORMAT_RGBA:
    case NiPixelFormat::FORMAT_BUMP:
    case NiPixelFormat::FORMAT_BUMPLUMA:
        switch (ucBPP)
        {
        case 1:
            eDXGIFormat = DXGI_FORMAT_R1_UNORM;
            break;
        case 8:
            if (uiComponentCount == 1)
            {
                if (eRep == NiPixelFormat::REP_NORM_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R8_SNORM;
                    else
                        eDXGIFormat = DXGI_FORMAT_R8_UNORM;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R8_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R8_UINT;
                }
                else
                {
                    eDXGIFormat = DXGI_FORMAT_R8_TYPELESS;
                }
            }
            break;
        case 16:
            if (uiComponentCount == 1)
            {
                if (eRep == NiPixelFormat::REP_FLOAT ||
                    eRep == NiPixelFormat::REP_HALF)
                {
                    eDXGIFormat = DXGI_FORMAT_R16_FLOAT;
                }
                else if (eRep == NiPixelFormat::REP_NORM_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R16_SNORM;
                    else
                        eDXGIFormat = DXGI_FORMAT_R16_UNORM;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R16_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R16_UINT;
                }
                else 
                {
                    eDXGIFormat = DXGI_FORMAT_R16_TYPELESS;
                }
            }
            else if (uiComponentCount == 2)
            {
                if (eRep == NiPixelFormat::REP_NORM_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R8G8_SNORM;
                    else
                        eDXGIFormat = DXGI_FORMAT_R8G8_UNORM;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R8G8_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R8G8_UINT;
                }
                else 
                {
                    eDXGIFormat = DXGI_FORMAT_R8G8_TYPELESS;
                }
                break;
            }
            else if (uiComponentCount == 3)
            {
                eDXGIFormat = DXGI_FORMAT_B5G6R5_UNORM;
            }
            else if (uiComponentCount == 4)
            {
                eDXGIFormat = DXGI_FORMAT_B5G5R5A1_UNORM;
            }
            break;
        case 32:
            if (ucBits == 11)
            {
                eDXGIFormat = DXGI_FORMAT_R11G11B10_FLOAT;
            }
            else if (ucBits == 10)
            {
                if (eRep == NiPixelFormat::REP_NORM_INT)
                    eDXGIFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
                else if (eRep == NiPixelFormat::REP_INT)
                    eDXGIFormat = DXGI_FORMAT_R10G10B10A2_UINT;
                else if (eRep == NiPixelFormat::REP_UNKNOWN)
                    eDXGIFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
            }
            else if (ucBits == 8)
            {
                if (kDesc.GetSRGBSpace())
                {
                    eDXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                }
                else
                {
                    if (eRep == NiPixelFormat::REP_NORM_INT)
                    {
                        if (eComponent == NiPixelFormat::COMP_RED)
                        {
                            if (bSigned)
                                eDXGIFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
                            else
                                eDXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
                        }
                        else
                        {
                            if (kDesc.GetBits(NiPixelFormat::COMP_ALPHA) != 0)
                                eDXGIFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
                            else
                                eDXGIFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
                        }
                    }
                    else if (eRep == NiPixelFormat::REP_INT)
                    {
                        if (bSigned)
                            eDXGIFormat = DXGI_FORMAT_R8G8B8A8_SINT;
                        else
                            eDXGIFormat = DXGI_FORMAT_R8G8B8A8_UINT;
                    }
                    else if (eRep == NiPixelFormat::REP_UNKNOWN)
                    {
                        eDXGIFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
                    }
                }
            }
            else if (ucBits == 16)
            {
                if (eRep == NiPixelFormat::REP_FLOAT ||
                    eRep == NiPixelFormat::REP_HALF)
                {
                    eDXGIFormat = DXGI_FORMAT_R16G16_FLOAT;
                }
                else if (eRep == NiPixelFormat::REP_NORM_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R16G16_SNORM;
                    else
                        eDXGIFormat = DXGI_FORMAT_R16G16_UNORM;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R16G16_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R16G16_UINT;
                }
                else if (eRep == NiPixelFormat::REP_UNKNOWN)
                {
                    eDXGIFormat = DXGI_FORMAT_R16G16_TYPELESS;
                }
            }
            else if (ucBits == 24)
            {
                if (eRep == NiPixelFormat::REP_NORM_INT)
                {
                    eDXGIFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
                }
                else if (eRep == NiPixelFormat::REP_UNKNOWN)
                {
                    if (eComponent == NiPixelFormat::COMP_RED)
                        eDXGIFormat = DXGI_FORMAT_R24G8_TYPELESS;
                    else
                        eDXGIFormat = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
                }
            }
            else if (ucBits == 32)
            {
                if (eRep == NiPixelFormat::REP_FLOAT)
                {
                    eDXGIFormat = DXGI_FORMAT_R32_FLOAT;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R32_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R32_UINT;
                }
                else if (eRep == NiPixelFormat::REP_UNKNOWN)
                {
                    eDXGIFormat = DXGI_FORMAT_R32_TYPELESS;
                }
            }
            break;
        case 64:
            if (uiComponentCount == 1)
            {
                if (eRep == NiPixelFormat::REP_FLOAT)
                    eDXGIFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                else if (eRep == NiPixelFormat::REP_UNKNOWN)
                    eDXGIFormat = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
            }
            else if (uiComponentCount == 2)
            {
                if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R32G32_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R32G32_UINT;
                }
                else
                {
                    if (eRep == NiPixelFormat::REP_FLOAT)
                    {
                        if (kDesc.GetBits(NiPixelFormat::COMP_GREEN) != 0)
                            eDXGIFormat = DXGI_FORMAT_R32G32_FLOAT;
                        else
                            eDXGIFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
                    }
                    else if (eRep == NiPixelFormat::REP_UNKNOWN)
                    {
                        if (eComponent == NiPixelFormat::COMP_RED)
                        {

                            if (kDesc.GetBits(NiPixelFormat::COMP_GREEN) == 32)
                                eDXGIFormat = DXGI_FORMAT_R32G32_TYPELESS;
                            else
                                eDXGIFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
                        }
                        else if (eComponent == NiPixelFormat::COMP_PADDING)
                        {
                            eDXGIFormat = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
                        }
                    }

                }
            }
            else if (uiComponentCount == 4)
            {
                if (eRep == NiPixelFormat::REP_FLOAT ||
                    eRep == NiPixelFormat::REP_HALF)
                {
                    eDXGIFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
                }
                else if (eRep == NiPixelFormat::REP_NORM_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R16G16B16A16_SNORM;
                    else
                        eDXGIFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R16G16B16A16_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R16G16B16A16_UINT;
                }
                else if (eRep == NiPixelFormat::REP_UNKNOWN)
                {
                    eDXGIFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
                }
            }
            break;
        case 96:
            if (uiComponentCount == 3)
            {
                if (eRep == NiPixelFormat::REP_FLOAT)
                {
                    eDXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R32G32B32_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R32G32B32_UINT;
                }
                else
                {
                    eDXGIFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
                }
            }
            break;
        case 128:
            if (uiComponentCount == 4)
            {
                if (eRep == NiPixelFormat::REP_FLOAT)
                {
                    eDXGIFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
                }
                else if (eRep == NiPixelFormat::REP_INT)
                {
                    if (bSigned)
                        eDXGIFormat = DXGI_FORMAT_R32G32B32A32_SINT;
                    else
                        eDXGIFormat = DXGI_FORMAT_R32G32B32A32_UINT;
                }
                else
                {
                    eDXGIFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
                }
            }
            break;
        }
        break;
    case NiPixelFormat::FORMAT_DXT1:
        if (kDesc.GetSRGBSpace())
            eDXGIFormat = DXGI_FORMAT_BC1_UNORM_SRGB;
        else
            eDXGIFormat = DXGI_FORMAT_BC1_UNORM;
        break;
    case NiPixelFormat::FORMAT_DXT3:
        if (kDesc.GetSRGBSpace())
            eDXGIFormat = DXGI_FORMAT_BC2_UNORM_SRGB;
        else
            eDXGIFormat = DXGI_FORMAT_BC2_UNORM;
        break;
    case NiPixelFormat::FORMAT_DXT5:
        if (kDesc.GetSRGBSpace())
            eDXGIFormat = DXGI_FORMAT_BC3_UNORM_SRGB;
        else
            eDXGIFormat = DXGI_FORMAT_BC3_UNORM;
        break;
    case NiPixelFormat::FORMAT_DEPTH_STENCIL:
        if (ucBPP == 64)
        {
            eDXGIFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        }
        else if (ucBPP == 32)
        {
            if (kDesc.GetBits(NiPixelFormat::COMP_STENCIL) == 0)
                eDXGIFormat = DXGI_FORMAT_D32_FLOAT;
            else
                eDXGIFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        }
        else if (ucBPP == 16)
        {
            eDXGIFormat = DXGI_FORMAT_D16_UNORM;
        }
        break;
    case NiPixelFormat::FORMAT_PAL:
    case NiPixelFormat::FORMAT_PALALPHA:
    case NiPixelFormat::FORMAT_RENDERERSPECIFIC:
    default:
        eDXGIFormat = DXGI_FORMAT_UNKNOWN;
        break;
    }

    return eDXGIFormat;
}
//---------------------------------------------------------------------------
DXGI_FORMAT NiD3D10PixelFormat::DetermineDXGIFormat(
    NiDataStreamElement::Format eFormat)
{
    switch (eFormat)
    {
    case NiDataStreamElement::F_INT8_1:
        return DXGI_FORMAT_R8_SINT;
    case NiDataStreamElement::F_INT8_2:
        return DXGI_FORMAT_R8G8_SINT;
    case NiDataStreamElement::F_INT8_4:
        return DXGI_FORMAT_R8G8B8A8_SINT;
    case NiDataStreamElement::F_UINT8_1:
        return DXGI_FORMAT_R8_UINT;
    case NiDataStreamElement::F_UINT8_2:
        return DXGI_FORMAT_R8G8_UINT;
    case NiDataStreamElement::F_UINT8_4:
        return DXGI_FORMAT_R8G8B8A8_UINT;
    case NiDataStreamElement::F_NORMINT8_1:
        return DXGI_FORMAT_R8_SNORM;
    case NiDataStreamElement::F_NORMINT8_2:
        return DXGI_FORMAT_R8G8_SNORM;
    case NiDataStreamElement::F_NORMINT8_4:
        return DXGI_FORMAT_R8G8B8A8_SNORM;
    case NiDataStreamElement::F_NORMUINT8_1:
        return DXGI_FORMAT_R8_UNORM;
    case NiDataStreamElement::F_NORMUINT8_2:
        return DXGI_FORMAT_R8G8_UNORM;
    case NiDataStreamElement::F_NORMUINT8_4:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case NiDataStreamElement::F_INT16_1:
        return DXGI_FORMAT_R16_SINT;
    case NiDataStreamElement::F_INT16_2:
        return DXGI_FORMAT_R16G16_SINT;
    case NiDataStreamElement::F_INT16_4:
        return DXGI_FORMAT_R16G16B16A16_SINT;
    case NiDataStreamElement::F_UINT16_1:
        return DXGI_FORMAT_R16_UINT;
    case NiDataStreamElement::F_UINT16_2:
        return DXGI_FORMAT_R16G16_UINT;
    case NiDataStreamElement::F_UINT16_4:
        return DXGI_FORMAT_R16G16B16A16_UINT;
    case NiDataStreamElement::F_NORMINT16_1:
        return DXGI_FORMAT_R16_SNORM;
    case NiDataStreamElement::F_NORMINT16_2:
        return DXGI_FORMAT_R16G16_SNORM;
    case NiDataStreamElement::F_NORMINT16_4:
        return DXGI_FORMAT_R16G16B16A16_SNORM;
    case NiDataStreamElement::F_NORMUINT16_1:
        return DXGI_FORMAT_R16_UNORM;
    case NiDataStreamElement::F_NORMUINT16_2:
        return DXGI_FORMAT_R16G16_UNORM;
    case NiDataStreamElement::F_NORMUINT16_4:
        return DXGI_FORMAT_R16G16B16A16_UNORM;
    case NiDataStreamElement::F_INT32_1:
        return DXGI_FORMAT_R32_SINT;
    case NiDataStreamElement::F_INT32_2:
        return DXGI_FORMAT_R32G32_SINT;
    case NiDataStreamElement::F_INT32_4:
        return DXGI_FORMAT_R32G32B32A32_SINT;
    case NiDataStreamElement::F_UINT32_1:
        return DXGI_FORMAT_R32_UINT;
    case NiDataStreamElement::F_UINT32_2:
        return DXGI_FORMAT_R32G32_UINT;
    case NiDataStreamElement::F_UINT32_4:
        return DXGI_FORMAT_R32G32B32A32_UINT;
    case NiDataStreamElement::F_FLOAT16_1:
        return DXGI_FORMAT_R16_FLOAT;
    case NiDataStreamElement::F_FLOAT16_2:
        return DXGI_FORMAT_R16G16_FLOAT;
    case NiDataStreamElement::F_FLOAT16_4:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case NiDataStreamElement::F_FLOAT32_1:
        return DXGI_FORMAT_R32_FLOAT;
    case NiDataStreamElement::F_FLOAT32_2:
        return DXGI_FORMAT_R32G32_FLOAT;
    case NiDataStreamElement::F_FLOAT32_3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case NiDataStreamElement::F_FLOAT32_4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case NiDataStreamElement::F_UINT_10_10_10_2:
        return DXGI_FORMAT_R10G10B10A2_UINT;
    case NiDataStreamElement::F_UINT_10_10_10_L1:
    case NiDataStreamElement::F_NORMINT_10_10_10_L1:
    case NiDataStreamElement::F_NORMINT_10_10_10_2:
    case NiDataStreamElement::F_INT8_3:
    case NiDataStreamElement::F_UINT8_3:
    case NiDataStreamElement::F_NORMINT8_3:
    case NiDataStreamElement::F_NORMUINT8_3:
    case NiDataStreamElement::F_INT16_3:
    case NiDataStreamElement::F_UINT16_3:
    case NiDataStreamElement::F_NORMINT16_3:
    case NiDataStreamElement::F_NORMUINT16_3:
    case NiDataStreamElement::F_INT32_3:
    case NiDataStreamElement::F_UINT32_3:
    case NiDataStreamElement::F_NORMINT32_1:
    case NiDataStreamElement::F_NORMINT32_2:
    case NiDataStreamElement::F_NORMINT32_3:
    case NiDataStreamElement::F_NORMINT32_4:
    case NiDataStreamElement::F_NORMUINT32_1:
    case NiDataStreamElement::F_NORMUINT32_2:
    case NiDataStreamElement::F_NORMUINT32_3:
    case NiDataStreamElement::F_NORMUINT32_4:
    case NiDataStreamElement::F_FLOAT16_3:
    case NiDataStreamElement::F_NORMINT_11_11_10:
    case NiDataStreamElement::F_NORMUINT8_4_BGRA:
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}
//---------------------------------------------------------------------------
 bool NiD3D10PixelFormat::IsDXGIFormatSupported( DXGI_FORMAT eDXGIFormat )
 {
     switch (eDXGIFormat)
     {
     case DXGI_FORMAT_B8G8R8X8_UNORM:
         return false;
     }
     return true;
 }
//---------------------------------------------------------------------------
 bool NiD3D10PixelFormat::DXGIFormatSupportsMipmaps( DXGI_FORMAT eDXGIFormat )
 {
     switch (eDXGIFormat)
     {
     case DXGI_FORMAT_B8G8R8X8_UNORM:
         return false;
     }
     return true;
 }
//---------------------------------------------------------------------------
void NiD3D10PixelFormat::InitFromDXGIFormat(DXGI_FORMAT eDXGIFormat, 
    NiPixelFormat& kFormat)
{
    // CreateFromDXGIFormat only ever returns an existing NiPixelFormat,
    // and will not create a new one.
    const NiPixelFormat* pkFormat = ObtainFromDXGIFormat(eDXGIFormat);
    kFormat = *pkFormat;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10PixelFormat::ObtainFromDXGIFormat(
    DXGI_FORMAT eDXGIFormat)
{
    switch (eDXGIFormat)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return &NI_FORMAT_R32G32B32A32_TYPELESS;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return &NI_FORMAT_R32G32B32A32_FLOAT;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return &NI_FORMAT_R32G32B32A32_UINT;
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return &NI_FORMAT_R32G32B32A32_SINT;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        return &NI_FORMAT_R32G32B32_TYPELESS;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        return &NI_FORMAT_R32G32B32_FLOAT;
    case DXGI_FORMAT_R32G32B32_UINT:
        return &NI_FORMAT_R32G32B32_UINT;
    case DXGI_FORMAT_R32G32B32_SINT:
        return &NI_FORMAT_R32G32B32_SINT;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return &NI_FORMAT_R16G16B16A16_TYPELESS;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return &NI_FORMAT_R16G16B16A16_FLOAT;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return &NI_FORMAT_R16G16B16A16_UNORM;
    case DXGI_FORMAT_R16G16B16A16_UINT:
        return &NI_FORMAT_R16G16B16A16_UINT;
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        return &NI_FORMAT_R16G16B16A16_SNORM;
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return &NI_FORMAT_R16G16B16A16_SINT;
    case DXGI_FORMAT_R32G32_TYPELESS:
        return &NI_FORMAT_R32G32_TYPELESS;
    case DXGI_FORMAT_R32G32_FLOAT:
        return &NI_FORMAT_R32G32_FLOAT;
    case DXGI_FORMAT_R32G32_UINT:
        return &NI_FORMAT_R32G32_UINT;
    case DXGI_FORMAT_R32G32_SINT:
        return &NI_FORMAT_R32G32_SINT;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
        return &NI_FORMAT_R32G8X24_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return &NI_FORMAT_D32_FLOAT_S8X24_UINT;
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        return &NI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return &NI_FORMAT_X32_TYPELESS_G8X24_UINT;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        return &NI_FORMAT_R10G10B10A2_TYPELESS;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        return &NI_FORMAT_R10G10B10A2_UNORM;
    case DXGI_FORMAT_R10G10B10A2_UINT:
        return &NI_FORMAT_R10G10B10A2_UINT;
    case DXGI_FORMAT_R11G11B10_FLOAT:
        return &NI_FORMAT_R11G11B10_FLOAT;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return &NI_FORMAT_R8G8B8A8_TYPELESS;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return &NI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return &NI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case DXGI_FORMAT_R8G8B8A8_UINT:
        return &NI_FORMAT_R8G8B8A8_UINT;
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        return &NI_FORMAT_R8G8B8A8_SNORM;
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return &NI_FORMAT_R8G8B8A8_SINT;
    case DXGI_FORMAT_R16G16_TYPELESS:
        return &NI_FORMAT_R16G16_TYPELESS;
    case DXGI_FORMAT_R16G16_FLOAT:
        return &NI_FORMAT_R16G16_FLOAT;
    case DXGI_FORMAT_R16G16_UNORM:
        return &NI_FORMAT_R16G16_UNORM;
    case DXGI_FORMAT_R16G16_UINT:
        return &NI_FORMAT_R16G16_UINT;
    case DXGI_FORMAT_R16G16_SNORM:
        return &NI_FORMAT_R16G16_SNORM;
    case DXGI_FORMAT_R16G16_SINT:
        return &NI_FORMAT_R16G16_SINT;
    case DXGI_FORMAT_R32_TYPELESS:
        return &NI_FORMAT_R32_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT:
        return &NI_FORMAT_D32_FLOAT;
    case DXGI_FORMAT_R32_FLOAT:
        return &NI_FORMAT_R32_FLOAT;
    case DXGI_FORMAT_R32_UINT:
        return &NI_FORMAT_R32_UINT;
    case DXGI_FORMAT_R32_SINT:
        return &NI_FORMAT_R32_SINT;
    case DXGI_FORMAT_R24G8_TYPELESS:
        return &NI_FORMAT_R24G8_TYPELESS;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return &NI_FORMAT_D24_UNORM_S8_UINT;
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        return &NI_FORMAT_R24_UNORM_X8_TYPELESS;
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return &NI_FORMAT_X24_TYPELESS_G8_UINT;
    case DXGI_FORMAT_R8G8_TYPELESS:
        return &NI_FORMAT_R8G8_TYPELESS;
    case DXGI_FORMAT_R8G8_UNORM:
        return &NI_FORMAT_R8G8_UNORM;
    case DXGI_FORMAT_R8G8_UINT:
        return &NI_FORMAT_R8G8_UINT;
    case DXGI_FORMAT_R8G8_SNORM:
        return &NI_FORMAT_R8G8_SNORM;
    case DXGI_FORMAT_R8G8_SINT:
        return &NI_FORMAT_R8G8_SINT;
    case DXGI_FORMAT_R16_TYPELESS:
        return &NI_FORMAT_R16_TYPELESS;
    case DXGI_FORMAT_R16_FLOAT:
        return &NI_FORMAT_R16_FLOAT;
    case DXGI_FORMAT_D16_UNORM:
        return &NI_FORMAT_D16_UNORM;
    case DXGI_FORMAT_R16_UNORM:
        return &NI_FORMAT_R16_UNORM;
    case DXGI_FORMAT_R16_UINT:
        return &NI_FORMAT_R16_UINT;
    case DXGI_FORMAT_R16_SNORM:
        return &NI_FORMAT_R16_SNORM;
    case DXGI_FORMAT_R16_SINT:
        return &NI_FORMAT_R16_SINT;
    case DXGI_FORMAT_R8_TYPELESS:
        return &NI_FORMAT_R8_TYPELESS;
    case DXGI_FORMAT_R8_UNORM:
        return &NI_FORMAT_R8_UNORM;
    case DXGI_FORMAT_R8_UINT:
        return &NI_FORMAT_R8_UINT;
    case DXGI_FORMAT_R8_SNORM:
        return &NI_FORMAT_R8_SNORM;
    case DXGI_FORMAT_R8_SINT:
        return &NI_FORMAT_R8_SINT;
    case DXGI_FORMAT_A8_UNORM:
        return &NI_FORMAT_A8_UNORM;
    case DXGI_FORMAT_R1_UNORM:
        return &NI_FORMAT_R1_UNORM;
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        return &NI_FORMAT_R9G9B9E5_SHAREDEXP;
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        return &NI_FORMAT_R8G8_B8G8_UNORM;
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        return &NI_FORMAT_G8R8_G8B8_UNORM;
    case DXGI_FORMAT_BC1_TYPELESS:
        return &NI_FORMAT_BC1_TYPELESS;
    case DXGI_FORMAT_BC1_UNORM:
        return &NI_FORMAT_BC1_UNORM;
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return &NI_FORMAT_BC1_UNORM_SRGB;
    case DXGI_FORMAT_BC2_TYPELESS:
        return &NI_FORMAT_BC2_TYPELESS;
    case DXGI_FORMAT_BC2_UNORM:
        return &NI_FORMAT_BC2_UNORM;
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return &NI_FORMAT_BC2_UNORM_SRGB;
    case DXGI_FORMAT_BC3_TYPELESS:
        return &NI_FORMAT_BC3_TYPELESS;
    case DXGI_FORMAT_BC3_UNORM:
        return &NI_FORMAT_BC3_UNORM;
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return &NI_FORMAT_BC3_UNORM_SRGB;
    case DXGI_FORMAT_BC4_TYPELESS:
        return &NI_FORMAT_BC4_TYPELESS;
    case DXGI_FORMAT_BC4_UNORM:
        return &NI_FORMAT_BC4_UNORM;
    case DXGI_FORMAT_BC4_SNORM:
        return &NI_FORMAT_BC4_SNORM;
    case DXGI_FORMAT_BC5_TYPELESS:
        return &NI_FORMAT_BC5_TYPELESS;
    case DXGI_FORMAT_BC5_UNORM:
        return &NI_FORMAT_BC5_UNORM;
    case DXGI_FORMAT_BC5_SNORM:
        return &NI_FORMAT_BC5_SNORM;
    case DXGI_FORMAT_B5G6R5_UNORM:
        return &NI_FORMAT_B5G6R5_UNORM;
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return &NI_FORMAT_B5G5R5A1_UNORM;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return &NI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return &NI_FORMAT_B8G8R8X8_UNORM;
    case DXGI_FORMAT_UNKNOWN:
    default:
        return NULL;
    }
}
//---------------------------------------------------------------------------
unsigned int NiD3D10PixelFormat::GetBitsPerPixel(
    DXGI_FORMAT eDXGIFormat)
{
    switch (eDXGIFormat)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return 32;
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return 16;
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;
    case DXGI_FORMAT_R1_UNORM:
        return 1;
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        return 0;
    case DXGI_FORMAT_UNKNOWN:
    default:
        return UINT_MAX;
    }
}
//---------------------------------------------------------------------------
const char* const NiD3D10PixelFormat::GetFormatName(DXGI_FORMAT eDXGIFormat,
    bool bWithPrefix)
{
    char* pcStr = NULL;
    switch (eDXGIFormat)
    {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            pcStr = "DXGI_FORMAT_R32G32B32A32_TYPELESS";
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            pcStr = "DXGI_FORMAT_R32G32B32A32_FLOAT";
            break;
        case DXGI_FORMAT_R32G32B32A32_UINT:
            pcStr = "DXGI_FORMAT_R32G32B32A32_UINT";
            break;
        case DXGI_FORMAT_R32G32B32A32_SINT:
            pcStr = "DXGI_FORMAT_R32G32B32A32_SINT";
            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            pcStr = "DXGI_FORMAT_R32G32B32_TYPELESS";
            break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            pcStr = "DXGI_FORMAT_R32G32B32_FLOAT";
            break;
        case DXGI_FORMAT_R32G32B32_UINT:
            pcStr = "DXGI_FORMAT_R32G32B32_UINT";
            break;
        case DXGI_FORMAT_R32G32B32_SINT:
            pcStr = "DXGI_FORMAT_R32G32B32_SINT";
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            pcStr = "DXGI_FORMAT_R16G16B16A16_TYPELESS";
            break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            pcStr = "DXGI_FORMAT_R16G16B16A16_FLOAT";
            break;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            pcStr = "DXGI_FORMAT_R16G16B16A16_UNORM";
            break;
        case DXGI_FORMAT_R16G16B16A16_UINT:
            pcStr = "DXGI_FORMAT_R16G16B16A16_UINT";
            break;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            pcStr = "DXGI_FORMAT_R16G16B16A16_SNORM";
            break;
        case DXGI_FORMAT_R16G16B16A16_SINT:
            pcStr = "DXGI_FORMAT_R16G16B16A16_SINT";
            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            pcStr = "DXGI_FORMAT_R32G32_TYPELESS";
            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            pcStr = "DXGI_FORMAT_R32G32_FLOAT";
            break;
        case DXGI_FORMAT_R32G32_UINT:
            pcStr = "DXGI_FORMAT_R32G32_UINT";
            break;
        case DXGI_FORMAT_R32G32_SINT:
            pcStr = "DXGI_FORMAT_R32G32_SINT";
            break;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            pcStr = "DXGI_FORMAT_R32G8X24_TYPELESS";
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            pcStr = "DXGI_FORMAT_D32_FLOAT_S8X24_UINT";
            break;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            pcStr = "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS";
            break;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            pcStr = "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT";
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            pcStr = "DXGI_FORMAT_R10G10B10A2_TYPELESS";
            break;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            pcStr = "DXGI_FORMAT_R10G10B10A2_UNORM";
            break;
        case DXGI_FORMAT_R10G10B10A2_UINT:
            pcStr = "DXGI_FORMAT_R10G10B10A2_UINT";
            break;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            pcStr = "DXGI_FORMAT_R11G11B10_FLOAT";
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            pcStr = "DXGI_FORMAT_R8G8B8A8_TYPELESS";
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            pcStr = "DXGI_FORMAT_R8G8B8A8_UNORM";
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            pcStr = "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB";
            break;
        case DXGI_FORMAT_R8G8B8A8_UINT:
            pcStr = "DXGI_FORMAT_R8G8B8A8_UINT";
            break;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            pcStr = "DXGI_FORMAT_R8G8B8A8_SNORM";
            break;
        case DXGI_FORMAT_R8G8B8A8_SINT:
            pcStr = "DXGI_FORMAT_R8G8B8A8_SINT";
            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            pcStr = "DXGI_FORMAT_R16G16_TYPELESS";
            break;
        case DXGI_FORMAT_R16G16_FLOAT:
            pcStr = "DXGI_FORMAT_R16G16_FLOAT";
            break;
        case DXGI_FORMAT_R16G16_UNORM:
            pcStr = "DXGI_FORMAT_R16G16_UNORM";
            break;
        case DXGI_FORMAT_R16G16_UINT:
            pcStr = "DXGI_FORMAT_R16G16_UINT";
            break;
        case DXGI_FORMAT_R16G16_SNORM:
            pcStr = "DXGI_FORMAT_R16G16_SNORM";
            break;
        case DXGI_FORMAT_R16G16_SINT:
            pcStr = "DXGI_FORMAT_R16G16_SINT";
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            pcStr = "DXGI_FORMAT_R32_TYPELESS";
            break;
        case DXGI_FORMAT_D32_FLOAT:
            pcStr = "DXGI_FORMAT_D32_FLOAT";
            break;
        case DXGI_FORMAT_R32_FLOAT:
            pcStr = "DXGI_FORMAT_R32_FLOAT";
            break;
        case DXGI_FORMAT_R32_UINT:
            pcStr = "DXGI_FORMAT_R32_UINT";
            break;
        case DXGI_FORMAT_R32_SINT:
            pcStr = "DXGI_FORMAT_R32_SINT";
            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
            pcStr = "DXGI_FORMAT_R24G8_TYPELESS";
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            pcStr = "DXGI_FORMAT_D24_UNORM_S8_UINT";
            break;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            pcStr = "DXGI_FORMAT_R24_UNORM_X8_TYPELESS";
            break;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            pcStr = "DXGI_FORMAT_X24_TYPELESS_G8_UINT";
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            pcStr = "DXGI_FORMAT_R8G8_TYPELESS";
            break;
        case DXGI_FORMAT_R8G8_UNORM:
            pcStr = "DXGI_FORMAT_R8G8_UNORM";
            break;
        case DXGI_FORMAT_R8G8_UINT:
            pcStr = "DXGI_FORMAT_R8G8_UINT";
            break;
        case DXGI_FORMAT_R8G8_SNORM:
            pcStr = "DXGI_FORMAT_R8G8_SNORM";
            break;
        case DXGI_FORMAT_R8G8_SINT:
            pcStr = "DXGI_FORMAT_R8G8_SINT";
            break;
        case DXGI_FORMAT_R16_TYPELESS:
            pcStr = "DXGI_FORMAT_R16_TYPELESS";
            break;
        case DXGI_FORMAT_R16_FLOAT:
            pcStr = "DXGI_FORMAT_R16_FLOAT";
            break;
        case DXGI_FORMAT_D16_UNORM:
            pcStr = "DXGI_FORMAT_D16_UNORM";
            break;
        case DXGI_FORMAT_R16_UNORM:
            pcStr = "DXGI_FORMAT_R16_UNORM";
            break;
        case DXGI_FORMAT_R16_UINT:
            pcStr = "DXGI_FORMAT_R16_UINT";
            break;
        case DXGI_FORMAT_R16_SNORM:
            pcStr = "DXGI_FORMAT_R16_SNORM";
            break;
        case DXGI_FORMAT_R16_SINT:
            pcStr = "DXGI_FORMAT_R16_SINT";
            break;
        case DXGI_FORMAT_R8_TYPELESS:
            pcStr = "DXGI_FORMAT_R8_TYPELESS";
            break;
        case DXGI_FORMAT_R8_UNORM:
            pcStr = "DXGI_FORMAT_R8_UNORM";
            break;
        case DXGI_FORMAT_R8_UINT:
            pcStr = "DXGI_FORMAT_R8_UINT";
            break;
        case DXGI_FORMAT_R8_SNORM:
            pcStr = "DXGI_FORMAT_R8_SNORM";
            break;
        case DXGI_FORMAT_R8_SINT:
            pcStr = "DXGI_FORMAT_R8_SINT";
            break;
        case DXGI_FORMAT_A8_UNORM:
            pcStr = "DXGI_FORMAT_A8_UNORM";
            break;
        case DXGI_FORMAT_R1_UNORM:
            pcStr = "DXGI_FORMAT_R1_UNORM";
            break;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            pcStr = "DXGI_FORMAT_R9G9B9E5_SHAREDEXP";
            break;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            pcStr = "DXGI_FORMAT_R8G8_B8G8_UNORM";
            break;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            pcStr = "DXGI_FORMAT_G8R8_G8B8_UNORM";
            break;
        case DXGI_FORMAT_BC1_TYPELESS:
            pcStr = "DXGI_FORMAT_BC1_TYPELESS";
            break;
        case DXGI_FORMAT_BC1_UNORM:
            pcStr = "DXGI_FORMAT_BC1_UNORM";
            break;
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            pcStr = "DXGI_FORMAT_BC1_UNORM_SRGB";
            break;
        case DXGI_FORMAT_BC2_TYPELESS:
            pcStr = "DXGI_FORMAT_BC2_TYPELESS";
            break;
        case DXGI_FORMAT_BC2_UNORM:
            pcStr = "DXGI_FORMAT_BC2_UNORM";
            break;
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            pcStr = "DXGI_FORMAT_BC2_UNORM_SRGB";
            break;
        case DXGI_FORMAT_BC3_TYPELESS:
            pcStr = "DXGI_FORMAT_BC3_TYPELESS";
            break;
        case DXGI_FORMAT_BC3_UNORM:
            pcStr = "DXGI_FORMAT_BC3_UNORM";
            break;
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            pcStr = "DXGI_FORMAT_BC3_UNORM_SRGB";
            break;
        case DXGI_FORMAT_BC4_TYPELESS:
            pcStr = "DXGI_FORMAT_BC4_TYPELESS";
            break;
        case DXGI_FORMAT_BC4_UNORM:
            pcStr = "DXGI_FORMAT_BC4_UNORM";
            break;
        case DXGI_FORMAT_BC4_SNORM:
            pcStr = "DXGI_FORMAT_BC4_SNORM";
            break;
        case DXGI_FORMAT_BC5_TYPELESS:
            pcStr = "DXGI_FORMAT_BC5_TYPELESS";
            break;
        case DXGI_FORMAT_BC5_UNORM:
            pcStr = "DXGI_FORMAT_BC5_UNORM";
            break;
        case DXGI_FORMAT_BC5_SNORM:
            pcStr = "DXGI_FORMAT_BC5_SNORM";
            break;
        case DXGI_FORMAT_B5G6R5_UNORM:
            pcStr = "DXGI_FORMAT_B5G6R5_UNORM";
            break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            pcStr = "DXGI_FORMAT_B5G5R5A1_UNORM";
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            pcStr = "DXGI_FORMAT_B8G8R8A8_UNORM";
            break;
        default:
            pcStr = "Unknown format";
            break;
    }

    if (bWithPrefix || strstr(pcStr, "DXGI_FORMAT_") == NULL)
        return pcStr;
    else
        return pcStr + strlen("DXGI_FORMAT_");
}
//---------------------------------------------------------------------------
