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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectFactory.h"
#include "NiD3D10EffectLoader.h"
#include "NiD3D10EffectParser.h"
#include "NiD3D10EffectShaderLibrary.h"
#include "NiD3D10EffectShaderSDM.h"

NiImplementSDMConstructor(NiD3D10EffectShader,
    "NiD3D10Renderer NiMesh NiFloodgate NiMain");

//---------------------------------------------------------------------------
void NiD3D10EffectShaderSDM::Init()
{
    NiImplementSDMInitCheck();

    NiD3D10EffectFactory::_SDMInit();
}
//---------------------------------------------------------------------------
void NiD3D10EffectShaderSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiD3D10EffectParser::_SDMShutdown();
    NiD3D10EffectShaderLibrary::SetDirectoryInfo(0);
    NiD3D10EffectLoader::Destroy();

    NiD3D10EffectFactory::_SDMShutdown();
}
//---------------------------------------------------------------------------
