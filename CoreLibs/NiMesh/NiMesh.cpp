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
#include "NiMeshPCH.h"
#include "NiMesh.h"

#include "NiMeshUtilities.h"
#include "NiSkinningMeshModifier.h"
#include <NiAVObject.h>
#include <NiCloningProcess.h>
#include <NiCommonSemantics.h>
#include <NiCullingProcess.h>
#include <NiEnumerationStrings.h>
#include <NiMainMetrics.h>
#include <NiPoint4.h>
#include <NiRenderer.h>
#include <NiShaderFactory.h>
#include <NiSystem.h>
#include <NiTNodeTraversal.h>
#include <NiLog.h>

#if defined(_WII)
#include "NiWiiSkinningMeshConverter.h"
#endif

NiImplementRTTI(NiMesh, NiRenderObject);
NiImplementCreateClone(NiMesh);
NiImplementCreateObject(NiMesh);

bool NiMesh::ms_bPreload = true;
bool NiMesh::ms_bDestroyAppData = true;

//---------------------------------------------------------------------------
NiMesh::NiMesh() :  
    m_uiCurrentMaterialOutputDataStreamIndex(0xFFFFFFFF),
    m_pkBaseInstanceStream(NULL),
    m_pkVisibleInstanceStream(NULL),
    m_bInstancingEnabled(false),
    m_uiNumModifiers(0),
    m_eMeshPrimitiveType(NiPrimitiveType::PRIMITIVE_TRIANGLES), 
    m_uiFirstSubmeshPrimitiveCount(0xFFFFFFFF),
    m_uiSubmeshCount(1),
    m_bInputDataIsFromStreamOut(false)
{
    m_kBound.SetCenter(0.0f, 0.0f, 0.0f);
    m_kBound.SetRadius(0.0f);

    // Set the entire mesh modifier attached array to 0 for each possible 
    // modifier that can be added.
    for (NiUInt32 ui = 0; ui < MAX_MESH_MODIFIER_COUNT; ui++)
        m_aspMeshModifiers[ui] = 0;

    for (NiUInt32 ui = 0; ui < MAX_MESH_MODIFIER_BITARRAY_COUNT; ui++)
        m_auiModifierAttached[ui] = 0;
}
//---------------------------------------------------------------------------
NiMesh::~NiMesh()
{
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    CompleteModifiers(&kSyncArgs);

    // Remove modifiers
    NiUInt32 uiNumModifiers = GetModifierCount();
    for (NiUInt32 ui = uiNumModifiers; ui > 0; ui--)
    {
        RemoveModifierAt(ui-1);
    }

    RemoveAllStreamRefs();
    RemoveOutputStreamsForAllMaterials();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiAVObject over-ride functions.
//---------------------------------------------------------------------------
void NiMesh::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    // The base class version of this function is purposely not called here to
    // allow us to customize the order of operations.

    NIMETRICS_MAIN_INCREMENTUPDATES();

    // Complete anything that must be completed in update or post-update.
    CompleteUpdateModifiers();

    if (kUpdate.GetUpdateControllers())
        UpdateObjectControllers(kUpdate.GetTime());
    
    // Submit any modifiers that should execute now.
    SubmitUpdateModifiers(kUpdate);

    UpdateWorldData();
    UpdateWorldBound();
}
//---------------------------------------------------------------------------
void NiMesh::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    // The base class version of this function is purposely not called here to
    // allow us to customize the order of operations.

    // Complete anything that must be completed in update or post-update.
    CompleteUpdateModifiers();

    NIMETRICS_MAIN_INCREMENTUPDATES();

    UpdateObjectControllers(kUpdate.GetTime(),
        GetSelectiveUpdatePropertyControllers());

    // Submit any modifiers that should execute now.
    SubmitUpdateModifiers(kUpdate);

    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
        UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiMesh::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    // The base class version of this function is purposely not called here to
    // allow us to customize the order of operations.

    // Complete anything that must be completed in update or post-update.
    CompleteUpdateModifiers();

    NIMETRICS_MAIN_INCREMENTUPDATES();

    UpdateObjectControllers(kUpdate.GetTime(),
        GetSelectiveUpdatePropertyControllers());

    // Submit any modifiers that should execute now.
    SubmitUpdateModifiers(kUpdate);

    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
        UpdateWorldBound();
    }
}
//---------------------------------------------------------------------------
void NiMesh::OnVisible(NiCullingProcess& kCuller)
{
    // Issue a prefetch to avoid cache miss latency in CompleteModifiers.
    NiPrefetch128(&m_uiNumModifiers);

    AppendToCullingProcess(kCuller);

    // Complete any tasks that need to be done now.
    NiCullingSyncArgs kSyncArgs(kCuller);
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_VISIBLE;
    CompleteModifiers(&kSyncArgs);

    if (kCuller.GetSubmitModifiers())
    {
        // The culling process must in fact be a NiMeshCullingProcess that
        // has set the workflow manager.
        NiSPWorkflowManager* pkWorkflowManager = kCuller.GetWorkflowManager();
        NIASSERT(pkWorkflowManager);
        
        // Modify the sync args for submission.
        kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_VISIBLE;
        kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
        
        // Schedule processing and execution of post-cull mesh modifiers
        SubmitModifiers(&kSyncArgs, pkWorkflowManager);
    }
}
//---------------------------------------------------------------------------
void NiMesh::RenderImmediate(NiRenderer* pkRenderer)
{
    // Issue prefetches to avoid cache miss latency in CompleteModifiers and
    // SetPropertyState.
    NiPrefetch128(&m_uiNumModifiers);
    NiPrefetch128(&m_spPropertyState);

    // Complete any tasks that need to be done now.
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_RENDER;
    CompleteModifiers(&kSyncArgs);

    NIASSERT(m_spPropertyState);
    pkRenderer->SetPropertyState(m_spPropertyState);
    pkRenderer->SetEffectState(m_spEffectState);

    NiTimeController::OnPreDisplayIterate(GetControllers());

    pkRenderer->RenderMesh(this);
}
//---------------------------------------------------------------------------
void NiMesh::UpdateWorldBound()
{
    // The world bound for instanced objects is updated via the 
    // NiInstaceCullingMeshModifier. 
    if (!GetInstanced())
    {
        m_kWorldBound.Update(m_kBound, m_kWorld);
    }
}
//---------------------------------------------------------------------------
void NiMesh::UpdatePropertiesDownward(NiPropertyState* pkParentState)
{
    m_spPropertyState = PushLocalProperties(pkParentState, true);
    SetMaterialNeedsUpdate(true);
}
//---------------------------------------------------------------------------
void NiMesh::UpdateEffectsDownward(NiDynamicEffectState* pkParentState)
{
    if (pkParentState)
    {
        m_spEffectState = pkParentState;
    }
    else
    {
        // DO NOT change the cached effect state - there is no point in
        // ever having this be NULL - all NULL's are the same as-is
        m_spEffectState = NULL;   
    }

    SetMaterialNeedsUpdate(true);
}
//---------------------------------------------------------------------------
bool NiMesh::RequiresMaterialOption(const NiFixedString& kMaterialOption)
    const
{
    bool bResult = false;

    // Ask the question of our mesh modifiers
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        if (GetModifierAttachedAt(ui) &&
            pkModifier->RequiresMaterialOption(kMaterialOption, bResult))
        {
            return bResult;
        }
    }

    // No mesh modifier responded to the query.
    // No response is equivalent to a negative response.
    return false;
}
//---------------------------------------------------------------------------
void NiMesh::SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms, bool& bRigid)
{
    // Sets the selective update flags the same as NiAVObject does,
    // except checks to see if geometry is skinned.  If it is, then
    // bSelectiveUpdateTransforms and bSelectiveUpdate are set to true,
    // and bRigid is returned as false.

    NiAVObject::SetSelectiveUpdateFlags(bSelectiveUpdate, 
        bSelectiveUpdateTransforms, bRigid);

    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        // If the modifier wants more update activity than the default,
        // then we defer to the modifier's requirements.
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        bool bModSelectiveUpdate = false;
        bool bModSelectiveUpdateTransforms = false;
        bool bModRigid = true;
        pkModifier->SetSelectiveUpdateFlags(bModSelectiveUpdate,
            bModSelectiveUpdateTransforms, bModRigid);

        if (bModSelectiveUpdate)
        {
            bSelectiveUpdate = true;
        }
        
        if (bModSelectiveUpdateTransforms)
        {
            bSelectiveUpdateTransforms = true;
        }
        
        if (!bModRigid)
        {
            bRigid = false;
        }
    }

    if (bSelectiveUpdate)
        SetSelectiveUpdate(bSelectiveUpdate);
    if (bSelectiveUpdateTransforms)
        SetSelectiveUpdateTransforms(true);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Data stream and prmitive functions
//---------------------------------------------------------------------------
NiUInt32 NiMesh::GetPrimitiveCountFromElementCount(
    NiUInt32 uiElementCount) const
{
    NiUInt32 uiPrimitiveCount = 0;
    switch (m_eMeshPrimitiveType)
    {
    case NiPrimitiveType::PRIMITIVE_TRISTRIPS:
        uiPrimitiveCount = uiElementCount - 2; 
        break;
    case NiPrimitiveType::PRIMITIVE_TRIANGLES: 
        uiPrimitiveCount = uiElementCount / 3; 
        break;
    case NiPrimitiveType::PRIMITIVE_LINES:
        uiPrimitiveCount = uiElementCount >> 1; // div 2
        break;
    case NiPrimitiveType::PRIMITIVE_LINESTRIPS:
        uiPrimitiveCount = uiElementCount - 1;
        break;
    case NiPrimitiveType::PRIMITIVE_POINTS:
        uiPrimitiveCount = uiElementCount; 
        break;
    case NiPrimitiveType::PRIMITIVE_QUADS:
        uiPrimitiveCount = uiElementCount >> 2; // div 4
        break;
    default:
        break;
    }
    return uiPrimitiveCount;
}
//---------------------------------------------------------------------------
NiDataStreamRef* NiMesh::AddStream(
    const NiFixedString& kSemantic, 
    NiUInt32 uiSemanticIndex, NiDataStreamElement::Format eFormat, 
    NiUInt32 uiCount, NiUInt8 uiAccessMask, 
    NiDataStream::Usage eUsage, const void* pvSource, 
    bool bForceToolDSCreate, bool bCreateDefaultRegion)
{
    // Create the data stream
    NiDataStream* pkDataStream = 
        NiDataStream::CreateSingleElementDataStream(eFormat, uiCount, 
        uiAccessMask, eUsage, pvSource, bCreateDefaultRegion, 
        bForceToolDSCreate);
    NIASSERT(pkDataStream);

    // Create stream reference for mesh data
    NiDataStreamRef* pkRef = AddStreamRef(pkDataStream, kSemantic, 
        uiSemanticIndex);
    if (bCreateDefaultRegion)
    {
        NIASSERT(pkDataStream->GetRegionCount() == 1);
        pkRef->BindRegionToSubmesh(0, 0);
    }

    return pkRef;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Modifier Support
//---------------------------------------------------------------------------
void NiMesh::SubmitModifiers(NiSyncArgs* pkArgs,
    NiSPWorkflowManager* pkWFManager)
{
    // Iterate over all modifiers looking for those with an interest in
    // the requested sync points.
    bool bSubmitted = false;
    const NiUInt32 uiCount = GetModifierCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        
        if (GetModifierAttachedAt(ui) &&
            pkModifier->SupportsSubmitSyncPoint(pkArgs->m_uiSubmitPoint) &&
            pkModifier->SupportsCompleteSyncPoint(pkArgs->m_uiCompletePoint))
        {
            bSubmitted = pkModifier->SubmitTasks(this, pkArgs, pkWFManager) ||
                bSubmitted;
        }
    }
    
    if (bSubmitted)
    {
        // Finish related tasks in workflow manager. This ensures that all the
        // tasks were grouped into one workflow.
        NiUInt32 uiTaskGroupID = 0;
        NiUInt32 uiTaskGroupMask = 0;
        pkArgs->GetTaskGroupID(uiTaskGroupID, uiTaskGroupMask);
        pkWFManager->FinishRelatedTasks(uiTaskGroupID, uiTaskGroupMask);
    }
}
//---------------------------------------------------------------------------
void NiMesh::CompleteModifiers(NiSyncArgs* pkArgs)
{
    // Iterate over all modifiers looking for those with an interest in
    // the requested sync points.
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        // If the next modifier supports this phase then
        // allow it to perform any post update operations.
        NiMeshModifier*  pkModifier = GetModifierAt(ui);
        
        if (GetModifierAttachedAt(ui) &&
            pkModifier->SupportsSubmitSyncPoint(pkArgs->m_uiSubmitPoint) &&
            pkModifier->SupportsCompleteSyncPoint(pkArgs->m_uiCompletePoint))
        {
            NIVERIFY(pkModifier->CompleteTasks(this, pkArgs));
        }
    }
}
//---------------------------------------------------------------------------
void NiMesh::SubmitUpdateModifiers(NiUpdateProcess& kUpdate)
{
    NiSPWorkflowManager* pkWorkflowManager = kUpdate.GetWorkflowManager();
    NIASSERT(pkWorkflowManager);
    
    // Create the sync args for submission.
    NiUpdateSyncArgs kSyncArgs(kUpdate);
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;

    if (kUpdate.GetSubmitModifiers())
    {
        // Iterate over all modifiers looking for those with an interest in
        // the requested sync points.
        bool bSubmitted = false;
        bool bSubmittedPost = false;
        const NiUInt32 uiCount = GetModifierCount();
        for (NiUInt32 ui = 0; ui < uiCount; ui++)
        {
            if (GetModifierAttachedAt(ui))
            {
                NiMeshModifier* pkModifier = GetModifierAt(ui);
                
                if (pkModifier->SupportsSubmitSyncPoint(
                    NiSyncArgs::SYNC_UPDATE))
                {
                    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_UPDATE;
                    bSubmitted = pkModifier->SubmitTasks(
                        this, &kSyncArgs, pkWorkflowManager) || bSubmitted;
                    // Do not change the order in the || above, because we must
                    // not short-circuit the call to SubmitTasks when 
                    // bSubmitted is true.
                }
                if (pkModifier->SupportsSubmitSyncPoint(
                    NiSyncArgs::SYNC_POST_UPDATE))
                {
                    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_POST_UPDATE;
                    bSubmittedPost = pkModifier->SubmitTasks(
                        this, &kSyncArgs, pkWorkflowManager) || bSubmittedPost;
                    // Do not change the order in the || above, because we must
                    // not short-circuit the call to SubmitTasks when 
                    // bSubmitted is true.
                }
            }
        }
        
        NiUInt32 uiTaskGroupID = 0;
        NiUInt32 uiTaskGroupMask = 0;
        if (bSubmitted)
        {
            // Finish related tasks in workflow manager. This ensures that all
            // the tasks were grouped into one workflow.
            kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_UPDATE;
            kSyncArgs.GetTaskGroupID(uiTaskGroupID, uiTaskGroupMask);
            pkWorkflowManager->FinishRelatedTasks(uiTaskGroupID, 
                uiTaskGroupMask);
        }
        if (bSubmittedPost)
        {
            // Finish related tasks in workflow manager. This ensures that all
            // the tasks were grouped into one workflow.
            kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_POST_UPDATE;
            kSyncArgs.GetTaskGroupID(uiTaskGroupID, uiTaskGroupMask);
            pkWorkflowManager->FinishRelatedTasks(uiTaskGroupID, 
                uiTaskGroupMask);
        }
    }
}
//---------------------------------------------------------------------------
void NiMesh::CompleteUpdateModifiers()
{
    NiSyncArgs kCompleteArgs;
    kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;

    // Iterate over all modifiers looking for those with an interest in
    // the requested sync points.
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        if (GetModifierAttachedAt(ui))
        {
            NiMeshModifier* pkModifier = GetModifierAt(ui);
        
            if (pkModifier->SupportsCompleteSyncPoint(NiSyncArgs::SYNC_UPDATE))
            {
                kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_UPDATE;
                NIVERIFY(pkModifier->CompleteTasks(this, &kCompleteArgs));
            }
            if (pkModifier->SupportsCompleteSyncPoint(
                NiSyncArgs::SYNC_POST_UPDATE))
            {
                kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_POST_UPDATE;
                NIVERIFY(pkModifier->CompleteTasks(this, &kCompleteArgs));
            }
        }
    }
}
//---------------------------------------------------------------------------
struct SubmitModifiersFunctor
{
    NiSyncArgs m_kSyncArgs;
    NiSPWorkflowManager* m_pkWorkflowManager;
    
    SubmitModifiersFunctor(NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWorkflowManager)
    {
        m_pkWorkflowManager = pkWorkflowManager;
        m_kSyncArgs.m_uiSubmitPoint = pkArgs->m_uiSubmitPoint;
        m_kSyncArgs.m_uiCompletePoint = pkArgs->m_uiCompletePoint;
    }
    
    inline void operator () (NiAVObject* pkAVObject)
    {
        if (NiIsKindOf(NiMesh, pkAVObject))
        {
            NiMesh* pkMesh = (NiMesh*)pkAVObject;
            pkMesh->CompleteModifiers(&m_kSyncArgs);
        }
    }
};
//---------------------------------------------------------------------------
void NiMesh::SubmitSceneModifiers(NiAVObject* pkScene, NiSyncArgs* pkArgs,
    NiSPWorkflowManager* pkWorkflowManager)
{
    SubmitModifiersFunctor kFunctor(pkArgs, pkWorkflowManager);
    NiTNodeTraversal::DepthFirst_AllObjects(pkScene, kFunctor);
}
//---------------------------------------------------------------------------
struct CompleteModifiersFunctor
{
    NiSyncArgs m_kSyncArgs;
    
    CompleteModifiersFunctor(NiSyncArgs* pkArgs)
    {
        if (pkArgs)
        {
            m_kSyncArgs.m_uiSubmitPoint = pkArgs->m_uiSubmitPoint;
            m_kSyncArgs.m_uiCompletePoint = pkArgs->m_uiCompletePoint;
        }
        else
        {
            m_kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
            m_kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
        }
    }
    
    inline void operator () (NiAVObject* pkAVObject)
    {
        if (NiIsKindOf(NiMesh, pkAVObject))
        {
            NiMesh* pkMesh = (NiMesh*)pkAVObject;
            pkMesh->CompleteModifiers(&m_kSyncArgs);
        }
    }
};
//---------------------------------------------------------------------------
void NiMesh::CompleteSceneModifiers(NiAVObject* pkScene,
    NiSyncArgs* pkArgs)
{
    CompleteModifiersFunctor kFunctor(pkArgs);
    NiTNodeTraversal::DepthFirst_AllObjects(pkScene, kFunctor);
}
//-------------------------------------------------------------------------
NiMeshModifier* NiMesh::GetModifierByType(const NiRTTI* pkRTTI) const
{
    if (GetModifierCount() == 0)
        return NULL;

    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        if ((pkModifier != NULL) && (pkModifier->IsKindOf(pkRTTI)))
        {
            return pkModifier;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiUInt32 NiMesh::GetModifierIndex(NiMeshModifier* pkModifier) const
{
    // find the modifier in the list
    for (NiUInt32 ui = 0; ui < m_uiNumModifiers; ui++)
    {
        if (m_aspMeshModifiers[ui] == pkModifier)
        {
            return ui;
        }
    }

    // Detect if the modifier was not found in the list
    return (NiUInt32)(-1);
}
//---------------------------------------------------------------------------
bool NiMesh::GetModifierAttachedAt(NiUInt32 uiIndex) const
{
    // This method assumes that the array is of size 1 as this greatly 
    // optimizes the algorithm. If this is not the case, then the algorithm
    // will need to be changed.
    NIASSERT(uiIndex < m_uiNumModifiers);
    NIASSERT(sizeof(m_auiModifierAttached) == sizeof(NiUInt8));
    return  ((m_auiModifierAttached[0] & (0x1 << uiIndex)) != 0);
}
//---------------------------------------------------------------------------
void NiMesh::SetModifierAttachedAt(NiUInt32 uiIndex, bool bAttached)
{
    NIASSERT(uiIndex < m_uiNumModifiers);
    NIASSERT(sizeof(m_auiModifierAttached) == sizeof(NiUInt8));
    if (bAttached)
    {
        m_auiModifierAttached[0] |= (0x1 << uiIndex);
    }
    else
    {
        m_auiModifierAttached[0] &= ~(0x1 << uiIndex);
    }
}
//---------------------------------------------------------------------------
bool NiMesh::AddModifierAt(NiUInt32 uiInsertIndex, 
    NiMeshModifier* pkModifier, bool bAttach)
{
    NIASSERT(uiInsertIndex <= m_uiNumModifiers);

    if (MAX_MESH_MODIFIER_COUNT == m_uiNumModifiers)
    {
        NIASSERT(!"No more modifiers can be added to this mesh."
            " It has reached maximum capacity. See "
            "NiMesh::MAX_MESH_MODIFIER_COUNT.");
        return false;
    }

    // Increase modifier count
    m_uiNumModifiers++;

    // Unless we are adding to the tail, we need to slide all the existing 
    // entries right by one to make room for the new one.
    if (uiInsertIndex < (NiUInt32)(m_uiNumModifiers-1))
    {
        for (NiUInt32 ui = uiInsertIndex; 
            ui < (NiUInt32)(m_uiNumModifiers - 1); ui++)
        {
            m_aspMeshModifiers[ui + 1] = m_aspMeshModifiers[ui];
            SetModifierAttachedAt(ui + 1, GetModifierAttachedAt(ui));
        }

    }

    m_aspMeshModifiers[uiInsertIndex] = pkModifier;

    bool bAttached = false;
    if (bAttach && pkModifier->AreRequirementsMet(this))
    {   
        bAttached = pkModifier->Attach(this);
    }

    SetModifierAttachedAt(uiInsertIndex, bAttached);
    return true;
}
//---------------------------------------------------------------------------
bool NiMesh::AttachModifier(NiMeshModifier* pkModifier)
{
    NIASSERT(pkModifier);

    NiUInt32 uiIndex = GetModifierIndex(pkModifier);
    if (uiIndex == (NiUInt32)(-1))
        return false;

    if (GetModifierAttachedAt(uiIndex))
        return true;

    if (pkModifier->AreRequirementsMet(this) && 
        pkModifier->Attach(this))
    {
        SetModifierAttachedAt(uiIndex, true);
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiMesh::RemoveModifier(NiMeshModifier* pkModifier)
{
    // find the modifier in the list
    NiUInt32 uiRemoveIndex = GetModifierIndex(pkModifier);
    if (uiRemoveIndex == (NiUInt32)(-1))
        return false;

    return RemoveModifierAt(uiRemoveIndex);
}
//---------------------------------------------------------------------------
bool NiMesh::RemoveModifierAt(NiUInt32 uiRemoveIndex)
{
    if (uiRemoveIndex >= m_uiNumModifiers)
        return false;

    // Don't let it self-destruct prematurely...
    NiMeshModifierPtr spModifier = m_aspMeshModifiers[uiRemoveIndex];

    if (GetModifierAttachedAt(uiRemoveIndex))
    {
        NiSyncArgs kSyncArgs;
        kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
        kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
        spModifier->CompleteTasks(this, &kSyncArgs);
        spModifier->Detach(this);
    }

    // Slide all existing entries after the index to the left one slot
    for (NiUInt32 ui = uiRemoveIndex; 
        ui < (NiUInt32)(m_uiNumModifiers - 1); ui++)
    {
        m_aspMeshModifiers[ui] = m_aspMeshModifiers[ui + 1];
        SetModifierAttachedAt(ui, GetModifierAttachedAt(ui + 1));
    }

    // Set the last modifier to NULL. This completes the removal loop above.
    m_aspMeshModifiers[m_uiNumModifiers - 1] = 0;
    SetModifierAttachedAt(m_uiNumModifiers - 1, false);

    // Subtract the removed modifier from the count
    --m_uiNumModifiers;

    return true;
}
//---------------------------------------------------------------------------
bool NiMesh::DetachModifier(NiMeshModifier* pkModifier)
{
    // find the modifier in the list
    NiUInt32 uiRemoveIndex = GetModifierIndex(pkModifier);
    if (uiRemoveIndex == (NiUInt32)(-1))
        return false;

    if (GetModifierAttachedAt(uiRemoveIndex))
    {
        NiSyncArgs kSyncArgs;
        kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
        kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
        pkModifier->CompleteTasks(this, &kSyncArgs);
        
        if (pkModifier->Detach(this))
        {
            SetModifierAttachedAt(uiRemoveIndex, false);
        }
        else
        {
            return false;
        }
    }
    
    // if it wasn't already attached, it has been successfully detached
    // already.
    return true;
}
//---------------------------------------------------------------------------
bool NiMesh::AttachAllModifiers()
{
    // We return false if any attach fails, but we do not abort until we've
    // tried all modifiers.
    bool bSuccess = true;
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        if (GetModifierAttachedAt(ui))
            continue;

        NiMeshModifier* pkModifier = GetModifierAt(ui);
        NIASSERT(pkModifier);

        if (pkModifier->AreRequirementsMet(this) && 
            pkModifier->Attach(this))
        {
            SetModifierAttachedAt(ui, true);
        }
        else
        {
            bSuccess = false;
        }
    }
    
    return bSuccess;
}
//---------------------------------------------------------------------------
bool NiMesh::DetachAllModifiers()
{
    NiSyncArgs kSyncArgs;
    kSyncArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_ANY;
    kSyncArgs.m_uiCompletePoint = NiSyncArgs::SYNC_ANY;
    
    // Detach all modifiers
    // We return false if any detach fails, but we do not abort until we've
    // tried all modifiers.
    bool bSuccess = true;
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        NIASSERT(pkModifier);   

        if (GetModifierAttachedAt(ui))
        {
            if (pkModifier->CompleteTasks(this, &kSyncArgs) &&
                pkModifier->Detach(this))
            {
                SetModifierAttachedAt(ui, false);
            }
            else
            {
                bSuccess = false;
            }
        }
    }

    // Make sure we clear the lock flags for the data streams
    if (bSuccess)
    {
        for (NiUInt32 uiStreamRef = 0; uiStreamRef < m_kDataStreams.GetSize();
            uiStreamRef++)
        {
            NiDataStreamRef* pkStreamRef = m_kDataStreams.GetAt(uiStreamRef);
            NiDataStream* pkDataStream = pkStreamRef->GetDataStream();
            if (pkDataStream)
            {
                pkDataStream->ClearLockFlags();
            }
        }
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
class AttachSceneModifiersFunctor
{
public:
    AttachSceneModifiersFunctor() : m_bSuccess(true) {}

    inline void operator() (NiAVObject* pkObject)
    {
        if (NiIsKindOf(NiMesh, pkObject))
        {
            if (!((NiMesh*) pkObject)->AttachAllModifiers())
                m_bSuccess = false;
        }
    }

    bool m_bSuccess;
};
//-------------------------------------------------------------------------
bool NiMesh::AttachSceneModifiers(NiAVObject* pkObject)
{
    AttachSceneModifiersFunctor kFunctor;
    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
    return kFunctor.m_bSuccess;
}
//---------------------------------------------------------------------------
class DetachSceneModifiersFunctor
{
public:
    DetachSceneModifiersFunctor() : m_bSuccess(true) {}

    inline void operator() (NiAVObject* pkObject)
    {
        if (NiIsKindOf(NiMesh, pkObject))
        {
            if (!((NiMesh*) pkObject)->DetachAllModifiers())
                m_bSuccess = false;
        }
    }

    bool m_bSuccess;
};
//-------------------------------------------------------------------------
bool NiMesh::DetachSceneModifiers(NiAVObject* pkObject)
{
    DetachSceneModifiersFunctor kFunctor;
    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
    return kFunctor.m_bSuccess;
}
//---------------------------------------------------------------------------
bool NiMesh::ResetModifiers()
{
    // If there's an error, we don't bail out but we do return false;
    bool bSuccess = true;
    
    // Detach all modifiers
    if (!DetachAllModifiers())
        bSuccess = false;

    // Reattach all modifiers
    if (!AttachAllModifiers())
        bSuccess = false;

    // Reset all time controllers that might have pointed to modifier data.
    NiTimeController* pkController = m_spControllers;
    while (pkController != NULL)
    {
        pkController->ResetModifierData();
        pkController = pkController->GetNext();
    }
    
    return bSuccess;
}
//---------------------------------------------------------------------------
class ResetSceneModifiersFunctor
{
public:
    ResetSceneModifiersFunctor() : m_bSuccess(true) {}

    inline void operator() (NiAVObject* pkObject)
    {
        if (NiIsKindOf(NiMesh, pkObject))
        {
            if (!((NiMesh*) pkObject)->ResetModifiers())
                m_bSuccess = false;
        }
    }

    bool m_bSuccess;
};
//-------------------------------------------------------------------------
bool NiMesh::ResetSceneModifiers(NiAVObject* pkObject)
{
    ResetSceneModifiersFunctor kFunctor;
    NiTNodeTraversal::DepthFirst_AllObjects(pkObject, kFunctor);
    return kFunctor.m_bSuccess;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiMesh::CopyMembers(NiMesh* pkDest, NiCloningProcess& kCloning)
{
    NiRenderObject::CopyMembers(pkDest, kCloning);

    pkDest->SetPrimitiveType(m_eMeshPrimitiveType);
    pkDest->m_uiSubmeshCount = m_uiSubmeshCount;
    pkDest->CloneStreamRefs(this, &kCloning);
    pkDest->SetInstanced(m_bInstancingEnabled);
    pkDest->SetModelBound(m_kBound);


    // Clone the modifiers. Don't add them yet - do that in ProcessClone
    // when all clones are known to have been generated.
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        
        // The clone will be stored in the clone map for later retrieval
        pkModifier->CreateClone(kCloning);
    }
}
//---------------------------------------------------------------------------
void NiMesh::ProcessClone(NiCloningProcess& kCloning)
{
    NiRenderObject::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned && pkClone->IsKindOf(GetRTTI()));
    if (bCloned && pkClone->IsKindOf(GetRTTI()))
    {
        NiMesh* pkMesh = (NiMesh*) pkClone;

        for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
        {
            NiMeshModifier* pkModifier = GetModifierAt(ui);
            pkModifier->ProcessClone(kCloning);

            NIVERIFY(kCloning.m_pkCloneMap->GetAt(pkModifier, pkClone));
            NiMeshModifier* pNewModifierPtr = (NiMeshModifier*)pkClone;
            pkMesh->AddModifier(pNewModifierPtr, true);
        }
    }

    UpdateCachedPrimitiveCount();
}
//---------------------------------------------------------------------------
void NiMesh::CloneStreamRefs(NiMesh* pkOriginal, NiCloningProcess* pkCloning)
{
    bool bLocalCloningProcess = false;
    if (pkCloning == NULL)
    {
        pkCloning = NiNew NiCloningProcess;
        bLocalCloningProcess = true;
    }

    // Make the internal array the size of the original's array.
    // This will keep unnecessary allocations to a minimum.
    m_kDataStreams.Realloc(pkOriginal->m_kDataStreams.GetSize());

    NiUInt32 ui = 0;
    for (; ui < pkOriginal->m_kDataStreams.GetSize(); ui++)
    {
        // Use the copy constructor to copy all relevant data
        NiDataStreamRef* pkRef = NiNew NiDataStreamRef(*
            pkOriginal->m_kDataStreams.GetAt(ui));
        m_kDataStreams.Add(pkRef);

        // Now override the datastreams with their cloned counterparts
        NiDataStream* pkDS = pkRef->GetDataStream();

        if (pkDS)
        {
            pkDS = (NiDataStream*)pkDS->CreateClone(*pkCloning);
            pkRef->SetDataStream(pkDS);
        }
    }

    // Clone output data streams as well
    const NiUInt32 uiOutputStreamMaterialCount = 
        pkOriginal->m_kOutputDataStreamsByMaterial.GetSize();
    m_kOutputDataStreamsByMaterial.Realloc(uiOutputStreamMaterialCount);
    for (ui = 0; ui < uiOutputStreamMaterialCount; ui++)
    {
        OutputStream* pkOrigOutputStream = 
            pkOriginal->m_kOutputDataStreamsByMaterial.GetAt(ui);
        NIASSERT(pkOrigOutputStream);

        OutputStream* pkNewOutputStream = OutputStream::CloneOutputStream(
            pkOrigOutputStream, *pkCloning);
        m_kOutputDataStreamsByMaterial.Add(pkNewOutputStream);
    }

    if (bLocalCloningProcess)
    {
        NiDelete pkCloning;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiMesh::LoadBinary(NiStream& kStream)
{
    NiRenderObject::LoadBinary(kStream);

    // Load mesh primitive type
    NiStreamLoadEnum(kStream, m_eMeshPrimitiveType);

    // Load the submesh count
    NiStreamLoadBinary(kStream, m_uiSubmeshCount);

    // Load instancing parameters
    NiBool kBool;
    NiStreamLoadBinary(kStream, kBool);
    m_bInstancingEnabled = NIBOOL_IS_TRUE(kBool);

    // Load mesh bounds
    m_kBound.LoadBinary(kStream);

    // Load DataStreamRefs
    StreamRefsLoadBinary(kStream);

    // Load Modifiers
    kStream.ReadMultipleLinkIDs();
}
//---------------------------------------------------------------------------
void NiMesh::LinkObject(NiStream& kStream)
{
    NiRenderObject::LinkObject(kStream);
   
    // Link modifiers
    NiUInt32 uiModifierCount = kStream.GetNumberOfLinkIDs();
    for (NiUInt32 i = 0; i < uiModifierCount; i++)
    {
        NiMeshModifier* pkModifier = 
            (NiMeshModifier*)kStream.GetObjectFromLinkID();
        if (pkModifier)
            AddModifier(pkModifier, false);
    }
}
//---------------------------------------------------------------------------
void NiMesh::PostLinkObject(NiStream& kStream)
{
    NiRenderObject::PostLinkObject(kStream);

    if (m_bInstancingEnabled)
    {
        // Ensure the provided instancing data will work on the current
        // platform.
        NiInstancingUtilities::ValidateInstancingData(this);
    }

#if defined(_WII)
    // Search for a skinning mesh modifier.
    NiSkinningMeshModifier* pkSkinningModifier = 
        NiGetModifier(NiSkinningMeshModifier, this);

    if (pkSkinningModifier)
    {
        NiWiiSkinningMeshConverter kConverter;

        // Transform hardware skinned streams to software streams.
        if (!pkSkinningModifier->GetSoftwareSkinned())
        {
            kConverter.ConvertToGenericSoftwareStreams(this);
            pkSkinningModifier->SetSoftwareSkinned(true);
        }

        // Transform generic software skinned streams to Wii specific streams.
        kConverter.ConvertToWiiSoftwareStreams(this);
    }
#endif

    // Prepares an NiMeshModifier on this NiMesh
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        AttachModifier(pkModifier);
    }

    UpdateCachedPrimitiveCount();

    if (ms_bPreload)
    {
        NiRenderer* pkRenderer = NiRenderer::GetRenderer();
        if (pkRenderer)
            pkRenderer->PrecacheMesh(this, ms_bDestroyAppData);
    }
}
//---------------------------------------------------------------------------
bool NiMesh::RegisterStreamables(NiStream& kStream)
{
    if (!NiRenderObject::RegisterStreamables(kStream))
        return false;

    if (!StreamRefsRegisterStreamables(kStream))
        return false;

    // Register modifiers
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        pkModifier->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMesh::SaveBinary(NiStream& kStream)
{
    NiRenderObject::SaveBinary(kStream);

    // Save primitive type
    NiStreamSaveEnum(kStream, m_eMeshPrimitiveType);

    // Save submesh count
    NiStreamSaveBinary(kStream, m_uiSubmeshCount);

    // Save instancing parameters
    NiBool kBool = m_bInstancingEnabled;
    NiStreamSaveBinary(kStream, kBool);

    // Save bounds
    m_kBound.SaveBinary(kStream);

    // Save stream refs
    StreamRefsSaveBinary(kStream);

    // Save Modifiers
    NiStreamSaveBinary(kStream, (NiUInt32)GetModifierCount());
    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        kStream.SaveLinkID(pkModifier);
    }
}
//---------------------------------------------------------------------------
void NiMesh::StreamRefsLoadBinary(NiStream& kStream)
{
    // Preallocate the datastreams
    NiUInt32 uiStreamRefCount = 0;
    NiStreamLoadBinary(kStream, uiStreamRefCount);
    m_kDataStreams.Realloc(uiStreamRefCount);

    // call LoadBinary on each stream ref
    for (NiUInt32 uiStreamRef = 0; uiStreamRef < uiStreamRefCount;
        uiStreamRef++)
    {
        NiDataStreamRef* pkStreamRef = NiNew NiDataStreamRef();
        pkStreamRef->LoadBinary(kStream);
        m_kDataStreams.Add(pkStreamRef);
    }

    // Output data streams are not streamed in or out
}
//---------------------------------------------------------------------------
bool NiMesh::StreamRefsRegisterStreamables(NiStream& kStream)
{
    // Register element usage references and data stream for each stream
    NiUInt32 uiStreamCount = m_kDataStreams.GetSize();
    for (NiUInt32 ui = 0; ui < uiStreamCount; ui++)
    {
        m_kDataStreams.GetAt(ui)->RegisterStreamables(kStream);
    }

    // Output data streams are not streamed in or out
    return true;
}
//---------------------------------------------------------------------------
void NiMesh::StreamRefsSaveBinary(NiStream& kStream)
{
    // Not all NiDataStream objects should be saved. Compute a count of how
    // many will be saved.
    NiUInt32 uiStreamRefCount = 0;
    for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ++ui)
    {
        NiDataStreamRef* pkStreamRef = m_kDataStreams.GetAt(ui);
        NiDataStream* pkDataStream = pkStreamRef->GetDataStream();
        if (pkDataStream && pkDataStream->GetStreamable())
        {
            uiStreamRefCount++;
        }
    }
    NiStreamSaveBinary(kStream, uiStreamRefCount);

    for (NiUInt32 uiStreamRef = 0; uiStreamRef < m_kDataStreams.GetSize();
        uiStreamRef++)
    {
        NiDataStreamRef* pkStreamRef = m_kDataStreams.GetAt(uiStreamRef);

        NiDataStream* pkDataStream = pkStreamRef->GetDataStream();
        if (!pkDataStream || !pkDataStream->GetStreamable())
        {
            continue;
        }

        pkStreamRef->SaveBinary(kStream);
    }

    // Output data streams are not streamed in or out
}
//---------------------------------------------------------------------------
bool NiMesh::StreamRefsIsEqual(const NiMesh* pkOther) const
{
    if (pkOther->GetStreamRefCount() != GetStreamRefCount())
        return false;

    NiUInt32 ui = 0;
    for (; ui < m_kDataStreams.GetSize(); ++ui)
    {
        NiDataStreamRef* pkStreamRef = m_kDataStreams.GetAt(ui);
        NiDataStreamRef* pkOtherStreamRef = pkOther->m_kDataStreams.GetAt(ui);

        if (!pkStreamRef->IsEqual(pkOtherStreamRef))
            return false;
    }

    const NiUInt32 uiOutputStreamMaterialCount = 
        m_kOutputDataStreamsByMaterial.GetSize();

    if (uiOutputStreamMaterialCount != 
        pkOther->m_kOutputDataStreamsByMaterial.GetSize())
    {
        return false;
    }

    for (ui = 0; ui < uiOutputStreamMaterialCount; ui++)
    {
        const OutputStream* pkOutputStream = 
            m_kOutputDataStreamsByMaterial.GetAt(ui);
        const OutputStream* pkOtherOutputStream = 
            pkOther->m_kOutputDataStreamsByMaterial.GetAt(ui);

        if ((pkOutputStream == NULL) != (pkOtherOutputStream == NULL))
            return false;
        if (pkOutputStream && !pkOutputStream->IsEqual(pkOtherOutputStream))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMesh::IsInstancable(const NiMesh* pkTestMesh) const
{
    // Compare primitive type
    if (m_eMeshPrimitiveType != pkTestMesh->m_eMeshPrimitiveType)
        return false;

    // Compare submesh count
    if (m_uiSubmeshCount != pkTestMesh->m_uiSubmeshCount)
        return false;

    // Stream Output meshes can't be instanced
    if (m_bInputDataIsFromStreamOut || pkTestMesh->m_bInputDataIsFromStreamOut)
        return false;

    // Make sure we are not GPU skinned
    NiSkinningMeshModifier* pkSkinningModifier = (NiSkinningMeshModifier*)
        GetModifierByType(&NiSkinningMeshModifier::ms_RTTI);
    if (pkSkinningModifier)
    {
        if (!pkSkinningModifier->GetSoftwareSkinned())
            return false;
    }

    // Compare the data stream count
    if (pkTestMesh->GetStreamRefCount() != GetStreamRefCount())
        return false;

    // Compare the data streams
    for (NiUInt32 uiStreamRef = 0; uiStreamRef < m_kDataStreams.GetSize(); 
        uiStreamRef++)
    {
        NiDataStreamRef* pkStreamRef = m_kDataStreams.GetAt(uiStreamRef);

        bool bFound = false;
        for (NiUInt32 ui = 0; ui < m_kDataStreams.GetSize(); ui++)
        {
            NiDataStreamRef* pkOtherStreamRef = 
                pkTestMesh->m_kDataStreams.GetAt(ui);

            if (!pkStreamRef->IsEqual(pkOtherStreamRef))
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
            return false;
    }

    // Compare output streams
    for (NiUInt32 uiOutputStreamMaterials = 0; 
        uiOutputStreamMaterials < m_kOutputDataStreamsByMaterial.GetSize(); 
        uiOutputStreamMaterials++)
    {
        OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(
            uiOutputStreamMaterials);

        bool bFound = false;
        for (NiUInt32 ui = 0; 
            ui < pkTestMesh->m_kOutputDataStreamsByMaterial.GetSize(); 
            ui++)
        {
            OutputStream* pkOtherOutputStream = 
                pkTestMesh->m_kOutputDataStreamsByMaterial.GetAt(ui);

            if (!pkOutputStream->IsEqual(pkOtherOutputStream))
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
            return false;
    }

    // Compare material count
    NiUInt32 uiNumMaterials = m_kMaterials.GetSize();

    if (uiNumMaterials != pkTestMesh->m_kMaterials.GetSize())
        return false;

    // Compare materials
    for (NiUInt32 i = 0; i < uiNumMaterials; i++)
    {
        const NiMaterial* pkThisMaterial = m_kMaterials.GetAt(i).GetMaterial();
        const NiMaterial* pkThatMaterial = 
            pkTestMesh->m_kMaterials.GetAt(i).GetMaterial();

        if (pkThisMaterial != pkThatMaterial)
        {
            if (pkThisMaterial->GetName() != pkThatMaterial->GetName())
                return false;

            if (m_kMaterials.GetAt(i).GetMaterialExtraData() != 
                pkTestMesh->m_kMaterials.GetAt(i).GetMaterialExtraData())
            {
                return false;
            }
        }
    }

    // Compare material flags
    if (m_uiActiveMaterial != pkTestMesh->m_uiActiveMaterial ||
        m_bMaterialNeedsUpdateDefault != 
        pkTestMesh->m_bMaterialNeedsUpdateDefault)
    {
        return false;
    }

    // Compare properties
    for (NiUInt32 ui = 0; ui < NiProperty::MAX_TYPES; ui++)
    {
        NiProperty* pkThisProperties[NiProperty::MAX_TYPES];
        m_spPropertyState->GetProperties(pkThisProperties);

        NiProperty* pkTestProperties[NiProperty::MAX_TYPES];
        pkTestMesh->GetPropertyState()->GetProperties(pkTestProperties);

        if (!pkThisProperties[ui] && !pkTestProperties[ui])
            continue;

        if ((!pkThisProperties[ui] && pkTestProperties[ui]) ||
            (pkThisProperties[ui] && !pkTestProperties[ui]) ||
            (!pkThisProperties[ui]->IsEqual(pkTestProperties[ui])))
        {
            return false;
        }
    }

    // Compare effects
    for (NiUInt32 ui = 0; ui < NiDynamicEffect::MAX_TYPES; ui++)
    {
        NiDynamicEffectState* pkTestEffects = pkTestMesh->GetEffectState();
        if (!m_spEffectState && !pkTestEffects)
            continue;

        if ((!m_spEffectState && pkTestEffects) ||
            (m_spEffectState && !pkTestEffects) ||
            (!m_spEffectState->Equal(pkTestEffects)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMesh::IsEqual(NiObject* pkObject)
{
    if (!NiRenderObject::IsEqual(pkObject))
        return false;

    NiMesh* pkMesh = (NiMesh*) pkObject;

    // Compare primitive type
    if (m_eMeshPrimitiveType != pkMesh->m_eMeshPrimitiveType)
        return false;

    // Compare submesh count
    if (m_uiSubmeshCount != pkMesh->m_uiSubmeshCount)
        return false;

    // Compare instancing parameters
    if (m_bInstancingEnabled != pkMesh->m_bInstancingEnabled)
        return false;

    // Compare IsStreamOutput setting
    if (m_bInputDataIsFromStreamOut != pkMesh->m_bInputDataIsFromStreamOut)
        return false;

    if (m_pkBaseInstanceStream)
    {
        if (!pkMesh->m_pkBaseInstanceStream)
        {
            return false;
        }
        if (!m_pkBaseInstanceStream->IsEqual(pkMesh->m_pkBaseInstanceStream))
        {
            return false;
        }
    }
    else
    {
        if (pkMesh->m_pkBaseInstanceStream)
        {
            return false;
        }
    }
    
    if (m_pkVisibleInstanceStream)
    {
        if (!pkMesh->m_pkVisibleInstanceStream)
        {
            return false;
        }
        if (!m_pkVisibleInstanceStream->
            IsEqual(pkMesh->m_pkVisibleInstanceStream))
        {
            return false;
        }
    }
    else
    {
        if (pkMesh->m_pkVisibleInstanceStream)
        {
            return false;
        }
    }
    
    // Compare mesh data
    if (!StreamRefsIsEqual(pkMesh))
        return false;

    // Compare bounds
    if (m_kBound != pkMesh->m_kBound)
        return false;
    
    // Compare modifiers
    if (GetModifierCount() != pkMesh->GetModifierCount())
        return false;

    for (NiUInt32 ui = 0; ui < GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = GetModifierAt(ui);
        NiMeshModifier* pkOtherModifier = pkMesh->GetModifierAt(ui);
        if ((pkModifier && !pkOtherModifier) ||
            (!pkModifier && pkOtherModifier) ||
            (pkModifier && pkOtherModifier &&
            !pkModifier->IsEqual(pkOtherModifier)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMesh::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiRenderObject::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(NiMesh::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString(
        "Primitive Type", 
        GetPrimitiveTypeString()));
}
//---------------------------------------------------------------------------
bool NiMesh::ContainsData(const NiFixedString& kSemantic, 
    NiUInt32 uiSemanticIndex) const
{
    // Determine if the semantic exists in the shader data
    const NiDataStreamRef* pkRef; 
    NiDataStreamElement kElement;

    return FindStreamRefAndElementBySemantic(kSemantic, uiSemanticIndex, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
}
//---------------------------------------------------------------------------
const NiDataStreamRef* NiMesh::GetFirstUsageVertexPerVertexStreamRef() const
{
    // Find first StreamRef with USAGE_VERTEX that contains per-vertex data

    // Iterate over all StreamRefs
    NiUInt32 uiStreamRefCount = GetStreamRefCount();
    for (unsigned int uiStreamRef = 0;
         uiStreamRef < uiStreamRefCount;
         uiStreamRef++)
    {
        const NiDataStreamRef* pkStreamRef = 
            GetStreamRefAt(uiStreamRef);
        NIASSERT(pkStreamRef && pkStreamRef->GetDataStream());
        bool bUsageVertex = pkStreamRef->GetDataStream()->GetUsage() == 
            NiDataStream::USAGE_VERTEX;

        if (bUsageVertex && !pkStreamRef->IsPerInstance())
        {
            return pkStreamRef;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiDataStreamRef* NiMesh::GetFirstUsageVertexPerVertexStreamRef()
{
    // Find first StreamRef with USAGE_VERTEX that contains per-vertex data

    // Iterate over all StreamRefs
    NiUInt32 uiStreamRefCount = GetStreamRefCount();
    for (unsigned int uiStreamRef = 0;
         uiStreamRef < uiStreamRefCount;
         uiStreamRef++)
    {
        NiDataStreamRef* pkStreamRef = GetStreamRefAt(uiStreamRef);
        NIASSERT(pkStreamRef && pkStreamRef->GetDataStream());
        bool bUsageVertex = pkStreamRef->GetDataStream()->GetUsage() == 
            NiDataStream::USAGE_VERTEX;

        if (bUsageVertex && !pkStreamRef->IsPerInstance())
        {
            return pkStreamRef;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiUInt32 NiMesh::GetVertexCount() const
{
    const NiDataStreamRef* pkReferenceStreamRef =
        GetFirstUsageVertexPerVertexStreamRef();
    if (pkReferenceStreamRef)
    {
        // Sum up the counts over all unique regions
        NiUInt32 uiSubmeshCount = GetSubmeshCount();
        NiUInt32 uiVertexCount = 0;
        NiUnsignedIntSet kVisitedSet;
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshCount; uiSubmesh++)
        {
            NiUInt32 uiRegionIdx = 
                pkReferenceStreamRef->GetRegionIndexForSubmesh(uiSubmesh);

            bool bSeenBefore = false;
            for (NiUInt32 j = 0; j < uiSubmesh; j++)
            {
                if (uiRegionIdx == 
                    pkReferenceStreamRef->GetRegionIndexForSubmesh(j))
                {
                    bSeenBefore = true;
                    break;
                }
            }

            // It is possible that a region is shared and therefor repeated.
            // For purposes of counting the total vertices, repeated (or 
            // shared) regions are skipped.
            if (bSeenBefore)
                continue;

            const NiDataStream::Region& kRegion = 
                pkReferenceStreamRef->GetRegionForSubmesh(uiSubmesh);
            uiVertexCount += kRegion.GetRange();
        }
        return uiVertexCount;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiMesh::GetVertexCount(NiUInt32 uiSubmesh) const
{
    const NiDataStreamRef* pkReferenceStreamRef =
        GetFirstUsageVertexPerVertexStreamRef();

    if (pkReferenceStreamRef)
    {
        const NiDataStream::Region& kRegion = 
            pkReferenceStreamRef->GetRegionForSubmesh(uiSubmesh);
        return kRegion.GetRange();
    }
    else
        return 0;
}
//---------------------------------------------------------------------------
void NiMesh::RecomputeBounds()
{   
    NiMeshUtilities::ComputeBoundsFromMesh(this);
}
//---------------------------------------------------------------------------
void NiMesh::RetrieveMeshSet(NiTPrimitiveSet<NiMesh*>& kMeshSet)
{
     kMeshSet.Add(this);
}
//---------------------------------------------------------------------------
const char* NiMesh::GetPrimitiveTypeString()
{
    return NiPrimitiveType::GetStringFromType(m_eMeshPrimitiveType);
}
//---------------------------------------------------------------------------
bool NiMesh::IsValid(NiString* pkInvalidDescription) const
{
    NiUInt32 uiStreamRefCount = GetStreamRefCount();
    for(NiUInt32 uiStream = 0; uiStream < uiStreamRefCount; uiStream++)
    {
        const NiDataStreamRef* pkRef = GetStreamRefAt(uiStream);

        // Check whether the submesh count on the NiDataStreamRef 
        // is the same as the submesh count on the NiMesh.
        NiUInt32 uiDSSubmeshCount = pkRef->GetSubmeshRemapCount();
        if (uiDSSubmeshCount != GetSubmeshCount())
        {
            if (pkInvalidDescription)
            {
                pkInvalidDescription->Format(
                    "Submesh count & NiDataStreamRef submesh count mismatch.");
            }
            return false;
        }

        // Check whether each NiDataStreamRef is valid
        if (!pkRef->IsValid(pkInvalidDescription))
        {
            if (pkInvalidDescription)
            {
                pkInvalidDescription->Format(
                    "Error in stream ref %d: %s", 
                    uiStream, (char*)pkInvalidDescription);
            }
            return false;
        }
    }

    // Check for only one INDEX stream
    if (GetSemanticCount(NiCommonSemantics::INDEX()) > 1)
    {
        if (pkInvalidDescription)
        {
            pkInvalidDescription->Format(
                "More than one INDEX stream.");
        }
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 NiMesh::GetCurrentMaterialOutputStreamRefCount() 
{
    UpdateCurrentMaterialOutputDataStreamIndex();
    if (m_uiCurrentMaterialOutputDataStreamIndex == 0xFFFFFFFF)
        return NULL;
    OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex);
    if (pkOutputStream)
        return pkOutputStream->m_kStreams.GetSize();
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiDataStreamRef* NiMesh::GetCurrentMaterialOutputStreamRefAt(
    NiUInt32 uiOutputStreamIdx)
{
    UpdateCurrentMaterialOutputDataStreamIndex();
    NIASSERT(m_uiCurrentMaterialOutputDataStreamIndex != 0xFFFFFFFF);
    OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex);
    NIASSERT(pkOutputStream);
    OutputStream::Entry* pkEntry = 
        pkOutputStream->m_kStreams.GetAt(uiOutputStreamIdx);
    NIASSERT(pkEntry);
    return pkEntry->m_pkStreamRef;
}
//---------------------------------------------------------------------------
const NiFixedString& NiMesh::GetCurrentMaterialOutputStreamRefNameAt(
    NiUInt32 uiOutputStreamIdx) 
{
    UpdateCurrentMaterialOutputDataStreamIndex();
    NIASSERT(m_uiCurrentMaterialOutputDataStreamIndex != 0xFFFFFFFF);
    OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex);
    NIASSERT(pkOutputStream);
    OutputStream::Entry* pkEntry = 
        pkOutputStream->m_kStreams.GetAt(uiOutputStreamIdx);
    NIASSERT(pkEntry);
    return pkEntry->m_kName;
}
//---------------------------------------------------------------------------
void NiMesh::ClearAllStreamOutBuffers()
{
    unsigned int uiCount = m_kOutputDataStreamsByMaterial.GetSize();
    for (unsigned int i = 0; i < uiCount; i++)
    {
        OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(i);
        if (pkOutputStream)
            pkOutputStream->m_bNeedsCleared = true;
    }
}
//---------------------------------------------------------------------------
void NiMesh::ClearStreamOutBuffers(const char* pcMaterialName)
{
    NiMaterial* pkMaterial = NiMaterial::GetMaterial(pcMaterialName);
    NIASSERT(pkMaterial);

    unsigned int uiCount = m_kOutputDataStreamsByMaterial.GetSize();
    for (unsigned int i = 0; i < uiCount; i++)
    {
        OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(i);
        if (pkOutputStream && pkOutputStream->m_pkMaterial == pkMaterial)
        {
            pkOutputStream->m_bNeedsCleared = true;
            return;
        }
    }

    // Otherwise, the material had not yet been configured for stream output
    // on this mesh.
    NIASSERT(!"This material has not been configured for stream output "
        "on this mesh"); 
}
//---------------------------------------------------------------------------
bool NiMesh::CheckIfActiveMaterialStreamOutBuffersNeedCleared()
{
    UpdateCurrentMaterialOutputDataStreamIndex();
    NIASSERT(m_uiCurrentMaterialOutputDataStreamIndex != 0xFFFFFFFF);
    OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex);
    NIASSERT(pkOutputStream);

    return pkOutputStream->m_bNeedsCleared;
}
//---------------------------------------------------------------------------
void NiMesh::OnActiveMaterialStreamOutBuffersCleared()
{
    UpdateCurrentMaterialOutputDataStreamIndex();
    NIASSERT(m_uiCurrentMaterialOutputDataStreamIndex != 0xFFFFFFFF);
    OutputStream* pkOutputStream = m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex);
    NIASSERT(pkOutputStream);

    pkOutputStream->m_bNeedsCleared = false;
}
//---------------------------------------------------------------------------
NiDataStreamRef* NiMesh::AddOutputStreamRef(
    const NiMaterial* pkMaterial, NiDataStreamRef* pkRef, 
    const NiFixedString& kBindName)
{
    NIASSERT(pkMaterial);

    // see if there is already an OutputStream for the material;
    // if so, append this [sub-]stream ref to it.
    unsigned int uiCount = m_kOutputDataStreamsByMaterial.GetSize();
    OutputStream* pkOutputStream = NULL;
    for (unsigned int i = 0; i < uiCount; i++)
    {
        OutputStream* pkTempOutputStream = m_kOutputDataStreamsByMaterial.GetAt(i);
        if (pkTempOutputStream && pkTempOutputStream->m_pkMaterial == pkMaterial)
        {
            pkOutputStream = pkTempOutputStream;
            break;
        }
    }

    // if we couldn't find OutputStream info for that material, create it:
    if (pkOutputStream == NULL)
    {
        pkOutputStream = NiNew OutputStream;
        pkOutputStream->m_pkMaterial = pkMaterial;
        m_kOutputDataStreamsByMaterial.Add(pkOutputStream);
    }
    NIASSERT(pkOutputStream);

    // finally, add the stream ref:
    OutputStream::Entry* pkOutputStreamRef = NiNew OutputStream::Entry;

    // The NiMesh assumes ownership of the provided stream ref
    pkOutputStreamRef->m_pkStreamRef = pkRef;
    pkOutputStreamRef->m_kName = kBindName;
    pkOutputStream->m_kStreams.Add(pkOutputStreamRef);

    pkRef->SetSubmeshCount(m_uiSubmeshCount, 0);
    
    return pkRef;    
}
//---------------------------------------------------------------------------
NiDataStreamRef* NiMesh::AddOutputStream(const NiMaterial* pkMaterial,
    NiDataStream* pkOutputStream, const NiFixedString& kBindName, 
    const NiFixedString& kSemanticName, NiUInt32 uiSemanticIndex)
{
    NIASSERT(pkMaterial);

    NiDataStreamRef* pkRef = NiNew NiDataStreamRef();
    
    NIASSERT(pkOutputStream);
    pkRef->SetSubmeshCount(m_uiSubmeshCount, 0);
    
    pkRef->SetDataStream(pkOutputStream);
    
    NIASSERT(pkOutputStream);
    pkRef->BindSemanticToElementDescAt(0, kSemanticName, uiSemanticIndex);
    
    pkRef->SetPerInstance(false);

    return AddOutputStreamRef(pkMaterial, pkRef, kBindName);
}
//---------------------------------------------------------------------------
NiDataStreamElement::Format NiMesh::VertexFormatEntryToDataStreamFormat(
    const NiOutputStreamDescriptor::VertexFormatEntry& kEntry)
{
    if (kEntry.m_eDataType == NiOutputStreamDescriptor::DATATYPE_FLOAT)
    {
        switch(kEntry.m_uiComponentCount)
        {
        case 1: 
            return NiDataStreamElement::F_FLOAT32_1; 
        case 2: 
            return NiDataStreamElement::F_FLOAT32_2; 
        case 3: 
            return NiDataStreamElement::F_FLOAT32_3; 
        case 4: 
            return NiDataStreamElement::F_FLOAT32_4; 
        }
    }
    else if (kEntry.m_eDataType == NiOutputStreamDescriptor::DATATYPE_UINT)
    {
        switch(kEntry.m_uiComponentCount)
        {
        case 1: 
            return NiDataStreamElement::F_UINT32_1; 
        case 2: 
            return NiDataStreamElement::F_UINT32_2; 
        case 3: 
            return NiDataStreamElement::F_UINT32_3; 
        case 4: 
            return NiDataStreamElement::F_UINT32_4; 
        }
    }
    else if (kEntry.m_eDataType == NiOutputStreamDescriptor::DATATYPE_INT)
    {
        switch(kEntry.m_uiComponentCount)
        {
        case 1: 
            return NiDataStreamElement::F_INT32_1; 
        case 2: 
            return NiDataStreamElement::F_INT32_2; 
        case 3: 
            return NiDataStreamElement::F_INT32_3; 
        case 4: 
            return NiDataStreamElement::F_INT32_4; 
        }
    }

    NIASSERT(0); 
    
    return NiDataStreamElement::F_TYPE_COUNT;
}
//---------------------------------------------------------------------------
void NiMesh::UpdateCurrentMaterialOutputDataStreamIndex()
{
    const NiMaterial* pkMat = GetActiveMaterial();
    unsigned int uiCount = m_kOutputDataStreamsByMaterial.GetSize();
    if (m_uiCurrentMaterialOutputDataStreamIndex >= uiCount ||
        m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex) == NULL ||
        m_kOutputDataStreamsByMaterial.GetAt(
        m_uiCurrentMaterialOutputDataStreamIndex)->m_pkMaterial != pkMat)
    {
        // search for it
        m_uiCurrentMaterialOutputDataStreamIndex = 0xFFFFFFFF;
        for (unsigned int i = 0; i < uiCount; i++)
        {
            OutputStream* pkOutputStream = 
                m_kOutputDataStreamsByMaterial.GetAt(i);
            if (pkOutputStream && pkOutputStream->m_pkMaterial == pkMat)
            {
                m_uiCurrentMaterialOutputDataStreamIndex = i;
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiMesh::CreateStreamOutputStreamsForActiveMaterial()
{
    // Configures this mesh to use Stream Output with the current material.  
    // Adds a set of 'output' data streams on the mesh, so that when the mesh
    // is drawn, the results will be saved into these streams.  
    // Note that if you switch away from this material and come back, 
    // the output streams will still be there and ready.

    // Check to see if we've already set up the output streams for this 
    // material.
    UpdateCurrentMaterialOutputDataStreamIndex();
    if (m_uiCurrentMaterialOutputDataStreamIndex != 0xFFFFFFFF)
    {
        // if so, just return.
        return true;
    }

    NiShader* pkShader = GetShaderFromMaterial();

    NIASSERT(pkShader);
    if (!pkShader)
    {
        NILOG("NiMesh::CreateStreamOutputStreamsForActiveMaterial(): Error: "
            "Can't set the mesh up for stream output until a material "
            "is applied and activated.\n");
        return false;
    }

    unsigned int uiStreamCount = pkShader->GetOutputStreamCount();
    if (uiStreamCount == 0)   // no stream output - we're fine.
        return true;

    for (unsigned int i = 0; i < uiStreamCount; i++)
    {
        // Create each OutputStream as described by the shader.
        NiOutputStreamDescriptor& kDescriptor = 
            pkShader->GetOutputStreamDescriptor(i);
        unsigned int uiMaxVertexCount = 
            kDescriptor.GetMaxVertexCount();
        const NiOutputStreamDescriptor::VertexFormat& kVertexFormat = 
            kDescriptor.GetVertexFormat();
        // note: some of them have more than one stream, interleaved.
        unsigned int uiSubStreams = kVertexFormat.GetSize();
        NiDataStreamPtr spNewStream;
        if (uiSubStreams == 1)
        {
            NiDataStreamElement::Format eFormat = 
                VertexFormatEntryToDataStreamFormat(kVertexFormat[0]);

            //Helper function to create a data stream with only one element:
            spNewStream = NiDataStream::CreateSingleElementDataStream(eFormat, 
                uiMaxVertexCount, 
                NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_GPU_WRITE,
                NiDataStream::USAGE_VERTEX,
                NULL, true, false);

            // create and add a new NiDataStreamRef based on the stream.
            AddOutputStream( 
                GetActiveMaterial(), 
                spNewStream, 
                kDescriptor.GetName(),
                kVertexFormat[0].m_kSemanticName, 
                kVertexFormat[0].m_uiSemanticIndex );
        }
        else if (uiSubStreams > 1)
        {
            NiDataStreamElementSet kSet;
            unsigned int j = 0;
            for (; j < uiSubStreams; j++)
            {
                NiDataStreamElement::Format eFormat = 
                    VertexFormatEntryToDataStreamFormat(kVertexFormat[j]);
                kSet.AddElement(eFormat);
            }

            //Create a data stream containing a set of elements. 
            spNewStream = NiDataStream::CreateDataStream(kSet, 
                uiMaxVertexCount, 
                NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_GPU_WRITE,
                NiDataStream::USAGE_VERTEX,
                false);

            // Allocate Region 0
            NiDataStream::Region kRegion(0, uiMaxVertexCount);
            spNewStream->AddRegion(kRegion);

            // create an NiDataStreamRef (manually) to it
            NiDataStreamRef* pkRef = NiNew NiDataStreamRef();
            pkRef->SetDataStream(spNewStream);
            for (j = 0; j < uiSubStreams; j++)
            {
                pkRef->BindSemanticToElementDescAt(j, 
                    kVertexFormat[j].m_kSemanticName,
                    kVertexFormat[j].m_uiSemanticIndex);
            }
            pkRef->SetPerInstance(false);

            // add the new NiDataStreamRef to the Mesh.
            AddOutputStreamRef( 
                GetActiveMaterial(), 
                pkRef, 
                kDescriptor.GetName());
        }
        
        if (!spNewStream)
        {
            NILOG("NiMesh::CreateStreamOutputStreamsForActiveMaterial(): "
                "Error: "
                "Unable to create output stream(s).\n");
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiMesh::BindToStreamOutputOf(NiMesh* pkSourceMesh)
{
    // Configures this mesh to draw the results of pkSourceMesh's 
    // Stream Output.  pkSourceMesh must have the desired stream-out 
    // material active, and the callee (this) mesh must have the material
    // designed to draw the results active, when this function is called.
    // After the call, however, you switch away from this material and come 
    // back to it without any problems.

    // References to the output streams of pkSourceMesh 
    // will be added to this mesh's input streams, and other configuration
    // changes necessary to draw the results of Stream Output will be made.

    // (Note: all Stream Output creates un-indexed geometry, so there is
    // no need to create (or copy) an index stream here.)


    // In case the source mesh's output streams haven't been created
    // yet, go ahead and do that now.  (If they were already allocated,
    // it will just return.)
    if (!pkSourceMesh->CreateStreamOutputStreamsForActiveMaterial())
        return false;

    // Set a flag saying this is a mesh created using stream output,
    // that has no indices, that has an unknown number of vertices, 
    // and should be drawn using DrawAuto:
    SetInputDataIsFromStreamOut(true);

    // Stream Output currently only supports one submesh per mesh.
    SetSubmeshCount(1);

    // Effectively turn off view-culling, since, being streamed out,
    // the CPU will have no idea where or how big this object is:
    NiBound kBound;
    kBound.SetCenterAndRadius(NiPoint3(0,0,0), 99999999.9f);
    SetModelBound(kBound);

    // Our (input) vertex streams will all be refs to the OutputDataStreams
    //  on the "stream output" mesh.  (Note: no index stream w/Stream Out.)
    RemoveAllStreamRefs();
    unsigned int uiOutputStreamCount = 
        pkSourceMesh->GetCurrentMaterialOutputStreamRefCount();
    for (unsigned int i = 0; i < uiOutputStreamCount; i++)
    {
        NiDataStreamRef* pkRef = 
            pkSourceMesh->GetCurrentMaterialOutputStreamRefAt(i);
        AddStreamRef(pkRef);
    }

    // Figure out what type of primitives were streamed out,
    // so we know what we're drawing:
    NiShader* pkSourceMeshShader = pkSourceMesh->GetShaderFromMaterial();
    if (!pkSourceMeshShader)
    {
        NILOG("NiMesh::BindToStreamOutputOf(): Error: "
            "No material was attached to source mesh yet.\n");
        return false;
    }
    unsigned int uiStreamCount = pkSourceMeshShader->GetOutputStreamCount();
    if (uiStreamCount < 1)
    {
        NILOG("NiMesh::BindToStreamOutputOf(): Error: "
            "Source mesh's shader had no OutputStreamDescriptors.\n");
        return false;
    }
    NiPrimitiveType::Type ePrimType = 
        pkSourceMeshShader->GetOutputStreamDescriptor(0).GetPrimType();
    SetPrimitiveType(ePrimType);

    // Also check the rest of the streams - make sure PrimType agrees.
    for (unsigned int i = 1; i < uiStreamCount; i++)
    {
        NiPrimitiveType::Type ePrimType2 = 
            pkSourceMeshShader->GetOutputStreamDescriptor(i).GetPrimType();
        if (ePrimType != ePrimType2)
        {
            NILOG("NiMesh::BindToStreamOutputOf(): "
                "Error: Some of the mesh's active shader's\n  "
                "OutputStreamDescriptors disagreed on the OutputPrimType.\n");
            return false; 
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiMesh::OutputStream::Entry class
//---------------------------------------------------------------------------
NiMesh::OutputStream::Entry::Entry() : 
    NiMemObject(), 
    m_pkStreamRef(NULL)
{ 
    /* */
}
//---------------------------------------------------------------------------
NiMesh::OutputStream::Entry::~Entry()
{
    NiDelete m_pkStreamRef;
}
//---------------------------------------------------------------------------
NiMesh::OutputStream::Entry* NiMesh::OutputStream::Entry::CloneEntry(
    const NiMesh::OutputStream::Entry* pkEntry, NiCloningProcess& kCloning)
{
    if (pkEntry == NULL)
        return NULL;

    Entry* pkNew = NiNew Entry;
    pkNew->m_kName = pkEntry->m_kName;

    // Use the copy constructor to copy all relevant data
    NiDataStreamRef* pkRef = NiNew NiDataStreamRef(*(pkEntry->m_pkStreamRef));

    // Now override the datastreams with their cloned counterparts
    NiDataStream* pkDS = pkRef->GetDataStream();

    if (pkDS)
    {
        pkDS = (NiDataStream*)pkDS->CreateClone(kCloning);
        pkRef->SetDataStream(pkDS);
    }
    pkNew->m_pkStreamRef = pkRef;

    return pkNew;
}
//---------------------------------------------------------------------------
bool NiMesh::OutputStream::Entry::IsEqual(
    const NiMesh::OutputStream::Entry* pkOther) const
{
    if (m_kName != pkOther->m_kName)
        return false;

    if ((m_pkStreamRef == NULL) != (pkOther->m_pkStreamRef == NULL))
        return false;

    if (m_pkStreamRef && !m_pkStreamRef->IsEqual(pkOther->m_pkStreamRef))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiMesh::OutputStream class
//---------------------------------------------------------------------------
NiMesh::OutputStream::OutputStream() : 
    NiMemObject(), 
    m_kStreams(0), 
    m_bNeedsCleared(false),
    m_pkMaterial(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiMesh::OutputStream::~OutputStream()
{
    const NiUInt32 uiCount = m_kStreams.GetSize();
    for (NiUInt32 i = 0; i < uiCount; i++)
    {
        OutputStream::Entry* pkEntry = m_kStreams.GetAt(i);
        NiDelete pkEntry;
    }

    m_kStreams.RemoveAll();
}
//---------------------------------------------------------------------------
NiMesh::OutputStream* NiMesh::OutputStream::CloneOutputStream(
    const NiMesh::OutputStream* pkStream, NiCloningProcess& kCloning)
{
    if (pkStream == NULL)
        return NULL;

    OutputStream* pkNew = NiNew OutputStream;

    pkNew->m_bNeedsCleared = pkStream->m_bNeedsCleared;
    pkNew->m_pkMaterial = pkStream->m_pkMaterial;

    const NiUInt32 uiOutputStreamCount = 
        pkStream->m_kStreams.GetSize();
    pkNew->m_kStreams.Realloc(uiOutputStreamCount);

    for (NiUInt32 i = 0; i < uiOutputStreamCount; i++)
    {
        OutputStream::Entry* pkOrigEntry = pkStream->m_kStreams.GetAt(i);
        NIASSERT (pkOrigEntry);

        OutputStream::Entry* pkNewEntry = 
            OutputStream::Entry::CloneEntry(pkOrigEntry, kCloning);
        pkNew->m_kStreams.Add(pkNewEntry);
    }

    return pkNew;
}
//---------------------------------------------------------------------------
bool NiMesh::OutputStream::IsEqual(const NiMesh::OutputStream* pkOther) const
{
    if (m_bNeedsCleared != pkOther->m_bNeedsCleared ||
        m_pkMaterial != pkOther->m_pkMaterial)
    {
        return false;
    }

    const NiUInt32 uiOutputStreamCount = m_kStreams.GetSize();
    if (uiOutputStreamCount != pkOther->m_kStreams.GetSize())
        return false;

    for (NiUInt32 i = 0; i < uiOutputStreamCount; i++)
    {
        const OutputStream::Entry* pkEntry = m_kStreams.GetAt(i);
        const OutputStream::Entry* pkOtherEntry = pkOther->m_kStreams.GetAt(i);

        if ((pkEntry == NULL) != (pkOtherEntry == NULL))
            return false;

        if (pkEntry && !pkEntry->IsEqual(pkOtherEntry))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
