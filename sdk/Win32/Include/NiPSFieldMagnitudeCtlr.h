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

#ifndef NIPSFIELDMAGNITUDECTLR_H
#define NIPSFIELDMAGNITUDECTLR_H

#include "NiPSForceFloatCtlr.h"

/// Animates the magnitude value of an NiPSFieldForce object.
class NIPARTICLE_ENTRY NiPSFieldMagnitudeCtlr : public NiPSForceFloatCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSFieldMagnitudeCtlr);
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
    NiPSFieldMagnitudeCtlr(const NiFixedString& kForceName);
    //@}

protected:
    /// @name Construction and Destruction
    //@{
    /// Protected default constructor for cloning and streaming only.
    NiPSFieldMagnitudeCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpTargetIsCorrectType(NiObjectNET* pkTarget) const;
    virtual void GetTargetFloatValue(float& fValue);
    virtual void SetTargetFloatValue(float fValue);
    //@}
};

NiSmartPointer(NiPSFieldMagnitudeCtlr);

#endif  // #ifndef NIPSFIELDMAGNITUDECTLR_H
