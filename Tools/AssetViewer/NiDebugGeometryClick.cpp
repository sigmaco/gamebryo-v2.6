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

#include "NiDebugGeometryClick.h"
#include <NiImmediateModeMacro.h>
#include <NiNode.h>
#include <NiCamera.h>
#include <NiAmbientLight.h>
#include <NiDirectionalLight.h>
#include <NiSpotLight.h>

NiImplementRTTI(NiDebugGeometryClick, NiRenderClick);

//---------------------------------------------------------------------------
NiDebugGeometryClick::NiDebugGeometryClick() :
    m_fCullTime(0.0f), m_fRenderTime(0.0f), m_uiNumObjectsDrawn(0),
    m_fScreenPercent(0.05f)
{

}
//---------------------------------------------------------------------------
float NiDebugGeometryClick::GetScreenPercent() const
{
    return m_fScreenPercent;
}
//---------------------------------------------------------------------------
void NiDebugGeometryClick::SetScreenPercent(float fScreenPercent)
{
    m_fScreenPercent = fScreenPercent;
}
//---------------------------------------------------------------------------
void NiDebugGeometryClick::SetCamera(NiCamera* pkCamera)
{
    m_kAdapter.SetCurrentCamera(pkCamera);
}
//---------------------------------------------------------------------------
const NiCamera* NiDebugGeometryClick::GetCamera() const
{
    return m_kAdapter.GetCurrentCamera();
}
//---------------------------------------------------------------------------
unsigned int NiDebugGeometryClick::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float NiDebugGeometryClick::GetCullTime() const
{
    return m_fCullTime;
}
//---------------------------------------------------------------------------
float NiDebugGeometryClick::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void NiDebugGeometryClick::ClearScene(unsigned int uiClearMask)
{
    if (uiClearMask & CAMERA_MASK)
    {
        m_kCameras.RemoveAll();
    }

    if ((uiClearMask & LIGHT_MASK) == LIGHT_MASK)
    {
        m_kLights.RemoveAll();
    }
    else if (uiClearMask & LIGHT_MASK)
    {
        for (unsigned int i = 0; i < m_kLights.GetSize(); i++)
        {
            NiLight* pkLight = m_kLights.GetAt(i);
            if (NiIsKindOf(NiSpotLight, pkLight))
            {
                if (uiClearMask & SPOT_LIGHT_MASK)
                    m_kLights.RemoveAt(i);
            }
            else if (NiIsKindOf(NiPointLight, pkLight))
            {
                if (uiClearMask & POINT_LIGHT_MASK)
                    m_kLights.RemoveAt(i);
            }
            else if (NiIsKindOf(NiDirectionalLight, pkLight))
            {
                if (uiClearMask & DIRECTIONAL_LIGHT_MASK)
                    m_kLights.RemoveAt(i);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiDebugGeometryClick::ProcessScene(NiAVObject* pkObject,
    unsigned int uiProcessMask)
{
    if (!pkObject)
    {
        return;
    }
    else if (pkObject->IsNode())
    {
        NiNode* pkNode = NiVerifyStaticCast(NiNode, pkObject);
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
            ProcessScene(pkNode->GetAt(i), uiProcessMask);
    }
    else if (NiIsKindOf(NiCamera, pkObject))
    {
        if (uiProcessMask & CAMERA_MASK)
            m_kCameras.Add((NiCamera*)pkObject);
    }
    else if (NiIsKindOf(NiLight, pkObject) && uiProcessMask & LIGHT_MASK)
    {
        if (NiIsKindOf(NiSpotLight, pkObject))
        {
            if (uiProcessMask & SPOT_LIGHT_MASK)
                m_kLights.Add((NiLight*)pkObject);
        }
        else if (NiIsKindOf(NiPointLight, pkObject))
        {
            if (uiProcessMask & POINT_LIGHT_MASK)
                m_kLights.Add((NiLight*)pkObject);
        }
        else if (NiIsKindOf(NiDirectionalLight, pkObject))
        {
            if (uiProcessMask & DIRECTIONAL_LIGHT_MASK)
                m_kLights.Add((NiLight*)pkObject);
        }
    }
}
//---------------------------------------------------------------------------
 void NiDebugGeometryClick::PerformRendering(unsigned int uiFrameID)
{
    m_fRenderTime = m_fCullTime = 0.0f;
    m_uiNumObjectsDrawn = 0;

    if (!m_kAdapter.GetCurrentCamera())
        return;

    float fBeginTime = NiGetCurrentTimeInSec();

    {
        NiImmediateModeMacro kMacro(m_kAdapter);

        // Colors
        const NiColorA kCameraColor(0.1098f, 0.3490f, 0.6941f, 1.0f);
        const NiColorA kPointLightColor(1.0f, 0.9216f, 0.0275f, 1.0f);
        const NiColorA kDirectionalLightColor(1.0f, 0.9216f, 0.0275f, 1.0f);
        const NiColorA kSpotLightColor(1.0f, 0.9216f, 0.0275f, 1.0f);

        // Draw cameras
        {
            m_kAdapter.SetCurrentColor(kCameraColor);
            unsigned int uiNumCameras = m_kCameras.GetSize();
            for (unsigned int i = 0; i < uiNumCameras; i++)
            {
                NiCamera* pkCamera = m_kCameras.GetAt(i);
                if (pkCamera->GetAppCulled() ||
                    pkCamera == m_kAdapter.GetCurrentCamera())
                {
                    continue;
                }

                float fScreenScale = m_fScreenPercent * 
                    kMacro.GetScreenScaleFactor(pkCamera);

                kMacro.WireCamera(pkCamera, fScreenScale);
                kMacro.WireFrustum(pkCamera);
            }
        }
        
        // Draw lights
        {
            unsigned int uiNumLights = m_kLights.GetSize();
            for (unsigned int i = 0; i < uiNumLights; i++)
            {
                NiLight* pkLight = m_kLights.GetAt(i);
                if (pkLight->GetAppCulled())
                    continue;

                float fScreenScale = m_fScreenPercent * 
                    kMacro.GetScreenScaleFactor(pkLight);
                
                // NOTE: check NiSpotLight before NiPointLight
                if (NiIsKindOf(NiSpotLight, pkLight))
                {
                    NiSpotLight* pkSLight = NiVerifyStaticCast(NiSpotLight, 
                        pkLight);

                    m_kAdapter.SetCurrentColor(kSpotLightColor);
                    kMacro.WireSpotLight(NiVerifyStaticCast(
                        NiSpotLight, pkLight), fScreenScale);
                }
                else if (NiIsKindOf(NiPointLight, pkLight))
                {
                    m_kAdapter.SetCurrentColor(kPointLightColor);
                    kMacro.WirePointLight(NiVerifyStaticCast(
                        NiPointLight, pkLight), fScreenScale);
                }
                else if (NiIsKindOf(NiDirectionalLight, pkLight))
                {
                    m_kAdapter.SetCurrentColor(kDirectionalLightColor);
                    kMacro.WireDirectionalLight(NiVerifyStaticCast(
                        NiDirectionalLight, pkLight), fScreenScale);
                }
            }
        }

        // Let the NiImmediateModeMacro fall out of local scope...
    }

    // Resize adapter...
    unsigned int uiUsedVerts = m_kAdapter.GetNumVerticesUsed();
    unsigned int uiUsedIndices = m_kAdapter.GetNumIndicesUsed();
    if (uiUsedVerts > m_kAdapter.GetVertexBufferSize())
        m_kAdapter.SetNumMaxVertices(uiUsedVerts);
    if (uiUsedIndices > m_kAdapter.GetIndexBufferSize())
        m_kAdapter.SetNumMaxIndices(uiUsedIndices);

    m_uiNumObjectsDrawn = (uiUsedVerts + uiUsedIndices > 0 ? 1 : 0);
    m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;
}
//---------------------------------------------------------------------------
inline void NiDebugGeometryClick::ExpandFrustumNearFar(NiCamera* pkCamera,
    NiFrustum& kFrust, NiPoint3& kCenter, float fRadius)
{
    float fDistToCenter = (kCenter - pkCamera->GetWorldTranslate()).Dot(
        pkCamera->GetWorldDirection());

    kFrust.m_fNear = NiMin(kFrust.m_fNear, fDistToCenter - fRadius);
    kFrust.m_fFar = NiMax(kFrust.m_fFar, fDistToCenter + fRadius);
}
//---------------------------------------------------------------------------
void NiDebugGeometryClick::ExtendCameraNearAndFar(NiCamera* pkCamera) const
{
    NiBound kBound;
    NiFrustum kFrust = pkCamera->GetViewFrustum();

    // Fit cameras
    {
        unsigned int uiNumCameras = m_kCameras.GetSize();
        for (unsigned int i = 0; i < uiNumCameras; i++)
        {
            NiCamera* pkDebugCamera = m_kCameras.GetAt(i);
            if (pkDebugCamera->GetAppCulled() ||
                pkDebugCamera == pkCamera)
            {
                continue;
            }

            float fScreenScale = m_fScreenPercent * 
                NiImmediateModeMacro::GetScreenScaleFactor(pkDebugCamera, 
                &m_kAdapter);

            NiPoint3 kCenter = pkDebugCamera->GetWorldTranslate();
            ExpandFrustumNearFar(pkCamera, kFrust, kCenter, fScreenScale);

            // Fit debug camera frustum - just check the four far plane points
            NiPoint3 akPoint[4];
            const NiTransform& kTrans = pkDebugCamera->GetWorldTransform();
            const NiFrustum& kDebugFrust = pkDebugCamera->GetViewFrustum();
            if (kFrust.m_bOrtho)
            {
                NiPoint3 kViewTL = kTrans.m_Rotate * NiPoint3(0.0f, 
                    kDebugFrust.m_fTop, kDebugFrust.m_fLeft);
                NiPoint3 kViewTR = kTrans.m_Rotate * NiPoint3(0.0f, 
                    kDebugFrust.m_fTop, kDebugFrust.m_fRight);
                NiPoint3 kViewBL = kTrans.m_Rotate * NiPoint3(0.0f, 
                    kDebugFrust.m_fBottom, kDebugFrust.m_fLeft);
                NiPoint3 kViewBR = kTrans.m_Rotate * NiPoint3(0.0f, 
                    kDebugFrust.m_fBottom, kDebugFrust.m_fRight);

                NiPoint3 kFar = pkDebugCamera->GetWorldDirection() * 
                    kDebugFrust.m_fFar;
                akPoint[0] = kTrans.m_Translate + kViewBL + kFar;
                akPoint[1] = kTrans.m_Translate + kViewTL + kFar;
                akPoint[2] = kTrans.m_Translate + kViewTR + kFar;
                akPoint[3] = kTrans.m_Translate + kViewBR + kFar;
            }
            else
            {
                NiPoint3 kViewTL = kTrans.m_Rotate * NiPoint3(1.0f, 
                    kDebugFrust.m_fTop, kDebugFrust.m_fLeft);
                NiPoint3 kViewTR = kTrans.m_Rotate * NiPoint3(1.0f, 
                    kDebugFrust.m_fTop, kDebugFrust.m_fRight);
                NiPoint3 kViewBL = kTrans.m_Rotate * NiPoint3(1.0f, 
                    kDebugFrust.m_fBottom, kDebugFrust.m_fLeft);
                NiPoint3 kViewBR = kTrans.m_Rotate * NiPoint3(1.0f,
                    kDebugFrust.m_fBottom, kDebugFrust.m_fRight);

                akPoint[0] = kTrans.m_Translate + kViewBL * kDebugFrust.m_fFar;
                akPoint[1] = kTrans.m_Translate + kViewTL * kDebugFrust.m_fFar;
                akPoint[2] = kTrans.m_Translate + kViewTR * kDebugFrust.m_fFar;
                akPoint[3] = kTrans.m_Translate + kViewBR * kDebugFrust.m_fFar;
            }

            ExpandFrustumNearFar(pkCamera, kFrust, akPoint[0], 0.0f);
            ExpandFrustumNearFar(pkCamera, kFrust, akPoint[1], 0.0f);
            ExpandFrustumNearFar(pkCamera, kFrust, akPoint[2], 0.0f);
            ExpandFrustumNearFar(pkCamera, kFrust, akPoint[3], 0.0f);
        }
    }
    
    // Fit lights
    {
        unsigned int uiNumLights = m_kLights.GetSize();
        for (unsigned int i = 0; i < uiNumLights; i++)
        {
            NiLight* pkLight = m_kLights.GetAt(i);
            if (pkLight->GetAppCulled())
                continue;

            float fScreenScale = m_fScreenPercent * 
                NiImmediateModeMacro::GetScreenScaleFactor(pkCamera, 
                &m_kAdapter);
            
            NiPoint3 kCenter = pkLight->GetWorldTranslate();
            ExpandFrustumNearFar(pkCamera, kFrust, kCenter, fScreenScale);
        }
    }

    pkCamera->SetViewFrustum(kFrust);
}
//---------------------------------------------------------------------------