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

#include "NiPSCommonSemantics.h"
#include "NiPhysXPSCommonSemantics.h"
#include "NiPhysXPSSimulatorInitialStep.h"
#include "NiPSCommonSemantics.h"
#include "NiPSKernelDefinitions.h"
#include "NiPhysXPSParticleSystem.h"
#include "NiPhysXPSMeshParticleSystem.h"

NiImplementRTTI(NiPhysXPSSimulatorInitialStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPhysXPSSimulatorInitialStep::NiPhysXPSSimulatorInitialStep() : 
    m_kInputStructIS(&m_kInputStruct, 1), m_pkActorPosesIS(NULL), 
    m_pkActorVelocitiesIS(NULL), m_pkPositionOS(NULL), m_pkVelocityOS(NULL),
    m_pkLastVelocityOS(NULL), m_pkRotationAnglesOS(NULL), 
    m_pkRotationAxesOS(NULL)
{
}
//---------------------------------------------------------------------------
NiSPKernel* NiPhysXPSSimulatorInitialStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPhysXPSSimulatorInitialStep::GetLargestInputStride()
{
    return m_kInputStructIS.GetStride();
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::PrepareInputStream(
    NiPSParticleSystem* pkParticleSystem, const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NIASSERT(NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem) 
        || NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem));

    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPhysXPSCommonSemantics::PARTICLEACTORPOSE())
    {
        if (m_pkActorPosesIS != pkStream)
        {
            NiDelete m_pkActorPosesIS;
            m_pkActorPosesIS = pkStream;
        }
        
        if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
        {
            NiPhysXPSParticleSystem* pkPSys = 
                (NiPhysXPSParticleSystem*)pkParticleSystem;
            m_pkActorPosesIS->SetData(pkPSys->GetPhysXParticleActorPose());
        }
        else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
        {
            NiPhysXPSMeshParticleSystem* pkPSys = 
                (NiPhysXPSMeshParticleSystem*)pkParticleSystem;
            m_pkActorPosesIS->SetData(pkPSys->GetPhysXParticleActorPose());
        }        
    }
    else if (kSemantic == NiPhysXPSCommonSemantics::PARTICLEACTORVELOCITY())
    {
        if (m_pkActorVelocitiesIS != pkStream)
        {
            NiDelete m_pkActorVelocitiesIS;
            m_pkActorVelocitiesIS = pkStream;
        }

        if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
        {
            NiPhysXPSParticleSystem* pkPSys = 
                (NiPhysXPSParticleSystem*)pkParticleSystem;
            m_pkActorVelocitiesIS->SetData(
                pkPSys->GetPhysXParticleActorVelocity());
        }
        else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
        {
            NiPhysXPSMeshParticleSystem* pkPSys = 
                (NiPhysXPSMeshParticleSystem*)pkParticleSystem;
            m_pkActorVelocitiesIS->SetData(
                pkPSys->GetPhysXParticleActorVelocity());
        }
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem, const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NI_UNUSED_ARG(pkStream);
    NIASSERT(NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem) 
        || NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem));

    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPSCommonSemantics::PARTICLEPOSITION())
    {
        NIASSERT(m_pkPositionOS == pkStream);
        m_pkPositionOS->SetData(pkParticleSystem->GetPositions());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEVELOCITY())
    {
        NIASSERT(m_pkVelocityOS == pkStream);
        m_pkVelocityOS->SetData(pkParticleSystem->GetVelocities());
    }
    else if (kSemantic == 
        NiPhysXPSCommonSemantics::PARTICLEACTORLASTVELOCITY())
    {
        NIASSERT(m_pkLastVelocityOS == pkStream);
        if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
        {
            NiPhysXPSParticleSystem* pkPhysXPSys = 
                (NiPhysXPSParticleSystem*)pkParticleSystem;
            m_pkLastVelocityOS->SetData(
                pkPhysXPSys->GetPhysXParticleLastVelocities());
        } 
        else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
        {
            NiPhysXPSMeshParticleSystem* pkPhysXPSys = 
                (NiPhysXPSMeshParticleSystem*)pkParticleSystem;
            m_pkLastVelocityOS->SetData(
                pkPhysXPSys->GetPhysXParticleLastVelocities());
        }
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEROTANGLE())
    {
        NIASSERT(m_pkRotationAnglesOS == pkStream);
        m_pkRotationAnglesOS->SetData(pkParticleSystem->GetRotationAngles());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEROTAXIS())
    {
        NIASSERT(m_pkRotationAxesOS == pkStream);
        m_pkRotationAxesOS->SetData(pkParticleSystem->GetRotationAxes());
    }
    else 
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiPhysXPSSimulatorInitialStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(3, 5);

    // Create input streams.
    SetInputCount(3);
    m_pkActorPosesIS = NiNew NiTSPStream<NxMat34>();
    AddInput(NiPhysXPSCommonSemantics::PARTICLEACTORPOSE(), m_pkActorPosesIS);
    m_pkActorVelocitiesIS = NiNew NiTSPStream<NxVec3>();
    AddInput(NiPhysXPSCommonSemantics::PARTICLEACTORVELOCITY(), 
        m_pkActorVelocitiesIS);

    // Create output streams.
    SetOutputCount(5);
    m_pkPositionOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEPOSITION(), m_pkPositionOS);
    m_pkVelocityOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocityOS);
    m_pkLastVelocityOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPhysXPSCommonSemantics::PARTICLEACTORLASTVELOCITY(), 
        m_pkLastVelocityOS);
    m_pkRotationAnglesOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLEROTANGLE(), m_pkRotationAnglesOS);
    m_pkRotationAxesOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEROTAXIS(), m_pkRotationAxesOS);
  
    // Add local streams to the task.
    spTask->AddInput(&m_kInputStructIS);

    return spTask;
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::Detach(
    NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkActorPosesIS = NULL;
    m_pkActorVelocitiesIS = NULL;

    // Clear out output stream pointers.
    m_pkPositionOS = NULL;
    m_pkVelocityOS = NULL;
    m_pkLastVelocityOS = NULL;
    m_pkRotationAnglesOS = NULL;
    m_pkRotationAxesOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulatorInitialStep::Update(
    NiPSParticleSystem* pkParticleSystem, float)
{
    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();

    m_pkActorPosesIS->SetBlockCount(uiNumParticles);
    m_pkActorVelocitiesIS->SetBlockCount(uiNumParticles);
    
    m_pkPositionOS->SetBlockCount(uiNumParticles);
    m_pkVelocityOS->SetBlockCount(uiNumParticles);
    m_pkLastVelocityOS->SetBlockCount(uiNumParticles);
    m_pkRotationAnglesOS->SetBlockCount(uiNumParticles);
    m_pkRotationAxesOS->SetBlockCount(uiNumParticles);
   
    // Set up input struct.

    if (NiIsKindOf(NiPhysXPSParticleSystem, pkParticleSystem))
    {
        NiPhysXPSParticleSystem* pkPhysXParticleSystem = 
            (NiPhysXPSParticleSystem*)pkParticleSystem;

        NiPhysXTypes::NiTransformToNxMat34(
            pkPhysXParticleSystem->GetPhysXToPSysTransform().m_Rotate,
            pkPhysXParticleSystem->GetPhysXToPSysTransform().m_Translate,
            m_kInputStruct.m_kPhysXToPSys);
        m_kInputStruct.m_fScalePToW =
            pkPhysXParticleSystem->GetScalePhysXToWorld();
        m_kInputStruct.m_bPhysXSpace = 
            pkPhysXParticleSystem->GetUsesPhysXSpace();
        m_kInputStruct.m_bPhysXRotations = 
            pkPhysXParticleSystem->GetUsesPhysXRotations();

        // Signal to the particle system to fetch state from PhysX.
        pkPhysXParticleSystem->GetPhysXState();
    }
    else if (NiIsKindOf(NiPhysXPSMeshParticleSystem, pkParticleSystem))
    {
        NiPhysXPSMeshParticleSystem* pkPhysXParticleSystem = 
            (NiPhysXPSMeshParticleSystem*)pkParticleSystem;

        NiPhysXTypes::NiTransformToNxMat34(
            pkPhysXParticleSystem->GetPhysXToPSysTransform().m_Rotate,
            pkPhysXParticleSystem->GetPhysXToPSysTransform().m_Translate,
            m_kInputStruct.m_kPhysXToPSys);
        m_kInputStruct.m_fScalePToW =
            pkPhysXParticleSystem->GetScalePhysXToWorld();
        m_kInputStruct.m_bPhysXSpace = 
            pkPhysXParticleSystem->GetUsesPhysXSpace();
        m_kInputStruct.m_bPhysXRotations = 
            pkPhysXParticleSystem->GetUsesPhysXRotations();

        // Signal to the particle system to fetch state from PhysX.
        pkPhysXParticleSystem->GetPhysXState();
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiPhysXPSSimulatorInitialStep::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSSimulatorInitialStep);
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::CopyMembers(
    NiPhysXPSSimulatorInitialStep* pkDest, NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSSimulatorInitialStep);
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulatorInitialStep::RegisterStreamables(NiStream& kStream)
{
    return NiPSSimulatorStep::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSSimulatorInitialStep::IsEqual(NiObject* pkObject)
{
    return NiPSSimulatorStep::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSSimulatorInitialStep::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPhysXPSSimulatorInitialStep::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
