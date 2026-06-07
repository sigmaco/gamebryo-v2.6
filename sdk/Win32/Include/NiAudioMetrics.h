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

#ifndef NIAUDIOMETRICS_H
#define NIAUDIOMETRICS_H

#include "NiAudioLibType.h"
#include <NiMetricsLayer.h>

#define NIMETRICS_AUDIO_PREFIX NIMETRICS_PREFIX "audio."

class NIAUDIO_ENTRY NiAudioMetrics
{
public:
    enum NiMetric
    {
        // Total sources loaded (polled once per update call)
        LOADED_SOURCES,
        // Number of loops each source is played per call to Play()
        PLAY_SOUND_LOOPS,

        NUM_METRICS
    };

    inline static void RecordMetrics();
    inline static void AddToLoadedSources(int iNum);

    const static char ms_acNames[NUM_METRICS][NIMETRICS_NAMELENGTH];

protected:
    static unsigned int ms_uiLoadedSources;

};

#if NIMETRICS
    #define NIMETRICS_AUDIO_ADDVALUE(eMetric, fValue) \
        NIMETRICS_ADDVALUE(NiAudioMetrics::ms_acNames[ \
            NiAudioMetrics::eMetric], fValue);
    #define NIMETRICS_AUDIO_RECORDMETRICS() \
        NiAudioMetrics::RecordMetrics();
    #define NIMETRICS_AUDIO_ADDTOLOADEDSOURCES(iNum) \
        NiAudioMetrics::AddToLoadedSources((int)iNum);
#else //#if NIMETRICS
    #define NIMETRICS_AUDIO_ADDVALUE(eMetric, fValue)
    #define NIMETRICS_AUDIO_RECORDMETRICS()
    #define NIMETRICS_AUDIO_ADDTOLOADEDSOURCES(iNum)
#endif //#if NIMETRICS

#include "NiAudioMetrics.inl"

#endif //#ifndef NIAUDIOMETRICS_H
