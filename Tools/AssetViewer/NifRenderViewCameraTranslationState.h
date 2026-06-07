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

// NifRenderViewCameraTranslationState.h

#ifndef NIFRENDERVIEWCAMERATRANSLATIONSTATE_H
#define NIFRENDERVIEWCAMERATRANSLATIONSTATE_H

#include "NifRenderViewBufferedUIState.h"
#include "NifCameraList.h"

class CNifRenderViewCameraTranslationState : 
    public CNifRenderViewBufferedUIState
{
    public:
        CNifRenderViewCameraTranslationState(CNifRenderView* pkView);
        ~CNifRenderViewCameraTranslationState();
        virtual void Update();
        virtual void Initialize();

        virtual void OnUpdate(UINT Msg);
        virtual bool CanExecuteCommand(char* pcCommandID);

        virtual void OnMouseMove(UINT nFlags, CPoint point);
        virtual void OnLButtonUp(UINT nFlags, CPoint point);
        virtual void OnLButtonDown(UINT nFlags, CPoint point);
    
        virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint);

    protected:
        virtual void UpdateDevices();

        virtual void OnForwardKey(NiDevice::DeviceState eState);
        virtual void OnBackwardKey(NiDevice::DeviceState eState);
        virtual void OnLeftKey(NiDevice::DeviceState eState);
        virtual void OnRightKey(NiDevice::DeviceState eState);
        virtual void OnUpKey(NiDevice::DeviceState eState);
        virtual void OnDownKey(NiDevice::DeviceState eState);

        NiPoint3 m_kDeltaTranslate;
        float m_fSceneScale;
        NiCameraInfo* m_pkCameraInfo;
        bool m_bTrackingMouse;
        CPoint m_kBeginPoint;
};

#endif  // #ifndef NIFRENDERVIEWCAMERATRANSLATIONSTATE_H
