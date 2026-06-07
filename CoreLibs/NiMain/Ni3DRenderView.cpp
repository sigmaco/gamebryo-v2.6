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
#include "NiMainPCH.h"

#include "Ni3DRenderView.h"
#include "NiRenderer.h"
#include "NiRenderObject.h"
#include "NiNode.h"

NiImplementRTTI(Ni3DRenderView, NiRenderView);

//---------------------------------------------------------------------------
void Ni3DRenderView::SetCameraData(const NiRect<float>& kViewport)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    if (m_spCamera)
    {
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
    else
    {
        // If no camera, set screen space camera data.
        pkRenderer->SetScreenSpaceCameraData(&kViewport);
    }
}
//---------------------------------------------------------------------------
void Ni3DRenderView::CalculatePVGeometry()
{
    NIASSERT(m_kCachedPVGeometry.GetCount() == 0);

    // Iterate over the list of scenes, culling each one.
    NiTListIterator kIter = m_kScenes.GetHeadPos();
    while (kIter)
    {
        NiAVObject* pkScene = m_kScenes.GetNext(kIter);
        NIASSERT(pkScene);

        if (m_spCamera && m_spCullingProcess)
        {
            // Use culling process to cull the scene, building up the cached
            // PV geometry array in the process.
            m_spCullingProcess->Cull(m_spCamera, pkScene,
                &m_kCachedPVGeometry);
        }
        else
        {
            // No camera or no culling process has been specified. Render
            // scene without culling.
            AddToPVGeometryArray(pkScene);
        }
    }
}
//---------------------------------------------------------------------------
void Ni3DRenderView::AddToPVGeometryArray(NiAVObject* pkObject)
{
    NIASSERT(pkObject);

    if (NiIsKindOf(NiRenderObject, pkObject))
    {
        // Add geometry object to PV geometry array.
        m_kCachedPVGeometry.Add(*((NiRenderObject*) pkObject));
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        // Recurse over children.
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                AddToPVGeometryArray(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
