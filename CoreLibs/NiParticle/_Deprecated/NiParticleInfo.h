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

#ifndef NIPARTICLEINFO_H
#define NIPARTICLEINFO_H

#include <NiParticleLibType.h>
#include <NiPoint3.h>

class NiStream;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSParticleSystem.
*/
class NIPARTICLE_ENTRY NiParticleInfo : public NiMemObject
{
public:
    NiParticleInfo();

    // *** begin Emergent internal use only ***
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);
    bool IsEqual(const NiParticleInfo& kData);
    // *** end Emergent internal use only ***

    NiPoint3 m_kVelocity;
    float m_fAge;
    float m_fLifeSpan;
    float m_fLastUpdate;
    unsigned short m_usGeneration;
    unsigned short m_usCode;
};

/// @endcond

#endif  // #ifndef NIPARTICLEINFO_H
