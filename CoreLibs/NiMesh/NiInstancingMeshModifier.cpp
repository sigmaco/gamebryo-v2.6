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
#include "NiRenderObjectMaterialOption.h"
#include "NiInstancingMeshModifier.h"
#include "NiInstanceCullingKernel.h"
#include "NiInstancingUtilities.h"
#include "NiStream.h"
#include "NiNode.h"
#include "NiRenderer.h"

//---------------------------------------------------------------------------
NiImplementRTTI(NiInstancingMeshModifier, NiObject);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiInstancingMeshModifier::NiInstancingMeshModifier() :
    m_pkAffectedMesh(NULL),
    m_pkKernel(NULL),
    m_spTask(0),
    m_spWorkflow(0),
    m_pkDummyOutData(NULL), 
    m_afScale(NULL),
    m_pkDummyOutElementCount(0), 
    m_uiVisibleOutputIndex(0),
    m_bInstanceNodes(false),
    m_bDirtyNodeTransforms(true),
    m_bPerInstanceCulling(false),
    m_bStaticBounds(false)
{
}
//---------------------------------------------------------------------------
NiInstancingMeshModifier::~NiInstancingMeshModifier()
{
    for (NiUInt32 ui = 0; ui < m_kInstanceNodes.GetSize(); ui++)
    {
        NiMeshHWInstance* pkInstance = m_kInstanceNodes.GetAt(ui);
        pkInstance->SetMesh(NULL);

        if (pkInstance->GetParent())
            pkInstance->GetParent()->DetachChild(pkInstance);
    }

    m_kInstanceNodes.RemoveAll();

    if (m_pkDummyOutElementCount > 0)
    {
        NiFree(m_pkDummyOutData);
        m_pkDummyOutData = NULL;
    }

    NiFree(m_afScale);
    m_afScale = NULL;

    m_spTask = 0;
    NiDelete m_pkKernel;
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::SetSelectiveUpdateFlags(bool& bSelectiveUpdate,
    bool& bSelectiveUpdateTransforms, bool& bRigid)
{
    if (!m_bStaticBounds || m_bInstanceNodes)
    {
        bSelectiveUpdate = true;
        bSelectiveUpdateTransforms = true;
        bRigid = false;
    }
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::AddMeshInstance(NiMeshHWInstance* pkHWInstance)
{
    if (!m_pkAffectedMesh || !pkHWInstance)
        return;

    if (NiInstancingUtilities::GetActiveInstanceCount(m_pkAffectedMesh) >= 
        NiInstancingUtilities::GetMaxInstanceCount(m_pkAffectedMesh))
    {
        return;
    }

    m_kInstanceNodes.Add(pkHWInstance);
    pkHWInstance->SetMesh(m_pkAffectedMesh);
    m_bInstanceNodes = true;

    // Update the region range with the new active count.
    NiInstancingUtilities::SetActiveInstanceCount(m_pkAffectedMesh, 
        m_kInstanceNodes.GetSize());
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::RemoveMeshInstance(
    NiMeshHWInstance* pkHWInstance)
{
    if (!m_pkAffectedMesh || !pkHWInstance)
        return;

    NiUInt32 uiIndex = 0;

    for (uiIndex = 0; uiIndex < m_kInstanceNodes.GetSize(); uiIndex++)
    {
        if (m_kInstanceNodes.GetAt(uiIndex) == pkHWInstance)
            break;
    }

    // Early out if the instance is not found.
    if (uiIndex == m_kInstanceNodes.GetSize())
        return;

    pkHWInstance->SetMesh(NULL);
    m_kInstanceNodes.RemoveAtAndFill(uiIndex);

    if (m_kInstanceNodes.GetSize() == 0)
        m_bInstanceNodes = false;

    // Update the region range with the new active count.
    NiInstancingUtilities::SetActiveInstanceCount(m_pkAffectedMesh, 
        m_kInstanceNodes.GetSize());
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::SetInstanceBoundScale(float* pkBoundScales, 
    NiUInt32 uiScaleCount, NiUInt32 uiInstanceOffset)
{
    NIASSERT(m_pkAffectedMesh);
    NIASSERT(m_afScale);
    NIASSERT(pkBoundScales);

    const NiUInt32 uiMaxInstances = 
        NiInstancingUtilities::GetMaxInstanceCount(m_pkAffectedMesh);

    if (uiInstanceOffset + uiScaleCount > uiMaxInstances)
    {
        NiOutputDebugString("Warning: Tried to apply an instance scale "
            "to a NiInstancingMeshModifier beyond the maximum number of "
            "instances on the mesh.\n");
        return false;
    }

    NiMemcpy(m_afScale + uiInstanceOffset, pkBoundScales, 
        uiScaleCount * sizeof(float));

    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::RequiresMaterialOption(
    const NiFixedString& kMaterialOption, bool& bResult) const
{
    if (kMaterialOption == NiRenderObjectMaterialOption::TRANSFORM_INSTANCED())
    {
        bResult = true;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::AreRequirementsMet(NiMesh* pkMesh,
    NiSystemDesc::RendererID) const
{
    // Validate setup.

    if (!pkMesh->GetInstanced())
        return false;

    if (m_bPerInstanceCulling)
    {
        NiDataStreamRef* pkDSRef = 0;

        // Check for CPU transforms positions
        pkDSRef = pkMesh->FindStreamRef(NiCommonSemantics::TRANSFORMS(), 0,
            NiDataStreamElement::F_FLOAT32_4);
        if (!pkDSRef)
        {
            NILOG("NiInstancingMeshModifier: There must be a CPU "
                "transforms data stream for instanced meshes with per instance"
                " culling.\n");
            return false;
        }
        
        NiUInt32 uiFlags = NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
        if ( (pkDSRef->GetAccessMask() & uiFlags) == 0)
        {
            NILOG("NiInstancingMeshModifier: The CPU transforms data "
                "stream must be CPU readable and CPU MUTABLE writable\n");
            return false;
        }

        // Check for GPU transforms positions
        pkDSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::INSTANCETRANSFORMS(), 0,
            NiDataStreamElement::F_FLOAT32_4);
        if (!pkDSRef)
        {
            NILOG("NiInstancingMeshModifier: There must be a GPU "
                "transforms data stream for instanced meshes with per instance"
                " culling.\n");
            return false;
        }

        uiFlags = NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
        if ( (pkDSRef->GetAccessMask() & uiFlags) == 0)
        {
            NILOG("NiInstancingMeshModifier: The GPU transforms data "
                "stream must be GPU readable and CPU VOLATILE writable\n");
            return false;
        }
    }
    else
    {
        // Check for GPU transforms positions
        NiDataStreamRef* pkDSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::INSTANCETRANSFORMS(), 0,
            NiDataStreamElement::F_FLOAT32_4);
        if (!pkDSRef)
        {
            NILOG("NiInstancingMeshModifier: There must be a GPU "
                "transforms data stream for instanced meshes.\n");
            return false;
        }

        const NiUInt32 uiFlags = NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
            if ( (pkDSRef->GetAccessMask() & uiFlags) == 0)
            {
                NILOG("NiInstancingMeshModifier: The GPU transforms data "
                    "stream must be GPU readable and CPU MUTABLE writable\n");
                return false;
            }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::RetrieveRequirements(
    NiMeshRequirements& kRequirements) const
{
    NiFixedString kSemantic;

    NiUInt32 uiSet = kRequirements.CreateNewRequirementSet();

    if (m_bPerInstanceCulling)
    {
        // CPU Stream to provide input to the NiInstanceCullingKernel
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::TRANSFORMS(), 0,
            NiMeshRequirements::FLOATER, 0, NiDataStream::USAGE_USER,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStreamElement::F_FLOAT32_4);

        kRequirements.AddRequirement(uiSet, NiCommonSemantics::TRANSFORMS(), 1,
            NiMeshRequirements::FLOATER, 0, NiDataStream::USAGE_USER,
            NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStreamElement::F_FLOAT32_4);

        kRequirements.AddRequirement(uiSet, NiCommonSemantics::TRANSFORMS(), 2,
            NiMeshRequirements::FLOATER, 0, NiDataStream::USAGE_USER,
            NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStreamElement::F_FLOAT32_4);

        // GPU Stream to store results of NiInstanceCullingKernel
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::
            INSTANCETRANSFORMS(), 0, NiMeshRequirements::STRICT_INTERLEAVE,
            1, NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            NiDataStreamElement::F_FLOAT32_4);

        kRequirements.AddRequirement(uiSet, NiCommonSemantics::
            INSTANCETRANSFORMS(), 1, NiMeshRequirements::STRICT_INTERLEAVE,
            1, NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            NiDataStreamElement::F_FLOAT32_4);

        kRequirements.AddRequirement(uiSet, NiCommonSemantics::
            INSTANCETRANSFORMS(), 2, NiMeshRequirements::STRICT_INTERLEAVE,
            1, NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_VOLATILE,
            NiDataStreamElement::F_FLOAT32_4);
    }
    else
    {
        // GPU Stream used directly
        kRequirements.AddRequirement(uiSet, NiCommonSemantics::
            INSTANCETRANSFORMS(), 0, NiMeshRequirements::STRICT_INTERLEAVE,
            1, NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStreamElement::F_FLOAT32_4);

        kRequirements.AddRequirement(uiSet, NiCommonSemantics::
            INSTANCETRANSFORMS(), 1, NiMeshRequirements::STRICT_INTERLEAVE,
            1, NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStreamElement::F_FLOAT32_4);

        kRequirements.AddRequirement(uiSet, NiCommonSemantics::
            INSTANCETRANSFORMS(), 2, NiMeshRequirements::STRICT_INTERLEAVE,
            1, NiDataStream::USAGE_VERTEX, NiDataStream::ACCESS_GPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
            NiDataStreamElement::F_FLOAT32_4);
    }
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::Attach(NiMesh* pkMesh)
{
    // Pre-size arrays
    m_kSubmitPoints.Realloc(2);
    m_kCompletePoints.Realloc(1);
    
    // We want to be notified of update to do some non-floodgate work
    AddSubmitSyncPoint(NiSyncArgs::SYNC_UPDATE);
    
    // Tasks are submitted if visible
    AddSubmitSyncPoint(NiSyncArgs::SYNC_VISIBLE);

    // And finished on render
    AddCompleteSyncPoint(NiSyncArgs::SYNC_RENDER);

    m_pkAffectedMesh = pkMesh;

    // Set the streams.
    SetMeshInstanceCache();

    if (!m_bPerInstanceCulling)
        return true;

    // Get the input data stream.
    NiDataStreamRef* pkInputDSRef = pkMesh->GetBaseInstanceStream();

    if (!pkInputDSRef)
        return false;

    NiDataStream* pkInputDS = pkInputDSRef->GetDataStream();

    NIASSERT(pkInputDS->GetStride() == sizeof(NiPoint4) * 3);

    NiUInt32 uiInBlockCount = 
        NiInstancingUtilities::GetActiveInstanceCount(pkMesh);

    // Get new task 
    m_spTask = NiSPTask::GetNewTask(5, 2);
    
    // Apply instance culling kernel
    m_pkKernel = NiNew NiInstanceCullingKernel();
    m_spTask->SetKernel(m_pkKernel);

    // Add output data stream. This is the stream that will contain the 
    // resulting instance transforms to be used by the GPU.
    NiDataStreamRef* pkOutputDSRef = pkMesh->GetVisibleInstanceStream();
    if (!pkOutputDSRef)
        return false;
    NiDataStream* pkOutputDS = pkOutputDSRef->GetDataStream();
    m_kOutSPStream.SetDataSource(pkOutputDS, false);
    // Over-ride the values set by SetDataSource
    m_kOutSPStream.SetStride((NiUInt16)(pkInputDS->GetStride()));
    m_kOutSPStream.SetBlockCount(uiInBlockCount);
    m_spTask->AddOutput(&m_kOutSPStream);
   
#ifndef _PS3
    // Add a dummy data stream to be used as a temporary storage location
    // for kernels to store the visible instances before the results are
    // copied to the final output data stream.
    m_kDummyOutSPStream.SetStride((NiUInt16)(pkInputDS->GetStride()));
    m_spTask->AddOutput(&m_kDummyOutSPStream);
#endif

    // Add input data stream. This data stream represents the primary 
    // transforms list. It contains the transforms for all of the active
    // instances.
    m_kInSPStream.SetDataSource(pkInputDS, false);
    m_spTask->AddInput(&m_kInSPStream);

    // Add the local bound as a fixed input. This local bound represents
    // the bounds for a single instance of the mesh in local space.
    m_kLocalBoundStream.SetStride(sizeof(NiBound));
    m_kLocalBoundStream.SetBlockCount(1);
    m_spTask->AddInput(&m_kLocalBoundStream);
   
    // Add an array of NiPlane objects as fixed input for culling.
    m_kFrustumPlanesStream.SetStride(sizeof(NiPlane));
    m_kFrustumPlanesStream.SetBlockCount(NiFrustumPlanes::MAX_PLANES);
    m_spTask->AddInput(&m_kFrustumPlanesStream);

    // Add the visible output buffer index as a fixed input. This will be used
    // by the kernel to allocate a section of the output stream.
    m_kVisibleArrayIndexStream.SetStride(sizeof(m_uiVisibleOutputIndex));
    m_kVisibleArrayIndexStream.SetData(&m_uiVisibleOutputIndex);
    m_kVisibleArrayIndexStream.SetBlockCount(1);
    m_spTask->AddInput(&m_kVisibleArrayIndexStream);

    // Allocate an array of floats to hold the scale of the bounds for all the
    // instances.
    NIASSERT(!m_afScale);
    m_afScale = (float*)NiMalloc(sizeof(float) * pkInputDS->GetTotalCount());

    // Set a default scale of 1.0f to be used in case no scale data 
    // is available.
    for (NiUInt32 ui = 0; ui < pkInputDS->GetTotalCount(); ui++)
        m_afScale[ui] = 1.0f;

    // Add the bound scale input stream. This is used to determine the scale
    // that should be applied to the bounding volume's radius. 
    m_kInScaleSPStream.SetStride(sizeof(float));
    m_kInScaleSPStream.SetData(m_afScale);
    m_kInScaleSPStream.SetBlockCount(pkInputDS->GetTotalCount());
    m_spTask->AddInput(&m_kInScaleSPStream);

    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::Detach(NiMesh*)
{
    if (!m_bPerInstanceCulling)
        return true;

    m_spTask = 0;
    NiDelete m_pkKernel;
    m_pkKernel = 0;

    m_pkAffectedMesh = NULL;

    m_kInSPStream.ClearTaskArrays();
    m_kOutSPStream.ClearTaskArrays(); 
#ifndef  _PS3
    m_kDummyOutSPStream.ClearTaskArrays();
#endif
    m_kWorldTransformStream.ClearTaskArrays();
    m_kLocalBoundStream.ClearTaskArrays();
    m_kFrustumPlanesStream.ClearTaskArrays();
    m_kOutputStreamEA.ClearTaskArrays();
    m_kVisibleArrayIndexStream.ClearTaskArrays();
    m_kInScaleSPStream.ClearTaskArrays();

    NiFree(m_afScale);
    m_afScale = NULL;

    return true;
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::SubmitTasks(NiMesh* pkMesh,
    NiSyncArgs* pkArgs, NiSPWorkflowManager* pkWFManager)
{
    // Complete any outstanding workflow. Only SYNC_VISIBLE results in
    // workflow submission, and we assume that OnVisible is only called once
    // per frame for any given object, so the modifier, if non-zero, must be
    // hanging around from the last culling pass. Although why the object was
    // visible but not rendered is inexplicable.
    if (m_spWorkflow)
    {
        NiSyncArgs kCompleteArgs;
        kCompleteArgs.m_uiSubmitPoint = NiSyncArgs::SYNC_VISIBLE;
        kCompleteArgs.m_uiCompletePoint = NiSyncArgs::SYNC_RENDER;
        CompleteTasks(pkMesh, &kCompleteArgs);
    }

    if (pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_UPDATE)
    {
        if (m_bInstanceNodes)
        {
            // Note: If bounds need to be updated they will be updated inside
            // of GatherInstanceNodeTransforms() in order to prevent having
            // to iterate through all the instance transforms multiple times
            // per frame.
            GatherInstanceNodeTransforms(pkMesh, !m_bStaticBounds);
        }
        else if (!m_bStaticBounds)
        {
            NiInstancingUtilities::ComputeWorldBound(pkMesh);
        }
        
        return false;
    }
    else if (m_bPerInstanceCulling &&
        pkArgs->m_uiSubmitPoint == NiSyncArgs::SYNC_VISIBLE)
    {
        NiUInt32 uiInBlockCount = 
            NiInstancingUtilities::GetActiveInstanceCount(pkMesh);

        // If no active instances, don't bother with running the kernel.
        if (uiInBlockCount == 0)
        {
            NiInstancingUtilities::SetVisibleInstanceCount(pkMesh, 0);
            return false;
        }

        m_kInSPStream.SetBlockCount(uiInBlockCount);
        m_kInScaleSPStream.SetBlockCount(uiInBlockCount);
        m_kOutSPStream.SetBlockCount(uiInBlockCount);         

        m_uiVisibleOutputIndex = 0;

        m_kBound = pkMesh->GetModelBound();
        m_kLocalBoundStream.SetData(&m_kBound);

        NiCullingSyncArgs* pkCullingArgs = (NiCullingSyncArgs*)pkArgs;
        const NiFrustumPlanes kFrustumPlanes =
            pkCullingArgs->m_kCullingProcess.GetFrustumPlanes();
        for (NiUInt32 uiPlane = 0; uiPlane < NiFrustumPlanes::MAX_PLANES; 
            uiPlane++)
        {
            m_kPlanes[uiPlane] = kFrustumPlanes.GetPlane(uiPlane);
        }

        m_kFrustumPlanesStream.SetData(&m_kPlanes[0]);

        m_kVisibleArrayIndexStream.SetData(&m_uiVisibleOutputIndex);

#ifndef  _PS3
        // Ensure the temporary dummy output data is properly allocated.
        if (uiInBlockCount > m_pkDummyOutElementCount)
        {
            m_pkDummyOutData = (NiPoint4*)NiRealloc(m_pkDummyOutData, 
                m_kOutSPStream.GetStride() * uiInBlockCount);
            m_pkDummyOutElementCount = uiInBlockCount;
        }
        m_kDummyOutSPStream.SetData(m_pkDummyOutData);
        m_kDummyOutSPStream.SetBlockCount(uiInBlockCount);
#endif

        NiUInt32 uiTaskGroupID = NiSyncArgs::GetTaskGroupID(
            NiSyncArgs::SYNC_VISIBLE, NiSyncArgs::SYNC_RENDER);
        m_spWorkflow =
            pkWFManager->AddRelatedTask(m_spTask, uiTaskGroupID, false);
        
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::CompleteTasks(NiMesh* pkMesh,
    NiSyncArgs*)
{
    if (m_spWorkflow)
    {
        NiStreamProcessor::Get()->Wait(m_spWorkflow);
        m_spWorkflow = 0;

        if (m_bPerInstanceCulling)
        {
            NiInstancingUtilities::SetVisibleInstanceCount(pkMesh,
                m_uiVisibleOutputIndex);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::GatherInstanceNodeTransforms(
    NiMesh* pkMesh, bool bUpdateWorldBounds)
{
    NiUInt32 uiSize = m_kInstanceNodes.GetSize();
    
    if (uiSize == 0 || !m_bDirtyNodeTransforms)
        return;

    NiDataStreamRef* pkStreamRef = pkMesh->GetBaseInstanceStream();
    NiDataStream* pkDataStream = pkStreamRef->GetDataStream();

    // The stream should have a stride of 3 NiPoint4 objects
    NIASSERT(pkDataStream->GetStride() == sizeof(NiPoint4) * 3);

    // Lock the data stream instance data for read.
    NiPoint4* pkData = (NiPoint4*)pkDataStream->Lock(NiDataStream::LOCK_WRITE);

    NiBound kMergedBound;
    NiBound kWorldBound;
    NiBound kModelBound = pkMesh->GetModelBound();
    NiMeshHWInstance* pkInstanceNode = m_kInstanceNodes.GetAt(0);

    NiTransform& kTransform = 
        const_cast<NiTransform&>(pkInstanceNode->GetWorldTransform());
    
    NiInstancingUtilities::PackTransform(kTransform, pkData);
    if (m_afScale)
        m_afScale[0] = kTransform.m_fScale;
    pkData += 3;

    if (bUpdateWorldBounds)
        kMergedBound.Update(kModelBound, kTransform);


    for (NiUInt32 ui = 1; ui < uiSize; ui++)
    {
        NiMeshHWInstance* pkInstNode = m_kInstanceNodes.GetAt(ui);

        NiTransform& kWorldTransform = 
            const_cast<NiTransform&>(pkInstNode->GetWorldTransform());

        NiInstancingUtilities::PackTransform(kWorldTransform, pkData);
        if (m_afScale)
            m_afScale[ui] = kWorldTransform.m_fScale;

        if (bUpdateWorldBounds)
        {
            kWorldBound.Update(kModelBound, kWorldTransform);
            if (kWorldBound.GetRadius() != 0.0)
                kMergedBound.Merge(&kWorldBound);
        }
        pkData += 3;
    }

    pkDataStream->Unlock(NiDataStream::LOCK_WRITE);

    if (bUpdateWorldBounds)
        pkMesh->SetWorldBound(kMergedBound);

    m_bDirtyNodeTransforms = false;
    pkMesh->UpdateCachedPrimitiveCount();
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::SetMeshInstanceCache()
{
    if (m_bPerInstanceCulling)
    {
        m_pkAffectedMesh->SetBaseInstanceStream(
            m_pkAffectedMesh->FindStreamRef(NiCommonSemantics::TRANSFORMS()) );

        NiDataStreamRef* pkCollectorStreamRef = 
            m_pkAffectedMesh->FindStreamRef(
            NiCommonSemantics::INSTANCETRANSFORMS());

        if (pkCollectorStreamRef)
        {
            m_pkAffectedMesh->SetVisibleInstanceStream(pkCollectorStreamRef);
        }
        else
        {
            NiUInt32 uiTransformsPerSubmesh = 
                NiInstancingUtilities::GetTransformsPerSubmesh(
                m_pkAffectedMesh);
            NiUInt32 uiMaxInstanceCount = 
                NiInstancingUtilities::GetMaxInstanceCount(m_pkAffectedMesh);

            // Need to create a NiCommonSemantics::INSTANCETRANSFORMS() data 
            // stream. This simply acts a data collector for the results of 
            // the InstanceCullingKernel so it is not streamed.
            NiInstancingUtilities::CreateCollectorDataStream(m_pkAffectedMesh,
                uiMaxInstanceCount);

            // Setup the regions for the collector data stream.
            NiInstancingUtilities::SetupTransformStreamRegions(
                m_pkAffectedMesh,
                NiCommonSemantics::INSTANCETRANSFORMS(), 
                m_pkAffectedMesh->GetSubmeshCount(), 
                uiTransformsPerSubmesh, 
                uiMaxInstanceCount);
        }
    }
    else
    {
        m_pkAffectedMesh->SetBaseInstanceStream(
            m_pkAffectedMesh->FindStreamRef(
            NiCommonSemantics::INSTANCETRANSFORMS()) );

        m_pkAffectedMesh->SetVisibleInstanceStream(
            m_pkAffectedMesh->GetBaseInstanceStream() );
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiInstancingMeshModifier);
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::CopyMembers(
    NiInstancingMeshModifier* pkDest, 
    NiCloningProcess& kCloning) 
{
    NiMeshModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_bInstanceNodes = m_bInstanceNodes;
    pkDest->m_bPerInstanceCulling = m_bPerInstanceCulling;
    pkDest->m_bStaticBounds = m_bStaticBounds;
    pkDest->m_kBound = m_kBound;

    pkDest->m_pkAffectedMesh = NULL;
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned && pkClone->IsKindOf(GetRTTI()));
    if (bCloned && pkClone->IsKindOf(GetRTTI()))
    {
        NiInstancingMeshModifier* pkModifierClone = 
            (NiInstancingMeshModifier*)pkClone;

        NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkAffectedMesh, pkClone));
        pkModifierClone->m_pkAffectedMesh = (NiMesh*)pkClone;

        pkModifierClone->SetMeshInstanceCache();

        if (m_bInstanceNodes)
        {
            NiInstancingUtilities::SetActiveInstanceCount(
                pkModifierClone->m_pkAffectedMesh, 0);

            m_bDirtyNodeTransforms = true;        
            NiUInt32 uiNumElements = m_kInstanceNodes.GetSize();
            for (NiUInt32 ui = 0; ui < uiNumElements; ui++)
            {
                NIVERIFY(kCloning.m_pkCloneMap->GetAt(
                    m_kInstanceNodes.GetAt(ui), pkClone));

                pkModifierClone->AddMeshInstance((NiMeshHWInstance*)pkClone);
            }
        }

    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiInstancingMeshModifier);
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::LoadBinary(NiStream& kStream)
{
    NiMeshModifier::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_bInstanceNodes);
    NiStreamLoadBinary(kStream, m_bPerInstanceCulling);
    NiStreamLoadBinary(kStream, m_bStaticBounds);

    kStream.ReadLinkID();

    if (m_bStaticBounds)
        m_kBound.LoadBinary(kStream);

    if (m_bInstanceNodes)
    {
        m_bDirtyNodeTransforms = true;
        kStream.ReadMultipleLinkIDs();  // m_kInstanceNodes
    }
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::LinkObject(NiStream& kStream)
{
    NiMeshModifier::LinkObject(kStream);

    m_pkAffectedMesh = (NiMesh*) kStream.GetObjectFromLinkID();

    // Link instance nodes
    if (m_bInstanceNodes)
    {
        NiUInt32 uiSize = kStream.GetNumberOfLinkIDs();

        if (uiSize)
        {
            for (NiUInt32 ui = 0; ui < uiSize; ui++)
            {
                NiMeshHWInstance* pkInstance = 
                    (NiMeshHWInstance*) kStream.GetObjectFromLinkID();
                if (pkInstance)
                {
                    m_kInstanceNodes.Add(pkInstance);
                    pkInstance->SetMesh(m_pkAffectedMesh);
                }
            }
        }
    }

    SetMeshInstanceCache();
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::PostLinkObject(NiStream& kStream)
{
    NiMeshModifier::PostLinkObject(kStream);

    if (m_bInstanceNodes)
    {
        // Set the region range with the active number of instance nodes.
        NiInstancingUtilities::SetActiveInstanceCount(m_pkAffectedMesh, 
            m_kInstanceNodes.GetSize());
    }
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshModifier::RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::SaveBinary(NiStream& kStream)
{
    NiMeshModifier::SaveBinary(kStream);

    // Save flags
    NiStreamSaveBinary(kStream, m_bInstanceNodes);
    NiStreamSaveBinary(kStream, m_bPerInstanceCulling);
    NiStreamSaveBinary(kStream, m_bStaticBounds);

    kStream.SaveLinkID(m_pkAffectedMesh);

    if (m_bStaticBounds)
        m_kBound.SaveBinary(kStream);

    if (m_bInstanceNodes)
    {
        NiUInt32 uiNumElements = m_kInstanceNodes.GetSize();
        
        NiStreamSaveBinary(kStream, uiNumElements);
        for (NiUInt32 ui = 0; ui < uiNumElements; ui++)
        {
            kStream.SaveLinkID(m_kInstanceNodes.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
bool NiInstancingMeshModifier::IsEqual(NiObject* pkObject)
{
    if (!NiMeshModifier::IsEqual(pkObject))
        return false;

    NiInstancingMeshModifier* pkModifier = (NiInstancingMeshModifier*)pkObject;

    if(pkModifier->m_bInstanceNodes != m_bInstanceNodes)
        return false;

    if(pkModifier->m_bPerInstanceCulling != m_bPerInstanceCulling)
        return false;

    if(pkModifier->m_bStaticBounds != m_bStaticBounds)
        return false;

    NiUInt32 uiNumElements = m_kInstanceNodes.GetSize();

    if(pkModifier->m_kInstanceNodes.GetSize() != uiNumElements)
        return false;

    for (NiUInt32 ui = 0; ui < uiNumElements; ui++)
    {
        if (!pkModifier->m_kInstanceNodes.GetAt(ui)->IsEqual(
            m_kInstanceNodes.GetAt(ui)) )
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiInstancingMeshModifier::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiMeshModifier::GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString(
        NiInstancingMeshModifier::ms_RTTI.GetName()));

    char acBuff[NI_MAX_PATH];

    NiSprintf(acBuff, NI_MAX_PATH, "Use Instance Nodes = %d", 
        m_bInstanceNodes);
    pkStrings->Add(NiGetViewerString(acBuff));

    NiSprintf(acBuff, NI_MAX_PATH, "Per Instance Culling = %d", 
        m_bPerInstanceCulling);
    pkStrings->Add(NiGetViewerString(acBuff));

    NiSprintf(acBuff, NI_MAX_PATH, "Static Bounds = %d", 
        m_bStaticBounds);
        pkStrings->Add(NiGetViewerString(acBuff));

    NiSprintf(acBuff, NI_MAX_PATH, "Max Instances = %d", 
        NiInstancingUtilities::GetMaxInstanceCount(m_pkAffectedMesh));
        pkStrings->Add(NiGetViewerString(acBuff));

    NiSprintf(acBuff, NI_MAX_PATH, "Active Instances = %d", 
        NiInstancingUtilities::GetActiveInstanceCount(m_pkAffectedMesh));
        pkStrings->Add(NiGetViewerString(acBuff));


    if (m_bInstanceNodes)
    {
        for (NiUInt32 ui = 0; ui < m_kInstanceNodes.GetSize(); ui++)
        {
            NiSprintf(acBuff, 64, "Instance node[%d]", ui);
            pkStrings->Add(NiGetViewerString(acBuff, 
                m_kInstanceNodes.GetAt(ui)));

        }
    }

}
//---------------------------------------------------------------------------
