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

#include "NiDX9RendererSDM.h"
#include "NiDX9Renderer.h"
#include "NiD3DShader.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DShaderProgramCreator.h"
#include "NiD3DShaderProgramCreatorAsm.h"
#include "NiD3DShaderProgramCreatorObj.h"
#include "NiD3DShaderProgramCreatorHLSL.h"

NiImplementSDMConstructor(NiDX9Renderer, "NiMesh NiFloodgate NiMain");

#ifdef NIDX9RENDERER_EXPORT
NiImplementDllMain(NiDX9Renderer);
#endif

//---------------------------------------------------------------------------
void NiDX9RendererSDM::Init()
{
    NiImplementSDMInitCheck();

    // Don't add D3DFMT_A2R10G10B10 because we don't have a corresponding
    // NiDX9Renderer::FrameBufferFormat to describe it.
    // NiDX9Renderer::AddDisplayFormat(D3DFMT_A2R10G10B10);
    NiDX9Renderer::AddDisplayFormat(D3DFMT_A8R8G8B8);
    NiDX9Renderer::AddDisplayFormat(D3DFMT_X8R8G8B8);
    NiDX9Renderer::AddDisplayFormat(D3DFMT_A1R5G5B5);
    NiDX9Renderer::AddDisplayFormat(D3DFMT_X1R5G5B5);
    NiDX9Renderer::AddDisplayFormat(D3DFMT_R5G6B5);

    NiD3DRenderStateGroup::InitializePools();
    NiD3DTextureStageGroup::InitializePools();
    NiD3DShader::InitializePools();

    NiD3DShaderFactory::_SDMInit();
    NiD3DShaderProgramFactory::_SDMInit();

    NiD3DShaderProgramCreator::_SDMInit();
    NiD3DShaderProgramCreatorAsm::_SDMInit();
    NiD3DShaderProgramCreatorObj::_SDMInit();
    NiD3DShaderProgramCreatorHLSL::_SDMInit();

    NiD3DShader::_SDMInit();
}
//---------------------------------------------------------------------------
void NiDX9RendererSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiD3DShader::_SDMShutdown();

    NiD3DShaderProgramCreatorHLSL::_SDMShutdown();
    NiD3DShaderProgramCreatorObj::_SDMShutdown();
    NiD3DShaderProgramCreatorAsm::_SDMShutdown();
    NiD3DShaderProgramCreator::_SDMShutdown();

    NiD3DShaderProgramFactory::_SDMShutdown();
    NiD3DShaderFactory::_SDMShutdown();

    NiD3DShader::ShutdownPools();
    NiD3DRenderStateGroup::ShutdownPools();
    NiD3DTextureStageGroup::ShutdownPools();

    NiDX9Renderer::_SDMShutdown();
}
//---------------------------------------------------------------------------
