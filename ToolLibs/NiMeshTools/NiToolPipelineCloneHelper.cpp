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

#include "NiToolPipelineCloneHelper.h"
#include <NiNode.h>

NiImplementRootRTTI(NiToolPipelineCloneHelper::CloneSet);

typedef NiToolPipelineCloneHelper::CloneSetPtr CloneSetPtr;
typedef NiTPrimitiveArray<NiMesh*> MeshPointerArray;
typedef NiTMap<NiMesh*, NiUInt32> MeshPointerToUInt32Map;

//---------------------------------------------------------------------------
// Helper functions
//---------------------------------------------------------------------------
bool IsLessThan(MeshPointerToUInt32Map& kMeshToIDMap,
    NiMesh* pkLeft, NiMesh* pkRight)
{
    NiUInt32 uiLeftID = 0;
    NiUInt32 uiRightID = 0;

    NIVERIFY(kMeshToIDMap.GetAt(pkLeft, uiLeftID));
    NIVERIFY(kMeshToIDMap.GetAt(pkRight, uiRightID));

    return (uiLeftID < uiRightID);
}
//---------------------------------------------------------------------------
bool IsLessThan(MeshPointerToUInt32Map& kMeshToIDMap,
    const CloneSetPtr& spLeft, const CloneSetPtr& spRight)
{
    MeshPointerArray& akLeft = spLeft->GetMeshSet();
    MeshPointerArray& akRight = spRight->GetMeshSet();

    NiUInt32 uiLeftSize = akLeft.GetSize();
    NiUInt32 uiRightSize = akRight.GetSize();

    for (NiUInt32 ui=0; ui<uiLeftSize; ui++)
    {
        if (ui >= uiRightSize)
            return false;

        NiMesh* pkLeftMesh = akLeft.GetAt(ui);
        NiUInt32 uiLeftID;
        NIVERIFY(kMeshToIDMap.GetAt(pkLeftMesh, uiLeftID));

        NiMesh* pkRightMesh = akRight.GetAt(ui);
        NiUInt32 uiRightID;
        NIVERIFY(kMeshToIDMap.GetAt(pkRightMesh, uiRightID));

        if (uiLeftID < uiRightID)
            return true;
        else if (uiRightID < uiLeftID)
            return false;
    }

    if (uiLeftSize == uiRightSize)
        return false; // because is equal to, not less than

    return true;
}
//---------------------------------------------------------------------------
bool IsEqualTo(MeshPointerToUInt32Map& kMeshToIDMap,
    const CloneSetPtr& spLeft, const CloneSetPtr& spRight)
{
    if (!spLeft && !spRight)
        return true;
    else if (spLeft && !spRight)
        return false;
    else if (!spLeft && spRight)
        return false;

    MeshPointerArray& akLeft = spLeft->GetMeshSet();
    MeshPointerArray& akRight = spRight->GetMeshSet();
    NiUInt32 uiLeftSize = akLeft.GetSize();
    NiUInt32 uiRightSize = akRight.GetSize();

    if (uiLeftSize != uiRightSize)
        return false;

    for (NiUInt32 ui=0; ui<uiLeftSize; ui++)
    {
        NiMesh* pkLeftMesh = akLeft.GetAt(ui);
        NiUInt32 uiLeftID;
        NIVERIFY(kMeshToIDMap.GetAt(pkLeftMesh, uiLeftID));

        NiMesh* pkRightMesh = akRight.GetAt(ui);
        NiUInt32 uiRightID;
        NIVERIFY(kMeshToIDMap.GetAt(pkRightMesh, uiRightID));

        if (uiLeftID != uiRightID)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
template <class C, class T>
void SwapPointers(C* pkArr, NiInt32 iAtIndex1, NiInt32 iAtIndex2)
{
    T pkTmp = pkArr->GetAt(iAtIndex1);
    pkArr->SetAt(iAtIndex1, pkArr->GetAt(iAtIndex2));
    pkArr->SetAt(iAtIndex2, pkTmp);
}
//---------------------------------------------------------------------------
template <class C, class T>
void QuickSortPointers(MeshPointerToUInt32Map& kMeshToIDMap,
    C* pkArr, NiInt32 iLeft, NiInt32 iRight)
{
    if (iRight <= iLeft) 
        return;

    NiInt32 iPartition = PartitionPointers<C,T>(kMeshToIDMap, pkArr,
        iLeft, iRight);
    QuickSortPointers<C, T>(kMeshToIDMap, pkArr, iLeft, iPartition - 1);
    QuickSortPointers<C, T>(kMeshToIDMap, pkArr, iPartition + 1,
        iRight);
}
//---------------------------------------------------------------------------
template <class C, class T>
NiInt32 PartitionPointers(MeshPointerToUInt32Map& kMeshToIDMap,
    C* pkArr, NiInt32 iLeft, NiInt32 iRight)
{
    NiInt32 i = iLeft - 1;
    NiInt32 j = iRight;
    T spElement = pkArr->GetAt(iRight);

    for (;;)
    {
        while (IsLessThan(kMeshToIDMap, pkArr->GetAt(++i), spElement));
        while (IsLessThan(kMeshToIDMap, spElement, pkArr->GetAt(--j)))
        {
            if (j == iLeft)
                break;
        }

        if (i >= j) 
            break;
        SwapPointers<C, T>(pkArr, i, j);
    }
    SwapPointers<C, T>(pkArr, i, iRight);

    return i;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiToolPipelineCloneHelper::CloneSet
//---------------------------------------------------------------------------
NiUInt32 NiToolPipelineCloneHelper::CloneSet::GetSize()
{
    return m_akMeshSet.GetSize(); 
}
//---------------------------------------------------------------------------
NiUInt32 NiToolPipelineCloneHelper::CloneSet::Add(NiMesh* pkMesh)
{
    return m_akMeshSet.Add(pkMesh); 
}
//---------------------------------------------------------------------------
NiMesh* NiToolPipelineCloneHelper::CloneSet::GetAt(NiUInt32 uiItem)
{
    return m_akMeshSet.GetAt(uiItem); 
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::CloneSet::Sort(
    NiTMap<NiMesh*, NiUInt32>& kMeshToIDMap)
{
    NiUInt32 uiSize = m_akMeshSet.GetSize();
    QuickSortPointers<MeshPointerArray, NiMesh*>(kMeshToIDMap, &m_akMeshSet, 0,
        uiSize-1);
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::CloneSet::Contains(NiMesh* pkMesh)
{
    NiUInt32 uiMeshCnt = GetSize();
    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        if (pkMesh == GetAt(ui))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiToolPipelineCloneHelper
//---------------------------------------------------------------------------
NiToolPipelineCloneHelper::NiToolPipelineCloneHelper()
{
    ClearMaps();
}
//---------------------------------------------------------------------------
NiToolPipelineCloneHelper::NiToolPipelineCloneHelper(NiAVObject* pkObject)
{
    InitializeCloneSetMaps(pkObject);
}
//---------------------------------------------------------------------------
NiToolPipelineCloneHelper::NiToolPipelineCloneHelper(
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    InitializeCloneSetMaps(kTopObjects);
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::ClearMaps()
{
    // Clear the clone set maps
    m_kDSToCloneSet.RemoveAll();
    m_kCompleteMeshSet.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::GetCloneSet(NiDataStream* pkDataStream,
    CloneSetPtr& spCloneSet)
{
    if (m_kDSToCloneSet.GetAt(pkDataStream, spCloneSet))
        return true;

    return false;
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::InitializeCloneSetMaps(
    NiTLargeObjectArray<NiObjectPtr>& kTopObjects)
{
    ClearMaps();

    // Traverse the scene and build the the clone sets.
    // This code assumes that if a datastream is shared between meshs, then
    // every datastream used is shared by the same set of mesh.
    for (unsigned int ui = 0; ui < kTopObjects.GetSize(); ui++)
    {
        NiAVObject* pkObject = NiDynamicCast(NiAVObject,
            kTopObjects.GetAt(ui));

        if (!pkObject)
            continue;

        RecursiveConstructMeshSet(pkObject);
    }

    InitializeCloneSetMaps();
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::InitializeCloneSetMaps(NiAVObject* pkObject)
{
    ClearMaps();
    RecursiveConstructMeshSet(pkObject);
    InitializeCloneSetMaps();
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::InitializeCloneSetMaps()
{
    // Create a mapping from DataStreams to CloneSets, ie, set of mesh that
    // share that data-stream. Note that at this stage, we don't care if there
    // are identical CloneSets created across different data-streams because
    // we'll collapse them later.
    NiTPointerMap<NiDataStream*, CloneSetPtr> kDSToCloneSet;
    NiUInt32 uiMeshCnt = m_kCompleteMeshSet.GetSize();
    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        NiMesh* pkMesh = m_kCompleteMeshSet.GetAt(ui);
        NIASSERT(pkMesh);

        NiUInt32 uiStreamRefCnt = pkMesh->GetStreamRefCount();
        if (uiStreamRefCnt < 1)
            continue;

        for (NiUInt32 uiStream=0; uiStream<uiStreamRefCnt; uiStream++)
        {
            NiDataStreamRef* pkSR = pkMesh->GetStreamRefAt(uiStream);
            NiDataStream* pkDS = pkSR->GetDataStream();
            NIASSERT(pkDS);

            CloneSetPtr kCloneSet;
            if (kDSToCloneSet.GetAt(pkDS, kCloneSet))
            {
                // We assume that a mesh won't be added twice. That would only
                // occur if the ds appeared twice on the same mesh which 
                // would break our assumptions any way.
                kCloneSet->Add(pkMesh);
                continue;
            }

            // No cloneset exist for this datastream or mesh, so add it.
            CloneSet* pkCloneSet = NiNew CloneSet();
            pkCloneSet->Add(pkMesh);

            kDSToCloneSet.SetAt(pkDS, pkCloneSet);
        }
    }

    // Now that we know how an individual datastream is shared by different
    // meshs, we need to collapse clonesets that are equilvalent. We begin by
    // creating a mapping of mesh pointer to ID assignments. We do not use the
    // mesh pointers as ID's because this would cause non-deterministic code
    // that would be (a) difficult to debug and (b) could cause different
    // export behaviors when merging of profiles are performed.
    MeshPointerToUInt32Map kMeshToIDMap;
    NiUInt32 uiID = 0;
    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        NiMesh* pkMesh = m_kCompleteMeshSet.GetAt(ui);
        NIASSERT(pkMesh);
        kMeshToIDMap.SetAt(pkMesh, uiID++);
    }

    // Now that we have a way of identifying a mesh by ID, we sort the 
    // contents of the clonesets (the mesh) by the assigned ID.
    NiTMapIterator kIter = kDSToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCloneSet;
        kDSToCloneSet.GetNext(kIter, pkDS, spCloneSet);
        spCloneSet->Sort(kMeshToIDMap);
    }

    // As the clonesets have been internally sorted, we now sort clonesets
    // relative to each other in an array. This will allow us to remove
    // duplicates.
    NiTObjectArray<CloneSetPtr> akCloneSetArray;
    kIter = kDSToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCS;
        kDSToCloneSet.GetNext(kIter, pkDS, spCS);
        akCloneSetArray.Add(spCS);
    }
    QuickSortPointers<NiTObjectArray<CloneSetPtr>, CloneSetPtr>(kMeshToIDMap,
        &akCloneSetArray, 0, akCloneSetArray.GetSize()-1);

    // Before we can remove duplicates, we need to determine the new mappings
    // from old cloneset pointers to the new ones. We take advantage of the
    // fact that the clone set array is sorted and only add the first unique
    // clonesets encountered. We also making a mapping from old cloneset
    // pointers to the one we intend to keep.
    NiTPointerMap<CloneSet*, CloneSet*> m_kNewCloneSetMapping;
    NiUInt32 uiCloneSets = akCloneSetArray.GetSize();
    CloneSet* pkPrev = NULL;
    for(NiUInt32 ui=0; ui<uiCloneSets; ui++)
    {
        CloneSetPtr spCS = akCloneSetArray.GetAt(ui);

        if (IsEqualTo(kMeshToIDMap, spCS, pkPrev))
        {
            m_kNewCloneSetMapping.SetAt(spCS, pkPrev);
        }
        else
        {
            pkPrev = spCS;
            m_kNewCloneSetMapping.SetAt(spCS, spCS);
        }
    }

    // Finally, we collapse the datastream to cloneset mapping by making 
    // datastreams that point to an equalvalent cloneset, point to the same
    // cloneset.
    kIter = kDSToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCS;
        kDSToCloneSet.GetNext(kIter, pkDS, spCS);

        CloneSet* pkNewCS;
        NIVERIFY(m_kNewCloneSetMapping.GetAt(spCS, pkNewCS));

        m_kDSToCloneSet.SetAt(pkDS, pkNewCS);
    }
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::RecursiveConstructMeshSet(NiAVObject*
    pkObject)
{
    if (NiIsKindOf(NiMesh, pkObject))
    {
        NiMesh* pkMesh = (NiMesh*)pkObject;
        NIASSERT(pkMesh);
        pkMesh->RetrieveMeshSet(m_kCompleteMeshSet);
        return;
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        for (NiUInt32 ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            RecursiveConstructMeshSet(pkNode->GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::ReleaseAllStreamRefs(CloneSetPtr& spCloneSet)
{
    NIASSERT(spCloneSet->GetSize() > 0);

    NiTPrimitiveSet<NiDataStream*> kDataStreamsToRemove;
    GetDataStreams(kDataStreamsToRemove, spCloneSet);
    NIASSERT(kDataStreamsToRemove.GetSize() > 0);

    // Create a data-stream to StreamRef map. Initialize StreamRefs to NULL.
    NiTMap<NiDataStream*, NiDataStreamRef*> kDSToSRMap;
    for(NiUInt32 uiSR=0; uiSR < kDataStreamsToRemove.GetSize(); uiSR++)
    {
        NiDataStream* pkDSToRemove = kDataStreamsToRemove.GetAt(uiSR);
        kDSToSRMap.SetAt(pkDSToRemove, NULL);
    }

    // Grab the first mesh and use it to find the StreamRefs that have the
    // matching data-streams
    NiMesh* pkMesh = spCloneSet->GetAt(0);
    NiUInt32 uiSRCnt = pkMesh->GetStreamRefCount();

    for(NiUInt32 ui=0; ui<uiSRCnt; ui++)
    {
        NiDataStreamRef* pkStreamRef = 
            pkMesh->GetStreamRefAt(ui);

        if (!pkStreamRef)
            continue;

        NiDataStream* pkDS = pkStreamRef->GetDataStream();
        NIASSERT(pkDS);

        NiDataStreamRef* pkTmpSR = NULL;
        if (kDSToSRMap.GetAt(pkDS, pkTmpSR))
        {
            NIASSERT(pkTmpSR == NULL);
            // We're looking for this DS, so store the StreamRef
            kDSToSRMap.SetAt(pkDS, pkStreamRef);
        }
    }

    // Now we can just use the ReleaseStreamRef on the stored streamRef.
    // Doing that will ensure that all like streamRefs are removed from every
    // mesh in the cloneset.
    NiTMapIterator kIter = kDSToSRMap.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        NiDataStreamRef* pkSR;
        kDSToSRMap.GetNext(kIter, pkDS, pkSR);
        NIASSERT(pkSR);
        NIVERIFY(ReleaseStreamRef(pkSR));
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::ReplaceStreamRef(CloneSetPtr& spCloneSet,
    const NiFixedString& kSemantic, NiUInt8 uiIndex,
    NiDataStreamElement::Format eFormat, NiDataStreamRef* pkNewStreamRef)
{
    // If the streamRef can't be deleted, it can't be replaced
    if (ReleaseStreamRef(spCloneSet, kSemantic, uiIndex, eFormat) == false)
        return false;

#ifdef NIDEBUG
    // It would be a misuse of ReplaceStreamRef to replace an existing
    // datastream with another existing one.
    NiDataStream* pkDS = pkNewStreamRef->GetDataStream();
    NIASSERT(pkDS);
    CloneSetPtr spDummyCloneSet;
    NIASSERT(m_kDSToCloneSet.GetAt(pkDS, spDummyCloneSet) == false);
#endif

    NIVERIFY(AddStreamRef(spCloneSet, pkNewStreamRef));

    return true;
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::ReplaceStreamRef(
    NiDataStreamRef* pkOldStreamRef,
    NiDataStreamRef* pkNewStreamRef)
{
    // Grab datastream from pkOldStreamRef.
    NiDataStream* pkOldDS = pkOldStreamRef->GetDataStream();
    NIASSERT(pkOldDS);
    NIASSERT(pkOldDS->GetElementDescCount());

    CloneSetPtr spCloneSet;
    if (m_kDSToCloneSet.GetAt(pkOldDS, spCloneSet) == false)
    {
        // Can not find a cloneset for the data stream, so return failure.
        return false;
    }

    if (pkOldStreamRef->GetElementDescCount() != 1)
    {
        NIASSERT(!"Existing StreamRef has multiple elements");
        return false;
    }


    const NiFixedString kSemantic = pkOldStreamRef->GetSemanticNameAt(0);
    const NiUInt8 uiIndex = (NiUInt8)pkOldStreamRef->GetSemanticIndexAt(0);
    NiDataStreamElement::Format eFormat = 
        pkOldDS->GetElementDescAt(0).GetFormat();

    return ReplaceStreamRef(spCloneSet, kSemantic, uiIndex, eFormat,
        pkNewStreamRef);
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::AddStreamRef(CloneSetPtr& spCloneSet,
    NiDataStreamRef* pkStreamRef)
{
    NiUInt32 uiSetSize = spCloneSet->GetSize();
    NiDataStreamRef* pkActualStreamRefAdded = NULL;

    for (NiUInt32 ui=0; ui<uiSetSize; ui++)
    {
        NiMesh* pkMesh = spCloneSet->GetAt(ui);
        pkActualStreamRefAdded = pkMesh->AddStreamRef(pkStreamRef);
        NIASSERT(pkActualStreamRefAdded);
    }

    // Ensure the datastream maps to the cloneset.
    m_kDSToCloneSet.SetAt(pkActualStreamRefAdded->GetDataStream(), spCloneSet);

    return true;
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::ReleaseStreamRef(NiDataStreamRef* pkStreamRef)
{
    NiDataStream* pkDS = pkStreamRef->GetDataStream();
    NIASSERT(pkDS);

    CloneSetPtr spCloneSet = 0;
    if (GetCloneSet(pkDS, spCloneSet) == false)
        return false;

    // It is ok if pkDS->GetElementDescCount is greater than 1, but that does
    // mean that all of those elements and the ds they exist on will be
    // released.

    // We are about to delete the thing that holds onto the 
    // semantic reference, so we need to copy it.
    const NiFixedString kSemantic = pkStreamRef->GetSemanticNameAt(0);
    const NiUInt8 uiIndex = (NiUInt8)pkStreamRef->GetSemanticIndexAt(0);

    NiDataStreamElement::Format eFormat = 
        pkDS->GetElementDescAt(0).GetFormat();

    return ReleaseStreamRef(spCloneSet, kSemantic, uiIndex, eFormat);
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::ReleaseStreamRef(CloneSetPtr& spCloneSet,
    const NiFixedString& kSemantic, NiUInt8 uiIndex,
    NiDataStreamElement::Format eFormat)
{
    NiUInt32 uiSetSize = spCloneSet->GetSize();

    if (uiSetSize == 0)
        return false;

    NiDataStream* pkDSToRemove = NULL;

    for (NiUInt32 ui=0; ui<uiSetSize; ui++)
    {
        NiMesh* pkMesh = spCloneSet->GetAt(ui);

        NiDataStreamRef* pkRef;
        NiDataStreamElement kElement;
        if (!pkMesh->FindStreamRefAndElementBySemantic(kSemantic, uiIndex,
            eFormat, pkRef, kElement))
        {
            // Ensure that in this call, the data stream element
            // wasn't found on a mesh.
            NIASSERT(!pkDSToRemove);
            continue;
        }

        bool bRemoveCloneSet = false;
        if (pkDSToRemove == NULL)
        {
            pkDSToRemove = pkRef->GetDataStream(); 
            bRemoveCloneSet = true;
        }

        // Found the data stream, so remove it.
        pkMesh->RemoveStreamRef(pkRef);

        if (bRemoveCloneSet)
        {
            m_kDSToCloneSet.RemoveAt(pkDSToRemove);
        }
        NIASSERT(pkDSToRemove == pkRef->GetDataStream());

    }

    if (!pkDSToRemove)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::GetDataStreams(
    NiTPrimitiveSet<NiDataStream*>& kDataStreamSet,
    CloneSet* pkCloneSet)
{
    // Iterate over each data
    NiTMapIterator kIter = m_kDSToCloneSet.GetFirstPos();

    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCloneSet;
        m_kDSToCloneSet.GetNext(kIter, pkDS, spCloneSet);

        if (pkCloneSet)
        {
            if (pkCloneSet == spCloneSet)
                kDataStreamSet.Add(pkDS);
        }
        else
        {
            kDataStreamSet.Add(pkDS);
        }
    }
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::GetAllCloneSets(NiTObjectSet<CloneSetPtr>&
    kAllCloneSets)
{
    NiTPointerMap<CloneSet*, bool> kUniqueCloneSetMap;
    NiTMapIterator kIter = m_kDSToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCloneSet;
        m_kDSToCloneSet.GetNext(kIter, pkDS, spCloneSet);

        kUniqueCloneSetMap.SetAt(spCloneSet, true);
    }

    kIter = kUniqueCloneSetMap.GetFirstPos();
    while(kIter)
    {
        CloneSet* pkClone;
        bool bTmp;
        kUniqueCloneSetMap.GetNext(kIter, pkClone, bTmp);
        kAllCloneSets.Add(pkClone);
    }
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::GetCloneSetsThatIncludeMesh(
    NiTObjectSet<CloneSetPtr>& kCloneSets, NiMesh* pkMesh)
{
    NiTPointerMap<CloneSet*, bool> kUniqueCloneSetMap;
    NiTMapIterator kIter = m_kDSToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCloneSet;
        m_kDSToCloneSet.GetNext(kIter, pkDS, spCloneSet);

        if (spCloneSet->Contains(pkMesh) == false)
            continue;

        kUniqueCloneSetMap.SetAt(spCloneSet, true);
    }

    kIter = kUniqueCloneSetMap.GetFirstPos();
    while(kIter)
    {
        CloneSet* pkClone;
        bool bTmp;
        kUniqueCloneSetMap.GetNext(kIter, pkClone, bTmp);
        kCloneSets.Add(pkClone);
    }
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::ReplaceDataStream(NiDataStream* pkDS,
    NiDataStream* pkReplacement)
{
    CloneSetPtr spCloneSet;
    if (GetCloneSet((NiDataStream*)pkDS, spCloneSet) == false)
        return false;

    NiUInt32 uiMeshCnt = spCloneSet->GetSize();
    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        NiMesh* pkMesh = spCloneSet->GetAt(ui);

        NiUInt32 uiSRCnt = pkMesh->GetStreamRefCount();
        for(NiUInt32 uiSR=0;uiSR <uiSRCnt; uiSR++)
        {
            NiDataStreamRef* pkSR =
                pkMesh->GetStreamRefAt(uiSR);

            if (pkSR->GetDataStream() == pkDS)
                pkSR->SetDataStream(pkReplacement);
        }
   }
    m_kDSToCloneSet.SetAt(pkReplacement, spCloneSet);
    m_kDSToCloneSet.RemoveAt(pkDS);
    return true;
}
//---------------------------------------------------------------------------
bool NiToolPipelineCloneHelper::BindRegionToSubmesh(NiDataStream* pkDS,
    NiUInt32 uiSubmeshIdx, NiUInt32 uiDataStreamRegionIdx)
{
    // First get all the mesh that are effected.
    CloneSetPtr spCloneSet;
    if (m_kDSToCloneSet.GetAt(pkDS, spCloneSet) == false)
        return false;

    NiUInt32 uiFound = 0;

    NiUInt32 uiMeshCnt = spCloneSet->GetSize();
    for(NiUInt32 ui=0; ui<uiMeshCnt; ui++)
    {
        NiMesh* pkMesh = spCloneSet->GetAt(ui);
        NIASSERT(pkMesh);

        // Next, find the datastreamRefs that are effected
        NiUInt32 uiStreamRefCnt = pkMesh->GetStreamRefCount();
        if (uiStreamRefCnt < 1)
            continue;

        for (NiUInt32 uiStream=0; uiStream<uiStreamRefCnt; uiStream++)
        {
            NiDataStreamRef* pkSR = pkMesh->GetStreamRefAt(uiStream);
            if (pkSR->GetDataStream() != pkDS)
                continue;

            pkSR->BindRegionToSubmesh(uiSubmeshIdx, uiDataStreamRegionIdx);
            uiFound++;
        }
    }

    return (uiFound == uiMeshCnt);
}
//---------------------------------------------------------------------------
NiTPrimitiveSet<NiMesh*>& NiToolPipelineCloneHelper::GetCompleteMeshSet()
{
    return m_kCompleteMeshSet;
}
//---------------------------------------------------------------------------
void NiToolPipelineCloneHelper::RetrieveAllCloneSets(
    NiTPrimitiveSet<CloneSet*>& kAllCloneSets)
{
    kAllCloneSets.RemoveAll();

    // Use a hash because it will be faster than calling AddUnique on the set.
    NiTPointerMap<CloneSet*, bool> kCloneMap;
    NiTMapIterator kIter = m_kDSToCloneSet.GetFirstPos();
    while(kIter)
    {
        NiDataStream* pkDS = 0;
        CloneSetPtr spCloneSet;
        m_kDSToCloneSet.GetNext(kIter, pkDS, spCloneSet);
        kCloneMap.SetAt(spCloneSet, true);
    }

    kIter = kCloneMap.GetFirstPos();
    while(kIter)
    {
        bool bDummy = false;
        CloneSet* pkCloneSet = NULL;
        kCloneMap.GetNext(kIter, pkCloneSet, bDummy);

        // We don't need to AddUnique since hash-table has one entry.
        kAllCloneSets.Add(pkCloneSet);
    }
}
//---------------------------------------------------------------------------
