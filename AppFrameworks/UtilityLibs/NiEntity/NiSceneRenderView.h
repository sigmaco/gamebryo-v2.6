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

#ifndef NISCENERENDERVIEW_H
#define NISCENERENDERVIEW_H

#include "NiEntityLibType.h"
#include <NiRenderView.h>
#include <NiRenderer.h>
#include "NiEntityRenderingContext.h"
#include <NiCamera.h>
#include <NiCullingProcess.h>
#include "NiEntityErrorInterface.h"
#include "NiScene.h"
#include <NiTPointerList.h>

class NIENTITY_ENTRY NiSceneRenderView : public NiRenderView
{
    NiDeclareRTTI;

public:
    NiSceneRenderView(NiCamera* pkCamera, NiCullingProcess* pkCullingProcess,
        NiEntityErrorInterface* pkErrorHandler,
        bool bAlwaysUseCameraViewport = true);

    // Functions for accessing the camera.
    inline void SetCamera(NiCamera* pkCamera);
    inline NiCamera* GetCamera() const;

    // Functions for accessing the culling process.
    inline void SetCullingProcess(NiCullingProcess* pkCullingProcess);
    inline NiCullingProcess* GetCullingProcess() const;

    // Functions for accessing the error handler.
    inline void SetErrorHandler(NiEntityErrorInterface* pkErrorHandler);
    inline NiEntityErrorInterface* GetErrorHandler() const;

    // Functions for accessing whether or not to use the stored camera's
    // viewport in the SetCameraData function instead of the viewport passed
    // into that function.
    inline void SetAlwaysUseCameraViewport(bool bAlwaysUseCameraViewport);
    inline bool GetAlwaysUseCameraViewport() const;

    // Functions for accessing the list of scenes.
    inline void AppendScene(NiScene* pkScene);
    inline void PrependScene(NiScene* pkScene);
    inline void RemoveScene(NiScene* pkScene);
    inline void RemoveAllScenes();
    inline NiTPointerList<NiScenePtr>& GetScenes();
    inline const NiTPointerList<NiScenePtr>& GetScenes() const;

    // Function to use the provided viewport to set the camera data for the
    // renderer.
    virtual void SetCameraData(const NiRect<float>& kViewport);

protected:
    // Function to compute the array of potentially visible geometry.
    virtual void CalculatePVGeometry();

    // The rendering context to use when rendering the scene.
    NiEntityRenderingContextPtr m_spRenderingContext;
    NiCameraPtr m_spCamera;
    NiCullingProcessPtr m_spCullingProcess;

    // The error handler to use when reporting errors.
    NiEntityErrorInterfaceIPtr m_spErrorHandler;

    // The list of scenes to process.
    NiTPointerList<NiScenePtr> m_kScenes;

    // Whether or not the SetCameraData function should always use the viewport
    // of the stored camera instead of the viewport passed into that function.
    bool m_bAlwaysUseCameraViewport;
};

NiSmartPointer(NiSceneRenderView);

#include "NiSceneRenderView.inl"

#endif  // #ifndef NISCENERENDERVIEW_H
