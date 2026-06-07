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

#ifndef NIPSEMITTERFLOATCTLR_H
#define NIPSEMITTERFLOATCTLR_H

#include "NiPSEmitterCtlr.h"

/**
    Abstract base class for controllers that animate a floating point value on
    an NiPSEmitter object.

    This class takes care of interpolating the animation keys and making them
    extractable. Derived classes just need to override the protected pure
    virtual member functions GetTargetFloatValue and SetTargetFloatValue to
    get and set the appropriate value on the target.

    This controller uses an NiFloatInterpolator internally to interpolate its
    animation values except for sequence animation, which utilizes UpdateValue
    to push externally computed values into the target object.
*/
class NIPARTICLE_ENTRY NiPSEmitterFloatCtlr : public NiPSEmitterCtlr
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSEmitterFloatCtlr);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Overridden from the base class to interpolate the float value and call
    /// SetTargetFloatValue to set it on the target.
    virtual void Update(float fTime);

    /// Overridden from the base class to call SetTargetFloatValue to set the
    /// specified value on the target. This function always returns true.
    virtual bool UpdateValue(float fTime, float fFloat, 
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

        @param kEmitterName The name of the NiPSEmitter object that should be
            animated.
    */
    NiPSEmitterFloatCtlr(const NiFixedString& kEmitterName);

    /// Protected default constructor for cloning and streaming only.
    NiPSEmitterFloatCtlr();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool InterpolatorIsCorrectType(NiInterpolator* pkInterpolator,
        unsigned short usIndex) const;
    //@}

    /// Called to get a floating point value from the target.
    virtual void GetTargetFloatValue(float& fValue) = 0;

    /// Called to set a floating point value on the target.
    virtual void SetTargetFloatValue(float fValue) = 0;
};

NiSmartPointer(NiPSEmitterFloatCtlr);

#endif  // #ifndef NIPSEMITTERFLOATCTLR_H
