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

// NifPropertyWindowFactory.h

#ifndef NIFPROPERTYWINDOWFACTORY_H
#define NIFPROPERTYWINDOWFACTORY_H

#include "NiObjectPropertyWindowCreator.h"

class CNifPropertyWindow;

class CNifPropertyWindowFactory
{
    public:
        static CNifPropertyWindow* CreatePropertyWindow(NiObject* pkObj,
            int iStartX, int iStartY, int iWidth, int iHeight);

        static void RegisterNiObjectPropertyCreator(
            CNiObjectPropertyWindowCreator* pkCreator);

        static void RemoveAllCreators();

        static void Init();
        static void Shutdown();

    protected:
        static NiTMap<NiRTTI*, CNiObjectPropertyWindowCreatorPtr>* 
            ms_pkCreatorMap; 
    
};

#endif  // #ifndef NIFPROPERTYWINDOWFACTORY_H
