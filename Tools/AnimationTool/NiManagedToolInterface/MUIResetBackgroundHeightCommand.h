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


#ifndef MUIRESETBACKGROUNDHEIGHTCOMMAND_H
#define MUIRESETBACKGROUNDHEIGHTCOMMAND_H

#include "MUICommand.h"

namespace NiManagedToolInterface
{
    public __gc class MUIResetBackgroundHeightCommand : public MUICommand
    {
    public:
        MUIResetBackgroundHeightCommand();
        virtual String* GetName();
        virtual bool Execute(MUIState* pkState);
        virtual void DeleteContents();
        virtual void RefreshData();
        virtual bool GetEnabled();
        
        virtual MUICommand::CommandType GetCommandType();
    protected:
        float m_fSceneHeight;
    };

}

#endif  // #ifndef MUIRESETBACKGROUNDHEIGHTCOMMAND_H
