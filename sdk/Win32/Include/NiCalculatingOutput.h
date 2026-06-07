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

#ifndef NICALCULATINGOUTPUT_H
#define NICALCULATINGOUTPUT_H

#include "NiMetricsOutputLibType.h"
#include <NiMetricsOutputModule.h>
#include <NiTStringMap.h>

// This class does not serve as a metrics output module by itself, but
// serves as an intermediate for any output modules that need to display
// a metric name and a single value associated with it.
//
// It is assumed that metric names point to data that will
// be present over the course of this output module's lifetime.  In
// most cases, the name will simply be a pointer to static data.
//
// Names can be registered multiple times and will return the same
// MetricContainer in each case.  They should be Unregistered an equal
// number of times.

class NIMETRICSOUTPUT_ENTRY NiCalculatingOutput : public NiMetricsOutputModule
{
public:
    enum MetricsFunction
    {
        FUNC_MIN,
        FUNC_MAX,
        // sum over all of the frames
        FUNC_SUM,
        // sum per frame (averaged)
        FUNC_SUM_PER_FRAME,
        FUNC_MEAN,
        FUNC_COUNT,
        NUM_FUNCS
    };

    // mapping of names to the above enumeration
    enum
    {
        FUNC_STRING_LENGTH = 9
    };
    static const char ms_aacFuncNames[NUM_FUNCS][FUNC_STRING_LENGTH];
    static const char* GetName(MetricsFunction eFunc);

protected:
    // MetricContainer is a utility class to handle light calculations of a 
    // metric over some set of frames.
    //
    // This is a RefObject, but the IncRef() and DecRef() are handled
    // manually because there will be only one pointer per
    // metric container.  It will allow for the same container to be used
    // multiple times.
    class NIMETRICSOUTPUT_ENTRY MetricContainer : public NiRefObject
    {
    public:
        MetricContainer(const char* pcName);
        ~MetricContainer();

        // Get a pointer to the calculated data that is represented by
        // this function.  Return NULL if eFunc is invalid.  This pointer
        // is always to the calculated data and so will never return
        // an intermediate value.
        float* GetPointerToValue(MetricsFunction eFunc);
        // Get the calculated data itself
        float GetValue(MetricsFunction eFunc);
        // Add a value for this metric on this frame.
        void AddValue(float fValue);
        // Store the current calculated values of this metric and reset
        // the temporaries so that they can be calculated for the next frame.
        // uiFramePeriod must be greater than zero and represents the number
        // of frames this bucket was captured over.
        void StoreCurrent(unsigned int uiFramePeriod);

        const char* GetName();

    protected:
        // Identifiers
        const char* m_pcName;

        // Temporaries for current calculations
        float m_fCurrentSum;
        unsigned int m_uiSamples;
        float m_fCurrentMin;
        float m_fCurrentMax;

        // Stored values for outside access
        float m_fStoredMin;
        float m_fStoredMax;
        float m_fStoredSum;
        float m_fStoredFSum;
        float m_fStoredMean;
        float m_fStoredCount;
    };

public:

    NiCalculatingOutput(unsigned int uiFramePeriod, 
        unsigned int uiHashSize = 127);
    virtual ~NiCalculatingOutput();

    // NiMetricsOutputModule interface
    virtual void AddValue(const char* pcName, float fValue);
    inline virtual void StartTimer(NiMetricsTimer&) {};
    virtual void EndTimer(NiMetricsTimer& kTimer);
    virtual void AddImmediateEvent(const char* pcName, float fValue);
    // Calculate values if frame period correct
    virtual void Update();

    void CalculateValues();

    inline void SetFramePeriod(unsigned int uiFramePeriod);
    inline unsigned int GetFramePeriod();


protected:
    // Register this metric and return a pointer to the metric container
    // that contains it.  For use only by derived classes.  If it the metric
    // name has already been registered, the same container will be returned
    // and its reference count incremented.  Pointers to MetricContainer*
    // should not be deleted by subclasses or used outside of those
    // subclasses.
    MetricContainer* RegisterMetric(const char* pcName);
    // Unregister this metric and no longer calculate it.
    void UnregisterMetric(const char* pcName);
    // returns NULL if not found
    MetricContainer* FindMetric(const char* pcName);
    // if not found, register and return new container
    MetricContainer* FindOrRegisterMetric(const char* pcName);

    NiTStringMap<MetricContainer*> m_kMetrics;

    unsigned int m_uiFrameCount;
    unsigned int m_uiFramePeriod;
};

#include "NiCalculatingOutput.inl"

#endif //#ifndef NICALCULATINGOUTPUT_H
