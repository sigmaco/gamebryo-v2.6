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

// NifRenderViewCameraRotateState.h

#ifndef NIFRENDERVIEWCAMERAROTATESTATE_H
#define NIFRENDERVIEWCAMERAROTATESTATE_H

#include "NifRenderViewBufferedUIState.h"
#include "NifCameraList.h"

class CNifRenderViewCameraRotateState : public CNifRenderViewBufferedUIState
{
    public:
        CNifRenderViewCameraRotateState(CNifRenderView* pkView);
        ~CNifRenderViewCameraRotateState();
        virtual void Update();
        virtual void Initialize();

        virtual void OnUpdate(UINT Msg);
        virtual bool CanExecuteCommand(char* pcCommandID);

        virtual void OnMouseMove(UINT nFlags, CPoint point);
        virtual void OnLButtonUp(UINT nFlags, CPoint point);
        virtual void OnLButtonDown(UINT nFlags, CPoint point);
        
        // Note: All angles are in Radians
        virtual void SetPitch(float fAngle);
        virtual void IncrementPitch(float fAngle);
        virtual float GetPitch();

        virtual void SetYaw(float fAngle);
        virtual void IncrementYaw(float fAngle);
        virtual float GetYaw();
        
        virtual void SetRoll(float fAngle);
        virtual void IncrementRoll(float fAngle);
        virtual float GetRoll();

        virtual void SetTranslate(NiPoint3 kTranslateVector);
        virtual void IncrementTranslate(NiPoint3 kDeltaTranslateVector);
        virtual NiPoint3 GetTranslate();

        virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint);

    protected:
        virtual void UpdateDevices();

        virtual void OnForwardKey(NiDevice::DeviceState eState);
        virtual void OnBackwardKey(NiDevice::DeviceState eState);
        virtual void OnLeftKey(NiDevice::DeviceState eState);
        virtual void OnRightKey(NiDevice::DeviceState eState);
        virtual void OnUpKey(NiDevice::DeviceState eState);
        virtual void OnDownKey(NiDevice::DeviceState eState);
        virtual void OnPitchUpKey(NiDevice::DeviceState eState);
        virtual void OnPitchDownKey(NiDevice::DeviceState eState);
        virtual void OnYawLeftKey(NiDevice::DeviceState eState);
        virtual void OnYawRightKey(NiDevice::DeviceState eState); 
        
        NiCameraInfo* m_pkCameraInfo;
        bool m_bTrackingMouse;
        CPoint m_kBeginPoint;
                void DoRollAxisTranslate();
        void DoYawAxisTranslate();
        void DoPitchAxisTranslate();
 
        float m_fSceneScale;
        NiPoint3 m_kRollAxis;
        NiPoint3 m_kPitchAxis;
        NiPoint3 m_kYawAxis;
       
        bool  m_bIsAnimatedCamera;
        float m_fPitchAngle;
        float m_fYawAngle;
        float m_fRollAngle;
        float m_fIncPitchAngle;
        float m_fIncYawAngle;
        float m_fIncRollAngle;
        NiPoint3 m_kTranslation;

        NiPoint3 m_kOriginalTranslate;
        NiMatrix3 m_kOriginalRotation;
};

#endif  // #ifndef NIFRENDERVIEWCAMERAROTATESTATE_H
