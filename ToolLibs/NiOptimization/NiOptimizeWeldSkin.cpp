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

#include <NiOptimize.h>
#include <NiMesh.h>
#include <NiTSimpleArray.h>
#include <NiDataStreamElementLock.h>
#include <NiPoint3.h>
#include <NiTMap.h>

typedef NiTPrimitiveSet<NiMesh*> NiMeshImplSet;

//---------------------------------------------------------------------------
class Interaction : public NiMemObject
{
public:
    const NiAVObject* m_pkBone;
    float m_fWeight;
};

//---------------------------------------------------------------------------
class SkinVert : public NiMemObject
{
public:
    NiPoint3 m_kPos;
    NiTObjectSet<Interaction> m_kBones;
    bool m_bInitialized;

    SkinVert();
};

//---------------------------------------------------------------------------
SkinVert::SkinVert()
{
    m_bInitialized = false;
}

//---------------------------------------------------------------------------
static int FloatCompare(const float* pf0, const float* pf1,
    NiUInt32 uiNum)
{
    for (NiUInt32 i = 0; i < uiNum; i++)
    {
        if (pf0[i] < pf1[i])
            return -1;
        if (pf0[i] > pf1[i])
            return 1;
    }

    return 0;
}

//---------------------------------------------------------------------------
static int ComparePoint3(const void* pv0, const void* pv1)
{
    const NiPoint3* pkV0 = (const NiPoint3*) pv0;
    const NiPoint3* pkV1 = (const NiPoint3*) pv1;

    return FloatCompare(&pkV0->x, &pkV1->x, 3);
}

//---------------------------------------------------------------------------
static int CompareObjectTransform(const NiAVObject* pk0,
    const NiAVObject* pk1)
{
    const NiTransform& kW0 = pk0->GetWorldTransform();
    const NiTransform& kW1 = pk1->GetWorldTransform();

    int iCmp;

    for (NiUInt32 r = 0; r < 3; r++)
    {
        NiPoint3 kCol0, kCol1;

        kW0.m_Rotate.GetCol(r, kCol0);
        kW1.m_Rotate.GetCol(r, kCol1);

        iCmp = FloatCompare(&kCol0.x, &kCol1.x, 3);
        if (iCmp != 0)
            return iCmp;
    }

    iCmp = FloatCompare(&kW0.m_Translate.x, &kW1.m_Translate.x, 3);
    if (iCmp != 0)
        return iCmp;

    return FloatCompare(&kW0.m_fScale, &kW1.m_fScale, 1);
}
//---------------------------------------------------------------------------
static int CompareSkinsOnMesh(const NiMesh* pk0, const NiMesh* pk1)
{
    int iCmp = CompareObjectTransform(pk0, pk1);

    if (iCmp != 0)
        return iCmp;

    NiSkinningMeshModifier* pkSkinMod0 = NiGetModifier(NiSkinningMeshModifier,
        pk0);
    NIASSERT(pkSkinMod0);
    NiSkinningMeshModifier* pkSkinMod1 = NiGetModifier(NiSkinningMeshModifier,
        pk1);
    NIASSERT(pkSkinMod1);

    NiTPointerMap<const NiAVObject*, bool> kObjMap;

    NiUInt32 uiBoneCnt0 = pkSkinMod0->GetBoneCount();
    NiAVObject*const* ppkBones0 = pkSkinMod0->GetBones();

    // Iterate over 1st and store in map...
    for(NiUInt32 uiBone=0; uiBone<uiBoneCnt0; uiBone++)
    {
        const NiAVObject* pkObj = ppkBones0[uiBone];
        kObjMap.SetAt(pkObj, true);
    }

    NiUInt32 uiBoneCnt1 = pkSkinMod0->GetBoneCount();
    NiAVObject*const* ppkBones1 = pkSkinMod1->GetBones();

    // Iterate over 2nd and look up in map...
    for(NiUInt32 uiBone=0; uiBone<uiBoneCnt1; uiBone++)
    {
        const NiAVObject* pkObj = ppkBones1[uiBone];
        bool bTmp;
        if (kObjMap.GetAt(pkObj, bTmp) == true)
        {
            // Shared bone found.
            kObjMap.RemoveAll();
            return 0;
        }
    }

    kObjMap.RemoveAll();

    if (uiBoneCnt0 < uiBoneCnt1)
        return -1;

    return 1;
}
//---------------------------------------------------------------------------
static int CompareSkinsOnMeshV(const void* pv0, const void* pv1)
{
    // Like the above function, but takes void arguments so it can be used
    // by qsort.

    const NiMesh** ppk0 = (const NiMesh**) pv0;
    const NiMesh** ppk1 = (const NiMesh**) pv1;
    return CompareSkinsOnMesh(*ppk0, *ppk1);
}
//---------------------------------------------------------------------------
static int CompareSkinVert(const void* pvKey, const void* pvDatum)
{
    const NiPoint3* pkKey = (const NiPoint3*) pvKey;
    const SkinVert* pkDatum = (const SkinVert*) pvDatum;

    return FloatCompare(&pkKey->x, &pkDatum->m_kPos.x, 3);
}

//---------------------------------------------------------------------------
static void Weld(NiMesh** ppkMesh, NiUInt32 uiObjects)
{
    // Count verts.
    NiUInt32 uiVertices = 0;

    for (NiUInt32 c = 0; c < uiObjects; c++)
    {
        NiMesh* pkMesh = ppkMesh[c];
        NiSkinningMeshModifier* pkSkinMod = NiGetModifier(
            NiSkinningMeshModifier, pkMesh);
        if (!pkSkinMod)
        {
            NILOG("Could not find skinning mesh modifier on mesh\n");
            return;
        }

        NiUInt32 uiSubmeshes = pkMesh->GetSubmeshCount();
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshes; uiSubmesh++)
        {
            uiVertices += pkMesh->GetVertexCount(uiSubmesh);
        }
    }

    // Allocate array.
    NiPoint3* pkVerts = NiNew NiPoint3[uiVertices];
    NiPoint3* p = pkVerts;

    // Gather up every vertex in every submesh of every mesh 
    // (ie every vertex position in the mesh array)
    for (NiUInt32 c = 0; c < uiObjects; c++)
    {
        NiMesh* pkMesh = ppkMesh[c];

        NiDataStreamRef* pkPosSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::POSITION_BP());

        if (!pkPosSRef)
        {
            NILOG("Could not find POSITION_BP on mesh\n");
            return;
        }
        if (pkPosSRef->GetElementDescCount() != 1)
        {
            NILOG("An interleaved mesh can not be welded.\n");
            return;
        }
        if (pkPosSRef->GetElementDescAt(0).GetFormat() != 
            NiDataStreamElement::F_FLOAT32_3)
        {
            NILOG("POSITION_BP on mesh is in the wrong format.\n");
            return;
        }

        NiDataStream* pkPosDS = pkPosSRef->GetDataStream();

        NiUInt32 uiSubmeshes = pkMesh->GetSubmeshCount();

        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshes; uiSubmesh++)
        {
            // Lock the relevant data streams
            NiPoint3* pkPos = (NiPoint3*)pkPosDS->LockRegion(
                pkPosSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);

            NiUInt32 uiChildVerts = pkMesh->GetVertexCount(uiSubmesh);
            for (NiUInt32 v = 0; v < uiChildVerts; v++)
            {
                p[v] = pkPos[v];
            }

            pkPosDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            p = &(p[uiChildVerts]);
        }

    }

    // Sort all of the vertex positions. This sorting will allow us
    // to perform comparisons very quickly.
    qsort(pkVerts, uiVertices, sizeof(pkVerts[0]), ComparePoint3);

    // Count the unique vertices in the overall vertex array.
    NiUInt32 uiUniqueVerts = 1;
    for (NiUInt32 v = 1; v < uiVertices; v++)
    {
        if (FloatCompare(&pkVerts[v - 1].x, &pkVerts[v].x, 3) != 0)
        {
            uiUniqueVerts++;
        }
    }

    // Allocate array of unique skinned vertices.
    SkinVert* pkSkinVerts = NiNew SkinVert[uiUniqueVerts];

    // Gather unique vertices into the SkinVert data structure
    pkSkinVerts[0].m_kPos = pkVerts[0];
    uiUniqueVerts = 1;
    for (NiUInt32 v = 1; v < uiVertices; v++)
    {
        if (FloatCompare(&pkVerts[v - 1].x, &pkVerts[v].x, 3) != 0)
        {
            pkSkinVerts[uiUniqueVerts++].m_kPos = pkVerts[v];
        }
    }

    // We no longer need the full vertex position array
    NiDelete[] pkVerts;

    // For each object a vert appears in, eliminate any bone's influence if
    // that bone does not influence every instance of the vertex.
    for (NiUInt32 c = 0; c < uiObjects; c++)
    {
        NiMesh* pkMesh = ppkMesh[c];
        NiSkinningMeshModifier* pkSkinMod = 
            NiGetModifier(NiSkinningMeshModifier, pkMesh);
        NiUInt32 uiSubmeshes = pkMesh->GetSubmeshCount();
        NiAVObject*const* ppkBones = pkSkinMod->GetBones();

        // Get the stream refs that we will be using.
        NiDataStreamRef* pkPosSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::POSITION_BP());
        NiDataStreamRef* pkBWSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BLENDWEIGHT());
        NiDataStreamRef* pkBISRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BLENDINDICES());
        // This stream ref may not be necessary if we aren't doing hardware
        NiDataStreamRef* pkBonesSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BONE_PALETTE());

        // Assert that the stream refs are valid
        if (!pkPosSRef)
        {
            NILOG("Could not find POSITION_BP on mesh\n");
            continue;
        }
        if (!pkBWSRef)
        {
            NILOG("Could not find BLENDWEIGHT on mesh\n");
            continue;
        }
        if (!pkBISRef)
        {
            NILOG("Could not find BLENDINDICES on mesh\n");
            continue;
        }

        // Assert that the relevant streams only have one element
        if (pkPosSRef->GetElementDescCount() != 1)
        {
            NILOG("An interleaved mesh can not be welded.\n");
            continue;
        }
        if (pkBWSRef->GetElementDescCount() != 1)
        {
            NILOG("An interleaved mesh can not be welded.\n");
            continue;
        }
        if (pkBISRef->GetElementDescCount() != 1)
        {
            NILOG("An interleaved mesh can not be welded.\n");
            continue;
        }

        // Assert that the relevant streams are formatted correctly
        if (pkPosSRef->GetElementDescAt(0).GetFormat() != 
            NiDataStreamElement::F_FLOAT32_3)
        {
            NILOG("POSITION_BP on mesh is in the wrong format.\n");
            continue;
        }
        if (pkBWSRef->GetElementDescAt(0).GetFormat() != 
            NiDataStreamElement::F_FLOAT32_4)
        {
            NILOG("BLENDWEIGHT on mesh is in the wrong format.\n");
            continue;
        }
        if (pkBISRef->GetElementDescAt(0).GetFormat() != 
            NiDataStreamElement::F_INT16_4)
        {
            NILOG("BLENDINDICES on mesh is in the wrong format.\n");
            continue;
        }
        if (pkBonesSRef && (pkBonesSRef->GetElementDescAt(0).GetFormat() !=
            NiDataStreamElement::F_UINT16_1))
        {
            NILOG("BONE_PALETTE on mesh is in the wrong format.\n");
            continue;
        }

        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshes; uiSubmesh++)
        {
            NiDataStream* pkPosDS = pkPosSRef->GetDataStream();
            NiDataStream* pkBWDS = pkBWSRef->GetDataStream();
            NiDataStream* pkBIDS = pkBISRef->GetDataStream();
            NiDataStream* pkBonesDS = NULL;
            if (pkBonesSRef)
                pkBonesDS = pkBonesSRef->GetDataStream();

            NIASSERT(sizeof(NiTSimpleArray<float, 4>) == sizeof(float) * 4);
            NIASSERT(sizeof(NiTSimpleArray<NiUInt32, 4>) == 
                sizeof(NiUInt32) * 4);

            // Lock the relevant data streams
            NiPoint3* pkPos = (NiPoint3*)pkPosDS->LockRegion(
                pkPosSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);
            NiTSimpleArray<float, 4>* pkBWs = 
                (NiTSimpleArray<float, 4>*)pkBWDS->LockRegion(
                pkBWSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);
            NiTSimpleArray<NiInt16, 4>* pkBIs = 
                (NiTSimpleArray<NiInt16, 4>*)pkBIDS->LockRegion(
                pkBISRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);
            NiUInt16* puiBones = NULL;
            if (pkBonesDS)
            {
                puiBones = (NiUInt16*)pkBonesDS->LockRegion(
                    pkBonesSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                    NiDataStream::LOCK_TOOL_READ);
            }

            NiUInt32 uiBonesPerVertex = (NiUInt32)pkBIs[0].Count();

            for (NiUInt32 v = 0; v < pkMesh->GetVertexCount(uiSubmesh); v++)
            {
                SkinVert* pkSearchVert = (SkinVert*) bsearch(
                    &pkPos[v], pkSkinVerts, uiUniqueVerts,
                    sizeof(SkinVert), CompareSkinVert);

                NIASSERT(pkSearchVert != NULL);
                if (!pkSearchVert)
                    continue;

                if (!pkSearchVert->m_bInitialized)
                {
                    NiUInt32 b;
                    float fWeightTallyPerVertex = 0.0f;
                    for (b = 0; b < uiBonesPerVertex; b++)
                    {
                        Interaction kInteraction;

                        float fWeight = pkBWs[v][b];
                        
                        if (fWeight > 0.0f)
                        {
                            // Add in the weights
                            fWeightTallyPerVertex += fWeight;
                            kInteraction.m_fWeight = fWeight;

                            // The blend index is relative to this submesh
                            NiUInt32 uiBone = pkBIs[v][b];

                            // Remap into the larger bone array if it exists
                            if (puiBones)
                                uiBone = (NiUInt32) puiBones[uiBone];

                            // Get the NiAVObject for the bone
                            kInteraction.m_pkBone = ppkBones[uiBone];

                            // Register the interaction with the unique vertex
                            pkSearchVert->m_kBones.Add(kInteraction);
                        }
                    }

                    pkSearchVert->m_bInitialized = true;
                }
                else
                {
                    // Intersect bone sets
                    for (NiUInt32 bv = 0; 
                        bv < pkSearchVert->m_kBones.GetSize(); )
                    {
                        const NiAVObject* pkBone =
                            pkSearchVert->m_kBones.GetAt(bv).m_pkBone;

                        float fWeightTallyPerVertex = 0.0f;
                        NiUInt32 b;
                        for (b = 0; b < uiBonesPerVertex; b++)
                        {
                            float fWeight = pkBWs[v][b];

                            if (fWeight > 0.0f)
                            {
                                fWeightTallyPerVertex += fWeight;

                                // The blend index is relative to this submesh
                                NiUInt32 uiBone = pkBIs[v][b];

                                // Remap into the larger bone array if we can
                                if (puiBones)
                                    uiBone = (NiUInt32) puiBones[uiBone];

                                // The bone is already known
                                if (ppkBones[uiBone] == pkBone)
                                    break;
                            }
                        }

                        if (b == uiBonesPerVertex)
                        {
                            // Bone not found.
                            // Remove it from pkSearchVert->m_kBones.
                            pkSearchVert->m_kBones.RemoveAt(bv);
                        }
                        else
                        {
                            bv++;
                        }
                    }
                }
            }

            pkPosDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            pkBWDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            pkBIDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            if (pkBonesDS)
                pkBonesDS->Unlock(NiDataStream::LOCK_TOOL_READ);
        }
    }

    // Unitize weights.
    for (NiUInt32 v = 0; v < uiUniqueVerts; v++)
    {
        float fWeightSum = 0.0f;
        NiUInt32 b;

        for (b = 0; b < pkSkinVerts[v].m_kBones.GetSize(); b++)
        {
            fWeightSum += pkSkinVerts[v].m_kBones.GetAt(b).m_fWeight;
        }

        for (b = 0; b < pkSkinVerts[v].m_kBones.GetSize(); b++)
        {
            pkSkinVerts[v].m_kBones.GetAt(b).m_fWeight /= fWeightSum;
        }
    }

    // Assign new weights.
    for (NiUInt32 c = 0; c < uiObjects; c++)
    {
        NiMesh* pkMesh = ppkMesh[c];
        NiSkinningMeshModifier* pkSkinMod = 
            NiGetModifier(NiSkinningMeshModifier, pkMesh);
        NiUInt32 uiSubmeshes = pkMesh->GetSubmeshCount();
        NiAVObject*const* ppkBones = pkSkinMod->GetBones();

        // Get the stream refs that we will be using.
        NiDataStreamRef* pkPosSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::POSITION_BP());
        NiDataStreamRef* pkBWSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BLENDWEIGHT());
        NiDataStreamRef* pkBISRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BLENDINDICES());
        // This stream ref may not exist for software skinning
        NiDataStreamRef* pkBonesSRef = pkMesh->FindStreamRef(
            NiCommonSemantics::BONE_PALETTE());

        // The assertions in the prior for loop do not need to be repeated.

        // Iterate over the submeshes to reset weights
        for (NiUInt32 uiSubmesh = 0; uiSubmesh < uiSubmeshes; uiSubmesh++)
        {
            NiDataStream* pkPosDS = pkPosSRef->GetDataStream();
            NiDataStream* pkBWDS = pkBWSRef->GetDataStream();
            NiDataStream* pkBIDS = pkBISRef->GetDataStream();
            NiDataStream* pkBonesDS = NULL;
            if (pkBonesSRef)
                pkBonesDS = pkBonesSRef->GetDataStream();

            NIASSERT(sizeof(NiTSimpleArray<float, 4>) == sizeof(float) * 4);
            NIASSERT(sizeof(NiTSimpleArray<NiInt16, 4>) == 
                sizeof(NiInt16) * 4);

            // Lock the relevant data streams
            NiPoint3* pkPos = (NiPoint3*)pkPosDS->LockRegion(
                pkPosSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);
            NiTSimpleArray<float, 4>* pkBWs = 
                (NiTSimpleArray<float, 4>*)pkBWDS->LockRegion(
                pkBWSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);
            NiTSimpleArray<NiInt16, 4>* pkBIs = 
                (NiTSimpleArray<NiInt16, 4>*)pkBIDS->LockRegion(
                pkBISRef->GetRegionIndexForSubmesh(uiSubmesh), 
                NiDataStream::LOCK_TOOL_READ);

            NiUInt32 uiBonesPerVertex = (NiUInt32)pkBIs[0].Count();

            // Create a hash table of the NiAVObject bone to local bone index.
            NiTMap<const NiAVObject*, NiUInt32> kAVObjectToSubBoneIdxMap;
            if (pkBonesSRef)
            {
                // if we are partitioned, we need to worry about regions
                NiUInt16* puiBones = (NiUInt16*)pkBonesDS->LockRegion(
                    pkBonesSRef->GetRegionIndexForSubmesh(uiSubmesh), 
                    NiDataStream::LOCK_TOOL_READ);
                NiDataStream::Region& kBonesRegion = 
                    pkBonesSRef->GetRegionForSubmesh(uiSubmesh);

                for (NiUInt32 ui = 0; ui < kBonesRegion.GetRange(); ui++)
                {
                    NiAVObject* pkBone = ppkBones[puiBones[ui]];
                    kAVObjectToSubBoneIdxMap.SetAt(pkBone, ui);
                }
            }
            else
            {
                NiUInt32 uiBoneCount = pkSkinMod->GetBoneCount();
                // if we aren't partitioned, we don't need the remapping array
                for (NiUInt32 ui = 0; ui < uiBoneCount; ui++)
                {
                    kAVObjectToSubBoneIdxMap.SetAt(ppkBones[ui], ui);
                }
            }

            // Iterate over all vertices, setting their weights and indices as 
            // appropriate
            for (NiUInt32 v = 0; v < pkMesh->GetVertexCount(uiSubmesh); v++)
            {
                SkinVert* pkSearchVert = (SkinVert*) bsearch(
                    &pkPos[v], pkSkinVerts, uiUniqueVerts,
                    sizeof(SkinVert), CompareSkinVert);

                NIASSERT(pkSearchVert != NULL);
                if (!pkSearchVert)
                    continue;
                NiTObjectSet<Interaction>& kBones = pkSearchVert->m_kBones;

                if (kBones.GetSize() > 0)
                {
                    NIASSERT(kBones.GetSize() <= uiBonesPerVertex);

                    // Copy over the data for the bone interactions
                    NiUInt32 uiSB = 0;
                    for (; uiSB < kBones.GetSize(); uiSB++)
                    {
                        Interaction& kInteraction = kBones.GetAt(uiSB);
                        NiUInt32 uiBone = 0;
                        bool bSuccess = 
                            kAVObjectToSubBoneIdxMap.GetAt(
                            kInteraction.m_pkBone, uiBone);
                        NI_UNUSED_ARG(bSuccess);
                        NIASSERT(bSuccess);

                        pkBWs[v][uiSB] = kInteraction.m_fWeight;
                        pkBIs[v][uiSB] = (NiInt16)uiBone;
                    }

                    // Fill in the remainder of data with zeroes
                    for (; uiSB < uiBonesPerVertex; uiSB++)
                    {
                        pkBWs[v][uiSB] = 0.0f;
                        pkBIs[v][uiSB] = 0;
                    }
                }
                else
                {
                    // To be safe, zero out all weights
                    for (NiUInt32 b = 0; b < uiBonesPerVertex; b++)
                    {
                        pkBWs[v][b] = 0.0f;
                    }

                    // To be safe, zero out all indices
                    for (NiUInt32 b = 0; b < uiBonesPerVertex; b++)
                    {
                        pkBIs[v][b] = 0;
                    }
                }
            }

            // unlock our data streams
            pkPosDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            pkBWDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            pkBIDS->Unlock(NiDataStream::LOCK_TOOL_READ);
            if (pkBonesDS)
                pkBonesDS->Unlock(NiDataStream::LOCK_TOOL_READ);
        }
    }

    NiDelete[] pkSkinVerts;
}

//---------------------------------------------------------------------------
static void GatherSkins(NiAVObject* pkObject,
    NiMeshImplSet& kSkins)
{
    // Add all mesh objects that have skin partitions to kSkins.

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;

        for (NiUInt32 i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                GatherSkins(pkChild, kSkins);
        }
    }
    else if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*) pkObject;
        NiSkinningMeshModifier* pkSkinMod = NiGetModifier(
            NiSkinningMeshModifier, pkMesh);
        if (pkSkinMod != NULL && !NiOptimize::CheckForExtraDataTags(pkObject))
        {
            kSkins.Add(pkMesh);
       }
    }
}

//---------------------------------------------------------------------------
void NiOptimize::WeldSkin(NiAVObject* pkScene)
{        
    // Only objects with identical world transforms are welded. Call Update
    // to make sure world transforms have been initialized.
    pkScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkScene);

    NiMeshImplSet kSkins;
    
    GatherSkins(pkScene, kSkins);
    
    // Sort objects based on world transform and pass groups of objects
    // with the same world transform to Weld.

    NiUInt32 uiSkins = kSkins.GetSize();
    NiMesh** ppkBase = kSkins.GetBase();
    qsort(ppkBase, uiSkins, sizeof(NiMesh*), CompareSkinsOnMeshV);
    
    NiUInt32 uiProcessed, uiAligned;
    
    for (uiProcessed = 0; uiProcessed < uiSkins; uiProcessed += uiAligned)
    {
        const NiAVObject* pkRef = kSkins.GetAt(uiProcessed);
        
        NiUInt32 j = uiProcessed + 1;
        for (; j < uiSkins && CompareSkinsOnMesh(
            (NiMesh*)pkRef, (NiMesh*)kSkins.GetAt(j)) == 0; j++)
        {
        }
        
        uiAligned = j - uiProcessed;
        
        if (uiAligned > 1)
        {
            Weld(&ppkBase[uiProcessed], uiAligned);
        }
    }
}
//---------------------------------------------------------------------------
