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

#ifndef CUBEMAPCAMERA_H
#define CUBEMAPCAMERA_H

#include <NiSystem.h>
#include <NiRenderStep.h>
#include <NiRenderedCubeMap.h>
#include <NiMeshCullingProcess.h>
#include <NiNode.h>
#include <NiRenderTargetGroup.h>

NiSmartPointer(NiCamera);
NiSmartPointer(NiAVObject);
NiSmartPointer(NiRenderer);

class CubeMapCamera : public NiRenderStep
{
public:
    static CubeMapCamera* Create(NiRenderedCubeMap* pkMap, 
        NiRenderer* pkRenderer, NiNode* pkScene, NiAVObject* pkReference);

    CubeMapCamera(NiNode* pkScene);
    ~CubeMapCamera();

    unsigned int GetCamerasPerUpdate();
    void SetCamerasPerUpdate(unsigned int uiNum);

	// NOTE: NiRenderedCubeMap pixel format appears to be uninitialized
    const NiPixelFormat* GetPixelFormat( NiRenderedCubeMap::FaceID eFaceID)
    {
		return m_aspRenderTargetGroups[eFaceID]->GetPixelFormat(0);
    }

    // Functions for getting and setting the render target group that will
    // contain the final output for this render step.
    virtual bool SetOutputRenderTargetGroup(
        NiRenderTargetGroup* pkOutputRenderTargetGroup);
    virtual NiRenderTargetGroup* GetOutputRenderTargetGroup();

    // Functions for reporting statistics about the most recent frame.
    virtual unsigned int GetNumObjectsDrawn() const;
    virtual float GetCullTime() const;
    virtual float GetRenderTime() const;

    virtual void PerformRendering();

protected:

    NiCameraPtr m_spCamera;

    NiRendererPtr m_spRenderer;

    NiRenderedCubeMapPtr m_spRenderedTexture;
    NiRenderTargetGroupPtr 
        m_aspRenderTargetGroups[NiRenderedCubeMap::FACE_NUM];
    
    NiAVObjectPtr m_spReference;

    unsigned int m_uiCamerasPerUpdate;
    unsigned int m_uiLastUpdatedCamera;

    NiNodePtr m_spScene;
    NiVisibleArray m_kVisible;
    NiMeshCullingProcess m_kCuller;

    // Rendering statistics.
    unsigned int m_uiNumObjectsDrawn;
    float m_fCullTime;
    float m_fRenderTime;
};

NiSmartPointer(CubeMapCamera);

//---------------------------------------------------------------------------
inline unsigned int CubeMapCamera::GetCamerasPerUpdate()
{
    return m_uiCamerasPerUpdate;
}
//---------------------------------------------------------------------------
inline void CubeMapCamera::SetCamerasPerUpdate(unsigned int uiNum)
{
    m_uiCamerasPerUpdate = (uiNum <= 6) ? uiNum : 6;
}
//---------------------------------------------------------------------------
#endif // CUBEMAPCAMERA_H
