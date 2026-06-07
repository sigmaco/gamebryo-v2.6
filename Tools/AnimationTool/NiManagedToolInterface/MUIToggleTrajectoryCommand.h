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


#ifndef MUITOGGLETRAJECTORYCOMMAND_H
#define MUITOGGLETRAJECTORYCOMMAND_H

#include "MUICommand.h"

namespace NiManagedToolInterface
{
    public __gc class MUIToggleTrajectoryCommand : public MUICommand
    {
    public:
        MUIToggleTrajectoryCommand();
        virtual String* GetName();
        virtual bool Execute(MUIState* pkState);
        virtual void DeleteContents();
        virtual void RefreshData();
        
        virtual bool IsToggle();
        virtual bool GetActive();
        virtual bool GetEnabled();
        virtual MUICommand::CommandType GetCommandType();
        virtual void Update(float fTime);
        virtual void ResetTrajectory();
    protected:
        void AttachTrajectory();
        void SetRegionRange(NiMesh* pkLines, NiUInt32 uiRange);

        NiMesh* GetTrajectory();
        unsigned int m_uiTrajectoryIdx;
        bool m_bOn;
        float m_fLastTime;
        unsigned int m_uiNextVertex;
    };

}

#endif  // #ifndef MUITOGGLETRAJECTORYCOMMAND_H
