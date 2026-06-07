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

// NifRenderViewCameraResetCommand.cpp

#include "stdafx.h"
#include "NifRenderViewCameraResetCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"
#include "NifRenderViewUIManager.h"
#include "NifRenderViewUIState.h"

//---------------------------------------------------------------------------
bool CNifRenderViewCameraResetCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        m_pkView = pkView;
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            NiCameraInfoIndex kIndex = 
                pkView->GetCurrentCameraIndices();

            NiCameraInfo* pkInfo = pkDoc->GetCameraInfo(kIndex);
            //No camera == error
            if(!pkInfo)
                return false;
            if(!pkInfo->m_bIsAnimated)
            {
                pkInfo->m_spCam->SetRotate(pkInfo->m_kOriginalRotation);
                pkInfo->m_spCam->SetTranslate(pkInfo->m_kOriginalTranslation);
                pkInfo->m_spCam->Update(0.0);
            }

            CNifRenderViewUIManager* pkManager = pkView->GetUIManager();
            if(pkManager)
            {
                CNifRenderViewUIState* pkState = 
                    pkManager->GetCurrentViewState();
                if(pkState)
                    pkState->Initialize();
            }
        }
        // No document == error
        else 
            return false;
    }
    // No view == error
    else 
        return false;

    return true;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraResetCommand::CNifRenderViewCameraResetCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraResetCommand::~CNifRenderViewCameraResetCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
