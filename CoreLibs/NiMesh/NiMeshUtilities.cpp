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
#include <NiTNodeTraversal.h>

#include "NiMesh.h"
#include "NiMeshUtilities.h"
#include "NiMeshVertexOperators.h"
#include "NiToolDataStream.h"
#include "NiTStridedRandomAccessIterator.h"
#include "NiDataStreamElementLock.h"
#include "NiDataStreamLock.h"
#include "NiMeshAlgorithms.h"
#include "NiMeshUtilities.h"
#include "NiPoint2.h"

#include <NiFloat16.h>
#include <NiGeometricUtils.h>

//-------------------------------------------------------------------------
template <typename TPosition>
class ComputeBoundsFromMeshImpl
{
public:
    inline void Execute(NiDataStreamElementLock& kLockPositions, 
        NiBound& kBound)
    {
        // Calculate the bounds of each sub mesh on this mesh. We need to use
        // the correct RegionRef to ensure the bounds are only calculated for
        // this mesh in the situation where the position data stream may be 
        // shared.
        NiUInt32 uiSubmeshCount = kLockPositions.GetSubmeshCount();
        NIASSERT(uiSubmeshCount > 0);

        NiTStridedRandomAccessIterator<TPosition[3]> kBegin =
            kLockPositions.begin<TPosition[3]>();        
        // At least one submesh is assumed. It is used to initialize m_kBound.
        {
            const NiDataStream::Region& kRegion = 
                kLockPositions.GetDataStreamRef()->GetRegionForSubmesh(0);

            NiTStridedRandomAccessIterator<TPosition[3]> kIter = kBegin + 
                kRegion.GetStartIndex();

            ComputeBoundsFromData(kBound, kRegion.GetRange(), kIter);
        }

        for (NiUInt32 ui = 1; ui < uiSubmeshCount; ui++)
        {        
            const NiDataStream::Region& kRegion = 
                kLockPositions.GetDataStreamRef()->GetRegionForSubmesh(ui);

            NiTStridedRandomAccessIterator<TPosition[3]> kIter = kBegin + 
                kRegion.GetStartIndex();

            NiBound kOtherBound;
            ComputeBoundsFromData(kOtherBound, kRegion.GetRange(), kIter);
            kBound.Merge(&kOtherBound);
        }
    }
private:

    inline void ComputeBoundsFromData(NiBound& kBound, NiUInt32 uiQuantity, 
        const NiTStridedRandomAccessIterator<TPosition[3]>& kData)
    {
        if (uiQuantity == 0)
        {
            kBound.SetCenter(NiPoint3::ZERO);
            kBound.SetRadius(0.0f);
            return;
        }

        // compute the axis-aligned box containing the data
        NiPoint3 kMin(kData[0][0], kData[0][1], kData[0][2]);
        NiPoint3 kMax = kMin;
        for (NiUInt32 ui = 1; ui < uiQuantity; ui++) 
        {
            if (kMin.x > kData[ui][0])
                kMin.x = kData[ui][0];
            if (kMin.y > kData[ui][1])
                kMin.y = kData[ui][1];
            if (kMin.z > kData[ui][2])
                kMin.z = kData[ui][2];
            if (kMax.x < kData[ui][0])
                kMax.x = kData[ui][0];
            if (kMax.y < kData[ui][1])
                kMax.y = kData[ui][1];
            if (kMax.z < kData[ui][2])
                kMax.z = kData[ui][2];
        }

        // sphere center is the axis-aligned box center
        NiPoint3 kCenter = 0.5f * (kMin + kMax);

        // compute the radius
        float fRadiusSqr = 0.0f;
        for(NiUInt32 ui = 0; ui < uiQuantity; ui++) 
        {
            NiPoint3 diff(kData[ui][0], kData[ui][1], kData[ui][2]);
            diff -= kCenter;
            float fLengthSqr = diff * diff;
            if (fLengthSqr > fRadiusSqr)
                fRadiusSqr = fLengthSqr;
        }

        kBound.SetRadius(NiSqrt(fRadiusSqr));
        kBound.SetCenter(kCenter);
    }
};
//---------------------------------------------------------------------------
void NiMeshUtilities::ComputeBoundsFromMesh(NiMesh* pkMesh, 
    const bool bForceToolLock)
{
    // Get the verts iterator
    NiUInt8 uiLockFlags = NiDataStream::LOCK_READ;
    if (bForceToolLock)
        uiLockFlags |= NiDataStream::LOCK_TOOL_READ;

    NiDataStreamElementLock kLockPositions(pkMesh,
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_UNKNOWN,
        uiLockFlags);
    if (!kLockPositions.IsLocked())
    {
        NiOutputDebugString("NiMeshUtilities::ComputeBoundsFromMesh: Could "
            "not lock position stream.\n");
        return;
    }
    
    NiBound kBound;
    NiDataStreamElement::Format eFormat = 
        kLockPositions.GetDataStreamElement().GetFormat();

    if (eFormat == NiDataStreamElement::F_FLOAT32_3)
    {
        ComputeBoundsFromMeshImpl<float> kFunctor;
        kFunctor.Execute(kLockPositions, kBound);
    }
    else if (eFormat == NiDataStreamElement::F_FLOAT16_3)
    {
        ComputeBoundsFromMeshImpl<NiFloat16> kFunctor;
        kFunctor.Execute(kLockPositions, kBound);
    }
    else
    {
        NIASSERT(!"Unknown vertex position format in NiMeshUtilities::"
            "ComputeBoundsFromMesh");
        return;
    }

    pkMesh->SetModelBound(kBound);
}

//---------------------------------------------------------------------------
// Internal Declarations for classes used with NiMeshAlgorithms to compute
// normals
//---------------------------------------------------------------------------
template <typename TPosition, typename TNormal>
class CalcNormalLoop
{
public:
    CalcNormalLoop(
        NiDataStreamRef* pkPositionStreamRef,
        NiTStridedRandomAccessIterator<TPosition[3]>& kPositionIter,
        NiDataStreamRef* pkNormalStreamRef,
        NiTStridedRandomAccessIterator<TNormal[3]>& kNormalIter,
        NiDataStreamRef* pkShareGroupsStreamRef,
        NiTStridedRandomAccessIterator<NiUInt32>& kShareGroupsIter,
        NiDataStreamRef* pkShareIndexesStreamRef,
        NiTStridedRandomAccessIterator<NiUInt32>& kShareIndexesIter) :
        m_pkPositionStreamRef(pkPositionStreamRef), 
        m_kPositionIter(kPositionIter),
        m_pkNormalStreamRef(pkNormalStreamRef), m_kNormalIter(kNormalIter), 
        m_pkShareGroupsStreamRef(pkShareGroupsStreamRef),
        m_kShareGroupsIter(kShareGroupsIter),
        m_pkShareIndexesStreamRef(pkShareIndexesStreamRef),
        m_kShareIndexesIter(kShareIndexesIter) {};

    inline bool operator()(const NiUInt32* pIndices, NiUInt32,
        NiUInt32, NiUInt16 uiSubMesh)
    {
        NiUInt32 uiPositionOffset = m_pkPositionStreamRef->GetRegionForSubmesh(
            uiSubMesh).GetStartIndex();
        NiUInt32 ui0 = pIndices[0] + uiPositionOffset;
        NiUInt32 ui1 = pIndices[1] + uiPositionOffset;
        NiUInt32 ui2 = pIndices[2] + uiPositionOffset;

        if (ui0 == ui1 ||
            ui1 == ui2 ||
            ui2 == ui0)
        {
            return true;
        }

        // compute unit length triangle normal
        NiPoint3 kE1(m_kPositionIter[ui1][0] - m_kPositionIter[ui0][0],
            m_kPositionIter[ui1][1] - m_kPositionIter[ui0][1],
            m_kPositionIter[ui1][2] - m_kPositionIter[ui0][2]);
        NiPoint3 kE2(m_kPositionIter[ui2][0] - m_kPositionIter[ui0][0],
            m_kPositionIter[ui2][1] - m_kPositionIter[ui0][1],
            m_kPositionIter[ui2][2] - m_kPositionIter[ui0][2]);
        NiPoint3 kNormal = kE1.UnitCross(kE2);

        // update the running sum of normals at the various positions
        NiUInt32 uiNormalOffset = m_pkNormalStreamRef->
            GetRegionForSubmesh(uiSubMesh).GetStartIndex();

        ui0 = (pIndices[0] + uiNormalOffset);
        ui1 = (pIndices[1] + uiNormalOffset);
        ui2 = (pIndices[2] + uiNormalOffset);

        m_kNormalIter[ui0][0] += kNormal.x;
        m_kNormalIter[ui0][1] += kNormal.y;
        m_kNormalIter[ui0][2] += kNormal.z;

        m_kNormalIter[ui1][0] += kNormal.x;
        m_kNormalIter[ui1][1] += kNormal.y;
        m_kNormalIter[ui1][2] += kNormal.z;

        m_kNormalIter[ui2][0] += kNormal.x;
        m_kNormalIter[ui2][1] += kNormal.y;
        m_kNormalIter[ui2][2] += kNormal.z;

        if (m_kShareGroupsIter.Exists() && m_kShareIndexesIter.Exists())
        {
            NiUInt32 uiShareIndexOffset = m_pkShareIndexesStreamRef->
                GetRegionForSubmesh(uiSubMesh).GetStartIndex();
            NiUInt32 uiShareGroupOffset = m_pkShareGroupsStreamRef->
                GetRegionForSubmesh(uiSubMesh).GetStartIndex();

            NiUInt32 uiIndex;
            NiUInt32 uiInd =
                m_kShareIndexesIter[pIndices[0] + uiShareIndexOffset];
            NiUInt32 uiNumSharing =
                m_kShareGroupsIter[uiShareGroupOffset + uiInd++];
            while (uiNumSharing--)
            {
                uiIndex = m_kShareGroupsIter[uiShareGroupOffset + uiInd++];
                m_kNormalIter[uiIndex][0] += kNormal.x;
                m_kNormalIter[uiIndex][1] += kNormal.y;
                m_kNormalIter[uiIndex][2] += kNormal.z;
            }

            uiInd = m_kShareIndexesIter[pIndices[1] + uiShareIndexOffset];
            uiNumSharing = m_kShareGroupsIter[uiShareGroupOffset + uiInd++];
            while (uiNumSharing--)
            {
                uiIndex = m_kShareGroupsIter[uiShareGroupOffset + uiInd++];
                m_kNormalIter[uiIndex][0] += kNormal.x;
                m_kNormalIter[uiIndex][1] += kNormal.y;
                m_kNormalIter[uiIndex][2] += kNormal.z;
            }
            
            uiInd = m_kShareIndexesIter[pIndices[2] + uiShareIndexOffset];
            uiNumSharing = m_kShareGroupsIter[uiShareGroupOffset + uiInd++];
            while (uiNumSharing--)
            {
                uiIndex = m_kShareGroupsIter[uiShareGroupOffset + uiInd++];
                m_kNormalIter[uiIndex][0] += kNormal.x;
                m_kNormalIter[uiIndex][1] += kNormal.y;
                m_kNormalIter[uiIndex][2] += kNormal.z;
            }
        }

        return true;
    }

    // Data
    NiDataStreamRef* m_pkPositionStreamRef;
    NiTStridedRandomAccessIterator<TPosition[3]>& m_kPositionIter;
    NiDataStreamRef* m_pkNormalStreamRef;
    NiTStridedRandomAccessIterator<TNormal[3]>& m_kNormalIter;
    NiDataStreamRef* m_pkShareGroupsStreamRef;
    NiTStridedRandomAccessIterator<NiUInt32>& m_kShareGroupsIter;
    NiDataStreamRef* m_pkShareIndexesStreamRef;
    NiTStridedRandomAccessIterator<NiUInt32>& m_kShareIndexesIter;

private:
    CalcNormalLoop & operator=( const CalcNormalLoop & );
};
//---------------------------------------------------------------------------
template <typename TPosition, typename TNormal>
class RecalculateNormalsImpl
{
public:
    bool Execute(NiMesh* pkMesh, 
        NiDataStreamElementLock& kLockPositions, 
        NiDataStreamElementLock& kLockNorms, 
        const NiUInt16 uiSubMesh);
};

template <typename TPosition, typename TNormal>
bool RecalculateNormalsImpl<TPosition, 
    TNormal>::Execute(NiMesh* pkMesh, 
    NiDataStreamElementLock& kLockPositions, 
    NiDataStreamElementLock& kLockNorms,
    const NiUInt16 uiSubMesh)
{
    NiTStridedRandomAccessIterator<TPosition[3]> kVertIter =
        kLockPositions.begin<TPosition[3]>();

    NiTStridedRandomAccessIterator<TNormal[3]> kNormIter =
        kLockNorms.begin<TNormal[3]>();

    NiUInt8 uiLockFlags = kLockPositions.GetLockMask();

    // Get the shared group iterator
    NiDataStreamElementLock kLockShareGroups(pkMesh,
        NiCommonSemantics::NORMALSHAREGROUP(), 0,
        NiDataStreamElement::F_UNKNOWN,
        (NiUInt8)uiLockFlags);

    NiTStridedRandomAccessIterator<NiUInt32> kShareGroupsIter;
    if (kLockShareGroups.DataStreamExists())
    {
        if (!kLockShareGroups.IsLocked())
            return false;
        kShareGroupsIter = kLockShareGroups.begin<NiUInt32>();
    }

    // Get the shared index iterator
    NiDataStreamElementLock kLockShareIndexes(pkMesh,
        NiCommonSemantics::NORMALSHAREINDEX(), 0,
        NiDataStreamElement::F_UNKNOWN,
        (NiUInt8)uiLockFlags);
    NiTStridedRandomAccessIterator<NiUInt32> kShareIndexesIter;
    if (kLockShareIndexes.DataStreamExists())
    {
        if (!kLockShareIndexes.IsLocked())
            return false;
        kShareIndexesIter = kLockShareIndexes.begin<NiUInt32>();
    }

    NiUInt32 uiRegionOffset = kLockNorms.GetDataStreamRef()->
        GetRegionForSubmesh(uiSubMesh).GetStartIndex();

    NiUInt32 uiRegionEnd = uiRegionOffset + 
        (kLockNorms.count(uiSubMesh));

    for (NiUInt32 ui = uiRegionOffset; ui < uiRegionEnd; ui++)
    {
        kNormIter[ui][0] = 0;
        kNormIter[ui][1] = 0;
        kNormIter[ui][2] = 0;
    }

    CalcNormalLoop<TPosition, TNormal>
        kCalcNormalLoop(kLockPositions.GetDataStreamRef(), 
        kVertIter,  kLockNorms.GetDataStreamRef(), kNormIter,
        kLockShareGroups.GetDataStreamRef(), kShareGroupsIter,
        kLockShareIndexes.GetDataStreamRef(), kShareIndexesIter);

    NiMeshAlgorithms::ForEachPrimitiveOneSubmesh(pkMesh, uiSubMesh,
        kCalcNormalLoop, (NiUInt8)uiLockFlags);

    // Unitize all normals, binormals, and tangents...
    for (NiUInt32 ui = uiRegionOffset; ui < uiRegionEnd; ui++)
    {
        NiPoint3 kNormal(kNormIter[ui][0], kNormIter[ui][1], 
            kNormIter[ui][2]);
        kNormal.Unitize();
        kNormIter[ui][0] = kNormal.x;
        kNormIter[ui][1] = kNormal.y;
        kNormIter[ui][2] = kNormal.z;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshUtilities::RecalculateNormals(NiMesh* pkMesh,
    const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
    const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex,
    const NiUInt16 uiSubMesh, const bool bForceToolLock)
{
    // Get the verts iterator
    NiUInt8 uiLockReadFlags = NiDataStream::LOCK_READ;
    NiUInt8 uiLockReadWriteFlags = 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE;
    if (bForceToolLock)
    {
        uiLockReadFlags |= NiDataStream::LOCK_TOOL_READ;
        uiLockReadWriteFlags |= 
            (NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    }

    NiDataStreamElementLock kLockPositions(pkMesh,
        kPosnSemantic, uiPosnIndex, NiDataStreamElement::F_UNKNOWN,
        uiLockReadFlags);
    if (!kLockPositions.IsLocked())
    {
        NiOutputDebugString("NiMeshUtilities::RecalculateNormals: Could not "
            "lock position stream.\n");
        return false;
    }

    // Get the normals iterator
    NiDataStreamElementLock kLockNormals(pkMesh,
        kNormSemantic, uiNormIndex, NiDataStreamElement::F_UNKNOWN,
        uiLockReadWriteFlags);
    if (!kLockNormals.IsLocked())
    {
        NiOutputDebugString("NiMeshUtilities::RecalculateNormals: Could not "
            "lock normals stream.\n");
        return false;
    }

    NiDataStreamElement::Format kPositionFormat =
        kLockPositions.GetDataStreamElement().GetFormat();
    NIASSERT(kPositionFormat == NiDataStreamElement::F_FLOAT16_3 ||
        kPositionFormat == NiDataStreamElement::F_FLOAT32_3);

    NiDataStreamElement::Format kNormalFormat =
        kLockNormals.GetDataStreamElement().GetFormat();
    NIASSERT(kNormalFormat == NiDataStreamElement::F_FLOAT16_3 ||
        kNormalFormat == NiDataStreamElement::F_FLOAT32_3);

    if (kPositionFormat == NiDataStreamElement::F_FLOAT32_3)
    {
        if (kNormalFormat == NiDataStreamElement::F_FLOAT32_3)
        {
            RecalculateNormalsImpl<float, float> kFunctor;
            return kFunctor.Execute(pkMesh, kLockPositions, kLockNormals, 
                uiSubMesh);
        }
        else
        {
            RecalculateNormalsImpl<float, NiFloat16> kFunctor;
            return kFunctor.Execute(pkMesh, kLockPositions, kLockNormals, 
                uiSubMesh);
        }
    }
    else
    {
        if (kNormalFormat == NiDataStreamElement::F_FLOAT32_3)
        {
            RecalculateNormalsImpl<NiFloat16, float> kFunctor;
            return kFunctor.Execute(pkMesh, kLockPositions, kLockNormals, 
                uiSubMesh);
        }
        else
        {
            RecalculateNormalsImpl<NiFloat16, NiFloat16> kFunctor;
            return kFunctor.Execute(pkMesh, kLockPositions, kLockNormals, 
                uiSubMesh);
        }
    }
}
//---------------------------------------------------------------------------
bool NiMeshUtilities::CalculateNormals(NiMesh* pkMesh,
   const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
   const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex,
   const NiUInt8 uiAccessMask, const bool bForceToolDSCreate)
{
    // This function should only be called on meshes with no existing normals
    NIASSERT(!pkMesh->FindStreamRef(kNormSemantic, uiNormIndex));

    NiDataStreamRef* pkPosnRef = 
        pkMesh->FindStreamRef(kPosnSemantic, uiPosnIndex);
    NIASSERT(pkPosnRef);

    NiUInt32 uiTotalNorms = pkMesh->GetTotalCount(kPosnSemantic, uiPosnIndex);
    NiDataStreamRef* pkNormRef = pkMesh->AddStream(kNormSemantic, uiNormIndex, 
        NiDataStreamElement::F_FLOAT32_3, uiTotalNorms, uiAccessMask, 
        NiDataStream::USAGE_VERTEX, 0, bForceToolDSCreate, false);

    // Assign regions - they are the same as the vertex stream regions
    NiDataStream* pkNorm = pkNormRef->GetDataStream();
    for (NiUInt16 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        pkNorm->AddRegion(pkPosnRef->GetRegionForSubmesh(ui));
    }

    for (NiUInt16 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        pkNormRef->BindRegionToSubmesh(ui, ui);
    }    

    for (NiUInt16 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
    {
        RecalculateNormals(pkMesh, kPosnSemantic, uiPosnIndex, 
            kNormSemantic, uiNormIndex, ui, bForceToolDSCreate);
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NBT calculation and recalculation for NDL-style.
//---------------------------------------------------------------------------
template <typename TType>
class NDLBasisLoop
{
public:
    NDLBasisLoop(NiTStridedRandomAccessIterator<TType[3]>& kVertIter,
        NiTStridedRandomAccessIterator<TType[3]>& kNormalIter,
        NiTStridedRandomAccessIterator<TType[3]>& kBiNormalIter,
        NiTStridedRandomAccessIterator<TType[3]>& kTangentIter,
        NiTStridedRandomAccessIterator<TType[2]>& kTexCoordIter,
        NiMesh* pkMesh) :
        m_kVertIter(kVertIter), m_kNormalIter(kNormalIter), 
        m_kBiNormalIter(kBiNormalIter), m_kTangentIter(kTangentIter),
        m_kTexCoordIter(kTexCoordIter), m_pkMesh(pkMesh) {};

    inline bool operator()(const NiUInt32* pIndices, NiUInt32,
        NiUInt32, NiUInt16)
    {
        NiUInt32 ui0 = pIndices[0];
        NiUInt32 ui1 = pIndices[1];
        NiUInt32 ui2 = pIndices[2];

        TType* pVert0 = m_kVertIter[ui0];
        TType* pVert1 = m_kVertIter[ui1];
        TType* pVert2 = m_kVertIter[ui2];
        NiPoint3 kV0(pVert0[0], pVert0[1], pVert0[2]);
        NiPoint3 kV1(pVert1[0], pVert1[1], pVert1[2]);
        NiPoint3 kV2(pVert2[0], pVert2[1], pVert2[2]);

        TType* pNorm0 = m_kNormalIter[ui0];
        TType* pNorm1 = m_kNormalIter[ui1];
        TType* pNorm2 = m_kNormalIter[ui2];
        NiPoint3 kN0(pNorm0[0], pNorm0[1], pNorm0[2]);
        NiPoint3 kN1(pNorm1[0], pNorm1[1], pNorm1[2]);
        NiPoint3 kN2(pNorm2[0], pNorm2[1], pNorm2[2]);

        TType* pTC0 = m_kTexCoordIter[ui0];
        TType* pTC1 = m_kTexCoordIter[ui1];
        TType* pTC2 = m_kTexCoordIter[ui2];
        NiPoint2 kT0(pTC0[0], pTC0[1]);
        NiPoint2 kT1(pTC1[0], pTC1[1]);
        NiPoint2 kT2(pTC2[0], pTC2[1]);

        NiPoint3 kBiNormal;
        NiPoint3 kTangent;

        // Get the binormal and tangent
        NiMeshUtilities::FindBT(kV0, kV1, kV2, kT0, kT1, kT2, kBiNormal,
            kTangent);


        TType* pBiNorm = m_kBiNormalIter[ui0];
        pBiNorm[0] += kBiNormal.x;
        pBiNorm[1] += kBiNormal.y;
        pBiNorm[2] += kBiNormal.z;

        pBiNorm = m_kBiNormalIter[ui1];
        pBiNorm[0] += kBiNormal.x;
        pBiNorm[1] += kBiNormal.y;
        pBiNorm[2] += kBiNormal.z;

        pBiNorm = m_kBiNormalIter[ui2];
        pBiNorm[0] += kBiNormal.x;
        pBiNorm[1] += kBiNormal.y;
        pBiNorm[2] += kBiNormal.z;

        TType* pTan = m_kTangentIter[ui0];
        pTan[0] += kTangent.x;
        pTan[1] += kTangent.y;
        pTan[2] += kTangent.z;

        pTan = m_kTangentIter[ui1];
        pTan[0] += kTangent.x;
        pTan[1] += kTangent.y;
        pTan[2] += kTangent.z;

        pTan = m_kTangentIter[ui2];
        pTan[0] += kTangent.x;
        pTan[1] += kTangent.y;
        pTan[2] += kTangent.z;

        return true;
    }

    // Data
    NiTStridedRandomAccessIterator<TType[3]>& m_kVertIter;
    NiTStridedRandomAccessIterator<TType[3]>& m_kNormalIter;
    NiTStridedRandomAccessIterator<TType[3]>& m_kBiNormalIter;
    NiTStridedRandomAccessIterator<TType[3]>& m_kTangentIter;
    NiTStridedRandomAccessIterator<TType[2]>& m_kTexCoordIter;
    NiMesh* m_pkMesh;

private:
    NDLBasisLoop & operator=( const NDLBasisLoop & );
};
//---------------------------------------------------------------------------
template <typename TType>
static bool TRecalculateNBTs_NDL(NiMesh* pkMesh, NiUInt16 usUVSet,
    const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
    const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex,
    const NiFixedString& kBiTangentSemantic,
    const NiFixedString& kTangentSemantic, const NiUInt16 uiSubMesh,
    const bool bForceToolLock)
{
    NiDataStreamElement::Format e2Comp;
    NiDataStreamElement::Format e3Comp;

    size_t stTypeSize = sizeof(TType);
    if (stTypeSize == sizeof(NiFloat16))
    {
        e2Comp = NiDataStreamElement::F_FLOAT16_2;
        e3Comp = NiDataStreamElement::F_FLOAT16_3;
    }
    else if (stTypeSize == sizeof(float))
    {
        e2Comp = NiDataStreamElement::F_FLOAT32_2;
        e3Comp = NiDataStreamElement::F_FLOAT32_3;
    }
    else
    {
        NIASSERT(!"TRecalculateNBTs_NDL can only be used with float types");
        return false;
    }

    NiUInt8 uiLockReadFlags = NiDataStream::LOCK_READ;
    NiUInt8 uiLockReadWriteFlags = 
        NiDataStream::LOCK_READ | NiDataStream::LOCK_WRITE;
    if (bForceToolLock)
    {
        uiLockReadFlags |= NiDataStream::LOCK_TOOL_READ;
        uiLockReadWriteFlags |= 
            (NiDataStream::LOCK_TOOL_READ | NiDataStream::LOCK_TOOL_WRITE);
    }

    // Attempt to lock the UVSet
    NiDataStreamElementLock kLockTexCoords(pkMesh, 
        NiCommonSemantics::TEXCOORD(), usUVSet, e2Comp,
        uiLockReadFlags);
    if (!kLockTexCoords.IsLocked())
        return false;

    // Get the verts iterator
    // Assume that verts are in the format of 3 floats
    NiDataStreamElementLock kLockPositions(pkMesh,
        kPosnSemantic, uiPosnIndex, e3Comp,
        uiLockReadFlags);
    if (!kLockPositions.IsLocked())
        return false;

    NiUInt32 uiVertCount = kLockPositions.count(uiSubMesh);
    NiTStridedRandomAccessIterator<TType[3]> kVertIter =
        kLockPositions.begin<TType[3]>(uiSubMesh);

    // Get the texture coord iterator
    NiTStridedRandomAccessIterator<TType[2]> kTexCoordIter =
        kLockTexCoords.begin<TType[2]>(uiSubMesh);
    NIASSERT(kLockTexCoords.count(uiSubMesh) == uiVertCount);

    // Get the normal iterator
    NiDataStreamElementLock kLockNormals(pkMesh,
        kNormSemantic, uiNormIndex, e3Comp,
        uiLockReadFlags);
    if (!kLockNormals.IsLocked())
        return false;

    NiTStridedRandomAccessIterator<TType[3]> kNormalIter =
        kLockNormals.begin<TType[3]>(uiSubMesh);

    // Get the Binormal iterator
    NiDataStreamElementLock kLockBiNormals(pkMesh,
        kBiTangentSemantic, usUVSet, e3Comp, 
        uiLockReadWriteFlags);
    if (!kLockBiNormals.IsLocked())
        return false;

    NiTStridedRandomAccessIterator<TType[3]> kBiNormalIter =
        kLockBiNormals.begin<TType[3]>(uiSubMesh);

    // Get the Tangent iterator
    NiDataStreamElementLock kLockTangents(pkMesh,
        kTangentSemantic, usUVSet, e3Comp, 
        uiLockReadWriteFlags);
    if (!kLockTangents.IsLocked())
        return false;

    NiTStridedRandomAccessIterator<TType[3]> kTangentIter =
        kLockTangents.begin<TType[3]>(uiSubMesh);

    // Clear out binormals and tangents. Can't use memset because the data
    // may be interleaved with other data.
    for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
    {
        TType* pvBN = kBiNormalIter[ui];
        TType* pvT = kTangentIter[ui];

        pvBN[0] = pvT[0] = 0.0f;
        pvBN[1] = pvT[1] = 0.0f;
        pvBN[2] = pvT[2] = 0.0f;
    }

    NDLBasisLoop<TType> kBasisLoop(kVertIter, kNormalIter, kBiNormalIter,
        kTangentIter, kTexCoordIter, pkMesh);

    NiMeshAlgorithms::ForEachPrimitiveOneSubmesh(pkMesh, uiSubMesh, kBasisLoop,
        uiLockReadFlags);

    // Unitize all normals, binormals, and tangents...
    for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
    {
        // Always convert to float32 for conversions
        TType* pvBN = kBiNormalIter[ui];
        NiPoint3 vBN32(pvBN[0], pvBN[1], pvBN[2]);
        NiPoint3::UnitizeVector(vBN32);
        pvBN[0] = vBN32[0];
        pvBN[1] = vBN32[1];
        pvBN[2] = vBN32[2];

        TType* pvT = kTangentIter[ui];
        NiPoint3 vT32(pvT[0], pvT[1], pvT[2]);
        NiPoint3::UnitizeVector(vT32);
        pvT[0] = vT32[0];
        pvT[1] = vT32[1];
        pvT[2] = vT32[2];
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiMeshUtilities::RecalculateNBTs_NDL(NiMesh* pkMesh, NiUInt16 usUVSet,
    const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
    const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex,
    const NiFixedString& kBiTangentSemantic,
    const NiFixedString& kTangentSemantic, const NiUInt16 uiSubMesh,
    const bool bForceToolLock)
{
    // Assumption is that all streams have been converted to float16 or 
    // none have. Hence, TEXCOORD is used to determine which format to work
    // with.
    NiDataStreamRef* pkDSRef = NULL;
    NiDataStreamElement kElement;

    if (pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::TEXCOORD(), usUVSet,
        NiDataStreamElement::F_FLOAT32_2, pkDSRef, kElement))
    {
        return TRecalculateNBTs_NDL<float>(pkMesh, usUVSet, kPosnSemantic,
            uiPosnIndex, kNormSemantic, uiNormIndex, kBiTangentSemantic,
            kTangentSemantic, uiSubMesh, bForceToolLock);

    }
    else if (pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::TEXCOORD(), usUVSet,
        NiDataStreamElement::F_FLOAT16_2, pkDSRef, kElement))
    {
        return TRecalculateNBTs_NDL<NiFloat16>( pkMesh, usUVSet,
            kPosnSemantic, uiPosnIndex, kNormSemantic,
            uiNormIndex, kBiTangentSemantic, kTangentSemantic, uiSubMesh,
            bForceToolLock);
    }
    return false;
}
//---------------------------------------------------------------------------
void NiMeshUtilities::FindBT(const NiPoint3& kVert0, const NiPoint3& kVert1, 
    const NiPoint3& kVert2, const NiPoint2& kTex0, const NiPoint2& kTex1, 
    const NiPoint2& kTex2, NiPoint3& kBinormal, NiPoint3& kTangent)
{
    // The edge vectors in cartesian coordinates
    NiPoint3 kEC0 = kVert1 - kVert0;
    NiPoint3 kEC1 = kVert2 - kVert0;

    // The edge vectors in texture coordinates
    NiPoint2 kET0 = kTex1 - kTex0;
    NiPoint2 kET1 = kTex2 - kTex0;

    // If this value is 0, then we can't calculate blending values.
    // We'll use the other edge.  If that has a zero value, then the
    // triangle is parametrically degenerate.
    if (kET0.x == 0)
    {
        kET0 = kTex2 - kTex0;
        if (kET0.x == 0)
        {
            kBinormal = kTangent = NiPoint3::ZERO;
            return;
        }

        kEC1 = kVert1 - kVert0;
        kEC0 = kVert2 - kVert0;

        kET1 = kTex1 - kTex0;
    }

    // The weighting factors for the equation:
    // Binormal = a * kEC0 + b * kEC1.  b = kET0.x/m.  
    // Thus calculate fMTerm and check for a 0 value.
    float fMTerm = (kET1.x * kET0.y) - (kET0.x * kET1.y);
    if (fMTerm == 0)
    {
        kBinormal = kTangent = NiPoint3::ZERO;
        return;
    }

    // At this point, it's safe to calculate the full values for a, b, and T
    float fBTerm = kET0.y / fMTerm;
    float fATerm = (1 - (fBTerm * kET1.x))/(kET0.x);

    kTangent = (fATerm * kEC0) + (fBTerm * kEC1);

    // We have to repeat with the binormal vector now which will have
    // different blending values.
    fBTerm = -kET0.x / fMTerm;
    fATerm = (-1.0f * (fBTerm * kET1.x))/(kET0.x);

    kBinormal = (fATerm * kEC0) + (fBTerm * kEC1);

    kTangent.Unitize();
    kBinormal.Unitize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool NiMeshUtilities::ValidateStream(
    const char* pcWhoIsValidating,
    NiMesh* pkMesh, const NiFixedString& kSemantic,
    const NiUInt32 uiSemanticIndex, 
    const NiTPrimitiveArray<NiDataStreamElement::Format>& kFormats,
    NiUInt8 uiRequiredAccessFlags)
{
    NI_UNUSED_ARG(pcWhoIsValidating);
    NiUInt32 uiFormatCount = kFormats.GetSize();

    NiDataStreamRef* pkDSRef = NULL;
    NiDataStreamElement kElement;
    pkMesh->FindStreamRefAndElementBySemantic(
        kSemantic, uiSemanticIndex, NiDataStreamElement::F_UNKNOWN, pkDSRef,
        kElement);

    if (pkDSRef)
    {
        // Check access mask requirements. If multiple ACCESS_CPU_WRITE 
        // flags are specified, at least one must be present.       

        NiUInt8 uiRequiredCPUWriteFlags = static_cast<NiUInt8>
            (uiRequiredAccessFlags & NiDataStream::ACCESS_CPU_WRITE_ANY);
        if (uiRequiredCPUWriteFlags)
        {
            NiUInt8 uiCPUWriteFlags = static_cast<NiUInt8>(
                pkDSRef->GetAccessMask() & NiDataStream::ACCESS_CPU_WRITE_ANY);

            if ((uiRequiredCPUWriteFlags & uiCPUWriteFlags) == 0)
            {
                NILOG("%s requires that the stream %s(%u) is compatible with"
                    " access of 0x%02X (CPU_WRITE mismatch).\n", 
                    pcWhoIsValidating, (const char*)kSemantic, 
                    uiSemanticIndex, uiRequiredAccessFlags);
                return false;
            }
        }

        NiUInt8 uiOtherRequiredFlags = static_cast<NiUInt8>(
            uiRequiredAccessFlags & ~NiDataStream::ACCESS_CPU_WRITE_ANY);

        if ((pkDSRef->GetAccessMask() & uiOtherRequiredFlags) !=
            uiOtherRequiredFlags)
        {
            NILOG("%s requires that the stream %s(%u) is compatible with"
                " access of 0x%02X.\n", pcWhoIsValidating,
                (const char*)kSemantic, uiSemanticIndex,
                uiRequiredAccessFlags);
            return false;
        }

        // Check the format to see if it is one of the supported ones
        bool bInvalid = true;
        for (NiUInt32 ui = 0; ui < uiFormatCount; ui++)
        {
            if (kElement.GetFormat() == kFormats[ui])
            {
                bInvalid = false;
                break;
            }
        }

        if (bInvalid)
        {
            NILOG("%s requires that the stream %s(%u) is compatible with"
                " a format:", pcWhoIsValidating,
                (const char*)kSemantic, uiSemanticIndex);
            for (NiUInt32 ui = 0; ui < uiFormatCount; ui++)
            {
                NILOG(" %s%s_%d", 
                    NiDataStreamElement::IsNormalized(kFormats[ui]) ?
                    "NORM" : "", NiDataStreamElement::GetTypeString(
                    NiDataStreamElement::GetType(kFormats[ui])), 
                    NiDataStreamElement::GetComponentCount(
                    kFormats[ui]));
            }
            NILOG(".\n");
            return false;
        }
    }
    else
    {
        bool bFound = false;
        for (NiUInt32 ui = 0; ui < uiFormatCount; ui++)
        {
            if (kFormats[ui] == NiDataStreamElement::F_UNKNOWN)
            {
                bFound = true;
                break;
            }
        }

        if (bFound)
        {
            NILOG("%s requires a stream with semantic %s(%u)\n",
                pcWhoIsValidating, (const char*)kSemantic, uiSemanticIndex);
        }
        else
        {
            NILOG("%s requires that the stream %s(%u) exists and is compatible"
                " with a format:", pcWhoIsValidating,
                (const char*)kSemantic, uiSemanticIndex);
            for (NiUInt32 ui = 0; ui < uiFormatCount; ui++)
            {
                NILOG(" %s%s_%d", 
                    NiDataStreamElement::IsNormalized(kFormats[ui]) ?
                    "NORM" : "", NiDataStreamElement::GetTypeString(
                    NiDataStreamElement::GetType(kFormats[ui])), 
                    NiDataStreamElement::GetComponentCount(
                    kFormats[ui]));
            }
            NILOG(".\n");
        }

        return false;
    }

    return true;
}
//-------------------------------------------------------------------------
bool NiMeshUtilities::ValidateStream(const char* pcWhoIsValidating,
    NiMesh* pkMesh, const NiFixedString& kSemantic, 
    const NiUInt32 uiSemanticIndex, const NiDataStreamElement::Format& eFormat,
    NiUInt8 uiRequiredAccessFlags)
{
    NiTPrimitiveArray<NiDataStreamElement::Format> kFormats;
    kFormats.Add(eFormat);
    return ValidateStream(pcWhoIsValidating, pkMesh, kSemantic, 
        uiSemanticIndex, kFormats, uiRequiredAccessFlags);
    
}
//---------------------------------------------------------------------------

