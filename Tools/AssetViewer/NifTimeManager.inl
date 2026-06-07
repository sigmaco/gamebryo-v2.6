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
inline float CNifTimeManager::GetScaleFactor() const
{ 
    return m_fScaleFactor;
}
//---------------------------------------------------------------------------
inline void CNifTimeManager::SetScaleFactor(float fScale)
{ 
     m_fScaleFactor = fScale;
     MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMINGINFOCHANGED, 
         TM_SCALE_FACTOR));
}
//---------------------------------------------------------------------------
inline float CNifTimeManager::GetStartTime() const
{ 
    return m_fStartTime;
}
//---------------------------------------------------------------------------
inline void CNifTimeManager::SetStartTime(float fStart)
{ 
    m_fStartTime = fStart;
    MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMINGINFOCHANGED,
        TM_START_TIME));
}
//---------------------------------------------------------------------------
inline float CNifTimeManager::GetEndTime() const
{ 
    return m_fEndTime;
}
//---------------------------------------------------------------------------
inline void CNifTimeManager::SetEndTime(float fEnd)
{ 
    m_fEndTime = fEnd;
    MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMINGINFOCHANGED, TM_END_TIME));
}
//---------------------------------------------------------------------------
inline bool CNifTimeManager::IsEnabled() const
{
    return m_bTimingEnabled;
}
//---------------------------------------------------------------------------
inline void CNifTimeManager::Enable()
{
    m_bTimingEnabled = true;
    m_fLastTime = -NI_INFINITY;
    MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMINGINFOCHANGED, TM_ENABLING));
}
//---------------------------------------------------------------------------
inline void CNifTimeManager::Disable()
{
    m_bTimingEnabled = false;
    MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMINGINFOCHANGED, TM_ENABLING));
}
//---------------------------------------------------------------------------
inline CNifTimeManager::TimeMode CNifTimeManager::GetTimeMode() const
{
    return m_eTimeMode;
}
//---------------------------------------------------------------------------
inline void CNifTimeManager::SetTimeMode(TimeMode eMode)
{
    NIASSERT(eMode >= 0 && eMode < NUM_MODES);
    m_eTimeMode = eMode;
    MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMINGINFOCHANGED, TM_TIME_MODE));
}
//---------------------------------------------------------------------------
