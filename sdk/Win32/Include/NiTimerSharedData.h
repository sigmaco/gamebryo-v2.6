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

#ifndef NITIMERSHAREDDATA_H
#define NITIMERSHAREDDATA_H

#include "NiStandardSharedDataLibType.h"
#include "NiSharedData.h"
#include <NiTStringMap.h>
#include <NiMain.h>

class NiKeyBasedInterpolator;

#define UNINITIALIZED_FLOAT 999999999.0f
#define UNINITIALIZED_UINT   (unsigned int) -1

/// This class contains the current information about timing
/// for the animations in the scene graph
class NISTANDARDSHAREDDATA_ENTRY NiTimerSharedData : public NiSharedData
{
    public:
        NiDeclareRTTI;

        enum TimeMode
        {
            LOOP = 0,
            CONTINUOUS,
            NUM_MODES
        };

        /// Default constructor.
        NiTimerSharedData();

        /// Virtual destructor.
        virtual ~NiTimerSharedData();
        float GetCurrentTime();
        float CalcCurrentTime();
        void  SetCurrentTime(float fTime);

        float GetScaleFactor();
        void  SetScaleFactor(float fScale);

        float IncrementTime(float fIncrement);

        float GetStartTime();
        void  SetStartTime(float fStart);
    
        float GetEndTime();
        void  SetEndTime(float fEnd);

        void  Enable();
        void  Disable();
        bool  IsEnabled();

        void  SetTimeMode(TimeMode eMode);
        
        TimeMode GetTimeMode();

        void CollectData(NiNode* pkRoot);

        unsigned int GetNumberOfControllers();
        
        unsigned int GetNumberOfKeyframes();
        
        float GetMinKeyTime();
        
        float GetMaxKeyTime();


    protected:

        void Add(NiTimeController* pkController);
        void AddInfo(NiKeyBasedInterpolator* pkInterp);
    
        void FindAndAdd(NiObjectNET* pkRoot);
        float m_fMinTime;
        float m_fMaxTime;
        unsigned int m_uiKeyframeCount;
        unsigned int m_uiControllerCount;

        float m_fAccumTime;
        float m_fLastTime;
        float m_fScaleFactor;
        float m_fStartTime;
        float m_fEndTime;
        bool  m_bTimingEnabled;
        TimeMode m_eTimeMode;

};


NiSmartPointer(NiTimerSharedData);

#endif  // #ifndef NITIMERSHAREDDATA_H
