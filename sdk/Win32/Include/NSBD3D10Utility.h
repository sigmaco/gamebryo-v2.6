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

#ifndef NSBD3D10UTILITY_H
#define NSBD3D10UTILITY_H

#include "NiD3D10BinaryShaderLibLibType.h"
#include "NSBD3D10RenderStates.h"
#include <NiBinaryStream.h>

class NSBD3D10StateGroup;
class NSBD3D10ConstantMap;

//---------------------------------------------------------------------------

#ifndef D3DSHADER_VERSION_MAJOR
#define D3DSHADER_VERSION_MAJOR(_Version) (((_Version)>>8)&0xFF)
#endif  // #ifndef D3DSHADER_VERSION_MAJOR

#ifndef D3DSHADER_VERSION_MINOR
#define D3DSHADER_VERSION_MINOR(_Version) (((_Version)>>0)&0xFF)
#endif  // #ifndef D3DSHADER_VERSION_MINOR

#ifndef D3DVS_VERSION
#define D3DVS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))
#endif  //#ifndef D3DVS_VERSION

#ifndef D3DGS_VERSION
#define D3DGS_VERSION(_Major,_Minor) (0xFFFD0000|((_Major)<<8)|(_Minor))
#endif  //#ifndef D3DGS_VERSION

#ifndef D3DPS_VERSION
#define D3DPS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))
#endif  //#ifndef D3DPS_VERSION

#if defined(NIDEBUG)
#define STATE_CASE_STRING(x)      case x: return #x;
#endif  //#if defined(NIDEBUG)

class NID3D10BINARYSHADERLIB_ENTRY NSBD3D10Utility : public NiMemObject
{
public:
    static bool SaveBinaryStateGroup(NiBinaryStream& kStream,
        NSBD3D10StateGroup* pkStateGroup);
    static bool LoadBinaryStateGroup(NiBinaryStream& kStream,
        NSBD3D10StateGroup*& pkStateGroup);

    static bool SaveBinaryConstantMap(NiBinaryStream& kStream,
        NSBD3D10ConstantMap* pkMap);
    static bool LoadBinaryConstantMap(NiBinaryStream& kStream,
        NSBD3D10ConstantMap*& pkMap);

    // GetFileXXX
    static bool GetSplitPath(const char* pcFilename, char* pcDrive, 
        char* pcDir, char* pcFname, char* pcExt);
    static bool GetFilePath(const char* pcFilename, char* pcPath,
        unsigned int uiMaxLen);
    static bool GetFileName(const char* pcFilename, char* pcName,
        unsigned int uiMaxLen);
    static bool GetFileExtension(const char* pcFilename, char* pcExt,
        unsigned int uiMaxLen);

    // String setting helper function.
    static unsigned int SetString(char*& pcDest, unsigned int uiDestSize,
        const char* pcSrc);

    // Write to stream
    static bool WriteData(NiBinaryStream& kStream, void* pvData,
        unsigned int uiSize);

    // Read from stream
    static bool ReadData(NiBinaryStream& kStream, void*& pvData,
        unsigned int& uiSize);
    static bool AllocateAndReadData(NiBinaryStream& kStream, void*& pvData,
        unsigned int& uiDataSize);

    // Log
    static void Log(char* pcFmt, ...);
    static bool GetLogEnabled();
    static void SetLogEnabled(bool bEnabled);

#if defined(NIDEBUG)
    static unsigned int ms_uiIndent;

    static void IndentInsert();
    static void IndentRemove();
    static void Dump(FILE* pf, bool bIndent, char* pcFmt, ...);
#endif  //#if defined(NIDEBUG)


protected:
    static bool ms_bLogEnabled;
};

//---------------------------------------------------------------------------

#if defined(_PS3)
    #include <NiPS3Utility.h>
    typedef NiPS3Utility NiRendererUtility;

    #include <NiPS3ShaderConstantMap.h>
    typedef NiPS3ShaderConstantMap NiPlatformShaderConstantMap;
#elif defined(WIN32) || defined(_XENON)
    #include <NiD3DUtility.h>
    typedef NiD3DUtility NiRendererUtility;

    #include <NiD3DShaderConstantMap.h>
    typedef NiD3DShaderConstantMap NiPlatformShaderConstantMap;
#endif

//---------------------------------------------------------------------------

#endif  //NSBD3D10UTILITY_H
