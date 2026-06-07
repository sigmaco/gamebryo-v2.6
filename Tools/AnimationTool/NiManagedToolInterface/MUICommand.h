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


#ifndef MUICOMMAND_H
#define MUICOMMAND_H

#include "MUIState.h"

namespace NiManagedToolInterface
{
    public __abstract __gc class MUICommand
    {
    public:
        __value enum CommandType : unsigned int
        {
            ZOOM_EXTENTS = 0,
            RESET_FRUSTUM,
            BONELOD_INCREMENT,
            BONELOD_DECREMENT,
            TOGGLE_WIREFRAME,
            TOGGLE_SCENE_LIGHTS,
            TOGGLE_BACKGROUND_SCENE_VIS,
            TOGGLE_FRAME_RATE,
            RESET_BACKGROUND_HEIGHT,
            TOGGLE_COLLISION_ABVS,
            TOGGLE_TRAJECTORY,
            RESET_ACTOR_POSITION,
            RESET_ORBIT_POINT,
            TOGGLE_ORBIT_FOLLOW,
            MAX_COMMAND_TYPE
        };

        MUICommand();
        virtual String* GetName();
        virtual bool Execute(MUIState* pkState);
        virtual void DeleteContents();
        virtual void RefreshData();
        virtual bool IsToggle();
        virtual bool GetEnabled();
        virtual bool GetActive();
        virtual void Update(float fTime);
        virtual CommandType GetCommandType() = 0;

        __event void OnCommandExecuted(MUICommand* pkCommand);
        __event void OnCommandStateChanged(MUICommand* pkCommand);
    };

}

#endif  // #ifndef MUICOMMAND_H
