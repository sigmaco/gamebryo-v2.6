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

#ifndef NIPSSIMULATOR_H
#define NIPSSIMULATOR_H

#include "NiParticleLibType.h"
#include <NiMeshModifier.h>
#include "NiPSSimulatorStep.h"
#include <NiTSet.h>
#include <NiTPtrSet.h>
#include <NiSPStream.h>
#include <NiSPTask.h>
#include <NiDataStream.h>

/**
    The mesh modifier that performs all particle system simulation.

    This class is required to be attached to an NiPSParticleSystem object. It
    uses Floodgate tasks to perform all the simulation of the particle
    system.
    
    The particle simulation work is done via a set of NiPSSimulatorStep
    objects, each of which encapsulates a single Floodgate kernel. Particle
    forces and colliders are simulated using these steps, along with other
    particle simulation tasks. See each specific step class for more
    information about them.

    The input and output Floodgate streams for each step are automatically
    remapped so that output streams from previous steps are used as input
    streams on subsequent steps if the streams refer to the same data. This is
    determined by comparing semantics strings. Each step declares a semantic
    string for each of its input and output streams. The semantics defined in
    NiPSCommonSemantics should be used to refer to particle data.

    This class ensures that the Floodgate kernel from the final step in the
    set of simulator steps is always executed in a stage after all other
    kernels. This is to allow NiPSSimulatorFinalStep to update the position
    and age of the particles after all forces, colliders, and other simulation
    tasks have been computed. As such, the final step attached to this class
    should always be an instance of the NiPSSimulatorFinalStep class.

    This class directly operates on the particle data exposed by
    NiPSParticleSystem. That data should not be accessed by any other code
    while the NiPSSimulator Floodgate tasks are executing.
*/
class NIPARTICLE_ENTRY NiPSSimulator : public NiMeshModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiPSSimulator);
    NiDeclareStream;
    NiDeclareViewerStrings;

    /// @endcond

public:
    /// Used to indicate a failure from the AddStep function.
    static const NiUInt32 INVALID_INDEX;

    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NiPSSimulator();
    /// Virtual destructor.
    virtual ~NiPSSimulator();
    //@}

    /// @name Base Class Overrides
    //@{
    virtual bool Attach(NiMesh* pkMesh);
    virtual bool Detach(NiMesh* pkMesh);
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs);
    virtual bool AreRequirementsMet(
        NiMesh* pkMesh,
        NiSystemDesc::RendererID eRenderer) const;
    //@}

    /**
        Simulator steps can only be added or removed prior to Attach being
        called on the simulator.

        Once Attach has been called, all functions that add or remove steps
        will fail and return an error code.
    */
    /// @name Simulator Step Management Functions
    //@{
    /// Returns the number of steps owned by the simulator.
    inline NiUInt32 GetStepCount() const;

    /**
        Returns the step at the specified index, which must be less than the
        value returned by GetStepCount.

        Simulator steps can also be retrieved by type using the
        NiGetSimulatorStep(classname, pkSimulator) macro.
    */
    inline NiPSSimulatorStep* GetStepAt(NiUInt32 uiIndex) const;

    /**
        Adds the specified step to the array of steps for the simulator.

        @note If Attach has already been called on the simulator, this
            function will fail and return INVALID_INDEX.

        @param pkStep The step to add.
        @return The index at which the step was added.
    */
    inline NiUInt32 AddStep(NiPSSimulatorStep* pkStep);

    /**
        Sets the specified step at the specified index, replacing any step
        already at that index.

        @note If Attach has already been called on the simulator, this
            function will fail and return NULL.

        @param uiIndex The index at which to replace the step. This must be
            less than the value returned by GetStepCount.
        @param pkStep The step to set.
        @return A smart pointer to the step replaced by this function, if there
            was already a step at that index.
    */
    inline NiPSSimulatorStepPtr SetStepAt(
        NiUInt32 uiIndex,
        NiPSSimulatorStep* pkStep);

    /**
        Removes the specified step from the simulator.

        @note If Attach has already been called on the simulator, this
            function will fail and return false.

        @param pkStep The step to remove.
        @return Whether or not the step was successfully removed.
    */
    inline bool RemoveStep(NiPSSimulatorStep* pkStep);

    /**
        Removes the step at the specified index.

        @note If Attach has already been called on the simulator, this
            function will fail and return NULL.

        @param uiIndex The index at which the simulator should be removed. This
            must be less than the value returned by GetStepCount.
        @return A smart pointer to the step that was removed.
    */
    inline NiPSSimulatorStepPtr RemoveStepAt(NiUInt32 uiIndex);

    /**
        Removes all steps from the simulator.

        @note If Attach has already been called on the simulator, this
            function will fail and return false.

        @return Whether or not the steps were successfully removed.
    */ 
    inline bool RemoveAllSteps();
    //@}

    /// @cond EMERGENT_INTERNAL

    /// @name Base Class Overrides
    //@{
    virtual void ProcessClone(NiCloningProcess& kCloning);
    //@}

    /// Called by NiPSParticleSystem when the particle system is reset.
    /// Completes any outstanding tasks and resets internal data.
    void ResetSimulator(NiPSParticleSystem* pkSystem);

    /// Called by the NiGetSimulatorStep macro to return the first simulator
    /// step found whose NiRTTI object matches the one passed in.
    NiPSSimulatorStep* GetSimulatorStepByType(const NiRTTI* pkRTTI) const;

    /// @endcond

protected:
    /// A helper function that is called from both Detach and the destructor
    /// that deletes all items in m_kInputStreams and m_kOutputStreams and
    /// removes all elements from those arrays.
    inline void DeleteInputAndOutputStreams();

    /// An array of simulator steps owned by the simulator.
    NiTObjectPtrSet<NiPSSimulatorStepPtr> m_kSteps;

    /// Encapsulates a Floodgate stream and semantic for use by the simulator
    /// steps.
    class SemanticStream : public NiMemObject
    {
    public:
        /// @name Construction and Destruction
        //@{
        /**
            Main constructor.

            @param kSemantic A semantic string uniquely identifying the
                content of the stream.
            @param pkStream A pointer to a Floodgate stream.
        */
        SemanticStream(const NiFixedString& kSemantic, NiSPStream* pkStream);
        //@}
        
        /// A semantic string uniquely identifying the content of the stream.
        const NiFixedString& m_kSemantic;

        /// A pointer to a Floodgate stream.
        NiSPStream* m_pkStream;

    private:
        SemanticStream & operator=( const SemanticStream & );
    };
    
    /// The array of input streams provided by simulation steps.
    NiTPrimitiveArray<SemanticStream*> m_kInputStreams;

    /// The array of output streams provided by simulation steps.
    NiTPrimitiveArray<SemanticStream*> m_kOutputStreams;

    //@{
    /**
        An array of Floodgate tasks and workflows that will be executed by the
        simulator.

        This task array is built up in the Attach function. Workflows are
        assigned in SubmitTasks and freed in CompleteTasks.
    */
    NiTObjectPtrSet<NiSPTaskPtr> m_kTasks;
    NiTObjectPtrSet<NiSPWorkflowPtr> m_kWorkflows;
    //@}

    /// A dummy Floodgate stream used to ensure that the final simulator step
    /// is always executed after the other steps.
    NiTSPStream<float> m_kDummyStream;

    /// The last time that the SubmitTasks function was called for. If called
    /// again for an update at the same time, the SubmitTasks function will
    /// do nothing.
    float m_fLastSubmitTime;

    /**
        Whether or not it is valid to add or remove additional steps.

        This variable is set to false at the end of the Attach function,
        indicating that steps can no longer be added or removed.
    */
    bool m_bCanChangeSteps;
};

/**
    A macro that returns the first simulator step with the specified type.

    classname is the RTTI name of the desired simulator step.
    pkSimulator is a pointer to an NiPSSimulator object. This macro returns
    NULL if no simulator step is found matching that type.
*/
#define NiGetSimulatorStep(classname, pkSimulator) \
    ((classname*) (pkSimulator)->GetSimulatorStepByType(&classname::ms_RTTI))

NiSmartPointer(NiPSSimulator);

#include "NiPSSimulator.inl"

#endif  // #ifndef NIPSSIMULATOR_H
