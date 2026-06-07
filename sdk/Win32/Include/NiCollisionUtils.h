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

#ifndef NICOLLISIONUTILS_H
#define NICOLLISIONUTILS_H

#include "NiCollisionLibType.h"
#include "NiCollisionGroup.h"
#include "NiPick.h"

class NiAVObject;
class NiCapsuleBV;
class NiSphereBV;
class NiBoxBV;
class NiHalfSpaceBV;
class NiUnionBV;

namespace NiCollisionUtils
{
    // bounding volume intersection routines
    bool NICOLLISION_ENTRY BoundingVolumeTestIntersect(float fTime,
        NiAVObject* pkObj1, NiAVObject* pkObj2);
    bool NICOLLISION_ENTRY BoundingVolumeFindIntersect(float fTime,
        NiAVObject* pkObj1, NiAVObject* pkObj2, float& fIntrTime,
        NiPoint3& kIntrPoint, bool bCalcNormals, NiPoint3& kNormal0,
        NiPoint3& kNormal1);

    // triangle-[triangle | bv] intersection routines
    int NICOLLISION_ENTRY TriTriTestIntersect(float fTime, 
        NiAVObject* pkGeom1, NiAVObject* pkGeom2, bool& bCollision);
    int NICOLLISION_ENTRY TriTriFindIntersect(float fTime,
        NiAVObject* pkGeom1, NiAVObject* pkGeom2, bool bCalcNormals, 
        NiCollisionGroup::Intersect& kIntr, bool& bCollision);
    int NICOLLISION_ENTRY TriToBndVolTestIntersect(float fTime,
        NiAVObject* pkTBG, NiAVObject* pkOther, bool& bCollision);
    int NICOLLISION_ENTRY TriToBndVolFindIntersect(float fTime, 
        NiAVObject* pkTBG, NiAVObject* pkOther, bool bCalcNormals, 
        NiCollisionGroup::Intersect& kIntr, bool& bCollision);

    // picking intersection routines
    bool NICOLLISION_ENTRY FindBoundIntersect(NiAVObject* pkObj, 
        const NiPoint3& kOrigin, const NiPoint3& kDir);
    inline bool NICOLLISION_ENTRY FindBoundIntersect(const NiBound& kWorldBound, 
        const NiPoint3& kOrigin, const NiPoint3& kDir);
    bool NICOLLISION_ENTRY IntersectTriangle(const NiPoint3& kOrigin,
        const NiPoint3& kDir, const NiPoint3& kV1, const NiPoint3& kV2,
        const NiPoint3& kV3, bool bCull, NiPoint3& kIntersect, float& r,
        float& s, float& t);
    void NICOLLISION_ENTRY ConvertRayFromWorldToModel(
        const NiTransform& kWorld, const NiPoint3& kWorldOrigin, 
        const NiPoint3& kWorldDir, NiPoint3& kModelOrigin, 
        NiPoint3& kModelDir);

    // CollisionData picking intersection routines.
    // intersection point, normal, distance are returned in the NiPick::Record.
    // Always check the GetRayInsideABV() method before using the intersection
    // and normal values. If the ray begins inside the ABV, then these values
    // will be set to the ray origin and direction, as no valid collision
    // point and normal can be computed.
    // The m_spObject field of the pick record will be always be uninitialized
    // as this is lower-level than NiAVObjects.
    bool NICOLLISION_ENTRY FindRaySphereIntersect(
        const NiPoint3& kRayOrigin, 
        const NiPoint3& kRayDirection,
        const NiSphereBV& kSphere, 
        NiPick::Record& kRecord);
    bool NICOLLISION_ENTRY FindRayCapsuleIntersect(
        const NiPoint3& kRayOrigin, 
        const NiPoint3& kRayDirection,
        const NiCapsuleBV& kCapsule, 
        NiPick::Record& kRecord);
    bool NICOLLISION_ENTRY FindRayHalfSpaceIntersect(
        const NiPoint3& kRayOrigin, 
        const NiPoint3& kRayDirection, 
        const NiHalfSpaceBV& kHalfSpace, 
        NiPick::Record& kRecord);
    bool NICOLLISION_ENTRY FindRayBoxIntersect(
        const NiPoint3& kRayOrigin, 
        const NiPoint3& kRayDirection, 
        const NiBoxBV& kBox, 
        NiPick::Record& kRecord);
    bool NICOLLISION_ENTRY FindRayUnionIntersect(
        const NiPoint3& kRayOrigin, 
        const NiPoint3& kRayDirection, 
        const NiUnionBV& kUnion, 
        NiPick::Record& kRecord);
}

#include "NiCollisionUtils.inl"

#endif
