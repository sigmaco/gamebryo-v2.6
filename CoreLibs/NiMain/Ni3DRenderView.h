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

#ifndef NI3DRENDERVIEW_H
#define NI3DRENDERVIEW_H

#include "NiRenderView.h"
#include "NiCullingProcess.h"

class NIMAIN_ENTRY Ni3DRenderView : public NiRenderView
{
    NiDeclareRTTI;

public:
    Ni3DRenderView(NiCamera* pkCamera = NULL,
        NiCullingProcess* pkCullingProcess = NULL,
        bool bAlwaysUseCameraViewport = true);

    // Function that sets the renderer camera data using the stored camera.
    virtual void SetCameraData(const NiRect<float>& kViewport);

    // Functions for accessing the camera to use when rendering.
    inline void SetCamera(NiCamera* pkCamera);
    inline NiCamera* GetCamera() const;

    // Functions for accessing the culling process to use when rendering.
    inline void SetCullingProcess(NiCullingProcess* pkCullingProcess);
    inline NiCullingProcess* GetCullingProcess() const;

    // Functions for accessing whether or not the SetCameraData function should
    // always use the viewport of the stored camera or the viewport passed
    // into that function.
    inline void SetAlwaysUseCameraViewport(bool bAlwaysUseCameraViewport);
    inline bool GetAlwaysUseCameraViewport() const;

    // Functions for accessing the list of scenes.
    inline void AppendScene(NiAVObject* pkScene);
    inline void PrependScene(NiAVObject* pkScene);
    inline void RemoveScene(NiAVObject* pkScene);
    inline void RemoveAllScenes();
    inline NiTPointerList<NiAVObjectPtr>& GetScenes();
    inline const NiTPointerList<NiAVObjectPtr>& GetScenes() const;

protected:
    // Function that adds computes the array of potentially visible geometry.
    virtual void CalculatePVGeometry();

    // Recursive helper function to add all NiRenderObjects in the
    // specified scene graph to the potentially visible geometry array.
    void AddToPVGeometryArray(NiAVObject* pkObject);

    // The camera to use when rendering.
    NiCameraPtr m_spCamera;

    // The culling process to use when rendering.
    NiCullingProcessPtr m_spCullingProcess;

    // The list of scenes to render.
    NiTPointerList<NiAVObjectPtr> m_kScenes;

    // Indicates whether or not the SetCameraData function should always use
    // the viewport of m_spCamera or the viewport passed into that function.
    bool m_bAlwaysUseCameraViewport;
};

NiSmartPointer(Ni3DRenderView);

#include "Ni3DRenderView.inl"

#endif  // #ifndef NI3DRENDERVIEW_H
