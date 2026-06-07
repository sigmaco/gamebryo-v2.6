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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10RenderStates.h"

#if defined(NIDEBUG)
#include "NSBD3D10Utility.h"
#endif  //#if defined(NIDEBUG)

//---------------------------------------------------------------------------
NSBD3D10RenderStates::NiD3D10RenderState
    NSBD3D10RenderStates::LookupRenderState(const char* pcRenderState)
{
    if (!pcRenderState || pcRenderState[0] == '\0')
        return NID3DRS_INVALID;

    if (NiStricmp(pcRenderState, "ZENABLE") == 0)
        return NID3DRS_ZENABLE;
    if (NiStricmp(pcRenderState, "FILLMODE") == 0)
        return NID3DRS_FILLMODE;
    if (NiStricmp(pcRenderState, "SHADEMODE") == 0)
        return NID3DRS_SHADEMODE;
    if (NiStricmp(pcRenderState, "ZWRITEENABLE") == 0)
        return NID3DRS_ZWRITEENABLE;
    if (NiStricmp(pcRenderState, "ALPHATESTENABLE") == 0)
        return NID3DRS_ALPHATESTENABLE;
    if (NiStricmp(pcRenderState, "LASTPIXEL") == 0)
        return NID3DRS_LASTPIXEL;
    if (NiStricmp(pcRenderState, "SRCBLEND") == 0)
        return NID3DRS_SRCBLEND;
    if (NiStricmp(pcRenderState, "DESTBLEND") == 0)
        return NID3DRS_DESTBLEND;
    if (NiStricmp(pcRenderState, "CULLMODE") == 0)
        return NID3DRS_CULLMODE;
    if (NiStricmp(pcRenderState, "ZFUNC") == 0)
        return NID3DRS_ZFUNC;
    if (NiStricmp(pcRenderState, "ALPHAREF") == 0)
        return NID3DRS_ALPHAREF;
    if (NiStricmp(pcRenderState, "ALPHAFUNC") == 0)
        return NID3DRS_ALPHAFUNC;
    if (NiStricmp(pcRenderState, "DITHERENABLE") == 0)
        return NID3DRS_DITHERENABLE;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE") == 0)
        return NID3DRS_ALPHABLENDENABLE;
    if (NiStricmp(pcRenderState, "FOGENABLE") == 0)
        return NID3DRS_FOGENABLE;
    if (NiStricmp(pcRenderState, "SPECULARENABLE") == 0)
        return NID3DRS_SPECULARENABLE;
    if (NiStricmp(pcRenderState, "FOGCOLOR") == 0)
        return NID3DRS_FOGCOLOR;
    if (NiStricmp(pcRenderState, "FOGTABLEMODE") == 0)
        return NID3DRS_FOGTABLEMODE;
    if (NiStricmp(pcRenderState, "FOGSTART") == 0)
        return NID3DRS_FOGSTART;
    if (NiStricmp(pcRenderState, "FOGEND") == 0)
        return NID3DRS_FOGEND;
    if (NiStricmp(pcRenderState, "FOGDENSITY") == 0)
        return NID3DRS_FOGDENSITY;
    if (NiStricmp(pcRenderState, "RANGEFOGENABLE") == 0)
        return NID3DRS_RANGEFOGENABLE;
    if (NiStricmp(pcRenderState, "STENCILENABLE") == 0)
        return NID3DRS_STENCILENABLE;
    if (NiStricmp(pcRenderState, "STENCILFAIL") == 0)
        return NID3DRS_STENCILFAIL;
    if (NiStricmp(pcRenderState, "STENCILZFAIL") == 0)
        return NID3DRS_STENCILZFAIL;
    if (NiStricmp(pcRenderState, "STENCILPASS") == 0)
        return NID3DRS_STENCILPASS;
    if (NiStricmp(pcRenderState, "STENCILFUNC") == 0)
        return NID3DRS_STENCILFUNC;
    if (NiStricmp(pcRenderState, "STENCILREF") == 0)
        return NID3DRS_STENCILREF;
    if (NiStricmp(pcRenderState, "STENCILMASK") == 0)
        return NID3DRS_STENCILMASK;
    if (NiStricmp(pcRenderState, "STENCILWRITEMASK") == 0)
        return NID3DRS_STENCILWRITEMASK;
    if (NiStricmp(pcRenderState, "TEXTUREFACTOR") == 0)
        return NID3DRS_TEXTUREFACTOR;
    if (NiStricmp(pcRenderState, "WRAP0") == 0)
        return NID3DRS_WRAP0;
    if (NiStricmp(pcRenderState, "WRAP1") == 0)
        return NID3DRS_WRAP1;
    if (NiStricmp(pcRenderState, "WRAP2") == 0)
        return NID3DRS_WRAP2;
    if (NiStricmp(pcRenderState, "WRAP3") == 0)
        return NID3DRS_WRAP3;
    if (NiStricmp(pcRenderState, "WRAP4") == 0)
        return NID3DRS_WRAP4;
    if (NiStricmp(pcRenderState, "WRAP5") == 0)
        return NID3DRS_WRAP5;
    if (NiStricmp(pcRenderState, "WRAP6") == 0)
        return NID3DRS_WRAP6;
    if (NiStricmp(pcRenderState, "WRAP7") == 0)
        return NID3DRS_WRAP7;
    if (NiStricmp(pcRenderState, "CLIPPING") == 0)
        return NID3DRS_CLIPPING;
    if (NiStricmp(pcRenderState, "LIGHTING") == 0)
        return NID3DRS_LIGHTING;
    if (NiStricmp(pcRenderState, "AMBIENT") == 0)
        return NID3DRS_AMBIENT;
    if (NiStricmp(pcRenderState, "FOGVERTEXMODE") == 0)
        return NID3DRS_FOGVERTEXMODE;
    if (NiStricmp(pcRenderState, "COLORVERTEX") == 0)
        return NID3DRS_COLORVERTEX;
    if (NiStricmp(pcRenderState, "LOCALVIEWER") == 0)
        return NID3DRS_LOCALVIEWER;
    if (NiStricmp(pcRenderState, "NORMALIZENORMALS") == 0)
        return NID3DRS_NORMALIZENORMALS;
    if (NiStricmp(pcRenderState, "DIFFUSEMATERIALSOURCE ") == 0)
        return NID3DRS_DIFFUSEMATERIALSOURCE ;
    if (NiStricmp(pcRenderState, "SPECULARMATERIALSOURCE") == 0)
        return NID3DRS_SPECULARMATERIALSOURCE;
    if (NiStricmp(pcRenderState, "AMBIENTMATERIALSOURCE") == 0)
        return NID3DRS_AMBIENTMATERIALSOURCE;
    if (NiStricmp(pcRenderState, "EMISSIVEMATERIALSOURCE") == 0)
        return NID3DRS_EMISSIVEMATERIALSOURCE;
    if (NiStricmp(pcRenderState, "VERTEXBLEND") == 0)
        return NID3DRS_VERTEXBLEND;
    if (NiStricmp(pcRenderState, "CLIPPLANEENABLE") == 0)
        return NID3DRS_CLIPPLANEENABLE;
    if (NiStricmp(pcRenderState, "POINTSIZE") == 0)
        return NID3DRS_POINTSIZE;
    if (NiStricmp(pcRenderState, "POINTSIZE_MIN") == 0)
        return NID3DRS_POINTSIZE_MIN;
    if (NiStricmp(pcRenderState, "POINTSPRITEENABLE") == 0)
        return NID3DRS_POINTSPRITEENABLE;
    if (NiStricmp(pcRenderState, "POINTSCALEENABLE") == 0)
        return NID3DRS_POINTSCALEENABLE;
    if (NiStricmp(pcRenderState, "POINTSCALE_A") == 0)
        return NID3DRS_POINTSCALE_A;
    if (NiStricmp(pcRenderState, "POINTSCALE_B") == 0)
        return NID3DRS_POINTSCALE_B;
    if (NiStricmp(pcRenderState, "POINTSCALE_C") == 0)
        return NID3DRS_POINTSCALE_C;
    if (NiStricmp(pcRenderState, "MULTISAMPLEANTIALIAS") == 0)
        return NID3DRS_MULTISAMPLEANTIALIAS;
    if (NiStricmp(pcRenderState, "MULTISAMPLEMASK") == 0)
        return NID3DRS_MULTISAMPLEMASK;
    if (NiStricmp(pcRenderState, "PATCHEDGESTYLE") == 0)
        return NID3DRS_PATCHEDGESTYLE;
    if (NiStricmp(pcRenderState, "DEBUGMONITORTOKEN") == 0)
        return NID3DRS_DEBUGMONITORTOKEN;
    if (NiStricmp(pcRenderState, "POINTSIZE_MAX") == 0)
        return NID3DRS_POINTSIZE_MAX;
    if (NiStricmp(pcRenderState, "INDEXEDVERTEXBLENDENABLE") == 0)
        return NID3DRS_INDEXEDVERTEXBLENDENABLE;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE") == 0)
        return NID3DRS_COLORWRITEENABLE;
    if (NiStricmp(pcRenderState, "TWEENFACTOR") == 0)
        return NID3DRS_TWEENFACTOR;
    if (NiStricmp(pcRenderState, "BLENDOP") == 0)
        return NID3DRS_BLENDOP;
    if (NiStricmp(pcRenderState, "POSITIONDEGREE") == 0)
        return NID3DRS_POSITIONDEGREE;
    if (NiStricmp(pcRenderState, "NORMALDEGREE") == 0)
        return NID3DRS_NORMALDEGREE;
    if (NiStricmp(pcRenderState, "SCISSORTESTENABLE") == 0)
        return NID3DRS_SCISSORTESTENABLE;
    if (NiStricmp(pcRenderState, "SLOPESCALEDEPTHBIAS") == 0)
        return NID3DRS_SLOPESCALEDEPTHBIAS;
    if (NiStricmp(pcRenderState, "ANTIALIASEDLINEENABLE") == 0)
        return NID3DRS_ANTIALIASEDLINEENABLE;
    if (NiStricmp(pcRenderState, "MINTESSELLATIONLEVEL") == 0)
        return NID3DRS_MINTESSELLATIONLEVEL;
    if (NiStricmp(pcRenderState, "MAXTESSELLATIONLEVEL") == 0)
        return NID3DRS_MAXTESSELLATIONLEVEL;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_X") == 0)
        return NID3DRS_ADAPTIVETESS_X;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_Y") == 0)
        return NID3DRS_ADAPTIVETESS_Y;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_Z") == 0)
        return NID3DRS_ADAPTIVETESS_Z;
    if (NiStricmp(pcRenderState, "ADAPTIVETESS_W") == 0)
        return NID3DRS_ADAPTIVETESS_W;
    if (NiStricmp(pcRenderState, "ENABLEADAPTIVETESSELATION") == 0 ||
        NiStricmp(pcRenderState, "ENABLEADAPTIVETESSELLATION") == 0)
    {
        return NID3DRS_ENABLEADAPTIVETESSELLATION;
    }
    if (NiStricmp(pcRenderState, "TWOSIDEDSTENCILMODE") == 0)
        return NID3DRS_TWOSIDEDSTENCILMODE;
    if (NiStricmp(pcRenderState, "CCW_STENCILFAIL") == 0)
        return NID3DRS_CCW_STENCILFAIL;
    if (NiStricmp(pcRenderState, "CCW_STENCILZFAIL") == 0)
        return NID3DRS_CCW_STENCILZFAIL;
    if (NiStricmp(pcRenderState, "CCW_STENCILPASS") == 0)
        return NID3DRS_CCW_STENCILPASS;
    if (NiStricmp(pcRenderState, "CCW_STENCILFUNC") == 0)
        return NID3DRS_CCW_STENCILFUNC;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE1") == 0)
        return NID3DRS_COLORWRITEENABLE1;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE2") == 0)
        return NID3DRS_COLORWRITEENABLE2;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE3") == 0)
        return NID3DRS_COLORWRITEENABLE3;
    if (NiStricmp(pcRenderState, "BLENDFACTOR") == 0)
        return NID3DRS_BLENDFACTOR;
    if (NiStricmp(pcRenderState, "SRGBWRITEENABLE") == 0)
        return NID3DRS_SRGBWRITEENABLE;
    if (NiStricmp(pcRenderState, "DEPTHBIAS") == 0)
        return NID3DRS_DEPTHBIAS;
    if (NiStricmp(pcRenderState, "WRAP8") == 0)
        return NID3DRS_WRAP8;
    if (NiStricmp(pcRenderState, "WRAP9") == 0)
        return NID3DRS_WRAP9;
    if (NiStricmp(pcRenderState, "WRAP10") == 0)
        return NID3DRS_WRAP10;
    if (NiStricmp(pcRenderState, "WRAP11") == 0)
        return NID3DRS_WRAP11;
    if (NiStricmp(pcRenderState, "WRAP12") == 0)
        return NID3DRS_WRAP12;
    if (NiStricmp(pcRenderState, "WRAP13") == 0)
        return NID3DRS_WRAP13;
    if (NiStricmp(pcRenderState, "WRAP14") == 0)
        return NID3DRS_WRAP14;
    if (NiStricmp(pcRenderState, "WRAP15") == 0)
        return NID3DRS_WRAP15;
    if (NiStricmp(pcRenderState, "SEPARATEALPHABLENDENABLE") == 0)
        return NID3DRS_SEPARATEALPHABLENDENABLE;
    if (NiStricmp(pcRenderState, "SRCBLENDALPHA") == 0 ||
        NiStricmp(pcRenderState, "SRCBLENDSEPARATEA") == 0)
        return NID3DRS_SRCBLENDALPHA;
    if (NiStricmp(pcRenderState, "DESTBLENDALPHA") == 0 ||
        NiStricmp(pcRenderState, "DSTBLENDSEPARATEA") == 0)
        return NID3DRS_DESTBLENDALPHA;
    if (NiStricmp(pcRenderState, "BLENDOPALPHA") == 0 ||
        NiStricmp(pcRenderState, "BLENDEQUATIONSEPARATEA") == 0)
        return NID3DRS_BLENDOPALPHA;
    if (NiStricmp(pcRenderState, "VIEWPORTENABLE") == 0)
        return NID3DRS_VIEWPORTENABLE;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE") == 0)
        return NID3DRS_HIGHPRECISIONBLENDENABLE;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE1") == 0)
        return NID3DRS_HIGHPRECISIONBLENDENABLE1;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE2") == 0)
        return NID3DRS_HIGHPRECISIONBLENDENABLE2;
    if (NiStricmp(pcRenderState, "HIGHPRECISIONBLENDENABLE3") == 0)
        return NID3DRS_HIGHPRECISIONBLENDENABLE3;
    if (NiStricmp(pcRenderState, "TESSELLATIONMODE") == 0)
        return NID3DRS_TESSELLATIONMODE;

    // Begin PS3-only states.
    if (NiStricmp(pcRenderState, "COLORLOGICOPENABLE") == 0)
        return NIOGLRS_COLORLOGICOPENABLE;
    if (NiStricmp(pcRenderState, "CULLFACEENABLE") == 0)
        return NIOGLRS_CULLFACEENABLE;
    if (NiStricmp(pcRenderState, "MULTISAMPLEENABLE") == 0)
        return NIOGLRS_MULTISAMPLEENABLE;
    if (NiStricmp(pcRenderState, "POINTSMOOTHENABLE") == 0)
        return NIOGLRS_POINTSMOOTHENABLE;
    if (NiStricmp(pcRenderState, "POLYGONOFFSETFILLENABLE") == 0)
        return NIOGLRS_POLYGONOFFSETFILLENABLE;
    // End PS3-only states.

    // PS3 and D3D10 state.
    if (NiStricmp(pcRenderState, "SAMPLEALPHATOCOVERAGEENABLE") == 0)
        return NIOGLRS_SAMPLEALPHATOCOVERAGEENABLE;

    // Begin PS3-only states.
    if (NiStricmp(pcRenderState, "SAMPLEALPHATOONEENABLE") == 0)
        return NIOGLRS_SAMPLEALPHATOONEENABLE;
    if (NiStricmp(pcRenderState, "SAMPLECOVERAGEENABLE") == 0)
        return NIOGLRS_SAMPLECOVERAGEENABLE;
    if (NiStricmp(pcRenderState, "VERTEXPROGRAMPOINTSIZEENABLE") == 0)
        return NIOGLRS_VERTEXPROGRAMPOINTSIZEENABLE;
    if (NiStricmp(pcRenderState, "BLENDCOLORR") == 0)
        return NIOGLRS_BLENDCOLORR;
    if (NiStricmp(pcRenderState, "BLENDCOLORG") == 0)
        return NIOGLRS_BLENDCOLORG;
    if (NiStricmp(pcRenderState, "BLENDCOLORB") == 0)
        return NIOGLRS_BLENDCOLORB;
    if (NiStricmp(pcRenderState, "BLENDCOLORA") == 0)
        return NIOGLRS_BLENDCOLORA;
    if (NiStricmp(pcRenderState, "SRCBLENDSEPARATERGB") == 0)
        return NIOGLRS_SRCBLENDSEPARATERGB;
    if (NiStricmp(pcRenderState, "DSTBLENDSEPARATERGB") == 0)
        return NIOGLRS_DSTBLENDSEPARATERGB;
    if (NiStricmp(pcRenderState, "BLENDEQUATIONSEPARATERGB") == 0)
        return NIOGLRS_BLENDEQUATIONSEPARATERGB;
    if (NiStricmp(pcRenderState, "CULLFACE") == 0)
        return NIOGLRS_CULLFACE;
    if (NiStricmp(pcRenderState, "COLORMASKR") == 0)
        return NIOGLRS_COLORMASKR;
    if (NiStricmp(pcRenderState, "COLORMASKG") == 0)
        return NIOGLRS_COLORMASKG;
    if (NiStricmp(pcRenderState, "COLORMASKB") == 0)
        return NIOGLRS_COLORMASKB;
    if (NiStricmp(pcRenderState, "COLORMASKA") == 0)
        return NIOGLRS_COLORMASKA;
    if (NiStricmp(pcRenderState, "DEPTHRANGENEAR") == 0)
        return NIOGLRS_DEPTHRANGENEAR;
    if (NiStricmp(pcRenderState, "DEPTHRANGEFAR") == 0)
        return NIOGLRS_DEPTHRANGEFAR;
    if (NiStricmp(pcRenderState, "FRONTFACE") == 0)
        return NIOGLRS_FRONTFACE;
    if (NiStricmp(pcRenderState, "LINEWIDTH") == 0)
        return NIOGLRS_LINEWIDTH;
    if (NiStricmp(pcRenderState, "POINTSPRITECOORDREPLACE") == 0)
        return NIOGLRS_POINTSPRITECOORDREPLACE;
    if (NiStricmp(pcRenderState, "POLYGONMODEFACE") == 0)
        return NIOGLRS_POLYGONMODEFACE;
    if (NiStricmp(pcRenderState, "POLYGONOFFSETFACTOR") == 0)
        return NIOGLRS_POLYGONOFFSETFACTOR;
    if (NiStricmp(pcRenderState, "POLYGONOFFSETUNITS") == 0)
        return NIOGLRS_POLYGONOFFSETUNITS;
    if (NiStricmp(pcRenderState, "SCISSORX") == 0)
        return NIOGLRS_SCISSORX;
    if (NiStricmp(pcRenderState, "SCISSORY") == 0)
        return NIOGLRS_SCISSORY;
    if (NiStricmp(pcRenderState, "SCISSORWIDTH") == 0)
        return NIOGLRS_SCISSORWIDTH;
    if (NiStricmp(pcRenderState, "SCISSORHEIGHT") == 0)
        return NIOGLRS_SCISSORHEIGHT;
    // End PS3-only states.

    // Begin D3D10-only states.
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE1") == 0)
        return NID3DRS_ALPHABLENDENABLE1;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE2") == 0)
        return NID3DRS_ALPHABLENDENABLE2;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE3") == 0)
        return NID3DRS_ALPHABLENDENABLE3;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE4") == 0)
        return NID3DRS_ALPHABLENDENABLE4;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE5") == 0)
        return NID3DRS_ALPHABLENDENABLE5;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE6") == 0)
        return NID3DRS_ALPHABLENDENABLE6;
    if (NiStricmp(pcRenderState, "ALPHABLENDENABLE7") == 0)
        return NID3DRS_ALPHABLENDENABLE7;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE4") == 0)
        return NID3DRS_COLORWRITEENABLE4;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE5") == 0)
        return NID3DRS_COLORWRITEENABLE5;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE6") == 0)
        return NID3DRS_COLORWRITEENABLE6;
    if (NiStricmp(pcRenderState, "COLORWRITEENABLE7") == 0)
        return NID3DRS_COLORWRITEENABLE7;
    if (NiStricmp(pcRenderState, "FRONTCCW") == 0)
        return NID3DRS_FRONTCCW;
    if (NiStricmp(pcRenderState, "DEPTHBIASCLAMP") == 0)
        return NID3DRS_DEPTHBIASCLAMP;
    // End D3D10-only states.

    // Deprecated values
    if (NiStricmp(pcRenderState, "LINEPATTERN") == 0 ||
        NiStricmp(pcRenderState, "EDGEANTIALIAS") == 0 ||
        NiStricmp(pcRenderState, "ZBIAS") == 0 ||
        NiStricmp(pcRenderState, "SOFTWAREVERTEXPROCESSING") == 0 ||
        NiStricmp(pcRenderState, "PATCHSEGMENTS") == 0 ||
        NiStricmp(pcRenderState, "POSITIONORDER") == 0 ||
        NiStricmp(pcRenderState, "NORMALORDER") == 0 ||
        NiStricmp(pcRenderState, "BLENDCOLOR") == 0 ||
        NiStricmp(pcRenderState, "SWATHWIDTH") == 0 ||
        NiStricmp(pcRenderState, "POLYGONOFFSETZSLOPESCALE") == 0 ||
        NiStricmp(pcRenderState, "POLYGONOFFSETZOFFSET") == 0 ||
        NiStricmp(pcRenderState, "POINTOFFSETENABLE") == 0 ||
        NiStricmp(pcRenderState, "WIREFRAMEOFFSETENABLE") == 0 ||
        NiStricmp(pcRenderState, "SOLIDOFFSETENABLE") == 0 ||
        NiStricmp(pcRenderState, "DEPTHCLIPCONTROL") == 0 ||
        NiStricmp(pcRenderState, "STIPPLEENABLE") == 0 ||
        NiStricmp(pcRenderState, "BACKSPECULARMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKDIFFUSEMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKAMBIENTMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKEMISSIVEMATERIALSOURCE") == 0 ||
        NiStricmp(pcRenderState, "BACKAMBIENT") == 0 ||
        NiStricmp(pcRenderState, "SWAPFILTER") == 0 ||
        NiStricmp(pcRenderState, "PRESENTATIONINTERVAL") == 0 ||
        NiStricmp(pcRenderState, "BACKFILLMODE") == 0 ||
        NiStricmp(pcRenderState, "TWOSIDEDLIGHTING") == 0 ||
        // NiStricmp(pcRenderState, "FRONTFACE") == 0 ||  // Now an OGL state.
        NiStricmp(pcRenderState, "LOGICOP") == 0 ||
        NiStricmp(pcRenderState, "MULTISAMPLEMODE") == 0 ||
        NiStricmp(pcRenderState, "MULTISAMPLERENDERTARGETMODE") == 0 ||
        NiStricmp(pcRenderState, "SHADOWFUNC") == 0 ||
        // NiStricmp(pcRenderState, "LINEWIDTH") == 0 ||  // Now an OGL state.
        NiStricmp(pcRenderState, "SAMPLEALPHA") == 0 ||
        NiStricmp(pcRenderState, "DXT1NOISEENABLE") == 0 ||
        NiStricmp(pcRenderState, "YUVENABLE") == 0 ||
        NiStricmp(pcRenderState, "OCCLUSIONCULLENABLE") == 0 ||
        NiStricmp(pcRenderState, "STENCILCULLENABLE") == 0 ||
        NiStricmp(pcRenderState, "ROPZCMPALWAYSREAD") == 0 ||
        NiStricmp(pcRenderState, "ROPZREAD") == 0 ||
        NiStricmp(pcRenderState, "DONOTCULLUNCOMPRESSED") == 0 ||
        NiStricmp(pcRenderState, "PSTEXTUREMODES") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAINPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERINPUTSABCD") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERINPUTSEFG") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_0") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_1") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_2") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_3") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_4") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_5") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_6") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT0_7") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_0") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_1") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_2") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_3") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_4") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_5") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_6") == 0 ||
        NiStricmp(pcRenderState, "PSCONSTANT1_7") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSALPHAOUTPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSRGBINPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSCOMPAREMODE") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERCONSTANT0") == 0 ||
        NiStricmp(pcRenderState, "PSFINALCOMBINERCONSTANT1") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS0") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS1") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS2") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS3") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS4") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS5") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS6") == 0 ||
        NiStricmp(pcRenderState, "PSRGBOUTPUTS7") == 0 ||
        NiStricmp(pcRenderState, "PSCOMBINERCOUNT") == 0 ||
        NiStricmp(pcRenderState, "PSDOTMAPPING") == 0 ||
        NiStricmp(pcRenderState, "PSINPUTTEXTURE") == 0)
    {
        return NID3DRS_DEPRECATED;
    }

    return NID3DRS_INVALID;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::LookupRenderStateValue(
    NiD3D10RenderState eRenderState, const char* pcValue,
    unsigned int& uiValue)
{
    // We will default to a value of 0, as this will most likely not cause any
    // assertions on devices; it will simply cause incorrect results.  (This
    // case assumes that the value string was not valid for the renderstate.)
    uiValue = 0;

    if (!pcValue || pcValue[0] == '\0')
        return false;

    switch (eRenderState)
    {
        // Simple states
    case NID3DRS_ZFUNC:
    case NID3DRS_ALPHAFUNC:
    case NID3DRS_STENCILFUNC:
    case NID3DRS_CCW_STENCILFUNC:
        {
            if (NiStricmp(pcValue, "NEVER") == 0)
            {
                uiValue = NID3DCMP_NEVER;
                return true;
            }
            else if (NiStricmp(pcValue, "LESS") == 0)
            {
                uiValue = NID3DCMP_LESS;
                return true;
            }
            else if (NiStricmp(pcValue, "EQUAL") == 0)
            {
                uiValue = NID3DCMP_EQUAL;
                return true;
            }
            else if (NiStricmp(pcValue, "LESSEQUAL") == 0)
            {
                uiValue = NID3DCMP_LESSEQUAL;
                return true;
            }
            else if (NiStricmp(pcValue, "GREATER") == 0)
            {
                uiValue = NID3DCMP_GREATER;
                return true;
            }
            else if (NiStricmp(pcValue, "NOTEQUAL") == 0)
            {
                uiValue = NID3DCMP_NOTEQUAL;
                return true;
            }
            else if (NiStricmp(pcValue, "GREATEREQUAL") == 0)
            {
                uiValue = NID3DCMP_GREATEREQUAL;
                return true;
            }
            else if (NiStricmp(pcValue, "ALWAYS") == 0)
            {
                uiValue = NID3DCMP_ALWAYS;
                return true;
            }
        }
        break;
    case NID3DRS_SRCBLEND:
    case NID3DRS_DESTBLEND:
    case NID3DRS_SRCBLENDALPHA:
    case NID3DRS_DESTBLENDALPHA:
    case NIOGLRS_SRCBLENDSEPARATERGB:     
    case NIOGLRS_DSTBLENDSEPARATERGB:     
        {
            if (NiStricmp(pcValue, "ZERO") == 0)
            {
                uiValue = NID3DBLEND_ZERO;
                return true;
            }
            else if (NiStricmp(pcValue, "ONE") == 0)
            {
                uiValue = NID3DBLEND_ONE;
                return true;
            }
            else if (NiStricmp(pcValue, "SRCCOLOR") == 0)
            {
                uiValue = NID3DBLEND_SRCCOLOR;
                return true;
            }
            else if (NiStricmp(pcValue, "INVSRCCOLOR") == 0)
            {
                uiValue = NID3DBLEND_INVSRCCOLOR;
                return true;
            }
            else if (NiStricmp(pcValue, "SRCALPHA") == 0)
            {
                uiValue = NID3DBLEND_SRCALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "INVSRCALPHA") == 0)
            {
                uiValue = NID3DBLEND_INVSRCALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "DESTALPHA") == 0)
            {
                uiValue = NID3DBLEND_DESTALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "INVDESTALPHA") == 0)
            {
                uiValue = NID3DBLEND_INVDESTALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "DESTCOLOR") == 0)
            {
                uiValue = NID3DBLEND_DESTCOLOR;
                return true;
            }
            else if (NiStricmp(pcValue, "INVDESTCOLOR") == 0)
            {
                uiValue = NID3DBLEND_INVDESTCOLOR;
                return true;
            }
            else if (NiStricmp(pcValue, "SRCALPHASAT") == 0)
            {
                uiValue = NID3DBLEND_SRCALPHASAT;
                return true;
            }
            else if (NiStricmp(pcValue, "BOTHSRCALPHA") == 0)
            {
                uiValue = NID3DBLEND_BOTHSRCALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "BOTHINVSRCALPHA") == 0)
            {
                uiValue = NID3DBLEND_BOTHINVSRCALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "CONSTANTCOLOR") == 0 ||
                NiStricmp(pcValue, "BLENDFACTOR") == 0)
            {
                uiValue = NID3DBLEND_BLENDFACTOR;
                return true;
            }
            else if (NiStricmp(pcValue, "INVCONSTANTCOLOR") == 0 ||
                NiStricmp(pcValue, "INVBLENDFACTOR") == 0)
            {
                uiValue = NID3DBLEND_INVBLENDFACTOR;
                return true;
            }
            else if (NiStricmp(pcValue, "CONSTANTALPHA") == 0)
            {
                uiValue = NID3DBLEND_CONSTANTALPHA;
                return true;
            }
            else if (NiStricmp(pcValue, "INVCONSTANTALPHA") == 0)
            {
                uiValue = NID3DBLEND_INVCONSTANTALPHA;
                return true;
            }
        }
        break;
    case NID3DRS_STENCILZFAIL:
    case NID3DRS_STENCILPASS:
    case NID3DRS_STENCILFAIL:
    case NID3DRS_CCW_STENCILFAIL:
    case NID3DRS_CCW_STENCILZFAIL:
    case NID3DRS_CCW_STENCILPASS:
        {
            if (NiStricmp(pcValue, "KEEP") == 0)
            {
                uiValue = NID3DSTENCILOP_KEEP;
                return true;
            }
            else if (NiStricmp(pcValue, "ZERO") == 0)
            {
                uiValue = NID3DSTENCILOP_ZERO;
                return true;
            }
            else if (NiStricmp(pcValue, "REPLACE") == 0)
            {
                uiValue = NID3DSTENCILOP_REPLACE;
                return true;
            }
            else if (NiStricmp(pcValue, "INCRSAT") == 0)
            {
                uiValue = NID3DSTENCILOP_INCRSAT;
                return true;
            }
            else if (NiStricmp(pcValue, "DECRSAT") == 0)
            {
                uiValue = NID3DSTENCILOP_DECRSAT;
                return true;
            }
            else if (NiStricmp(pcValue, "INVERT") == 0)
            {
                uiValue = NID3DSTENCILOP_INVERT;
                return true;
            }
            else if (NiStricmp(pcValue, "INCR") == 0)
            {
                uiValue = NID3DSTENCILOP_INCR;
                return true;
            }
            else if (NiStricmp(pcValue, "DECR") == 0)
            {
                uiValue = NID3DSTENCILOP_DECR;
                return true;
            }
        }
        break;
    case NID3DRS_BLENDOP:
    case NID3DRS_BLENDOPALPHA:
    case NIOGLRS_BLENDEQUATIONSEPARATERGB:
        {
            if (NiStricmp(pcValue, "ADD") == 0)
            {
                uiValue = NID3DBLENDOP_ADD;
                return true;
            }
            else if (NiStricmp(pcValue, "SUBTRACT") == 0)
            {
                uiValue = NID3DBLENDOP_SUBTRACT;
                return true;
            }
            else if (NiStricmp(pcValue, "REVSUBTRACT") == 0)
            {
                uiValue = NID3DBLENDOP_REVSUBTRACT;
                return true;
            }
            else if (NiStricmp(pcValue, "MIN") == 0)
            {
                uiValue = NID3DBLENDOP_MIN;
                return true;
            }
            else if (NiStricmp(pcValue, "MAX") == 0)
            {
                uiValue = NID3DBLENDOP_MAX;
                return true;
            }
            // Deprecated values
            else if (NiStricmp(pcValue, "ADDSIGNED") == 0 ||
                NiStricmp(pcValue, "REVSUBTRACTSIGNED") == 0)
            {
                uiValue = NID3DBLENDOP_DEPRECATED;
                return true;
            }
        }
        break;
    case NID3DRS_FILLMODE:
        {
            if (NiStricmp(pcValue, "POINT") == 0)
            {
                uiValue = NID3DFILL_POINT;
                return true;
            }
            else if (NiStricmp(pcValue, "WIREFRAME") == 0)
            {
                uiValue = NID3DFILL_WIREFRAME;
                return true;
            }
            else if (NiStricmp(pcValue, "SOLID") == 0)
            {
                uiValue = NID3DFILL_SOLID;
                return true;
            }
        }
        break;
    case NID3DRS_CULLMODE:
    case NIOGLRS_FRONTFACE:
        {
            if (NiStricmp(pcValue, "NONE") == 0)
            {
                uiValue = NID3DCULL_NONE;
                return true;
            }
            else if (NiStricmp(pcValue, "CW") == 0)
            {
                uiValue = NID3DCULL_CW;
                return true;
            }
            else if (NiStricmp(pcValue, "CCW") == 0)
            {
                uiValue = NID3DCULL_CCW;
                return true;
            }
        }
        break;
    default:
        {
#ifdef NIDEBUG
            NILOG(NIMESSAGE_GENERAL_0, "LookupRenderStateValue(%s): "
                "Render state not found.\n",
                LookupRenderStateString(eRenderState));
#endif
            break;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
    NSBD3D10RenderStates::NiD3D10RenderState eNSBD3D10State,
    unsigned int uiNSBD3D10Value, unsigned int& uiD3D10Value)
{
    bool bResult = true;

    switch (eNSBD3D10State)
    {
        // Simple states
    case NID3DRS_ZFUNC:
    case NID3DRS_STENCILFUNC:
    case NID3DRS_CCW_STENCILFUNC:
        bResult
            = GetD3D10CmpFunc((NiD3D10CmpFunc)uiNSBD3D10Value, uiD3D10Value);
        break;
    case NID3DRS_SRCBLEND:
    case NID3DRS_DESTBLEND:
    case NID3DRS_SRCBLENDALPHA:
    case NID3DRS_DESTBLENDALPHA:
        bResult = GetD3D10Blend((NiD3D10Blend)uiNSBD3D10Value, uiD3D10Value);
        break;
    case NID3DRS_STENCILZFAIL:
    case NID3DRS_STENCILPASS:
    case NID3DRS_STENCILFAIL:
    case NID3DRS_CCW_STENCILFAIL:
    case NID3DRS_CCW_STENCILZFAIL:
    case NID3DRS_CCW_STENCILPASS:
        bResult = GetD3D10StencilOp((NiD3D10StencilOp)uiNSBD3D10Value,
            uiD3D10Value);
        break;
    case NID3DRS_BLENDOP:
    case NID3DRS_BLENDOPALPHA:
        bResult
            = GetD3D10BlendOp((NiD3D10BlendOp)uiNSBD3D10Value, uiD3D10Value);
        break;
    case NID3DRS_FILLMODE:
        bResult = GetD3D10FillMode((NiD3D10FillMode)uiNSBD3D10Value,
            uiD3D10Value);
        break;
    case NID3DRS_CULLMODE:
        bResult = GetD3D10Cull((NiD3D10Cull)uiNSBD3D10Value, uiD3D10Value);
        break;
    default:
        // Assume we are passing the value straight through!
        uiD3D10Value = uiNSBD3D10Value;
        bResult = true;
        break;
    }

    return bResult;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::GetD3D10FillMode(NiD3D10FillMode eFillMode, 
    unsigned int& uiD3D10Value)
{
    switch (eFillMode)
    {
    // FILL_POINT does not exist in D3D10.
    case NID3DFILL_WIREFRAME:
        uiD3D10Value = (unsigned int)D3D10_FILL_WIREFRAME;
        return true;
    case NID3DFILL_SOLID:
        uiD3D10Value = (unsigned int)D3D10_FILL_SOLID;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::GetD3D10Blend(NiD3D10Blend eBlend, 
    unsigned int& uiD3D10Value)
{
    switch (eBlend)
    {
    case NID3DBLEND_ZERO:
        uiD3D10Value = (unsigned int)D3D10_BLEND_ZERO;
        return true;
    case NID3DBLEND_ONE:
        uiD3D10Value = (unsigned int)D3D10_BLEND_ONE;
        return true;
    case NID3DBLEND_SRCCOLOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_SRC_COLOR;
        return true;
    case NID3DBLEND_INVSRCCOLOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_SRC_COLOR;
        return true;
    case NID3DBLEND_SRCALPHA:
        uiD3D10Value = (unsigned int)D3D10_BLEND_SRC_ALPHA;
        return true;
    case NID3DBLEND_INVSRCALPHA:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_SRC_ALPHA;
        return true;
    case NID3DBLEND_DESTALPHA:
        uiD3D10Value = (unsigned int)D3D10_BLEND_DEST_ALPHA;
        return true;
    case NID3DBLEND_INVDESTALPHA:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_DEST_ALPHA;
        return true;
    case NID3DBLEND_DESTCOLOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_DEST_COLOR;
        return true;
    case NID3DBLEND_INVDESTCOLOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_DEST_COLOR;
        return true;
    case NID3DBLEND_SRCALPHASAT:
        uiD3D10Value = (unsigned int)D3D10_BLEND_SRC_ALPHA_SAT;
        return true;
    case NID3DBLEND_BLENDFACTOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_BLEND_FACTOR;
        return true;
    case NID3DBLEND_INVBLENDFACTOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_BLEND_FACTOR;
        return true;
    case NID3DBLEND_SRC1_COLOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_SRC1_COLOR;
        return true;
    case NID3DBLEND_INV_SRC1_COLOR:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_SRC1_COLOR;
        return true;
    case NID3DBLEND_SRC1_ALPHA:
        uiD3D10Value = (unsigned int)D3D10_BLEND_SRC1_ALPHA;
        return true;
    case NID3DBLEND_INV_SRC1_ALPHA:
        uiD3D10Value = (unsigned int)D3D10_BLEND_INV_SRC1_ALPHA;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::GetD3D10Cull(NiD3D10Cull eCull,
    unsigned int& uiD3D10Value)
{
    switch (eCull)
    {
    case NID3DCULL_NONE:
        uiD3D10Value = (unsigned int)D3D10_CULL_NONE;
        return true;
    case NID3DCULL_CW:
        uiD3D10Value = (unsigned int)D3D10_CULL_FRONT;
        return true;
    case NID3DCULL_CCW:
        uiD3D10Value = (unsigned int)D3D10_CULL_BACK;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::GetD3D10CmpFunc(NiD3D10CmpFunc eCmpFunc, 
    unsigned int& uiD3D10Value)
{
    switch (eCmpFunc)
    {
    case NID3DCMP_NEVER:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_NEVER;
        return true;
    case NID3DCMP_LESS:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_LESS;
        return true;
    case NID3DCMP_EQUAL:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_EQUAL;
        return true;
    case NID3DCMP_LESSEQUAL:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_LESS_EQUAL;
        return true;
    case NID3DCMP_GREATER:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_GREATER;
        return true;
    case NID3DCMP_NOTEQUAL:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_NOT_EQUAL;
        return true;
    case NID3DCMP_GREATEREQUAL:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_GREATER_EQUAL;
        return true;
    case NID3DCMP_ALWAYS:
        uiD3D10Value = (unsigned int)D3D10_COMPARISON_ALWAYS;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::GetD3D10StencilOp(NiD3D10StencilOp eStencilOp, 
    unsigned int& uiD3D10Value)
{
    switch (eStencilOp)
    {
    case NID3DSTENCILOP_KEEP:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_KEEP;
        return true;
    case NID3DSTENCILOP_ZERO:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_ZERO;
        return true;
    case NID3DSTENCILOP_REPLACE:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_REPLACE;
        return true;
    case NID3DSTENCILOP_INCRSAT:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_INCR_SAT;
        return true;
    case NID3DSTENCILOP_DECRSAT:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_DECR_SAT;
        return true;
    case NID3DSTENCILOP_INVERT:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_INVERT;
        return true;
    case NID3DSTENCILOP_INCR:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_INCR;
        return true;
    case NID3DSTENCILOP_DECR:
        uiD3D10Value = (unsigned int)D3D10_STENCIL_OP_DECR;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NSBD3D10RenderStates::GetD3D10BlendOp(NiD3D10BlendOp eBlendOp, 
    unsigned int& uiD3D10Value)
{
    switch (eBlendOp)
    {
    case NID3DBLENDOP_ADD:
        uiD3D10Value = (unsigned int)D3D10_BLEND_OP_ADD;
        return true;
    case NID3DBLENDOP_SUBTRACT:
        uiD3D10Value = (unsigned int)D3D10_BLEND_OP_SUBTRACT;
        return true;
    case NID3DBLENDOP_REVSUBTRACT:
        uiD3D10Value = (unsigned int)D3D10_BLEND_OP_REV_SUBTRACT;
        return true;
    case NID3DBLENDOP_MIN:
        uiD3D10Value = (unsigned int)D3D10_BLEND_OP_MIN;
        return true;
    case NID3DBLENDOP_MAX:
        uiD3D10Value = (unsigned int)D3D10_BLEND_OP_MAX;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
#if defined(NIDEBUG)
//---------------------------------------------------------------------------
const char* NSBD3D10RenderStates::LookupRenderStateString(
    NiD3D10RenderState eRS)
{
    switch (eRS)
    {
    STATE_CASE_STRING(NID3DRS_ZENABLE)
    STATE_CASE_STRING(NID3DRS_FILLMODE)
    STATE_CASE_STRING(NID3DRS_SHADEMODE)
    STATE_CASE_STRING(NID3DRS_ZWRITEENABLE)
    STATE_CASE_STRING(NID3DRS_ALPHATESTENABLE)
    STATE_CASE_STRING(NID3DRS_LASTPIXEL)
    STATE_CASE_STRING(NID3DRS_SRCBLEND)
    STATE_CASE_STRING(NID3DRS_DESTBLEND)
    STATE_CASE_STRING(NID3DRS_CULLMODE)
    STATE_CASE_STRING(NID3DRS_ZFUNC)
    STATE_CASE_STRING(NID3DRS_ALPHAREF)
    STATE_CASE_STRING(NID3DRS_ALPHAFUNC)
    STATE_CASE_STRING(NID3DRS_DITHERENABLE)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE)
    STATE_CASE_STRING(NID3DRS_FOGENABLE)
    STATE_CASE_STRING(NID3DRS_SPECULARENABLE)
    STATE_CASE_STRING(NID3DRS_FOGCOLOR)
    STATE_CASE_STRING(NID3DRS_FOGTABLEMODE)
    STATE_CASE_STRING(NID3DRS_FOGSTART)
    STATE_CASE_STRING(NID3DRS_FOGEND)
    STATE_CASE_STRING(NID3DRS_FOGDENSITY)
    STATE_CASE_STRING(NID3DRS_RANGEFOGENABLE)
    STATE_CASE_STRING(NID3DRS_STENCILENABLE)
    STATE_CASE_STRING(NID3DRS_STENCILFAIL)
    STATE_CASE_STRING(NID3DRS_STENCILZFAIL)
    STATE_CASE_STRING(NID3DRS_STENCILPASS)
    STATE_CASE_STRING(NID3DRS_STENCILFUNC)
    STATE_CASE_STRING(NID3DRS_STENCILREF)
    STATE_CASE_STRING(NID3DRS_STENCILMASK)
    STATE_CASE_STRING(NID3DRS_STENCILWRITEMASK)
    STATE_CASE_STRING(NID3DRS_TEXTUREFACTOR)
    STATE_CASE_STRING(NID3DRS_WRAP0)
    STATE_CASE_STRING(NID3DRS_WRAP1)
    STATE_CASE_STRING(NID3DRS_WRAP2)
    STATE_CASE_STRING(NID3DRS_WRAP3)
    STATE_CASE_STRING(NID3DRS_WRAP4)
    STATE_CASE_STRING(NID3DRS_WRAP5)
    STATE_CASE_STRING(NID3DRS_WRAP6)
    STATE_CASE_STRING(NID3DRS_WRAP7)
    STATE_CASE_STRING(NID3DRS_CLIPPING)
    STATE_CASE_STRING(NID3DRS_LIGHTING)
    STATE_CASE_STRING(NID3DRS_AMBIENT)
    STATE_CASE_STRING(NID3DRS_FOGVERTEXMODE)
    STATE_CASE_STRING(NID3DRS_COLORVERTEX)
    STATE_CASE_STRING(NID3DRS_LOCALVIEWER)
    STATE_CASE_STRING(NID3DRS_NORMALIZENORMALS)
    STATE_CASE_STRING(NID3DRS_DIFFUSEMATERIALSOURCE )
    STATE_CASE_STRING(NID3DRS_SPECULARMATERIALSOURCE)
    STATE_CASE_STRING(NID3DRS_AMBIENTMATERIALSOURCE)
    STATE_CASE_STRING(NID3DRS_EMISSIVEMATERIALSOURCE)
    STATE_CASE_STRING(NID3DRS_VERTEXBLEND)
    STATE_CASE_STRING(NID3DRS_CLIPPLANEENABLE)
    STATE_CASE_STRING(NID3DRS_POINTSIZE)
    STATE_CASE_STRING(NID3DRS_POINTSIZE_MIN)
    STATE_CASE_STRING(NID3DRS_POINTSPRITEENABLE)
    STATE_CASE_STRING(NID3DRS_POINTSCALEENABLE)
    STATE_CASE_STRING(NID3DRS_POINTSCALE_A)
    STATE_CASE_STRING(NID3DRS_POINTSCALE_B)
    STATE_CASE_STRING(NID3DRS_POINTSCALE_C)
    STATE_CASE_STRING(NID3DRS_MULTISAMPLEANTIALIAS)
    STATE_CASE_STRING(NID3DRS_MULTISAMPLEMASK)
    STATE_CASE_STRING(NID3DRS_PATCHEDGESTYLE)
    STATE_CASE_STRING(NID3DRS_DEBUGMONITORTOKEN)
    STATE_CASE_STRING(NID3DRS_POINTSIZE_MAX)
    STATE_CASE_STRING(NID3DRS_INDEXEDVERTEXBLENDENABLE)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE)
    STATE_CASE_STRING(NID3DRS_TWEENFACTOR)
    STATE_CASE_STRING(NID3DRS_BLENDOP)
    STATE_CASE_STRING(NID3DRS_POSITIONDEGREE)
    STATE_CASE_STRING(NID3DRS_NORMALDEGREE)
    STATE_CASE_STRING(NID3DRS_SCISSORTESTENABLE)
    STATE_CASE_STRING(NID3DRS_SLOPESCALEDEPTHBIAS)
    STATE_CASE_STRING(NID3DRS_ANTIALIASEDLINEENABLE)
    STATE_CASE_STRING(NID3DRS_MINTESSELLATIONLEVEL)
    STATE_CASE_STRING(NID3DRS_MAXTESSELLATIONLEVEL)
    STATE_CASE_STRING(NID3DRS_ADAPTIVETESS_X)
    STATE_CASE_STRING(NID3DRS_ADAPTIVETESS_Y)
    STATE_CASE_STRING(NID3DRS_ADAPTIVETESS_Z)
    STATE_CASE_STRING(NID3DRS_ADAPTIVETESS_W)
    STATE_CASE_STRING(NID3DRS_ENABLEADAPTIVETESSELLATION)
    STATE_CASE_STRING(NID3DRS_TWOSIDEDSTENCILMODE)
    STATE_CASE_STRING(NID3DRS_CCW_STENCILFAIL)
    STATE_CASE_STRING(NID3DRS_CCW_STENCILZFAIL)
    STATE_CASE_STRING(NID3DRS_CCW_STENCILPASS)
    STATE_CASE_STRING(NID3DRS_CCW_STENCILFUNC)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE1)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE2)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE3)
    STATE_CASE_STRING(NID3DRS_BLENDFACTOR)
    STATE_CASE_STRING(NID3DRS_SRGBWRITEENABLE)
    STATE_CASE_STRING(NID3DRS_DEPTHBIAS)
    STATE_CASE_STRING(NID3DRS_WRAP8)
    STATE_CASE_STRING(NID3DRS_WRAP9)
    STATE_CASE_STRING(NID3DRS_WRAP10)
    STATE_CASE_STRING(NID3DRS_WRAP11)
    STATE_CASE_STRING(NID3DRS_WRAP12)
    STATE_CASE_STRING(NID3DRS_WRAP13)
    STATE_CASE_STRING(NID3DRS_WRAP14)
    STATE_CASE_STRING(NID3DRS_WRAP15)
    STATE_CASE_STRING(NID3DRS_SEPARATEALPHABLENDENABLE)
    STATE_CASE_STRING(NID3DRS_SRCBLENDALPHA)
    STATE_CASE_STRING(NID3DRS_DESTBLENDALPHA)
    STATE_CASE_STRING(NID3DRS_BLENDOPALPHA)
    STATE_CASE_STRING(NID3DRS_VIEWPORTENABLE)
    STATE_CASE_STRING(NID3DRS_HIGHPRECISIONBLENDENABLE)
    STATE_CASE_STRING(NID3DRS_HIGHPRECISIONBLENDENABLE1)
    STATE_CASE_STRING(NID3DRS_HIGHPRECISIONBLENDENABLE2)
    STATE_CASE_STRING(NID3DRS_HIGHPRECISIONBLENDENABLE3)
    STATE_CASE_STRING(NID3DRS_TESSELLATIONMODE)
    STATE_CASE_STRING(NIOGLRS_COLORLOGICOPENABLE)          
    STATE_CASE_STRING(NIOGLRS_CULLFACEENABLE)              
    STATE_CASE_STRING(NIOGLRS_MULTISAMPLEENABLE)
    STATE_CASE_STRING(NIOGLRS_POINTSMOOTHENABLE)           
    STATE_CASE_STRING(NIOGLRS_POLYGONOFFSETFILLENABLE) 
    STATE_CASE_STRING(NIOGLRS_SAMPLEALPHATOCOVERAGEENABLE)
    STATE_CASE_STRING(NIOGLRS_SAMPLEALPHATOONEENABLE)      
    STATE_CASE_STRING(NIOGLRS_SAMPLECOVERAGEENABLE)        
    STATE_CASE_STRING(NIOGLRS_VERTEXPROGRAMPOINTSIZEENABLE)
    STATE_CASE_STRING(NIOGLRS_SRCBLENDSEPARATERGB)     
    STATE_CASE_STRING(NIOGLRS_DSTBLENDSEPARATERGB)     
    STATE_CASE_STRING(NIOGLRS_BLENDCOLORR)
    STATE_CASE_STRING(NIOGLRS_BLENDCOLORG)
    STATE_CASE_STRING(NIOGLRS_BLENDCOLORB)
    STATE_CASE_STRING(NIOGLRS_BLENDCOLORA)
    STATE_CASE_STRING(NIOGLRS_BLENDEQUATIONSEPARATERGB)
    STATE_CASE_STRING(NIOGLRS_CULLFACE)                
    STATE_CASE_STRING(NIOGLRS_COLORMASKR)
    STATE_CASE_STRING(NIOGLRS_COLORMASKG)
    STATE_CASE_STRING(NIOGLRS_COLORMASKB)
    STATE_CASE_STRING(NIOGLRS_COLORMASKA)
    STATE_CASE_STRING(NIOGLRS_DEPTHRANGENEAR)
    STATE_CASE_STRING(NIOGLRS_DEPTHRANGEFAR)
    STATE_CASE_STRING(NIOGLRS_FRONTFACE)               
    STATE_CASE_STRING(NIOGLRS_LINEWIDTH)               
    STATE_CASE_STRING(NIOGLRS_POINTSPRITECOORDREPLACE) 
    STATE_CASE_STRING(NIOGLRS_POLYGONMODEFACE)             
    STATE_CASE_STRING(NIOGLRS_POLYGONOFFSETFACTOR)           
    STATE_CASE_STRING(NIOGLRS_POLYGONOFFSETUNITS)           
    STATE_CASE_STRING(NIOGLRS_SCISSORX)                 
    STATE_CASE_STRING(NIOGLRS_SCISSORY)                 
    STATE_CASE_STRING(NIOGLRS_SCISSORWIDTH)                 
    STATE_CASE_STRING(NIOGLRS_SCISSORHEIGHT)

    // Begin D3D10-only states.
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE1)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE2)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE3)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE4)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE5)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE6)
    STATE_CASE_STRING(NID3DRS_ALPHABLENDENABLE7)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE4)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE5)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE6)
    STATE_CASE_STRING(NID3DRS_COLORWRITEENABLE7)
    STATE_CASE_STRING(NID3DRS_FRONTCCW)
    STATE_CASE_STRING(NID3DRS_DEPTHBIASCLAMP)
    // End D3D10-only states.
    
    default:
        return "***** UNKNOWN RENDER STATE ****";
    }
}
//---------------------------------------------------------------------------
#endif  //#if defined(NIDEBUG)
//---------------------------------------------------------------------------
