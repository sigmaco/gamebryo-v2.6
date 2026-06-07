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

// NifRenderViewZoomExtentsCommand.cpp

#include "stdafx.h"
#include "NifRenderViewZoomExtentsCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"
#include "NifRenderViewUIManager.h"
#include "NifRenderViewUIState.h"

//---------------------------------------------------------------------------
bool CNifRenderViewZoomExtentsCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        m_pkView = pkView;
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            NiCameraInfoIndex kIndex = pkView->GetCurrentCameraIndices();
            NiCameraInfo* pkInfo = 
                pkDoc->GetCameraInfo(kIndex);
            // No camera == error
            if(!pkInfo)
                return false;
            // Zooming out is useless in this case, but it isn't an error
            // per se
            else if(pkInfo->m_bIsAnimated)
                return true; 
            m_spCamera = pkInfo->m_spCam;

            if(m_eType == EXTENTS_ALL)
            {   
                m_spScene = pkDoc->GetSceneGraph();
                CalcCameraFrustum();
                CalcCameraTransform();
                if(kIndex.uiCameraIndex == 0 && kIndex.uiRootIndex == 0)
                {
                    m_spCamera->Update(0.0f);
                    NiMesh::CompleteSceneModifiers(m_spCamera);
                    pkInfo->m_kOriginalTranslation =
                        m_spCamera->GetWorldTranslate();
                    pkInfo->m_kOriginalRotation =
                        m_spCamera->GetWorldRotate();

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
            else if(m_eType == EXTENTS_SELECTED)
            {
                m_spScene = pkDoc->GetSceneGraph();
                // No support yet for selected objects
                return false;
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
CNifRenderViewZoomExtentsCommand::CNifRenderViewZoomExtentsCommand(
    ExtentsType eType)
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
    m_eType = eType;
}
//---------------------------------------------------------------------------
CNifRenderViewZoomExtentsCommand::~CNifRenderViewZoomExtentsCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewZoomExtentsCommand::CalcCameraTransform()
{
    if(!m_spCamera || !m_spScene)
        return;

    NiBound kSceneBound;
    NiPoint3 kSceneCenter, kInitialCameraPos;
    float fSceneRadius;

    kSceneBound = m_spScene->GetWorldBound();
    kSceneCenter = kSceneBound.GetCenter();
    fSceneRadius = kSceneBound.GetRadius();

    // Rotate and position the camera so that we are looking down the world
    // y-axis and z is up.

    kInitialCameraPos = NiPoint3(
        kSceneCenter.x, 
        kSceneCenter.y - 2.0f * fSceneRadius, 
        kSceneCenter.z
    );

    NiMatrix3 kRot(
        NiPoint3(0.0f, 1.0f, 0.0f),
        NiPoint3(0.0f, 0.0f, 1.0f),
        NiPoint3(1.0f, 0.0f, 0.0f));

    m_spCamera->SetRotate(kRot);
    m_spCamera->SetTranslate(kInitialCameraPos);
}
//---------------------------------------------------------------------------
void CNifRenderViewZoomExtentsCommand::CalcCameraFrustum()
{
    if(!m_spCamera || !m_spScene)
        return;

    float fSceneRadius = m_spScene->GetWorldBound().GetRadius();

    float fRight, fTop;

    fRight = fTop = 1.0f / NiSqrt(3.0f);

    float fFar = fSceneRadius * 4.0f;

    NiPoint3 kCameraLocation = m_spCamera->GetTranslate();
    NiPoint3 kWorldCenter = (m_spScene->GetWorldBound()).GetCenter();

    NiPoint3 kDistanceVector = kCameraLocation - kWorldCenter;
    float fDist = kDistanceVector.Length();

    fFar += fDist;

    fFar > 0.0f ? 0 : fFar = 5.0f;

    RECT rect;
    m_pkView->GetClientRect(&rect);

    if (rect.right > rect.bottom)
        fRight *= (float) rect.right / (float) rect.bottom;
    else
        fTop *= (float) rect.bottom / (float) rect.right;

    NiFrustum kOldFr = m_spCamera->GetViewFrustum();
    NiFrustum fr;
    if (kOldFr.m_bOrtho)
    {
        fr = kOldFr;
    }
    else
    {
        fr.m_fLeft = -fRight;
        fr.m_fRight = fRight;
        fr.m_fTop = fTop;
        fr.m_fBottom = -fTop;
        fr.m_fNear = 0.01f;
        fr.m_fFar =fFar; 
    }
    m_spCamera->SetViewFrustum(fr);

}
//---------------------------------------------------------------------------
