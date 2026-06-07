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

#ifndef NIANIMATIONLOOPCOMMAND_H
#define NIANIMATIONLOOPCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   AnimationLoop
// Command Purpose: To loop the animation of the world using sceneviewer start
// and end times Command String Syntax: ANIMATION_LOOP_TOGGLE() SINGLE

class NiAnimationLoopCommand : public NiSceneCommand
{
public:
    NiAnimationLoopCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiAnimationLoopCommand();
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
