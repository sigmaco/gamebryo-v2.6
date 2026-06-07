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


#ifndef NIFRENDERVIEWCAMERAMAYASTATE_H
#define NIFRENDERVIEWCAMERAMAYASTATE_H


#include "NifRenderViewCameraBaseState.h"


class CNifRenderViewCameraMayaState :
    public CNifRenderViewCameraBaseState
{
public:
    CNifRenderViewCameraMayaState(CNifRenderView* pkView);

    virtual void Initialize();

    virtual void OnMouseMove(UINT nFlags, CPoint point);

    virtual void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void OnLButtonDown(UINT nFlags, CPoint point);

    virtual void OnMButtonUp(UINT nFlags, CPoint point);
    virtual void OnMButtonDown(UINT nFlags, CPoint point);

    virtual void OnRButtonUp(UINT nFlags, CPoint point);
    virtual void OnRButtonDown(UINT nFlags, CPoint point);

    virtual void OnUpdate(UINT Msg);

protected:
    virtual void UpdateDevices();
    virtual void HandleZoom(
        const CPoint& kDelta,
        const NiPoint2& kScreenBounds);

    bool m_bLDown;
    bool m_bMDown;
    bool m_bRDown;

    bool m_bMouseShifted;

public:
    static float ms_fPanSpeed;
};


#endif // NIFRENDERVIEWCAMERAMAYASTATE_H