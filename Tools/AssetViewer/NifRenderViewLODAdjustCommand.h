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

// NifRenderViewLODAdjustCommand.h

#ifndef NIFRENDERVIEWLODADJUSTCOMMAND_H
#define NIFRENDERVIEWLODADJUSTCOMMAND_H

#include "NifRenderViewUICommand.h"

class CNifRenderView;

class CNifRenderViewLODAdjustCommand : public CNifRenderViewUICommand
{    
    public:
        CNifRenderViewLODAdjustCommand(int iAdjust);
        ~CNifRenderViewLODAdjustCommand();
        bool Execute(CNifRenderView* pkView);
        virtual char* GetIDString() {return "CameraLODAdjust";};

        static bool IsAdjustEnabled();
        static void ToggleAdjust(bool bAdjustOn);

        static bool IsDecrementEnabled();
        static bool IsIncrementEnabled();

    protected:
        int m_iAdjust;
        static bool m_bAllowAdjust;


};

#endif  // #ifndef NIFRENDERVIEWLODADJUSTCOMMAND_H
