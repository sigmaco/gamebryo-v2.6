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

#ifndef NIVISCONTROLLER_H
#define NIVISCONTROLLER_H

#include "NiBoolInterpController.h"

class NIANIMATION_ENTRY NiVisController : public NiBoolInterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiVisController);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiVisController();

    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, bool bBool, 
        unsigned short usIndex = 0);

protected:
    // Virtual function overrides from base classes.
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetBoolValue(bool& bValue);
};

NiSmartPointer(NiVisController);

#endif  // #ifndef NIVISCONTROLLER_H
