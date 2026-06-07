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

#ifndef NIPSEMITTERDECLINATIONCTLR_H
#define NIPSEMITTERDECLINATIONCTLR_H

#include "NiPSEmitterFloatCtlr.h"

/// Animates the declination value on an NiPSEmitter object.
class NIPARTICLE_ENTRY NiPSEmitterDeclinationCtlr : public NiPSEmitterFloatCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSEmitterDeclinationCtlr);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kEmitterName The name of the NiPSEmitter object that should be
            animated.
    */
    NiPSEmitterDeclinationCtlr(const NiFixedString& kEmitterName);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSEmitterDeclinationCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual void GetTargetFloatValue(float& fValue);
    virtual void SetTargetFloatValue(float fValue);
    //@}
};

NiSmartPointer(NiPSEmitterDeclinationCtlr);

#endif  // #ifndef NIPSEMITTERDECLINATIONCTLR_H
