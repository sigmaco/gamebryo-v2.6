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

//---------------------------------------------------------------------------
inline float* NiCalculatingOutput::MetricContainer::GetPointerToValue(
    MetricsFunction eFunc)
{
    switch(eFunc)
    {
    case FUNC_MIN:
        return &m_fStoredMin;
    case FUNC_MAX:
        return &m_fStoredMax;
    case FUNC_SUM:
        return &m_fStoredSum;
    case FUNC_SUM_PER_FRAME:
        return &m_fStoredFSum;
    case FUNC_MEAN:
        return &m_fStoredMean;
    case FUNC_COUNT:
        return &m_fStoredCount;
    default:
        NIASSERT(!"Invalid function.");
        return NULL;
    };
}
//---------------------------------------------------------------------------
inline float NiCalculatingOutput::MetricContainer::GetValue(
    MetricsFunction eFunc)
{
    switch(eFunc)
    {
    case FUNC_MIN:
        return m_fStoredMin;
    case FUNC_MAX:
        return m_fStoredMax;
    case FUNC_SUM:
        return m_fStoredSum;
    case FUNC_SUM_PER_FRAME:
        return m_fStoredFSum;
    case FUNC_MEAN:
        return m_fStoredMean;
    case FUNC_COUNT:
        return m_fStoredCount;
    default:
        NIASSERT(!"Invalid function.");
        return 0.0f;
    };
}
//---------------------------------------------------------------------------
inline const char* NiCalculatingOutput::MetricContainer::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
inline void NiCalculatingOutput::MetricContainer::AddValue(float fValue)
{
    if (!m_uiSamples)
    {
        m_fCurrentMin = m_fCurrentMax = m_fCurrentSum = fValue;
    }
    else
    {
        m_fCurrentSum += fValue;
        m_fCurrentMax = (fValue > m_fCurrentMax) ? fValue : m_fCurrentMax;
        m_fCurrentMin = (fValue < m_fCurrentMin) ? fValue : m_fCurrentMin;
    }

    m_uiSamples++;
}
//---------------------------------------------------------------------------
inline void NiCalculatingOutput::MetricContainer::StoreCurrent(unsigned int
    uiFramePeriod)
{
    m_fStoredSum = m_fCurrentSum;
    m_fStoredMin = m_fCurrentMin;
    m_fStoredMax = m_fCurrentMax;
    m_fStoredCount = (float)m_uiSamples;
    m_fStoredFSum = m_fCurrentSum / (float)uiFramePeriod;

    // Do something reasonable if trying to divide by zero
    m_fStoredMean = (m_uiSamples > 0) ? m_fStoredSum / m_fStoredCount : 0.0f;

    // This is sufficient to reset min, max, and sum on the next AddValue
    // call.  This will mean that metrics with a count of 0 will have a
    // stale min/max/sum value lying around if they don't happen to get
    // updated, but those values would have been bogus anyway.
    m_uiSamples = 0;
}
//---------------------------------------------------------------------------
inline const char* NiCalculatingOutput::GetName(MetricsFunction eFunc)
{
    return ms_aacFuncNames[eFunc];
}
//---------------------------------------------------------------------------
inline void NiCalculatingOutput::SetFramePeriod(unsigned int uiFramePeriod)
{
    m_uiFramePeriod = uiFramePeriod;
}
//---------------------------------------------------------------------------
inline unsigned int NiCalculatingOutput::GetFramePeriod()
{
    return m_uiFramePeriod;
}
//---------------------------------------------------------------------------
