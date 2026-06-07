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

#ifndef NSBUTILITY_H
#define NSBUTILITY_H

#include "NiBinaryShaderLibLibType.h"
#include <NiMemObject.h>
#include <NiFile.h>

class NSBStateGroup;
class NSBConstantMap;
class NiBinaryStream;

//---------------------------------------------------------------------------

#ifndef D3DGS_VERSION
#define D3DGS_VERSION(_Major,_Minor) (0xFFFD0000|((_Major)<<8)|(_Minor))
#endif  //#ifndef D3DGS_VERSION

#if defined(NIDEBUG)
#define STATE_CASE_STRING(x)      case x: return #x;
#endif  //#if defined(NIDEBUG)

class NIBINARYSHADERLIB_ENTRY NSBUtility : public NiMemObject
{
public:
    static bool SaveBinaryStateGroup(NiBinaryStream& kStream,
        NSBStateGroup* pkStateGroup);
    static bool LoadBinaryStateGroup(NiBinaryStream& kStream,
        NSBStateGroup*& pkStateGroup);

    static bool SaveBinaryConstantMap(NiBinaryStream& kStream,
        NSBConstantMap* pkMap);
    static bool LoadBinaryConstantMap(NiBinaryStream& kStream,
        NSBConstantMap*& pkMap);

#if defined(NIDEBUG)
    static unsigned int ms_uiIndent;

    static void IndentInsert();
    static void IndentRemove();
    static void Dump(FILE* pf, bool bIndent, const char* pcFmt, ...);
#endif  //#if defined(NIDEBUG)

};

//---------------------------------------------------------------------------

#if defined(_PS3)
    #include <NiPS3Utility.h>
    typedef NiPS3Utility NiRendererUtility;

    #include <NiPS3ShaderConstantMap.h>
    typedef NiPS3ShaderConstantMap NiPlatformShaderConstantMap;

    #include <NiPS3Pass.h>
    typedef NiPS3Pass NiPlatformShaderPass;

    #include <NiPS3TextureSampler.h>
    typedef NiPS3TextureSampler NiPlatformTextureSampler;
#elif defined(WIN32) || defined(_XENON)
    #include <NiD3DUtility.h>
    typedef NiD3DUtility NiRendererUtility;

    #include <NiD3DShaderConstantMap.h>
    typedef NiD3DShaderConstantMap NiPlatformShaderConstantMap;

    #include <NiD3DPass.h>
    typedef NiD3DPass NiPlatformShaderPass;

    #include <NiD3DTextureStage.h>
    typedef NiD3DTextureStage NiPlatformTextureSampler;
#else
#error "Unsupported platform"
#endif

//---------------------------------------------------------------------------

#endif  //NSBUTILITY_H
