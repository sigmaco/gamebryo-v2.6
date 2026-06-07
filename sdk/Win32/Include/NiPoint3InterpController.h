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

#ifndef NIPOINT3INTERPCONTROLLER_H
#define NIPOINT3INTERPCONTROLLER_H

#include "NiSingleInterpController.h"
class NiPoint3;

class NIANIMATION_ENTRY NiPoint3InterpController :
    public NiSingleInterpController
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPoint3InterpController);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

public:
    // *** begin Emergent internal use only ***
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) 
        const;
    // *** end Emergent internal use only ***

protected:
    NiPoint3InterpController();
    
    // Virtual function overrides from base classes.
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;

    virtual void GetTargetPoint3Value(NiPoint3& kValue) = 0;
};

NiSmartPointer(NiPoint3InterpController);

#endif  // #ifndef NIPOINT3INTERPCONTROLLER_H
