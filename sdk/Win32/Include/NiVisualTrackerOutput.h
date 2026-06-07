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

#ifndef NIVISUALTRACKEROUTPUT_H
#define NIVISUALTRACKEROUTPUT_H

#include "NiMetricsOutputLibType.h"
#include "NiCalculatingOutput.h"
#include <NiVisualTracker.h>

class NIMETRICSOUTPUT_ENTRY NiVisualTrackerOutput : public NiCalculatingOutput
{
public:
    NiVisualTrackerOutput(unsigned int uiFramePeriod);
    virtual ~NiVisualTrackerOutput();

    // Add a graph to this tracker for a particular metric.  Graphs are
    // (ideally uniquely) determined by their tracker, metric name, and 
    // function.  If an alternate name is specified, it (plus optional
    // suffix) will be used on the graph instead of pcMetricName.
    void AddGraph(NiVisualTracker* pkTracker, const char* pcMetricName, 
        MetricsFunction eFunction = FUNC_MEAN, 
        const NiColor& kColor = NiColor::WHITE, 
        unsigned int uiNumSamplesToKeep = 100, float fMinSampleTime = 0.1f,
        bool bShow = true, float fScale = 1.0f,
        bool bAddFunctionSuffix = true, const char* pcAlternateName = NULL);

    // Remove the graph from this tracker for a particular metric, which
    // is determined by its tracker, metric name, and function.  If this
    // is not unique, it will remove the first one it finds.  Returns true
    // if it found and removed one.
    bool RemoveGraph(NiVisualTracker* pkTracker, const char* pcMetricName,
        MetricsFunction eFunction);

protected:

    // Name mapping for metrics -> graph names.  This is required for proper
    // graph removal.
    struct NameMapping
    {
        // Metric name (key)
        const char* m_pcMetricName;
        // Metric function (key)
        MetricsFunction m_eFunction;
        // Visual Tracker (key)
        NiVisualTracker* m_pkTracker;

        // Graph name (data)
        const char* m_pcNewName;
    };

    NiTPrimitiveSet<NameMapping*> m_kNameMap;
};

#endif //#ifndef NIVISUALTRACKEROUTPUT_H
