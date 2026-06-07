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

#ifndef NISCENEMOUSECOMMANDINFO_H
#define NISCENEMOUSECOMMANDINFO_H

#include "NiSceneCommandInfo.h"

class NiSceneMouseCommandInfo : public NiSceneCommandInfo
{
public:
    enum MouseMovementType
    {
        MOUSE_MOVEMENT_NONE = -1,
        FRONT_BACK = 0, 
        LEFT_RIGHT,
        NUM_MOUSE_MOVEMENT_TYPES
    };

    NiSceneMouseCommandInfo();
    NiSceneMouseCommandInfo(NiSceneCommandInfo* pkOldInfo);

    NiSceneCommandInfo* Clone();

    int m_iWheelDeltaAffectedParameterIndex;
    int m_iDxAffectedParameterIndex;
    int m_iDyAffectedParameterIndex;
    int m_iXAffectedParameterIndex;
    int m_iYAffectedParameterIndex;
};

NiSmartPointer(NiSceneMouseCommandInfo);
#endif