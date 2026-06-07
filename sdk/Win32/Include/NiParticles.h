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

#ifndef NIPARTICLES_H
#define NIPARTICLES_H

#include "NiGeometry.h"

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSParticleSystem.
*/
class NIMAIN_ENTRY NiParticles : public NiGeometry
{
    /// @cond DEPRECATED_CLASS

    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    float* GetRadii();
    float* GetSizes();
    NiQuaternion* GetRotations();
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiParticles();

    /// @endcond
};

NiSmartPointer(NiParticles);

#include "NiParticles.inl"

#endif  // #ifndef NIPARTICLES_H
