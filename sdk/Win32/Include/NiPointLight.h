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

#ifndef NIPOINTLIGHT_H
#define NIPOINTLIGHT_H

#include "NiBool.h"
#include "NiLight.h"
#include "NiPoint3.h"


class NIMAIN_ENTRY NiPointLight : public NiLight
{
    NiDeclareRTTI;
    NiDeclareClone(NiPointLight);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiPointLight();

    // The model location of the light is (0,0,0).  The world location is
    // the world translation vector.
    inline const NiPoint3& GetWorldLocation() const;

    inline void SetConstantAttenuation(float fC);
    inline float GetConstantAttenuation() const;
    inline void SetLinearAttenuation(float fL);
    inline float GetLinearAttenuation() const;
    inline void SetQuadraticAttenuation(float fQ);
    inline float GetQuadraticAttenuation() const;

protected:
    void UpdateWorldData();

    // attenuation
    float m_fAtten0;
    float m_fAtten1;
    float m_fAtten2;
};


typedef NiPointer<NiPointLight> NiPointLightPtr;

#include "NiPointLight.inl"

#endif
