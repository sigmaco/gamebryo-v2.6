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

#ifndef NIPSSIMULATORFINALSTEP_H
#define NIPSSIMULATORFINALSTEP_H

#include "NiPSSimulatorStep.h"
#include <NiPSSimulatorFinalKernel.h>
#include <NiSPStream.h>

/**
    Encapsulates a Floodgate kernel that updates particle positions and ages.

    As indicated by its name, this step should be attached last in the
    NiPSSimulator mesh modifier. NiPSSimulator ensures that the kernel from
    its last step will be executed in a Floodgate stage after all other
    kernels.
    
    This step uses the NiPSSimulatorFinalKernel Floodgate kernel. The kernel
    is always executed.
*/
class NIPARTICLE_ENTRY NiPSSimulatorFinalStep : public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSimulatorFinalStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSSimulatorFinalStep();
    //@}

    /// @name Base Class Overrides
    //@{
    /// Returns the Floodgate kernel associated with the step, which will
    /// be of type NiPSSimulatorFinalKernel.
    virtual NiSPKernel* GetKernel();

    /// Returns the largest stride value of all the input streams in the step.
    virtual NiUInt16 GetLargestInputStride();

    /**
        Prepares the input stream with the specified semantic for execution.

        The following semantics are recognised:
        NiPSCommonSemantics::PARTICLEPOSITION,
        NiPSCommonSemantics::PARTICLEVELOCITY,
        NiPSCommonSemantics::PARTICLEAGE,
        NiPSCommonSemantics::PARTICLELIFESPAN,
        NiPSCommonSemantics::PARTICLELASTUPDATE and
        NiPSCommonSemantics::PARTICLEFLAGS. Other semantics will assert in
        Debug builds.
    */
    virtual void PrepareInputStream(
        NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic,
        NiSPStream* pkStream);

    /**
        Prepares the output stream with the specified semantic for execution.

        The following semantics are recognised:
        NiPSCommonSemantics::PARTICLEPOSITION,
        NiPSCommonSemantics::PARTICLELASTUPDATE,
        NiPSCommonSemantics::PARTICLEAGE and
        NiPSCommonSemantics::PARTICLEFLAGS. Other semantics will assert in
        Debug builds.
    */
    virtual void PrepareOutputStream(
        NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic,
        NiSPStream* pkStream);

    /**
        Creates a task and add several input and output streams, including
        the fixed inputs.
    */
    virtual NiSPTaskPtr Attach(NiPSParticleSystem* pkParticleSystem);

    /**
        Sets all pointers to data streams to NULL.
    */
    virtual void Detach(NiPSParticleSystem* pkParticleSystem);

    /**
        Update function called each frame.

        This function sets the block count on all streams and prepares any
        spawner data. Always returns true.
    */
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}

protected:
    /// The Floodgate kernel used by the step.
    NiPSSimulatorFinalKernel m_kKernel;

    /// A structure containing input data for the Floodgate kernel.
    NiPSSimulatorFinalKernelStruct m_kInputStruct;

    /// @name Input Floodgate Streams
    //@{
    NiTSPFixedInput<NiPSSimulatorFinalKernelStruct> m_kInputStructIS;
    NiSPStream* m_pkPositionIS;
    NiSPStream* m_pkVelocityIS;
    NiSPStream* m_pkAgeIS;
    NiSPStream* m_pkLifeSpanIS;
    NiSPStream* m_pkLastUpdateIS;
    NiSPStream* m_pkFlagsIS;
    //@}

    /// @name Output Floodgate Streams
    //@{
    NiSPStream* m_pkPositionOS;
    NiSPStream* m_pkAgeOS;
    NiSPStream* m_pkFlagsOS;
    NiSPStream* m_pkLastUpdateOS;
    //@}
};

NiSmartPointer(NiPSSimulatorFinalStep);

#endif  // #ifndef NIPSSIMULATORFINALSTEP_H
