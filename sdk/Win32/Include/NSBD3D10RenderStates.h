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

#ifndef NSBD3D10RENDERSTATES_H
#define NSBD3D10RENDERSTATES_H

#include <NiD3D10Renderer.h>
#include "NiD3D10BinaryShaderLibLibType.h"

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10RenderStates : public NiMemObject
{
public:
    enum NiD3D10RenderState
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

    enum NiD3D10FillMode
    {
        NID3DFILL_POINT     = 0,
        NID3DFILL_WIREFRAME,
        NID3DFILL_SOLID,
        // 
        NID3DFILL_COUNT,
        NID3DFILL_INVALID   = 0x7fffffff
    };

    enum NiD3D10Blend
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

    enum NiD3D10Cull
    {
        NID3DCULL_NONE      = 0,
        NID3DCULL_CW,
        NID3DCULL_CCW,
        // 
        NID3DCULL_COUNT,
        NID3DCULL_INVALID   = 0x7fffffff
    };

    enum NiD3D10CmpFunc
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

    enum NiD3D10StencilOp
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

    enum NiD3D10BlendOp
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

    static bool ConvertNSBD3D10RenderStateValue(
        NiD3D10RenderState eNSBD3D10State, unsigned int uiNSBD3D10Value,
        unsigned int& uiD3D10Value);

    static NiD3D10RenderState LookupRenderState(const char* pcRenderState);
    static bool LookupRenderStateValue(NiD3D10RenderState eRenderState, 
        const char* pcValue, unsigned int& uiValue);

    static bool GetD3D10FillMode(NiD3D10FillMode eFillMode, 
        unsigned int& uiD3D10Value);
    static bool GetD3D10Blend(NiD3D10Blend eBlend,
        unsigned int& uiD3D10Value);
    static bool GetD3D10Cull(NiD3D10Cull eCull, unsigned int& uiD3D10Value);
    static bool GetD3D10CmpFunc(NiD3D10CmpFunc eCmpFunc, 
        unsigned int& uiD3D10Value);
    static bool GetD3D10StencilOp(NiD3D10StencilOp eStencilOp, 
        unsigned int& uiD3D10Value);
    static bool GetD3D10BlendOp(NiD3D10BlendOp eBlendOp, 
        unsigned int& uiD3D10Value);

#if defined(NIDEBUG)
    static const char* LookupRenderStateString(NiD3D10RenderState eRS);
#endif  //#if defined(NIDEBUG)

};

#endif  //NSBD3D10RENDERSTATES_H
