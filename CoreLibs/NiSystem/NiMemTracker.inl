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

#include "NiSystem.h"

//---------------------------------------------------------------------------
#if defined(NISYSTEM_EXPORT) || defined(NISYSTEM_IMPORT)
// DLL builds
//---------------------------------------------------------------------------
inline NiFLF::NiFLF(const char* pcFile, unsigned int uiLine, 
    const char* pcFunc)
{
    Set(pcFile, uiLine, pcFunc);
}
//---------------------------------------------------------------------------
inline void NiFLF::Set(const char* pcFile, unsigned int uiLine, 
    const char* pcFunc)
{
    if (pcFile != NULL)
    {
        size_t stStrLen = strlen(pcFile);
        if (stStrLen >= NI_FLF_FILE_LENGTH)
        {
            pcFile = pcFile + stStrLen - (NI_FLF_FILE_LENGTH - 1);
        }

        NiStrcpy(m_pcFile, NI_FLF_FILE_LENGTH, pcFile);
    }
    else
    {
        m_pcFile[0] = '\0';
    }

    m_uiLine = uiLine;

    if (pcFunc != NULL)
    {
        size_t stStrLen = strlen(pcFunc);
        if (stStrLen >= NI_FLF_FUNCTION_LENGTH)
        {
            pcFunc = pcFunc + stStrLen - (NI_FLF_FUNCTION_LENGTH - 1);
        }

        NiStrcpy(m_pcFunc, NI_FLF_FUNCTION_LENGTH, pcFunc);
    }
    else
    {
        m_pcFunc[0] = '\0';
    }
}
//---------------------------------------------------------------------------
inline void NiFLF::Reset()
{
    Set(NiFLF::UNKNOWN.m_pcFile, 
        NiFLF::UNKNOWN.m_uiLine,
        NiFLF::UNKNOWN.m_pcFunc);
}
#else // Non-DLL builds
//---------------------------------------------------------------------------
inline NiFLF::NiFLF(const char* pcFile, unsigned int uiLine, 
    const char* pcFunc) :
    m_pcFile(pcFile), m_pcFunc(pcFunc), m_uiLine(uiLine) 
{
}
//---------------------------------------------------------------------------
inline void NiFLF::Set(const char* pcFile, unsigned int uiLine, 
    const char* pcFunc)
{
    m_pcFile = pcFile;
    m_uiLine = uiLine;
    m_pcFunc = pcFunc;
}
//---------------------------------------------------------------------------
inline void NiFLF::Reset()
{
    m_pcFile = NiFLF::UNKNOWN.m_pcFile;
    m_uiLine = NiFLF::UNKNOWN.m_uiLine;
    m_pcFunc = NiFLF::UNKNOWN.m_pcFunc;
}
#endif
//---------------------------------------------------------------------------
inline NiFLF::NiFLF()
{
    Reset();
}
//---------------------------------------------------------------------------
inline bool NiFLF::IsValid() const
{
    return (m_uiLine != 0);
}
//---------------------------------------------------------------------------
inline const char* NiFLF::SourceFileStripper() const
{
    const char* pcSlash = strrchr(m_pcFile, '\\');
    if (pcSlash)
        return pcSlash + 1;

    pcSlash = strrchr(m_pcFile, '/');
    if (pcSlash)
        return pcSlash + 1;

    return m_pcFile;
}
//---------------------------------------------------------------------------
inline const char* NiFLF::CreateTempFLFString()
{
    static char acString[90];
    
    memset(acString, 0, sizeof(acString));
    NiSprintf(acString, 90, "%s(%05d)::%s", SourceFileStripper(), 
        m_uiLine, m_pcFunc);
    return acString;
}
//---------------------------------------------------------------------------
inline void NiAllocUnit::Reset()
{
    m_stAllocationID = (size_t) -1;
    m_stAlignment = 0;
    m_ulAllocThreadId = 0;
    m_pvMem = NULL;
    m_stSizeRequested = 0;
    m_stSizeAllocated = 0;
    m_kFLF.Reset();
    m_eAllocType = NI_UNKNOWN;
    m_kAllocHint = NI_MEMHINT_NONE;
    m_fAllocTime = -FLT_MAX;
    m_pkPrev = NULL;
    m_pkNext = NULL;
}
//---------------------------------------------------------------------------
inline bool NiMemTracker::GetAlwaysValidateAll() const
{
    return m_bAlwaysValidateAll;
}
//---------------------------------------------------------------------------
inline void NiMemTracker::SetAlwaysValidateAll(bool bOn)
{
    m_bAlwaysValidateAll = bOn;
}
//---------------------------------------------------------------------------
inline bool NiMemTracker::GetCheckForArrayOverruns() const
{
    return m_bCheckArrayOverruns;
}
//---------------------------------------------------------------------------
inline unsigned int NiMemTracker::AddressToHashIndex(const void* pvAddress)
{
    // Use the address to locate the hash index. Note that we shift off the 
    // lower four bits. This is because most allocated addresses will be on 
    // four-, eight- or even sixteen-byte boundaries. If we didn't do this, 
    // the hash index would not have very good coverage.

    // The size_t may be cast to an unsigned int as long as ms_uiHashSize
    // is less than the largest unsigned int.

    return (unsigned int)(reinterpret_cast<size_t>(pvAddress) >> 4) & 
        (ms_uiHashSize - 1);
}
//---------------------------------------------------------------------------
inline NiMemTracker* NiMemTracker::Get()
{
    return ms_pkTracker;
}
//---------------------------------------------------------------------------
inline size_t NiMemTracker::GetCurrentAllocationID() const
{
    return m_stCurrentAllocID;
}
//---------------------------------------------------------------------------
inline bool NiMemTracker::IsInsideBreakRange(const void* pvMem, size_t stSize)
{
    return ((((const char*)pvMem) + stSize) >= ms_pvBreakOnAllocRangeStart &&
        (pvMem <= ms_pvBreakOnAllocRangeEnd));
}
//---------------------------------------------------------------------------
inline void NiMemTracker::SetWriteToLog(bool bOn)
{
    m_bWriteToLog = bOn;
}
//---------------------------------------------------------------------------
