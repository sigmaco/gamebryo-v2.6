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
#include "HullManager.h"
#include "NiConvexHull3.h"

//---------------------------------------------------------------------------
HullManager::HullManager(int iNumPoints, NiPoint3* akPoints) :
    m_iNumPoints(iNumPoints),
    m_akPoints(akPoints)
{
    NIASSERT(iNumPoints > 0 && akPoints);

    // Start out only with the axis-aligned bounding box of the input points.
    // This might be a sufficient approximation for bounding the room.  As
    // long as m_akPoints is not null, the HullManager object represents
    // the AABB.

    m_kMin = m_akPoints[0];
    m_kMax = m_kMin;
    m_kAvg = m_kMin;
    for (int i = 1; i < m_iNumPoints; i++)
    {
        const NiPoint3& kPt = m_akPoints[i];
        m_kAvg += kPt;
        for (int j = 0; j < 3; j++)
        {
            if (kPt[j] > m_kMax[j])
                m_kMax[j] = kPt[j];
            else if (kPt[j] < m_kMin[j])
                m_kMin[j] = kPt[j];
        }
    }
    m_kAvg /= (float)m_iNumPoints;

    // unique vertices
    m_iNumVertices = 8;
    m_akVertices = NiNew NiPoint3[8];
    m_akVertices[0] = m_kMin;
    m_akVertices[1] = NiPoint3(m_kMax.x, m_kMin.y, m_kMin.z);
    m_akVertices[2] = NiPoint3(m_kMax.x, m_kMax.y, m_kMin.z);
    m_akVertices[3] = NiPoint3(m_kMin.x, m_kMax.y, m_kMin.z);
    m_akVertices[4] = NiPoint3(m_kMin.x, m_kMin.y, m_kMax.z);
    m_akVertices[5] = NiPoint3(m_kMax.x, m_kMin.y, m_kMax.z);
    m_akVertices[6] = m_kMax;
    m_akVertices[7] = NiPoint3(m_kMin.x, m_kMax.y, m_kMax.z);

    // unique planes
    m_iNumPlanes = 6;
    m_akPlanes = NiNew NiPlane[6];
    m_akPlanes[0] = NiPlane(NiPoint3::UNIT_X, m_kMin.x);
    m_akPlanes[1] = NiPlane(-NiPoint3::UNIT_X, -m_kMax.x);
    m_akPlanes[2] = NiPlane(NiPoint3::UNIT_Y, m_kMin.y);
    m_akPlanes[3] = NiPlane(-NiPoint3::UNIT_Y, -m_kMax.y);
    m_akPlanes[4] = NiPlane(NiPoint3::UNIT_Z, m_kMin.z);
    m_akPlanes[5] = NiPlane(-NiPoint3::UNIT_Z, -m_kMax.z);

    // triangles not needed for box bounds
    m_aiTriangles = 0;

    // unique edges
    m_kEdges.SetAt(EdgeKey(0, 1), NiPoint3::UNIT_X);
    m_kEdges.SetAt(EdgeKey(0, 3), NiPoint3::UNIT_Y);
    m_kEdges.SetAt(EdgeKey(0, 4), NiPoint3::UNIT_Z);
}
//---------------------------------------------------------------------------
HullManager::~HullManager()
{
    NiDelete[] m_akPoints;
    NiDelete[] m_akVertices;
    NiDelete[] m_akPlanes;
    NiFree(m_aiTriangles);
}
//---------------------------------------------------------------------------
int HullManager::GetNumVertices() const
{
    return m_iNumVertices;
}
//---------------------------------------------------------------------------
const NiPoint3* HullManager::GetVertices() const
{
    return m_akVertices;
}
//---------------------------------------------------------------------------
int HullManager::GetNumPlanes() const
{
    return m_iNumPlanes;
}
//---------------------------------------------------------------------------
const NiPlane* HullManager::GetPlanes() const
{
    return m_akPlanes;
}
//---------------------------------------------------------------------------
const int* HullManager::GetTriangles() const
{
    return m_aiTriangles;
}
//---------------------------------------------------------------------------
bool HullManager::UsesHull() const
{
    // The AABB hull is destroyed at the moment it is determined that the
    // full convex hull must be used by the room.
    return (m_akPoints == 0);
}
//---------------------------------------------------------------------------
float HullManager::AreSeparated(HullManager* pkHull)
{
    if (!UsesHull())
    {
        if (!pkHull->UsesHull())
        {
            // Both objects are still using their AABBs as the approximate
            // hull.

            if (AreSeparated(m_kMin, m_kMax, pkHull->m_kMin, pkHull->m_kMax))
            {
                // The AABBs of the hulls do not intersect, so the hulls do
                // not.  At this time it is quite possible that the AABBs may
                // be used as the bounds for the rooms.  The status, though,
                // might change on a later hull-hull comparison.
                return 0.0f;
            }

            // The AABBs intersect.  Both objects now need to use their
            // convex hulls.
            if (ComputeConvexHull() == false)
                return -1.0f;

            if (pkHull->ComputeConvexHull() == false)
                return -1.0f;
        }
        else
        {
            // The first object is using its AABB, the second its hull.
            if (AreSeparated(*this, *pkHull))
            {
                // The first object's AABB does not intersect the second
                // object's hull, so keep the AABB for now.
                return 0.0f;
            }

            // The first object now must use its convex hull.
            if (ComputeConvexHull() == false)
                return -1.0f;
        }
    }
    else if (!pkHull->UsesHull())
    {
        // The first object is using its hull, the second its AABB.
        if (AreSeparated(*this, *pkHull))
        {
            // The first object's hull does not intersect the second
            // object's AABB, so keep the AABB for now.
            return 0.0f;
        }

        // The second object now must use its convex hull.
        if (pkHull->ComputeConvexHull() == false)
            return -1.0f;
    }

    // Test the convex hulls for intersection.
    if (AreSeparated(*this, *pkHull))
        return 0.0f;

    float fMinFraction = EstimatePenetrationFraction(*this, *pkHull);
    NIASSERT(fMinFraction > 0.0f);
    return fMinFraction;
}
//---------------------------------------------------------------------------
HullManager::HullManager(const HullManager&)
{
    // stub to prevent implicit copy construction
}
//---------------------------------------------------------------------------
HullManager& HullManager::operator=(const HullManager&)
{
    // stub to prevent implicit assignment
    return *this;
}
//---------------------------------------------------------------------------
bool HullManager::ComputeConvexHull()
{
    NIASSERT(m_iNumPoints > 0 && m_akPoints);

    // Clear out the axis-aligned box that was stored previously.
    NiDelete[] m_akVertices;
    NiDelete[] m_akPlanes;
    
    m_akVertices = NULL;
    m_akPlanes = NULL;

    // Eliminate nearly duplicate points.  This preprocessing step tends to
    // help out most convex hull constructors.
    EliminateDuplicates(1e-10f);

    // Compute the convex hull.
    NiConvexHull3 kHull(m_iNumPoints, m_akPoints);
    kHull.DimThreshold() = 0.0f;
    bool bSuccess = kHull.DoIncremental();
    if (!bSuccess || kHull.GetDimension() != 3)
        return false;

    m_iNumPlanes = kHull.GetQuantity() / 3;
    m_aiTriangles = kHull.GetIndices(true);

    // Repackage the points to contain only the hull vertices.
    CompactifyHull();

    // Compute the planes of the triangles.  These are used in making the
    // triangle vertex ordering consistent and for the testing of hull
    // intersections.  Flip the triangle normals so they point into the room.
    ComputeConsistentPlanes();

    // Eliminate planes associated with nearly coplanar triangles and with
    // needle-like triangles.
    GenerateUniquePlanes();

    // Generate a list of hull edges for use in hull-hull intersection
    // testing.
    GenerateUniqueEdges();

    // Mark the object as being the true hull by setting m_akPoints to null.
    m_iNumPoints = 0;
    m_akPoints = 0; // NiConvexHull3 is now responsible for deleting.

    return true;
}
//---------------------------------------------------------------------------
void HullManager::EliminateDuplicates(const float fEpsilon)
{
    // Eliminate the duplicate points using the range of the points to
    // define a fuzzy equality test.
    NiPoint3 kDiff = m_kMax - m_kMin;
    float fCutoff = kDiff.SqrLength() * fEpsilon;

    int iDistinct = 0;
    const int iNumPointsM1 = m_iNumPoints - 1;
    for (int i = 0; i < iNumPointsM1; i++)
    {
        const NiPoint3& kPointI = m_akPoints[i];

        int j;
        for (j = i + 1; j < m_iNumPoints; j++)
        {
            const NiPoint3& kPointJ = m_akPoints[j];
            kDiff = kPointI - kPointJ;
            if (kDiff.SqrLength() < fCutoff)
                break;
        }

        if (j >= m_iNumPoints)
            m_akPoints[iDistinct++] = kPointI;
    }

    // The last point cannot be a duplicate.
    m_akPoints[iDistinct++] = m_akPoints[iNumPointsM1];

    // Update the number of points to be the unique number.
    m_iNumPoints = iDistinct;
    NIASSERT(m_iNumPoints > 0);
}
//---------------------------------------------------------------------------
void HullManager::CompactifyHull()
{
    // Repackage the points to contain only the hull vertices.
    int* aiPermute = NiAlloc(int, m_iNumPoints);
    memset(aiPermute, 0xFF, m_iNumPoints * sizeof(int));  // set all to -1
    m_iNumVertices = 0;
    m_akVertices = NiNew NiPoint3[m_iNumPoints];
    for (int i = 0; i < m_iNumPlanes; i++)
    {
        int* aiTri = &m_aiTriangles[3*i];
        for (int j = 0; j < 3; j++)
        {
            int iOldIndex = aiTri[j];
            int& iNewIndex = aiPermute[iOldIndex];

            if (iNewIndex == -1)
            {
                // This is the first time the vertex has been visited.  Add
                // it to the compacted array.
                iNewIndex = m_iNumVertices++;
                m_akVertices[iNewIndex] = m_akPoints[iOldIndex];
            }

            // Map the triangle index to the new vertex location.
            aiTri[j] = iNewIndex;
        }
    }
    NiFree(aiPermute);
}
//---------------------------------------------------------------------------
void HullManager::ComputeConsistentPlanes()
{
    m_akPlanes = NiNew NiPlane[m_iNumPlanes];
    for (int i = 0; i < m_iNumPlanes; i++)
    {
        int* aiTri = &m_aiTriangles[3*i];
        NiPlane& kPlane = m_akPlanes[i];

        kPlane = NiPlane(m_akVertices[aiTri[0]], m_akVertices[aiTri[1]],
            m_akVertices[aiTri[2]]);

        // Create a vector whose direction is from a hull vertex to the
        // centroid of the hull.
        NiPoint3 kInVector = m_kAvg - m_akVertices[aiTri[0]]; 

        // If the triangle normal faces outwards, then reverse the vertex
        // order by swapping the last two indices.
        if (kInVector.Dot(kPlane.GetNormal()) < 0.0f)
        {
            int iSave = aiTri[1];
            aiTri[1] = aiTri[2];
            aiTri[2] = iSave;
            kPlane.SetNormal(-kPlane.GetNormal());
            kPlane.SetConstant(-kPlane.GetConstant());
        }
    }
}
//---------------------------------------------------------------------------
void HullManager::GenerateUniquePlanes()
{
    // The convex hull finder can produce nearly coplanar triangles due to
    // numerical round-off errors.  The finder can also generate needle-like
    // triangles; for example, a triangle associated with three nearly
    // collinear points can do this.  The corresponding plane of such a
    // triangle can be significantly wrong due to numerical round-off errors.
    // These planes must be omitted from the room's wall planes.
    //
    // The error can show up in one of two ways.  Later comments in the code
    // refer to the items here by number.
    //
    // (1) The pre-unitized plane normal vector has small enough length, in
    //     which case the final normal is set to the zero vector.  These
    //     planes must be omitted from the room's wall planes.
    //
    // (2) The unitized normal vector is nonzero, but the plane slices through
    //     the hull, in which case a subset of the hull vertices are outside
    //     the plane (the plane normals are directed inside the room).  To
    //     detect such planes, the signed distances from the hull vertices
    //     are to the plane are calculated.  The most negative distance is
    //     tracked.  If that negative distance is large enough, the plane is
    //     considered to be bogus and omitted from the room's wall planes.
    //     The threshold is
    //         negDistThreshold = -ratio * thickness
    //     where "ratio" is a small positive number and "thickness" is the
    //     largest dimension of the axis-aligned bounding box of the hull.
    //     The ratio chosen here is 0.01, a number that appears to be a
    //     reasonable value.

    float fThickness = m_kMax.x - m_kMin.x;
    float fDiff = m_kMax.y - m_kMin.y;
    if (fDiff > fThickness)
        fThickness = fDiff;
    fDiff = m_kMax.z - m_kMin.z;
    if (fDiff > fThickness)
        fThickness = fDiff;
    const float fRatio = 0.01f;
    const float fNegDistThreshold = -fThickness * fRatio;

    const int iNumPlanesM1 = m_iNumPlanes - 1;

    unsigned int uiInt3Size = 3*sizeof(int);

    int iDistinct = 0;
    for (int i = 0; i < iNumPlanesM1; i++)
    {
        NiPlane& kPlaneI = m_akPlanes[i];

        // See item (1) in the comments above.
        if (kPlaneI.GetNormal() == NiPoint3::ZERO)
            continue;

        // Loop through all planes not yet tested to compare them to the
        // current plane.
        int j;
        for (j = i + 1; j < m_iNumPlanes; j++)
        {
            NiPlane& kPlaneJ = m_akPlanes[j];

            // Tests if two planes have nearly the same normal and constant.
            // The construction assumes that the normal vectors are inner
            // pointing (which they are).  Generally, comparisons need to be
            // between <N0,c0> and <N1,c1> and between <N0,c0> and <-N1,-c1>.

            float fAbsDiffConstants = NiAbs(kPlaneI.GetConstant() -
                kPlaneJ.GetConstant());
            const float fPlaneThreshold = 0.005f;
            if (fAbsDiffConstants >= fPlaneThreshold)
                continue;

            const float fAngleThreshold = 0.9990f;  // about 2.5 degrees
            float fCosAngle = kPlaneI.GetNormal().Dot(kPlaneJ.GetNormal());
            if (fCosAngle > fAngleThreshold)
                break;
        }

        if (j >= m_iNumPlanes) 
        {
            // Plane j is unique.  Add it to the array of unique planes, but
            // first check to make sure tha plane is valid.

            // Compute the signed distance of the hull vertex that is
            // farthest from the candidate plane.
            float fMaxNegDist = 0.0f;
            const NiPoint3* pkVertex = m_akVertices;
            for (int iVertex = 0; iVertex < m_iNumVertices; iVertex++)
            {
                float fSgnDist = kPlaneI.Distance(*pkVertex++);
                if (fSgnDist < fMaxNegDist)
                    fMaxNegDist = fSgnDist;
            }

            // See item (2) in the comments above.
            if (fMaxNegDist < fNegDistThreshold)
                continue;

            // The plane is unique, so keep it.
            int* aiTri0 = &m_aiTriangles[3*i];
            int* aiTri1 = &m_aiTriangles[3*iDistinct];
            NiMemcpy(aiTri1, aiTri0, uiInt3Size);
            m_akPlanes[iDistinct] = m_akPlanes[i];
            iDistinct++;
        }
    }    

    // The last plane cannot be a duplicate.
    int* aiTri0 = &m_aiTriangles[3*iNumPlanesM1];
    int* aiTri1 = &m_aiTriangles[3*iDistinct];
    NiMemcpy(aiTri1, aiTri0, uiInt3Size);
    m_akPlanes[iDistinct] = m_akPlanes[iNumPlanesM1];
    iDistinct++;

    // Update the counter of how many planes we have.
    m_iNumPlanes = iDistinct;
}
//---------------------------------------------------------------------------
void HullManager::GenerateUniqueEdges()
{
    for (int i = 0; i < m_iNumPlanes; i++)
    {
        int* aiTri = &m_aiTriangles[3*i];

        m_kEdges.SetAt(EdgeKey(aiTri[0], aiTri[1]),
            m_akVertices[aiTri[0]] - m_akVertices[aiTri[1]]);

        m_kEdges.SetAt(EdgeKey(aiTri[1], aiTri[2]),
            m_akVertices[aiTri[1]] - m_akVertices[aiTri[2]]);

        m_kEdges.SetAt(EdgeKey(aiTri[2], aiTri[0]),
            m_akVertices[aiTri[2]] - m_akVertices[aiTri[0]]);
    }
}
//---------------------------------------------------------------------------
bool HullManager::AreSeparated (const NiPoint3& kMin0,
    const NiPoint3& kMax0, const NiPoint3& kMin1, const NiPoint3& kMax1)
{
    for (int i = 0; i < 3; i++)
    {
        // Just-touching intervals are considered not to be intersecting.
        if (kMax0[i] <= kMin1[i] || kMax1[i] <= kMin0[i])
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool HullManager::AreSeparated (const HullManager& kHull0,
    const HullManager& kHull1)
{
    // Test facets of hull 0 for possible separation from hull 1.
    int i, iSide0, iSide1;
    for (i = 0; i < kHull0.m_iNumPlanes; i++)
    {
        // Determine if hull 1 vertices are all on the same side of the
        // plane of hull 0 facet i.
        iSide1 = OnSameSide(kHull0.m_akPlanes[i], kHull1.m_iNumVertices,
            kHull1.m_akVertices);
        if (iSide1 < 0)
        {
            // Hull 1 is on the negative side of the facet, so the hulls are
            // separated.
            return true;
        }
    }

    // Test facets of hull 1 for possible separation from hull 0.
    for (i = 0; i < kHull1.m_iNumPlanes; i++)
    {
        // Determine if hull 0 vertices are all on the same side of the
        // plane of hull 1 facet i.
        iSide0 = OnSameSide(kHull1.m_akPlanes[i], kHull0.m_iNumVertices,
            kHull0.m_akVertices);
        if (iSide0 < 0)
        {
            // Hull 0 is on the negative side of the facet, so the hulls are
            // separated.
            return true;
        }
    }

    // Potential separating planes are those whose normals are cross products
    // of two edges, one from each hull.
    NiTMapIterator pkIter0 = kHull0.m_kEdges.GetFirstPos();
    while (pkIter0)
    {
        // Get an edge from hull 0.
        EdgeKey kKey0;
        NiPoint3 kE0;
        kHull0.m_kEdges.GetNext(pkIter0, kKey0, kE0);

        NiTMapIterator pkIter1 = kHull1.m_kEdges.GetFirstPos();
        while (pkIter1)
        {
            // Get an edge from hull 1.
            EdgeKey kKey1;
            NiPoint3 kE1;
            kHull1.m_kEdges.GetNext(pkIter1, kKey1, kE1);

            // Compute a potential separating plane.
            NiPoint3 kNormal = kE0.UnitCross(kE1);
            if (kNormal == NiPoint3::ZERO)
            {
                // The edges are (nearly) parallel, so no separating axis
                // test needs to be performed.  The exact comparison to the
                // zero vector is reasonable, since UnitCross sets its
                // output to the zero vector if the actual cross product has
                // length nearly zero.
                continue;
            }
            NiPlane kPlane(kNormal, kNormal.Dot(kHull0.m_akVertices[
                kKey0.m_aiV[0]]));

            // Determine the sidedness of the hulls relative to the
            // potential separating plane.
            iSide0 = OnSameSide(kPlane, kHull0.m_iNumVertices,
                kHull0.m_akVertices);
            iSide1 = OnSameSide(kPlane, kHull1.m_iNumVertices,
                kHull1.m_akVertices);
            if (iSide0 * iSide1 < 0)
            {
                // The hulls are on opposite sides of the plane, so they are
                // separated.
                return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
int HullManager::OnSameSide (const NiPlane& kPlane, int iVQuantity,
    const NiPoint3* pkPoint)
{
    int iPosSide = 0, iNegSide = 0;

    for (int i = 0; i < iVQuantity; i++)
    {
        const float fDistanceThreshold = 0.01f;
        float fDistance = kPlane.Distance(*pkPoint++);
        if (fDistance > fDistanceThreshold)
            iPosSide++;
        else if (fDistance < -fDistanceThreshold)
            iNegSide++;

        if (iPosSide && iNegSide)
        {
            // The plane splits the point set.
            return 0;
        }
    }

    return iPosSide ? +1 : -1;
}
//---------------------------------------------------------------------------
float HullManager::EstimatePenetrationFraction (
    const HullManager& kHull0, const HullManager& kHull1)
{
    // Estimate the penetration fraction by computing the smallest interval
    // of intersection for the projections of the hulls onto the potential
    // separating axes, and dividing that length by the total width of the
    // union of the projection intervals.

    float fMinFraction = NI_INFINITY, fFraction;
    float fMin0, fMax0, fMin1, fMax1, fMinI, fMaxI, fMinU, fMaxU;
    int i, iQuantity;

    // Project the hulls onto normal lines for the hull 0 planes.
    for (i = 0; i < kHull0.m_iNumPlanes; i++)
    {
        ComputeProjection(kHull0.m_akPlanes[i],kHull0.m_iNumVertices,
            kHull0.m_akVertices, fMin0, fMax0);
        ComputeProjection(kHull0.m_akPlanes[i],kHull1.m_iNumVertices,
            kHull1.m_akVertices, fMin1, fMax1);
        iQuantity = GetIntervalInfo(fMin0, fMax0, fMin1, fMax1,
            fMinI, fMaxI, fMinU, fMaxU);
        if (iQuantity == 2)
        {
            fFraction = (fMaxI - fMinI) / (fMaxU - fMinU);
            if (fFraction < fMinFraction)
                fMinFraction = fFraction;
        }
    }

    // Project the hulls onto normal lines for the hull 1 planes.
    for (i = 0; i < kHull1.m_iNumPlanes; i++)
    {
        ComputeProjection(kHull1.m_akPlanes[i],kHull0.m_iNumVertices,
            kHull0.m_akVertices, fMin0, fMax0);
        ComputeProjection(kHull1.m_akPlanes[i],kHull1.m_iNumVertices,
            kHull1.m_akVertices, fMin1, fMax1);
        iQuantity = GetIntervalInfo(fMin0, fMax0, fMin1, fMax1,
            fMinI, fMaxI, fMinU, fMaxU);
        if (iQuantity == 2)
        {
            fFraction = (fMaxI - fMinI) / (fMaxU - fMinU);
            if (fFraction < fMinFraction)
                fMinFraction = fFraction;
        }
    }

    // Project the hulls onto normal lines for the planes determined by
    // cross products of two edges, one from each hull.
    NiTMapIterator pkIter0 = kHull0.m_kEdges.GetFirstPos();
    while (pkIter0)
    {
        // Get an edge from hull 0.
        EdgeKey kKey0;
        NiPoint3 kE0;
        kHull0.m_kEdges.GetNext(pkIter0, kKey0, kE0);

        NiTMapIterator pkIter1 = kHull1.m_kEdges.GetFirstPos();
        while (pkIter1)
        {
            // Get an edge from hull 1.
            EdgeKey kKey1;
            NiPoint3 kE1;
            kHull1.m_kEdges.GetNext(pkIter1, kKey1, kE1);

            // Compute a plane whose normal line is used for the hull
            // projection.
            NiPoint3 kNormal = kE0.UnitCross(kE1);
            if (kNormal == NiPoint3::ZERO)
            {
                continue;
            }
            NiPlane kPlane(kNormal, kNormal.Dot(kHull0.m_akVertices[
                kKey0.m_aiV[0]]));

            ComputeProjection(kPlane,kHull0.m_iNumVertices,
                kHull0.m_akVertices, fMin0, fMax0);
            ComputeProjection(kPlane,kHull1.m_iNumVertices,
                kHull1.m_akVertices, fMin1, fMax1);
            iQuantity = GetIntervalInfo(fMin0, fMax0, fMin1, fMax1,
                fMinI, fMaxI, fMinU, fMaxU);
            if (iQuantity == 2)
            {
                fFraction = (fMaxI - fMinI) / (fMaxU - fMinU);
                if (fFraction < fMinFraction)
                    fMinFraction = fFraction;
            }
        }
    }

    return (fMinFraction > 0.0f ? fMinFraction : 0.0f);
}
//---------------------------------------------------------------------------
void HullManager::ComputeProjection(const NiPlane& kPlane,
    int iVQuantity, const NiPoint3* pkPoint, float& fLower, float& fUpper)
{
    fLower = kPlane.Distance(*pkPoint++);
    fUpper = fLower;
    for (int i = 1; i < iVQuantity; i++)
    {
        float fDistance = kPlane.Distance(*pkPoint++);
        if (fDistance > fUpper)
            fUpper = fDistance;
        else if (fDistance < fLower)
            fLower = fDistance;
    }
}
//---------------------------------------------------------------------------
int HullManager::GetIntervalInfo(float fMin0, float fMax0, float fMin1,
    float fMax1, float& fMinI, float& fMaxI, float& fMinU, float& fMaxU)
{
    // compute the intersection
    int iQuantity;

    if (fMax0 < fMin1 || fMin0 > fMax1)
    {
        iQuantity = 0;
    }
    else if (fMax0 > fMin1)
    {
        if (fMin0 < fMax1)
        {
            iQuantity = 2;
            fMinI = (fMin0 < fMin1 ? fMin1 : fMin0);
            fMaxI = (fMax0 > fMax1 ? fMax1 : fMax0);
            if (fMinI == fMaxI)
                iQuantity = 1;
        }
        else  // fMin0 == fMax1
        {
            iQuantity = 1;
            fMinI = fMin0;
            fMaxI = fMinI;
        }
    }
    else  // fMax0 == fMin1
    {
        iQuantity = 1;
        fMinI = fMax0;
        fMaxI = fMinI;
    }

    // compute the union
    fMinU = (fMin0 <= fMin1 ? fMin0 : fMin1);
    fMaxU = (fMax0 >= fMax1 ? fMax0 : fMax1);

    return iQuantity;
}
//---------------------------------------------------------------------------
