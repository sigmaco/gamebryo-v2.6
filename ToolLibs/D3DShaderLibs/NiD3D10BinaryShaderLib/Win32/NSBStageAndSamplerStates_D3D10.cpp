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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10StageAndSamplerStates.h"
#include "NSBD3D10RenderStates.h"

#if defined(NIDEBUG)
#include "NSBD3D10Utility.h"
#endif  //#if defined(NIDEBUG)

//---------------------------------------------------------------------------
bool NSBD3D10StageAndSamplerStates::ConvertNSBD3D10TextureSamplerStateValue(
    NiD3D10TextureSamplerState eTSS, unsigned int uiNSBD3D10Value,
    unsigned int& uiD3D10Value)
{
    switch (eTSS)
    {
    case NID3DSAMP_ADDRESSU:
    case NID3DSAMP_ADDRESSV:
    case NID3DSAMP_ADDRESSW:
        return GetD3D10TextureAddress((NiD3D10TextureAddress)uiNSBD3D10Value, 
            uiD3D10Value);
    case NID3DSAMP_COMPARISONFUNC:   // D3D10-specific
        return NSBD3D10RenderStates::GetD3D10CmpFunc(
            (NSBD3D10RenderStates::NiD3D10CmpFunc)uiNSBD3D10Value,
            uiD3D10Value);
    default:
        // Assume we are passing the value straight through.
        uiD3D10Value = uiNSBD3D10Value;
        return true;
    }
}
//---------------------------------------------------------------------------
bool NSBD3D10StageAndSamplerStates::ConvertNSBD3D10TextureSamplerFilterValue(
    NiD3D10TextureFilter eMinFilter, NiD3D10TextureFilter eMagFilter,
    NiD3D10TextureFilter eMipFilter, unsigned int& uiD3D10Value, 
    bool& bMipmapEnable)
{
    if ((eMinFilter == NID3DTEXF_ANISOTROPIC) ||
        (eMagFilter == NID3DTEXF_ANISOTROPIC) ||
        (eMipFilter == NID3DTEXF_ANISOTROPIC))
    {
        uiD3D10Value = D3D10_FILTER_ANISOTROPIC;
        bMipmapEnable = true;
        return true;
    }

    unsigned int uiCombination = 0;

    if (eMipFilter == NID3DTEXF_LINEAR)
    {
        uiCombination += 0x001;
    }
    else 
    {
        // D3D10 doesn't directly support "none" for the mipmap setting - 
        // treat any other values as POINT, but save the fact that mipmapping
        // is disabled.
        if (eMipFilter == NID3DTEXF_NONE)
            bMipmapEnable = false;
        else
            bMipmapEnable = true;
    }

    if (eMagFilter == NID3DTEXF_LINEAR)
    {
        uiCombination += 0x010;
    }
    if (eMinFilter == NID3DTEXF_LINEAR)
    {
        uiCombination += 0x100;
    }

    switch(uiCombination)
    {
    case 0x000:
        uiD3D10Value = D3D10_FILTER_MIN_MAG_MIP_POINT;
        return true;
    case 0x001:
        uiD3D10Value = D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        return true;
    case 0x010:
        uiD3D10Value = D3D10_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
        return true;
    case 0x011:
        uiD3D10Value = D3D10_FILTER_MIN_POINT_MAG_MIP_LINEAR;
        return true;
    case 0x100:
        uiD3D10Value = D3D10_FILTER_MIN_LINEAR_MAG_MIP_POINT;
        return true;
    case 0x101:
        uiD3D10Value = D3D10_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        return true;
    case 0x110:
        uiD3D10Value = D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        return true;
    case 0x111:
        uiD3D10Value = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
        return true;
    default:
        NIASSERT("Invalid sampler filter value!" && false);
        return false;
    }
}
//---------------------------------------------------------------------------
bool NSBD3D10StageAndSamplerStates::GetD3D10TextureAddress(
    NiD3D10TextureAddress eTA, unsigned int& uiD3D10Value)
{
    switch (eTA)
    {
    case NID3DTADDRESS_WRAP:
        uiD3D10Value = (unsigned int)D3D10_TEXTURE_ADDRESS_WRAP;
        return true;
    case NID3DTADDRESS_MIRROR:
        uiD3D10Value = (unsigned int)D3D10_TEXTURE_ADDRESS_MIRROR;
        return true;
    case NID3DTADDRESS_CLAMP:
        uiD3D10Value = (unsigned int)D3D10_TEXTURE_ADDRESS_CLAMP;
        return true;
    case NID3DTADDRESS_BORDER:
        uiD3D10Value = (unsigned int)D3D10_TEXTURE_ADDRESS_BORDER;
        return true;
    case NID3DTADDRESS_MIRRORONCE:
        uiD3D10Value = (unsigned int)D3D10_TEXTURE_ADDRESS_MIRROR_ONCE;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
const char* NSBD3D10StageAndSamplerStates::LookupTextureSamplerString(
    NSBD3D10StageAndSamplerStates::NiD3D10TextureSamplerState eState)
{
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
    STATE_CASE_STRING(NID3DSAMP_MINMIPLEVEL)    // D3D10-specific
    STATE_CASE_STRING(NID3DSAMP_MAXANISOTROPY)
    STATE_CASE_STRING(NID3DSAMP_SRGBTEXTURE)
    STATE_CASE_STRING(NID3DSAMP_ELEMENTINDEX)
    STATE_CASE_STRING(NID3DSAMP_DMAPOFFSET)
    STATE_CASE_STRING(NID3DSAMP_COMPARISONFUNC)
    }
    return "**** UNKNOWN SAMPLER STATE ****";
}
//---------------------------------------------------------------------------
const char* NSBD3D10StageAndSamplerStates::LookupTextureSamplerValueString(
    NiD3D10TextureSamplerState eState, unsigned int uiValue)
{
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
    case NID3DSAMP_MINMIPLEVEL: // D3D10-specific
    case NID3DSAMP_MAXANISOTROPY:
    case NID3DSAMP_SRGBTEXTURE:
    case NID3DSAMP_ELEMENTINDEX:
    case NID3DSAMP_DMAPOFFSET:
        NiSprintf(s_acTemp, 256, "0x%08x", uiValue);
        break;
    case NID3DSAMP_COMPARISONFUNC:
        switch (uiValue)
        {
        case NSBD3D10RenderStates::NID3DCMP_NEVER:
            return "NEVER";
        case NSBD3D10RenderStates::NID3DCMP_LESS:
            return "LESS";
        case NSBD3D10RenderStates::NID3DCMP_EQUAL:
            return "EQUAL";
        case NSBD3D10RenderStates::NID3DCMP_LESSEQUAL:
            return "LESSEQUAL";
        case NSBD3D10RenderStates::NID3DCMP_GREATER:
            return "GREATER";
        case NSBD3D10RenderStates::NID3DCMP_NOTEQUAL:
            return "NOTEQUAL";
        case NSBD3D10RenderStates::NID3DCMP_GREATEREQUAL:
            return "GREATEREQUAL";
        case NSBD3D10RenderStates::NID3DCMP_ALWAYS:
            return "ALWAYS";
        default:
            return "*** USS ***";
        }
        break;
    default:
        break;
    }

    return s_acTemp;
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
