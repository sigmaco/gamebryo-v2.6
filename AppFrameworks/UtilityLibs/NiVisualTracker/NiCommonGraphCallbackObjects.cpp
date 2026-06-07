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

// Precmpiled Header
#include "NiVisualTrackerPCH.h"

#include "NiCommonGraphCallbackObjects.h"

//---------------------------------------------------------------------------
MemHighWaterMarkUpdate::MemHighWaterMarkUpdate(float fScalar, 
    NiMemTracker* pkTracker)
{
    m_fScalar = fScalar;
    m_pkTracker = pkTracker;
}
//---------------------------------------------------------------------------
float MemHighWaterMarkUpdate::TakeSample(float)
{
    if (m_pkTracker != NULL)
    {
        return ((float) m_pkTracker->m_stPeakMemory) / 
            m_fScalar;
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
MemCurrentUpdate::MemCurrentUpdate(float fScalar, NiMemTracker* pkTracker)
{
    m_fScalar = fScalar;
    m_pkTracker = pkTracker;
}
//---------------------------------------------------------------------------
float MemCurrentUpdate::TakeSample(float)
{
    if (m_pkTracker != NULL)
    {
        return ((float)m_pkTracker->m_stActiveMemory) / 
            m_fScalar;
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
MemCurrentAllocCountUpdate::MemCurrentAllocCountUpdate(float fScalar, 
    NiMemTracker* pkTracker)
{
    m_fScalar = fScalar;
    m_pkTracker = pkTracker;
}
//---------------------------------------------------------------------------
float MemCurrentAllocCountUpdate::TakeSample(float)
{
    if (m_pkTracker != NULL)
    {
        return ((float) 
            m_pkTracker->m_stActiveAllocationCount) / 
            m_fScalar;
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
FrameRateUpdate::FrameRateUpdate(float *pfFrameRate)
{
   m_pfFrameRate = pfFrameRate;
}
//---------------------------------------------------------------------------
float FrameRateUpdate::TakeSample(float)
{
    if (m_pfFrameRate)
    {
        return *m_pfFrameRate;
    }
    else return 0.0f;
}
//---------------------------------------------------------------------------
VisibleArrayUpdate::VisibleArrayUpdate(NiVisibleArray& kVisArray)
    : m_kVisArray(kVisArray)
{
}
//---------------------------------------------------------------------------
float VisibleArrayUpdate::TakeSample(float)
{
    return (float) m_kVisArray.GetCount();
}
//---------------------------------------------------------------------------
GenericFloatUpdate::GenericFloatUpdate(float fScalar, float* pfValue)
{
    m_fScalar = fScalar;
    m_pfValue = pfValue;
}
//---------------------------------------------------------------------------
float GenericFloatUpdate::TakeSample(float)
{
    if (m_pfValue != NULL)
    {
        return *(m_pfValue) / m_fScalar;
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
GenericUnsignedIntUpdate::GenericUnsignedIntUpdate(float fScalar, 
    unsigned int* puiValue)
{
    m_fScalar = fScalar;
    m_puiValue = puiValue;
}
//---------------------------------------------------------------------------
float GenericUnsignedIntUpdate::TakeSample(float)
{
    if (m_puiValue != NULL)
    {
        return ((float)*(m_puiValue)) / m_fScalar;
    }
    else
    {
        return 0.0f;
    }
}
//---------------------------------------------------------------------------
