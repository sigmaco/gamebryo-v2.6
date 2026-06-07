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

#ifndef NIPSYSAIRFIELDSPREADCTLR_H
#define NIPSYSAIRFIELDSPREADCTLR_H

#include "NiPSysModifierFloatCtlr.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSAirFieldSpreadCtlr.
*/
class NIPARTICLE_ENTRY NiPSysAirFieldSpreadCtlr :
    public NiPSysModifierFloatCtlr
{
    NiDeclareRTTI;
    NiDeclareStream;

protected:
    // For streaming only.
    NiPSysAirFieldSpreadCtlr();
};

NiSmartPointer(NiPSysAirFieldSpreadCtlr);

/// @endcond

#endif  // #ifndef NIPSYSAIRFIELDSPREADCTLR_H
