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

#include "NiD3D10RendererPCH.h"

#include "NiCubeMapDepthStencilBuffer.h"
#include "NiD3D10Renderer.h"

NiImplementRTTI(NiCubeMapDepthStencilBuffer, NiDepthStencilBuffer);

//---------------------------------------------------------------------------
NiCubeMapDepthStencilBuffer::NiCubeMapDepthStencilBuffer()
{
}
//---------------------------------------------------------------------------
NiCubeMapDepthStencilBuffer::~NiCubeMapDepthStencilBuffer()
{
}
//---------------------------------------------------------------------------
NiCubeMapDepthStencilBuffer* NiCubeMapDepthStencilBuffer::Create(
    unsigned int uiWidth, unsigned int uiHeight, NiRenderer*, 
    const NiPixelFormat& kFormat, Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    if (kFormat.GetFormat() != NiPixelFormat::FORMAT_DEPTH_STENCIL)
        return NULL;

    NiCubeMapDepthStencilBuffer* pkBuffer = NiNew NiCubeMapDepthStencilBuffer;
    pkBuffer->m_uiWidth = uiWidth;
    pkBuffer->m_uiHeight = uiHeight;
    
    if (!pkBuffer->CreateRendererData(kFormat, eMSAAPref))
    {
        NiDelete pkBuffer;
        return NULL;
    }
    return pkBuffer;
}
//---------------------------------------------------------------------------
bool NiCubeMapDepthStencilBuffer::CreateRendererData(
    const NiPixelFormat& kFormat, Ni2DBuffer::MultiSamplePreference eMSAAPref) 
{
    if (m_spRendererData)
        return true;

    NIASSERT(NiIsKindOf(NiD3D10Renderer, NiRenderer::GetRenderer()));
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    if (pkRenderer && !pkRenderer->CreateDepthStencilRendererData(this, 
        &kFormat, eMSAAPref, true))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
