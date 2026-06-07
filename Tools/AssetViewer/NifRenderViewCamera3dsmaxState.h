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


#ifndef NIFRENDERVIEWCAMERA3DSMAXSTATE_H
#define NIFRENDERVIEWCAMERA3DSMAXSTATE_H


#include "NifRenderViewCameraBaseState.h"


class CNifRenderViewCamera3dsmaxState :
    public CNifRenderViewCameraBaseState
{
public:
    CNifRenderViewCamera3dsmaxState(CNifRenderView* pkView);

    virtual void Initialize();
    virtual void Update();

    virtual void OnMouseMove(UINT nFlags, CPoint point);
    virtual void OnMButtonUp(UINT nFlags, CPoint point);
    virtual void OnMButtonDown(UINT nFlags, CPoint point);

    virtual void OnLButtonDown(UINT nFlags, CPoint point);

    virtual void OnUpdate(UINT Msg);

protected:
    virtual void UpdateDevices();

    bool m_bSelectionHappened;

public:
    static float ms_fSlowPanSpeed;
    static float ms_fFastPanSpeed;
};


#endif // NIFRENDERVIEWCAMERA3DSMAXSTATE_H