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

// Precompiled Header
#include "NiCollisionPCH.h"

#include "NiCollision.h"
NiImplementSDMConstructor(NiCollision, "NiMesh NiFloodgate NiMain");

#ifdef NICOLLISION_EXPORT
NiImplementDllMain(NiCollision);
#endif

//---------------------------------------------------------------------------
void NiCollisionSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiCollisionData);

    NiRegisterBVStream(NiBoxBV, BOX_BV);
    NiRegisterBVStream(NiCapsuleBV, CAPSULE_BV);
    NiRegisterBVStream(NiHalfSpaceBV, HALFSPACE_BV);

    // Streaming code in support of converting antiquated legacy data
    // (lozenges).
    NiRegisterBVLoader(NiSphereBV::ConvertLozengeToSphereBoundingVolume,
        LOZENGE_BV);
    
    NiRegisterBVStream(NiSphereBV, SPHERE_BV);
    NiRegisterBVStream(NiUnionBV, UNION_BV);

    // Set function pointers in NiBoundingVolume so that the collision
    // system can use alternate bounding volumes.

    // Ensure that every possible combination of bounding volumes is
    // handled by at least one function of each type

    unsigned int uiSize = NiBoundingVolume::MAXTYPE_BV;
    unsigned int uiSizeSqr = uiSize*uiSize;

    NiBVTestIntersectFunction* pTIF =
        NiAlloc(NiBVTestIntersectFunction, uiSizeSqr);

    pTIF[ 0] = NiSphereBV::SphereSphereTestIntersect;
    pTIF[ 1] = 0; // Sphere-Box
    pTIF[ 2] = 0; // Sphere-Capsule
    pTIF[ 3] = 0; // Sphere-UNUSED
    pTIF[ 4] = 0; // Sphere-Union
    pTIF[ 5] = 0; // Sphere-HalfSpace

    pTIF[ 6] = NiBoxBV::BoxSphereTestIntersect;
    pTIF[ 7] = NiBoxBV::BoxBoxTestIntersect;
    pTIF[ 8] = NiBoxBV::BoxCapsuleTestIntersect;
    pTIF[ 9] = 0; // Box-UNUSED
    pTIF[10] = 0; // Box-Union
    pTIF[11] = 0; // Box-HalfSpace

    pTIF[12] = NiCapsuleBV::CapsuleSphereTestIntersect;
    pTIF[13] = 0; // Capsule-Box
    pTIF[14] = NiCapsuleBV::CapsuleCapsuleTestIntersect;
    pTIF[15] = 0; // Capsule-UNUSED
    pTIF[16] = 0; // Capsule-Union
    pTIF[17] = 0; // Capsule-HalfSpace

    pTIF[18] = 0; // UNUSED-Sphere
    pTIF[19] = 0; // UNUSED-Box
    pTIF[20] = 0; // UNUSED-Capsule
    pTIF[21] = 0; // UNUSED-UNUSED
    pTIF[22] = 0; // UNUSED-Union
    pTIF[23] = 0; // UNUSED-HalfSpace

    pTIF[24] = NiUnionBV::UnionOtherTestIntersect; // Union-Sphere
    pTIF[25] = NiUnionBV::UnionOtherTestIntersect; // Union-Box
    pTIF[26] = NiUnionBV::UnionOtherTestIntersect; // Union-Capsule
    pTIF[27] = 0;                                  // Union-UNUSED
    pTIF[28] = NiUnionBV::UnionUnionTestIntersect; // Union-Union
    pTIF[29] = NiUnionBV::UnionOtherTestIntersect; // Union-HalfSpace

    pTIF[30] = NiHalfSpaceBV::HalfSpaceSphereTestIntersect;
    pTIF[31] = NiHalfSpaceBV::HalfSpaceBoxTestIntersect;
    pTIF[32] = NiHalfSpaceBV::HalfSpaceCapsuleTestIntersect;
    pTIF[33] = 0; // HalfSpace-UNUSED
    pTIF[34] = 0; // HalfSpace-Union
    pTIF[35] = NiHalfSpaceBV::HalfSpaceHalfSpaceTestIntersect;

    NiBVFindIntersectFunction* pFIF =
        NiAlloc(NiBVFindIntersectFunction, uiSizeSqr);

    pFIF[ 0] = NiSphereBV::SphereSphereFindIntersect;
    pFIF[ 1] = 0; // Sphere-Box
    pFIF[ 2] = 0; // Sphere-Capsule
    pFIF[ 3] = 0; // Sphere-UNUSED
    pFIF[ 4] = 0; // Sphere-Union
    pFIF[ 5] = 0; // Sphere-HalfSpace

    pFIF[ 6] = NiBoxBV::BoxSphereFindIntersect;
    pFIF[ 7] = NiBoxBV::BoxBoxFindIntersect;
    pFIF[ 8] = NiBoxBV::BoxCapsuleFindIntersect;
    pFIF[ 9] = 0; // Box-UNUSED
    pFIF[10] = 0; // Box-Union
    pFIF[11] = 0; // Box-HalfSpace

    pFIF[12] = NiCapsuleBV::CapsuleSphereFindIntersect;
    pFIF[13] = 0; // Capsule-Box
    pFIF[14] = NiCapsuleBV::CapsuleCapsuleFindIntersect;
    pFIF[15] = 0; // Capsule-UNUSED
    pFIF[16] = 0; // Capsule-Union
    pFIF[17] = 0; // Capsule-HalfSpace

    pFIF[18] = 0; // UNUSED-Sphere
    pFIF[19] = 0; // UNUSED-Box
    pFIF[20] = 0; // UNUSED-Capsule
    pFIF[21] = 0; // UNUSED-UNUSED
    pFIF[22] = 0; // UNUSED-Union
    pFIF[23] = 0; // UNUSED-HalfSpace

    pFIF[24] = NiUnionBV::UnionOtherFindIntersect;
    pFIF[25] = NiUnionBV::UnionOtherFindIntersect;
    pFIF[26] = NiUnionBV::UnionOtherFindIntersect;
    pFIF[27] = NiUnionBV::UnionOtherFindIntersect;
    pFIF[28] = NiUnionBV::UnionUnionFindIntersect;
    pFIF[29] = NiUnionBV::UnionOtherFindIntersect;

    pFIF[30] = NiHalfSpaceBV::HalfSpaceSphereFindIntersect;
    pFIF[31] = NiHalfSpaceBV::HalfSpaceBoxFindIntersect;
    pFIF[32] = NiHalfSpaceBV::HalfSpaceCapsuleFindIntersect;
    pFIF[33] = 0; // HalfSpace-UNUSED
    pFIF[34] = 0; // HalfSpace-Union
    pFIF[35] = NiHalfSpaceBV::HalfSpaceHalfSpaceFindIntersect;

    NiBVTriTestIntersectFunction* pTTF =
        NiAlloc(NiBVTriTestIntersectFunction, uiSize);

    pTTF[0] = NiSphereBV::SphereTriTestIntersect;
    pTTF[1] = NiBoxBV::BoxTriTestIntersect;
    pTTF[2] = NiCapsuleBV::CapsuleTriTestIntersect;
    pTTF[3] = 0; // UNUSED-TRI
    pTTF[4] = NiUnionBV::UnionTriTestIntersect;
    pTTF[5] = NiHalfSpaceBV::HalfSpaceTriTestIntersect;

    NiBVTriFindIntersectFunction* pFTF =
        NiAlloc(NiBVTriFindIntersectFunction, uiSize);

    pFTF[0] = NiSphereBV::SphereTriFindIntersect;
    pFTF[1] = NiBoxBV::BoxTriFindIntersect;
    pFTF[2] = NiCapsuleBV::CapsuleTriFindIntersect;
    pFTF[3] = 0; // UNUSED-TRI
    pFTF[4] = NiUnionBV::UnionTriFindIntersect;
    pFTF[5] = NiHalfSpaceBV::HalfSpaceTriFindIntersect;

    NiBoundingVolume::ms_pfnTestFuncTable = pTIF;
    NiBoundingVolume::ms_pfnFindFuncTable = pFIF;
    NiBoundingVolume::ms_pfnTriTestFuncTable = pTTF;
    NiBoundingVolume::ms_pfnTriFindFuncTable = pFTF;
    NiBoundingVolume::ms_pfnConvertToSphereBV_Fast = 
        NiCollisionConvert::ConvertToSphereBV_Fast;
    NiBoundingVolume::ms_pfnConvertToSphereBV_Safe = 
        NiCollisionConvert::ConvertToSphereBV_Safe;
    NiBoundingVolume::ms_pfnConvertToBoxBV = 
        NiCollisionConvert::ConvertToBoxBV;
    NiBoundingVolume::ms_pfnConvertToBoxBV_FromMesh =
        NiCollisionConvert::ConvertToBoxBV;

    NiPick::_SDMInit();
}
//---------------------------------------------------------------------------
void NiCollisionSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiCollisionData);

    NiUnregisterBVStream(BOX_BV);
    NiUnregisterBVStream(CAPSULE_BV);
    NiUnregisterBVStream(HALFSPACE_BV);
    NiUnregisterBVStream(LOZENGE_BV);
    NiUnregisterBVStream(SPHERE_BV);
    NiUnregisterBVStream(UNION_BV);

    NiFree(NiBoundingVolume::ms_pfnTestFuncTable);
    NiFree(NiBoundingVolume::ms_pfnFindFuncTable);
    NiFree(NiBoundingVolume::ms_pfnTriTestFuncTable);
    NiFree(NiBoundingVolume::ms_pfnTriFindFuncTable);

    NiBoundingVolume::ms_pfnTestFuncTable = 0;
    NiBoundingVolume::ms_pfnFindFuncTable = 0;
    NiBoundingVolume::ms_pfnTriTestFuncTable = 0;
    NiBoundingVolume::ms_pfnTriFindFuncTable = 0;
    NiBoundingVolume::ms_pfnConvertToSphereBV_Fast = 0;
    NiBoundingVolume::ms_pfnConvertToSphereBV_Safe = 0;
    NiBoundingVolume::ms_pfnConvertToBoxBV = 0;
    NiBoundingVolume::ms_pfnConvertToBoxBV_FromMesh = 0;

    NiPick::_SDMShutdown();
}
//---------------------------------------------------------------------------
