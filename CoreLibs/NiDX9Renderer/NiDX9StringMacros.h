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
#define DEBUG_STRING_MACROS

#if defined(NIDEBUG) && defined(DEBUG_STRING_MACROS)

#define DX9_TEXTURE_OP_STRING(pcStr, eOp)                           \
    {                                                               \
        switch (eOp)                                                \
        {                                                           \
        case D3DTeOp_DISABLE:                                       \
            strcat(pcStr, " DISABLE\n");                     break; \
        case D3DTeOp_SELECTARG1:                                    \
            strcat(pcStr, " SELECTARG1\n");                  break; \
        case D3DTeOp_SELECTARG2:                                    \
            strcat(pcStr, " SELECTARG2\n");                  break; \
        case D3DTeOp_MODULATE:                                      \
            strcat(pcStr, " MODULATE\n");                    break; \
        case D3DTeOp_MODULATE2X:                                    \
            strcat(pcStr, " MODULATE2X\n");                  break; \
        case D3DTeOp_MODULATE4X:                                    \
            strcat(pcStr, " MODULATE4X\n");                  break; \
        case D3DTeOp_ADD:                                           \
            strcat(pcStr, " ADD\n");                         break; \
        case D3DTeOp_ADDSIGNED:                                     \
            strcat(pcStr, " ADDSIGNED\n");                   break; \
        case D3DTeOp_ADDSIGNED2X:                                   \
            strcat(pcStr, " ADDSIGNED2X\n");                 break; \
        case D3DTeOp_SUBTRACT:                                      \
            strcat(pcStr, " SUBTRACT\n");                    break; \
        case D3DTeOp_ADDSMOOTH:                                     \
            strcat(pcStr, " ADDSMOOTH\n");                   break; \
        case D3DTeOp_BLENDDIFFUSEALPHA:                             \
            strcat(pcStr, " BLENDDIFFUSEALPHA\n");           break; \
        case D3DTeOp_BLENDTEXTUREALPHA:                             \
            strcat(pcStr, " BLENDTEXTUREALPHA\n");           break; \
        case D3DTeOp_BLENDFACTORALPHA:                              \
            strcat(pcStr, " BLENDFACTORALPHA\n");            break; \
        case D3DTeOp_BLENDTEXTUREALPHAPM:                           \
            strcat(pcStr, " BLENDTEXTUREALPHAPM\n");         break; \
        case D3DTeOp_BLENDCURRENTALPHA:                             \
            (pcStr, " BLENDCURRENTALPHA\n");                 break; \
        case D3DTeOp_PREMODULATE:                                   \
            strcat(pcStr, " PREMODULATE\n");                 break; \
        case D3DTeOp_MODULATEALPHA_ADDCOLOR:                        \
            strcat(pcStr, " MODULATEALPHA_ADDCOLOR\n");      break; \
        case D3DTeOp_MODULATECOLOR_ADDALPHA:                        \
            strcat(pcStr, " MODULATECOLOR_ADDALPHA\n");      break; \
        case D3DTeOp_MODULATEINVALPHA_ADDCOLOR:                     \
            strcat(pcStr, " MODULATEINVALPHA_ADDCOLOR\n");   break; \
        case D3DTeOp_MODULATEINVCOLOR_ADDALPHA:                     \
            strcat(pcStr, " MODULATEINVCOLOR_ADDALPHA\n");   break; \
        case D3DTeOp_BUMPENVMAP:                                    \
            strcat(pcStr, " BUMPENVMAP\n");                  break; \
        case D3DTeOp_BUMPENVMAPLUMINANCE:                           \
            strcat(pcStr, " BUMPENVMAPLUMINANCE\n");         break; \
        case D3DTeOp_DOTPRODUCT3:                                   \
            strcat(pcStr, " DOTPRODUCT3\n");                 break; \
        case D3DTeOp_MULTIPLYADD:                                   \
            strcat(pcStr, " MULTIPLYADD\n");                 break; \
        case D3DTeOp_LERP:                                          \
            strcat(pcStr, " LERP\n");                        break; \
        }                                                           \
    }

#define DX9_TEXTURE_ARG_STRING(pcStr, eArg)                         \
    {                                                               \
        if (eArg & D3DTA_TEXTURE)                                   \
            strcat(pcStr, "TEXTURE");                               \
        else                                                        \
        if (eArg & D3DTA_CURRENT)                                   \
            strcat(pcStr, "CURRENT");                               \
        else                                                        \
        if (!(eArg & D3DTA_SELECTMASK))                             \
            strcat(pcStr, "DIFFUSE");                               \
        else                                                        \
        if (eArg & D3DTA_SPECULAR)                                  \
            strcat(pcStr, "SPECULAR");                              \
        else                                                        \
        if (eArg & D3DTA_TEMP)                                      \
            strcat(pcStr, "TEMP");                                  \
        else                                                        \
        if (eArg & D3DTA_TFACTOR)                                   \
            strcat(pcStr, "TFACTOR");                               \
                                                                    \
        if (eArg & D3DTA_ALPHAREPLICATE)                            \
        {                                                           \
            if (eArg & D3DTA_COMPLEMENT)                            \
                strcat(pcStr, " ALPHAREPLICATE");                   \
            else                                                    \
                strcat(pcStr, " ALPHAREPLICATE\n");                 \
        }                                                           \
        if (eArg & D3DTA_COMPLEMENT)                                \
            strcat(pcStr, " COMPLEMENT\n");                         \
        else                                                        \
            strcat(pcStr, "\n");                                    \
    }

#define DX9_TEXTURE_WRAP_STRING(pcStr, eArg)                        \
    {                                                               \
        switch (eArg)                                               \
        {                                                           \
        case D3DTADDRESS_WRAP:                                      \
            strcat(pcStr, " WRAP\n");                               \
            break;                                                  \
        case D3DTADDRESS_MIRROR:                                    \
            strcat(pcStr, " MIRROR\n");                             \
            break;                                                  \
        case D3DTADDRESS_CLAMP:                                     \
            strcat(pcStr, " CLAMP\n");                              \
            break;                                                  \
        case D3DTADDRESS_BORDER:                                    \
            strcat(pcStr, " BORDER\n");                             \
            break;                                                  \
        case D3DTADDRESS_MIRRORONCE:                                \
            strcat(pcStr, " MIRRORONCE\n");                         \
            break;                                                  \
        default:                                                    \
            strcat(pcStr, " UNKNOWN\n");                            \
            break;                                                  \
        }                                                           \
    }

#define DX9_TEXTURE_FILTER_STRING(pcStr, eArg)                      \
    {                                                               \
        switch (eArg)                                               \
        {                                                           \
        case D3DTEXF_NONE:                                          \
            strcat(pcStr, " NONE\n");                               \
            break;                                                  \
        case D3DTEXF_POINT:                                         \
            strcat(pcStr, " POINT\n");                              \
            break;                                                  \
        case D3DTEXF_LINEAR:                                        \
            strcat(pcStr, " LINEAR\n");                             \
            break;                                                  \
        case D3DTEXF_ANISOTReOpIC:                                  \
            strcat(pcStr, " ANISOTROPIC\n");                        \
            break;                                                  \
        case D3DTEXF_FLATCUBIC:                                     \
            strcat(pcStr, " FLATCUBIC\n");                          \
            break;                                                  \
        case D3DTEXF_GAUSSIANCUBIC:                                 \
            strcat(pcStr, " GAUSSIANCUBIC\n");                      \
            break;                                                  \
        default:                                                    \
            strcat(pcStr, " UNKNOWN\n");                            \
            break;                                                  \
        }                                                           \
    }

    
#else   //#if defined(NIDEBUG && DEBUG_STRING_MACROS)

#define DX9_TEXTURE_OP_STRING(pcStr, eOp)                {}
#define DX9_TEXTURE_ARG_STRING(pcStr, eArg)              {}
#define DX9_TEXTURE_WRAP_STRING(pcStr, eArg)              {}
#define DX9_TEXTURE_FILTER_STRING(pcStr, eArg)            {}

#endif  //#if defined(NIDEBUG && DEBUG_STRING_MACROS)
