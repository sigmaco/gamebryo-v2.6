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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"
#include "NiMesh.h"


NiImplementRTTI(NiPhysXScene, NiObjectNET);    
        
//---------------------------------------------------------------------------
NiPhysXScene::NiPhysXScene(NiSPWorkflowManager* pkWorkflowManager) : 
    m_pkPhysXScene(0), m_kCompartmentMap(7), m_spTriangleDebugGeometries(0), 
    m_spLineDebugGeometries(0), m_spPointDebugGeometries(0)
{
    m_pkManager = NiPhysXManager::GetPhysXManager();
    
    m_kSceneXform.MakeIdentity();
    m_fScalePToW = 1.0f;
    
    m_uFlags = 0x0;
    SetUpdateSrc(false);
    SetUpdateDest(false);

    SetDoFixedStep(true);
    SetAsynchronousSimulation(false);
    m_fTimestep = 1.0f / 60.0f;
    m_uiNumSubSteps = 1;
    m_uiMaxSubSteps = 8;
    
    m_bInSimFetch = false;
    m_fPrevFetchTime = 0.0f;
    m_fNextFetchTime = 0.0f;
    
    SetDebugRender(false, 0);
    m_spDebugRoot = 0;

    if (pkWorkflowManager)
    {
        m_pkWorkflowManager = pkWorkflowManager;
        SetBit(false, OWN_MANAGER_MASK);
    }
    else
    {
        m_pkWorkflowManager =
            NiNew NiSPWorkflowManager(64, NiStreamProcessor::MEDIUM, 3);
        SetBit(true, OWN_MANAGER_MASK);
    }

    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
NiPhysXScene::~NiPhysXScene()
{
    // Remove any props
    NiUInt32 uiSize = m_kProps.GetSize();
    for (NiUInt32 ui = uiSize; ui > 0; ui--)
        RemovePropAt(ui - 1);

    if (m_pkPhysXScene)
        ReleaseScene();

    // The compartment pointers will all be bad now.
    m_kCompartmentMap.RemoveAll();
    
    // Delete all the sources, destinations and modified meshes
    m_kSources.RemoveAll();
    m_kDestinations.RemoveAll();
    m_kModifiedMeshes.RemoveAll();
        
    // Release the smart pointer to the scene root and the debug rendering
    m_spDebugRoot = 0;
    
    // Delete the snapshot
    m_spSnapshot = 0;

    if (GetBit(OWN_MANAGER_MASK))
    {
        NiDelete m_pkWorkflowManager;
    }
    m_pkWorkflowManager = 0;
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetPhysXScene(NxScene* pkScene)
{
    NIASSERT(!m_pkPhysXScene);

    m_pkPhysXScene = pkScene;
    
    NotifyAllSceneChanged();
}
//---------------------------------------------------------------------------
void NiPhysXScene::ReleaseScene()
{
    FetchResults(NI_INFINITY, true);
    
    // Catch the case when the SDK has already been shut down.
    if (m_pkManager->m_pkPhysXSDK)
    {
        m_pkManager->WaitSDKLock();

        // Release all content in Props to avoid errors with hanging pointers.
        NiUInt32 uiNumProps = m_kProps.GetSize();
        for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
        {
            NiPhysXPropDesc* pkSnapshot = m_kProps.GetAt(ui)->GetSnapshot();
            if (pkSnapshot)
                pkSnapshot->RemoveFromScene(m_pkPhysXScene);
        }
        
        // Release the scene.
        m_pkManager->m_pkPhysXSDK->releaseScene(*m_pkPhysXScene);

        m_pkManager->ReleaseSDKLock();
    }

    m_pkPhysXScene = 0;
    
    m_fPrevFetchTime = 0.0f;
    m_fNextFetchTime = 0.0f;
    
    m_kCompartmentMap.RemoveAll();
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXScene::AddProp(NiPhysXProp* pkProp, NiUInt32 uiState)
{
    // Add to array
    NiUInt32 uiIndex = m_kProps.AddFirstEmpty(pkProp);
    
    // Add sources, destinations and modified meshes
    NiUInt32 uiNumDests = pkProp->GetDestinationsCount();
    for (NiUInt32 ui = 0; ui < uiNumDests; ui++)
    {
        AddDestination(pkProp->GetDestinationAt(ui));
    }
    
    NiUInt32 uiNumSrcs = pkProp->GetSourcesCount();
    for (NiUInt32 ui = 0; ui < uiNumSrcs; ui++)
    {
        AddSource(pkProp->GetSourceAt(ui));
    }

    NiUInt32 uiNumMeshes = pkProp->GetModifiedMeshCount();
    for (NiUInt32 ui = 0; ui < uiNumMeshes; ui++)
    {
        AddModifiedMesh(pkProp->GetModifiedMeshAt(ui));
    }

    // Create if a scene exists
    if (m_pkPhysXScene)
    {
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NxCompartment* pkRBCompartment = 0;
            NiUInt32 uiID = pkSnapshot->GetRBCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkRBCompartment);
                NIASSERT(pkRBCompartment);
            }
            NxCompartment* pkClothCompartment = 0;
            uiID = pkSnapshot->GetClothCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkClothCompartment);
                NIASSERT(pkClothCompartment);
            }
            NxCompartment* pkFluidCompartment = 0;
            uiID = pkSnapshot->GetFluidCompartmentID();
            if (uiID > 0)
            {
                m_kCompartmentMap.GetAt(uiID, pkFluidCompartment);
                NIASSERT(pkFluidCompartment);
            }
            m_pkManager->WaitSDKLock();
            bool bResult = pkSnapshot->CreateInScene(m_pkPhysXScene,
                pkProp->GetXform(), pkRBCompartment, pkClothCompartment,
                pkFluidCompartment, pkProp->GetKeepMeshes(), uiState);
            m_pkManager->ReleaseSDKLock();
            if (!bResult)
            {
                RemovePropAt(uiIndex);
                return (NiUInt32)-1;
            }
        }
    }
    
    pkProp->AttachSceneCallback(this);

    return uiIndex;
}
//---------------------------------------------------------------------------
void NiPhysXScene::RemoveProp(NiPhysXProp* pkTarget)
{
    NiUInt32 uiSize = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        if (m_kProps.GetAt(ui) == pkTarget)
        {
            RemovePropAt(ui);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::RemovePropAt(const NiUInt32 uiIndex)
{
    NiPhysXProp* pkProp = m_kProps.GetAt(uiIndex);
    NIASSERT(pkProp != 0);
    
    // Remove sources and destinations
    NiUInt32 uiNumDests = pkProp->GetDestinationsCount();
    for (NiUInt32 ui = 0; ui < uiNumDests; ui++)
    {
        DeleteDestination(pkProp->GetDestinationAt(ui));
    }
    
    NiUInt32 uiNumSrcs = pkProp->GetSourcesCount();
    for (NiUInt32 ui = 0; ui < uiNumSrcs; ui++)
    {
        DeleteSource(pkProp->GetSourceAt(ui));
    }

    NiUInt32 uiNumMeshes = pkProp->GetModifiedMeshCount();
    for (NiUInt32 ui = 0; ui < uiNumMeshes; ui++)
    {
        DeleteModifiedMesh(pkProp->GetModifiedMeshAt(ui));
    }

    // Remove from scene, if there is one
    if (m_pkPhysXScene)
    {
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            m_pkManager->WaitSDKLock();
            pkSnapshot->RemoveFromScene(m_pkPhysXScene);
            m_pkManager->ReleaseSDKLock();
        }
    }

    pkProp->DetachSceneCallback(this);

    m_kProps.RemoveAtAndFill(uiIndex);    
}
//---------------------------------------------------------------------------
bool NiPhysXScene::AddCompartment(NiUInt32 uiID,
    NxCompartmentDesc& pkDescriptor)
{
    NIASSERT(pkDescriptor.isValid());
    NIASSERT(m_pkPhysXScene);
    
    NxCompartment* pkCompartment =
        m_pkPhysXScene->createCompartment(pkDescriptor);
    NIASSERT(pkCompartment);
    
    return AddCompartment(uiID, pkCompartment);
}
//---------------------------------------------------------------------------
bool NiPhysXScene::Simulate(const float fTargetTime, const bool bForceSim)
{
    // Return value is whether or not we are now in a sim/fetch pair
    
    // First test if we should and need the update
    if (!m_pkPhysXScene || (m_fPrevFetchTime >= fTargetTime && !bForceSim))
        return false;
        
    // If we are simulating already, don't go any further
    if (m_bInSimFetch)
        return true;

    // Figure out our target time step. This code takes some convulated
    // steps to be sure that PhysX steps as far as we want and no further.
    // It is necessary because of the way that time is internally measured
    // by PhysX, but not reported back to the application. It will probably
    // have to change if the internal time management done by PhysX is
    // changed.
    float fPhysXStep = 0.0f;
    if (bForceSim && m_fPrevFetchTime > fTargetTime)
    {
        // This will force a zero time step
        m_fNextFetchTime = m_fPrevFetchTime;
    }
    else if (GetBit(FIXED_TIMESTEP_MASK))
    {
        // We aim to keep the time close to an integer multiple of the step.
        // Compute the requested delta, rounding down a little to ensure that
        // we get the right number of steps if it's an exact multiple of
        // the step length. 
        float fDeltaT = fTargetTime - m_fPrevFetchTime - 1.0e-06f;
        
        // Get the number of steps. Use ceiling because we always want to
        // go past the requested time.
        NiUInt32 uiNumGBSteps = (NiUInt32)ceil(fDeltaT / m_fTimestep);
        
        // m_fNextFetchTime might drift away from a multiple of
        // m_fTimestep, but that's OK with us.
        m_fNextFetchTime = m_fPrevFetchTime + uiNumGBSteps * m_fTimestep;
        
        // Set up PhysX. We deliberately ask for a much larger time step
        // so that we force it to take the requested number of iterations.
        // In practice, we are using uiNumNxSteps to control how far
        // PhysX steps.
        NiUInt32 uiNumNxSteps = uiNumGBSteps * m_uiNumSubSteps;
        if (uiNumNxSteps > m_uiMaxSubSteps)
        {
            uiNumNxSteps = m_uiMaxSubSteps;
            NiOutputDebugString("NiPhysXScene: PhysX timestep truncated\n");
        }
        float fStepLength = m_fTimestep / (float)m_uiNumSubSteps;
        m_pkPhysXScene->setTiming(
            fStepLength, uiNumNxSteps, NX_TIMESTEP_FIXED);
        
        // Sure to be bigger than needed
        fPhysXStep = fStepLength * uiNumNxSteps + 1.0f;
    }
    else
    {
        // Variable step length.
        // The first two args to setTiming don't matter.
        m_pkPhysXScene->setTiming(m_fTimestep, 1, NX_TIMESTEP_VARIABLE);
        m_fNextFetchTime = fTargetTime;
        fPhysXStep = m_fNextFetchTime - m_fPrevFetchTime;
    }

    // Lock the SDK. If someone else is working on something preventing
    // simulation from starting, then they must be holding this lock
    m_pkManager->WaitSDKLock();
    
    // Push all the sources to the simulation actors.
    if (GetUpdateSrc())
        UpdateSourcesToActors(m_fPrevFetchTime, m_fNextFetchTime);
        
    // Complete/Submit on all mesh modifiers
    DoMeshModifiers(NiSyncArgs::SYNC_PHYSICS_SIMULATE, bForceSim);

    // Do the simulation
    m_pkPhysXScene->simulate(fPhysXStep);
    m_pkPhysXScene->flushStream();
    
    m_bInSimFetch = true;

    m_pkManager->ReleaseSDKLock();
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXScene::FetchResults(const float fTargetTime, const bool bBlock,
    NxU32* puiErrorState)
{
    // Return value is whether or not simulation was done.
    
    if (!m_bInSimFetch)
        return true;

    m_pkManager->WaitSDKLock();
    if (bBlock || m_fNextFetchTime <= fTargetTime)
    {
        // We need the results, now!
        m_pkPhysXScene->fetchResults(NX_RIGID_BODY_FINISHED, true,
            puiErrorState);
    }
    else if (!m_pkPhysXScene->fetchResults(NX_RIGID_BODY_FINISHED, false,
        puiErrorState))
    {
        // Results not ready yet
        m_pkManager->ReleaseSDKLock();
        return false;
    }

    m_bInSimFetch = false;

    const NxDebugRenderable* pkRenderState = 0;
    if (GetDebugRender())
        pkRenderState = m_pkPhysXScene->getDebugRenderable();

    // Complete/Submit on all mesh modifiers
    DoMeshModifiers(NiSyncArgs::SYNC_PHYSICS_COMPLETED);

    // Push all the destinations to the simulation actors.
    if (GetUpdateDest())
        UpdateActorsToDests(false);

    m_pkManager->ReleaseSDKLock();

    // Wait till now because we don't need the SDK lock to do this.
    if (pkRenderState)
        SetDebugGeometry(pkRenderState);
    
    m_fPrevFetchTime = m_fNextFetchTime;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSources(const float fTime, const bool bForce)
{
    if (!GetUpdateSrc())
        return;

    // This needs to extract data from the Gamebryo nodes, BUT NOT push it
    // to PhysX. That gets done immediately before simulation. It's more
    // efficient because we don't lock the SDK and we only update PhysX once
    // for each sim step, even if the application is calling this function
    // at a higher frequency.

    NiTransform kInvRootXform;
    GetSceneXform().Invert(kInvRootXform);
        
    for (NiUInt32 i = 0; i < m_kSources.GetSize(); i++)
    {
        m_kSources.GetAt(i)->UpdateFromSceneGraph(fTime, kInvRootXform,
            1.0f / m_fScalePToW, bForce);   
    }

    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        UpdateSourcesToActors(fTime, fTime);
        m_pkManager->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSource(NiPhysXSrc* pkSrc, const float fTime,
    const bool bForce)
{
    NIASSERT(pkSrc != 0);

    NiTransform kInvRootXform;
    GetSceneXform().Invert(kInvRootXform);

    pkSrc->UpdateFromSceneGraph(fTime, kInvRootXform, 1.0f / m_fScalePToW,
        bForce);

    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        pkSrc->UpdateToActors(fTime, fTime);
        m_pkManager->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateSourcesToActors(const float fTBegin,
    const float fTEnd)
{
    // Push all the source state out to actors in the scene. We
    // assume that we hold the SDK lock at this point.
    for (NiUInt32 i = 0; i < m_kSources.GetSize(); i++)
        m_kSources.GetAt(i)->UpdateToActors(fTBegin, fTEnd);   
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateActorsToDests(const bool bForce)
{
    // Get state from actors. Assume we have the SDK lock to do this.
    for (NiUInt32 i = 0; i < m_kDestinations.GetSize(); i++)
    {
        m_kDestinations.GetAt(i)->UpdateFromActors(m_fNextFetchTime,
            m_kSceneXform, m_fScalePToW, bForce);
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateDestinations(const float fTime, const bool bForce)
{
    if (!GetUpdateDest())
        return;
        
    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        UpdateActorsToDests(bForce);
        m_pkManager->ReleaseSDKLock();
    }

    for (NiUInt32 i = 0; i < m_kDestinations.GetSize(); i++)
    {
        m_kDestinations.GetAt(i)->
            UpdateSceneGraph(fTime, m_kSceneXform, m_fScalePToW, bForce);
    }  
}    
//---------------------------------------------------------------------------
void NiPhysXScene::UpdateDestination(NiPhysXDest* pkDest, const float fTime,
    const bool bForce)
{
    NIASSERT(pkDest != 0);

    if (bForce)
    {
        m_pkManager->WaitSDKLock();
        pkDest->UpdateFromActors(m_fPrevFetchTime, m_kSceneXform, m_fScalePToW,
            bForce);
        m_pkManager->ReleaseSDKLock();
    }
    
    pkDest->UpdateSceneGraph(fTime, m_kSceneXform, m_fScalePToW, bForce);   
}    
//---------------------------------------------------------------------------
void NiPhysXScene::DoMeshModifiers(NiSyncArgs::SyncPoint kSyncPoint,
    bool bForced)
{
    // Complete any tasks that need to be done now.
    NiPhysXSyncArgs kSyncArgs;
    kSyncArgs.m_pkPhysXScene = this;
    kSyncArgs.m_bForce = bForced;

    NiUInt32 uiNumModifiedMeshes = m_kModifiedMeshes.GetSize();

    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = kSyncPoint;
    for (NiUInt32 ui = 0; ui < uiNumModifiedMeshes; ui++)
    {
        NiMesh* pkMesh = m_kModifiedMeshes.GetAt(ui);
        if (!pkMesh)
            continue;
            
        pkMesh->CompleteModifiers(&kSyncArgs);
    }

    kSyncArgs.m_uiSubmitPoint = kSyncPoint;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    for (NiUInt32 ui = 0; ui < uiNumModifiedMeshes; ui++)
    {
        NiMesh* pkMesh = m_kModifiedMeshes.GetAt(ui);
        if (!pkMesh)
            continue;
            
        pkMesh->SubmitModifiers(&kSyncArgs, m_pkWorkflowManager);
    }

    m_pkWorkflowManager->FlushTaskGroup(NiSyncArgs::SYNC_ANY,
        NiSyncArgs::SYNC_ANY);
}
//---------------------------------------------------------------------------
void NiPhysXScene::NotifyAllSceneChanged()
{
    NiUInt32 uiNumModifiedMeshes = m_kModifiedMeshes.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumModifiedMeshes; ui++)
    {
        NiMesh* pkMesh = m_kModifiedMeshes.GetAt(ui);
        if (!pkMesh)
            continue;
            
        NotifySceneChanged(pkMesh);
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::NotifySceneChanged(NiMesh* pkMesh, bool bRemoving)
{
    NiUInt32 uiNumModifiers = pkMesh->GetModifierCount();
    for (NiUInt32 ui = 0; ui < uiNumModifiers; ui++)
    {
        NiMeshModifier* pkModifier = pkMesh->GetModifierAt(ui);
        
        NiPhysXMeshModifier* pkPhysXModifier =
            NiDynamicCast(NiPhysXMeshModifier, pkModifier);
        if (pkPhysXModifier)
        {   
            pkPhysXModifier->SceneChanged(pkMesh, bRemoving ? NULL : this);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Debug Rendering
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugRender(const bool bActive, NiNode* pkDebugParent)
{
    SetBit(bActive, DEBUG_RENDER_MASK);
    
    if (bActive && !m_spDebugRoot)
    {
        NIASSERT(pkDebugParent && "NiPhysXScene::SetDebugRender");
        m_spDebugRoot = NiNew NiNode();
        pkDebugParent->AttachChild(m_spDebugRoot);
    }
    else if (!bActive && m_spDebugRoot)
    {
        if (m_spDebugRoot->GetParent())
            m_spDebugRoot->GetParent()->DetachChild(m_spDebugRoot);
        m_spDebugRoot = 0;
        m_spTriangleDebugGeometries = 0;
        m_spLineDebugGeometries = 0;
        m_spPointDebugGeometries = 0;
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::ClearDebugGeometry()
{
    if (m_spDebugRoot)
    {
        NiAVObjectPtr spDebugGeometry;
        for (NiUInt32 us = m_spDebugRoot->GetArrayCount(); us > 0; us--)
        {
            spDebugGeometry = m_spDebugRoot->DetachChildAt(us - 1);
            spDebugGeometry = 0;
        }
    } // if (m_spDebugRoot)
    
    m_spTriangleDebugGeometries = 0;
    m_spLineDebugGeometries = 0;
    m_spPointDebugGeometries = 0;
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugGeometry(const NxDebugRenderable* pkRenderState)
{
    if (!m_pkPhysXScene)
        return;
        
    NIASSERT(m_spDebugRoot != 0);

    // Set the transform on the debug node to make sure coordinate systems
    // match up.
    NiTransform kParentInverse;
    m_spDebugRoot->GetParent()->GetWorldTransform().Invert(kParentInverse);
    NiTransform kDebugLocal = kParentInverse * GetSceneXform();
    m_spDebugRoot->SetTranslate(kDebugLocal.m_Translate);
    m_spDebugRoot->SetRotate(kDebugLocal.m_Rotate);
    m_spDebugRoot->SetScale(kDebugLocal.m_fScale);

    SetDebugTriangleGeometry(pkRenderState);
    SetDebugLineGeometry(pkRenderState);
    SetDebugPointGeometry(pkRenderState);

    m_spDebugRoot->Update(m_fPrevFetchTime);
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugTriangleGeometry(
    const NxDebugRenderable* pkRenderState)
{
    // Retrieve the triangle count and triangle list from PhysX
    NxU32 uiNTriangles = pkRenderState->getNbTriangles();

    if (!uiNTriangles)
        return;

    const NxDebugTriangle* pkNxTriangles = pkRenderState->getTriangles();

    NiUInt32 uiElementCount = uiNTriangles * 3;

#if defined(_WII)
    // The Wii doesn't support float colors.
    NiDataStreamElement::Format eColorFormat = NiDataStreamElement::F_NORMUINT8_4;
#else
    NiDataStreamElement::Format eColorFormat = NiDataStreamElement::F_FLOAT32_4;
#endif

    // If a mesh allready exists, check if there is enough room in the stream 
    // to fill it with the current lines. If not, resize the stream.
    if (m_spTriangleDebugGeometries)
    {
        ResizeDataStreamIfNecessary(m_spTriangleDebugGeometries, 
            uiElementCount);
    }
    else
    {
        NiDataStreamElementSet kElements;
        kElements.AddElement(NiDataStreamElement::F_FLOAT32_3);
        kElements.AddElement(NiDataStreamElement::F_FLOAT32_3);
        kElements.AddElement(eColorFormat);

        NiTObjectArray<NiFixedString> kSemantics;
        kSemantics.SetAtGrow(0, NiCommonSemantics::POSITION());
        kSemantics.SetAtGrow(1, NiCommonSemantics::NORMAL());
        kSemantics.SetAtGrow(2, NiCommonSemantics::COLOR());

        m_spTriangleDebugGeometries = CreateMesh(
            NiPrimitiveType::PRIMITIVE_TRIANGLES,
            uiElementCount, kElements, kSemantics);
    }

    // Retrieve locks to the position, normal and color streams.
    NiDataStreamElementLock kVertexLock = NiDataStreamElementLock(
        m_spTriangleDebugGeometries, NiCommonSemantics::POSITION(), 0, 
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    NiDataStreamElementLock kNormalLock = NiDataStreamElementLock(
        m_spTriangleDebugGeometries, NiCommonSemantics::NORMAL(), 0, 
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    NiDataStreamElementLock kColorLock = NiDataStreamElementLock(
        m_spTriangleDebugGeometries, NiCommonSemantics::COLOR(), 0, 
        eColorFormat, NiDataStream::LOCK_WRITE);

    // Retrieve an iterator to each one of the streams
    NiTStridedRandomAccessIterator<NiPoint3> kVertexIterator = 
        kVertexLock.begin<NiPoint3>();
    NiTStridedRandomAccessIterator<NiPoint3> kNormalIterator = 
        kNormalLock.begin<NiPoint3>();
#if defined(_WII)
    NiTStridedRandomAccessIterator<NiRGBA> kColorIterator = 
        kColorLock.begin<NiRGBA>();
#else
    NiTStridedRandomAccessIterator<NiColorA> kColorIterator = 
        kColorLock.begin<NiColorA>();
#endif

    // Iterate over the PhysX debug triangles and fill the NiMesh streams with
    // data
    for (NiUInt32 uiIndex = 0; uiIndex < uiNTriangles; uiIndex++)
    {
        NiUInt32 ui3i = 3 * uiIndex;
        NiPhysXTypes::NxVec3ToNiPoint3(pkNxTriangles[uiIndex].p0, 
            kVertexIterator[ui3i]);
        kVertexIterator[ui3i] *= m_fScalePToW;
        NiPhysXTypes::NxVec3ToNiPoint3(pkNxTriangles[uiIndex].p1, 
            kVertexIterator[ui3i+1]);
        kVertexIterator[ui3i+1] *= m_fScalePToW;
        NiPhysXTypes::NxVec3ToNiPoint3(pkNxTriangles[uiIndex].p2, 
            kVertexIterator[ui3i+2]);
        kVertexIterator[ui3i+2] *= m_fScalePToW;


        NiPoint3 kE1 = kVertexIterator[ui3i+1] - kVertexIterator[ui3i];
        NiPoint3 kE2 = kVertexIterator[ui3i+2] - kVertexIterator[ui3i];
        NiPoint3 kNorm = kE1.Cross(kE2);
        kNorm.Unitize();

        kNormalIterator[ui3i] = kNorm;
        kNormalIterator[ui3i+1] = kNorm;
        kNormalIterator[ui3i+2] = kNorm;

#if defined(_WII)
        NiPhysXTypes::NxColorToNiRGBA(
            pkNxTriangles[uiIndex].color, kColorIterator[ui3i]);
#else
        NiPhysXTypes::NxColorToNiColorA(
            pkNxTriangles[uiIndex].color, kColorIterator[ui3i]);
#endif
        kColorIterator[ui3i+1] = kColorIterator[ui3i];
        kColorIterator[ui3i+2] = kColorIterator[ui3i];
    }

    // Unlock all the datastreams
    kVertexLock.Unlock();
    kNormalLock.Unlock();
    kColorLock.Unlock();

    // Recompute the bounds
    m_spTriangleDebugGeometries->RecomputeBounds();

    // Update the mesh
    m_spTriangleDebugGeometries->UpdateProperties();
    m_spTriangleDebugGeometries->Update(0.0f);
    m_spTriangleDebugGeometries->UpdateEffects();
}
//-----------------------------------------------------------------------------
void NiPhysXScene::SetDebugLineGeometry(const NxDebugRenderable* pkRenderState)
{
    NxU32 uiNLines = pkRenderState->getNbLines();

    if (!uiNLines)
        return;

    const NxDebugLine* pkNxLines = pkRenderState->getLines();

    NiUInt32 uiElementCount = uiNLines * 2;

#if defined(_WII)
    // The Wii doesn't support float colors.
    NiDataStreamElement::Format eColorFormat = NiDataStreamElement::F_NORMUINT8_4;
#else
    NiDataStreamElement::Format eColorFormat = NiDataStreamElement::F_FLOAT32_4;
#endif

    // If a mesh allready exists, check if there is enough room in the stream 
    // to fill it with the current lines. If not, resize the stream.
    if (m_spLineDebugGeometries)
    {
        ResizeDataStreamIfNecessary(m_spLineDebugGeometries, uiElementCount);
    }
    else
    {
        // Creates an interleaved datastream filled with positions and colors.
        NiDataStreamElementSet kElements;
        kElements.AddElement(NiDataStreamElement::F_FLOAT32_3);
        kElements.AddElement(eColorFormat);

        NiTObjectArray<NiFixedString> kSemmantics;
        kSemmantics.SetAtGrow(0, NiCommonSemantics::POSITION());
        kSemmantics.SetAtGrow(1, NiCommonSemantics::COLOR());

        m_spLineDebugGeometries = CreateMesh(
            NiPrimitiveType::PRIMITIVE_LINES,
            uiElementCount, kElements, kSemmantics);
    }

    // Retrieve locks to the position and color streams
    NiDataStreamElementLock kVertexLock = NiDataStreamElementLock(
        m_spLineDebugGeometries, NiCommonSemantics::POSITION(), 0, 
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    NiDataStreamElementLock kColorLock = NiDataStreamElementLock(
        m_spLineDebugGeometries, NiCommonSemantics::COLOR(), 0, 
        eColorFormat, NiDataStream::LOCK_WRITE);

    // Retrieve an iterator to each one of the streams
    NiTStridedRandomAccessIterator<NiPoint3> kVertexIterator = 
        kVertexLock.begin<NiPoint3>();
#if defined(_WII)
    NiTStridedRandomAccessIterator<NiRGBA> kColorIterator = 
        kColorLock.begin<NiRGBA>();
#else
    NiTStridedRandomAccessIterator<NiColorA> kColorIterator = 
        kColorLock.begin<NiColorA>();
#endif

    // Iterate over the PhysX debug lines and fill the NiMesh streams with
    // data
    for (NiUInt32 uiIndex = 0; uiIndex < uiNLines; uiIndex++)
    {
        NiUInt32 ui2i = 2 * uiIndex;
        NiPhysXTypes::NxVec3ToNiPoint3(pkNxLines[uiIndex].p0,
            kVertexIterator[ui2i]);
        kVertexIterator[ui2i] *= m_fScalePToW;
        NiPhysXTypes::NxVec3ToNiPoint3(pkNxLines[uiIndex].p1,
            kVertexIterator[ui2i+1]);
        kVertexIterator[ui2i+1] *= m_fScalePToW;

#if defined(_WII)
        NiPhysXTypes::NxColorToNiRGBA(pkNxLines[uiIndex].color, 
            kColorIterator[ui2i]);
#else
        NiPhysXTypes::NxColorToNiColorA(pkNxLines[uiIndex].color,
            kColorIterator[ui2i]);
#endif
        kColorIterator[ui2i+1] = kColorIterator[ui2i];
    }

    // Unlock all the datastreams
    kVertexLock.Unlock();
    kColorLock.Unlock();

    // Recompute the bounds
    m_spLineDebugGeometries->RecomputeBounds();

    // Update the mesh
    m_spLineDebugGeometries->UpdateProperties();
    m_spLineDebugGeometries->Update(0.0f);
    m_spLineDebugGeometries->UpdateEffects();
}
//---------------------------------------------------------------------------
void NiPhysXScene::SetDebugPointGeometry(
    const NxDebugRenderable* pkRenderState)
{
    NxU32 uiNPoints = pkRenderState->getNbPoints();

    if (!uiNPoints)
        return;

    const NxDebugPoint* pkNxPoints = pkRenderState->getPoints();

    NiUInt32 uiElementCount = uiNPoints;

#if defined(_WII)
    // The Wii doesn't support float colors.
    NiDataStreamElement::Format eColorFormat = NiDataStreamElement::F_NORMUINT8_4;
#else
    NiDataStreamElement::Format eColorFormat = NiDataStreamElement::F_FLOAT32_4;
#endif

    // If a mesh allready exists, check if there is enough room in the stream 
    // to fill it with the current lines. If not, resize the stream.
    if (m_spPointDebugGeometries)
    {
        ResizeDataStreamIfNecessary(m_spPointDebugGeometries, uiElementCount);
    }
    else
    {
        NiDataStreamElementSet kElements;
        kElements.AddElement(NiDataStreamElement::F_FLOAT32_3);
        kElements.AddElement(eColorFormat);

        NiTObjectArray<NiFixedString> kSemmantics;
        kSemmantics.SetAtGrow(0, NiCommonSemantics::POSITION());
        kSemmantics.SetAtGrow(1, NiCommonSemantics::COLOR());

        m_spPointDebugGeometries = CreateMesh(
            NiPrimitiveType::PRIMITIVE_POINTS,
            uiElementCount, kElements, kSemmantics);
    }

    // Retrieve locks to the position and color streams
    NiDataStreamElementLock kVertexLock = NiDataStreamElementLock(
        m_spPointDebugGeometries, NiCommonSemantics::POSITION(), 0, 
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_WRITE);

    NiDataStreamElementLock kColorLock = NiDataStreamElementLock(
        m_spPointDebugGeometries, NiCommonSemantics::COLOR(), 0, 
        eColorFormat, NiDataStream::LOCK_WRITE);

    // Retrieve an iterator to each one of the streams
    NiTStridedRandomAccessIterator<NiPoint3> kVertexIterator = 
        kVertexLock.begin<NiPoint3>();
#if defined(_WII)
    NiTStridedRandomAccessIterator<NiRGBA> kColorIterator = 
        kColorLock.begin<NiRGBA>();
#else
    NiTStridedRandomAccessIterator<NiColorA> kColorIterator = 
        kColorLock.begin<NiColorA>();
#endif

    // Iterate over the PhysX debug triangles and fill the NiMesh streams with
    // data
    for (NiUInt32 uiIndex = 0; uiIndex < uiNPoints; uiIndex++)
    {
        NiPhysXTypes::NxVec3ToNiPoint3(
            pkNxPoints[uiIndex].p, kVertexIterator[uiIndex]);
        kVertexIterator[uiIndex] *= m_fScalePToW;

#if defined(_WII)
        NiPhysXTypes::NxColorToNiRGBA(
            pkNxPoints[uiIndex].color, kColorIterator[uiIndex]);
#else
        NiPhysXTypes::NxColorToNiColorA(
            pkNxPoints[uiIndex].color, kColorIterator[uiIndex]);
#endif
    }

    // Unlock all the datastreams
    kVertexLock.Unlock();
    kColorLock.Unlock();

    // Recompute the bounds
    m_spPointDebugGeometries->RecomputeBounds();

    // Update the mesh
    m_spPointDebugGeometries->UpdateProperties();
    m_spPointDebugGeometries->Update(0.0f);
    m_spPointDebugGeometries->UpdateEffects();
}
//---------------------------------------------------------------------------
void NiPhysXScene::ResizeDataStreamIfNecessary(NiMesh* pkMesh, 
    NiUInt32 uiElementCount)
{
    NiDataStreamRef* pkInterleavedDataStreamRef = pkMesh->GetStreamRefAt(0);
    NiDataStream* pkInterleavedDataStream = 
        pkInterleavedDataStreamRef->GetDataStream();

    NiDataStream::Region& kRegion = pkInterleavedDataStream->GetRegion(0);

    NiUInt32 uiTotalElementCount = pkInterleavedDataStream->GetTotalCount();

    // If the element count exceeds the total element count in the data stream,
    // resize the datastream.
    if (uiElementCount > uiTotalElementCount)
    {
        NiUInt32 uiStride = pkInterleavedDataStream->GetStride();
        NiUInt32 uiNewSize = uiStride * uiElementCount;

        pkInterleavedDataStream->Resize(uiNewSize);
    }

    kRegion.SetRange(uiElementCount);
}
//---------------------------------------------------------------------------
NiMesh* NiPhysXScene::CreateMesh(NiPrimitiveType::Type ePrimitiveType, 
    NiUInt32 uiElementCount, const NiDataStreamElementSet& kElements, 
    const NiTObjectArray<NiFixedString>& kSemmantics)
{
    // Create the debug mesh and attach it to the m_spDebugRoot node
    NiMesh* pkMesh = NiNew NiMesh();
    pkMesh->SetPrimitiveType(ePrimitiveType);
    pkMesh->SetSubmeshCount(1);
    m_spDebugRoot->AttachChild(pkMesh, true);

    // Create properties and attach them to the mesh
    NiMaterialProperty* pkMaterial = NiNew NiMaterialProperty();

    NiVertexColorProperty* pkVertColorProp = NiNew NiVertexColorProperty();
    pkVertColorProp->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
    pkVertColorProp->SetLightingMode(NiVertexColorProperty::LIGHTING_E);

    pkMesh->AttachProperty(pkMaterial);
    pkMesh->AttachProperty(pkVertColorProp);

    // Creates an interleaved datastream.
    NiDataStream* pkInterleavedDataStream = NiDataStream::CreateDataStream(
        kElements, uiElementCount, NiDataStream::ACCESS_CPU_WRITE_MUTABLE | 
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_GPU_READ, 
        NiDataStream::USAGE_VERTEX);
    NIASSERT(pkInterleavedDataStream);

    NiDataStream::Region kRegion(0, uiElementCount);
    pkInterleavedDataStream->AddRegion(kRegion);

    NiDataStreamRef* pkInterleavedDataStreamRef = pkMesh->AddStreamRef();
    NIASSERT(pkInterleavedDataStreamRef);

    pkInterleavedDataStreamRef->SetDataStream(pkInterleavedDataStream);

    for (NiUInt32 uiIndex = 0; uiIndex < kSemmantics.GetSize(); uiIndex++)
    {
        pkInterleavedDataStreamRef->BindSemanticToElementDescAt(uiIndex, 
            kSemmantics[uiIndex], 0);
    }

    pkInterleavedDataStreamRef->BindRegionToSubmesh(0, 0);

    return pkMesh;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Snapshot functions
//---------------------------------------------------------------------------
bool NiPhysXScene::CreateSceneFromSnapshot(const NiUInt32 uiIndex)
{
    NIASSERT(m_spSnapshot != 0);
  
    if (m_pkPhysXScene)
        ReleaseScene();
    
    m_pkManager->WaitSDKLock();
    NxScene* pkScene = m_spSnapshot->CreateScene();
    if (!pkScene)
        return false;
    
    if (!CreateSceneCompartments(pkScene))
        return false;

    // Create any props.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NIASSERT(pkProp);
        if (!CreateSceneFromProp(pkProp, pkScene, uiIndex))
            return false;
    }

    m_pkManager->ReleaseSDKLock();
    
    SetPhysXScene(pkScene);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXScene::CreateSceneFromSnapshot(const NiFixedString& kName)
{
    NIASSERT(m_spSnapshot != 0);
  
    if (m_pkPhysXScene)
        ReleaseScene();
    
    m_pkManager->WaitSDKLock();
    NxScene* pkScene = m_spSnapshot->CreateScene();
    if (!pkScene)
        return false;
    
    if (!CreateSceneCompartments(pkScene))
        return false;

    // Create any props.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NIASSERT(pkProp);
        
        NiUInt32 uiIndex = pkProp->GetSnapshot()->GetStateIndex(kName);
        
        if (!CreateSceneFromProp(pkProp, pkScene, uiIndex))
            return false;
    }

    m_pkManager->ReleaseSDKLock();
    
    SetPhysXScene(pkScene);
    
    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiPhysXScene::AddSnapshotState(NiFixedString& kName)
{
    NiUInt32 uiStateNum = 0;
    
    m_pkManager->WaitSDKLock();

    // Work through all props, setting snapshot state.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NxMat34 kInvXform;
            pkProp->GetXform().getInverse(kInvXform);
            NiUInt32 uiThisStateNum;
            uiThisStateNum =
                pkSnapshot->AddState(m_pkPhysXScene, kInvXform, kName);
            if (ui == 0)
            {
                uiStateNum = uiThisStateNum;
            }
            NIASSERT(uiStateNum == uiThisStateNum);
        }
    }

    m_pkManager->ReleaseSDKLock();
    
    return uiStateNum;
}
//---------------------------------------------------------------------------
void NiPhysXScene::RestoreSnapshotState(const NiUInt32 uiIndex)
{
    m_pkManager->WaitSDKLock();

    // Work through all props, restoring snapshot state.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            pkSnapshot->RestoreState(m_pkPhysXScene, uiIndex,
                pkProp->GetXform(), pkProp->GetKeepMeshes());
        }
    }

    m_pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXScene::RestoreSnapshotState(const NiFixedString& kName)
{
    m_pkManager->WaitSDKLock();

    // Work through all props, restoring snapshot state.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NiUInt32 uiIndex = pkSnapshot->GetStateIndex(kName);
            pkSnapshot->RestoreState(m_pkPhysXScene, uiIndex,
                pkProp->GetXform(), pkProp->GetKeepMeshes());
        }
    }

    m_pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXScene::RemoveSnapshotState(NiFixedString& kName)
{
    m_pkManager->WaitSDKLock();

    // Work through all props, removing snapshot state.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            NiUInt32 uiIndex = pkSnapshot->GetStateIndex(kName);
            pkSnapshot->RemoveState(uiIndex);
        }
    }

    m_pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXScene::RemoveSnapshotState(NiUInt32 uiIndex)
{
    m_pkManager->WaitSDKLock();

    // Work through all props, removing snapshot state.
    NiUInt32 uiNumProps = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiNumProps; ui++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
        if (pkSnapshot)
        {
            pkSnapshot->RemoveState(uiIndex);
        }
    }

    m_pkManager->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
bool NiPhysXScene::CreateSceneCompartments(NxScene* pkScene)
{
    // Create any compartments
    NiTMap<NiUInt32, NxCompartmentDesc*>& kCompartmentMap =
        m_spSnapshot->GetCompartmentMap();
    NiTMapIterator kIter = kCompartmentMap.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiID;
        NxCompartmentDesc* pkDesc;
        kCompartmentMap.GetNext(kIter, uiID, pkDesc);
        
        NxCompartment* pkCompartment = pkScene->createCompartment(*pkDesc);
        NIASSERT(pkCompartment);
        m_kCompartmentMap.SetAt(uiID, pkCompartment);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXScene::CreateSceneFromProp(NiPhysXProp* pkProp, NxScene* pkScene,
    const NiUInt32 uiIndex)
{
    NiPhysXPropDesc* pkSnapshot = pkProp->GetSnapshot();
    if (pkSnapshot)
    {
        NxCompartment* pkRBCompartment = 0;
        NiUInt32 uiID = pkSnapshot->GetRBCompartmentID();
        if (uiID > 0)
        {
            m_kCompartmentMap.GetAt(uiID, pkRBCompartment);
            NIASSERT(pkRBCompartment);
        }
        NxCompartment* pkClothCompartment = 0;
        uiID = pkSnapshot->GetClothCompartmentID();
        if (uiID > 0)
        {
            m_kCompartmentMap.GetAt(uiID, pkClothCompartment);
            NIASSERT(pkClothCompartment);
        }
        NxCompartment* pkFluidCompartment = 0;
        uiID = pkSnapshot->GetFluidCompartmentID();
        if (uiID > 0)
        {
            m_kCompartmentMap.GetAt(uiID, pkFluidCompartment);
            NIASSERT(pkFluidCompartment);
        }
        if (!pkSnapshot->CreateInScene(pkScene, pkProp->GetXform(),
            pkRBCompartment, pkClothCompartment, pkFluidCompartment,
            pkProp->GetKeepMeshes(), uiIndex))
        {
            return false;
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXScene);
//---------------------------------------------------------------------------
void NiPhysXScene::CopyMembers(NiPhysXScene* pkDest, 
    NiCloningProcess& kCloning)
{
    NiObjectNET::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkManager = m_pkManager;

    pkDest->m_kSceneXform = m_kSceneXform;
    pkDest->m_fScalePToW = m_fScalePToW;
    pkDest->m_pkPhysXScene = 0;

    pkDest->m_kProps.SetSize(m_kProps.GetSize());
    for (NiUInt32 ui = 0; ui < m_kProps.GetSize(); ui++)
    {
        NiPhysXProp* pkPropClone = (NiPhysXProp*)
            m_kProps.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kProps.SetAt(ui, pkPropClone);
    }  

    pkDest->m_kSources.SetSize(m_kSources.GetSize());
    for (NiUInt32 ui = 0; ui < m_kSources.GetSize(); ui++)
    {
        NiPhysXSrc* pkSrcClone = (NiPhysXSrc*)
            m_kSources.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kSources.SetAt(ui, pkSrcClone);
    }
    pkDest->m_kDestinations.SetSize(m_kDestinations.GetSize());
    for (NiUInt32 ui = 0; ui < m_kDestinations.GetSize(); ui++)
    {
        NiPhysXDest* pkDestClone = (NiPhysXDest*)
            m_kDestinations.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kDestinations.SetAt(ui, pkDestClone);
    }
    pkDest->m_kModifiedMeshes.SetSize(m_kModifiedMeshes.GetSize());
    for (NiUInt32 ui = 0; ui < m_kModifiedMeshes.GetSize(); ui++)
    {
        NiMesh* pkMeshClone = (NiMesh*)
            m_kModifiedMeshes.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kModifiedMeshes.SetAt(ui, pkMeshClone);
    }

    pkDest->m_fTimestep = m_fTimestep;
    pkDest->m_uiNumSubSteps = m_uiNumSubSteps;
    pkDest->m_uiMaxSubSteps = m_uiMaxSubSteps;

    pkDest->m_fPrevFetchTime = m_fPrevFetchTime;
    pkDest->m_fNextFetchTime = m_fNextFetchTime;

    pkDest->m_bInSimFetch = false;

    pkDest->m_spDebugRoot = 0;
    
    // Snapshots
    if (m_spSnapshot)
    {
        pkDest->m_spSnapshot =
            (NiPhysXSceneDesc*)m_spSnapshot->CreateSharedClone(kCloning);
    }
    else
    {
        pkDest->m_spSnapshot = 0;
    }

    pkDest->m_uFlags = m_uFlags;
    
    if (!GetBit(OWN_MANAGER_MASK))
    {
        pkDest->SetWorkflowManager(m_pkWorkflowManager);
    }
}
//---------------------------------------------------------------------------
void NiPhysXScene::ProcessClone(NiCloningProcess& kCloning)
{
    NiObjectNET::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;

    for (NiUInt32 ui = 0; ui < m_kProps.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXProp* pkProp = m_kProps.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkProp, bJunk))
            pkProp->ProcessClone(kCloning);
    }

    for (NiUInt32 ui = 0; ui < m_kSources.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXSrc* pkSrc = m_kSources.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkSrc, bJunk))
            pkSrc->ProcessClone(kCloning);
    }
    for (NiUInt32 ui = 0; ui < m_kDestinations.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXDest* pkDest = m_kDestinations.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkDest, bJunk))
            pkDest->ProcessClone(kCloning);
    }
    for (NiUInt32 ui = 0; ui < m_kModifiedMeshes.GetSize(); ui++)
    {
        bool bJunk;
        NiMesh* pkMesh = m_kModifiedMeshes.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkMesh, bJunk))
            pkMesh->ProcessClone(kCloning);
    }
    if (m_spSnapshot)
    {
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(m_spSnapshot, bJunk))
            m_spSnapshot->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXScene::RegisterStreamables(NiStream& kStream)
{
    if (!NiObjectNET::RegisterStreamables(kStream))
        return false;
        
    NiUInt32 uiSize;

    // Props
    uiSize = m_kProps.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXPropPtr spProp = m_kProps.GetAt(ui);
        NIASSERT(spProp != 0);
        spProp->RegisterStreamables(kStream);
    }
    
    // Sources, destinations and modified meshes
    uiSize = m_kSources.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXSrcPtr spSrc = m_kSources.GetAt(ui);
        NIASSERT(spSrc != 0);
        spSrc->RegisterStreamables(kStream);
    }
    uiSize = m_kDestinations.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiPhysXDestPtr spDest = m_kDestinations.GetAt(ui);
        NIASSERT(spDest != 0);
        spDest->RegisterStreamables(kStream);
    }
    uiSize = m_kModifiedMeshes.GetSize();
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        NiMeshPtr spMesh = m_kModifiedMeshes.GetAt(ui);
        NIASSERT(spMesh != 0);
        spMesh->RegisterStreamables(kStream);
    }
       
    if (m_spSnapshot)
        m_spSnapshot->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXScene::SaveBinary(NiStream& kStream)
{
    NiObjectNET::SaveBinary(kStream);

    m_kSceneXform.SaveBinary(kStream);
        
    NiStreamSaveBinary(kStream, m_fScalePToW);

    NiUInt32 uiSize;

    // Props
    uiSize = m_kProps.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kProps.GetAt(ui));
    }
    
    // Sources, destinations and modified meshes
    uiSize = m_kSources.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kSources.GetAt(ui));
    }
    uiSize = m_kDestinations.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kDestinations.GetAt(ui));
    }
    uiSize = m_kModifiedMeshes.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (NiUInt32 ui = 0; ui < uiSize; ui++)
    {
        kStream.SaveLinkID(m_kModifiedMeshes.GetAt(ui));
    }
    
    NiStreamSaveBinary(kStream, m_fTimestep);
    NiStreamSaveBinary(kStream, m_uiNumSubSteps);
    NiStreamSaveBinary(kStream, m_uiMaxSubSteps);

    kStream.SaveLinkID(m_spSnapshot);
    
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXScene);
//---------------------------------------------------------------------------
void NiPhysXScene::LoadBinary(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
    {
        LoadOldBinary(kStream);
        return;
    }

    NiObjectNET::LoadBinary(kStream);
    
    m_kSceneXform.LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_fScalePToW);

    NiUInt32 uiSize;

    // Props
    NiStreamLoadBinary(kStream, uiSize);
    m_kProps.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kProps.SetAt(i, (NiPhysXProp*)kStream.ResolveLinkID());
    }

    // Sources and Destinations
    NiStreamLoadBinary(kStream, uiSize);
    m_kSources.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kSources.SetAt(i, (NiPhysXSrc*)kStream.ResolveLinkID());
    }
    NiStreamLoadBinary(kStream, uiSize);
    m_kDestinations.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kDestinations.SetAt(i, (NiPhysXDest*)kStream.ResolveLinkID());
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 4, 0, 0))
    {
        NiStreamLoadBinary(kStream, uiSize);
        m_kModifiedMeshes.SetSize(uiSize);
        for (NiUInt32 i = 0; i < uiSize; i++)
        {
            m_kModifiedMeshes.SetAt(i, (NiMesh*)kStream.ResolveLinkID());
        }
    }

    NiStreamLoadBinary(kStream, m_fTimestep);
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 9))
    {
        NiStreamLoadBinary(kStream, m_uiNumSubSteps);
        NiStreamLoadBinary(kStream, m_uiMaxSubSteps);
    }

    m_spSnapshot = (NiPhysXSceneDesc*)kStream.ResolveLinkID();

    NiStreamLoadBinary(kStream, m_uFlags);

    // The flags we just loaded could have broken workflow manager owning
    // We must own, because we were constructed using the default
    // constructor.
    SetBit(true, OWN_MANAGER_MASK);
}
//---------------------------------------------------------------------------
void NiPhysXScene::LinkObject(NiStream& kStream)
{
    NiObjectNET::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXScene::LoadOldBinary(NiStream& kStream)
{
    NiObjectNET::LoadBinary(kStream);
    
    m_kSceneXform.LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_fScalePToW);

    NiUInt32 uiSize;

    // Sources and Destinations
    NiStreamLoadBinary(kStream, uiSize);
    m_kSources.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kSources.SetAt(i, (NiPhysXSrc*)kStream.ResolveLinkID());
    }
    NiStreamLoadBinary(kStream, uiSize);
    m_kDestinations.SetSize(uiSize);
    for (NiUInt32 i = 0; i < uiSize; i++)
    {
        m_kDestinations.SetAt(i, (NiPhysXDest*)kStream.ResolveLinkID());
    }
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 2, 0, 8))
    {
        NiStreamLoadBinary(kStream, m_fTimestep);
        NiBool bVal;
        NiStreamLoadBinary(kStream, bVal);
        m_bKeepMeshesBackCompatable = bVal ? true : false;
    }

    m_spSnapshot = (NiPhysXSceneDesc*)kStream.ResolveLinkID();
    NIASSERT(m_spSnapshot != 0);

    NiStreamLoadBinary(kStream, m_uFlags);
    SetBit(true, OWN_MANAGER_MASK);
}
//---------------------------------------------------------------------------
void NiPhysXScene::PostLinkObject(NiStream&)
{
    if (!m_spSnapshot)
        return;

    // Do the work of converting prior versions. All pointers are correctly
    // set at this stage. We have to find content in the snapshot and move it
    // to a new NiPhysXProp object.
    //
    if (m_spSnapshot->GetActorCountBackCompatable())
    {
        // Remove any props from, say, particle systems. Keep them to
        // add back later.
        NiTObjectArray<NiPhysXPropPtr> kExistingProps;
        for (NiUInt32 ui = GetPropCount(); ui > 0; ui--)
        {
            kExistingProps.Add(GetPropAt(ui - 1));
            RemovePropAt(ui - 1);
        }
    
        NiPhysXProp* pkProp = NiNew NiPhysXProp();
        
        pkProp->SetScaleFactor(GetScaleFactor());
        
        NiUInt32 uiSize;
        uiSize = m_kSources.GetSize();
        for (NiUInt32 ui = 0; ui < uiSize; ui++)
        {
            pkProp->AddSource(m_kSources.GetAt(ui));
        }
        uiSize = m_kDestinations.GetSize();
        for (NiUInt32 ui = 0; ui < uiSize; ui++)
        {
            pkProp->AddDestination(m_kDestinations.GetAt(ui));
        }
        
        pkProp->SetKeepMeshes(m_bKeepMeshesBackCompatable);
        
        NiPhysXPropDesc* pkPropSnapshot = NiNew NiPhysXPropDesc();
        
        uiSize = m_spSnapshot->GetActorCountBackCompatable();
        for (NiUInt32 ui = 0; ui < uiSize; ui++)
        {
            pkPropSnapshot->AddActor(
                m_spSnapshot->GetActorAtBackCompatable(ui));
        }
        
        uiSize = m_spSnapshot->GetJointCountBackCompatable();
        for (NiUInt32 ui = 0; ui < uiSize; ui++)
        {
            pkPropSnapshot->AddJoint(
                m_spSnapshot->GetJointAtBackCompatable(ui));
        }
        
        NiTMapIterator kIter = m_spSnapshot->GetFirstMaterialBackCompatable();
        while (kIter)
        {
            NiPhysXMaterialDescPtr spMatDesc = 0;
            NxMaterialIndex usIndex;
            m_spSnapshot->
                GetNextMaterialBackCompatable(kIter, usIndex, spMatDesc);
            NxMaterialDesc kMaterial;
            spMatDesc->ToMaterialDesc(kMaterial);
            pkPropSnapshot->SetMaterialAt(kMaterial, usIndex);
            spMatDesc = 0;
        }
        
        pkPropSnapshot->SetNumStates(
            m_spSnapshot->GetNumStatesBackCompatable());
        
        pkProp->SetSnapshot(pkPropSnapshot);
        
        AddProp(pkProp);
        
        for (NiUInt32 ui = 0; ui < kExistingProps.GetSize(); ui++)
        {
            AddProp(kExistingProps.GetAt(ui));
        }
    }
    
    m_spSnapshot->ClearBackCompatable();
}
//---------------------------------------------------------------------------
bool NiPhysXScene::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXScene, pkObject));
    if(!NiObjectNET::IsEqual(pkObject))
        return false;
        
    NiPhysXScene* pkNewObject = (NiPhysXScene*)pkObject;

    if (m_kSceneXform != pkNewObject->m_kSceneXform)
        return false;
        
    if (m_fScalePToW != pkNewObject->m_fScalePToW)
        return false;

    if (m_kProps.GetSize() != pkNewObject->m_kProps.GetSize())
        return false;
    for (NiUInt32 i = 0; i < m_kProps.GetSize(); i++)
    {
        NiPhysXProp* pkProp = m_kProps.GetAt(i);
        NiPhysXProp* pkNewProp = pkNewObject->m_kProps.GetAt(i);
        if (!pkProp->IsEqual(pkNewProp))
            return false;
    }

    if (m_kSources.GetSize() != pkNewObject->m_kSources.GetSize())
        return false;
    for (NiUInt32 i = 0; i < m_kSources.GetSize(); i++)
    {
        NiPhysXSrc* pkSrc = m_kSources.GetAt(i);
        NiPhysXSrc* pkNewSrc = pkNewObject->m_kSources.GetAt(i);
        if (pkSrc && !pkNewSrc)
            return false;
        else if (!pkSrc && pkNewSrc)
            return false;
        else if (pkSrc && pkNewSrc && !pkSrc->IsEqual(pkNewSrc))
            return false;
    }

    if (m_kDestinations.GetSize() != pkNewObject->m_kDestinations.GetSize())
        return false;
    for (NiUInt32 i = 0; i < m_kDestinations.GetSize(); i++)
    {
        NiPhysXDest* pkDest = m_kDestinations.GetAt(i);
        NiPhysXDest* pkNewDest = pkNewObject->m_kDestinations.GetAt(i);
        if (pkDest && !pkNewDest)
            return false;
        else if (!pkDest && pkNewDest)
            return false;
        else if (pkDest && pkNewDest && !pkDest->IsEqual(pkNewDest))
            return false;
    }

    if (m_kModifiedMeshes.GetSize() !=
        pkNewObject->m_kModifiedMeshes.GetSize())
    {
        return false;
    }
    for (NiUInt32 i = 0; i < m_kModifiedMeshes.GetSize(); i++)
    {
        NiMesh* pkMesh = m_kModifiedMeshes.GetAt(i);
        NiMesh* pkNewMesh = pkNewObject->m_kModifiedMeshes.GetAt(i);
        if (pkMesh && !pkNewMesh)
            return false;
        else if (!pkMesh && pkNewMesh)
            return false;
        else if (pkMesh && pkNewMesh && !pkMesh->IsEqual(pkNewMesh))
            return false;
    }

    if (m_fTimestep != pkNewObject->m_fTimestep ||
        m_uiNumSubSteps != pkNewObject->m_uiNumSubSteps ||
        m_uiMaxSubSteps != pkNewObject->m_uiMaxSubSteps)
    {
        return false;
    }
    
    if (m_spSnapshot)
    {
        if (!pkNewObject->m_spSnapshot)
            return false;
        if (!m_spSnapshot->IsEqual(pkNewObject->m_spSnapshot))
            return false;
    }
    else
    {
        if (pkNewObject->m_spSnapshot)
            return false;
    }

    if (m_uFlags != pkNewObject->m_uFlags)
        return false;
        
    return true;
}
//---------------------------------------------------------------------------
