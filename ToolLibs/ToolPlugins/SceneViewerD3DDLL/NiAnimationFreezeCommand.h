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

#ifndef NIANIMATIONFREEZECOMMAND_H
#define NIANIMATIONFREEZECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   AnimationFreeze
// Command ClassID: 7
// Command Purpose: To pause the animation of the world
// Command String Syntax: ANIMATION_FREEZE_TOGGLE() SINGLE

class NiAnimationFreezeCommand : public NiSceneCommand
{
public:
    NiAnimationFreezeCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiAnimationFreezeCommand();
    bool Apply(NiSceneViewer* pkViewer);
    static NiCommandID GetClassID();
    static unsigned int GetParameterCount();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();
protected:
    static NiCommandID ms_kID;
};

#endif