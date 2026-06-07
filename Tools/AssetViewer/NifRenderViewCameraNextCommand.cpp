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

// NifRenderViewCameraNextCommand.cpp

#include "stdafx.h"
#include "NifRenderViewCameraNextCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
bool CNifRenderViewCameraNextCommand::Execute(CNifRenderView* pkView)
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
            bool bKeepLooping = true;
            while(kIndex.uiCameraIndex+1 == 
                  pkCamList->NumCamerasForRoot(kIndex.uiRootIndex) &&
                  kIndex.uiRootIndex <= pkDoc->GetNumberOfRoots() &&
                  bKeepLooping)
            {
                if(bEnteredLoop && 
                   pkCamList->NumCamerasForRoot(kIndex.uiRootIndex) == 1)
                    bKeepLooping = false;
                else
                {
                    kIndex.uiCameraIndex = 0;
                    kIndex.uiRootIndex++;
                    bEnteredLoop = true;
                }
            }

            if(kIndex.uiRootIndex >= pkDoc->GetNumberOfRoots())
            {
                kIndex.uiCameraIndex = 0;
                kIndex.uiRootIndex = 0;
            }
            else if(kIndex.uiRootIndex <= pkDoc->GetNumberOfRoots() && 
                !bEnteredLoop)
                    kIndex.uiCameraIndex++;

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
CNifRenderViewCameraNextCommand::CNifRenderViewCameraNextCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
CNifRenderViewCameraNextCommand::~CNifRenderViewCameraNextCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
