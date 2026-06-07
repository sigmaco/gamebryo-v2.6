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

#ifndef NIPSYSGRAVITYFIELDMODIFIER_H
#define NIPSYSGRAVITYFIELDMODIFIER_H

#include "NiPSysFieldModifier.h"
#include <NiPoint3.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSGravityFieldForce.
*/
class NIPARTICLE_ENTRY NiPSysGravityFieldModifier : public NiPSysFieldModifier
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    const NiPoint3& GetDirection() const;
    // *** end Emergent internal use only ***

protected:
    // For streaming only.
    NiPSysGravityFieldModifier();

    NiPoint3 m_kDirection;
    NiPoint3 m_kUnitDirection;
};

NiSmartPointer(NiPSysGravityFieldModifier);

/// @endcond

#include "NiPSysGravityFieldModifier.inl"

#endif  // #ifndef NIPSYSGRAVITYFIELDMODIFIER_H
