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

#ifndef NSBSTAGEANDSAMPLERSTATES_H
#define NSBSTAGEANDSAMPLERSTATES_H

#include <NiD3DDefines.h>
#include "NiBinaryShaderLibLibType.h"

class NIBINARYSHADERLIB_ENTRY NSBStageAndSamplerStates : public NiMemObject
{
public:
    NSBStageAndSamplerStates();
    ~NSBStageAndSamplerStates();

    // Texture stage state values
    enum NiD3DTextureStageState
    {
        NID3DTSS_COLOROP    = 0,
        NID3DTSS_COLORARG0,
        NID3DTSS_COLORARG1,
        NID3DTSS_COLORARG2,
        NID3DTSS_ALPHAOP,
        NID3DTSS_ALPHAARG0,
        NID3DTSS_ALPHAARG1,
        NID3DTSS_ALPHAARG2,
        NID3DTSS_RESULTARG,
        NID3DTSS_BUMPENVMAT00,
        NID3DTSS_BUMPENVMAT01,
        NID3DTSS_BUMPENVMAT10,
        NID3DTSS_BUMPENVMAT11,
        NID3DTSS_BUMPENVLSCALE,
        NID3DTSS_BUMPENVLOFFSET,
        NID3DTSS_TEXCOORDINDEX,
        NID3DTSS_TEXTURETRANSFORMFLAGS,
        //
        NID3DTSS_COUNT,
        NID3DTSS_INVALID    = 0x7fffffff
    };

    enum NiD3DTextureOp
    {
        NID3DTOP_DISABLE    = 0,
        NID3DTOP_SELECTARG1,
        NID3DTOP_SELECTARG2,
        NID3DTOP_MODULATE,
        NID3DTOP_MODULATE2X,
        NID3DTOP_MODULATE4X,
        NID3DTOP_ADD,
        NID3DTOP_ADDSIGNED,
        NID3DTOP_ADDSIGNED2X,
        NID3DTOP_SUBTRACT,
        NID3DTOP_ADDSMOOTH,
        NID3DTOP_BLENDDIFFUSEALPHA,
        NID3DTOP_BLENDTEXTUREALPHA,
        NID3DTOP_BLENDFACTORALPHA,
        NID3DTOP_BLENDTEXTUREALPHAPM,
        NID3DTOP_BLENDCURRENTALPHA,
        NID3DTOP_PREMODULATE,
        NID3DTOP_MODULATEALPHA_ADDCOLOR,
        NID3DTOP_MODULATECOLOR_ADDALPHA,
        NID3DTOP_MODULATEINVALPHA_ADDCOLOR,
        NID3DTOP_MODULATEINVCOLOR_ADDALPHA,
        NID3DTOP_BUMPENVMAP,
        NID3DTOP_BUMPENVMAPLUMINANCE,
        NID3DTOP_DOTPRODUCT3,
        NID3DTOP_MULTIPLYADD,
        NID3DTOP_LERP,
        //
        NID3DTOP_COUNT,
        NID3DTOP_INVALID    = 0x7fffffff
    };

    enum NiD3DTextureArg
    {
        NID3DTA_CURRENT         = 0,
        NID3DTA_DIFFUSE,
        NID3DTA_SELECTMASK,
        NID3DTA_SPECULAR,
        NID3DTA_TEMP,
        NID3DTA_TEXTURE,
        NID3DTA_TFACTOR,
        // 
        NID3DTA_COUNT,
        // Modifier flags
        NID3DTA_ALPHAREPLICATE  = 0x40000000,
        NID3DTA_COMPLEMENT      = 0x80000000,
        NID3DTA_INVALID         = 0x7fffffff
    };

    enum NiD3DTextureTransformFlags
    {
        NID3DTTFF_DISABLE   = 0,
        NID3DTTFF_COUNT1,
        NID3DTTFF_COUNT2,
        NID3DTTFF_COUNT3,
        NID3DTTFF_COUNT4,
        NID3DTTFF_PROJECTED = 0x00000100,
        // 
        NID3DTTFF_INVALID   = 0x7fffffff
    };

    enum NiD3DTexCoordIndexFlags
    {
        NID3DTSI_PASSTHRU                       = 0x00000000,
        NID3DTSI_CAMERASPACENORMAL              = 0x10000000,
        NID3DTSI_CAMERASPACEPOSITION            = 0x20000000,
        NID3DTSI_CAMERASPACEREFLECTIONVECTOR    = 0x30000000,
        NID3DTSI_SPHEREMAP                      = 0x40000000,
        // 
        NID3DTSI_INVALID    = 0x7fffffff
    };

    // Texture sampler states
    enum NiD3DTextureSamplerState
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

    enum NiD3DTextureAddress
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

    enum NiD3DTextureFilter
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

    static NiD3DTextureStageState LookupTextureStageState(
        const char* pcTextureStageState);
    static unsigned int LookupTextureStageStateValue(
        NiD3DTextureStageState eStageState, const char* pcValue);

    static NiD3DTextureSamplerState LookupTextureSamplerState(
        const char* pcTextureSamplerState);
    static unsigned int LookupTextureSamplerStateValue(
        NiD3DTextureSamplerState eSamplerState, const char* pcValue);

    static bool GetD3DTextureStageState(NiD3DTextureStageState eTSS,
        unsigned int& uiD3DValue);
    static bool ConvertNSBTextureStageStateValue(
        NiD3DTextureStageState eTSS, unsigned int uiNSBValue,
        unsigned int& uiD3DValue);
    static bool ConvertNSBTextureSamplerStateValue(
        NiD3DTextureSamplerState eTSS, unsigned int uiNSBValue,
        unsigned int& uiD3DValue);

    static bool GetD3DTextureOp(NiD3DTextureOp eTOP,
        unsigned int& uiD3DValue);
    static bool GetD3DTextureArg(unsigned int uiTA, 
        unsigned int& uiD3DValue);
    static bool GetD3DTextureTransformFlags(NiD3DTextureTransformFlags eTTF,
        unsigned int& uiD3DValue);
    static bool GetD3DTexCoordIndexFlags(unsigned int uiTCI,
        unsigned int& uiD3DValue);
    static bool GetD3DTextureSamplerState(NiD3DTextureSamplerState eTSS,
        unsigned int& uiD3DValue);
    static bool GetD3DTextureAddress(NiD3DTextureAddress eTA,
        unsigned int& uiD3DValue);
    static bool GetD3DTextureFilter(NiD3DTextureFilter eTF,
        unsigned int& uiD3DValue);

#if defined(NIDEBUG)
    static const char* LookupTextureStageString(
        NiD3DTextureStageState eState);
    static const char* LookupTextureStageValueString(
        NiD3DTextureStageState eState, unsigned int uiValue);
    static const char* LookupTextureSamplerString(
        NiD3DTextureSamplerState eState);
    static const char* LookupTextureSamplerValueString(
        NiD3DTextureSamplerState eState, unsigned int uiValue);
#endif  //#if defined(NIDEBUG)
};

#endif  //NSBSTAGEANDSAMPLERSTATES_H
