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

#ifndef NIDATASTREAMPROPERTYWINDOWCREATOR_H
#define NIDATASTREAMPROPERTYWINDOWCREATOR_H

#include "NiAVObjectPropertyWindowCreator.h"

class CNiDataStreamPropertyWindowCreator :
    public CNiObjectPropertyWindowCreator
{
public:
    virtual NiRTTI* GetTargetRTTI();
    virtual void AddPages(NiObject* pkObj, CNifPropertyWindow* pkWindow);

protected:
    int AddDataStreamDialog(NiObject* pkObj, CNifPropertyWindow* pkWindow, 
        bool bExpanded = false, int iIndex = -1);
};

#endif  // #ifndef NIDATASTREAMPROPERTYWINDOWCREATOR_H
