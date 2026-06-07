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
#include "NiD3D10RendererPCH.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10ShadowWriteShader.h"

bool NiD3D10ShadowWriteShader::ms_bRenderBackfaces = true;
NiImplementRTTI(NiD3D10ShadowWriteShader, NiD3D10FragmentShader);

//---------------------------------------------------------------------------
NiD3D10ShadowWriteShader::NiD3D10ShadowWriteShader(
    NiMaterialDescriptor* pkDesc) : NiD3D10FragmentShader(pkDesc)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10ShadowWriteShader::~NiD3D10ShadowWriteShader()
{
    /* */ 
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShadowWriteShader::PreProcessPipeline(
    const NiRenderCallContext& kRCC)
{
    unsigned int uiRet = NiD3D10FragmentShader::PreProcessPipeline(kRCC);

    // If set to render backfaces, flip the direction of the cull mode render 
    // state.
    if (ms_bRenderBackfaces)
    {
        NiD3D10RenderStateManager* pkRenderState = 
            NiD3D10Renderer::GetRenderer()->GetRenderStateManager();

        D3D10_RASTERIZER_DESC kRasterizerDesc;
        pkRenderState->GetRasterizerStateDesc(kRasterizerDesc);
        
        if (kRasterizerDesc.CullMode == D3D10_CULL_FRONT)
            kRasterizerDesc.CullMode = D3D10_CULL_BACK;
        else if (kRasterizerDesc.CullMode == D3D10_CULL_BACK)
            kRasterizerDesc.CullMode = D3D10_CULL_FRONT;

        pkRenderState->SetRasterizerStateDesc(
            kRasterizerDesc, NiD3D10RenderStateManager::RSVALID_CULLMODE);
    }

    return uiRet;
}
//---------------------------------------------------------------------------