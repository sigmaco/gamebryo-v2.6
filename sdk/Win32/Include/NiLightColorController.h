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

#ifndef NILIGHTCOLORCONTROLLER_H
#define NILIGHTCOLORCONTROLLER_H

#include <NiLight.h>
#include <NiPoint3InterpController.h>
#include "NiPosData.h"

NiSmartPointer(NiLightColorController);

class NIANIMATION_ENTRY NiLightColorController :
    public NiPoint3InterpController
{
    NiDeclareRTTI;
    NiDeclareClone(NiLightColorController);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

public:
    NiLightColorController ();
    virtual ~NiLightColorController();

    virtual void Update(float fTime);
    virtual bool UpdateValue(float fTime, const NiPoint3& kPoint3, 
        unsigned short usIndex = 0);

    // animation data access
    inline void SetAmbient(bool bIsAmbient);
    inline bool GetAmbient() const;

    // *** begin Emergent internal use only ***
    virtual const char* GetCtlrID();
    // *** begin Emergent internal use only ***

protected:
    // Virtual function overrides from base classes.
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetPoint3Value(NiPoint3& kValue);

    // flags
    enum
    {
        ISAMBIENT_MASK  = 0x0001
    };
};

#include "NiLightColorController.inl"

#endif

