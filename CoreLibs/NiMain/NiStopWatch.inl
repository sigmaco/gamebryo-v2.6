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

// NiStopWatch inline files

#include "NiSystem.h"

//---------------------------------------------------------------------------
inline NiStopWatch::NiStopWatch() :
m_uiCount(0), m_uiTime(0), m_uiHoldTime(0), m_bHold(false)
{
    /* */
}
//---------------------------------------------------------------------------
inline void NiStopWatch::Start()
{
    if (m_uiCount == 0)
        m_uiTime = NiGetPerformanceCounter();
    m_uiCount++;
}
//---------------------------------------------------------------------------
inline void NiStopWatch::Stop()
{
    NIASSERT(m_uiCount > 0);
    m_uiCount--;

    if (m_uiCount == 0)
    {
        m_uiTime = NiGetPerformanceCounter() - m_uiTime;
    }
}
//---------------------------------------------------------------------------
inline void NiStopWatch::Reset()
{
    m_uiCount = m_uiTime = 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiStopWatch::GetTime() const
{
    // Timing data is not relevant in Debug mode and thus can be misleading.
#ifdef NIDEBUG
    NIASSERT(m_uiCount == 0);
    return 1;
#else
    return m_uiTime;
#endif
}
//---------------------------------------------------------------------------
inline float NiStopWatch::GetTimeInSeconds() const
{
#ifdef NIDEBUG
    NIASSERT(m_uiCount == 0);
    return 1.0f;
#else
    return GetTime() / 
        NiSystemDesc::GetSystemDesc().GetPerformanceCounterHz();
#endif
}
//---------------------------------------------------------------------------
inline unsigned int NiStopWatch::GetCount() const
{
    return m_uiCount;
}
//---------------------------------------------------------------------------
inline void NiStopWatch::Hold()
{
    m_uiHoldTime = NiGetPerformanceCounter();
    m_bHold = true;
}
//---------------------------------------------------------------------------
inline void NiStopWatch::Unhold()
{
    NIASSERT(m_bHold);

    // add hold time to start time
    m_uiTime += NiGetPerformanceCounter() - m_uiHoldTime;
    m_uiHoldTime = 0;
    m_bHold = false;
}
//---------------------------------------------------------------------------
