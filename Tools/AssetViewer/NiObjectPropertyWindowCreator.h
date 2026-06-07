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

// NiObjectPropertyWindowCreator.h

#ifndef NIOBJECTPROPERTYWINDOWCREATOR_H
#define NIOBJECTPROPERTYWINDOWCREATOR_H

class CNifPropertyWindow;

class CNiObjectPropertyWindowCreator : public NiRefObject
{
    public:
        virtual ~CNiObjectPropertyWindowCreator();
        virtual NiRTTI* GetTargetRTTI();
        virtual CNifPropertyWindow* CreatePropertyWindow(NiObject* pkObj,
            int iStartX, int iStartY, int iWidth, int iHeight);
        virtual void AddPages(NiObject* pkObj, CNifPropertyWindow* pkWindow);

    protected:
        virtual int AddViewerStringsDialog(NiObject* pkObj,
            CNifPropertyWindow* pkWindow, bool bExpanded = false,
            int iIndex = -1, char* pcString = "Viewer Strings", 
            bool bLimitedText = false);
        virtual int AddBasicInfoDialog(NiObject* pkObj,
            CNifPropertyWindow* pkWindow, bool bExpanded = false,
            int iIndex = -1);

        static int ms_iWindowID;
};

NiSmartPointer(CNiObjectPropertyWindowCreator);

#endif  // #ifndef NIOBJECTPROPERTYWINDOWCREATOR_H
