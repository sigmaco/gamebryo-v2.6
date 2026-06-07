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
#include "NiAnimationPCH.h"

#include <NiMesh.h>
#include <NiGeometryConverter.h>
#include <NiSkinningMeshModifier.h>

#include "NiGeomMorpherConverter.h"
#include "NiMorphMeshModifier.h"

//---------------------------------------------------------------------------
// Internal method for generating normal sharing. Note that this method 
// does not handle already existing cloned meshs. Hence, NORMALSHAREINDEX and
// NORMALSHAREGROUP streams will not be shared in the backword compatibility
// path.
//---------------------------------------------------------------------------
bool GenerateNormalSharing(NiMesh* pkMesh,
    const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
    const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex)
{
    // This function finds vertices that have the same location and normal
    // vector. It is used to enable normal recomputation for morphing, so it
    // makes all the assumtions that morphing makes.
    
    // Two streams are added to the mesh. The first stream records the
    // sharing groups. The first entry is 0, indicating that group 0 has no
    // sharing. Each group then follows in linear order, with a count followed
    // by the vertices that are in the group. Then the next count, next group,
    // and so on.
    
    // The second stream has an entry for every vertex, with an index
    // into the first stream. If 0, the vertex is not in a group. Otherwise,
    // the index is the count for the group.
    
    // Get the sub-mesh count which is needed to know how many regions we'll
    // need.
    NiUInt16 uiSubMeshCount = (NiUInt16)(pkMesh->GetSubmeshCount());
    
    // Storage for region information
    NiUInt32* puiRegionOffset = NiAlloc(NiUInt32, uiSubMeshCount);
    
    // Declared to be outside the scope of the locks, below.
    NiUInt32 uiVertexCount = 0;
    NiUInt32* puwIndices = 0;
    NiUInt32* puwGroups = 0;

    // Tracks the size of the groups array.
    NiUInt32 uiGroupArraySize = 0;

    // Scope the locks so that they are deleted before the streams are changed
    {
        NiDataStreamElementLock kPosnLock(pkMesh, kPosnSemantic, uiPosnIndex,
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_READ);
        NiDataStreamElementLock kNormLock(pkMesh, kNormSemantic, uiNormIndex,
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_READ);

        if (!kPosnLock.IsLocked() || !kNormLock.IsLocked())
            return false;
    
        uiVertexCount = kPosnLock.GetDataStream()->GetTotalCount();
        NIASSERT(uiVertexCount == kNormLock.GetDataStream()->GetTotalCount());
        NIASSERT(uiVertexCount < 0x8FFFFFFF);    
    
        // Create the index array.
        puwIndices = NiAlloc(NiUInt32, uiVertexCount);
        memset(puwIndices, 0, sizeof(NiUInt32) * uiVertexCount);

        // Create a temporary sharing array. The worst case is for every vertex
        // to be in a group with one other, with one count for each group,
        // leaving 3/2 size.
        puwGroups = NiAlloc(NiUInt32, uiVertexCount * 3 / 2 + 1);
        
        for (NiUInt16 uiSubMesh = 0; uiSubMesh < uiSubMeshCount; uiSubMesh++)
        {
            // The first group always stores 0, which means no other vertices
            // are shared by vertices in this group. It is the group for
            // vertices that don't share.
            puwGroups[uiGroupArraySize] = 0;
            NiUInt32 uiNoShareGroup = uiGroupArraySize;
            puiRegionOffset[uiSubMesh] = uiGroupArraySize;
            
            uiGroupArraySize++;
            
            NiUInt32 uiPosnCount = kPosnLock.count(uiSubMesh);
            
            NiTStridedRandomAccessIterator<NiPoint3> kPointIter =
                kPosnLock.begin<NiPoint3>(uiSubMesh); 
            NiTStridedRandomAccessIterator<NiPoint3> kNormIter =
                kNormLock.begin<NiPoint3>(uiSubMesh); 

            for (NiUInt32 ui = 0; ui < uiPosnCount; ui++)
            {
                // If we've already found a partner the group index will be
                // non-zero.
                if (puwIndices[ui])
                    continue;
                
                // Default is no-share
                puwIndices[ui] = uiNoShareGroup;

                // If there is to be a group, it will be located at the
                // next open position in the group array.
                NiUInt32 uiCountLocation = uiGroupArraySize;

                NiPoint3 kVert1 = kPointIter[ui];
                NiPoint3 kNorm1 = kNormIter[ui];
                for (unsigned int uj = ui + 1; uj < uiPosnCount; uj++)
                {
                    NiPoint3 kVert2 = kPointIter[uj];
                    NiPoint3 kNorm2 = kNormIter[uj];
                    if (kVert1 == kVert2 && kNorm1 == kNorm2)
                    {
                        // These vertices should share
                        if (!puwIndices[ui])
                        {
                            puwIndices[ui] = uiCountLocation;
                            puwGroups[uiGroupArraySize++] = 1;
                            puwGroups[uiGroupArraySize++] = ui;
                        }
                        puwIndices[uj] = uiCountLocation;
                        puwGroups[uiGroupArraySize++] = uj;
                        puwGroups[uiCountLocation]++;
                    }
                }
            }
        }
    }

    NIASSERT(puwIndices);
    NIASSERT(puwGroups);

    // Remove any existing share index stream.
    NiDataStreamRef* pkShareIndexRef = pkMesh->FindStreamRef(
        NiCommonSemantics::NORMALSHAREINDEX(), 0);
    if (pkShareIndexRef)
    {
        // Delete and replace, because the original stream was
        // generated WRITE_STATIC
        pkMesh->RemoveStreamRef(pkShareIndexRef);
    }

    // Create the share index data stream but do not assign regions
    NiDataStreamPtr spShareIndexStream = 
        NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1, uiVertexCount,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_VERTEX, puwIndices, false, false);
    NIASSERT(spShareIndexStream);
    
    // Assign regions for the share indexes
    NiDataStream::Region kRegion;
    NiUInt32 uiRegionStart = 0;
    for (NiUInt16 uiSubMesh = 0; uiSubMesh < uiSubMeshCount; uiSubMesh++)
    {
        kRegion.SetStartIndex(uiRegionStart);
        NiUInt32 uiSubmeshVertexCount = pkMesh->GetVertexCount(uiSubMesh);
        kRegion.SetRange(uiSubmeshVertexCount);
        uiRegionStart += uiSubmeshVertexCount;
        
        spShareIndexStream->AddRegion(kRegion);
    }
    pkShareIndexRef = pkMesh->AddStreamRef(spShareIndexStream,
        NiCommonSemantics::NORMALSHAREINDEX(), 0); 
    for (NiUInt16 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        pkShareIndexRef->BindRegionToSubmesh(ui, ui);
    }

    // Remove any existing share group stream.
    NiDataStreamRef* pkShareGroupRef = pkMesh->FindStreamRef(
        NiCommonSemantics::NORMALSHAREGROUP(), 0);
    if (pkShareGroupRef)
    {
        // Delete and replace, because the original stream was
        // generated WRITE_STATIC
        pkMesh->RemoveStreamRef(pkShareGroupRef);
    }

    // Create the share group data stream
    NiDataStreamPtr spShareGroupStream = 
        NiDataStream::CreateSingleElementDataStream(
        NiDataStreamElement::F_UINT32_1, uiGroupArraySize,
        NiDataStream::ACCESS_CPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        NiDataStream::USAGE_USER, puwGroups, false, false);
    NIASSERT(spShareGroupStream);
    
    // Assign regions for the share groups
    for (NiUInt16 uiSubMesh = 0; uiSubMesh < uiSubMeshCount; uiSubMesh++)
    {
        kRegion.SetStartIndex(puiRegionOffset[uiSubMesh]);
        NiUInt32 uiEndPosn = uiSubMesh == uiSubMeshCount - 1 ?
            uiGroupArraySize : puiRegionOffset[uiSubMesh + 1];
        kRegion.SetRange(uiEndPosn - puiRegionOffset[uiSubMesh]);
        
        spShareGroupStream->AddRegion(kRegion);
    }
    pkShareGroupRef = pkMesh->AddStreamRef(spShareGroupStream,
        NiCommonSemantics::NORMALSHAREGROUP(), 0); 
    for (NiUInt16 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        pkShareGroupRef->BindRegionToSubmesh(ui, ui);
    }

    NiFree(puiRegionOffset);
    NiFree(puwGroups);
    NiFree(puwIndices);

    return true;
}
//----------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool NiGeomMorpherConverter::m_bAttachModifiers = true;
//---------------------------------------------------------------------------
void NiGeomMorpherConverter::Convert(NiStream& kStream, 
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 10))
        return;

    if (!NiGeometryConverter::GetAutoGeometryConversion())
        return;    

    m_bAttachModifiers = kStream.GetPrepareMeshModifiers();

    for (unsigned int i = 0; i < kTopObjects.GetSize(); i++)
    {
        NiObject* pkObject = kTopObjects.GetAt(i);
        RecurseScene(pkObject);
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherConverter::RecurseScene(NiObject* pkObject)
{
    if (pkObject == NULL)
        return;

    NiMesh* pkMesh = NiDynamicCast(NiMesh, pkObject);
    if (pkMesh)
    {
        // Iterate over all time controllers, converting as necessary
        NiTimeController* pkController = pkMesh->GetControllers();
        NiTimeController* pkNextController = NULL;
        while (pkController != NULL)
        {
            // Hold on to the next pointer, as it might change in the
            // conversion process
            pkNextController = pkController->GetNext();
            NiGeomMorpherControllerPtr spMorph =
                NiDynamicCast(NiGeomMorpherController, pkController);

            if (spMorph)
                ConvertGeomMorpherController(spMorph, pkMesh);
            
            pkController = pkNextController;
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        // If this is an NiNode, we need to convert its children
        NiNode* pkNode = NiVerifyStaticCast(NiNode, pkObject);
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
            RecurseScene(pkNode->GetAt(ui));
    }
}
//---------------------------------------------------------------------------
void NiGeomMorpherConverter::ConvertGeomMorpherController(
    NiGeomMorpherController* pkMorph, NiMesh* pkMesh)
{
    pkMesh->RemoveController(pkMorph);
    
    NiMorphData* pkMorphData = pkMorph->GetMorphData();
    NiUInt32 uiNumTargets = pkMorphData->GetNumTargets();
    NiUInt32 uiNumVertices = pkMorphData->GetNumVertsPerTarget();
    float* pfWeights = NiAlloc(float, uiNumTargets);

    // Create the modifier now, so we can set the skinned flag on it.
    NiMorphMeshModifier* pkModifier = 
        NiNew NiMorphMeshModifier((NiUInt16)uiNumTargets);

    // Determine NiDataStreamRef semantic to morph - POSITION or POSITION_BP.
    NiFixedString kMorphedElement;
    NiSkinningMeshModifier* pkSkinningModifier =
        NiGetModifier(NiSkinningMeshModifier, pkMesh);
    NiSkinPartition::Partition* pkPartitions = 0;
    NiUInt32 uiPartitionCount = 0;

    if (pkSkinningModifier != 0)
    {
        pkModifier->SetSkinned(true);
        pkModifier->SetSWSkinned(pkSkinningModifier->GetSoftwareSkinned());
        kMorphedElement = NiCommonSemantics::POSITION_BP();

#if defined(_WII)
        // We already converted the skinning streams, and we no longer
        // have the mapping from the old vertex order to the new vertex
        // order, so we can't properly convert any new morphing streams.
        NILOG(
            "WARNING: %s> The backwards compatibility path on Wii "
            "cannot properly convert a legacy NiGeometry that is both morphed "
            "and skinned.  Morph results on mesh (pointer: 0x%X, name: %s) will "
            "be incorrect.  Re-export the mesh to remove this error.\n",
            __FUNCTION__,
            pkMesh, 
            (const char*) pkMesh->GetName());
#endif

        // If the target is skinned, we may need to partition the morph targets
        NiGeometry* pkNiGeometry = NiDynamicCast(NiGeometry, 
            pkMorph->NiTimeController::GetTarget());
        if (pkNiGeometry && pkNiGeometry->GetSkinInstance())
        {
            #ifdef NIDEBUG
            if (uiNumVertices != pkNiGeometry->GetVertexCount())
            {
                NILOG("NiGeomMorpherConverter WARNING> Morph vertex count "
                    "mismatch on object \"%s\".", 
                    (const char*)pkNiGeometry->GetName());
            }
            #endif

            NiSkinInstance* pkSkin = pkNiGeometry->GetSkinInstance();

            NiSkinPartition* pkSkinPartition = pkSkin->GetSkinPartition();
            if (pkSkinPartition)
            {
                pkPartitions = pkSkinPartition->GetPartitions();
                NIASSERT(pkPartitions);

                uiPartitionCount = pkSkinPartition->GetPartitionCount();
                NIASSERT(uiPartitionCount > 0);
                uiNumVertices = 0;
                for (NiUInt32 ui = 0; ui < uiPartitionCount; ui++)
                {
                    NiSkinPartition::Partition* pkPartition = 
                        &pkPartitions[ui];
                    uiNumVertices += pkPartition->m_usVertices;
                }
            }
        }
    }
    else
    {
        pkModifier->SetSkinned(false);
        kMorphedElement = NiCommonSemantics::POSITION();
    }
    NiFixedString kMorphTarget = NiMorphMeshModifier::MorphElementSemantic(
        kMorphedElement);

    // Add the target data streams to the mesh
    for (NiUInt32 ui = 0; ui < uiNumTargets; ui++)
    {
        NiMorphData::MorphTarget* pkTargetData = pkMorphData->GetTarget(ui);
        NiDataStreamElement::Format eFormat = NiDataStreamElement::F_FLOAT32_3;

        NiPoint3* pkVerts = pkTargetData->GetTargetVerts();
        NiDataStreamRef* pkStreamRef = pkMesh->AddStream(kMorphTarget, ui,
            eFormat, uiNumVertices,
            NiDataStream::ACCESS_CPU_READ |
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_VERTEX, NULL, 
            false, false);
        NiGeometryConverter::BuildPartitions<NiPoint3>(pkStreamRef, 
            uiNumVertices, pkVerts, pkMorphData->GetNumVertsPerTarget(),
            pkPartitions, uiPartitionCount);
            
        pfWeights[ui] = pkMorph->GetWeight(ui);
    }
    
    // Add the weights stream. Note that it is marked as CLONE_COPY so that
    // independently animated characters will not overwrite each other's 
    // morph weights.
    pkMesh->AddStream(NiCommonSemantics::MORPHWEIGHTS(), 0,
        NiDataStreamElement::F_FLOAT32_1, uiNumTargets,
        NiDataStream::ACCESS_CPU_READ |
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE,
        NiDataStream::USAGE_USER, pfWeights);

    NiFree(pfWeights);

    // Normal sharing will not be addressed in backwards compatibility path
    // to get around issues with cloned-mesh.
    // Prepare for normal updating
    bool bUpdateNormals = pkMorph->GetUpdateNormals();
    if (bUpdateNormals)
    {
        const NiFixedString& kPosnSemantic = pkModifier->GetSkinned() ?
            NiCommonSemantics::POSITION_BP() : NiCommonSemantics::POSITION();
        const NiFixedString& kNormSemantic = pkModifier->GetSkinned() ?
           NiCommonSemantics::NORMAL_BP() : NiCommonSemantics::NORMAL();
        if (!GenerateNormalSharing(pkMesh, kPosnSemantic, 0, kNormSemantic, 0))
        {
            bUpdateNormals = false;
        }
    }
    
    // Set flags
    pkModifier->SetAlwaysUpdate(pkMorph->GetAlwaysUpdate());
    if (pkMorph->GetActive())
        pkModifier->SetNeedsUpdate();
    pkModifier->SetRelativeTargets(pkMorphData->GetRelativeTargets());
    pkModifier->SetUpdateNormals(bUpdateNormals);

    // Specify which semantics it is morphing
    pkModifier->AddMorphedElement(kMorphedElement, 0);
    
    // Add it to the mesh as the first modifier in the chain
    pkModifier->CalculateMorphBound(pkMesh);
    pkMesh->AddModifierAt(0, pkModifier, m_bAttachModifiers);

    // Create a new NiMorphWeightsController
    NiMorphWeightsController* pkWeightsController = 
        NiNew NiMorphWeightsController(uiNumTargets);

    // Attach all the interpolators
    NiUInt16 uiNumInterps = pkMorph->GetInterpolatorCount();
    for (NiUInt16 ui = 0; ui < uiNumInterps; ui++)
    {
        pkWeightsController->SetInterpolator(pkMorph->GetInterpolator(ui), ui);
        pkWeightsController->SetTargetName(ui,
            pkMorphData->GetTarget(ui)->GetName());
    }

    // Copy NiTimeController properties
    pkWeightsController->SetAnimType(pkMorph->GetAnimType());
    pkWeightsController->SetCycleType(pkMorph->GetCycleType());
    pkWeightsController->SetPlayBackwards(pkMorph->GetPlayBackwards());
    pkWeightsController->SetFrequency(pkMorph->GetFrequency());
    pkWeightsController->SetPhase(pkMorph->GetPhase());
    pkWeightsController->SetBeginKeyTime(pkMorph->GetBeginKeyTime());
    pkWeightsController->SetEndKeyTime(pkMorph->GetEndKeyTime());
    pkWeightsController->SetActive(pkMorph->GetActive());

    // Attach it to the target mesh.
    pkWeightsController->SetTarget(pkMesh);
    
    pkWeightsController->ResetTimeExtrema();
}
//---------------------------------------------------------------------------
