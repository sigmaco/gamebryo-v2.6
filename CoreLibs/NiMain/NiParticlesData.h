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

#ifndef NIPARTICLESDATA_H
#define NIPARTICLESDATA_H

#include "NiGeometryData.h"
#include "NiQuaternion.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSParticleSystem.
*/
class NIMAIN_ENTRY NiParticlesData : public NiGeometryData
{
    /// @cond DEPRECATED_CLASS

    NiDeclareRTTI;
    NiDeclareStream;

public:
    virtual ~NiParticlesData();

    // *** begin Emergent internal use only ***
    float* GetRadii();
    const float* GetRadii() const;
    float* GetSizes();
    const float* GetSizes() const;
    NiQuaternion* GetRotations();
    const NiQuaternion* GetRotations() const;
    float* GetRotationAngles();
    const float* GetRotationAngles() const;
    NiPoint3* GetRotationAxes();
    const NiPoint3* GetRotationAxes() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiParticlesData();

    float* m_pfRadii;
    float* m_pfSizes;
    NiQuaternion* m_pkRotations;
    float* m_pfRotationAngles;
    NiPoint3* m_pkRotationAxes;

    /// @endcond
};

typedef NiPointer<NiParticlesData> NiParticlesDataPtr;

#include "NiParticlesData.inl"

#endif // NIPARTICLESDATA_H
