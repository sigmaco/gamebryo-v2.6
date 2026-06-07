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

#ifndef NIMETRICSSCOPETIMER_H
#define NIMETRICSSCOPETIMER_H

#include "NiMetricsTimer.h"

// This timer is identical to the NiMetricsTimer, except that it starts
// automatically when it is created.  It is designed to be used as such:
//
// void FunctionToProfile()
// {
//     NiMetricsScopeTimer("mymetricname.functiontoprofile.time");
//     // Do stuff
//     // ...
// 
//     // Timer stops when destroyed at end of scope
// }

template<class T>
class NiTMetricsScopeTimer : public NiTMetricsTimer<T>
{
public:
    NiTMetricsScopeTimer(const char* pcName);
    ~NiTMetricsScopeTimer();
};

#include "NiMetricsScopeTimer.inl"

// typedef'd for convenience
typedef NiTMetricsScopeTimer<NiSystemClockTimer> NiMetricsScopeClockTimer;
typedef NiTMetricsScopeTimer<NiPerformanceCounterTimer> 
    NiMetricsScopePCTimer;

#endif //#ifndef NIMETRICSSCOPETIMER_H
