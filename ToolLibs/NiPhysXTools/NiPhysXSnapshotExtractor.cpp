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


#include "NiPhysXSnapshotExtractor.h"

const int NiPhysXSnapshotExtractor::NUM_PRIMES = 10;
const unsigned int NiPhysXSnapshotExtractor::PRIMES[10] =
    { 17, 37, 67, 127, 257, 521, 1031, 2053, 4127, 8237 };

//---------------------------------------------------------------------------
NiPhysXSnapshotExtractor::NiPhysXSnapshotExtractor(const NxPlatform kPlatform)
{
    kTargetPlatform = kPlatform;
}
//---------------------------------------------------------------------------
NxPlatform NiPhysXSnapshotExtractor::GetTargetPlatform() const
{
    return kTargetPlatform;
}
//---------------------------------------------------------------------------
void NiPhysXSnapshotExtractor::SetTargetPlatform(const NxPlatform kPlatform)
{
    kTargetPlatform = kPlatform;
}
//---------------------------------------------------------------------------
NiPhysXScene* NiPhysXSnapshotExtractor::ExtractSnapshot(
    NxScene* pkScene, const NxMat34& kXform, bool bSetPhysXPtrs)
{
    NiPhysXScene* pkResult = NiNew NiPhysXScene();
    
    NiPhysXSceneDesc* pkSceneSnapshot = NiNew NiPhysXSceneDesc();
    pkResult->SetSnapshot(pkSceneSnapshot);
    
    // First extract all the info in the scene descriptor.
    NxSceneDesc kSceneDesc;
    pkScene->saveToDesc(kSceneDesc);
    pkSceneSnapshot->SetNxSceneDesc(kSceneDesc);
    
    // Now extract the other global scene information
    for (unsigned int i = 0; i < 32; i++)
    {
        for (unsigned int j = 0; j < 32; j++)
        {
            pkSceneSnapshot->SetGroupCollisionFlag(
                (unsigned char)i, (unsigned char)j,
                pkScene->getGroupCollisionFlag(
                (NxCollisionGroup)i, (NxCollisionGroup)j));
        }
    }
    NxFilterOp akOps[3];
    pkScene->getFilterOps(akOps[0], akOps[1], akOps[2]);
    pkSceneSnapshot->SetFilterOps(akOps[0], akOps[1], akOps[2]); 
    pkSceneSnapshot->SetFilterBool(pkScene->getFilterBool());
    NxGroupsMask kMask[2];
    kMask[0] = pkScene->getFilterConstant0();
    kMask[1] = pkScene->getFilterConstant1();
    pkSceneSnapshot->SetFilterConstant0(kMask[0]);
    pkSceneSnapshot->SetFilterConstant1(kMask[1]);

    NiTMap<NxCompartment*, unsigned int> kCompartmentIDMap;
    NiTMap<unsigned int, NiPhysXProp*> kCompartmentPropMap;
    CreateProps(pkScene, pkResult, kCompartmentIDMap, kCompartmentPropMap);

    NiTMap<NxActor*, NiPhysXActorDescPtr> kActorMap;
    NiTMap<NxShape*, NiPhysXShapeDescPtr> kShapeMap;

    // Get material arrays and add all materials to all props.
    unsigned int uiNumMaterials = pkScene->getNbMaterials();
    NxMaterial** pkMaterialArray = NiAlloc(NxMaterial*, uiNumMaterials);
    unsigned int uiMaterialIter = 0;
    pkScene->getMaterialArray(pkMaterialArray, uiNumMaterials, uiMaterialIter);
    for (unsigned int i = 0; i < uiNumMaterials; i++)
    {
        NxMaterialDesc kMatDesc;
        pkMaterialArray[i]->saveToDesc(kMatDesc);
        for (unsigned int ui = 0; ui < pkResult->GetPropCount(); ui++)
        {
            pkResult->GetPropAt(ui)->GetSnapshot()->SetMaterialAt(
                kMatDesc, pkMaterialArray[i]->getMaterialIndex());
        }
    }
    NiFree(pkMaterialArray);

    // Set up cooking ready for actor extraction
    NxCookingInterface* pkCookingInterface =
        NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
    NIASSERT(pkCookingInterface != 0);

    pkCookingInterface->NxInitCooking(NxGetPhysicsSDKAllocator(),
        NiPhysXManager::GetPhysXManager()->m_pkPhysXSDK->getFoundationSDK().
        getErrorStream());

    NxCookingParams kCookingParams;
    kCookingParams.targetPlatform = kTargetPlatform;
    pkCookingInterface->NxSetCookingParams(kCookingParams);

    // Extract the actors
    unsigned int uiNumActors = pkScene->getNbActors();
    NxActor** pkActorArray = pkScene->getActors();
    unsigned int uiNum = 0;
    for (unsigned int i = 0; i < uiNumActors; i++)
    {
        NiPhysXActorDescPtr spActorDesc = CaptureActor(pkActorArray[i],
            kXform, bSetPhysXPtrs, pkCookingInterface, kShapeMap);
        if (spActorDesc)
        {
            unsigned int uiID;
            kCompartmentIDMap.GetAt(pkActorArray[i]->getCompartment(), uiID);
            NiPhysXProp* pkProp;
            kCompartmentPropMap.GetAt(uiID, pkProp);
            pkProp->GetSnapshot()->AddActor(spActorDesc);
            kActorMap.SetAt(pkActorArray[i], spActorDesc);
    
        }
    }
    
    // Extract any cloth
    unsigned int uiNumCloth = pkScene->getNbCloths();
    NxCloth** pkClothArray = pkScene->getCloths();
    uiNum = 0;
    for (unsigned int i = 0; i < uiNumCloth; i++)
    {
        NiPhysXClothDescPtr spClothDesc = CaptureCloth(pkClothArray[i],
            kXform, bSetPhysXPtrs, pkCookingInterface);
        if (spClothDesc)
        {
            unsigned int uiID;
            kCompartmentIDMap.GetAt(pkClothArray[i]->getCompartment(), uiID);
            NiPhysXProp* pkProp;
            kCompartmentPropMap.GetAt(uiID, pkProp);
            pkProp->GetSnapshot()->AddCloth(spClothDesc);
        }
    }

    // Shut down cooking
    pkCookingInterface->NxCloseCooking();

    // Extract the joints
    unsigned int uiNumJoints = pkScene->getNbJoints();
    NI_UNUSED_ARG(uiNumJoints);
    pkScene->resetJointIterator();
    NxJoint* pkJoint = pkScene->getNextJoint();
    while (pkJoint)
    {
        NiPhysXJointDescPtr spJointDesc =
            NiPhysXJointDesc::CreateFromJoint(pkJoint, kActorMap, kXform);
        // Figure out which prop we should be in
        NxActor* pkActor1;
        NxActor* pkActor2;
        pkJoint->getActors(&pkActor1, &pkActor2);
        unsigned int uiID = 0;
        if (pkActor1)
        {
            kCompartmentIDMap.GetAt(pkActor1->getCompartment(), uiID);
        }
        else if (pkActor2)
        {
            kCompartmentIDMap.GetAt(pkActor2->getCompartment(), uiID);
        }
        NiPhysXProp* pkProp;
        kCompartmentPropMap.GetAt(uiID, pkProp);
        pkProp->GetSnapshot()->AddJoint(spJointDesc);

        pkJoint = pkScene->getNextJoint();
    }

    unsigned int uiNumFluids = pkScene->getNbFluids();
    if (uiNumFluids)
    {
        // Extract the fluids
        NxFluid** ppkFluids = pkScene->getFluids();
        for (unsigned int ui = 0; ui < uiNumFluids; ui++)
        {
            NiPhysXFluidDescPtr spFluidDesc = NiNew NiPhysXFluidDesc();
            spFluidDesc->SetFluid(ppkFluids[ui], kXform, kShapeMap);
            unsigned int uiID;
            kCompartmentIDMap.GetAt(ppkFluids[ui]->getCompartment(), uiID);
            NiPhysXProp* pkProp;
            kCompartmentPropMap.GetAt(uiID, pkProp);
            ((NiPhysXFluidPropDesc*)pkProp->GetSnapshot())
                ->AddFluidDesc(spFluidDesc);
        }
    }
    
    if (bSetPhysXPtrs)
        pkResult->SetPhysXScene(pkScene);
    
    return pkResult;
}
//---------------------------------------------------------------------------
void NiPhysXSnapshotExtractor::CreateProps(NxScene* pkScene,
    NiPhysXScene* pkResult,
    NiTMap<NxCompartment*, unsigned int>& kCompartmentIDMap,
    NiTMap<unsigned int, NiPhysXProp*>& kCompartmentPropMap)
{
    unsigned int uiNumMaterials = pkScene->getNbMaterials();

    unsigned int uiNumCompartments = pkScene->getNbCompartments();
    NxCompartment** pkCompartments = 0;
    if (uiNumCompartments)
    {
        pkCompartments = NiAlloc(NxCompartment*, uiNumCompartments);
        unsigned int ui = 0;
        pkScene->getCompartmentArray(pkCompartments, uiNumCompartments, ui);
    }
    
    NiTMap<unsigned int, NxCompartmentDesc*>& kSceneCompartmentMap =
        pkResult->GetSnapshot()->GetCompartmentMap();

    unsigned int uiNextID = 1;
    unsigned int uiNumRBCompartments = 0;
    unsigned int uiNumClothCompartments = 0;
    unsigned int uiNumFluidCompartments = 0;
    kCompartmentIDMap.SetAt(0, 0);
    for (unsigned int ui = 0; ui < uiNumCompartments; ui++)
    {
        NxCompartmentDesc* pkDesc = NiAlloc(NxCompartmentDesc, 1);
        pkCompartments[ui]->saveToDesc(*pkDesc);
        kSceneCompartmentMap.SetAt(uiNextID, pkDesc);
        kCompartmentIDMap.SetAt(pkCompartments[ui], uiNextID);
        
        switch (pkDesc->type)
        {
            case NX_SCT_RIGIDBODY:
                uiNumRBCompartments++;
                break;
            case NX_SCT_CLOTH:
                uiNumClothCompartments++;
                break;
            case NX_SCT_FLUID:
                uiNumFluidCompartments++;
                break;
        }
        
        uiNextID++;
    }
    NiFree(pkCompartments);

    unsigned int uiNumProps = uiNumRBCompartments;
    if (uiNumProps < uiNumClothCompartments)
        uiNumProps = uiNumClothCompartments;
    if (uiNumProps < uiNumFluidCompartments)
        uiNumProps = uiNumFluidCompartments;

    NiPhysXProp* pkProp;
    NiPhysXPropDesc* pkPropSnapshot;

    // Create all the props
    NiFixedString kStateName("Default");
    pkProp = NiNew NiPhysXProp();
    if (pkScene->getNbFluids())
        pkPropSnapshot = NiNew NiPhysXFluidPropDesc(NextPrime(uiNumMaterials));
    else
        pkPropSnapshot = NiNew NiPhysXPropDesc(NextPrime(uiNumMaterials));
    pkProp->SetSnapshot(pkPropSnapshot);
    pkPropSnapshot->SetNumStates(1);
    pkPropSnapshot->SetStateName(0, kStateName);
    pkResult->AddProp(pkProp);
    kCompartmentPropMap.SetAt(0, pkProp);
    
    for (unsigned int ui = 0; ui < uiNumFluidCompartments; ui++)
    {
        pkProp = NiNew NiPhysXProp();
        pkPropSnapshot = NiNew NiPhysXFluidPropDesc(NextPrime(uiNumMaterials));
        pkProp->SetSnapshot(pkPropSnapshot);
        pkPropSnapshot->SetNumStates(1);
        pkPropSnapshot->SetStateName(0, kStateName);
        pkResult->AddProp(pkProp);
    }
    for (unsigned int ui = uiNumFluidCompartments; ui < uiNumProps; ui++)
    {
        pkProp = NiNew NiPhysXProp();
        pkPropSnapshot = NiNew NiPhysXPropDesc(NextPrime(uiNumMaterials));
        pkProp->SetSnapshot(pkPropSnapshot);
        pkPropSnapshot->SetNumStates(1);
        pkPropSnapshot->SetStateName(0, kStateName);
        pkResult->AddProp(pkProp);
    }
    
    // Assign compartments to props.
    unsigned int uiNextFreeRBProp = 1;
    unsigned int uiNextFreeClothProp = 1;
    unsigned int uiNextFreeFluidProp = 1;
    NiTMapIterator kIter = kCompartmentIDMap.GetFirstPos();
    while (kIter)
    {
        unsigned int uiID;
        NxCompartment* pkCompartment;
        kCompartmentIDMap.GetNext(kIter, pkCompartment, uiID);
        
        if (!pkCompartment)
        {
            // Assigned this one above.
            continue;
        }

        switch (pkCompartment->getType())
        {
            case NX_SCT_RIGIDBODY:
                pkProp = pkResult->GetPropAt(uiNextFreeRBProp);
                pkProp->GetSnapshot()->SetRBCompartmentID(uiID);
                kCompartmentPropMap.SetAt(uiID, pkProp);
                uiNextFreeRBProp++;
                break;
            case NX_SCT_CLOTH:
                pkProp = pkResult->GetPropAt(uiNextFreeClothProp);
                pkProp->GetSnapshot()->SetClothCompartmentID(uiID);
                kCompartmentPropMap.SetAt(uiID, pkProp);
                uiNextFreeClothProp++;
                break;
            case NX_SCT_FLUID:
                pkProp = pkResult->GetPropAt(uiNextFreeFluidProp);
                pkProp->GetSnapshot()->SetFluidCompartmentID(uiID);
                kCompartmentPropMap.SetAt(uiID, pkProp);
                uiNextFreeFluidProp++;
                break;
        }
    }
}
//---------------------------------------------------------------------------
NiPhysXActorDesc* NiPhysXSnapshotExtractor::CaptureActor(
    NxActor* pkActor, const NxMat34& kXform, bool bSetPhysXPtrs,
    NxCookingInterface* pkCookInterface,
    NiTMap<NxShape*, NiPhysXShapeDescPtr>& kShapeMap)
{
    NiPhysXActorDesc* pkActorDesc = NiNew NiPhysXActorDesc;

    NxActorDesc kActorDesc;
    pkActor->saveToDesc(kActorDesc);
    
    NiFixedString kActorName;
    if (pkActor->getName())
        kActorName = NiFixedString(pkActor->getName());
    else
        kActorName = NiFixedString(NULL);
    
    NxBodyDesc kBodyDesc;
    NiPhysXBodyDesc* pkBodyDesc;
    bool bDynamic = pkActor->saveBodyToDesc(kBodyDesc);
    if (bDynamic)
    {
        pkBodyDesc = NiNew NiPhysXBodyDesc;
        pkBodyDesc->SetFromBodyDesc(kBodyDesc, kXform.M);
    }
    else
    {
        pkBodyDesc = 0;
    }

    NiTObjectArray<NiPhysXShapeDescPtr> kActorShapes;
      
    unsigned int uiNumShapes = pkActor->getNbShapes();
    NxShape*const* pkActorShapes = pkActor->getShapes();
    for (unsigned int i = 0; i < uiNumShapes; i++, pkActorShapes++)
    {
        NxShape* pkShape = *pkActorShapes;
        switch (pkShape->getType())
        {
            case NX_SHAPE_PLANE: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxPlaneShapeDesc kPlaneShapeDesc;
                ((NxPlaneShape*)pkShape)->saveToDesc(kPlaneShapeDesc);
                pkShapeDesc->FromPlaneDesc(kPlaneShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                kShapeMap.SetAt(pkShape, pkShapeDesc);
                } break;

            case NX_SHAPE_SPHERE: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxSphereShapeDesc kSphereShapeDesc;
                ((NxSphereShape*)pkShape)->saveToDesc(kSphereShapeDesc);
                pkShapeDesc->FromSphereDesc(kSphereShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                kShapeMap.SetAt(pkShape, pkShapeDesc);
                } break;

            case NX_SHAPE_CAPSULE: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxCapsuleShapeDesc kCapsuleShapeDesc;
                ((NxCapsuleShape*)pkShape)->saveToDesc(kCapsuleShapeDesc);
                pkShapeDesc->FromCapsuleDesc(kCapsuleShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                kShapeMap.SetAt(pkShape, pkShapeDesc);
                } break;

            case NX_SHAPE_BOX: {
                NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
                NxBoxShapeDesc kBoxShapeDesc;
                ((NxBoxShape*)pkShape)->saveToDesc(kBoxShapeDesc);
                pkShapeDesc->FromBoxDesc(kBoxShapeDesc);
                kActorShapes.AddFirstEmpty(pkShapeDesc);
                kShapeMap.SetAt(pkShape, pkShapeDesc);
                } break;
            
            case NX_SHAPE_CONVEX: {
                NxConvexShapeDesc kConvexShapeDesc;
                ((NxConvexShape*)pkShape)->saveToDesc(kConvexShapeDesc);
                
                NiPhysXShapeDesc* pkShapeDesc =
                    CaptureConvex(kConvexShapeDesc, pkCookInterface);
                if (pkShapeDesc)
                    kActorShapes.AddFirstEmpty(pkShapeDesc);
                kShapeMap.SetAt(pkShape, pkShapeDesc);
                } break;
                
            case NX_SHAPE_MESH: {
                NxTriangleMeshShapeDesc kTriangleMeshShapeDesc;
                ((NxTriangleMeshShape*)pkShape)->saveToDesc(
                    kTriangleMeshShapeDesc);
                    
                NiPhysXShapeDesc* pkShapeDesc =
                    CaptureTriMesh(kTriangleMeshShapeDesc, pkCookInterface);
                if (pkShapeDesc)
                    kActorShapes.AddFirstEmpty(pkShapeDesc);
                kShapeMap.SetAt(pkShape, pkShapeDesc);
                } break;
                
            default:;
        }
    }

    pkActorDesc->SetConstants(kActorName, kActorDesc.density,
        kActorDesc.flags, kActorDesc.group, kActorDesc.dominanceGroup,
        kActorDesc.contactReportFlags, kActorDesc.forceFieldMaterial,
        pkBodyDesc, kActorShapes);
         
    // Store one set of state.
    NxMat34 kPose = kXform * pkActor->getGlobalPose();
    pkActorDesc->SetPose(kPose, 0);

    if (bSetPhysXPtrs)
        pkActorDesc->SetActor(pkActor);

    return pkActorDesc;
}
//---------------------------------------------------------------------------
NiPhysXClothDesc* NiPhysXSnapshotExtractor::CaptureCloth(
    NxCloth* pkCloth, const NxMat34& kXform, bool bSetPhysXPtrs,
    NxCookingInterface* pkCookInterface)
{
    NiPhysXClothDesc* pkClothDesc = NiNew NiPhysXClothDesc;

    NxClothDesc kClothDesc;
    pkCloth->saveToDesc(kClothDesc);
    
    NiFixedString kClothName;
    if (pkCloth->getName())
        kClothName = NiFixedString(pkCloth->getName());
    else
        kClothName = NiFixedString(NULL);
   
    NiPhysXMeshDesc* pkMeshDesc = CaptureClothMesh(pkCloth->getClothMesh(),
        kClothName, pkCookInterface, kXform);

    pkClothDesc->SetConstantParameters(kClothName, pkMeshDesc,
        kClothDesc.thickness, kClothDesc.density, kClothDesc.bendingStiffness,
        kClothDesc.stretchingStiffness, kClothDesc.dampingCoefficient,
        kClothDesc.friction, kClothDesc.pressure, kClothDesc.tearFactor,
        kClothDesc.collisionResponseCoefficient,
        kClothDesc.attachmentResponseCoefficient,
        kClothDesc.attachmentTearFactor, kClothDesc.toFluidResponseCoefficient,
        kClothDesc.fromFluidResponseCoefficient, kClothDesc.minAdhereVelocity,
        kClothDesc.relativeGridSpacing, kClothDesc.solverIterations,
        kXform.M * kClothDesc.externalAcceleration,
        kXform.M * kClothDesc.windAcceleration, kClothDesc.wakeUpCounter,
        kClothDesc.sleepLinearVelocity, kClothDesc.collisionGroup,
        kClothDesc.groupsMask, kClothDesc.forceFieldMaterial,
        kClothDesc.flags);

    // Save one set of state, with ID matrix for pose.
    pkClothDesc->PresizeStateArrays(1);

    if (bSetPhysXPtrs)
        pkClothDesc->SetCloth(pkCloth);

    return pkClothDesc;
}
//---------------------------------------------------------------------------
NiPhysXShapeDesc* NiPhysXSnapshotExtractor::CaptureConvex(
    const NxConvexShapeDesc& kConvexDesc, NxCookingInterface* pkCookInterface)
{
    NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
    
    pkShapeDesc->FromShapeDesc(kConvexDesc);

    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    NiPhysXConvexMeshPtr spMesh =
        pkManager->GetConvexMesh(kConvexDesc.meshData);
    
    NiPhysXMeshDesc* pkMeshDesc;
    bool bStreamInline = true;
    if (spMesh)
    {
        pkMeshDesc = spMesh->GetMeshDesc();
        if (pkMeshDesc)
        {
            pkShapeDesc->SetMeshDesc(pkMeshDesc);
            return pkShapeDesc;
        }
        else
        {
            pkMeshDesc = NiNew NiPhysXMeshDesc;
            pkMeshDesc->SetName(spMesh->GetName());
            bStreamInline = spMesh->GetStreamInline();
        }
    }
    else
    {
        pkMeshDesc = NiNew NiPhysXMeshDesc;  
        pkMeshDesc->SetName(kConvexDesc.name);
    }

    pkShapeDesc->SetMeshDesc(pkMeshDesc);
    pkMeshDesc->SetIsConvex();
    pkMeshDesc->SetFlags(kConvexDesc.meshFlags);
    
    NxConvexMesh* pkMesh = kConvexDesc.meshData;
    if (!pkMesh)
    {
        NiDelete pkShapeDesc;
        return 0;
    }
    
    
    if (!bStreamInline)
    {
        pkMeshDesc->SetData(0, 0);
        return pkShapeDesc;
    }
    
    NxConvexMeshDesc pkConvexMeshDesc;
    pkMesh->saveToDesc(pkConvexMeshDesc);
    NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
    bool bCookRes =
        pkCookInterface->NxCookConvexMesh(pkConvexMeshDesc, *kCookStream);
    if (!bCookRes)
    {
        NiDelete kCookStream;
        NiDelete pkShapeDesc;
        return false;
    }
    pkMeshDesc->SetData(kCookStream->GetSize(),
        (unsigned char*)kCookStream->GetBuffer());
    
    NiDelete kCookStream;
    
    return pkShapeDesc;
}
//---------------------------------------------------------------------------
NiPhysXShapeDesc* NiPhysXSnapshotExtractor::CaptureTriMesh(
    const NxTriangleMeshShapeDesc& kTriMeshDesc,
    NxCookingInterface* pkCookInterface)
{
    NiPhysXShapeDesc* pkShapeDesc = NiNew NiPhysXShapeDesc;
    
    pkShapeDesc->FromShapeDesc(kTriMeshDesc);

    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    NiPhysXTriangleMeshPtr spMesh =
        pkManager->GetTriangleMesh(kTriMeshDesc.meshData);
    
    NiPhysXMeshDesc* pkMeshDesc;
    bool bStreamInline = true;
    if (spMesh)
    {
        pkMeshDesc = spMesh->GetMeshDesc();
        if (pkMeshDesc)
        {
            pkShapeDesc->SetMeshDesc(pkMeshDesc);
            return pkShapeDesc;
        }
        else
        {
            pkMeshDesc = NiNew NiPhysXMeshDesc;
            pkMeshDesc->SetName(spMesh->GetName());
            bStreamInline = spMesh->GetStreamInline();
        }
    }
    else
    {
        pkMeshDesc = NiNew NiPhysXMeshDesc;  
        pkMeshDesc->SetName(kTriMeshDesc.name);
    }
    
    pkShapeDesc->SetMeshDesc(pkMeshDesc);
    pkMeshDesc->SetIsTriMesh();
    pkMeshDesc->SetFlags(kTriMeshDesc.meshFlags);
    
    NxTriangleMesh* pkMesh = kTriMeshDesc.meshData;
    if (!pkMesh)
    {
        NiDelete pkShapeDesc;
        return 0;
    }
    
    if (!bStreamInline)
    {
        pkMeshDesc->SetData(0, 0);
        return pkShapeDesc;
    }
    
    NxTriangleMeshDesc pkTriangleMeshDesc;
    pkMesh->saveToDesc(pkTriangleMeshDesc);
    NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
    bool bCookRes =
        pkCookInterface->NxCookTriangleMesh(pkTriangleMeshDesc, *kCookStream);
    if (!bCookRes)
    {
        NiDelete kCookStream;
        NiDelete pkShapeDesc;
        return false;
    }
    pkMeshDesc->SetData(kCookStream->GetSize(),
        (unsigned char*)kCookStream->GetBuffer());
    
    NiDelete kCookStream;
    
    return pkShapeDesc;

}
//---------------------------------------------------------------------------
NiPhysXMeshDesc* NiPhysXSnapshotExtractor::CaptureClothMesh(
    const NxClothMesh* pkClothMesh, NiFixedString& kName,
    NxCookingInterface* pkCookInterface, const NxMat34& kXform)
{
    NiPhysXManager* pkManager = NiPhysXManager::GetPhysXManager();
    NiPhysXClothMeshPtr spMesh = pkManager->GetClothMesh(pkClothMesh);
    
    NiPhysXMeshDesc* pkMeshDesc;
    bool bStreamInline = true;
    if (spMesh)
    {
        pkMeshDesc = spMesh->GetMeshDesc();
        if (pkMeshDesc)
        {
            return pkMeshDesc;
        }
        else
        {
            pkMeshDesc = NiNew NiPhysXMeshDesc;
            pkMeshDesc->SetName(spMesh->GetName());
            bStreamInline = spMesh->GetStreamInline();
            spMesh->SetMeshDesc(pkMeshDesc);
        }
    }
    else
    {
        pkMeshDesc = NiNew NiPhysXMeshDesc;  
        pkMeshDesc->SetName(kName);
    }
    
    pkMeshDesc->SetIsCloth();
    
    if (!bStreamInline)
    {
        pkMeshDesc->SetData(0, 0);
        return pkMeshDesc;
    }
    
    NxClothMeshDesc pkClothMeshDesc;
    pkClothMesh->saveToDesc(pkClothMeshDesc);
    unsigned int uiCount = pkClothMeshDesc.numVertices;
    char* pkData = (char*)pkClothMeshDesc.points;
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NxVec3* pkVec = (NxVec3*)pkData;
        NxVec3 kNewPosn = kXform * (*pkVec);
        *pkVec = kNewPosn;
        pkData += pkClothMeshDesc.pointStrideBytes;
    }
    NiPhysXMemStream* kCookStream = NiNew NiPhysXMemStream();
    bool bCookRes =
        pkCookInterface->NxCookClothMesh(pkClothMeshDesc, *kCookStream);
    if (!bCookRes)
    {
        NiDelete kCookStream;
        NiDelete pkMeshDesc;
        return false;
    }
    pkMeshDesc->SetData(kCookStream->GetSize(),
        (unsigned char*)kCookStream->GetBuffer());
    
    NiDelete kCookStream;
    
    return pkMeshDesc;

}
//---------------------------------------------------------------------------
unsigned int NiPhysXSnapshotExtractor::NextPrime(const unsigned int target)
{
    int i;
    for (i = 0; i < NUM_PRIMES && PRIMES[i] < target; i++);
    return PRIMES[i];
}
//---------------------------------------------------------------------------
