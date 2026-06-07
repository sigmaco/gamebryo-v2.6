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

#ifndef NIMETRICSTIMER_H
#define NIMETRICSTIMER_H

// This file represents a series of classes to implement NiMetricsTimer.
// End users should use NiMetricsClockTimer and NiMetricsPCTimer to
// time events.
//
// The NiMetricsTimer class can be used to refer to
// any timer and get its information, so long as no timing needs
// to be done with it.  This is what is used internally by the
// NiMetricsLayer and its output modules.
//
// The name in the constructor must point to data that will exist for the 
// duration of the timer's existence.  In general, this will be static data.
//

// NiMetricsTimer is the timer base class without a template that all timers 
// derive from.  Derived classes should set m_fValue appropriately.
class NIMAIN_ENTRY NiMetricsTimer
{
public:
    // pcName must not be NULL
    NiMetricsTimer(const char* pcName);

    inline float GetElapsed() const;
    inline const char* GetName() const;

protected:
    const char* m_pcName;
    float m_fElapsed;
};

// NiTMetricsTimer adds timing abilities to the basic NiMetricsTimer.
// It is typedef'd below to two commonly used types of timers.
// It takes a parameter T to specify the type of Timer to be used.  This is
// because, depending on the length of time that this timer will be used for,
// different precision timers should be used.
//
// The T template parameter must implement these functions:
//  1) float GetElapsed() const
//  2) void Start()
//  3) void Stop()
//  4) T()
template<class T> class NiTMetricsTimer : public NiMetricsTimer
{
public:
    // pcName must not be NULL
    NiTMetricsTimer(const char* pcName);
    ~NiTMetricsTimer();

    // Reset the timer and start timing
    inline void Start();
    // Stop the timer
    inline void Stop();

protected:
    T m_kTimer;
};

// Timer based on the system clock.  Less precise, but longer range.
class NIMAIN_ENTRY NiSystemClockTimer
{
public:
    inline float GetElapsed() const;
    inline void Start();
    inline void Stop();
protected:
    float m_fStart;
    float m_fElapsed;
};

// Timer based on the high frequency performance counter.  More precise,
// but much shorter valid range.
class NIMAIN_ENTRY NiPerformanceCounterTimer
{
public:
    inline float GetElapsed() const;
    inline void Start();
    inline void Stop();
protected:
    unsigned int m_uiStart;
    float m_fElapsed;
};

#include "NiMetricsTimer.inl"

// Typedef'd for convenience
typedef NiTMetricsTimer<NiSystemClockTimer> NiMetricsClockTimer;
typedef NiTMetricsTimer<NiPerformanceCounterTimer> NiMetricsPCTimer;

#endif //#ifndef NIMETRICSTIMER_H
