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

#ifndef NIHALFSPACEBV_H
#define NIHALFSPACEBV_H

#include "NiBoundingVolume.h"
#include "NiPlane.h"

// A half space has a plane boundary.  The normal vector of the plane points
// to the half space.  If the plane is Dot(N,X) = c, then a point P is inside
// the half space whenever Dot(N,P) >= c.

class NICOLLISION_ENTRY NiHalfSpaceBV : public NiBoundingVolume
{
    NiDeclareBVStream;
public:
    NiHalfSpaceBV(const NiPoint3& kPoint = NiPoint3::ZERO, 
        const NiPoint3& kNormal = NiPoint3::UNIT_Y);
    NiHalfSpaceBV(const NiHalfSpaceBV& kABV);

    inline void SetCenter(NiPoint3& kCenter);
    inline const NiPoint3& GetCenter() const;

    inline void SetNormal(NiPoint3& kNormal);
    inline const NiPoint3& GetNormal() const;

    // run-time type information
    virtual int Type() const;

    
    // *** begin Emergent internal use only ***

    void SetPlane(const NiPlane& kPlane);
    inline const NiPlane& GetPlane() const;

    virtual void UpdateWorldData(const NiBoundingVolume& kModelABV,
        const NiTransform &kWorld);

    virtual NiBoundingVolume* Create() const;
    virtual void Copy(const NiBoundingVolume& kABV);

    // cloning
    virtual NiBoundingVolume* Clone() const;

    // comparison
    virtual bool operator==(const NiBoundingVolume& kABV) const;
    virtual bool operator!=(const NiBoundingVolume& kABV) const;

    // 'test' functions
    static bool HalfSpaceSphereTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);
    
    static bool HalfSpaceBoxTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);
    
    static bool HalfSpaceCapsuleTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);
    
    static bool HalfSpaceHalfSpaceTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);

    static bool HalfSpaceTriTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiPoint3& kVert0, const NiPoint3& kVert1, 
        const NiPoint3& kVert2, const NiPoint3& kV1);

    // 'find' functions
    static bool HalfSpaceSphereFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
    
    static bool HalfSpaceBoxFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
    
    static bool HalfSpaceCapsuleFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
       
    static bool HalfSpaceHalfSpaceFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);

    static bool HalfSpaceTriFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiPoint3& kVert0, const NiPoint3& kVert1, 
        const NiPoint3& kVert2, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
    
    // For printing of ABV types
    virtual void AddViewerStrings(const char* pcPrefix, 
        NiViewerStringsArray* pkStrings) const;

    // *** end Emergent internal use only ***

protected:
    NiPlane m_kPlane;
    NiPoint3 m_kCenter; // storage if using Center * Normal constructor
};

#include "NiHalfSpaceBV.inl"

#endif

