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

#include "NiMetricsOutputPCH.h"
#include "NiVisualTrackerOutput.h"
#include "NiCommonGraphCallbackObjects.h"
//---------------------------------------------------------------------------
NiVisualTrackerOutput::NiVisualTrackerOutput(unsigned int uiFramePeriod) :
    NiCalculatingOutput(uiFramePeriod),
    m_kNameMap(10)
{
}
//---------------------------------------------------------------------------
NiVisualTrackerOutput::~NiVisualTrackerOutput()
{
    // No need to try to release these, as NiVisualTracker will handle
    // all remaining graphs and NiCalculatingOutput will handle any
    // remaining registered metrics.  So, just clean up memory.
    for (unsigned int i = 0; i < m_kNameMap.GetSize(); i++)
    {
        NiFree(m_kNameMap.GetAt(i));
    }
}
//---------------------------------------------------------------------------
void NiVisualTrackerOutput::AddGraph(NiVisualTracker* pkTracker, 
    const char* pcMetricName, MetricsFunction eFunction,  
    const NiColor& kColor, unsigned int uiNumSamplesToKeep, 
    float fMinSampleTime, bool bShow, float fScale,
    bool bAddFunctionSuffix, const char* pcAlternateName)
{
    // Register with parent class that we want to listen for this metric
    MetricContainer* pkCont = RegisterMetric(pcMetricName);

    // Adding a graph becomes much more complicated if we allow for users
    // to have custom names for graphs (which is useful if you ever want
    // to have the same metric on the same graph, but with a different
    // function or if you want to have suffixes on the metric names).
    // This is because removal of a graph requires the name
    // of the graph itself (and the unsigned int index returned by
    // adding a graph is not valid after removing one) and so we need
    // to keep a mapping between tracker/oldname/function and
    // newname.  Yuck.

    // if graph name is different than metric name, then allocate new name
    // because we need to keep it around.
    const char* pcGraphName;
    if (pcAlternateName || bAddFunctionSuffix)
    {
        char acBuffer[256];
        const char* pcBase = pcAlternateName ? pcAlternateName : pcMetricName;
        NiStrcpy(acBuffer, sizeof(acBuffer), pcBase);
        if (bAddFunctionSuffix)
            NiStrcat(acBuffer, sizeof(acBuffer), GetName(eFunction));

        pcGraphName = acBuffer;
    }
    else
    {
        pcGraphName = pcMetricName;
    }
    
    unsigned int uiG = pkTracker->AddGraph(NiNew GenericFloatUpdate(fScale, 
        pkCont->GetPointerToValue(eFunction)), pcGraphName, kColor,
        uiNumSamplesToKeep, fMinSampleTime, bShow);

    NameMapping* pkMap = NiAlloc(NameMapping, 1);
    pkMap->m_eFunction = eFunction;
    pkMap->m_pcMetricName = pcMetricName;
    pkMap->m_pkTracker = pkTracker;
    pkMap->m_pcNewName = pkTracker->GetName(uiG);
    m_kNameMap.Add(pkMap);
}
//---------------------------------------------------------------------------
bool NiVisualTrackerOutput::RemoveGraph(NiVisualTracker* pkTracker, 
    const char* pcMetricName, MetricsFunction eFunction)
{
    // search for name mapping to remove
    for (unsigned int i = 0; i < m_kNameMap.GetSize(); i++)
    {
        NameMapping* pkM = m_kNameMap.GetAt(i);
        if (pkM->m_eFunction == eFunction && pkM->m_pkTracker == pkTracker
            && !strcmp(pcMetricName, pkM->m_pcMetricName))
        {
            pkTracker->RemoveGraph(pkM->m_pcNewName);
            UnregisterMetric(pcMetricName);
            return true;
        }
    }

    NIASSERT(!"Removing invalid graph");
    return false;
}
//---------------------------------------------------------------------------
