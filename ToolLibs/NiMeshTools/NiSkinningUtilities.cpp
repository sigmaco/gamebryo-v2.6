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


//---------------------------------------------------------------------------
#include <NiNode.h>
#include <NiMesh.h>
#include <NiSkinningMeshModifier.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiDataStreamElementLock.h>
#include <NiDataStream.h>
#include <NiTSet.h>
#include <NiSystem.h>
#include <NiTSimpleArray.h>

#include "NiSkinningUtilities.h"

// Uncomment this to test removing skinning
//#define TEST_NO_SKIN

// Helper Class Definitions
//---------------------------------------------------------------------------
class Interaction : public NiMemObject
{
public:
    NiUInt32 m_uiBone;
    float m_fWeight;
};
//---------------------------------------------------------------------------
class VertexInteractions : 
    public NiTPrimitiveSet<Interaction>
{
public:
    inline NiUInt32 GetBone(NiUInt32 i) const;
    inline float GetWeight(NiUInt32 i) const;
    float FindWeight(NiUInt32 uiBone) const;
    bool RemoveBone(NiUInt32 uiBone);
    void Unitize();
    void SortByWeight();
    void SortByIndices();
    void Truncate(NiUInt32 uiBones);
    bool SameBones(const VertexInteractions& kI) const;
    void AddInteraction(Interaction kI);
protected:
    static int CompareWeights(const void* pv0, const void* pv1);
    static int CompareIndices(const void* pv0, const void* pv1);
};
//---------------------------------------------------------------------------
class BoneSet : public NiUnsignedIntSet
{
public:
    bool Contains(const BoneSet& kThat) const;
    NiUInt32 MergedSize(const BoneSet& kThat) const;
    void Merge(const BoneSet& kThat);
    bool ContainsTriangle(const NiUInt32* puiTriangle,
        const VertexInteractions* pkVertex) const;
    bool ContainsVertex(const VertexInteractions& kVertex) const;
    void Sort();
protected:
    static int CompareBones(const void* pv0, const void* pv1);
};
//---------------------------------------------------------------------------
class BoneSetMerger : public NiMemObject
{
public:
    BoneSetMerger(const BoneSet& kSet0, const BoneSet& kSet1);
    bool Done() const;
    NiUInt32 GetNext();
protected:
    NiUInt32 m_uiIndex[2];
    NiUInt32 m_uiEl[2];
    const BoneSet& m_kSet0;
    const BoneSet& m_kSet1;
private:
    BoneSetMerger & operator=( const BoneSetMerger & );
};
//---------------------------------------------------------------------------
class TriangleSet : public NiUnsignedIntSet
{
public:
    void AddTriangles(NiUInt32 uiSubmesh,
        const NiUInt32* puiTriangleToSubmesh, NiUInt32 uiTriangles);
};
//---------------------------------------------------------------------------
// VertexInteractions member functions
//---------------------------------------------------------------------------
inline NiUInt32 VertexInteractions::GetBone(
    NiUInt32 i) const
{
    return GetAt(i).m_uiBone;
}
//---------------------------------------------------------------------------
inline float VertexInteractions::GetWeight(NiUInt32 i) 
    const
{
    return GetAt(i).m_fWeight;
}
//---------------------------------------------------------------------------
float VertexInteractions::FindWeight(NiUInt32 uiBone)
    const
{
    NiUInt32 b;

    for (b = 0; b < m_uiUsed; b++)
    {
        if (GetAt(b).m_uiBone == uiBone)
            return GetAt(b).m_fWeight;
    }

    return 0.0f;
}
//---------------------------------------------------------------------------
void VertexInteractions::Unitize()
{
    NiUInt32 b;
    float fWeightSum = 0.0f;
    
    for (b = 0; b < m_uiUsed; b++)
    {
        fWeightSum += GetAt(b).m_fWeight;
    }

    NIASSERT(fWeightSum != 0.0f);
    
    float fScale = 1.0f / fWeightSum;
    
    for (b = 0; b < m_uiUsed; b++)
    {
        m_pBase[b].m_fWeight *= fScale;
    }
}
//---------------------------------------------------------------------------
bool VertexInteractions::RemoveBone(NiUInt32 uiBone)
{
    NiUInt32 b;

    for (b = 0; b < m_uiUsed; b++)
    {
        if (GetAt(b).m_uiBone == uiBone)
        {
            // Bones are sorted. Do an ordered remove so that they stay sorted.
            OrderedRemoveAt(b);
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void VertexInteractions::SortByWeight()
{
    qsort(m_pBase, GetSize(), sizeof(m_pBase[0]), CompareWeights);
}
//---------------------------------------------------------------------------
int VertexInteractions::CompareWeights(const void* pv0,
    const void* pv1)
{
    // compare function used by qsort

    float fWeight0, fWeight1;

    fWeight0 = ((Interaction*) pv0)->m_fWeight;
    fWeight1 = ((Interaction*) pv1)->m_fWeight;

    // Compare such that larger weights will be first:

    if (fWeight0 > fWeight1)
        return -1;
    if (fWeight0 < fWeight1)
        return 1;
    return 0;
}
//---------------------------------------------------------------------------
void VertexInteractions::SortByIndices()
{
    qsort(m_pBase, GetSize(), sizeof(m_pBase[0]), CompareIndices);
}
//---------------------------------------------------------------------------
int VertexInteractions::CompareIndices(const void* pv0,
    const void* pv1)
{
    // compare function used by qsort

    NiUInt32 uiIndex0, uiIndex1;

    uiIndex0 = ((Interaction*) pv0)->m_uiBone;
    uiIndex1 = ((Interaction*) pv1)->m_uiBone;

    // Compare such that larger weights will be first:

    if (uiIndex0 > uiIndex1)
        return -1;
    if (uiIndex0 < uiIndex1)
        return 1;
    return 0;
}
//---------------------------------------------------------------------------
void VertexInteractions::Truncate(NiUInt32 uiBones)
{
    NIASSERT(uiBones < m_uiUsed);
    m_uiUsed = uiBones;
}
//---------------------------------------------------------------------------
bool VertexInteractions::SameBones(
    const VertexInteractions& kI) const
{
    if (m_uiUsed != kI.m_uiUsed)
        return false;

    for (NiUInt32 b = 0; b < m_uiUsed; b++)
    {
#ifdef NIDEBUG
        // Bones should be sorted in order of increasing index at this point
        if (b > 0)
        {
            NIASSERT(m_pBase[b - 1].m_uiBone < m_pBase[b].m_uiBone);
            NIASSERT(kI.m_pBase[b - 1].m_uiBone < kI.m_pBase[b].m_uiBone);
        }
#endif

        if (m_pBase[b].m_uiBone != kI.m_pBase[b].m_uiBone)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void VertexInteractions::AddInteraction(Interaction kI)
{
    // If the weight won't matter, don't add it.
    if (kI.m_fWeight <= 0.0f)
        return;

    // Go ahead and add to the end.
    Add(kI);

    if (m_uiUsed == 1)
        return;
    
    // Now make sure the bones are sorted properly.
    NiUInt32 ui = 0;
    for (ui = 0; ui < m_uiUsed - 1; ui++)
    {
        if (m_pBase[ui].m_uiBone > kI.m_uiBone)
        {
            Interaction temp = m_pBase[ui];
            m_pBase[ui] = kI;
            kI = temp;
        }
    }

    m_pBase[ui] = kI;
}
//---------------------------------------------------------------------------
// BoneSet member functions
//---------------------------------------------------------------------------
bool BoneSet::Contains(const BoneSet& kThat) const
{
    // This function returns true if kThat is a subset of "this".
    // Assumes "this" and kThat have been sorted.

    NiUInt32 uiThis = 0, uiThat = 0;

    while (uiThat < kThat.GetSize())
    {
        while (uiThis < GetSize() && GetAt(uiThis) < kThat.GetAt(uiThat))
        {
            uiThis++;
        }

        if (uiThis == GetSize() || GetAt(uiThis) != kThat.GetAt(uiThat))
        {
            return false;
        }

        uiThis++;
        uiThat++;
    }

    return true;
}
//---------------------------------------------------------------------------
NiUInt32 BoneSet::MergedSize(const BoneSet& kThat) const
{
    BoneSetMerger kMerger(*this, kThat);
    NiUInt32 uiMergeSize = 0;

    while (!kMerger.Done())
    {
        kMerger.GetNext();
        uiMergeSize++;
    }

    return uiMergeSize;
}
//---------------------------------------------------------------------------
void BoneSet::Merge(const BoneSet& kThat)
{
    // Add all elements of kThat to "this".

    BoneSetMerger kMerger(*this, kThat);
    NiUInt32 uiMergeSize = 0;
    NiUInt32 uiNewAlloced = GetSize() + kThat.GetSize();
    NiUInt32* puiNewBase = NiAlloc(NiUInt32, uiNewAlloced);
    NIASSERT(puiNewBase != NULL);

    while (!kMerger.Done())
    {
        puiNewBase[uiMergeSize++] = kMerger.GetNext();
    }

    NiFree(m_pBase);
    m_pBase = puiNewBase;
    m_uiAlloced = uiNewAlloced;
    m_uiUsed = uiMergeSize;
}
//---------------------------------------------------------------------------
bool BoneSet::ContainsTriangle(
    const NiUInt32* puiTriangle, const VertexInteractions* pkVertex)
    const
{
    NiUInt32 v;

    for (v = 0; v < 3; v++)
    {
        if (!ContainsVertex(pkVertex[puiTriangle[v]]))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool BoneSet::ContainsVertex(
    const VertexInteractions& kVertex) const
{
    // "this" is assumed to be sorted.

    // In order to "contain" a vertex, the boneset must include every bone
    // that influences the vertex.

    NiUInt32 b, uiVertexBones = kVertex.GetSize();
    
    for (b = 0; b < uiVertexBones; b++)
    {
        NiUInt32 usBone = kVertex.GetBone(b);
        
        if (bsearch(&usBone, m_pBase, GetSize(), sizeof(m_pBase[0]),
            CompareBones) == NULL)
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void BoneSet::Sort()
{
    qsort(m_pBase, GetSize(), sizeof(m_pBase[0]), CompareBones);
}
//---------------------------------------------------------------------------
int BoneSet::CompareBones(const void* pv0, const void* pv1)
{
    // compare function used by qsort and bsearch

    NiUInt32 usBone0, usBone1;

    usBone0 = *((NiUInt32*) pv0);
    usBone1 = *((NiUInt32*) pv1);

    if (usBone0 < usBone1)
        return -1;
    if (usBone0 > usBone1)
        return 1;
    return 0;
}
//---------------------------------------------------------------------------
// BoneSetMerger member functions
//---------------------------------------------------------------------------
BoneSetMerger::BoneSetMerger(const BoneSet& kSet0,
    const BoneSet& kSet1)
    : m_kSet0(kSet0), m_kSet1(kSet1)
{
    NIASSERT(kSet0.GetSize() > 0);
    NIASSERT(kSet1.GetSize() > 0);

    m_uiIndex[0] = m_uiIndex[1] = 0;
    m_uiEl[0] = kSet0.GetAt(0);
    m_uiEl[1] = kSet1.GetAt(0);

    // Both sets are assumed to have been sorted.
}
//---------------------------------------------------------------------------
bool BoneSetMerger::Done() const
{
    return m_uiEl[0] == UINT_MAX && m_uiEl[1] == UINT_MAX;
}
//---------------------------------------------------------------------------
NiUInt32 BoneSetMerger::GetNext()
{
    NiUInt32 usNext;

    if (m_uiEl[1] < m_uiEl[0])
    {
        usNext = m_uiEl[1];

        m_uiEl[1] = ++m_uiIndex[1] < m_kSet1.GetSize() ? 
            m_kSet1.GetAt(m_uiIndex[1]) : UINT_MAX;

        NIASSERT(usNext < m_uiEl[1]);
    }
    else 
    {
        usNext = m_uiEl[0];

        if (m_uiEl[1] == m_uiEl[0])
        {
            m_uiEl[1] = ++m_uiIndex[1] < m_kSet1.GetSize() ? 
                m_kSet1.GetAt(m_uiIndex[1]) : UINT_MAX;
            
            NIASSERT(usNext < m_uiEl[1]);
        }
        
        m_uiEl[0] = (++m_uiIndex[0] < m_kSet0.GetSize()) ? 
            m_kSet0.GetAt(m_uiIndex[0]) : UINT_MAX;

        NIASSERT(usNext < m_uiEl[0]);
    }

    return usNext;
}
//---------------------------------------------------------------------------
// TriangleSet member functions
//---------------------------------------------------------------------------
void TriangleSet::AddTriangles(NiUInt32 uiSubmesh,
    const NiUInt32* puiTriangleToSubmesh, NiUInt32 uiTriangles)
{
    for (NiUInt32 t = 0; t < uiTriangles; t++)
    {
        if (puiTriangleToSubmesh[t] == uiSubmesh)
        {
            Add(t);
        }
    }
}
//-------------------------------------------------------------------------
//  Typedefs and Constants for NiSkinning Utilities 
//-------------------------------------------------------------------------
const static NiUInt32 gs_uiBonesPerVertex = 4;
typedef NiTPrimitiveSet<BoneSet*> NiBoneSetCollection;
typedef NiTSimpleArray<NiInt16, 4> NiBlendIndexArray;
typedef NiTSimpleArray<float, 4> NiBlendWeightArray;
typedef NiTStridedRandomAccessIterator<NiBlendWeightArray> 
    NiBlendWeightIterator;
typedef NiTStridedRandomAccessIterator<NiBlendIndexArray> 
    NiBlendIndexIterator;
typedef NiTStridedRandomAccessIterator<NiPoint3> 
    NiPositionIterator;
//---------------------------------------------------------------------------
// Misc helper functions
//---------------------------------------------------------------------------
VertexInteractions* CreateVertexInteractions(NiUInt32 uiVertices,
    const NiSkinningMeshModifier*, NiMesh* pkMesh)
{
    // NiSkinningMeshModifier stores a list of vertices influenced by 
    // each bone. Shuffle the data so that we have a list of bones that 
    // influences each vertex.
    VertexInteractions* pkVertexInteraction = 
        NiNew VertexInteractions[uiVertices];
    NIASSERT(pkVertexInteraction != NULL);

    // Find and lock the blend weights. If they don't exist, early out.
    NiDataStreamElementLock kBlendWeightsLock(pkMesh,
        NiCommonSemantics::BLENDWEIGHT(), 0,
        NiDataStreamElement::F_FLOAT32_4,
        NiDataStream::LOCK_TOOL_READ);

    if (!kBlendWeightsLock.IsLocked())
    {
        NILOG("NiSkinningUtilities::CreateVertexInteractions failed. "
            "BLENDWEIGHT could not be locked.\n");
        return NULL;
    }

    // Find and lock the blend indices. If they don't exist, early out.
    NiDataStreamElementLock kBlendIndicesLock(pkMesh,
        NiCommonSemantics::BLENDINDICES(), 0, 
        NiDataStreamElement::F_INT16_4,
        NiDataStream::LOCK_TOOL_READ);

    if (!kBlendIndicesLock.IsLocked())
    {
        NILOG("NiSkinningUtilities::CreateVertexInteractions failed. "
            "BLENDINDICES could not be locked.\n");
        return NULL;
    }

    NiBlendWeightIterator kBWIter = 
        kBlendWeightsLock.begin<NiBlendWeightArray>();

    NiBlendIndexIterator kBIIter = 
        kBlendIndicesLock.begin<NiBlendIndexArray>();

    for (NiUInt32 ui = 0; ui < uiVertices; ui++)
    {
        for (NiUInt32 uj = 0; uj < 4; uj++)
        {
            Interaction kInteraction;
            kInteraction.m_fWeight = kBWIter[ui][uj];
            kInteraction.m_uiBone = kBIIter[ui][uj];
            pkVertexInteraction[ui].AddInteraction(kInteraction);
        }
    
        pkVertexInteraction[ui].Unitize();
    }

    return pkVertexInteraction;
}
//---------------------------------------------------------------------------
int FloatCompare(const float* pf0, const float* pf1,
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
int VertexCompare(NiUInt32 v0, NiUInt32 v1,
    const NiPositionIterator& kBegin, NiUInt32) 
{
    // This compare function is similar to the one used by the c library
    // functions qsort and bsearch.  It also relies on the fact that
    // NiPoint3 members are stored as consecutive floats.

    int iCmp = FloatCompare(&kBegin[v0].x, &kBegin[v1].x, 3);
    if (iCmp != 0)
        return iCmp;

    return 0;
}
//---------------------------------------------------------------------------
int ChoosePivot(NiUInt32 *puiIndex, const NiPositionIterator& kBegin, 
    NiUInt32 uiCount, int l, int r) 
{
    // Check the first, middle, and last element. Choose the one which falls
    // between the other two. This has a good chance of discouraging 
    // quadratic behavior from qsort.
    // In the case when all three are equal, this code chooses the middle
    // element, which will prevent quadratic behavior for a list with 
    // all elements equal.

    int m = (l + r) >> 1;

    if (VertexCompare(puiIndex[l], puiIndex[m], kBegin, uiCount) < 0)
    {
        if (VertexCompare(puiIndex[m], puiIndex[r], kBegin, uiCount) < 0)
        {
            return puiIndex[m];
        }
        else
        {
            if (VertexCompare(puiIndex[l], puiIndex[r], kBegin, uiCount) < 0)
                return puiIndex[r];
            else
                return puiIndex[l];
        }
    }
    else
    {
        if (VertexCompare(puiIndex[l], puiIndex[r], kBegin, uiCount) < 0)
        {
            return puiIndex[l];
        }
        else
        {
            if (VertexCompare(puiIndex[m], puiIndex[r], kBegin, uiCount) < 0)
                return puiIndex[r];
            else
                return puiIndex[m];
        }
    }
}
//---------------------------------------------------------------------------
void IndexQSort(NiUInt32 *puiIndex, const NiPositionIterator& kBegin, 
    NiUInt32 uiCount, int l, int r) 
{
    if (r > l)
    {
        int i, j;

        i = l - 1;
        j = r + 1;
        int iPivot = ChoosePivot(puiIndex, kBegin, uiCount, l, r);

        for (;;)
        {
            do 
            {
                j--;
            } while (VertexCompare(iPivot, puiIndex[j], kBegin, uiCount) < 0);

            do
            {
                i++;
            } while (VertexCompare(puiIndex[i], iPivot, kBegin, uiCount) < 0);

            if (i < j)
            {
                NiUInt32 usTmp;                
                usTmp = puiIndex[i];
                puiIndex[i] = puiIndex[j];
                puiIndex[j] = usTmp;
            }
            else
            {
                break;
            }
        }

        if (j == r)
        {
            IndexQSort(puiIndex, kBegin, uiCount, l, j - 1);
        }
        else
        {
            IndexQSort(puiIndex, kBegin, uiCount, l, j);
            IndexQSort(puiIndex, kBegin, uiCount, j + 1, r);
        }
    }
}
//---------------------------------------------------------------------------
void IndexSort(NiUInt32*& puiIndex0, NiUInt32*& puiIndex1, 
    const NiPositionIterator& kBegin, NiUInt32 uiCount)
{
    // After a call to IndexSort, vertex[puiIndex0[i]] is the ith smallest
    // and puiIndex1[i] tells where the ith vertex ended up in the sorted 
    // array. Therefore puiIndex0[puiIndex1[i]] == i. This function allocates
    // puiIndex0 & puiIndex1. It is the calling function's responsibility to
    // delete them.

    NiUInt32 uiVertices = uiCount;
    puiIndex0 = NiAlloc(NiUInt32, uiVertices);
    NIASSERT(puiIndex0 != NULL);

    puiIndex1 = NiAlloc(NiUInt32, uiVertices);
    NIASSERT(puiIndex1 != NULL);

    NiUInt32 i;
    for (i = 0; i < uiVertices; i++)
    {
        puiIndex0[i] = i;
    }
    
    IndexQSort(puiIndex0, kBegin, uiCount, 0, uiVertices - 1);

    for (i = 0; i < uiVertices; i++)
    {
        puiIndex1[puiIndex0[i]] = i;
    }
}
//---------------------------------------------------------------------------
void MakeBoneMap(NiTPointerMap<NiUInt32, float>& kMap,
    const NiUInt32* puiTriangle,
    const VertexInteractions* pkVertexInteraction)
{
    // Creates a hash table that maps bone indices to the maximum weight that
    // bone exerts on any vertex in the triangle.

    for (NiUInt32 v = 0; v < 3; v++)
    {
        NiUInt32 uiVert = puiTriangle[v];
        const VertexInteractions* pkI = &pkVertexInteraction[uiVert];
        NiUInt32 uiVertexBones = pkI->GetSize();
        
        NIASSERT(uiVertexBones != 0);

        for (NiUInt32 b = 0; b < uiVertexBones; b++)
        {
            float fWeight;
            NiUInt32 uiBone = pkI->GetBone(b);
            
            bool bMapped = kMap.GetAt(uiBone, fWeight);
            if (!bMapped || fWeight < pkI->GetWeight(b))
            {
                kMap.SetAt(uiBone, pkI->GetWeight(b));
            }
        }
    }
}
//---------------------------------------------------------------------------
bool RemoveExcessInteractions(
    NiMesh* pkMesh, unsigned char ucBonesPerSubmesh,
    unsigned char ucBonesPerVertex, 
    VertexInteractions* pkVertexInteraction)
{
    NiUInt32 uiVertices = pkMesh->GetVertexCount(0);
    NiUInt32 uiTriangles = pkMesh->GetPrimitiveCount(0);

    NiDataStreamElementLock kPosLock(pkMesh, NiCommonSemantics::POSITION(), 
        0, NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);

    if (!kPosLock.IsLocked())
        return false;

    NIASSERT(kPosLock.count() == uiVertices);
    const NiPositionIterator& kPosIter = kPosLock.begin<NiPoint3>();

    NiDataStreamPrimitiveLock kIndexLock(pkMesh, NiDataStream::LOCK_TOOL_READ);

    if (!kIndexLock.IndexBufferLocked())
        return false;

    NIASSERT(kIndexLock.count() == uiTriangles);
    const NiIndexedPrimitiveIterator32 kIndexIter =
        kIndexLock.BeginIndexed32();

    // Assumption made by this code: The sum of the weights of a vertex's 
    // interactions is one. This allows us to pitch the interaction with the
    // smallest weight without checking to see if that interaction is a
    // vertex's only interaction.

    // kMap maps bone indices to weights. It could be declared inside the
    // loop, but declaring it hear and calling RemoveAll at the end of the
    // loop prevents some deletion and reallocation work that would be done 
    // if the map went out of scope after every loop iteration.

    NiTPointerMap<NiUInt32, float> kMap;
    NiUInt32 t, v;

    // Ditto remarks about kMap's declaration outside the loop.
    NiUnsignedIntSet kCoincidentVerts;

    NiUInt32* puiIndex0 = NULL;
    NiUInt32* puiIndex1 = NULL;
    IndexSort(puiIndex0, puiIndex1, kPosIter, uiVertices);

    // This loop identifies triangles that are influenced by more than the
    // allowed number of bones. For each such triangle, the least significant
    // vertex-bone interactions are thrown out until the number of bones
    // influencing the triangle reaches the allowed number.
    for (t = 0; t < uiTriangles; t++)
    {
        // Create a list of bones that influence this triangle. For each 
        // bone, save the maximum weight of that bone in any of the 
        // triangle's verts.

        NiUInt32 auiTriangle[3];
        kIndexIter[t].Get(auiTriangle, 3);

        if (kIndexIter[t].IsDegenerate())
            continue;

        MakeBoneMap(kMap, auiTriangle, pkVertexInteraction);

        // If the triangle is influenced by more than the maximum allowed
        // number of bones, toss out the least influential interactions until
        // the maximum allowed number is reached.
        NiUInt32 uiBones = kMap.GetCount();
        if (uiBones > ucBonesPerSubmesh)
        {
            for (;;)
            {
                NiTMapIterator pos = kMap.GetFirstPos();
                float fMinWeight = FLT_MAX;
                NiUInt32 uiMinBone = 0;
                
                do
                {
                    NiUInt32 uiBone;
                    float fWeight;
                    
                    kMap.GetNext(pos, uiBone, fWeight);
                    if (fWeight < fMinWeight)
                    {
                        fMinWeight = fWeight;
                        uiMinBone = uiBone;
                    }
                } while (pos);
                
                for (v = 0; v < 3; v++)
                {
                    NiUInt32 uiVert = auiTriangle[v];

                    // Identify all vertices coincident with vertex uiVert
                    // and remove the interaction of bone uiMinBone with
                    // those vertices.
                    NiUInt32 uiStart, uiEnd, uiCoincidentVert;

                    kCoincidentVerts.Add(uiVert);
                    VertexInteractions* pkI = &pkVertexInteraction[uiVert];

                    uiStart = uiEnd = puiIndex1[uiVert];
                    while (uiStart > 0 && VertexCompare(
                        uiCoincidentVert = puiIndex0[uiStart - 1], uiVert,
                        kPosIter, uiVertices) == 0)
                    {
                        if (pkI->SameBones(pkVertexInteraction[
                            uiCoincidentVert]))
                        {
                            kCoincidentVerts.Add(uiCoincidentVert);
                        }
                        uiStart--;
                    }

                    while (uiEnd < uiVertices - 1 && VertexCompare(
                        uiCoincidentVert = puiIndex0[uiEnd + 1], uiVert,
                        kPosIter, uiVertices) == 0)
                    {
                        if (pkI->SameBones(pkVertexInteraction[
                            uiCoincidentVert]))
                        {
                            kCoincidentVerts.Add(uiCoincidentVert);
                        }
                        uiEnd++;
                    }

                    for (NiUInt32 j = 0; j < kCoincidentVerts.GetSize();
                        j++)
                    {
                        uiCoincidentVert = kCoincidentVerts.GetAt(j);
                        pkI = &pkVertexInteraction[uiCoincidentVert];

                        if (pkI->RemoveBone(uiMinBone))
                        {
                            if (pkI->GetSize() == 0)
                            {
                                // Deleted bone was vertex's only bone, so
                                // return failure. This should only happen if
                                // the number of bones per Submesh is less
                                // than 3.
                                NIASSERT(ucBonesPerSubmesh < 3);
                                NiFree(puiIndex0);
                                NiFree(puiIndex1);
                                return false;
                            }
                            
                            pkI->Unitize();
                        }
                    }

                    kCoincidentVerts.RemoveAll();
                }

                if (--uiBones == ucBonesPerSubmesh)
                    break;

                kMap.RemoveAll();
                MakeBoneMap(kMap, auiTriangle, pkVertexInteraction);
            }
        }
        
        kMap.RemoveAll();
    }

    for (v = 0; v < uiVertices; v++)
    {
        VertexInteractions* pkI = &pkVertexInteraction[v];

        // We've insured that no triangle is influenced by more than 
        // ucBonesPerSubmesh bones. Now make sure no vertex is influenced
        // by more tham ucBonesPerVertex bones.
        if (pkI->GetSize() > ucBonesPerVertex)
        {
            pkI->SortByWeight();
            pkI->Truncate(ucBonesPerVertex);
            pkI->Unitize();
        }
    }

    NiFree(puiIndex0);
    NiFree(puiIndex1);

    return true;
}
//---------------------------------------------------------------------------
void CreateInitialBoneSets(
    NiMesh* pkMesh,
    VertexInteractions* pkVertexInteraction,
    NiBoneSetCollection& kBoneSets)
{
    NIASSERT(kBoneSets.GetSize() == 0);
    NiUInt32 uiTriangles = pkMesh->GetPrimitiveCount(0);

    NiDataStreamPrimitiveLock kIndexLock(pkMesh, NiDataStream::LOCK_TOOL_READ);

    NIVERIFY(kIndexLock.IndexBufferLocked());

    NIASSERT(kIndexLock.count() == uiTriangles);
    NiIndexedPrimitiveIterator32 kIndexIter = kIndexLock.BeginIndexed32();


    for (NiUInt32 t = 0; t < uiTriangles; t++)
    {
        NiUInt32 auiTriangle[3];
        kIndexIter[t].Get(auiTriangle, 3);

        if (kIndexIter[t].IsDegenerate())
            continue;

        BoneSet* pkNewSet = NiNew BoneSet;
        NIASSERT(pkNewSet != NULL);

        // Create a set that contains all the vertices that influence the
        // triangle.

        for (NiUInt32 v = 0; v < 3; v++)
        {
            NiUInt32 uiVert = auiTriangle[v];
            VertexInteractions* pkI = &pkVertexInteraction[uiVert];
            NiUInt32 uiVertexBones = pkI->GetSize();
            
            for (NiUInt32 b = 0; b < uiVertexBones; b++)
            {
                NiUInt32 uiBone = pkI->GetBone(b);
                pkNewSet->AddUnique(uiBone);
            }
        }
        
        pkNewSet->Sort();
        
        // Check existing sets.

        NiUInt32 s;

        for (s = 0; s < kBoneSets.GetSize(); /**/)
        {
            BoneSet* pkExistingSet = kBoneSets.GetAt(s);

            if (pkExistingSet->Contains(*pkNewSet))
            {
                // If there is already a boneset that contains this
                // triangle's boneset, we can stop.
                NiDelete pkNewSet;
                break;
            }
            
            if (pkNewSet->Contains(*pkExistingSet))
            {
                // If this triangle's bone set contains an existing
                // boneset, remove the existing boneset.
                NiDelete pkExistingSet;
                kBoneSets.RemoveAt(s);
            }
            else
            {
                s++;
            }
        }
        
        if (s == kBoneSets.GetSize())
        {
            // No boneset was found that contains this triangle's boneset, so
            // add the new boneset to the boneset set.
            kBoneSets.Add(pkNewSet);
        }
    }
}
//---------------------------------------------------------------------------
void MergeBoneSets(unsigned char ucBonesPerPartition,
    NiBoneSetCollection& kBoneSets)
{
    // Partition merging algorithm. Time required is O(p^3), where p is the 
    // number of submeshes. p is O(t), where t is the number of triangles.

    for (;;)
    {
        // Find best merge candidates. Currently, best merge candidates
        // are considered those which share the most bones.
        
        NiUInt32 auiMergeSets[2];
        auiMergeSets[0] = auiMergeSets[1] = 0;
        int iMaxShare = -1;
        
        for (NiUInt32 p0 = 0; p0 < kBoneSets.GetSize(); p0++)
        {
            BoneSet& kBoneSet0 = *kBoneSets.GetAt(p0);
            
            for (NiUInt32 p1 = p0 + 1; p1 < kBoneSets.GetSize(); p1++)
            {
                BoneSet& kBoneSet1 = *kBoneSets.GetAt(p1);                
                NiUInt32 uiMergeSize = kBoneSet0.MergedSize(kBoneSet1);
                
                if (uiMergeSize <= ucBonesPerPartition)
                {
                    int iShare = kBoneSet0.GetSize() +
                        kBoneSet1.GetSize() - uiMergeSize;
                    
                    if (iShare > iMaxShare)
                    {
                        auiMergeSets[0] = p0;
                        auiMergeSets[1] = p1;
                        iMaxShare = iShare;
                    }
                }
            }
        }
        
        if (iMaxShare < 0)
            break;
        
        // Merge
        
        kBoneSets.GetAt(auiMergeSets[0])->Merge(
            *kBoneSets.GetAt(auiMergeSets[1]));
        NIASSERT(kBoneSets.GetAt(auiMergeSets[0])->GetSize() <=
            ucBonesPerPartition);
        NiDelete kBoneSets.GetAt(auiMergeSets[1]);
        kBoneSets.RemoveAt(auiMergeSets[1]);
        
        // See if any sets are now a subset of the newly merged set.

        BoneSet* pkNewSet = kBoneSets.GetAt(auiMergeSets[0]);

        for (NiUInt32 p = 0; p < kBoneSets.GetSize(); /**/)
        {
            BoneSet* pkSet = kBoneSets.GetAt(p);

            if (pkSet != pkNewSet && pkNewSet->Contains(*pkSet))
            {
                NiDelete pkSet;
                kBoneSets.RemoveAt(p);
            }
            else
            {
                p++;
            }
        }
    }
}
//---------------------------------------------------------------------------
int AssignTriangle(NiBoneSetCollection& kBoneSets, 
    const NiUInt32* puiTriangle, VertexInteractions* pkVertexInteraction, 
    bool* pbAssigned, bool bAssign)
{
    for (NiUInt32 p = 0; p < kBoneSets.GetSize(); p++)
    {
        if (pbAssigned[p] == bAssign && kBoneSets.GetAt(p)->ContainsTriangle(
            puiTriangle, pkVertexInteraction))
        {
            pbAssigned[p] = true;
            return p;
        }
    }

    return -1;
}
//---------------------------------------------------------------------------
void MakeBoneSets(NiMesh* pkMesh, 
    unsigned char ucBonesPerPartition,
    VertexInteractions* pkVertexInteraction, NiBoneSetCollection& kBoneSets,
    NiUInt32*& puiTriangleToPartition)
{
    NiUInt32 uiTriangles = pkMesh->GetPrimitiveCount(0);

    CreateInitialBoneSets(pkMesh, pkVertexInteraction, kBoneSets);

    MergeBoneSets(ucBonesPerPartition, kBoneSets);
    
    // Assign each triangle to a submesh. It is possible that a submesh
    // will have no triangles assigned. For example, suppose these submeshes
    // exist after the call to CreateInitialBoneSets:

    // 0.0: A B C
    // 0.1: A B D
    // 0.2: A B Z
    // 0.3: A C Z
    // 0.4: B D Y
    // 0.5: A D Y

    // Then, during MergeBoneSets, the following mergings occur. 0.0 merges
    // with 0.1:

    // 1.0: A B C D
    // 1.1: A B Z
    // 1.2: A C Z
    // 1.3: B D Y
    // 1.4: A D Y

    // 1.1 merges with 1.2:

    // 2.0: A B C D
    // 2.1: A B C Z
    // 2.2: B D Y
    // 2.3: A D Y

    // 2.2 merges with 2.3:

    // 3.0: A B C D
    // 3.1: A B C Z
    // 3.2: A B D Y

    // Set 3.0 was created from the merging of sets 0.0 and 0.1. But all the
    // bones from set 0.0 appear not only in 3.0, but also in 3.1.
    // Likewise, all the bones from set 0.1 appear not only in 3.0, but also
    // in 3.2. So all the triangles that led to the creation of set 3.0 can be
    // assigned to some other set. The following code attempts to take 
    // advantage of this possibility by first checking to see if a triangle 
    // can be assigned to a submesh that already has triangles assigned to
    // it before checking submeshes that as of yet have no triangles
    // assigned.
    
    puiTriangleToPartition = NiAlloc(NiUInt32, uiTriangles);
    NIASSERT(puiTriangleToPartition != NULL);
        
    bool* pbAssigned = NiAlloc(bool, kBoneSets.GetSize());
    NiUInt32 p;

    for (p = 0; p < kBoneSets.GetSize(); p++)
    {
        pbAssigned[p] = false;
    }
    
    
    NiDataStreamPrimitiveLock kIndexLock(pkMesh, 
        NiDataStream::LOCK_TOOL_READ);

    NIVERIFY(kIndexLock.IndexBufferLocked());

    NIASSERT(kIndexLock.count() == uiTriangles);
    NiIndexedPrimitiveIterator32 kIndexIter = kIndexLock.BeginIndexed32();

    for (NiUInt32 t = 0; t < uiTriangles; t++)
    {
        NiUInt32 auiTriangle[3];
        
        kIndexIter[t].Get(auiTriangle, 3);

        if (kIndexIter[t].IsDegenerate())
        {
            puiTriangleToPartition[t] = UINT_MAX;
        }
        else
        {
            int iPart;

            if ((iPart = AssignTriangle(kBoneSets, auiTriangle,
                pkVertexInteraction, pbAssigned, true)) < 0)
            {
                iPart = AssignTriangle(kBoneSets, auiTriangle,
                    pkVertexInteraction, pbAssigned, false);
                NIASSERT(iPart >= 0);
            }

            puiTriangleToPartition[t] = iPart;
        }
    }

    // Delete submeshes to which no triangle was assigned. 
    for (p = 0; p < kBoneSets.GetSize(); )
    {
        if (pbAssigned[p])
        {
            p++;
        }
        else
        {
            kBoneSets.RemoveAt(p);

            // Do submesh reassignment necessitated by compacting
            // of kBoneSets when element p is removed:

            NiUInt32 uiOldIndex = kBoneSets.GetSize();

            for (NiUInt32 t = 0; t < uiTriangles; t++)
            {
                if (puiTriangleToPartition[t] == uiOldIndex)
                {
                    puiTriangleToPartition[t] = p;
                }
            }
        }
    }

    NiFree(pbAssigned);
}
//---------------------------------------------------------------------------
NiUInt32* CreateVertexMap(NiUInt32 uiFullVertexCount, 
    NiDataStreamPrimitiveLock& kIndexLock, const TriangleSet& kTriangles,
    NiUInt32*& puiSubmeshVertexMap, NiUInt32& uiSubmeshVertexCount)
{
    NIASSERT(kIndexLock.IndexBufferLocked());
    NIASSERT(kIndexLock.GetSubmeshCount() == 1);

    // puiPartitionIndex maps original MeshImpl indices to submesh indices.
    // puiSubmeshVertexMap maps submesh indices to original TriShape indices.

    NiUInt32* puiVertexSubmeshIndex = NiAlloc(NiUInt32, 
        uiFullVertexCount);
    NIASSERT(puiVertexSubmeshIndex != NULL);
    
    // Vertices not in the submesh will map to UINT_MAX, so initialize all
    // entries to that value.
    for (NiUInt32 v = 0; v < uiFullVertexCount; v++)
    {
        puiVertexSubmeshIndex[v] = UINT_MAX;
    }

    NiUInt32 uiPartitionVertices = 0;
    
    NiIndexedPrimitiveIterator32 kIndexIter = kIndexLock.BeginIndexed32();

    // Assign submesh indices to vertices in the submesh triangles.
    for (NiUInt32 t = 0; t < kTriangles.GetSize(); t++)
    {
        NiUInt32 uiTriangle = kTriangles.GetAt(t);
        NiUInt32 auiTriangle[3];

        kIndexIter[uiTriangle].Get(auiTriangle, 3);

        for (NiUInt32 v = 0; v < 3; v++)
        {
            NiUInt32 uiVert = auiTriangle[v];
            if (puiVertexSubmeshIndex[uiVert] == UINT_MAX)
            {
                puiVertexSubmeshIndex[uiVert] = uiPartitionVertices++;
            }
        }
    }

    puiSubmeshVertexMap = NiAlloc(NiUInt32, uiPartitionVertices);
    NIASSERT(puiSubmeshVertexMap != NULL);

    uiSubmeshVertexCount = uiPartitionVertices;

    for (NiUInt32 v = 0; v < uiFullVertexCount; v++)
    {
        NiUInt32 usPartitionVert = puiVertexSubmeshIndex[v];
        if (usPartitionVert != UINT_MAX)
        {
            puiSubmeshVertexMap[usPartitionVert] = v;
        }
    }
    
    return puiVertexSubmeshIndex;
}
//---------------------------------------------------------------------------
NiUInt32* CreateTriangleList(const TriangleSet& kTriangles, 
    NiDataStreamPrimitiveLock& kIndexLock, 
    const NiUInt32* puiVertexSubmeshIndex)
{
    // puiVertexSubmeshIndex maps TriShape indices to submesh indices.
    // Use it to create a TriList for the submesh that refers to submesh
    // indices.

    NiUInt32* puiTriList = NiAlloc(NiUInt32, 3 * kTriangles.GetSize());
    NIASSERT(puiTriList != NULL);

    NiIndexedPrimitiveIterator32 kIndexIter = kIndexLock.BeginIndexed32();

    for (NiUInt32 t = 0; t < kTriangles.GetSize(); t++)
    {
        NiUInt32 uiTriangle = kTriangles.GetAt(t);
        NiUInt32 auiTriangle[3];

        kIndexIter[uiTriangle].Get(auiTriangle, 3);

        NiUInt32* puiPartitionTriangle = &puiTriList[3 * t];

        for (NiUInt32 v = 0; v < 3; v++)
        {
            puiPartitionTriangle[v] =
                puiVertexSubmeshIndex[auiTriangle[v]];
        }
    }

    return puiTriList;
}
//-------------------------------------------------------------------------
class Submesh : public NiMemObject
{
public:
    Submesh()
    {
        m_uiTriangles = 0;
        m_uiVertices = 0;
        m_uiBones = 0;
        m_puiBones = 0;
        m_puiTriList = 0;
        m_puiVertexMap = 0;
        m_uiVBOffsetIndex = 0;
        m_uiIBOffsetIndex = 0;
    }

    ~Submesh()
    {
        NiFree(m_puiBones);
        NiFree(m_puiVertexMap);
        NiFree(m_puiTriList);
    }

    // Copy a larger buffer into a region for this submesh, using
    // the vertex remapping or index remapping
    inline bool Convert(NiDataStream* pkDSRead, NiDataStream* pkDSWrite)
    {
        // Make sure the usage flags match up
        NiDataStream::Usage eUsage = pkDSRead->GetUsage();
        if (eUsage != pkDSWrite->GetUsage())
            return false;

        // Make sure the strides match up
        NiUInt32 uiStride = pkDSRead->GetStride();
        if (uiStride != pkDSWrite->GetStride())
            return false;

        // Compute the correct data to remap for the usage type
        NiUInt32 uiCount = 0;
        NiUInt32 uiWriteOffsetIndex = 0;
        NiUInt32 uiWriteOffsetInBytes = 0;
        NiUInt32* puiRemapBuffer = NULL;
        
        if (eUsage == NiDataStream::USAGE_VERTEX)
        {
            uiCount = m_uiVertices;
            uiWriteOffsetIndex = m_uiVBOffsetIndex;
            uiWriteOffsetInBytes = m_uiVBOffsetIndex * uiStride;
            puiRemapBuffer = m_puiVertexMap;
        }
        else
        {
            return false;
        }

        if (uiCount == 0)
        {
            pkDSWrite->AddRegion(NiDataStream::Region(uiWriteOffsetIndex, 0));
            return true;
        }

        NiUInt8* pbRead = (NiUInt8*)pkDSRead->Lock(
            NiDataStream::LOCK_TOOL_READ);
        NiUInt8* pbWrite = (NiUInt8*)pkDSWrite->Lock(
            NiDataStream::LOCK_TOOL_WRITE);

        // Move the write pointer to the start of this submesh
        pbWrite += uiWriteOffsetInBytes;

        // Copy the data from read to write using the remap buffer
        // as a guide.
        for (NiUInt32 uiWriteIdx = 0; uiWriteIdx < uiCount; uiWriteIdx++)
        {
            // Determine the read index
            NiUInt32 uiReadIdx = puiRemapBuffer[uiWriteIdx];
            
            // Offset the buffers
            NiUInt8* pbReadData = pbRead + uiStride * uiReadIdx;
            NiUInt8* pbWriteData = pbWrite + uiStride * uiWriteIdx;

            // Copy the data
            NIVERIFY(
                !NiMemcpy(pbWriteData, uiStride + 1, pbReadData, uiStride));
        }
    
        pkDSWrite->AddRegion(NiDataStream::Region(uiWriteOffsetIndex, 
            uiCount));
        pkDSWrite->Unlock(NiDataStream::LOCK_TOOL_WRITE);
        pkDSRead->Unlock(NiDataStream::LOCK_TOOL_READ);    
        return true;
    }

    // Create an index buffer
    inline bool CreateIndexBuffer(NiDataStream* pkDSWrite)
    {
        // Compute the correct data to remap for the usage type
        NiUInt32 uiCount = m_uiTriangles * 3;
        NiUInt32 uiWriteOffsetIndex = m_uiIBOffsetIndex;

        // If no data is to be added, create a zero-length region
        if (uiCount == 0)
        {
            pkDSWrite->AddRegion(NiDataStream::Region(uiWriteOffsetIndex, 0));
            return true;
        }
        
        // Make sure the index buffer is one of our supported formats
        NiDataStreamElement::Format eFormat = 
            pkDSWrite->GetElementDescAt(0).GetFormat();

        if (eFormat == NiDataStreamElement::F_UINT32_1)
        {
            // The format is supported.
        }
        else
        {
            // The format is unsupported
            return false;
        }

        NiUInt32* puiWrite = (NiUInt32*)pkDSWrite->Lock(
            NiDataStream::LOCK_TOOL_WRITE);

        // Move the write pointer to the start of this submesh
        puiWrite += uiWriteOffsetIndex;

        for (NiUInt32 ui = 0; ui < uiCount; ui++)
        {
            puiWrite[ui] = m_puiTriList[ui];
        }

        // Clean up
        pkDSWrite->AddRegion(NiDataStream::Region(uiWriteOffsetIndex, 
            uiCount));
        pkDSWrite->Unlock(NiDataStream::LOCK_TOOL_WRITE); 
        return true;
    }

    // Enum used to determine which buffer to create from the submesh's 
    // internal data
    enum CreateElementType
    {
        CET_BLEND_WEIGHTS = 0,
        CET_BLEND_INDICES
    };

    // Create either a blend weight or a blend index buffers
    inline bool Create(NiDataStream* pkDSWrite, 
        VertexInteractions* pkInteractions,
        CreateElementType eCreateType)
    {
        // Make sure the strides match up
        NiUInt32 uiStride = pkDSWrite->GetStride();

        // Compute the correct data to remap for the usage type
        NiUInt32 uiCount = 0;
        NiUInt32 uiWriteOffsetIndex = 0;
        NiUInt32 uiWriteOffsetInBytes = 0;
        NiUInt32* puiRemapBuffer = NULL;
        if (pkDSWrite->GetUsage() == NiDataStream::USAGE_VERTEX)
        {
            uiCount = m_uiVertices;
            uiWriteOffsetIndex = m_uiVBOffsetIndex;
            uiWriteOffsetInBytes = m_uiVBOffsetIndex * uiStride;
            puiRemapBuffer = m_puiVertexMap;
        }
        else
        {
            return false;
        }

        // If no data is to be added, create a zero-length region
        if (uiCount == 0)
        {
            pkDSWrite->AddRegion(NiDataStream::Region(uiWriteOffsetIndex, 0));
            return true;
        }

        NiUInt8* pbWrite = (NiUInt8*)pkDSWrite->Lock(
            NiDataStream::LOCK_TOOL_WRITE);

        // Move the write pointer to the start of this submesh
        pbWrite += uiWriteOffsetInBytes;

        // Copy the data from read to write using the remap buffer
        // as a guide.
        for (NiUInt32 uiWriteIdx = 0; uiWriteIdx < uiCount; uiWriteIdx++)
        {
            // Determine the read index
            NiUInt32 uiReadIdx = puiRemapBuffer[uiWriteIdx];

            // Get the correct vertex interactions object
            VertexInteractions& kInteractionSet = pkInteractions[uiReadIdx];

            // Make sure the indices are in order from lowest to highest
            kInteractionSet.SortByIndices();

            // Make sure that we don't somehow have too many interactions
            // per vertex.
            NIASSERT(kInteractionSet.GetSize() <= gs_uiBonesPerVertex); 

            // Copy the data over
            if (eCreateType == CET_BLEND_WEIGHTS)
            {
                // Get the correct offset of the data
                float* pfWriteData = (float*)(pbWrite + uiStride * uiWriteIdx);

                if (m_uiBones <= 4)
                {
                    // Special case for four bones.
                    // Blend indices are always 0, 1, 2, 3.
                    // Reorder weights for this sorting.
                    pfWriteData[0] = 0.0f;
                    pfWriteData[1] = 0.0f;
                    pfWriteData[2] = 0.0f;
                    pfWriteData[3] = 0.0f;

                    for (NiUInt32 uiSubElemIdx = 0; 
                        uiSubElemIdx < kInteractionSet.GetSize(); 
                        uiSubElemIdx++)
                    {
                        NiUInt32 uiMasterBoneIdx = kInteractionSet.GetBone(
                            uiSubElemIdx); 
                        NiUInt32 uiBoneIdx = RemapBone(uiMasterBoneIdx);
                        NIASSERT(uiBoneIdx < 4);
                        if (uiBoneIdx < 4)
                        {
                            pfWriteData[uiBoneIdx] += 
                                kInteractionSet.GetWeight(uiSubElemIdx);
                        }
                    }
                }
                else
                {
                    // Copy the stored blend weights
                    NiUInt32 uiSubElemIdx = 0;
                    for (; uiSubElemIdx < kInteractionSet.GetSize() &&
                        uiSubElemIdx < gs_uiBonesPerVertex; uiSubElemIdx++)
                    {
                        float fWeight = kInteractionSet.GetWeight(
                            uiSubElemIdx); 
                        pfWriteData[uiSubElemIdx] = fWeight;
                    }

                    // Fill in the leftover blend weights
                    for (; uiSubElemIdx < gs_uiBonesPerVertex; uiSubElemIdx++)
                    {
                        pfWriteData[uiSubElemIdx] = 0.0f;
                    }
                }
            }
            else if (eCreateType == CET_BLEND_INDICES)
            {
                // Get the correct offset of the data
                NiInt16* puiWriteData = (NiInt16*)(pbWrite + 
                    uiStride * uiWriteIdx);

                if (m_uiBones <= 4)
                {
                    // Special case for four bones to support the fixed
                    // function pipeline.
                    puiWriteData[0] = 0;
                    puiWriteData[1] = 1;
                    puiWriteData[2] = 2;
                    puiWriteData[3] = 3;
                }
                else
                {
                    // Copy the stored blend indices
                    NiUInt32 uiSubElemIdx = 0;
                    for (; uiSubElemIdx < kInteractionSet.GetSize() &&
                        uiSubElemIdx < gs_uiBonesPerVertex; 
                        uiSubElemIdx++)
                    {
                        NiUInt32 uiMasterBoneIdx = kInteractionSet.GetBone(
                            uiSubElemIdx); 
                        NiUInt32 uiBoneIdx = RemapBone(uiMasterBoneIdx);
                        NIASSERT(uiBoneIdx < 256);
                        puiWriteData[uiSubElemIdx] = (NiInt16) uiBoneIdx;
                    }

                    // Fill in the leftover blend indices
                    for (; uiSubElemIdx < gs_uiBonesPerVertex; uiSubElemIdx++)
                    {
                        puiWriteData[uiSubElemIdx] = 0;
                    }
                }
            }
        }
    
        pkDSWrite->AddRegion(NiDataStream::Region(uiWriteOffsetIndex, 
            uiCount));
        pkDSWrite->Unlock(NiDataStream::LOCK_TOOL_WRITE); 
        return true;
    }

    // Remap from the larger bone matrix index to the local submesh index
    inline NiUInt32 RemapBone(NiUInt32 uiMasterBone)
    {
        for (NiUInt32 ui = 0; ui < m_uiBones; ui++)
        {
            if (m_puiBones[ui] == uiMasterBone)
                return ui;
        }

        NIASSERT(!"We should never reach this point.");
        return UINT_MAX;
    }

    inline NiUInt32 FindVertexIndex(NiUInt32 uiVertexIndex)
    {
        for (NiUInt32 ui = 0; ui < m_uiVertices; ui++)
        {
            if (m_puiVertexMap[ui] == uiVertexIndex)
                return ui;
        }

        NIASSERT(!"We should never reach this point.");
        return UINT_MAX;
    }


    // Create the remapping array for bones in this submesh to the
    // larger bone matrix array
    inline bool CreateBoneRemap(NiDataStream* pkDSWrite, 
        NiUInt32 uiOffsetInBytes, NiUInt32 uiBonesPerPartition)
    {
        NiUInt16* puiWrite = (NiUInt16*)((NiUInt8*)pkDSWrite->Lock(
            NiDataStream::LOCK_TOOL_WRITE) + uiOffsetInBytes);

        NIASSERT(m_uiBones <= uiBonesPerPartition);

        // Copy in the known bones
        NiUInt32 uiCurrBoneIdx = 0;
        for (; uiCurrBoneIdx < m_uiBones; uiCurrBoneIdx++)
        {
            NIASSERT(m_puiBones[uiCurrBoneIdx] <= USHRT_MAX);
            NiUInt16 uiRemappedIdx = (NiUInt16)(m_puiBones[uiCurrBoneIdx]);
            puiWrite[uiCurrBoneIdx] = uiRemappedIdx;
        }

        // Fill in the remainder of bones
        for (; uiCurrBoneIdx < uiBonesPerPartition; uiCurrBoneIdx++)
        {
            puiWrite[uiCurrBoneIdx] = 0;
        }

        pkDSWrite->Unlock(NiDataStream::LOCK_TOOL_WRITE);

        return true;
    }

    NiUInt32 m_uiVBOffsetIndex;
    NiUInt32 m_uiIBOffsetIndex;
    NiUInt32 m_uiTriangles;
    NiUInt32 m_uiVertices;
    NiUInt32 m_uiBones;
    NiUInt32* m_puiBones;
    NiUInt32* m_puiTriList;
    NiUInt32* m_puiVertexMap;
};
//-------------------------------------------------------------------------
//  NiSkinningUtilities 
//-------------------------------------------------------------------------
bool NiSkinningUtilities::MakeSubmeshesForGPUSkinning(
    NiToolPipelineCloneHelper& kCloneHelper,
    NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet, 
    unsigned char ucBonesPerSubmesh)
{
    // Assume all mesh in cloneset has same requirements, modifiers, etc.
    NiMesh* pkMesh = spCloneSet->GetAt(0);

    // Does the mesh exist?
    if (!pkMesh)
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh does not exist.\n");
        return false;
    }

    // Is this a triangle-based mesh? If not, this routine will not work.
    if (pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES)
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh is not a triangle list.\n");
        return false;
    }

    // Is this mesh instanced. We do not support instanced gpu skinned meshes.
    if (pkMesh->GetInstanced())
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh is instanced.\n");
        return false;
    }
    
    // Has this mesh already been broken up into submeshes? If so, this routine
    // cannot handle this case.
    if (pkMesh->GetSubmeshCount() != 1)
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh does not have 1 submesh.\n");
        return false;
    }

    // Is this an indexed mesh? If not, this routine will not work
    NiDataStream* pkIndices = NULL;
    
    NiDataStreamRef* pkDataStreamRef = pkMesh->FindStreamRef(NiCommonSemantics::INDEX());
    if (pkDataStreamRef != NULL)
        pkIndices = pkDataStreamRef->GetDataStream();

    if (pkIndices == NULL)
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh does not have a INDEX stream.\n");
        return false;
    }

    // Does it have a skinning modifier?
    NiSkinningMeshModifier* pkSkinMod = NiGetModifier(
        NiSkinningMeshModifier, pkMesh);

    if (!pkSkinMod)
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh does not have a skinning modifier attached.\n");
        return false;
    }

    // If the modifier has been set up for software skinning, there is no
    // need to set up for hardware skinning.
    if (pkSkinMod->GetSoftwareSkinned())
    {
        NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. Input "
            "mesh is software skinned.\n");
        return false;
    }

    // Check to see if the remap array already exists. 
    // If it does, it is assumed that the underlying datastream belongs to
    // this cloneset.
    NiDataStreamRef* pkBonesRemapRef = pkMesh->FindStreamRef(
        NiCommonSemantics::BONE_PALETTE());

    NiDataStreamPtr spBoneRemap;

    if (pkBonesRemapRef)
    {
        spBoneRemap = pkBonesRemapRef->GetDataStream();
        NiToolPipelineCloneHelper::CloneSetPtr spTmpCloneSet = NULL;
        NIASSERT(kCloneHelper.GetCloneSet(spBoneRemap, spTmpCloneSet));
        NIASSERT(spTmpCloneSet == spCloneSet);
    }

    NiUInt32 uiNumBones = pkSkinMod->GetBoneCount();
    NiUInt32 uiTriangles = pkMesh->GetPrimitiveCount(0);
    
    // Is the number of bones already less than the number of bones per
    // submesh? If so, the existing submesh should do. Just make sure that 
    // the right data streams are in place for GPU skinning.
    if (uiNumBones <= (NiUInt32) ucBonesPerSubmesh)
    {
        // If the remap array already exists we've already been broken up 
        // into the appropriate number of submeshes so there is nothing
        // more to do.
        if (spBoneRemap != NULL)
        {
            return true;
        }

        // Create the remap buffer data stream
        spBoneRemap = NiDataStream::CreateSingleElementDataStream(
            NiDataStreamElement::F_UINT16_1, uiNumBones, 
            NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_USER);

        if (spBoneRemap == NULL)
        {
            NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. "
                "BONE_PALETTE could not be created.\n");
            return false;
        }

        // Lock the datastream and fill it in, referencing itself.
        NiUInt16* puiRemapBuffer = (NiUInt16*) spBoneRemap->Lock(
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);

        if (puiRemapBuffer == NULL)
        {
            spBoneRemap->Unlock(
                NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);
            NILOG("NiSkinningUtilities::MakeSubmeshesForGPUSkinning failed. "
                "Could not allocate remap buffer.\n");
            return false;
        }

        for (NiUInt32 ui = 0; ui < uiNumBones; ui++)
            puiRemapBuffer[ui] = (NiUInt16)ui;

        spBoneRemap->Unlock(
            NiDataStream::LOCK_WRITE | NiDataStream::LOCK_TOOL_WRITE);

        // Add the datastream to the the set of cloned meshs
        NiDataStreamRef kBoneRef;
        kBoneRef.SetDataStream(spBoneRemap);
        kBoneRef.BindSemanticToElementDescAt(0,
            NiCommonSemantics::BONE_PALETTE(), 0);
        kBoneRef.SetPerInstance(false);
        kCloneHelper.AddStreamRef(spCloneSet, &kBoneRef);

        NiUInt32 uiMeshCnt = spCloneSet->GetSize();
        for(NiUInt32 uiMesh = 0; uiMesh < uiMeshCnt; uiMesh++)
        {
            NiMesh* pkCurrMesh = spCloneSet->GetAt(uiMesh);
            pkCurrMesh->ResetModifiers();
        }

        return true;
    }
    
    // If the remap array already exists, it won't work with hardware
    // limitations. We should just throw it away and start again.
    if (spBoneRemap != NULL)
    {
        for (NiUInt32 ui = 0; ui < pkMesh->GetStreamRefCount(); ui++)
        {
            NiDataStreamRef* pkRef = pkMesh->GetStreamRefAt(ui);
            if (pkRef && pkRef->GetDataStream() == spBoneRemap)
            {
                kCloneHelper.ReleaseStreamRef(pkRef);
                spBoneRemap = NULL;
                break;
            }
        }
    }

    // We have more bones than can fit in a single submesh. We will need
    // to sort the triangles into submeshes that still fit inside the 
    // constraints of number of bones per submesh.
    VertexInteractions* pkVertexInteraction = NULL;
    NiBoneSetCollection kBoneSets;
    NiUInt32* puiTriangleToSubmesh = NULL;
    NiUInt32 uiVertices = pkMesh->GetVertexCount(0);

    // Create Vertex Interactions
    // This is a list of bones with an array per bone 
    // of the vertices it influences
    pkVertexInteraction = CreateVertexInteractions(uiVertices, pkSkinMod,
        pkMesh);

    if (!pkVertexInteraction)
        return false;

    // Remove Excess Interactions
    // Drops the influences to 4 or less bones / vertex and 
    // less than the maximum number of bones per submesh per triangle
    bool bSubmeshes = RemoveExcessInteractions(pkMesh, 
        ucBonesPerSubmesh, gs_uiBonesPerVertex, pkVertexInteraction);

    if (bSubmeshes)
    {
        // Make Bone Sets
        // CreateInitialBoneSets: 
        //      Assign each triangle to a Submesh
        // MergeBoneSets
        //      If any bone sets have less bones than needed and
        //      there is an intersection between two bones sets,
        //      merge them into one bone set.
        MakeBoneSets(pkMesh, ucBonesPerSubmesh,
            pkVertexInteraction, kBoneSets, puiTriangleToSubmesh);

        // Determine the number of submeshes required
        NiUInt32 uiSubmeshes = kBoneSets.GetSize();

        // Create the submesh helper classes
        Submesh* pkSubmeshes = NiNew Submesh[uiSubmeshes];
        NIASSERT(pkSubmeshes != NULL);

        NiUInt32 uiTotalIndicesForSubmeshes = 0;
        NiUInt32 uiTotalVerticesForSubmeshes = 0;

        // Scoping the primitive lock so that it unlocks
        // prior to populating the submeshes.
        {
            NiDataStreamPrimitiveLock kIndexLock(pkMesh, 
                NiDataStream::LOCK_TOOL_READ);

            TriangleSet kTriangles;
            for (NiUInt32 s = 0; s < uiSubmeshes; s++)
            {
                // Add triangles
                //     foreach triangle, put it in the Submesh it
                //     was specified to go into
                kTriangles.AddTriangles(s, puiTriangleToSubmesh, uiTriangles);

                BoneSet* pkBoneSet = kBoneSets.GetAt(s);

                // Get the current submesh
                Submesh& kSubmesh = pkSubmeshes[s];
                
                // Set the triangle count for the submesh
                kSubmesh.m_uiTriangles = kTriangles.GetSize();

                // Set the bone count for the submesh
                NiUInt32 uiNumBonesInSet = pkBoneSet->GetSize();
                kSubmesh.m_uiBones = NiMax((int)uiNumBonesInSet, 
                    (int)gs_uiBonesPerVertex);

                // Initialize the bone lookup array for the submesh
                kSubmesh.m_puiBones = NiAlloc(NiUInt32, kSubmesh.m_uiBones);
                NIASSERT(kSubmesh.m_puiBones != NULL);
                for (NiUInt32 b = 0; b < kSubmesh.m_uiBones; b++)
                {
                    kSubmesh.m_puiBones[b] = (b < pkBoneSet->GetSize()) ? 
                        pkBoneSet->GetAt(b) : 0;
                }

                // Compute the vertex remapping list for the submesh
                NiUInt32* puiVertexSubmeshIndex = CreateVertexMap(uiVertices, 
                    kIndexLock, kTriangles, kSubmesh.m_puiVertexMap,
                    kSubmesh.m_uiVertices);

                // Create the triangle remapping list for the submesh
                kSubmesh.m_puiTriList = CreateTriangleList(kTriangles, 
                    kIndexLock, puiVertexSubmeshIndex);
                NiFree(puiVertexSubmeshIndex);

                // Set the initial offsets in the VB and IB
                kSubmesh.m_uiVBOffsetIndex = uiTotalVerticesForSubmeshes;
                kSubmesh.m_uiIBOffsetIndex = uiTotalIndicesForSubmeshes;

                // Increment counters for later use
                uiTotalIndicesForSubmeshes += (kSubmesh.m_uiTriangles * 3);
                uiTotalVerticesForSubmeshes += kSubmesh.m_uiVertices;

                kTriangles.RemoveAll();
                NiDelete pkBoneSet;
            }
        }

        NiUInt32 uiMeshCnt = spCloneSet->GetSize();
        for(NiUInt32 uiMesh = 0; uiMesh < uiMeshCnt; uiMesh++)
        {
            NiMesh* pkCurrMesh = spCloneSet->GetAt(uiMesh);
            pkCurrMesh->SetSubmeshCount(uiSubmeshes);
        }

        // Convert the existing datastreams to be broken up into the newly 
        // minted submeshes.
        for (NiUInt32 uiSRef = 0; uiSRef < pkMesh->GetStreamRefCount();
            uiSRef++)
        {
            NiDataStreamRef* pkRef = 
                pkMesh->GetStreamRefAt(uiSRef);
            NiDataStreamPtr spDSToConvert = pkRef->GetDataStream();
            NiDataStream::Usage eUsage = spDSToConvert->GetUsage();
            NiDataStreamPtr spConvertedDS;
        
            // Regenerate blend indices and blend weights rather than
            // converting them. All others should be converted as-is.
            if (pkRef->GetSemanticNameAt(0) == 
                NiCommonSemantics::POSITION_BP() ||
                pkRef->GetSemanticNameAt(0) ==
                NiCommonSemantics::NORMAL_BP())
            {
                // Copy a vertex-buffer
                spConvertedDS = NiDataStream::CreateSingleElementDataStream(
                    spDSToConvert->GetElementDescAt(0).GetFormat(),
                    uiTotalVerticesForSubmeshes,
                    spDSToConvert->GetAccessMask(),
                    eUsage, NULL, false, true);

                // Populate the converted datastream with each submesh's 
                // datas
                for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
                {
                    NIVERIFY(pkSubmeshes[ui].Convert(spDSToConvert, 
                        spConvertedDS));
                }
            }
            else if (pkRef->GetSemanticNameAt(0) == 
                NiCommonSemantics::BLENDINDICES())
            {
                spConvertedDS = 
                    NiDataStream::CreateSingleElementDataStream(
                    spDSToConvert->GetElementDescAt(0).GetFormat(),
                    uiTotalVerticesForSubmeshes, 
                    spDSToConvert->GetAccessMask(),
                    eUsage, NULL, false, true);

                // Populate the converted datastream with each submesh's 
                // datas
                for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
                {
                    NIVERIFY(pkSubmeshes[ui].Create(spConvertedDS,
                        pkVertexInteraction, 
                        Submesh::CET_BLEND_INDICES));
                }
            }
            else if (pkRef->GetSemanticNameAt(0) == 
                NiCommonSemantics::BLENDWEIGHT())
            {
                spConvertedDS = 
                    NiDataStream::CreateSingleElementDataStream(
                    spDSToConvert->GetElementDescAt(0).GetFormat(),
                    uiTotalVerticesForSubmeshes, 
                    spDSToConvert->GetAccessMask(),
                    eUsage, NULL, false, true);

                // Populate the converted datastream with each submesh's 
                // datas
                for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
                {
                    NIVERIFY(pkSubmeshes[ui].Create(spConvertedDS,
                        pkVertexInteraction, 
                        Submesh::CET_BLEND_WEIGHTS));
                }
            }               
            else if (pkRef->GetSemanticNameAt(0) == 
                NiCommonSemantics::INDEX())
            {
                // Copy an index buffer
                spConvertedDS = NiDataStream::CreateSingleElementDataStream(
                    spDSToConvert->GetElementDescAt(0).GetFormat(),
                    uiTotalIndicesForSubmeshes, 
                    spDSToConvert->GetAccessMask(),
                    eUsage, NULL, false, true);

                // Populate the converted datastream with each submesh's
                // datas
                for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
                {
                    NIVERIFY(pkSubmeshes[ui].CreateIndexBuffer(
                        spConvertedDS));
                }
            }
            else if (pkRef->GetUsage() == NiDataStream::USAGE_VERTEX &&
                !pkRef->IsPerInstance())
            {
                // Copy a vertex-buffer
                spConvertedDS = NiDataStream::CreateSingleElementDataStream(
                    spDSToConvert->GetElementDescAt(0).GetFormat(),
                    uiTotalVerticesForSubmeshes,
                    spDSToConvert->GetAccessMask(),
                    eUsage, NULL, false, true);

                // Populate the converted datastream with each submesh's 
                // datas
                for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
                {
                    NIVERIFY(pkSubmeshes[ui].Convert(spDSToConvert, 
                        spConvertedDS));
                }
            }

            if (spConvertedDS)
            {
                // Replace the datastream
                kCloneHelper.ReplaceDataStream(pkRef->GetDataStream(),
                    spConvertedDS);

                // Find every datastreamRef that uses the datastream and
                // bind the region to the submesh.
                for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
                {
                    kCloneHelper.BindRegionToSubmesh(pkRef->GetDataStream(),
                        ui, ui);
                }
            }
        }

        // Build the bone remapping buffer
        NiUInt32 uiTotalBoneCount = ((NiUInt32)ucBonesPerSubmesh) *
            uiSubmeshes;

        // Create the bone remap stream.
        spBoneRemap = NiDataStream::CreateSingleElementDataStream(
            NiDataStreamElement::F_UINT16_1, uiTotalBoneCount, 
            NiDataStream::ACCESS_CPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC,
            NiDataStream::USAGE_USER, NULL, false, true);

        // Create the streamref for the bone remap stream
        NiDataStreamRef kBonesRef;
        kBonesRef.SetDataStream(spBoneRemap);
        kBonesRef.BindSemanticToElementDescAt(0,
            NiCommonSemantics::BONE_PALETTE(), 0);
        kBonesRef.SetPerInstance(false);

        // Copy the bone remap indicies in per submesh.
        for (NiUInt32 ui = 0; ui < uiSubmeshes; ui++)
        {
            pkSubmeshes[ui].CreateBoneRemap(spBoneRemap,
                ((NiUInt32)ucBonesPerSubmesh) * ui * sizeof(NiUInt16), 
                ucBonesPerSubmesh);
            kBonesRef.BindRegionToSubmesh(ui, NiDataStream::Region(
                ((NiUInt32)ucBonesPerSubmesh) * ui, ucBonesPerSubmesh));
        }

        kCloneHelper.AddStreamRef(spCloneSet, &kBonesRef);

        NiFree(puiTriangleToSubmesh);
        NiDelete [] pkSubmeshes;
    }

    NiDelete[] pkVertexInteraction;

    NiUInt32 uiMeshCnt = spCloneSet->GetSize();
    for(NiUInt32 uiMesh = 0; uiMesh < uiMeshCnt; uiMesh++)
    {
        NiMesh* pkCurrMesh = spCloneSet->GetAt(uiMesh);
        pkCurrMesh->ResetModifiers();
    }
    return bSubmeshes;
}
//-------------------------------------------------------------------------
bool NiSkinningUtilities::ReorderBonesForSkinnedMeshes(NiNode* pkRootNode)
{
    NiTPointerList<NiAVObject*> kMeshes;
    pkRootNode->GetObjectsByType(&NiMesh::ms_RTTI, kMeshes);

    NiTListIterator kIter = kMeshes.GetHeadPos();
    while (kIter)
    {
        NiMeshPtr spMesh = (NiMesh*)kMeshes.GetNext(kIter);
        const NiSkinningMeshModifier* pkModifier = (NiSkinningMeshModifier*)
            spMesh->GetModifierByType(&NiSkinningMeshModifier::ms_RTTI);
        if (pkModifier)
        {
            // Get the root bones
            NiTPointerList<NiAVObject*> kRootBones;
            pkModifier->GetRootBones(kRootBones);

            NiTListIterator kBoneIter = kRootBones.GetHeadPos();
            while (kBoneIter)
            {
                NiAVObject* pkRootBone = 
                    (NiAVObject*)kRootBones.GetNext(kBoneIter);

                // Find the common parents
                NiAVObjectPtr spChild0 = pkRootBone;
                NiNodePtr spParent0 = pkRootBone->GetParent();
                NiAVObjectPtr spChild1 = spMesh;
                NiNodePtr spParent1 = spMesh->GetParent();
                while (spParent0 != spParent1 && spParent0)
                {
                    spChild1 = spMesh;
                    spParent1 = spMesh->GetParent();
                    while (spParent0 != spParent1 && spParent1)
                    {
                        spChild1 = spParent1;
                        spParent1 = spParent1->GetParent();
                    }

                    if (spParent0 == spParent1)
                        break;

                    spChild0 = spParent0;
                    spParent0 = spParent0->GetParent();
                }

                NIASSERT(spParent0 == spParent1 && spParent0 != NULL);
                spParent0->DetachChild(spChild0);
                spParent0->DetachChild(spChild1);
                spParent0->AttachChild(spChild0, true);
                spParent0->AttachChild(spChild1, true);
            }
        }
    }

    return true;
}
//-------------------------------------------------------------------------


