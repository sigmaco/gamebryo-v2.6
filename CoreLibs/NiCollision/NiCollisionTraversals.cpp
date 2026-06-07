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

#include <NiSwitchNode.h>
#include <NiMesh.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiDataStreamElementLock.h>
#include <NiMeshAlgorithms.h>
#include <NiSkinningMeshModifier.h>

#include "NiCollisionMetrics.h"
#include "NiCollisionTraversals.h"
#include "NiCollisionUtils.h"
#include "NiPick.h"
#include "NiSphereBV.h"

typedef NiTStridedRandomAccessIterator<NiPoint3> NiVertIter;
typedef NiTStridedRandomAccessIterator<NiColorA> NiColorIter;
typedef NiTStridedRandomAccessIterator<NiPoint3> NiNormalIter;
typedef NiTStridedRandomAccessIterator<NiPoint2> NiTexCoordIter;

namespace NiCollisionTraversals
{
// Forward declare
NiBoundingVolume* OptimallyConvertToBoxBV(NiAVObject* pkObject);

//---------------------------------------------------------------------------
// Internal Declaration for FindMesh class used with NiMeshAlgorithms
//---------------------------------------------------------------------------
class FindMesh
{
public:
    NiPick& m_kPick;
    NiMesh* m_pkMesh;
    const NiTransform& m_kWorld;
    NiPoint3& m_kModelOrigin;
    NiPoint3& m_kModelDir;
    NiDataStreamRef* m_pkVertStreamRef;
    NiVertIter& m_kVertIter;
    NiDataStreamRef* m_pkColorStreamRef;
    NiColorIter& m_kColorIter;
    NiDataStreamRef* m_pkNormalStreamRef;
    NiNormalIter& m_kNormalIter;
    NiDataStreamRef* m_pkTexCoordStreamRef;
    NiTexCoordIter& m_kTexCoordIter;
    bool m_bFound;

    FindMesh(NiPick& kPick, NiMesh* pkMesh, const NiTransform& kWorld,
        NiPoint3& kModelOrigin, NiPoint3& kModelDir,
        NiDataStreamRef* pkVertStreamRef, NiVertIter& kVertIter,
        NiDataStreamRef* pkColorStreamRef, NiColorIter& kColorIter,
        NiDataStreamRef* pkNormalStreamRef, NiNormalIter& kNormalIter,
        NiDataStreamRef* pkTexCoordStreamRef, NiTexCoordIter& kTexCoordIter) :
        m_kPick(kPick), m_pkMesh(pkMesh), m_kWorld(kWorld),
        m_kModelOrigin(kModelOrigin), m_kModelDir(kModelDir),
        m_pkVertStreamRef(pkVertStreamRef), m_kVertIter(kVertIter),
        m_pkColorStreamRef(pkColorStreamRef), m_kColorIter(kColorIter),
        m_pkNormalStreamRef(pkNormalStreamRef), m_kNormalIter(kNormalIter),
        m_pkTexCoordStreamRef(pkTexCoordStreamRef),
        m_kTexCoordIter(kTexCoordIter), m_bFound(false) {}

    bool operator ()(const NiUInt32* pIndices, NiUInt32 uiCount, 
        NiUInt32 uiTri, NiUInt16 uiSubMesh);

private:
    FindMesh & operator=( const FindMesh & );
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// local utility function
//---------------------------------------------------------------------------
static void SetIntersectionData(const NiPick& kPick, 
    const NiPick::Record* pkSource, NiPick::Record* pkDest)
{
    if (kPick.GetCoordinateType() == NiPick::COORDINATES_WORLD)
    {
        pkDest->SetIntersection(pkSource->GetIntersection());
        pkDest->SetNormal(pkSource->GetNormal());
    }
    else
    {
        NiAVObject* pkObj = pkDest->GetAVObject();
        const NiTransform& kTrans = pkObj->GetWorldTransform();
        NiTransform kInverseTrans;
        kTrans.Invert(kInverseTrans);
        
        pkDest->SetIntersection(kInverseTrans * pkSource->GetIntersection());
        pkDest->SetNormal(kInverseTrans.m_Rotate * pkSource->GetNormal());
    }
}

//---------------------------------------------------------------------------
// Collision detection scene graph traversal routines.
//---------------------------------------------------------------------------
bool CheckForCollisionData(NiAVObject* pkRoot, bool bRecursive)
{
    NiCollisionData* pkData = NiGetCollisionData(pkRoot);

    if (pkData)
        return true;

    if (bRecursive == false)
        return false;

    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
            {
                bool bResult = CheckForCollisionData(pkChild, bRecursive);

                if (bResult)
                    return true;
            }
        }
    }   

    return false;
}
//---------------------------------------------------------------------------
void ClearWorldVelocities(NiAVObject* pkRoot, NiPoint3& kValue)
{
    NiCollisionData* pkData = NiGetCollisionData(pkRoot);

    if (pkData)
    {
        pkData->SetLocalVelocity(kValue);
        pkData->SetWorldVelocity(kValue);
    }

    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                ClearWorldVelocities(pkChild, kValue);
        }
    }
}
//---------------------------------------------------------------------------
// World vertices and normals.
void UpdateWorldVertices(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->UpdateWorldVertices();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                UpdateWorldVertices(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void DestroyWorldVertices(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->DestroyWorldVertices();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;  
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                DestroyWorldVertices(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void UpdateWorldNormals(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->UpdateWorldNormals();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                UpdateWorldNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void DestroyWorldNormals(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->DestroyWorldNormals();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                DestroyWorldNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool TestCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1)
{
    bool bCollision = false;
    TestCollisions(fDeltaTime, kRecord0, kRecord1, bCollision);

    return bCollision;
}
//---------------------------------------------------------------------------
int BoundsTestCheck(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2, 
    NiCollisionData::CollisionTest eCollisionTest)    
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);
    NiCollisionData::CollisionMode CollisionMode1 = pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode CollisionMode2 = pkCD2->GetCollisionMode();
    int iNumOfABVs = 0;

    // Create or update ABV Sphere for TRI_NIBOUND case only.
    if (eCollisionTest == NiCollisionData::TRI_NIBOUND)
    {
        NiTransform kWorldInverse;
        if (CollisionMode1 == NiCollisionData::USE_NIBOUND)
        {
            NiBound kBound = pkObj1->GetWorldBound();
            NiTransform kWorldTrans = pkObj1->GetWorldTransform();
            kWorldTrans.Invert(kWorldInverse);

            kBound.SetCenter(kWorldInverse.m_fScale * (kWorldInverse.m_Rotate
                * kBound.GetCenter()) + kWorldInverse.m_Translate);
            kBound.SetRadius(kWorldInverse.m_fScale * kBound.GetRadius());

            NiBoundingVolume* pkSphABV = pkCD1->GetModelSpaceABV();
            if (pkSphABV)   // Update pre-existing ABV Sphere.
            {
                NiSphereBV& kSphABV = (NiSphereBV&)(*pkSphABV);
                kSphABV.SetCenter(kBound.GetCenter());
                kSphABV.SetRadius(kBound.GetRadius());
                iNumOfABVs++;
            }
            else    // Create new ABV Sphere.
            {
                pkSphABV = NiBoundingVolume::ConvertToSphereBV_Safe(kBound);
                if (pkSphABV)
                {
                    pkCD1->SetModelSpaceABV(pkSphABV);
                    iNumOfABVs++;
                }
            }
        }
        else if (CollisionMode2 == NiCollisionData::USE_NIBOUND)
        {
            NiBound kBound = pkObj2->GetWorldBound();
            NiTransform kWorldTrans = pkObj2->GetWorldTransform();
            kWorldTrans.Invert(kWorldInverse);

            kBound.SetCenter(kWorldInverse.m_fScale * (kWorldInverse.m_Rotate
                * kBound.GetCenter()) + kWorldInverse.m_Translate);
            kBound.SetRadius(kWorldInverse.m_fScale * kBound.GetRadius());

            NiBoundingVolume* pkSphABV = pkCD2->GetModelSpaceABV();
            if (pkSphABV)   // Update pre-existing ABV Sphere.
            {
                NiSphereBV& kSphABV = (NiSphereBV&)(*pkSphABV);
                kSphABV.SetCenter(kBound.GetCenter());
                kSphABV.SetRadius(kBound.GetRadius());
                iNumOfABVs++;
            }
            else    // Create new ABV Sphere.
            {
                pkSphABV = NiBoundingVolume::ConvertToSphereBV_Safe(kBound);
                if (pkSphABV)
                {
                    pkCD2->SetModelSpaceABV(pkSphABV);
                    iNumOfABVs++;
                }
            }
        }
    }

    // Create an ABV Box for selected cases, and perform a quick out test if
    // appropriate.
    switch(eCollisionTest)
    {
        case NiCollisionData::ABV_ABV:
            return true;    // Simply let collision testing of ABVs handle it.
            break;
        case NiCollisionData::NOTEST_NOTEST:
            return false;
            break;
        case NiCollisionData::TRI_NIBOUND:
        case NiCollisionData::ABV_NIBOUND:
        case NiCollisionData::OBB_NIBOUND:
        case NiCollisionData::NIBOUND_NIBOUND:
        // Still a chance that we may benefit by creating an ABV for
        // collisions with TRIs or OBBs.
        case NiCollisionData::OBB_ABV:       
        case NiCollisionData::TRI_ABV:      
        case NiCollisionData::TRI_TRI:
        case NiCollisionData::OBB_OBB:
        case NiCollisionData::OBB_TRI:
        {
            // Geometry can have lots of triangles and each would be tested
            // against the ABV.  Instead of testing against the world bounds
            // first, create a BoxABV for "quick out" tests, since it is a
            // better fit and enables more efficient culling.
            if (CollisionMode1 == NiCollisionData::USE_TRI ||
                CollisionMode1 == NiCollisionData::USE_OBB)
            {
                if (!pkCD1->GetModelSpaceABV())
                {
                    NiBoundingVolume* pkBoxBV = OptimallyConvertToBoxBV(
                        pkObj1);

                    if (pkBoxBV)
                    {
                        pkCD1->SetModelSpaceABV(pkBoxBV);
                        iNumOfABVs++;
                    }
                }
                else
                {
                    iNumOfABVs++;
                }
            }
            else if (CollisionMode1 == NiCollisionData::USE_ABV)
            {
                // Should have an ABV then.
                NIASSERT(pkCD1->GetModelSpaceABV());
                if (pkCD1->GetModelSpaceABV())
                {
                    iNumOfABVs++;
                }
                else
                {
                    NiOutputDebugString(
                        "Warning:  BoundsTestCheck() has failed since\n  "
                        "collision data for one of the objects has mode "
                        "USE_ABV but there is no associated ABV.\n");
                }
            }

            if (CollisionMode2 == NiCollisionData::USE_TRI ||
                CollisionMode2 == NiCollisionData::USE_OBB)
            {
                if (!pkCD2->GetModelSpaceABV())
                {
                    NiBoundingVolume* pkBoxBV = OptimallyConvertToBoxBV(
                        pkObj2);

                    if (pkBoxBV)
                    {
                        pkCD2->SetModelSpaceABV(pkBoxBV);
                        iNumOfABVs++;
                    }
                }
                else
                {
                    iNumOfABVs++;
                }
            }
            else if (CollisionMode2 == NiCollisionData::USE_ABV)
            {
                // Should have an ABV then.
                NIASSERT(pkCD2->GetModelSpaceABV());
                if (pkCD2->GetModelSpaceABV())
                {
                    iNumOfABVs++;
                }
                else
                {
                    NiOutputDebugString(
                        "Warning:  BoundsTestCheck() has failed since\n  "
                        "collision data for one of the objects has mode "
                        "USE_ABV but there is no associated ABV.\n");
                }
            }

            // If tighter fitting ABVs exist, they're used in a 1st pass
            // "quick out" test.
            if (iNumOfABVs == 2)
            {
                return NiCollisionUtils::BoundingVolumeTestIntersect(
                    fDeltaTime, pkObj1, pkObj2);
            }

            break;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
int TestCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, bool& bCollision)
{
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    // Look for collision data.
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    // In cases were we don't have collision data, we attempt to
    // go deeper into the scene graph to find it. This is to support
    // not having to have collision data at every node.
    if (pkCD1 == NULL || pkCD2 == NULL)
    {
        return TestCollisionDataAndCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollision);
    }

    NiCollisionData::CollisionMode eCollisionMode1
        = pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode eCollisionMode2
        = pkCD2->GetCollisionMode();

    bCollision = false;

    // NOTEST cases go directly to propagation handling.
    // And only NOTEST with PROPAGATE_ALWAYS will result
    // in possible collisions.

    if (eCollisionMode1 != NiCollisionData::NOTEST &&
        eCollisionMode2 != NiCollisionData::NOTEST)
    {
        if (TestHandleCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollision) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    else
    {
        if (eCollisionMode1 == NiCollisionData::NOTEST ||
            eCollisionMode2 == NiCollisionData::NOTEST)
            bCollision = true;
    }

    return TestPropagateCollisions(fDeltaTime, kRecord0, kRecord1,
        bCollision);
}
//---------------------------------------------------------------------------
int TestCollisionDataAndCollisions(float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    // If pkObj1 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

    if ((pkCD1 == NULL) && pkObj1 && pkObj1->IsNode())
    {
        NiNode* pkNode1 = (NiNode*)pkObj1;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode1->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj1))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj1)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode1->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

            if (TestCollisions(fDeltaTime, kNewRecord0, kRecord1,bCollision) 
                == NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    // If pkObj2 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj2 = kRecord1.GetAVObject();
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if ((pkCD2 == NULL) && pkObj2 && pkObj2->IsNode())
    {
        NiNode* pkNode2 = (NiNode*)pkObj2;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode2->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj2))   // Special case switch nodes.  
        {
            uiStart = ((NiSwitchNode*)pkObj2)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode2->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
                pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

            if (TestCollisions(fDeltaTime, kRecord0, kNewRecord1,bCollision)
                == NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindCollisionDataAndCollisions(float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    // If pkObj1 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

    if ((pkCD1 == NULL) && pkObj1 && pkObj1->IsNode())
    {
        NiNode* pkNode1 = (NiNode*)pkObj1;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode1->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj1))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj1)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode1->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

            if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    // If pkObj2 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj2 = kRecord1.GetAVObject();
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if ((pkCD2 == NULL) && pkObj2 && pkObj2->IsNode())
    {
        NiNode* pkNode2 = (NiNode*)pkObj2;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode2->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj2))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj2)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode2->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
                pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

            if (FindCollisions(fDeltaTime, kRecord0, kNewRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindNotNOTESTAndCollisions(float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool bNoTest1, bool bNoTest2)
{
    // If pkObj1 is set up as NOTEST and PROPAGATE_ALWAYS, traverse the tree
    // and call FindCollision on everything in the scene graph that does not
    // have NOTEST and PROPAGATE_ALWAYS, depending on propagation flags.

    NiAVObject* pkObj1 = kRecord0.GetAVObject();

    if (bNoTest1 && pkObj1 && pkObj1->IsNode())
    {
        NiNode* pkNode1 = (NiNode*)pkObj1;
        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode1->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj1))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj1)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode1->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

            if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }

        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    // If pkObj2 is set up as NOTEST and PROPAGATE_ALWAYS, traverse the tree
    // and call FindCollision on everything in the scene graph that does not
    // have NOTEST and PROPAGATE_ALWAYS, depending on propagation flags.

    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    if (bNoTest2 && pkObj2 && pkObj2->IsNode())
    {
        NiNode* pkNode2 = (NiNode*)pkObj2;
        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode2->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj2))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj2)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode2->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
                pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

            if (FindCollisions(fDeltaTime, kRecord0, kNewRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TestCollisionsForRecord0(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollisionSuccess)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
            pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

        if (TestCollisions(fDeltaTime, kNewRecord0, kRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TestCollisionsForRecord1(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollisionSuccess)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
            pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

        if (TestCollisions(fDeltaTime, kRecord0, kNewRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TestPropagateCollisions(float fDeltaTime, 
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollisionSuccess)
{
    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

        if (pkCD1)
        {
            // Essentially we never lock down conditionals, but instead pass
            // along to their immediate children.
             if ((pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_SUCCESS && bCollisionSuccess) ||
                 (pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_FAILURE && !bCollisionSuccess))
             {
                NIASSERT(NiIsKindOf(NiNode,pkObj1));

                NiNode* pkNode = (NiNode*)pkObj1;
                unsigned int uiTotalCnt = pkNode->GetArrayCount();

                // Propagate to children.
                for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(uiCnt);

                    if (pkChild == NULL)
                        continue;

                    NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                        pkChild, kRecord0.GetMaxDepth(),
                        kRecord0.GetBinSize());

                    if (TestCollisions(fDeltaTime, kNewRecord0, kRecord1,
                        bCollisionSuccess)
                        == NiCollisionGroup::TERMINATE_COLLISIONS)
                    {
                        return NiCollisionGroup::TERMINATE_COLLISIONS;
                    }
                }
                return NiCollisionGroup::CONTINUE_COLLISIONS;
             }
        }

        // Begin the lock down.
        kRecord0.m_bLocked = true;
        if (TestPropagateCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            kRecord0.m_bLocked = false;
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
        kRecord0.m_bLocked = false;
    }

    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        if (!pkObj1 || pkObj1->IsLeaf())
            return NiCollisionGroup::CONTINUE_COLLISIONS;

        bool bPropagate;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

        if (pkCD1)
        {
            NiCollisionData::PropagationMode ePropagation1 =
                pkCD1->GetPropagationMode();

            if (pkCD1->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation1 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else
                {
                    bPropagate = true;
                }
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation1, 
                   bCollisionSuccess);
            }
        }
        else
        {
            bPropagate = true;
        }

        if (bPropagate)
        {
            return TestCollisionsForRecord0((NiNode*)pkObj1, fDeltaTime,
                kRecord0, kRecord1, bCollisionSuccess);
        }
    }
    else 
    {
        bool bPropagate;

        NiAVObject* pkObj2 = kRecord1.GetAVObject();
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        if (pkCD2)
        {
            NiCollisionData::PropagationMode ePropagation2 =
                pkCD2->GetPropagationMode();
        
            if (pkCD2->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation2 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else
                {
                    bPropagate = true;
                }
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation2, 
                    bCollisionSuccess);
            }
        }
        else
        {
            bPropagate = true;
        }

        if (bPropagate)
        {
            return TestCollisionsForRecord1((NiNode*)pkObj2, fDeltaTime,
                kRecord0, kRecord1, bCollisionSuccess);
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;  // Didn't need propagation.
}
//---------------------------------------------------------------------------
bool ShouldPropagationOccur(NiCollisionData::PropagationMode ePropagation,
    bool bCollisionSuccess)
{
    switch(ePropagation)
    {
        case NiCollisionData::PROPAGATE_ALWAYS:
            return true;
        case NiCollisionData::PROPAGATE_ON_SUCCESS:
            if ( bCollisionSuccess )
                return true;
            break;
        case NiCollisionData::PROPAGATE_ON_FAILURE:
            if ( !bCollisionSuccess )
                return true;
            break;
        default:
            break;
    }

    return false;
}
//---------------------------------------------------------------------------
int FindCollisionsForRecord0(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
            pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

        if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
            NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindCollisionsForRecord1(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
            pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

        if (FindCollisions(fDeltaTime, kRecord0, kNewRecord1) ==
            NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int PropagateCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, bool bCollisionSuccess)
{
    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

        if (pkCD1)
        {
            // Essentially we never lock down conditionals, but instead pass
            // along to their immediate children.
             if ((pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_SUCCESS && bCollisionSuccess) ||
                 (pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_FAILURE && !bCollisionSuccess))
             {
                NIASSERT(NiIsKindOf(NiNode,pkObj1));

                NiNode* pkNode = (NiNode*)pkObj1;

                unsigned int uiTotalCnt = pkNode->GetArrayCount();

                // Propagate to children.
                for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(uiCnt);

                    if (pkChild == NULL)
                        continue;

                    NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                        pkChild, kRecord0.GetMaxDepth(),
                        kRecord0.GetBinSize());

                    if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
                        NiCollisionGroup::TERMINATE_COLLISIONS)
                    {
                        return NiCollisionGroup::TERMINATE_COLLISIONS;
                    }
                }

                return NiCollisionGroup::CONTINUE_COLLISIONS;
             }
        }

        // Begin the lock down.
        kRecord0.m_bLocked = true;
        if (PropagateCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            kRecord0.m_bLocked = false;
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
        kRecord0.m_bLocked = false;
    }

    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        if (!pkObj1 || pkObj1->IsLeaf())
            return NiCollisionGroup::CONTINUE_COLLISIONS;
        
        bool bPropagate;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
 
        if (pkCD1)
        {
            NiCollisionData::PropagationMode ePropagation1 =
                pkCD1->GetPropagationMode();

            if (pkCD1->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation1 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else bPropagate = true;
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation1, 
                   bCollisionSuccess);
            }
        }
        else bPropagate = true;

        if (bPropagate)
            return FindCollisionsForRecord0((NiNode*)pkObj1, fDeltaTime,
                kRecord0, kRecord1);
    }
    else 
    {
        NiAVObject* pkObj2 = kRecord1.GetAVObject();

        bool bPropagate;
 
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        if (pkCD2)
        {
            NiCollisionData::PropagationMode ePropagation2 =
                pkCD2->GetPropagationMode();

            if (!pkObj2 || pkObj2->IsLeaf())
                bPropagate = false;
            else if (pkCD2->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation2 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else bPropagate = true;
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation2, 
                    bCollisionSuccess);
            }
        }
        else bPropagate = true;

        if (bPropagate)
            return FindCollisionsForRecord1((NiNode*)pkObj2, fDeltaTime,
                kRecord0, kRecord1);
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;  // Didn't need propagation.
}
//---------------------------------------------------------------------------
int TestHandleCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, bool& bCollision)
{
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    NiCollisionData::CollisionTest eCollisionTest
        = NiCollisionData::GetCollisionTestType(pkObj1, pkObj2);

    if (BoundsTestCheck(fDeltaTime, pkObj1, pkObj2, eCollisionTest) == false)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    switch(eCollisionTest)
    {
        case NiCollisionData::OBB_NIBOUND:
        case NiCollisionData::ABV_NIBOUND:
        case NiCollisionData::NIBOUND_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(TEST_NIBOUND_NIBOUND, 1);
            return Test_NIBOUNDvsNIBOUND(
                fDeltaTime, pkObj1, pkObj2, bCollision);

        case NiCollisionData::OBB_OBB:
        {
            NIMETRICS_COLLISION_ADDVALUE(TEST_OBB_OBB, 1);
            bool bResult = Test_OBBvsOBB(fDeltaTime, pkObj1, pkObj2, kRecord0, 
                kRecord1);

            if (bResult)
                bCollision = true;

            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }

        case NiCollisionData::OBB_TRI: 
        {
            NIMETRICS_COLLISION_ADDVALUE(TEST_OBB_TRI, 1);
            bool bResult = Test_OBBvsTRI(fDeltaTime, pkObj1, pkObj2, kRecord0,
                kRecord1);
            
            if (bResult)
                bCollision = true;

            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }

        case NiCollisionData::OBB_ABV:
            NIMETRICS_COLLISION_ADDVALUE(TEST_OBB_ABV, 1);
            return Test_OBBvsABV(fDeltaTime, pkObj1, pkObj2, kRecord0,
                kRecord1, bCollision);

        case NiCollisionData::TRI_TRI:
            NIMETRICS_COLLISION_ADDVALUE(TEST_TRI_TRI, 1);
            return Test_TRIvsTRI(fDeltaTime, pkObj1, pkObj2,bCollision);
        
        case NiCollisionData::TRI_ABV:      
        case NiCollisionData::TRI_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(TEST_TRI_ABV, 1);
            return Test_TRIvsABV(fDeltaTime, pkObj1, pkObj2, bCollision);
        
        case NiCollisionData::ABV_ABV:
            NIMETRICS_COLLISION_ADDVALUE(TEST_ABV_ABV, 1);
            return Test_ABVvsABV(fDeltaTime, pkObj1, pkObj2, bCollision);

        default:
            break;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int HandleCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, NiCollisionGroup::Intersect& kIntr,
    bool& bCollision)
{
    bCollision = false;
    bool bCalcNormals = true;

    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    kIntr.pkRoot0 = kRecord0.GetRoot();
    kIntr.pkObj0 = kRecord0.GetAVObject();
    kIntr.pkRoot1 = kRecord1.GetRoot();
    kIntr.pkObj1 = kRecord1.GetAVObject();

    NiCollisionData::CollisionTest eCollisionTest =
        NiCollisionData::GetCollisionTestType(pkObj1, pkObj2);

    if (BoundsTestCheck(fDeltaTime, pkObj1, pkObj2, eCollisionTest) == false)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    switch(eCollisionTest)
    {
        case NiCollisionData::OBB_NIBOUND:
        case NiCollisionData::ABV_NIBOUND:
        case NiCollisionData::NIBOUND_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(FIND_NIBOUND_NIBOUND, 1);
            return Find_NIBOUNDvsNIBOUND(fDeltaTime, pkObj1, pkObj2, 
                bCalcNormals, kIntr, bCollision);

        case NiCollisionData::OBB_OBB:
            NIMETRICS_COLLISION_ADDVALUE(FIND_OBB_OBB, 1);
            return Find_OBBvsOBB(fDeltaTime, pkObj1, pkObj2, kRecord0, 
                kRecord1, bCollision);

        case NiCollisionData::OBB_TRI: 
            NIMETRICS_COLLISION_ADDVALUE(FIND_OBB_TRI, 1);
            return Find_OBBvsTRI(fDeltaTime, pkObj1, pkObj2, kRecord0, 
                kRecord1, bCollision);

        case NiCollisionData::OBB_ABV:
            NIMETRICS_COLLISION_ADDVALUE(FIND_OBB_ABV, 1);
            return Find_OBBvsABV(fDeltaTime, pkObj1, pkObj2, kRecord0,
                kRecord1, true, kIntr, bCollision);

        case NiCollisionData::TRI_TRI:
            NIMETRICS_COLLISION_ADDVALUE(FIND_TRI_TRI, 1);
            return Find_TRIvsTRI(fDeltaTime, pkObj1, pkObj2, true, kIntr, 
                bCollision);
        
        case NiCollisionData::TRI_ABV:      
        case NiCollisionData::TRI_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(FIND_TRI_ABV, 1);
            return Find_TRIvsABV(fDeltaTime, pkObj1, pkObj2, true, kIntr,
                bCollision);
        
        case NiCollisionData::ABV_ABV:
            NIMETRICS_COLLISION_ADDVALUE(FIND_ABV_ABV, 1);
            return Find_ABVvsABV(fDeltaTime, pkObj1, pkObj2, bCalcNormals,
                kIntr, bCollision);

        default:
            break;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1)
{
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    // Look for collision data.
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    // In cases were there is no collision data, attempt to go deeper into the
    // scene graph to find it.  This approach is to support not requiring
    // collision data at every node.
    if (pkCD1 == NULL || pkCD2 == NULL)
        return FindCollisionDataAndCollisions(fDeltaTime, kRecord0, kRecord1);

    // NOTEST Optimization.
    bool bNoTest1 =
        pkCD1->GetPropagationMode() == NiCollisionData::PROPAGATE_ALWAYS &&
        pkCD1->GetCollisionMode() == NiCollisionData::NOTEST;
    bool bNoTest2 =
        pkCD2->GetPropagationMode() == NiCollisionData::PROPAGATE_ALWAYS &&
        pkCD2->GetCollisionMode() == NiCollisionData::NOTEST;
    if (bNoTest1 || bNoTest2)
    {
        return FindNotNOTESTAndCollisions(fDeltaTime, kRecord0, kRecord1,
            bNoTest1, bNoTest2);
    }

    // Steps to insure consistency for PROPAGATE_ON_FAILURE.  Note that
    // PROPAGATE_ON_FAILURE has been deprecated.
    if ((!(pkCD1->GetPropagationMode() ==
        NiCollisionData::PROPAGATE_ON_FAILURE)) && pkCD2->GetPropagationMode()
        == NiCollisionData::PROPAGATE_ON_FAILURE)
    {
        // Swap records.
        return FindCollisions(fDeltaTime, kRecord1, kRecord0);
    }

    NiCollisionData::CollisionMode eCollisionMode1 = 
        pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode eCollisionMode2 = 
        pkCD2->GetCollisionMode();

    bool bCollision = false;

    // NOTEST cases go directly to propagation handling, and only NOTEST with
    // PROPAGATE_ALWAYS will result in possible collisions.
    if (eCollisionMode1 != NiCollisionData::NOTEST &&
        eCollisionMode2 != NiCollisionData::NOTEST)
    {
        NiCollisionGroup::Intersect kIntr;

        kIntr.fTime = 0.0f; // Initialize.

        if (HandleCollisions(fDeltaTime, kRecord0, kRecord1, kIntr,
            bCollision) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }    
    }
    else
    {
        if (eCollisionMode1 == NiCollisionData::NOTEST ||
            eCollisionMode2 == NiCollisionData::NOTEST)
            bCollision = true;
    }

    return PropagateCollisions(fDeltaTime, kRecord0, kRecord1, bCollision);
}
//---------------------------------------------------------------------------
// ABV updates via scene graph traversal
//---------------------------------------------------------------------------
void UpdateWorldData(NiAVObject* pkRoot)
{
    if (pkRoot)
    {
        NiCollisionData* pkData = NiGetCollisionData(pkRoot);

        if (pkData)
            pkData->UpdateWorldData();

        if (pkRoot && pkRoot->IsNode())
        {
            NiNode* pkNode = NiStaticCast(NiNode, pkRoot);

            for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
            {
                NiAVObject* pkChild = pkNode->GetAt(i);

                if (!pkChild)
                    continue;

                NiCollisionTraversals::UpdateWorldData(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------

int Find_NIBOUNDvsNIBOUND(float fDeltaTime, NiAVObject* pkObj1,
    NiAVObject* pkObj2, bool bCalcNormals, NiCollisionGroup::Intersect& kIntr,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

   // Bounding volume will be converted to a sphereABV if possible.
    bool bResult = NiCollisionUtils::BoundingVolumeFindIntersect(fDeltaTime, 
        pkObj1, pkObj2, kIntr.fTime, kIntr.kPoint, bCalcNormals,
        kIntr.kNormal0, kIntr.kNormal1);

    if (bResult)
    {
        bCollision = true;

        // The rule of callbacks:  Since each object has its own callback, the
        // rule is that an object's callback is called only if the other
        // object's propagation flag is NEVER are ALWAYS.  I.e., it is not
        // conditional.
        return pkCD1->FindCollisionProcessing(kIntr,
            pkCD1->GetPropagationMode(), pkCD2->GetPropagationMode()); 
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int Find_OBBvsOBB(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    // Create OBB trees just in time.
    pkCD1->CreateOBB(kRecord0.GetBinSize());
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->FindOBBCollisions(fDeltaTime, pkCD2, kRecord0.GetRoot(),
        kRecord1.GetRoot(), kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth(), bCollision));
}
//---------------------------------------------------------------------------
int Find_OBBvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    // Create OBB trees just in time.
    pkCD1->CreateOBB(kRecord0.GetBinSize());
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->FindOBBCollisions(fDeltaTime, pkCD2, kRecord0.GetRoot(),
        kRecord1.GetRoot(), kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth(), bCollision));
}
//---------------------------------------------------------------------------
// There is no code to handle OBB vs. ABV collisions directly.  The strategy
// employed is:  if OBBs can easily be obtained for both, use them; otherwise
// use the triangles directly against the ABV.
int Find_OBBvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool, NiCollisionGroup::Intersect& kIntr, bool &bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() && pkCD2->GetWorldVertices())
    {
        pkCD1->UpdateWorldVertices();
        pkCD2->UpdateWorldVertices();

        // Create OBB trees just in time.
        pkCD1->CreateOBB(kRecord0.GetBinSize());
        pkCD2->CreateOBB(kRecord1.GetBinSize());         

        return (pkCD1->FindOBBCollisions(fDeltaTime, pkCD2,
            kRecord0.GetRoot(), kRecord1.GetRoot(), kRecord0.GetAVObject(),
            kRecord1.GetAVObject(), kRecord0.GetMaxDepth(),
            kRecord1.GetMaxDepth(), bCollision));
    }

    return NiCollisionUtils::TriToBndVolFindIntersect(fDeltaTime, pkObj1,
        pkObj2, true, kIntr, bCollision);
}
//---------------------------------------------------------------------------
int Find_TRIvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool, NiCollisionGroup::Intersect& kIntr, bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    return NiCollisionUtils::TriTriFindIntersect(fDeltaTime, pkObj1, pkObj2,
        true, kIntr, bCollision);
}
//---------------------------------------------------------------------------
int Find_ABVvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool bCalcNormals, NiCollisionGroup::Intersect& kIntr, bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    bool bResult = NiCollisionUtils::BoundingVolumeFindIntersect(fDeltaTime, 
        pkObj1, pkObj2, kIntr.fTime, kIntr.kPoint, bCalcNormals,
        kIntr.kNormal0, kIntr.kNormal1);

    if (bResult)
    {
        bCollision = true;

        // The rule of callbacks:  Since each object has its own callback, the
        // rule is that an object's callback is called only if the other
        // object's propagation flag is NEVER are ALWAYS.  I.e., it is not
        // conditional.
        return pkCD1->FindCollisionProcessing(kIntr,
            pkCD1->GetPropagationMode(), pkCD2->GetPropagationMode()); 
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int Find_TRIvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool, NiCollisionGroup::Intersect& kIntr, bool &bCollision)
{
    // Because we may want to exit on the first NiRenderObject collision 
    // detected, we enable this function to do callbacks
    return NiCollisionUtils::TriToBndVolFindIntersect(fDeltaTime, pkObj1,
        pkObj2, true, kIntr, bCollision);
}
//---------------------------------------------------------------------------
int Test_NIBOUNDvsNIBOUND(float fDeltaTime, NiAVObject* pkObj1,
    NiAVObject* pkObj2, bool& bCollision)
{
   // Bounding volume will be converted to a SphereABV if possible.
   bool bResult = NiCollisionUtils::BoundingVolumeTestIntersect(fDeltaTime,
       pkObj1, pkObj2); 

    if (bResult)
    {
        bCollision = true;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        NIASSERT(pkCD1);
        NIASSERT(pkCD2);
        if (!pkCD1 || !pkCD2)
        {
            NiOutputDebugString(
                "Warning:  Test_NIBOUNDvsNIBOUND() is failing since\n  "
                "at least one of the two objects has no collision data.\n");
        }

        NiCollisionData::PropagationMode ePropagationMode1
            = pkCD1->GetPropagationMode();
        NiCollisionData::PropagationMode ePropagationMode2
            = pkCD2->GetPropagationMode();

        if (pkCD1->TestCollisionProcessing(ePropagationMode1,
            ePropagationMode2) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
bool Test_OBBvsOBB(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2, 
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    // Create OBB trees just in time.
    pkCD1->CreateOBB(kRecord0.GetBinSize());
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->TestOBBCollisions(fDeltaTime, pkCD2,
        kRecord0.GetAVObject(), kRecord1.GetAVObject(),
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth()));
}
//---------------------------------------------------------------------------
bool Test_OBBvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2, 
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    pkCD1->CreateOBB(kRecord0.GetBinSize()); // Create OBB trees just in time.
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->TestOBBCollisions(fDeltaTime, pkCD2, 
        kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth()));
}
//---------------------------------------------------------------------------
// There is no code to handle OBB vs. ABV collisions directly.  The strategy
// employed is:  if OBBs can easily be obtained for both, use them; otherwise
// use the triangles directly against the ABV.
int Test_OBBvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() && pkCD2->GetWorldVertices())
    {
        pkCD1->UpdateWorldVertices();
        pkCD2->UpdateWorldVertices();

        // Create OBB trees just in time.
        pkCD1->CreateOBB(kRecord0.GetBinSize());
        pkCD2->CreateOBB(kRecord1.GetBinSize());         

        return (pkCD1->TestOBBCollisions(fDeltaTime, pkCD2, 
            kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
            kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth()));
    }

    return NiCollisionUtils::TriToBndVolTestIntersect(fDeltaTime, pkObj1,
        pkObj2, bCollision); 
}
//---------------------------------------------------------------------------
int Test_TRIvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    return NiCollisionUtils::TriTriTestIntersect(fDeltaTime, pkObj1, pkObj2,
        bCollision);
}
//---------------------------------------------------------------------------
int Test_TRIvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool &bCollision)
{
    return NiCollisionUtils::TriToBndVolTestIntersect(fDeltaTime, pkObj1,
        pkObj2, bCollision);
}
//---------------------------------------------------------------------------
int Test_ABVvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool& bCollision)                   
{
    bool bResult = NiCollisionUtils::BoundingVolumeTestIntersect(fDeltaTime,
        pkObj1, pkObj2);    

    if (bResult)
    {
        bCollision = true;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        NIASSERT(pkCD1);
        NIASSERT(pkCD2);
        if (!pkCD1 || !pkCD2)
        {
            NiOutputDebugString(
                "Warning:  Test_ABVvsABV() is failing since\n  "
                "at least one of the two objects has no collision data.\n");
        }

        NiCollisionData::PropagationMode ePropagationMode1
            = pkCD1->GetPropagationMode();
        NiCollisionData::PropagationMode ePropagationMode2 
            = pkCD2->GetPropagationMode();

        if (pkCD1->TestCollisionProcessing(ePropagationMode1,
            ePropagationMode2) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Mesh / Geometry releated methods
//---------------------------------------------------------------------------
NiBoundingVolume* OptimallyConvertToBoxBV(NiAVObject* pkObject)
{
    // Assert if this object, which has collision data set to
    // USE_TRI or USE_OBB, is not NiMesh-based geometry.
    // Such a case represents an error in how this object was
    // constructed, since USE_TRI or USE_OBB collision data
    // should be applied directly to NiMesh-based geometry in
    // the scene graph.  Also, to avoid other downstream issues
    // in Release or Shipping builds, this "garbage in" case is
    // treated as a non-collision.
    NIASSERT(NiIsKindOf(NiMesh, pkObject));
    if (NiIsKindOf(NiMesh, pkObject) == false)
    {
        NiOutputDebugString(
            "Warning:  An NiAVObject has failed OptimallyConvertToBoxBV() "
            "since\n  it's not an NiMesh-derived object.\n");
        return NULL;
    }

    NiMesh* pkMesh = (NiMesh*)pkObject;

    unsigned int uiVerts = pkMesh->GetVertexCount();
    if (uiVerts <= 25)
        return NULL;

    return NiBoundingVolume::ConvertToBoxBV(pkMesh);
}
//---------------------------------------------------------------------------
void CreateWorldVertices(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return; 

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData == NULL)
    {
        // If this object is geometry, assign collision data so that the
        // vertices may be created.
        if (NiIsKindOf(NiMesh, pkObj))
            pkColData = NiNew NiCollisionData(pkObj);
    }
    if (pkColData)
        pkColData->CreateWorldVertices();

    if (pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                CreateWorldVertices(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void CreateWorldNormals(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData == NULL)
    {
        // If this object is geometry, assign collision data so that the 
        // vertices may be created.
        if (NiIsKindOf(NiMesh, pkObj))
            pkColData = NiNew NiCollisionData(pkObj);
    }
    if (pkColData)
        pkColData->CreateWorldNormals();

    if (pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                CreateWorldNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void CreateCollisionData(NiAVObject* pkRoot,
    NiCollisionData::CollisionMode eCollision)
{
    if (pkRoot == NULL)
        return;

    NiCollisionData* pkData = NiGetCollisionData(pkRoot);

    // Create data only for NiMesh object.
    if (NiIsKindOf(NiMesh, pkRoot))
    {
        if (!pkData)
        {
            pkData = NiNew NiCollisionData(pkRoot);
            NIASSERT(pkData);
            pkData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);
            pkData->SetCollisionMode(eCollision);
            pkRoot->SetCollisionObject(pkData);
        }
    } 
    else if (pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                CreateCollisionData(pkChild, eCollision);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// OBB creation via scene graph traversal
//---------------------------------------------------------------------------
void CreateOBB(NiAVObject* pkRoot, int iBinSize)
{
    if (!pkRoot)
        return;

    if (pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);

            if (pkChild)
                CreateOBB(pkChild, iBinSize);
        }
    }
    else if (NiIsKindOf(NiMesh, pkRoot))
    {
        NiCollisionData* pkData = NiGetCollisionData(pkRoot);

        if (!pkData)
        {
            pkData = NiNew NiCollisionData(pkRoot);
            NIASSERT(pkData);
        }

        pkData->CreateOBB(iBinSize);
    }
}
//---------------------------------------------------------------------------
void DestroyOBB(NiAVObject* pkRoot)
{
    if (!pkRoot)
        return;

    if (pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                DestroyOBB(pkChild);
        }
    }
    else if (NiIsKindOf(NiMesh, pkRoot))
    {
        NiCollisionData* pkData = NiGetCollisionData(pkRoot);

        if (pkData)
           pkData->DestroyOBB();
    }
}
//---------------------------------------------------------------------------
// Picking scene graph traversal routine.
//---------------------------------------------------------------------------
bool FindIntersections(const NiPoint3& kOrigin, const NiPoint3& kDir,
    NiPick& kPick, NiAVObject* pkRoot)
{
    if (!pkRoot)
        return false;

    if (kPick.GetObserveAppCullFlag() && pkRoot->GetAppCulled())
        return false;

    bool bIntersectHere = false;
    bool bIntersectChildren = false;
    bool bPropagate = true;

    switch (kPick.GetIntersectType())
    {
    case NiPick::INTERSECT_BOUND:
        bIntersectHere = NiCollisionUtils::FindBoundIntersect(pkRoot,
            kOrigin, kDir);
        // add to pick on 2 conditions:
        // 1) Leaf node and QUERY_ALL
        if (bIntersectHere && kPick.GetQueryType() == NiPick::QUERY_ALL &&
            !pkRoot->IsNode())
        {
            kPick.Add(pkRoot);
        }
        // 2) QUERY_FIRST or QUERY_CLOSEST
        else if (bIntersectHere && kPick.GetQueryType() != NiPick::QUERY_ALL)
        {
            kPick.Add(pkRoot);
        }
        break;
    case NiPick::INTERSECT_COLLISION_DATA:
        // will add record to kPick if found
        // value of bPropagate will be updated
        bIntersectHere = FindIntersectionsCollisionData(kOrigin, kDir, kPick,
            pkRoot, bPropagate);
        break;
    case NiPick::INTERSECT_TRIANGLE:
        if (NiIsKindOf(NiRenderObject, pkRoot))
        {
            // will add record to kPick if found
            bIntersectHere = FindIntersectionsRenderObject(kOrigin, kDir, 
                kPick, (NiRenderObject*)pkRoot);
        }
        break;
    default:
        break;
    }
    
    // No need to process children if we only care about first find
    if (bIntersectHere && kPick.GetQueryType() == NiPick::QUERY_FIRST)
    {
        bPropagate = false;
    }

    if (bPropagate && pkRoot->IsNode())
    {
        bIntersectChildren = FindIntersectionsNode(kOrigin, kDir, kPick,
            (NiNode*)pkRoot);
    }

    return bIntersectHere | bIntersectChildren;
}
//---------------------------------------------------------------------------
bool FindIntersectionsSwitchNode(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiSwitchNode* pkSwitch)
{
    // Pick only on the active child.
    bool bFound = false;

    if (pkSwitch->GetIndex() >= 0)
    {
        NiAVObject* pkChild = pkSwitch->GetActiveChild();

        if (pkChild)
        {
            if (FindIntersections(kOrigin, kDir, kPick, pkChild))
                bFound = true;
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool FindIntersectionsNode(const NiPoint3& kOrigin, const NiPoint3& kDir,
    NiPick& kPick, NiNode* pkNode)
{
#if NIMETRICS
    kPick.m_uiNodeComparisons++;
#endif

    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        return FindIntersectionsSwitchNode(kOrigin, kDir, kPick, 
            (NiSwitchNode*)pkNode);
    }

    bool bFound = false;
    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        // if recursive call below results in elements in kPick's array
        // of intersections and we only want the first, return early
        if (kPick.GetQueryType() == NiPick::QUERY_FIRST && kPick.GetSize())
        {
           return true;
        }

        NiAVObject* pkChild = pkNode->GetAt(i);

        if (!pkChild)
            continue;

        if (FindIntersections(kOrigin, kDir, kPick, pkChild))
            bFound = true;
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool FindIntersectionsCollisionData(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiAVObject* pkObj, bool& bPropagate)
{
    // assume that any CollisionObject is a CollisionData. 
    NiCollisionData* pkCollData = NiDynamicCast(NiCollisionData,
        pkObj->GetCollisionObject());

    bool bIntersectHere = false;

    // if no collision data, early out based on fallback
    if (pkCollData == NULL)
    {
        // want to always propagate on null collision data
        bPropagate = true;
        switch (kPick.GetFallbackType())
        {
        case NiPick::FALLBACK_BOUND:
            bIntersectHere = NiCollisionUtils::FindBoundIntersect(pkObj, 
                kOrigin, kDir);
            if (bIntersectHere && kPick.GetQueryType() != NiPick::QUERY_ALL)
            {
                kPick.Add(pkObj);
            }
            break;
        case NiPick::FALLBACK_TRIANGLE:
            if (NiIsKindOf(NiRenderObject, pkObj))
            {
                // will add record to kPick if found
                bIntersectHere = FindIntersectionsRenderObject(
                    kOrigin, kDir, kPick, (NiRenderObject*)pkObj);
            }
            break;
        case NiPick::FALLBACK_FALSE:
        default:
            break;
        }

        return bIntersectHere;
    }

    NiBoundingVolume* pkBV = pkCollData->GetWorldSpaceABV();
    if (pkBV != NULL)
    {
        bool bCollides = false;

        NiPick::Record kTempRecord(NULL);

        switch (pkBV->Type())
        {
        case NiBoundingVolume::CAPSULE_BV:
            {
                NiCapsuleBV* pkCap = (NiCapsuleBV*) pkBV;
                bCollides = NiCollisionUtils::FindRayCapsuleIntersect(kOrigin,
                    kDir, *pkCap, kTempRecord);
            }
            break;
        case NiBoundingVolume::BOX_BV:
            {
                NiBoxBV* pkBox = (NiBoxBV*) pkBV;
                bCollides = NiCollisionUtils::FindRayBoxIntersect(kOrigin,
                    kDir, *pkBox, kTempRecord);
            }
            break;
        case NiBoundingVolume::HALFSPACE_BV:
            {
                NiHalfSpaceBV* pkHalfSpace = (NiHalfSpaceBV*) pkBV;
                bCollides = NiCollisionUtils::FindRayHalfSpaceIntersect(
                    kOrigin, kDir, *pkHalfSpace, kTempRecord);
            }
            break;
        case NiBoundingVolume::SPHERE_BV:
            {
                NiSphereBV* pkSphere = (NiSphereBV*) pkBV;
                bCollides = NiCollisionUtils::FindRaySphereIntersect(kOrigin,
                    kDir, *pkSphere, kTempRecord);
            }
            break;
        case NiBoundingVolume::UNION_BV:
            {
                NiUnionBV* pkUnion = (NiUnionBV*) pkBV;
                bCollides = NiCollisionUtils::FindRayUnionIntersect(kOrigin,
                    kDir, *pkUnion, kTempRecord);
            }
            break;
        }

        if (bCollides)
        {
            bIntersectHere = true;

            switch (pkCollData->GetPropagationMode())
            {
            case NiCollisionData::PROPAGATE_ALWAYS:
                bPropagate = true;
                break;
            case NiCollisionData::PROPAGATE_NEVER:
                bPropagate = false;
                break;
            case NiCollisionData::PROPAGATE_ON_SUCCESS:
                bPropagate = bIntersectHere;
                break;
            default:
                bPropagate = true;
                break;
            }

            NiPick::Record* pkRecord = kPick.Add(pkObj);
            pkRecord->SetDistance(kTempRecord.GetDistance());
            SetIntersectionData(kPick, &kTempRecord, pkRecord);
        }
    }

    return bIntersectHere;
}
//---------------------------------------------------------------------------
// Helper for FindIntersectionsMesh -- not in header
//---------------------------------------------------------------------------
bool ValidateLockAgainstVerts(bool bNeedValidate,
    NiDataStreamElementLock& kLockElem,
    NiDataStreamElementLock& kLockVerts)
{
    if (bNeedValidate == false)
        return true;
    
    // If the lock failed, no further validation is possible.
    // If the lock succeeded, but it's a single-entry data stream,
    // return true because the next test is invalid.
    if (!kLockElem.IsLocked() || 
        kLockElem.GetDataStream()->GetGPUConstantSingleEntry())
    {
        return true;
    }

    // Colors, Normals, Texcoords are indexed in the same manner that verts
    // are, so they must be the same.
    return (kLockElem.count() == kLockVerts.count());
}
//---------------------------------------------------------------------------
bool FindIntersectionsRenderObject(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiRenderObject* pkRenderObj)
{   
    // early out if no bounds intersect
    if (!NiCollisionUtils::FindBoundIntersect(pkRenderObj, kOrigin, kDir))
    {
        return false;
    }

    NiPick::PickObjectPolicy* pkPickPolicy = kPick.GetPickObjectPolicy();

    NIASSERT(pkPickPolicy != NULL);

    return pkPickPolicy->FindIntersections(kOrigin, kDir, kPick, pkRenderObj);

}
//---------------------------------------------------------------------------
bool FindIntersectionsMeshPosition(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiMesh* pkMesh)
{   
    // Code assumes that type is triangles or tristrips.
    if (pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRISTRIPS &&
        pkMesh->GetPrimitiveType() != NiPrimitiveType::PRIMITIVE_TRIANGLES)
    {
        NiOutputDebugString(
            "Warning:  No intersections are being returned for an "
            "NiMesh-derived object in FindIntersectionsMesh() because\n  its "
            "primitive type is not triangles or tristrips.\n");
        return false;
    }

    // The collision detection system requires 32-bit floats for vertex
    // coordinates, which is the default data type for export from the
    // art packages.
    NiDataStreamElementLock kLockVerts(pkMesh, NiCommonSemantics::POSITION(),
        0, NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_READ);

    if (!kLockVerts.IsLocked())
    {
        NiOutputDebugString(
            "Warning:  Geometry for NiPick operation is missing.\n"
            "  May need to ensure that CPU_READ is specified for POSITION\n");
        return false;
    } 

    const NiTransform& kWorld = pkMesh->GetWorldTransform();
    NiPoint3 kModelOrigin;
    NiPoint3 kModelDir;

    // Find ray's model space kOrigin and kDir.
    NiCollisionUtils::ConvertRayFromWorldToModel(kWorld,
        kOrigin, kDir, kModelOrigin, kModelDir);

    NiVertIter kVertIter = kLockVerts.begin<NiPoint3>();

    // The same assumption above is also made regarding color, normals,
    // and UVs - however, these attributes may or may not be requested.
    NiDataStreamElementLock kLockColors(pkMesh, NiCommonSemantics::COLOR(),
        0, NiDataStreamElement::F_FLOAT32_4, NiDataStream::LOCK_READ);
    NiDataStreamElementLock kLockNormals(pkMesh, NiCommonSemantics::NORMAL(),
        0, NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_READ);
    NiDataStreamElementLock kLockTexCoords(pkMesh,
        NiCommonSemantics::TEXCOORD(), 0, NiDataStreamElement::F_FLOAT32_2,
        NiDataStream::LOCK_READ);

    // Set to a default NULL iterator
    NiColorIter kColorIter;
    NiNormalIter kNormalIter;
    NiTexCoordIter kTexCoordIter;

    if (kLockColors.IsLocked())
        kColorIter = kLockColors.begin<NiColorA>();

    if (kLockNormals.IsLocked())
        kNormalIter = kLockNormals.begin<NiPoint3>();

    if (kLockTexCoords.IsLocked())
        kTexCoordIter = kLockTexCoords.begin<NiPoint2>();

    // Validate that if color, normal, or texture is being requested,
    // that the stream has a single region matching the verts
    NIASSERT(ValidateLockAgainstVerts(kPick.GetReturnColor(),
        kLockColors, kLockVerts));
    NIASSERT(ValidateLockAgainstVerts(kPick.GetReturnNormal(),
        kLockNormals, kLockVerts));
    NIASSERT(ValidateLockAgainstVerts(kPick.GetReturnTexture(),
        kLockTexCoords, kLockVerts));

    FindMesh kFindMesh(kPick, pkMesh, kWorld, kModelOrigin, kModelDir,
        kLockVerts.GetDataStreamRef(), kVertIter,
        kLockColors.GetDataStreamRef(), kColorIter,
        kLockNormals.GetDataStreamRef(), kNormalIter,
        kLockTexCoords.GetDataStreamRef(), kTexCoordIter);

    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(pkMesh, kFindMesh,
        NiDataStream::LOCK_READ, true);

    return kFindMesh.m_bFound;
}
//---------------------------------------------------------------------------
bool NICOLLISION_ENTRY FindIntersectionsSkinnedMeshBounds(
    const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
    NiMesh* pkMesh)
{
    NiSkinningMeshModifier* pkSkinMod = NiGetModifier(NiSkinningMeshModifier,
        pkMesh);

    if (!pkSkinMod)
        return false;

    NiUInt32 uiBoneCount = pkSkinMod->GetBoneCount();
    NiBound* pkBoneWorldBounds = pkSkinMod->GetBoneBounds();

    if (!pkBoneWorldBounds)
        return false;

    NiPick::Record kTempRecord(NULL);
    NiUInt32 uiCollisions = 0;

    // Find any bone bounds that intersect the ray.
    for (NiUInt32 ui = 0; ui < uiBoneCount; ui++)
    {
        NiBound kBoneBound;
        kBoneBound.Update(pkBoneWorldBounds[ui], 
            pkSkinMod->GetWorldBoneMatrix(ui));
        if (NiCollisionUtils::FindBoundIntersect(kBoneBound, kOrigin, kDir))
        {
            NiSphereBV kTestSphereBV(kBoneBound.GetRadius(),
                kBoneBound.GetCenter());

            // Add a new record if it exists.
            if (uiCollisions == 0)
            {
                NIVERIFY(NiCollisionUtils::FindRaySphereIntersect(kOrigin, 
                    kDir, kTestSphereBV, kTempRecord));
            }
            else
            {
                NiPick::Record kTestTempRecord(NULL);
                NIVERIFY(NiCollisionUtils::FindRaySphereIntersect(kOrigin,
                    kDir, kTestSphereBV, kTestTempRecord));

                // If the new ray intersection is closer, then choose to return
                // the new intersection instead of the old.
                if (kTestTempRecord.GetDistance() >
                    kTempRecord.GetDistance())
                {                    
                    kTempRecord.SetIntersection(
                        kTestTempRecord.GetIntersection());
                    kTempRecord.SetNormal(kTestTempRecord.GetNormal());
                    kTempRecord.SetDistance(kTestTempRecord.GetDistance());
                }
            }

            uiCollisions++;
        }
    }

    // No collisions occurred, exit
    if (0 == uiCollisions)
        return false;

    // Set the real pick record.
    NiPick::Record* pkRecord = kPick.Add(pkMesh);
    SetIntersectionData(kPick, &kTempRecord, pkRecord);
    pkRecord->SetDistance(kTempRecord.GetDistance());

    return true;

}
//---------------------------------------------------------------------------
void UpdateTexCoordRecord(NiPick& kPick, NiPick::Record* pkRecord,
    const float fTriParam0, const float fTriParam1, const float fTriParam2,
    const NiUInt32* puiIndices, NiUInt16 uiSubMesh,
    NiDataStreamRef* pkTexCoordStreamRef, NiTexCoordIter& kTexCoordIter)
{
    NIASSERT(pkRecord);
    if (kPick.GetReturnTexture())
    {
        if (kTexCoordIter.Exists())
        {
            NiUInt32 uiOffset = pkTexCoordStreamRef->
                GetRegionForSubmesh(uiSubMesh).GetStartIndex();
            NiPoint2 kTexture = 
                fTriParam0 * kTexCoordIter[puiIndices[0] + uiOffset] +
                fTriParam1 * kTexCoordIter[puiIndices[1] + uiOffset] +
                fTriParam2 * kTexCoordIter[puiIndices[2] + uiOffset];
            pkRecord->SetTexture(kTexture);
        }
        else
        {
            pkRecord->SetTexture(NiPoint2::ZERO);
        }
    }
    else
    {
        pkRecord->SetTexture(NiPoint2::ZERO);
    }
}
//---------------------------------------------------------------------------
void UpdateNormalRecord(NiPick& kPick, NiPick::Record* pkRecord,
    const float fTriParam0, const float fTriParam1, const float fTriParam2,
    const NiUInt32* puiIndices, NiUInt16 uiSubMesh,
    NiDataStreamRef* pkNormalStreamRef, NiNormalIter& kNormalIter,
    NiDataStreamRef* pkVertStreamRef, NiVertIter& kVertIter,
    const NiTransform& kWorld)
{
    if (kPick.GetReturnNormal())
    {
        NiPoint3 kNormal;
        if (kNormalIter.Exists() && 
            pkNormalStreamRef->GetDataStream()->GetGPUConstantSingleEntry())
        {
            kNormal = kNormalIter[0];
        }
        else if (kPick.GetReturnSmoothNormal() && kNormalIter.Exists())
        {
            NiUInt32 uiOffset = pkNormalStreamRef->
                GetRegionForSubmesh(uiSubMesh).GetStartIndex();
            kNormal =
                fTriParam0 * kNormalIter[puiIndices[0] + uiOffset] +
                fTriParam1 * kNormalIter[puiIndices[1] + uiOffset] +
                fTriParam2 * kNormalIter[puiIndices[2] + uiOffset];
        }
        else
        {
            NiPoint3 v0, kV1;

            NiUInt32 uiOffset = pkVertStreamRef->
                GetRegionForSubmesh(uiSubMesh).GetStartIndex();
            
            v0 = kVertIter[puiIndices[1] + uiOffset] -
                kVertIter[puiIndices[0] + uiOffset];
            kV1 = kVertIter[puiIndices[2] + uiOffset] -
                kVertIter[puiIndices[0] + uiOffset];
            kNormal = v0.Cross(kV1);
        }
        kNormal.Unitize();
        
        if (kPick.GetCoordinateType() == NiPick::COORDINATES_WORLD)
        {
            kNormal = kWorld.m_Rotate * kNormal;
        }
        
        pkRecord->SetNormal(kNormal);
    }
    else
    {
        pkRecord->SetNormal(NiPoint3::ZERO);
    }
}
//---------------------------------------------------------------------------
void UpdateColorRecord(NiPick& kPick, NiPick::Record* pkRecord,
    const float fTriParam0, const float fTriParam1, const float fTriParam2,
    const NiUInt32* puiIndices, NiUInt16 uiSubMesh,
    NiDataStreamRef* pkColorStreamRef, NiColorIter& kColorIter)
{
    if (kPick.GetReturnColor())
    {
        if (kColorIter.Exists())
        {
            NiUInt32 uiOffset = pkColorStreamRef->
                GetRegionForSubmesh(uiSubMesh).GetStartIndex();

            NiColorA color =
                fTriParam0 * kColorIter[puiIndices[0] + uiOffset] +
                fTriParam1 * kColorIter[puiIndices[1] + uiOffset] +
                fTriParam2 * kColorIter[puiIndices[2] + uiOffset];
            pkRecord->SetColor(color);
        }
        else
        {
            pkRecord->SetColor(NiColorA::WHITE);
        }
    }
    else
    {
        pkRecord->SetColor(NiColorA::WHITE);
    }
}
//---------------------------------------------------------------------------
// Functor operator() definition for FindMesh used with NiMeshAlgorithms
//---------------------------------------------------------------------------
bool FindMesh::operator ()(const NiUInt32* pIndices, NiUInt32, 
    NiUInt32 uiTri, NiUInt16 uiSubMesh)
{
#if NIMETRICS
    m_kPick.m_uiTriComparisons++;
#endif
    NiUInt32 uiVertexOffset =
        m_pkVertStreamRef->GetRegionForSubmesh(uiSubMesh).GetStartIndex();
     
    NiUInt32 uiIndices[3];
    uiIndices[0] = pIndices[0] + uiVertexOffset;
    uiIndices[1] = pIndices[1] + uiVertexOffset;
    uiIndices[2] = pIndices[2] + uiVertexOffset;

    NiPoint3 kIntersect;
    float fLineParam, fTriParam1, fTriParam2;

    bool bIntersects = NiCollisionUtils::IntersectTriangle(
        m_kModelOrigin, m_kModelDir, m_kVertIter[uiIndices[0]],
        m_kVertIter[uiIndices[1]], m_kVertIter[uiIndices[2]],
        m_kPick.GetFrontOnly(), kIntersect, fLineParam, fTriParam1,
        fTriParam2);

    if (bIntersects)
    {
        m_bFound = true;

        // Update pick record...
        NiPick::Record* pkRecord = m_kPick.Add(m_pkMesh);
        if (m_kPick.GetCoordinateType() == NiPick::COORDINATES_WORLD)
        {
            kIntersect = m_kWorld.m_fScale * 
                (m_kWorld.m_Rotate * kIntersect) + m_kWorld.m_Translate;
        }
        pkRecord->SetIntersection(kIntersect);
        pkRecord->SetDistance(fLineParam * m_kWorld.m_fScale);
        pkRecord->SetSubmeshIndex((NiUInt32)uiSubMesh);
        pkRecord->SetTriangleIndex(uiTri); 
        pkRecord->SetVertexIndices(pIndices[0], pIndices[1], pIndices[2]);

        float fTriParam0 = 1.0f - (fTriParam1 + fTriParam2);

        UpdateTexCoordRecord(m_kPick, pkRecord, fTriParam0, fTriParam1,
            fTriParam2, pIndices, uiSubMesh, m_pkTexCoordStreamRef,
            m_kTexCoordIter);
        UpdateNormalRecord(m_kPick, pkRecord, fTriParam0, fTriParam1,
            fTriParam2, pIndices, uiSubMesh, m_pkNormalStreamRef,
            m_kNormalIter,
            m_pkVertStreamRef, m_kVertIter, m_kWorld);
        UpdateColorRecord(m_kPick, pkRecord, fTriParam0, fTriParam1,
            fTriParam2, pIndices, uiSubMesh, m_pkColorStreamRef,
            m_kColorIter);

        if (m_kPick.GetQueryType() == NiPick::QUERY_FIRST)
            return false; // break
    }
    return true;
}
//---------------------------------------------------------------------------
} // namespace
