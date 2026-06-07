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


#ifndef MUITOGGLESCENELIGHTSCOMMAND_H
#define MUITOGGLESCENELIGHTSCOMMAND_H

#include "MUICommand.h"

namespace NiManagedToolInterface
{
    public __gc class MUIToggleSceneLightsCommand : public MUICommand
    {
    public:
        MUIToggleSceneLightsCommand();
        virtual String* GetName();
        virtual bool Execute(MUIState* pkState);
        virtual void DeleteContents();
        virtual void RefreshData();
        
        virtual bool IsToggle();
        virtual bool GetActive();
        virtual bool GetEnabled();
        virtual MUICommand::CommandType GetCommandType();
    protected:
        NiNode* CreateDefaultLights();
        void AttachSceneLights();
        NiDirectionalLight* GetDirLight1();
        NiDirectionalLight* GetDirLight2();
        unsigned int m_uiSceneLightsIdx;
        bool m_bOn;
    };

}

#endif  // #ifndef MUITOGGLESCENELIGHTSCOMMAND_H
