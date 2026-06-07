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

#ifndef NSBD3D10STAGEANDSAMPLERSTATES_H
#define NSBD3D10STAGEANDSAMPLERSTATES_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NiMemObject.h"

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10StageAndSamplerStates
    : public NiMemObject
{
public:
    NSBD3D10StageAndSamplerStates();
    ~NSBD3D10StageAndSamplerStates();

    // Texture sampler states
    enum NiD3D10TextureSamplerState
    {
        NID3DSAMP_ADDRESSU      = 0,
        NID3DSAMP_ADDRESSV,
        NID3DSAMP_ADDRESSW,
        NID3DSAMP_BORDERCOLOR,
        NID3DSAMP_MAGFILTER,
        NID3DSAMP_MINFILTER,
        NID3DSAMP_MIPFILTER,
        NID3DSAMP_MIPMAPLODBIAS,
        NID3DSAMP_MAXMIPLEVEL,
        NID3DSAMP_MAXANISOTROPY,
        // DX9, Xenon
        NID3DSAMP_SRGBTEXTURE,
        // DX9
        NID3DSAMP_ELEMENTINDEX,
        NID3DSAMP_DMAPOFFSET,
        // D3D10
        NID3DSAMP_MINMIPLEVEL,
        NID3DSAMP_COMPARISONFUNC,
        //
        NID3DSAMP_COUNT,
        NID3DSAMP_DEPRECATED    = 0x7ffffffe,
        NID3DSAMP_INVALID       = 0x7fffffff
    };

    enum NiD3D10TextureAddress
    {
        NID3DTADDRESS_WRAP      = 0,
        NID3DTADDRESS_MIRROR,
        NID3DTADDRESS_CLAMP,
        NID3DTADDRESS_BORDER,
        // DX9, D3D10
        NID3DTADDRESS_MIRRORONCE,
        // 
        NID3DTADDRESS_COUNT,
        NID3DTADDRESS_INVALID   = 0x7fffffff
    };

    enum NiD3D10TextureFilter
    {
        NID3DTEXF_NONE      = 0,
        NID3DTEXF_POINT,
        NID3DTEXF_LINEAR,
        NID3DTEXF_ANISOTROPIC,
        // DX9
        NID3DTEXF_PYRAMIDALQUAD,
        NID3DTEXF_GAUSSIANQUAD,
        //
        NID3DTEXF_COUNT,
        NID3DTEXF_INVALID   = 0x7fffffff
    };

    static bool ConvertNSBD3D10TextureSamplerStateValue(
        NiD3D10TextureSamplerState eTSS, unsigned int uiNSBD3D10Value,
        unsigned int& uiD3D10Value);
    static bool ConvertNSBD3D10TextureSamplerFilterValue(
        NiD3D10TextureFilter eMinFilter, NiD3D10TextureFilter eMagFilter,
        NiD3D10TextureFilter eMipFilter, unsigned int& uiD3D10Value, 
        bool& bMipmapEnable);

    static bool GetD3D10TextureAddress(NiD3D10TextureAddress eTA,
        unsigned int& uiD3D10Value);

#if defined(NIDEBUG)
    static const char* LookupTextureSamplerString(
        NiD3D10TextureSamplerState eState);
    static const char* LookupTextureSamplerValueString(
        NiD3D10TextureSamplerState eState, unsigned int uiValue);
#endif  //#if defined(NIDEBUG)
};

#endif  //NSBD3D10STAGEANDSAMPLERSTATES_H
