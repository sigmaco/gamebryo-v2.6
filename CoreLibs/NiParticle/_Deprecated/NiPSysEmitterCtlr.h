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

#ifndef NIPSYSEMITTERCTLR_H
#define NIPSYSEMITTERCTLR_H

#include "NiPSysModifierCtlr.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiPSEmitParticlesCtlr.
*/
class NIPARTICLE_ENTRY NiPSysEmitterCtlr : public NiPSysModifierCtlr
{
    NiDeclareRTTI;
    NiDeclareStream;

public:
    // *** begin Emergent internal use only ***
    virtual void Update(float fTime);
    virtual unsigned short GetInterpolatorCount() const;
    virtual NiInterpolator* GetInterpolator(unsigned short usIndex = 0) const;
    virtual void SetInterpolator(NiInterpolator* pkInterpolator,
        unsigned short usIndex = 0);
    virtual NiEvaluator* CreatePoseEvaluator(unsigned short usIndex = 0);
    virtual NiInterpolator* CreatePoseInterpolator(unsigned short usIndex = 0);
    virtual void SynchronizePoseInterpolator(NiInterpolator* pkInterp, 
        unsigned short usIndex = 0);
    virtual NiBlendInterpolator* CreateBlendInterpolator(
        unsigned short usIndex = 0, bool bManagerControlled = false, 
        float fWeightThreshold = 0.0f, unsigned char ucArraySize = 2) const;
    // *** end Emergent internal use only ***

protected:
    // For cloning and streaming only.
    NiPSysEmitterCtlr();

    bool InterpolatorIsCorrectType(NiInterpolator* pkInterp,
        unsigned short usIndex) const;

    NiInterpolatorPtr m_spEmitterActiveInterpolator;
    NiInterpolator* m_pkLastBirthRateInterpolator;
};

NiSmartPointer(NiPSysEmitterCtlr);

/// @endcond

#endif  // #ifndef NIPSYSEMITTERCTLR_H
