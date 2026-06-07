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
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NSBStageAndSamplerStates.h"
#if defined(NIDEBUG)
#include "NSBUtility.h"
#endif  //#if defined(NIDEBUG)

#if defined(WIN32)
#include <NiDX9Renderer.h>

//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureStageState(
    NiD3DTextureStageState eTSS, unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTSS)
    {
    case NID3DTSS_COLOROP:
        uiD3DValue = (unsigned int)D3DTSS_COLOROP;
        return true;
    case NID3DTSS_COLORARG0:
        uiD3DValue = (unsigned int)D3DTSS_COLORARG0;
        return true;
    case NID3DTSS_COLORARG1:
        uiD3DValue = (unsigned int)D3DTSS_COLORARG1;
        return true;
    case NID3DTSS_COLORARG2:
        uiD3DValue = (unsigned int)D3DTSS_COLORARG2;
        return true;
    case NID3DTSS_ALPHAOP:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAOP;
        return true;
    case NID3DTSS_ALPHAARG0:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAARG0;
        return true;
    case NID3DTSS_ALPHAARG1:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAARG1;
        return true;
    case NID3DTSS_ALPHAARG2:
        uiD3DValue = (unsigned int)D3DTSS_ALPHAARG2;
        return true;
    case NID3DTSS_RESULTARG:
        uiD3DValue = (unsigned int)D3DTSS_RESULTARG;
        return true;
    case NID3DTSS_BUMPENVMAT00:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT00;
        return true;
    case NID3DTSS_BUMPENVMAT01:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT01;
        return true;
    case NID3DTSS_BUMPENVMAT10:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT10;
        return true;
    case NID3DTSS_BUMPENVMAT11:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVMAT11;
        return true;
    case NID3DTSS_BUMPENVLSCALE:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVLSCALE;
        return true;
    case NID3DTSS_BUMPENVLOFFSET:
        uiD3DValue = (unsigned int)D3DTSS_BUMPENVLOFFSET;
        return true;
    case NID3DTSS_TEXCOORDINDEX:
        uiD3DValue = (unsigned int)D3DTSS_TEXCOORDINDEX;
        return true;
    case NID3DTSS_TEXTURETRANSFORMFLAGS:
        uiD3DValue = (unsigned int)D3DTSS_TEXTURETRANSFORMFLAGS;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::ConvertNSBTextureStageStateValue(
    NiD3DTextureStageState eTSS, unsigned int uiNSBValue, 
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTSS)
    {
    case NID3DTSS_COLOROP:
        return GetD3DTextureOp((NiD3DTextureOp)uiNSBValue, uiD3DValue);
    case NID3DTSS_COLORARG0:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_COLORARG1:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_COLORARG2:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_ALPHAOP:
        return GetD3DTextureOp((NiD3DTextureOp)uiNSBValue, uiD3DValue);
    case NID3DTSS_ALPHAARG0:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_ALPHAARG1:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_ALPHAARG2:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_RESULTARG:
        return GetD3DTextureArg(uiNSBValue, uiD3DValue);
    case NID3DTSS_BUMPENVMAT00:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DTSS_BUMPENVMAT01:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DTSS_BUMPENVMAT10:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DTSS_BUMPENVMAT11:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DTSS_BUMPENVLSCALE:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DTSS_BUMPENVLOFFSET:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DTSS_TEXCOORDINDEX:
        return GetD3DTexCoordIndexFlags(uiNSBValue, uiD3DValue);
    case NID3DTSS_TEXTURETRANSFORMFLAGS:
        return GetD3DTextureTransformFlags(
            (NiD3DTextureTransformFlags)uiNSBValue, uiD3DValue);
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::ConvertNSBTextureSamplerStateValue(
    NiD3DTextureSamplerState eTSS, unsigned int uiNSBValue,
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTSS)
    {
    case NID3DSAMP_ADDRESSU:
    case NID3DSAMP_ADDRESSV:
    case NID3DSAMP_ADDRESSW:
        return GetD3DTextureAddress((NiD3DTextureAddress)uiNSBValue, 
            uiD3DValue);
    case NID3DSAMP_BORDERCOLOR:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DSAMP_MAGFILTER:
    case NID3DSAMP_MINFILTER:
    case NID3DSAMP_MIPFILTER:
        return GetD3DTextureFilter((NiD3DTextureFilter)uiNSBValue,
            uiD3DValue);
    case NID3DSAMP_MIPMAPLODBIAS:
    case NID3DSAMP_MAXMIPLEVEL:
    case NID3DSAMP_MAXANISOTROPY:
        uiD3DValue = uiNSBValue;
        return true;
    case NID3DSAMP_SRGBTEXTURE:
    case NID3DSAMP_ELEMENTINDEX:
    case NID3DSAMP_DMAPOFFSET:
        uiD3DValue = uiNSBValue;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureOp(NiD3DTextureOp eTOP,
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTOP)
    {
    case NID3DTOP_DISABLE:
        uiD3DValue = (unsigned int)D3DTOP_DISABLE;
        return true;
    case NID3DTOP_SELECTARG1:
        uiD3DValue = (unsigned int)D3DTOP_SELECTARG1;
        return true;
    case NID3DTOP_SELECTARG2:
        uiD3DValue = (unsigned int)D3DTOP_SELECTARG2;
        return true;
    case NID3DTOP_MODULATE:
        uiD3DValue = (unsigned int)D3DTOP_MODULATE;
        return true;
    case NID3DTOP_MODULATE2X:
        uiD3DValue = (unsigned int)D3DTOP_MODULATE2X;
        return true;
    case NID3DTOP_MODULATE4X:
        uiD3DValue = (unsigned int)D3DTOP_MODULATE4X;
        return true;
    case NID3DTOP_ADD:
        uiD3DValue = (unsigned int)D3DTOP_ADD;
        return true;
    case NID3DTOP_ADDSIGNED:
        uiD3DValue = (unsigned int)D3DTOP_ADDSIGNED;
        return true;
    case NID3DTOP_ADDSIGNED2X:
        uiD3DValue = (unsigned int)D3DTOP_ADDSIGNED2X;
        return true;
    case NID3DTOP_SUBTRACT:
        uiD3DValue = (unsigned int)D3DTOP_SUBTRACT;
        return true;
    case NID3DTOP_ADDSMOOTH:
        uiD3DValue = (unsigned int)D3DTOP_ADDSMOOTH;
        return true;
    case NID3DTOP_BLENDDIFFUSEALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDDIFFUSEALPHA;
        return true;
    case NID3DTOP_BLENDTEXTUREALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDTEXTUREALPHA;
        return true;
    case NID3DTOP_BLENDFACTORALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDFACTORALPHA;
        return true;
    case NID3DTOP_BLENDTEXTUREALPHAPM:
        uiD3DValue = (unsigned int)D3DTOP_BLENDTEXTUREALPHAPM;
        return true;
    case NID3DTOP_BLENDCURRENTALPHA:
        uiD3DValue = (unsigned int)D3DTOP_BLENDCURRENTALPHA;
        return true;
    case NID3DTOP_PREMODULATE:
        uiD3DValue = (unsigned int)D3DTOP_PREMODULATE;
        return true;
    case NID3DTOP_MODULATEALPHA_ADDCOLOR:
        uiD3DValue = (unsigned int)D3DTOP_MODULATEALPHA_ADDCOLOR;
        return true;
    case NID3DTOP_MODULATECOLOR_ADDALPHA:
        uiD3DValue = (unsigned int)D3DTOP_MODULATECOLOR_ADDALPHA;
        return true;
    case NID3DTOP_MODULATEINVALPHA_ADDCOLOR:
        uiD3DValue = (unsigned int)D3DTOP_MODULATEINVALPHA_ADDCOLOR;
        return true;
    case NID3DTOP_MODULATEINVCOLOR_ADDALPHA:
        uiD3DValue = (unsigned int)D3DTOP_MODULATEINVCOLOR_ADDALPHA;
        return true;
    case NID3DTOP_BUMPENVMAP:
        uiD3DValue = (unsigned int)D3DTOP_BUMPENVMAP;
        return true;
    case NID3DTOP_BUMPENVMAPLUMINANCE:
        uiD3DValue = (unsigned int)D3DTOP_BUMPENVMAPLUMINANCE;
        return true;
    case NID3DTOP_DOTPRODUCT3:
        uiD3DValue = (unsigned int)D3DTOP_DOTPRODUCT3;
        return true;
    case NID3DTOP_MULTIPLYADD:
        uiD3DValue = (unsigned int)D3DTOP_MULTIPLYADD;
        return true;
    case NID3DTOP_LERP:
        uiD3DValue = (unsigned int)D3DTOP_LERP;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureArg(unsigned int uiTA,
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    if (uiTA == NID3DTA_INVALID)
        return false;

    unsigned int uiArg = uiTA & 0x0FFFFFFF;

    switch (uiArg)
    {
    case NID3DTA_CURRENT:
        uiD3DValue = (unsigned int)D3DTA_CURRENT;
        break;
    case NID3DTA_DIFFUSE:
        uiD3DValue = (unsigned int)D3DTA_DIFFUSE;
        break;
    case NID3DTA_SELECTMASK:
        uiD3DValue = (unsigned int)D3DTA_SELECTMASK;
        break;
    case NID3DTA_SPECULAR:
        uiD3DValue = (unsigned int)D3DTA_SPECULAR;
        break;
    case NID3DTA_TEMP:
        uiD3DValue = (unsigned int)D3DTA_TEMP;
        break;
    case NID3DTA_TEXTURE:
        uiD3DValue = (unsigned int)D3DTA_TEXTURE;
        break;
    case NID3DTA_TFACTOR:
        uiD3DValue = (unsigned int)D3DTA_TFACTOR;
        break;
    }

    if (uiTA & NID3DTA_ALPHAREPLICATE)
        uiD3DValue |= D3DTA_ALPHAREPLICATE;
    if (uiTA & NID3DTA_COMPLEMENT)
        uiD3DValue |= D3DTA_COMPLEMENT;

    return true;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureTransformFlags(
    NiD3DTextureTransformFlags eTTF, unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    uiD3DValue = 0;

    switch (eTTF & ~NID3DTTFF_PROJECTED)
    {
    case NID3DTTFF_DISABLE:
        uiD3DValue = (unsigned int)D3DTTFF_DISABLE;
        break;
    case NID3DTTFF_COUNT1:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT1;
        break;
    case NID3DTTFF_COUNT2:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT2;
        break;
    case NID3DTTFF_COUNT3:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT3;
        break;
    case NID3DTTFF_COUNT4:
        uiD3DValue = (unsigned int)D3DTTFF_COUNT4;
        break;
    default:
        return false;
    }

    if (eTTF & NID3DTTFF_PROJECTED)
        uiD3DValue |= (unsigned int)D3DTTFF_PROJECTED;

    return true;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTexCoordIndexFlags(unsigned int uiTCI,
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    unsigned int uiFlags = uiTCI & 0xF0000000;
    unsigned int uiIndex = uiTCI & 0x0FFFFFFF;

    switch (uiFlags)
    {
    case NID3DTSI_PASSTHRU:
        uiD3DValue = (unsigned int)D3DTSS_TCI_PASSTHRU;
        break;
    case NID3DTSI_CAMERASPACENORMAL:
        uiD3DValue = (unsigned int)D3DTSS_TCI_CAMERASPACENORMAL;
        break;
    case NID3DTSI_CAMERASPACEPOSITION:
        uiD3DValue = (unsigned int)D3DTSS_TCI_CAMERASPACEPOSITION;
        break;
    case NID3DTSI_CAMERASPACEREFLECTIONVECTOR:
        uiD3DValue = (unsigned int)D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR;
        break;
    case NID3DTSI_SPHEREMAP:
        uiD3DValue = (unsigned int)D3DTSS_TCI_SPHEREMAP;
        break;
    default:
        uiD3DValue = 0;
        break;
    }

    uiD3DValue |= uiIndex;
    return true;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureSamplerState(
    NiD3DTextureSamplerState eTSS, unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTSS)
    {
    case NID3DSAMP_ADDRESSU:
        uiD3DValue = (unsigned int)D3DSAMP_ADDRESSU;
        return true;
    case NID3DSAMP_ADDRESSV:
        uiD3DValue = (unsigned int)D3DSAMP_ADDRESSV;
        return true;
    case NID3DSAMP_ADDRESSW:
        uiD3DValue = (unsigned int)D3DSAMP_ADDRESSW;
        return true;
    case NID3DSAMP_BORDERCOLOR:
        uiD3DValue = (unsigned int)D3DSAMP_BORDERCOLOR;
        return true;
    case NID3DSAMP_MAGFILTER:
        uiD3DValue = (unsigned int)D3DSAMP_MAGFILTER;
        return true;
    case NID3DSAMP_MINFILTER:
        uiD3DValue = (unsigned int)D3DSAMP_MINFILTER;
        return true;
    case NID3DSAMP_MIPFILTER:
        uiD3DValue = (unsigned int)D3DSAMP_MIPFILTER;
        return true;
    case NID3DSAMP_MIPMAPLODBIAS:
        uiD3DValue = (unsigned int)D3DSAMP_MIPMAPLODBIAS;
        return true;
    case NID3DSAMP_MAXMIPLEVEL:
        uiD3DValue = (unsigned int)D3DSAMP_MAXMIPLEVEL;
        return true;
    case NID3DSAMP_MAXANISOTROPY:
        uiD3DValue = (unsigned int)D3DSAMP_MAXANISOTROPY;
        return true;
    case NID3DSAMP_SRGBTEXTURE:
        uiD3DValue = (unsigned int)D3DSAMP_SRGBTEXTURE;
        return true;
    case NID3DSAMP_ELEMENTINDEX:
        uiD3DValue = (unsigned int)D3DSAMP_ELEMENTINDEX;
        return true;
    case NID3DSAMP_DMAPOFFSET:
        uiD3DValue = (unsigned int)D3DSAMP_DMAPOFFSET;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureAddress(NiD3DTextureAddress eTA,
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTA)
    {
    case NID3DTADDRESS_WRAP:
        uiD3DValue = (unsigned int)D3DTADDRESS_WRAP;
        return true;
    case NID3DTADDRESS_MIRROR:
        uiD3DValue = (unsigned int)D3DTADDRESS_MIRROR;
        return true;
    case NID3DTADDRESS_CLAMP:
        uiD3DValue = (unsigned int)D3DTADDRESS_CLAMP;
        return true;
    case NID3DTADDRESS_BORDER:
        uiD3DValue = (unsigned int)D3DTADDRESS_BORDER;
        return true;
    case NID3DTADDRESS_MIRRORONCE:
        uiD3DValue = (unsigned int)D3DTADDRESS_MIRRORONCE;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBStageAndSamplerStates::GetD3DTextureFilter(NiD3DTextureFilter eTF,
    unsigned int& uiD3DValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eTF)
    {
    case NID3DTEXF_NONE:
        uiD3DValue = (unsigned int)D3DTEXF_NONE;
        return true;
    case NID3DTEXF_POINT:
        uiD3DValue = (unsigned int)D3DTEXF_POINT;
        return true;
    case NID3DTEXF_LINEAR:
        uiD3DValue = (unsigned int)D3DTEXF_LINEAR;
        return true;
    case NID3DTEXF_ANISOTROPIC:
        uiD3DValue = (unsigned int)D3DTEXF_ANISOTROPIC;
        return true;
    case NID3DTEXF_PYRAMIDALQUAD:
        uiD3DValue = (unsigned int)D3DTEXF_PYRAMIDALQUAD;
        return true;
    case NID3DTEXF_GAUSSIANQUAD:
        uiD3DValue = (unsigned int)D3DTEXF_GAUSSIANQUAD;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureStageString(
    NSBStageAndSamplerStates::NiD3DTextureStageState eState)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eState)
    {
    STATE_CASE_STRING(NID3DTSS_COLOROP)
    STATE_CASE_STRING(NID3DTSS_COLORARG0)
    STATE_CASE_STRING(NID3DTSS_COLORARG1)
    STATE_CASE_STRING(NID3DTSS_COLORARG2)
    STATE_CASE_STRING(NID3DTSS_ALPHAOP)
    STATE_CASE_STRING(NID3DTSS_ALPHAARG0)
    STATE_CASE_STRING(NID3DTSS_ALPHAARG1)
    STATE_CASE_STRING(NID3DTSS_ALPHAARG2)
    STATE_CASE_STRING(NID3DTSS_RESULTARG)
    STATE_CASE_STRING(NID3DTSS_BUMPENVMAT00)
    STATE_CASE_STRING(NID3DTSS_BUMPENVMAT01)
    STATE_CASE_STRING(NID3DTSS_BUMPENVMAT10)
    STATE_CASE_STRING(NID3DTSS_BUMPENVMAT11)
    STATE_CASE_STRING(NID3DTSS_BUMPENVLSCALE)
    STATE_CASE_STRING(NID3DTSS_BUMPENVLOFFSET)
    STATE_CASE_STRING(NID3DTSS_TEXCOORDINDEX)
    STATE_CASE_STRING(NID3DTSS_TEXTURETRANSFORMFLAGS)
    }
    return "***** UNKNOWN STAGE STATE *****";
}
//---------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureStageValueString(
    NiD3DTextureStageState eState, unsigned int uiValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    static char s_acTemp[256];

    NiStrcpy(s_acTemp, 256, "*** USS ***");

    switch (eState)
    {
    case NID3DTSS_COLOROP:
    case NID3DTSS_ALPHAOP:
        {
            switch (uiValue)
            {
            STATE_CASE_STRING(NID3DTOP_DISABLE)
            STATE_CASE_STRING(NID3DTOP_SELECTARG1)
            STATE_CASE_STRING(NID3DTOP_SELECTARG2)
            STATE_CASE_STRING(NID3DTOP_MODULATE)
            STATE_CASE_STRING(NID3DTOP_MODULATE2X)
            STATE_CASE_STRING(NID3DTOP_MODULATE4X)
            STATE_CASE_STRING(NID3DTOP_ADD)
            STATE_CASE_STRING(NID3DTOP_ADDSIGNED)
            STATE_CASE_STRING(NID3DTOP_ADDSIGNED2X)
            STATE_CASE_STRING(NID3DTOP_SUBTRACT)
            STATE_CASE_STRING(NID3DTOP_ADDSMOOTH)
            STATE_CASE_STRING(NID3DTOP_BLENDDIFFUSEALPHA)
            STATE_CASE_STRING(NID3DTOP_BLENDTEXTUREALPHA)
            STATE_CASE_STRING(NID3DTOP_BLENDFACTORALPHA)
            STATE_CASE_STRING(NID3DTOP_BLENDTEXTUREALPHAPM)
            STATE_CASE_STRING(NID3DTOP_BLENDCURRENTALPHA)
            STATE_CASE_STRING(NID3DTOP_PREMODULATE)
            STATE_CASE_STRING(NID3DTOP_MODULATEALPHA_ADDCOLOR)
            STATE_CASE_STRING(NID3DTOP_MODULATECOLOR_ADDALPHA)
            STATE_CASE_STRING(NID3DTOP_MODULATEINVALPHA_ADDCOLOR)
            STATE_CASE_STRING(NID3DTOP_MODULATEINVCOLOR_ADDALPHA)
            STATE_CASE_STRING(NID3DTOP_BUMPENVMAP)
            STATE_CASE_STRING(NID3DTOP_BUMPENVMAPLUMINANCE)
            STATE_CASE_STRING(NID3DTOP_DOTPRODUCT3)
            STATE_CASE_STRING(NID3DTOP_MULTIPLYADD)
            STATE_CASE_STRING(NID3DTOP_LERP)
            }
            return "*** USS ***";
        }
        break;
    case NID3DTSS_COLORARG0:
    case NID3DTSS_COLORARG1:
    case NID3DTSS_COLORARG2:
    case NID3DTSS_ALPHAARG0:
    case NID3DTSS_ALPHAARG1:
    case NID3DTSS_ALPHAARG2:
    case NID3DTSS_RESULTARG:
        {
            unsigned int uiArg = uiValue & 0x0fffffff;
            switch (uiArg)
            {
            case NID3DTA_CURRENT: 
                NiSprintf(s_acTemp, 256, "CURRENT");
                break;
            case NID3DTA_DIFFUSE: 
                NiSprintf(s_acTemp, 256, "DIFFUSE");
                break;
            case NID3DTA_SELECTMASK:
                NiSprintf(s_acTemp, 256, "SELECTMASK");
                break;
            case NID3DTA_SPECULAR: 
                NiSprintf(s_acTemp, 256, "SPECULAR");
                break;
            case NID3DTA_TEMP: 
                NiSprintf(s_acTemp, 256, "TEMP");
                break;
            case NID3DTA_TEXTURE: 
                NiSprintf(s_acTemp, 256, "TEXTURE");
                break;
            case NID3DTA_TFACTOR: 
                NiSprintf(s_acTemp, 256, "TFACTOR");
                break;
            default:                
                NiSprintf(s_acTemp, 256, "*** USS ***"); 
                break;
            }

            if (uiValue & NID3DTA_ALPHAREPLICATE)
                NiStrcat(s_acTemp, 256, "   ALPHAREPLICATE");
            if (uiValue & NID3DTA_COMPLEMENT)
                NiStrcat(s_acTemp, 256, "   COMPLEMENT");
        }
        break;

    case NID3DTSS_BUMPENVMAT00:
    case NID3DTSS_BUMPENVMAT01:
    case NID3DTSS_BUMPENVMAT10:
    case NID3DTSS_BUMPENVMAT11:
    case NID3DTSS_BUMPENVLSCALE:
    case NID3DTSS_BUMPENVLOFFSET:
        {
            float fValue = *((float*)&uiValue);
            NiSprintf(s_acTemp, 256, "%8.5f", fValue);
        }
        break;
    case NID3DTSS_TEXCOORDINDEX:
        {
            NiSprintf(s_acTemp, 256, "%2d - ", uiValue & 0x0fffffff);
            switch (uiValue & 0xf0000000)
            {
            case NID3DTSI_PASSTHRU:
                NiStrcat(s_acTemp, 256, "PASSTHRU");
                break;
            case NID3DTSI_CAMERASPACENORMAL:
                NiStrcat(s_acTemp, 256, "CAMERASPACENORMAL");
                break;
            case NID3DTSI_CAMERASPACEPOSITION:
                NiStrcat(s_acTemp, 256, "CAMERASPACEPOSITION");
                break;
            case NID3DTSI_CAMERASPACEREFLECTIONVECTOR:
                NiStrcat(s_acTemp, 256, "CAMERASPACEREFLECTIONVECTOR");
                break;
            case NID3DTSI_SPHEREMAP:
                NiStrcat(s_acTemp, 256, "SPHEREMAP");
                break;
            default:
                NiStrcat(s_acTemp, 256, "*** USS ***");
                break;
            }
        }
        break;
    case NID3DTSS_TEXTURETRANSFORMFLAGS:
        {
            switch (uiValue)
            {
            case NID3DTTFF_DISABLE:     return "DISABLE";
            case NID3DTTFF_COUNT1:      return "COUNT1";
            case NID3DTTFF_COUNT2:      return "COUNT2";
            case NID3DTTFF_COUNT3:      return "COUNT3";
            case NID3DTTFF_COUNT4:      return "COUNT4";
            case NID3DTTFF_PROJECTED:   return "PROJECTED";
            default:                    return "*** USS ***";
            }
        }
        break;
    }
    return s_acTemp;
}
//---------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureSamplerString(
    NSBStageAndSamplerStates::NiD3DTextureSamplerState eState)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    switch (eState)
    {
    STATE_CASE_STRING(NID3DSAMP_ADDRESSU)
    STATE_CASE_STRING(NID3DSAMP_ADDRESSV)
    STATE_CASE_STRING(NID3DSAMP_ADDRESSW)
    STATE_CASE_STRING(NID3DSAMP_BORDERCOLOR)
    STATE_CASE_STRING(NID3DSAMP_MAGFILTER)
    STATE_CASE_STRING(NID3DSAMP_MINFILTER)
    STATE_CASE_STRING(NID3DSAMP_MIPFILTER)
    STATE_CASE_STRING(NID3DSAMP_MIPMAPLODBIAS)
    STATE_CASE_STRING(NID3DSAMP_MAXMIPLEVEL)
    STATE_CASE_STRING(NID3DSAMP_MAXANISOTROPY)
    STATE_CASE_STRING(NID3DSAMP_SRGBTEXTURE)
    STATE_CASE_STRING(NID3DSAMP_ELEMENTINDEX)
    STATE_CASE_STRING(NID3DSAMP_DMAPOFFSET)
    }
    return "**** UNKNOWN SAMPLER STATE ****";
}
//---------------------------------------------------------------------------
const char* NSBStageAndSamplerStates::LookupTextureSamplerValueString(
    NiD3DTextureSamplerState eState, unsigned int uiValue)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    static char s_acTemp[256];

    NiStrcpy(s_acTemp, 256, "*** USS ***");

    switch (eState)
    {
    case NID3DSAMP_ADDRESSU:
    case NID3DSAMP_ADDRESSV:
    case NID3DSAMP_ADDRESSW:
        switch (uiValue)
        {
        case NID3DTADDRESS_WRAP:            return "WRAP";
        case NID3DTADDRESS_MIRROR:          return "MIRROR";
        case NID3DTADDRESS_CLAMP:           return "CLAMP";
        case NID3DTADDRESS_BORDER:          return "BORDER";
        case NID3DTADDRESS_MIRRORONCE:      return "MIRRORONCE";
        default:                            return "*** USS ***";
        }
        break;
    case NID3DSAMP_MAGFILTER:
    case NID3DSAMP_MINFILTER:
    case NID3DSAMP_MIPFILTER:
        switch (uiValue)
        {
        case NID3DTEXF_NONE:                return "NONE";
        case NID3DTEXF_POINT:               return "POINT";
        case NID3DTEXF_LINEAR:              return "LINEAR";
        case NID3DTEXF_ANISOTROPIC:         return "ANISOTROPIC";
        case NID3DTEXF_PYRAMIDALQUAD:       return "PYRAMIDALQUAD";
        case NID3DTEXF_GAUSSIANQUAD:        return "GAUSSIANQUAD";
        default:                            return "*** USS ***";
        }
        break;
    case NID3DSAMP_BORDERCOLOR:
    case NID3DSAMP_MIPMAPLODBIAS:
    case NID3DSAMP_MAXMIPLEVEL:
    case NID3DSAMP_MAXANISOTROPY:
    case NID3DSAMP_SRGBTEXTURE:
    case NID3DSAMP_ELEMENTINDEX:
    case NID3DSAMP_DMAPOFFSET:
        NiSprintf(s_acTemp, 256, "0x%08x", uiValue);
        break;
    }

    return s_acTemp;
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
#endif //#if defined(WIN32)