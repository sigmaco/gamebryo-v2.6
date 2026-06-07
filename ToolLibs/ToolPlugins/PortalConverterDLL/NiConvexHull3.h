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

#ifndef NICONVEXHULL3_H
#define NICONVEXHULL3_H

#include <NiTArray.h>
#include <NiTMap.h>
#include "NiHullTriangle3.h"
#include <NiMemObject.h>

class NiConvexHull3 : public NiMemObject
{
public:
    // Construction and destruction.  The caller is responsible for deleting
    // the akVertex array.
    NiConvexHull3(int iVQuantity, const NiPoint3* akVertex);
    ~NiConvexHull3();

    // Parameter for the incremental method.
    float& DimThreshold();  // default = 0.001

    // Compute the hull using an incremental method. The Boolean return
    // value is 'true' if the construction was successful.  If the value is
    // 'false', then a release-build run failed and a bug report should be
    // filed.
    bool DoIncremental();

    // The dimension of the hull is in {0,1,2}.  The index array has
    // GetQuantity() elements. The possibilities are
    //     dim = 0, quantity = 1, indices = {0}
    //     dim = 1, quantity = 2, indices = {i[0],i[1]}
    //     dim = 2, quantity >= 3, indices = {i[0],...,i[quantity-1]}
    //     dim = 3, quantity >= 3, The number of triangles is quantity/3.
    //              The array contains triples of vertex indices, one triple
    //              per triangle.
    int GetDimension() const;
    int GetQuantity() const;
    int* GetIndices(bool bTakeOwnership = false);

    // If the hull dimension is 1, the hull is linear and lives on the line
    // with these parameters.
    const NiPoint3& GetLineOrigin() const;
    const NiPoint3& GetLineDirection() const;

    // If the hull dimension is 2, the hull is planar and lives on the plane
    // with these parameters.
    const NiPoint3& GetPlaneOrigin() const;
    const NiPoint3& GetPlaneNormal() const;

private:
    bool Update(NiHullTriangle3*& pkHull, int i);
    void ExtractIndices();
    void DeleteHull();
    void FillAndSortHullArray(NiTPrimitiveArray<NiHullTriangle3*>&
        kHullArray);

    // input points
    int m_iVQuantity;
    const NiPoint3* m_akVertex;

    // axis-aligned bounding box of input points
    NiPoint3 m_kMin, m_kMax;
    int m_aiIMin[3], m_aiIMax[3];

    // scaled points to support 64-bit integer calculations
    NiPoint3* m_akSVertex;

    // parameter for the incremental method
    float m_fDimThreshold;

    // for a linear hull only
    NiPoint3 m_kLineOrigin, m_kLineDirection;

    // for a planar hull only
    NiPoint3 m_kPlaneOrigin, m_kPlaneNormal;

    // The hull.  NiTMap is used for fast insert/delete since there is no
    // fast NiTSet class.
    NiTMap<NiHullTriangle3*,int> m_kHull;
    int m_iHDim, m_iHQuantity;
    int* m_aiHIndex;

    unsigned int m_uiOrder;
};

#endif
