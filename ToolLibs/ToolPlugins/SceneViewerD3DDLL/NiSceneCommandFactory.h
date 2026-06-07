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

#ifndef NISCENECOMMANDFACTORY_H
#define NISCENECOMMANDFACTORY_H

#include "NiSceneCommandClasses.h"
#include "NiUIMap.h"
#include <NiRefObject.h>
#include "NiSceneMouseCommandInfo.h"
#include "NiSceneKeyboardCommandInfo.h"

class NiSceneCommandFactory: public NiRefObject
{
public:
    NiSceneCommandFactory(NiUIMap* pkParent);
    void SetParent(NiUIMap* pkParent);

    NiSceneCommand* CreateSceneKeyboardCommand(
        NiSceneKeyboardCommandInfo* pkInfo);      
    NiSceneCommand* CreateSceneMouseCommand(
        NiSceneMouseCommandInfo* pkInfo, int iDX, int iDY, int iX, int iY, 
        short sWheelDelta = 0);   
    NiSceneCommand* CreateSceneCommand(NiSceneCommandInfo* pkInfo);
protected:
    NiUIMap* m_pkParent;

    void AddToScaleFactor(NiSceneCommandInfo::UserInterfaceType m_eType, 
        float fScale);
    float m_fScaleFactorGamePad;
    float m_fScaleFactorMouse;
    float m_fScaleFactorKeyboard;
    NiCommandID m_kSpecialCaseClassID;
};

NiSmartPointer(NiSceneCommandFactory);
#endif