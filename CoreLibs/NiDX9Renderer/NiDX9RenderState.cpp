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
#include "NiD3DRendererPCH.h"

#include "NiDX9RenderState.h"
#include "NiDX9Renderer.h"
#include "NiDX9ShaderConstantManager.h"

#include <NiMatrix3.h>
#include <NiPoint3.h>
#include <NiColor.h>
#include <NiFogProperty.h>
#include <NiDitherProperty.h>
#include <NiMaterialProperty.h>
#include <NiShadeProperty.h>
#include <NiSpecularProperty.h>
#include <NiTexturingProperty.h>
#include <NiTransform.h>
#include <NiWireframeProperty.h>
#include <NiZBufferProperty.h>

#include "NiD3DDefines.h"
#include "NiD3DPass.h"
#include "NiD3DUtils.h"

// Make sure these are always accurate!!!
// Entries labeled ** depend on device and must be set at runtime
NiDX9RenderState::RenderStateInitialValue
NiDX9RenderState::ms_akInitialRenderStateSettings[] = 
{
    D3DRS_ZENABLE,                      D3DZB_FALSE,        //**
    D3DRS_FILLMODE,                     D3DFILL_SOLID,
    D3DRS_SHADEMODE,                    D3DSHADE_GOURAUD,
    D3DRS_ZWRITEENABLE,                 TRUE,
    D3DRS_ALPHATESTENABLE,              FALSE,
    D3DRS_LASTPIXEL,                    TRUE,
    D3DRS_SRCBLEND,                     D3DBLEND_ONE,
    D3DRS_DESTBLEND,                    D3DBLEND_ZERO,
    D3DRS_CULLMODE,                     D3DCULL_CCW,
    D3DRS_ZFUNC,                        D3DCMP_LESSEQUAL,
    D3DRS_ALPHAREF,                     0x00000000,
    D3DRS_ALPHAFUNC,                    D3DCMP_ALWAYS,
    D3DRS_DITHERENABLE,                 FALSE,
    D3DRS_ALPHABLENDENABLE,             FALSE,
    D3DRS_FOGENABLE,                    FALSE,
    D3DRS_SPECULARENABLE,               FALSE,
    D3DRS_FOGCOLOR,                     0x00000000,
    D3DRS_FOGTABLEMODE,                 D3DFOG_NONE,
    D3DRS_FOGSTART,                     0x00000000,
    D3DRS_FOGEND,                       0x3F800000,
    D3DRS_FOGDENSITY,                   0x3F800000,
    D3DRS_RANGEFOGENABLE,               FALSE,
    D3DRS_STENCILENABLE,                FALSE,
    D3DRS_STENCILFAIL,                  D3DSTENCILOP_KEEP,
    D3DRS_STENCILZFAIL,                 D3DSTENCILOP_KEEP,
    D3DRS_STENCILPASS,                  D3DSTENCILOP_KEEP,
    D3DRS_STENCILFUNC,                  D3DCMP_ALWAYS,
    D3DRS_STENCILREF,                   0,
    D3DRS_STENCILMASK,                  0xFFFFFFFF,
    D3DRS_STENCILWRITEMASK,             0xFFFFFFFF,
    D3DRS_TEXTUREFACTOR,                0xFFFFFFFF,
    D3DRS_WRAP0,                        0,
    D3DRS_WRAP1,                        0,
    D3DRS_WRAP2,                        0,
    D3DRS_WRAP3,                        0,
    D3DRS_WRAP4,                        0,
    D3DRS_WRAP5,                        0,
    D3DRS_WRAP6,                        0,
    D3DRS_WRAP7,                        0,
    D3DRS_CLIPPING,                     TRUE,
    D3DRS_LIGHTING,                     TRUE,
    D3DRS_AMBIENT,                      0x00000000,
    D3DRS_FOGVERTEXMODE,                D3DFOG_NONE,
    D3DRS_COLORVERTEX,                  TRUE,
    D3DRS_LOCALVIEWER,                  TRUE,
    D3DRS_NORMALIZENORMALS,             FALSE,
    D3DRS_DIFFUSEMATERIALSOURCE,        D3DMCS_COLOR1,
    D3DRS_SPECULARMATERIALSOURCE,       D3DMCS_COLOR2,
    D3DRS_AMBIENTMATERIALSOURCE,        D3DMCS_MATERIAL,
    D3DRS_EMISSIVEMATERIALSOURCE,       D3DMCS_MATERIAL,
    D3DRS_VERTEXBLEND,                  D3DVBF_DISABLE,
    D3DRS_CLIPPLANEENABLE,              0,
    D3DRS_POINTSIZE,                    0x3F800000,
    D3DRS_POINTSIZE_MIN,                0x3F800000,
    D3DRS_POINTSPRITEENABLE,            FALSE,
    D3DRS_POINTSCALEENABLE,             FALSE,
    D3DRS_POINTSCALE_A,                 0x3F800000,
    D3DRS_POINTSCALE_B,                 0x00000000,
    D3DRS_POINTSCALE_C,                 0x00000000,
    D3DRS_MULTISAMPLEANTIALIAS,         TRUE,
    D3DRS_MULTISAMPLEMASK,              0xFFFFFFFF,
    D3DRS_PATCHEDGESTYLE,               D3DPATCHEDGE_DISCRETE,
    D3DRS_DEBUGMONITORTOKEN,            D3DDMT_ENABLE,
    D3DRS_POINTSIZE_MAX,                0x42800000,
    D3DRS_INDEXEDVERTEXBLENDENABLE,     FALSE,
    D3DRS_COLORWRITEENABLE,             0x0000000F,
    D3DRS_TWEENFACTOR,                  0x00000000,
    D3DRS_BLENDOP,                      D3DBLENDOP_ADD,
    D3DRS_POSITIONDEGREE,               D3DDEGREE_CUBIC,
    D3DRS_NORMALDEGREE,                 D3DDEGREE_LINEAR,
    D3DRS_SCISSORTESTENABLE,            FALSE,
    D3DRS_SLOPESCALEDEPTHBIAS,          0x00000000,
    D3DRS_ANTIALIASEDLINEENABLE,        FALSE, 
    D3DRS_MINTESSELLATIONLEVEL,         0x3F800000,
    D3DRS_MAXTESSELLATIONLEVEL,         0x3F800000,
    D3DRS_ADAPTIVETESS_X,               0x00000000,
    D3DRS_ADAPTIVETESS_Y,               0x00000000,
    D3DRS_ADAPTIVETESS_Z,               0x3F800000,
    D3DRS_ADAPTIVETESS_W,               0x00000000,
    D3DRS_ENABLEADAPTIVETESSELLATION,   FALSE,
    D3DRS_TWOSIDEDSTENCILMODE,          FALSE,
    D3DRS_CCW_STENCILFAIL,              0x00000001,
    D3DRS_CCW_STENCILZFAIL,             0x00000001,
    D3DRS_CCW_STENCILPASS,              0x00000001,
    D3DRS_CCW_STENCILFUNC,              0x00000008,
    D3DRS_COLORWRITEENABLE1,            0x0000000F,
    D3DRS_COLORWRITEENABLE2,            0x0000000F,
    D3DRS_COLORWRITEENABLE3,            0x0000000F,
    D3DRS_BLENDFACTOR,                  0xFFFFFFFF,
    D3DRS_SRGBWRITEENABLE,              0x00000000,
    D3DRS_DEPTHBIAS,                    0x00000000,
    D3DRS_WRAP8,                        0,
    D3DRS_WRAP9,                        0,
    D3DRS_WRAP10,                       0,
    D3DRS_WRAP11,                       0,
    D3DRS_WRAP12,                       0,
    D3DRS_WRAP13,                       0,
    D3DRS_WRAP14,                       0,
    D3DRS_WRAP15,                       0,
    D3DRS_SEPARATEALPHABLENDENABLE,     FALSE,
    D3DRS_SRCBLENDALPHA,                D3DBLEND_ONE,
    D3DRS_DESTBLENDALPHA,               D3DBLEND_ZERO,
    D3DRS_BLENDOPALPHA,                 D3DBLENDOP_ADD,
    (D3DRENDERSTATETYPE)0xFFFFFFFF,     0xFFFFFFFF
};

// Make sure these are always accurate!!!
// Entries labeled ** depend on stage value and must be set at runtime
NiDX9RenderState::TextureStageStateInitialValue 
NiDX9RenderState::ms_akInitialTextureStageStateSettings[] = 
{
    D3DTSS_COLOROP,                 D3DTOP_MODULATE,    //**
    D3DTSS_COLORARG1,               D3DTA_TEXTURE,
    D3DTSS_COLORARG2,               D3DTA_CURRENT,
    D3DTSS_ALPHAOP,                 D3DTOP_SELECTARG1,  //**
    D3DTSS_ALPHAARG1,               D3DTA_TEXTURE,
    D3DTSS_ALPHAARG2,               D3DTA_CURRENT,
    D3DTSS_BUMPENVMAT00,            0x00000000,
    D3DTSS_BUMPENVMAT01,            0x00000000,
    D3DTSS_BUMPENVMAT10,            0x00000000,
    D3DTSS_BUMPENVMAT11,            0x00000000,
    D3DTSS_TEXCOORDINDEX,           0,                  //**
    D3DTSS_BUMPENVLSCALE,           0x00000000,
    D3DTSS_BUMPENVLOFFSET,          0x00000000,
    D3DTSS_TEXTURETRANSFORMFLAGS,   D3DTTFF_DISABLE,
    D3DTSS_COLORARG0,               D3DTA_CURRENT,
    D3DTSS_ALPHAARG0,               D3DTA_CURRENT,
    D3DTSS_RESULTARG,               D3DTA_CURRENT,
    D3DTSS_RESULTARG,               D3DTA_CURRENT,
    //
    (D3DTEXTURESTAGESTATETYPE)0xFFFFFFFF, 0xFFFFFFFF
};

// Sampler States
// Mappings
unsigned int NiDX9RenderState::ms_auiSamplesStateMappings[
    NiDX9RenderState::NISAMP_TOTAL_COUNT] = 
{
    D3DSAMP_ADDRESSU,       //NISAMP_ADDRESSU,
    D3DSAMP_ADDRESSV,       //NISAMP_ADDRESSV,
    D3DSAMP_ADDRESSW,       //NISAMP_ADDRESSW,
    D3DSAMP_BORDERCOLOR,    //NISAMP_BORDERCOLOR,
    D3DSAMP_MAGFILTER,      //NISAMP_MAGFILTER,
    D3DSAMP_MINFILTER,      //NISAMP_MINFILTER,
    D3DSAMP_MIPFILTER,      //NISAMP_MIPFILTER,
    D3DSAMP_MIPMAPLODBIAS,  //NISAMP_MIPMAPLODBIAS,
    D3DSAMP_MAXMIPLEVEL,    //NISAMP_MAXMIPLEVEL,
    D3DSAMP_MAXANISOTROPY,  //NISAMP_MAXANISOTROPY,
    D3DSAMP_SRGBTEXTURE,    //NISAMP_SRGBTEXTURE,
    D3DSAMP_ELEMENTINDEX,   //NISAMP_ELEMENTINDEX,
    D3DSAMP_DMAPOFFSET      //NISAMP_DMAPOFFSET,
};

NiDX9RenderState::SamplerStateInitialValue 
NiDX9RenderState::ms_akInitialSamplerStateSettings[] = 
{
    D3DSAMP_ADDRESSU,                D3DTADDRESS_WRAP, 
    D3DSAMP_ADDRESSV,                D3DTADDRESS_WRAP, 
    D3DSAMP_ADDRESSW,                D3DTADDRESS_WRAP, 
    D3DSAMP_BORDERCOLOR,             0x00000000, 
    D3DSAMP_MAGFILTER,               D3DTEXF_POINT, 
    D3DSAMP_MINFILTER,               D3DTEXF_POINT, 
    D3DSAMP_MIPFILTER,               D3DTEXF_NONE, 
    D3DSAMP_MIPMAPLODBIAS,           0, 
    D3DSAMP_MAXMIPLEVEL,             0,  
    D3DSAMP_MAXANISOTROPY,           1, 
    D3DSAMP_SRGBTEXTURE,             0, 
    D3DSAMP_ELEMENTINDEX,            0, 
    D3DSAMP_DMAPOFFSET,              256, 
    // 
    (D3DSAMPLERSTATETYPE)0xFFFFFFFF, 0xFFFFFFFF
};

//---------------------------------------------------------------------------
// Device and renderer access functions
//---------------------------------------------------------------------------
void NiDX9RenderState::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (m_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    m_pkD3DDevice = pkD3DDevice;
    if (m_pkD3DDevice)
        D3D_POINTER_REFERENCE(m_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiDX9RenderState::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    m_pkD3DRenderer = pkD3DRenderer;
    if (m_pkD3DRenderer)
        SetD3DDevice(m_pkD3DRenderer->GetD3DDevice());
    else
        SetD3DDevice(0);
}
//---------------------------------------------------------------------------
NiDX9RenderState* NiDX9RenderState::Create(NiD3DRenderer* pkRenderer, 
    const D3DCAPS9& kD3DCaps9, bool bZBuffer)
{
    NiDX9RenderState* pkRS = NiNew NiDX9RenderState(pkRenderer, kD3DCaps9);
    NIASSERT(pkRS);

    pkRS->ReadRenderStateCaps();

    // Initial render state values
    for (unsigned int i = 0; i < NI_MAX_RENDER_STATES; i++)
    {
        if (ms_akInitialRenderStateSettings[i].m_eType == 
            (D3DRENDERSTATETYPE)0xFFFFFFFF)
        {
            break;
        }
        else if (ms_akInitialRenderStateSettings[i].m_eType == D3DRS_ZENABLE)
        {
            ms_akInitialRenderStateSettings[i].m_uiValue = 
                (bZBuffer ? D3DZB_TRUE : D3DZB_FALSE);
        }
    }

    if (bZBuffer)
        pkRS->SetFlag(CAPS_ZBUFFER);

    pkRS->m_spShaderConstantManager = 
        NiDX9ShaderConstantManager::Create(pkRenderer, kD3DCaps9);

    return pkRS;
}
//---------------------------------------------------------------------------
void NiDX9RenderState::Reset(const D3DCAPS9& kD3DCaps9, 
    bool bZBuffer)
{
    NiMemcpy((void*)&m_kD3DCaps9, (const void*)&kD3DCaps9,
        sizeof(m_kD3DCaps9));

    ReadRenderStateCaps();

    // Initial render state values
    for (unsigned int i = 0; i < NI_MAX_RENDER_STATES; i++)
    {
        if (ms_akInitialRenderStateSettings[i].m_eType == 
            (D3DRENDERSTATETYPE)0xFFFFFFFF)
        {
            break;
        }
        else if (ms_akInitialRenderStateSettings[i].m_eType == D3DRS_ZENABLE)
        {
            ms_akInitialRenderStateSettings[i].m_uiValue = 
                (bZBuffer ? D3DZB_TRUE : D3DZB_FALSE);
        }
    }

    if (bZBuffer)
        SetFlag(CAPS_ZBUFFER);

    // Recreate the constant manager
    m_spShaderConstantManager = 
        NiDX9ShaderConstantManager::Create(m_pkD3DRenderer, kD3DCaps9);

    InitRenderModes();
}
//---------------------------------------------------------------------------
NiDX9RenderState::NiDX9RenderState(NiD3DRenderer* pkRenderer, 
    const D3DCAPS9& kD3DCaps9) :
    m_pkD3DDevice(0), 
    m_pkD3DRenderer(0)
{
    SetD3DRenderer(pkRenderer);

    m_aD3DVertexBlendMappings[0] = D3DVBF_DISABLE;
    m_aD3DVertexBlendMappings[1] = D3DVBF_1WEIGHTS;             // 1
    m_aD3DVertexBlendMappings[2] = D3DVBF_1WEIGHTS;             // 1
    m_aD3DVertexBlendMappings[3] = D3DVBF_2WEIGHTS;             // 3
    m_aD3DVertexBlendMappings[4] = D3DVBF_3WEIGHTS;             // 5

    m_uiSrcAlphaCaps = 0;
    m_uiDestAlphaCaps = 0;

    m_spDisabledAlphaProperty = NiNew NiAlphaProperty; // default is disabled

    m_auiAlphaMapping[NiAlphaProperty::ALPHA_ONE] = D3DBLEND_ONE;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_ZERO] = D3DBLEND_ZERO;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_SRCCOLOR] = D3DBLEND_SRCCOLOR;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_INVSRCCOLOR] = 
        D3DBLEND_INVSRCCOLOR;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_DESTCOLOR] = 
        D3DBLEND_DESTCOLOR;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_INVDESTCOLOR] = 
        D3DBLEND_INVDESTCOLOR;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_SRCALPHA] = D3DBLEND_SRCALPHA;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_INVSRCALPHA] = 
        D3DBLEND_INVSRCALPHA;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_DESTALPHA] = 
        D3DBLEND_DESTALPHA;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_INVDESTALPHA] = 
        D3DBLEND_INVDESTALPHA;
    m_auiAlphaMapping[NiAlphaProperty::ALPHA_SRCALPHASAT] = 
        D3DBLEND_SRCALPHASAT;

    m_auiAlphaTestMapping[NiAlphaProperty::TEST_ALWAYS] = D3DCMP_ALWAYS;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_LESS] = D3DCMP_LESS;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_EQUAL] = D3DCMP_EQUAL;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_LESSEQUAL] = 
        D3DCMP_LESSEQUAL;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_GREATER] = D3DCMP_GREATER;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_NOTEQUAL] = D3DCMP_NOTEQUAL;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_GREATEREQUAL] = 
        D3DCMP_GREATEREQUAL;
    m_auiAlphaTestMapping[NiAlphaProperty::TEST_NEVER] = D3DCMP_NEVER;

    m_auiStencilTestMapping[NiStencilProperty::TEST_ALWAYS] = D3DCMP_ALWAYS;
    m_auiStencilTestMapping[NiStencilProperty::TEST_LESS] = D3DCMP_LESS;
    m_auiStencilTestMapping[NiStencilProperty::TEST_EQUAL] = D3DCMP_EQUAL;
    m_auiStencilTestMapping[NiStencilProperty::TEST_LESSEQUAL] = 
        D3DCMP_LESSEQUAL;
    m_auiStencilTestMapping[NiStencilProperty::TEST_GREATER] = 
        D3DCMP_GREATER;
    m_auiStencilTestMapping[NiStencilProperty::TEST_NOTEQUAL] = 
        D3DCMP_NOTEQUAL;
    m_auiStencilTestMapping[NiStencilProperty::TEST_GREATEREQUAL] = 
        D3DCMP_GREATEREQUAL;
    m_auiStencilTestMapping[NiStencilProperty::TEST_NEVER] = D3DCMP_NEVER;

    m_auiStencilActionMapping[NiStencilProperty::ACTION_KEEP] = 
        D3DSTENCILOP_KEEP;
    m_auiStencilActionMapping[NiStencilProperty::ACTION_ZERO] = 
        D3DSTENCILOP_ZERO;
    m_auiStencilActionMapping[NiStencilProperty::ACTION_REPLACE] = 
        D3DSTENCILOP_REPLACE;
    m_auiStencilActionMapping[NiStencilProperty::ACTION_INCREMENT] = 
        D3DSTENCILOP_INCRSAT;
    m_auiStencilActionMapping[NiStencilProperty::ACTION_DECREMENT] = 
        D3DSTENCILOP_DECRSAT;
    m_auiStencilActionMapping[NiStencilProperty::ACTION_INVERT]    = 
        D3DSTENCILOP_INVERT;

    m_auiCullModeMapping[NiStencilProperty::DRAW_CCW_OR_BOTH][0] = 
        D3DCULL_CW;
    m_auiCullModeMapping[NiStencilProperty::DRAW_CCW][0] = D3DCULL_CW;
    m_auiCullModeMapping[NiStencilProperty::DRAW_CW][0] = D3DCULL_CCW;
    m_auiCullModeMapping[NiStencilProperty::DRAW_BOTH][0] = D3DCULL_NONE;
    m_auiCullModeMapping[NiStencilProperty::DRAW_CCW_OR_BOTH][1] = 
        D3DCULL_CCW;
    m_auiCullModeMapping[NiStencilProperty::DRAW_CCW][1] = D3DCULL_CCW;
    m_auiCullModeMapping[NiStencilProperty::DRAW_CW][1] = D3DCULL_CW;
    m_auiCullModeMapping[NiStencilProperty::DRAW_BOTH][1] = D3DCULL_NONE;

    m_uiLeftHanded = 0;

    m_fCameraNear = 0.0f;
    m_fCameraFar = 0.0f;
    m_fCameraDepthRange = 0.0f;
    m_fMaxFogFactor = 0.0f;
    m_fMaxFogValue = 1.0f;

    m_kCurrentFogColor = NiColor::BLACK;
    m_uiCurrentFogColor = D3DCOLOR_XRGB(
        (unsigned char)(255.0f*m_kCurrentFogColor.r),
        (unsigned char)(255.0f*m_kCurrentFogColor.g),
        (unsigned char)(255.0f*m_kCurrentFogColor.b));

    m_uiFlags = 0;

    m_auiZBufferTestMapping[NiZBufferProperty::TEST_ALWAYS] = D3DCMP_ALWAYS;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_LESS] = D3DCMP_LESS;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_EQUAL] = D3DCMP_EQUAL;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_LESSEQUAL] = 
        D3DCMP_LESSEQUAL;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_GREATER] = D3DCMP_GREATER;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_NOTEQUAL] = 
        D3DCMP_NOTEQUAL;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_GREATEREQUAL] = 
        D3DCMP_GREATEREQUAL;
    m_auiZBufferTestMapping[NiZBufferProperty::TEST_NEVER] = D3DCMP_NEVER;

    m_bForceNormalizeNormals = false;
    m_bInternalNormalizeNormals = false;

    NiMemcpy((void*)&m_kD3DCaps9, (const void*)&kD3DCaps9,
        sizeof(m_kD3DCaps9));
}
//---------------------------------------------------------------------------
NiDX9RenderState::~NiDX9RenderState()
{
    m_spDisabledAlphaProperty = 0;
    SetD3DRenderer(0);
}
//---------------------------------------------------------------------------
void NiDX9RenderState::UpdateRenderState(const NiPropertyState* pkNew)
{
    // Fog gets applied by SetupRenderingPass in the shader system default
    // pipeline.
    ApplyWireframe(pkNew->GetWireframe());
    ApplyShading(pkNew->GetShade());
    ApplyAlpha(pkNew->GetAlpha());
    ApplyZBuffer(pkNew->GetZBuffer());
    ApplyDitherMode(pkNew->GetDither());
    ApplySpecular(pkNew->GetSpecular());
    ApplyStencil(pkNew->GetStencil());
    ApplyMaterial(pkNew->GetMaterial());
    ApplyFog(pkNew->GetFog());
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyAlpha(const NiAlphaProperty* pkNew)
{
    bool bNewAlpha = pkNew->GetAlphaBlending();
    if (bNewAlpha)
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, true);
        SetRenderState(D3DRS_SRCBLEND, 
            m_auiAlphaMapping[pkNew->GetSrcBlendMode()]);
        SetRenderState(D3DRS_DESTBLEND, 
            m_auiAlphaMapping[pkNew->GetDestBlendMode()]);
    }
    else
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    }

    if (pkNew->GetAlphaTesting())
    {
        SetRenderState(D3DRS_ALPHATESTENABLE, true);
        SetRenderState(D3DRS_ALPHAFUNC, 
            m_auiAlphaTestMapping[pkNew->GetTestMode()]);
        SetRenderState(D3DRS_ALPHAREF, pkNew->GetTestRef());
    }
    else
    {
        SetRenderState(D3DRS_ALPHATESTENABLE, false);
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyDitherMode(const NiDitherProperty* pkNew)
{
    SetRenderState(D3DRS_DITHERENABLE, pkNew->GetDithering());
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyFog(const NiFogProperty* pkNew)
{
    // there are 3 possible outcomes:
    //
    // Linear Z per-vertex fog computed by D3D
    // Per-vertex fog supplied to D3D in TLVERT by IGP (or NI)
    // No fog
    if (pkNew->GetFog())
    {
        SetRenderState(D3DRS_FOGENABLE, TRUE);
        // Replacement code for above allowing the renderer to use a max
        // fog density other than 100%.  The given fog density is the 
        // fraction fog that is attained at the far plane.  Note that values
        // less than 1.0f can allow far-plane clipping to be evident
        float fWorldDepth = m_fCameraDepthRange * pkNew->GetDepth();
        float fFogNear = m_fCameraFar - fWorldDepth;
        float fFogFar = m_fCameraFar + m_fMaxFogFactor * fWorldDepth;

        float fDensity = 1.0f / fWorldDepth;
        SetRenderState(D3DRS_FOGDENSITY, F2DW(fDensity));
        SetRenderState(D3DRS_FOGSTART, F2DW(fFogNear));
        SetRenderState(D3DRS_FOGEND, F2DW(fFogFar));            

        // the following code assumes that:
        // 1) D3DRS_FOGVERTEXMODE = D3DFOG_LINEAR at app start
        // 2) D3DRS_FOGTABLEMODE is not changed unless table fog is
        // available
        // 3) The table fog capability never changes during an app - this is
        // sensible, since HW caps don't change

        if (pkNew->GetFogFunction() == NiFogProperty::FOG_Z_LINEAR)
        {
            // this flag cannot change over the course of the app
            if (IsFlag(CAPS_TABLE_FOG))
            {
                SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
                SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
            }

            SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
        }
        else if (pkNew->GetFogFunction() == NiFogProperty::FOG_RANGE_SQ)
        {
            if (IsFlag(CAPS_TABLE_FOG))
            {
                SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);

                SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
            }

            SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
        }
        else
        {
            // This turns off fogging if fog vertex alpha is supported
            SetRenderState(D3DRS_FOGENABLE, FALSE);
        }

        // Only reconvert the fog color from NiColor to DWORD when 
        // the new fog color is different from the last known fog color
        const NiColor& kFogColor = pkNew->GetFogColor();
        if (kFogColor.r != m_kCurrentFogColor.r ||
            kFogColor.g != m_kCurrentFogColor.g ||
            kFogColor.b != m_kCurrentFogColor.b)
        {
            m_uiCurrentFogColor = D3DCOLOR_XRGB(
                (unsigned char)(255.0f*kFogColor.r),
                (unsigned char)(255.0f*kFogColor.g),
                (unsigned char)(255.0f*kFogColor.b));

            m_kCurrentFogColor = kFogColor;
        }

        // Always set render state with the last known fog color, in case
        // the fog color has been changed outside of this function
        // (due to a shader or application changing the render state).
        // Redundant state changes will be caught by the standard mechanism.
        SetRenderState(D3DRS_FOGCOLOR, m_uiCurrentFogColor);
    }
    else
    {
        SetRenderState(D3DRS_FOGENABLE, FALSE);
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyMaterial(NiMaterialProperty* pkNew)
{
    m_kMaterials.SetCurrentMaterial(*pkNew);
    m_kMaterials.UpdateToDevice(m_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyShading(const NiShadeProperty* pkNew)
{
    SetRenderState(D3DRS_SHADEMODE, 
        (pkNew->GetSmooth() ? D3DSHADE_GOURAUD : D3DSHADE_FLAT));
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplySpecular(const NiSpecularProperty* pkNew)
{
    SetRenderState(D3DRS_SPECULARENABLE, pkNew->GetSpecular());
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyStencil(const NiStencilProperty* pkNew)
{

    if(pkNew->GetStencilOn())
    {
        SetRenderState(D3DRS_STENCILENABLE, TRUE);
        SetRenderState(D3DRS_STENCILFUNC, 
            m_auiStencilTestMapping[pkNew->GetStencilFunction()]);
        SetRenderState(D3DRS_STENCILREF, pkNew->GetStencilReference());
        SetRenderState(D3DRS_STENCILMASK, pkNew->GetStencilMask());
        SetRenderState(D3DRS_STENCILFAIL, 
            m_auiStencilActionMapping[pkNew->GetStencilFailAction()]);
        SetRenderState(D3DRS_STENCILZFAIL, 
            m_auiStencilActionMapping[pkNew->GetStencilPassZFailAction()]);
        SetRenderState(D3DRS_STENCILPASS, 
            m_auiStencilActionMapping[pkNew->GetStencilPassAction()]);
    }
    else
    {
        SetRenderState(D3DRS_STENCILENABLE, FALSE);
    }

    SetRenderState(D3DRS_CULLMODE, 
        m_auiCullModeMapping[pkNew->GetDrawMode()][m_uiLeftHanded]);
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyWireframe(const NiWireframeProperty* pkNew)
{
    SetRenderState(D3DRS_FILLMODE, 
        (pkNew->GetWireframe() ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ApplyZBuffer(const NiZBufferProperty* pkNew)
{
    if (pkNew->GetZBufferTest() || pkNew->GetZBufferWrite())
    {
        SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

        if (pkNew->GetZBufferTest())
        {
            SetRenderState(D3DRS_ZFUNC, 
                m_auiZBufferTestMapping[pkNew->GetTestFunction()]);
        }
        else
        {
            SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
        }
    }
    else
    {
        SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    }

    SetRenderState(D3DRS_ZWRITEENABLE, pkNew->GetZBufferWrite());
}
//---------------------------------------------------------------------------
void NiDX9RenderState::SetModelTransform(const NiTransform& kXform)
{
    if (((kXform.m_fScale >= 0.99f) && (kXform.m_fScale <= 1.01f)) &&
        !m_bInternalNormalizeNormals && !m_bForceNormalizeNormals)
    {
        SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
    }
    else
    {
        SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::SetAntiAlias(bool)
{
    /* */
}
//---------------------------------------------------------------------------
void NiDX9RenderState::PurgeMaterial(NiMaterialProperty* pkMaterial)
{
    m_kMaterials.PurgeMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiDX9RenderState::InitRenderState()
{
    NiDX9RenderState::RenderStateInitialValue* pkInit = 
        &ms_akInitialRenderStateSettings[0];
    unsigned int uiIndex = 0;
    while (pkInit->m_eType != 0xFFFFFFFF)
    {
        m_akRenderStateSettings[pkInit->m_eType].InitValue(
            pkInit->m_uiValue);
        m_pkD3DDevice->SetRenderState(pkInit->m_eType, 
            pkInit->m_uiValue);
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(RENDER_STATE_CHANGES, 1);

        uiIndex++;
        pkInit = &ms_akInitialRenderStateSettings[uiIndex];
        NIASSERT(pkInit);
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::SaveRenderState()
{
    for (unsigned int i = 0; i < NI_MAX_RENDER_STATES; i++)
    {
        m_akRenderStateSettings[i].SaveValue();
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::RestoreRenderState()
{
    for (unsigned int i = 0; i < NI_MAX_RENDER_STATES; i++)
    {
        RestoreRenderState((D3DRENDERSTATETYPE)i);
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::InitShaderState()
{
    m_bDeclaration = false;
    m_uiCurrentFVF = 0;
    m_uiPreviousFVF = 1;
    m_hCurrentVertexDeclaration = (NiD3DVertexDeclaration)0;
    m_hPreviousVertexDeclaration = (NiD3DVertexDeclaration)1;

    m_hCurrentVertexShader = (NiD3DVertexShaderHandle)0;
    m_hPreviousVertexShader = (NiD3DVertexShaderHandle)1;
    m_hCurrentPixelShader = (NiD3DPixelShaderHandle)0;
    m_hPreviousPixelShader = (NiD3DPixelShaderHandle)1;
}
//---------------------------------------------------------------------------
void NiDX9RenderState::SaveShaderState()
{
    m_uiPreviousFVF = m_uiCurrentFVF;
    m_hPreviousVertexDeclaration = m_hCurrentVertexDeclaration;

    m_hPreviousVertexShader = m_hCurrentVertexShader;
    m_hPreviousPixelShader = m_hCurrentPixelShader;
}
//---------------------------------------------------------------------------
void NiDX9RenderState::RestoreShaderState()
{
    RestoreFVF();
    RestoreDeclaration();

    RestorePixelShader();
    RestoreVertexShader();
}
//---------------------------------------------------------------------------
void NiDX9RenderState::InitTextureStageState()
{
    for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxTextureBlendStages; i++)
    {
        unsigned int uiIndex = 0;

        TextureStageStateInitialValue* pkTSSInit = 
            &ms_akInitialTextureStageStateSettings[0];

        D3DTEXTURESTAGESTATETYPE eTSSType = pkTSSInit->m_eType;
        while (eTSSType != 0xFFFFFFFF)
        {
            if (eTSSType == D3DTSS_TEXCOORDINDEX)
            {
                pkTSSInit->m_uiValue = i;
            }
            else if (eTSSType == D3DTSS_COLOROP || eTSSType == D3DTSS_ALPHAOP)
            {
                if (i != 0)
                    pkTSSInit->m_uiValue = D3DTOP_DISABLE;
            }

            m_akTextureStageStateSettings[i][eTSSType].InitValue(
                pkTSSInit->m_uiValue);
            m_pkD3DDevice->SetTextureStageState(i, eTSSType, 
                pkTSSInit->m_uiValue);

            uiIndex++;
            pkTSSInit = &ms_akInitialTextureStageStateSettings[uiIndex];
            NIASSERT(pkTSSInit);
            eTSSType = pkTSSInit->m_eType;
        }
    }

    for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxSamplers; i++)
    {
        unsigned int uiIndex = 0;

        SamplerStateInitialValue* pkSSInit = 
            &ms_akInitialSamplerStateSettings[0];

        D3DSAMPLERSTATETYPE eSSType = pkSSInit->m_eType;
        while (eSSType != 0xFFFFFFFF)
        {
            m_akSamplerStateSettings[i][eSSType].InitValue(
                pkSSInit->m_uiValue);
            m_pkD3DDevice->SetSamplerState(i, eSSType, pkSSInit->m_uiValue);

            uiIndex++;
            pkSSInit = &ms_akInitialSamplerStateSettings[uiIndex];
            NIASSERT(pkSSInit);
            eSSType = pkSSInit->m_eType;
        }
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::SaveTextureStageState()
{
    for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxTextureBlendStages; i++)
    {
        unsigned int j = 0;
        for (; j < NI_MAX_TEXTURE_STAGE_STATES; j++)
        {
            m_akTextureStageStateSettings[i][j].SaveValue();
        }
        for (j = 0; j < NI_MAX_SAMPLER_STATES; j++)
        {
            m_akSamplerStateSettings[i][j].SaveValue();
        }
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::RestoreTextureStageState()
{
    for (unsigned int i = 0; i < NiD3DPass::ms_uiMaxTextureBlendStages; i++)
    {
        unsigned int j = 0;
        for (; j < NI_MAX_TEXTURE_STAGE_STATES; j++)
        {
            RestoreTextureStageState(i, (D3DTEXTURESTAGESTATETYPE)j);
        }
        for (j = 0; j < NI_MAX_SAMPLER_STATES; j++)
        {
            RestoreSamplerState(i, (D3DSAMPLERSTATETYPE)j);
        }
    }
}
//---------------------------------------------------------------------------
void NiDX9RenderState::InitRenderModes()
{
    InitRenderState();
    InitShaderState();
    InitTextureStageState();
    InitTextures();
    m_kMaterials.Initialize();

    SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    SetRenderState(D3DRS_SRCBLEND, 
        m_auiAlphaMapping[NiAlphaProperty::ALPHA_SRCALPHA]);
    SetRenderState(D3DRS_DESTBLEND, 
        m_auiAlphaMapping[NiAlphaProperty::ALPHA_INVSRCALPHA]);
    SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
    SetRenderState(D3DRS_ALPHAREF, 0);
    SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    SetRenderState(D3DRS_DITHERENABLE, FALSE);
    SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    SetRenderState(D3DRS_FOGENABLE, FALSE);
    SetRenderState(D3DRS_FOGTABLEMODE, IsFlag(CAPS_TABLE_FOG) ? 
        D3DFOG_LINEAR : D3DFOG_NONE);
    SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    SetRenderState(D3DRS_FOGCOLOR, 0);
    SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
    SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    SetRenderState(D3DRS_COLORVERTEX, TRUE);

    m_kCurrentFogColor = NiColor::BLACK;
    m_uiCurrentFogColor = D3DCOLOR_XRGB(
        (unsigned char)(255.0f*m_kCurrentFogColor.r),
        (unsigned char)(255.0f*m_kCurrentFogColor.g),
        (unsigned char)(255.0f*m_kCurrentFogColor.b));
}
//---------------------------------------------------------------------------
unsigned int NiDX9RenderState::GetSamplerStateMapping(
    unsigned int uiSamplerState)
{
    if (uiSamplerState < NISAMP_TOTAL_COUNT)
        return ms_auiSamplesStateMappings[uiSamplerState];
    else
        return 0xFFFFFFFF;
}
//---------------------------------------------------------------------------
void NiDX9RenderState::ReadRenderStateCaps()
{
    //  Again, for DX9, these will be knowns...
    if ((m_kD3DCaps9.RasterCaps & D3DPRASTERCAPS_FOGTABLE) &&
        (m_kD3DCaps9.RasterCaps & D3DPRASTERCAPS_WFOG))
    {
        SetFlag(CAPS_TABLE_FOG);
    }
    else
    {
        ClearFlag(CAPS_TABLE_FOG);
    }

    // now, test the alpha blending caps for blend modes
    // Note, we still won't work correctly on the old Permedia cards,
    // which cannot support some _combinations_ of supported modes
    // we would have to read the framebuffer to test this.
    m_uiSrcAlphaCaps = m_uiDestAlphaCaps = 0;

    for(unsigned int i = 0; i < NiAlphaProperty::ALPHA_MAX_MODES; i++)
    {
        if (m_auiAlphaMapping[i] & m_kD3DCaps9.SrcBlendCaps)
            m_uiSrcAlphaCaps |= (1<<i);
        if (m_auiAlphaMapping[i] & m_kD3DCaps9.DestBlendCaps)
            m_uiDestAlphaCaps |= (1<<i);
    }

    if (m_kD3DCaps9.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV)
    {
        SetFlag(CAPS_IND_UV_WRAP);
    }
    else
    {
        ClearFlag(CAPS_IND_UV_WRAP);
    }
}
//---------------------------------------------------------------------------
