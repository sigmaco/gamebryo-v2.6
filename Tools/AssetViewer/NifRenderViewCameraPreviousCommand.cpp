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

// NifRenderViewCameraPreviousCommand.cpp

#include "stdafx.h"
#include "NifRenderViewCameraPreviousCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
bool CNifRenderViewCameraPreviousCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        m_pkView = pkView;
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            CNifCameraList* pkCamList = pkDoc->GetCameraList();
            NiCameraInfoIndex kIndex = pkView->GetCurrentCameraIndices();
            NiCameraInfo* pkInfo = pkDoc->GetCameraInfo(kIndex);
            //No camera == error
            if(!pkInfo || !pkCamList)
                return false;

            bool bEnteredLoop = false;
            while(kIndex.uiCameraIndex == 0 &&
                  kIndex.uiRootIndex != 0)
            {
                kIndex.uiRootIndex--;
                kIndex.uiCameraIndex = 
                    pkCamList->NumCamerasForRoot(kIndex.uiRootIndex) - 1;
                bEnteredLoop = true;
            }

            if(kIndex.uiCameraIndex == (unsigned int) -1)
            {
                kIndex.uiCameraIndex = 0;
                kIndex.uiRootIndex = 0;
            }
            else if(!bEnteredLoop && kIndex.uiCameraIndex != 0)
                kIndex.uiCameraIndex--;
            else if(!bEnteredLoop)
            {
                kIndex.uiRootIndex = pkDoc->GetNumberOfRoots() - 1;
                while(pkCamList->NumCamerasForRoot(kIndex.uiRootIndex) == 0)
                    kIndex.uiRootIndex--;
                kIndex.uiCameraIndex = 
                    pkCamList->NumCamerasForRoot(kIndex.uiRootIndex) - 1;
            }

            pkInfo = pkDoc->GetCameraInfo(kIndex);
            if(pkInfo)
            {
                NiOutputDebugString("\n\nSelected Camera: ");
                NiOutputDebugString(pkInfo->m_spCam->GetName());
                NiOutputDebugString("\n");
                
                pkView->SetCurrentCameraIndices(kIndex);
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
CNifRenderViewCameraPreviousCommand::CNifRenderViewCameraPreviousCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraPreviousCommand::~CNifRenderViewCameraPreviousCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
