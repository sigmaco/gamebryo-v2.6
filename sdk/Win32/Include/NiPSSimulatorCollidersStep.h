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

#ifndef NIPSSIMULATORCOLLIDERSSTEP_H
#define NIPSSIMULATORCOLLIDERSSTEP_H

#include "NiPSSimulatorStep.h"
#include <NiPSSimulatorCollidersKernel.h>
#include <NiSPStream.h>
#include "NiPSCollider.h"
#include <NiTPtrSet.h>

/**
    Encapsulates a Floodgate kernel that simulates particle colliders.

    This simulator step owns an array of NiPSCollider objects. When the step
    is updated, it builds a buffer containing the data blocks provided by all
    the colliders and sends it to the Floodgate kernel as a fixed input. This
    buffer is aligned to 16 bytes, as is each data block contained in the
    buffer.

    This step uses the NiPSSimulatorCollidersKernel Floodgate kernel. The
    kernel is only executed if one or more colliders exist.
*/
class NIPARTICLE_ENTRY NiPSSimulatorCollidersStep : public NiPSSimulatorStep
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSimulatorCollidersStep);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSSimulatorCollidersStep();

    /// Virtual destructor.
    virtual ~NiPSSimulatorCollidersStep();
    //@}

    /// @name Base Class Overrides
    //@{
    /// Returns the Floodgate kernel associated with the step, which will
    /// be of type NiPSSimulatorCollidersKernel.
    virtual NiSPKernel* GetKernel();

    /// Returns the largest stride value of all the input streams in the step.
    virtual NiUInt16 GetLargestInputStride();

    /**
        Prepares the input stream with the specified semantic for execution.

        The following semantics are recognised:
        NiPSCommonSemantics::PARTICLELASTUPDATE,
        NiPSCommonSemantics::PARTICLEPOSITION,
        NiPSCommonSemantics::PARTICLEVELOCITY and
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
        NiPSCommonSemantics::PARTICLELASTUPDATE,
        NiPSCommonSemantics::PARTICLEPOSITION,
        NiPSCommonSemantics::PARTICLEVELOCITY and
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
    virtual NiSPTaskPtr Attach(
        NiPSParticleSystem* pkParticleSystem);

    /**
        Sets all pointers to data streams to NULL.
    */
    virtual void Detach(NiPSParticleSystem* pkParticleSystem);

    /**
        Update function called each frame.

        This function sets the block count on all streams and then iterates
        through the colliders, building up a stream of collider data. The
        function returns true if there are any active colliders, and false
        otherwise.
    */
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime);
    //@}

    /// @name Collider Management Functions
    //@{
    /// Returns the number of collider objects owned by the step.
    inline NiUInt32 GetCollidersCount() const;

    /**
        Returns the collider object stored at an index.
        
        @param uiIndex The index of the collider object to get. Must be less
            than the value returned by GetCollidersCount.
        @return The collider stored at uiIndex.
    */
    inline NiPSCollider* GetColliderAt(NiUInt32 uiIndex) const;

    /// Adds the specified collider object to the array of colliders for the
    /// step.
    inline void AddCollider(NiPSCollider* pkCollider);

    /**
        Replaces the collider stored at an index with another collider.
        
        @param uiIndex The index at which to replace the collider. Must be
            less than the value returned by GetCollidersCount.
        @param pkCollider The collider to set at uiIndex. Must not be NULL.
        @return A smart pointer to the collider that used to be stored at
            uiIndex.
    */
    inline NiPSColliderPtr SetColliderAt(NiUInt32 uiIndex, NiPSCollider* pkCollider);

    /**
        Removes a collider from the step.
        
        @param pkCollider The collider to remove.
        @return Whether or not pkCollider was found.
    */
    inline bool RemoveCollider(NiPSCollider* pkCollider);

    /**
        Removes the collider at an index from the step.

        @param uiIndex The index of the collider to be removed. Must be less
            than the value returned by GetCollidersCount.
        @return A smart pointer to the collider that was removed.
    */
    inline NiPSColliderPtr RemoveColliderAt(NiUInt32 uiIndex);

    /// Removes all colliders from the step.
    inline void RemoveAllColliders();
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// @endcond

protected:
    /**
        Allocates the memory buffer used to store collider data.

        This function will free any existing buffer first before creating a
        new one. The buffer is allocated aligned to NIPSKERNEL_ALIGNMENT.

        The structure of the buffer is as follows for each collider:

        -# Collider Type (NiUInt32 padded out to 16 bytes)
        -# Collider Data (padded out to 16 bytes)
    */
    void AllocateColliderDataBuffer();

    /// The set of colliders owned by the step.
    NiTObjectPtrSet<NiPSColliderPtr> m_kColliders;

    /// The Floodgate kernel used by the step.
    NiPSSimulatorCollidersKernel m_kKernel;

    /// A structure containing input data for the Floodgate kernel.
    NiPSSimulatorCollidersKernelStruct m_kInputStruct;

    /// @name Input Floodgate Streams
    //@{
    NiTSPFixedInput<NiPSSimulatorCollidersKernelStruct> m_kInputStructIS;
    NiTSPFixedInput<NiUInt8> m_kColliderDataIS;
    NiSPStream* m_pkLastUpdateIS;
    NiSPStream* m_pkPositionIS;
    NiSPStream* m_pkVelocityIS;
    NiSPStream* m_pkFlagsIS;
    //@}

    /// @name Output Floodgate Streams
    //@{
    NiSPStream* m_pkPositionOS;
    NiSPStream* m_pkVelocityOS;
    NiSPStream* m_pkFlagsOS;
    NiSPStream* m_pkLastUpdateOS;
    //@}

    /// A memory buffer used to hold the combined collider data buffers.
    NiUInt8* m_pvColliderData;

    /// The size of the m_pvColliderData buffer in bytes.
    size_t m_stColliderDataBufSize;

    /**
        Indicates whether or not the memory block for collider data should be
        allocated during the next update.

        This value is initialized to true and is set to true whenever the set
        of colliders is modified. It is set to false when
        AllocateColliderDataBuffer is called.
    */
    bool m_bShouldAllocateDataBuffer;
};

NiSmartPointer(NiPSSimulatorCollidersStep);

#include "NiPSSimulatorCollidersStep.inl"

#endif  // #ifndef NIPSSIMULATORCOLLIDERSSTEP_H
