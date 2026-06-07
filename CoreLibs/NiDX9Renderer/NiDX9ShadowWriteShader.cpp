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
#include "NiDX9ShadowWriteShader.h"
#include "NiD3DShaderProgramFactory.h"

bool NiDX9ShadowWriteShader::ms_bRenderBackfaces = true;
unsigned int NiDX9ShadowWriteShader::ms_auiBackfaceCullModeMapping[
    NiStencilProperty::DRAW_MAX][2] =
{
    D3DCULL_CCW,    // NiStencilProperty::DRAW_CCW_OR_BOTH, Left Hand
    D3DCULL_CW,     // NiStencilProperty::DRAW_CCW_OR_BOTH, Right Hand
    D3DCULL_CCW,    // NiStencilProperty::DRAW_CCW, Left Hand
    D3DCULL_CW,     // NiStencilProperty::DRAW_CCW, Right Hand
    D3DCULL_CW,     // NiStencilProperty::DRAW_CW, Left Hand
    D3DCULL_CCW,    // NiStencilProperty::DRAW_CW, Right Hand
    D3DCULL_NONE,   // NiStencilProperty::DRAW_BOTH, Left Hand
    D3DCULL_NONE    // NiStencilProperty::DRAW_BOTH, Right Hand 
};

NiImplementRTTI(NiDX9ShadowWriteShader, NiDX9FragmentShader);

//---------------------------------------------------------------------------
NiDX9ShadowWriteShader::NiDX9ShadowWriteShader(
    NiMaterialDescriptor* pkDesc) : NiDX9FragmentShader(pkDesc)
{
    /* */
}
//---------------------------------------------------------------------------
NiDX9ShadowWriteShader::~NiDX9ShadowWriteShader()
{
    /* */ 
}
//---------------------------------------------------------------------------
unsigned int NiDX9ShadowWriteShader::PreProcessPipeline(
    const NiRenderCallContext& kRCC)
{
    unsigned int uiRet = NiDX9FragmentShader::PreProcessPipeline(kRCC);

    // If set to render backfaces, flip the direction of the cull mode render 
    // state.
    if (ms_bRenderBackfaces)
    {
        NiDX9RenderState* pkRenderState = 
            NiDX9Renderer::GetRenderer()->GetRenderState();

        NiStencilProperty* pkStencilProp = kRCC.m_pkState->GetStencil();

        NiStencilProperty::DrawMode eDrawMode = pkStencilProp->GetDrawMode();
        bool bLeftHand = pkRenderState->GetLeftHanded();

        pkRenderState->SetRenderState(D3DRS_CULLMODE, 
            ms_auiBackfaceCullModeMapping[eDrawMode][bLeftHand]);
    }

    return uiRet;
}
//---------------------------------------------------------------------------