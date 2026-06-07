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

#ifndef NISCENECOMMANDINFO_H
#define NISCENECOMMANDINFO_H

#include "NiSceneCommandRegistry.h"
#include <NiMain.h>
#include "NiDevice.h"

class NiSceneCommandInfo : public NiRefObject
{
public:
    enum UserInterfaceType
    {
        UI_TYPE_NONE = -1,
        MOUSE = 0,
        KEYBOARD,
        GAMEPAD,
        NUM_DEVICES
    };

    NiSceneCommandInfo();
    virtual ~NiSceneCommandInfo();
    virtual NiSceneCommandInfo* Clone();
    void SetName(char* pcName);
    
    unsigned int m_uiClassID;
    long m_lModifiers;
    NiTPrimitiveArray<float> m_kParamList;
    NiDevice::DeviceState m_eDeviceState;
    UserInterfaceType m_eType;
    int m_iPriority;
    float m_fSystemDependentScaleFactor;
    char* m_pcName;
};

NiSmartPointer(NiSceneCommandInfo);
#endif