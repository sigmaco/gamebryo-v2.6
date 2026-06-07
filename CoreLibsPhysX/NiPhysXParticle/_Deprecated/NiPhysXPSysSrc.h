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

#ifndef NIPHYSXPSYSSRC_H
#define NIPHYSXPSYSSRC_H

#include "NiPhysXParticleLibType.h"

#include "NiPhysXSrc.h"

class NiParticleSystem;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXPSParticleSystemSrc class.
*/

class NIPHYSXPARTICLE_ENTRY NiPhysXPSysSrc : public NiPhysXSrc
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;
    
    /// @endcond

public:
    /// Destructor.
    ~NiPhysXPSysSrc();

    /// @cond EMERGENT_INTERNAL

    NiParticleSystem* GetSource();
    
    // Update functions
    virtual void UpdateFromSceneGraph(const float fT, 
        const NiTransform& kInvRootTransform, const float fScaleWToP, 
        const bool bForce = false);
    virtual void UpdateToActors(const float fTBegin, const float fTEnd);

    virtual bool StreamCanSkip();
    
    /// @endcond

protected:
    // For streaming only.
    NiPhysXPSysSrc();

    NiParticleSystem* m_pkSource;
};

NiSmartPointer(NiPhysXPSysSrc);

#include "NiPhysXPSysSrc.inl"

/// @endcond

#endif  // #ifndef NIPHYSXPSYSSRC_H
