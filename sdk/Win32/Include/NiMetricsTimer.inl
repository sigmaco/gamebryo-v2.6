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

#include "NiMetricsLayer.h"
#include <NiSystemDesc.h>

//---------------------------------------------------------------------------
// NiSystemClockTimer definition
//---------------------------------------------------------------------------
inline float NiSystemClockTimer::GetElapsed() const
{
    return m_fElapsed;
}
//---------------------------------------------------------------------------
inline void NiSystemClockTimer::Start()
{
    m_fStart = NiGetCurrentTimeInSec();
}
//---------------------------------------------------------------------------
inline void NiSystemClockTimer::Stop()
{
    m_fElapsed = NiGetCurrentTimeInSec() - m_fStart;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// NiPerformanceCounterTimer definition
//---------------------------------------------------------------------------
inline float NiPerformanceCounterTimer::GetElapsed() const
{
    return m_fElapsed;
}
//---------------------------------------------------------------------------
inline void NiPerformanceCounterTimer::Start()
{
    m_uiStart = NiGetPerformanceCounter();
}
//---------------------------------------------------------------------------
inline void NiPerformanceCounterTimer::Stop()
{
    unsigned int uiEnd = NiGetPerformanceCounter();

    m_fElapsed = (uiEnd - m_uiStart) / 
        NiSystemDesc::GetSystemDesc().GetPerformanceCounterHz();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// NiMetricsTimer base class definition
//---------------------------------------------------------------------------
inline NiMetricsTimer::NiMetricsTimer(const char* pcName) :
    m_pcName(pcName),
    m_fElapsed(0.0f)
{
    NIASSERT(pcName);
}
//---------------------------------------------------------------------------
inline const char* NiMetricsTimer::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline float NiMetricsTimer::GetElapsed() const
{
    return m_fElapsed;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// NiMetricsTimerTemp derived templated definition
//---------------------------------------------------------------------------
template<class T>
inline NiTMetricsTimer<T>::NiTMetricsTimer(const char* pcName) :
    NiMetricsTimer(pcName)
{
    
}
//---------------------------------------------------------------------------
template<class T>
inline NiTMetricsTimer<T>::~NiTMetricsTimer()
{

}
//---------------------------------------------------------------------------
template<class T>
inline void NiTMetricsTimer<T>::Start()
{
    m_kTimer.Start();
}
//---------------------------------------------------------------------------
template<class T>
inline void NiTMetricsTimer<T>::Stop()
{
    m_kTimer.Stop();
    m_fElapsed = m_kTimer.GetElapsed();
}
//---------------------------------------------------------------------------
