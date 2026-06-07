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

#ifndef NIPSFORCEACTIVECTLR_H
#define NIPSFORCEACTIVECTLR_H

#include "NiPSForceBoolCtlr.h"
#include <NiBoolData.h>

/// Animates whether or not an NiPSForce object is active.
class NIPARTICLE_ENTRY NiPSForceActiveCtlr : public NiPSForceBoolCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSForceActiveCtlr);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kForceName The name of the NiPSForce object that should be
            animated.
    */
    NiPSForceActiveCtlr(const NiFixedString& kForceName);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSForceActiveCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual void GetTargetBoolValue(bool& bValue);
    virtual void SetTargetBoolValue(bool bValue);
    //@}
};

NiSmartPointer(NiPSForceActiveCtlr);

#endif  // #ifndef NIPSFORCEACTIVECTLR_H
