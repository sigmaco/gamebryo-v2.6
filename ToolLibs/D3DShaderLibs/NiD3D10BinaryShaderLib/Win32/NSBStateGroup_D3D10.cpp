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

//---------------------------------------------------------------------------
#include "NSBD3D10StateGroup.h"
#include "NSBD3D10RenderStates.h"
#include "NSBD3D10StageAndSamplerStates.h"

#include <NiD3D10RenderStateGroup.h>

//---------------------------------------------------------------------------
bool NSBD3D10StateGroup::SetupRenderStateGroup_D3D10(
    NiD3D10RenderStateGroup& kRSGroup)
{
    if (m_kStateList.GetSize() == 0)
        return false;

    NSBD3D10SGEntry* pkEntry = GetFirstState();

    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            unsigned int uiD3D10Value = 0x7fffffff;

            NSBD3D10RenderStates::NiD3D10RenderState eState =
                (NSBD3D10RenderStates::NiD3D10RenderState)pkEntry->GetState();

            if (eState == 0x7fffffff)  // Sanity check.
                continue;

            switch(eState)
            {
            case NSBD3D10RenderStates::NID3DRS_ZENABLE:
                kRSGroup.SetDSSDepthEnable((pkEntry->GetValue()) ? (true) :
                    (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_FILLMODE:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetRSFillMode((D3D10_FILL_MODE)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_ZWRITEENABLE:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSDepthWriteMask(
                        (D3D10_DEPTH_WRITE_MASK)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_SRCBLEND:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetBSSrcBlend((D3D10_BLEND)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_DESTBLEND:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetBSDestBlend((D3D10_BLEND)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_CULLMODE:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetRSCullMode((D3D10_CULL_MODE)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_ZFUNC:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSDepthFunc(
                        (D3D10_COMPARISON_FUNC)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE:
                kRSGroup.SetBSBlendEnable(0,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE1:
                kRSGroup.SetBSBlendEnable(1,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE2:
                kRSGroup.SetBSBlendEnable(2,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE3:
                kRSGroup.SetBSBlendEnable(3,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE4:
                kRSGroup.SetBSBlendEnable(4,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE5:
                kRSGroup.SetBSBlendEnable(5,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE6:
                kRSGroup.SetBSBlendEnable(6,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_ALPHABLENDENABLE7:
                kRSGroup.SetBSBlendEnable(7,
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILENABLE:
                kRSGroup.SetDSSStencilEnable(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILFAIL:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSFrontFaceStencilFailOp(
                        (D3D10_STENCIL_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILZFAIL:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSFrontFaceStencilDepthFailOp(
                        (D3D10_STENCIL_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILPASS:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSFrontFaceStencilPassOp(
                        (D3D10_STENCIL_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILFUNC:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSFrontFaceStencilFunc(
                        (D3D10_COMPARISON_FUNC)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_CCW_STENCILFAIL:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSBackFaceStencilFailOp(
                        (D3D10_STENCIL_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_CCW_STENCILZFAIL:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSBackFaceStencilDepthFailOp(
                        (D3D10_STENCIL_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_CCW_STENCILPASS:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSBackFaceStencilPassOp(
                        (D3D10_STENCIL_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_CCW_STENCILFUNC:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetDSSBackFaceStencilFunc(
                        (D3D10_COMPARISON_FUNC)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILREF:
                kRSGroup.SetStencilRef(pkEntry->GetValue());
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILMASK:
                kRSGroup.SetDSSStencilReadMask(pkEntry->GetValue());
                break;
            case NSBD3D10RenderStates::NID3DRS_STENCILWRITEMASK:
                kRSGroup.SetDSSStencilWriteMask(pkEntry->GetValue());
                break;
            case NSBD3D10RenderStates::NID3DRS_CLIPPLANEENABLE:
                kRSGroup.SetRSDepthClipEnable(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_MULTISAMPLEANTIALIAS:
                kRSGroup.SetRSMultisampleEnable(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_MULTISAMPLEMASK:
                kRSGroup.SetSampleMask(pkEntry->GetValue());
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE:
                kRSGroup.SetBSRenderTargetWriteMask(0,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE1:
                kRSGroup.SetBSRenderTargetWriteMask(1,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE2:
                kRSGroup.SetBSRenderTargetWriteMask(2,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE3:
                kRSGroup.SetBSRenderTargetWriteMask(3,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE4:
                kRSGroup.SetBSRenderTargetWriteMask(4,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE5:
                kRSGroup.SetBSRenderTargetWriteMask(5,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE6:
                kRSGroup.SetBSRenderTargetWriteMask(6,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_COLORWRITEENABLE7:
                kRSGroup.SetBSRenderTargetWriteMask(7,
                    (unsigned char)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_BLENDOP:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetBSBlendOp((D3D10_BLEND_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_SCISSORTESTENABLE:
                kRSGroup.SetRSScissorEnable(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_SLOPESCALEDEPTHBIAS:
                uiD3D10Value = pkEntry->GetValue();
                kRSGroup.SetRSSlopeScaledDepthBias(
                    *((float*)&uiD3D10Value));
                break;
            case NSBD3D10RenderStates::NID3DRS_ANTIALIASEDLINEENABLE:
                kRSGroup.SetRSAntialiasedLineEnable(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_BLENDFACTOR:
                uiD3D10Value = pkEntry->GetValue();
                float afBlendFactor[4];
                afBlendFactor[0] = (float)(uiD3D10Value & 0xf);
                afBlendFactor[1] = (float)((uiD3D10Value>>8) & 0xf);
                afBlendFactor[2] = (float)((uiD3D10Value>>16) & 0xf);
                afBlendFactor[3] = (float)((uiD3D10Value>>24) & 0xf);
                kRSGroup.SetBlendFactor(afBlendFactor);
                break;
            case NSBD3D10RenderStates::NID3DRS_DEPTHBIAS:
                kRSGroup.SetRSDepthBias((int)(pkEntry->GetValue()));
                break;
            case NSBD3D10RenderStates::NID3DRS_SRCBLENDALPHA:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetBSSrcBlendAlpha((D3D10_BLEND)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_DESTBLENDALPHA:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetBSDestBlendAlpha((D3D10_BLEND)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NID3DRS_BLENDOPALPHA:
                if (NSBD3D10RenderStates::ConvertNSBD3D10RenderStateValue(
                    eState, pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetBSBlendOpAlpha((D3D10_BLEND_OP)uiD3D10Value);
                }
                break;
            case NSBD3D10RenderStates::NIOGLRS_SAMPLEALPHATOCOVERAGEENABLE:
                kRSGroup.SetBSAlphaToCoverageEnable(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_FRONTCCW:
                kRSGroup.SetRSFrontCounterClockwise(
                    (pkEntry->GetValue()) ? (true) : (false));
                break;
            case NSBD3D10RenderStates::NID3DRS_DEPTHBIASCLAMP:
                uiD3D10Value = pkEntry->GetValue();
                kRSGroup.SetRSDepthBiasClamp(*((float*)&uiD3D10Value));
                break;
            // Explicit NO-OP for unsupported render states, so unrecognized
            // states will trigger an assertion failure.
            case NSBD3D10RenderStates::NID3DRS_SHADEMODE:
            case NSBD3D10RenderStates::NID3DRS_ALPHATESTENABLE:
            case NSBD3D10RenderStates::NID3DRS_LASTPIXEL:
            case NSBD3D10RenderStates::NID3DRS_ALPHAREF:
            case NSBD3D10RenderStates::NID3DRS_ALPHAFUNC:
            case NSBD3D10RenderStates::NID3DRS_DITHERENABLE:
            case NSBD3D10RenderStates::NID3DRS_FOGENABLE:
            case NSBD3D10RenderStates::NID3DRS_SPECULARENABLE:
            case NSBD3D10RenderStates::NID3DRS_FOGCOLOR:
            case NSBD3D10RenderStates::NID3DRS_FOGTABLEMODE:
            case NSBD3D10RenderStates::NID3DRS_FOGSTART:
            case NSBD3D10RenderStates::NID3DRS_FOGEND:
            case NSBD3D10RenderStates::NID3DRS_FOGDENSITY:
            case NSBD3D10RenderStates::NID3DRS_RANGEFOGENABLE:
            case NSBD3D10RenderStates::NID3DRS_TEXTUREFACTOR:
            case NSBD3D10RenderStates::NID3DRS_WRAP0:
            case NSBD3D10RenderStates::NID3DRS_WRAP1:
            case NSBD3D10RenderStates::NID3DRS_WRAP2:
            case NSBD3D10RenderStates::NID3DRS_WRAP3:
            case NSBD3D10RenderStates::NID3DRS_WRAP4:
            case NSBD3D10RenderStates::NID3DRS_WRAP5:
            case NSBD3D10RenderStates::NID3DRS_WRAP6:
            case NSBD3D10RenderStates::NID3DRS_WRAP7:
            case NSBD3D10RenderStates::NID3DRS_CLIPPING:
            case NSBD3D10RenderStates::NID3DRS_LIGHTING:
            case NSBD3D10RenderStates::NID3DRS_AMBIENT:
            case NSBD3D10RenderStates::NID3DRS_FOGVERTEXMODE:
            case NSBD3D10RenderStates::NID3DRS_COLORVERTEX:
            case NSBD3D10RenderStates::NID3DRS_LOCALVIEWER:
            case NSBD3D10RenderStates::NID3DRS_NORMALIZENORMALS:
            case NSBD3D10RenderStates::NID3DRS_DIFFUSEMATERIALSOURCE:
            case NSBD3D10RenderStates::NID3DRS_SPECULARMATERIALSOURCE:
            case NSBD3D10RenderStates::NID3DRS_AMBIENTMATERIALSOURCE:
            case NSBD3D10RenderStates::NID3DRS_EMISSIVEMATERIALSOURCE:
            case NSBD3D10RenderStates::NID3DRS_VERTEXBLEND:
            case NSBD3D10RenderStates::NID3DRS_POINTSIZE:
            case NSBD3D10RenderStates::NID3DRS_POINTSIZE_MIN:
            case NSBD3D10RenderStates::NID3DRS_POINTSPRITEENABLE:
            case NSBD3D10RenderStates::NID3DRS_POINTSCALEENABLE:
            case NSBD3D10RenderStates::NID3DRS_POINTSCALE_A:
            case NSBD3D10RenderStates::NID3DRS_POINTSCALE_B:
            case NSBD3D10RenderStates::NID3DRS_POINTSCALE_C:
            case NSBD3D10RenderStates::NID3DRS_PATCHEDGESTYLE:
            case NSBD3D10RenderStates::NID3DRS_DEBUGMONITORTOKEN:
            case NSBD3D10RenderStates::NID3DRS_POINTSIZE_MAX:
            case NSBD3D10RenderStates::NID3DRS_INDEXEDVERTEXBLENDENABLE:
            case NSBD3D10RenderStates::NID3DRS_TWEENFACTOR:
            case NSBD3D10RenderStates::NID3DRS_POSITIONDEGREE:
            case NSBD3D10RenderStates::NID3DRS_NORMALDEGREE:
            case NSBD3D10RenderStates::NID3DRS_MINTESSELLATIONLEVEL:
            case NSBD3D10RenderStates::NID3DRS_MAXTESSELLATIONLEVEL:
            case NSBD3D10RenderStates::NID3DRS_ADAPTIVETESS_X:
            case NSBD3D10RenderStates::NID3DRS_ADAPTIVETESS_Y:
            case NSBD3D10RenderStates::NID3DRS_ADAPTIVETESS_Z:
            case NSBD3D10RenderStates::NID3DRS_ADAPTIVETESS_W:
            case NSBD3D10RenderStates::NID3DRS_ENABLEADAPTIVETESSELLATION:
            case NSBD3D10RenderStates::NID3DRS_TWOSIDEDSTENCILMODE:
            case NSBD3D10RenderStates::NID3DRS_SRGBWRITEENABLE:
            case NSBD3D10RenderStates::NID3DRS_WRAP8:
            case NSBD3D10RenderStates::NID3DRS_WRAP9:
            case NSBD3D10RenderStates::NID3DRS_WRAP10:
            case NSBD3D10RenderStates::NID3DRS_WRAP11:
            case NSBD3D10RenderStates::NID3DRS_WRAP12:
            case NSBD3D10RenderStates::NID3DRS_WRAP13:
            case NSBD3D10RenderStates::NID3DRS_WRAP14:
            case NSBD3D10RenderStates::NID3DRS_WRAP15:
            case NSBD3D10RenderStates::NID3DRS_SEPARATEALPHABLENDENABLE:
            case NSBD3D10RenderStates::NID3DRS_VIEWPORTENABLE:
            case NSBD3D10RenderStates::NID3DRS_HIGHPRECISIONBLENDENABLE:
            case NSBD3D10RenderStates::NID3DRS_HIGHPRECISIONBLENDENABLE1:
            case NSBD3D10RenderStates::NID3DRS_HIGHPRECISIONBLENDENABLE2:
            case NSBD3D10RenderStates::NID3DRS_HIGHPRECISIONBLENDENABLE3:
            case NSBD3D10RenderStates::NID3DRS_TESSELLATIONMODE:
            case NSBD3D10RenderStates::NIOGLRS_COLORLOGICOPENABLE:
            case NSBD3D10RenderStates::NIOGLRS_CULLFACEENABLE:
            case NSBD3D10RenderStates::NIOGLRS_MULTISAMPLEENABLE:
            case NSBD3D10RenderStates::NIOGLRS_POINTSMOOTHENABLE:
            case NSBD3D10RenderStates::NIOGLRS_POLYGONOFFSETFILLENABLE:
            case NSBD3D10RenderStates::NIOGLRS_SAMPLEALPHATOONEENABLE:
            case NSBD3D10RenderStates::NIOGLRS_SAMPLECOVERAGEENABLE:
            case NSBD3D10RenderStates::NIOGLRS_VERTEXPROGRAMPOINTSIZEENABLE:
            case NSBD3D10RenderStates::NIOGLRS_BLENDCOLORR:
            case NSBD3D10RenderStates::NIOGLRS_BLENDCOLORG:
            case NSBD3D10RenderStates::NIOGLRS_BLENDCOLORB:
            case NSBD3D10RenderStates::NIOGLRS_BLENDCOLORA:
            case NSBD3D10RenderStates::NIOGLRS_SRCBLENDSEPARATERGB:
            case NSBD3D10RenderStates::NIOGLRS_DSTBLENDSEPARATERGB:
            case NSBD3D10RenderStates::NIOGLRS_BLENDEQUATIONSEPARATERGB:
            case NSBD3D10RenderStates::NIOGLRS_CULLFACE:
            case NSBD3D10RenderStates::NIOGLRS_COLORMASKR:
            case NSBD3D10RenderStates::NIOGLRS_COLORMASKG:
            case NSBD3D10RenderStates::NIOGLRS_COLORMASKB:
            case NSBD3D10RenderStates::NIOGLRS_COLORMASKA:
            case NSBD3D10RenderStates::NIOGLRS_DEPTHRANGENEAR:
            case NSBD3D10RenderStates::NIOGLRS_DEPTHRANGEFAR:
            case NSBD3D10RenderStates::NIOGLRS_FRONTFACE:
            case NSBD3D10RenderStates::NIOGLRS_LINEWIDTH:
            case NSBD3D10RenderStates::NIOGLRS_POINTSPRITECOORDREPLACE:
            case NSBD3D10RenderStates::NIOGLRS_POLYGONMODEFACE:
            case NSBD3D10RenderStates::NIOGLRS_POLYGONOFFSETFACTOR:
            case NSBD3D10RenderStates::NIOGLRS_POLYGONOFFSETUNITS:
            case NSBD3D10RenderStates::NIOGLRS_SCISSORX:
            case NSBD3D10RenderStates::NIOGLRS_SCISSORY:
            case NSBD3D10RenderStates::NIOGLRS_SCISSORWIDTH:
            case NSBD3D10RenderStates::NIOGLRS_SCISSORHEIGHT:
                break; 
            default:
                NIASSERT(!"NSBD3D10StateGroup::SetupRenderStateGroup_D3D10>"
                    "  State not recognized!");
                break;
            }
        }
        else
        {
            // Not supported.
            NIASSERT(!"NSBD3D10StateGroup::GetRenderStateGroup_D3D10>  "
                "Attribute state support not implemented.");
        }

        pkEntry = GetNextState();
    }

    return true;
}
//---------------------------------------------------------------------------
// Handle filtering options used during texture sampling.

// Only the following D3D10_FILTER values are supported:
//   MIN_MAG_MIP_POINT               MIN_MAG_POINT_MIP_LINEAR
//   MIN_POINT_MAG_LINEAR_MIP_POINT  MIN_POINT_MAG_MIP_LINEAR
//   MIN_LINEAR_MAG_MIP_POINT        MIN_LINEAR_MAG_POINT_MIP_LINEAR 
//   MIN_MAG_LINEAR_MIP_POINT        MIN_MAG_MIP_LINEAR 
//   ANISOTROPIC

bool NSBD3D10StateGroup::SetupTextureSamplerGroup_D3D10(
    NiD3D10RenderStateGroup& kRSGroup, unsigned int uiStage)
{
    if (m_kStateList.GetSize() == 0)
        return false;

    // Accumulate state info to set combination states in D3D10.
    NSBD3D10StageAndSamplerStates::NiD3D10TextureFilter eMinFilter
        = NSBD3D10StageAndSamplerStates::NID3DTEXF_NONE;
    NSBD3D10StageAndSamplerStates::NiD3D10TextureFilter eMagFilter
        = NSBD3D10StageAndSamplerStates::NID3DTEXF_NONE;
    NSBD3D10StageAndSamplerStates::NiD3D10TextureFilter eMipFilter
        = NSBD3D10StageAndSamplerStates::NID3DTEXF_NONE;
    bool bMipmapEnable = true;

    NSBD3D10SGEntry* pkEntry = GetFirstState();
    while (pkEntry)
    {
        if (pkEntry->UsesAttribute() == false)
        {
            // Convert it and set sampler states.

            unsigned int uiNSBValue = 0x7fffffff;
            unsigned int uiD3D10Value = 0x7fffffff;

            NSBD3D10StageAndSamplerStates::NiD3D10TextureSamplerState eTSS =
                (NSBD3D10StageAndSamplerStates::NiD3D10TextureSamplerState)
                pkEntry->GetState();

            if (eTSS == 0x7fffffff)  // Sanity check.
                continue;

            switch(eTSS)
            {
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MINFILTER:
                uiNSBValue = pkEntry->GetValue();
                eMinFilter = (NSBD3D10StageAndSamplerStates::
                    NiD3D10TextureFilter)uiNSBValue;
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MAGFILTER:
                uiNSBValue = pkEntry->GetValue();
                eMagFilter = (NSBD3D10StageAndSamplerStates::
                    NiD3D10TextureFilter)uiNSBValue;
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MIPFILTER:
                uiNSBValue = pkEntry->GetValue();
                eMipFilter = (NSBD3D10StageAndSamplerStates::
                    NiD3D10TextureFilter)uiNSBValue;
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_ADDRESSU:
                if (NSBD3D10StageAndSamplerStates::
                    ConvertNSBD3D10TextureSamplerStateValue(eTSS,
                    pkEntry->GetValue(), uiD3D10Value))
                {
                kRSGroup.SetSamplerAddressU(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, (D3D10_TEXTURE_ADDRESS_MODE)uiD3D10Value);
                }
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_ADDRESSV:
                if (NSBD3D10StageAndSamplerStates::
                    ConvertNSBD3D10TextureSamplerStateValue(eTSS,
                    pkEntry->GetValue(), uiD3D10Value))
                {
                kRSGroup.SetSamplerAddressV(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, (D3D10_TEXTURE_ADDRESS_MODE)uiD3D10Value);
                }
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_ADDRESSW:
                if (NSBD3D10StageAndSamplerStates::
                    ConvertNSBD3D10TextureSamplerStateValue(eTSS,
                    pkEntry->GetValue(), uiD3D10Value))
                {
                kRSGroup.SetSamplerAddressW(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, (D3D10_TEXTURE_ADDRESS_MODE)uiD3D10Value);
                }
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MIPMAPLODBIAS:
                uiD3D10Value = pkEntry->GetValue();
                kRSGroup.SetSamplerMipLODBias(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, *((float*)&uiD3D10Value));
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MAXANISOTROPY:
                kRSGroup.SetSamplerMaxAnisotropy(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, pkEntry->GetValue());
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_COMPARISONFUNC:
                if (NSBD3D10StageAndSamplerStates::
                    ConvertNSBD3D10TextureSamplerStateValue(eTSS,
                    pkEntry->GetValue(), uiD3D10Value))
                {
                    kRSGroup.SetSamplerComparisonFunc(
                        NiGPUProgram::PROGRAM_PIXEL, uiStage,
                        (D3D10_COMPARISON_FUNC)uiD3D10Value);
                }
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_BORDERCOLOR:
                    uiD3D10Value = pkEntry->GetValue();
                float afSampBorderColor[4];
                afSampBorderColor[0] = (float)(uiD3D10Value & 0xf);
                afSampBorderColor[1] = (float)((uiD3D10Value>>8) & 0xf);
                afSampBorderColor[2] = (float)((uiD3D10Value>>16) & 0xf);
                afSampBorderColor[3] = (float)((uiD3D10Value>>24) & 0xf);
                kRSGroup.SetSamplerBorderColor(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, afSampBorderColor);
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MINMIPLEVEL:
                uiD3D10Value = pkEntry->GetValue();
                kRSGroup.SetSamplerMinLOD(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, *((float*)&uiD3D10Value));
                break;
            case NSBD3D10StageAndSamplerStates::NID3DSAMP_MAXMIPLEVEL:
                uiD3D10Value = pkEntry->GetValue();
                kRSGroup.SetSamplerMaxLOD(NiGPUProgram::PROGRAM_PIXEL,
                    uiStage, *((float*)&uiD3D10Value));
                break;
            default:
                break;
            }
        }
        else
        {
            // Not supported.
            NIASSERT(!"NSBD3D10StateGroup::GetRenderStateGroup_D3D10>  "
                "Attribute state support not implemented.");
        }

        pkEntry = GetNextState();
    }

    unsigned int uiD3D10Value = 0x7fffffff;

    if (NSBD3D10StageAndSamplerStates::
        ConvertNSBD3D10TextureSamplerFilterValue(eMinFilter, eMagFilter,
        eMipFilter, uiD3D10Value, bMipmapEnable))
    {
        kRSGroup.SetSamplerFilter(NiGPUProgram::PROGRAM_PIXEL, uiStage,
            (D3D10_FILTER)uiD3D10Value);
        float fMaxLOD = (bMipmapEnable ? D3D10_FLOAT32_MAX : 0.0f);
        kRSGroup.SetSamplerMaxLOD(NiGPUProgram::PROGRAM_PIXEL, uiStage, 
            fMaxLOD);
    }

    return true;
}
//---------------------------------------------------------------------------
