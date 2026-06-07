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

#include "NiPhysXFluidDest.h"
#include "NiPhysXFluidEmitter.h"
#include "NiPhysXPSFluidSystem.h"
#include "NiPhysXPSMeshFluidSystem.h"
#include <NiPSParticleSystem.h>

NiImplementRTTI(NiPhysXFluidDest, NiPhysXDest);

//---------------------------------------------------------------------------
NiPhysXFluidDest::NiPhysXFluidDest(NiPSParticleSystem* pkTarget, 
    NiPhysXPSFluidEmitter* pkEmitter) : m_pkTarget(pkTarget),
    m_spEmitter(pkEmitter)
{
    m_kPhysXToFluid.MakeIdentity();
    
    m_spUpdateStep = 0;
    
    SetUpdateStep();
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::SetUpdateStep()
{
    NIASSERT(m_pkTarget);
    
    NiPSSimulator* pkSimulator = m_pkTarget->GetSimulator();
    NiUInt32 uiSimulatorStepCount = pkSimulator->GetStepCount();
    for (NiUInt32 ui = 0; ui < uiSimulatorStepCount; ui++)
    {
        NiPSSimulatorStep* pkSimulatorStep = pkSimulator->GetStepAt(ui);
        if (NiIsKindOf(NiPhysXFluidSimulatorStep, pkSimulatorStep))
        {
            m_spUpdateStep = (NiPhysXFluidSimulatorStep*)pkSimulatorStep;
        }
    }
    NIASSERT(m_spUpdateStep);
}
//---------------------------------------------------------------------------
NiPhysXFluidDest::NiPhysXFluidDest() : m_pkTarget(0), m_spEmitter(0),
    m_spUpdateStep(0)
{
    m_kPhysXToFluid.MakeIdentity();
}
//---------------------------------------------------------------------------
NiPhysXFluidDest::~NiPhysXFluidDest()
{
    m_pkTarget = 0;
    m_spEmitter = 0;
    m_spUpdateStep = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::UpdateFromActors(const float fT,
    const NiTransform& kRootTransform, const float fScalePToW,
    const bool)
{
    if (!m_bActive)
        return;

    NIASSERT(!m_spUpdateStep->GetUpdateNecessary() &&
        "\nThe particle system associated with PhysX fluids must be updated\n"
        "when particles were added or removed in the previous simulation\n"
        "step.\n");

    NiTransform kFluid = m_pkTarget->GetWorldTransform();
    NiTransform kInvFluid;
    kFluid.Invert(kInvFluid);
    m_kPhysXToFluid = kInvFluid * kRootTransform;
    m_fScalePToW = fScalePToW;

    // PhysX will re-use particle IDs if multiple sub-steps are taken and a
    // particle is deleted in one sub-step and a new particle is created in
    // another. Sometimes, a particle is created and deleted in the same
    // sub-step, which produces the same data in the creation and deletion
    // buffers as the aforementioned situation, but the particle never appears
    // in the particle data array. The following code attempts to deal with
    // this problem, and makes the assumption that invalid PhysX IDs (those
    // the the particle system does not know of) have PhysX to GB map entries
    // of -1, if they are in the map at all.
    // We break all of the processing into two cases because we need
    // references to data in the particle systems, rather than copies.
    NiUInt32 uiNumCreated = 0;
    NiUInt32 uiNumRemoved = 0;
    if (NiIsKindOf(NiPhysXPSFluidSystem, m_pkTarget))
    {
        NiPhysXPSFluidSystem* pkPhysXFluid = (NiPhysXPSFluidSystem*)m_pkTarget;
        
        NxParticleIdData& kFluidCreateIds =
            pkPhysXFluid->GetParticleCreationData();
        NxParticleIdData& kFluidDeleteIds =
            pkPhysXFluid->GetParticleDeletionData();
        NiUInt32* uiPhysXtoGB = pkPhysXFluid->GetPhysXToGBMap();

        NiUInt32 uiDeleteIndex = 0;
        while (uiDeleteIndex < *kFluidDeleteIds.numIdsPtr)
        {
            if (uiPhysXtoGB[kFluidDeleteIds.bufferId[uiDeleteIndex]]
                == (NiUInt32)-1)
            {
                // We are deleting a particle that we have never seen before.
                // It should be in the creation id buffer. Remove it from both
                // buffers so that Gamebryo never knows it exists.
                NiUInt32 uiCreateIndex = 0;
                while (uiCreateIndex < *kFluidCreateIds.numIdsPtr)
                {
                    if (kFluidDeleteIds.bufferId[uiDeleteIndex] ==
                        kFluidCreateIds.bufferId[uiCreateIndex])
                    {
                        break;
                    }
                    uiCreateIndex++;
                }
                // This assert is hit when the particle is deleted but not
                // created, and Gamebryo does not know about the particle.
                NIASSERT(uiCreateIndex < *kFluidCreateIds.numIdsPtr);

                kFluidCreateIds.bufferId[uiCreateIndex] = kFluidCreateIds.
                    bufferId[((*kFluidCreateIds.numIdsPtr)--) - 1];
                kFluidDeleteIds.bufferId[uiDeleteIndex] = kFluidDeleteIds.
                    bufferId[((*kFluidDeleteIds.numIdsPtr)--) - 1];
            }
            else
            {
                NiUInt32 uiPhysXID = kFluidDeleteIds.bufferId[uiDeleteIndex];
                kFluidDeleteIds.bufferId[uiDeleteIndex] =
                    uiPhysXtoGB[uiPhysXID];
                uiPhysXtoGB[uiPhysXID] = (NiUInt32)-1;
                uiDeleteIndex++;
            }
        }

        uiNumCreated = *kFluidCreateIds.numIdsPtr;
        uiNumRemoved = *kFluidDeleteIds.numIdsPtr;
    }
    else if (NiIsKindOf(NiPhysXPSMeshFluidSystem, m_pkTarget))
    {
        NiPhysXPSMeshFluidSystem* pkPhysXFluid = 
            (NiPhysXPSMeshFluidSystem*)m_pkTarget;

        NxParticleIdData& kFluidCreateIds =
            pkPhysXFluid->GetParticleCreationData();
        NxParticleIdData& kFluidDeleteIds =
            pkPhysXFluid->GetParticleDeletionData();
        NiUInt32* uiPhysXtoGB = pkPhysXFluid->GetPhysXToGBMap();

        NiUInt32 uiDeleteIndex = 0;
        while (uiDeleteIndex < *kFluidDeleteIds.numIdsPtr)
        {
            if (uiPhysXtoGB[kFluidDeleteIds.bufferId[uiDeleteIndex]]
                == (NiUInt32)-1)
            {
                // We are deleting a particle that we have never seen before.
                // It should be in the creation id buffer. Remove it from both
                // buffers so that Gamebryo never knows it exists.
                NiUInt32 uiCreateIndex = 0;
                while (uiCreateIndex < *kFluidCreateIds.numIdsPtr)
                {
                    if (kFluidDeleteIds.bufferId[uiDeleteIndex] ==
                        kFluidCreateIds.bufferId[uiCreateIndex])
                    {
                        break;
                    }
                    uiCreateIndex++;
                }
                // This assert is hit when the particle is deleted but not
                // created, and Gamebryo does not know about the particle.
                NIASSERT(uiCreateIndex < *kFluidCreateIds.numIdsPtr);

                kFluidCreateIds.bufferId[uiCreateIndex] = kFluidCreateIds.
                    bufferId[((*kFluidCreateIds.numIdsPtr)--) - 1];
                kFluidDeleteIds.bufferId[uiDeleteIndex] = kFluidDeleteIds.
                    bufferId[((*kFluidDeleteIds.numIdsPtr)--) - 1];
            }
            else
            {
                NiUInt32 uiPhysXID = kFluidDeleteIds.bufferId[uiDeleteIndex];
                kFluidDeleteIds.bufferId[uiDeleteIndex] =
                    uiPhysXtoGB[uiPhysXID];
                uiPhysXtoGB[uiPhysXID] = (NiUInt32)-1;
                uiDeleteIndex++;
            }
        }

        uiNumCreated = *kFluidCreateIds.numIdsPtr;
        uiNumRemoved = *kFluidDeleteIds.numIdsPtr;
    }
    else
    {
        NIASSERT(false && "NiPhysXFluidDest: Wrong type\n");
    }

    // Emit newly created fluid particles.
    m_spEmitter->EmitParticles(m_pkTarget, fT, uiNumCreated, 0);

    // Inform the simulator step that it needs to do some work.
    m_spUpdateStep->SetUpdateUseful(true);
    
    // And inform it if it must process added/removed particles
    m_spUpdateStep->SetUpdateNecessary(uiNumCreated || uiNumRemoved);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXFluidDest);
//---------------------------------------------------------------------------
void NiPhysXFluidDest::CopyMembers(NiPhysXFluidDest* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXDest::CopyMembers(pkDest, kCloning);

    // Cached for updating of particles
    pkDest->m_kPhysXToFluid.MakeIdentity();
    pkDest->m_fScalePToW = 1.0f;

    // We expect the target to have already been cloned with the same cloning
    // process.
    pkDest->m_pkTarget =
        (NiPSParticleSystem*)m_pkTarget->CreateSharedClone(kCloning);
    
    // The fluid emitter is cloned as if sharing, but since nothing else
    // shares the original emitter, nothing will share this either. We just
    // want anything else holding a pointer to the emitter to get the same
    // emitter clone.
    pkDest->m_spEmitter =
        (NiPhysXPSFluidEmitter*)m_spEmitter->CreateSharedClone(kCloning);

    pkDest->m_spUpdateStep = 0;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXDest::ProcessClone(kCloning);

    bool bCloned = false;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
        
    bool bJunk;
    
    if (!kCloning.m_pkProcessMap->GetAt(m_pkTarget, bJunk))
        m_pkTarget->ProcessClone(kCloning);
    
    if (!kCloning.m_pkProcessMap->GetAt(m_spEmitter, bJunk))
        m_spEmitter->ProcessClone(kCloning);
        
    NIASSERT(NiIsKindOf(NiPhysXFluidDest, pkCloneObj));
    NiPhysXFluidDest* pkClone = (NiPhysXFluidDest*)pkCloneObj;
    pkClone->SetUpdateStep();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXFluidDest::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXDest::RegisterStreamables(kStream))
        return false;

    m_pkTarget->RegisterStreamables(kStream);
    m_spEmitter->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::SaveBinary(NiStream& kStream)
{
    NiPhysXDest::SaveBinary(kStream);
    
    kStream.SaveLinkID(m_pkTarget);
    kStream.SaveLinkID(m_spEmitter);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidDest);
//---------------------------------------------------------------------------
void NiPhysXFluidDest::LoadBinary(NiStream& kStream)
{
    NiPhysXDest::LoadBinary(kStream);

    m_pkTarget = (NiPSParticleSystem*)kStream.ResolveLinkID();

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        m_spEmitter = (NiPhysXPSFluidEmitter*)kStream.ResolveLinkID();
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidDest::LinkObject(NiStream& kStream)
{
    NiPhysXDest::LinkObject(kStream);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        SetUpdateStep();
    }
}
//---------------------------------------------------------------------------
bool NiPhysXFluidDest::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXFluidDest, pkObject));
    if(!NiPhysXDest::IsEqual(pkObject))
        return false;

    NiPhysXFluidDest* pkNewObject = (NiPhysXFluidDest*)pkObject;

    if (!m_pkTarget->IsEqual(pkNewObject->m_pkTarget) ||
        !m_spEmitter->IsEqual(pkNewObject->m_spEmitter))
        return false;

    return true;
}
//---------------------------------------------------------------------------
