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

#ifndef NIPHYSXPARTICLESIMULATOR_H
#define NIPHYSXPARTICLESIMULATOR_H

#include "NiPhysXParticleLibType.h"

#include "NiPSSimulator.h"

/**
    NiPhysXPSSimulator objects add additonal sync points to
    a NiPSSimulator mesh modifier.
    
    These objects are created as part of physics-based particle or fluid
    system construction, and should never be explicitly created by an
    application.
*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSSimulator : public NiPSSimulator
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXPSSimulator);
    NiDeclareStream;
    NiDeclareViewerStrings;
    /// @endcond

public:

    /// Default constructor.
    NiPhysXPSSimulator();

    /// Virtual destructor.
    virtual ~NiPhysXPSSimulator();

    /// Attach function calls attach on the NiSPSimulator object and adds a
    /// completion sync point for PHYSICS_SIMULATE.
    virtual bool Attach(NiMesh* pkMesh);

    /// Detach is passed through to the NiSPSimulator object, and then
    /// the PHYSICS_SIMULATE complete point is removed.
    virtual bool Detach(NiMesh* pkMesh);

    /// @cond EMERGENT_INTERNAL
    
    virtual bool StreamCanSkip();
    
    /// @endcond
};

NiSmartPointer(NiPhysXPSSimulator);

#include "NiPhysXPSSimulator.inl"

#endif  // #ifndef NIPHYSXPSSIMULATOR_H
