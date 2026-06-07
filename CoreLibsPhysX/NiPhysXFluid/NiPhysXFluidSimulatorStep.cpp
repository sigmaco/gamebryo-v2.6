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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluidSimulatorStep.h"
#include "NiPhysXPSFluidSystem.h"
#include "NiPhysXPSMeshFluidSystem.h"
#include "NiPhysXFluidCommonSemantics.h"
#include <NiPSCommonSemantics.h>
#include <NiPSKernelDefinitions.h>
#include <NiPSParticleSystem.h>

NiImplementRTTI(NiPhysXFluidSimulatorStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPhysXFluidSimulatorStep::NiPhysXFluidSimulatorStep() : 
    m_kInputStructIS(&m_kInputStruct, 1), m_pkPositionIS(NULL), 
    m_pkAgeIS(NULL), m_pkLastUpdateIS(NULL),
    m_pkFlagsIS(NULL), m_pkPositionOS(NULL), m_pkAgeOS(NULL),
    m_pkFlagsOS(NULL), m_pkLastUpdateOS(NULL),
    m_bUpdateUseful(false), m_bUpdateNecessary(false)
{
}
//---------------------------------------------------------------------------
NiSPKernel* NiPhysXFluidSimulatorStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPhysXFluidSimulatorStep::GetLargestInputStride()
{
    return m_pkPositionOS->GetStride();
}
//---------------------------------------------------------------------------
void NiPhysXFluidSimulatorStep::PrepareInputStream(
    NiPSParticleSystem* pkParticleSystem, const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    // Update the stored pointer to the stream.
    // Associate particle data with Floodgate streams.
    if (kSemantic == NiPhysXFluidCommonSemantics::PHYSXFLUIDPOSITION())
    {
        if (m_pkPositionIS != pkStream)
        {
            NiDelete m_pkPositionIS;
            m_pkPositionIS = pkStream;
        }

        if (NiIsKindOf(NiPhysXPSFluidSystem, pkParticleSystem))
        {
            NxVec3* pkBuffer = ((NiPhysXPSFluidSystem*)pkParticleSystem)->
                GetPositionsBuffer();
            m_pkPositionIS->SetData(pkBuffer);
        }
        else if (NiIsKindOf(NiPhysXPSMeshFluidSystem, pkParticleSystem))
        {
            NxVec3* pkBuffer = ((NiPhysXPSMeshFluidSystem*)pkParticleSystem)->
                GetPositionsBuffer();
            m_pkPositionIS->SetData(pkBuffer);
        }
        else
        {
            NIASSERT(false && "NiPhysXFluidSimulator::PrepareInput - Wrong "
                "type of particle system");
        }
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
void NiPhysXFluidSimulatorStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem, const NiFixedString& kSemantic,
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
NiSPTaskPtr NiPhysXFluidSimulatorStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(6, 4);

    // Create input streams.
    SetInputCount(5);
    m_pkPositionIS = NiNew NiTSPStream<NxVec3>();
    AddInput(NiPhysXFluidCommonSemantics::PHYSXFLUIDPOSITION(),
        m_pkPositionIS);
    m_pkAgeIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLEAGE(), m_pkAgeIS);
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
void NiPhysXFluidSimulatorStep::Detach(NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkPositionIS = NULL;
    m_pkAgeIS = NULL;
    m_pkLastUpdateIS = NULL;
    m_pkFlagsIS = NULL;

    // Clear out output stream pointers.
    m_pkPositionOS = NULL;
    m_pkAgeOS = NULL;
    m_pkFlagsOS = NULL;
    m_pkLastUpdateOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPhysXFluidSimulatorStep::Update(NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    if (!m_bUpdateUseful && !m_bUpdateNecessary)
        return false;

    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    m_pkPositionIS->SetBlockCount(uiNumParticles);
    m_pkPositionOS->SetBlockCount(uiNumParticles);
    m_pkAgeIS->SetBlockCount(uiNumParticles);
    m_pkAgeOS->SetBlockCount(uiNumParticles);
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
    
    NxVec3* pkPositionsBuffer = 0;
    NiUInt32* puiPhysXIDToGBIndex = 0;
    NiUInt32* puiFlags = 0;
    NxU32* puiPhysXIndexToID = 0;
    NxF32* pkPhysXBuffer = 0;
    NxU32 uiPhysXCount = 0;
    NxU32* puiPhysXDeletedIDs = 0;
    NxU32 uiPhysXDeletedIDCount = 0;
    if (NiIsKindOf(NiPhysXPSFluidSystem, pkParticleSystem))
    {
        NiPhysXPSFluidSystem* pkFluidSystem = 
            (NiPhysXPSFluidSystem*)pkParticleSystem;

        puiPhysXIDToGBIndex = pkFluidSystem->GetPhysXToGBMap();
        puiFlags = pkFluidSystem->GetFlags();
        pkPositionsBuffer = pkFluidSystem->GetPositionsBuffer();

        NxParticleData& kFluidData = pkFluidSystem->GetFluidData();
        pkPhysXBuffer = kFluidData.bufferPos;
        uiPhysXCount = *(kFluidData.numParticlesPtr);
        puiPhysXIndexToID = kFluidData.bufferId;
        
        NxParticleIdData& kFluidIdData =
            pkFluidSystem->GetParticleDeletionData();
        puiPhysXDeletedIDs = kFluidIdData.bufferId;
        uiPhysXDeletedIDCount = *(kFluidIdData.numIdsPtr);

        m_kInputStruct.m_bPhysXSpace = pkFluidSystem->GetUsesPhysXSpace();
        m_kInputStruct.m_fScalePhysXToFluid = 
            pkFluidSystem->GetScalePhysXToFluid();
        m_kInputStruct.m_kPhysXToFluid = pkFluidSystem->GetPhysXToFluid();
    }
    else if (NiIsKindOf(NiPhysXPSMeshFluidSystem, pkParticleSystem))
    {
        NiPhysXPSMeshFluidSystem* pkFluidSystem = 
            (NiPhysXPSMeshFluidSystem*)pkParticleSystem;

        puiPhysXIDToGBIndex = pkFluidSystem->GetPhysXToGBMap();
        puiFlags = pkFluidSystem->GetFlags();
        pkPositionsBuffer = pkFluidSystem->GetPositionsBuffer();

        NxParticleData& kFluidData = pkFluidSystem->GetFluidData();
        pkPhysXBuffer = kFluidData.bufferPos;
        uiPhysXCount = *(kFluidData.numParticlesPtr);
        puiPhysXIndexToID = kFluidData.bufferId;

        NxParticleIdData& kFluidIdData =
            pkFluidSystem->GetParticleDeletionData();
        puiPhysXDeletedIDs = kFluidIdData.bufferId;
        uiPhysXDeletedIDCount = *(kFluidIdData.numIdsPtr);

        m_kInputStruct.m_bPhysXSpace = pkFluidSystem->GetUsesPhysXSpace();
        m_kInputStruct.m_fScalePhysXToFluid = 
            pkFluidSystem->GetScalePhysXToFluid();
        m_kInputStruct.m_kPhysXToFluid = pkFluidSystem->GetPhysXToFluid();
    }

    //
    // Set deletion and spawning flags here, and set up positions.
    //

    // Iterate over all dead particles and mark for death. Note that the
    // particle to delete array has already been re-mapped to hold Gamebryo
    // indexes, not PhysX IDs.
    for (NiUInt32 ui = 0; ui < uiPhysXDeletedIDCount; ui++)
    {
        // Set the particle to be killed.
        NiPSFlagsHelpers::SetShouldDie(puiFlags[puiPhysXDeletedIDs[ui]], true);
    }
    
    // Copy over positions
    for (NiUInt32 ui = 0; ui < uiPhysXCount; ui++)
    {
        NiUInt32 uiGBIndex = puiPhysXIDToGBIndex[puiPhysXIndexToID[ui]];
        pkPositionsBuffer[uiGBIndex].x = pkPhysXBuffer[3 * ui];
        pkPositionsBuffer[uiGBIndex].y = pkPhysXBuffer[3 * ui + 1];
        pkPositionsBuffer[uiGBIndex].z = pkPhysXBuffer[3 * ui + 2];
    }

    m_bUpdateUseful = false;
    m_bUpdateNecessary = false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXFluidSimulatorStep);
//---------------------------------------------------------------------------
void NiPhysXFluidSimulatorStep::CopyMembers(NiPhysXFluidSimulatorStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidSimulatorStep);
//---------------------------------------------------------------------------
void NiPhysXFluidSimulatorStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidSimulatorStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidSimulatorStep::RegisterStreamables(NiStream& kStream)
{
    return NiPSSimulatorStep::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidSimulatorStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidSimulatorStep::IsEqual(NiObject* pkObject)
{
    return NiPSSimulatorStep::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXFluidSimulatorStep::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulatorStep::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
