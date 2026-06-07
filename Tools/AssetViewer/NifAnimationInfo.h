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

// NifAnimationInfo.h

#ifndef NIFANIMATIONINFO_H
#define NIFANIMATIONINFO_H

#define UNINITIALIZED_FLOAT 999999999.0f
#define UNINITIALIZED_UINT   (unsigned int) -1

class CNifAnimationInfo : public NiMemObject
{
    public:
        CNifAnimationInfo();
        ~CNifAnimationInfo();

        void CollectData(NiNode* pkRoot);
        unsigned int GetNumberOfControllers();
        unsigned int GetNumberOfKeyframes();
        float GetMinTime();
        float GetMaxTime();


    protected:

        void Add(NiTimeController* pkController);

        void AddInfo(NiTimeController* pkController);
        void AddInterpInfo(NiInterpController* pkController);
    
        void FindAndAdd(NiObjectNET* pkRoot);
        float m_fMinTime;
        float m_fMaxTime;
        unsigned int m_uiKeyframeCount;
        unsigned int m_uiControllerCount;
};

#endif  // #ifndef NIFANIMATIONINFO_H
