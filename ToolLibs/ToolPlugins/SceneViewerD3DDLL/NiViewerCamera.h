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

#ifndef NIVIEWERCAMERA_H
#define NIVIEWERCAMERA_H

#include <NiMain.h>
#include <NiCamera.h>
#include "NiSceneCommand.h"
#include <NiSystem.h>

class NiViewerCamera: public NiObjectNET  
{
NiDeclareRTTI;
public:

    NiViewerCamera(NiCamera* pkAffectedCam);
    virtual ~NiViewerCamera();
    
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
    
    virtual void Reset();
    virtual void UpdateCamera();
    virtual NiSceneCommand::NiSceneCommandApplyType GetApplyType();

    bool IsAnimatedCamera();
    void SetAnimatedCamera(bool bIsAnimated);

protected:

    void DoRollAxisTranslate();
    void DoYawAxisTranslate();
    void DoPitchAxisTranslate();

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

    NiCamera* m_pkCamera;

    NiPoint3 m_kOriginalTranslate;
    NiMatrix3 m_kOriginalRotation;
};

#endif
