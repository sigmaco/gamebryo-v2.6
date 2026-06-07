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

#include "NiPSSimulatorCollidersStep.h"
#include "NiPSCommonSemantics.h"
#include "NiPSKernelDefinitions.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSSimulatorCollidersStep, NiPSSimulatorStep);

//---------------------------------------------------------------------------
NiPSSimulatorCollidersStep::NiPSSimulatorCollidersStep() :
    m_kInputStructIS(&m_kInputStruct, 1),
    m_pkLastUpdateIS(NULL),
    m_pkPositionIS(NULL),
    m_pkVelocityIS(NULL),
    m_pkFlagsIS(NULL),
    m_pkPositionOS(NULL),
    m_pkVelocityOS(NULL),
    m_pkFlagsOS(NULL),
    m_pkLastUpdateOS(NULL),
    m_pvColliderData(NULL),
    m_stColliderDataBufSize(0),
    m_bShouldAllocateDataBuffer(true)
{
}
//---------------------------------------------------------------------------
NiPSSimulatorCollidersStep::~NiPSSimulatorCollidersStep()
{
    NiAlignedFree(m_pvColliderData);
}
//---------------------------------------------------------------------------
NiSPKernel* NiPSSimulatorCollidersStep::GetKernel()
{
    return &m_kKernel;
}
//---------------------------------------------------------------------------
NiUInt16 NiPSSimulatorCollidersStep::GetLargestInputStride()
{
    return m_pkPositionIS->GetStride();
}
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::PrepareInputStream(
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
void NiPSSimulatorCollidersStep::PrepareOutputStream(
    NiPSParticleSystem* pkParticleSystem,
    const NiFixedString& kSemantic,
    NiSPStream* pkStream)
{
    NI_UNUSED_ARG(pkStream);
    // Associate particle data with Floodgate streams.
    // Update the stored pointer to the stream.
    if (kSemantic == NiPSCommonSemantics::PARTICLEPOSITION())
    {
        NIASSERT(pkStream == m_pkPositionOS);
        m_pkPositionOS->SetData(pkParticleSystem->GetPositions());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEVELOCITY())
    {
        NIASSERT(pkStream == m_pkVelocityOS);
        m_pkVelocityOS->SetData(pkParticleSystem->GetVelocities());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLEFLAGS())
    {
        NIASSERT(pkStream == m_pkFlagsOS);
        m_pkFlagsOS->SetData(pkParticleSystem->GetFlags());
    }
    else if (kSemantic == NiPSCommonSemantics::PARTICLELASTUPDATE())
    {
        NIASSERT(pkStream == m_pkLastUpdateOS);
        m_pkLastUpdateOS->SetData(pkParticleSystem->GetLastUpdateTimes());
    }
    else
    {
        NIASSERT(!"Unknown semantic type!");
    }
}
//---------------------------------------------------------------------------
NiSPTaskPtr NiPSSimulatorCollidersStep::Attach(
    NiPSParticleSystem*)
{
    NiSPTaskPtr spTask = NiSPTask::GetNewTask(6, 5);

    // Create input streams.
    SetInputCount(4);
    m_pkLastUpdateIS = NiNew NiTSPStream<float>();
    AddInput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateIS);
    m_pkPositionIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEPOSITION(), m_pkPositionIS);
    m_pkVelocityIS = NiNew NiTSPStream<NiPoint3>();
    AddInput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocityIS);
    m_pkFlagsIS = NiNew NiTSPStream<NiUInt32>();
    AddInput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsIS);

    // Create output streams.
    SetOutputCount(4);
    m_pkPositionOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEPOSITION(), m_pkPositionOS);
    m_pkVelocityOS = NiNew NiTSPStream<NiPoint3>();
    AddOutput(NiPSCommonSemantics::PARTICLEVELOCITY(), m_pkVelocityOS);
    m_pkFlagsOS = NiNew NiTSPStream<NiUInt32>();
    AddOutput(NiPSCommonSemantics::PARTICLEFLAGS(), m_pkFlagsOS);
    m_pkLastUpdateOS = NiNew NiTSPStream<float>();
    AddOutput(NiPSCommonSemantics::PARTICLELASTUPDATE(), m_pkLastUpdateOS);

    // Add local streams to the task.
    spTask->AddInput(&m_kInputStructIS);
    spTask->AddInput(&m_kColliderDataIS);

    return spTask;
}
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::Detach(NiPSParticleSystem* pkParticleSystem)
{
    // Call base class version to ensure that streams and semantics arrays
    // are properly cleared.
    NiPSSimulatorStep::Detach(pkParticleSystem);

    // Clear out input stream pointers.
    m_pkLastUpdateIS = NULL;
    m_pkPositionIS = NULL;
    m_pkVelocityIS = NULL;
    m_pkFlagsIS = NULL;

    // Clear out output stream pointers.
    m_pkPositionOS = NULL;
    m_pkVelocityOS = NULL;
    m_pkFlagsOS = NULL;
    m_pkLastUpdateOS = NULL;
}
//---------------------------------------------------------------------------
bool NiPSSimulatorCollidersStep::Update(
    NiPSParticleSystem* pkParticleSystem,
    float fTime)
{
    // Update block count for Floodgate streams to the number of active
    // particles.
    const NiUInt32 uiNumParticles = pkParticleSystem->GetNumParticles();
    m_pkLastUpdateIS->SetBlockCount(uiNumParticles);
    m_pkLastUpdateOS->SetBlockCount(uiNumParticles);
    m_pkPositionIS->SetBlockCount(uiNumParticles);
    m_pkPositionOS->SetBlockCount(uiNumParticles);
    m_pkVelocityIS->SetBlockCount(uiNumParticles);
    m_pkVelocityOS->SetBlockCount(uiNumParticles);
    m_pkFlagsIS->SetBlockCount(uiNumParticles);
    m_pkFlagsOS->SetBlockCount(uiNumParticles);

    // Set up input struct.
    m_kInputStruct.m_uiColliderCount = 0;
    m_kInputStruct.m_fCurrentTime = fTime;

    // Create collider data buffer, if necessary.
    if (m_bShouldAllocateDataBuffer)
    {
        AllocateColliderDataBuffer();
        m_bShouldAllocateDataBuffer = false;
    }
    NIASSERT(m_kColliders.GetSize() == 0 || m_pvColliderData);

    // Iterate over all colliders, updating each and determining which
    // colliders should be executed by the kernel.
    size_t stDataOffset = 0;
    for (NiUInt32 ui = 0; ui < m_kColliders.GetSize(); ++ui)
    {
        NiPSCollider* pkCollider = m_kColliders.GetAt(ui);

        // Give the collider a chance to update its internal data.
        if (pkCollider->GetActive() &&
            pkCollider->Update(pkParticleSystem, fTime))
        {
            // Copy the data from each active collider into the buffer.
            m_kInputStruct.m_uiColliderCount++;

            // Get collider type.
            NiUInt32 uiColliderType = (NiUInt32) pkCollider->GetType();
            size_t stBufSize = 16;

            // Copy the collider type into the data buffer.
            NiMemcpy(m_pvColliderData + stDataOffset,
                m_stColliderDataBufSize - stDataOffset, &uiColliderType,
                sizeof(uiColliderType));
            stDataOffset += 16;

            // Get the data pointer and size from the collider.
            size_t stDataSize = pkCollider->GetDataSize();
            NIASSERT(stDataSize > 0);
            stBufSize += stDataSize;
            void* pvData = pkCollider->GetData();
            NIASSERT(pvData);

            // Copy the collider data into the full buffer.
            NiMemcpy(m_pvColliderData + stDataOffset,
                m_stColliderDataBufSize - stDataOffset, pvData, stDataSize);
            stDataOffset += stDataSize;

            // Compute padding to bring the buffer size to be 16-byte aligned.
            size_t stPadding = stBufSize % 16;
            stDataOffset += stPadding;

            // Set the data range for this collider type. This assumes that all
            // colliders of the same type have the same data range. This data
            // range includes any necessary padding.
            m_kInputStruct.m_astDataRanges[uiColliderType] = stDataSize +
                stPadding;
        }
    }

    m_kInputStruct.m_stDataBufferSize = stDataOffset;

    return (m_kInputStruct.m_stDataBufferSize > 0);
}
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::AllocateColliderDataBuffer()
{
    // Free old buffer, if it exists.
    NiAlignedFree(m_pvColliderData);
    m_pvColliderData = NULL;
    m_stColliderDataBufSize = 0;

    // Determine the full size of the collider data buffer.
    for (NiUInt32 ui = 0; ui < m_kColliders.GetSize(); ++ui)
    {
        // Start with space to hold the the collider type, padded out to 16
        // bytes.
        size_t stBufSize = 16;

        // Add in size of the collider data.
        stBufSize += m_kColliders.GetAt(ui)->GetDataSize();

        // Add in padding to bring size to be 16-byte aligned.
        stBufSize += stBufSize % 16;

        // Apply buffer size for this collider to the full buffer size.
        m_stColliderDataBufSize += stBufSize;
    }

    if (m_stColliderDataBufSize > 0)
    {
        // Allocate the collider data buffer.
        m_pvColliderData = NiAlignedAlloc(NiUInt8, m_stColliderDataBufSize,
            NIPSKERNEL_ALIGNMENT);
    }

    // Update the collider data input stream.
    m_kColliderDataIS.SetData(m_pvColliderData);
    m_kColliderDataIS.SetBlockCount((NiUInt32)m_stColliderDataBufSize);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSSimulatorCollidersStep);
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::CopyMembers(
    NiPSSimulatorCollidersStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::CopyMembers(pkDest, kCloning);

    const NiUInt32 uiCollidersCount = m_kColliders.GetSize();
    for (NiUInt32 ui = 0; ui < uiCollidersCount; ++ui)
    {
        pkDest->m_kColliders.Add((NiPSCollider*)
            m_kColliders.GetAt(ui)->CreateClone(kCloning));
    }
}
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSSimulatorStep::ProcessClone(kCloning);

    const NiUInt32 uiCollidersCount = m_kColliders.GetSize();
    for (NiUInt32 ui = 0; ui < uiCollidersCount; ++ui)
    {
        m_kColliders.GetAt(ui)->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSSimulatorCollidersStep);
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::LoadBinary(NiStream& kStream)
{
    NiPSSimulatorStep::LoadBinary(kStream);

    NiUInt32 uiCollidersCount;
    NiStreamLoadBinary(kStream, uiCollidersCount);
    for (NiUInt32 ui = 0; ui < uiCollidersCount; ++ui)
    {
        m_kColliders.Add((NiPSCollider*) kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::LinkObject(NiStream& kStream)
{
    NiPSSimulatorStep::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorCollidersStep::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSSimulatorStep::RegisterStreamables(kStream))
    {
        return false;
    }

    const NiUInt32 uiCollidersCount = m_kColliders.GetSize();
    for (NiUInt32 ui = 0; ui < uiCollidersCount; ++ui)
    {
        m_kColliders.GetAt(ui)->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::SaveBinary(NiStream& kStream)
{
    NiPSSimulatorStep::SaveBinary(kStream);

    const NiUInt32 uiCollidersCount = m_kColliders.GetSize();
    NiStreamSaveBinary(kStream, uiCollidersCount);
    for (NiUInt32 ui = 0; ui < uiCollidersCount; ++ui)
    {
        kStream.SaveLinkID(m_kColliders.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiPSSimulatorCollidersStep::IsEqual(NiObject* pkObject)
{
    if (!NiPSSimulatorStep::IsEqual(pkObject))
    {
        return false;
    }

    NiPSSimulatorCollidersStep* pkDest = (NiPSSimulatorCollidersStep*)
        pkObject;

    if (pkDest->m_kColliders.GetSize() != m_kColliders.GetSize())
    {
        return false;
    }

    const NiUInt32 uiCollidersCount = pkDest->m_kColliders.GetSize();
    for (NiUInt32 ui = 0; ui < uiCollidersCount; ++ui)
    {
        if (!pkDest->m_kColliders.GetAt(ui)->IsEqual(m_kColliders.GetAt(ui)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSimulatorCollidersStep::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSSimulatorStep::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulatorCollidersStep::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("NumColliders", m_kColliders.GetSize()));
}
//---------------------------------------------------------------------------
