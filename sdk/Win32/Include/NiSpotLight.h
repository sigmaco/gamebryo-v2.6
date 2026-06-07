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

#ifndef NISPOTLIGHT_H
#define NISPOTLIGHT_H

#include "NiPointLight.h"

class NIMAIN_ENTRY NiSpotLight : public NiPointLight
{
    NiDeclareRTTI;
    NiDeclareClone(NiSpotLight);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiSpotLight();

    // The model location of the light is (0,0,0).  The world location is
    // the world translation vector.  The model direction of the light is
    // (1,0,0).  The world direction is the first column of the world
    // rotation matrix.
    inline const NiPoint3& GetWorldDirection() const;

    inline void SetSpotAngle(float fSpotAngle);
    inline float GetSpotAngle() const;
    inline float GetSpotAngleCos() const;
    inline void SetInnerSpotAngle(float fSpotAngle);
    inline float GetInnerSpotAngle() const;
    inline float GetInnerSpotAngleCos() const;
    inline void SetSpotExponent(float fSpotExponent);
    inline float GetSpotExponent() const;

protected:
    void UpdateWorldData();

    // world direction
    NiPoint3 m_kWorldDir;

    // spot cone values
    float m_fOuterSpotAngle;
    float m_fInnerSpotAngle;
    float m_fSpotExponent;
    float m_fOuterSpotAngleCos;
    float m_fInnerSpotAngleCos;
};

typedef NiPointer<NiSpotLight> NiSpotLightPtr;

#include "NiSpotLight.inl"

#endif
