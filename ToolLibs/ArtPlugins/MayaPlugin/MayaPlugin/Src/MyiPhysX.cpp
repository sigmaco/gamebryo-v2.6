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

// Precompiled Headers
#include "MayaPluginPCH.h"


#define PHYSX_ACTOR_DYNAMIC   1
#define PHYSX_ACTOR_KINEMATIC 2
#define PHYSX_ACTOR_STATIC    3
#define PHYSX_ACTOR_RB_OVER   4
#define PHYSX_ACTOR_UNDEF     5
#define PHYSX_ACTOR_CLOTH     6

#define PHYSX_SHAPE_SPHERE  1
#define PHYSX_SHAPE_BOX     2
#define PHYSX_SHAPE_CAPSULE 3
#define PHYSX_SHAPE_MESH    4
#define PHYSX_SHAPE_CONVEX  5

// Material source enumeration
#define PHYSX_MATERIALTYPE_INVALID 0
#define PHYSX_MATERIALTYPE_3DS     1
#define PHYSX_MATERIALTYPE_USER    2
#define PHYSX_MATERIALTYPE_DEFAULT 3
#define PHYSX_MATERIALTYPE_PRESET  4

#define ED_ACTOR_TYPE         "NiPhysXActorType"
#define ED_DENSITY            "NiPhysXActorDensity"
#define ED_ACTOR_GROUP        "NiPhysXActorGroup"
#define ED_MASS               "NiPhysXBodyMass"
#define ED_LINEAR_VELOCITY    "NiPhysXBodyLinearVelocity"
#define ED_ANGULAR_VELOCITY   "NiPhysXBodyAngularVelocity"
#define ED_DISABLE_GRAVITY    "NiPhysXBodyDisableGravity"
#define ED_LINEAR_DAMPING     "NiPhysXBodyLinearDamping"
#define ED_ANGULAR_DAMPING    "NiPhysXBodyAngularDamping"
#define ED_SOLVER_ITERATIONS  "NiPhysXBodySolverIterations"
#define ED_SHAPE_MESH_PARENT  "NiPhysXShapeMeshParent"
#define ED_SHAPE_MESH_CHILD   "NiPhysXShapeMeshChild"
#define ED_SHAPE_BASIC_PARENT "NiPhysXShapeBasicParent"
#define ED_SHAPE_BASIC_CHILD  "NiPhysXShapeBasicChild"
#define ED_SHAPE_COMPOUND_ID  "NiPhysXShapeCompoundID"
#define ED_SHAPE_COMPOUND     "NiPhysXShapeCompound"
#define ED_SHAPE_GROUP        "NiPhysXShapeGroup"
#define ED_PROXY_TARGET       "NiPhysXShapeProxyTarget"
#define ED_PROXY_SOURCE       "NiPhysXShapeProxySource"
#define ED_SHAPE_TYPE         "NiPhysXShapeType"
#define ED_SHAPE_POSE         "NiPhysXShapePose"
#define ED_SHAPE_RAD          "NiPhysXShapeRadius"
#define ED_SHAPE_HEIGHT       "NiPhysXShapeHeight"
#define ED_SHAPE_DIM          "NiPhysXShapeDimensions"
#define ED_SHAPE_OFFSET       "NiPhysXShapeOffset"
#define ED_MATERIAL_TYPE      "NiPhysXMaterialType"
#define ED_RESTITUTION        "NiPhysXMaterialRestitution"
#define ED_STATIC_FRICTION    "NiPhysXMaterialStaticFriction"
#define ED_DYNAMIC_FRICTION   "NiPhysXMaterialDynamicFriction"
#define ED_PARTICLE_ACTOR     "NiPhysXParticleActor"
#define ED_JOINT_PARENT       "NiPhysXJointParent"
#define ED_JOINT_CHILD        "NiPhysXJointChild"
#define ED_JOINT_POSN1        "NiPhysXJointPosn1"
#define ED_JOINT_AXIS1        "NiPhysXJointAxis1"
#define ED_JOINT_NORMAL1      "NiPhysXJointNormal1"
#define ED_JOINT_POSN2        "NiPhysXJointPosn2"
#define ED_JOINT_AXIS2        "NiPhysXJointAxis2"
#define ED_JOINT_NORMAL2      "NiPhysXJointNormal2"
#define ED_JOINT_LOCKS        "NiPhysXJointLocks"
#define ED_JOINT_LIMITS       "NiPhysXJointLimits"
#define ED_JOINT_RESTITUTIONS "NiPhysXJointRests"
#define ED_JOINT_SPRINGS      "NiPhysXJointSprings"
#define ED_JOINT_DAMPINGS     "NiPhysXJointDampings"
#define ED_JOINT_MAX_FORCE    "NiPhysXJointMaxForce"
#define ED_JOINT_MAX_TORQUE   "NiPhysXJointMaxTorque"
#define ED_JOINT_NAME         "NiPhysXJointName"
#define ED_JOINT_COLLIDES     "NiPhysXJointCollides"
#define ED_JOINT_PROJ_MODE    "NiPhysXJointProjMode"
#define ED_JOINT_PROJ_DIST    "NiPhysXJointProjDist"
#define ED_JOINT_PROJ_ANGLE   "NiPhysXJointProjAngle"
#define ED_JOINT_GEARING      "NiPhysXJointGearing"
#define ED_JOINT_GEAR_RATIO   "NiPhysXJointGearRatio"
#define ED_CLOTH_PRESSURE     "NiPhysXClothPressure"
#define ED_CLOTH_STATIC       "NiPhysXClothStatic"
#define ED_CLOTH_DISABLE_COLL "NiPhysXClothDisCol"
#define ED_CLOTH_SELF_COLLIDE "NiPhysXClothSelfCol"
#define ED_CLOTH_GRAVITY      "NiPhysXClothGravity"
#define ED_CLOTH_BENDING      "NiPhysXClothBending"
#define ED_CLOTH_ORTHO_BEND   "NiPhysXClothOrthoBend"
#define ED_CLOTH_DAMPING      "NiPhysXClothDamping"
#define ED_CLOTH_TWO_COLLIDE  "NiPhysXClothTwoCol"
#define ED_CLOTH_TRI_COLLIDE  "NiPhysXClothTriCol"
#define ED_CLOTH_TEARABLE     "NiPhysXClothTearable"
#define ED_CLOTH_HARDWARE     "NiPhysXClothHardware"
#define ED_CLOTH_THICKNESS    "NiPhysXClothThickness"
#define ED_CLOTH_DENSITY      "NiPhysXClothDensity"
#define ED_CLOTH_BEND_STIFF   "NiPhysXClothBendStiff"
#define ED_CLOTH_STR_STIFF    "NiPhysXClothStretchStiff"
#define ED_CLOTH_DAMPING_COEF "NiPhysXClothDampingCoef"
#define ED_CLOTH_FRICTION     "NiPhysXClothFriction"
#define ED_CLOTH_PRESSURE_VAL "NiPhysXClothPressureVal"
#define ED_CLOTH_TEAR_FACTOR  "NiPhysXClothTearFactor"
#define ED_CLOTH_ATTACH_TEAR  "NiPhysXClothAttachTearFactor"
#define ED_CLOTH_COLLIDE_RESP "NiPhysXClothCollisionResponse"
#define ED_CLOTH_ATTACH_RESP  "NiPhysXClothAttachmentResponse"
#define ED_CLOTH_EXT_ACCEL    "NiPhysXClothExternalAccel"
#define ED_CLOTH_WAKE_COUNTER "NiPhysXClothWakeUpCounter"
#define ED_CLOTH_SLEEP_VEL    "NiPhysXClothSleepLinVel"
#define ED_CLOTH_ATTACH_SHAPE "NiPhysXClothAttachShape"
#define ED_CLOTH_ATTACHMENT   "NiPhysXClothAttachment"
#define ED_CLOTH_ATTACH_POSN  "NiPhysXClothAttachPosn"
#define ED_CLOTH_UPDATE_NBT   "NiPhysXClothUpdateNBT"


unsigned int MyiPhysX::ms_uiNextIdentifier = 1;
NiTMap<int, unsigned int>* MyiPhysX::ms_pkShapeMap = 0;
NiTMap<int, unsigned int>* MyiPhysX::ms_pkPhysXShapeMap = 0;
NiTMap<int, unsigned int>* MyiPhysX::ms_pkPhysXClothConstrMap = 0;
NiTMap<unsigned int, bool>* MyiPhysX::ms_pkConvexMap = 0;

//---------------------------------------------------------------------------
void MyiPhysX::Initialize()
{
    ms_uiNextIdentifier = 1;
    ms_pkShapeMap = NiNew NiTMap<int, unsigned int>;
    ms_pkPhysXShapeMap = NiNew NiTMap<int, unsigned int>;
    ms_pkPhysXClothConstrMap = NiNew NiTMap<int, unsigned int>;
    ms_pkConvexMap = NiNew NiTMap<unsigned int, bool>;
}
//---------------------------------------------------------------------------
void MyiPhysX::Shutdown()
{
    NiDelete ms_pkShapeMap;
    NiDelete ms_pkPhysXShapeMap;
    NiDelete ms_pkPhysXClothConstrMap;
    NiDelete ms_pkConvexMap;
}
//---------------------------------------------------------------------------
void MyiPhysX::ProcessActor(NiNode* pkNewNode, MFnDagNode& dgNode)
{
    MStatus kStatus;
    
    //
    // Try to find the simulator object
    //
    MPlug kSimulatePlug = dgNode.findPlug("inSimulate", true, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find inSimulate plug\n");
        return;
    }
    
    MPlugArray kPlugArr;
    kSimulatePlug.connectedTo(kPlugArr, true, false, &kStatus);
    if (kPlugArr.length() != 1)
    {
        DtExt_Err("Error:: Could not find inSimulate plug connection\n");
        return;
    }
    MObject kSolverObj = kPlugArr[0].node();
    
    MFnDependencyNode kSolverNode;
    kSolverNode.setObject(kSolverObj);
    if (kSolverNode.typeId().id() != 0x0010BC00)
    {
        DtExt_Err("Error:: InSimulate plug not connected to solver\n");
        return;    
    }
    
    //
    // Figure out what type of physics object it is.
    //
    bool bDynamic = true;
    MPlug kActivePlug = dgNode.findPlug("active", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find active plug\n");
    }
    else
    {
        kActivePlug.getValue(bDynamic);
    }
    
    //
    // All things coming out of Maya are dynamic actors, but maybe kinematic
    // So we always need mass or density
    // Are we using Mass or density?
    //
    int iUseMassDensity = 0;
    double dMass = 0.0;
    double dDensity = 0.0;
    MPlug kUseDensityPlug =
        dgNode.findPlug("overrideMassOrDensity", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find overrideMassOrDensity plug\n");
    }
    else
    {    
        kUseDensityPlug.getValue(iUseMassDensity);
    }
    switch (iUseMassDensity)
    {
        case 1: {
            MPlug kMassPlug = dgNode.findPlug("mass", false, &kStatus);
            if (kStatus != MS::kSuccess)
            {
                DtExt_Err("Error:: Could not find mass plug\n");
            }
            else
            {
                kMassPlug.getValue(dMass);
            }
        } break;

        case 2: {
            MPlug kDensityPlug = dgNode.findPlug("density", false, &kStatus);
            if (kStatus != MS::kSuccess)
            {
                DtExt_Err("Error:: Could not find density plug\n");
            }
            else
            { 
                kDensityPlug.getValue(dDensity);
            }
        } break;
        
        default: {
            dDensity = 1.0;
        } break;
    }
    
    //
    // Material properties. Defaults from PhysX plugin
    //
    double dRestitution = 0.6;
    double dStaticFriction = 0.2;
    double dDynamicFriction = 0.2;
    MPlug kRestitutionPlug = dgNode.findPlug("bounciness", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find bounciness plug\n");
    }
    else
    {    
        kRestitutionPlug.getValue(dRestitution);
    }
    MPlug kStatFricPlug = dgNode.findPlug("staticFriction", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find staticFriction plug\n");
    }
    else
    {
        kStatFricPlug.getValue(dStaticFriction);
    }
    MPlug kDynFricPlug = dgNode.findPlug("dynamicFriction", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find dynamicFriction plug\n");
    }
    else
    { 
        kDynFricPlug.getValue(dDynamicFriction);
    }
    
    //
    // Damping
    //
    double dLinearDamping = 0.0;
    double dAngularDamping = 0.0;
    MPlug kLinDampPlug = dgNode.findPlug("damping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find damping plug\n");
    }
    else
    {    
        kLinDampPlug.getValue(dLinearDamping);
    }
    MPlug kAngDampPlug = dgNode.findPlug("angularDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find angularDamping plug\n");
    }
    else
    { 
        kAngDampPlug.getValue(dAngularDamping);
    }
    
    //
    // Disable gravity.
    //
    bool bDisableGravity = false;
    MPlug kGravityPlug = dgNode.findPlug("defaultGravity", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find defaultGravity plug\n");
    }
    else
    {   
        kGravityPlug.getValue(bDisableGravity);
        bDisableGravity = !bDisableGravity;
    }
    
    //
    // Solver iterations
    //
    int iSolverIterations = 4;
    MPlug kIterationsPlug =
        dgNode.findPlug("solverIterationCount", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find solverIterationCount plug\n");
    }
    else
    {    
        kIterationsPlug.getValue(iSolverIterations);
    }

    //
    // Initial conditions
    //
    double adInitLinVel[3];
    double adInitAngVel[3];
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        adInitLinVel[ui] = 0.0;
        adInitAngVel[ui] = 0.0;
    }    
    MPlug kComponent;
    MPlug kLinVelPlug = dgNode.findPlug("initialVelocity", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find initialVelocity plug\n");
    }
    else
    {
        kComponent = kLinVelPlug.child(0);
        kComponent.getValue(adInitLinVel[0]);
        kComponent = kLinVelPlug.child(1);
        kComponent.getValue(adInitLinVel[1]);
        kComponent = kLinVelPlug.child(2);
        kComponent.getValue(adInitLinVel[2]);
    }
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        adInitLinVel[0] *= gExport.m_fLinearUnitMultiplier;
        adInitLinVel[1] *= gExport.m_fLinearUnitMultiplier;
        adInitLinVel[2] *= gExport.m_fLinearUnitMultiplier;
    }
    MPlug kAngVelPlug = dgNode.findPlug("initialSpin", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find initialSpin plug\n");
    }
    else
    {
        kComponent = kAngVelPlug.child(0);
        kComponent.getValue(adInitAngVel[0]);
        kComponent = kAngVelPlug.child(1);
        kComponent.getValue(adInitAngVel[1]);
        kComponent = kAngVelPlug.child(2);
        kComponent.getValue(adInitAngVel[2]);
    }
    
    //
    // Actor group
    //
    int iActorGroup = 0;
    bool bHaveActorGroup =
        GetExtraAttribute(dgNode, "NiPhysX_ActorGroup", true, iActorGroup);
    
    //
    // Particle actor
    //
    MString sParticleActor;
    bool bHaveParticleActor = GetExtraAttribute(dgNode,
        "NiPhysX_ParticleActor", true, sParticleActor);
    
    
    //
    // Set all the extra data for the actor
    //
    NiIntegerExtraData* pkActorTypeED = NiNew NiIntegerExtraData(
        bDynamic ? PHYSX_ACTOR_DYNAMIC : PHYSX_ACTOR_KINEMATIC);
    pkNewNode->AddExtraData(ED_ACTOR_TYPE, pkActorTypeED);

    NiFloatExtraData* pkMassED = NiNew NiFloatExtraData((float)dMass);
    pkNewNode->AddExtraData(ED_MASS, pkMassED);
    
    NiFloatExtraData* pkDensityED = NiNew NiFloatExtraData((float)dDensity);
    pkNewNode->AddExtraData(ED_DENSITY, pkDensityED);

    NiIntegerExtraData* pkMatTypeED =
        NiNew NiIntegerExtraData(PHYSX_MATERIALTYPE_USER);
    pkNewNode->AddExtraData(ED_MATERIAL_TYPE, pkMatTypeED);

    NiFloatExtraData* pkRestED = NiNew NiFloatExtraData((float)dRestitution);
    pkNewNode->AddExtraData(ED_RESTITUTION, pkRestED);

    NiFloatExtraData* pkStatFricED =
        NiNew NiFloatExtraData((float)dStaticFriction);
    pkNewNode->AddExtraData(ED_STATIC_FRICTION, pkStatFricED);

    NiFloatExtraData* pkDynFricED =
        NiNew NiFloatExtraData((float)dDynamicFriction);
    pkNewNode->AddExtraData(ED_DYNAMIC_FRICTION, pkDynFricED);

    NiFloatExtraData* pkLinDampED =
        NiNew NiFloatExtraData((float)dLinearDamping);
    pkNewNode->AddExtraData(ED_LINEAR_DAMPING, pkLinDampED);

    NiFloatExtraData* pkAngDampED =
        NiNew NiFloatExtraData((float)dAngularDamping);
    pkNewNode->AddExtraData(ED_ANGULAR_DAMPING, pkAngDampED);

    NiIntegerExtraData* pkSolvIterED =
        NiNew NiIntegerExtraData(iSolverIterations);
    pkNewNode->AddExtraData(ED_SOLVER_ITERATIONS, pkSolvIterED);

    NiBooleanExtraData* pkGravityED = 
        NiNew NiBooleanExtraData(bDisableGravity);
    pkNewNode->AddExtraData(ED_DISABLE_GRAVITY, pkGravityED);

    float afData[3];
    afData[0] = (float)adInitLinVel[0];
    afData[1] = (float)adInitLinVel[1];
    afData[2] = (float)adInitLinVel[2];
    NiFloatsExtraData* pkLinVelED = NiNew NiFloatsExtraData(3, afData);
    pkNewNode->AddExtraData(ED_LINEAR_VELOCITY, pkLinVelED);
    
    afData[0] = (float)adInitAngVel[0];
    afData[1] = (float)adInitAngVel[1];
    afData[2] = (float)adInitAngVel[2];
    NiFloatsExtraData* pkAngVelED = NiNew NiFloatsExtraData(3, afData);
    pkNewNode->AddExtraData(ED_ANGULAR_VELOCITY, pkAngVelED);

    if (bHaveActorGroup)
    {
        NiIntegerExtraData* pkActorGroupED =
            NiNew NiIntegerExtraData(iActorGroup);
        pkNewNode->AddExtraData(ED_ACTOR_GROUP, pkActorGroupED);
    }
    
    if (bHaveParticleActor)
    {
        NiStringExtraData* pkPartActED =
            NiNew NiStringExtraData(sParticleActor.asChar());
        pkNewNode->AddExtraData(ED_PARTICLE_ACTOR, pkPartActED); 
    }

    // The shape id for this actor
    unsigned int uiShapeId = ms_uiNextIdentifier++;

    //
    // Work through looking for the shapes.
    //
    MPlug kMeshShapesPlug = dgNode.findPlug("shapes", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find shapes plug\n");
    }
    else
    {
        // Set an ID for this compound, even if it's not a compound.
        // Each mesh will also need its own multi-mesh ID, which
        // is not essential in Maya, but it does make locating meshes
        // and coping with scene graph optimization easier.
        NiIntegerExtraData* pkCompoundED = NiNew NiIntegerExtraData(uiShapeId);
        pkNewNode->AddExtraData(ED_SHAPE_COMPOUND, pkCompoundED);
        
        for (unsigned int ui = 0; ui < kMeshShapesPlug.numElements(); ui++)
        {
            MPlug kMeshShapePlug = kMeshShapesPlug.elementByPhysicalIndex(ui);
            MPlugArray kPlugArr;
            kMeshShapePlug.connectedTo(kPlugArr, true, false, &kStatus);
            if (kPlugArr.length() != 1)
            {
                DtExt_Err("Error:: Shape not connected\n");
            }
            MObject kMeshObj = kPlugArr[0].node();
            
            for (int i = 0; i < DtShapeGetCount(); i++)
            {
                MObject kObj;
                int iGroupCount = DtGroupGetCount(i);
                for (int j = 0; j < iGroupCount; j++)
                {
                    DtExt_ShapeGetOriginal(i, j, kObj);
                    if (kMeshObj == kObj)
                    {
                        ms_pkShapeMap->SetAt(i, uiShapeId);
                    }
                }
            }
        }
    }

    MPlug kPhysXShapesPlug = dgNode.findPlug("physicsShapes", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find shapes plug\n");
    }
    else
    {
        NiExtraData* pkED = pkNewNode->GetExtraData(ED_SHAPE_COMPOUND);
        if (!pkED)
        {
            NiIntegerExtraData* pkCompoundED =
                NiNew NiIntegerExtraData(uiShapeId);
            pkNewNode->AddExtraData(ED_SHAPE_COMPOUND, pkCompoundED);
        }
        
        for (unsigned int ui = 0; ui < kPhysXShapesPlug.numElements(); ui++)
        {
            MPlug kPhysXShapePlug = 
                kPhysXShapesPlug.elementByPhysicalIndex(ui);
            MPlugArray kPlugArr;
            kPhysXShapePlug.connectedTo(kPlugArr, true, false, &kStatus);
            if (kPlugArr.length() != 1)
            {
                DtExt_Err("Error:: PhysX shape not connected\n");
            }
            MObject kShapeObj = kPlugArr[0].node();
            
            for (int i = 0; i < gPhysXShapeManager.GetNumShapes(); i++)
            {
                MObject kObj;

                DtExt_PhysXGetShapeNode(i, kObj );
                if (kShapeObj == kObj)
                {
                    ms_pkPhysXShapeMap->SetAt(i, uiShapeId);
                }
            }
        }
    }

    //
    // Cook shapes and convex, or not
    //
    int iGeomType = 0;
    MPlug kConvexPlug = dgNode.findPlug("geometryType", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find meshType plug\n");
    }
    else
    {   
        kConvexPlug.getValue(iGeomType);
        ms_pkConvexMap->SetAt(uiShapeId, iGeomType == 0);
    }
    
    // See if any cloth is attached to this actor
    unsigned int uiNumAttachments =
        gPhysXClothConstrManager.GetNumConstraints();
    for (unsigned int ui = 0; ui < uiNumAttachments; ui++)
    {
        MDtPhysXClothConstr* pkDtConstr =
            gPhysXClothConstrManager.GetClothConstr(ui);
        if (pkDtConstr->m_kShapeNode == dgNode.object())
        {
            unsigned int uiConstrIndex = 0;
            if (!ms_pkPhysXClothConstrMap->GetAt(ui, uiConstrIndex))
            {
                uiConstrIndex = ms_uiNextIdentifier++;
                ms_pkPhysXClothConstrMap->SetAt(ui, uiConstrIndex);
            }
            AppendIntExtraData(pkNewNode, ED_CLOTH_ATTACH_SHAPE,
                uiConstrIndex);
        }
    }
}
//---------------------------------------------------------------------------
void MyiPhysX::ProcessMesh(NiAVObject* pkMesh, int iShapeNum,
    int iGroupID)
{
    NI_UNUSED_ARG(iGroupID);
    // Get the Maya node
    MObject mObj = MObject::kNullObj;
    DtExt_ShapeGetShapeNode(iShapeNum, mObj);
    MFnDagNode fnDagNode;
    fnDagNode.setObject(mObj);
    
    // Find the parent
    int iComponentNum = gMDtObjectFindComponentID(ETypeShape, iShapeNum);
    NiNode* pkNode = gUserData.GetNode(iComponentNum);

    // Look for cloth on it. Start by getting mesh inputs.
    MStatus kStatus;
    MPlug kMeshPlug = fnDagNode.findPlug("inMesh", true, &kStatus);
    if (kStatus == MS::kSuccess)
    {
        MPlugArray kMeshPlugArr;
        kMeshPlug.connectedTo(kMeshPlugArr, true, false, &kStatus);
        if (kMeshPlugArr.length() == 1)
        {
            MObject kMeshInObj = kMeshPlugArr[0].node();
            MFnDependencyNode fnMeshInDepNode(kMeshInObj);
            if (fnMeshInDepNode.typeId().id() == 0x0010BC13)
            {
                // This node has cloth properties! They actually need to
                // be attached to the parent NiNode, not this shape node.
                ProcessCloth(pkNode, kMeshInObj);
                
                // The shape id for this piece of cloth
                unsigned int uiShapeId = ms_uiNextIdentifier++;

                NiIntegerExtraData* pkCompoundED =
                    NiNew NiIntegerExtraData(uiShapeId);
                pkNode->AddExtraData(ED_SHAPE_COMPOUND, pkCompoundED);
        
                ms_pkShapeMap->SetAt(iShapeNum, uiShapeId);
            }
        }
    }
        
    // Find out if it's a physics shape
    unsigned int uiShapeId;
    if (ms_pkShapeMap->GetAt(iShapeNum, uiShapeId))
    {
        unsigned int uiMeshId;
        
        // See if the parent already has a mesh group tag
        NiExtraData* pkED;
        pkED = pkNode->GetExtraData(ED_SHAPE_COMPOUND_ID);
        if (!pkED)
        {
            NiIntegerExtraData* pkCompoundED =
                NiNew NiIntegerExtraData(uiShapeId);
            pkNode->AddExtraData(ED_SHAPE_COMPOUND_ID, pkCompoundED);
            
            // Add it's mesh group parent tag
            uiMeshId = ms_uiNextIdentifier++;
            NiIntegerExtraData* pkMeshED = NiNew NiIntegerExtraData(uiMeshId);
            pkNode->AddExtraData(ED_SHAPE_MESH_PARENT, pkMeshED);
            
            // And it's shape type
            bool bConvex = false;
            ms_pkConvexMap->GetAt(uiShapeId, bConvex);
            
            NiIntegerExtraData* pkShapeED = NiNew NiIntegerExtraData(
                bConvex ? PHYSX_SHAPE_CONVEX : PHYSX_SHAPE_MESH);
            pkNode->AddExtraData(ED_SHAPE_TYPE, pkShapeED);
        }
        else
        {
            // Get the mesh id
            pkED = pkNode->GetExtraData(ED_SHAPE_MESH_PARENT);
            NIASSERT(pkED && NiIsKindOf(NiIntegerExtraData, pkED));
            uiMeshId = ((NiIntegerExtraData*)pkED)->GetValue();
        }
        
        // Add the tag to this node
        NiIntegerExtraData* pkMeshChildED = NiNew NiIntegerExtraData(uiMeshId);
        pkMesh->AddExtraData(ED_SHAPE_MESH_CHILD, pkMeshChildED);
    }
}
//---------------------------------------------------------------------------
void MyiPhysX::ProcessPhysXShape(int iShapeNum)
{
    // Find out if it's a physics shape
    int iComponentNum = gMDtObjectGetTypeIndex(iShapeNum);
    unsigned int uiShapeId;
    if (ms_pkPhysXShapeMap->GetAt(iComponentNum, uiShapeId))
    {
        // Find the parent
        NiNode* pkNode = gUserData.GetNode(iShapeNum);

        // See if the parent already has a compound group tag. It shouldn't.
        NiExtraData* pkED;
        pkED = pkNode->GetExtraData(ED_SHAPE_COMPOUND_ID);
        if (!pkED)
        {
            NiIntegerExtraData* pkCompoundED =
                NiNew NiIntegerExtraData(uiShapeId);
            pkNode->AddExtraData(ED_SHAPE_COMPOUND_ID, pkCompoundED);
            
        }
        else
        {
            DtExt_Err("Error:: PhysXShape used twice\n");
            return;
        }

        // Set up all the shape data
        kMDtPhysXShape* pkDtShape = gPhysXShapeManager.GetShape(iComponentNum);
        unsigned int uiMeshId = ms_uiNextIdentifier++;

        NiIntegerExtraData* pkMeshED = NiNew NiIntegerExtraData(uiMeshId);
        pkNode->AddExtraData(ED_SHAPE_BASIC_PARENT, pkMeshED);
        pkMeshED = NiNew NiIntegerExtraData(uiMeshId);
        pkNode->AddExtraData(ED_SHAPE_BASIC_CHILD, pkMeshED);

        switch (pkDtShape->m_eShapeType)
        {
            case kMDtPhysXShape::kEllipsoid:
            case kMDtPhysXShape::kConvexHull:
            case kMDtPhysXShape::kBox: {
                NiIntegerExtraData* pkShapeED = NiNew NiIntegerExtraData(
                    PHYSX_SHAPE_BOX);
                pkNode->AddExtraData(ED_SHAPE_TYPE, pkShapeED);
                
                float afDimensions[3];
                afDimensions[0] = pkDtShape->m_fShapeScaleX;
                afDimensions[1] = pkDtShape->m_fShapeScaleY;
                afDimensions[2] = pkDtShape->m_fShapeScaleZ;
                if (gExport.m_bUseCurrentWorkingUnits)
                {
                    afDimensions[0] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[1] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[2] *= gExport.m_fLinearUnitMultiplier;
                }
                NiFloatsExtraData* pkDimED = NiNew NiFloatsExtraData(
                    3, afDimensions);
                pkNode->AddExtraData(ED_SHAPE_DIM, pkDimED);

                NiBooleanExtraData* pkOffsetED =
                    NiNew NiBooleanExtraData(false);
                pkNode->AddExtraData(ED_SHAPE_OFFSET, pkOffsetED);
                } break;
            
            case kMDtPhysXShape::kCylinder:
            case kMDtPhysXShape::kCapsule: {
                NiIntegerExtraData* pkShapeED = NiNew NiIntegerExtraData(
                    PHYSX_SHAPE_CAPSULE);
                pkNode->AddExtraData(ED_SHAPE_TYPE, pkShapeED);

                float afDimensions[6];
                afDimensions[0] = pkDtShape->m_fPoint1X;
                afDimensions[1] = pkDtShape->m_fPoint1Y;
                afDimensions[2] = pkDtShape->m_fPoint1Z;
                afDimensions[3] = pkDtShape->m_fPoint2X;
                afDimensions[4] = pkDtShape->m_fPoint2Y;
                afDimensions[5] = pkDtShape->m_fPoint2Z;
                if (gExport.m_bUseCurrentWorkingUnits)
                {
                    afDimensions[0] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[1] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[2] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[3] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[4] *= gExport.m_fLinearUnitMultiplier;
                    afDimensions[5] *= gExport.m_fLinearUnitMultiplier;
                }
                NiFloatsExtraData* pkDimED = NiNew NiFloatsExtraData(
                    6, afDimensions);
                pkNode->AddExtraData(ED_SHAPE_DIM, pkDimED);
                
                float fRadius = pkDtShape->m_fRadius;
                if (gExport.m_bUseCurrentWorkingUnits)
                {
                    fRadius *= gExport.m_fLinearUnitMultiplier;
                }
                NiFloatExtraData* pkRadiusED = NiNew NiFloatExtraData(fRadius);
                pkNode->AddExtraData(ED_SHAPE_RAD, pkRadiusED);
                } break;
            
            case kMDtPhysXShape::kSphere: {
                NiIntegerExtraData* pkShapeED = NiNew NiIntegerExtraData(
                    PHYSX_SHAPE_SPHERE);
                pkNode->AddExtraData(ED_SHAPE_TYPE, pkShapeED);

                float fRadius = pkDtShape->m_fRadius;
                if (gExport.m_bUseCurrentWorkingUnits)
                {
                    fRadius *= gExport.m_fLinearUnitMultiplier;
                }
                NiFloatExtraData* pkRadiusED = NiNew NiFloatExtraData(fRadius);
                pkNode->AddExtraData(ED_SHAPE_RAD, pkRadiusED);
                } break;
            
            default:;
        }
        
        // Density and mass
        if (pkDtShape->m_bUseDensity)
        {
            float fDensity = pkDtShape->m_fDensity;
            NiFloatExtraData* pkDensityED = NiNew NiFloatExtraData(fDensity);
            pkNode->AddExtraData(ED_DENSITY, pkDensityED);        
        }
        else
        {
            NiFloatExtraData* pkMassED = NiNew NiFloatExtraData(
                pkDtShape->m_fMass);
            pkNode->AddExtraData(ED_MASS, pkMassED);
        }
    }
    else
    {
        DtExt_Err("Error:: PhysXShape not used\n");
    }
}
//---------------------------------------------------------------------------
void MyiPhysX::ProcessPhysXJoint(int iComponentNum)
{
    // Find out if it's a physics joint
    int iJointNum = gMDtObjectGetTypeIndex(iComponentNum);
    kMDtPhysXJoint* pkDtJoint = gPhysXJointManager.GetJoint(iJointNum);
    MObject kJointObj = pkDtJoint->m_MObjectJoint;
    
    MPlug kPlug;
    MPlug kComponent;
    MPlugArray kPlugArr;
    MStatus kStatus;
    
    float fVal;
    int iVal;

    //
    // Get the connected rigid bodies
    //
    NiNode* pkParentNode = 0;
    NiNode* pkChildNode = 0;
    
    MFnDependencyNode dgNode(kJointObj, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: ProcessPhysXJoint:"
            "Joint object is not a dependency node\n");
        return;
    }
    
    kPlug = dgNode.findPlug("rigidBody1", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: ProcessPhysXJoint:"
            "Could not find rigidBody1 plug\n");
        return;
    }
    kPlug.connectedTo(kPlugArr, true, false, &kStatus);
    if (kPlugArr.length() == 1)
    {
        MObject kBody1Obj = kPlugArr[0].node();
        for (int i = 0; i < gMDtObjectGetUsed(); i++)
        {
            MObject kObj;
            gMDtObjectGetTransform(i, kObj);
            if (kBody1Obj == kObj)
            {
                pkParentNode = gUserData.GetNode(i);
                break;
            }
        }
    }

    kPlug = dgNode.findPlug("rigidBody2", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: ProcessPhysXJoint:"
            "Could not find rigidBody2 plug\n");
        return;
    }
    kPlug.connectedTo(kPlugArr, true, false, &kStatus);
    if (kPlugArr.length() == 1)
    {
        MObject kBody2Obj = kPlugArr[0].node();
        for (int i = 0; i < gMDtObjectGetUsed(); i++)
        {
            MObject kObj;
            gMDtObjectGetTransform(i, kObj);
            if (kBody2Obj == kObj)
            {
                pkChildNode = gUserData.GetNode(i);
                break;
            }
        }
    }

    // If an actor is connected to a fixed point, then the child is always
    // the actor, while the parent is null.
    bool bSwitch = false;
    if (!pkChildNode)
    {
        if (!pkParentNode)
            return;
        pkChildNode = pkParentNode;
        pkParentNode = 0;
        bSwitch = true;
    }
    
    unsigned int uiJointId = ms_uiNextIdentifier++;
    
    // Mark the nodes as belonging to a joint
    if (pkParentNode)
        AppendIntExtraData(pkParentNode, ED_JOINT_PARENT, uiJointId);
    AppendIntExtraData(pkChildNode, ED_JOINT_CHILD, uiJointId);
    
    //
    // Set up the reference frames
    //
    float afPosn[3];
    double adOrient[3];
    float afAxis[3];
    float afNormal[3];
    kPlug = dgNode.findPlug("translateCopy", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find translateCopy plug\n");
    }
    else
    {
        kComponent = kPlug.child(0);
        kComponent.getValue(afPosn[0]);
        kComponent = kPlug.child(1);
        kComponent.getValue(afPosn[1]);
        kComponent = kPlug.child(2);
        kComponent.getValue(afPosn[2]);
    }
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        afPosn[0] *= gExport.m_fLinearUnitMultiplier;
        afPosn[1] *= gExport.m_fLinearUnitMultiplier;
        afPosn[2] *= gExport.m_fLinearUnitMultiplier;
    }
    kPlug = dgNode.findPlug("rotateCopy", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find rotateCopy plug\n");
    }
    else
    {
        kComponent = kPlug.child(0);
        kComponent.getValue(adOrient[0]);
        kComponent = kPlug.child(1);
        kComponent.getValue(adOrient[1]);
        kComponent = kPlug.child(2);
        kComponent.getValue(adOrient[2]);
    }
    ExtractAxisAngle(adOrient, afAxis, afNormal);
    if (bSwitch)
    {
        AppendVectorExtraData(pkChildNode, ED_JOINT_POSN2, afPosn);
        AppendVectorExtraData(pkChildNode, ED_JOINT_AXIS2, afAxis);
        AppendVectorExtraData(pkChildNode, ED_JOINT_NORMAL2, afNormal);
    }
    else
    {
        AppendVectorExtraData(pkChildNode, ED_JOINT_POSN1, afPosn);
        AppendVectorExtraData(pkChildNode, ED_JOINT_AXIS1, afAxis);
        AppendVectorExtraData(pkChildNode, ED_JOINT_NORMAL1, afNormal);        
    }

    kPlug = dgNode.findPlug("localPosition2", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find localPosition2 plug\n");
    }
    else
    {
        kComponent = kPlug.child(0);
        kComponent.getValue(afPosn[0]);
        kComponent = kPlug.child(1);
        kComponent.getValue(afPosn[1]);
        kComponent = kPlug.child(2);
        kComponent.getValue(afPosn[2]);
    }
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        afPosn[0] *= gExport.m_fLinearUnitMultiplier;
        afPosn[1] *= gExport.m_fLinearUnitMultiplier;
        afPosn[2] *= gExport.m_fLinearUnitMultiplier;
    }
    kPlug = dgNode.findPlug("localOrientation2", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find localOrientation2 plug\n");
    }
    else
    {
        kComponent = kPlug.child(0);
        kComponent.getValue(adOrient[0]);
        kComponent = kPlug.child(1);
        kComponent.getValue(adOrient[1]);
        kComponent = kPlug.child(2);
        kComponent.getValue(adOrient[2]);
    }
    ExtractAxisAngle(adOrient, afAxis, afNormal);
    if (bSwitch)
    {
        AppendVectorExtraData(pkChildNode, ED_JOINT_POSN1, afPosn);
        AppendVectorExtraData(pkChildNode, ED_JOINT_AXIS1, afAxis);
        AppendVectorExtraData(pkChildNode, ED_JOINT_NORMAL1, afNormal);
    }
    else
    {
        AppendVectorExtraData(pkChildNode, ED_JOINT_POSN2, afPosn);
        AppendVectorExtraData(pkChildNode, ED_JOINT_AXIS2, afAxis);
        AppendVectorExtraData(pkChildNode, ED_JOINT_NORMAL2, afNormal);        
    }

    //
    // Set up miscellaneous things
    //
    fVal = FLT_MAX;
    kPlug = dgNode.findPlug("breakableForce", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find breakableForce plug\n");
    }
    else
    {
        bool bBreakable;
        kPlug.getValue(bBreakable);
        if (bBreakable)
        {
            kPlug = dgNode.findPlug("maxForce", false, &kStatus);
            if (kStatus != MS::kSuccess)
            {
                DtExt_Err("Error:: Could not find maxForce plug\n");
            }
            else
            {
                kPlug.getValue(fVal);
            }
        }
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_MAX_FORCE, fVal);

    fVal = FLT_MAX;
    kPlug = dgNode.findPlug("breakableTorque", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find breakableTorque plug\n");
    }
    else
    {
        bool bBreakable;
        kPlug.getValue(bBreakable);
        if (bBreakable)
        {
            kPlug = dgNode.findPlug("maxTorque", false, &kStatus);
            if (kStatus != MS::kSuccess)
            {
                DtExt_Err("Error:: Could not find maxTorque plug\n");
            }
            else
            {
                kPlug.getValue(fVal);
            }
        }
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_MAX_TORQUE, fVal);

    AppendStringExtraData(pkChildNode, ED_JOINT_NAME,
        gMDtObjectGetName(iComponentNum));
    
    iVal = 0;
    kPlug = dgNode.findPlug("interpenetrate", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find interpenetrate plug\n");
    }
    else
    { 
        bool bInterpenetrate;
        kPlug.getValue(bInterpenetrate);
        iVal = bInterpenetrate ? 0 : 1;
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_COLLIDES, iVal);

    //
    // Joint locked state
    //
    iVal = 0;
    kPlug = dgNode.findPlug("motionX", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find motionX plug\n");
    }
    else
    {
        kPlug.getValue(iVal);
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iVal);

    iVal = 0;
    kPlug = dgNode.findPlug("motionY", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find motionY plug\n");
    }
    else
    {
        kPlug.getValue(iVal);
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iVal);

    iVal = 0;
    kPlug = dgNode.findPlug("motionZ", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find motionZ plug\n");
    }
    else
    {
        kPlug.getValue(iVal);
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iVal);

    iVal = 0;
    kPlug = dgNode.findPlug("motionTwist", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find motionTwist plug\n");
    }
    else
    {
        kPlug.getValue(iVal);
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iVal);

    iVal = 0;
    kPlug = dgNode.findPlug("motionSwing1", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find motionSwing1 plug\n");
    }
    else
    {
        kPlug.getValue(iVal);
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iVal);

    iVal = 0;
    kPlug = dgNode.findPlug("motionSwing2", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find motionSwing2 plug\n");
    }
    else
    {
        kPlug.getValue(iVal);
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iVal);

    //
    // Joint limits
    //
    fVal = 1.0f;
    kPlug = dgNode.findPlug("linearLimitValue", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find linearLimitValue plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        fVal *= gExport.m_fLinearUnitMultiplier;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS, fVal * NI_PI / 180.0f);
    
    fVal = -90.0f;
    kPlug = dgNode.findPlug("twistLowLimitValue", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistLowLimitValue plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS, fVal * NI_PI / 180.0f);
    
    fVal = 90.0f;
    kPlug = dgNode.findPlug("twistHighLimitValue", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistHighLimitValue plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS, fVal * NI_PI / 180.0f);
    
    fVal = 90.0f;
    kPlug = dgNode.findPlug("swing1LimitValue", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing1LimitValue plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS, fVal * NI_PI / 180.0f);
    
    fVal = 90.0f;
    kPlug = dgNode.findPlug("swing2LimitValue", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing2LimitValue plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS, fVal * NI_PI / 180.0f);
    
    //
    // Joint restitutions
    //
    fVal = 0.4f;
    kPlug = dgNode.findPlug("linearLimitRestitution", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find linearLimitRestitution plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("twistLowLimitRestitution", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistLowLimitRestitution plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("twistHighLimitRestitution", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistHighLimitRestitution plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("swing1LimitRestitution", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing1LimitRestitution plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("swing2LimitRestitution", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing2LimitRestitution plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fVal);

    //
    // Joint springs
    //
    fVal = 20.0f;
    kPlug = dgNode.findPlug("linearLimitSpring", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find linearLimitSpring plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fVal);
    
    fVal = 20.0f;
    kPlug = dgNode.findPlug("twistLowLimitSpring", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistLowLimitSpring plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fVal);
    
    fVal = 20.0f;
    kPlug = dgNode.findPlug("twistHighLimitSpring", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistHighLimitSpring plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fVal);
    
    fVal = 20.0f;
    kPlug = dgNode.findPlug("swing1LimitSpring", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing1LimitSpring plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fVal);
    
    fVal = 20.0f;
    kPlug = dgNode.findPlug("swing2LimitSpring", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing2LimitSpring plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fVal);

    //
    // Joint restitutions
    //
    fVal = 0.4f;
    kPlug = dgNode.findPlug("linearLimitDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find linearLimitDamping plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("twistLowLimitDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistLowLimitDamping plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("twistHighLimitDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twistHighLimitDamping plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("swing1LimitDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing1LimitDamping plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fVal);
    
    fVal = 0.4f;
    kPlug = dgNode.findPlug("swing2LimitDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find swing2LimitDamping plug\n");
    }
    else
    {
        kPlug.getValue(fVal);
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fVal);
}
//---------------------------------------------------------------------------
void MyiPhysX::ProcessCloth(NiNode* pkNode, MObject& kMeshInObj)
{
    MStatus kStatus;
    MFnDependencyNode dgNode(kMeshInObj, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: ProcessCloth:"
            "Mesh object is not a dependency node\n");
        return;
    }

    MPlug kPlug;
    MPlug kComponent;
    MPlugArray kPlugArr;
    
    bool bValue = false;
    float fValue = 0.0f;

    NiIntegerExtraData* pkIntED = 0;
    NiFloatExtraData* pkFloatED = 0;
    NiBooleanExtraData* pkBoolED = 0;

    // Self collision and gravity are always on in the Maya PhysX plug-in
    pkBoolED = NiNew NiBooleanExtraData(true);
    pkNode->AddExtraData(ED_CLOTH_SELF_COLLIDE, pkBoolED);
    pkBoolED = NiNew NiBooleanExtraData(true);
    pkNode->AddExtraData(ED_CLOTH_GRAVITY, pkBoolED);

    kPlug = dgNode.findPlug("enablePressure", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find enablePressure plug\n");
    }
    else
    { 
        kPlug.getValue(bValue);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_PRESSURE, pkBoolED);
    }

    kPlug = dgNode.findPlug("static", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find static plug\n");
    }
    else
    { 
        kPlug.getValue(bValue);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_STATIC, pkBoolED);
    }

    kPlug = dgNode.findPlug("enableBending", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find enableBending plug\n");
    }
    else
    { 
        kPlug.getValue(bValue);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_BENDING, pkBoolED);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_ORTHO_BEND, pkBoolED);
    }

    kPlug = dgNode.findPlug("enableDamping", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find enableDamping plug\n");
    }
    else
    { 
        kPlug.getValue(bValue);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_DAMPING, pkBoolED);
    }

    kPlug = dgNode.findPlug("twoWayCollision", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find twoWayCollision plug\n");
    }
    else
    { 
        kPlug.getValue(bValue);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_TWO_COLLIDE, pkBoolED);
    }

    kPlug = dgNode.findPlug("enableTearing", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find enableTearing plug\n");
    }
    else
    { 
        kPlug.getValue(bValue);
        pkBoolED = NiNew NiBooleanExtraData(bValue);
        pkNode->AddExtraData(ED_CLOTH_TEARABLE, pkBoolED);
    }

    fValue = 0.01f;
    kPlug = dgNode.findPlug("thickness", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find thickness plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_THICKNESS, pkFloatED);
    }

    fValue = 1.0f;
    kPlug = dgNode.findPlug("bendingStiffness", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find bendingStiffness plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_BEND_STIFF, pkFloatED);
    }

    fValue = 1.0f;
    kPlug = dgNode.findPlug("stretchingStiffness", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find stretchingStiffness plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_STR_STIFF, pkFloatED);
    }

    fValue = 0.5f;
    kPlug = dgNode.findPlug("dampingCoefficient", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find dampingCoefficient plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_DAMPING_COEF, pkFloatED);
    }

    fValue = 0.5f;
    kPlug = dgNode.findPlug("friction", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find friction plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_FRICTION, pkFloatED);
    }

    fValue = 1.5f;
    kPlug = dgNode.findPlug("pressure", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find pressure plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_PRESSURE_VAL, pkFloatED);
    }

    fValue = 1.5f;
    kPlug = dgNode.findPlug("tearFactor", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find tearFactor plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_TEAR_FACTOR, pkFloatED);
    }

    fValue = 0.2f;
    kPlug = dgNode.findPlug("bounciness", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find bounciness plug\n");
    }
    else
    {
        kPlug.getValue(fValue);
        pkFloatED = NiNew NiFloatExtraData(fValue);
        pkNode->AddExtraData(ED_CLOTH_COLLIDE_RESP, pkFloatED);
    }

    // These things are not set-able by the Maya plug-in
    // ED_CLOTH_TRI_COLLIDE  "NiPhysXClothTriCol"
    // ED_CLOTH_DISABLE_COLL "NiPhysXClothDisCol"
    // ED_CLOTH_HARDWARE     "NiPhysXClothHardware"
    // ED_CLOTH_DENSITY      "NiPhysXClothDensity"
    // ED_CLOTH_ATTACH_TEAR  "NiPhysXClothAttachTearFactor"
    // ED_CLOTH_ATTACH_RESP  "NiPhysXClothAttachmentResponse"
    // ED_CLOTH_EXT_ACCEL    "NiPhysXClothExternalAccel"
    // ED_CLOTH_WAKE_COUNTER "NiPhysXClothWakeUpCounter"
    // ED_CLOTH_SLEEP_VEL    "NiPhysXClothSleepLinVel"

    kPlug = dgNode.findPlug("inConstraint", false, &kStatus);
    if (kStatus != MS::kSuccess)
    {
        DtExt_Err("Error:: Could not find cloth inConstraint plug\n");
    }
    else
    {
        for (unsigned int ui = 0; ui < kPlug.numElements(); ui++)
        {
            MPlug kAttachPlug = kPlug.elementByPhysicalIndex(ui);
            MPlugArray kPlugArr;
            kAttachPlug.connectedTo(kPlugArr, true, false, &kStatus);
            if (kPlugArr.length() != 1)
            {
                DtExt_Err("Error:: PhysX constraint not connected\n");
            }
            MObject kConstrObj = kPlugArr[0].node();
            
            int iComp = gPhysXClothConstrManager.GetClothConstr(kConstrObj);
            if (iComp == -1)
            {
                DtExt_Err("Error:: PhysX cloth constraint doesn't exist\n");
            }
            unsigned int uiConstrIndex = 0;
            if (!ms_pkPhysXClothConstrMap->GetAt(iComp, uiConstrIndex))
            {
                uiConstrIndex = ms_uiNextIdentifier++;
                ms_pkPhysXClothConstrMap->SetAt(iComp, uiConstrIndex);
            }
            
            // Stash attachment info
            MDtPhysXClothConstr* pkDtConstr =
                gPhysXClothConstrManager.GetClothConstr(iComp);
            if (pkDtConstr->m_kShapeNode != MObject::kNullObj)
            {
                AppendIntExtraData(pkNode, ED_CLOTH_ATTACHMENT, uiConstrIndex);
            }
            else
            {
                AppendIntExtraData(pkNode, ED_CLOTH_ATTACHMENT, -1);
            }
            
            AppendIntExtraData(pkNode, ED_CLOTH_ATTACHMENT,
                pkDtConstr->m_uiVertexCount);
            for (unsigned int ui = 0; ui < pkDtConstr->m_uiVertexCount; ui++)
            {
                AppendFloatExtraData(pkNode, ED_CLOTH_ATTACH_POSN,
                    pkDtConstr->m_pkPositions[ui * 3]);
                AppendFloatExtraData(pkNode, ED_CLOTH_ATTACH_POSN,
                    pkDtConstr->m_pkPositions[ui * 3 + 1]);
                AppendFloatExtraData(pkNode, ED_CLOTH_ATTACH_POSN,
                    pkDtConstr->m_pkPositions[ui * 3 + 2]);
            }
        }
    }

    //
    // Update normals
    //
    bool bUpdateNormals = false;
    if (GetExtraAttribute(dgNode, "Ni_ClothUpdateNormals", true,
        bUpdateNormals))
    {
        NiBooleanExtraData* pkUpdateNormsED =
            NiNew NiBooleanExtraData(bUpdateNormals);
        pkNode->AddExtraData(ED_CLOTH_UPDATE_NBT, pkUpdateNormsED);
    }

    //
    // Actor group
    //
    int iActorGroup = 0;
    if (GetExtraAttribute(dgNode, "NiPhysX_ActorGroup", true, iActorGroup))
    {
        NiIntegerExtraData* pkActorGroupED =
            NiNew NiIntegerExtraData(iActorGroup);
        pkNode->AddExtraData(ED_ACTOR_GROUP, pkActorGroupED);
    }

    pkIntED = NiNew NiIntegerExtraData(PHYSX_ACTOR_CLOTH);
    pkNode->AddExtraData(ED_ACTOR_TYPE, pkIntED);
}
//---------------------------------------------------------------------------
void MyiPhysX::ExtractAxisAngle(double* adAngles, float* afAxis,
    float* afNormal)
{
    MTransformationMatrix::RotationOrder kRotOrder =
        MTransformationMatrix::kXYZ;
    MTransformationMatrix kXform;
    kXform.setRotation(adAngles, kRotOrder);
    MMatrix kRotMat = kXform.asMatrix();

    MVector kAxis = MVector(1,0,0) * kRotMat;
    afAxis[0] = (float)kAxis[0];
    afAxis[1] = (float)kAxis[1];
    afAxis[2] = (float)kAxis[2];

    MVector kNormal = MVector(0,1,0) * kRotMat;
    afNormal[0] = (float)kNormal[0];
    afNormal[1] = (float)kNormal[1];
    afNormal[2] = (float)kNormal[2];
}
//---------------------------------------------------------------------------
void MyiPhysX::AppendIntExtraData(NiNode* pkNode, const char* pcTag, int iVal)
{
    NiExtraData* pkED = pkNode->GetExtraData(pcTag);
    if (pkED)
    {
        NiIntegersExtraData* pkIntsED = (NiIntegersExtraData*)pkED;
        int* aiVals;
        unsigned int uiSize;
        pkIntsED->GetArray(uiSize, aiVals);
        int* aiNewVals = NiAlloc(int, uiSize+1);
        for (unsigned int ui = 0; ui < uiSize; ui++)
            aiNewVals[ui] = aiVals[ui];
        aiNewVals[uiSize] = iVal;
        pkIntsED->SetArray(uiSize+1, aiNewVals);
        NiFree(aiNewVals);
    }
    else
    {
        NiIntegersExtraData* pkIntsED = NiNew NiIntegersExtraData(1, &iVal);
        pkNode->AddExtraData(pcTag, pkIntsED);
    }
}
//---------------------------------------------------------------------------
void MyiPhysX::AppendVectorExtraData(NiNode* pkNode, const char* pcTag,
    float* afAddVals)
{
    NiExtraData* pkED = pkNode->GetExtraData(pcTag);
    if (pkED)
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkED;
        float* afVals;
        unsigned int uiSize;
        pkFloatsED->GetArray(uiSize, afVals);
        float* afNewVals = NiAlloc(float, uiSize+3);
        for (unsigned int ui = 0; ui < uiSize; ui++)
            afNewVals[ui] = afVals[ui];
        afNewVals[uiSize] = afAddVals[0];
        afNewVals[uiSize+1] = afAddVals[1];
        afNewVals[uiSize+2] = afAddVals[2];
        pkFloatsED->SetArray(uiSize+3, afNewVals);
        NiFree(afNewVals);
    }
    else
    {
        NiFloatsExtraData* pkFloatsED = NiNew NiFloatsExtraData(3, afAddVals);
        pkNode->AddExtraData(pcTag, pkFloatsED);
    }
}
//---------------------------------------------------------------------------
void MyiPhysX::AppendFloatExtraData(NiNode* pkNode, const char* pcTag,
    float fVal)
{
    NiExtraData* pkED = pkNode->GetExtraData(pcTag);
    if (pkED)
    {
        NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkED;
        float* afVals;
        unsigned int uiSize;
        pkFloatsED->GetArray(uiSize, afVals);
        float* afNewVals = NiAlloc(float, uiSize+1);
        for (unsigned int ui = 0; ui < uiSize; ui++)
            afNewVals[ui] = afVals[ui];
        afNewVals[uiSize] = fVal;
        pkFloatsED->SetArray(uiSize+1, afNewVals);
        NiFree(afNewVals);
    }
    else
    {
        NiFloatsExtraData* pkFloatsED = NiNew NiFloatsExtraData(1, &fVal);
        pkNode->AddExtraData(pcTag, pkFloatsED);
    }
}
//---------------------------------------------------------------------------
void MyiPhysX::AppendStringExtraData(NiNode* pkNode, const char* pcTag,
    const char* pcVal)
{
    NiExtraData* pkED = pkNode->GetExtraData(pcTag);
    if (pkED)
    {
        NiStringsExtraData* pkStringsED = (NiStringsExtraData*)pkED;
        char** apcVals;
        unsigned int uiSize;
        pkStringsED->GetArray(uiSize, apcVals);
        const char** apcNewVals = NiAlloc(const char*, uiSize+1);
        for (unsigned int ui = 0; ui < uiSize; ui++)
            apcNewVals[ui] = apcVals[ui];
        apcNewVals[uiSize] = pcVal;
        pkStringsED->SetArray(uiSize+1, apcNewVals);
        NiFree(apcNewVals);
    }
    else
    {
        NiStringsExtraData* pkStringsED = NiNew NiStringsExtraData(1, &pcVal);
        pkNode->AddExtraData(pcTag, pkStringsED);
    }
}
//---------------------------------------------------------------------------
