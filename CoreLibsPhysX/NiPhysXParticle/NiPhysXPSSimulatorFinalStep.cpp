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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXPSSimulatorFinalStep.h"
#include "NiPSCommonSemantics.h"
#include "NiPSKernelDefinitions.h"
#include "NiPhysXPSParticleSystem.h"
#include "NiPhysXPSMeshParticleSystem.h"
#include "NiPhysXPSCommonSemantics.h"

NiImplementRTTI(NiPhysXPSSimulatorFinalStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPhysXPSSimulatorFinalStep::NiPhysXPSSimulatorFinalStep() : 
    m_kInputStructIS(&m_kInputStruct, 1), m_pkVelocitiesIS(NULL), 
    m_pkLastVelocitiesIS(NULL), m_pkAgesIS(NULL), m_pkLifeSpansIS(NULL), 
    m_pkLastUpdateTimesIS(NULL), m_pkFlagsIS(NULL), m_pkAgesOS(NULL), 
    m_pkFlagsOS(NULL), m_pkLastUpdateTimesOS(NULL), 
    m_pkActorVelocityChangeOS(NULL)
{
}
//---------------------------------------------------------------------------
NiSPKernel* NiPhysXPSSimulatorFinalStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPhysXPSSimulatorFinalStep::GetLargestInputStride()
{
    return m_pkVelocitiesIS->GetStride();
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::PrepareInputStream(
    NiPSParticleSystem* pkParticleSystem, const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPSCommonSemantics::PARTICLEVELOCITY())
    {
        if (m_pkVelocitiesIS != pkStream)
        {
            NiDelete m_pkVelocitiesIS;
            m_pkVelocitiesIS = pkStream;
        }
        m_pkVelocitiesIS->SetData(pkParticleSystem->GetVelocities());
    } 
    else if (kSemantic == 
        NiPhysXPSCommonSemantics::PARTICLEACTORLASTVELOCITY())
    {
        if (m_pkLastVelocitiesIS != pkStream)
        {
            NiDelete m_pkLastVelocitiesIS;
            m_pkLastVelocitiesIS = pkStream;
        }

        if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
        {
            NiPhysXPSParticleSystem* pkPhysXParticleSystem = 
                (NiPhysXPSParticleSystem*)pkParticleSystem;
            m_pkLastVelocitiesIS->SetData(
                pkPhysXParticleSystem->GetPhysXParticleLastVelocities());
        }
        else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
        {
            NiPhysXPSMeshParticleSystem* pkPhysXParticleSystem = 
                (NiPhysXPSMeshParticleSystem*)pkParticleSystem;
            m_pkLastVelocitiesIS->SetData(
                pkPhysXParticleSystem->GetPhysXParticleLastVelocities());
        }
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEAGE())
    {
        if (m_pkAgesIS != pkStream)
        {
            NiDelete m_pkAgesIS;
            m_pkAgesIS = pkStream;
        }
        m_pkAgesIS->SetData(pkParticleSystem->GetAges());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELIFESPAN())
    {
        if (m_pkLifeSpansIS != pkStream)
        {
            NiDelete m_pkLifeSpansIS;
            m_pkLifeSpansIS = pkStream;
        }
        m_pkLifeSpansIS->SetData(pkParticleSystem->GetLifeSpans());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        if (m_pkLastUpdateTimesIS != pkStream)
        {
            NiDelete m_pkLastUpdateTimesIS;
            m_pkLastUpdateTimesIS = pkStream;
        }
        m_pkLastUpdateTimesIS->SetData(pkParticleSystem->GetLastUpdateTimes());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEFLAGS())
    {
        if (m_pkFlagsIS != pkStream)
        {
            NiDelete m_pkFlagsIS;
            m_pkFlagsIS = pkStream;
        }
        m_pkFlagsIS->SetData(pkParticleSystem->GetFlags());
    }   
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem, const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NI_UNUSED_ARG(pkStream);
    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPSCommonSemantics::PARTICLEAGE())
    {
        NIASSERT(m_pkAgesOS == pkStream);
        m_pkAgesOS->SetData(pkParticleSystem->GetAges());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEFLAGS())
    {
        NIASSERT(m_pkFlagsOS == pkStream);
        m_pkFlagsOS->SetData(pkParticleSystem->GetFlags());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        NIASSERT(m_pkLastUpdateTimesOS == pkStream);
        m_pkLastUpdateTimesOS->SetData(pkParticleSystem->GetLastUpdateTimes());
    }
    else if (kSemantic == NiPhysXPSCommonSemantics::PARTICLEACTORVELOCITY())
    {
        if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
        {
            NiPhysXPSParticleSystem* pkPhysXParticleSystem = 
                (NiPhysXPSParticleSystem*)pkParticleSystem;
            NIASSERT(m_pkActorVelocityChangeOS == pkStream);
            m_pkActorVelocityChangeOS->SetData(
                pkPhysXParticleSystem->GetPhysXParticleActorVelocity());
        }
        else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
        {
            NiPhysXPSMeshParticleSystem* pkPhysXParticleSystem = 
                (NiPhysXPSMeshParticleSystem*)pkParticleSystem;
            NIASSERT(m_pkActorVelocityChangeOS == pkStream);
            m_pkActorVelocityChangeOS->SetData(
                pkPhysXParticleSystem->GetPhysXParticleActorVelocity());
        }
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiPhysXPSSimulatorFinalStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(7, 4);

    // Create input streams.
    SetInputCount(7);
    m_pkVelocitiesIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocitiesIS);
    m_pkLastVelocitiesIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPhysXPSCommonSemantics::PARTICLEACTORLASTVELOCITY(), 
        m_pkLastVelocitiesIS);
    m_pkAgesIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLEAGE(), m_pkAgesIS);
    m_pkLifeSpansIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELIFESPAN(), m_pkLifeSpansIS);
    m_pkLastUpdateTimesIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateTimesIS);
    m_pkFlagsIS = NiNew NiTSPStream<NiUInt32>();
    AddInput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsIS);

    // Create output streams.
    SetOutputCount(4);
    m_pkAgesOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLEAGE(), m_pkAgesOS);
    m_pkFlagsOS = NiNew NiTSPStream<NiUInt32>();
    AddOutput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsOS);
    m_pkLastUpdateTimesOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLELASTUPDATE(), 
        m_pkLastUpdateTimesOS);
    m_pkActorVelocityChangeOS = NiNew NiTSPStream<NxVec3>();
    AddOutput(NiPhysXPSCommonSemantics::PARTICLEACTORVELOCITY(), 
        m_pkActorVelocityChangeOS);

    // Add local streams to the task.
    spTask->AddInput(&m_kInputStructIS);
    
    return spTask;
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::Detach(NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkVelocitiesIS = NULL;
    m_pkAgesIS = NULL;
    m_pkLifeSpansIS = NULL;
    m_pkLastUpdateTimesIS = NULL;
    m_pkFlagsIS = NULL;

    // Clear out output stream pointers.
    m_pkAgesOS = NULL;
    m_pkFlagsOS = NULL;
    m_pkLastUpdateTimesOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulatorFinalStep::Update(NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    
    m_pkVelocitiesIS->SetBlockCount(uiNumParticles);
    m_pkLastVelocitiesIS->SetBlockCount(uiNumParticles);
    m_pkAgesIS->SetBlockCount(uiNumParticles);
    m_pkLifeSpansIS->SetBlockCount(uiNumParticles);
    m_pkLastUpdateTimesIS->SetBlockCount(uiNumParticles);
    m_pkFlagsIS->SetBlockCount(uiNumParticles);
    
    m_pkAgesOS->SetBlockCount(uiNumParticles);
    m_pkFlagsOS->SetBlockCount(uiNumParticles);
    m_pkLastUpdateTimesOS->SetBlockCount(uiNumParticles);
    m_pkActorVelocityChangeOS->SetBlockCount(uiNumParticles);

    // Set up input struct.
    m_kInputStruct.m_fCurrentTime = fTime;
    NiPSSpawner* pkDeathSpawner = pkParticleSystem->GetDeathSpawner();

    if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
    {
        NiPhysXPSParticleSystem* pkPhysXParticleSystem = 
            (NiPhysXPSParticleSystem*)pkParticleSystem;

        m_kInputStruct.m_bPhysXSpace = 
            pkPhysXParticleSystem->GetUsesPhysXSpace();
        m_kInputStruct.m_fScaleWToP =
            1.0f / pkPhysXParticleSystem->GetScalePhysXToWorld();
        pkPhysXParticleSystem->GetPhysXToPSysTransform().Invert(
            m_kInputStruct.m_kPSysToPhysX);
        m_kInputStruct.m_bPhysXRotations = 
            pkPhysXParticleSystem->GetUsesPhysXRotations();
    }
    else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
    {
        NiPhysXPSMeshParticleSystem* pkPhysXParticleSystem = 
            (NiPhysXPSMeshParticleSystem*)pkParticleSystem;

        m_kInputStruct.m_bPhysXSpace = 
            pkPhysXParticleSystem->GetUsesPhysXSpace();
        m_kInputStruct.m_fScaleWToP =
            1.0f / pkPhysXParticleSystem->GetScalePhysXToWorld();
        pkPhysXParticleSystem->GetPhysXToPSysTransform().Invert(
            m_kInputStruct.m_kPSysToPhysX);
        m_kInputStruct.m_bPhysXRotations = 
            pkPhysXParticleSystem->GetUsesPhysXRotations();
    }

    if (pkDeathSpawner)
    {
        m_kInputStruct.m_bSpawnOnDeath = true;
        m_kInputStruct.m_ucSpawnerID =
            pkParticleSystem->GetSpawnerID(pkDeathSpawner);
    }
    else
    {
        m_kInputStruct.m_bSpawnOnDeath = false;
        m_kInputStruct.m_ucSpawnerID = NiPSSpawner::INVALID_ID;
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSSimulatorFinalStep::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSSimulatorFinalStep);
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::CopyMembers(
    NiPhysXPSSimulatorFinalStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSSimulatorFinalStep);
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulatorFinalStep::RegisterStreamables(NiStream& kStream)
{
    return NiPSSimulatorStep::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulatorFinalStep::IsEqual(NiObject* pkObject)
{
    return NiPSSimulatorStep::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorFinalStep::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPhysXPSSimulatorFinalStep::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
