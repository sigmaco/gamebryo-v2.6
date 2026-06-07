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

#include "NiPhysXPSParticleSystem.h"
#include "NiPhysXPSSimulatorInitialStep.h"
#include "NiPhysXPSSimulatorFinalStep.h"
#include "NiPhysXParticle.h"
#include "NiPhysXManager.h"

NiImplementRTTI(NiPhysXPSParticleSystem, NiPSParticleSystem);

//---------------------------------------------------------------------------
NiPhysXPSParticleSystem::NiPhysXPSParticleSystem(NiPSSimulator* pkSimulator,
    NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations,
    bool bWorldSpace, NiPSBoundUpdater* pkBoundUpdater, 
    NiPhysXPSParticleSystemProp* pkProp, NiUInt32 uiCompartmentID, 
    bool bPhysXSpace, bool bPhysXRotations, NiUInt32 uiActorPoolSize, 
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes) : NiPSParticleSystem(
    pkSimulator, uiMaxNumParticles, bHasColors, 
    (bHasRotations || bPhysXRotations), (bHasRotations || bPhysXRotations), 
    bWorldSpace, pkBoundUpdater), m_ppkActors(0), m_pkLastVelocities(0),
    m_pkActorPose(0), m_pkActorVelocity(0)
{
    SetOnConstruction(uiMaxNumParticles, pkProp, uiCompartmentID,
        uiActorPoolSize,  bFillActorPoolsOnLoad, bKeepsMeshes, bPhysXRotations,
        bPhysXSpace);
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem::NiPhysXPSParticleSystem(NiPSParticleSystem* pkPSys,
    NiPhysXPSParticleSystemProp* pkProp, NiUInt32 uiCompartmentID, 
    bool bPhysXSpace, bool bPhysXRotations, NiUInt32 uiActorPoolSize, 
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes) : NiPSParticleSystem(),
    m_ppkActors(0), m_pkLastVelocities(0), m_pkActorPose(0), 
    m_pkActorVelocity(0)
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    ((NiPhysXPSParticleSystem*)pkPSys)->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);

    // Cloning misses setting the target on the controllers attached to the
    // particle system object. Go through and set them now.
    NiTimeController* pkControllers = GetControllers();
    while (pkControllers)
    {
        pkControllers->SetTarget(this);
        pkControllers = pkControllers->GetNext();
    }

    NiPSSimulator* pkNewSimulator = NiNew NiPhysXPSSimulator();
    // First add the NiPhysXPSSimulatorInitialStep.
    pkNewSimulator->AddStep(NiNew NiPhysXPSSimulatorInitialStep());

    // Iterate over all the original steps and add them, but don't
    // add the NiPSSimulatorFinalStep, it will be replaced by a
    // NiPhysXPSSimulatorFinalStep.
    for (NiUInt32 ui = 0; ui < m_pkSimulator->GetStepCount(); ui++)
    {
        NiPSSimulatorStep* pkStep = m_pkSimulator->GetStepAt(ui);
        if (!NiDynamicCast(NiPSSimulatorFinalStep, pkStep))
        {
            pkNewSimulator->AddStep(pkStep);
        }
    }

    // Add the NiPhysXPSSimulatorFinalStep as the last step.
    pkNewSimulator->AddStep(NiNew NiPhysXPSSimulatorFinalStep());

    // Detach the current simulator from the particle system and 
    // attach the new simulator.
    RemoveModifier(m_pkSimulator);
    m_pkSimulator = pkNewSimulator;
    AddModifierAt(0, m_pkSimulator, true);

    // Set the stuff the clone didn't catch
    SetOnConstruction(m_uiMaxNumParticles, pkProp, uiCompartmentID,
        uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes, bPhysXRotations,
        bPhysXSpace);
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem::NiPhysXPSParticleSystem()
{
    m_uFlags = 0;

    m_ppkActors = 0;
    m_pkLastVelocities = 0;
    m_pkActorPose = 0;
    m_pkActorVelocity = 0;

    m_pkScene = 0;
    m_pkProp = 0;
    m_spDest = 0;
    m_uiCompartmentID = 0;

    m_kPhysXToPSys.MakeIdentity();
    m_fScalePToW = 1.0f;

    m_uiDefaultActorPoolSize =
        (NiUInt32)NiPSMeshParticleSystem::DEFAULT_POOL_SIZE;
    m_kActorPoolCenter = NiPoint3::ZERO;
    m_kActorPoolDim = NiPoint3::ZERO;

    m_spActorDescriptor = 0;
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem::~NiPhysXPSParticleSystem()
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

    EmptyActorPool();

    DetachFromProp();
    m_spDest = 0;

    FreeDataBuffers();
    
    m_spActorDescriptor = 0;
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem* NiPhysXPSParticleSystem::Create(
    NiUInt32 uiMaxNumParticles, bool bHasColors, bool bHasRotations, 
    bool bWorldSpace, bool bDynamicBounds, bool bCreateDefaultGenerator,
    bool bAttachMeshModifiers, NiPhysXPSParticleSystemProp* pkProp, 
    NiUInt32 uiCompartmentID, bool bPhysXSpace, bool bPhysXRotations,
    NiUInt32 uiActorPoolSize, bool bFillActorPoolsOnLoad, bool bKeepsMeshes)
{
    // Create simulator.
    NiPhysXPSSimulator* pkSimulator = NiNew NiPhysXPSSimulator();

    // Add simulation steps.
    pkSimulator->AddStep(NiNew NiPhysXPSSimulatorInitialStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorGeneralStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorForcesStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorCollidersStep());
    pkSimulator->AddStep(NiNew NiPhysXPSSimulatorFinalStep());

    // Create bound updater, if requested.
    NiPSBoundUpdater* pkBoundUpdater = NULL;
    if (bDynamicBounds)
    {
        pkBoundUpdater = NiNew NiPSBoundUpdater();
    }

    NiPhysXPSParticleSystem* pkPhysXPSystem = NiNew NiPhysXPSParticleSystem(
        pkSimulator, uiMaxNumParticles, bHasColors, bHasRotations, bWorldSpace,
        pkBoundUpdater, pkProp, uiCompartmentID, bPhysXSpace, bPhysXRotations,
        uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes);

    // Add simulator.
    pkPhysXPSystem->AddModifier(pkSimulator, bAttachMeshModifiers);

    // Create and add default generator, if requested.
    if (bCreateDefaultGenerator)
    {
        pkPhysXPSystem->AddModifier(NiNew NiPSFacingQuadGenerator(),
            bAttachMeshModifiers);
    }

    return pkPhysXPSystem;
}
//---------------------------------------------------------------------------
NiPhysXPSParticleSystem* NiPhysXPSParticleSystem::Create(
    NiPSParticleSystem *pkPSys, NiPhysXPSParticleSystemProp* pkProp, 
    NiUInt32 uiCompartmentID, bool bPhysXSpace, bool bPhysXRotations, 
    NiUInt32 uiActorPoolSize, bool bFillActorPoolsOnLoad, bool bKeepsMeshes)
{
    NiPhysXPSParticleSystem* pkPhysXPSystem = NiNew NiPhysXPSParticleSystem(
        pkPSys, pkProp, uiCompartmentID, bPhysXSpace, bPhysXRotations, 
        uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes);

    return pkPhysXPSystem;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::SetOnConstruction(NiUInt32 usMaxNumParticles,
    NiPhysXPSParticleSystemProp* pkProp, NiUInt32 uiCompartmentID,
    NiUInt32 uiActorPoolSize, bool bFillActorPoolOnLoad, bool bKeepsMeshes,
    bool bPhysXRotations, bool bPhysXSpace)
{
    m_kPhysXToPSys.MakeIdentity();
    m_fScalePToW = 1.0f;

    m_spActorDescriptor = 0;

    NIASSERT(usMaxNumParticles > 0);

    // Set the scene in which it resides to 0 for now
    m_pkScene = 0;
    m_uiCompartmentID = uiCompartmentID;

    SetBit(bFillActorPoolOnLoad, FLAG_FILL_POOLS_ON_LOAD);
    SetBit(bKeepsMeshes, FLAG_KEEP_ACTOR_MESHES);

    // Initialize the size of the pool
    m_uiDefaultActorPoolSize = uiActorPoolSize;
    if (m_uiDefaultActorPoolSize ==
        (NiUInt32)NiPSMeshParticleSystem::DEFAULT_POOL_SIZE)
    {
        m_uiDefaultActorPoolSize = usMaxNumParticles;
    }

    m_kActorPool.SetSize(m_uiDefaultActorPoolSize);
    m_kActorPoolCenter = NiPoint3::ZERO;
    m_kActorPoolDim = NiPoint3(100.0f, 100.0f, 100.0f);

    SetBit(bPhysXRotations, FLAG_PHYSX_ROTATIONS);
    SetBit(bPhysXSpace, FLAG_PHYSX_SPACE);
    SetBit(false, FLAG_NEEDS_UPDATE);

    // Reallocate the data buffers.
    AllocateDataBuffers(m_uiMaxNumParticles, HasColors(), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()));

    m_pkProp = pkProp;
    if (pkProp)
    {
        // Set up source to out of PhysX.
        pkProp->AddSystem(this);
        m_spDest = NiNew NiPhysXPSParticleSystemDest(this);
        pkProp->AddDestination(m_spDest);
        pkProp->AddModifiedMesh(this);

        // Attach the scene, which will fill actor pools if necessary
        if (pkProp->GetScene())
            SetScene(pkProp->GetScene());
    }
    else
    {
        m_spDest = 0;
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::AllocateDataBuffers(NiUInt32 uiBufferSize,
    bool bAllocateColors, bool bAllocateRotations, bool bAllocateRotationAxes)
{
    FreeDataBuffers();

    NiPSParticleSystem::AllocateDataBuffers(uiBufferSize, bAllocateColors, 
        bAllocateRotations, bAllocateRotationAxes);

    m_ppkActors = NiAlloc(NxActor*, uiBufferSize);
    m_pkLastVelocities = NiAlignedAlloc(NiPoint3, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);

    m_pkActorPose = NiAlignedAlloc(NxMat34, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);

    m_pkActorVelocity = NiAlignedAlloc(NxVec3, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);

    memset(m_ppkActors, 0, sizeof(NxActor*) * uiBufferSize);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::FreeDataBuffers()
{
    NiFree(m_ppkActors);
    m_ppkActors = NULL;

    NiAlignedFree(m_pkLastVelocities);
    m_pkLastVelocities = NULL;

    NiAlignedFree(m_pkActorPose);
    m_pkActorPose = NULL;
    
    NiAlignedFree(m_pkActorVelocity);
    m_pkActorVelocity = NULL;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::UpdateParticlesUponCompletion()
{
    NiPSParticleSystem::UpdateParticlesUponCompletion();

    if (!m_pkScene)
        return;

    // Push GB particle state to PhysX.
    NiPhysXManager::GetPhysXManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ui++)
    {
        NxActor* pkActor = m_ppkActors[ui];

        pkActor->addForce(m_pkActorVelocity[ui], NX_VELOCITY_CHANGE);
    }
    NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::AttachToProp(NiPhysXPSParticleSystemProp* pkProp)
{
    DetachFromProp();

    AttachToScene(pkProp->GetScene());

    m_pkProp = pkProp;    
    pkProp->AddSystem(this);
    m_spDest = NiNew NiPhysXPSParticleSystemDest(this);
    pkProp->AddDestination(m_spDest);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::DetachFromProp()
{
    if (!m_pkProp)
        return;

    // Cache the prop pointer then set it to null. This function can get
    // called recursively and we don't want the second call to do anything.
    NiPhysXPSParticleSystemProp* pkProp = m_pkProp;
    m_pkProp = 0;

    DetachFromScene();

    pkProp->RemoveSystem(this);
    pkProp->DeleteDestination(m_spDest);

    m_spDest = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::AttachToScene(NiPhysXScene* pkScene)
{
    if (m_pkScene)
        DetachFromScene();

    SetScene(pkScene);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::DetachFromScene(void)
{
    ResetParticleSystem();
    EmptyActorPool();
    m_pkScene = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::InitializeParticle(NiUInt32 uiNewParticle)
{
    NiPSParticleSystem::InitializeParticle(uiNewParticle);
    
    if (!m_pkScene)
    {
        m_ppkActors[uiNewParticle] = NULL;
        return;
    }
        
    NIASSERT(m_spActorDescriptor &&
        "NULL actor descriptor in NiPhysXPSParticleSystem");

    // Try to pull from the pool
    NxActor* pkActor = RemoveActorFromPool();

    NiPhysXManager::GetPhysXManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    // If no actor was available in the pool, create one.
    if (!pkActor)
    {
        NxCompartment* pkCompartment = 0;
        if (m_uiCompartmentID)
        {
            pkCompartment =
                m_pkScene->GetCompartmentAt(m_uiCompartmentID);
        }
        
        pkActor = m_spActorDescriptor->CreateActor(m_pkScene->GetPhysXScene(),
            pkCompartment, 0, NiPhysXTypes::NXMAT34_ID,
            GetBit(FLAG_KEEP_ACTOR_MESHES));
    }
    pkActor->clearActorFlag(NX_AF_DISABLE_COLLISION);

    if (GetBit(FLAG_PHYSX_SPACE))
    {
        NxVec3 kVec;            
        if (m_pfRotationAngles && m_pkRotationAxes)
        {
            NiMatrix3 kRotate;
            kRotate.MakeRotation(m_pfRotationAngles[uiNewParticle],
                m_pkRotationAxes[uiNewParticle]);
            NiPoint3 kPosn = m_pkPositions[uiNewParticle] / m_fScalePToW;
            NxMat34 kPose;
            NiPhysXTypes::NiTransformToNxMat34(kRotate, kPosn, kPose);
            pkActor->setGlobalPose(kPose);
        }
        else
        {
            NiPoint3 kPosn = m_pkPositions[uiNewParticle] / m_fScalePToW;
            NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
            pkActor->setGlobalPosition(kVec);
        }        
        NiPhysXTypes::NiPoint3ToNxVec3(m_pkVelocities[uiNewParticle], kVec);
        kVec /= m_fScalePToW;
        pkActor->setLinearVelocity(kVec);
    }
    else
    {
        NiTransform m_kPSysToPhysX;
        m_kPhysXToPSys.Invert(m_kPSysToPhysX);

        NxVec3 kVec;            
        if (m_pfRotationAngles && m_pkRotationAxes)
        {
            NiMatrix3 kLocalRotate;
            kLocalRotate.MakeRotation(m_pfRotationAngles[uiNewParticle],
                m_pkRotationAxes[uiNewParticle]);
            NiMatrix3 kRotate = m_kPSysToPhysX.m_Rotate 
                * kLocalRotate;
            NiPoint3 kPosn = m_kPSysToPhysX * m_pkPositions[uiNewParticle];
            kPosn /= m_fScalePToW;
            NxMat34 kPose;
            NiPhysXTypes::NiTransformToNxMat34(kRotate, kPosn, kPose);
            pkActor->setGlobalPose(kPose);
        }
        else
        {
            NiPoint3 kPosn = m_kPSysToPhysX * m_pkPositions[uiNewParticle];
            kPosn /= m_fScalePToW;
            NiPhysXTypes::NiPoint3ToNxVec3(kPosn, kVec);
            pkActor->setGlobalPosition(kVec);
        }
        NiPoint3 kVelocity = 
            m_kPSysToPhysX.m_Rotate * m_pkVelocities[uiNewParticle];
        kVelocity /= m_fScalePToW;
        NiPhysXTypes::NiPoint3ToNxVec3(kVelocity, kVec);
        pkActor->setLinearVelocity(kVec);
    }

    NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();

    m_ppkActors[uiNewParticle] = pkActor;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::RemoveParticle(NiUInt32 uiIndexToRemove)
{
    NiUInt32 uiFinalIndex = m_uiNumParticles - 1;

    NiPSParticleSystem::RemoveParticle(uiIndexToRemove);

    NxActor* pkRemovedActor = m_ppkActors[uiIndexToRemove];
    m_ppkActors[uiIndexToRemove] = 0;
   
    // If the index is less than the maximum number of particles,
    // then swap position of the last element to uiIndexRemove.
    if (uiIndexToRemove < uiFinalIndex)
    {
        m_ppkActors[uiIndexToRemove] = m_ppkActors[uiFinalIndex];
        m_pkActorVelocity[uiIndexToRemove] = m_pkActorVelocity[uiFinalIndex];
    }

    if (pkRemovedActor)
    {
        // Add the actor to the appropriate pool. It will be deleted if the
        // pool is full
        AddActorToPool(pkRemovedActor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::ResetParticleSystem()
{
    // Completes tasks.
    m_pkSimulator->ResetSimulator(this);

    // Remove each particle, adding the actor back to the pool.
    for (NiUInt32 uiParticle = 0; uiParticle < m_uiNumParticles; ++uiParticle)
    {
        NxActor* pkRemovedActor = m_ppkActors[uiParticle];
        if (pkRemovedActor)
        {
            // Add the actor to the appropriate pool. It will be deleted if
            // the pool is full
            AddActorToPool(pkRemovedActor);
        }
        m_ppkActors[uiParticle] = 0;
    }

    // Call the base class version of this function to actually reset the
    // particle counts.
    NiPSParticleSystem::ResetParticleSystem();
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::SetScene(NiPhysXScene* pkScene)
{
    if (m_pkScene)
    {
        // Clear out all particles.
        ResetParticleSystem();

        // Clear the actor pool
        EmptyActorPool();
    }

    m_pkScene = pkScene;

    if(!m_pkScene)
        return;

    // Prime the transforms. If they change later, it will get caught in an
    // update.
    NiTransform kInvPSysXform;
    GetWorldTransform().Invert(kInvPSysXform);
    m_kPhysXToPSys = kInvPSysXform * m_pkScene->GetSceneXform();
    m_fScalePToW = m_pkScene->GetScaleFactor();

    m_kActorPool.SetSize(m_uiDefaultActorPoolSize);

    if (GetBit(FLAG_FILL_POOLS_ON_LOAD) && m_uiDefaultActorPoolSize > 0)
    {
        FillActorPool(m_spActorDescriptor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::FillActorPool(NiPhysXActorDesc* pkOriginal)
{
    if (!m_pkScene || !m_pkScene->GetPhysXScene())
        return;

    // Get any compartment
    NxCompartment* pkCompartment = 0;
    if (m_uiCompartmentID)
    {
        pkCompartment = m_pkScene->GetCompartmentAt(m_uiCompartmentID);
    }

    NIASSERT(pkOriginal != NULL);

    NiPhysXManager::GetPhysXManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    for(NiUInt32 ui = 0; ui < m_kActorPool.GetAllocatedSize(); ui++)
    {
        NxActor* pkActor = pkOriginal->CreateActor(m_pkScene->GetPhysXScene(),
            pkCompartment, 0, NiPhysXTypes::NXMAT34_ID,
            GetBit(FLAG_KEEP_ACTOR_MESHES));

        if (!pkActor)
            continue;

        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);

        NxVec3 kPosn(
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.x
            + m_kActorPoolCenter.x,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.y
            + m_kActorPoolCenter.y,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.z
            + m_kActorPoolCenter.z);
        pkActor->setGlobalPosition(kPosn);

        m_kActorPool.SetAt(ui, pkActor);
    }
    NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::AddActorToPool(NxActor* pkActor)
{
    if (!pkActor)
        return;

    // Add the actor to the pool if it isn't full, if full
    // then delete the actor.
    if (m_kActorPool.GetSize() < m_kActorPool.GetAllocatedSize())
    {
        m_kActorPool.Add(pkActor);
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        NIASSERT(!m_pkScene->GetInSimFetch());
        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
    else
    {
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        NIASSERT(!m_pkScene->GetInSimFetch());
        m_pkScene->GetPhysXScene()->releaseActor(*pkActor);
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::EmptyActorPool()
{
    // It could be that the PhysX scene has already been deleted, in which
    // case the actors are all gone.
    if (m_pkScene && m_pkScene->GetPhysXScene())
    {
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        for (NiUInt32 ui = 0; ui < m_kActorPool.GetSize(); ui++)
        {
            NxActor* pkActor = m_kActorPool.GetAt(ui);
            if (pkActor)
                m_pkScene->GetPhysXScene()->releaseActor(*pkActor);
        }
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
    m_kActorPool.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::MapActorPool(NiTMap<NxActor*, bool>& kMap)
{
    for (NiUInt32 uj = 0; uj < m_kActorPool.GetSize(); uj++)
    {
        NxActor* pkActor = m_kActorPool.GetAt(uj);
        if (pkActor)
            kMap.SetAt(pkActor, true);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::GetPhysXState()
{
    if (!m_pkScene)
        return;

    // Push PhysX particle state to GB.
    NiPhysXManager::GetPhysXManager()->WaitSDKLock();
    NIASSERT(!m_pkScene->GetInSimFetch());
    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ui++)
    {
        NxActor* pkActor = m_ppkActors[ui];

        m_pkActorPose[ui] = pkActor->getGlobalPose();

        m_pkActorVelocity[ui] = pkActor->getLinearVelocity();
    }
    NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSParticleSystem);
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::CopyMembers(NiPhysXPSParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSParticleSystem::CopyMembers(pkDest, kCloning);

    if (!NiIsKindOf(NiPhysXPSParticleSystem, this))
        return;

    pkDest->m_pkProp = 0;
    pkDest->m_spDest = 0;

     // The cloned particle system must be attached to a prop and scene
    pkDest->m_pkScene = 0;
    
    pkDest->m_uFlags = m_uFlags;
    SetBit(false, FLAG_NEEDS_UPDATE);

    pkDest->AllocateDataBuffers(m_uiMaxNumParticles, HasColors(), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()));

    // Copy particle data. This also copies the actor pointers, which we want
    // to zero so that new actors are created later.
    for (NiUInt16 us = 0; us < GetNumParticles(); us++)
    {
        // Make sure the old actor is gone
        pkDest->m_ppkActors[us] = 0;
    }

    pkDest->m_uiDefaultActorPoolSize = m_uiDefaultActorPoolSize;
    pkDest->m_kActorPoolCenter = m_kActorPoolCenter;
    pkDest->m_kActorPoolDim = m_kActorPoolDim;
    
    // We do not clone the pool - systems do not share pools
    pkDest->m_kActorPool.SetSize(m_uiDefaultActorPoolSize);
    
    pkDest->m_spActorDescriptor =
        (NiPhysXActorDesc*)m_spActorDescriptor->CreateSharedClone(kCloning);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSParticleSystem::ProcessClone(kCloning);

    NiObject* pkClone = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));

    NiPhysXPSParticleSystem* pkDest = (NiPhysXPSParticleSystem*)pkClone;
    pkDest->SetScene(0);

    m_spActorDescriptor->ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSParticleSystem);
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::LoadBinary(NiStream& kStream)
{
    NiPSParticleSystem::LoadBinary(kStream);

    m_pkProp = (NiPhysXPSParticleSystemProp*)kStream.ResolveLinkID();
    m_spDest = (NiPhysXPSParticleSystemDest*)kStream.ResolveLinkID();

    m_pkScene = (NiPhysXScene*)kStream.ResolveLinkID();

    NiStreamLoadBinary(kStream, m_uFlags);
    SetBit(false, FLAG_NEEDS_UPDATE);
    NiStreamLoadBinary(kStream, m_uiDefaultActorPoolSize);

    NiUInt32 uiGenerationPoolSize;
    NiStreamLoadBinary(kStream, uiGenerationPoolSize);
    m_kActorPoolCenter.LoadBinary(kStream);
    m_kActorPoolDim.LoadBinary(kStream);

    if (uiGenerationPoolSize > 0)
    {
        m_kActorPool.SetSize(uiGenerationPoolSize);
    }

    m_spActorDescriptor =(NiPhysXActorDesc*)kStream.ResolveLinkID();

    AllocateDataBuffers(m_uiMaxNumParticles, HasColors(), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()), 
        (GetBit(FLAG_PHYSX_ROTATIONS) || HasRotations()));
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::LinkObject(NiStream& kStream)
{
    NiPSParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::PostLinkObject(NiStream& kStream)
{
    NiPSParticleSystem::PostLinkObject(kStream);

    if (GetBit(FLAG_FILL_POOLS_ON_LOAD))
        FillActorPool(m_spActorDescriptor);
}
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_pkProp)
        m_pkProp->RegisterStreamables(kStream);
    if (m_spDest)
        m_spDest->RegisterStreamables(kStream);

    if (m_pkScene) 
        m_pkScene->RegisterStreamables(kStream);
    
    if (m_spActorDescriptor)
        m_spActorDescriptor->RegisterStreamables(kStream);

    return true;

}
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::SaveBinary(NiStream& kStream)
{
    NiPSParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkProp);
    kStream.SaveLinkID(m_spDest);
    
    kStream.SaveLinkID(m_pkScene);
    
    NiStreamSaveBinary(kStream, m_uFlags);
    NiStreamSaveBinary(kStream, m_uiDefaultActorPoolSize);

    NiStreamSaveBinary(kStream, m_kActorPool.GetAllocatedSize());
    m_kActorPoolCenter.SaveBinary(kStream);
    m_kActorPoolDim.SaveBinary(kStream);

    kStream.SaveLinkID(m_spActorDescriptor);
}
//---------------------------------------------------------------------------
bool NiPhysXPSParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiPSParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXPSParticleSystem* pkNewObject = (NiPhysXPSParticleSystem*)pkObject;

    // some m_uFlags are dependent on Update so we check them individually
    if (GetFillActorPoolOnLoad() != pkNewObject->GetFillActorPoolOnLoad() ||
        GetKeepsActorMeshes() != pkNewObject->GetKeepsActorMeshes() ||
        GetUsesPhysXSpace() != pkNewObject->GetUsesPhysXSpace() ||
        GetUsesPhysXRotations() != pkNewObject->GetUsesPhysXRotations())
    {
        return false;
    }

    if (m_uiDefaultActorPoolSize != pkNewObject->m_uiDefaultActorPoolSize ||
        m_kActorPoolCenter != pkNewObject->m_kActorPoolCenter ||
        m_kActorPoolDim != pkNewObject->m_kActorPoolDim)
    {
        return false;
    }
                    
    if (m_kActorPool.GetAllocatedSize() !=
        pkNewObject->m_kActorPool.GetAllocatedSize())
        return false;

    if (!m_spActorDescriptor->IsEqual(pkNewObject->m_spActorDescriptor))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSParticleSystem::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSParticleSystem::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPhysXPSParticleSystem::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("PhysXSpace",
        GetBit(FLAG_PHYSX_SPACE)));
    pkStrings->Add(NiGetViewerString("PhysXRotations",
        GetBit(FLAG_PHYSX_ROTATIONS)));

    if (m_spDest)
    {
        m_spDest->GetViewerStrings(pkStrings);
    }
    else
    {
        pkStrings->Add(NiGetViewerString("m_spDest", 0));
    }

    pkStrings->Add(NiGetViewerString("FillActorPoolsOnLoad",
        GetBit(FLAG_FILL_POOLS_ON_LOAD)));
    pkStrings->Add(NiGetViewerString("KeepsActorMeshes",
        GetBit(FLAG_KEEP_ACTOR_MESHES)));
    pkStrings->Add(NiGetViewerString("DefaultActorPoolSize",
        m_uiDefaultActorPoolSize));
        
    if (m_spActorDescriptor)
    {
        m_spActorDescriptor->GetViewerStrings(pkStrings);
    }
    else
    {
        pkStrings->Add(NiGetViewerString("m_spActorDescriptor", 0));
    }
}
//---------------------------------------------------------------------------
