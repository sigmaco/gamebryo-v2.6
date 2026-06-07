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

#include "NiPSParticleSystem.h"
#include <NiVersion.h>
#include "NiPSCommonSemantics.h"
#include "NiPSKernelDefinitions.h"
#include "NiPSSimulatorGeneralStep.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSSimulatorCollidersStep.h"
#include "NiPSSimulatorFinalStep.h"
#include "NiPSFacingQuadGenerator.h"
#include "NiPSFlagsHelpers.h"
#include "NiPSMetrics.h"

NiImplementRTTI(NiPSParticleSystem, NiMesh);

const NiUInt32 NiPSParticleSystem::INVALID_PARTICLE = (NiUInt32) -1;

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING(NiParticle);
//---------------------------------------------------------------------------
NiPSParticleSystem* NiPSParticleSystem::Create(
    NiUInt32 uiMaxNumParticles,
    bool bHasColors,
    bool bHasRotations,
    bool bWorldSpace,
    bool bDynamicBounds,
    bool bCreateDefaultGenerator,
    bool bAttachMeshModifiers)
{
    // Create simulator.
    NiPSSimulator* pkSimulator = NiNew NiPSSimulator();

    // Add simulation steps.
    pkSimulator->AddStep(NiNew NiPSSimulatorGeneralStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorForcesStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorCollidersStep());
    pkSimulator->AddStep(NiNew NiPSSimulatorFinalStep());

    // Create bound updater, if requested.
    NiPSBoundUpdater* pkBoundUpdater = NULL;
    if (bDynamicBounds)
    {
        pkBoundUpdater = NiNew NiPSBoundUpdater();
    }

    // Create particle system.
    NiPSParticleSystem* pkSystem = NiNew NiPSParticleSystem(
        pkSimulator,
        uiMaxNumParticles,
        bHasColors,
        bHasRotations,
        false,
        bWorldSpace,
        pkBoundUpdater);

    // Add simulator.
    pkSystem->AddModifier(pkSimulator, bAttachMeshModifiers);

    // Create and add default generator, if requested.
    if (bCreateDefaultGenerator)
    {
        pkSystem->AddModifier(NiNew NiPSFacingQuadGenerator(),
            bAttachMeshModifiers);
    }

    return pkSystem;
}
//---------------------------------------------------------------------------
NiPSParticleSystem::NiPSParticleSystem() :
    m_pkSimulator(NULL),
    m_pkDeathSpawner(NULL),
    m_fLastTime(-NI_INFINITY),
    m_uiMaxNumParticles(0),
    m_uiNumParticles(0),
    m_uiAddedParticlesBase(0),
    m_uiNumAddedParticles(0),
    m_pkPositions(NULL),
    m_pkVelocities(NULL),
    m_pfAges(NULL),
    m_pfLifeSpans(NULL),
    m_pfLastUpdateTimes(NULL),
    m_puiFlags(NULL),
    m_pfRadii(NULL),
    m_pfSizes(NULL),
    m_pkColors(NULL),
    m_pfRotationAngles(NULL),
    m_pfRotationSpeeds(NULL),
    m_pkRotationAxes(NULL),
    m_fPreviousWorldScale(1.0f),
    m_bWorldSpace(true),
    m_bCompleteSimulation(false),
    m_bScaleSet(false)
{
}
//---------------------------------------------------------------------------
NiPSParticleSystem::NiPSParticleSystem(
    NiPSSimulator* pkSimulator,
    NiUInt32 uiMaxNumParticles,
    bool bHasColors,
    bool bHasRotations,
    bool bHasRotationAxes,
    bool bWorldSpace,
    NiPSBoundUpdater* pkBoundUpdater) :
    m_pkSimulator(pkSimulator),
    m_spBoundUpdater(pkBoundUpdater),
    m_pkDeathSpawner(NULL),
    m_fLastTime(-NI_INFINITY),
    m_uiMaxNumParticles(uiMaxNumParticles),
    m_uiNumParticles(0),
    m_uiAddedParticlesBase(0),
    m_uiNumAddedParticles(0),
    m_pkPositions(NULL),
    m_pkVelocities(NULL),
    m_pfAges(NULL),
    m_pfLifeSpans(NULL),
    m_pfLastUpdateTimes(NULL),
    m_puiFlags(NULL),
    m_pfRadii(NULL),
    m_pfSizes(NULL),
    m_pkColors(NULL),
    m_pfRotationAngles(NULL),
    m_pfRotationSpeeds(NULL),
    m_pkRotationAxes(NULL),
    m_fPreviousWorldScale(1.0f),
    m_bWorldSpace(bWorldSpace),
    m_bCompleteSimulation(false),
    m_bScaleSet(false)
{
    NIASSERT(m_pkSimulator);

    AllocateDataBuffers(m_uiMaxNumParticles, bHasColors, bHasRotations,
        bHasRotationAxes);
}
//---------------------------------------------------------------------------
NiPSParticleSystem::~NiPSParticleSystem()
{
    // Must do this here so that the object is still alive to
    // complete simulation.
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteModifiers(&kSyncArgs);

    // Detach the modifier to free the tasks which hold pointers to the
    // streams we are about to delete.
    RemoveModifier(m_pkSimulator);

    RemoveAllSpawners();

    m_spBoundUpdater = 0;

    FreeDataBuffers();
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::ResetParticleSystem()
{
    // Completes tasks.
    m_pkSimulator->ResetSimulator(this);

    m_uiNumParticles = 0;
    m_uiNumAddedParticles = 0;
    m_uiAddedParticlesBase = 0;
    m_fLastTime = -NI_INFINITY;
}
//---------------------------------------------------------------------------
NiUInt32 NiPSParticleSystem::AddParticle()
{
    // Simulation may be active. We can't just add the particle because it
    // will confuse the particle count. So stash it at the end but do
    // not increment the particle count.
    if (m_uiNumAddedParticles == 0)
    {
        m_uiAddedParticlesBase = m_uiNumParticles;
    }

    // Ensure that we are not exceeding our max number of particles.
    if (m_uiAddedParticlesBase + m_uiNumAddedParticles <
        m_uiMaxNumParticles)
    {
        return (m_uiAddedParticlesBase + m_uiNumAddedParticles++);
    }
    else
    {
        return INVALID_PARTICLE;
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::InitializeParticle(NiUInt32 uiNewParticle)
{
    // Set the size correctly. The emitter does not have enough information to
    // set this and it must be set before the first particle update.
    NiPSSimulatorGeneralStep* pkGeneralStep = (NiPSSimulatorGeneralStep*)
        m_pkSimulator->GetSimulatorStepByType(
        &NiPSSimulatorGeneralStep::ms_RTTI);
    if (pkGeneralStep)
    {
        float fAge = m_pfAges[uiNewParticle];
        float fGrow = 1.0f;
        float fGrowTime = pkGeneralStep->GetGrowTime();

        NiUInt32 uiFlags = m_puiFlags[uiNewParticle];
        NiUInt16 usGeneration = NiPSFlagsHelpers::GetGeneration(uiFlags);

        if (usGeneration == pkGeneralStep->GetGrowGeneration() &&
            fAge < fGrowTime && fGrowTime > 0.0f)
        {
            fGrow = fAge / fGrowTime;
        }

        float fShrink = 1.0f;
        float fShrinkTime = pkGeneralStep->GetShrinkTime();
        float fTimeLeft = m_pfLifeSpans[uiNewParticle] - fAge;
        if (usGeneration == pkGeneralStep->GetShrinkGeneration() &&
            fTimeLeft < fShrinkTime &&
            fShrinkTime > 0.0f)
        {
            fShrink = fTimeLeft / fShrinkTime;
        }

        m_pfSizes[uiNewParticle] = NiMin(fGrow, fShrink);
        if (m_pfSizes[uiNewParticle] < NIPSKERNEL_EPSILON)
        {
            m_pfSizes[uiNewParticle] = NIPSKERNEL_EPSILON;
        }
    }
}
//---------------------------------------------------------------------------
NiPSEmitter* NiPSParticleSystem::GetEmitterByName(const NiFixedString& kName)
    const
{
    const NiUInt32 uiEmitterCount = m_kEmitters.GetSize();
    for (NiUInt32 ui = 0; ui < uiEmitterCount; ++ui)
    {
        NiPSEmitter* pkEmitter = m_kEmitters.GetAt(ui);
        if (pkEmitter->GetName() == kName)
        {
            return pkEmitter;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::AddSpawner(NiPSSpawner* pkSpawner)
{
    NIASSERT(pkSpawner);

    m_kSpawners.Add(pkSpawner);

    pkSpawner->SetMasterPSystem(this);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::RemoveSpawnerAt(NiUInt32 uiIndex, bool bMaintainOrder)
{
    NIASSERT(uiIndex < m_kSpawners.GetSize());

    // Reset the spawner ID.
    NiPSSpawner* pkOldSpawner = m_kSpawners.GetAt(uiIndex);
    pkOldSpawner->SetMasterPSystem(NULL);

    if (bMaintainOrder)
    {
        m_kSpawners.OrderedRemoveAt(uiIndex);
    }
    else
    {
        m_kSpawners.RemoveAt(uiIndex);
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::RemoveAllSpawners()
{
    // Reset the spawner IDs.
    const NiUInt32 uiSpawnerCount = m_kSpawners.GetSize();
    for (NiUInt32 ui = 0; ui < uiSpawnerCount; ++ui)
    {
        NiPSSpawner* pkSpawner = m_kSpawners.GetAt(ui);
            
        pkSpawner->SetMasterPSystem(NULL);
    }

    m_kSpawners.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    // Perform pre-update steps.
    PreUpdate(kUpdate);

    NiMesh::UpdateDownwardPass(kUpdate);

    // Perform post-update steps.
    PostUpdate(kUpdate);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    // Perform pre-update steps.
    PreUpdate(kUpdate);

    NiMesh::UpdateSelectedDownwardPass(kUpdate);

    // Perform post-update steps.
    PostUpdate(kUpdate);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    // Perform pre-update steps.
    PreUpdate(kUpdate);

    NiMesh::UpdateRigidDownwardPass(kUpdate);

    // Perform post-update steps.
    PostUpdate(kUpdate);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::PreUpdate(NiUpdateProcess& kUpdate)
{
    // Do nothing if modifiers will not be submitted.
    if (!kUpdate.GetSubmitModifiers())
        return;

    // Catch the case where object is updated twice in a row without being
    // rendered.
    if (m_bCompleteSimulation)
    {
        NiSyncArgs kCompleteArgs;
        kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_UPDATE;
        kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
        CompleteModifiers(&kCompleteArgs);
    }
    m_bCompleteSimulation = true;

    NIMETRICS_PARTICLE_SCOPETIMER(UPDATE_PSYS_TIME);

    float fTime = kUpdate.GetTime();

    // Initialize the last time.
    if (m_fLastTime == -NI_INFINITY)
    {
        m_fLastTime = fTime;
    }

    // Reset particle system if the last time is greater than the current time.
    if (m_fLastTime > fTime)
    {
        ResetParticleSystem();
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::PostUpdate(NiUpdateProcess& kUpdate)
{
    // Do nothing if modifiers will not be submitted.
    if (!kUpdate.GetSubmitModifiers())
        return;

    // Update the last time.
    m_fLastTime = kUpdate.GetTime();
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::CompleteSimulation()
{
    UpdateParticlesUponCompletion();

    // Update bound.
    if (m_spBoundUpdater)
    {
        m_spBoundUpdater->UpdateBound(this);
    }

    m_bCompleteSimulation = false;

    NIMETRICS_PARTICLE_ADDVALUE(UPDATED_PARTICLES, m_uiNumParticles);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::UpdateParticlesUponCompletion()
{
    // Add any particles that were emitted during the simulation phase. Doing
    // this now, before particles are deleted, reduces copying of particle
    // data.
    ResolveAddedParticles(false);

    // Process particles that need to be spawned or killed.
    ResolveSpawnedAndRemovedParticles(m_fLastTime);

    if (m_bWorldSpace)
    {
        if (m_bScaleSet && m_kWorld.m_fScale != m_fPreviousWorldScale)
        {
            // The world scale has been changed. We need to un-apply the
            // difference between the old scale and the new scale from the
            // particle positions so that existing particles do not change
            // position.
            float fScaleDiffInv = m_fPreviousWorldScale / m_kWorld.m_fScale;
            for (NiUInt32 ui = 0; ui < m_uiNumParticles; ++ui)
            {
                m_pkPositions[ui] *= fScaleDiffInv;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::SetSelectiveUpdateFlags(
    bool& bSelectiveUpdate,
    bool bSelectiveUpdateTransforms,
    bool& bRigid)
{
    bSelectiveUpdate = true;
    bSelectiveUpdateTransforms = true;
    bRigid = false;
    SetSelectiveUpdate(bSelectiveUpdate);
    SetSelectiveUpdateTransforms(bSelectiveUpdateTransforms);
    SetSelectiveUpdatePropertyControllers(true);
    SetSelectiveUpdateRigid(bRigid);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::RenderImmediate(NiRenderer* pkRenderer)
{
    if (m_uiNumParticles > 0)
    {
        NiMesh::RenderImmediate(pkRenderer);
    }
    else
    {
        // This code is taken from NiMesh::RenderImmediate. If no particles
        // exist, we don't want to render the system, but we do want to
        // complete any modifiers that are currently executing.
        NiSyncArgs kCompleteArgs;
        kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
        kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_RENDER;
        CompleteModifiers(&kCompleteArgs);
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::UpdateWorldData()
{
    m_fPreviousWorldScale = m_kWorld.m_fScale;

    NiMesh::UpdateWorldData();

    m_kUnmodifiedWorld = m_kWorld;

    if (m_bWorldSpace)
    {
        m_kWorld.m_Translate = NiPoint3::ZERO;
        m_kWorld.m_Rotate = NiMatrix3::IDENTITY;
    }

    m_bScaleSet = true;
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::UpdateWorldBound()
{
    if (m_bWorldSpace)
    {
        if (m_spBoundUpdater)
        {
            m_kWorldBound.SetCenterAndRadius(
                m_kWorld.m_fScale * m_kBound.GetCenter(),
                m_kWorld.m_fScale * m_kBound.GetRadius());
        }
        else
        {
            m_kWorldBound.Update(m_kBound, m_kUnmodifiedWorld);
        }
    }
    else
    {
        NiMesh::UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::ResolveSpawnedAndRemovedParticles(float fTime)
{
#ifdef _XENON
    // Prefetch the flags array to reduce L2 cache misses.
    NiUInt32 uiTotalSize = m_uiNumParticles * sizeof(NiUInt32);
    for (NiUInt32 ui = 0; ui < uiTotalSize; ui += 128)
    {
        __dcbt(ui, m_puiFlags);
    }
#endif  // #ifdef _XENON

    // Build set of particle indices that should be removed.
    NIASSERT(m_kParticlesToProcess.GetSize() == 0);
    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ++ui)
    {
        ParticleProcessInfo kInfo;
        bool bAddToSet = false;

        NiUInt32& uiFlags = m_puiFlags[ui];
        if (NiPSFlagsHelpers::GetShouldSpawn(uiFlags))
        {
            kInfo.m_bShouldSpawn = true;
            kInfo.m_ucSpawnerID = NiPSFlagsHelpers::GetSpawnerID(uiFlags);
            bAddToSet = true;
            NiPSFlagsHelpers::SetShouldSpawn(uiFlags, false);
        }
        if (NiPSFlagsHelpers::GetShouldDie(uiFlags))
        {
            kInfo.m_bShouldDie = true;
            bAddToSet = true;
            NiPSFlagsHelpers::SetShouldDie(uiFlags, false);
        }

        if (bAddToSet)
        {
            kInfo.m_uiIndex = ui;
            m_kParticlesToProcess.Add(kInfo);
        }
    }

    // Iterate over array of indices to remove backwards, removing each
    // particle.
#if NIMETRICS
    NiUInt32 uiNumSpawned = 0;
    NiUInt32 uiNumDestroyed = 0;
#endif  // #if NIMETRICS
    for (NiUInt32 ui = m_kParticlesToProcess.GetSize(); ui > 0; --ui)
    {
        ParticleProcessInfo kInfo = m_kParticlesToProcess.GetAt(ui - 1);

        if (kInfo.m_bShouldSpawn)
        {
            NIASSERT(kInfo.m_ucSpawnerID < GetMaxValidSpawnerID());
            
            // Retrieve spawner from ID.
            NiPSSpawner* pkSpawner = GetSpawnerFromID(kInfo.m_ucSpawnerID);

            NIASSERT(pkSpawner);
                        
            if (pkSpawner)
            {
                // Use the last update time for the particle as the spawn time.
                float fSpawnTime = m_pfLastUpdateTimes[kInfo.m_uiIndex];

                pkSpawner->SpawnParticles(fTime, fSpawnTime, kInfo.m_uiIndex,
                    this);

#if NIMETRICS
                uiNumSpawned++;
#endif  // #if NIMETRICS
            }
        }

        if (kInfo.m_bShouldDie)
        {
            RemoveParticle(kInfo.m_uiIndex);

#if NIMETRICS
            uiNumDestroyed++;
#endif  // #if NIMETRICS
        }
    }

    m_kParticlesToProcess.RemoveAll();
    
    // Clear active spawners. The list is repopulated each time update is
    // called.
    m_kActiveSpawners.RemoveAll();

#if NIMETRICS
    if (uiNumSpawned > 0)
    {
        NIMETRICS_PARTICLE_ADDVALUE(PARTICLES_SPAWNED, uiNumSpawned);
    }
    if (uiNumDestroyed > 0)
    {
        NIMETRICS_PARTICLE_ADDVALUE(PARTICLES_DESTROYED, uiNumDestroyed);
    }
#endif  // #if NIMETRICS
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::ResolveAddedParticles(const bool bUpdateBound)
{
    if (m_uiNumParticles < m_uiAddedParticlesBase)
    {
        // Some particles were removed; fill array so that it is packed.
        // This only happens when a spawner adds particles.
        NiUInt32 uiIndex = m_uiNumParticles;
        m_uiNumParticles = m_uiAddedParticlesBase + m_uiNumAddedParticles;
        while (uiIndex < m_uiAddedParticlesBase &&
            m_uiNumParticles > m_uiAddedParticlesBase)
        {
            RemoveParticle(uiIndex++);
        }

        // If more particles were removed than were added, adjust number
        // of particles appropriately.
        if (uiIndex < m_uiAddedParticlesBase)
        {
            m_uiNumParticles = uiIndex;
        }
    }
    else
    {
        // m_uiNumParticles should never be greater than
        // m_uiAddedParticlesBase.
        NIASSERT(m_uiNumParticles == m_uiAddedParticlesBase);
        m_uiNumParticles += m_uiNumAddedParticles;
        
        if (bUpdateBound && m_spBoundUpdater && m_uiNumParticles &&
            m_uiNumParticles == m_uiNumAddedParticles)
        {
            // Update the bound because the entire set of particles is new.
            m_spBoundUpdater->UpdateBound(this);
        }
    }

    m_uiNumAddedParticles = 0;
    m_uiAddedParticlesBase = m_uiNumParticles;
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::RemoveParticle(NiUInt32 uiIndexToRemove)
{
    NIASSERT(uiIndexToRemove < m_uiNumParticles);
    NiUInt32 uiFinalIndex = m_uiNumParticles - 1;

    // Copy the particle data from the final particle into the slot for the
    // removed particle to ensure that the particle array is always packed.
    m_pkPositions[uiIndexToRemove] = m_pkPositions[uiFinalIndex];
    m_pkVelocities[uiIndexToRemove] = m_pkVelocities[uiFinalIndex];
    m_pfAges[uiIndexToRemove] = m_pfAges[uiFinalIndex];
    m_pfLifeSpans[uiIndexToRemove] = m_pfLifeSpans[uiFinalIndex];
    m_pfLastUpdateTimes[uiIndexToRemove] = m_pfLastUpdateTimes[uiFinalIndex];
    m_puiFlags[uiIndexToRemove] = m_puiFlags[uiFinalIndex];
    m_pfRadii[uiIndexToRemove] = m_pfRadii[uiFinalIndex];
    m_pfSizes[uiIndexToRemove] = m_pfSizes[uiFinalIndex];
    if (m_pkColors)
    {
        m_pkColors[uiIndexToRemove] = m_pkColors[uiFinalIndex];
    }
    if (m_pfRotationAngles)
    {
        m_pfRotationAngles[uiIndexToRemove] = m_pfRotationAngles[uiFinalIndex];
    }
    if (m_pfRotationSpeeds)
    {
        m_pfRotationSpeeds[uiIndexToRemove] = m_pfRotationSpeeds[uiFinalIndex];
    }
    if (m_pkRotationAxes)
    {
        m_pkRotationAxes[uiIndexToRemove] = m_pkRotationAxes[uiFinalIndex];
    }

    m_uiNumParticles--;
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::AllocateDataBuffers(
    NiUInt32 uiBufferSize,
    bool bAllocateColors,
    bool bAllocateRotations,
    bool bAllocateRotationAxes)
{
    FreeDataBuffers();

    // Floodgate performance is better with aligned memory.
    m_pkPositions = NiAlignedAlloc(NiPoint3, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);
    m_pkVelocities = NiAlignedAlloc(NiPoint3, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);
    m_pfAges = NiAlignedAlloc(float, uiBufferSize, NIPSKERNEL_ALIGNMENT);
    m_pfLifeSpans = NiAlignedAlloc(float, uiBufferSize, NIPSKERNEL_ALIGNMENT);
    m_pfLastUpdateTimes = NiAlignedAlloc(float, uiBufferSize,
        NIPSKERNEL_ALIGNMENT);
    m_puiFlags = NiAlignedAlloc(NiUInt32, uiBufferSize, NIPSKERNEL_ALIGNMENT);
    m_pfRadii = NiAlignedAlloc(float, uiBufferSize, NIPSKERNEL_ALIGNMENT);
    m_pfSizes = NiAlignedAlloc(float, uiBufferSize, NIPSKERNEL_ALIGNMENT);
    if (bAllocateColors)
    {
        m_pkColors = NiAlignedAlloc(NiRGBA, uiBufferSize,
            NIPSKERNEL_ALIGNMENT);
    }
    if (bAllocateRotations)
    {
        m_pfRotationAngles = NiAlignedAlloc(float, uiBufferSize,
            NIPSKERNEL_ALIGNMENT);
        m_pfRotationSpeeds = NiAlignedAlloc(float, uiBufferSize,
            NIPSKERNEL_ALIGNMENT);
    }
    if (bAllocateRotationAxes)
    {
        m_pkRotationAxes = NiAlignedAlloc(NiPoint3, uiBufferSize,
            NIPSKERNEL_ALIGNMENT);
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::FreeDataBuffers()
{
    NiAlignedFree(m_pkPositions);
    m_pkPositions = NULL;

    NiAlignedFree(m_pkVelocities);
    m_pkVelocities = NULL;

    NiAlignedFree(m_pfAges);
    m_pfAges = NULL;

    NiAlignedFree(m_pfLifeSpans);
    m_pfLifeSpans = NULL;

    NiAlignedFree(m_pfLastUpdateTimes);
    m_pfLastUpdateTimes = NULL;

    NiAlignedFree(m_puiFlags);
    m_puiFlags = NULL;

    NiAlignedFree(m_pfRadii);
    m_pfRadii = NULL;

    NiAlignedFree(m_pfSizes);
    m_pfSizes = NULL;

    NiAlignedFree(m_pkColors);
    m_pkColors = NULL;

    NiAlignedFree(m_pfRotationAngles);
    m_pfRotationAngles = NULL;

    NiAlignedFree(m_pfRotationSpeeds);
    m_pfRotationSpeeds = NULL;

    NiAlignedFree(m_pkRotationAxes);
    m_pkRotationAxes = NULL;
}
//---------------------------------------------------------------------------
NiPSParticleSystem::ParticleProcessInfo::ParticleProcessInfo() :
    m_uiIndex(0),
    m_ucSpawnerID(0),
    m_bShouldSpawn(false),
    m_bShouldDie(false)
{
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSParticleSystem);
//---------------------------------------------------------------------------
void NiPSParticleSystem::CopyMembers(
    NiPSParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    // We must complete all modifiers before accessing the particle data.
    ForceSimulationToComplete();

    NiMesh::CopyMembers(pkDest, kCloning);

    if (m_spBoundUpdater)
    {
        pkDest->m_spBoundUpdater = (NiPSBoundUpdater*)
            m_spBoundUpdater->CreateClone(kCloning);
    }

    const NiUInt32 uiEmittersCount = m_kEmitters.GetSize();
    for (NiUInt32 ui = 0; ui < uiEmittersCount; ++ui)
    {
        pkDest->AddEmitter((NiPSEmitter*)
            m_kEmitters.GetAt(ui)->CreateClone(kCloning));
    }

    const NiUInt32 uiSpawnersCount = m_kSpawners.GetSize();
    for (NiUInt32 ui = 0; ui < uiSpawnersCount; ++ui)
    {
        pkDest->AddSpawner((NiPSSpawner*)
            m_kSpawners.GetAt(ui)->CreateClone(kCloning));
    }

    pkDest->m_uiMaxNumParticles = m_uiMaxNumParticles;
    pkDest->m_uiNumParticles = m_uiNumParticles;

    pkDest->m_uiAddedParticlesBase = m_uiAddedParticlesBase;
    pkDest->m_uiNumAddedParticles = m_uiNumAddedParticles;

    pkDest->m_bWorldSpace = m_bWorldSpace;
    pkDest->m_bCompleteSimulation = m_bCompleteSimulation;

    // Particle data is not shared, so copy it here.
    pkDest->AllocateDataBuffers(m_uiMaxNumParticles, HasColors(),
        HasRotations(), HasRotationAxes());
    NiMemcpy(pkDest->m_pkPositions, m_uiMaxNumParticles * sizeof(NiPoint3),
        m_pkPositions, m_uiNumParticles * sizeof(NiPoint3));
    NiMemcpy(pkDest->m_pkVelocities, m_uiMaxNumParticles * sizeof(NiPoint3),
        m_pkVelocities, m_uiNumParticles * sizeof(NiPoint3));
    NiMemcpy(pkDest->m_pfAges, m_uiMaxNumParticles * sizeof(float), m_pfAges,
        m_uiNumParticles * sizeof(float));
    NiMemcpy(pkDest->m_pfLifeSpans, m_uiMaxNumParticles * sizeof(float),
        m_pfLifeSpans, m_uiNumParticles * sizeof(float));
    NiMemcpy(pkDest->m_pfLastUpdateTimes, m_uiMaxNumParticles * sizeof(float),
        m_pfLastUpdateTimes, m_uiNumParticles * sizeof(float));
    NiMemcpy(pkDest->m_puiFlags, m_uiMaxNumParticles * sizeof(NiUInt32),
        m_puiFlags, m_uiNumParticles * sizeof(NiUInt32));
    NiMemcpy(pkDest->m_pfRadii, m_uiMaxNumParticles * sizeof(float), m_pfRadii,
        m_uiNumParticles * sizeof(float));
    NiMemcpy(pkDest->m_pfSizes, m_uiMaxNumParticles * sizeof(float), m_pfSizes,
        m_uiNumParticles * sizeof(float));
    if (HasColors())
    {
        NiMemcpy(pkDest->m_pkColors, m_uiMaxNumParticles * sizeof(NiRGBA),
            m_pkColors, m_uiNumParticles * sizeof(NiRGBA));
    }
    if (HasRotations())
    {
        NiMemcpy(pkDest->m_pfRotationAngles,
            m_uiMaxNumParticles * sizeof(float), m_pfRotationAngles,
            m_uiNumParticles * sizeof(float));
        NiMemcpy(pkDest->m_pfRotationSpeeds,
            m_uiMaxNumParticles * sizeof(float), m_pfRotationSpeeds,
            m_uiNumParticles * sizeof(float));
    }
    if (HasRotationAxes())
    {
        NiMemcpy(pkDest->m_pkRotationAxes,
            m_uiMaxNumParticles * sizeof(NiPoint3), m_pkRotationAxes,
            m_uiNumParticles * sizeof(NiPoint3));
    }
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiMesh::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSParticleSystem* pkDest = (NiPSParticleSystem*) pkObject;

    // The simulator must have been cloned during this cloning operation. It
    // cannot be shared between particle system clones.
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkSimulator, pkObject));
    pkDest->m_pkSimulator = (NiPSSimulator*) pkObject;

    if (m_spBoundUpdater)
    {
        m_spBoundUpdater->ProcessClone(kCloning);
    }

    const NiUInt32 uiEmittersCount = m_kEmitters.GetSize();
    for (NiUInt32 ui = 0; ui < uiEmittersCount; ++ui)
    {
        m_kEmitters.GetAt(ui)->ProcessClone(kCloning);
    }

    const NiUInt32 uiSpawnersCount = m_kSpawners.GetSize();
    for (NiUInt32 ui = 0; ui < uiSpawnersCount; ++ui)
    {
        m_kSpawners.GetAt(ui)->ProcessClone(kCloning);
    }

    if (kCloning.m_pkCloneMap->GetAt(m_pkDeathSpawner, pkObject))
    {
        pkDest->m_pkDeathSpawner = (NiPSSpawner*) pkObject;
    }
    else
    {
        pkDest->m_pkDeathSpawner = m_pkDeathSpawner;
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSParticleSystem);
//---------------------------------------------------------------------------
void NiPSParticleSystem::LoadBinary(NiStream& kStream)
{
    NiMesh::LoadBinary(kStream);

    m_pkSimulator = (NiPSSimulator*) kStream.ResolveLinkID();

    m_spBoundUpdater = (NiPSBoundUpdater*) kStream.ResolveLinkID();

    NiUInt32 uiEmittersCount;
    NiStreamLoadBinary(kStream, uiEmittersCount);
    for (NiUInt32 ui = 0; ui < uiEmittersCount; ++ui)
    {
        AddEmitter((NiPSEmitter*) kStream.ResolveLinkID());
    }

    NiUInt32 uiSpawnersCount;
    NiStreamLoadBinary(kStream, uiSpawnersCount);
    for (NiUInt32 ui = 0; ui < uiSpawnersCount; ++ui)
    {
        AddSpawner((NiPSSpawner*) kStream.ResolveLinkID());
    }

    m_pkDeathSpawner = (NiPSSpawner*) kStream.ResolveLinkID();

    NiStreamLoadBinary(kStream, m_uiMaxNumParticles);

    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    bool bHasColors = NIBOOL_IS_TRUE(bValue);
    NiStreamLoadBinary(kStream, bValue);
    bool bHasRotations = NIBOOL_IS_TRUE(bValue);
    NiStreamLoadBinary(kStream, bValue);
    bool bHasRotationAxes = NIBOOL_IS_TRUE(bValue);
    NiStreamLoadBinary(kStream, bValue);
    m_bWorldSpace = NIBOOL_IS_TRUE(bValue);

    AllocateDataBuffers(m_uiMaxNumParticles, bHasColors, bHasRotations,
        bHasRotationAxes);
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::LinkObject(NiStream& kStream)
{
    NiMesh::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSParticleSystem::RegisterStreamables(NiStream& kStream)
{
    // We must complete all modifiers before accessing the particle data.
    ForceSimulationToComplete();

    if (!NiMesh::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spBoundUpdater)
    {
        m_spBoundUpdater->RegisterStreamables(kStream);
    }

    const NiUInt32 uiEmittersCount = m_kEmitters.GetSize();
    for (NiUInt32 ui = 0; ui < uiEmittersCount; ++ui)
    {
        m_kEmitters.GetAt(ui)->RegisterStreamables(kStream);
    }

    const NiUInt32 uiSpawnersCount = m_kSpawners.GetSize();
    for (NiUInt32 ui = 0; ui < uiSpawnersCount; ++ui)
    {
        m_kSpawners.GetAt(ui)->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSParticleSystem::SaveBinary(NiStream& kStream)
{
    // We must complete all modifiers before accessing the particle data.
    ForceSimulationToComplete();

    NiMesh::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkSimulator);

    kStream.SaveLinkID(m_spBoundUpdater);

    const NiUInt32 uiEmittersCount = m_kEmitters.GetSize();
    NiStreamSaveBinary(kStream, uiEmittersCount);
    for (NiUInt32 ui = 0; ui < uiEmittersCount; ++ui)
    {
        kStream.SaveLinkID(m_kEmitters.GetAt(ui));
    }

    const NiUInt32 uiSpawnersCount = m_kSpawners.GetSize();
    NiStreamSaveBinary(kStream, uiSpawnersCount);
    for (NiUInt32 ui = 0; ui < uiSpawnersCount; ++ui)
    {
        kStream.SaveLinkID(m_kSpawners.GetAt(ui));
    }

    kStream.SaveLinkID(m_pkDeathSpawner);

    NiStreamSaveBinary(kStream, m_uiMaxNumParticles);

    NiStreamSaveBinary(kStream, NiBool(HasColors()));
    NiStreamSaveBinary(kStream, NiBool(HasRotations()));
    NiStreamSaveBinary(kStream, NiBool(HasRotationAxes()));
    NiStreamSaveBinary(kStream, NiBool(m_bWorldSpace));
}
//---------------------------------------------------------------------------
bool NiPSParticleSystem::IsEqual(NiObject* pkObject)
{
    // We must complete all modifiers before accessing the particle data.
    ForceSimulationToComplete();

    if (!NiMesh::IsEqual(pkObject))
    {
        return false;
    }

    NiPSParticleSystem* pkDest = (NiPSParticleSystem*) pkObject;

    if ((pkDest->m_pkSimulator && !m_pkSimulator) ||
        (!pkDest->m_pkSimulator && m_pkSimulator))
    {
        return false;
    }

    if ((pkDest->m_spBoundUpdater && !m_spBoundUpdater) ||
        (!pkDest->m_spBoundUpdater && m_spBoundUpdater) ||
        (pkDest->m_spBoundUpdater &&
            !pkDest->m_spBoundUpdater->IsEqual(m_spBoundUpdater)))
    {
        return false;
    }

    if (pkDest->m_kEmitters.GetSize() != m_kEmitters.GetSize())
    {
        return false;
    }

    const NiUInt32 uiEmittersCount = pkDest->m_kEmitters.GetSize();
    for (NiUInt32 ui = 0; ui < uiEmittersCount; ++ui)
    {
        if (!pkDest->m_kEmitters.GetAt(ui)->IsEqual(m_kEmitters.GetAt(ui)))
        {
            return false;
        }
    }

    if (pkDest->m_kSpawners.GetSize() != pkDest->m_kSpawners.GetSize())
    {
        return false;
    }

    const NiUInt32 uiSpawnersCount = pkDest->m_kSpawners.GetSize();
    for (NiUInt32 ui = 0; ui < uiSpawnersCount; ++ui)
    {
        if (!pkDest->m_kSpawners.GetAt(ui)->IsEqual(m_kSpawners.GetAt(ui)))
        {
            return false;
        }
    }

    if ((pkDest->m_pkDeathSpawner && !m_pkDeathSpawner) ||
        (!pkDest->m_pkDeathSpawner && m_pkDeathSpawner))
    {
        return false;
    }

    if (pkDest->m_uiMaxNumParticles != m_uiMaxNumParticles ||
        pkDest->m_bWorldSpace != m_bWorldSpace)
    {
        return false;
    }

    if ((pkDest->m_pkPositions && !m_pkPositions) ||
        (!pkDest->m_pkPositions && m_pkPositions) ||
        (pkDest->m_pkVelocities && !m_pkVelocities) ||
        (!pkDest->m_pkVelocities && m_pkVelocities) ||
        (pkDest->m_pfAges && !m_pfAges) ||
        (!pkDest->m_pfAges && m_pfAges) ||
        (pkDest->m_pfLifeSpans && !m_pfLifeSpans) ||
        (!pkDest->m_pfLifeSpans && m_pfLifeSpans) ||
        (pkDest->m_pfLastUpdateTimes && !m_pfLastUpdateTimes) ||
        (!pkDest->m_pfLastUpdateTimes && m_pfLastUpdateTimes) ||
        (pkDest->m_puiFlags && !m_puiFlags) ||
        (!pkDest->m_puiFlags && m_puiFlags) ||
        (pkDest->m_pfRadii && !m_pfRadii) ||
        (!pkDest->m_pfRadii && m_pfRadii) ||
        (pkDest->m_pfSizes && !m_pfSizes) ||
        (!pkDest->m_pfSizes && m_pfSizes) ||
        (pkDest->m_pkColors && !m_pkColors) ||
        (!pkDest->m_pkColors && m_pkColors) ||
        (pkDest->m_pfRotationAngles && !m_pfRotationAngles) ||
        (!pkDest->m_pfRotationAngles && m_pfRotationAngles) ||
        (pkDest->m_pfRotationSpeeds && !m_pfRotationSpeeds) ||
        (!pkDest->m_pfRotationSpeeds && m_pfRotationSpeeds) ||
        (pkDest->m_pkRotationAxes && !m_pkRotationAxes) ||
        (!pkDest->m_pkRotationAxes && m_pkRotationAxes))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSParticleSystem::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMesh::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSParticleSystem::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Simulator", m_pkSimulator));
    pkStrings->Add(NiGetViewerString("BoundUpdater", m_spBoundUpdater));
    pkStrings->Add(NiGetViewerString("NumEmitters", m_kEmitters.GetSize()));
    pkStrings->Add(NiGetViewerString("NumSpawners", m_kSpawners.GetSize()));
    pkStrings->Add(NiGetViewerString("DeathSpawner", m_pkDeathSpawner));
    pkStrings->Add(NiGetViewerString("MaxNumParticles", m_uiMaxNumParticles));
    pkStrings->Add(NiGetViewerString("NumParticles", m_uiNumParticles));
    pkStrings->Add(NiGetViewerString("HasColors", HasColors()));
    pkStrings->Add(NiGetViewerString("HasRotations", HasRotations()));
    pkStrings->Add(NiGetViewerString("HasRotationAxes", HasRotationAxes()));
    pkStrings->Add(NiGetViewerString("WorldSpace", m_bWorldSpace));
}
//---------------------------------------------------------------------------
