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

#include "NiD3DShaderLibrary.h"
#include <NiRTLib.h>

//---------------------------------------------------------------------------
char* NiD3DShaderLibrary::ms_pcDirectory = 0;
unsigned int NiD3DShaderLibrary::ms_uiPlatform = 0;
unsigned int NiD3DShaderLibrary::ms_uiPSVersion = 0;
unsigned int NiD3DShaderLibrary::ms_uiVSVersion = 0;
unsigned int NiD3DShaderLibrary::ms_uiUserVersion = 0;
//---------------------------------------------------------------------------
NiD3DShaderLibrary::NiD3DShaderLibrary(const char* pcName) :
    NiShaderLibrary(pcName),
    m_hLibrary(0),
    m_pkD3DRenderer(0)
{
}
//---------------------------------------------------------------------------
NiD3DShaderLibrary::~NiD3DShaderLibrary()
{
    m_spShaderLibraryDesc = 0;
    m_hLibrary = 0;
}
//---------------------------------------------------------------------------
void NiD3DShaderLibrary::SetDirectoryInfo(const char* pcDirectory)
{
    NiDelete [] ms_pcDirectory;
    if (pcDirectory && pcDirectory[0] != '\0')
    {
        size_t stLen = strlen(pcDirectory) + 1;
        ms_pcDirectory = NiAlloc(char, stLen);
        NIASSERT(ms_pcDirectory);
        NiStrcpy(ms_pcDirectory, stLen, pcDirectory);
    }
}
//---------------------------------------------------------------------------
const char* NiD3DShaderLibrary::GetDirectory()
{
    return ms_pcDirectory;
}
//---------------------------------------------------------------------------
void NiD3DShaderLibrary::SetPlatform(unsigned int uiPlatform)
{
    ms_uiPlatform = uiPlatform;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderLibrary::GetPlatform()
{
    return ms_uiPlatform;
}
//---------------------------------------------------------------------------
void NiD3DShaderLibrary::SetVersionInfo(const char* pcVersionName, 
    unsigned int uiVersion)
{
    if (NiStricmp(pcVersionName, "PSVERSION") == 0)
        ms_uiPSVersion = uiVersion;
    else if (NiStricmp(pcVersionName, "VSVERSION") == 0)
        ms_uiVSVersion = uiVersion;
    else if (NiStricmp(pcVersionName, "USERVERSION") == 0)
        ms_uiUserVersion = uiVersion;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderLibrary::GetVersionNumber(const char* pcVersionName)
{
    if (NiStricmp(pcVersionName, "PSVERSION") == 0)
        return ms_uiPSVersion;
    else if (NiStricmp(pcVersionName, "VSVERSION") == 0)
        return ms_uiVSVersion;
    else if (NiStricmp(pcVersionName, "USERVERSION") == 0)
        return ms_uiUserVersion;
    return 0x7fffffff;
}
//---------------------------------------------------------------------------
void NiD3DShaderLibrary::SetRenderer(NiD3DRenderer* pkRenderer)
{
    m_pkD3DRenderer = pkRenderer;
}
//---------------------------------------------------------------------------
NiD3DRenderer* NiD3DShaderLibrary::GetRenderer()
{
    return m_pkD3DRenderer;
}
//---------------------------------------------------------------------------
NiShaderLibraryDesc* NiD3DShaderLibrary::GetShaderLibraryDesc()
{
    return m_spShaderLibraryDesc;
}
//---------------------------------------------------------------------------
void NiD3DShaderLibrary::SetShaderLibraryDesc(
    NiShaderLibraryDesc* pkLibDesc)
{
    m_spShaderLibraryDesc = pkLibDesc;
}
//---------------------------------------------------------------------------
