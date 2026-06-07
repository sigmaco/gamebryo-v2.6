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

#ifndef NIORBITCAMERA_H
#define NIORBITCAMERA_H

#include "NiViewerCamera.h"

class NiOrbitCamera : public NiViewerCamera
{
NiDeclareRTTI;
public:

    NiOrbitCamera(NiCamera* pkAffectedCam);
    ~NiOrbitCamera();
            
    void UpdateCamera();
    void IncrementPitch(float fAngle);
    void IncrementYaw(float fAngle);
    void IncrementRoll(float fAngle);
    
    void SetOrbitPoint(NiPoint3 kOrbitPt);
    NiPoint3 GetOrbitPoint();

    void SetTranslate(NiPoint3 kTranslateVector);
    void IncrementTranslate(NiPoint3 kDeltaTranslateVector);
    NiPoint3 GetTranslate();
    void Reset();

    NiSceneCommand::NiSceneCommandApplyType GetApplyType();

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

};

#endif
