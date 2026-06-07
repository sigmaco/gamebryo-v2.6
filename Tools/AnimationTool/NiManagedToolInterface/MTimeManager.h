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

namespace NiManagedToolInterface
{
    public __gc class MTimeManager
    {
    public:
        MTimeManager();

        __event void OnRunUpTime(float fTime);
        __event void OnScaleFactorChanged(float fScaleFactor);
        __event void OnEnabledChanged(bool bEnabled);

        __value enum TimeModeType : unsigned char
        {
            LOOP,
            CONTINUOUS,
            CLAMP
        };

        __property float get_CurrentTime();
        __property void set_CurrentTime(float fTime);

        __property float get_ScaleFactor();
        __property void set_ScaleFactor(float fScaleFactor);

        __property float get_StartTime();
        __property void set_StartTime(float fStartTime);
        
        __property float get_EndTime();
        __property void set_EndTime(float fEnd);

        __property bool get_Enabled();
        __property void set_Enabled(bool bEnabled);

        __property TimeModeType get_TimeMode();
        __property void set_TimeMode(TimeModeType eMode);

        __property bool get_RunUpEnabled();
        __property void set_RunUpEnabled(bool bEnable);

        void UpdateTime();
        void ResetTime(float fTime);
        void RunUpTime(float fTime);
        float IncrementTime(float fIncrement);
        
    protected:
        float ComputeScaledTime(float fTime);

        float m_fAccumTime;
        float m_fScaleFactor;
        float m_fStartTime;
        float m_fEndTime;
        bool m_bTimingEnabled;
        TimeModeType m_eTimeMode;
        bool m_bRunUpEnabled;

        float m_fLastTime;
    };
}

#endif  // #ifndef NIFTIMEMANAGER_H
