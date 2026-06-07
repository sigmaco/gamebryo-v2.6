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
#include "NiSystemPCH.h"

#include "NiSystemDesc.h"
#include <Windows.h>

//---------------------------------------------------------------------------
NiSystemDesc* NiSystemDesc::ms_pkSystemDesc = NULL;
//---------------------------------------------------------------------------
void NiSystemDesc::InitSystemDesc()
{
    NIASSERT(ms_pkSystemDesc == NULL);
    ms_pkSystemDesc = NiNew NiSystemDesc();
}
//---------------------------------------------------------------------------
void NiSystemDesc::ShutdownSystemDesc()
{
    NiDelete ms_pkSystemDesc;
    ms_pkSystemDesc = NULL;
}
//---------------------------------------------------------------------------
NiSystemDesc::NiSystemDesc()
{
    
    m_bSSE_Supported = false;
    m_bMMX_Supported = false;
    m_bSSE2_Supported = false;

    // CPUID code is not 64bit safe, ensure we're running in a Win32 platform.
    // Otherwise, we'll just use the SYSTEM_INFO struct.
#ifdef WIN32
    // CPUID_Init, if successful, will fill in m_uiNum variables
    if(CPUID_Init() != 0)
#endif
    {
        SYSTEM_INFO kInfo;
        GetSystemInfo(&kInfo);
        m_uiNumPhysicalProcessors = 1;
        m_uiNumPhysicalCores = kInfo.dwNumberOfProcessors;
        m_uiNumLogicalProcessors = kInfo.dwNumberOfProcessors;  
        m_bSSE_Supported = (CPUID_SSE_Supported() != 0);
        m_bMMX_Supported = (CPUID_MMX_Supported() != 0);
        m_bSSE2_Supported = (CPUID_SSE2_Supported() != 0);
    }
    

    LARGE_INTEGER ticksPerSec;
    QueryPerformanceFrequency(&ticksPerSec);
    m_fPCCyclesPerSecond = (float)ticksPerSec.QuadPart;
    
    m_bToolMode = false;
    m_eToolModeRendererID = RENDERER_GENERIC;
}
//---------------------------------------------------------------------------
NiSystemDesc::NiSystemDesc(const NiSystemDesc&)
{
    NIASSERT(!"This method should never be called");
}
//---------------------------------------------------------------------------
const char* NiSystemDesc::GetRendererString(
    const NiSystemDesc::RendererID eRenderer)
{
    switch(eRenderer)
    {
    case NiSystemDesc::RENDERER_PS3:
        return "PS3";
    case NiSystemDesc::RENDERER_XENON:
        return "XENON";
    case NiSystemDesc::RENDERER_DX9:
        return "DX9";
    case NiSystemDesc::RENDERER_D3D10:
        return "D3D10";
    case NiSystemDesc::RENDERER_WII:
        return "WII";
    case NiSystemDesc::RENDERER_GENERIC:
        return "Generic";
    default:
        break;
    }

    return "Unknown";
}
//---------------------------------------------------------------------------
NiSystemDesc::RendererID NiSystemDesc::GetRendererID(
    const char* pcRendererName)
{
    for (unsigned int ui = 0; ui < NiSystemDesc::RENDERER_NUM; ui++)
    {
        if (NiStricmp(pcRendererName, GetRendererString(
            (NiSystemDesc::RendererID)ui)) == 0)
        {
            return (NiSystemDesc::RendererID)ui;
        }
    }

    return NiSystemDesc::RENDERER_GENERIC;
}
//---------------------------------------------------------------------------
const char* NiSystemDesc::GetPlatformString(PlatformID e)
{
    switch(e)
    {
    case NI_WIN32:
        return "WIN32";
    case NI_XENON:
        return "XENON";
    case NI_PS3:
        return "PS3";
    case NI_WII:
        return "WII";
    default:
        return "Unknown";
    }
}
//---------------------------------------------------------------------------
NiSystemDesc::PlatformID NiSystemDesc::GetPlatformID(
    const char* pString)
{
    for (unsigned int ui = 0; ui < NI_NUM_PLATFORM_IDS; ui++)
    {
        if (NiStricmp(pString, GetPlatformString(
            (NiSystemDesc::PlatformID)ui)) == 0)
        {
            return (NiSystemDesc::PlatformID)ui;
        }
    }
    return (NiSystemDesc::PlatformID)-1;
}
//---------------------------------------------------------------------------
bool NiSystemDesc::IsPlatformLittleEndian(PlatformID ePlatform)
{
    switch (ePlatform)
    {
    case NI_WIN32:
        return true;
    case NI_XENON:
    case NI_PS3:
    case NI_WII:
        return false;
    default:
        NIASSERT(false && "Unknown platform");
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiSystemDesc::IsRendererLittleEndian(RendererID eRenderer)
{
    switch (eRenderer)
    {
    case RENDERER_XENON:
    case RENDERER_PS3:
    case RENDERER_WII:
        return false;
    case RENDERER_DX9:
    case RENDERER_D3D10:
    case RENDERER_GENERIC:
        return true;
    default:
        NIASSERT(false && "Unknown platform");
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiSystemDesc::GetToolModeRendererIsLittleEndian() const
{
    return IsRendererLittleEndian(m_eToolModeRendererID);
}
//---------------------------------------------------------------------------
