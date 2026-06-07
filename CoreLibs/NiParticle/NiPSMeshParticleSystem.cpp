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

#include "NiPSMeshParticleSystem.h"
#include "NiPSSimulatorGeneralStep.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSSimulatorCollidersStep.h"
#include "NiPSSimulatorFinalStep.h"
#include "NiPSFlagsHelpers.h"
#include "NiPSCommonSemantics.h"

#include <NiMeshUpdateProcess.h>

NiImplementRTTI(NiPSMeshParticleSystem, NiPSParticleSystem);

//---------------------------------------------------------------------------
NiPSMeshParticleSystem* NiPSMeshParticleSystem::Create(
    NiUInt32 uiMaxNumParticles,
    bool bHasColors,
    bool bHasRotations,
    bool bWorldSpace,
    bool bDynamicBounds,
    bool bAttachMeshModifiers,
    NiUInt32 uiPoolSize,
    NiUInt16 usNumGenerations,
    bool bAutoFillPools)
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
    NiPSMeshParticleSystem* pkSystem = NiNew NiPSMeshParticleSystem(
        pkSimulator,
        uiMaxNumParticles,
        bHasColors,
        bHasRotations,
        bHasRotations,
        bWorldSpace,
        pkBoundUpdater,
        uiPoolSize,
        usNumGenerations,
        bAutoFillPools);

    // Add simulator.
    pkSystem->AddModifier(pkSimulator, bAttachMeshModifiers);

    return pkSystem;
}
//---------------------------------------------------------------------------
NiPSMeshParticleSystem::NiPSMeshParticleSystem(
    NiPSSimulator* pkSimulator,
    NiUInt32 uiMaxNumParticles,
    bool bHasColors,
    bool bHasRotations,
    bool bHasRotationAxes,
    bool bWorldSpace,
    NiPSBoundUpdater* pkBoundUpdater,
    NiUInt32 uiPoolSize,
    NiUInt16 usNumGenerations,
    bool bAutoFillPools) :
    NiPSParticleSystem(
        pkSimulator,
        uiMaxNumParticles,
        bHasColors,
        bHasRotations,
        bHasRotationAxes,
        bWorldSpace,
        pkBoundUpdater),
    m_kPools(usNumGenerations),
    m_kMasterParticles(usNumGenerations),
    m_uiPoolSize(uiPoolSize),
    m_bAutoFillPools(bAutoFillPools)
{
    // Create the particle container node.
    m_spParticleContainer = NiNew NiNode(m_uiMaxNumParticles);

    // Set the pool size.
    if (m_uiPoolSize == (NiUInt32) DEFAULT_POOL_SIZE)
    {
        m_uiPoolSize = m_uiMaxNumParticles;
    }

    // Create the individual pools for each generation.
    for (NiUInt16 us = 0; us < usNumGenerations; ++us)
    {
        m_kPools.SetAt(us, NiNew NiAVObjectArray(m_uiPoolSize));
    }
}
//---------------------------------------------------------------------------
NiPSMeshParticleSystem::NiPSMeshParticleSystem() :
    m_uiPoolSize(0),
    m_bAutoFillPools(false)
{
}
//---------------------------------------------------------------------------
NiPSMeshParticleSystem::~NiPSMeshParticleSystem()
{
    // Delete each pool.
    const NiUInt32 uiPoolsCount = m_kPools.GetSize();
    for (NiUInt32 ui = 0; ui < uiPoolsCount; ++ui)
    {
        NiDelete m_kPools.GetAt(ui);
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::ResetParticleSystem()
{
    // Completes tasks.
    m_pkSimulator->ResetSimulator(this);

    // Remove each particle, adding the removed particle back to the pool for
    // its generation.
    for (NiUInt32 uiParticle = 0; uiParticle < m_uiNumParticles; ++uiParticle)
    {
        NiAVObjectPtr spRemovedParticle = m_spParticleContainer->DetachChildAt(
            uiParticle);

        NiUInt16 usGeneration = NiPSFlagsHelpers::GetGeneration(
            m_puiFlags[uiParticle]);
        if (usGeneration >= GetNumGenerations())
        {
            usGeneration = GetNumGenerations() - 1;
        }
        if (m_kPools.GetAt(usGeneration)->GetSize() <
            m_kPools.GetAt(usGeneration)->GetAllocatedSize())
        {
            m_kPools.GetAt(usGeneration)->Add(spRemovedParticle);
        }
    }

    // Call the base class version of this function to actually reset the
    // particle counts.
    NiPSParticleSystem::ResetParticleSystem();
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::FillPools()
{
    const NiUInt32 uiPoolsCount = m_kPools.GetSize();
    for (NiUInt32 ui = 0; ui < uiPoolsCount; ++ui)
    {
        // A master particle must be specified for each generation prior to
        // this function being called.
        NiAVObject* pkMasterParticle = m_kMasterParticles.GetAt(ui);
        NIASSERT(pkMasterParticle);

        NiAVObjectArray* pkPool = m_kPools.GetAt(ui);
        NIASSERT(pkPool);

        const NiUInt32 uiPoolSize = pkPool->GetAllocatedSize();
        for (NiUInt32 uj = 0; uj < uiPoolSize; ++uj)
        {
            if (!pkPool->GetAt(uj))
            {
                pkPool->SetAt(uj, (NiAVObject*) pkMasterParticle->Clone());
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::InitializeParticle(NiUInt32 uiNewParticle)
{
    NiPSParticleSystem::InitializeParticle(uiNewParticle);

    // Get particle generation.
    NiUInt32 uiFlags = m_puiFlags[uiNewParticle];
    NiUInt16 usGeneration = NiPSFlagsHelpers::GetGeneration(uiFlags);
    NiUInt16 usPoolGeneration = usGeneration;
    if (usPoolGeneration >= GetNumGenerations())
    {
        usPoolGeneration = GetNumGenerations() - 1;
    }

    // Try to pull from the pool.
    NiAVObjectPtr spClonedParticle =
        m_kPools.GetAt(usPoolGeneration)->RemoveEnd();
    if (!spClonedParticle)
    {
        // Get master particle. It is a configuration error if a master
        // particle does not exist for this generation.
        NiAVObject* pkMasterParticle =
            m_kMasterParticles.GetAt(usPoolGeneration);
        NIASSERT(pkMasterParticle);

        // If no particles in pool, clone one.
        spClonedParticle = (NiAVObject*) pkMasterParticle->Clone();
    }

    // Initialize particle and add to container.
    spClonedParticle->SetAppCulled(true);
    NiTimeController::StartAnimations(spClonedParticle, 0.0f);
    m_spParticleContainer->SetAt(uiNewParticle, spClonedParticle);
    spClonedParticle->UpdatePropertiesDownward(m_spPropertyState);
    spClonedParticle->UpdateEffectsDownward(m_spEffectState);

}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::RemoveParticle(NiUInt32 uiIndexToRemove)
{
    NiUInt32 uiFinalIndex = m_uiNumParticles - 1;

    // Get particle generation.
    NiUInt32 uiFlags = m_puiFlags[uiIndexToRemove];
    NiUInt16 usGeneration = NiPSFlagsHelpers::GetGeneration(uiFlags);

    NiPSParticleSystem::RemoveParticle(uiIndexToRemove);

    NiAVObjectPtr spRemovedParticle;
    if (uiIndexToRemove == uiFinalIndex)
    {
        spRemovedParticle = m_spParticleContainer->DetachChildAt(
            uiFinalIndex);
    }
    else
    {
        NiAVObjectPtr spLastParticle = m_spParticleContainer->DetachChildAt(
            uiFinalIndex);
        spRemovedParticle = m_spParticleContainer->DetachChildAt(
            uiIndexToRemove);
        m_spParticleContainer->SetAt(uiIndexToRemove, spLastParticle);
    }

    // We can be removing a NULL mesh when called from ResolveAddedParticles.
    if (!spRemovedParticle)
        return;

    // Add the removed particle back to the pool. It will be deleted if the
    // pool is full.
    if (usGeneration >= GetNumGenerations())
    {
        usGeneration = GetNumGenerations() - 1;
    }
    if (m_kPools.GetAt(usGeneration)->GetSize() <
        m_kPools.GetAt(usGeneration)->GetAllocatedSize())
    {
        m_kPools.GetAt(usGeneration)->Add(spRemovedParticle);
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::ResolveAddedParticles(const bool bUpdateBound)
{
    for (NiUInt32 ui = m_uiAddedParticlesBase;
        ui < m_uiAddedParticlesBase + m_uiNumAddedParticles; ++ui)
    {
        NiAVObject* pkParticle = m_spParticleContainer->GetAt(ui);
        if (pkParticle)
        {
            pkParticle->SetAppCulled(false);
        }
    }

    NiPSParticleSystem::ResolveAddedParticles(bUpdateBound);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::PostUpdate(NiUpdateProcess& kUpdate)
{
    NiPSParticleSystem::PostUpdate(kUpdate);

    if (m_uiNumParticles == 0)
    {
        return;
    }

    float fCurrentTime = kUpdate.GetTime();

    for (NiUInt32 ui = 0; ui < m_uiNumParticles; ++ui)
    {
        // Get particle.
        NiAVObject* pkParticle = m_spParticleContainer->GetAt(ui);
        NIASSERT(pkParticle);

        // Update the position, rotation, and scale of the particle.
        pkParticle->SetTranslate(m_pkPositions[ui]);
        if (m_pfRotationAngles && m_pkRotationAxes)
        {
            NiMatrix3 kRotation;
            kRotation.MakeRotation(m_pfRotationAngles[ui],
                m_pkRotationAxes[ui]);
            pkParticle->SetRotate(kRotation);
        }
        pkParticle->SetScale(m_pfSizes[ui] * m_pfRadii[ui]);

        // Update the particle with its age.
        kUpdate.SetTime(m_pfAges[ui]);
        pkParticle->UpdateDownwardPass(kUpdate);
    }

    // Set the time back to current
    kUpdate.SetTime(fCurrentTime);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::UpdatePropertiesDownward(
    NiPropertyState* pkParentState)
{
    NiPSParticleSystem::UpdatePropertiesDownward(pkParentState);

    m_spParticleContainer->UpdatePropertiesDownward(m_spPropertyState);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::UpdateEffectsDownward(
    NiDynamicEffectState* pkParentState)
{
    NiPSParticleSystem::UpdateEffectsDownward(pkParentState);

    m_spParticleContainer->UpdateEffectsDownward(m_spEffectState);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::OnVisible(NiCullingProcess& kCuller)
{
    NiPSParticleSystem::OnVisible(kCuller);

    m_spParticleContainer->OnVisible(kCuller);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::SetSelectiveUpdateFlags(
    bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms,
    bool& bRigid)
{
    NiPSParticleSystem::SetSelectiveUpdateFlags(bSelectiveUpdate,
        bSelectiveUpdateTransforms, bRigid);

    m_spParticleContainer->SetSelectiveUpdateFlags(bSelectiveUpdate, true,
        bRigid);
    bRigid = false;
    m_spParticleContainer->SetSelectiveUpdateRigid(bRigid);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::RenderImmediate(NiRenderer*)
{
    // Since this object does not contain any renderable geometry, we will
    // not call RenderMesh here. RenderImmediate will be called separately on
    // all active particles, so they do not need to be handled here.

    // The mesh modifiers should be completed no matter what.
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_RENDER;
    CompleteModifiers(&kSyncArgs);

    if (m_uiNumParticles > 0)
    {
        // Only call OnPreDisplay for time controllers if some particles exist.
        NiTimeController::OnPreDisplayIterate(GetControllers());
    }   
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::UpdateWorldData()
{
    NiPSParticleSystem::UpdateWorldData();

    m_spParticleContainer->SetRotate(m_kWorld.m_Rotate);
    m_spParticleContainer->SetTranslate(m_kWorld.m_Translate);
    m_spParticleContainer->SetScale(m_kWorld.m_fScale);
    m_spParticleContainer->UpdateWorldData();
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::UpdateWorldBound()
{
    if (m_spBoundUpdater)
    {
        m_kWorldBound = m_spParticleContainer->GetWorldBound();
    }
    else
    {
        NiPSParticleSystem::UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::PurgeRendererData(NiRenderer* pkRenderer)
{
    NiPSParticleSystem::PurgeRendererData(pkRenderer);

    // Purge particles.
    m_spParticleContainer->PurgeRendererData(pkRenderer);

    // Purge pools.
    NiUInt32 uiPoolsCount = m_kPools.GetSize();
    for (NiUInt32 ui = 0; ui < uiPoolsCount; ++ui)
    {
        NiAVObjectArray* pkPool = m_kPools.GetAt(ui);

        const NiUInt32 uiPoolSize = pkPool->GetSize();
        for (NiUInt32 uj = 0; uj < uiPoolSize; ++uj)
        {
            if (pkPool->GetAt(uj))
                pkPool->GetAt(uj)->PurgeRendererData(pkRenderer);
        }
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::RetrieveMeshSet(
    NiTPrimitiveSet<NiMesh*>& kMeshSet)
{
    NiPSParticleSystem::RetrieveMeshSet(kMeshSet);

    // Process master particles.
    const NiUInt32 uiMasterParticlesCount = m_kMasterParticles.GetSize();
    for (NiUInt32 ui = 0; ui < uiMasterParticlesCount; ++ui)
    {
        NiAVObject* pkMasterParticle = m_kMasterParticles.GetAt(ui);
        if (pkMasterParticle)
        {
            RecursiveRetrieveMeshSet(pkMasterParticle, kMeshSet);
        }
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::RecursiveRetrieveMeshSet(
    NiAVObject* pkObject,
    NiTPrimitiveSet<NiMesh*>& kMeshSet)
{
    if (NiIsKindOf(NiMesh, pkObject))
    {
        ((NiMesh*) pkObject)->RetrieveMeshSet(kMeshSet);
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        const NiUInt32 uiChildCount = pkNode->GetArrayCount();
        for (NiUInt32 ui = 0; ui < uiChildCount; ++ui)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RecursiveRetrieveMeshSet(pkChild, kMeshSet);
            }
        }
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSMeshParticleSystem);
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::CopyMembers(
    NiPSMeshParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSParticleSystem::CopyMembers(pkDest, kCloning);

    const NiUInt32 uiMasterParticlesCount = m_kMasterParticles.GetSize();
    pkDest->m_kMasterParticles.SetSize(uiMasterParticlesCount);
    for (NiUInt32 ui = 0; ui < uiMasterParticlesCount; ++ui)
    {
        // It is a configuration error if a master particle does not exist for
        // all generations.
        NiAVObject* pkMasterParticle = m_kMasterParticles.GetAt(ui);
        NIASSERT(pkMasterParticle);

        pkDest->m_kMasterParticles.SetAt(ui, (NiAVObject*)
            pkMasterParticle->CreateClone(kCloning));
    }

    pkDest->m_uiPoolSize = m_uiPoolSize;
    pkDest->m_bAutoFillPools = m_bAutoFillPools;

    // Create pools.
    const NiUInt32 uiPoolsCount = m_kPools.GetSize();
    pkDest->m_kPools.SetSize(uiPoolsCount);
    for (NiUInt32 ui = 0; ui < uiPoolsCount; ++ui)
    {
        NiAVObjectArray* pkOldPool = m_kPools.GetAt(ui);
        NIASSERT(pkOldPool);

        const NiUInt32 uiPoolSize = pkOldPool->GetAllocatedSize();
        NiAVObjectArray* pkNewPool = NiNew NiAVObjectArray(uiPoolSize);
        pkDest->m_kPools.SetAt(ui, pkNewPool);

        if (m_bAutoFillPools)
        {
            for (NiUInt32 uj = 0; uj < uiPoolSize; ++uj)
            {
                if (pkOldPool->GetAt(uj))
                {
                    pkNewPool->SetAt(uj, (NiAVObject*) pkOldPool->GetAt(uj)
                        ->CreateClone(kCloning));
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSParticleSystem::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSMeshParticleSystem* pkDest = (NiPSMeshParticleSystem*) pkObject;

    // Create the particle container node.
    pkDest->m_spParticleContainer = NiNew NiNode(m_uiMaxNumParticles);

    const NiUInt32 uiMasterParticlesCount = m_kMasterParticles.GetSize();
    for (NiUInt32 ui = 0; ui < uiMasterParticlesCount; ++ui)
    {
        // It is a configuration error if a master particle does not exist for
        // all generations.
        NiAVObject* pkMasterParticle = m_kMasterParticles.GetAt(ui);
        NIASSERT(pkMasterParticle);

        pkMasterParticle->ProcessClone(kCloning);
    }

    // Auto-fill pools.
    if (m_bAutoFillPools)
    {
        const NiUInt32 uiPoolsCount = m_kPools.GetSize();
        for (NiUInt32 ui = 0; ui < uiPoolsCount; ++ui)
        {
            NiAVObjectArray* pkPool = m_kPools.GetAt(ui);
            NIASSERT(pkPool);

            const NiUInt32 uiPoolSize = pkPool->GetAllocatedSize();
            for (NiUInt32 uj = 0; uj < uiPoolSize; ++uj)
            {
                if (pkPool->GetAt(uj))
                {
                    pkPool->GetAt(uj)->ProcessClone(kCloning);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSMeshParticleSystem);
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::LoadBinary(NiStream& kStream)
{
    NiPSParticleSystem::LoadBinary(kStream);

    NiUInt32 uiNumGenerations;
    NiStreamLoadBinary(kStream, uiNumGenerations);
    m_kMasterParticles.SetSize(uiNumGenerations);
    for (NiUInt32 ui = 0; ui < uiNumGenerations; ++ui)
    {
        m_kMasterParticles.SetAt(ui, (NiAVObject*) kStream.ResolveLinkID());
    }

    NiStreamLoadBinary(kStream, m_uiPoolSize);

    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    m_bAutoFillPools = NIBOOL_IS_TRUE(bValue);

    // Create the individual pools for each generation.
    m_kPools.SetSize(uiNumGenerations);
    for (NiUInt32 ui = 0; ui < uiNumGenerations; ++ui)
    {
        m_kPools.SetAt(ui, NiNew NiAVObjectArray(m_uiPoolSize));
    }
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::LinkObject(NiStream& kStream)
{
    NiPSParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::PostLinkObject(NiStream& kStream)
{
    NiPSParticleSystem::PostLinkObject(kStream);

    m_spParticleContainer = NiNew NiNode(m_uiMaxNumParticles);

    // Auto-fill pools.
    if (m_bAutoFillPools)
    {
        FillPools();
    }
}
//---------------------------------------------------------------------------
bool NiPSMeshParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    const NiUInt32 uiMasterParticlesCount = m_kMasterParticles.GetSize();
    for (NiUInt32 ui = 0; ui < uiMasterParticlesCount; ++ui)
    {
        // It is a configuration error if a master particle does not exist for
        // all generations.
        NiAVObject* pkMasterParticle = m_kMasterParticles.GetAt(ui);
        NIASSERT(pkMasterParticle);

        pkMasterParticle->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::SaveBinary(NiStream& kStream)
{
    NiPSParticleSystem::SaveBinary(kStream);

    const NiUInt32 uiNumGenerations = m_kMasterParticles.GetSize();
    NiStreamSaveBinary(kStream, uiNumGenerations);
    for (NiUInt32 ui = 0; ui < uiNumGenerations; ++ui)
    {
        // It is a configuration error if a master particle does not exist for
        // all generations.
        NiAVObject* pkMasterParticle = m_kMasterParticles.GetAt(ui);
        NIASSERT(pkMasterParticle);

        kStream.SaveLinkID(pkMasterParticle);
    }

    NiStreamSaveBinary(kStream, m_uiPoolSize);

    NiStreamSaveBinary(kStream, NiBool(m_bAutoFillPools));
}
//---------------------------------------------------------------------------
bool NiPSMeshParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiPSParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPSMeshParticleSystem* pkDest = (NiPSMeshParticleSystem*) pkObject;

    if (pkDest->m_kMasterParticles.GetSize() != m_kMasterParticles.GetSize())
    {
        return false;
    }

    const NiUInt32 uiMasterParticlesCount =
        pkDest->m_kMasterParticles.GetSize();
    for (NiUInt32 ui = 0; ui < uiMasterParticlesCount; ++ui)
    {
        NiAVObject* pkDestMasterParticle =
            pkDest->m_kMasterParticles.GetAt(ui);
        NiAVObject* pkSrcMasterParticle = m_kMasterParticles.GetAt(ui);
        if ((pkDestMasterParticle && !pkSrcMasterParticle) ||
            (!pkDestMasterParticle && pkSrcMasterParticle) ||
            (pkDestMasterParticle &&
                !pkDestMasterParticle->IsEqual(pkSrcMasterParticle)))
        {
            return false;
        }
    }

    if (pkDest->m_uiPoolSize != m_uiPoolSize ||
        pkDest->m_bAutoFillPools != m_bAutoFillPools)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSMeshParticleSystem::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSParticleSystem::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSMeshParticleSystem::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("NumGenerations", m_kPools.GetSize()));
    pkStrings->Add(NiGetViewerString("PoolSize", m_uiPoolSize));
    pkStrings->Add(NiGetViewerString("AutoFillPools", m_bAutoFillPools));
}
//---------------------------------------------------------------------------
