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

#ifndef NSBRENDERSTATES_H
#define NSBRENDERSTATES_H

#include <NiD3DDefines.h>
#include "NiBinaryShaderLibLibType.h"

class NIBINARYSHADERLIB_ENTRY NSBRenderStates : public NiMemObject
{
public:
    enum NiD3DRenderState
    {
        // NOTE: This enum is used in the streaming code, and deprecated
        // enumerants must be left in place to maintain ordering.

        // NOTE: This enum is duplicated across all platforms, and changes
        // in one should be reflected in the others.

        // Render states supported under these platforms:
        // 9 = DX9, x = Xenon, p = PS3, 0 = D3D10
        NID3DRS_ZENABLE = 0,                     // 9xp0
        NID3DRS_FILLMODE,                        // 9xp0
        NID3DRS_SHADEMODE,                       // 9 
        NID3DRS_ZWRITEENABLE,                    // 9xp0
        NID3DRS_ALPHATESTENABLE,                 // 9xp
        NID3DRS_LASTPIXEL,                       // 9 
        NID3DRS_SRCBLEND,                        // 9xp0
        NID3DRS_DESTBLEND,                       // 9xp0
        NID3DRS_CULLMODE,                        // 9x 0
        NID3DRS_ZFUNC,                           // 9xp0
        NID3DRS_ALPHAREF,                        // 9xp
        NID3DRS_ALPHAFUNC,                       // 9xp
        NID3DRS_DITHERENABLE,                    // 9xp
        NID3DRS_ALPHABLENDENABLE,                // 9xp0
        NID3DRS_FOGENABLE,                       // 9 
        NID3DRS_SPECULARENABLE,                  // 9 
        NID3DRS_FOGCOLOR,                        // 9 
        NID3DRS_FOGTABLEMODE,                    // 9 
        NID3DRS_FOGSTART,                        // 9 
        NID3DRS_FOGEND,                          // 9 
        NID3DRS_FOGDENSITY,                      // 9 
        NID3DRS_RANGEFOGENABLE,                  // 9 
        NID3DRS_STENCILENABLE,                   // 9xp0
        NID3DRS_STENCILFAIL,                     // 9xp0
        NID3DRS_STENCILZFAIL,                    // 9xp0
        NID3DRS_STENCILPASS,                     // 9xp0
        NID3DRS_STENCILFUNC,                     // 9xp0
        NID3DRS_STENCILREF,                      // 9xp0
        NID3DRS_STENCILMASK,                     // 9xp0
        NID3DRS_STENCILWRITEMASK,                // 9xp0
        NID3DRS_TEXTUREFACTOR,                   // 9 
        NID3DRS_WRAP0,                           // 9x
        NID3DRS_WRAP1,                           // 9x
        NID3DRS_WRAP2,                           // 9x
        NID3DRS_WRAP3,                           // 9x
        NID3DRS_WRAP4,                           // 9x
        NID3DRS_WRAP5,                           // 9x
        NID3DRS_WRAP6,                           // 9x
        NID3DRS_WRAP7,                           // 9x
        NID3DRS_CLIPPING,                        // 9 
        NID3DRS_LIGHTING,                        // 9 
        NID3DRS_AMBIENT,                         // 9 
        NID3DRS_FOGVERTEXMODE,                   // 9 
        NID3DRS_COLORVERTEX,                     // 9 
        NID3DRS_LOCALVIEWER,                     // 9 
        NID3DRS_NORMALIZENORMALS,                // 9 
        NID3DRS_DIFFUSEMATERIALSOURCE ,          // 9 
        NID3DRS_SPECULARMATERIALSOURCE,          // 9 
        NID3DRS_AMBIENTMATERIALSOURCE,           // 9 
        NID3DRS_EMISSIVEMATERIALSOURCE,          // 9 
        NID3DRS_VERTEXBLEND,                     // 9 
        NID3DRS_CLIPPLANEENABLE,                 // 9x 0
        NID3DRS_POINTSIZE,                       // 9xp
        NID3DRS_POINTSIZE_MIN,                   // 9x
        NID3DRS_POINTSPRITEENABLE,               // 9xp
        NID3DRS_POINTSCALEENABLE,                // 9 
        NID3DRS_POINTSCALE_A,                    // 9 
        NID3DRS_POINTSCALE_B,                    // 9 
        NID3DRS_POINTSCALE_C,                    // 9 
        NID3DRS_MULTISAMPLEANTIALIAS,            // 9xp0
        NID3DRS_MULTISAMPLEMASK,                 // 9xp0
        NID3DRS_PATCHEDGESTYLE,                  // 9x
        NID3DRS_DEBUGMONITORTOKEN,               // 9 
        NID3DRS_POINTSIZE_MAX,                   // 9x
        NID3DRS_INDEXEDVERTEXBLENDENABLE,        // 9 
        NID3DRS_COLORWRITEENABLE,                // 9x 0
        NID3DRS_TWEENFACTOR,                     // 9 
        NID3DRS_BLENDOP,                         // 9xp0
        NID3DRS_POSITIONDEGREE,                  // 9x
        NID3DRS_NORMALDEGREE,                    // 9x
        NID3DRS_SCISSORTESTENABLE,               // 9xp0
        NID3DRS_SLOPESCALEDEPTHBIAS,             // 9x 0
        NID3DRS_ANTIALIASEDLINEENABLE,           // 9xp0
        NID3DRS_MINTESSELLATIONLEVEL,            // 9x
        NID3DRS_MAXTESSELLATIONLEVEL,            // 9x
        NID3DRS_ADAPTIVETESS_X,                  // 9x
        NID3DRS_ADAPTIVETESS_Y,                  // 9x
        NID3DRS_ADAPTIVETESS_Z,                  // 9x
        NID3DRS_ADAPTIVETESS_W,                  // 9x
        NID3DRS_ENABLEADAPTIVETESSELLATION,      // 9x
        NID3DRS_TWOSIDEDSTENCILMODE,             // 9xp
        NID3DRS_CCW_STENCILFAIL,                 // 9xp0
        NID3DRS_CCW_STENCILZFAIL,                // 9xp0
        NID3DRS_CCW_STENCILPASS,                 // 9xp0
        NID3DRS_CCW_STENCILFUNC,                 // 9xp0
        NID3DRS_COLORWRITEENABLE1,               // 9x 0
        NID3DRS_COLORWRITEENABLE2,               // 9x 0
        NID3DRS_COLORWRITEENABLE3,               // 9x 0
        NID3DRS_BLENDFACTOR,                     // 9x 0
        NID3DRS_SRGBWRITEENABLE,                 // 9xp
        NID3DRS_DEPTHBIAS,                       // 9x 0
        NID3DRS_WRAP8,                           // 9x
        NID3DRS_WRAP9,                           // 9x
        NID3DRS_WRAP10,                          // 9x
        NID3DRS_WRAP11,                          // 9x
        NID3DRS_WRAP12,                          // 9x
        NID3DRS_WRAP13,                          // 9x
        NID3DRS_WRAP14,                          // 9x
        NID3DRS_WRAP15,                          // 9x
        NID3DRS_SEPARATEALPHABLENDENABLE,        // 9xp
        NID3DRS_SRCBLENDALPHA,                   // 9xp0
        NID3DRS_DESTBLENDALPHA,                  // 9xp0
        NID3DRS_BLENDOPALPHA,                    // 9xp0
        NID3DRS_VIEWPORTENABLE,                  //  x
        NID3DRS_HIGHPRECISIONBLENDENABLE,        //  x
        NID3DRS_HIGHPRECISIONBLENDENABLE1,       //  x
        NID3DRS_HIGHPRECISIONBLENDENABLE2,       //  x
        NID3DRS_HIGHPRECISIONBLENDENABLE3,       //  x
        NID3DRS_TESSELLATIONMODE,                //  x

        // Begin PS3-only states
        NIOGLRS_COLORLOGICOPENABLE,              //       (deprecated)
        NIOGLRS_CULLFACEENABLE,                  //   p
        NIOGLRS_MULTISAMPLEENABLE,               //   p
        NIOGLRS_POINTSMOOTHENABLE,               //       (deprecated)
        NIOGLRS_POLYGONOFFSETFILLENABLE,         //   p
        // End PS3-only states

        // PS3 and D3D10 states
        NIOGLRS_SAMPLEALPHATOCOVERAGEENABLE,     //   p0

        // Begin PS3-only states
        NIOGLRS_SAMPLEALPHATOONEENABLE,          //   p
        NIOGLRS_SAMPLECOVERAGEENABLE,            //       (deprecated)
        NIOGLRS_VERTEXPROGRAMPOINTSIZEENABLE,    //       (deprecated)
        NIOGLRS_BLENDCOLORR,                     //   p
        NIOGLRS_BLENDCOLORG,                     //   p
        NIOGLRS_BLENDCOLORB,                     //   p
        NIOGLRS_BLENDCOLORA,                     //   p
        NIOGLRS_SRCBLENDSEPARATERGB,             //       (deprecated)
        NIOGLRS_DSTBLENDSEPARATERGB,             //       (deprecated)     
        NIOGLRS_BLENDEQUATIONSEPARATERGB,        //       (deprecated)
        NIOGLRS_CULLFACE,                        //   p
        NIOGLRS_COLORMASKR,                      //   p
        NIOGLRS_COLORMASKG,                      //   p
        NIOGLRS_COLORMASKB,                      //   p
        NIOGLRS_COLORMASKA,                      //   p
        NIOGLRS_DEPTHRANGENEAR,                  //   p
        NIOGLRS_DEPTHRANGEFAR,                   //   p
        NIOGLRS_FRONTFACE,                       //   p
        NIOGLRS_LINEWIDTH,                       //   p
        NIOGLRS_POINTSPRITECOORDREPLACE,         //       (deprecated)
        NIOGLRS_POLYGONMODEFACE,                 //       (deprecated)
        NIOGLRS_POLYGONOFFSETFACTOR,             //   p
        NIOGLRS_POLYGONOFFSETUNITS,              //   p
        NIOGLRS_SCISSORX,                        //   p
        NIOGLRS_SCISSORY,                        //   p
        NIOGLRS_SCISSORWIDTH,                    //   p
        NIOGLRS_SCISSORHEIGHT,                   //   p
        // End PS3-only states

        // Begin D3D10-only states.
        NID3DRS_ALPHABLENDENABLE1,               //    0
        NID3DRS_ALPHABLENDENABLE2,               //    0
        NID3DRS_ALPHABLENDENABLE3,               //    0
        NID3DRS_ALPHABLENDENABLE4,               //    0
        NID3DRS_ALPHABLENDENABLE5,               //    0
        NID3DRS_ALPHABLENDENABLE6,               //    0
        NID3DRS_ALPHABLENDENABLE7,               //    0
        NID3DRS_COLORWRITEENABLE4,               //    0
        NID3DRS_COLORWRITEENABLE5,               //    0
        NID3DRS_COLORWRITEENABLE6,               //    0
        NID3DRS_COLORWRITEENABLE7,               //    0
        NID3DRS_FRONTCCW,                        //    0
        NID3DRS_DEPTHBIASCLAMP,                  //    0
        // End D3D10-only states.

        //
        NID3DRS_COUNT,
        NID3DRS_DEPRECATED = 0x7ffffffe,
        NID3DRS_INVALID    = 0x7fffffff
    };

    enum NiD3DZBufferType
    {
        NID3DZB_FALSE   = 0,
        NID3DZB_TRUE,
        NID3DZB_USEW,
        // 
        NID3DZB_COUNT,
        NID3DZB_INVALID = 0x7fffffff
    };

    enum NiD3DFillMode
    {
        NID3DFILL_POINT     = 0,
        NID3DFILL_WIREFRAME,
        NID3DFILL_SOLID,
        // 
        NID3DFILL_COUNT,
        NID3DFILL_INVALID   = 0x7fffffff
    };

    enum NiD3DShadeMode
    {
        NID3DSHADE_FLAT     = 0,
        NID3DSHADE_GOURAUD,
        NID3DSHADE_PHONG,
        // 
        NID3DSHADE_COUNT,
        NID3DSHADE_INVALID  = 0x7fffffff
    };

    enum NiD3DBlend
    {
        NID3DBLEND_ZERO     =  0,
        NID3DBLEND_ONE,
        NID3DBLEND_SRCCOLOR,
        NID3DBLEND_INVSRCCOLOR,
        NID3DBLEND_SRCALPHA,
        NID3DBLEND_INVSRCALPHA,
        NID3DBLEND_DESTALPHA,
        NID3DBLEND_INVDESTALPHA,
        NID3DBLEND_DESTCOLOR,
        NID3DBLEND_INVDESTCOLOR,
        NID3DBLEND_SRCALPHASAT,
        // DX9
        NID3DBLEND_BOTHSRCALPHA,
        NID3DBLEND_BOTHINVSRCALPHA,
        // DX9, Xenon, PS3
        NID3DBLEND_BLENDFACTOR,
        NID3DBLEND_INVBLENDFACTOR,
        // Xenon, PS3
        NID3DBLEND_CONSTANTALPHA,
        NID3DBLEND_INVCONSTANTALPHA,
        // D3D10
        NID3DBLEND_SRC1_COLOR,
        NID3DBLEND_INV_SRC1_COLOR,
        NID3DBLEND_SRC1_ALPHA,
        NID3DBLEND_INV_SRC1_ALPHA,
        // 
        NID3DBLEND_COUNT,
        NID3DBLEND_INVALID  = 0x7fffffff
    };

    enum NiD3DCull
    {
        NID3DCULL_NONE      = 0,
        NID3DCULL_CW,
        NID3DCULL_CCW,
        // 
        NID3DCULL_COUNT,
        NID3DCULL_INVALID   = 0x7fffffff
    };

    enum NiD3DCmpFunc
    {
        NID3DCMP_NEVER    = 0,
        NID3DCMP_LESS,
        NID3DCMP_EQUAL,
        NID3DCMP_LESSEQUAL,
        NID3DCMP_GREATER,
        NID3DCMP_NOTEQUAL,
        NID3DCMP_GREATEREQUAL,
        NID3DCMP_ALWAYS,
        // 
        NID3DCMP_COUNT,
        NID3DCMP_INVALID  = 0x7fffffff
    };

    enum NiD3DFogMode
    {
        // DX9
        NID3DFOG_NONE     = 0,
        NID3DFOG_EXP,
        NID3DFOG_EXP2,
        NID3DFOG_LINEAR,
        // 
        NID3DFOG_COUNT,
        NID3DFOG_INVALID  = 0x7fffffff
    };

    enum NiD3DStencilOp
    {
        NID3DSTENCILOP_KEEP     = 0,
        NID3DSTENCILOP_ZERO,
        NID3DSTENCILOP_REPLACE,
        NID3DSTENCILOP_INCRSAT,
        NID3DSTENCILOP_DECRSAT,
        NID3DSTENCILOP_INVERT,
        NID3DSTENCILOP_INCR,
        NID3DSTENCILOP_DECR,
        // 
        NID3DSTENCILOP_COUNT,
        NID3DSTENCILOP_INVALID  = 0x7fffffff
    };

    enum NiD3DWrap
    {
        NID3DWRAP_DISABLED  = 0,
        NID3DWRAP_U,
        NID3DWRAP_V,
        NID3DWRAP_W,
        NID3DWRAP_UV,
        NID3DWRAP_UW,
        NID3DWRAP_VW,
        NID3DWRAP_UVW,
        // 
        NID3DWRAP_COUNT,
        NID3DWRAP_INVALID   = 0x7fffffff
    };

    enum NiD3DMaterialColorSource
    {
        NID3DMCS_MATERIAL   = 0,
        NID3DMCS_COLOR1,
        NID3DMCS_COLOR2,
        // 
        NID3DMCS_COUNT,
        NID3DMCS_INVALID    = 0x7fffffff
    };

    enum NiD3DVertexBlendFlags
    {
        // DX9
        NID3DVBF_DISABLE    = 0,
        NID3DVBF_1WEIGHTS,
        NID3DVBF_2WEIGHTS,
        NID3DVBF_3WEIGHTS,
        NID3DVBF_TWEENING,
        NID3DVBF_0WEIGHTS,
        // 
        NID3DVBF_COUNT,
        NID3DVBF_DEPRECATED = 0x7ffffffe,
        NID3DVBF_INVALID    = 0x7fffffff
    };

    enum NiD3DPatchEdgeStyle
    {
        NID3DPATCHEDGE_DISCRETE = 0,
        NID3DPATCHEDGE_CONTINUOUS,
        // 
        NID3DPATCHEDGE_COUNT,
        NID3DPATCHEDGE_INVALID  = 0x7fffffff,
    };

    enum NiD3DDebugMonitorTokens
    {
        // DX9
        NID3DDMT_ENABLE     = 0,
        NID3DDMT_DISABLE,
        // 
        NID3DDMT_COUNT,
        NID3DDMT_INVALID    = 0x7fffffff
    };

    enum NiD3DBlendOp
    {
        NID3DBLENDOP_ADD          = 0,
        NID3DBLENDOP_SUBTRACT,
        NID3DBLENDOP_REVSUBTRACT,
        NID3DBLENDOP_MIN,
        NID3DBLENDOP_MAX,
        // 
        NID3DBLENDOP_COUNT,
        NID3DBLENDOP_DEPRECATED = 0x7ffffffe,
        NID3DBLENDOP_INVALID    = 0x7fffffff
    };

    enum NiD3DDegreeType
    {
        NID3DDEGREE_LINEAR      = 1,
        NID3DDEGREE_QUADRATIC   = 2,
        NID3DDEGREE_CUBIC       = 3,
        NID3DDEGREE_QUINTIC     = 5,
        NID3DDEGREE_INVALID     = 0x7fffffff
    };

    enum NiD3DTessellationMode
    {
        // Xenon
        NID3DTM_DISCRETE    = 0,
        NID3DTM_CONTINUOUS,
        NID3DTM_PEREDGE,
        //
        NID3DTM_COUNT,
        NID3DTM_INVALID     = 0x7fffffff
    };

    enum NiOGLFace
    {
        NIOGLFACE_FRONT = 0,
        NIOGLFACE_BACK,
        NIOGLFACE_FRONT_AND_BACK,
        //
        NIOGLFACE_COUNT,
        NIOGLFACE_INVALID     = 0x7fffffff
    };

    static bool ConvertNSBRenderStateValue(
        NiD3DRenderState eNSBState, unsigned int uiNSBValue,
        unsigned int& uiD3DValue);

    static NiD3DRenderState LookupRenderState(const char* pcRenderState);
    static bool LookupRenderStateValue(NiD3DRenderState eRenderState, 
        const char* pcValue, unsigned int& uiValue);

    static bool GetD3DRenderStateType(NiD3DRenderState eRenderState,
        D3DRENDERSTATETYPE& eD3DRS);
    static bool GetD3DZBufferType(NiD3DZBufferType eZBufferType, 
        unsigned int& uiD3DValue);
    static bool GetD3DFillMode(NiD3DFillMode eFillMode, 
        unsigned int& uiD3DValue);
    static bool GetD3DShadeMode(NiD3DShadeMode eShadeMode, 
        unsigned int& uiD3DValue);
    static bool GetD3DBlend(NiD3DBlend eBlend, unsigned int& uiD3DValue);
    static bool GetD3DCull(NiD3DCull eCull, unsigned int& uiD3DValue);
    static bool GetD3DCmpFunc(NiD3DCmpFunc eCmpFunc, 
        unsigned int& uiD3DValue);
    static bool GetD3DFogMode(NiD3DFogMode eFogMode, 
        unsigned int& uiD3DValue);
    static bool GetD3DStencilOp(NiD3DStencilOp eStencilOp, 
        unsigned int& uiD3DValue);
    static bool GetD3DWrap(NiD3DWrap eWrap, unsigned int& uiD3DValue);
    static bool GetD3DMaterialColorSource(
        NiD3DMaterialColorSource eMaterialColorSource, 
        unsigned int& uiD3DValue);
    static bool GetD3DVertexBlendFlags(
        NiD3DVertexBlendFlags eVertexBlendFlags, unsigned int& uiD3DValue);
    static bool GetD3DPatchEdgeStyle(NiD3DPatchEdgeStyle ePatchEdgeStyle, 
        unsigned int& uiD3DValue);
    static bool GetD3DDebugMonitorTokens(
        NiD3DDebugMonitorTokens eDebugMonitorTokens, 
        unsigned int& uiD3DValue);
    static bool GetD3DBlendOp(NiD3DBlendOp eBlendOp, 
        unsigned int& uiD3DValue);
    static bool GetD3DDegreeType(NiD3DDegreeType eDegreeType, 
        unsigned int& uiD3DValue);
    static bool GetD3DTessellationMode(NiD3DTessellationMode eTessMode, 
        unsigned int& uiD3DValue);

#if defined(NIDEBUG)
    static const char* LookupRenderStateString(NiD3DRenderState eRS);
#endif  //#if defined(NIDEBUG)
};

#endif  //NSBRENDERSTATES_H
