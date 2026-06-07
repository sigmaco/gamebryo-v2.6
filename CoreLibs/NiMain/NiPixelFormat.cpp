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

#include "NiPixelFormat.h"
#include "NiStream.h"
#include "NiBool.h"

// Palettized formats
const NiPixelFormat NiPixelFormat::PAL4(FORMAT_PAL, 
    COMP_INDEX, REP_INDEX,  4);
const NiPixelFormat NiPixelFormat::PALA4(FORMAT_PALALPHA, 
    COMP_INDEX, REP_INDEX,  4);
const NiPixelFormat NiPixelFormat::PAL8(FORMAT_PAL, 
    COMP_INDEX, REP_INDEX,  8);
const NiPixelFormat NiPixelFormat::PALA8(FORMAT_PALALPHA, 
    COMP_INDEX, REP_INDEX,  8);

// Standard RGB formats
const NiPixelFormat NiPixelFormat::RGBA32(FORMAT_RGBA,
    COMP_RED,   REP_NORM_INT,   8,
    COMP_GREEN, REP_NORM_INT,   8,
    COMP_BLUE,  REP_NORM_INT,   8,
    COMP_ALPHA, REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::RGB24(FORMAT_RGB,
    COMP_RED,   REP_NORM_INT,   8,
    COMP_GREEN, REP_NORM_INT,   8,
    COMP_BLUE,  REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::RGBA16(FORMAT_RGBA,
    COMP_RED,   REP_NORM_INT,   5,
    COMP_GREEN, REP_NORM_INT,   5,
    COMP_BLUE,  REP_NORM_INT,   5,
    COMP_ALPHA, REP_NORM_INT,   1);
const NiPixelFormat NiPixelFormat::BGRA16(FORMAT_RGBA,
    COMP_BLUE,  REP_NORM_INT,   5,
    COMP_GREEN, REP_NORM_INT,   5,
    COMP_RED,   REP_NORM_INT,   5,
    COMP_ALPHA, REP_NORM_INT,   1);
const NiPixelFormat NiPixelFormat::BGRA4443(FORMAT_RGBA,
    16, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_BLUE,  REP_NORM_INT,   4,  false,
    COMP_GREEN, REP_NORM_INT,   4,  false,
    COMP_RED,   REP_NORM_INT,   4,  false,
    COMP_ALPHA, REP_NORM_INT,   3,  false);


// Bump formats
const NiPixelFormat NiPixelFormat::BUMP16(FORMAT_BUMP,
    16, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_V,  REP_NORM_INT,   8,  true,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::BUMPLUMA32(FORMAT_BUMPLUMA,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_V,  REP_NORM_INT,   8,  true,
    COMP_LUMA,      REP_NORM_INT,   8,  false,
    COMP_PADDING,   REP_UNKNOWN,    8,  false);
// BUMPLUMA32_WII is Wii-specific LUMA32 format where the bits are
// height, luma, dV, dU. (instead of dU, dV, luma, height). Also, 
// the dV and dU are represented as biased, unsigned 8-bit ints
const NiPixelFormat NiPixelFormat::BUMPLUMA32_WII(FORMAT_BUMPLUMA_WII,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_V,  REP_NORM_INT,   8,  true,
    COMP_LUMA,      REP_NORM_INT,   8,  false,
    COMP_PADDING,   REP_UNKNOWN,    8,  false);

// Compressed formats
const NiPixelFormat NiPixelFormat::DXT1(FORMAT_DXT1, 
    COMP_COMPRESSED,    REP_COMPRESSED, 0);
const NiPixelFormat NiPixelFormat::DXT3(FORMAT_DXT3, 
    COMP_COMPRESSED,    REP_COMPRESSED, 0);
const NiPixelFormat NiPixelFormat::DXT5(FORMAT_DXT5, 
    COMP_COMPRESSED,    REP_COMPRESSED, 0);

// Floating-point formats
const NiPixelFormat NiPixelFormat::RGBA128(FORMAT_RGBA,
    128, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,   REP_FLOAT,      32, true,
    COMP_GREEN, REP_FLOAT,      32, true,
    COMP_BLUE,  REP_FLOAT,      32, true,
    COMP_ALPHA, REP_FLOAT,      32, true);
const NiPixelFormat NiPixelFormat::RGBA64(FORMAT_RGBA,
    64, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,   REP_HALF,       16, true,
    COMP_GREEN, REP_HALF,       16, true,
    COMP_BLUE,  REP_HALF,       16, true,
    COMP_ALPHA, REP_HALF,       16, true);
const NiPixelFormat NiPixelFormat::RG64(FORMAT_TWO_CHANNEL,
    64, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,   REP_FLOAT,      32, true,
    COMP_GREEN, REP_FLOAT,      32, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::RG32(FORMAT_TWO_CHANNEL,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,   REP_HALF,       16, true,
    COMP_GREEN, REP_HALF,       16, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::R32(FORMAT_ONE_CHANNEL,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,   REP_FLOAT,      32, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::R16(FORMAT_ONE_CHANNEL,
    16, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,   REP_HALF,       16, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);

// Big-endian Floating-point formats
const NiPixelFormat NiPixelFormat::RGBA128_BE(FORMAT_RGBA,
    128, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_FLOAT,      32, true,
    COMP_GREEN, REP_FLOAT,      32, true,
    COMP_BLUE,  REP_FLOAT,      32, true,
    COMP_ALPHA, REP_FLOAT,      32, true);
const NiPixelFormat NiPixelFormat::RGBA64_BE(FORMAT_RGBA, 
    64, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_HALF,       16, true,
    COMP_GREEN, REP_HALF,       16, true,
    COMP_BLUE,  REP_HALF,       16, true,
    COMP_ALPHA, REP_HALF,       16, true);
const NiPixelFormat NiPixelFormat::R32_BE(FORMAT_ONE_CHANNEL,
    32, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_FLOAT,      32, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::RG32_BE(FORMAT_TWO_CHANNEL,
    32, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_HALF,       16, true,
    COMP_GREEN, REP_HALF,       16, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::RG64_BE(FORMAT_TWO_CHANNEL,
    64, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_FLOAT,      32, true,
    COMP_GREEN, REP_FLOAT,      32, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::R16_BE(FORMAT_ONE_CHANNEL,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_HALF,       16, true,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);


// Big-endian 32 bpp formats
const NiPixelFormat NiPixelFormat::RGBA32_BE(FORMAT_RGBA,
    32, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_NORM_INT,   8,  false,
    COMP_GREEN, REP_NORM_INT,   8,  false,
    COMP_BLUE,  REP_NORM_INT,   8,  false,
    COMP_ALPHA, REP_NORM_INT,   8,  false);


// Big-endian 16 bpp formats
const NiPixelFormat NiPixelFormat::RGBA5551_BE(FORMAT_RGBA,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_NORM_INT,   5,  false,
    COMP_GREEN, REP_NORM_INT,   5,  false,
    COMP_BLUE,  REP_NORM_INT,   5,  false,
    COMP_ALPHA, REP_NORM_INT,   1,  false);
const NiPixelFormat NiPixelFormat::BGRA5551_BE(FORMAT_RGBA,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_BLUE,  REP_NORM_INT,   5,  false,
    COMP_GREEN, REP_NORM_INT,   5,  false,
    COMP_RED,   REP_NORM_INT,   5,  false,
    COMP_ALPHA, REP_NORM_INT,   1,  false);
const NiPixelFormat NiPixelFormat::RGBA16_BE(FORMAT_RGBA,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_RED,   REP_NORM_INT,   5,  false,
    COMP_GREEN, REP_NORM_INT,   5,  false,
    COMP_BLUE,  REP_NORM_INT,   5,  false,
    COMP_ALPHA, REP_NORM_INT,   1,  false);
const NiPixelFormat NiPixelFormat::BGR565_BE(FORMAT_RGB,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_BLUE,  REP_NORM_INT,   5,  false,
    COMP_GREEN, REP_NORM_INT,   6,  false,
    COMP_RED,   REP_NORM_INT,   5,  false,
    COMP_EMPTY, REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::BGRA4444_BE(FORMAT_RGBA,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_BLUE,  REP_NORM_INT,   4,  false,
    COMP_GREEN, REP_NORM_INT,   4,  false,
    COMP_RED,   REP_NORM_INT,   4,  false,
    COMP_ALPHA, REP_NORM_INT,   4,  false);
const NiPixelFormat NiPixelFormat::BGRA4443_BE(FORMAT_RGBA,
    16, TILE_NONE, false, INVALID_RENDERER_HINT,
    COMP_BLUE,  REP_NORM_INT,   4,  false,
    COMP_GREEN, REP_NORM_INT,   4,  false,
    COMP_RED,   REP_NORM_INT,   4,  false,
    COMP_ALPHA, REP_NORM_INT,   3,  false);


// Misc formats
const NiPixelFormat NiPixelFormat::I8(FORMAT_ONE_CHANNEL, 
    COMP_INTENSITY, REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::A8(FORMAT_ONE_CHANNEL, 
    COMP_ALPHA,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::IA88(FORMAT_TWO_CHANNEL,
    COMP_INTENSITY, REP_NORM_INT,   8,
    COMP_ALPHA,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::I4(FORMAT_ONE_CHANNEL, 
    COMP_INTENSITY, REP_NORM_INT,   4);
const NiPixelFormat NiPixelFormat::IA44(FORMAT_TWO_CHANNEL,
    COMP_INTENSITY, REP_NORM_INT,   4,
    COMP_ALPHA,     REP_NORM_INT,   4);
const NiPixelFormat NiPixelFormat::BGR233(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   2,
    COMP_GREEN,     REP_NORM_INT,   3,
    COMP_RED,       REP_NORM_INT,   3,
    COMP_PADDING,   REP_UNKNOWN,    2);
const NiPixelFormat NiPixelFormat::BGRA2338(FORMAT_RGBA,
    COMP_BLUE,      REP_NORM_INT,   2,
    COMP_GREEN,     REP_NORM_INT,   3,
    COMP_RED,       REP_NORM_INT,   3,
    COMP_ALPHA,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::RGB555(FORMAT_RGB,
    16, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,       REP_NORM_INT,   5,  false,
    COMP_GREEN,     REP_NORM_INT,   5,  false,
    COMP_BLUE,      REP_NORM_INT,   5,  false,
    COMP_PADDING,   REP_UNKNOWN,    1,  false);
const NiPixelFormat NiPixelFormat::BGR555(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   5,
    COMP_GREEN,     REP_NORM_INT,   5,
    COMP_RED,       REP_NORM_INT,   5,
    COMP_PADDING,   REP_UNKNOWN,    1);
const NiPixelFormat NiPixelFormat::BGR565(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   5,
    COMP_GREEN,     REP_NORM_INT,   6,
    COMP_RED,       REP_NORM_INT,   5);
const NiPixelFormat NiPixelFormat::RGBA5551(FORMAT_RGBA,
    COMP_RED,       REP_NORM_INT,   5,
    COMP_GREEN,     REP_NORM_INT,   5,
    COMP_BLUE,      REP_NORM_INT,   5,
    COMP_ALPHA,     REP_NORM_INT,   1);
const NiPixelFormat NiPixelFormat::BGRA5551(FORMAT_RGBA,
    COMP_BLUE,      REP_NORM_INT,   5,
    COMP_GREEN,     REP_NORM_INT,   5,
    COMP_RED,       REP_NORM_INT,   5,
    COMP_ALPHA,     REP_NORM_INT,   1);
const NiPixelFormat NiPixelFormat::BGRX5551(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   5,
    COMP_GREEN,     REP_NORM_INT,   5,
    COMP_RED,       REP_NORM_INT,   5,
    COMP_PADDING,   REP_UNKNOWN,    1);
const NiPixelFormat NiPixelFormat::BGRA4444(FORMAT_RGBA,
    COMP_BLUE,      REP_NORM_INT,   4,
    COMP_GREEN,     REP_NORM_INT,   4,
    COMP_RED,       REP_NORM_INT,   4,
    COMP_ALPHA,     REP_NORM_INT,   4);
const NiPixelFormat NiPixelFormat::BGRX4444(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   4,
    COMP_GREEN,     REP_NORM_INT,   4,
    COMP_RED,       REP_NORM_INT,   4,
    COMP_PADDING,   REP_NORM_INT,   4);
const NiPixelFormat NiPixelFormat::BGR888(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   8,
    COMP_GREEN,     REP_NORM_INT,   8,
    COMP_RED,       REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::BGRA8888(FORMAT_RGBA,
    COMP_BLUE,      REP_NORM_INT,   8,
    COMP_GREEN,     REP_NORM_INT,   8,
    COMP_RED,       REP_NORM_INT,   8,
    COMP_ALPHA,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::ARGB8888(FORMAT_RGBA,
    COMP_ALPHA,     REP_NORM_INT,   8,
    COMP_RED,       REP_NORM_INT,   8,
    COMP_GREEN,     REP_NORM_INT,   8,
    COMP_BLUE,      REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::BGRX8888(FORMAT_RGB,
    COMP_BLUE,      REP_NORM_INT,   8,
    COMP_GREEN,     REP_NORM_INT,   8,
    COMP_RED,       REP_NORM_INT,   8,
    COMP_PADDING,   REP_UNKNOWN,    8);
const NiPixelFormat NiPixelFormat::RGBX8888(FORMAT_RGB,
    COMP_RED,       REP_NORM_INT,   8,
    COMP_GREEN,     REP_NORM_INT,   8,
    COMP_BLUE,      REP_NORM_INT,   8,
    COMP_PADDING,   REP_UNKNOWN,    8);
const NiPixelFormat NiPixelFormat::RGBA1010102(FORMAT_RGBA,
    COMP_RED,       REP_NORM_INT,   10,
    COMP_GREEN,     REP_NORM_INT,   10,
    COMP_BLUE,      REP_NORM_INT,   10,
    COMP_ALPHA,     REP_NORM_INT,   2);
const NiPixelFormat NiPixelFormat::BGRA1010102(FORMAT_RGBA,
    COMP_BLUE,      REP_NORM_INT,   10,
    COMP_GREEN,     REP_NORM_INT,   10,
    COMP_RED,       REP_NORM_INT,   10,
    COMP_ALPHA,     REP_NORM_INT,   2);
const NiPixelFormat NiPixelFormat::RGBA1010102F(FORMAT_RGBA,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_RED,       REP_FLOAT,      10, true,
    COMP_GREEN,     REP_FLOAT,      10, true,
    COMP_BLUE,      REP_FLOAT,      10, true,
    COMP_ALPHA,     REP_NORM_INT,   2,  false);
const NiPixelFormat NiPixelFormat::GR1616(FORMAT_TWO_CHANNEL,
    COMP_GREEN,     REP_NORM_INT,   16,
    COMP_RED,       REP_NORM_INT,   16);
const NiPixelFormat NiPixelFormat::BUMPLUMA556(FORMAT_BUMPLUMA,
    16, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   5,  true,
    COMP_OFFSET_V,  REP_NORM_INT,   5,  true,
    COMP_LUMA,      REP_NORM_INT,   6,  false,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::PA88(FORMAT_TWO_CHANNEL,
    COMP_INDEX,     REP_INDEX,      8,
    COMP_ALPHA,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::L8(FORMAT_ONE_CHANNEL,
    COMP_LUMA,      REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::L16(FORMAT_ONE_CHANNEL,
    COMP_LUMA,      REP_NORM_INT,   16);
const NiPixelFormat NiPixelFormat::LA88(FORMAT_TWO_CHANNEL,
    COMP_LUMA,      REP_NORM_INT,   8,
    COMP_ALPHA,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::LA44(FORMAT_TWO_CHANNEL,
    COMP_LUMA,      REP_NORM_INT,   4,
    COMP_ALPHA,     REP_NORM_INT,   4);
const NiPixelFormat NiPixelFormat::BUMPWQ8888(FORMAT_BUMP,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_V,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_W,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_Q,  REP_NORM_INT,   8,  true);
const NiPixelFormat NiPixelFormat::BUMP1616(FORMAT_BUMP,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   16, true,
    COMP_OFFSET_V,  REP_NORM_INT,   16, true,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::BUMPW101111(FORMAT_BUMP,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   10, true,
    COMP_OFFSET_V,  REP_NORM_INT,   11, true,
    COMP_OFFSET_W,  REP_NORM_INT,   11, true,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::BUMPLUMAX8888(FORMAT_BUMPLUMA,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   8,  true,
    COMP_OFFSET_V,  REP_NORM_INT,   8,  true,
    COMP_LUMA,      REP_NORM_INT,   8,  false,
    COMP_PADDING,   REP_UNKNOWN,    8,  false);
const NiPixelFormat NiPixelFormat::BUMPWA1010102(FORMAT_BUMP,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_OFFSET_U,  REP_NORM_INT,   10, true,
    COMP_OFFSET_V,  REP_NORM_INT,   10, true,
    COMP_OFFSET_W,  REP_NORM_INT,   10, true,
    COMP_ALPHA,     REP_NORM_INT,   2,  false);
const NiPixelFormat NiPixelFormat::RENDERERSPECIFICCOMPRESSED(
    FORMAT_ONE_CHANNEL,
    COMP_PADDING,   REP_UNKNOWN,    0);
const NiPixelFormat NiPixelFormat::RENDERERSPECIFIC16(FORMAT_ONE_CHANNEL,
    COMP_PADDING,   REP_UNKNOWN,    16);
const NiPixelFormat NiPixelFormat::RENDERERSPECIFIC32(FORMAT_ONE_CHANNEL,
    COMP_PADDING,   REP_UNKNOWN,    32);
const NiPixelFormat NiPixelFormat::RENDERERSPECIFIC64(FORMAT_ONE_CHANNEL,
    COMP_PADDING,   REP_UNKNOWN,    64);

// Depth/Stencil buffer formats
const NiPixelFormat NiPixelFormat::DEPTH8(FORMAT_DEPTH_STENCIL,
    COMP_DEPTH,     REP_NORM_INT,   8);
const NiPixelFormat NiPixelFormat::DEPTH16(FORMAT_DEPTH_STENCIL,
    COMP_DEPTH,     REP_NORM_INT,   16);
const NiPixelFormat NiPixelFormat::DEPTH32(FORMAT_DEPTH_STENCIL,
    COMP_DEPTH,     REP_NORM_INT,   32);
const NiPixelFormat NiPixelFormat::STENCILDEPTH115(FORMAT_DEPTH_STENCIL,
    COMP_STENCIL,   REP_NORM_INT,   1,
    COMP_DEPTH,     REP_NORM_INT,   15);
const NiPixelFormat NiPixelFormat::STENCILDEPTH824(FORMAT_DEPTH_STENCIL,
    COMP_STENCIL,   REP_NORM_INT,   8,
    COMP_DEPTH,     REP_NORM_INT,   24);
const NiPixelFormat NiPixelFormat::STENCILDEPTH824FLOAT(FORMAT_DEPTH_STENCIL,
    32, TILE_NONE, true, INVALID_RENDERER_HINT,
    COMP_STENCIL,   REP_NORM_INT,   8,  false,
    COMP_DEPTH,     REP_FLOAT,      24, true,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false,
    COMP_EMPTY,     REP_UNKNOWN,    0,  false);
const NiPixelFormat NiPixelFormat::XDEPTH824(FORMAT_DEPTH_STENCIL,
    COMP_PADDING,   REP_NORM_INT,   8,
    COMP_DEPTH,     REP_NORM_INT,   24);
const NiPixelFormat NiPixelFormat::STENCILXDEPTH4424(FORMAT_DEPTH_STENCIL,
    COMP_STENCIL,   REP_NORM_INT,   4,
    COMP_PADDING,   REP_NORM_INT,   4,
    COMP_DEPTH,     REP_NORM_INT,   24);

//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat()
{
    InitPixelFormat(FORMAT_PAL, 8, 
        TILE_NONE, true, INVALID_RENDERER_HINT, false,
        COMP_INDEX, REP_INDEX, 8, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false);
}
//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat(Format eFormat, 
        Component eComp, Representation eRep, unsigned char ucBPP)
{
    InitPixelFormat(eFormat, ucBPP, 
        TILE_NONE, true, INVALID_RENDERER_HINT, false,
        eComp, eRep, ucBPP, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false);
}
//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat(Format eFormat, 
    Component eComp0, Representation eRep0, unsigned char ucBPC0,
    Component eComp1, Representation eRep1, unsigned char ucBPC1)
{
    InitPixelFormat(eFormat, ucBPC0 + ucBPC1,
        TILE_NONE, true, INVALID_RENDERER_HINT, false,
        eComp0, eRep0, ucBPC0, false,
        eComp1, eRep1, ucBPC1, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false);
}
//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat(Format eFormat, 
    Component eComp0, Representation eRep0, unsigned char ucBPC0, 
    Component eComp1, Representation eRep1, unsigned char ucBPC1, 
    Component eComp2, Representation eRep2, unsigned char ucBPC2)
{
    InitPixelFormat(eFormat, ucBPC0 + ucBPC1 + ucBPC2, 
        TILE_NONE, true, INVALID_RENDERER_HINT, false,
        eComp0, eRep0, ucBPC0, false,
        eComp1, eRep1, ucBPC1, false,
        eComp2, eRep2, ucBPC2, false,
        COMP_EMPTY, REP_UNKNOWN, 0, false);
}
//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat(Format eFormat, 
    Component eComp0, Representation eRep0, unsigned char ucBPC0, 
    Component eComp1, Representation eRep1, unsigned char ucBPC1, 
    Component eComp2, Representation eRep2, unsigned char ucBPC2,
    Component eComp3, Representation eRep3, unsigned char ucBPC3)
{
    InitPixelFormat(eFormat, ucBPC0 + ucBPC1 + ucBPC2 + ucBPC3, 
        TILE_NONE, true, INVALID_RENDERER_HINT, false,
        eComp0, eRep0, ucBPC0, false,
        eComp1, eRep1, ucBPC1, false,
        eComp2, eRep2, ucBPC2, false,
        eComp3, eRep3, ucBPC3, false);
}
//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat(Format eFormat, unsigned char ucBPP,  
    Tiling eTiled, bool bLittleEndian, unsigned int uiRendererHint,
    Component eComp0, Representation eRep0, unsigned char ucBPC0,
    bool bSigned0,
    Component eComp1, Representation eRep1, unsigned char ucBPC1,
    bool bSigned1,
    Component eComp2, Representation eRep2, unsigned char ucBPC2,
    bool bSigned2,
    Component eComp3, Representation eRep3, unsigned char ucBPC3,
    bool bSigned3)
{
    InitPixelFormat(eFormat, ucBPP, eTiled, 
        bLittleEndian, uiRendererHint, false, 
        eComp0, eRep0, ucBPC0, bSigned0,
        eComp1, eRep1, ucBPC1, bSigned1,
        eComp2, eRep2, ucBPC2, bSigned2,
        eComp3, eRep3, ucBPC3, bSigned3);
}
//---------------------------------------------------------------------------
NiPixelFormat::NiPixelFormat(Format eFormat, unsigned char ucBPP,  
    Tiling eTiled, bool bLittleEndian, unsigned int uiRendererHint, 
    bool bSRGBSpace,
    Component eComp0, Representation eRep0, unsigned char ucBPC0,
    bool bSigned0,
    Component eComp1, Representation eRep1, unsigned char ucBPC1,
    bool bSigned1,
    Component eComp2, Representation eRep2, unsigned char ucBPC2,
    bool bSigned2,
    Component eComp3, Representation eRep3, unsigned char ucBPC3,
    bool bSigned3)
{
    InitPixelFormat(eFormat, ucBPP, eTiled, 
        bLittleEndian, uiRendererHint, bSRGBSpace, 
        eComp0, eRep0, ucBPC0, bSigned0,
        eComp1, eRep1, ucBPC1, bSigned1,
        eComp2, eRep2, ucBPC2, bSigned2,
        eComp3, eRep3, ucBPC3, bSigned3);
}
//---------------------------------------------------------------------------
void NiPixelFormat::InitPixelFormat(Format eFormat, unsigned char ucBPP,  
    Tiling eTiled, bool bLittleEndian, unsigned int uiRendererHint, 
    bool bSRGBSpace,
    Component eComp0, Representation eRep0, unsigned char ucBPC0,
    bool bSigned0,
    Component eComp1, Representation eRep1, unsigned char ucBPC1,
    bool bSigned1,
    Component eComp2, Representation eRep2, unsigned char ucBPC2,
    bool bSigned2,
    Component eComp3, Representation eRep3, unsigned char ucBPC3,
    bool bSigned3)
{
    m_uFlags = 0;
    m_ucBitsPerPixel = ucBPP;
    m_bSRGBSpace = bSRGBSpace;
    m_eFormat = eFormat;
    m_uiRendererHint = uiRendererHint;
    m_uiExtraData = 0;
    SetLittleEndian(bLittleEndian);
    m_eTiling = eTiled;
   
    m_akComponents[0].m_eComponent = eComp0;
    m_akComponents[0].m_eRepresentation = eRep0;
    m_akComponents[0].m_ucBitsPerComponent = ucBPC0;
    m_akComponents[0].m_bSigned = bSigned0;

    m_akComponents[1].m_eComponent = eComp1;
    m_akComponents[1].m_eRepresentation = eRep1;
    m_akComponents[1].m_ucBitsPerComponent = ucBPC1;
    m_akComponents[1].m_bSigned = bSigned1;

    m_akComponents[2].m_eComponent = eComp2;
    m_akComponents[2].m_eRepresentation = eRep2;
    m_akComponents[2].m_ucBitsPerComponent = ucBPC2;
    m_akComponents[2].m_bSigned = bSigned2;

    m_akComponents[3].m_eComponent = eComp3;
    m_akComponents[3].m_eRepresentation = eRep3;
    m_akComponents[3].m_ucBitsPerComponent = ucBPC3;
    m_akComponents[3].m_bSigned = bSigned3;

    NIASSERT((RoundBitsUpToNextByte(ucBPC0 + ucBPC1 + ucBPC2 + ucBPC3)
        == m_ucBitsPerPixel) || 
        (ucBPC0 + ucBPC1 + ucBPC2 + ucBPC3 == m_ucBitsPerPixel));

}
//---------------------------------------------------------------------------
NiPixelFormat NiPixelFormat::CreateFromRGBAMasks(
    unsigned char ucBitsPerPixel, unsigned int uiMaskRed,  
    unsigned int uiMaskGreen, unsigned int uiMaskBlue, 
    unsigned int uiMaskAlpha)
{
    NiPixelFormat kFormat = NiPixelFormat();
    kFormat.SetBitsPerPixel(ucBitsPerPixel);
    if (uiMaskAlpha == 0x00000000)
        kFormat.SetFormat(FORMAT_RGB);
    else
        kFormat.SetFormat(FORMAT_RGBA);
    kFormat.SetLittleEndian(true);
    kFormat.SetRendererHint(
        (unsigned int) NiPixelFormat::INVALID_RENDERER_HINT);
    kFormat.SetTiling(TILE_NONE);
    
    NiPixelFormat::Component aeComps[4];
    unsigned char aucSizes[4];
    unsigned int auiMasks[4];

    auiMasks[0] = uiMaskRed;
    aeComps[0]  = COMP_RED;
    aucSizes[0] = static_cast<unsigned char>(CalcMaskSize(uiMaskRed));
    
    auiMasks[1] = uiMaskGreen;
    aeComps[1]  = COMP_GREEN;
    aucSizes[1] = static_cast<unsigned char>(CalcMaskSize(uiMaskGreen));
    
    auiMasks[2] = uiMaskBlue;
    aeComps[2]  = COMP_BLUE;
    aucSizes[2] = static_cast<unsigned char>(CalcMaskSize(uiMaskBlue));
    
    auiMasks[3] = uiMaskAlpha;
    aeComps[3]  = COMP_ALPHA;
    aucSizes[3] = static_cast<unsigned char>(CalcMaskSize(uiMaskAlpha));

    for (int i = 0; i < 3; i++)
    {
        if (auiMasks[i] != 0)
        {
            for (int j = i + 1; j < 3; j++)
            {
                if (auiMasks[i] > auiMasks[j])
                {
                    unsigned int uiTempMask = auiMasks[j];
                    unsigned char ucTempSize = aucSizes[j];
                    NiPixelFormat::Component eTempComp = aeComps[j];
                    auiMasks[j] = auiMasks[i];
                    aucSizes[j] = aucSizes[i];
                    aeComps[j] = aeComps[i];
                    auiMasks[i] = uiTempMask;
                    aucSizes[i] = ucTempSize;
                    aeComps[i] = eTempComp;
                }
            }
        }
    }

    unsigned int uiComponent = 0;
    for (unsigned int ui = 0; ui < 4; ui++)
    {
        if (auiMasks[ui] != 0)
        {
            kFormat.SetComponent(uiComponent, aeComps[ui],
                REP_NORM_INT, aucSizes[ui], false);
            uiComponent++;
        }
    }

    for (unsigned int uj = uiComponent; uj < 4; uj++)
    {
        kFormat.SetComponent(uiComponent, COMP_EMPTY,
            REP_UNKNOWN, 0, false);
        uiComponent++;
    }

    return kFormat;
}
//---------------------------------------------------------------------------
unsigned int NiPixelFormat::CalcMaskSize(unsigned int uiMask)
{
    unsigned int uiTotal = 0;
    while (uiMask != 0)
    {
        uiTotal += (uiMask & 1);
        uiMask = uiMask >> 1;
    }
    return uiTotal;    
}
//---------------------------------------------------------------------------
void NiPixelFormat::LoadBinary(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 3))
    {
        NiStreamLoadEnum(kStream, m_eFormat);
        unsigned int auiColorMasks[4];

        for(unsigned int i = 0; i < 4; i++)
        {
            NiStreamLoadBinary(kStream, auiColorMasks[i]);
        }

        unsigned int uiBPP;
        NiStreamLoadBinary(kStream, uiBPP);
        m_ucBitsPerPixel = (unsigned char)uiBPP;
        unsigned int uiOldFastCompare;
        NiStreamLoadBinary(kStream, uiOldFastCompare);
        NiStreamLoadBinary(kStream, uiOldFastCompare);

        Tiling eTiling;
        NiStreamLoadEnum(kStream, eTiling);

        ConvertOldPixelFormat(m_eFormat, m_ucBitsPerPixel, auiColorMasks);
    }
    else
    {
        NiStreamLoadEnum(kStream, m_eFormat);
        NiStreamLoadBinary(kStream, m_ucBitsPerPixel);
        NiStreamLoadBinary(kStream, m_uiRendererHint);
        NiStreamLoadBinary(kStream, m_uiExtraData);
        NiStreamLoadBinary(kStream, m_uFlags);
        NiStreamLoadEnum(kStream, m_eTiling);
        if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 4))
        {
            NiBool bSRGBSpace = 0;
            NiStreamLoadBinary(kStream, bSRGBSpace);
            m_bSRGBSpace = (bSRGBSpace != 0);
        }
        else
        {
            m_bSRGBSpace = false;
        }
       
        for (unsigned int ui = 0; ui < NUM_COMPS; ui++)
        {
            NiStreamLoadEnum(kStream, 
                m_akComponents[ui].m_eComponent);
            NiStreamLoadEnum(kStream, 
                m_akComponents[ui].m_eRepresentation);
            NiStreamLoadBinary(kStream, 
                m_akComponents[ui].m_ucBitsPerComponent);
            NiBool kBool;
            NiStreamLoadBinary(kStream, kBool);
            if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 4))
            {
                m_akComponents[ui].m_bSigned = kBool > 0 ? true: false;
            }
            else
            {
                // Ignore streamed value, which was almost certainly wrong,
                // and determine signed flag from component itself.
                // Offset components or floating point representations are
                // considered to be signed, all others are unsigned.
                if (m_akComponents[ui].m_eComponent == COMP_OFFSET_U ||
                    m_akComponents[ui].m_eComponent == COMP_OFFSET_V ||
                    m_akComponents[ui].m_eComponent == COMP_OFFSET_W ||
                    m_akComponents[ui].m_eComponent == COMP_OFFSET_Q ||
                    m_akComponents[ui].m_eRepresentation == REP_FLOAT ||
                    m_akComponents[ui].m_eRepresentation == REP_HALF)
                {
                    m_akComponents[ui].m_bSigned = true;
                }
                else
                {
                    m_akComponents[ui].m_bSigned = false;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPixelFormat::ConvertOldPixelFormat(Format eFormat, unsigned char ucBPP, 
    unsigned int auiColorMasks[4])
{
    m_uFlags = 0;
    m_ucBitsPerPixel = ucBPP;
    m_bSRGBSpace = false;
    m_eFormat = eFormat;
    m_uiRendererHint = INVALID_RENDERER_HINT;
    m_uiExtraData = 0;
    SetBit(true, ENDIAN_MASK);
    m_eTiling = TILE_NONE;

#ifdef NIDEBUG
    const char* pcInvalidString = 
        "Invalid Pre-Gamebryo 2.0 pixel format found!\n"
        "Please re-export this asset.\n";
#endif

    switch(eFormat)
    {
        case FORMAT_RGB:
            switch (ucBPP)
            {
                case 24:
                    if (auiColorMasks[0] == 0x000000ff && 
                        auiColorMasks[1] == 0x0000ff00 &&
                        auiColorMasks[2] == 0x00ff0000 && 
                        auiColorMasks[3] == 0x00000000)
                    {
                        *(this) = RGB24;
                    }
                    else if (auiColorMasks[0] == 0x00ff0000 && 
                        auiColorMasks[1] == 0x0000ff00 &&
                        auiColorMasks[2] == 0x000000ff && 
                        auiColorMasks[3] == 0x00000000)
                    {
                        *(this) = BGR888;
                    }
                    else
                    {
                        SetFormat(FORMAT_UNKNOWN);
                        NiOutputDebugString(pcInvalidString);
                    }
                    break;
                case 16:
                    if (auiColorMasks[0] == 0x0000f800 && 
                        auiColorMasks[1] == 0x000007e0 &&
                        auiColorMasks[2] == 0x0000001f && 
                        auiColorMasks[3] == 0x00000000)
                    {
                        *(this) = BGR565;
                    }
                    else
                    {
                        SetFormat(FORMAT_UNKNOWN);
                        NiOutputDebugString(pcInvalidString);
                    }
                    break;
                default:
                    SetFormat(FORMAT_UNKNOWN);
                    NiOutputDebugString(pcInvalidString);
                    break;
            }
            break;
        case FORMAT_RGBA:
            switch (ucBPP)
            {
                case 32:
                    if (auiColorMasks[0] == 0x00ff0000 && 
                        auiColorMasks[1] == 0x0000ff00 &&
                        auiColorMasks[2] == 0x000000ff && 
                        auiColorMasks[3] == 0xff000000)
                    {
                        *(this) = BGRA8888;
                    }
                    else
                    {
                        *(this) = RGBA32;
                    }
                    break;
                case 16:
                    if (auiColorMasks[0] == 0x0000001f && 
                        auiColorMasks[1] == 0x000003e0 &&
                        auiColorMasks[2] == 0x00007c00 && 
                        auiColorMasks[3] == 0x00008000)
                    {
                        *(this) = RGBA16;
                    }
                    else if (auiColorMasks[0] == 0x00007c00 && 
                        auiColorMasks[1] == 0x000003e0 &&
                        auiColorMasks[2] == 0x0000001f && 
                        auiColorMasks[3] == 0x00008000)
                    {
                        *(this) = BGRA16;
                    }
                    else if (auiColorMasks[0] == 0x0000f00 && 
                        auiColorMasks[1] == 0x000000f0 &&
                        auiColorMasks[2] == 0x0000000f && 
                        auiColorMasks[3] == 0x0000f000)
                    {
                        *(this) = BGRA4444;
                    }
                    else
                    {
                        SetFormat(FORMAT_UNKNOWN);
                        NiOutputDebugString(pcInvalidString);
                    }
                    break;
                default:
                    SetFormat(FORMAT_UNKNOWN);
                    NiOutputDebugString(pcInvalidString);
                    break;
            }
            break;
        case FORMAT_PAL:
            switch(ucBPP)
            {
                case 8:
                    *(this) = PAL8;
                    break;
                default:
                    SetFormat(FORMAT_UNKNOWN);
                    NiOutputDebugString(pcInvalidString);
                    break;
            }
            break;
        case FORMAT_PALALPHA:
            switch(ucBPP)
            {
                case 8:
                    *(this) = PALA8;
                    break;
                default:                    
                    SetFormat(FORMAT_UNKNOWN);
                    NiOutputDebugString(pcInvalidString);
                    break;
            }
            break;
        case FORMAT_DXT1:
            *(this) = DXT1;
            break;
        case FORMAT_DXT3:
            *(this) = DXT3;
            break;
        case FORMAT_DXT5:
            *(this) = DXT5;
            break;
        case FORMAT_BUMP:
            NIASSERT(ucBPP == 16);
            *(this) = BUMP16;
            break;
        case FORMAT_BUMPLUMA:
            NIASSERT(ucBPP == 32);
            *(this) = BUMPLUMA32;
            break;
        default:           
            SetFormat(FORMAT_UNKNOWN);
            NiOutputDebugString(pcInvalidString);
            break;
    }
}
//---------------------------------------------------------------------------
void NiPixelFormat::SaveBinary(NiStream& kStream)
{
    NiStreamSaveEnum(kStream, m_eFormat);
    NiStreamSaveBinary(kStream, m_ucBitsPerPixel);
    NiStreamSaveBinary(kStream, m_uiRendererHint);
    NiStreamSaveBinary(kStream, m_uiExtraData);
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveEnum(kStream, m_eTiling);
    NiBool bSRGBSpace = m_bSRGBSpace;
    NiStreamSaveBinary(kStream, bSRGBSpace);
   
    for (unsigned int ui = 0; ui < NUM_COMPS; ui++)
    {
        NiStreamSaveEnum(kStream, m_akComponents[ui].m_eComponent);
        NiStreamSaveEnum(kStream, m_akComponents[ui].m_eRepresentation);
        NiStreamSaveBinary(kStream, m_akComponents[ui].m_ucBitsPerComponent);
        NiBool kBool = m_akComponents[ui].m_bSigned;
        NiStreamSaveBinary(kStream, kBool);
    }
}
//---------------------------------------------------------------------------
// Returns true if the two formats are identical in bits per component,
// number of components, signedness; essentially everything except for
// endianness and tiling (actual organization of the information in memory)
bool NiPixelFormat::FunctionallyIdentical(const NiPixelFormat& kOther,
    unsigned int uiComparisonMode) const
{
    if ((uiComparisonMode & CMP_FORMAT) &&
        (kOther.m_eFormat != m_eFormat))
        return false;
    
    if ((uiComparisonMode & CMP_ENDIAN) && 
        (((kOther.m_uFlags ^ m_uFlags) & ENDIAN_MASK) != 0))
        return false;
        
    if ((uiComparisonMode & CMP_TILING) && 
        (kOther.m_eTiling != m_eTiling))
        return false;

    if ((uiComparisonMode & CMP_SRGB) && 
        (kOther.m_bSRGBSpace != m_bSRGBSpace))
        return false;

    if ((uiComparisonMode & CMP_RENDERER_HINT) && 
        (kOther.m_uiRendererHint != m_uiRendererHint) &&
        (m_uiRendererHint != INVALID_RENDERER_HINT) &&
        (kOther.m_uiRendererHint != INVALID_RENDERER_HINT))
        return false;

    if (uiComparisonMode & CMP_BASIC)
    {
        // Currently BPP and any flag bits that are not endian are considered
        // as part of the basic comparison, but could be broken out later
        if ((((kOther.m_uFlags ^ m_uFlags) & ~ENDIAN_MASK) != 0) ||
            (kOther.m_ucBitsPerPixel != m_ucBitsPerPixel))
            return false;

        for (int i = 0; i < NUM_COMPS; i++)
        {
            if ((kOther.m_akComponents[i].m_eComponent != 
                    m_akComponents[i].m_eComponent) ||
                (kOther.m_akComponents[i].m_eRepresentation  != 
                    m_akComponents[i].m_eRepresentation) ||
                (kOther.m_akComponents[i].m_ucBitsPerComponent != 
                    m_akComponents[i].m_ucBitsPerComponent) ||
                (kOther.m_akComponents[i].m_bSigned != 
                    m_akComponents[i].m_bSigned))
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
