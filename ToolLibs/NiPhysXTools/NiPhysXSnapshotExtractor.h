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

#ifndef NIPHYSXSNAPSHOTEXTRACTOR_H
#define NIPHYSXSNAPSHOTEXTRACTOR_H

#include "NiPhysXToolsLibType.h"

#include <NiSystem.h>
#include <NiMain.h>
#include <NiPhysX.h>
#include <NiPhysXFluid.h>
#include <NiPhysXParticle.h>

#include <NxPhysics.h>
#include <NxCooking.h>


class NIPHYSXTOOLS_ENTRY NiPhysXSnapshotExtractor : public NiMemObject
{
public:
    NiPhysXSnapshotExtractor(const NxPlatform kPlatform = PLATFORM_PC);

    NxPlatform GetTargetPlatform() const;
    void SetTargetPlatform(const NxPlatform kPlatform);

    NiPhysXScene* ExtractSnapshot(NxScene* pkScene, const NxMat34& kXform,
        bool bSetPhysXPtrs);
    
protected:
    void CreateProps(NxScene* pkScene, NiPhysXScene* pkResult,
        NiTMap<NxCompartment*, unsigned int>& kCompartmentIDMap,
        NiTMap<unsigned int, NiPhysXProp*>& kCompartmentPropMap);
    NiPhysXActorDesc* CaptureActor(NxActor* pkActor, const NxMat34& kXform,
        bool bSetPhysXPtrs, NxCookingInterface* pkCookInterface,
        NiTMap<NxShape*, NiPhysXShapeDescPtr>& kShapeMap);
    NiPhysXClothDesc* CaptureCloth(NxCloth* pkCloth, const NxMat34& kXform,
        bool bSetPhysXPtrs, NxCookingInterface* pkCookInterface);
    NiPhysXShapeDesc* CaptureConvex(const NxConvexShapeDesc& kConvexDesc,
        NxCookingInterface* pkCookInterface);
    NiPhysXShapeDesc* CaptureTriMesh(
        const NxTriangleMeshShapeDesc& kTriMeshDesc,
        NxCookingInterface* pkCookInterface);
    NiPhysXMeshDesc* CaptureClothMesh(const NxClothMesh* pkClothMesh,
        NiFixedString& kName, NxCookingInterface* pkCookInterface,
        const NxMat34& kXform);
                
    // Prime numbers
    static const int NUM_PRIMES;
    static const unsigned int PRIMES[10];
    static unsigned int NextPrime(const unsigned int target);
    
    NxPlatform kTargetPlatform;
};

#endif  // #ifndef NIPHYSXSNAPSHOTEXTRACTOR_H

