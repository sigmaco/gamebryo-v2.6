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


#include "NiTerrainPCH.h"

#include <NiAVObject.h>
#include <NiCollisionUtils.h>

#include "NiTerrainUtils.h"
#include "NiTerrainDataLeaf.h"
#include "NiTerrainBlock.h"
#include "NiTerrainVertex.h"
#include "NiTerrainSector.h"

namespace NiTerrainUtils 
{
//---------------------------------------------------------------------------
bool TestRay2D(NiRay& kRay, const NiTerrainDataLeaf* pkDataRoot, 
    NiUInt32 uiDetailLOD)
{
    // create the transformation matrix to go from world space to 
    // terrain model space:    
    NiPoint3 kOrigin = kRay.GetOrigin();
    NiPoint3 kPosition;

    if (!pkDataRoot)
        return false;

    if (uiDetailLOD == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiDetailLOD = pkDataRoot->GetLevel() + pkDataRoot->GetNumSubDivisions();
    }

    NiTransform kTerrainTransform = pkDataRoot->GetContainingSector()->
        GetSectorData()->GetWorldTransform();

    //Test if there are any children
    if (pkDataRoot->GetChildAt(0) && 
        pkDataRoot->GetChildAt(0)->GetLevel() <= uiDetailLOD) 
    {
        //Figure out which child to intersect with
        //Get the center coordinates of the quad
        pkDataRoot->GetVertexAt(kPosition, NiIndex(
            pkDataRoot->GetBlockSize() / 2, pkDataRoot->GetBlockSize() / 2));
        
        if (kOrigin.x < kPosition.x)
        {
            if (kOrigin.y < kPosition.y)
            {
                return TestRay2D(kRay,pkDataRoot->GetChildAt(0), uiDetailLOD);
            }
            else
            {
                return TestRay2D(kRay,pkDataRoot->GetChildAt(3), uiDetailLOD);
            }
        }
        else
        {
            if (kOrigin.y < kPosition.y)
            {
                return TestRay2D(kRay,pkDataRoot->GetChildAt(1), uiDetailLOD);
            }
            else
            {
                return TestRay2D(kRay,pkDataRoot->GetChildAt(2), uiDetailLOD);
            }
        }
    }
    else
    {    
        NiUInt32 uiMaxX = pkDataRoot->GetBlockSize();
        NiUInt32 uiMinX = 0;
        NiUInt32 uiMaxY = pkDataRoot->GetBlockSize();
        NiUInt32 uiMinY = 0;
        NiUInt32 uiMidX =(uiMaxX + uiMinX)/2;
        NiUInt32 uiMidY = (uiMaxY + uiMinY)/2;

        // Keep subdividing until we find the intersection!
        while (uiMidX != uiMaxX && uiMidX != uiMinX)
        {
            // Subdivide the area until we have found the lowest LOD
            pkDataRoot->GetVertexAt(kPosition, NiIndex(uiMidX, uiMidY));
            if (kOrigin.x < kPosition.x)
            {
                uiMaxX = uiMidX;
            }
            else
            {
                uiMinX = uiMidX;
            }

            uiMidX = (uiMaxX + uiMinX)/2;
        }

        // Keep subdividing until we find the intersection!
        while (uiMidY != uiMaxY && uiMidY != uiMinY)
        {
            // Subdivide the area until we have found the lowest LOD
            pkDataRoot->GetVertexAt(kPosition, NiIndex(uiMidX, uiMidY));
            if (kOrigin.y < kPosition.y)
            {
                uiMaxY = uiMidY;
            }
            else
            {
                uiMinY = uiMidY;
            }

            uiMidY = (uiMaxY + uiMinY)/2;
        }

        float fRayDotNormal;
        float fDistFromPlane;
        NiPoint3 kCollisionPoint = NiPoint3::ZERO;

        NiPoint3 k0;
        NiPoint3 k1;
        NiPoint3 k2;
        NiPoint3 k3;

        NiPoint3 kNormal0;
        NiPoint3 kNormal1;

        // We have reached the finest level of detail. Test the two triangles!
        pkDataRoot->GetVertexAt(k0, NiIndex(uiMinX, uiMinY));
        pkDataRoot->GetVertexAt(k1, NiIndex(uiMinX+1, uiMinY));
        pkDataRoot->GetVertexAt(k2, NiIndex(uiMinX, uiMinY+1));

        /*
         * TRIANGLE 1: 012
         */            
        kNormal0 = (k1 - k0).Cross(k2 - k0);
        fRayDotNormal = kRay.GetDirection().Dot(kNormal0);

        // Triangle facing right direction?
        if (fRayDotNormal < 0.0f)
        {
            // Find intersection point on the triangles plane. 
            // All get functions are inlined.
            if (kRay.GetOrigin().z == NI_INFINITY)
            {
                NiPoint3 kNewRay(
                    kOrigin.x,
                    kOrigin.y,
                    NiMax(NiMax((k0).z, (k1).z), (k2).z) + 1.0f);

                fDistFromPlane = 
                    -1.0f * ((kNewRay - k0).Dot(kNormal0) / fRayDotNormal);

                kCollisionPoint = 
                    kNewRay + (fDistFromPlane * kRay.GetDirection());
            }
            else if (kRay.GetOrigin().z == FLT_MIN)
            {
                NiPoint3 kNewRay(
                    kOrigin.x,
                    kOrigin.y,
                    NiMin(NiMin(k0.z, k1.z), k2.z) - 1.0f);

                fDistFromPlane = 
                    -1.0f * ( (kNewRay - k0).Dot(kNormal0) / fRayDotNormal);

                kCollisionPoint = 
                    kNewRay + (fDistFromPlane * kRay.GetDirection());
            }
            else
            {
                fDistFromPlane = 
                    -1.0f * ((kOrigin - k0).Dot(kNormal0) / fRayDotNormal );

                kCollisionPoint = 
                    kOrigin + (fDistFromPlane * kRay.GetDirection());
            }

            // Now see if point is on same side as all 3 triangles
            if ((k1 - k0).Cross
                (kCollisionPoint - k0).Dot(kNormal0) >= - 1e-05f &&
                (k2 - k1).Cross
                (kCollisionPoint - k1).Dot(kNormal0) >= - 1e-05f &&
                (k0 - k2).Cross
                (kCollisionPoint - k2).Dot(kNormal0) >= - 1e-05f) 
            {    
                 NiPoint3 kDistance = kOrigin - kCollisionPoint;
                if (kRay.GetLength() == 0 || 
                    kDistance.Length() < (kRay.GetLength()))
                {
                    kNormal0 = kTerrainTransform.m_Rotate.Inverse() * kNormal0;
                    kNormal0.Unitize();
                    kCollisionPoint = kTerrainTransform * kCollisionPoint;
                    kRay.UseClosestIntersection(kCollisionPoint, kNormal0);
                    kRay.SetCollidedLeaf(pkDataRoot);
                    return true;
                }
            }
        }

        /*
         * TRIANGLE 2: 132
         */

        // Final data point
        pkDataRoot->GetVertexAt(k3, NiIndex(uiMinX+1, uiMinY+1));
        
        kNormal1 = (k2 - k3).Cross(k1 - k3);
        fRayDotNormal = kRay.GetDirection().Dot(kNormal1);

        // Triangle facing right direction?
        if (fRayDotNormal < 0.0f) 
        {
            // Find intersection point on the triangles plane. 
            // Get functions are inlined.
            if (kRay.GetOrigin().z == NI_INFINITY)
            {
                NiPoint3 kNewRay(
                    kOrigin.x,
                    kOrigin.y,
                    NiMax(NiMax(k1.z, k2.z), k3.z) + 1.0f);

                fDistFromPlane = 
                    -1.0f * ( (kNewRay - k1).Dot(kNormal1) / fRayDotNormal);

                kCollisionPoint = 
                    kNewRay + (fDistFromPlane * kRay.GetDirection());
            }
            else if (kRay.GetOrigin().z == FLT_MIN)
            {
                NiPoint3 kNewRay(
                    kOrigin.x,
                    kOrigin.y,
                    NiMin(NiMin(k1.z, k2.z), k3.z) - 1.0f);

                fDistFromPlane = 
                    -1.0f * ((kNewRay - k1).Dot(kNormal1) / fRayDotNormal);
                kCollisionPoint = 
                    kNewRay + (fDistFromPlane * kRay.GetDirection());
            }
            else
            {
                fDistFromPlane = 
                    -1.0f * ((kOrigin - k1).Dot(kNormal1) / fRayDotNormal);
                kCollisionPoint = 
                    kOrigin + (fDistFromPlane * kRay.GetDirection());
            }

            // Now see if point is on same side as all 3 triangles
            if ((k3 - k1).Cross
                (kCollisionPoint - k1).Dot(kNormal1) >= - 1e-05f &&
                (k2 - k3).Cross
                (kCollisionPoint - k3).Dot(kNormal1) >= - 1e-05f &&
                (k1 - k2).Cross
                (kCollisionPoint - k2).Dot(kNormal1) >= - 1e-05f) 
            {
                NiPoint3 kDistance = kOrigin - kCollisionPoint;
                if (kRay.GetLength() == 0 || 
                    kDistance.Length() < kRay.GetLength())
                {
                    kNormal1 = kTerrainTransform.m_Rotate.Inverse() * kNormal1;
                    kNormal1.Unitize();
                    kCollisionPoint = kTerrainTransform * kCollisionPoint;
                    kRay.UseClosestIntersection(kCollisionPoint, kNormal1);
                    kRay.SetCollidedLeaf(pkDataRoot);
                    return true;
                }
            }
        }

        // Check that this ray hasn't intersected perfectly with one
        // of the triangle's verts (May be missed due to floating point error)
        kOrigin = kRay.GetOrigin();
        if (kOrigin.x == k0.x && kOrigin.y == k0.y)
        {
            pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiMinX, uiMinY));
            kRay.UseClosestIntersection(k0, kNormal0);
            return true;
        }
        else if (kOrigin.x == k1.x && kOrigin.y == k1.y)
        {
            pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiMinX+1, uiMinY));
            kRay.UseClosestIntersection(k1, kNormal0);
            return true;
        }
        else if (kOrigin.x == k2.x && kOrigin.y == k2.y)
        {
            pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiMinX, uiMinY+1));
            kRay.UseClosestIntersection(k2, kNormal0);
            return true;
        }
        else if (kOrigin.x == k3.x && kOrigin.y == k3.y)
        {
            pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiMinX+1, uiMinY+1));
            kRay.UseClosestIntersection(k3, kNormal0);
            return true;
        }
        return false;
    }
}
//---------------------------------------------------------------------------
bool TestRay(NiRay& kRay, const NiTerrainDataLeaf* pkDataRoot, 
    NiUInt32 uiDetailLOD)
{
    bool bIntersection = false;

    NiPoint3 kOrigin = kRay.GetOrigin();
    NiTransform kTerrainTransform = 
        pkDataRoot->GetContainingSector()->GetSectorData()
        ->GetWorldTransform();
    
    if (!pkDataRoot) 
    {
        return false;
    }

    if (uiDetailLOD == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiDetailLOD = pkDataRoot->GetLevel() + pkDataRoot->GetNumSubDivisions();
    }

    // Collide with any of the data leaf's children?
    // Using optimization assumption: we will only ever have 0 or 4 children.
    if (pkDataRoot->GetChildAt(0) && 
        pkDataRoot->GetChildAt(0)->GetLevel() <= uiDetailLOD) 
    {
        float afChildDistances[4];
        float fCurDistance;
        const NiTerrainDataLeaf *pkCurChild = NULL;

        // Figure out each child's distance from the RAY origin:
        for (int i = 0; i < 4; ++i)
        {
            pkCurChild = pkDataRoot->GetChildAt((NiUInt16)i);
            afChildDistances[i] = (
                kOrigin - pkCurChild->GetLocalBound().GetCenter()).SqrLength();
        }

        // Loop through all the children in order to find the intersector:
        for (int iIteration = 0; iIteration < 4; ++iIteration)
        {
            // Find the closest child:
            fCurDistance = NI_INFINITY;
            NiUInt32 usChosenIndex = 4;
            for (NiUInt32 ui = 0; ui<4; ++ui)
            {
                if (afChildDistances[ui] < fCurDistance)
                {
                    fCurDistance = afChildDistances[ui];
                    pkCurChild = pkDataRoot->GetChildAt(ui);

                    usChosenIndex = ui;
                }
            }

            if (usChosenIndex == 4)
            {
                // Invalid bound!
                continue;
            }

            // Remove this child's distance from the array:
            afChildDistances[usChosenIndex] = NI_INFINITY;

            const NiBound* pkBound = &pkCurChild->GetLocalBound();

            if (kRay.PassesThroughBound(*pkBound))
            {
                /*
                 * Passes through the bounding sphere, but what about the box?
                 */

                if (!NiBoxBV::BoxTriTestIntersect(
                    0.0f, pkCurChild->GetLocalBoxBound(), NiPoint3::ZERO,
                        kOrigin, kOrigin, 
                        kOrigin + (kRay.GetDirection() * 10000.0f),
                        NiPoint3::ZERO)
                    )
                {
                    continue;
                }

                if (TestRay(kRay, pkCurChild, uiDetailLOD))
                    return true;
            }
        }

        // No children contained matching geometry.
        return false;
    }

    /*
     * Examine our 'collision squares'.
     */
    float fRayDotNormal;
    float fDistFromPlane;
    NiPoint3 kCollisionPoint(0.0f, 0.0f, 0.0f);

    NiPoint3 k0;
    NiPoint3 k1;
    NiPoint3 k2;
    NiPoint3 k3;

    NiPoint3 kNormal0;
    NiPoint3 kNormal1;

    NiUInt32 uiSize = pkDataRoot->GetBlockSize();
    for (NiUInt32 uiY = 0; uiY < uiSize; ++uiY) 
    {
        for (NiUInt32 uiX = 0; uiX < uiSize; ++uiX) 
        {
            pkDataRoot->GetVertexAt(k0, NiIndex(uiX, uiY));
            pkDataRoot->GetVertexAt(k1, NiIndex(uiX+1, uiY));
            pkDataRoot->GetVertexAt(k2, NiIndex(uiX, uiY+1));

            /*
             * TRIANGLE 1: 012
             */            
            kNormal0 = (k1 - k0).UnitCross(k2 - k0);
            fRayDotNormal = kRay.GetDirection().Dot(kNormal0);

            // Triangle facing right direction?
            if (fRayDotNormal < 0.0f) 
            {
                // Find intersection point on the triangles plane. 
                // Get functions are inlined.
                fDistFromPlane = 
                    -1.0f * ((kOrigin - k0).Dot(kNormal0) / fRayDotNormal );
                kCollisionPoint = 
                    kOrigin + (fDistFromPlane * kRay.GetDirection());

                // Now see if point is on same side as all 3 triangles
                if ((k1 - k0).Cross
                    (kCollisionPoint - k0).Dot(kNormal0) >= - 1e-05f &&
                    (k2 - k1).Cross
                    (kCollisionPoint - k1).Dot(kNormal0) >= - 1e-05f &&
                    (k0 - k2).Cross
                    (kCollisionPoint - k2).Dot(kNormal0) >= - 1e-05f) 
                {
                    kCollisionPoint = kTerrainTransform * kCollisionPoint;
                    kNormal0 = kTerrainTransform.m_Rotate.Inverse() * kNormal0;
                    kNormal0.Unitize();

                    bIntersection |= 
                        kRay.UseClosestIntersection(kCollisionPoint, kNormal0);
                }

            }

            /*
             * TRIANGLE 2: 132
             */

            // Final data point
            pkDataRoot->GetVertexAt(k3, NiIndex(uiX+1, uiY+1));
            
            kNormal1 = (k2 - k3).UnitCross(k1 - k3);
            fRayDotNormal = kRay.GetDirection().Dot(kNormal1);

            // Triangle facing right direction?
            if (fRayDotNormal < 0.0f) 
            {
                // Find intersection point on the triangles plane. 
                // Get functions are inlined.
                fDistFromPlane = 
                    -1.0f * ((kOrigin - k1).Dot(kNormal1) / fRayDotNormal);
                kCollisionPoint = 
                    kOrigin + (fDistFromPlane * kRay.GetDirection());

                // Now see if point is on same side as all 3 triangles
                if ((k3 - k1).Cross
                    (kCollisionPoint - k1).Dot(kNormal1) >= - 1e-05f &&
                    (k2 - k3).Cross
                    (kCollisionPoint - k3).Dot(kNormal1) >= - 1e-05f &&
                    (k1 - k2).Cross
                    (kCollisionPoint - k2).Dot(kNormal1) >= - 1e-05f) 
                {
                    kCollisionPoint = kTerrainTransform * kCollisionPoint;
                    kNormal1 = kTerrainTransform.m_Rotate.Inverse() * kNormal1;
                    kNormal1.Unitize();
                    bIntersection |= kRay.UseClosestIntersection(
                        kCollisionPoint, kNormal1);
                }
            }

            // Check that this ray hasn't intersected perfectly with one
            // of the triangle's verts (May be missed due to floating point error)
            kOrigin = kRay.GetOrigin();
            if (kOrigin.x == k0.x && kOrigin.y == k0.y)
            {
                pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiX, uiY));
                kRay.UseClosestIntersection(k0, kNormal0);
            }
            else if (kOrigin.x == k1.x && kOrigin.y == k1.y)
            {
                pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiX+1, uiY));
                kRay.UseClosestIntersection(k1, kNormal0);
            }
            else if (kOrigin.x == k2.x && kOrigin.y == k2.y)
            {
                pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiX, uiY+1));
                kRay.UseClosestIntersection(k2, kNormal0);
            }
            else if (kOrigin.x == k3.x && kOrigin.y == k3.y)
            {
                pkDataRoot->GetNormalAt(kNormal0, NiIndex(uiX+1, uiY+1));
                kRay.UseClosestIntersection(k3, kNormal0);
            }
        }
    }
    
    if(bIntersection)
        kRay.SetCollidedLeaf(pkDataRoot);

    return bIntersection;
}
//---------------------------------------------------------------------------
bool TestBound(const NiBound& kVolume, const NiTerrainDataLeaf* pkDataLeaf)
{
    float fDistDelta = (kVolume.GetCenter() - 
        pkDataLeaf->GetLocalBound().GetCenter()).SqrLength();
    float fRadiusDelta = NiSqr(
        kVolume.GetRadius() + pkDataLeaf->GetLocalBound().GetRadius());

    if (fDistDelta < fRadiusDelta)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
bool TestBound2D(const NiBound& kVolume, const NiTerrainDataLeaf* pkDataLeaf) 
{
    NiPoint2 fDiff(
        kVolume.GetCenter().x - pkDataLeaf->GetLocalBound().GetCenter().x,
        kVolume.GetCenter().y - pkDataLeaf->GetLocalBound().GetCenter().y);

    float fRadiusDiff = NiSqr(kVolume.GetRadius() + 
        pkDataLeaf->GetLocalBound().GetRadius());

    if (NiSqr(fDiff.x) + NiSqr(fDiff.y) < fRadiusDiff)
        return true;
    else
        return false;
}
//----------------------------------------------------------------------------
void GetVertsInBound(const NiBound &kLocalBound,
    NiDeformablePointSet* pkPointSet, NiUInt32& uiNumVerts,
    NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
    NiUInt32 uiDetailLevel, bool bIncludeLowerDetail,
    const NiTerrainDataLeaf* pkDataLeaf) 
{
    if (!pkDataLeaf) 
        return;

    if (uiDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiDetailLevel = pkDataLeaf->GetLevel() + 
            pkDataLeaf->GetNumSubDivisions();
    }

    // If we have children, then all of our points will be contained within 
    // them. Therefore, we don't need to check ourselves.
    if (pkDataLeaf->GetChildAt(0) && pkDataLeaf->GetLevel() < uiDetailLevel) 
    {
        if (TestBound(kLocalBound, pkDataLeaf->GetChildAt(0))) 
        {
            GetVertsInBound( 
                kLocalBound, pkPointSet, uiNumVerts, pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, pkDataLeaf->GetChildAt(0));
        }
        if (TestBound(kLocalBound, pkDataLeaf->GetChildAt(1))) 
        {
            GetVertsInBound(
                kLocalBound, pkPointSet, uiNumVerts, pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, pkDataLeaf->GetChildAt(1));
        }
        if (TestBound(kLocalBound, pkDataLeaf->GetChildAt(2))) 
        {
            GetVertsInBound(
                kLocalBound, pkPointSet, uiNumVerts, pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, pkDataLeaf->GetChildAt(2));
        }
        if (TestBound(kLocalBound, pkDataLeaf->GetChildAt(3))) 
        {
            GetVertsInBound(
                kLocalBound, pkPointSet, uiNumVerts, pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, pkDataLeaf->GetChildAt(3));
        }
    }

    // If we don't want the parent's data, then return.
    if (pkDataLeaf->GetLevel() != uiDetailLevel && !bIncludeLowerDetail)
        return;

    // Now, perform the actual calculation on our vertices.
    NiUInt32 uiSize = pkDataLeaf->GetBlockSize();
    float fDistSqr = 0;
    float fRadiusSquared = NiSqr(kLocalBound.GetRadius());
    NiTerrainVertex* pkVertex = 0;
    NiIndex kIndex(0, 0);
    NiPoint3 kPosition;

    NiTerrainNormalRandomAccessIterator kNormalIter;
    pkDataLeaf->GetNormalIterator(kNormalIter);
    NiIndex kBottomLeftIndex;
    pkDataLeaf->GetBottomLeftIndex(kBottomLeftIndex);

    // Less than the total number of vertices by 1 on each border intentionally
    // to avoid overlap between neighboring blocks.
    for (; kIndex.y <= uiSize; ++kIndex.y) 
    {
        for (; kIndex.x <= uiSize; ++kIndex.x) 
        {
            pkDataLeaf->GetVertexAt(kPosition, kIndex);
            
            fDistSqr = (kLocalBound.GetCenter() - kPosition).SqrLength();

            if (fDistSqr <= fRadiusSquared) 
            {
                NiUInt32 uiPoolIndex = pkOriginalPointPool->GetNew();
                pkOriginalPointPool->GetPointerTo(uiPoolIndex, pkVertex);

                pkDataLeaf->GetVertexAt(*pkVertex, kIndex);

                // Calculate the slope angle for each points
                NiIndex kInd = kIndex - kBottomLeftIndex;
                kInd.x >>= pkDataLeaf->GetLevel();
                kInd.y >>= pkDataLeaf->GetLevel();
                
                NiPoint3 kNormal;
                kNormalIter.GetHighDetail(kInd.x + kInd.y * uiSize, kNormal);
                kNormal.Unitize();
                
                float fSlope = NiACos(kNormal.Dot(NiPoint3::UNIT_Z));
                if (fSlope > NI_HALF_PI)
                    fSlope = NI_PI - fSlope;
                
                ++uiNumVerts;
                pkPointSet->AddPoint(
                    uiPoolIndex,
                    pkVertex->GetHeight(),
                    pkVertex->m_kWorldLocation,
                    0.0f, // Vertex positions have no radius
                    fSlope,
                    NiDeformablePoint::VERTEX);
            }
        }
    }
}
//---------------------------------------------------------------------------
void GetVertsInBound2D(const NiBound &kLocalBound,
    NiDeformablePointSet* pkPointSet, NiUInt32& uiNumVerts,
    NiTQuickReleaseObjectPool<NiTerrainVertex>* pkOriginalPointPool,
    NiUInt32 uiDetailLevel, bool bIncludeLowerDetail,
    const NiTerrainDataLeaf* pkDataLeaf) 
{
    if (!pkDataLeaf) 
        return;

    if (uiDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiDetailLevel = pkDataLeaf->GetLevel() + 
            pkDataLeaf->GetNumSubDivisions();
    }

    // If we have children, then all of our points will be contained within 
    // them. Therefore, we don't need to check ourself afterwards.
    if (pkDataLeaf->GetChildAt(0) && pkDataLeaf->GetLevel() < uiDetailLevel) 
    {
        if (TestBound2D(kLocalBound, pkDataLeaf->GetChildAt(0))) 
        {
            GetVertsInBound2D(kLocalBound, pkPointSet, uiNumVerts, 
                pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, 
                pkDataLeaf->GetChildAt(0) );
        }
        if (TestBound2D(kLocalBound, pkDataLeaf->GetChildAt(1))) 
        {
            GetVertsInBound2D(kLocalBound, pkPointSet, uiNumVerts, 
                pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail,  
                pkDataLeaf->GetChildAt(1) );
        }
        if (TestBound2D(kLocalBound, pkDataLeaf->GetChildAt(2))) 
        {
            GetVertsInBound2D(kLocalBound, pkPointSet, uiNumVerts, 
                pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, 
                pkDataLeaf->GetChildAt(2) );
        }
        if (TestBound2D(kLocalBound, pkDataLeaf->GetChildAt(3))) 
        {
            GetVertsInBound2D(kLocalBound, pkPointSet, uiNumVerts, 
                pkOriginalPointPool,
                uiDetailLevel, bIncludeLowerDetail, 
                pkDataLeaf->GetChildAt(3) );
        }
    }

    // If we don't want the parent's data, then return.
    if (pkDataLeaf->GetLevel() != uiDetailLevel && !bIncludeLowerDetail)
        return;

    // Now, perform the actual calculation on our vertices.
    NiUInt32 uiSize = pkDataLeaf->GetBlockSize();
    NiUInt32 uiHalfSectorSize =
       pkDataLeaf->GetContainingSector()->GetSectorData()->GetSectorSize() / 2;

    NiTerrainVertex* pkVertex = 0;
    NiIndex kIndex;
    pkDataLeaf->GetBottomLeftIndex(kIndex);
    
    NiPoint2 kBottomLeftLocal(
        float(kIndex.x) - float(uiHalfSectorSize),
        float(kIndex.y) - float(uiHalfSectorSize));
    NiPoint2 kTopRightLocal(
        kBottomLeftLocal.x + float(uiSize),
        kBottomLeftLocal.y + float(uiSize));
    float fInc = float(1 << pkDataLeaf->GetNumSubDivisions());

    const NiPoint3& kBoundCenter = kLocalBound.GetCenter();
    float fRadiusSquared = NiSqr(kLocalBound.GetRadius());
    float fDistSqr = 0;
    
    NiTerrainNormalRandomAccessIterator kNormalIter;
    pkDataLeaf->GetNormalIterator(kNormalIter);
    NiIndex kBottomLeftIndex;
    pkDataLeaf->GetBottomLeftIndex(kBottomLeftIndex);

    // Less than the total number of vertices by 1 on each border intentionally
    // to avoid overlap between neighboring blocks.
    for (float y = kBottomLeftLocal.y; y <= kTopRightLocal.y; y += fInc) 
    {
        for (float x = kBottomLeftLocal.x; x <= kTopRightLocal.x; x += fInc) 
        {
            fDistSqr = NiSqr(kBoundCenter.x - x) + NiSqr(kBoundCenter.y - y);

            if (fDistSqr <= fRadiusSquared) 
            {
                kIndex.x = NiUInt32(x) + uiHalfSectorSize;
                kIndex.y = NiUInt32(y) + uiHalfSectorSize;

                NiUInt32 uiPoolIndex = pkOriginalPointPool->GetNew();

                pkOriginalPointPool->GetPointerTo(uiPoolIndex, pkVertex);

                // We have a sector scope index
                pkDataLeaf->GetVertexAtSector(*pkVertex, kIndex);

                // Calculate the slope angle for each points
                NiIndex kInd = kIndex - kBottomLeftIndex;
                kInd.x >>= pkDataLeaf->GetLevel();
                kInd.y >>= pkDataLeaf->GetLevel();

                NiPoint3 kNormal;
                kNormalIter.GetHighDetail(kInd.x + kInd.y * uiSize, kNormal);

                kNormal.Unitize();
                float fSlope = NiACos(kNormal.Dot(NiPoint3::UNIT_Z));
                if (fSlope > NI_HALF_PI)
                    fSlope = NI_PI - fSlope;

                ++uiNumVerts;
                pkPointSet->AddPoint(
                    uiPoolIndex,
                    pkVertex->GetHeight(),
                    pkVertex->m_kWorldLocation,
                    0.0f,
                    fSlope,
                    NiDeformablePoint::VERTEX);
            }
        }
    }
}
//----------------------------------------------------------------------------
void GetPixelsInBound(const NiBound &, NiDeformablePointSet*, NiUInt32&,
    NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>*, const NiSurface*,
    NiUInt32, bool, NiTerrainDataLeaf*)
{ 
}
//---------------------------------------------------------------------------
void GetPixelsInBound2D(const NiBound &kLocalBound,
    NiDeformablePointSet* pkPointSet, NiUInt32& uiNumPixels,
    NiTQuickReleaseObjectPool<NiSurfaceMaskPixel>* pkOriginalPointPool,
    const NiSurface* pkSurface, NiUInt32 uiDetailLevel,
    bool bIncludeLowerDetail, NiTerrainDataLeaf* pkDataLeaf)
{
    if (!pkDataLeaf) 
    {
        return;    
    }
    
    // Go to the parent block if this one does not have any surfaces attached. 
    while ((pkDataLeaf->GetSurfaceCount() == 0 && bIncludeLowerDetail) || 
        pkDataLeaf->GetLevel() > uiDetailLevel)
    {
        pkDataLeaf = pkDataLeaf->GetParent();
        if (!pkDataLeaf)
            return;
    }

    const NiSurfaceMask* pkThisMask = pkDataLeaf->GetSurfaceMask(pkSurface);
    if (!pkThisMask)
        return;

    NiUInt32 uiSurfaceCount = pkDataLeaf->GetSurfaceCount();
    for (NiUInt32 uiPriority = 0; uiPriority < uiSurfaceCount; uiPriority++)
    {
        const NiSurfaceMask* pkMask = pkDataLeaf->GetSurfaceMask(uiPriority);
        if (!pkMask)
            return;

        NiRay kRay(NiPoint3::ZERO, NiPoint3(0.0f, 0.0f, -1.0f));

        // The masks must be power of two dimensions... so therefore division
        // by 2 should result in a whole number
        NIASSERT(pkMask->GetWidth() % 2 == 0);
        NiUInt32 uiHalfWidth = pkMask->GetWidth() / 2;

        NiPoint3 kBottomLeft;
        pkDataLeaf->GetVertexAt(kBottomLeft, NiIndex::ZERO);
        
        
        float fLeafModelWidth = float(
        pkDataLeaf->GetBlockSize() << pkDataLeaf->GetNumSubDivisions());
        float fIncrement = fLeafModelWidth / (float)pkMask->GetWidth();
        float fHalfIncrement = fIncrement / 2.0f;

        float fRadiusSqr = 
            NiSqr(kLocalBound.GetRadius() + fIncrement);

        NiSurfaceMaskPixel* pkPixel;

        float fAccumY = kBottomLeft.y + fHalfIncrement;
        for (NiUInt32 uiY = 0; uiY <= pkMask->GetWidth(); ++uiY)
        {
            float fAccumX = kBottomLeft.x + fHalfIncrement;
            for (NiUInt32 uiX = 0; uiX <= pkMask->GetWidth(); ++uiX)
            {
                // In range? (2D)
                float fDistSqr = NiSqr(kLocalBound.GetCenter().x - fAccumX) + 
                    NiSqr(kLocalBound.GetCenter().y - fAccumY);

                if (fDistSqr > fRadiusSqr)
                {
                    fAccumX += fIncrement;
                    continue;
                }

                if (uiDetailLevel > pkDataLeaf->GetLevel())
                {
                    // Check which child we would correspond too.
                    NiTerrainDataLeaf* pkChild;
                    if (uiX < uiHalfWidth) 
                    {
                        if (uiY < uiHalfWidth) 
                        {
                            // BOTTOM LEFT
                            pkChild = pkDataLeaf->GetChildAt(0);
                        } 
                        else 
                        {
                            // TOP LEFT
                            pkChild = pkDataLeaf->GetChildAt(3);
                        }
                    } 
                    else 
                    {
                        if (uiY < uiHalfWidth) 
                        {
                            // BOTTOM RIGHT
                            pkChild = pkDataLeaf->GetChildAt(1);
                        }
                        else 
                        {
                            // TOP RIGHT
                            pkChild = pkDataLeaf->GetChildAt(2);
                        }
                    }

                    if (pkChild && pkChild->GetSurfaceCount())
                    {
                        const NiSurfaceMask* pkChildMask = 
                            pkChild->GetSurfaceMask(pkSurface);

                        if (pkChildMask)
                        {
                            GetPixelsInBound2D(
                                kLocalBound, pkPointSet, uiNumPixels, 
                                pkOriginalPointPool, pkSurface, 
                                uiDetailLevel, bIncludeLowerDetail, 
                                pkChild);
                        }
                    }
                    else
                    {
                        // A valid child does not exist!
                        NIASSERT(!"Should never reach this point");
                    }
                }
                else if (uiDetailLevel < pkDataLeaf->GetLevel())
                {
                    // Should never reach this point
                    NIASSERT(!"Should never reach this point");
                    return;
                }
                else
                {
                    // Get the world location of the pixel
                    kRay.SetRay(
                        NiPoint3(fAccumX, fAccumY, NI_INFINITY),
                        NiPoint3(0.0f, 0.0f, -1.0f),
                        0.0f);

                    if (!NiTerrainUtils::TestRay2D(kRay, pkDataLeaf))
                        continue;
                    
                    // This is the leaf that we wish to obtain the pixel 
                    // data from
                    NiUInt32 uiPoolIndex = pkOriginalPointPool->GetNew();
                    pkOriginalPointPool->GetPointerTo(uiPoolIndex, pkPixel);

                    pkPixel->SetFirstContainingLeaf(pkDataLeaf);
                    pkPixel->SetMask(pkMask);
                    pkPixel->SetX(uiX);
                    pkPixel->SetY(uiY);

                    NiPoint3 kNormal;
                    NiPoint3 kIntersect;
                    kRay.GetIntersection(kIntersect, kNormal);
                    
                    // Calculate the slope angle for each points                   
                    kNormal.Unitize();
                    float fSlope = NiACos(kNormal.Dot(NiPoint3::UNIT_Z));
                    if (fSlope > NI_HALF_PI)
                        fSlope = NI_PI - fSlope;
                    
                    pkPointSet->AddPoint(
                        uiPoolIndex,
                        pkPixel->GetValue(),
                        kIntersect,
                        fIncrement,
                        fSlope,
                        NiDeformablePoint::MASK_PIXEL,
                        pkMask == pkThisMask);
                    uiNumPixels++;
                }

                fAccumX += fIncrement;
            }

            fAccumY += fIncrement;
        }
    }
}
//---------------------------------------------------------------------------
bool GetSurfaceOpacity(const NiSurface* pkSurface, const NiPoint3& kLocation,
    NiUInt8& ucValue, NiUInt32 uiDetailLevel, bool bSearchLowerDetail,
    const NiTerrainDataLeaf* pkCurDataLeaf)
{
    ucValue = 0;

    const NiTerrainDataLeaf* pkDataLeaf = pkCurDataLeaf;
    if (!pkDataLeaf)
        return false;

    if (uiDetailLevel == NiTerrainUtils::ms_uiMAX_LOD)
    {
        uiDetailLevel = pkCurDataLeaf->GetLevel() + 
            pkCurDataLeaf->GetNumSubDivisions();
    }

    NiTransform kTerrainTransform = pkCurDataLeaf->GetContainingSector()
        ->GetSectorData()->GetWorldTransform();

    const NiPoint3& kModelLoc =  kTerrainTransform.m_Rotate.Inverse() * (
        (kLocation - kTerrainTransform.m_Translate)
        / kTerrainTransform.m_fScale); 
       
    const NiSurfaceMask* pkMask = pkDataLeaf->GetSurfaceMask(pkSurface);
    while ((pkMask == 0 && bSearchLowerDetail) || 
        pkDataLeaf->GetLevel() > uiDetailLevel)
    {
        pkDataLeaf = pkDataLeaf->GetParent();
        if(pkDataLeaf)
        {
            pkMask = pkDataLeaf->GetSurfaceMask(pkSurface);
        }
        else
        {
            pkMask = 0;
            break;
        }
    }

    if (!pkMask) 
        return false;

    if (uiDetailLevel > pkDataLeaf->GetLevel() && pkDataLeaf == pkCurDataLeaf)
    {
        // The detail level we want is one of our children
        NiPoint3 kCenter = pkDataLeaf->GetLocalBound().GetCenter();

        // Find which child we intersect with
        if (kModelLoc.y < kCenter.y)
        {
            // Bottom
            if (kModelLoc.x < kCenter.x)
            {
                // Bottom Left
                return GetSurfaceOpacity(pkSurface, kLocation, ucValue,
                    uiDetailLevel, bSearchLowerDetail, 
                    pkDataLeaf->GetChildAt(0));
            }
            else
            {
                // Bottom Right
                return GetSurfaceOpacity(pkSurface, kLocation, ucValue, 
                    uiDetailLevel, bSearchLowerDetail,
                    pkDataLeaf->GetChildAt(1));
            }
        }
        else
        {
            // Top
            if (kModelLoc.x < kCenter.x)
            {
                // Top Left
                return GetSurfaceOpacity(pkSurface, kLocation, ucValue, 
                    uiDetailLevel, bSearchLowerDetail, 
                    pkDataLeaf->GetChildAt(3));
            }
            else
            {
                // Top Right
                return GetSurfaceOpacity(pkSurface, kLocation, ucValue, 
                    uiDetailLevel, bSearchLowerDetail, 
                    pkDataLeaf->GetChildAt(2));
            }
        }
    }
    else if (uiDetailLevel == pkDataLeaf->GetLevel())
    {
        // The detail level we want is on this child

        // Get the width of the world
        NiPoint3 kTopRight;
        pkDataLeaf->GetVertexAt(kTopRight, 
            NiIndex::UNIT * pkDataLeaf->GetBlockSize());

        NiPoint3 kBottomLeft;
        pkDataLeaf->GetVertexAt(kBottomLeft, NiIndex::ZERO);

        float fLeafWorldWidth = kTopRight.y - kBottomLeft.y;

        // Get the intersection location, in percent
        float fPercentX = NiAbs(1.0f - 
            (kTopRight.x - kModelLoc.x) / fLeafWorldWidth);

        float fPercentY = NiAbs(1.0f - 
            (kTopRight.y - kModelLoc.y) / fLeafWorldWidth);

        // We need to interpolate the values of our surrounding points
        NiUInt32 uiFloorX = (NiUInt32)(
            fPercentX * float(pkMask->GetWidth() - 1));
        NiUInt32 uiFloorY = (NiUInt32)(
            fPercentY * float(pkMask->GetWidth() - 1));

        if (uiFloorX > pkMask->GetWidth() - 1)
            uiFloorX = pkMask->GetWidth() - 1;
        if (uiFloorY > pkMask->GetWidth() - 1)
            uiFloorY = pkMask->GetWidth() - 1;

        NiUInt32 uiCeilX = uiFloorX + 1;
        NiUInt32 uiCeilY = uiFloorY + 1;
        if (uiCeilX > pkMask->GetWidth() - 1)
            uiCeilX = pkMask->GetWidth() - 1;
        if (uiCeilY > pkMask->GetWidth() - 1)
            uiCeilY = pkMask->GetWidth() - 1;

        NiUInt32 uiSum = 
            (NiUInt32)pkMask->GetAt(uiCeilX, uiCeilY) +
            (NiUInt32)pkMask->GetAt(uiCeilX, uiFloorY) +
            (NiUInt32)pkMask->GetAt(uiFloorX, uiCeilY) +
            (NiUInt32)pkMask->GetAt(uiFloorX, uiFloorY);

        ucValue = NiUInt8(uiSum / 4);
        return true;
    }
    else
    {
        // Should never reach this point
        NIASSERT(!"Should never reach this point");
        return false;
    }
}

} // End namespace.
