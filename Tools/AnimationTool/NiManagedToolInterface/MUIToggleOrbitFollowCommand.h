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


#ifndef MUITOGGLEORBITFOLLOWCOMMAND_H
#define MUITOGGLEORBITFOLLOWCOMMAND_H

#include "MUICommand.h"

namespace NiManagedToolInterface
{
    public __gc class MUIToggleOrbitFollowCommand : public MUICommand
    {
    public:
        MUIToggleOrbitFollowCommand();
        virtual String* GetName();
        virtual bool Execute(MUIState* pkState);
        virtual void DeleteContents();
        virtual void RefreshData();
        
        virtual bool IsToggle();
        virtual bool GetActive();
        virtual bool GetEnabled();
        virtual MUICommand::CommandType GetCommandType();
        
        void SetOrbitPoint(MOrbitPoint* pkOrbitPt);
    
    protected:
        MOrbitPoint* m_pkOrbitPt;
    };

}

#endif  // #ifndef MUITOGGLEORBITFOLLOWCOMMAND_H
