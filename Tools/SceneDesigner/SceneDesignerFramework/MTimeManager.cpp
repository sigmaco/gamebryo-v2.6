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

// Precompiled Header
#include "SceneDesignerFrameworkPCH.h"

#include "MTimeManager.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

// Uncomment the following line to run MTimeManager with constant time for
// debugging purposes.
//#define USE_CONSTANT_TIME
#define CONSTANT_TIME_INCREMENT 0.0167f

//---------------------------------------------------------------------------
void MTimeManager::Init()
{
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MTimeManager();
    }
}
//---------------------------------------------------------------------------
void MTimeManager::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
}
//---------------------------------------------------------------------------
bool MTimeManager::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MTimeManager* MTimeManager::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MTimeManager::MTimeManager() : m_fAccumTime(0.0f), m_fScaleFactor(1.0f),
    m_fStartTime(0.0f), m_fEndTime(0.0f), m_bTimingEnabled(false),
    m_eTimeMode(TimeModeType::Continuous), m_bRunUpEnabled(true),
    m_bSlowFrameHit(false), m_fLastTime(-NI_INFINITY), m_fContinuousTime(0.0f)
{
}
//---------------------------------------------------------------------------
void MTimeManager::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
void MTimeManager::UpdateTime()
{
    MVerifyValidInstance;

    float fCurrentTime = MUtility::GetCurrentTimeInSec();
    if (m_fLastTime == -NI_INFINITY)
    {
        m_fLastTime = fCurrentTime;
    }

    float fDelta;
#ifdef USE_CONSTANT_TIME
    fDelta = CONSTANT_TIME_INCREMENT;
#else
    fDelta = fCurrentTime - m_fLastTime;
#endif
    if (fDelta > 0.25f)
    {
        // Prevent large jumps in time due to the main window going out of
        // focus. If this time is slow for two frames in a row, the frame
        // rate is just very slow for the tool, so update like normal to
        // prevent slowing down animations.
        if (!m_bSlowFrameHit)
        {
            fDelta = CONSTANT_TIME_INCREMENT;
        }
        m_bSlowFrameHit = true;
    }
    else
    {
        m_bSlowFrameHit = false;
    }
    fDelta *= m_fScaleFactor;

    if (m_bTimingEnabled)
    {
        float fAccumTime = m_fAccumTime;
        fAccumTime += fDelta;
        this->CurrentTime = fAccumTime;
    }
    m_fContinuousTime += fDelta;

    m_fLastTime = fCurrentTime;
}
//---------------------------------------------------------------------------
void MTimeManager::ResetTime(float fTime)
{
    MVerifyValidInstance;

    m_fAccumTime = ComputeScaledTime(fTime);
}
//---------------------------------------------------------------------------
void MTimeManager::RunUpTime(float fTime)
{
    MVerifyValidInstance;

    if (m_bRunUpEnabled)
    {
        float fScaledTime = ComputeScaledTime(fTime);

        RunUpTimeRequested(fScaledTime);

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
    MVerifyValidInstance;

    this->CurrentTime = m_fAccumTime + fIncrement;
    return m_fAccumTime;
}
//---------------------------------------------------------------------------
float MTimeManager::ComputeScaledTime(float fTime)
{
    MVerifyValidInstance;

    switch (m_eTimeMode)
    {
    case TimeModeType::Loop:
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
    case TimeModeType::Clamp:
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
    MVerifyValidInstance;

    return m_fAccumTime;
}
//---------------------------------------------------------------------------
void MTimeManager::set_CurrentTime(float fTime)
{
    MVerifyValidInstance;

    if (m_fAccumTime != fTime)
    {
        float fScaledTime = ComputeScaledTime(fTime);
        float fDiff = fScaledTime - m_fAccumTime;
        if (fDiff < 0.0f)
        {
            if (fabs(fDiff) > 0.01f)
            {
                RunUpTimeRequested(fScaledTime);
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
float MTimeManager::get_ContinuousTime()
{
    MVerifyValidInstance;

    return m_fContinuousTime;
}
//---------------------------------------------------------------------------
float MTimeManager::get_ScaleFactor()
{
    MVerifyValidInstance;

    return m_fScaleFactor;
}
//---------------------------------------------------------------------------
void MTimeManager::set_ScaleFactor(float fScaleFactor)
{
    MVerifyValidInstance;

    m_fScaleFactor = fScaleFactor;
    ScaleFactorChanged(fScaleFactor);
}
//---------------------------------------------------------------------------
float MTimeManager::get_StartTime()
{
    MVerifyValidInstance;

    return m_fStartTime;
}
//---------------------------------------------------------------------------
void MTimeManager::set_StartTime(float fStartTime)
{
    MVerifyValidInstance;

    m_fStartTime = fStartTime;
}
//---------------------------------------------------------------------------
float MTimeManager::get_EndTime()
{
    MVerifyValidInstance;

    return m_fEndTime;
}
//---------------------------------------------------------------------------
void MTimeManager::set_EndTime(float fEndTime)
{
    MVerifyValidInstance;

    m_fEndTime = fEndTime;
}
//---------------------------------------------------------------------------
bool MTimeManager::get_Enabled()
{
    MVerifyValidInstance;

    return m_bTimingEnabled;
}
//---------------------------------------------------------------------------
void MTimeManager::set_Enabled(bool bEnabled)
{
    MVerifyValidInstance;

    if (m_bTimingEnabled != bEnabled)
    {
        m_bTimingEnabled = bEnabled;
        if (m_bTimingEnabled)
        {
            m_fLastTime = -NI_INFINITY;
        }
        EnabledStateChanged(m_bTimingEnabled);
    }
}
//---------------------------------------------------------------------------
MTimeManager::TimeModeType MTimeManager::get_TimeMode()
{
    MVerifyValidInstance;

    return m_eTimeMode;
}
//---------------------------------------------------------------------------
void MTimeManager::set_TimeMode(TimeModeType eMode)
{
    MVerifyValidInstance;

    m_eTimeMode = eMode;
}
//---------------------------------------------------------------------------
bool MTimeManager::get_RunUpEnabled()
{
    MVerifyValidInstance;

    return m_bRunUpEnabled;
}
//---------------------------------------------------------------------------
void MTimeManager::set_RunUpEnabled(bool bEnable)
{
    MVerifyValidInstance;

    m_bRunUpEnabled = bEnable;
}
//---------------------------------------------------------------------------
