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

#ifndef NIPSYSCYLINDEREMITTER_H
#define NIPSYSCYLINDEREMITTER_H

#include "NiPSysVolumeEmitter.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSCylinderEmitter.
*/
class NIPARTICLE_ENTRY NiPSysCylinderEmitter : public NiPSysVolumeEmitter
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    float GetEmitterRadius() const;
    float GetEmitterHeight() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysCylinderEmitter();

    float m_fEmitterRadius;
    float m_fEmitterHeight;
};

NiSmartPointer(NiPSysCylinderEmitter);

/// @endcond

#include "NiPSysCylinderEmitter.inl"

#endif  // #ifndef NIPSYSCYLINDEREMITTER_H
