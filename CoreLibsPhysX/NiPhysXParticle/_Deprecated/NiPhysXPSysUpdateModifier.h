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

#ifndef NIPHYSXPSYSUPDATEMODIFIER_H
#define NIPHYSXPSYSUPDATEMODIFIER_H

#include "NiPhysXParticleLibType.h"

#include <NiPSysModifier.h>

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the PS simulator step functionality in NiPhysXPSSimulatorFinalStep.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXPSysUpdateModifier : public NiPSysModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond
    
public:
    ~NiPhysXPSysUpdateModifier();

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    // For streaming only.
    NiPhysXPSysUpdateModifier();
};

NiSmartPointer(NiPhysXPSysUpdateModifier);

/// @endcond

#endif  // #ifndef NIPHYSXPSYSUPDATEMODIFIER_H
