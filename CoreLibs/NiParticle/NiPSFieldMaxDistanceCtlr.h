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

#ifndef NIPSFIELDMAXDISTANCECTLR_H
#define NIPSFIELDMAXDISTANCECTLR_H

#include "NiPSForceFloatCtlr.h"

/// Animates the max distance value of an NiPSFieldForce object.
class NIPARTICLE_ENTRY NiPSFieldMaxDistanceCtlr : public NiPSForceFloatCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSFieldMaxDistanceCtlr);
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
    NiPSFieldMaxDistanceCtlr(const NiFixedString& kForceName);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSFieldMaxDistanceCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetFloatValue(float& fValue);
    virtual void SetTargetFloatValue(float fValue);
    //@}
};

NiSmartPointer(NiPSFieldMaxDistanceCtlr);

#endif  // #ifndef NIPSFIELDMAXDISTANCECTLR_H
