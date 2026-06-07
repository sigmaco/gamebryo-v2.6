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

#ifndef NI_COLLISION_H
#define NI_COLLISION_H

#include "NiCollisionLibType.h"

#include "NiBox.h"
#include "NiBoxBV.h"
#include "NiBoxBVMacros.h"
#include "NiBoxSphereIntersector.h"
#include "NiCapsule.h"
#include "NiCapsuleBV.h"
#include "NiCapsuleCapsuleIntersector.h"
#include "NiCapsuleSphereIntersector.h"
#include "NiCapsuleTriIntersector.h"
#include "NiCollisionData.h"
#include "NiCollisionGroup.h"
#include "NiCollisionMetrics.h"
#include "NiCollisionTraversals.h"
#include "NiCollisionSDM.h"
#include "NiCollisionUtils.h"
#include "NiDrawableBV.h"
#include "NiHalfSpaceBV.h"
#include "NiIntersector.h"
#include "NiOBBLeaf.h"
#include "NiOBBNode.h"
#include "NiOBBox.h"
#include "NiOBBRoot.h"
#include "NiParallelogram.h"
#include "NiPick.h"
#include "NiSegment.h"
#include "NiSphere.h"
#include "NiSphereBV.h"
#include "NiSphereTriIntersector.h"
#include "NiSqrDistance.h"
#include "NiTrigon.h"
#include "NiTriIntersect.h"
#include "NiUnionBV.h"

static NiCollisionSDM NiCollisionSDMObject;

class NICOLLISION_ENTRY NiCollisionConvert
{
public:
    static NiBoundingVolume* ConvertToSphereBV_Fast(float fRadius,
        const NiPoint3& kCenter);

    static NiBoundingVolume* ConvertToSphereBV_Safe(float fRadius, 
        const NiPoint3& kCenter);

    static NiBoundingVolume* ConvertToBoxBV(unsigned short usQuantity,
        const NiPoint3* pkVertex);

    static NiBoundingVolume* ConvertToBoxBV(NiMesh* pkMesh);

protected:
    // A static instance of sphereBV used during NiMain's collision testing
    // when converting an NiBound to an NiSphereBV.  Eliminates overhead of
    // memory allocation.
    static NiSphereBV ms_kSphereABV;
};

#endif

