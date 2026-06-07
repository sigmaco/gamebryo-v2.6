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

#ifndef NIDEBUGGEOMETRYCLICK_H
#define NIDEBUGGEOMETRYCLICK_H

#include <NiRenderClick.h>
#include <NiImmediateModeAdapter.h>

NiSmartPointer(NiCamera);
NiSmartPointer(NiLight);

class NiDebugGeometryClick : public NiRenderClick
{
    NiDeclareRTTI;
    NiDeclareFlags(unsigned char);
public:
    NiDebugGeometryClick();

    void SetCamera(NiCamera* pkCamera);
    const NiCamera* GetCamera() const;

    float GetScreenPercent() const;
    void SetScreenPercent(float fScreenPercent);

    void ExtendCameraNearAndFar(NiCamera* pkCamera) const;

    virtual unsigned int GetNumObjectsDrawn() const;
    virtual float GetCullTime() const;
    virtual float GetRenderTime() const;

    // flags
    enum
    {
        POINT_LIGHT_MASK       = 0x01,
        DIRECTIONAL_LIGHT_MASK = 0x02,
        SPOT_LIGHT_MASK        = 0x04,
        LIGHT_MASK             = 0x07,
        CAMERA_MASK            = 0x08,
        ALL_MASK               = 0x0F
    };

    // Remove debug geometry
    void ClearScene(unsigned int uiClearMask = ALL_MASK);
    // Adds debug geometry for objects in the scene
    void ProcessScene(NiAVObject* pkObject, 
        unsigned int uiProcessMask = ALL_MASK);

protected:
    virtual void PerformRendering(unsigned int uiFrameID);
    static void ExpandFrustumNearFar(NiCamera* pkCamera,
        NiFrustum& kFrust, NiPoint3& kCenter, float fRadius);

    float m_fCullTime;
    float m_fRenderTime;
    unsigned int m_uiNumObjectsDrawn;
    float m_fScreenPercent;

    NiTObjectSet<NiCameraPtr> m_kCameras;
    NiTObjectSet<NiLightPtr> m_kLights;

    NiImmediateModeAdapter m_kAdapter;
};

NiSmartPointer(NiDebugGeometryClick);

#endif  // #ifndef NIDEBUGGEOMETRYCLICK_H
