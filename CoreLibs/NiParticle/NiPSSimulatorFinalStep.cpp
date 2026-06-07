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

#include "NiPSSimulatorFinalStep.h"
#include "NiPSCommonSemantics.h"
#include "NiPSKernelDefinitions.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSSimulatorFinalStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPSSimulatorFinalStep::NiPSSimulatorFinalStep() :
    m_kInputStructIS(&m_kInputStruct, 1),
    m_pkPositionIS(NULL),
    m_pkVelocityIS(NULL),
    m_pkAgeIS(NULL),
    m_pkLifeSpanIS(NULL),
    m_pkLastUpdateIS(NULL),
    m_pkFlagsIS(NULL),
    m_pkPositionOS(NULL),
    m_pkAgeOS(NULL),
    m_pkFlagsOS(NULL),
    m_pkLastUpdateOS(NULL)
{
}
//---------------------------------------------------------------------------
NiSPKernel* NiPSSimulatorFinalStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPSSimulatorFinalStep::GetLargestInputStride()
{
    return m_pkPositionIS->GetStride();
}
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::PrepareInputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPSCommonSemantics::PARTICLEPOSITION())
    {
        if (m_pkPositionIS != pkStream)
        {
            NiDelete m_pkPositionIS;
            m_pkPositionIS = pkStream;
        }
        m_pkPositionIS->SetData(pkParticleSystem->GetPositions());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEVELOCITY())
    {
        if (m_pkVelocityIS != pkStream)
        {
            NiDelete m_pkVelocityIS;
            m_pkVelocityIS = pkStream;
        }
        m_pkVelocityIS->SetData(pkParticleSystem->GetVelocities());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEAGE())
    {
        if (m_pkAgeIS != pkStream)
        {
            NiDelete m_pkAgeIS;
            m_pkAgeIS = pkStream;
        }
        m_pkAgeIS->SetData(pkParticleSystem->GetAges());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELIFESPAN())
    {
        if (m_pkLifeSpanIS != pkStream)
        {
            NiDelete m_pkLifeSpanIS;
            m_pkLifeSpanIS = pkStream;
        }
        m_pkLifeSpanIS->SetData(pkParticleSystem->GetLifeSpans());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        if (m_pkLastUpdateIS != pkStream)
        {
            NiDelete m_pkLastUpdateIS;
            m_pkLastUpdateIS = pkStream;
        }
        m_pkLastUpdateIS->SetData(pkParticleSystem->GetLastUpdateTimes());
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
void NiPSSimulatorFinalStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NI_UNUSED_ARG(pkStream);
    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPSCommonSemantics::PARTICLEPOSITION())
    {
        NIASSERT(m_pkPositionOS == pkStream);
        m_pkPositionOS->SetData(pkParticleSystem->GetPositions());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEAGE())
    {
        NIASSERT(m_pkAgeOS == pkStream);
        m_pkAgeOS->SetData(pkParticleSystem->GetAges());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEFLAGS())
    {
        NIASSERT(m_pkFlagsOS == pkStream);
        m_pkFlagsOS->SetData(pkParticleSystem->GetFlags());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        NIASSERT(m_pkLastUpdateOS == pkStream);
        m_pkLastUpdateOS->SetData(pkParticleSystem->GetLastUpdateTimes());
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiPSSimulatorFinalStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(8, 4);

    // Create input streams.
    SetInputCount(6);
    m_pkPositionIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEPOSITION(), m_pkPositionIS);
    m_pkVelocityIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocityIS);
    m_pkAgeIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLEAGE(), m_pkAgeIS);
    m_pkLifeSpanIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELIFESPAN(), m_pkLifeSpanIS);
    m_pkLastUpdateIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateIS);
    m_pkFlagsIS = NiNew NiTSPStream<NiUInt32>();
    AddInput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsIS);

    // Create output streams.
    SetOutputCount(4);
    m_pkPositionOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEPOSITION(), m_pkPositionOS);
    m_pkAgeOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLEAGE(), m_pkAgeOS);
    m_pkFlagsOS = NiNew NiTSPStream<NiUInt32>();
    AddOutput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsOS);
    m_pkLastUpdateOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateOS);

    // Add local streams to the task.
    spTask->AddInput(&m_kInputStructIS);
    
    return spTask;
}
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::Detach(NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkPositionIS = NULL;
    m_pkVelocityIS = NULL;
    m_pkAgeIS = NULL;
    m_pkLifeSpanIS = NULL;
    m_pkLastUpdateIS = NULL;
    m_pkFlagsIS = NULL;

    // Clear out output stream pointers.
    m_pkPositionOS = NULL;
    m_pkAgeOS = NULL;
    m_pkFlagsOS = NULL;
    m_pkLastUpdateOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPSSimulatorFinalStep::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    m_pkPositionIS->SetBlockCount(uiNumParticles);
    m_pkPositionOS->SetBlockCount(uiNumParticles);
    m_pkVelocityIS->SetBlockCount(uiNumParticles);
    m_pkAgeIS->SetBlockCount(uiNumParticles);
    m_pkAgeOS->SetBlockCount(uiNumParticles);
    m_pkLifeSpanIS->SetBlockCount(uiNumParticles);
    m_pkLastUpdateIS->SetBlockCount(uiNumParticles);
    m_pkLastUpdateOS->SetBlockCount(uiNumParticles);
    m_pkFlagsIS->SetBlockCount(uiNumParticles);
    m_pkFlagsOS->SetBlockCount(uiNumParticles);

    // Set up input struct.
    m_kInputStruct.m_fCurrentTime = fTime;
    NiPSSpawner* pkDeathSpawner = pkParticleSystem->GetDeathSpawner();
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
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSimulatorFinalStep);
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::CopyMembers(
    NiPSSimulatorFinalStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSimulatorFinalStep);
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorFinalStep::RegisterStreamables(NiStream& kStream)
{
    return NiPSSimulatorStep::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorFinalStep::IsEqual(NiObject* pkObject)
{
    return NiPSSimulatorStep::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSimulatorFinalStep::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulatorFinalStep::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
