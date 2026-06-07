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

// MStatisticsManager.h

#pragma once

#pragma unmanaged
#include <NiRefObject.h>
#include <NiTSet.h>
#include <NiSmartPointer.h>
#pragma managed

class NiNode;
class NiAVObject;

namespace NiManagedToolInterface
{
    class MStatisticsManager
    {
    public:
        MStatisticsManager();
        virtual ~MStatisticsManager();

        void ResetTimers();

        void SetFrameRateSampleTime(float fSampleTime);
        float GetFrameRateSampleTime() const;

        void SampleFrameRate();
        float GetFrameRate();

        void StartUpdateTimer();
        void StartClearTimer();
        void StartClickTimer();
        void StartSwapTimer();

        void StopUpdateTimer();
        void StopClearTimer();
        void StopClickTimer();
        void StopSwapTimer();

        float GetLastUpdateTime();
        float GetLastClearTime();
        float GetLastClickTime();
        float GetLastSwapTime();

        void ResetRoots();

        void AddRoot(NiNode* pkRoot);
        void RemoveRoot(NiNode* pkRoot);

        unsigned int GetNumObjects();
        unsigned int GetNumTriangles();
        unsigned int GetNumVertices();

    protected:
        struct RootInfo
        {
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
        float m_fClearTimer;
        float m_fClickTimer;
        float m_fSwapTimer;

        float m_fLastUpdateTime;
        float m_fLastClearTime;
        float m_fLastClickTime;
        float m_fLastSwapTime;

        NiTPrimitiveSet<RootInfo*> m_kRoots;
    };
}
