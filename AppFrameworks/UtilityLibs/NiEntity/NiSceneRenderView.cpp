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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiSceneRenderView.h"

NiImplementRTTI(NiSceneRenderView, NiRenderView);

//---------------------------------------------------------------------------
void NiSceneRenderView::SetCameraData(const NiRect<float>& kViewport)
{
    NIASSERT(m_spCamera);

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Get pointer to viewport to use when setting camera data.
    const NiRect<float>* pkViewportToUse = NULL;
    if (m_bAlwaysUseCameraViewport)
    {
        pkViewportToUse = &m_spCamera->GetViewPort();
    }
    else
    {
        pkViewportToUse = &kViewport;
    }

    // Set camera data on renderer.
    pkRenderer->SetCameraData(m_spCamera->GetWorldLocation(),
        m_spCamera->GetWorldDirection(), m_spCamera->GetWorldUpVector(),
        m_spCamera->GetWorldRightVector(), m_spCamera->GetViewFrustum(),
        *pkViewportToUse);

}
//---------------------------------------------------------------------------
void NiSceneRenderView::CalculatePVGeometry()
{
    // If there are no scenes, do nothing.
    if (m_kScenes.GetSize() == 0)
    {
        return;
    }

    // Prepare rendering context.
    m_spRenderingContext->m_pkCamera = m_spCamera;
    m_spRenderingContext->m_pkCullingProcess = m_spCullingProcess;

    NIASSERT(m_kCachedPVGeometry.GetCount() == 0);

    // Backup and replace the visible set on the culling process.
    NiVisibleArray* pkOrigVisibleSet = m_spCullingProcess->GetVisibleSet();
    m_spCullingProcess->SetVisibleSet(&m_kCachedPVGeometry);

    // Iterate over the list of scenes, culling each one.
    NiTListIterator kIter = m_kScenes.GetHeadPos();
    while (kIter)
    {
        NiScene* pkScene = m_kScenes.GetNext(kIter);
        NIASSERT(pkScene);

        pkScene->BuildVisibleSet(m_spRenderingContext, m_spErrorHandler);
    }

    // Replace the original visible set on the culling process.
    m_spCullingProcess->SetVisibleSet(pkOrigVisibleSet);
}
//---------------------------------------------------------------------------
