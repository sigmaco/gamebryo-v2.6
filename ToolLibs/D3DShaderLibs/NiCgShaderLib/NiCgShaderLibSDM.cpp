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
#include "NiCgShaderLibPCH.h"

#include "NiCgShaderLib.h"
#include "NiCgShaderLibSDM.h"

#include "NiCgShaderContext.h"
#include "NiD3DCgPixelShader.h"
#include "NiD3DCgVertexShader.h"
#include "NiD3DShaderProgramCreatorCg.h"
#include "NiD3DShaderProgramCreatorCgObj.h"

NiImplementSDMConstructor(NiCgShaderLib,
    "NiDX9Renderer NiMesh NiFloodgate NiMain");

#ifdef NICGSHADERLIB_ENTRY
NiImplementDllMain(NiCgShaderLib);
#endif

//---------------------------------------------------------------------------
void NiCgShaderLibSDM::Init()
{
    NiImplementSDMInitCheck();

    NiCgShaderContext::_SDMInit();

    NiD3DShaderProgramCreatorCg::_SDMInit();
    NiD3DShaderProgramCreatorCgObj::_SDMInit();
}
//---------------------------------------------------------------------------
void NiCgShaderLibSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiD3DShaderProgramCreatorCgObj::_SDMShutdown();
    NiD3DShaderProgramCreatorCg::_SDMShutdown();

#if defined(WIN32)
    cgD3D9SetDevice(NULL);
#endif

    NiCgShaderContext::_SDMShutdown();
}
//---------------------------------------------------------------------------
