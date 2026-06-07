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

#include "CubeMapCamera.h"

#include <NiCamera.h>
#include <NiRenderer.h>
#include <NiRenderTargetGroup.h>
#include <NiDrawSceneUtility.h>

//---------------------------------------------------------------------------
CubeMapCamera* CubeMapCamera::Create(NiRenderedCubeMap* pkMap, 
    NiRenderer* pkRenderer, NiNode* pkScene, NiAVObject* pkReference)
{
    if (!(pkMap && pkRenderer && pkScene && pkReference))
        return NULL;

    CubeMapCamera* pkThis = NiNew CubeMapCamera(pkScene);

    pkThis->m_spCamera = NiNew NiCamera;
    NiFrustum kFr = pkThis->m_spCamera->GetViewFrustum();
    kFr.m_fLeft = -1.0f;
    kFr.m_fRight = 1.0f;
    kFr.m_fBottom = -1.0f;
    kFr.m_fTop = 1.0f;
    kFr.m_fNear = 1.0f;
    kFr.m_fFar = 10000.0f;
    pkThis->m_spCamera->SetViewFrustum(kFr);

    pkThis->m_spRenderer = pkRenderer;

    pkThis->m_spRenderedTexture = pkMap;
    
    for (unsigned int ui = 0; ui < NiRenderedCubeMap::FACE_NUM; ui++)
    {
        pkThis->m_aspRenderTargetGroups[ui] = NiRenderTargetGroup::Create(
            pkThis->m_spRenderedTexture->GetFaceBuffer(
            (NiRenderedCubeMap::FaceID)ui), pkThis->m_spRenderer, true, true);
        NIASSERT(pkThis->m_aspRenderTargetGroups[ui]);
    }

    pkThis->m_spReference = pkReference;

    pkThis->m_uiCamerasPerUpdate = 6;
    pkThis->m_uiLastUpdatedCamera = 5;

    return pkThis;
}
//---------------------------------------------------------------------------
CubeMapCamera::CubeMapCamera(NiNode* pkScene)
    :
    m_spScene(pkScene),
    m_kVisible(1024, 1024),
    m_kCuller(&m_kVisible, NULL),
    m_uiNumObjectsDrawn(0),
    m_fCullTime(0.0f),
    m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
CubeMapCamera::~CubeMapCamera()
{
}
//---------------------------------------------------------------------------
unsigned int CubeMapCamera::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float CubeMapCamera::GetCullTime() const
{
    return m_fCullTime;
}
//---------------------------------------------------------------------------
float CubeMapCamera::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
bool CubeMapCamera::SetOutputRenderTargetGroup(
    NiRenderTargetGroup* pkOutputRenderTargetGroup)
{
    // This render step does not have an output render target group and thus
    // does not allow it to be set.
    return false;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* CubeMapCamera::GetOutputRenderTargetGroup()
{
    // This render step does not have an output render target group and thus
    // does not allow it to be retrieved.
    return NULL;
}
//---------------------------------------------------------------------------
void CubeMapCamera::PerformRendering()
{
    m_spCamera->SetTranslate(m_spReference->GetWorldTranslate());

    m_spRenderer->SetLeftRightSwap(true);

    // Reset rendering statistics.
    m_uiNumObjectsDrawn = 0;
    m_fCullTime = m_fRenderTime = 0.0f;

    unsigned int i;
    for (i = 0; i < m_uiCamerasPerUpdate; i++)
    {
        m_uiLastUpdatedCamera++;

        if (m_uiLastUpdatedCamera >= NiRenderedCubeMap::FACE_NUM)
            m_uiLastUpdatedCamera = 0;

        NiMatrix3 kMat;

        // Orient the camera
        switch((NiRenderedCubeMap::FaceID)m_uiLastUpdatedCamera)
        {
            case NiRenderedCubeMap::FACE_POS_X:
                kMat.SetRow(0, 1.0f, 0.0f, 0.0f);
                kMat.SetRow(1, 0.0f, 1.0f, 0.0f);
                kMat.SetRow(2, 0.0f, 0.0f, 1.0f);
                break;
            case NiRenderedCubeMap::FACE_NEG_X:
                kMat.SetRow(0, -1.0f, 0.0f, 0.0f);
                kMat.SetRow(1, 0.0f, 1.0f, 0.0f);
                kMat.SetRow(2, 0.0f, 0.0f, -1.0f);
                break;
            case NiRenderedCubeMap::FACE_POS_Y:
                kMat.SetRow(0, 0.0f, 0.0f, -1.0f);
                kMat.SetRow(1, 1.0f, 0.0f, 0.0f);
                kMat.SetRow(2, 0.0f, -1.0f, 0.0f);
                break;
            case NiRenderedCubeMap::FACE_NEG_Y:
                kMat.SetRow(0, 0.0f, 0.0f, -1.0f);
                kMat.SetRow(1, -1.0f, 0.0f, 0.0f);
                kMat.SetRow(2, 0.0f, 1.0f, 0.0f);
                break;
            case NiRenderedCubeMap::FACE_POS_Z:
                kMat.SetRow(0, 0.0f, 0.0f, -1.0f);
                kMat.SetRow(1, 0.0f, 1.0f, 0.0f);
                kMat.SetRow(2, 1.0f, 0.0f, 0.0f);
                break;
            case NiRenderedCubeMap::FACE_NEG_Z:
                kMat.SetRow(0, 0.0f, 0.0f, 1.0f);
                kMat.SetRow(1, 0.0f, 1.0f, 0.0f);
                kMat.SetRow(2, -1.0f, 0.0f, 0.0f);
                break;
            case NiRenderedCubeMap::FACE_NUM:
                break;
        }

        m_spCamera->SetRotate(kMat);
        m_spCamera->Update(0.0f);

        // Set the correct cube face and render target
        m_spRenderedTexture->SetCurrentCubeFace(
            (NiRenderedCubeMap::FaceID)m_uiLastUpdatedCamera);
   
        m_spRenderer->BeginUsingRenderTargetGroup(
            m_aspRenderTargetGroups[m_uiLastUpdatedCamera],
            NiRenderer::CLEAR_ALL);

        // Render the reflection
        NiVisibleArray* pkVisibleSet = m_kCuller.GetVisibleSet();
        if (pkVisibleSet)
        {
            // Cull scene.
            float fBeginTime = NiGetCurrentTimeInSec();
            NiCullScene(m_spCamera, m_spScene, m_kCuller, *pkVisibleSet);
            m_fCullTime += NiGetCurrentTimeInSec() - fBeginTime;

            // Render scene.
            fBeginTime = NiGetCurrentTimeInSec();
            NiDrawVisibleArray(m_spCamera, *pkVisibleSet);
            m_fRenderTime += NiGetCurrentTimeInSec() - fBeginTime;
            m_uiNumObjectsDrawn += pkVisibleSet->GetCount();
        }

        m_spRenderer->EndUsingRenderTargetGroup();
    }

    m_spRenderer->SetLeftRightSwap(false);
}
//---------------------------------------------------------------------------

