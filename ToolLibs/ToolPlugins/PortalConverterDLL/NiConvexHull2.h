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

#ifndef NICONVEXHULL2_H
#define NICONVEXHULL2_H

#include "NiHullEdge2.h"
#include <NiMemObject.h>

class NiConvexHull2 : public NiMemObject
{
public:
    // Construction and destruction.  The caller is responsible for deleting
    // the akVertex array.
    NiConvexHull2(int iVQuantity, const NiPoint2* akVertex);
    ~NiConvexHull2();

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
    int GetDimension() const;
    int GetQuantity() const;
    int* GetIndices(bool bTakeOwnership = false);

    // If the hull dimension is 1, the hull is linear and lives on the line
    // with these parameters.
    const NiPoint2& GetLineOrigin() const;
    const NiPoint2& GetLineDirection() const;

private:
    bool Update(NiHullEdge2*& pkHull, int i);

    // input points
    int m_iVQuantity;
    const NiPoint2* m_akVertex;

    // axis-aligned bounding box of input points
    NiPoint2 m_kMin, m_kMax;
    int m_aiIMin[2], m_aiIMax[2];

    // scaled points to support 64-bit integer calculations
    NiPoint2* m_akSVertex;

    // parameter for the incremental method
    float m_fDimThreshold;

    // for a linear hull only
    NiPoint2 m_kLineOrigin, m_kLineDirection;

    // the hull
    int m_iHDim, m_iHQuantity;
    int* m_aiHIndex;
};

#endif
