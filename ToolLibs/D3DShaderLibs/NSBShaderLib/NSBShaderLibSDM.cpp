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
#include "NSBShaderLibPCH.h"

#include "NSBShaderLib.h"
#include "NSBShaderLibrary.h"

#ifdef WIN32
#include "NSBD3D10ShaderLibrary.h"
#endif

#include "NSBShaderLibSDM.h"

//---------------------------------------------------------------------------
#if defined(WIN32)
NiImplementSDMConstructor(NSBShaderLib, "NiMain NiMesh NiFloodgate \
    NiDX9Renderer NiD3D10Renderer");
#elif defined(_PS3)
NiImplementSDMConstructor(NSBShaderLib, "NiMain NiMesh NiFloodgate \
    NiPS3Renderer");
#elif defined(_XENON)
NiImplementSDMConstructor(NSBShaderLib, "NiMain NiMesh NiFloodgate \
    NiXenonRenderer");
#endif

//---------------------------------------------------------------------------
void NSBShaderLibSDM::Init()
{
    NiImplementSDMInitCheck();
}
//---------------------------------------------------------------------------
void NSBShaderLibSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NSBShaderLibrary::SetDirectoryInfo(0);

#ifdef WIN32
    NSBD3D10ShaderLibrary::SetDirectoryInfo(0);
#endif

}
//---------------------------------------------------------------------------
