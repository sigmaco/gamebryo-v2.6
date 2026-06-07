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

#ifndef NISTOPWATCH_H
#define NISTOPWATCH_H

#include <NiMemObject.h>

class NiStopWatch : public NiMemObject
{
public:
    NiStopWatch();
    
    inline void Start();                   // start timing if not already started
    inline void Stop();                    // stop timing if no other counts pending

    inline void Hold();                    // temporarily stop timing
    inline void Unhold();                  // resume timing

    inline void Reset();
    inline unsigned int GetTime() const;   // GetTime() returns timing data in Release
                                    // builds, 1 in Debug builds.
    inline float GetTimeInSeconds() const; // Returns the same as GetTime(), but in
                                    // seconds.  Returns 1.0f in Debug builds.

    inline unsigned int GetCount() const;  // Get # times started - # times stopped

protected:
    unsigned int m_uiCount;         // # times started - # times stopped
    unsigned int m_uiTime;          // time when Start() was first called
    unsigned int m_uiHoldTime;      // time when Hold() was called
    bool m_bHold;                   // true if currently holding
};

#include "NiStopWatch.inl"

#endif // #ifndef NISTOPWATCH_H
