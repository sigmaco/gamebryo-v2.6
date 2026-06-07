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

#ifndef NIFLOATEXTRADATACONTROLLER_H
#define NIFLOATEXTRADATACONTROLLER_H

#include "NiExtraDataController.h"

class NIANIMATION_ENTRY NiFloatExtraDataController :
    public NiExtraDataController
{
    NiDeclareRTTI;
    NiDeclareClone(NiFloatExtraDataController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiFloatExtraDataController(const NiFixedString& kFloatExtraDataName);
    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, float fFloat, 
        unsigned short usIndex = 0);

    // *** begin Emergent internal use only ***
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) 
        const;
    // *** begin Emergent internal use only ***

protected:
    // For cloning and streaming only.
    NiFloatExtraDataController();

    // Virtual function overrides from base classes.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;
};

NiSmartPointer(NiFloatExtraDataController);

#endif  // #ifndef NIFLOATEXTRADATACONTROLLER_H
