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

#include "NiD3D10ShaderLibrary.h"

#include <NiShaderLibraryDesc.h>

char* NiD3D10ShaderLibrary::ms_pcDirectory = 0;
unsigned int NiD3D10ShaderLibrary::ms_uiPlatform = 0;
unsigned int NiD3D10ShaderLibrary::ms_uiVSVersion = 0;
unsigned int NiD3D10ShaderLibrary::ms_uiGSVersion = 0;
unsigned int NiD3D10ShaderLibrary::ms_uiPSVersion = 0;
unsigned int NiD3D10ShaderLibrary::ms_uiUserVersion = 0;

//---------------------------------------------------------------------------
NiD3D10ShaderLibrary::NiD3D10ShaderLibrary(const char* pcName) :
    NiShaderLibrary(pcName)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10ShaderLibrary::~NiD3D10ShaderLibrary()
{
    /* */
}
//---------------------------------------------------------------------------
void NiD3D10ShaderLibrary::SetDirectoryInfo(const char* pcDirectory)
{
    NiDelete[] ms_pcDirectory;
    if (pcDirectory && pcDirectory[0] != '\0')
    {
        size_t stLen = strlen(pcDirectory) + 1;
        ms_pcDirectory = NiAlloc(char, stLen);
        NIASSERT(ms_pcDirectory);
        NiStrcpy(ms_pcDirectory, stLen, pcDirectory);
    }
}
//---------------------------------------------------------------------------
const char* NiD3D10ShaderLibrary::GetDirectory()
{
    return ms_pcDirectory;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderLibrary::SetPlatform(unsigned int uiPlatform)
{
    ms_uiPlatform = uiPlatform;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderLibrary::GetPlatform()
{
    return ms_uiPlatform;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderLibrary::SetVersionInfo(const char* pcVersionName, 
    unsigned int uiVersion)
{
    if (NiStricmp(pcVersionName, "PSVERSION") == 0)
        ms_uiPSVersion = uiVersion;
    else if (NiStricmp(pcVersionName, "VSVERSION") == 0)
        ms_uiVSVersion = uiVersion;
    else if (NiStricmp(pcVersionName, "GSVERSION") == 0)
        ms_uiGSVersion = uiVersion;
    else if (NiStricmp(pcVersionName, "USERVERSION") == 0)
        ms_uiUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderLibrary::GetVersionNumber(const char* pcVersionName)
{
    if (NiStricmp(pcVersionName, "PSVERSION") == 0)
        return ms_uiPSVersion;
    else if (NiStricmp(pcVersionName, "VSVERSION") == 0)
        return ms_uiVSVersion;
    else if (NiStricmp(pcVersionName, "GSVERSION") == 0)
        return ms_uiGSVersion;
    else if (NiStricmp(pcVersionName, "USERVERSION") == 0)
        return ms_uiUserVersion;
    return 0x7fffffff;
}
//---------------------------------------------------------------------------
NiShaderLibraryDesc* NiD3D10ShaderLibrary::GetShaderLibraryDesc()
{
    return m_spShaderLibraryDesc;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderLibrary::SetShaderLibraryDesc(
    NiShaderLibraryDesc* pkLibDesc)
{
    m_spShaderLibraryDesc = pkLibDesc;
}
//---------------------------------------------------------------------------
