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

#ifndef NIMETRICSLAYER_H
#define NIMETRICSLAYER_H

#include "NiVersion.h"

// Prepend all metric names with gb (to differentiate from games).  Also, with
// the configuration and platform so that different metrics are not
// automatically conflated.

#if defined(WIN32)
    #if (_MSC_VER == 1500) //VC9.0
        #define NI_PLATFORM_NAME "Win32VC90"
    #elif (_MSC_VER == 1400) //VC8.0
        #define NI_PLATFORM_NAME "Win32VC80"
    #elif (_MSC_VER == 1310) //VC7.1
        #define NI_PLATFORM_NAME "Win32VC71"
    #else
        #error Unsupported version of Visual Studio
    #endif
#elif defined(_XENON)
    #define NI_PLATFORM_NAME "Xenon"
#elif defined(_PS3)
    #define NI_PLATFORM_NAME "PS3"
#elif defined(_WII)
    #define NI_PLATFORM_NAME "Wii"
#else
    #define NI_PLATFORM_NAME ""
#endif

#if defined(NIDEBUG)
    #define NI_CONFIG_NAME "Debug"
#elif defined(NIRELEASE)
    #define NI_CONFIG_NAME "Release"
#elif defined(NISHIPPING)
    #define NI_CONFIG_NAME "Shipping"
#else
    #define NI_CONFIG_NAME ""
#endif

#define NIMETRICS_PREFIX "gb" NI_PLATFORM_NAME NI_CONFIG_NAME "."

#define NIMETRICS_NAMELENGTH 256

// Convenience macros
// These macros should be used rather than calling
// NiMetricsLayer directly so that, in non-metrics builds, all of the metrics
// gathering code can be easily compiled out.
//
// AddValue and AddEvent are the most frequently used in instrumented code.
// StartTimer/EndTimer are called automatically by NiMetricsTimers.
// PreDisplay/Update/Flush are all called at the application level.
#if NIMETRICS
    #define NIMETRICS_ADDMODULE(pkModule) NiMetricsLayer::AddOutputModule( \
        pkModule)
    #define NIMETRICS_REMOVEMODULE(pkModule) \
        NiMetricsLayer::RemoveOutputModule(pkModule);
    #define NIMETRICS_ADDVALUE(pcName, fValue) NiMetricsLayer::AddValue( \
        (pcName), (float)(fValue));
    #define NIMETRICS_STARTTIMER(kTimer) NiMetricsLayer::StartTimer( \
        kTimer); (kTimer).Start();
    #define NIMETRICS_ENDTIMER(kTimer) (kTimer).Stop(); \
        NiMetricsLayer::EndTimer(kTimer);
    #define NIMETRICS_ADDEVENT(pcName, fValue) \
        NiMetricsLayer::AddImmediateEvent((pcName), (float)(fValue));
    #define NIMETRICS_UPDATE() NiMetricsLayer::Update();
    #define NIMETRICS_EVAL(x) x
#else //#if NIMETRICS
    #define NIMETRICS_ADDMODULE(pkModule)
    #define NIMETRICS_REMOVEMODULE(pkModule)
    #define NIMETRICS_ADDVALUE(pcName, fValue)
    #define NIMETRICS_STARTTIMER(kTimer)
    #define NIMETRICS_ENDTIMER(kTimer)
    #define NIMETRICS_ADDEVENT(pcName, fValue)
    #define NIMETRICS_UPDATE()
    #define NIMETRICS_EVAL(x)
#endif //#if NIMETRICS

#include "NiTArray.h"
#include "NiMetricsOutputModule.h"

// Forward declared to avoid circular reference.
class NiMetricsTimer;

class NIMAIN_ENTRY NiMetricsLayer
{
public:
    static void _SDMInit();
    static void _SDMShutdown();

    static void AddOutputModule(NiMetricsOutputModule* pkMod);
    static void RemoveOutputModule(NiMetricsOutputModule* pkMod);

    // Output Module Functions

    // pcName must not be NULL
    static void AddValue(const char* pcName, float fValue);
    static void StartTimer(NiMetricsTimer& kTimer);
    static void EndTimer(NiMetricsTimer& kTimer);
    static void AddImmediateEvent(const char* pcName, float fValue);
    static void Update();

protected:
    static NiTObjectArray<NiMetricsOutputModulePtr> ms_kOutputs;
    
    // Lock for all of the static functions to avoid adding/removing a module
    // while calling an output function or calling two output functions at
    // once.
    static NiFastCriticalSection ms_kMutex;
};

#include "NiMetricsLayer.inl"

#endif //#ifndef NIMETRICSLAYER_H
