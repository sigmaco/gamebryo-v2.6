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

// NiVertexColorPropPropertyWindowCreator.h

#ifndef NIVERTEXCOLORPROPPROPERTYWINDOWCREATOR_H
#define NIVERTEXCOLORPROPPROPERTYWINDOWCREATOR_H

#include "NiObjectNETPropertyWindowCreator.h"
class CNifPropertyWindow;

class CNiVertexColorPropertyPropertyWindowCreator : 
    public CNiObjectNETPropertyWindowCreator
{
    public:
        virtual NiRTTI* GetTargetRTTI();
        virtual void AddPages(NiObject* pkObj, CNifPropertyWindow* pkWindow);
    protected:
        virtual int AddVertexColorPropertyDialog(NiObject* pkObj,
            CNifPropertyWindow* pkWindow, bool bExpanded = false,
            int iIndex = -1);
};

#endif  // #ifndef NIVERTEXCOLORPROPPROPERTYWINDOWCREATOR_H
