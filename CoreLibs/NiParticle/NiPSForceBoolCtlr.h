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

#ifndef NIPSFORCEBOOLCTLR_H
#define NIPSFORCEBOOLCTLR_H

#include "NiPSForceCtlr.h"

/**
    Abstract base class for controllers that animate a Boolean value on an
    NiPSForce object.

    This class takes care of interpolating the animation keys and making them
    extractable. Derived classes just need to override the protected pure
    virtual member functions GetTargetBoolValue and SetTargetBoolValue to
    get and set the appropriate value on the target.

    This controller uses an NiBoolInterpolator internally to interpolate its
    animation values except for sequence animation, which utilizes UpdateValue
    to push externally computed values into the target object.
*/
class NIPARTICLE_ENTRY NiPSForceBoolCtlr : public NiPSForceCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSForceBoolCtlr);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Overridden from the base class to interpolate the Boolean value and
    /// call SetTargetBoolValue to set it on the target.
    virtual void Update(float fTime);

    /// Overridden from the base class to call SetTargetBoolValue to set the
    /// specified value on the target. This function always returns true.
    virtual bool UpdateValue(float fTime, bool bBool, 
        unsigned short usIndex = 0);

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) const;
    //@}

    /// @endcond

protected:
    /// @name Construction and Destruction
    //@{
    /**
        Main constructor.

        @param kForceName The name of the NiPSForce object that should be
            animated.
    */
    NiPSForceBoolCtlr(const NiFixedString& kForceName);

    /// Protected default constructor for cloning and streaming only.
    NiPSForceBoolCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;
    //@}

    /// Called to get a Boolean value from the target.
    virtual void GetTargetBoolValue(bool& bValue) = 0;

    /// Called to set a Boolean value on the target.
    virtual void SetTargetBoolValue(bool bValue) = 0;
};

NiSmartPointer(NiPSForceBoolCtlr);

#endif  // #ifndef NIPSFORCEBOOLCTLR_H
