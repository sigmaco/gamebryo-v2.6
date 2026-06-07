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


#ifndef NISMOOTHCAMERACONTROLLER_H
#define NISMOOTHCAMERACONTROLLER_H


#include <NiCamera.h>


class CNiSmoothCameraController : public NiRefObject
{
public:
    CNiSmoothCameraController(
        NiCameraPtr spCamera,
        const NiPoint3& kTargetPos,
        float fMinDistToTarget,
        float fControlTime);

    virtual void Update(float fTimeStep);
    bool IsDone() const;

protected:
    NiCameraPtr m_spCamera;
    NiPoint3 m_kStartPos;
    NiPoint3 m_kTargetPos;
    NiQuaternion m_kStartRot;
    NiQuaternion m_kTargetRot;

    float m_fControlTime;
    float m_fTime;
};


NiSmartPointer(CNiSmoothCameraController);


#endif // NISMOOTHCAMERACONTROLLER_H