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

// NifRenderViewResetFrustumCommand.cpp

#include "stdafx.h"
#include "NifRenderViewResetFrustumCommand.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
bool CNifRenderViewResetFrustumCommand::Execute(CNifRenderView* pkView)
{
    if(pkView)
    {
        m_pkView = pkView;
        CNifDoc* pkDoc = pkView->GetDocument();
        if(pkDoc)
        {
            NiCameraInfo* pkInfo = 
                pkDoc->GetCameraInfo(pkView->GetCurrentCameraIndices());
            //No camera == error
            if(!pkInfo)
                return false;

            m_spCamera = pkInfo->m_spCam;
            NIASSERT(m_spCamera);

            NiFrustum kFrustum = m_spCamera->GetViewFrustum();
            m_fNearDist = kFrustum.m_fNear;
            m_fFarDist = kFrustum.m_fFar;

            if(pkView->GetCurrentCameraIndices().uiRootIndex == 0)
                m_bIsDefaultCamera = true;
            else
                m_bIsDefaultCamera = false;

            m_spScene = pkDoc->GetSceneGraph();
            CalcCameraFrustum();

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
CNifRenderViewResetFrustumCommand::CNifRenderViewResetFrustumCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
CNifRenderViewResetFrustumCommand::~CNifRenderViewResetFrustumCommand()
{
    m_spCamera = NULL;
    m_spScene = NULL;
    m_pkView = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewResetFrustumCommand::CalcCameraFrustum()
{
    if(!m_spCamera || !m_spScene)
        return;

    float fSceneRadius = m_spScene->GetWorldBound().GetRadius();

    float fRight, fTop, fLeft, fBottom, fFar, fNear;

    m_spCamera->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spCamera);
    NiFrustum kFrustum = m_spCamera->GetViewFrustum();
 
    RECT rect;
    m_pkView->GetClientRect(&rect);
    
    if (kFrustum.m_bOrtho)
    {
        NiFrustum fr = kFrustum;
        m_spCamera->SetViewFrustum(fr);
    }
    else
    {       
        float fWindowAspectRatio = ((float) rect.bottom / (float) rect.right);
        fRight = kFrustum.m_fRight;
        fLeft = kFrustum.m_fLeft;
        fTop = fRight * fWindowAspectRatio;
        fBottom = -fTop;
        fFar = kFrustum.m_fFar;
        fNear = kFrustum.m_fNear;

        if(m_bIsDefaultCamera)
        {
            fFar = fSceneRadius * 4.0f;
            fNear = 0.01f;
            NiPoint3 kCameraLocation = m_spCamera->GetTranslate();
            NiPoint3 kWorldCenter = (m_spScene->GetWorldBound()).GetCenter();
            NiPoint3 kDistanceVector = kCameraLocation - kWorldCenter;
            float fDist = kDistanceVector.Length();
            fFar += fDist;
            fFar > 0.0f ? 0 : fFar = 5.0f;
        } 


        NiFrustum fr;
        fr.m_fLeft = fLeft;
        fr.m_fRight = fRight;
        fr.m_fTop = fTop;
        fr.m_fBottom = fBottom;
        fr.m_fNear = fNear;
        fr.m_fFar = fFar; 
        m_spCamera->SetViewFrustum(fr);
    }

}
//---------------------------------------------------------------------------
