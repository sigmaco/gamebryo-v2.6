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

#include "NiPhysXPSMeshFluidSystem.h"
#include "NiPhysXFluidDesc.h"
#include "NiPhysXFluidDest.h"
#include "NiPhysXPSFluidSystem.h"
#include "NiPhysXFluidPropDesc.h"
#include "NiPhysXFluidEmitter.h"
#include "NiPhysXFluidSimulatorStep.h"

NiImplementRTTI(NiPhysXPSMeshFluidSystem, NiPSMeshParticleSystem);

//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem::~NiPhysXPSMeshFluidSystem()
{
    // Must do this here so that the object is still alive to
    // complete simulation.
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteModifiers(&kSyncArgs);
    
    // We need to remove the simulator modifier now, while the particle
    // system simulator still exists to detach.
    NiMeshModifier* pkSimulator =
        GetModifierByType(&NiPhysXPSSimulator::ms_RTTI);
    RemoveModifier(pkSimulator);

    m_spFluidEmitter = 0;
    m_spDestination = 0;

    OnDeletion();
}
//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem* NiPhysXPSMeshFluidSystem::Create(
    NiPhysXPSFluidEmitter* pkEmitter,
    NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations, 
    bool bWorldSpace, bool bDynamicBounds, bool bAttachMeshModifiers,
    NiUInt32 uiPoolSize, NiUInt16 usNumGenerations, bool bFillPoolsOnLoad,
    NiPhysXProp* pkProp, bool bPhysXSpace, float fExpansionFactor)
{
    // Create simulator.
    NiPhysXPSSimulator* pkSimulator = NiNew NiPhysXPSSimulator();

    // Add simulation steps.
    pkSimulator->AddStep(NiNew NiPSSimulatorGeneralStep());
    pkSimulator->AddStep(NiNew NiPhysXFluidSimulatorStep());

    // Create bound updater, if requested.
    NiPSBoundUpdater* pkBoundUpdater = NULL;
    if (bDynamicBounds)
    {
        pkBoundUpdater = NiNew NiPSBoundUpdater();
    }

    NiPhysXPSMeshFluidSystem* pkPhysXFluidSystem = 
        NiNew NiPhysXPSMeshFluidSystem(pkEmitter, pkSimulator, 
        NiUInt32(uiMaxNumParticles * fExpansionFactor), bHasColors, 
        bHasRotations, bWorldSpace, pkBoundUpdater, uiPoolSize, 
        usNumGenerations, bFillPoolsOnLoad, pkProp, bPhysXSpace, 
        fExpansionFactor);

    // Add simulator.
    pkPhysXFluidSystem->AddModifier(pkSimulator, bAttachMeshModifiers);

    return pkPhysXFluidSystem;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem* NiPhysXPSMeshFluidSystem::Create(
    NiPSMeshParticleSystem* pkPSys, NiPhysXProp* pkProp, bool bPhysXSpace,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap,
    float fExpansionFactor)
{
    NiPhysXPSMeshFluidSystem* pkFluidSystem = NiNew NiPhysXPSMeshFluidSystem(
        pkPSys, pkProp, bPhysXSpace, kActorMap, fExpansionFactor);

    return pkFluidSystem;
}
//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem::NiPhysXPSMeshFluidSystem(
    NiPhysXPSFluidEmitter* pkEmitter, NiPhysXPSSimulator* pkSimulator, 
    NiUInt32 uiMaxNumParticles, bool bHasColors,
    bool bHasRotations, bool bWorldSpace, NiPSBoundUpdater* pkBoundUpdater,
    NiUInt32 uiPoolSize, NiUInt16 usNumGenerations, bool bFillPoolsOnLoad,
    NiPhysXProp* pkProp, bool bPhysXSpace, float fExpansionFactor) : 
    NiPSMeshParticleSystem(pkSimulator, 
    NiUInt32(uiMaxNumParticles * fExpansionFactor), bHasColors, bHasRotations, 
    bHasRotations, bWorldSpace, pkBoundUpdater, uiPoolSize, usNumGenerations,
    bFillPoolsOnLoad)
{
    m_uiMaxNumPhysXParticles = uiMaxNumParticles;
    m_bPhysXSpace = bPhysXSpace;

    m_spFluidEmitter = pkEmitter;

    // Set up sources and destinations to get data into and out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
    else
    {
        m_spDestination = 0;
    }
    OnCreation();
}
//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem::NiPhysXPSMeshFluidSystem(
    NiPSMeshParticleSystem* pkPSys, NiPhysXProp* pkProp, bool bPhysXSpace,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap,
    float fExpansionFactor) : NiPSMeshParticleSystem()
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    ((NiPhysXPSMeshFluidSystem*)pkPSys)->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);

    // Detach all modifiers and then delete all of the datastreamsrefs.
    // The datastreamsrefs will be recreated as soon as the modifiers are
    // reattached. Another purpose with the detachment of all the modifiers is
    // to be able to manipulate add and remove steps from the NiPSSimulator.
    NIVERIFY(DetachAllModifiers());
    NiMesh::RemoveAllStreamRefs();

    // Reallocate the data buffers and set the PhysX particles max size
    // to m_uiMaxNumParticles.
    m_uiMaxNumPhysXParticles = m_uiMaxNumParticles;
    m_uiMaxNumParticles = NiUInt32(m_uiMaxNumParticles * fExpansionFactor);
    AllocateDataBuffers(m_uiMaxNumParticles, 
        HasColors(), HasRotations(), HasRotationAxes());

    // Set the stuff the clone didn't catch
    m_bPhysXSpace = bPhysXSpace;
    
    OnCreation();   
    
    // Create a fluid descriptor and add it to the scene
    
    // First get the scene descriptor.
    NIASSERT(pkProp->GetSnapshot() &&
        NiIsKindOf(NiPhysXFluidPropDesc, pkProp->GetSnapshot()));
    NiPhysXFluidPropDesc* pkPropDesc =
        (NiPhysXFluidPropDesc*)pkProp->GetSnapshot();

    // Create the Fluid descriptor
    NiPhysXFluidDescPtr spFluidDesc = NiNew NiPhysXFluidDesc();
    spFluidDesc->SetName(GetName());
    spFluidDesc->SetParticleSystem(this);

    // Reset the simulator
    NiPSSimulatorPtr spInputSimulator = m_pkSimulator;
    RemoveModifier(m_pkSimulator);
    m_pkSimulator = NiNew NiPhysXPSSimulator();
    AddModifier(m_pkSimulator, false);

    m_spFluidEmitter = NiPhysXPSFluidSystem::ConvertModifiers(this,
        spInputSimulator, pkProp, spFluidDesc, kActorMap);
    AddEmitter(m_spFluidEmitter);

    // Reattach all modifiers.
    NIVERIFY(AttachAllModifiers());

    pkPropDesc->AddFluidDesc(spFluidDesc);

    // Set up the destinations to get data out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
    else
    {
        m_spDestination = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXPSMeshFluidSystem::NiPhysXPSMeshFluidSystem()
{
    m_spDestination = 0;
    m_spFluidEmitter = 0;
    m_uiMaxNumPhysXParticles = 0;
    m_pkPositionsBuffer = 0;
    m_puiPhysXToGBMap = 0;
    m_puiGBToPhysXMap = 0;
    m_bPhysXSpace = false;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::InitializeParticle(NiUInt32 uiNewParticle)
{
    NiPSMeshParticleSystem::InitializeParticle(uiNewParticle);

    NiUInt32 uiIndexFromPhysXToGB = 
        m_kFluidCreationData.bufferId[m_uiNumAddedParticles - 1];

    m_puiPhysXToGBMap[uiIndexFromPhysXToGB] = uiNewParticle;
    m_puiGBToPhysXMap[uiNewParticle] = uiIndexFromPhysXToGB;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::RemoveParticle(NiUInt32 uiIndexToRemove)
{
    NiUInt32 uiLastIndex = m_uiNumParticles - 1;
    NiPSMeshParticleSystem::RemoveParticle(uiIndexToRemove);

    NiUInt32 uiIndexFromPhysXToGB = m_puiGBToPhysXMap[uiLastIndex];
    m_puiPhysXToGBMap[uiIndexFromPhysXToGB] = uiIndexToRemove;
    m_puiGBToPhysXMap[uiIndexToRemove] = uiIndexFromPhysXToGB;
    m_pkPositionsBuffer[uiIndexToRemove] = m_pkPositionsBuffer[uiLastIndex];
}
//---------------------------------------------------------------------------
NiPhysXPSFluidEmitter* NiPhysXPSMeshFluidSystem::GetFluidEmitter()
{
    return m_spFluidEmitter;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::SetFluidEmitter(
    NiPhysXPSFluidEmitter* pkFluidEmitter)
{
    if (m_spDestination)
    {
        m_spDestination->SetEmitter(pkFluidEmitter);
    }
    m_spFluidEmitter = pkFluidEmitter;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::SetDestination(NiPhysXFluidDest* pkFluidDest)
{
    m_spDestination = pkFluidDest;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::AttachToProp(NiPhysXProp* pkProp)
{
    NIASSERT(m_spFluidEmitter);
    m_spDestination = NiNew NiPhysXFluidDest(this, m_spFluidEmitter);
    pkProp->AddDestination(m_spDestination);
    pkProp->AddModifiedMesh(this);
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::DetachFromProp(NiPhysXProp* pkProp)
{
    pkProp->DeleteModifiedMesh(this);
    pkProp->DeleteDestination(m_spDestination);
    m_spDestination = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::OnCreation()
{
    // Fluid data, position, velocity and buffer ID.
    m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
    *(m_kFluidData.numParticlesPtr) = 0;
    m_kFluidData.bufferPosByteStride = 3 * sizeof(NxF32);
    m_kFluidData.bufferPos = NiAlloc(NxF32, m_uiMaxNumPhysXParticles * 3);
    m_kFluidData.bufferId = NiAlloc(NxU32, m_uiMaxNumPhysXParticles);
    m_kFluidData.bufferIdByteStride = sizeof(NxU32);

    // Array with indices to map particles from PhysX to GB.
    m_puiPhysXToGBMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);
    memset(m_puiPhysXToGBMap, 0xFF, m_uiMaxNumParticles * sizeof(NiUInt32));
    m_puiGBToPhysXMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);

    // Creation of PhysX particles.
    m_kFluidCreationData.numIdsPtr = NiAlloc(NxU32, 1);
    *(m_kFluidCreationData.numIdsPtr) = 0;
    m_kFluidCreationData.bufferId = NiAlloc(NxU32, m_uiMaxNumPhysXParticles);
    m_kFluidCreationData.bufferIdByteStride = sizeof(NxU32);
   
    // Deletion  of PhysX particles.
    m_kFluidDeletionData.numIdsPtr = NiAlloc(NxU32, 1);
    *(m_kFluidDeletionData.numIdsPtr) = 0;
    m_kFluidDeletionData.bufferId = NiAlloc(NxU32, m_uiMaxNumPhysXParticles);
    m_kFluidDeletionData.bufferIdByteStride = sizeof(NxU32);

    // Position buffer in Gamebryo order
    m_pkPositionsBuffer = NiAlloc(NxVec3, m_uiMaxNumParticles);
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::OnDeletion()
{
    NiFree(m_kFluidData.numParticlesPtr);
    NiFree(m_kFluidData.bufferPos);    
    NiFree(m_kFluidData.bufferId);
    
    NiFree(m_puiPhysXToGBMap);
    NiFree(m_puiGBToPhysXMap);

    NiFree(m_kFluidCreationData.numIdsPtr);
    NiFree(m_kFluidCreationData.bufferId);

    NiFree(m_kFluidDeletionData.numIdsPtr);
    NiFree(m_kFluidDeletionData.bufferId);
    
    NiFree(m_pkPositionsBuffer);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSMeshFluidSystem);
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::CopyMembers(NiPhysXPSMeshFluidSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSMeshParticleSystem::CopyMembers(pkDest, kCloning);

    if (!NiIsKindOf(NiPhysXPSMeshFluidSystem, this))
        return;

    if (m_spDestination)
    {
        // We expect the prop that this system is associated with to also
        // clone the destination, so use shared clone function.
        pkDest->m_spDestination =
            (NiPhysXFluidDest*)m_spDestination->CreateSharedClone(kCloning);
    }
    else
    {
        m_spDestination = 0;
    }
    
    pkDest->m_bPhysXSpace = m_bPhysXSpace;

    // The fluid emitter is cloned as if sharing, but since nothing else
    // shares the original emitter, nothing will share this either. We just
    // want anything else holding a pointer to the emitter to get the same
    // emitter clone.
    pkDest->m_spFluidEmitter =
        (NiPhysXPSFluidEmitter*)m_spFluidEmitter->CreateSharedClone(kCloning);

    pkDest->m_uiMaxNumPhysXParticles = m_uiMaxNumPhysXParticles;

    // Sets all the fluid data structures.
    pkDest->OnCreation();
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSMeshParticleSystem::ProcessClone(kCloning);

    bool bCloned = false;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    
    if (m_spDestination)
    {
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(m_spDestination, bJunk))
            m_spDestination->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSMeshFluidSystem);
//---------------------------------------------------------------------------
bool NiPhysXPSMeshFluidSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::LoadBinary(NiStream& kStream)
{
    NiPSMeshParticleSystem::LoadBinary(kStream);

    m_spDestination = (NiPhysXFluidDest*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );

    NiStreamLoadBinary(kStream, m_uiMaxNumPhysXParticles);

    m_puiPhysXToGBMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);
    memset(m_puiPhysXToGBMap, 0xFF, m_uiMaxNumParticles * sizeof(NiUInt32));
    m_puiGBToPhysXMap = NiAlloc(NiUInt32, m_uiMaxNumParticles);
    m_pkPositionsBuffer = NiAlloc(NxVec3, m_uiMaxNumParticles);

    m_spFluidEmitter = (NiPhysXPSFluidEmitter*)kStream.ResolveLinkID();

    m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidData.numParticlesPtr));

    NiUInt32 uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidData.bufferPosByteStride);
        NIASSERT(uiSize == m_uiMaxNumPhysXParticles *
            m_kFluidData.bufferPosByteStride / sizeof(NxF32));
        m_kFluidData.bufferPos = NiAlloc(NxF32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidData.bufferPos, uiSize);
        m_kFluidData.bufferId = NiAlloc(NxU32, m_uiMaxNumParticles);
        m_kFluidData.bufferIdByteStride = sizeof(NxU32);
    }

    m_kFluidCreationData.numIdsPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidCreationData.numIdsPtr));
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, 
            m_kFluidCreationData.bufferIdByteStride);
        NIASSERT(uiSize == m_uiMaxNumPhysXParticles *
            m_kFluidCreationData.bufferIdByteStride / sizeof(NxU32));
        m_kFluidCreationData.bufferId = NiAlloc(NxU32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidCreationData.bufferId, 
            uiSize);
    } 

    m_kFluidDeletionData.numIdsPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidDeletionData.numIdsPtr));
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, 
            m_kFluidDeletionData.bufferIdByteStride);
        NIASSERT(uiSize == m_uiMaxNumPhysXParticles *
            m_kFluidDeletionData.bufferIdByteStride / sizeof(NxU32));
        m_kFluidDeletionData.bufferId = NiAlloc(NxU32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidDeletionData.bufferId, 
            uiSize);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::LinkObject(NiStream& kStream)
{
    NiPSMeshParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSMeshFluidSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSMeshParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSMeshFluidSystem::SaveBinary(NiStream& kStream)
{
    NiPSMeshParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spDestination);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));

    NiStreamSaveBinary(kStream, m_uiMaxNumPhysXParticles);

    kStream.SaveLinkID(m_spFluidEmitter);

    NiStreamSaveBinary(kStream, *(m_kFluidData.numParticlesPtr));
        
    NiUInt32 uiSize;
    
    if (m_kFluidData.bufferPos)
    {
        uiSize = m_uiMaxNumPhysXParticles * m_kFluidData.bufferPosByteStride
            / sizeof(NxF32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferPosByteStride);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferPos, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
    
    NiStreamSaveBinary(kStream, *(m_kFluidCreationData.numIdsPtr));
        
    if (m_kFluidCreationData.bufferId)
    {
        uiSize = m_uiMaxNumPhysXParticles * 
            m_kFluidCreationData.bufferIdByteStride / sizeof(NxU32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidCreationData.bufferIdByteStride);
        NiStreamSaveBinary(kStream, m_kFluidCreationData.bufferId, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }

    NiStreamSaveBinary(kStream, *(m_kFluidDeletionData.numIdsPtr));
        
    if (m_kFluidDeletionData.bufferId)
    {
        uiSize = m_uiMaxNumPhysXParticles * 
            m_kFluidDeletionData.bufferIdByteStride / sizeof(NxU32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidDeletionData.bufferIdByteStride);
        NiStreamSaveBinary(kStream, m_kFluidDeletionData.bufferId, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXPSMeshFluidSystem::IsEqual(NiObject* pkObject)
{
    if (!NiPSMeshParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXPSMeshFluidSystem* pkNewObject = 
        (NiPhysXPSMeshFluidSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    if (*(m_kFluidData.numParticlesPtr) != 
        *(pkNewObject->m_kFluidData.numParticlesPtr))
        return false;

    if (m_kFluidData.bufferPos)
    {
        if (!pkNewObject->m_kFluidData.bufferPos)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidData.numParticlesPtr) * 3;
            ui++)
        {
            if (m_kFluidData.bufferPos[ui] !=
                pkNewObject->m_kFluidData.bufferPos[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidData.bufferPos)
            return false;
    }

    if (m_kFluidData.bufferVel)
    {
        if (!pkNewObject->m_kFluidData.bufferVel)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidData.numParticlesPtr) * 3;
            ui++)
        {
            if (m_kFluidData.bufferVel[ui] !=
                pkNewObject->m_kFluidData.bufferVel[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidData.bufferVel)
            return false;
    }    

    if (m_kFluidCreationData.bufferId)
    {
        if (!pkNewObject->m_kFluidCreationData.bufferId)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidCreationData.numIdsPtr);
            ui++)
        {
            if (m_kFluidCreationData.bufferId[ui] !=
                pkNewObject->m_kFluidCreationData.bufferId[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidCreationData.bufferId)
            return false;
    }

    if (m_kFluidDeletionData.bufferId)
    {
        if (!pkNewObject->m_kFluidDeletionData.bufferId)
            return false;
        for (NiUInt32 ui = 0;
            ui < *(m_kFluidDeletionData.numIdsPtr);
            ui++)
        {
            if (m_kFluidDeletionData.bufferId[ui] !=
                pkNewObject->m_kFluidDeletionData.bufferId[ui])
                return false;
        }
    }
    else
    {
        if (pkNewObject->m_kFluidDeletionData.bufferId)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
