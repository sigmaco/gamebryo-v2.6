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

#ifndef NIPSYSEMITTERDECLINATIONCTLR_H
#define NIPSYSEMITTERDECLINATIONCTLR_H

#include "NiPSysModifierFloatCtlr.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSEmitterDeclinationCtlr.
*/
class NIPARTICLE_ENTRY NiPSysEmitterDeclinationCtlr :
    public NiPSysModifierFloatCtlr
{
    NiDeclareRTTI;
    NiDeclareStream;

protected:
    // For streaming only.
    NiPSysEmitterDeclinationCtlr();
};

NiSmartPointer(NiPSysEmitterDeclinationCtlr);

/// @endcond

#endif  // #ifndef NIPSYSEMITTERDECLINATIONCTLR_H
