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

// NifRenderViewCameraTumbleState.h

#ifndef NIFRENDERVIEWCAMERATUMBLESTATE_H
#define NIFRENDERVIEWCAMERATUMBLESTATE_H

#include "NifRenderViewBufferedUIState.h"
#include "NifCameraList.h"

class CNifRenderViewCameraTumbleState : public CNifRenderViewBufferedUIState
{
    public:
        CNifRenderViewCameraTumbleState(CNifRenderView* pkView);
        ~CNifRenderViewCameraTumbleState();
        virtual void Update();
        virtual void Initialize();

        virtual void OnUpdate(UINT Msg);
        virtual bool CanExecuteCommand(char* pcCommandID);

        virtual void OnMouseMove(UINT nFlags, CPoint point);
        virtual void OnLButtonUp(UINT nFlags, CPoint point);
        virtual void OnLButtonDown(UINT nFlags, CPoint point);
        
       // Note: All angles are in Radians
        void SetPitch(float fAngle);
        float GetPitch();

        void SetYaw(float fAngle);
        float GetYaw();
        
        void SetRoll(float fAngle);
        float GetRoll();

        void IncrementPitch(float fAngle);
        void IncrementYaw(float fAngle);
        void IncrementRoll(float fAngle);
        
        void SetOrbitPoint(NiPoint3 kOrbitPt);
        NiPoint3 GetOrbitPoint();

        virtual bool IsAxisConstraintAllowed(UIAxisConstraint eConstraint);
    protected:
        NiPoint3 m_kOrbitPt;
        NiPoint3 m_kOffset;
        float m_fDistance;
        NiMatrix3 m_kViewMatrix;
        NiMatrix3 m_kFinalViewMatrix;
        NiPoint3 GetCompositedTranslate(NiMatrix3& kRotMatrix);
        NiMatrix3 GetPitchMatrix();
        NiMatrix3 GetYawMatrix();
        NiMatrix3 GetRollMatrix();

        virtual void UpdateDevices();
        void IncrementTranslate(NiPoint3 kDeltaTranslateVector);
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

        float m_fSceneScale;
        NiPoint3 m_kRollAxis;
        NiPoint3 m_kPitchAxis;
        NiPoint3 m_kYawAxis;

        float m_fPitchAngle;
        float m_fYawAngle;
        float m_fRollAngle;
        float m_fIncPitchAngle;
        float m_fIncYawAngle;
        float m_fIncRollAngle;

        NiMatrix3 m_kOriginalRotation;

        NiCameraInfo* m_pkCameraInfo;
        bool m_bTrackingMouse;
        CPoint m_kBeginPoint;
};

#endif  // #ifndef NIFRENDERVIEWCAMERATUMBLESTATE_H
