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

#ifndef NIPSYSMODIFIERACTIVECTLR_H
#define NIPSYSMODIFIERACTIVECTLR_H

#include "NiPSysModifierBoolCtlr.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSForceActiveCtlr.
*/
class NIPARTICLE_ENTRY NiPSysModifierActiveCtlr : public NiPSysModifierBoolCtlr
{
    NiDeclareRTTI;
    NiDeclareStream;

protected:
    // For streaming only.
    NiPSysModifierActiveCtlr();
};

NiSmartPointer(NiPSysModifierActiveCtlr);

/// @endcond

#endif  // #ifndef NIPSYSMODIFIERACTIVECTLR_H
