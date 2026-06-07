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

#ifndef NIUNIONBV_H
#define NIUNIONBV_H

#include "NiCollisionLibType.h"
#include "NiBoundingVolume.h"
#include "NiTArray.h"

class NICOLLISION_ENTRY NiUnionBV : public NiBoundingVolume
{
    NiDeclareBVStream;
public:
    NiUnionBV();
    NiUnionBV(const NiUnionBV& kABV);
    virtual ~NiUnionBV();

    // NiUnionBV object assumes ownership of the input bounding volume.  The
    // application must therefore pass in dynamically allocated bounding
    // volumes and these should not be deleted by the application.
    void AddBoundingVolume(NiBoundingVolume* pkABV);
    void RemoveBoundingVolume(NiBoundingVolume* pkABV);
    void RemoveAllBoundingVolumes();

    // for iteration and access of bounding volumes and for cloning
    inline unsigned int GetSize() const;
    inline const NiBoundingVolume* GetBoundingVolume(unsigned int i) const;

    // Index of primitive bounding volume that generated a collision in the
    // last call to NiCollisionGroup::FindCollisions(...).
    virtual unsigned int WhichObjectIntersect() const;

    // run-time type information
    virtual int Type() const;

    
    // *** begin Emergent internal use only ***

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
    static bool UnionOtherTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0,
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);

    static bool UnionUnionTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0,
        const NiBoundingVolume& kABV1, const NiPoint3& kV1);

    static bool UnionTriTestIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0,
        const NiPoint3& kVert0, const NiPoint3& kVert1,
        const NiPoint3& kVert2, const NiPoint3& kV1);

    // 'find' functions
    static bool UnionOtherFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0,
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);

    static bool UnionUnionFindIntersect(float fTime,
        const NiBoundingVolume& kABV0, const NiPoint3& kV0,
        const NiBoundingVolume& kABV1, const NiPoint3& kV1,
        float& fIntrTime, NiPoint3& kIntrPt, bool bCalcNormals,
        NiPoint3& kNormal0, NiPoint3& kNormal1);

    static bool UnionTriFindIntersect(float fTime,
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
    NiTPrimitiveArray<NiBoundingVolume*> m_kBoundingVolume;
    unsigned int m_uiWhichObjectIntersect;
};

#include "NiUnionBV.inl"

#endif
