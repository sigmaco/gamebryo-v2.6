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

// NifRenderViewUICommand.h

#ifndef NIFRENDERVIEWUICOMMAND_H
#define NIFRENDERVIEWUICOMMAND_H

class CNifRenderView;

class CNifRenderViewUICommand
{
    public:
        virtual ~CNifRenderViewUICommand() {};
        virtual bool Execute(CNifRenderView* pkView) = 0;
        virtual char* GetIDString() {return "NULL";};
};

#endif  // #ifndef NIFRENDERVIEWUICOMMAND_H
