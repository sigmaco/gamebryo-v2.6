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

#ifndef NISCENEKEYBOARDCOMMANDINFO_H
#define NISCENEKEYBOARDCOMMANDINFO_H
#include "NiSceneCommandInfo.h"
#include "NiSceneKeyboard.h"

class NiSceneKeyboardCommandInfo : 
    public NiSceneCommandInfo
{
public:
    NiSceneKeyboardCommandInfo();
    NiSceneKeyboardCommandInfo(NiSceneCommandInfo* pkOldInfo);
    NiSceneCommandInfo* Clone();
    
    NiSceneKeyboard::KeyCode m_eKey;          
};

NiSmartPointer(NiSceneKeyboardCommandInfo);

#endif