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

#ifndef NIPSRESETONLOOPCTLR_H
#define NIPSRESETONLOOPCTLR_H

#include "NiParticleLibType.h"
#include <NiTimeController.h>

/**
    Calls ResetParticleSystem on an NiPSParticleSystem target upon looping.

    When this controller loops, it calls ResetParticleSystem on its target
    (which must be an NiPSParticleSystem object). ResetParticleSystem removes
    all particles from the system.
*/
class NIPARTICLE_ENTRY NiPSResetOnLoopCtlr : public NiTimeController
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSResetOnLoopCtlr);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSResetOnLoopCtlr();
    //@}

    /// If the controller has looped since the last frame, calls
    /// ResetParticleSystem on the target of the controller.
    virtual void Update(float fTime);

protected:
    /// Overridden from the base class to ensure that the target is of type
    /// NiPSParticleSystem.
    virtual bool TargetIsRequiredType() const;

    /**
        The scaled time for this controller on the previous update.

        This time is compared against the current scaled time to determine if
        the controller has looped.
    */
    float m_fLastScaledTime;
};

#endif  // #ifndef NIPSRESETONLOOPCTLR_H
