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

#ifndef NIOBBOX_H
#define NIOBBOX_H

#include "NiCollisionLibType.h"
#include <NiPoint3.h>

class NiMatrix3;
class NiOBBRoot;
class NiTransform;

class NICOLLISION_ENTRY NiOBBox : public NiMemObject
{
public:
    // construction
    NiOBBox (NiOBBRoot* pRoot = 0);

    // attributes
    inline NiPoint3& GetCenter ();
    inline NiPoint3* GetBasis ();
    inline float* GetExtent ();
    inline const NiPoint3& GetWorldCenter () const;
    inline const NiPoint3* GetWorldBasis () const;
    inline const float* GetWorldExtent () const;
    inline void SetRoot (NiOBBRoot* pRoot);
    inline const NiOBBRoot* GetRoot () const;

    // update bounding box
    void Transform (const NiTransform& xform);

    // intersections
    int TestIntersection (const NiOBBox& box);
    int FindIntersection (const NiOBBox& box, float& t, NiPoint3& p);
    void ComputeNormal (const NiPoint3& p, NiPoint3& normal) const;

    int TestIntersection (const NiPoint3& origin, const NiPoint3& dir);
    int FindIntersection (const NiPoint3& origin, const NiPoint3& dir,
        float& t, NiPoint3& normal);

protected:
    NiPoint3 m_center;
    NiPoint3 m_basis[3];
    float m_extent[3];

    NiPoint3 m_wCenter;
    NiPoint3 m_wBasis[3];
    float m_wExtent[3];

    NiOBBRoot* m_pRoot;  // stores velocity V and time interval dt
};

#include "NiOBBox.inl"

#endif

