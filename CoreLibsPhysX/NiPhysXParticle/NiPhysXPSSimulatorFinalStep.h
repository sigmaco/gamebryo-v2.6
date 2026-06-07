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

#ifndef NIPHYSXPSSIMULATORFINALSTEP_H
#define NIPHYSXPSSIMULATORFINALSTEP_H

#include "NiPSSimulatorStep.h"
#include <NiPhysXPSSimulatorFinalKernel.h>
#include <NiSPStream.h>

/**
    The NiPhysXPSSimulatorFinalStep class is used to push the results of the
    particle system update into the PhysX actors representing the particles
    in a physics-based particle system.
  
    The class itself is responsible primarily for setting up and executing
    a Floodgate task that uses the NiPhysXPSSimulatorFinalKernel class
    to perform the computation.
    
    An instance of this class should be the last simulation step for
    any physics-based particle system. Beyond the constructor, all of the
    functionality of this class is automatically accessed through the
    NiPSSimulator object that the step belongs to and the application should
    never directly call a function on the class.
*/
class NIPHYSXPARTICLE_ENTRY NiPhysXPSSimulatorFinalStep : 
    public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPhysXPSSimulatorFinalStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Constructor initializes all internal state to null.
    NiPhysXPSSimulatorFinalStep();

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
    NiPhysXPSSimulatorFinalKernel m_kKernel;

    NiPhysXPSSimulatorFinalKernelStruct m_kInputStruct;

    // Fixed input.
    NiTSPFixedInput<NiPhysXPSSimulatorFinalKernelStruct> m_kInputStructIS;

    // Input streams.
    NiSPStream* m_pkVelocitiesIS;
    NiSPStream* m_pkLastVelocitiesIS;
    NiSPStream* m_pkAgesIS;
    NiSPStream* m_pkLifeSpansIS;
    NiSPStream* m_pkLastUpdateTimesIS;
    NiSPStream* m_pkFlagsIS;

    // Output streams.
    NiSPStream* m_pkAgesOS;
    NiSPStream* m_pkFlagsOS;
    NiSPStream* m_pkLastUpdateTimesOS;
    NiSPStream* m_pkActorVelocityChangeOS;
};

NiSmartPointer(NiPhysXPSSimulatorFinalStep);

#endif  // #ifndef NIPHYSXPSSIMULATORFINALSTEP_H
