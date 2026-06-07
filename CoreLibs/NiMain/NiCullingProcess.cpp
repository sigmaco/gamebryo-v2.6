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

#include "NiMainPCH.h"
#include "NiCullingProcess.h"

NiImplementRootRTTI(NiCullingProcess);

//---------------------------------------------------------------------------
NiCullingProcess::NiCullingProcess(NiVisibleArray* pkVisibleSet, 
    NiSPWorkflowManager* pkWorkflowManager)
    :
    m_bUseVirtualAppend(false),
    m_kFrustum(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,false),
    m_bSubmitModifiers(true)
{
    m_pkVisibleSet = pkVisibleSet;
    m_pkWorkflowManager = pkWorkflowManager;
    m_pkCamera = 0;
}
//---------------------------------------------------------------------------
NiCullingProcess::NiCullingProcess(NiVisibleArray* pkVisibleSet, 
    NiSPWorkflowManager* pkWorkflowManager, bool bUseVirtualAppend)
    :
    m_bUseVirtualAppend(bUseVirtualAppend),
    m_kFrustum(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,false),
    m_bSubmitModifiers(true)
{
    m_pkVisibleSet = pkVisibleSet;
    m_pkWorkflowManager = pkWorkflowManager;
    m_pkCamera = 0;
}
//---------------------------------------------------------------------------
NiCullingProcess::~NiCullingProcess()
{
}
//---------------------------------------------------------------------------
void NiCullingProcess::Cull(const NiCamera* pkCamera, NiAVObject* pkScene,
        NiVisibleArray* pkVisibleSet)
{
    PreProcess();

    Process(pkCamera, pkScene, pkVisibleSet);

    PostProcess();
}
//---------------------------------------------------------------------------
void NiCullingProcess::Process(const NiCamera* pkCamera, NiAVObject* pkScene,
        NiVisibleArray* pkVisibleSet)
{
    NIASSERT(pkCamera && pkScene);
    if (!pkCamera || !pkScene)
        return;

    m_pkCamera = pkCamera;
    SetFrustum(m_pkCamera->GetViewFrustum());

    NiVisibleArray* pkSaveVisibleSet = NULL;
    if (pkVisibleSet)
    {
        pkSaveVisibleSet = m_pkVisibleSet;
        m_pkVisibleSet = pkVisibleSet;
    }

    NIASSERT(m_pkVisibleSet);
    if (m_pkVisibleSet && pkScene)
        pkScene->Cull(*this);

    if (pkVisibleSet)
        m_pkVisibleSet = pkSaveVisibleSet;

    m_pkCamera = 0;
}
//---------------------------------------------------------------------------
void NiCullingProcess::Process(NiAVObject* pkObject)
{
    if (!m_kPlanes.IsAnyPlaneActive())
    {
        pkObject->OnVisible(*this);
    }
    else
    {
        // Determine if the object is not visible by comparing its world
        // bound to each culling plane.
        unsigned int uiSaveActive = m_kPlanes.GetActivePlaneState();

        unsigned int i;
        for (i = 0; i < NiFrustumPlanes::MAX_PLANES; i++)
        {
            if (m_kPlanes.IsPlaneActive(i))
            {
                int iSide = pkObject->GetWorldBound().WhichSide(
                    m_kPlanes.GetPlane(i));

                if (iSide == NiPlane::NEGATIVE_SIDE)
                {
                    // The object is not visible since it is on the negative
                    // side of the plane.
                    break;
                }

                if (iSide == NiPlane::POSITIVE_SIDE)
                {
                    // The object is fully on the positive side of the plane,
                    // so there is no need to compare child objects to this
                    // plane.
                    m_kPlanes.DisablePlane(i);
                }
            }
        }

        if (i == NiFrustumPlanes::MAX_PLANES)
            pkObject->OnVisible(*this);

        m_kPlanes.SetActivePlaneState(uiSaveActive);
    }
}
//---------------------------------------------------------------------------
void NiCullingProcess::SetFrustum(const NiFrustum& kFrustum)
{
    NIASSERT(m_pkCamera);
    if (!m_pkCamera)
        return;

    m_kFrustum = kFrustum;

    // Do not use the version of Set that takes an NiCamera, as we need to
    // supply our own frustum.  Instead, we use the versions that takes a
    // transform and an external frustum
    m_kPlanes.Set(m_kFrustum, m_pkCamera->GetWorldTransform());

    m_kPlanes.EnableAllPlanes();
}
//---------------------------------------------------------------------------
