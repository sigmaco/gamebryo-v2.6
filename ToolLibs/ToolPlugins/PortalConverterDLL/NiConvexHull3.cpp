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

#include "StdAfx.h"
#include "NiConvexHull3.h"
#include "NiConvexHull2.h"
#include "TerminatorDataMap.h"

//----------------------------------------------------------------------------
NiConvexHull3::NiConvexHull3(int iVQuantity, const NiPoint3* akVertex)
    :
    m_kLineOrigin(NiPoint3::ZERO),
    m_kLineDirection(NiPoint3::ZERO),
    m_kPlaneOrigin(NiPoint3::ZERO),
    m_kPlaneNormal(NiPoint3::ZERO)
{
    NIASSERT( iVQuantity > 0 && akVertex );

    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_fDimThreshold = 0.001f;
    m_iHDim = 0;
    m_iHQuantity = 0;
    m_aiHIndex = 0;

    m_akSVertex = 0;

    // Compute the axis-aligned bounding box for the input points.
    m_kMin = m_akVertex[0];
    m_kMax = m_kMin;

    int j;
    for (j = 0; j < 3; j++)
    {
        m_aiIMin[j] = 0;
        m_aiIMax[j] = 0;
    }
    for (int i = 1; i < m_iVQuantity; i++)
    {
        for (j = 0; j < 3; j++)
        {
            if (m_akVertex[i][j] < m_kMin[j])
            {
                m_kMin[j] = m_akVertex[i][j];
                m_aiIMin[j] = i;
            }
            else if (m_akVertex[i][j] > m_kMax[j])
            {
                m_kMax[j] = m_akVertex[i][j];
                m_aiIMax[j] = i;
            }
        }
    }
}
//----------------------------------------------------------------------------
NiConvexHull3::~NiConvexHull3()
{
    NiDelete[] m_akSVertex;
    NiDelete[] m_akVertex;
    NiFree(m_aiHIndex);
}
//----------------------------------------------------------------------------
float& NiConvexHull3::DimThreshold()
{
    return m_fDimThreshold;
}
//----------------------------------------------------------------------------
bool NiConvexHull3::DoIncremental()
{
    // Clear out the index array in case this function is called more than
    // one time (for different dimThreshold or uncertainty parameters).
    NiFree(m_aiHIndex);
    m_aiHIndex = 0;

    // Clear out the line and plane parameters in case the hull turns out not
    // to be linear or planar.
    m_kLineOrigin = NiPoint3::ZERO;
    m_kLineDirection = NiPoint3::ZERO;
    m_kPlaneOrigin = NiPoint3::ZERO;
    m_kPlaneNormal = NiPoint3::ZERO;

    // Determine the maximum range for the bounding box.
    NiPoint3 kRange = m_kMax - m_kMin;
    float fMaxRange = kRange[0];
    int i0 = m_aiIMin[0], i1 = m_aiIMax[0];
    if (kRange[1] > fMaxRange)
    {
        fMaxRange = kRange[1];
        i0 = m_aiIMin[1];
        i1 = m_aiIMax[1];
    }
    if (kRange[2] > fMaxRange)
    {
        fMaxRange = kRange[2];
        i0 = m_aiIMin[2];
        i1 = m_aiIMax[2];
    }

    // Test if the hull is (nearly) a point.
    if (fMaxRange < m_fDimThreshold)
    {
        m_iHDim = 0;
        m_iHQuantity = 1;
        m_aiHIndex = NiAlloc(int, 1);
        m_aiHIndex[0] = 0;
        return true;
    }

    // Translate to box center and uniformly scale to cube [-1,1]^3.  The
    // real-valued vertices are computed here.  The rational-valued vertices
    // are computed only when necessary to increase performance.
    float fHalfRange = 0.5f * fMaxRange;
    float fInvHalfRange = 1.0f / fHalfRange;
    int i;
    if (!m_akSVertex)
    {
        NiPoint3 kCenter = 0.5f * (m_kMin + m_kMax);
        m_akSVertex = NiNew NiPoint3[m_iVQuantity];
        for (i = 0; i < m_iVQuantity; i++)
            m_akSVertex[i] = fInvHalfRange * (m_akVertex[i] - kCenter);
    }

    // Test if the hull is (nearly) a line segment.
    NiPoint3 kU0 = m_akSVertex[i1] - m_akSVertex[i0];
    kU0.Unitize();
    float fLMax = 0.0f, fL, fDot;
    NiPoint3* akDiff = NiNew NiPoint3[m_iVQuantity];
    int i2 = i0;
    for (i = 0; i < m_iVQuantity; i++)
    {
        akDiff[i] = m_akSVertex[i] - m_akSVertex[i0];
        fDot = kU0.Dot(akDiff[i]);
        NiPoint3 kProj = akDiff[i] - fDot * kU0;
        fL = kProj.Length();
        if (fL > fLMax)
        {
            fLMax = fL;
            i2 = i;
        }
    }

    if (fLMax < m_fDimThreshold)
    {
        m_iHDim = 1;
        m_iHQuantity = 2;
        m_aiHIndex = NiAlloc(int, 2);
        m_aiHIndex[0] = i0;
        m_aiHIndex[1] = i1;
        m_kLineOrigin = m_akVertex[i0];
        m_kLineDirection = kU0;
        NiDelete[] akDiff;
        return true;
    }

    // Test if the hull is (nearly) a convex polygon.
    NiPoint3 kU1 = m_akSVertex[i2] - m_akSVertex[i0];
    fDot = kU0.Dot(kU1);
    kU1 -= fDot * kU0;
    kU1.Unitize();
    NiPoint3 kU2 = kU0.Cross(kU1);
    fLMax = 0.0f;
    float fMaxSign = 0.0f, fSign;
    int i3 = i0;
    for (i = 0; i < m_iVQuantity; i++)
    {
        fL = kU2.Dot(akDiff[i]);
        fSign = NiSign(fL);
        fL = NiAbs(fL);
        if (fL > fLMax)
        {
            fLMax = fL;
            fMaxSign = fSign;
            i3 = i;
        }
    }

    if (fLMax < m_fDimThreshold)
    {
        // Project points into the U0/U1 plane.
        NiPoint2* akPVertex = NiNew NiPoint2[m_iVQuantity];
        for (i = 0; i < m_iVQuantity; i++)
        {
            akPVertex[i][0] = kU0.Dot(akDiff[i]);
            akPVertex[i][1] = kU1.Dot(akDiff[i]);
        }
        NiDelete[] akDiff;

        m_kPlaneOrigin = m_akVertex[i0];
        m_kPlaneNormal = kU2;

        NiConvexHull2 kHull2(m_iVQuantity, akPVertex);
        bool bSuccess = kHull2.DoIncremental();
        NIASSERT(bSuccess);
        m_iHDim = kHull2.GetDimension();
        m_iHQuantity = kHull2.GetQuantity();
        m_aiHIndex = kHull2.GetIndices(true);
        NiDelete[] akPVertex;
        return bSuccess;
    }

    NiDelete[] akDiff;

    // Scale the data to [-2^{20},2^{20}]^3 in order to support integer-valued
    // computation of visibility determinants.  The determinants use the
    // truncated values of the vertex components.
    const float fExpand = (float)(1 << 20);
    for (i = 0; i < m_iVQuantity; i++)
        m_akSVertex[i] *= fExpand;

    m_iHDim = 3;

    NiHullTriangle3* pkT0;
    NiHullTriangle3* pkT1;
    NiHullTriangle3* pkT2;
    NiHullTriangle3* pkT3;

    if (fMaxSign > 0.0f)
    {
        pkT0 = NiNew NiHullTriangle3(i0, i1, i3);
        pkT1 = NiNew NiHullTriangle3(i0, i2, i1);
        pkT2 = NiNew NiHullTriangle3(i0, i3, i2);
        pkT3 = NiNew NiHullTriangle3(i1, i2, i3);
        pkT0->AttachTo(pkT1, pkT3, pkT2);
        pkT1->AttachTo(pkT2, pkT3, pkT0);
        pkT2->AttachTo(pkT0, pkT3, pkT1);
        pkT3->AttachTo(pkT1, pkT2, pkT0);
    }
    else
    {
        pkT0 = NiNew NiHullTriangle3(i0, i3, i1);
        pkT1 = NiNew NiHullTriangle3(i0, i1, i2);
        pkT2 = NiNew NiHullTriangle3(i0, i2, i3);
        pkT3 = NiNew NiHullTriangle3(i1, i3, i2);
        pkT0->AttachTo(pkT2, pkT3, pkT1);
        pkT1->AttachTo(pkT0, pkT3, pkT2);
        pkT2->AttachTo(pkT1, pkT3, pkT0);
        pkT3->AttachTo(pkT0, pkT2, pkT1);
    }

    m_kHull.RemoveAll();
    m_uiOrder = 0;
    m_kHull.SetAt(pkT0, m_uiOrder++);
    m_kHull.SetAt(pkT1, m_uiOrder++);
    m_kHull.SetAt(pkT2, m_uiOrder++);
    m_kHull.SetAt(pkT3, m_uiOrder++);

    NiHullTriangle3* pkHull = pkT0;
    for (i = 0; i < m_iVQuantity; i++)
    {
        if (!Update(pkHull,i))
        {
            DeleteHull();
            return false;
        }
    }

    ExtractIndices();
    return true;
}
//----------------------------------------------------------------------------
int NiConvexHull3::GetDimension() const
{
    return m_iHDim;
}
//----------------------------------------------------------------------------
int NiConvexHull3::GetQuantity() const
{
    return m_iHQuantity;
}
//----------------------------------------------------------------------------
int* NiConvexHull3::GetIndices(bool bTakeOwnership)
{
    int* aiIndices = m_aiHIndex;
    if (bTakeOwnership)
        m_aiHIndex = 0;

    return aiIndices;
}
//----------------------------------------------------------------------------
const NiPoint3& NiConvexHull3::GetLineOrigin() const
{
    return m_kLineOrigin;
}
//----------------------------------------------------------------------------
const NiPoint3& NiConvexHull3::GetLineDirection() const
{
    return m_kLineDirection;
}
//----------------------------------------------------------------------------
const NiPoint3& NiConvexHull3::GetPlaneOrigin() const
{
    return m_kPlaneOrigin;
}
//----------------------------------------------------------------------------
const NiPoint3& NiConvexHull3::GetPlaneNormal() const
{
    return m_kPlaneNormal;
}
//----------------------------------------------------------------------------
bool NiConvexHull3::Update(NiHullTriangle3*&, int i)
{
    // Locate a triangle visible to the input point (if possible).
    NiHullTriangle3* pkVisible = 0;
    NiHullTriangle3* pkTri = 0;

#ifndef DEBUG_DETERMINISTIC
    int iDummy;
    NiTMapIterator pkIter = m_kHull.GetFirstPos();
    while (pkIter)
    {
        m_kHull.GetNext(pkIter, pkTri, iDummy);

        if (pkTri->GetSign(i, m_akSVertex) > 0  )
        {
            pkVisible = pkTri;
            break;
        }
    }
#else
    // If DEBUG_DETERMINISTIC
    // Build a deterministic ordering...
    NiTPrimitiveArray<NiHullTriangle3*> kHullArray;
    FillAndSortHullArray(kHullArray);

    for(unsigned int uiElem=0; uiElem<kHullArray.GetSize(); uiElem++)
    {
        pkTri = kHullArray.GetAt(uiElem);
        if (!pkTri)
            continue;

        if (pkTri->GetSign(i, m_akSVertex) > 0  )
        {
            pkVisible = pkTri;
            break;
        }
    }
#endif

    if (!pkVisible)
    {
        // The point is inside the current hull; nothing to do.
        return true;
    }

    // Locate and remove the visible triangles.  A stack is used to support
    // depth-first traversal in Update(...).  An NiTArray is used since there
    // is no NiTStack class.
    NiTPrimitiveArray<NiHullTriangle3*> kVisible;
    int iTop = 0;
    kVisible.SetAtGrow(0,pkVisible);
    pkVisible->OnStack = true;

    // The edges of the polyline separating visible from invisible triangles.
    TerminatorDataMap kTerminator;

    int j, iV0, iV1;
    while (iTop >= 0)
    {
        pkTri = kVisible.GetAt(iTop--);
        pkTri->OnStack = false;
        for (j = 0; j < 3; j++)
        {
            NiHullTriangle3* pkAdj = pkTri->A[j];
            if (pkAdj)
            {
                // Detach triangle and adjacent triangle from each other.
                int iNullIndex = pkTri->DetachFrom(j,pkAdj);

                if (pkAdj->GetSign(i, m_akSVertex) > 0)
                {
                    if (!pkAdj->OnStack)
                    {
                        // Adjacent triangle is visible.
                        kVisible.SetAtGrow(++iTop,pkAdj);
                        pkAdj->OnStack = true;
                    }
                }
                else
                {
                    // Adjacent triangle is invisible.
                    iV0 = pkTri->V[j];
                    iV1 = pkTri->V[(j+1)%3];
                    
                    TerminatorData kDummyTerminator;
                    bool bSuccess = kTerminator.GetAt(iV0, kDummyTerminator);
                    if (bSuccess)
                    {
                        NIASSERT(!"Mal-formed ConvexHull Topology.");
                        return false;
                    }

                    kTerminator.SetAt(iV0, TerminatorData(iV0, iV1,
                        iNullIndex, pkAdj));
                }

            }
        }
        m_kHull.RemoveAt(pkTri);
        NiDelete pkTri;
    }

    // Insert the new edges formed by the input point and the terminator
    // between visible and invisible triangles.
    int iSize = (int)kTerminator.GetCount();

    // Topology is incorrect, this means that the hull code must have
    // mis-classfied a point
    if (iSize < 3)
    {
        NIASSERT(!"Mal-formed ConvexHull Topology.");
        return false;
    }

    TerminatorData kData;
    NiTMapIterator pkEdge = kTerminator.GetFirstPos();
    kTerminator.GetNext(pkEdge, iV0, kData);
    iV1 = kData.V[1];
    pkTri = NiNew NiHullTriangle3(i, iV0, iV1);
    m_kHull.SetAt(pkTri, m_uiOrder++);

    // save information for linking first/last inserted new triangles
    int iSaveV0 = iV0;
    NiHullTriangle3* pkSaveTri = pkTri;

    // establish adjacency links across terminator edge
    pkTri->A[1] = kData.Tri;
    kData.Tri->A[kData.NullIndex] = pkTri;
    for (j = 1; j < iSize; j++)
    {
        bool bFound = kTerminator.GetAt(iV1, kData);

        // Topology is incorrect, this means that the hull code must have
        // mis-classfied a point
        if (!bFound)
        {
            NIASSERT(!"Mal-formed ConvexHull Topology.");
            return false;
        }

        iV0 = iV1;
        iV1 = kData.V[1];
        NiHullTriangle3* pkNext = NiNew NiHullTriangle3(i, iV0, iV1);
        m_kHull.SetAt(pkNext, m_uiOrder++);

        // establish adjacency links across terminator edge
        pkNext->A[1] = kData.Tri;
        kData.Tri->A[kData.NullIndex] = pkNext;

        // establish adjacency links with previously inserted triangle
        pkNext->A[0] = pkTri;
        pkTri->A[2] = pkNext;

        pkTri = pkNext;
    }

    // Topology is incorrect, this means that the hull code must have
    // mis-classfied a point
    if (iV1 != iSaveV0)
    {
        NIASSERT(!"Mal-formed ConvexHull Topology.");
        return false;
    }

    // establish adjacency links between first/last triangles
    pkSaveTri->A[0] = pkTri;
    pkTri->A[2] = pkSaveTri;

    return true;
}
//----------------------------------------------------------------------------
void NiConvexHull3::ExtractIndices()
{
    int iTQuantity = (int)m_kHull.GetCount();
    m_iHQuantity = 3*iTQuantity;
    m_aiHIndex = NiAlloc(int, m_iHQuantity);

    NiHullTriangle3* pkTri = 0;
    int i = 0;

#ifndef DEBUG_DETERMINISTIC
    int iDummy;
    NiTMapIterator pkIter = m_kHull.GetFirstPos();
    while (pkIter)
    {
        m_kHull.GetNext(pkIter, pkTri, iDummy);
        for (int j = 0; j < 3; j++)
            m_aiHIndex[i++] = pkTri->V[j];
        NiDelete pkTri;
    }
    m_kHull.RemoveAll();
#else
    // Build a deterministic ordering...
    NiTPrimitiveArray<NiHullTriangle3*> kHullArray;
    FillAndSortHullArray(kHullArray);

    for(unsigned int uiElem=0; uiElem<kHullArray.GetSize(); uiElem++)
    {
        pkTri = kHullArray.GetAt(uiElem);
        if (!pkTri)
            continue;

        for (int j = 0; j < 3; j++)
            m_aiHIndex[i++] = pkTri->V[j];
        NiDelete pkTri;
    }
    m_kHull.RemoveAll();
#endif
}
//----------------------------------------------------------------------------
void NiConvexHull3::DeleteHull()
{
    NiHullTriangle3* pkTri;
    int iDummy;
    NiTMapIterator pkIter = m_kHull.GetFirstPos();
    while (pkIter)
    {
        m_kHull.GetNext(pkIter, pkTri, iDummy);
        NiDelete pkTri;
    }
    m_kHull.RemoveAll();
}
//----------------------------------------------------------------------------
void NiConvexHull3::FillAndSortHullArray(
    NiTPrimitiveArray<NiHullTriangle3*>& kHullArray)
{
    NiTPrimitiveArray<int> kOrderArray;

    kHullArray.RemoveAll();

    // Add all NiHullTriangle3's to kHullArray
    NiTMapIterator pkIter = m_kHull.GetFirstPos();

    unsigned int uiHighestV[3] = { 0, 0, 0 };

    while (pkIter)
    {
        NiHullTriangle3* pkTri;
        int iDummy;
        m_kHull.GetNext(pkIter, pkTri, iDummy);

        for(int i=0; i<3; i++)
        {
            if ((unsigned int)pkTri->V[i] > uiHighestV[i])
                uiHighestV[i] = (unsigned int)pkTri->V[i];
        }

        kHullArray.Add(pkTri);
        kOrderArray.Add(iDummy);
    }

    // Make hashing scheme where V0 is weighted most, then V1, then V2
    // V2 will just have a multiplier of 1
    // V1 will have the highest value of v2 + 1
    // V0 will have (V1 * highest value V1 + 1) + highest value of V2
    unsigned int uiV1Mult = uiHighestV[2] + 1;
    unsigned int uiV0Mult = uiV1Mult * (uiHighestV[1] + 1) + uiHighestV[2];

    // Now sort the hull array based on info in NiHullTriangle3 elements
    unsigned int uiHullSize = kHullArray.GetEffectiveSize();
    for (unsigned int uiIndex=0; uiIndex < uiHullSize; uiIndex++)
    {
        NiHullTriangle3* pkTri = kHullArray.GetAt(uiIndex);
        int iTriOrder = kOrderArray.GetAt(uiIndex);

        unsigned int uiC1 = (pkTri->V[0] * uiV0Mult) + 
            (pkTri->V[1] * uiV1Mult) + pkTri->V[2];

        NIASSERT(pkTri);

        for (unsigned int uiSort=uiIndex + 1; uiSort < uiHullSize; uiSort++)
        {
            NiHullTriangle3* pkNextTri = kHullArray.GetAt(uiSort);
            int iNextTriOrder = kOrderArray.GetAt(uiSort);

            unsigned int uiC2 = (pkNextTri->V[0] * uiV0Mult) + 
                (pkNextTri->V[1] * uiV1Mult) + pkNextTri->V[2];

            // if (uiC1 > uiC2) // Sort base on order added
            if (iTriOrder > iNextTriOrder) // Sort base on V[0], V[1], V[2]
            {
                kHullArray.SetAt(uiSort, pkTri);
                kHullArray.SetAt(uiIndex, pkNextTri);
                pkTri = pkNextTri;
                uiC1 = uiC2;

                kOrderArray.SetAt(uiSort, iTriOrder);
                kOrderArray.SetAt(uiIndex, iNextTriOrder);
                iTriOrder = iNextTriOrder;
            }
        }
    }

    NiOutputDebugString("----------\n");
    for(unsigned int uiElem=0; uiElem < kHullArray.GetSize(); uiElem++)
    {
        NiHullTriangle3* pkTri = kHullArray.GetAt(uiElem);

        char acBuf[512];
        NiSprintf(acBuf, 512, "kHullArray[%d].pkTri->V = %d,%d,%d\n",
            uiElem, pkTri->V[0], pkTri->V[1], pkTri->V[2]);
        NiOutputDebugString(acBuf);
    }
}
//----------------------------------------------------------------------------
