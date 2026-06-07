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

#ifndef NIPOSEBLENDER_H
#define NIPOSEBLENDER_H

#include "NiAnimationLibType.h"
#include "NiObject.h"
#include "NiPoseBuffer.h"

class NiControllerSequence;


NiSmartPointer(NiPoseBlender);

class NIANIMATION_ENTRY NiPoseBlender : public NiObject
{
    NiDeclareRTTI;

public:
    virtual ~NiPoseBlender();

    // *** begin Emergent internal use only ***
    NiPoseBlender(NiControllerManager* pkOwner);
    NiControllerManager* GetOwner() const;

    bool Update(unsigned int uiNumContributingSequences, int iLOD, 
        NiPoseBuffer*& pkFinalPoseBuffer, 
        NiControllerSequence*& pkSoleSequence);
    // *** end Emergent internal use only ***

protected:
    void Init(NiControllerManager* pkOwner);
    void Shutdown();

    void AddNewLODsFromPoseBinding();

    enum
    {
        MAXBITPATTERNS = 16,
        MAXBLENDEDSEQUENCES = 32,
    };

    struct ActivePose
    {
        ActivePose();

        NiPoseBuffer* m_pkPoseBuffer;
        unsigned int* m_puiFlagWeight;
        bool m_bHasContributingItems;
        int m_iPriority;
        float m_fWeight;
        NiControllerSequence* m_pkSequence;
    };

    NiControllerManager* m_pkOwner;
    NiPoseBufferPtr m_spFinalPoseBuffer;

    unsigned int m_uiNumLODs;
    short* m_psLODs;

    unsigned int m_uiActivePoseSize;
    ActivePose* m_pkActivePoseList;

    unsigned int m_uiNumBitPatterns;
    unsigned int m_auiBitPatternList[MAXBITPATTERNS];
    unsigned int m_auiBitMaskList[MAXBITPATTERNS];

    unsigned int m_uiFlagWeightSize;
    unsigned int* m_puiFlagWeightArray;
};

#include "NiPoseBlender.inl"

#endif
