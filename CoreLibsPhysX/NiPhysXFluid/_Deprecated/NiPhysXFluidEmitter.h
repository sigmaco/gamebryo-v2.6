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

#ifndef NIPHYSXFLUIDEMITTER_H
#define NIPHYSXFLUIDEMITTER_H

#include "NiPhysXFluidLibType.h"

#include "NiPSysEmitter.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. Its functionality has
    been replaced with the NiPhysXPSFluidEmitter class.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidEmitter : public NiPSysEmitter
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;

    /// @endcond

public:

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    // For cloning and streaming only.
    NiPhysXFluidEmitter();
};

NiSmartPointer(NiPhysXFluidEmitter);

/// @endcond

#endif  // #ifndef NIPHYSXFLUIDEMITTER_H
