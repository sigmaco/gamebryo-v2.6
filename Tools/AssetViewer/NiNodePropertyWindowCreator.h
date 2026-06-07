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

// NiNodePropertyWindowCreator.h

#ifndef NINODEPROPERTYWINDOWCREATOR_H
#define NINODEPROPERTYWINDOWCREATOR_H

#include "NiAVObjectPropertyWindowCreator.h"

class CNifPropertyWindow;

class CNiNodePropertyWindowCreator : public CNiAVObjectPropertyWindowCreator
{
    public:
        virtual NiRTTI* GetTargetRTTI();
        virtual void AddPages(NiObject* pkObj, CNifPropertyWindow* pkWindow);

    protected:
        virtual int AddEffectsDialog(NiObject* pkObj,
            CNifPropertyWindow* pkWindow, bool bExpanded = false,
            int iIndex = -1);
        virtual int AddChildrenDialog(NiObject* pkObj,
            CNifPropertyWindow* pkWindow, bool bExpanded = false,
            int iIndex = -1);
};

#endif  // #ifndef NINODEPROPERTYWINDOWCREATOR_H
