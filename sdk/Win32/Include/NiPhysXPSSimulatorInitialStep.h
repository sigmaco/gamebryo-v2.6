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

#ifndef NIPHYSXPSSIMULATORINITIALSTEP_H
#define NIPHYSXPSSIMULATORINITIALSTEP_H

#include "NiPSSimulatorStep.h"
#include <NiPhysXPSSimulatorInitialKernel.h>
#include <NiSPStream.h>

/**
    The NiPhysXPSSimulatorInitialStep class is used to set the post-simulation
    state of particles in a physics-enabled particle system.
  
    The class itself is responsible primarily for setting up and executing
    a Floodgate task that uses the NiPhysXPSSimulatorInitialKernel class
    to perform the computation.
    
    An instance of this class should be the first simulation step for
    any physics-based particle system. Beyond the constructor, all of the
    functionality of this class is automatically accessed through the
    NiPSSimulator object that the step belongs to and the application should
    never directly call a function on the class.
*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSSimulatorInitialStep : 
    public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPhysXPSSimulatorInitialStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Constructor initializes all internal state to null.
    NiPhysXPSSimulatorInitialStep();

    /// @cond EMERGENT_INTERNAL

    virtual NiSPKernel* GetKernel();

    virtual NiUInt16 GetLargestInputStride();

    virtual void PrepareInputStream(NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic, NiSPStream* pkStream);

    virtual void PrepareOutputStream(NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic, NiSPStream* pkStream);

    virtual NiSPTaskPtr Attach(NiPSParticleSystem* pkParticleSystem);
    virtual void Detach(NiPSParticleSystem* pkParticleSystem);

    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);

    virtual bool StreamCanSkip();

    /// @endcond

protected:
    NiPhysXPSSimulatorInitialKernel m_kKernel;

    NiPhysXPSSimulatorInitialKernelStruct m_kInputStruct;

    NiTSPFixedInput<NiPhysXPSSimulatorInitialKernelStruct> m_kInputStructIS;
    NiSPStream* m_pkActorPosesIS;
    NiSPStream* m_pkActorVelocitiesIS;
    
    NiSPStream* m_pkPositionOS;
    NiSPStream* m_pkVelocityOS;
    NiSPStream* m_pkLastVelocityOS;
    NiSPStream* m_pkRotationAnglesOS;
    NiSPStream* m_pkRotationAxesOS;
};

NiSmartPointer(NiPhysXPSSimulatorInitialStep);

#endif  // #ifndef NIPHYSXPSSIMULATORINITIALSTEP_H
