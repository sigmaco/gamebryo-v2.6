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

#ifndef NICAPSULEBV_H
#define NICAPSULEBV_H

#include "NiBoundingVolume.h"
#include "NiCapsule.h"
#include "NiPoint3.h"

class NICOLLISION_ENTRY NiCapsuleBV : public NiBoundingVolume
{
    NiDeclareBVStream;
public:
    NiCapsuleBV(float fRadius = 1.0f, float fExtent = 1.0f, 
        NiPoint3 kCenter = NiPoint3::ZERO, NiPoint3 kAxis = NiPoint3::UNIT_X);
    NiCapsuleBV(const NiCapsuleBV& kABV);
    
    inline void SetCenter(const NiPoint3& kCenter);
    inline const NiPoint3& GetCenter() const;
    
    inline void SetAxis(const NiPoint3& kAxis);
    inline const NiPoint3& GetAxis() const;

    inline void SetExtent(const float fExtent);
    inline float GetExtent() const;
    
    inline void SetRadius(const float fRadius);
    inline float GetRadius() const;

    // run-time type information
    virtual int Type() const;


    // *** begin Emergent internal use only ***

    // capsule access functions
    inline const NiCapsule& GetCapsule() const;
    inline const NiSegment& GetSegment() const;
    inline const NiPoint3& GetOrigin() const;
    inline const NiPoint3& GetDirection() const;
    
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
    static bool CapsuleSphereTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);
    
    static bool CapsuleCapsuleTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);
           
    static bool CapsuleTriTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiPoint3& kVert0, const NiPoint3& kVert1, 
        const NiPoint3& kVert2, const NiPoint3& kV1);

    // 'find' functions
    static bool CapsuleSphereFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
    
    static bool CapsuleCapsuleFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0, 
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);
    
    static bool CapsuleTriFindIntersect(float fTime,
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
    inline void CalculateCapsule();

    NiPoint3 m_kCenter;
    NiPoint3 m_kAxis;
    float m_fExtent;

    NiCapsule m_kCapsule;
};

#include "NiCapsuleBV.inl"

#endif

