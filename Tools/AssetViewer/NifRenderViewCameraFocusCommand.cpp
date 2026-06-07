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


#include "StdAfx.h"
#include "NifRenderViewCameraFocusCommand.h"
#include "NifDoc.h"
#include "NifRenderView.h"
#include "NifRenderViewUIManager.h"


//---------------------------------------------------------------------------
bool CNifRenderViewCameraFocusCommand::Execute(CNifRenderView* pkView)
{
    if (pkView)
    {
        CNifRenderViewUIManager* pkUIManager = pkView->GetUIManager();
        if (pkUIManager)
        {
            // Sending message because not all modes support focusing
            pkUIManager->OnUpdate(NIF_CAMERAFOCUSREQUESTED);
        }
        else 
        {
            // No UI manager == error
            return false;
        }
    }
    else
    {
        // No view == error
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraFocusCommand::CNifRenderViewCameraFocusCommand()
{
}
//---------------------------------------------------------------------------
