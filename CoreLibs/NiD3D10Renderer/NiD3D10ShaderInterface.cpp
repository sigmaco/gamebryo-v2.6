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

#include "NiD3D10ShaderInterface.h"

#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10MeshMaterialBinding.h"

NiImplementRTTI(NiD3D10ShaderInterface, NiShader);

//---------------------------------------------------------------------------
NiD3D10ShaderInterface::NiD3D10ShaderInterface()
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->RegisterD3D10Shader(this);
}
//---------------------------------------------------------------------------
NiD3D10ShaderInterface::~NiD3D10ShaderInterface()
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer)
        pkRenderer->ReleaseD3D10Shader(this);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderInterface::Initialize()
{
    return NiShader::Initialize();
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::DestroyRendererData()
{
    /* */
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::RecreateRendererData()
{
    /* */
}
//---------------------------------------------------------------------------
const D3D10_STATE_BLOCK_MASK* NiD3D10ShaderInterface::GetStateBlockMask(
    const NiRenderCallContext&) const
{
    return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::ReferenceVertexDeclarationCache(
    NiVertexDeclarationCache kCache) const
{
    // In D3D10, NiVertexDeclarationCache is a NiD3D10MeshMaterialBinding
    if (kCache)
    {
        ((NiD3D10MeshMaterialBinding*)kCache)->IncRefCount();
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::ReleaseVertexDeclarationCache(
    NiVertexDeclarationCache kCache) const
{
    // In DX9, NiVertexDeclarationCache is NiD3D10MeshMaterialBinding
    if (kCache)
    {
        ((NiD3D10MeshMaterialBinding*)kCache)->DecRefCount();
    }
}
//---------------------------------------------------------------------------
