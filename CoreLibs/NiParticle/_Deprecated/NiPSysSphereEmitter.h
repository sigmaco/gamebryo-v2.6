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

#ifndef NIPSYSSPHEREEMITTER_H
#define NIPSYSSPHEREEMITTER_H

#include "NiPSysVolumeEmitter.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSSphereEmitter.
*/
class NIPARTICLE_ENTRY NiPSysSphereEmitter : public NiPSysVolumeEmitter
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    float GetEmitterRadius() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysSphereEmitter();

    float m_fEmitterRadius;
};

NiSmartPointer(NiPSysSphereEmitter);

/// @endcond

#include "NiPSysSphereEmitter.inl"

#endif  // #ifndef NIPSYSSphereEMITTER_H
