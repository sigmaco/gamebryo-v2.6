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

#ifndef NICOLLISIONTRAVERSALS_H
#define NICOLLISIONTRAVERSALS_H

#include "NiCollisionGroup.h"
#include "NiCollisionData.h"

class NiAVObject;
class NiPick;
class NiNode;
class NiSwitchNode;
class NiMesh;
class NiRenderObject;

// These routines isolate the scene graph traversal mechanisms of
// the collision system

namespace NiCollisionTraversals
{
    // collision data via scene graph traversal
    void NICOLLISION_ENTRY CreateCollisionData(NiAVObject* pkRoot,
        NiCollisionData::CollisionMode eCollision = 
        NiCollisionData::USE_OBB);
    bool NICOLLISION_ENTRY CheckForCollisionData(NiAVObject* pkRoot, 
        bool bRecursive);
    void NICOLLISION_ENTRY ClearWorldVelocities(NiAVObject* pkRoot,
        NiPoint3& kValue);

    // world vertices and normals via scene graph traversal
    void NICOLLISION_ENTRY CreateWorldVertices(NiAVObject* pkObj);
    void NICOLLISION_ENTRY UpdateWorldVertices(NiAVObject* pkObj);
    void NICOLLISION_ENTRY DestroyWorldVertices(NiAVObject* pkObj);
    void NICOLLISION_ENTRY CreateWorldNormals(NiAVObject* pkObj);
    void NICOLLISION_ENTRY UpdateWorldNormals(NiAVObject* pkObj);
    void NICOLLISION_ENTRY DestroyWorldNormals(NiAVObject* pkObj);

    // ABV updates via scene graph traversal
    void NICOLLISION_ENTRY UpdateWorldData(NiAVObject* pkRoot);

    // *** begin Emergent internal use only ***

    // OBB creation via scene graph traversal
    void NICOLLISION_ENTRY CreateOBB(NiAVObject* pkRoot, int iBinSize = 1);
    void NICOLLISION_ENTRY DestroyOBB();

    // collision detection scene graph traversal routines
    bool NICOLLISION_ENTRY ShouldPropagationOccur(
        NiCollisionData::PropagationMode ePropagation,
        bool bCollisionSuccess);

    int NICOLLISION_ENTRY TestPropagateCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollisionSuccess);

    bool NICOLLISION_ENTRY TestCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1);

    int NICOLLISION_ENTRY TestCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1,
        bool& bCollision);

    int NICOLLISION_ENTRY TestCollisionsForRecord0(NiNode* pkNode, 
        float fDeltaTime, NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollisionSuccess);

    int NICOLLISION_ENTRY TestCollisionsForRecord1(NiNode* pkNode,
        float fDeltaTime, NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollisionSuccess);

    int NICOLLISION_ENTRY TestHandleCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollision);

    int NICOLLISION_ENTRY PropagateCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool bCollisionSuccess);

    int NICOLLISION_ENTRY FindCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1);

    int NICOLLISION_ENTRY FindCollisionsForRecord0(NiNode* pkNode, 
        float fDeltaTime,
        NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1);

    int NICOLLISION_ENTRY FindCollisionsForRecord1(NiNode* pkNode, 
        float fDeltaTime, NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1);

    int NICOLLISION_ENTRY FindCollisionDataAndCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1);

    int NICOLLISION_ENTRY FindNotNOTESTAndCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1, bool bNoTest1, bool bNoTest2);

    int NICOLLISION_ENTRY TestCollisionDataAndCollisions(float fDeltaTime,
        NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollision);

    // picking scene graph traversal routine
    bool NICOLLISION_ENTRY FindIntersections(const NiPoint3& kOrigin,
        const NiPoint3& kDir, NiPick& kPick, NiAVObject* pkRoot);

    bool NICOLLISION_ENTRY FindIntersectionsNode(const NiPoint3& kOrigin,
        const NiPoint3& kDir, NiPick& kPick, NiNode* pkNode);

    bool NICOLLISION_ENTRY FindIntersectionsSwitchNode(
        const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
        NiAVObject* pkSwitch);

    bool NICOLLISION_ENTRY FindIntersectionsRenderObject(
        const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
        NiRenderObject* pkRenderObj);

    bool NICOLLISION_ENTRY FindIntersectionsMeshPosition(
        const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
        NiMesh* pkMesh);

    bool NICOLLISION_ENTRY FindIntersectionsSkinnedMeshBounds(
        const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick, 
        NiMesh* pkMesh);

    bool NICOLLISION_ENTRY FindIntersectionsCollisionData(
        const NiPoint3& kOrigin, const NiPoint3& kDir, NiPick& kPick,
        NiAVObject* pkObj, bool& bPropagate);

    int NICOLLISION_ENTRY Find_NIBOUNDvsNIBOUND(float fDeltaTime,
        NiAVObject* pkObj1, NiAVObject* pkObj2,
        bool bCalcNormals, NiCollisionGroup::Intersect& kIntr, 
        bool& bCollision);

    int NICOLLISION_ENTRY Find_OBBvsOBB(float fDeltaTime, NiAVObject* pkObj1, 
        NiAVObject* pkObj2, NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollision);

    int NICOLLISION_ENTRY Find_OBBvsTRI(float fDeltaTime, NiAVObject* pkObj1, 
        NiAVObject* pkObj2, NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1, bool& bCollision);

    int NICOLLISION_ENTRY Find_OBBvsABV(float fDeltaTime, NiAVObject* pkObj1,
        NiAVObject* pkObj2, NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1, bool bNormals,
        NiCollisionGroup::Intersect& kIntr, bool& bCollision);

    int NICOLLISION_ENTRY Find_TRIvsTRI(float fDeltaTime, NiAVObject* pkObj1,
        NiAVObject* pkObj2, bool bNormals,
        NiCollisionGroup::Intersect& kIntr, bool &bCollision);

    int NICOLLISION_ENTRY Find_TRIvsABV(float fDeltaTime, NiAVObject* pkObj1, 
        NiAVObject* pkObj2, bool bCalcNormals,
        NiCollisionGroup::Intersect& kIntr, bool& bCollision);

    int NICOLLISION_ENTRY Find_ABVvsABV(float fDeltaTime, NiAVObject* pkObj1, 
        NiAVObject* pkObj2, bool bCalcNormals,
        NiCollisionGroup::Intersect& kIntr, bool& bCollision);

    int NICOLLISION_ENTRY Test_NIBOUNDvsNIBOUND(float fDeltaTime,
        NiAVObject* pkObj1, NiAVObject* pkObj2, bool& bCollision);

    bool NICOLLISION_ENTRY Test_OBBvsOBB(float fDeltaTime, 
        NiAVObject* pkObj1, NiAVObject* pkObj2,
        NiCollisionGroup::Record& kRecord0, 
        NiCollisionGroup::Record& kRecord1);

    bool NICOLLISION_ENTRY Test_OBBvsTRI(float fDeltaTime, 
        NiAVObject* pkObj1, NiAVObject* pkObj2,
        NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1);

    int NICOLLISION_ENTRY Test_OBBvsABV(float fDeltaTime, NiAVObject* pkObj1,
        NiAVObject* pkObj2, NiCollisionGroup::Record& kRecord0,
        NiCollisionGroup::Record& kRecord1, bool& bCollision);

    int NICOLLISION_ENTRY Test_TRIvsTRI(float fDeltaTime, NiAVObject* pkObj1, 
        NiAVObject* pkObj2, bool& bCollision);

    int NICOLLISION_ENTRY Test_TRIvsABV(float fDeltaTime, NiAVObject* pkObj1,
        NiAVObject* pkObj2, bool& bCollision);

    int NICOLLISION_ENTRY Test_ABVvsABV(float fDeltaTime, NiAVObject* pkObj1,
        NiAVObject* pkObj2, bool& bCollision);

    int NICOLLISION_ENTRY BoundsTestCheck(float fDeltaTime, 
        NiAVObject* pkObj1, NiAVObject* pkObj2, 
        NiCollisionData::CollisionTest eCollisionTest);

    
    // *** end Emergent internal use only ***
}    

#endif
