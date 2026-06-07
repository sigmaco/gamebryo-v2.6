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

#include "NiPSSimulatorForcesStep.h"
#include "NiPSCommonSemantics.h"
#include "NiPSKernelDefinitions.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSSimulatorForcesStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPSSimulatorForcesStep::NiPSSimulatorForcesStep() :
    m_kInputStructIS(&m_kInputStruct, 1),
    m_pkLastUpdateIS(NULL),
    m_pkPositionIS(NULL),
    m_pkVelocityIS(NULL),
    m_pkVelocityOS(NULL),
    m_pvForceData(NULL),
    m_stForceDataBufSize(0),
    m_bShouldAllocateDataBuffer(true)
{
}
//---------------------------------------------------------------------------
NiPSSimulatorForcesStep::~NiPSSimulatorForcesStep()
{
    NiAlignedFree(m_pvForceData);
}
//---------------------------------------------------------------------------
NiSPKernel* NiPSSimulatorForcesStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPSSimulatorForcesStep::GetLargestInputStride()
{
    return m_pkPositionIS->GetStride();
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::PrepareInputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    // Associate particle data with Floodgate streams.
    // Update the stored pointer to the stream.
    if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        if (m_pkLastUpdateIS != pkStream)
        {
            NiDelete m_pkLastUpdateIS;
            m_pkLastUpdateIS = pkStream;
        }
        m_pkLastUpdateIS->SetData(pkParticleSystem->GetLastUpdateTimes());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEPOSITION())
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
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NI_UNUSED_ARG(pkStream);
    // Associate particle data with Floodgate streams.
    // Update the stored pointer to the stream.
    if (kSemantic == NiPSCommonSemantics::PARTICLEVELOCITY())
    {
        NIASSERT(m_pkVelocityOS == pkStream);
        m_pkVelocityOS->SetData(pkParticleSystem->GetVelocities());
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiPSSimulatorForcesStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(5, 2);

    // Create input streams.
    SetInputCount(3);
    m_pkLastUpdateIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateIS);
    m_pkPositionIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEPOSITION(), m_pkPositionIS);
    m_pkVelocityIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocityIS);

    // Create output streams.
    SetOutputCount(1);
    m_pkVelocityOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocityOS);

    // Add local streams to the task.
    spTask->AddInput(&m_kInputStructIS);
    spTask->AddInput(&m_kForceDataIS);
    
    return spTask;
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::Detach(NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkLastUpdateIS = NULL;
    m_pkPositionIS = NULL;
    m_pkVelocityIS = NULL;

    // Clear out output stream pointers.
    m_pkVelocityOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPSSimulatorForcesStep::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    m_pkLastUpdateIS->SetBlockCount(uiNumParticles);
    m_pkPositionIS->SetBlockCount(uiNumParticles);
    m_pkVelocityIS->SetBlockCount(uiNumParticles);
    m_pkVelocityOS->SetBlockCount(uiNumParticles);

    // Set up input struct.
    m_kInputStruct.m_fCurrentTime = fTime;

    // Create force data buffer, if necessary.
    if (m_bShouldAllocateDataBuffer)
    {
        AllocateForceDataBuffer();
        m_bShouldAllocateDataBuffer = false;
    }
    NIASSERT(m_kForces.GetSize() == 0 || m_pvForceData);

    // Iterate over all forces, updating each and determining which forces
    // should be executed by the kernel.
    size_t stDataOffset = 0;
    for (NiUInt32 ui = 0; ui < m_kForces.GetSize(); ++ui)
    {
        NiPSForce* pkForce = m_kForces.GetAt(ui);

        // Give the force a chance to update its internal data.
        if (pkForce->GetActive() &&
            pkForce->Update(pkParticleSystem, fTime))
        {
            // Copy the data from each active force into the buffer.

            // Get force type.
            NiUInt32 uiForceType = (NiUInt32) pkForce->GetType();
            size_t stBufSize = 16;

            // Copy the force type into the data buffer.
            NiMemcpy(m_pvForceData + stDataOffset,
                m_stForceDataBufSize - stDataOffset, &uiForceType,
                sizeof(uiForceType));
            stDataOffset += 16;

            // Get the data pointer and size from the force.
            size_t stDataSize = pkForce->GetDataSize();
            NIASSERT(stDataSize > 0);
            stBufSize += stDataSize;
            void* pvData = pkForce->GetData();
            NIASSERT(pvData);

            // Copy the force data into the full buffer.
            NiMemcpy(m_pvForceData + stDataOffset,
                m_stForceDataBufSize - stDataOffset, pvData, stDataSize);
            stDataOffset += stDataSize;

            // Compute padding to bring the buffer size to be 16-byte aligned.
            size_t stPadding = stBufSize % 16;
            stDataOffset += stPadding;

            // Set the data range for this force type. This assumes that all
            // forces of the same type have the same data range. This data
            // range includes any necessary padding.
            m_kInputStruct.m_astDataRanges[uiForceType] = stDataSize +
                stPadding;
        }
    }

    m_kInputStruct.m_stDataBufferSize = stDataOffset;

    return (m_kInputStruct.m_stDataBufferSize > 0);
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::AllocateForceDataBuffer()
{
    // Free old buffer, if it exists.
    NiAlignedFree(m_pvForceData);
    m_pvForceData = NULL;
    m_stForceDataBufSize = 0;

    // Determine the full size of the force data buffer.
    for (NiUInt32 ui = 0; ui < m_kForces.GetSize(); ++ui)
    {
        // Start with space to hold the force type, padded out to 16 bytes.
        size_t stBufSize = 16;

        // Add in size of the force data.
        stBufSize += m_kForces.GetAt(ui)->GetDataSize();

        // Add in padding to bring size to be 16-byte aligned.
        stBufSize += stBufSize % 16;

        // Apply buffer size for this force to the full buffer size.
        m_stForceDataBufSize += stBufSize;
    }

    if (m_stForceDataBufSize > 0)
    {
        // Allocate the force data buffer.
        m_pvForceData = NiAlignedAlloc(NiUInt8, m_stForceDataBufSize,
            NIPSKERNEL_ALIGNMENT);
    }

    // Update the force data input stream.
    m_kForceDataIS.SetData(m_pvForceData);
    m_kForceDataIS.SetBlockCount((NiUInt32)m_stForceDataBufSize);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSimulatorForcesStep);
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::CopyMembers(
    NiPSSimulatorForcesStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);

    const NiUInt32 uiForcesCount = m_kForces.GetSize();
    for (NiUInt32 ui = 0; ui < uiForcesCount; ++ui)
    {
        pkDest->m_kForces.Add((NiPSForce*)
            m_kForces.GetAt(ui)->CreateClone(kCloning));
    }
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::ProcessClone(kCloning);

    const NiUInt32 uiForcesCount = m_kForces.GetSize();
    for (NiUInt32 ui = 0; ui < uiForcesCount; ++ui)
    {
        m_kForces.GetAt(ui)->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSimulatorForcesStep);
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);

    NiUInt32 uiForcesCount;
    NiStreamLoadBinary(kStream, uiForcesCount);
    for (NiUInt32 ui = 0; ui < uiForcesCount; ++ui)
    {
        m_kForces.Add((NiPSForce*) kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorForcesStep::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSSimulatorStep::RegisterStreamables(kStream))
    {
        return false;
    }

    const NiUInt32 uiForcesCount = m_kForces.GetSize();
    for (NiUInt32 ui = 0; ui < uiForcesCount; ++ui)
    {
        m_kForces.GetAt(ui)->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);

    const NiUInt32 uiForcesCount = m_kForces.GetSize();
    NiStreamSaveBinary(kStream, uiForcesCount);
    for (NiUInt32 ui = 0; ui < uiForcesCount; ++ui)
    {
        kStream.SaveLinkID(m_kForces.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiPSSimulatorForcesStep::IsEqual(NiObject* pkObject)
{
    if (!NiPSSimulatorStep::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSimulatorForcesStep* pkDest = (NiPSSimulatorForcesStep*) pkObject;

    if (pkDest->m_kForces.GetSize() != m_kForces.GetSize())
    {
        return false;
    }

    const NiUInt32 uiForcesCount = pkDest->m_kForces.GetSize();
    for (NiUInt32 ui = 0; ui < uiForcesCount; ++ui)
    {
        if (!pkDest->m_kForces.GetAt(ui)->IsEqual(m_kForces.GetAt(ui)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSimulatorForcesStep::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulatorForcesStep::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("NumForces", m_kForces.GetSize()));
}
//---------------------------------------------------------------------------
