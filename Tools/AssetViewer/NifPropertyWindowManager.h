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

// NifPropertyWindowManager.h

#ifndef NIFPROPERTYWINDOWMANAGER_H
#define NIFPROPERTYWINDOWMANAGER_H

#include "NifPropertyWindow.h"
#include <NiMemObject.h>

class CNifPropertyWindowManager : public NiMemObject
{
    public:
        static CNifPropertyWindowManager* GetPropertyWindowManager();
        static void DestroyPropertyWindowManager();

        CNifPropertyWindow* CreatePropertyWindow(NiObject* pkObj);
        void KillAllPropertyWindows();
        void RemovePropertyWindow(CNifPropertyWindow* pkWindow);
        void UpdateAllVisiblePropertyWindows(bool bForceUpdate = false);

        CNifPropertyWindow* GetPropertyWindowForNiObject(NiObject* pkObj);
        void OnRemoveRoot();
        void OnDestroyScene();
        virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
        static void SendUpdateMessage(CView* pSender, LPARAM lHint,
            CObject* pHint);
    protected:
        static CNifPropertyWindowManager* ms_pkThis;
        float m_fLastTimeUpdated;
        CNifPropertyWindowManager();
        ~CNifPropertyWindowManager();
        NiTPrimitiveArray<CNifPropertyWindow*> m_kWindowsArray;
};

#endif  // #ifndef NIFPROPERTYWINDOWMANAGER_H
