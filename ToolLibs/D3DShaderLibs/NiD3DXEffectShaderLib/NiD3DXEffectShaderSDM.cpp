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
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectShaderLibrary.h"
#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectLoader.h"
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectShaderSDM.h"

#if defined(_XENON)
NiImplementSDMConstructor(NiD3DXEffectShader,
    "NiXenonRenderer NiMesh NiFloodgate NiMain");
#elif defined(WIN32)
NiImplementSDMConstructor(NiD3DXEffectShader,
    "NiDX9Renderer NiMesh NiFloodgate NiMain");
#else
#error Invalid platform specification.
#endif

//---------------------------------------------------------------------------
void NiD3DXEffectShaderSDM::Init()
{
    NiImplementSDMInitCheck();
}
//---------------------------------------------------------------------------
void NiD3DXEffectShaderSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiD3DXEffectParameter::ReleaseBoneArray();
    NiD3DXEffectShaderLibrary::SetDirectoryInfo(0);
    NiD3DXEffectLoader::Destroy();
    
    NiD3DXEffectFactory::_SDMShutdown();
}
//---------------------------------------------------------------------------
