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
// NiThread inline functions
//---------------------------------------------------------------------------

#include <NiSystem.h>

//---------------------------------------------------------------------------
inline NiThread* NiThread::GetThread()
{
    return ms_kTLSCurrentThread;
}
//---------------------------------------------------------------------------
inline NiThreadProcedure* NiThread::GetProcedure() const
{
    return m_pkProcedure;
}
//---------------------------------------------------------------------------
inline void NiThread::SetStackSize(unsigned int uiStackSize)
{
    if (uiStackSize == USEDEF_STACK_SIZE)
        m_uiStackSize = DEFAULT_STACK_SIZE;
    else
        m_uiStackSize = uiStackSize;
}
//---------------------------------------------------------------------------
inline void NiThread::SetProcedure(NiThreadProcedure* pkProcedure)
{
    m_pkProcedure = pkProcedure;
}
//---------------------------------------------------------------------------
inline const NiThread::Priority NiThread::GetPriority() const
{
    return m_ePriority;
}
//---------------------------------------------------------------------------
inline bool NiThread::SetThreadAffinity(const NiProcessorAffinity& kAffinity)
{
    if (SystemSetAffinity(kAffinity))
    {
        m_kAffinity.SetAffinityMask(kAffinity.GetAffinityMask());
        m_kAffinity.SetIdealProcessor(kAffinity.GetIdealProcessor());
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline const NiProcessorAffinity NiThread::GetThreadAffinity()
{
    return NiProcessorAffinity(m_kAffinity.GetIdealProcessor(),
        m_kAffinity.GetAffinityMask());
}
//---------------------------------------------------------------------------
inline const NiThread::Status NiThread::GetStatus() const
{
    return m_eStatus;
}
//---------------------------------------------------------------------------
inline const unsigned int NiThread::GetReturnValue() const
{
    return m_uiReturnValue;
}
//---------------------------------------------------------------------------
inline void NiThread::SetName(const char* pcName)
{
    NiFree(m_pcName);

    if (pcName && pcName[0] != '\0')
    {
        unsigned int uiStrLen = (unsigned int)strlen(pcName);
        m_pcName = NiAlloc(char, uiStrLen + 1);
        NIASSERT(m_pcName);
        NiStrcpy(m_pcName, uiStrLen + 1, pcName);
    }
}
//---------------------------------------------------------------------------
inline const char* NiThread::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
