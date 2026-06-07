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

#ifndef NISYSTEMDESC_H
#define NISYSTEMDESC_H

#include "NiMemObject.h"
#include "NiSystemLibType.h"
#include "NiRTLib.h"

class NISYSTEM_ENTRY NiSystemDesc : public NiMemObject
{
public:
    static const NiSystemDesc& GetSystemDesc();

    typedef enum
    {
        RENDERER_XENON = 0,
        RENDERER_PS3,
        RENDERER_DX9,
        RENDERER_D3D10,
        RENDERER_WII,
        // Generic renderer is used for tool mode so that assets can be
        // generated independent of the current renderer.
        RENDERER_GENERIC, 
        RENDERER_NUM = RENDERER_GENERIC
    } RendererID;

    typedef enum
    {
        NI_WIN32 = 0,
        NI_XENON,
        NI_PS3,
        NI_WII,
        NI_NUM_PLATFORM_IDS
    } PlatformID;

    /// Returns a string representation of the RendererID
    static const char* GetRendererString(const RendererID eRenderer);
    /// Returns the RendererID for the string. RENDERER_GENERIC is returned
    /// if no renderers match this string.
    static RendererID GetRendererID(const char* pcRendererName);

    /// Returns a string representation of the PlatformID
    static const char* GetPlatformString(PlatformID e);
    /// Returns the PlatformID for the string. -1 is returned
    /// if no platforms match this string.
    static PlatformID GetPlatformID(const char* pString);

    /// Returns true if the specified platform is little-endian
    static bool IsPlatformLittleEndian(PlatformID ePlatform);

    /// Returns true if the specified renderer is little-endian
    static bool IsRendererLittleEndian(RendererID eRenderer);

    /// The following functions ignore tool-mode and return information about
    /// the current runtime system
    inline bool IsLittleEndian() const;

    unsigned int GetPhysicalProcessorCount() const;
    unsigned int GetPhysicalCoreCount() const;
    unsigned int GetLogicalProcessorCount() const;

    PlatformID GetPlatformID() const;
    inline float GetPerformanceCounterHz() const;

                   
    // Currently these are only used by DX9 and D3D10.
#if defined(WIN32) 
    bool MMX_Supported(void) const { return m_bMMX_Supported; }
    bool SSE_Supported(void) const { return m_bSSE_Supported; }
    bool SSE2_Supported(void) const { return m_bSSE2_Supported; }
#endif

    // Handle tool mode renderer
    inline bool GetToolMode() const;
    inline void SetToolMode(const bool bToolMode) const;
    RendererID GetToolModeRendererID() const;
    inline void SetToolModeRendererID(const RendererID eRendererID) const;

    bool GetToolModeRendererIsLittleEndian() const;

    static void InitSystemDesc();
    static void ShutdownSystemDesc();

protected:
    NiSystemDesc();
    NiSystemDesc(const NiSystemDesc&);

    static NiSystemDesc* ms_pkSystemDesc;

#ifdef WIN32
    unsigned int CPUID_Init();
    unsigned int CPUID_CpuIDSupported(void);
    unsigned int CPUID_GenuineIntel(void);
    unsigned int CPUID_HWD_MTSupported(void);
    unsigned int CPUID_MaxLogicalProcPerPhysicalProc(void);
    unsigned int CPUID_MaxCorePerPhysicalProc(void);
    unsigned int CPUID_Find_Maskwidth(unsigned int);
    unsigned char CPUID_GetAPIC_ID(void);
    unsigned char CPUID_GetNzbSubID(unsigned char,
                              unsigned char,
                              unsigned char);
    unsigned char CPUID_CPUCount(unsigned int *,
                           unsigned int *,
                           unsigned int *, unsigned int *,
                           unsigned int);
    unsigned int CPUID_QueryCacheType(unsigned int Index);
    unsigned int CPUID_CheckCPU_ExtendedFamilyModel(void);
#endif
    unsigned int m_uiNumPhysicalProcessors;
    unsigned int m_uiNumPhysicalCores;
    unsigned int m_uiNumLogicalProcessors;

    float m_fPCCyclesPerSecond;
  
    // Currently these are only used by DX9 and D3D10.
#if defined(WIN32)

    unsigned int CPUID_MMX_Supported(void);
    unsigned int CPUID_SSE_Supported(void);
    unsigned int CPUID_SSE2_Supported(void);

    bool m_bSSE_Supported;
    bool m_bMMX_Supported;
    bool m_bSSE2_Supported;
#endif

    // Declared mutable so they can be modified using the constant function
    // GetSystemDesc()
    mutable bool m_bToolMode;
    mutable RendererID m_eToolModeRendererID;
};

#include "NiSystemDesc.inl"

#endif // NISYSTEMDESC_H

