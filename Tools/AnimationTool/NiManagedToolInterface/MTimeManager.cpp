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

// NifTimeManager.cpp

#include "stdafx.h"
#include "MTimeManager.h"

using namespace NiManagedToolInterface;

// Uncomment the following lines to run MTimeManager with constant time for
// debugging purposes.
//#define USE_CONSTANT_TIME
//#define CONSTANT_TIME_INCREMENT 0.0167f

// Uncomment the following lines to run MTimeManger with a random fix
// time. Will overrided the USE_CONSTANT_TIME flag.
//#define USE_RANDOM_FIX_TIME
//#define SEED_FOR_RANDOM_FIX_TIME 1
//#define RANDOM_FIX_TIME_INCREMENT_HIGH 0.0333f
//#define RANDOM_FIX_TIME_INCREMENT_LOW 0.0167f

//---------------------------------------------------------------------------
MTimeManager::MTimeManager() : m_fAccumTime(0.0f),
    m_fScaleFactor(1.0f), m_fStartTime(0.0f), m_fEndTime(0.0f),
    m_bTimingEnabled(false), m_eTimeMode(CONTINUOUS),
    m_fLastTime(-NI_INFINITY), m_bRunUpEnabled(true)
{
#ifdef USE_RANDOM_CONSTANT_TIME
    srand(SEED_FOR_RANDOM_CONSTANT_TIME);
#endif
}
//---------------------------------------------------------------------------
void MTimeManager::UpdateTime()
{
    if (!m_bTimingEnabled)
    {
        return;
    }

    float fCurrentTime = NiGetCurrentTimeInSec();
    if (m_fLastTime == -NI_INFINITY)
    {
        m_fLastTime = fCurrentTime;
    }

    float fDelta;
#ifdef USE_RANDOM_FIX_TIME
    float fRandomValue = rand() / (float)RAND_MAX;
    float fRandomTime = fRandomValue * 
        (RANDOM_FIX_TIME_INCREMENT_HIGH - RANDOM_FIX_TIME_INCREMENT_LOW) +
        RANDOM_FIX_TIME_INCREMENT_LOW;

    fDelta = fRandomTime * m_fScaleFactor;
#else
#ifdef USE_CONSTANT_TIME
    fDelta = CONSTANT_TIME_INCREMENT * m_fScaleFactor;
#else
    fDelta = fCurrentTime - m_fLastTime;
    fDelta *= m_fScaleFactor;
#endif
#endif

    float fAccumTime = m_fAccumTime;
    fAccumTime += fDelta;

    this->CurrentTime = fAccumTime;
    m_fLastTime = fCurrentTime;
}
//---------------------------------------------------------------------------
void MTimeManager::ResetTime(float fTime)
{
    m_fAccumTime = ComputeScaledTime(fTime);
}
//---------------------------------------------------------------------------
void MTimeManager::RunUpTime(float fTime)
{
    if (m_bRunUpEnabled)
    {
        float fScaledTime = ComputeScaledTime(fTime);

        OnRunUpTime(fScaledTime);

        if (fScaledTime != m_fAccumTime)
        {
            m_fAccumTime = fScaledTime;
            m_fLastTime = -NI_INFINITY;
        }
    }
}
//---------------------------------------------------------------------------
float MTimeManager::IncrementTime(float fIncrement)
{
    CurrentTime = m_fAccumTime + fIncrement;
    return m_fAccumTime;
}
//---------------------------------------------------------------------------
float MTimeManager::ComputeScaledTime(float fTime)
{
    switch (m_eTimeMode)
    {
        case LOOP:
            if (m_fStartTime == m_fEndTime)
            {
                fTime = m_fEndTime;
            }
            else
            {
                while (fTime < m_fStartTime)
                {
                    fTime += m_fEndTime - m_fStartTime;
                }
                while (fTime > m_fEndTime)
                {
                    fTime -= m_fEndTime - m_fStartTime;
                }
            }
            break;
        case CLAMP:
            if (fTime < m_fStartTime)
            {
                fTime = m_fStartTime;
            }
            else if (fTime > m_fEndTime)
            {
                fTime = m_fEndTime;
            }
            break;
    }

    return fTime;
}
//---------------------------------------------------------------------------
float MTimeManager::get_CurrentTime()
{
    return m_fAccumTime;
}
//---------------------------------------------------------------------------
void MTimeManager::set_CurrentTime(float fTime)
{
    if (m_fAccumTime != fTime)
    {
        float fScaledTime = ComputeScaledTime(fTime);
        float fDiff = fScaledTime - m_fAccumTime;
        if (fDiff < 0.0f)
        {
            if (fabs(fDiff) > 0.01f)
            {
                OnRunUpTime(fScaledTime);
            }
            else
            {
                fScaledTime = m_fAccumTime;
            }
        }

        if (fScaledTime != m_fAccumTime)
        {
            m_fAccumTime = fScaledTime;
            m_fLastTime = -NI_INFINITY;
        }
    }
}
//---------------------------------------------------------------------------
float MTimeManager::get_ScaleFactor()
{
    return m_fScaleFactor;
}
//---------------------------------------------------------------------------
void MTimeManager::set_ScaleFactor(float fScaleFactor)
{
     m_fScaleFactor = fScaleFactor;
     OnScaleFactorChanged(fScaleFactor);
}
//---------------------------------------------------------------------------
float MTimeManager::get_StartTime()
{
    return m_fStartTime;
}
//---------------------------------------------------------------------------
void MTimeManager::set_StartTime(float fStartTime)
{
    m_fStartTime = fStartTime;
}
//---------------------------------------------------------------------------
float MTimeManager::get_EndTime()
{
    return m_fEndTime;
}
//---------------------------------------------------------------------------
void MTimeManager::set_EndTime(float fEndTime)
{
    m_fEndTime = fEndTime;
}
//---------------------------------------------------------------------------
bool MTimeManager::get_Enabled()
{
    return m_bTimingEnabled;
}
//---------------------------------------------------------------------------
void MTimeManager::set_Enabled(bool bEnabled)
{
    if (m_bTimingEnabled != bEnabled)
    {
        m_bTimingEnabled = bEnabled;
        if (bEnabled)
            m_fLastTime = -NI_INFINITY;
        OnEnabledChanged(bEnabled);
    }
}
//---------------------------------------------------------------------------
MTimeManager::TimeModeType MTimeManager::get_TimeMode()
{
    return m_eTimeMode;
}
//---------------------------------------------------------------------------
void MTimeManager::set_TimeMode(TimeModeType eMode)
{
    m_eTimeMode = eMode;
}
//---------------------------------------------------------------------------
bool MTimeManager::get_RunUpEnabled()
{
    return m_bRunUpEnabled;
}
//---------------------------------------------------------------------------
void MTimeManager::set_RunUpEnabled(bool bEnable)
{
    m_bRunUpEnabled = bEnable;
}
//---------------------------------------------------------------------------
