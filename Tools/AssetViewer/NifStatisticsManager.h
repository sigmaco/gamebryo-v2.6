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

// NifStatisticsManager.h

#ifndef NIFSTATISTICSMANAGER_H
#define NIFSTATISTICSMANAGER_H

#include <NiRefObject.h>

NiSmartPointer(CNifStatisticsManager);

class CNifStatisticsManager : public NiRefObject
{
public:
    CNifStatisticsManager();
    virtual ~CNifStatisticsManager();

    // threadsafe access to the one-and-only CNifStatisticsManager object
    static void Lock();
    static CNifStatisticsManager* AccessStatisticsManager();
    static void UnLock();
    static void Destroy();

    void ResetTimers();

    void SetNumObjectsDrawnPerFrame(unsigned int uiNumObjs);
    unsigned int GetNumObjectsDrawnPerFrame();

    void SetFrameRateSampleTime(float fSampleTime);
    float GetFrameRateSampleTime() const;

    void SampleFrameRate();
    float GetFrameRate();

    void StartUpdateTimer();
    void StopUpdateTimer();
    float GetUpdateTime();

    void SetCullTime(float fCullTime);
    float GetCullTime();

    void SetRenderTime(float fRenderTime);
    float GetRenderTime();

    void StartSwapTimer();
    void StopSwapTimer();
    float GetSwapTime();

    void ResetRoots();

    void AddRoot(NiNode* pkRoot);
    void RemoveRoot(NiNode* pkRoot);

    unsigned int GetNumObjects();
    unsigned int GetNumTriangles();
    unsigned int GetNumVertices();

protected:
    class RootInfo : public NiMemObject
    {
    public:
        NiNode* m_pkRoot;
        unsigned int m_uiNumObjects;
        unsigned int m_uiNumTriangles;
        unsigned int m_uiNumVertices;
    };

    void ResetFrameRateTimer();
    void GatherStats(NiAVObject* pkObject, RootInfo* pkInfo);

    float m_fFrameRateSampleTime;

    unsigned int m_uiNumSamples;
    float m_fTimeElapsed;
    float m_fLastTime;

    float m_fFrameRate;

    float m_fUpdateTimer;
    float m_fSwapTimer;

    float m_fUpdateTime;
    float m_fCullTime;
    float m_fRenderTime;
    float m_fSwapTime;

    unsigned int m_uiLastNumObjectsDrawnPerFrame;
    NiTPrimitiveSet<RootInfo*> m_kRoots;

    // threadsafe access to the one-and-only CNifStatisticsManager object
    static CCriticalSection ms_kCriticalSection;
    static unsigned long ms_ulThreadId;
    static CNifStatisticsManagerPtr ms_spThis;
    static bool ms_bWorking;
};

#endif  // #ifndef NIFSTATISTICSMANAGER_H
