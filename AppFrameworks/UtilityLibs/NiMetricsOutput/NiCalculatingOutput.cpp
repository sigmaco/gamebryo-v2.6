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
#include "NiCalculatingOutput.h"
#include "NiMetricsTimer.h"

const char NiCalculatingOutput::ms_aacFuncNames[NUM_FUNCS][FUNC_STRING_LENGTH] 
    =
{
    "Min",
    "Max",
    "Sum",
    "FrameSum",
    "Mean",
    "Count"
};

//---------------------------------------------------------------------------
NiCalculatingOutput::NiCalculatingOutput(unsigned int uiFramePeriod,
    unsigned int uiHashSize) :
    m_kMetrics(uiHashSize, false),
    m_uiFrameCount(0), m_uiFramePeriod(uiFramePeriod)
{
    NIASSERT(uiFramePeriod > 0);
}
//---------------------------------------------------------------------------
NiCalculatingOutput::~NiCalculatingOutput()
{
    // Clean up any remaining metric containers
    NiTMapIterator kIter = m_kMetrics.GetFirstPos();

    while(kIter)
    {
        const char* pcName;
        MetricContainer* pkCont;
        m_kMetrics.GetNext(kIter, pcName, pkCont);
        NiDelete pkCont;
    }
}
//---------------------------------------------------------------------------
NiCalculatingOutput::MetricContainer* NiCalculatingOutput::RegisterMetric(
    const char* pcName)
{
    // Search for previously existing metric
    MetricContainer* pkCont;
    if (!m_kMetrics.GetAt(pcName, pkCont))
    {
        pkCont = NiNew MetricContainer(pcName);
        m_kMetrics.SetAt(pcName, pkCont);
    }
    pkCont->IncRefCount();
    return pkCont;
}
//---------------------------------------------------------------------------
void NiCalculatingOutput::UnregisterMetric(const char* pcName)
{
    // Search for previously existing metric
    MetricContainer* pkCont;
    if (m_kMetrics.GetAt(pcName, pkCont))
    {
        // Decrement and potentially delete container
        pkCont->DecRefCount();
        if (!pkCont->GetRefCount())
        {
            NiDelete pkCont;
            m_kMetrics.RemoveAt(pcName);
        }
    }
}
//---------------------------------------------------------------------------
void NiCalculatingOutput::AddValue(const char* pcName, float fValue)
{
    MetricContainer* pkCont;
    if (m_kMetrics.GetAt(pcName, pkCont))
    {
        pkCont->AddValue(fValue);
    }
}
//---------------------------------------------------------------------------
NiCalculatingOutput::MetricContainer* NiCalculatingOutput::FindMetric(
    const char* pcName)
{
    MetricContainer* pkCont;
    if (m_kMetrics.GetAt(pcName, pkCont))
    {
        return pkCont;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
NiCalculatingOutput::MetricContainer* NiCalculatingOutput::
    FindOrRegisterMetric(const char* pcName)
{
    MetricContainer* pkCont;
    if (m_kMetrics.GetAt(pcName, pkCont))
    {
        return pkCont;
    }
    else
    {
        pkCont = NiNew MetricContainer(pcName);
        m_kMetrics.SetAt(pcName, pkCont);
        pkCont->IncRefCount();
        return pkCont;
    }
}
//---------------------------------------------------------------------------
void NiCalculatingOutput::EndTimer(NiMetricsTimer& kTimer)
{
    // In this case, EndTimer just maps to AddValue with the elapsed
    // time of the timer.

    AddValue(kTimer.GetName(), kTimer.GetElapsed());
}
//---------------------------------------------------------------------------
void NiCalculatingOutput::AddImmediateEvent(const char* pcName, float fValue)
{
    AddValue(pcName, fValue);
}
//---------------------------------------------------------------------------
void NiCalculatingOutput::Update()
{
    if (++m_uiFrameCount >= m_uiFramePeriod)
    {
        // Calculate values
        CalculateValues();

        m_uiFrameCount = 0;
    }
}
//---------------------------------------------------------------------------
void NiCalculatingOutput::CalculateValues()
{
    NiTMapIterator kIter = m_kMetrics.GetFirstPos();

    while(kIter)
    {
        const char* pcName;
        MetricContainer* pkCont;
        m_kMetrics.GetNext(kIter, pcName, pkCont);
        pkCont->StoreCurrent(m_uiFramePeriod);
    }
}
//---------------------------------------------------------------------------
// MetricContainer nested class implementation
//---------------------------------------------------------------------------
NiCalculatingOutput::MetricContainer::MetricContainer(const char* pcName) :
    m_pcName(pcName),
    m_fCurrentSum(0.0f),
    m_uiSamples(0),
    m_fCurrentMin(0.0f),
    m_fCurrentMax(0.0f),
    m_fStoredMin(0.0f),
    m_fStoredMax(0.0f),
    m_fStoredSum(0.0f),
    m_fStoredMean(0.0f),
    m_fStoredCount(0.0f)
{

}
//---------------------------------------------------------------------------
NiCalculatingOutput::MetricContainer::~MetricContainer()
{
}
//---------------------------------------------------------------------------
