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

// NiAVObjectPropertyWindowCreator.h

#ifndef NIAVOBJECTPROPERTYWINDOWCREATOR_H
#define NIAVOBJECTPROPERTYWINDOWCREATOR_H

#include "NiObjectNETPropertyWindowCreator.h"

class CNifPropertyWindow;

class CNiAVObjectPropertyWindowCreator :
    public CNiObjectNETPropertyWindowCreator
{
public:
    virtual NiRTTI* GetTargetRTTI();
    virtual void AddPages(NiObject* pkObj, CNifPropertyWindow* pkWindow);

protected:
    virtual int AddWorldTransformsDialog(NiObject* pkObj,
        CNifPropertyWindow* pkWindow, bool bExpanded = false,
        int iIndex = -1);
    virtual int AddLocalTransformsDialog(NiObject* pkObj,
        CNifPropertyWindow* pkWindow, bool bExpanded = false,
        int iIndex = -1);
    virtual int AddNiBoundDialog(NiObject* pkObj,
        CNifPropertyWindow* pkWindow, bool bExpanded = false,
        int iIndex = -1);
    virtual int AddSelectiveUpdateDialog(NiObject* pkObj,
        CNifPropertyWindow* pkWindow, bool bExpanded = false,
        int iIndex = -1);
    virtual int AddPropertiesDialog(NiObject* pkObj,
        CNifPropertyWindow* pkWindow, bool bExpanded = false,
        int iIndex = -1);
    virtual int AddCollisionDialog(NiObject* pkObj,
        CNifPropertyWindow* pkWindow, bool bExpanded = false,
        int iIndex = -1);
};

#endif  // #ifndef NIAVOBJECTPROPERTYWINDOWCREATOR_H
