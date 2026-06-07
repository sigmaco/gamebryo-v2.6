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

#include "NiD3D10RendererSDM.h"
#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10Shader.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10ShaderProgramCreatorHLSL.h"
#include "NiD3D10ShaderProgramFactory.h"

NiImplementSDMConstructor(NiD3D10Renderer, "NiMesh NiFloodgate NiMain");

#ifdef NID3D10RENDERER_EXPORT
NiImplementDllMain(NiD3D10Renderer);
#endif

//---------------------------------------------------------------------------
void NiD3D10RendererSDM::Init()
{
    NiImplementSDMInitCheck();

    NiD3D10Error::_SDMInit();
    NiD3D10Shader::_SDMInit();
    NiD3D10ShaderFactory::_SDMInit();
    NiD3D10ShaderProgramFactory::_SDMInit();

    NiD3D10ShaderProgramCreatorHLSL::_SDMInit();
}
//---------------------------------------------------------------------------
void NiD3D10RendererSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiD3D10Renderer::_SDMShutdown();

    NiD3D10ShaderProgramCreatorHLSL::_SDMShutdown();

    NiD3D10ShaderProgramFactory::_SDMShutdown();
    NiD3D10ShaderFactory::_SDMShutdown();
    NiD3D10Shader::_SDMShutdown();
    NiD3D10Error::_SDMShutdown();
}
//---------------------------------------------------------------------------
