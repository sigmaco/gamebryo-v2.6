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

#ifndef NIPSSIMULATORSTEP_H
#define NIPSSIMULATORSTEP_H

#include "NiParticleLibType.h"
#include <NiObject.h>
#include <NiTSet.h>
#include <NiMeshModifier.h>

class NiSPKernel;
class NiSPStream;
class NiPSParticleSystem;

/**
    Abstract base class for a single step in the particle system simulation
    process.

    Each simulator step encapsulates a Floodgate kernel that performs an
    aspect of the particle simulation. During its Update function, each step
    has the chance to suspend execution of its Floodgate kernel based on
    various criteria.

    This class stores sets of associated Floodgate streams and semantics.
    These stream/semantic pairs are used by the NiPSSimulator mesh modifier
    to re-arrange streams on the step to ensure the proper ordering during
    Floodgate execution. In the Attach function for derived classes, any
    streams that refer to shared particle data should be constructed and added
    using the protected AddInput and AddOutput functions in this class. This
    will pass ownership of these streams to the NiPSSimulator object and allow
    it to re-arrange them as necessary. Additionally, the semantics used for
    streams that reference particle data should come from NiPSCommonSemantics.
    The PrepareInputStream and PrepareOutputStream functions must be
    implemented in each derived class to handle initializing these shared
    streams properly.

    The Attach function is called when the NiPSSimulator object is attached
    to the particle system. This function should be used to add any locally
    owned streams (not shared streams) directly to the Floodgate task. This
    function is called before PrepareInputStream and PrepareOutputStream, so
    any streams added here should be handled first in the kernel.
    
    The Detach function is called when the NiPSSimulator object is detached
    from the particle system. Derived classes should set all their pointers to
    shared Floodgate streams to NULL in this function. They should be sure to
    call the base class version of Detach, though, which ensures that the
    array of streams and semantics stored in this class are properly emptied.

    A pure virtual function, GetKernel, exists that is called by the
    NiPSSimulator object to get the Floodgate kernel associated with the step.
    The GetLargestInputStride function should be implemented to return the
    largest stride of all the input streams used by the step. This value is
    used by NiPSSimulator to compute the optimal block count for the Floodgate
    task using the kernel.

    The Update function should be used to initialize any data in locally owned
    streams, update the block count of shared streams to match the number of
    active particles, and determine whether or not execution of the kernel
    should be skipped due to various factors.
*/
class NIPARTICLE_ENTRY NiPSSimulatorStep : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiPSSimulatorStep);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Returns the Floodgate kernel associated with the step.
    virtual NiSPKernel* GetKernel() = 0;

    /**
        Returns the largest stride value of all the input streams in the step.

        This value is used to compute an optimal block count for the Floodgate
        task that executes the step's kernel.
    */
    virtual NiUInt16 GetLargestInputStride() = 0;

    /// @name Input Stream Functions
    //@{
    /**
        Returns the stored set of input stream semantics.

        This set is populated by the AddInput protected function.
    */
    inline const NiTObjectSet<NiFixedString>& GetInputSemantics() const;

    /**
        Returns the stored set of input streams.

        This set is populated by the AddInput protected function.
    */
    inline const NiTPrimitiveSet<NiSPStream*>& GetInputStreams() const;

    /**
        Prepares the input stream with the specified semantic for execution.

        This function should compare kSemantic to all known input stream
        semantics and perform the following actions:

        - If pkStream does not match the stored input stream for that semantic,
          delete the old stream and replace it with pkStream.
        - Set the data for the stored stream to point to the particle system's
          particle data.

        @param pkParticleSystem A pointer to the particle system object.
        @param kSemantic The semantic for the input stream to be prepared.
        @param pkStream A pointer to the input stream that needs to be
            prepared. This may not match the existing stored input stream, in
            which case the existing stream should be deleted and replaced with
            this one.
    */
    virtual void PrepareInputStream(
        NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic,
        NiSPStream* pkStream) = 0;
    //@}

    /// @name Output Stream Functions
    //@{
    /**
        Returns the stored set of output stream semantics.

        This set is populated by the AddOutput protected function.
    */
    inline const NiTObjectSet<NiFixedString>& GetOutputSemantics() const;

    /**
        Returns the stored set of output streams.

        This set is populated by the AddOutput protected function.
    */
    inline const NiTPrimitiveSet<NiSPStream*>& GetOutputStreams() const;

    /**
        Prepares the output stream with the specified semantic for execution.

        This function should compare kSemantic to all known output stream
        semantics and perform the following actions:

        - If pkStream does not match the stored output stream for that
          semantic, delete the old stream and replace it with pkStream.
        - Set the data for the stored stream to point to the particle system's
          particle data.

        @param pkParticleSystem A pointer to the particle system object.
        @param kSemantic The semantic for the output stream to be prepared.
        @param pkStream A pointer to the output stream that needs to be
            prepared. This may not match the existing stored output stream, in
            which case the existing stream should be deleted and replaced with
            this one.
    */
    virtual void PrepareOutputStream(
        NiPSParticleSystem* pkParticleSystem,
        const NiFixedString& kSemantic,
        NiSPStream* pkStream) = 0;
    //@}

    /**
        Called when the NiPSSimulator object is attached to the particle
        system.

        This function should allocate a task and create any Floodgate streams
        that will be shared - call AddInput and AddOutput to associate them
        with semantics. It should also add only its locally owned streams to
        the allocated task, not the shared streams that are registered with
        AddInput and AddOutput. Those streams will be added to the task by
        the NiPSSimulator::Attach function.

        @param pkParticleSystem A pointer to the particle system object.
        
        @return A task allocated to perform this step. It can be NULL on error.
    */
    virtual NiSPTaskPtr Attach(NiPSParticleSystem* pkParticleSystem) = 0;

    /**
        Called when the NiPSSimulator object is detached from the particle
        system.

        This function should clear out any pointers to shared Floodgate
        streams, since those streams will be deleted by the
        NiPSSimulator::Detach function. The base class version of this
        function must be called to ensure that the semantics and stream arrays
        are properly emptied.

        @param pkParticleSystem A pointer to the particle system object.
    */
    virtual void Detach(NiPSParticleSystem* pkParticleSystem) = 0;

    /**
        Update function called each frame.

        This function should be used to initialize any data in locally owned
        streams, update the block count of shared streams to match the number
        of active particles, and determine whether or not execution of the
        kernel should be skipped due to various factors.

        @param pkParticleSystem A pointer to the particle system object.
        @param fTime The current simulation time.
        @return Whether or not the kernel for the step should be executed.
    */
    virtual bool Update(NiPSParticleSystem* pkParticleSystem, float fTime) = 0;

protected:
    //@{
    /**
        Sets the size of the semantic and stream arrays.

        This function should be called prior to adding streams to the array to
        initialize its size to the maximum amount of streams that will be set.
        This can help reduce memory allocation.
    */
    inline void SetInputCount(NiUInt32 uiCount);
    inline void SetOutputCount(NiUInt32 uiCount);
    //@}

    //@{
    /**
        Adds the specified stream and semantic to the stored array.

        These streams are public streams that are actually owned by the
        NiPSSimulator object and will be deleted by that object. This function
        should be called for each public stream after it has been created in
        the Attach function of derived classes.
    */
    inline void AddInput(const NiFixedString& kSemantic, NiSPStream* pkStream);
    inline void AddOutput(const NiFixedString& kSemantic, NiSPStream* pkStream);
    //@}

    /// A set of input stream semantics that refer to particle data.
    NiTObjectSet<NiFixedString> m_kInputSemantics;

    /// A set of input streams that refer to particle system data.
    NiTPrimitiveSet<NiSPStream*> m_kInputStreams;

    /// A set of output stream semantics that refer to particle data.
    NiTObjectSet<NiFixedString> m_kOutputSemantics;

    /// A set of output streams that refer to particle system data.
    NiTPrimitiveSet<NiSPStream*> m_kOutputStreams;
};

NiSmartPointer(NiPSSimulatorStep);

#include "NiPSSimulatorStep.inl"

#endif  // #ifndef NIPSSIMULATORSTEP_H
