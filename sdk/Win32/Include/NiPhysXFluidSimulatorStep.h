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

#ifndef NIPHYSXFLUIDSIMULATORSTEP_H
#define NIPHYSXFLUIDSIMULATORSTEP_H

#include "NiPhysXFluidLibType.h"
#include "NiPSSimulatorStep.h"
#include <NiPhysXFluidSimulatorKernel.h>
#include <NiSPStream.h>

/**
    The NiPhysXFluidSimulatorStep class manages the work required to push
    PhysX fluid data into a Gamebryo particle system.
    
    Applications should not need to explicitly create or manage one of these
    objects - they are created when a NiPhysXPSFluidSystem object or
    NiPhysXPSMeshFluidSystem object is created. Along with an
    NiPSSimuatorGeneralStep, this step should be the only step applied
    to a fluid-based particle system. It transforms particles from PhysX
    to Gamebryo coordinates and updates other PhysX fluid data required
    by the particle system. Most of the work is done by a
    NiPhysXFluidSimulatorKernel object that is managed by the object.
*/
class NIPHYSXFLUID_ENTRY NiPhysXFluidSimulatorStep : public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL
    
    NiDeclareRTTI;
    NiDeclareClone(NiPhysXFluidSimulatorStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Constructor.
    NiPhysXFluidSimulatorStep();

    /// Obtain the kernel that is owned by this step.
    virtual NiSPKernel* GetKernel();
    
    /// Obtain the maximum stride for any input or output stream owned
    /// by this object.
    virtual NiUInt16 GetLargestInputStride();
    
    /// Called by NiSPSimulator::Prepare, this function should do any
    /// work required to prepare an input stream for execution.
    virtual void PrepareInputStream(NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic, NiSPStream* pkStream);
    
    /// Called by NiSPSimulator::Prepare, this function should do any
    /// work required to prepare an output stream for execution.
    virtual void PrepareOutputStream(NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic, NiSPStream* pkStream);
    
    /// This function creates a Floodgate task for fluid updating and defines
    /// the appropriate data streams.
    virtual NiSPTaskPtr Attach(NiPSParticleSystem* pkParticleSystem);

    /// This function releases any data stream resources.
    virtual void Detach(NiPSParticleSystem* pkParticleSystem);

    /// Called on each particle system update step, this function does
    /// per-frame set-up for the Floodgate task.
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);

    /**
        Set the flag indicating whether or not an update is necessary.

        This flag is set by the particle system's NiPhysXFluidDest object when
        a simulation step has finished and particles have been created or
        removed. The flag is cleared every time the Update method executes.
        
        The flag is used to verify that the particle system consumes
        information about newly added and removed particles before that
        information is lost to another physics simulation step.
    */
    void SetUpdateNecessary(const bool bUpdateNecessary);
    
    /// Get the flag indicating whether or not an update is necessary.
    bool GetUpdateNecessary() const;

    /**
        Set the flag indicating whether or not an update is useful.

        This flag is set by the particle system's NiPhysXFluidDest object when
        a simulation step has finished and new particle positions are
        available. The flag is cleared every time the Update method executes.
        The particle system will only update if this flag is true.
    */
    void SetUpdateUseful(const bool bUpdateUseful);
    
    /// Get the flag indicating whether or not an update is useful.
    bool GetUpdateUseful() const;

    /// @cond EMERGENT_INTERNAL

    // This object can be skipped on loading.
    virtual bool StreamCanSkip();

    /// @endcond

protected:
    NiPhysXFluidSimulatorKernel m_kKernel;

    NiPhysXFluidSimulatorKernelStruct m_kInputStruct;

    NiTSPFixedInput<NiPhysXFluidSimulatorKernelStruct> m_kInputStructIS;
    NiSPStream* m_pkPositionIS;
    NiSPStream* m_pkAgeIS;
    NiSPStream* m_pkLastUpdateIS;
    NiSPStream* m_pkFlagsIS;
    
    NiSPStream* m_pkPositionOS;
    NiSPStream* m_pkAgeOS;
    NiSPStream* m_pkFlagsOS;
    NiSPStream* m_pkLastUpdateOS;
    
    bool m_bUpdateUseful;
    bool m_bUpdateNecessary;
};

NiSmartPointer(NiPhysXFluidSimulatorStep);

#include "NiPhysXFluidSimulatorStep.inl"

#endif  // #ifndef NIPHYSXFLUIDSIMULATORSTEP_H
