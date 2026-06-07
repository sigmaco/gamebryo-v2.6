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

// NifTimeManager.h

#ifndef NIFTIMEMANAGER_H
#define NIFTIMEMANAGER_H

class CNifTimeManager : public NiMemObject
{
public:
    enum TimeMode
    {
        LOOP = 0,
        CONTINUOUS,
        NUM_MODES
    };

    enum 
    {
        TM_SCALE_FACTOR = 0,
        TM_START_TIME,
        TM_END_TIME,
        TM_TIME_MODE,
        TM_ENABLING
    };

    CNifTimeManager();

    float GetCurrentTime();
    void SetCurrentTime(float fTime);

    float GetScaleFactor() const;
    void SetScaleFactor(float fScale);

    float IncrementTime(float fIncrement);

    float GetStartTime() const;
    void SetStartTime(float fStart);
    
    float GetEndTime() const;
    void SetEndTime(float fEnd);

    bool IsEnabled() const;
    void Enable();
    void Disable();

    TimeMode GetTimeMode() const;
    void SetTimeMode(TimeMode eMode);

protected:
    float m_fAccumTime;
    float m_fScaleFactor;
    float m_fStartTime;
    float m_fEndTime;
    bool m_bTimingEnabled;
    TimeMode m_eTimeMode;

    float m_fLastTime;
};

#include "NifTimeManager.inl"

#endif  // #ifndef NIFTIMEMANAGER_H
