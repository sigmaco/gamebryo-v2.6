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


#ifndef NIFRENDERVIEWCAMERABASESTATE_H
#define NIFRENDERVIEWCAMERABASESTATE_H


#include "NifRenderViewBufferedUIState.h"
#include "NifCameraList.h"
#include "NiSmoothCameraController.h"


class CNifRenderViewCameraBaseState : public CNifRenderViewBufferedUIState
{
public:
    CNifRenderViewCameraBaseState(CNifRenderView* pkView);
    
    virtual void Initialize();
    virtual void Update();

    virtual void OnUpdate(UINT Msg);
    virtual bool CanExecuteCommand(char* pcCommandID);

protected:
    virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint);

    virtual void HandleZoom(
        const CPoint& kDelta,
        const NiPoint2& kScreenBounds);
    virtual void HandleWheelZoom(float fWheelDelta);
    virtual void HandleRotate(
        const CPoint& kDelta,
        const NiPoint2& kScreenBounds);
    virtual void HandlePan(
        const CPoint& kDelta,
        const NiPoint2& kScreenBounds,
        float fPanCoef);
    virtual void PerformPick(CPoint point);

    virtual void ApplyTranslation(
        NiPoint3& kCameraPos,
        NiMatrix3& kCameraRot,
        NiPoint3& kFocusPos,
        const NiPoint3& kTrans) const;
    virtual void ApplyZoom(
        NiPoint3& kCameraPos,
        NiMatrix3& kCameraRot,
        NiPoint3& kFocusPos,
        const float fZoom) const;
    virtual void ApplyRotation(
        NiPoint3& kCameraPos,
        NiMatrix3& kCameraRot,
        NiPoint3& kFocusPos,
        const NiPoint3& kRotation) const;

    virtual void ComputeFocus();

    virtual void UpdateFocus();
    virtual bool UpdateTransformController();
    virtual void UpdateBases();

    void StartMouseTracking(CPoint kCurrMousePoint);
    void StopMouseTracking(CPoint kCurrMousePoint);
    void UpdateMouseTracking(CPoint kCurrMousePoint);

protected:
    NiCameraInfo* m_pkCameraInfo;
    CNiSmoothCameraControllerPtr m_spTransformController;

    bool m_bTrackingMouse;
    CPoint m_kBeginPoint;

    float m_fFocusRadius;
    NiPoint3 m_kFocusPoint;

    NiPoint3 m_kDeltaTrans;
    NiPoint3 m_kDeltaRot;
    float m_fDeltaZoom;

    NiMatrix3 m_kBaseRot;
    NiPoint3 m_kBasePos;
    NiPoint3 m_kBaseFocus;

public:
    // Speed variables
    static float ms_fWheelZoomSpeed;
    static float ms_fZoomSpeed;
    static float ms_fRotateSpeed;
    static float ms_fCameraSmoothMoveTime;
};

#endif // NIFRENDERVIEWCAMERABASESTATE_H