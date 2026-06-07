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

// Precompiled Header
#include "NiParticlePCH.h"

#include "NiPSSimulator.h"
#include "NiPSParticleSystem.h"
#include "NiPSCommonSemantics.h"
#include <NiSPWorkflow.h>

NiImplementRTTI(NiPSSimulator, NiMeshModifier);

const NiUInt32 NiPSSimulator::INVALID_INDEX = (NiUInt32) -1;

//---------------------------------------------------------------------------
NiPSSimulator::NiPSSimulator() :
    m_fLastSubmitTime(-1.0f), m_bCanChangeSteps(true)
{
}
//---------------------------------------------------------------------------
NiPSSimulator::~NiPSSimulator()
{
    m_kWorkflows.RemoveAll();
    m_kTasks.RemoveAll();
    m_kSteps.RemoveAll();

    DeleteInputAndOutputStreams();
}
//---------------------------------------------------------------------------
bool NiPSSimulator::Attach(NiMesh* pkMesh)
{
    AddSubmitSyncPoint(NiSyncArgs::SYNC_UPDATE);
    AddCompleteSyncPoint(NiSyncArgs::SYNC_VISIBLE);

    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;

    // Get maximum number of particles.
    const NiUInt32 uiMaxNumParticles = pkParticleSystem->GetMaxNumParticles();

    // Do nothing if there are no particles.
    if (uiMaxNumParticles == 0)
    {
        return true;
    }

    // Set up simulation steps.
    const NiUInt32 uiNumSteps = m_kSteps.GetSize();
    for (NiUInt32 uiStep = 0; uiStep < uiNumSteps; ++uiStep)
    {
        NiPSSimulatorStep* pkStep = m_kSteps.GetAt(uiStep);

        // Call Attach on the step to allow it to add its local Floodgate
        // streams to the task. Additionally, all input and output streams that
        // will be owned by this object will be created by the step in this
        // function and associated with the appropriate semantic.
        NiSPTaskPtr spTask = pkStep->Attach(pkParticleSystem);

        if (!spTask)
            return false;
            
        m_kTasks.Add(spTask);
        NiSPWorkflowPtr spWorkflow = 0;
        m_kWorkflows.Add(spWorkflow);
        
        // Set kernel for task.
        spTask->SetKernel(pkStep->GetKernel());

#ifdef _XENON
        // Set optimal block count for task.
        spTask->SetOptimalBlockCount((NiUInt32) ceil(1536.0f /
            pkStep->GetLargestInputStride()));
#endif  // #ifdef _XENON

        // Set task input streams.
        const NiTObjectSet<NiFixedString>& kInputSemantics =
            pkStep->GetInputSemantics();
        for (NiUInt32 uiSemantic = 0; uiSemantic < kInputSemantics.GetSize();
            uiSemantic++)
        {
            const NiFixedString& kSemantic = kInputSemantics.GetAt(
                uiSemantic);
                
            // Look for an output with that semantic
            NiUInt32 uiNumOutputs = m_kOutputStreams.GetSize();
            NiSPStream* pkStream = 0;
            for (NiUInt32 ui = 0; ui < uiNumOutputs; ui++)
            {
                if (m_kOutputStreams.GetAt(ui)->m_kSemantic == kSemantic)
                {
                    pkStream = m_kOutputStreams.GetAt(ui)->m_pkStream;
                    break;
                }
            }
            if (!pkStream)
            {
                pkStream = pkStep->GetInputStreams().GetAt(uiSemantic);

                SemanticStream* pkNewStream = NiNew SemanticStream(kSemantic,
                    pkStream);
                m_kInputStreams.Add(pkNewStream);
            }
            
            pkStep->PrepareInputStream(pkParticleSystem, kSemantic, pkStream);
            
            spTask->AddInput(pkStream);
        }
        
        // Set task output streams.
        const NiTObjectSet<NiFixedString>& kOutputSemantics =
            pkStep->GetOutputSemantics();
        for (NiUInt32 uiSemantic = 0; uiSemantic < kOutputSemantics.GetSize();
            uiSemantic++)
        {
            const NiFixedString& kSemantic = kOutputSemantics.GetAt(
                uiSemantic);

            NiSPStream* pkStream =
                pkStep->GetOutputStreams().GetAt(uiSemantic);

            SemanticStream* pkNewStream =
                NiNew SemanticStream(kSemantic, pkStream);
            m_kOutputStreams.Add(pkNewStream);
            
            pkStep->PrepareOutputStream(pkParticleSystem, kSemantic, pkStream);

            spTask->AddOutput(pkStream);
        }
        
        // Ensure that final task runs after all other tasks.
        if (uiStep < uiNumSteps - 1)
        {
            // Not the final step. Add a dummy output to the task to ensure
            // that the final step executes after this one.
            spTask->AddOutput(&m_kDummyStream);
        }
        else
        {
            // The final step. Add a dummy input to the task to ensure that
            // this step executes after all previous ones.
            spTask->AddInput(&m_kDummyStream);
        }
    }
    
    // Reset the update time
    m_fLastSubmitTime = -1.0f;

    // Disable adding or removing steps.
    m_bCanChangeSteps = false;

    return true;
}
//---------------------------------------------------------------------------
bool NiPSSimulator::Detach(NiMesh* pkMesh)
{
    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;

    // Call Detach on each simulation step to allow it to clear out its
    // pointers to Floodgate streams. These streams will be deleted in the
    // DeleteInputAndOutputStreams function.
    const NiUInt32 uiNumSteps = m_kSteps.GetSize();
    for (NiUInt32 uiStep = 0; uiStep < uiNumSteps; ++uiStep)
    {
        NiPSSimulatorStep* pkStep = m_kSteps.GetAt(uiStep);
        pkStep->Detach(pkParticleSystem);
    }

    // Clear out all tasks.
    m_kTasks.RemoveAll();
    m_kWorkflows.RemoveAll();
    
    // Delete the stored input and output streams and clear the arrays. They
    // will be recreated by each simulation step's attach function in the
    // simulator's Attach function.
    DeleteInputAndOutputStreams();

    m_bCanChangeSteps = true;

    RemoveSubmitSyncPoint(NiSyncArgs::SYNC_UPDATE);
    RemoveCompleteSyncPoint(NiSyncArgs::SYNC_VISIBLE);

    return true;
}
//---------------------------------------------------------------------------
bool NiPSSimulator::SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
    NiSPWorkflowManager* pkWFManager)
{
    // Assert that we're at a sync point which guarantees pkArgs is of 
    // the type, NiUpdateSyncArgs.
    NIASSERT((pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_UPDATE) ||
        (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_POST_UPDATE));
    float fTime = ((NiUpdateSyncArgs*)pkArgs)->m_kUpdateProcess.GetTime();
    if (fTime == m_fLastSubmitTime)
        return false;

    NiSyncArgs kCompleteArgs;
    kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteTasks(pkMesh, &kCompleteArgs);

    // Get pointer to NiPSParticleSystem.
    NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
    NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;

    pkParticleSystem->ResolveAddedParticles(true);

    if (pkParticleSystem->GetNumParticles() == 0)
        return false;
    
    // Update simulation steps.
    NIASSERT(m_kSteps.GetSize() == m_kTasks.GetSize());
    NiUInt32 uiTaskGroup = NiSyncArgs::GetTaskGroupID(
        NiSyncArgs::SYNC_UPDATE, NiSyncArgs::SYNC_VISIBLE);
    bool bSubmitted = false;
    for (NiUInt32 ui = 0; ui < m_kSteps.GetSize(); ++ui)
    {
        if (m_kSteps.GetAt(ui)->Update(pkParticleSystem, fTime))
        {
            NiSPWorkflowPtr m_spWorkflow = pkWFManager->AddRelatedTask(
                m_kTasks.GetAt(ui), uiTaskGroup, false);
            m_kWorkflows.ReplaceAt(ui, m_spWorkflow);
            bSubmitted = true;
        }
    }

    m_fLastSubmitTime = fTime;
    
    return bSubmitted;
}
//---------------------------------------------------------------------------
bool NiPSSimulator::CompleteTasks(NiMesh* pkMesh, NiSyncArgs*)
{
    bool bAnyOutstandingWorkflowsCompleted = false;
    for (NiUInt32 ui = 0; ui < m_kWorkflows.GetSize(); ++ui)
    {
        NiSPWorkflowPtr m_spWorkflow = m_kWorkflows.GetAt(ui);
        if (m_spWorkflow)
        {
            NiStreamProcessor::Get()->Wait(m_spWorkflow);
            m_kWorkflows.ReplaceAt(ui, 0);
            bAnyOutstandingWorkflowsCompleted = true;
        }
    }

    // Only complete the simulation if we actually had a workflow that did
    // some work.
    if (bAnyOutstandingWorkflowsCompleted)
    {
        NIASSERT(NiIsKindOf(NiPSParticleSystem, pkMesh));
        NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*) pkMesh;

        pkParticleSystem->CompleteSimulation();
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSSimulator::ResetSimulator(NiPSParticleSystem* pkSystem)
{
    NiSyncArgs kCompleteArgs;
    kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteTasks(pkSystem, &kCompleteArgs);
    
    m_fLastSubmitTime = -1.0f;
}
//---------------------------------------------------------------------------
bool NiPSSimulator::AreRequirementsMet(
    NiMesh* pkMesh, NiSystemDesc::RendererID) const
{
    return NiIsKindOf(NiPSParticleSystem, pkMesh);
}
//---------------------------------------------------------------------------
NiPSSimulator::SemanticStream::SemanticStream(
    const NiFixedString& kSemantic,
    NiSPStream* pkStream) :
    m_kSemantic(kSemantic),
    m_pkStream(pkStream)
{
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSimulator);
//---------------------------------------------------------------------------
void NiPSSimulator::CopyMembers(
    NiPSSimulator* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);

    const NiUInt32 uiStepsCount = m_kSteps.GetSize();
    for (NiUInt32 ui = 0; ui < uiStepsCount; ++ui)
    {
        pkDest->AddStep((NiPSSimulatorStep*)
            m_kSteps.GetAt(ui)->CreateClone(kCloning));
    }
}
//---------------------------------------------------------------------------
void NiPSSimulator::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshModifier::ProcessClone(kCloning);

    const NiUInt32 uiStepsCount = m_kSteps.GetSize();
    for (NiUInt32 ui = 0; ui < uiStepsCount; ++ui)
    {
        m_kSteps.GetAt(ui)->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSimulator);
//---------------------------------------------------------------------------
void NiPSSimulator::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);

    NiUInt32 uiStepsCount;
    NiStreamLoadBinary(kStream, uiStepsCount);
    for (NiUInt32 ui = 0; ui < uiStepsCount; ++ui)
    {
        NiPSSimulatorStep* pkStep =
            (NiPSSimulatorStep*)kStream.ResolveLinkID();
        if (pkStep)
            AddStep(pkStep);
    }
}
//---------------------------------------------------------------------------
void NiPSSimulator::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulator::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    const NiUInt32 uiStepsCount = m_kSteps.GetSize();
    for (NiUInt32 ui = 0; ui < uiStepsCount; ++ui)
    {
        m_kSteps.GetAt(ui)->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSSimulator::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);

    const NiUInt32 uiStepsCount = m_kSteps.GetSize();
    NiStreamSaveBinary(kStream, uiStepsCount);
    for (NiUInt32 ui = 0; ui < uiStepsCount; ++ui)
    {
        kStream.SaveLinkID(m_kSteps.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiPSSimulator::IsEqual(NiObject* pkObject)
{
    if (!NiMeshModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSimulator* pkDest = (NiPSSimulator*) pkObject;

    if (pkDest->m_kSteps.GetSize() != m_kSteps.GetSize())
    {
        return false;
    }

    const NiUInt32 uiStepsCount = pkDest->m_kSteps.GetSize();
    for (NiUInt32 ui = 0; ui < uiStepsCount; ++ui)
    {
        if (!pkDest->m_kSteps.GetAt(ui)->IsEqual(m_kSteps.GetAt(ui)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSimulator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("NumSteps", m_kSteps.GetSize()));
}
//---------------------------------------------------------------------------
