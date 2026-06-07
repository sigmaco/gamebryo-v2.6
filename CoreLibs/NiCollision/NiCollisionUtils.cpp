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


#include "NiCollisionPCH.h" // Precompiled header.

#include "NiBoundingVolume.h"
#include "NiCapsuleBV.h"
#include "NiBoxBV.h"
#include "NiHalfSpaceBV.h"
#include "NiUnionBV.h"
#include "NiSphereBV.h"
#include "NiCollisionData.h"
#include "NiCollisionUtils.h"
#include <NiNode.h>
#include "NiTriIntersect.h"
#include <NiMesh.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiMeshAlgorithms.h>

namespace NiCollisionUtils
{

//---------------------------------------------------------------------------
// File scope helpers
//---------------------------------------------------------------------------
inline bool GetIndices(const NiUInt32* pIndices, NiUInt32& ui0,
    NiUInt32& ui1, NiUInt32& ui2)
{
    ui0 = *pIndices++;
    ui1 = *pIndices++;
    ui2 = *pIndices;

    if (ui0 == ui1 || ui0 == ui2 || ui1 == ui2)
        return false;   // Degenerate case.

    return true;
}
//---------------------------------------------------------------------------
// The kRecord return values for this are a bit different than the outer-level
// intersection tests.  If the ray begins behind the plane, the intersection
// point is not set to the ray origin, but the "out" intersection, if one
// exists (i.e. return value is true) -- This is necessary for box picking
// and should be noted for half-space picking, since one cannot simply pass
// through to this method.
bool NICOLLISION_ENTRY FindRayPlaneHelper(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection,
    const float fPlaneDistance, const NiPoint3 kPlaneNormal,
    NiPick::Record& kRecord)
{
    // find distance from ray origin to plane
    float fRayDist = kPlaneNormal * kRayOrigin - fPlaneDistance;
    if (fRayDist <= 0)
    {
        // ray begins inside plane
        kRecord.SetRayInsideABV(true);
    }
    else
    {
        kRecord.SetRayInsideABV(false);
    }

    float denom = kPlaneNormal.Dot(kRayDirection);
    if (denom == 0) // ray is parallel to plane
    {
        kRecord.SetDistance(-1);
        return false;   // No intersection - outside half space.
    }

    float fTime = (fPlaneDistance - kPlaneNormal.Dot(kRayOrigin)) / denom;

    kRecord.SetIntersection(kRayOrigin + (fTime * kRayDirection));
    kRecord.SetNormal(kRayDirection);
    kRecord.SetDistance(fTime);
    
    return true;
}
//---------------------------------------------------------------------------
bool NICOLLISION_ENTRY FindRaySphereHelper(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection,
    const NiPoint3& kSphereCenter, const float fSphereRadius,
    NiPick::Record& kRecord)
{
    // Check if ray begins inside sphere.
    float fRayToCenterSqr = (kSphereCenter - kRayOrigin).SqrLength();
    if (fRayToCenterSqr < fSphereRadius * fSphereRadius)
    {
        kRecord.SetRayInsideABV(true);
        kRecord.SetIntersection(kRayOrigin);
        kRecord.SetNormal(kRayDirection);
        kRecord.SetDistance(0);
        return true;
    }

    NiPoint3 m = kRayOrigin - kSphereCenter;
    float b = m.Dot(kRayDirection);
    float c = m.Dot(m) - fSphereRadius * fSphereRadius;

    if (c > 0.0f && b > 0.0f)
    {
        kRecord.SetRayInsideABV(false);
        kRecord.SetDistance(-1);
        return false;
    }

    float discr = b * b - c;

    if (discr < 0.0f)
    {
        kRecord.SetRayInsideABV(false);
        kRecord.SetDistance(-1);
        return false;
    }

    float t = -b - NiSqrt(discr);
    if (t < 0.0f) 
        t = 0.0f;
    NiPoint3 kIntersect = kRayOrigin + t * kRayDirection;
    NiPoint3 kNormal = kIntersect - kSphereCenter;
    kNormal.Unitize();
    kRecord.SetIntersection(kIntersect);
    kRecord.SetNormal(kNormal);
    kRecord.SetDistance(t);
    kRecord.SetRayInsideABV(false);

    return true;
}
//---------------------------------------------------------------------------
// Internal Declarations for classes used with NiMeshAlgorithms
//---------------------------------------------------------------------------
class TriToBndVolBase
{
    public:
        TriToBndVolBase(bool& bCollision, float fTime) :
          m_bCollision(bCollision), m_fTime(fTime), 
          m_iReturn(NiCollisionGroup::CONTINUE_COLLISIONS) {}

        bool &m_bCollision;
        float m_fTime;
        int m_iReturn;

        const NiPoint3* m_pkTriWorldVerts;
        NiBoundingVolume* m_pkABV;
        NiPoint3 m_kABVWorldVelocity;
        NiPoint3 m_kTriWorldVelocity;
        NiCollisionData::PropagationMode m_eABVPropagation;
        NiCollisionData::PropagationMode m_eTriPropagation;
        NiCollisionData* m_pkTriData;
    
    private:
        TriToBndVolBase & operator=( const TriToBndVolBase & );
};
//---------------------------------------------------------------------------
class TriToBndVolTest : public TriToBndVolBase
{
public:
    TriToBndVolTest(bool &bCollision, float fTime) :
      TriToBndVolBase(bCollision, fTime) {};
    bool operator()(const NiUInt32* pIndices, NiUInt32 uiCount, 
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    TriToBndVolTest & operator=( const TriToBndVolTest & );
};
//---------------------------------------------------------------------------
class TriToBndVolFind : public TriToBndVolBase
{
public:
    TriToBndVolFind(NiCollisionGroup::Intersect& kIntr, bool &bCollision,
        bool bCalcNormals, float fTime) : TriToBndVolBase(bCollision, fTime),
        m_kIntr(kIntr), m_bCalcNormals(bCalcNormals){};

    NiCollisionGroup::Intersect& m_kIntr;
    bool m_bCalcNormals;
    bool m_bTriDataIsFirst;

    bool operator()(const NiUInt32* pIndices, NiUInt32 uiCount,
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    TriToBndVolFind & operator=( const TriToBndVolFind & );
};
//---------------------------------------------------------------------------
class TriTriTestOuter
{
public:
    TriTriTestOuter(float& fTime, bool& bCollision) : m_fTime(fTime),
        m_bCollision(bCollision) 
    { m_iReturn = NiCollisionGroup::CONTINUE_COLLISIONS; }

    float& m_fTime;
    bool& m_bCollision;
    int m_iReturn;

    const NiPoint3* m_pkMesh1WorldVerts;
    const NiPoint3* m_pkMesh2WorldVerts;
    NiMesh* m_pMesh2;
    NiPoint3 m_kMesh1WorldVelocity;
    NiPoint3 m_kMesh2WorldVelocity;

    bool operator()(const NiUInt32* pIndices, NiUInt32 uiCount,
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    TriTriTestOuter & operator=( const TriTriTestOuter & );
};
//---------------------------------------------------------------------------
class TriTriFindOuter
{
public:
    TriTriFindOuter(NiCollisionGroup::Intersect& kIntr,
        bool bCalcNormals, float& fTime, bool& bCollision) : 
        m_kIntr(kIntr), m_bCalcNormals(bCalcNormals),
        m_fTime(fTime), m_bCollision(bCollision) 
    { m_iReturn = NiCollisionGroup::CONTINUE_COLLISIONS; }

    NiCollisionGroup::Intersect& m_kIntr;
    bool m_bCalcNormals;

    float& m_fTime;
    bool& m_bCollision;
    int m_iReturn;

    const NiPoint3* m_pkMesh1WorldVerts;
    const NiPoint3* m_pkMesh2WorldVerts;
    NiPoint3 m_kMesh1WorldVelocity;
    NiPoint3 m_kMesh2WorldVelocity;
    NiCollisionData::PropagationMode m_eMesh1Propagation;
    NiCollisionData::PropagationMode m_eMesh2Propagation;
    NiMesh* m_pkMesh1;
    NiMesh* m_pkMesh2;

    NiCollisionData* m_pkData1;
    const NiPoint3* apkVerts[3];

    bool operator()(const NiUInt32* pIndices, NiUInt32 uiCount,
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    TriTriFindOuter & operator=( const TriTriFindOuter & );
};
//---------------------------------------------------------------------------
class TriTriTestInner
{
public:
    TriTriTestInner(TriTriTestOuter& kTestOuter) : m_kTestOuter(kTestOuter){}

    TriTriTestOuter& m_kTestOuter;
    const NiPoint3* m_apkVerts[3];

    bool operator()(const NiUInt32* pIndices, NiUInt32 uiCount,
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    TriTriTestInner & operator=( const TriTriTestInner & );
};
//---------------------------------------------------------------------------
class TriTriFindInner
{
public:
    TriTriFindInner(TriTriFindOuter& kFindOuter) : m_kFindOuter(kFindOuter){}

    TriTriFindOuter& m_kFindOuter;

    bool operator()(const NiUInt32* pIndices, NiUInt32 uiCount,
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    TriTriFindInner & operator=( const TriTriFindInner & );
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functor operator() definitions for classes used with NiMeshAlgorithms
//---------------------------------------------------------------------------
bool TriToBndVolTest::operator()(const NiUInt32* pIndices, NiUInt32,
    NiUInt32,
    NiUInt16 uiSubMesh)
{
    NI_UNUSED_ARG(uiSubMesh);
    // These functions only support single-sub-mesh meshes.
    NIASSERT(uiSubMesh == 0);
    
    // Check for degenerate case.
    NiUInt32 ui0, ui1, ui2;
    if (GetIndices(pIndices, ui0, ui1, ui2) == false)
        return true;    // Continue.

    if (NiBoundingVolume::TestTriIntersect(m_fTime, *m_pkABV, 
        m_kABVWorldVelocity, m_pkTriWorldVerts[ui0],
        m_pkTriWorldVerts[ui1], m_pkTriWorldVerts[ui2],
        m_kTriWorldVelocity))
    {
        m_bCollision = true;

        int iRet = m_pkTriData->TestCollisionProcessing(
            m_eTriPropagation, m_eABVPropagation);

        if (iRet == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            m_iReturn = NiCollisionGroup::TERMINATE_COLLISIONS;
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool TriToBndVolFind::operator()(const NiUInt32* pIndices, NiUInt32,
    NiUInt32,
    NiUInt16 uiSubMesh)
{
    NI_UNUSED_ARG(uiSubMesh);
    // These functions only support single-sub-mesh meshes.
    NIASSERT(uiSubMesh == 0);

    // Check for degenerate case.
    NiUInt32 ui0, ui1, ui2;
    if (GetIndices(pIndices, ui0, ui1, ui2) == false)
        return true;    // Continue.

    if (NiBoundingVolume::FindTriIntersect(m_fTime, *m_pkABV, 
         m_kABVWorldVelocity, m_pkTriWorldVerts[ui0], 
         m_pkTriWorldVerts[ui1], m_pkTriWorldVerts[ui2], 
         m_kTriWorldVelocity, m_kIntr.fTime, m_kIntr.kPoint, 
         m_bCalcNormals, m_kIntr.kNormal0, m_kIntr.kNormal1))
    {
        const NiPoint3* apkVerts[3];
        apkVerts[0] = &m_pkTriWorldVerts[ui0];
        apkVerts[1] = &m_pkTriWorldVerts[ui1];
        apkVerts[2] = &m_pkTriWorldVerts[ui2];

        if (m_bTriDataIsFirst)
            m_kIntr.appkTri1 = (const NiPoint3**)&apkVerts[0];
        else
            m_kIntr.appkTri2 = (const NiPoint3**)&apkVerts[0];

        m_bCollision = true;

        int iRet = m_pkTriData->FindCollisionProcessing(m_kIntr,
            m_eTriPropagation, m_eABVPropagation);

        if (iRet == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            m_iReturn = NiCollisionGroup::TERMINATE_COLLISIONS;
            return false;   // Stop testing at tri level.
        }

        if (iRet == NiCollisionGroup::BREAKOUT_COLLISIONS)
        {
            m_iReturn = NiCollisionGroup::CONTINUE_COLLISIONS;
            return false;   // Stop testing at tri level.
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool TriTriTestOuter::operator()(const NiUInt32* pIndices, NiUInt32,
    NiUInt32,
    NiUInt16 uiSubMesh)
{
    NI_UNUSED_ARG(uiSubMesh);
    // These functions only support single-sub-mesh meshes.
    NIASSERT(uiSubMesh == 0);
    
    NiUInt32 ui0, ui1, ui2;
    if (GetIndices(pIndices, ui0, ui1, ui2) == false)
        return true;    // Continue.

    TriTriTestInner kTestInner(*this);

    kTestInner.m_apkVerts[0] = &m_pkMesh1WorldVerts[ui0];
    kTestInner.m_apkVerts[1] = &m_pkMesh1WorldVerts[ui1];
    kTestInner.m_apkVerts[2] = &m_pkMesh1WorldVerts[ui2];

    // Traversal for triangle-to-triangle tests.
    return NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(m_pMesh2,
        kTestInner, NiDataStream::LOCK_READ, true);
}
//---------------------------------------------------------------------------
bool TriTriTestInner::operator()(const NiUInt32* pIndices, NiUInt32,
    NiUInt32, 
    NiUInt16 uiSubMesh)
{
    NI_UNUSED_ARG(uiSubMesh);
    // These functions only support single-sub-mesh meshes.
    NIASSERT(uiSubMesh == 0);
    
    NiUInt32 ui0, ui1, ui2;
    if (GetIndices(pIndices, ui0, ui1, ui2) == false)
        return true;    // Continue.

    const NiPoint3* apkVerts2[3];
    apkVerts2[0] = &m_kTestOuter.m_pkMesh2WorldVerts[ui0];
    apkVerts2[1] = &m_kTestOuter.m_pkMesh2WorldVerts[ui1];
    apkVerts2[2] = &m_kTestOuter.m_pkMesh2WorldVerts[ui2];

    NiTriIntersect kTI(m_apkVerts, m_kTestOuter.m_kMesh1WorldVelocity,
        apkVerts2, m_kTestOuter.m_kMesh2WorldVelocity);

    if (kTI.Test(m_kTestOuter.m_fTime))
    {
        m_kTestOuter.m_bCollision = true;
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool TriTriFindOuter::operator()(const NiUInt32* pIndices, NiUInt32,
    NiUInt32,
    NiUInt16 uiSubMesh)
{
    NI_UNUSED_ARG(uiSubMesh);
    // These functions only support single-sub-mesh meshes.
    NIASSERT(uiSubMesh == 0);
    
    NiUInt32 ui0, ui1, ui2;
    if (GetIndices(pIndices, ui0, ui1, ui2) == false)
        return true;    // Continue.

    apkVerts[0] = &m_pkMesh1WorldVerts[ui0];
    apkVerts[1] = &m_pkMesh1WorldVerts[ui1];
    apkVerts[2] = &m_pkMesh1WorldVerts[ui2];

    TriTriFindInner kFindInner(*this);

    // Traversal for triangle-to-triangle tests.
    return NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(m_pkMesh2,
        kFindInner, NiDataStream::LOCK_READ, true);
}
//---------------------------------------------------------------------------
bool TriTriFindInner::operator()(const NiUInt32* pIndices, NiUInt32,
    NiUInt32,
    NiUInt16 uiSubMesh)
{
    NI_UNUSED_ARG(uiSubMesh);
    // These functions only support single-sub-mesh meshes.
    NIASSERT(uiSubMesh == 0);
    
    NiUInt32 ui0, ui1, ui2;
    if (GetIndices(pIndices, ui0, ui1, ui2) == false)
        return true;    // Continue.

    const NiPoint3* apkVerts2[3];
    apkVerts2[0] = &m_kFindOuter.m_pkMesh2WorldVerts[ui0];
    apkVerts2[1] = &m_kFindOuter.m_pkMesh2WorldVerts[ui1];
    apkVerts2[2] = &m_kFindOuter.m_pkMesh2WorldVerts[ui2];

    NiTriIntersect kTI((const NiPoint3**)m_kFindOuter.apkVerts,
        m_kFindOuter.m_kMesh1WorldVelocity, (const NiPoint3**)apkVerts2,
        m_kFindOuter.m_kMesh2WorldVelocity);

    if (kTI.Find(m_kFindOuter.m_fTime, m_kFindOuter.m_kIntr.fTime,
        m_kFindOuter.m_kIntr.kPoint, m_kFindOuter.m_bCalcNormals, 
        &m_kFindOuter.m_kIntr.kNormal0, &m_kFindOuter.m_kIntr.kNormal1))
    {
        m_kFindOuter.m_kIntr.appkTri1 =
            (const NiPoint3**)&m_kFindOuter.apkVerts[0];
        m_kFindOuter.m_kIntr.appkTri2 = (const NiPoint3**)&apkVerts2[0];

        m_kFindOuter.m_bCollision = true;

        int iRet = m_kFindOuter.m_pkData1->FindCollisionProcessing(
            m_kFindOuter.m_kIntr, m_kFindOuter.m_eMesh1Propagation,
            m_kFindOuter.m_eMesh2Propagation);

        if (iRet == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            m_kFindOuter.m_iReturn = NiCollisionGroup::TERMINATE_COLLISIONS;
            return false;
        }

        if (iRet == NiCollisionGroup::BREAKOUT_COLLISIONS)
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
// Bounding volume intersection routines.
//---------------------------------------------------------------------------
bool BoundingVolumeTestIntersect(float fTime, NiAVObject* pkObj1, 
    NiAVObject* pkObj2)
{
    // NOTE:  User responsible for UpdateWorldData on all ABVs prior to call,
    // most likely by calling NiCollisionGroup::UpdateWorldABVs().
    
    NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkObj2);

    if (!pkData1 || !pkData2)
        return false;

    if (pkData1->GetModelSpaceABV())
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Both objects are using alternate bounding volumes.
            return NiBoundingVolume::TestIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(), 
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity());
        }
        else
        {
            // Only object 1 is using an alternate bounding volume.
            return NiBoundingVolume::TestIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(), 
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj2->GetWorldBound()), pkData2->GetWorldVelocity());
        }
    }
    else
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Only object 2 is using an alternate bounding volume.
            return NiBoundingVolume::TestIntersect(fTime, 
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj1->GetWorldBound()), pkData1->GetWorldVelocity(), 
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity());
        }
        else
        {
            // Neither object is using an alternate bounding volume, so rely
            // on NiBound::TestIntersect.
            return NiBound::TestIntersect(fTime, 
                pkObj1->GetWorldBound(), pkData1->GetWorldVelocity(), 
                pkObj2->GetWorldBound(), pkData2->GetWorldVelocity());
        }
    }
}
//---------------------------------------------------------------------------
bool BoundingVolumeFindIntersect(float fTime, NiAVObject* pkObj1,
    NiAVObject* pkObj2, float& fIntrTime, NiPoint3& kIntrPoint, 
    bool bCalcNormals, NiPoint3& kNormal0, NiPoint3& kNormal1)
{
    // NOTE:  The application is responsible for calling UpdateWorldData() on
    // all ABVs prior to this call, most likely by calling
    // NiCollisionGroup::UpdateWorldABVs().
    
    NiCollisionData* pkData1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkObj2);

    if (!pkData1 || !pkData2)
    {
        NiOutputDebugString(
            "Warning:  BoundingVolumeFindIntersect() is trivially returning "
            "a non-collision result since\n  at least one of the NiAVObjects "
            "being compared has no collision data.\n");
        return false;
    }

    if (pkData1->GetModelSpaceABV())
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Both objects are using alternate bounding volumes.
            return NiBoundingVolume::FindIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(),
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity(),
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
        else
        {
            // Only object 1 is using an alternate bounding volume.
            return NiBoundingVolume::FindIntersect(fTime, 
                *(pkData1->GetWorldSpaceABV()), pkData1->GetWorldVelocity(),
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj2->GetWorldBound()), pkData2->GetWorldVelocity(),
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
    }
    else
    {
        if (pkData2->GetModelSpaceABV())
        {
            // Only object 2 is using an alternate bounding volume.
            return NiBoundingVolume::FindIntersect(fTime, 
                *NiBoundingVolume::ConvertToSphereBV_Fast(
                pkObj1->GetWorldBound()), pkData1->GetWorldVelocity(), 
                *(pkData2->GetWorldSpaceABV()), pkData2->GetWorldVelocity(),
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
        else
        {
            // Neither object is using an alternate bounding volume, so rely
            // on NiBound::FindIntersect.
            return NiBound::FindIntersect(fTime, 
                pkObj1->GetWorldBound(), pkData1->GetWorldVelocity(), 
                pkObj2->GetWorldBound(), pkData2->GetWorldVelocity(), 
                fIntrTime, kIntrPoint, bCalcNormals, kNormal0, kNormal1);
        }
    }
}
//---------------------------------------------------------------------------
// Triangle-[triangle | bv] intersection routines.
//---------------------------------------------------------------------------
int TriTriTestIntersect(float fTime, NiAVObject* pkGeom1, 
    NiAVObject* pkGeom2, bool& bCollision)
{
    TriTriTestOuter kTest(fTime, bCollision);

    NiCollisionData* pkData1 = NiGetCollisionData(pkGeom1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkGeom2);

    NIASSERT(pkData1 && pkData2);
    if (!pkData1 || !pkData2)
    {
        NiOutputDebugString(
            "Warning:  TriTriTestIntersect() is failing since\n  "
            "at least one of the two objects has no collision data.\n");
    }

    if (!pkData1 || !pkData2)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    kTest.m_pMesh2 = pkData2->GetCollisionMesh();

    kTest.m_pkMesh1WorldVerts = pkData1->GetWorldVertices();
    kTest.m_pkMesh2WorldVerts = pkData2->GetWorldVertices();

    kTest.m_kMesh1WorldVelocity = pkData1->GetWorldVelocity();
    kTest.m_kMesh2WorldVelocity = pkData2->GetWorldVelocity();

    // Traversal for triangle-to-triangle tests.
    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(
        pkData1->GetCollisionMesh(), kTest, NiDataStream::LOCK_READ,
        true);

    return kTest.m_iReturn;
}
//---------------------------------------------------------------------------
int TriTriFindIntersect(float fTime, NiAVObject* pkGeom1, NiAVObject* pkGeom2,
    bool bCalcNormals, NiCollisionGroup::Intersect& kIntr, bool& bCollision)
{
    TriTriFindOuter kFind(kIntr, bCalcNormals, fTime, bCollision);

    kFind.m_pkData1 = NiGetCollisionData(pkGeom1);
    NiCollisionData* pkData2 = NiGetCollisionData(pkGeom2);

    NIASSERT(kFind.m_pkData1 && pkData2);
    if (!kFind.m_pkData1 || !pkData2)
    {
        NiOutputDebugString(
            "Warning:  TriTriFindIntersect() is failing since\n  "
            "at least one of the two objects has no collision data.\n");
    }

    if (!kFind.m_pkData1 || !pkData2)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    kFind.m_pkMesh1 = kFind.m_pkData1->GetCollisionMesh();
    kFind.m_pkMesh2 = pkData2->GetCollisionMesh();

    kFind.m_pkMesh1WorldVerts = kFind.m_pkData1->GetWorldVertices();
    kFind.m_pkMesh2WorldVerts = pkData2->GetWorldVertices();

    kFind.m_kMesh1WorldVelocity = kFind.m_pkData1->GetWorldVelocity();
    kFind.m_kMesh2WorldVelocity = pkData2->GetWorldVelocity();

    kFind.m_eMesh1Propagation = kFind.m_pkData1->GetPropagationMode();
    kFind.m_eMesh2Propagation = pkData2->GetPropagationMode();

    // Traversal for triangle-to-triangle tests.
    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(kFind.m_pkMesh1, kFind,
        NiDataStream::LOCK_READ, true);
    return kFind.m_iReturn;
}
//---------------------------------------------------------------------------
int TriToBndVolTestIntersect(float fTime, NiAVObject* pkTBG,
    NiAVObject* pkOther, bool& bCollision)
{
    TriToBndVolTest kTest(bCollision, fTime);

    NiCollisionData* pkData1 = NiGetCollisionData(pkTBG);
    NiCollisionData* pkData2 = NiGetCollisionData(pkOther);

    NIASSERT(pkData1 && pkData2);
    if (!pkData1 || !pkData2)
    {
        NiOutputDebugString(
            "Warning:  TriToBndVolTestIntersect() is failing since\n  "
            "at least one of the two objects has no collision data.\n");
    }

    NiCollisionData::CollisionMode eMode1 = pkData1->GetCollisionMode();

    NiTransform kTrans;
    NiCollisionData* pkABVData;

    if (eMode1 == NiCollisionData::USE_TRI ||
        eMode1 == NiCollisionData::USE_OBB)
    {
        kTest.m_pkTriData = pkData1;
        pkABVData = pkData2;
        kTrans = pkOther->GetWorldTransform();
    }
    else    // USE_ABV or USE_NIBOUND
    {
        kTest.m_pkTriData = pkData2;
        pkABVData = pkData1;
        kTrans = pkTBG->GetWorldTransform();
    }

    kTest.m_kABVWorldVelocity = pkABVData->GetWorldVelocity();
    kTest.m_kTriWorldVelocity = kTest.m_pkTriData->GetWorldVelocity();
    kTest.m_eTriPropagation = kTest.m_pkTriData->GetPropagationMode();
    kTest.m_eABVPropagation = pkABVData->GetPropagationMode();

    if (!pkABVData->GetModelSpaceABV())
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    if (kTest.m_pkTriData->GetWorldVertices() == NULL)
    {
        kTest.m_pkTriData->CreateWorldVertices();
    }
    kTest.m_pkTriData->UpdateWorldVertices();
    kTest.m_pkTriWorldVerts = kTest.m_pkTriData->GetWorldVertices();

    // 'Other' is using an alternate bounding volume.
    kTest.m_pkABV = pkABVData->GetWorldSpaceABV();

    NIASSERT(kTest.m_pkABV);
    if (!kTest.m_pkABV)
    {
        NiOutputDebugString(
            "Warning:  TriToBndVolTestIntersect() is failing since\n  "
            "at least one of the two objects has no ABV.\n");
    }
    
    kTest.m_pkABV->UpdateWorldData(*pkABVData->GetModelSpaceABV(),
        kTrans);

    NiMesh* pTriMesh = kTest.m_pkTriData->GetCollisionMesh();

    // Traversal for triangle-to-triangle tests.
    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(pTriMesh,
        kTest, NiDataStream::LOCK_READ, true);
    return kTest.m_iReturn;
}
//---------------------------------------------------------------------------
int TriToBndVolFindIntersect(float fTime, NiAVObject* pkTBG,
    NiAVObject* pkOther, bool bCalcNormals,
    NiCollisionGroup::Intersect& kIntr, bool &bCollision)
{
    TriToBndVolFind kFind(kIntr, bCollision, bCalcNormals, fTime);

    NiCollisionData* pkData1 = NiGetCollisionData(pkTBG);
    NiCollisionData* pkData2 = NiGetCollisionData(pkOther);

    NIASSERT(pkData1 && pkData2);
    if (!pkData1 || !pkData2)
    {
        NiOutputDebugString(
            "Warning:  TriToBndVolFindIntersect() is failing since\n  "
            "at least one of the two objects has no collision data.\n");
    }

    NiCollisionData::CollisionMode eMode1 = pkData1->GetCollisionMode();
    NiCollisionData* pkABVData;
    NiTransform kTrans;

    if (eMode1 == NiCollisionData::USE_TRI ||
        eMode1 == NiCollisionData::USE_OBB)
    {
        kFind.m_pkTriData = pkData1;
        kFind.m_bTriDataIsFirst = true;
        pkABVData = pkData2;
        kTrans = pkOther->GetWorldTransform();
    }
    else    // USE_ABV or USE_NIBOUND
    {
        kFind.m_pkTriData = pkData2;
        kFind.m_bTriDataIsFirst = false;
        pkABVData = pkData1;
        kTrans = pkTBG->GetWorldTransform();
    }

    kFind.m_kABVWorldVelocity = pkABVData->GetWorldVelocity();
    kFind.m_kTriWorldVelocity = kFind.m_pkTriData->GetWorldVelocity();
    kFind.m_eTriPropagation = kFind.m_pkTriData->GetPropagationMode();
    kFind.m_eABVPropagation = pkABVData->GetPropagationMode();

    if (!pkABVData->GetModelSpaceABV())
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    if (kFind.m_pkTriData->GetWorldVertices() == NULL)
    {
        kFind.m_pkTriData->CreateWorldVertices();
    }
    kFind.m_pkTriData->UpdateWorldVertices();
    kFind.m_pkTriWorldVerts = kFind.m_pkTriData->GetWorldVertices();

    // 'Other' is using an alternate bounding volume.
    kFind.m_pkABV = pkABVData->GetWorldSpaceABV();

    NIASSERT(kFind.m_pkABV);
    if (!kFind.m_pkABV)
    {
        NiOutputDebugString(
            "Warning:  TriToBndVolFindIntersect() is failing since\n  "
            "at least one of the two objects has no ABV.\n");
    }

    kFind.m_pkABV->UpdateWorldData(*pkABVData->GetModelSpaceABV(),
        kTrans);

    NiMesh* pTriMesh = kFind.m_pkTriData->GetCollisionMesh();

    // Traversal for triangle-to-triangle tests.
    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(pTriMesh,
        kFind, NiDataStream::LOCK_READ, true);
    return kFind.m_iReturn;
}
//---------------------------------------------------------------------------
// ABV picking routines
//---------------------------------------------------------------------------
bool FindRaySphereIntersect(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection,
    const NiSphereBV& kSphereBV, NiPick::Record& kRecord)
{
    NiPoint3 kRayNormalized = kRayDirection;
    kRayNormalized.Unitize();

    const NiSphere& kSphere = kSphereBV.GetSphere();

    return FindRaySphereHelper(kRayOrigin, kRayNormalized, kSphere.m_kCenter,
        kSphere.m_fRadius, kRecord);
}
//---------------------------------------------------------------------------
bool FindRayCapsuleIntersect(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection,
    const NiCapsuleBV& kCapsuleBV, NiPick::Record& kRecord)
{
    NiPoint3 kRayNormalized = kRayDirection;
    kRayNormalized.Unitize();

    // First, check if the ray/line intersects the infinite cylinder that
    // surrounds the line going through the center points of the capsule.
    // There are 3 possibilities:
    //
    // If the ray/line intersects two times, we may have a hit. Compute the
    // capsule t value for the closer hit.  If 0 <= t <= 1, we have a hit.
    // compute point and normal values.
    // If t is outside 0..1, compute hits for hemispheres at ends of capsule.
    // If we have a  hit there, compute point and normal.
    //
    // If the ray/line intersects one time, it's a tangential touch. Check the
    // capsule t value, if it's 0 <= t <= 1, then it's a true hit, and
    // set the appropriate point and normal values. If it's outside 0..1, it's
    // a miss
    //
    // If it hits 0 times, it's a miss.

    bool bCheckEnds = false;

    // based on segment/cylinder intersection test
    // Christer Ericson, "Real-Time Collision Detection" p. 197
    const float EPSILON = 0.0000001f;

    const NiCapsule& kCap = kCapsuleBV.GetCapsule();

    // before beginning the main computation, we will check if the ray origin
    // is inside the cylinder.

    float fOriginDistance = 0.0f;

    NiPoint3 ab = kCap.m_kSegment.m_kDirection;
    NiPoint3 ac = kRayOrigin - kCap.m_kSegment.m_kOrigin;
    NiPoint3 bc = kRayOrigin - (kCap.m_kSegment.m_kOrigin + 
        kCap.m_kSegment.m_kDirection);

    float e = ac.Dot(ab);
    float f = ab.Dot(ab);

    if (e <= 0.0f)
        fOriginDistance = ac.Dot(ac);
    else if (e >= f)
        fOriginDistance = bc.Dot(bc);
    else
        fOriginDistance = ac.Dot(ac) - e * e / f;

    if (fOriginDistance < kCap.m_fRadius * kCap.m_fRadius)
    {
        // ray begins inside capsule
        kRecord.SetRayInsideABV(true);
        kRecord.SetIntersection(kRayOrigin);
        kRecord.SetNormal(kRayNormalized);
        kRecord.SetDistance(0);
        return true;
    }

    NiPoint3 n = kRayNormalized;
    NiPoint3 sa = kRayOrigin;
    NiPoint3 p = kCap.m_kSegment.m_kOrigin;
    NiPoint3 d = kCap.m_kSegment.m_kDirection;
    NiPoint3 q = kCap.m_kSegment.m_kOrigin + kCap.m_kSegment.m_kDirection;

    float r = kCap.m_fRadius;
    NiPoint3 m = sa - p;


    float md = m.Dot(d);
    float nd = n.Dot(d);
    float dd = d.Dot(d);

    // infinite cylinder, no need to test endcaps as in book

    float nn = n.Dot(n);
    float mn = m.Dot(n);

    float a = dd * nn - nd * nd;
    float k = m.Dot(m) - r * r;
    float c = dd * k - md * md;

    if (NiAbs(a) < EPSILON)
    {
        // ray is parallel to cylinder
        if (c > 0.0f)
        {
            kRecord.SetDistance(-1);
            kRecord.SetRayInsideABV(false);
            return false;
        }
        bCheckEnds = true;
    }

    float b = dd * mn - nd * md;
    float discr = b * b - a * c;
    if (discr < 0.0f)
    {
        kRecord.SetDistance(-1);
        kRecord.SetRayInsideABV(false);
        return false;
    }

    float t = (-b - NiSqrt(discr)) / a;

    // again, no need to check range of t, since using a ray not a segment.
    // will clamp t to zero at end, but need possible negative value to
    // calculate proper capsule point and normal.

    float s = md + t * nd;
    if (s < 0.0f)
    {
        bCheckEnds = true;
    }
    else if (s > dd)
    {
        bCheckEnds = true;
    }

    NiPoint3 kClosest = p + (s / dd) * d;
    NiPoint3 kIntersect = sa + t * n;
    NiPoint3 kNormal = (kIntersect - kClosest);
    kNormal.Unitize();

    if (bCheckEnds)
    {
        bool bHitAnEnd =false;
        NiPick::Record kRec1(NULL);
        if (FindRaySphereHelper(kRayOrigin, kRayNormalized, p, r, kRec1))
        {
            t = kRec1.GetDistance();
            kIntersect = kRec1.GetIntersection();
            kNormal = kRec1.GetNormal();
            bHitAnEnd = true;
        }

        NiPick::Record kRec2(NULL);
        if (FindRaySphereHelper(kRayOrigin, kRayNormalized, q, r,
            kRec2))
        {
            if (kRec2.GetDistance() < t || kRec1.GetDistance() < 0)
            {
                t = kRec2.GetDistance();
                kIntersect = kRec2.GetIntersection();
                kNormal = kRec2.GetNormal();
                bHitAnEnd = true;
            }
        }

        if (!bHitAnEnd)
        {
            kRecord.SetRayInsideABV(false);
            kRecord.SetDistance(-1);
            return false;
        }
    }

    kRecord.SetRayInsideABV(false);
    kRecord.SetDistance(t);
    kRecord.SetIntersection(kIntersect);
    kRecord.SetNormal(kNormal);
    return true;
}
//---------------------------------------------------------------------------
bool FindRayHalfSpaceIntersect(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection, 
    const NiHalfSpaceBV& kHalfSpaceBV, NiPick::Record& kRecord)
{
    NiPoint3 kRayNormalized = kRayDirection;
    kRayNormalized.Unitize();

    // note negative sign in below, reflecting that NiPlane is defined
    // to have normal pointing into the half-space, which is opposite of
    // standard plane definitions
    float fConst = -kHalfSpaceBV.GetPlane().GetConstant();
    bool bIntersect =  FindRayPlaneHelper(kRayOrigin, kRayNormalized, fConst,
        -kHalfSpaceBV.GetNormal(), kRecord);
    if (bIntersect && kRecord.GetRayInsideABV())
    {
        // see notes to ray-plane helper for why these lines are needed.
        kRecord.SetIntersection(kRayOrigin);
        kRecord.SetNormal(kRayNormalized);
        kRecord.SetDistance(0);
    }

    return bIntersect;
}
//---------------------------------------------------------------------------
bool FindRayBoxIntersect(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection, 
    const NiBoxBV& kBoxBV, NiPick::Record& kRecord)
{
    NiPoint3 kRayNormalized = kRayDirection;
    kRayNormalized.Unitize();

    // Box is made up of 6 planes. Find intersection points with those
    // planes.
    const NiPoint3* pAxes = kBoxBV.GetAxes();
    const float* pExtents = kBoxBV.GetExtents();

    // create 6 planes, each representing a side of the box
    NiPlane akPlanes[6];
    NiPoint3 akPlaneIntersects[6];

    akPlanes[0] = NiPlane(pAxes[0], 
        kBoxBV.GetCenter() + pAxes[0] * pExtents[0]);
    akPlanes[1] = NiPlane(pAxes[1], 
        kBoxBV.GetCenter() + pAxes[1] * pExtents[1]);
    akPlanes[2] = NiPlane(pAxes[2], 
        kBoxBV.GetCenter() + pAxes[2] * pExtents[2]);
    akPlanes[3] = NiPlane(-pAxes[0], 
        kBoxBV.GetCenter() + pAxes[0] * -pExtents[0]);
    akPlanes[4] = NiPlane(-pAxes[1], 
        kBoxBV.GetCenter() + pAxes[1] * -pExtents[1]);
    akPlanes[5] = NiPlane(-pAxes[2], 
        kBoxBV.GetCenter() + pAxes[2] * -pExtents[2]);

    float fIn = 0;
    float fOut = 0;
    int iInPlane = 0;
    int iInsideCount = 0;
    bool bValidIn = false;
    bool bValidOut = false;

    for (unsigned int uiIndex = 0; uiIndex < 6; uiIndex ++)
    {
        NiPick::Record kTempRecord(NULL);
        bool bIntersect = FindRayPlaneHelper(kRayOrigin, kRayNormalized,
            akPlanes[uiIndex].GetConstant(), akPlanes[uiIndex].GetNormal(),
            kTempRecord);
        akPlaneIntersects[uiIndex] = kTempRecord.GetIntersection();
        if (kTempRecord.GetRayInsideABV())
        {
            iInsideCount ++;
        }
        if (bIntersect)
        {
            // need to determine if this is an "in" intersection or an "out"
            // intersection
            bool bIsIn = !kTempRecord.GetRayInsideABV();

            if (bIsIn && (!bValidIn || (bValidIn && 
                kTempRecord.GetDistance() > fIn)))
            {
                fIn = kTempRecord.GetDistance();
                bValidIn = true;
                iInPlane = uiIndex;
            }
			else if (kTempRecord.GetDistance() >= 0.0f && 
				!bIsIn && (!bValidOut || (bValidOut && 
                kTempRecord.GetDistance() < fOut)))
            {
                fOut = kTempRecord.GetDistance();
                bValidOut = true;
            }
        }
        else if (!kTempRecord.GetRayInsideABV())
        {
            kRecord.SetDistance(-1);
            return false;
        }
    }

    // ray begins inside box.
    if (iInsideCount == 6)
    {
        kRecord.SetIntersection(kRayOrigin);
        kRecord.SetNormal(kRayNormalized);
        kRecord.SetRayInsideABV(true);
        kRecord.SetDistance(0);

        return true;
    }

    if (!bValidIn || !bValidOut || fIn > fOut)
    {
        kRecord.SetDistance(-1);
        return false;
    }
        
    kRecord.SetIntersection(akPlaneIntersects[iInPlane]);
    kRecord.SetNormal(akPlanes[iInPlane].GetNormal());
    kRecord.SetRayInsideABV(false);
    kRecord.SetDistance(fIn);
    return true;
}
//---------------------------------------------------------------------------
bool FindRayUnionIntersect(
    const NiPoint3& kRayOrigin, const NiPoint3& kRayDirection, 
    const NiUnionBV& kUnionBV, NiPick::Record& kRecord)
{
    // Union is made up of other bounding volumes. Find intersections
    // of all sub-volumes, return closest intersection.

    unsigned int uiCount = kUnionBV.GetSize();
    bool bCollide = false;
    float fDist = -1;
    NiPick::Record kTempRecord(NULL);

    for (unsigned int uiIndex = 0; uiIndex < uiCount; uiIndex ++)
    {
        const NiBoundingVolume* pkBV = kUnionBV.GetBoundingVolume(uiIndex);
        bool bTempCollide = false;
        switch (pkBV->Type())
        {
        case NiBoundingVolume::CAPSULE_BV:
            bTempCollide = FindRayCapsuleIntersect(
                kRayOrigin, 
                kRayDirection,
                *((NiCapsuleBV*)pkBV), 
                kTempRecord);
            break;
        case NiBoundingVolume::SPHERE_BV:
            bTempCollide = FindRaySphereIntersect(
                kRayOrigin, 
                kRayDirection,
                *((NiSphereBV*)pkBV), 
                kTempRecord);
            break;
        case NiBoundingVolume::BOX_BV:
            bTempCollide = FindRayBoxIntersect(
                kRayOrigin, 
                kRayDirection,
                *((NiBoxBV*)pkBV), 
                kTempRecord);
            break;
        case NiBoundingVolume::HALFSPACE_BV:
            bTempCollide = FindRayHalfSpaceIntersect(
                kRayOrigin, 
                kRayDirection,
                *((NiHalfSpaceBV*)pkBV),
                kTempRecord);
            break;
        case NiBoundingVolume::UNION_BV:
            bTempCollide = FindRayUnionIntersect(
                kRayOrigin, 
                kRayDirection,
                *((NiUnionBV*)pkBV), 
                kTempRecord);
            break;
        }

        if (bTempCollide && (fDist < 0 || kTempRecord.GetDistance() < fDist))
        {
            bCollide = true;
            fDist = kTempRecord.GetDistance();
            kRecord.SetIntersection(kTempRecord.GetIntersection());
            kRecord.SetNormal(kTempRecord.GetNormal());
            kRecord.SetDistance(kTempRecord.GetDistance());
        }
    }
    if (!bCollide)
    {
        kTempRecord.SetDistance(-1);
    }
    return bCollide;
}
//---------------------------------------------------------------------------
} // namespace
