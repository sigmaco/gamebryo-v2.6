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

#ifndef NIPSSIMULATORFORCESSTEP_H
#define NIPSSIMULATORFORCESSTEP_H

#include "NiPSSimulatorStep.h"
#include <NiPSSimulatorForcesKernel.h>
#include <NiSPStream.h>
#include "NiPSForce.h"
#include <NiTPtrSet.h>

/**
    Encapsulates a Floodgate kernel that simulates particle forces.

    This simulator step owns an array of NiPSForce objects. When the step is
    updated, it builds a buffer containing the data blocks provided by all the
    forces and sends it to the Floodgate kernel as a fixed input. This buffer
    is aligned to 16 bytes, as is each data block contained in the buffer.

    This step uses the NiPSSimulatorForcesKernel Floodgate kernel. The
    kernel is only executed if one or more forces exist.
*/
class NIPARTICLE_ENTRY NiPSSimulatorForcesStep : public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSimulatorForcesStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSSimulatorForcesStep();

    /// Virtual destructor.
    virtual ~NiPSSimulatorForcesStep();
    //@}

    /// @name Base Class Overrides
    //@{
    /// Returns the Floodgate kernel associated with the step, which will
    /// be of type NiPSSimulatorForcesKernel.
    virtual NiSPKernel* GetKernel();

    /// Returns the largest stride value of all the input streams in the step.
    virtual NiUInt16 GetLargestInputStride();

    /**
        Prepares the input stream with the specified semantic for execution.

        The following semantics are recognised:
        NiPSCommonSemantics::PARTICLEPOSITION,
        NiPSCommonSemantics::PARTICLEVELOCITY and
        NiPSCommonSemantics::PARTICLELASTUPDATE. Other semantics will assert
        in Debug builds.
    */
    virtual void PrepareInputStream(NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic, NiSPStream* pkStream);

    /**
        Prepares the output stream with the specified semantic for execution.

        The NiPSCommonSemantics::PARTICLEVELOCITY semantic is the only one
        recognised. Other semantics will assert in Debug builds.
    */
    virtual void PrepareOutputStream(NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic, NiSPStream* pkStream);

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

        This function sets the block count on all streams. It iterates
        through all of the forces associated with the particle systems and
        constructs a stream of data for the Floodgate task. The function
        returns true if there are any forces active, and false otherwise.
    */
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}

    /// @name Force Management Functions
    //@{
    /// Returns the number of force objects owned by the step.
    inline NiUInt32 GetForcesCount() const;

    /**
        Returns the force object stored at an index.
        
        @param uiIndex The index of the force object to get. Must be less than
            the value returned by GetForcesCount.
        @return The force stored at uiIndex.
    */
    inline NiPSForce* GetForceAt(NiUInt32 uiIndex) const;

    /**
        Returns the first force found with a particular name.

        If more than one force exists with the same name, only the first force
        with that name will be returned by this function.

        @param kName The name of the force to find.
        @return The force found with the specified name. Will be NULL if no
            force is found with that name.
    */
    inline NiPSForce* GetForceByName(const NiFixedString& kName) const;

    /// Adds the specified force object to the array of forces for the step.
    void AddForce(NiPSForce* pkForce);

    /**
        Replaces the force stored at an index with another force.
        
        @param uiIndex The index at which to replace the force. Must be
            less than the value returned by GetForcesCount.
        @param pkForce The force to set at uiIndex. Must not be NULL.
        @return A smart pointer to the force that used to be stored at uiIndex.
    */
    inline NiPSForcePtr SetForceAt(NiUInt32 uiIndex, NiPSForce* pkForce);

    /**
        Removes a force from the step.
        
        @param pkForce The force to remove.
        @return Whether or not pkForce was found.
    */
    inline bool RemoveForce(NiPSForce* pkForce);

    /**
        Removes the force at an index from the step.

        @param uiIndex The index of the force to be removed. Must be less
            than the value returned by GetForcesCount.
        @return A smart pointer to the force that was removed.
    */
    inline NiPSForcePtr RemoveForceAt(NiUInt32 uiIndex);

    /// Removes all forces from the step.
    inline void RemoveAllForces();
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /**
        Allocates the memory buffer used to store force data.

        This function will free any existing buffer first before creating a
        new one. The buffer is allocated aligned to NIPSKERNEL_ALIGNMENT.

        The structure of the buffer is as follows for each force:

        -# Force Type (NiUInt32 padded out to 16 bytes)
        -# Force Data (padded out to 16 bytes)
    */
    void AllocateForceDataBuffer();

    /// The set of forces owned by the step.
    NiTObjectPtrSet<NiPSForcePtr> m_kForces;

    /// The Floodgate kernel used by the step.
    NiPSSimulatorForcesKernel m_kKernel;

    /// A structure containing input data for the Floodgate kernel.
    NiPSSimulatorForcesKernelStruct m_kInputStruct;

    /// @name Input Floodgate Streams
    //@{
    NiTSPFixedInput<NiPSSimulatorForcesKernelStruct> m_kInputStructIS;
    NiTSPFixedInput<NiUInt8> m_kForceDataIS;
    NiSPStream* m_pkLastUpdateIS;
    NiSPStream* m_pkPositionIS;
    NiSPStream* m_pkVelocityIS;
    //@}

    /// @name Output Floodgate Streams
    //@{
    NiSPStream* m_pkVelocityOS;
    //@}

    /// A memory buffer used to hold the combined force data buffers.
    NiUInt8* m_pvForceData;

    /// The size of the m_pvForceData buffer in bytes.
    size_t m_stForceDataBufSize;

    /**
        Indicates whether or not the memory block for force data should be
        allocated during the next update.

        This value is initialized to true and is set to true whenever the set
        of forces is modified. It is set to false when AllocateForceDataBuffer
        is called.
    */
    bool m_bShouldAllocateDataBuffer;
};

NiSmartPointer(NiPSSimulatorForcesStep);

#include "NiPSSimulatorForcesStep.inl"

#endif  // #ifndef NIPSSIMULATORFORCESSTEP_H
