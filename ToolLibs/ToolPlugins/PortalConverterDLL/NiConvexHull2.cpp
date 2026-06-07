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
#include "NiConvexHull2.h"

//----------------------------------------------------------------------------
NiConvexHull2::NiConvexHull2(int iVQuantity, const NiPoint2* akVertex)
    :
    m_kLineOrigin(NiPoint2::ZERO),
    m_kLineDirection(NiPoint2::ZERO)
{
    NIASSERT(iVQuantity > 0 && akVertex);

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
    for (j = 0; j < 2; j++)
    {
        m_aiIMin[j] = 0;
        m_aiIMax[j] = 0;
    }
    for (int i = 1; i < m_iVQuantity; i++)
    {
        for (j = 0; j < 2; j++)
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
NiConvexHull2::~NiConvexHull2()
{
    NiDelete[] m_akSVertex;
    NiFree(m_aiHIndex);
}
//----------------------------------------------------------------------------
float& NiConvexHull2::DimThreshold()
{
    return m_fDimThreshold;
}
//----------------------------------------------------------------------------
bool NiConvexHull2::DoIncremental()
{
    // Clear out the index array in case this function is called more than
    // one time (for different dimThreshold or uncertainty parameters).
    NiFree(m_aiHIndex);
    m_aiHIndex = 0;

    // Clear out the line parameters in case the hull turns out not to be
    // linear.
    m_kLineOrigin = NiPoint2::ZERO;
    m_kLineDirection = NiPoint2::ZERO;

    // Determine the maximum range for the bounding box.
    NiPoint2 kRange = m_kMax - m_kMin;
    float fMaxRange = kRange[0];
    int i0 = m_aiIMin[0], i1 = m_aiIMax[0];
    if (kRange[1] > fMaxRange)
    {
        fMaxRange = kRange[1];
        i0 = m_aiIMin[1];
        i1 = m_aiIMax[1];
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

    // Translate to box center and uniformly scale to square [-1,1]^2.  The
    // real-valued vertices are computed here.  The rational-valued vertices
    // are computed only when necessary to increase performance.
    float fHalfRange = 0.5f * fMaxRange;
    float fInvHalfRange = 1.0f / fHalfRange;
    int i;
    if (!m_akSVertex)
    {
        NiPoint2 kCenter = 0.5f * (m_kMin + m_kMax);
        m_akSVertex = NiNew NiPoint2[m_iVQuantity];
        for (i = 0; i < m_iVQuantity; i++)
            m_akSVertex[i] = fInvHalfRange * (m_akVertex[i] - kCenter);
    }

    // Test if the hull is (nearly) a line segment.
    NiPoint2 kU0 = m_akSVertex[i1] - m_akSVertex[i0];
    kU0.Unitize();
    NiPoint2 kU1(-kU0.y, kU0.x);
    float fLMax = 0.0f, fMaxSign = 0.0f;
    int i2 = i0;
    for (i = 0; i < m_iVQuantity; i++)
    {
        NiPoint2 kDiff = m_akSVertex[i] - m_akSVertex[i0];
        float fL = kU1.Dot(kDiff);
        float fSign = NiSign(fL);
        fL = NiAbs(fL);
        if (fL > fLMax)
        {
            fLMax = fL;
            fMaxSign = fSign;
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
        return true;
    }

    // Scale the data to [-2^{30},2^{30}]^3 in order to support integer-valued
    // computation of visibility determinants.  The determinants use the
    // truncated values of the vertex components.
    const float fExpand = (float)(1 << 30);
    for (i = 0; i < m_iVQuantity; i++)
        m_akSVertex[i] *= fExpand;

    m_iHDim = 2;

    NiHullEdge2* pkE0;
    NiHullEdge2* pkE1;
    NiHullEdge2* pkE2;

    if (fMaxSign > 0.0f)
    {
        pkE0 = NiNew NiHullEdge2(i0, i1);
        pkE1 = NiNew NiHullEdge2(i1, i2);
        pkE2 = NiNew NiHullEdge2(i2, i0);
    }
    else
    {
        pkE0 = NiNew NiHullEdge2(i0, i2);
        pkE1 = NiNew NiHullEdge2(i2, i1);
        pkE2 = NiNew NiHullEdge2(i1, i0);
    }

    pkE0->Insert(pkE2, pkE1);
    pkE1->Insert(pkE0, pkE2);
    pkE2->Insert(pkE1, pkE0);

    NiHullEdge2* pkHull = pkE0;
    for (i = 0; i < m_iVQuantity; i++)
    {
        if ( !Update(pkHull,i) )
        {
            pkHull->DeleteAll();
            return false;
        }
    }

    pkHull->GetIndices(m_iHQuantity, m_aiHIndex);
    pkHull->DeleteAll();

    return true;
}
//----------------------------------------------------------------------------
int NiConvexHull2::GetDimension() const
{
    return m_iHDim;
}
//----------------------------------------------------------------------------
int NiConvexHull2::GetQuantity() const
{
    return m_iHQuantity;
}
//----------------------------------------------------------------------------
int* NiConvexHull2::GetIndices(bool bTakeOwnership)
{
    int* aiIndices = m_aiHIndex;
    if (bTakeOwnership)
        m_aiHIndex = 0;

    return aiIndices;
}
//----------------------------------------------------------------------------
const NiPoint2& NiConvexHull2::GetLineOrigin() const
{
    return m_kLineOrigin;
}
//----------------------------------------------------------------------------
const NiPoint2& NiConvexHull2::GetLineDirection() const
{
    return m_kLineDirection;
}
//----------------------------------------------------------------------------
bool NiConvexHull2::Update(NiHullEdge2*& pkHull, int i)
{
    // Locate an edge visible to the input point (if possible).
    NiHullEdge2* pkVisible = 0;
    NiHullEdge2* pkCurrent = pkHull;
    do
    {
        if (pkCurrent->GetSign(i, m_akSVertex) > 0)
        {
            pkVisible = pkCurrent;
            break;
        }

        pkCurrent = pkCurrent->A[1];
    }
    while (pkCurrent != pkHull);

    if (!pkVisible)
    {
        // The point is inside the current hull; nothing to do.
        return true;
    }

    // Remove the visible edges.
    NiHullEdge2* pkAdj0 = pkVisible->A[0];
    NIASSERT(pkAdj0);
    if (!pkAdj0)
        return false;

    NiHullEdge2* pkAdj1 = pkVisible->A[1];
    NIASSERT(pkAdj1);
    if (!pkAdj1)
        return false;

    pkVisible->DeleteSelf();

    while (pkAdj0->GetSign(i, m_akSVertex) > 0)
    {
        pkHull = pkAdj0;
        pkAdj0 = pkAdj0->A[0];
        NIASSERT(pkAdj0);
        if (!pkAdj0)
            return false;

        pkAdj0->A[1]->DeleteSelf();
    }

    while (pkAdj1->GetSign(i, m_akSVertex) > 0)
    {
        pkHull = pkAdj1;
        pkAdj1 = pkAdj1->A[1];
        NIASSERT(pkAdj1);
        if (!pkAdj1)
            return false;

        pkAdj1->A[0]->DeleteSelf();
    }

    // Insert the new edges formed by the input point and the end points of
    // the polyline of invisible edges.
    NiHullEdge2* pkEdge0 = NiNew NiHullEdge2(pkAdj0->V[1], i);
    NiHullEdge2* pkEdge1 = NiNew NiHullEdge2(i, pkAdj1->V[0]);
    pkEdge0->Insert(pkAdj0, pkEdge1);
    pkEdge1->Insert(pkEdge0, pkAdj1);
    pkHull = pkEdge0;

    return true;
}
//----------------------------------------------------------------------------
