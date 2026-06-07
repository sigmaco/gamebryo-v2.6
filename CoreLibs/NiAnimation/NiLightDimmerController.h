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

#ifndef NILIGHTDIMMERCONTROLLER_H
#define NILIGHTDIMMERCONTROLLER_H

#include <NiLight.h>
#include <NiFloatInterpController.h>

NiSmartPointer(NiLightDimmerController);

class NIANIMATION_ENTRY NiLightDimmerController :
    public NiFloatInterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiLightDimmerController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiLightDimmerController ();
    virtual ~NiLightDimmerController();

    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, float fFloat, 
        unsigned short usIndex = 0);

protected:
    // Virtual function overrides from base classes.
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetFloatValue(float& fValue);
};


#endif

