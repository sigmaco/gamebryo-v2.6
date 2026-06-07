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

#include "MaxImmerse.h"

#include "NiMAXPhysicsConverter.h"
#include "NIMAXHierConverter.h"
#include "NiUtils.h"


// All these defines must be mirrored in the Maya exporter and
// the PhysXExtractorDLL tool plug-in
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

#define PHYSX_JOINT_FIXED   2
#define PHYSX_JOINT_HINGE   3
#define PHYSX_JOINT_SPHERE  4

#define PHYSX_MATERIALTYPE_INVALID 0
#define PHYSX_MATERIALTYPE_3DS     1
#define PHYSX_MATERIALTYPE_USER    2
#define PHYSX_MATERIALTYPE_DEFAULT 3
#define PHYSX_MATERIALTYPE_PRESET  4

#define PHYSX_PHYSICSTYPE         "PhysicsType"
#define PHYSX_PROXY               "Proxy_Geometry"
#define PHYSX_MASS                "Mass"
#define PHYSX_DENSITY             "Density"
#define PHYSX_PARTICLE_ACTOR      "NiPhysXParticleActor"
#define PHYSX_FRICTION            "Friction"
#define PHYSX_STATICFRICTION      "StaticFriction"
#define PHYSX_ELLASTICITY         "Ellasticity"
#define PHYSX_RESTITUTION         "Restitution"
#define PHYSX_MATERIALTYPE        "MaterialType"
#define PHYSX_INIT_LV_X           "InitialVelocityX"
#define PHYSX_INIT_LV_Y           "InitialVelocityY"
#define PHYSX_INIT_LV_Z           "InitialVelocityZ"
#define PHYSX_INIT_AV_X           "InitialSpinX"
#define PHYSX_INIT_AV_Y           "InitialSpinY"
#define PHYSX_INIT_AV_Z           "InitialSpinZ"
#define PHYSX_ACTOR_GROUP         "NiPhysXActorGroup"
#define PHYSX_SHAPE_GROUP         "NiPhysXShapeGroup"
#define PHYSX_ACTOR_ITERATIONS    "NiPhysXActorIterations"
#define PHYSX_CLOTH_UPDATE_NBT    "NiClothUpdateNormals"
#define PHYSX_IS_CONCAVE          "IsConcave"
#define PHYSX_JOINT_TYPE          "pmljointtype"
#define PHYSX_JOINT_AXIS          "pmljointaxis"
#define PHYSX_JOINT_NORMAL        "pmljointnrml"
#define PHYSX_JOINT_LIMITS        "pmljointlimits"
#define PHYSX_JOINT_TWIST_LOW     "pmljointtwistlow"
#define PHYSX_JOINT_TWIST_HIGH    "pmljointtwisthigh"
#define PHYSX_JOINT_SWING_1       "pmljointswing1"
#define PHYSX_JOINT_SWING_2       "pmljointswing2"
#define PHYSX_CLOTH_PRESSURE_FLAG "px_clf_pressure"
#define PHYSX_CLOTH_STATIC        "px_clf_static"
#define PHYSX_CLOTH_DISABLE_COL   "px_clf_disable_collision"
#define PHYSX_CLOTH_SELF_COLLIDE  "px_clf_selfcollision"
#define PHYSX_CLOTH_GRAVITY       "px_clf_gravity"
#define PHYSX_CLOTH_BENDING       "px_clf_bending"
#define PHYSX_CLOTH_BENDING_ORTHO "px_clf_bending_ortho"
#define PHYSX_CLOTH_DAMPING       "px_clf_damping"
#define PHYSX_CLOTH_COLLIDE_TWO   "px_clf_collision_twoway"
#define PHYSX_CLOTH_COLLIDE_TRI   "px_clf_triangle_collision"
#define PHYSX_CLOTH_TEARABLE      "px_clf_tearable"
#define PHYSX_CLOTH_HARDWARE      "px_clf_hardware"
#define PHYSX_CLOTH_THICKNESS     "px_clp_thickness"
#define PHYSX_CLOTH_DENSITY       "px_clp_density"
#define PHYSX_CLOTH_BENDING_STIFF "px_clp_bendingStiffness"
#define PHYSX_CLOTH_STRETCH_STIFF "px_clp_stretchingStiffness"
#define PHYSX_CLOTH_DAMPING_COEF  "px_clp_dampingCoefficient"
#define PHYSX_CLOTH_FRICTION      "px_clp_friction"
#define PHYSX_CLOTH_PRESSURE_VAL  "px_clp_pressure"
#define PHYSX_CLOTH_TEAR_FACTOR   "px_clp_tearFactor"
#define PHYSX_CLOTH_COL_RESPONSE  "px_clp_collisionResponseCoef"
#define PHYSX_CLOTH_ATTACH_RESP   "px_clp_attachResponseCoef"
#define PHYSX_CLOTH_SOLVER_ITER   "px_clp_solverIterations"
#define PHYSX_CLOTH_AUTOATTACH    "px_cll_autoattach"

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
#define ED_MAX_JOINT_POSN     "NiPhysXMaxJointPosn"
#define ED_MAX_JOINT_AXIS     "NiPhysXMaxJointAxis"
#define ED_MAX_JOINT_NORMAL   "NiPhysXMaxJointNormal"
#define ED_MAX_JOINT_LOCKS    "NiPhysXMaxJointLocks"
#define ED_MAX_JOINT_LIMITS   "NiPhysXMaxJointLimits"
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

extern const char* gpcMultiMtlName;

unsigned int NiMAXPhysicsConverter::ms_uiNextShapeID = 1;
NiTStringMap<unsigned int>* NiMAXPhysicsConverter::ms_pkProxyMap = 0;
NiTMap<INode*, unsigned int>* NiMAXPhysicsConverter::ms_pkCompoundMap = 0;
NiTMap<unsigned int, INode*>* NiMAXPhysicsConverter::ms_pkJointMap = 0;
NiTMap<unsigned int, INode*>* NiMAXPhysicsConverter::ms_pkD6JointMap = 0;

//---------------------------------------------------------------------------
NiMAXPhysicsConverter::NiMAXPhysicsConverter()
{
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::Init()
{
    ms_uiNextShapeID = 1;
    ms_pkProxyMap = NiNew NiTStringMap<unsigned int>(113, true);
    ms_pkCompoundMap = NiNew NiTMap<INode*, unsigned int>(113);
    ms_pkJointMap = NiNew NiTMap<unsigned int, INode*>(113);
    ms_pkD6JointMap = NiNew NiTMap<unsigned int, INode*>(113);
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::Shutdown()
{
    NiDelete ms_pkProxyMap;
    NiDelete ms_pkCompoundMap;
    NiDelete ms_pkJointMap;
    NiDelete ms_pkD6JointMap;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::Finish(TimeValue fTime)
{
    // Hook up D6 joints with their actors
    NiTMapIterator iter = ms_pkD6JointMap->GetFirstPos();
    while (iter)
    {
        unsigned int uiJointID;
        INode* pkMaxNode;
        ms_pkD6JointMap->GetNext(iter, uiJointID, pkMaxNode);
        int iStatus;
        iStatus = ProcessD6Joint(uiJointID, pkMaxNode, fTime);
        if (iStatus != W3D_STAT_OK)
            return iStatus;
    }
    
    return W3D_STAT_OK;
}
//---------------------------------------------------------------------------
bool NiMAXPhysicsConverter::IsPhysicsJoint(INode* pkMaxNode, TimeValue fTime)
{
    ObjectState kOState = pkMaxNode->EvalWorldState(fTime);
    Object* pObj = kOState.obj;

    Class_ID OLD_D6JOINT_CLASS_ID(0x5fa2e45d, 0x16e7a9e);
    if (pObj && pObj->ClassID() == OLD_D6JOINT_CLASS_ID)
    {
        return true;
    }
    
    Class_ID D6JOINT_CLASS_ID(0x5fa2e45d, 0x17e7a9e);
    if (pObj && pObj->ClassID() == D6JOINT_CLASS_ID)
    {
        return true;
    }
    
    return false;
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::PreChildrenProcess(INode* pMaxNode,
    NiNode* pkNiNode, Object* pObj, TimeValue m_animStart)
{
    // Deal with groups first
    if (IsPhysicsGroupHead(pMaxNode))
    {
        ConvertGroup(pObj, pMaxNode, pkNiNode, m_animStart);
    }

}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::PostChildrenProcess(INode* pMaxNode,
    NiNode* pkNiNode, Object*, TimeValue )
{
    // Check for joint parentage
    SetJointParent(pMaxNode, pkNiNode);
}
//---------------------------------------------------------------------------
bool NiMAXPhysicsConverter::IsPhysicsGroupHead(INode* pMaxNode)
{
    if (!pMaxNode->IsGroupHead() || pMaxNode->IsGroupMember())
        return false;
        
    if (pMaxNode->UserPropExists(PHYSX_PHYSICSTYPE))
        return true;
    
    return false;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertGeom(Object* pObj, INode* pMaxNode,
    NiNode* pkNode, TimeValue fTime, ScaleInfo *pkParentScale)
{
    Class_ID OLD_D6JOINT_CLASS_ID(0x5fa2e45d, 0x16e7a9e);
    Class_ID D6JOINT_CLASS_ID(0x5fa2e45d, 0x17e7a9e);
    if (pObj && (pObj->ClassID() == OLD_D6JOINT_CLASS_ID ||
        pObj->ClassID() == D6JOINT_CLASS_ID))
    {
        return ConvertD6Joint(pObj, pMaxNode, fTime);
    }

    // First check if in a group. The PhysX plugin does not mark group
    // elements as physical.
    unsigned int uiShapeGroupID = 0;
    // Check if we are part of a group. If so, go looking for the head.
    if (pMaxNode->IsGroupMember())
    {
        INode* pMaxGroupHead = pMaxNode->GetParentNode();
        while (pMaxGroupHead->IsGroupMember())
            pMaxGroupHead = pMaxGroupHead->GetParentNode();
        
        uiShapeGroupID = GetCompoundID(pMaxGroupHead);
        if (uiShapeGroupID)
        {
            NiIntegerExtraData* pkShapeGroupED =
                NiNew NiIntegerExtraData(uiShapeGroupID);
            pkNode->AddExtraData(ED_SHAPE_COMPOUND_ID, pkShapeGroupED);
            
            return ConvertGroupMember(pObj, pMaxNode, pkNode, fTime,
                pkParentScale);
        }
    }
    
    int iPhysXType;
    bool bHasPhysX = pMaxNode->GetUserPropInt(PHYSX_PHYSICSTYPE, iPhysXType) ?
        true : false;
    if (!bHasPhysX || !(
        iPhysXType == PHYSX_ACTOR_DYNAMIC ||
        iPhysXType == PHYSX_ACTOR_KINEMATIC ||
        iPhysXType == PHYSX_ACTOR_STATIC ||
        iPhysXType == PHYSX_ACTOR_RB_OVER || // This is for older assets with proxies.
        iPhysXType == PHYSX_ACTOR_UNDEF ||
        iPhysXType == PHYSX_ACTOR_CLOTH))
    {
        return 0;
    }
   
    bool bHasProxy;
    int iStatus = ConvertActor(pMaxNode, pkNode, iPhysXType, bHasProxy);
    if (iStatus)
        return iStatus;
        
    if (bHasProxy)
    {
        ConvertMaterial(pMaxNode, pkNode, fTime);
        return iStatus;
    }

    return ConvertShape(pObj, pMaxNode, pkNode, fTime, pkParentScale);    
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertGroup(Object*, INode* pMaxNode,
    NiNode* pkNode, TimeValue)
{
    if (pMaxNode->IsGroupMember())
    {
        // We are actually part of another group, so we ourselves do nothing.
        return 0;
    }
    
    int iPhysXType;
    bool bHasPhysX = pMaxNode->GetUserPropInt(PHYSX_PHYSICSTYPE, iPhysXType) ?
        true : false;
    if (!bHasPhysX || !(
        iPhysXType == PHYSX_ACTOR_DYNAMIC ||
        iPhysXType == PHYSX_ACTOR_KINEMATIC ||
        iPhysXType == PHYSX_ACTOR_STATIC ||
        iPhysXType == PHYSX_ACTOR_RB_OVER ||
        iPhysXType == PHYSX_ACTOR_UNDEF ||
        iPhysXType == PHYSX_ACTOR_CLOTH))
    {
        return 0;
    }
        
    bool bHasProxy;
    int iStatus = ConvertActor(pMaxNode, pkNode, iPhysXType, bHasProxy);
    if (iStatus || bHasProxy)
        return iStatus;

    // Set up the shape ID
    unsigned int uiShapeGroupID = ms_uiNextShapeID++;
    NiIntegerExtraData* pkCompoundED =
        NiNew NiIntegerExtraData(uiShapeGroupID);
    pkNode->AddExtraData(ED_SHAPE_COMPOUND, pkCompoundED);
   
    SetCompoundID(pMaxNode, uiShapeGroupID);

    return 0;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertActor(INode* pMaxNode, NiNode* pkNode,
    int iPhysXType, bool& bHasProxy)
{
    NiIntegerExtraData* pkPhysXTypeED = NiNew NiIntegerExtraData(iPhysXType);
    pkNode->AddExtraData(ED_ACTOR_TYPE, pkPhysXTypeED);

    // Deal with cloth. None of the other processing should be used on cloth.
    if (iPhysXType == PHYSX_ACTOR_CLOTH)
    {
        bHasProxy = false;
        return ConvertCloth(pMaxNode, pkNode);
    }

    if (iPhysXType != PHYSX_ACTOR_RB_OVER && iPhysXType != PHYSX_ACTOR_UNDEF) // Not a proxy
    { 
        // PhysX puts actor/shape density in "Density"
        float fDensity = 0.0;
        if (pMaxNode->GetUserPropFloat(PHYSX_DENSITY, fDensity))
        {
            NiFloatExtraData* pkPhysXDensityED =
                NiNew NiFloatExtraData(fDensity);
            pkNode->AddExtraData(ED_DENSITY, pkPhysXDensityED);
        }

        // PhysX puts actor/shape mass in "Mass"
        float fMass = 0.0;
        if (pMaxNode->GetUserPropFloat(PHYSX_MASS, fMass))
        {
            NiFloatExtraData* pkPhysXMassED = NiNew NiFloatExtraData(fMass);
            pkNode->AddExtraData(ED_MASS, pkPhysXMassED);    
        }
    }
    
    if (iPhysXType == PHYSX_ACTOR_DYNAMIC)
    {
        float afLinVel[3];
        afLinVel[0] = 0.0f;
        afLinVel[1] = 0.0f;
        afLinVel[2] = 0.0f;
        pMaxNode->GetUserPropFloat(PHYSX_INIT_LV_X, afLinVel[0]);
        pMaxNode->GetUserPropFloat(PHYSX_INIT_LV_Y, afLinVel[1]);
        pMaxNode->GetUserPropFloat(PHYSX_INIT_LV_Z, afLinVel[2]);
        NiFloatsExtraData* pkPhysXLinVED =
            NiNew NiFloatsExtraData(3, afLinVel);
        pkNode->AddExtraData(ED_LINEAR_VELOCITY, pkPhysXLinVED);    

        float afAngVel[3];
        afAngVel[0] = 0.0f;
        afAngVel[1] = 0.0f;
        afAngVel[2] = 0.0f;
        pMaxNode->GetUserPropFloat(PHYSX_INIT_AV_X, afAngVel[0]);
        pMaxNode->GetUserPropFloat(PHYSX_INIT_AV_Y, afAngVel[1]);
        pMaxNode->GetUserPropFloat(PHYSX_INIT_AV_Z, afAngVel[2]);
        NiFloatsExtraData* pkPhysXAngVED =
            NiNew NiFloatsExtraData(3, afAngVel);
        pkNode->AddExtraData(ED_ANGULAR_VELOCITY, pkPhysXAngVED);
    }
    
    // We put collision group information into "NiPhysXActorGroup"
    int iGroup = 0;
    if (pMaxNode->GetUserPropInt(PHYSX_ACTOR_GROUP, iGroup))
    {
        NiIntegerExtraData* pkPhysXGroupED = NiNew NiIntegerExtraData(iGroup);
        pkNode->AddExtraData(ED_ACTOR_GROUP, pkPhysXGroupED);    
    }
    
    // We put solver iteration count into "NiPhysXActoriterations"
    int iIter = 0;
    if (pMaxNode->GetUserPropInt(PHYSX_ACTOR_ITERATIONS, iIter))
    {
        NiIntegerExtraData* pkPhysXIterED = NiNew NiIntegerExtraData(iIter);
        pkNode->AddExtraData(ED_SOLVER_ITERATIONS, pkPhysXIterED);    
    }

    TSTR kMaxStr;
    
    // Look to see if we are a particle system particle actor
    if (pMaxNode->GetUserPropString(PHYSX_PARTICLE_ACTOR, kMaxStr))
    {
        NiStringExtraData* pkPhysXPSysActorED =
            NiNew NiStringExtraData((char*)kMaxStr);
        pkNode->AddExtraData(ED_PARTICLE_ACTOR, pkPhysXPSysActorED);
    }

    // Check if I'm a proxy.
    bHasProxy = false;    
    if (iPhysXType == PHYSX_ACTOR_RB_OVER || iPhysXType == PHYSX_ACTOR_UNDEF) // Could be a proxy
    {
        unsigned int uiProxySrcID = GetProxyID((char*)pMaxNode->GetName());
        if (!uiProxySrcID)
        {
            uiProxySrcID = ms_uiNextShapeID++;
            SetProxyID((char*)pMaxNode->GetName(), uiProxySrcID);
        }
        NiIntegerExtraData* pkProxyED =
            NiNew NiIntegerExtraData(uiProxySrcID);
        pkNode->AddExtraData(ED_PROXY_SOURCE, pkProxyED);
    }
    else // Look for a proxy
    {
        if (pMaxNode->GetUserPropString(PHYSX_PROXY, kMaxStr))
        {
            if (strcmp((char*)kMaxStr, "<None>"))
            {
                unsigned int uiProxyID = GetProxyID((char*)kMaxStr);
                if (!uiProxyID)
                {
                    uiProxyID = ms_uiNextShapeID++;
                    SetProxyID((char*)kMaxStr, uiProxyID);
                }
                NiIntegerExtraData* pkPhysXProxyED =
                    NiNew NiIntegerExtraData(uiProxyID);
                pkNode->AddExtraData(ED_PROXY_TARGET, pkPhysXProxyED);
                bHasProxy = true;
            }
        }
    }
    
    // Look for a joint
    int iJointType;
    bool bHasJoint = pMaxNode->GetUserPropInt(PHYSX_JOINT_TYPE, iJointType) ?
        true : false;
    if (bHasJoint && iJointType >= PHYSX_JOINT_FIXED)
    {
        int iStatus = ConvertJoint(pMaxNode, pkNode, iJointType);
        if (iStatus)
            return iStatus;
    }
    
    return 0;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertCloth(INode* pMaxNode, NiNode* pkNode)
{
    BOOL bValue = false;
    float fValue = 0.0f;
    int iValue = 0;
    NiIntegerExtraData* pkIntED = 0;
    NiIntegersExtraData* pkIntsED = 0;
    NiFloatExtraData* pkFloatED = 0;
    NiBooleanExtraData* pkBoolED = 0;

    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_PRESSURE_FLAG, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_PRESSURE, pkBoolED);

    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_STATIC, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_STATIC, pkBoolED);

    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_DISABLE_COL, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_DISABLE_COLL, pkBoolED);

    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_SELF_COLLIDE, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_SELF_COLLIDE, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_GRAVITY, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_GRAVITY, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_BENDING, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_BENDING, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_BENDING_ORTHO, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_ORTHO_BEND, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_DAMPING, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_DAMPING, pkBoolED);

    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_COLLIDE_TWO, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_TWO_COLLIDE, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_COLLIDE_TRI, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_TRI_COLLIDE, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_TEARABLE, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_TEARABLE, pkBoolED);
    
    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_HARDWARE, bValue))
    {
        pkBoolED = NiNew NiBooleanExtraData(bValue ? true : false);
    }
    else
    {
        pkBoolED = NiNew NiBooleanExtraData(false);
    }
    pkNode->AddExtraData(ED_CLOTH_HARDWARE, pkBoolED);

    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_THICKNESS, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(0.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_THICKNESS, pkFloatED);

    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_DENSITY, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(0.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_DENSITY, pkFloatED);

    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_BENDING_STIFF, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(0.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_BEND_STIFF, pkFloatED);

    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_STRETCH_STIFF, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(0.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_STR_STIFF, pkFloatED);
    
    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_DAMPING_COEF, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(0.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_DAMPING_COEF, pkFloatED);
    
    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_FRICTION, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(0.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_FRICTION, pkFloatED);
    
    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_PRESSURE_VAL, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(1.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_PRESSURE_VAL, pkFloatED);
    
    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_TEAR_FACTOR, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(1.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_TEAR_FACTOR, pkFloatED);

    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_COL_RESPONSE, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(1.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_COLLIDE_RESP, pkFloatED);

    if (pMaxNode->GetUserPropFloat(PHYSX_CLOTH_ATTACH_RESP, fValue))
    {
        pkFloatED = NiNew NiFloatExtraData(fValue);
    }
    else
    {
        pkFloatED = NiNew NiFloatExtraData(1.0f);
    }
    pkNode->AddExtraData(ED_CLOTH_ATTACH_RESP, pkFloatED);

    if (pMaxNode->GetUserPropInt(PHYSX_CLOTH_SOLVER_ITER, iValue))
    {
        pkIntED = NiNew NiIntegerExtraData(iValue);
    }
    else
    {
        pkIntED = NiNew NiIntegerExtraData(1);
    }
    pkNode->AddExtraData(ED_SOLVER_ITERATIONS, pkIntED);    

    if (pMaxNode->GetUserPropBool(PHYSX_CLOTH_AUTOATTACH, bValue))
    {
        int aiAttach[2];
        aiAttach[0] = -1;
        aiAttach[1] = 0;
        pkIntsED = NiNew NiIntegersExtraData(2, aiAttach);
        pkNode->AddExtraData(ED_CLOTH_ATTACHMENT, pkIntsED);
    }

    // We put collision group information into "NiPhysXActorGroup"
    int iGroup = 0;
    if (pMaxNode->GetUserPropInt(PHYSX_ACTOR_GROUP, iGroup))
    {
        NiIntegerExtraData* pkPhysXGroupED = NiNew NiIntegerExtraData(iGroup);
        pkNode->AddExtraData(ED_ACTOR_GROUP, pkPhysXGroupED);    
    }

    // We put cloth info, if present, into "NiPhysXClothUpdateNBT"
    TSTR pcPropBuffer;
    pMaxNode->GetUserPropBuffer(pcPropBuffer);
    if (strstr(pcPropBuffer, PHYSX_CLOTH_UPDATE_NBT))
    {
        NiBooleanExtraData* pkClothNBTED = NiNew NiBooleanExtraData(true);
        pkNode->AddExtraData(ED_CLOTH_UPDATE_NBT, pkClothNBTED);    
    }
    
    return W3D_STAT_OK;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertGroupMember(Object* pObj, INode* pMaxNode,
    NiNode* pkNode, TimeValue fTime, ScaleInfo *pkParentScale)
{
    // A group member can only have mass and density, because it is a shape,
    // not an actor.
    
    NiIntegerExtraData* pkPhysXTypeED =
        NiNew NiIntegerExtraData(PHYSX_ACTOR_UNDEF);
    pkNode->AddExtraData(ED_ACTOR_TYPE, pkPhysXTypeED);

    // PhysX puts actor/shape density in "Density"
    float fDensity = 0.0;
    if (pMaxNode->GetUserPropFloat(PHYSX_DENSITY, fDensity))
    {
        NiFloatExtraData* pkPhysXDensityED = NiNew NiFloatExtraData(fDensity);
        pkNode->AddExtraData(ED_DENSITY, pkPhysXDensityED);
    }

    // PhysX puts actor/shape mass in "Mass"
    float fMass = 0.0;
    if (pMaxNode->GetUserPropFloat(PHYSX_MASS, fMass))
    {
        NiFloatExtraData* pkPhysXMassED = NiNew NiFloatExtraData(fMass);
        pkNode->AddExtraData(ED_MASS, pkPhysXMassED);    
    }

    return ConvertShape(pObj, pMaxNode, pkNode, fTime, pkParentScale);    
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertShape(Object* pObj, INode* pMaxNode,
    NiNode* pkNode, TimeValue fTime, ScaleInfo *pkParentScale)
{
    ConvertMaterial(pMaxNode, pkNode, fTime);
    
    // We put collision group information into "NiPhysXShapeGroup"
    int iGroup = 0;
    if (pMaxNode->GetUserPropInt(PHYSX_SHAPE_GROUP, iGroup))
    {
        NiIntegerExtraData* pkPhysXGroupED = NiNew NiIntegerExtraData(iGroup);
        pkNode->AddExtraData(ED_SHAPE_GROUP, pkPhysXGroupED);    
    }
    
    // IsConcave
    int iPhysXConcave;
    bool bExportMesh = false;
    if (pMaxNode->GetUserPropInt(PHYSX_IS_CONCAVE, iPhysXConcave))
    {
        if (iPhysXConcave)
            bExportMesh = true;
    }
    
    // Deal with the scale. Non-uniform causes us to export as a mesh.
    // Flips we can handle by sign changes on certain parameters.
    Quat q;
    Point3 p;
    ScaleInfo scaleInfo;
    NiMAXHierConverter::GetOOTM(pMaxNode, &p, &q, pkParentScale, &scaleInfo);
    bool bFlipX = false;
    bool bFlipY = false;
    bool bFlipZ = false;
    bool bUniform = false;
    ProcessScale(&scaleInfo, bFlipX, bFlipY, bFlipZ, bUniform);

    // Need to store base class of geometry, and associated data
    // because Gamebryo throws it away.
    // 1 -> Sphere
    // 2 -> Box
    // 3 -> Capsule
    // 4 -> Mesh
    // 5 -> Convex
    Class_ID CAPS_CLASS_ID(0x6d3d77ac, 0x79c939a9);  // not in any header file
    Class_ID AGGHULL_CLASS_ID(145927,562814); // From PhysX max plugin
    if (bUniform && pObj->ClassID() == Class_ID(SPHERE_CLASS_ID, 0))
    {
        SimpleObject* so = (SimpleObject*) pObj;
        float fRadius;
        Interval intForever = FOREVER;
        so->pblock->GetValue(SPHERE_RADIUS, 0, fRadius, intForever);

        // Look for the single child that is the geometry and attach the
        // shape information to it.
        unsigned int uiShapeID = ms_uiNextShapeID++;
        unsigned int uiNumChildren = 0;
        NiIntegerExtraData* pkChildED;
        for (unsigned int uj = 0; uj < pkNode->GetArrayCount(); uj++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uj);
            if (!pkChild)
                continue;
            if (!NiIsExactKindOf(NiMesh, pkChild))
                continue;

            pkChildED = (NiIntegerExtraData*)
                pkChild->GetExtraData(ED_SHAPE_BASIC_CHILD);
            if (pkChildED)
            {
                // This only happens with instanced geometry, where
                // we have the original node's extra data in here (it
                // was cloned to make this node).
                pkChildED->SetValue(uiShapeID);
            }
            else
            {
                pkChildED = NiNew NiIntegerExtraData(uiShapeID);
                pkChild->AddExtraData(ED_SHAPE_BASIC_CHILD, pkChildED);

                NiFloatExtraData* pkRadED = NiNew NiFloatExtraData(fRadius);
                pkChild->AddExtraData(ED_SHAPE_RAD, pkRadED);
            }
            uiNumChildren++;        
        }
        if (uiNumChildren == 1)
        {
            NiIntegerExtraData* pkTypeED =
                NiNew NiIntegerExtraData(PHYSX_SHAPE_SPHERE);
            pkNode->AddExtraData(ED_SHAPE_TYPE, pkTypeED);
            
            NiIntegerExtraData* pkParentED =
                NiNew NiIntegerExtraData(uiShapeID);
            pkNode->AddExtraData(ED_SHAPE_BASIC_PARENT, pkParentED);
        }
        else
        {
            NIASSERT(false && "Contact Gamebryo support if you hit this.\n");
        }
    }
    else if (bUniform && pObj->ClassID() == Class_ID(BOXOBJ_CLASS_ID, 0))
    {
        SimpleObject* so = (SimpleObject*) pObj;
        float afDim[3];
        Interval intForever = FOREVER;
        so->pblock->GetValue(BOXOBJ_WIDTH , 0, afDim[0], intForever);
        so->pblock->GetValue(BOXOBJ_LENGTH, 0, afDim[1], intForever);
        so->pblock->GetValue(BOXOBJ_HEIGHT, 0, afDim[2], intForever);

        if (bFlipX)
            afDim[0] = -afDim[0];
        if (bFlipY)
            afDim[1] = -afDim[1];
        if (bFlipZ)
            afDim[2] = -afDim[2];

        // Look for the single child that is the geometry and attach the
        // shape information to it.
        unsigned int uiShapeID = ms_uiNextShapeID++;
        unsigned int uiNumChildren = 0;
        NiIntegerExtraData* pkChildED;
        for (unsigned int uj = 0; uj < pkNode->GetArrayCount(); uj++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uj);
            if (!pkChild)
                continue;
            if (!NiIsExactKindOf(NiMesh, pkChild))
                continue;

            pkChildED = (NiIntegerExtraData*)
                pkChild->GetExtraData(ED_SHAPE_BASIC_CHILD);
            if (pkChildED)
            {
                // This only happens with instanced geometry, where
                // we have the original node's extra data in here (it
                // was cloned to make this node).
                pkChildED->SetValue(uiShapeID);
            }
            else
            {
                pkChildED = NiNew NiIntegerExtraData(uiShapeID);
                pkChild->AddExtraData(ED_SHAPE_BASIC_CHILD, pkChildED);

                // Add the extra data
                NiFloatsExtraData* pkDimED = NiNew NiFloatsExtraData(3, afDim);
                pkChild->AddExtraData(ED_SHAPE_DIM, pkDimED);
                
                NiBooleanExtraData* pkOffsetED =
                    NiNew NiBooleanExtraData(true);
                pkChild->AddExtraData(ED_SHAPE_OFFSET, pkOffsetED);
            }
            uiNumChildren++;        
        }
        if (uiNumChildren == 1)
        {
            NiIntegerExtraData* pkTypeED =
                NiNew NiIntegerExtraData(PHYSX_SHAPE_BOX);
            pkNode->AddExtraData(ED_SHAPE_TYPE, pkTypeED);
            
            NiIntegerExtraData* pkParentED =
                NiNew NiIntegerExtraData(uiShapeID);
            pkNode->AddExtraData(ED_SHAPE_BASIC_PARENT, pkParentED);
        }
        else
        {
            NIASSERT(false && "Contact Gamebryo support if you hit this.\n");
        }
    }
    else if (bUniform && pObj->ClassID() == CAPS_CLASS_ID)
    {
        int iCapsRadiusInd = 0; // these are the ParamBlock indexes
        int iCapsHeightInd = 1; // copied from PhysX plugin
        int iCapsCenterInd = 2;

        SimpleObject* so = (SimpleObject*) pObj;
        float fCapsRadius;
        float fCapsHeight;
        int iCentersFlag;
        Interval intForever = FOREVER;
        so->pblock->GetValue(iCapsRadiusInd, 0, fCapsRadius, intForever);
        so->pblock->GetValue(iCapsHeightInd, 0, fCapsHeight, intForever);
        so->pblock->GetValue(iCapsCenterInd, 0, iCentersFlag, intForever);
        if (!iCentersFlag)
        {
            if (fCapsHeight > 0)
            {
                fCapsHeight -= fCapsRadius * 2.0f;
            }
            else
            {
                fCapsHeight += fCapsRadius * 2.0f;
            }
        }
        if (bFlipY)
            fCapsHeight = -fCapsHeight;
        
        // Look for the single child that is the geometry and attach the
        // shape information to it.
        unsigned int uiShapeID = ms_uiNextShapeID++;
        unsigned int uiNumChildren = 0;
        NiIntegerExtraData* pkChildED;
        for (unsigned int uj = 0; uj < pkNode->GetArrayCount(); uj++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uj);
            if (!pkChild)
                continue;
            if (!NiIsExactKindOf(NiMesh, pkChild))
                continue;

            pkChildED = (NiIntegerExtraData*)
                pkChild->GetExtraData(ED_SHAPE_BASIC_CHILD);
            if (pkChildED)
            {
                // This only happens with instanced geometry, where
                // we have the original node's extra data in here (it
                // was cloned to make this node).
                pkChildED->SetValue(uiShapeID);
            }
            else
            {
                pkChildED = NiNew NiIntegerExtraData(uiShapeID);
                pkChild->AddExtraData(ED_SHAPE_BASIC_CHILD, pkChildED);
        
                // Add the extra data
                NiFloatExtraData* pkRadED =
                    NiNew NiFloatExtraData(fCapsRadius);
                pkChild->AddExtraData(ED_SHAPE_RAD, pkRadED);
                
                NiFloatExtraData* pkHeightED =
                    NiNew NiFloatExtraData(fCapsHeight);
                pkChild->AddExtraData(ED_SHAPE_HEIGHT, pkHeightED);
                
                NiBooleanExtraData* pkOffsetED =
                    NiNew NiBooleanExtraData(true);
                pkChild->AddExtraData(ED_SHAPE_OFFSET, pkOffsetED);
            }
            uiNumChildren++;
        }
        if (uiNumChildren == 1)
        {
            NiIntegerExtraData* pkTypeED =
                NiNew NiIntegerExtraData(PHYSX_SHAPE_CAPSULE);
            pkNode->AddExtraData(ED_SHAPE_TYPE, pkTypeED);
            
            NiIntegerExtraData* pkParentED =
                NiNew NiIntegerExtraData(uiShapeID);
            pkNode->AddExtraData(ED_SHAPE_BASIC_PARENT, pkParentED);
        }
        else
        {
            NIASSERT(false && "Contact Gamebryo support if you hit this.\n");
        }
    }
    else
    {
        int iShapeType = bExportMesh ? PHYSX_SHAPE_MESH : PHYSX_SHAPE_CONVEX;
        NiIntegerExtraData* pkTypeED = NiNew NiIntegerExtraData(iShapeType);
        pkNode->AddExtraData(ED_SHAPE_TYPE, pkTypeED);

        // We need to tell the extractor where the mesh data is
        unsigned int uiMeshID = ms_uiNextShapeID++;
        unsigned int uiNumChildren = 0;

        for (unsigned int uj = 0; uj < pkNode->GetArrayCount(); uj++)
        {
            NiAVObject* pkGeomNode = pkNode->GetAt(uj);
            if (!pkGeomNode)
                continue;
            
            NiIntegerExtraData* pkMeshChildED;
            if (!strcmp(gpcMultiMtlName, pkGeomNode->GetName()))
            {
                // Work through children and add to group
                NIASSERT(NiIsKindOf(NiNode, pkGeomNode));
                NiNode* pkMultiNode = (NiNode*)pkGeomNode;
                for (unsigned int ui = 0;
                    ui < pkMultiNode->GetChildCount(); ui++)
                {
                    NiAVObject* pkChild = pkMultiNode->GetAt(ui);
                    if (pkChild)
                    {
                        if (!NiIsExactKindOf(NiMesh, pkChild))
                            continue;

                        pkMeshChildED = (NiIntegerExtraData*)
                            pkChild->GetExtraData(ED_SHAPE_MESH_CHILD);
                        if (pkMeshChildED)
                        {
                            // This only happens with instanced geometry, where
                            // we have the original node's extra data in here
                            // (it was cloned to make this node).
                            pkMeshChildED->SetValue(uiMeshID);
                        }
                        else
                        {
                            pkMeshChildED = NiNew NiIntegerExtraData(uiMeshID);
                            pkChild->AddExtraData(ED_SHAPE_MESH_CHILD,
                                pkMeshChildED);
                        }
                        uiNumChildren++;
                    }
                }
            }
            else
            {
                if (!NiIsExactKindOf(NiMesh, pkGeomNode))
                    continue;
                
                pkMeshChildED = (NiIntegerExtraData*)
                    pkGeomNode->GetExtraData(ED_SHAPE_MESH_CHILD);
                if (pkMeshChildED)
                {
                    // This only happens with instanced geometry, where
                    // we have the original node's extra data in here
                    // (it was cloned to make this node).
                    pkMeshChildED->SetValue(uiMeshID);
                }
                else
                {
                    pkMeshChildED = NiNew NiIntegerExtraData(uiMeshID);
                    pkGeomNode->
                        AddExtraData(ED_SHAPE_MESH_CHILD, pkMeshChildED);
                }
                uiNumChildren++;
            }
        }

        if (uiNumChildren)
        {
            NiIntegerExtraData* pkMeshParentED =
                NiNew NiIntegerExtraData(uiMeshID);
            pkNode->AddExtraData(ED_SHAPE_MESH_PARENT, pkMeshParentED);
        }
    }

    // There are some properties stored in the PhysX plugin and
    // available through the Function Publishing interface. But to use
    // them we would need to include the header for the PhysX plugin.
    // The only relevant one right now seems to be solveriterationcount.

    return 0;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertMaterial(INode* pMaxNode, NiNode* pkNode,
    TimeValue fTime)
{
    int iMaterialType = 2;
    pMaxNode->GetUserPropInt(PHYSX_MATERIALTYPE, iMaterialType);
    
    NiFloatExtraData* pkFloatED;

    float fValue;
    if (iMaterialType == PHYSX_MATERIALTYPE_3DS)
    {
        // Look for the dynamic properties of the standard material
        Mtl *kMaxMaterial = pMaxNode->GetMtl();
        if (kMaxMaterial)
        {
            fValue = kMaxMaterial->GetDynamicsProperty(
                fTime, 0, DYN_STATIC_FRICTION);
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_STATIC_FRICTION, pkFloatED);

            fValue = kMaxMaterial->GetDynamicsProperty(
                fTime, 0, DYN_SLIDING_FRICTION);
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_DYNAMIC_FRICTION, pkFloatED);

            fValue = kMaxMaterial->GetDynamicsProperty(
                fTime, 0, DYN_BOUNCE);
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_RESTITUTION, pkFloatED);
        }
        iMaterialType = PHYSX_MATERIALTYPE_USER;
    }
    else if (iMaterialType == PHYSX_MATERIALTYPE_USER)
    {
        if (pMaxNode->GetUserPropFloat(PHYSX_FRICTION, fValue))
        {
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_DYNAMIC_FRICTION, pkFloatED);
        }

        if (pMaxNode->GetUserPropFloat(PHYSX_STATICFRICTION, fValue))
        {
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_STATIC_FRICTION, pkFloatED);
        }

        if (pMaxNode->GetUserPropFloat(PHYSX_RESTITUTION, fValue))
        {
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_RESTITUTION, pkFloatED);
        }
        else if (pMaxNode->GetUserPropFloat(PHYSX_ELLASTICITY, fValue))
        {
            pkFloatED = NiNew NiFloatExtraData(fValue);
            pkNode->AddExtraData(ED_RESTITUTION, pkFloatED);
        }
    }
    else
    {
        iMaterialType = PHYSX_MATERIALTYPE_DEFAULT;
    }
    
    NiIntegerExtraData* pkIntED = NiNew NiIntegerExtraData(iMaterialType);
    pkNode->AddExtraData(ED_MATERIAL_TYPE, pkIntED);
    
    return 0;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertD6Joint(Object*, INode* pMaxNode,
    TimeValue)
{
    ms_pkD6JointMap->SetAt(ms_uiNextShapeID, pMaxNode);
    ms_uiNextShapeID++;
    
    return W3D_STAT_OK;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ProcessD6Joint(unsigned int uiID, INode* pkMaxNode,
    TimeValue fTime)
{
    ObjectState kOState = pkMaxNode->EvalWorldState(fTime);
    Object* pObj = kOState.obj;

    Class_ID OLD_D6JOINT_CLASS_ID(0x5fa2e45d, 0x16e7a9e);
    bool bLegacy = pObj->ClassID() == OLD_D6JOINT_CLASS_ID ? true : false;

    SimpleObject* so = (SimpleObject*) pObj;
    NIASSERT(so->NumParamBlocks() == 4);

    IParamBlock2* pkBodySpecBlock = 0;
    IParamBlock2* pkSwingTwistBlock = 0;
    IParamBlock2* pkTranslateBlock = 0;

    // Find the 3 properties we care about and save them to the IParamBlock2's above.
    // Ignore the displayprops property.
    short checkBlocks = 0;
    char* blockName = 0;
    for (short i = 0; i < so->NumParamBlocks(); ++i)
    {
        blockName = so->GetParamBlockByID(i)->GetDesc()->int_name;
        if (strcmp(blockName, "bodyspec") == 0)
        {
            pkBodySpecBlock = so->GetParamBlockByID(i);
            NIASSERT(pkBodySpecBlock != 0);
            pkBodySpecBlock->GetDesc();
            NIASSERT(pkBodySpecBlock->NumParams() == 12);
            ++checkBlocks;
        }
        else if (strcmp(blockName, "swingtwist") == 0)
        {
            pkSwingTwistBlock = so->GetParamBlockByID(i);
            NIASSERT(pkSwingTwistBlock != 0);
            pkSwingTwistBlock->GetDesc();
            NIASSERT(pkSwingTwistBlock->NumParams() == 19);
            ++checkBlocks;
        }
        else if (strcmp(blockName, "xlate") == 0)
        {
            pkTranslateBlock = so->GetParamBlockByID(i);
            NIASSERT(pkTranslateBlock != 0);
            pkTranslateBlock->GetDesc();
            NIASSERT(pkTranslateBlock->NumParams() == 4);
            ++checkBlocks;
        }
    }
    NIASSERT(checkBlocks == 3);

    Interval kInterval = FOREVER;
    
    // Get the joint loal axis control parameter
    int iAxisControl = 0;
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(11)) == TYPE_INDEX);
    pkBodySpecBlock->GetValue(pkBodySpecBlock->IndextoID(11), fTime,
        iAxisControl, kInterval);
    if (bLegacy)
    {
        --iAxisControl;
    }
    if (iAxisControl < 0)
    {
        iAxisControl = 0;
    }

    //
    // Get the connected rigid bodies
    //
    NiNode* pkChildNode = 0;
    NiNode* pkParentNode = 0;

    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(0)) == TYPE_INODE);
    INode* pkPMaxNode;
    Matrix3 kPXForm = pkMaxNode->GetNodeTM(fTime);
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(0), fTime, pkPMaxNode, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    if (pkPMaxNode)
    {
        Matrix3 kNodeInv = pkPMaxNode->GetNodeTM(fTime);
        kNodeInv.Invert();
        kPXForm = kPXForm * kNodeInv;
        while (pkPMaxNode->IsGroupMember())
        {
            kNodeInv = pkPMaxNode->GetParentNode()->GetNodeTM(fTime);
            kNodeInv.Invert();
            kPXForm = kPXForm * pkPMaxNode->GetNodeTM(fTime) * kNodeInv;
            pkPMaxNode = pkPMaxNode->GetParentNode();
        }
        pkParentNode = NiMAXHierConverter::Lookup(pkPMaxNode, NULL);
        NIASSERT(pkParentNode);
    }
    
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(1)) == TYPE_INODE);
    INode* pkCMaxNode;
    Matrix3 kCXForm = pkMaxNode->GetNodeTM(fTime);
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(1), fTime, pkCMaxNode, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    if (pkCMaxNode)
    {
        Matrix3 kNodeInv = pkCMaxNode->GetNodeTM(fTime);
        kNodeInv.Invert();
        kCXForm = kCXForm * kNodeInv;
        while (pkCMaxNode->IsGroupMember())
        {
            kNodeInv = pkCMaxNode->GetParentNode()->GetNodeTM(fTime);
            kNodeInv.Invert();
            kCXForm = kCXForm * pkCMaxNode->GetNodeTM(fTime) * kNodeInv;
            pkCMaxNode = pkCMaxNode->GetParentNode();
        }
        pkChildNode = NiMAXHierConverter::Lookup(pkCMaxNode, NULL);
        NIASSERT(pkChildNode);
    }

    // If an actor is connected to a fixed point, then the child is always
    // the actor, while the parent is null.
    if (!pkChildNode)
    {
        if (!pkParentNode)
            return W3D_STAT_OK;
        pkChildNode = pkParentNode;
        pkCMaxNode = pkPMaxNode;
        pkParentNode = 0;
        pkPMaxNode = 0;
        Matrix3 kTempXForm = kCXForm;
        kCXForm = kPXForm;
        kPXForm = kTempXForm;
        if (iAxisControl == 1)
        {
            iAxisControl = 2;
        }
        else if (iAxisControl == 2)
        {
            iAxisControl = 1;
        }
    }

    // Mark the nodes as belonging to a joint
    if (pkParentNode)
        AppendIntExtraData(pkParentNode, ED_JOINT_PARENT, uiID);
    AppendIntExtraData(pkChildNode, ED_JOINT_CHILD, uiID);

    float afChildPosn[3];
    float afChildAxis[3];
    float afChildNormal[3];
    float afParentPosn[3];
    float afParentAxis[3];
    float afParentNormal[3];
    
    Point3 kChildNorm = kCXForm.GetRow(1).Normalize();
    Point3 kChildAxis = kCXForm.GetRow(0).Normalize();
    afChildAxis[0] = kChildAxis.x;
    afChildAxis[1] = kChildAxis.y;
    afChildAxis[2] = kChildAxis.z;
    afChildNormal[0] = kChildNorm.x;
    afChildNormal[1] = kChildNorm.y;
    afChildNormal[2] = kChildNorm.z;

    Point3 kParentNorm = kPXForm.GetRow(1).Normalize();
    Point3 kParentAxis = kPXForm.GetRow(0).Normalize();
    afParentAxis[0] = kParentAxis.x;
    afParentAxis[1] = kParentAxis.y;
    afParentAxis[2] = kParentAxis.z;
    afParentNormal[0] = kParentNorm.x;
    afParentNormal[1] = kParentNorm.y;
    afParentNormal[2] = kParentNorm.z;

    /// Value of 4 is "legacy", which is the same as 0.
    if (iAxisControl == 0 || iAxisControl > 3 ||
        (iAxisControl == 1 && !pkParentNode))
    {
        // Child joint reference frame
        Point3 kChildPosn = kCXForm.GetTrans();
        
        afChildPosn[0] = kChildPosn.x;
        afChildPosn[1] = kChildPosn.y;
        afChildPosn[2] = kChildPosn.z;

        // Parent joint reference frame
        Point3 kParentPosn = kPXForm.GetTrans();
        
        afParentPosn[0] = kParentPosn.x;
        afParentPosn[1] = kParentPosn.y;
        afParentPosn[2] = kParentPosn.z;
    }
    else if (iAxisControl == 1)
    {
        afParentPosn[0] = 0.0f;
        afParentPosn[1] = 0.0f;
        afParentPosn[2] = 0.0f;
        
        Matrix3 kNodeInv = pkCMaxNode->GetNodeTM(fTime);
        kNodeInv.Invert();
        if (pkPMaxNode)
        {
            Point3 kChildPosn =
                pkPMaxNode->GetNodeTM(fTime).GetRow(3) * kNodeInv;

            afChildPosn[0] = kChildPosn.x;
            afChildPosn[1] = kChildPosn.y;
            afChildPosn[2] = kChildPosn.z;
        }
        else
        {
            afChildPosn[0] = kNodeInv.GetRow(3).x;
            afChildPosn[1] = kNodeInv.GetRow(3).y;
            afChildPosn[2] = kNodeInv.GetRow(3).z;
        }
    }
    else if (iAxisControl == 2)
    {
        afChildPosn[0] = 0.0f;
        afChildPosn[1] = 0.0f;
        afChildPosn[2] = 0.0f;
        
        if (pkPMaxNode)
        {
            Matrix3 kNodeInv = pkPMaxNode->GetNodeTM(fTime);
            kNodeInv.Invert();
            Point3 kParentPosn =
                pkCMaxNode->GetNodeTM(fTime).GetRow(3) * kNodeInv;

            afParentPosn[0] = kParentPosn.x;
            afParentPosn[1] = kParentPosn.y;
            afParentPosn[2] = kParentPosn.z;
        }
        else
        {
            afParentPosn[0] = pkCMaxNode->GetNodeTM(fTime).GetRow(3).x;
            afParentPosn[1] = pkCMaxNode->GetNodeTM(fTime).GetRow(3).y;
            afParentPosn[2] = pkCMaxNode->GetNodeTM(fTime).GetRow(3).z;
        }
    }
    else if (iAxisControl == 3)
    {
        TSTR kString;
        if (pkMaxNode->GetUserPropString("GlobalAttachmentPoint", kString))
        {
            float afPt[3];
            StringToPoint3(kString, afPt);

            if (pkPMaxNode)
            {
                Matrix3 kNodeInv = pkPMaxNode->GetNodeTM(fTime);
                kNodeInv.Invert();
                Point3 kParentPosn = afPt * kNodeInv;
            
                afParentPosn[0] = kParentPosn.x;
                afParentPosn[1] = kParentPosn.y;
                afParentPosn[2] = kParentPosn.z;
            }
            else
            {
                afParentPosn[0] = afPt[0];
                afParentPosn[1] = afPt[1];
                afParentPosn[2] = afPt[2];
            }

            Matrix3 kNodeInv = pkCMaxNode->GetNodeTM(fTime);
            kNodeInv.Invert();
            Point3 kChildPosn = afPt * kNodeInv;

            afChildPosn[0] = kChildPosn.x;
            afChildPosn[1] = kChildPosn.y;
            afChildPosn[2] = kChildPosn.z;
        }
        else
        {
            if (pkMaxNode->GetUserPropString("LocalAttachmentPoint0", kString))
            {
                float afPt[3];
                StringToPoint3(kString, afPt);
                
                afParentPosn[0] = afPt[0];
                afParentPosn[1] = afPt[1];
                afParentPosn[2] = afPt[2];
            }
            else
            {
                afParentPosn[0] = 0.0f;
                afParentPosn[1] = 0.0f;
                afParentPosn[2] = 0.0f;
            }
            
            if (pkMaxNode->GetUserPropString("LocalAttachmentPoint1", kString))
            {
                float afPt[3];
                StringToPoint3(kString, afPt);
                
                afChildPosn[0] = afPt[0];
                afChildPosn[1] = afPt[1];
                afChildPosn[2] = afPt[2];
            }
            else
            {
                afChildPosn[0] = 0.0f;
                afChildPosn[1] = 0.0f;
                afChildPosn[2] = 0.0f;
            }
        }
    }

    AppendVectorExtraData(pkChildNode, ED_JOINT_POSN2, afChildPosn);
    AppendVectorExtraData(pkChildNode, ED_JOINT_AXIS2, afChildAxis);
    AppendVectorExtraData(pkChildNode, ED_JOINT_NORMAL2, afChildNormal);

    AppendVectorExtraData(pkChildNode, ED_JOINT_POSN1, afParentPosn);
    AppendVectorExtraData(pkChildNode, ED_JOINT_AXIS1, afParentAxis);
    AppendVectorExtraData(pkChildNode, ED_JOINT_NORMAL1, afParentNormal);

    // Mark the joint nodes as non-optimized, so that
    // its anchor location remains correct.
    NiStringExtraData* pkOptKeepED =
        (NiStringExtraData*)pkChildNode->GetExtraData("NiOptimizeKeep");
    if (!pkOptKeepED)
    {
        pkOptKeepED = NiNew NiStringExtraData("NiOptimizeKeep");
        pkChildNode->AddExtraData("NiOptimizeKeep", pkOptKeepED);
    }
    if (pkParentNode)
    {
        pkOptKeepED =
            (NiStringExtraData*)pkParentNode->GetExtraData("NiOptimizeKeep");
        if (!pkOptKeepED)
        {
            pkOptKeepED = NiNew NiStringExtraData("NiOptimizeKeep");
            pkParentNode->AddExtraData("NiOptimizeKeep", pkOptKeepED);
        }
    }

    //breakable
    NIASSERT(pkBodySpecBlock->
        GetParameterType(pkBodySpecBlock->IndextoID(2)) == TYPE_BOOL);
    BOOL bBreakable;
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(2), fTime, bBreakable, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    if (bBreakable)
    {
        NIASSERT(pkBodySpecBlock->
            GetParameterType(pkBodySpecBlock->IndextoID(3)) == TYPE_FLOAT);
        NIASSERT(pkBodySpecBlock->
            GetParameterType(pkBodySpecBlock->IndextoID(4)) == TYPE_FLOAT);
            
        float fMaxForce;
        if (!pkBodySpecBlock->GetValue(
            pkBodySpecBlock->IndextoID(3), fTime, fMaxForce, kInterval))
        {
            return W3D_STAT_FAILED;
        }
        AppendFloatExtraData(pkChildNode, ED_JOINT_MAX_FORCE, fMaxForce);

        float fMaxTorque;
        if (!pkBodySpecBlock->GetValue(
            pkBodySpecBlock->IndextoID(4), fTime, fMaxTorque, kInterval))
        {
            return W3D_STAT_FAILED;
        }
        AppendFloatExtraData(pkChildNode, ED_JOINT_MAX_TORQUE, fMaxTorque);
    }
    
    //collision
    NIASSERT(pkBodySpecBlock->
        GetParameterType(pkBodySpecBlock->IndextoID(5)) == TYPE_BOOL);
    BOOL bCollides;
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(5), fTime, bCollides, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_COLLIDES, bCollides ? 1 : 0);

    //projectionMode, "none" "point_mindist"
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(6)) == TYPE_STRING);
    // This code always returns a null string, presumably because it is a
    // drop down list of strings. So we just set the projection mode to be
    // none, pending information on how to get the string value.
    //TCHAR* kMode;
    //kMode = pkBodySpecBlock->GetStr(pkBodySpecBlock->IndextoID(6), fTime);
    //if (!kMode)
    //    AppendIntExtraData(pkChildNode, ED_JOINT_PROJ_MODE, 0);
    //else if (!NiStrnicmp((const char*)kMode, "none", 4))
    //    AppendIntExtraData(pkChildNode, ED_JOINT_PROJ_MODE, 0);
    //else if (!NiStrnicmp((const char*)kMode, "point_mindist", 13))
    //    AppendIntExtraData(pkChildNode, ED_JOINT_PROJ_MODE, 1);
    AppendIntExtraData(pkChildNode, ED_JOINT_PROJ_MODE, 0);
        
    //projectionDist
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(7)) == TYPE_FLOAT);
    float fProjDist;
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(7), fTime, fProjDist, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_PROJ_DIST, fProjDist);

    //projectionAngle
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(8)) == TYPE_FLOAT);
    float fProjAngle;
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(8), fTime, fProjAngle, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_PROJ_ANGLE, fProjAngle);

    //gearing
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(9)) == TYPE_BOOL);
    BOOL bGeared;
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(9), fTime, bGeared, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_GEARING, bGeared ? 1 : 0);

    //gearRatio
    NIASSERT(pkBodySpecBlock->GetParameterType(
        pkBodySpecBlock->IndextoID(10)) == TYPE_FLOAT);
    float fGearRatio;
    if (!pkBodySpecBlock->
        GetValue(pkBodySpecBlock->IndextoID(10), fTime, fGearRatio, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_GEAR_RATIO, fGearRatio);
    
    // Name
    AppendStringExtraData(pkChildNode, ED_JOINT_NAME, pkMaxNode->GetName());

    //
    // Joint locked state
    //
    // Translate: 1, 2, 3 -> Locked, Limited, Free (subtract 1)
    //x_state
    NIASSERT(pkTranslateBlock->GetParameterType(
        pkTranslateBlock->IndextoID(0)) == TYPE_INT);
    int iLimitState;
    if (!pkTranslateBlock->GetValue(
        pkTranslateBlock->IndextoID(0), fTime, iLimitState, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iLimitState - 1);

    NIASSERT(pkTranslateBlock->GetParameterType(
        pkTranslateBlock->IndextoID(1)) == TYPE_INT);
    if (!pkTranslateBlock->GetValue(
        pkTranslateBlock->IndextoID(1), fTime, iLimitState, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iLimitState - 1);

    NIASSERT(pkTranslateBlock->GetParameterType(
        pkTranslateBlock->IndextoID(2)) == TYPE_INT);
    if (!pkTranslateBlock->GetValue(
        pkTranslateBlock->IndextoID(2), fTime, iLimitState, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, iLimitState - 1);
    
    // Twist locked state
    NIASSERT(pkSwingTwistBlock->
        GetParameterType(pkSwingTwistBlock->IndextoID(12)) == TYPE_BOOL);
    BOOL bLimitState;
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(12), fTime, bLimitState, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    if (bLimitState)
    {
        NIASSERT(pkSwingTwistBlock->GetParameterType(
            pkSwingTwistBlock->IndextoID(18)) == TYPE_BOOL);
        if (!pkSwingTwistBlock->GetValue(
            pkSwingTwistBlock->IndextoID(18), fTime, bLimitState, kInterval))
        {
            return W3D_STAT_FAILED;
        }
        if (bLimitState)
            AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 1);
        else
            AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 2);
    }
    else
    {
        AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 0);
    }

    //swing1_limited, then if limited, locked
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(0)) == TYPE_BOOL);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(0), fTime, bLimitState, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    if (bLimitState)
    {
        NIASSERT(pkSwingTwistBlock->GetParameterType(
            pkSwingTwistBlock->IndextoID(1)) == TYPE_BOOL);
        if (!pkSwingTwistBlock->GetValue(
            pkSwingTwistBlock->IndextoID(1), fTime, bLimitState, kInterval))
        {
            return W3D_STAT_FAILED;
        }
        if (bLimitState)
            AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 0);
        else
            AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 1);
    }
    else
    {
        AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 2);
    }

    //swing2_limited, then if limited, locked
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(6)) == TYPE_BOOL);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(6), fTime, bLimitState, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    if (bLimitState)
    {
        NIASSERT(pkSwingTwistBlock->GetParameterType(
            pkSwingTwistBlock->IndextoID(7)) == TYPE_BOOL);
        if (!pkSwingTwistBlock->GetValue(
            pkSwingTwistBlock->IndextoID(7), fTime, bLimitState, kInterval))
        {
            return W3D_STAT_FAILED;
        }
        if (bLimitState)
            AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 0);
        else
            AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 1);
    }
    else
    {
        AppendIntExtraData(pkChildNode, ED_JOINT_LOCKS, 2);
    }

    //
    // Joint limits
    //
    float fLimitVal;
    //xlate_rad
    NIASSERT(pkTranslateBlock->GetParameterType(
        pkTranslateBlock->IndextoID(3)) == TYPE_FLOAT);
    if (!pkTranslateBlock->GetValue(
        pkTranslateBlock->IndextoID(3), fTime, fLimitVal, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS, fLimitVal);

    //twistlow
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(13)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(13), fTime, fLimitVal, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS,
        fLimitVal * NI_PI / 180.0f);

    //twisthigh
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(14)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(14), fTime, fLimitVal, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS,
        fLimitVal * NI_PI / 180.0f);

    //swing1_angle
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(2)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(2), fTime, fLimitVal, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS,
        fLimitVal * NI_PI / 180.0f);

    //swing2_angle
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(8)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(8), fTime, fLimitVal, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_LIMITS,
        fLimitVal * NI_PI / 180.0f);
    
    //
    // Joint restitutions
    //
    float fRestitution;
    // Linear is undefined by plug-in, so use default
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, 0.4f);
    
    // Twist low and high are the same
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(15)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(15), fTime, fRestitution, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fRestitution);
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fRestitution);
    
    //swing1_rest
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(3)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(3), fTime, fRestitution, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fRestitution);
    
    //swing2_rest
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(9)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(9), fTime, fRestitution, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_RESTITUTIONS, fRestitution);

    //
    // Joint springs
    //
    float fSpring;
    // No linear spring in plug-in, so use default
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, 20.0f);
    
    // High and low twist are the same
    //twist_spring
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(16)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(16), fTime, fSpring, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fSpring);
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fSpring);
    
    //swing1_spring
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(4)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(4), fTime, fSpring, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fSpring);
    
    //swing2_spring
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(10)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(10), fTime, fSpring, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_SPRINGS, fSpring);

    //
    // Joint restitutions
    //
    float fDamping;
    // No linear damping defined
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, 0.4f);

    // Twist is symmetric
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(17)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(17), fTime, fDamping, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fDamping);
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fDamping);
    
    //swing1_spring
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(5)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(5), fTime, fDamping, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fDamping);
    
    //swing2_spring
    NIASSERT(pkSwingTwistBlock->GetParameterType(
        pkSwingTwistBlock->IndextoID(11)) == TYPE_FLOAT);
    if (!pkSwingTwistBlock->GetValue(
        pkSwingTwistBlock->IndextoID(11), fTime, fDamping, kInterval))
    {
        return W3D_STAT_FAILED;
    }
    AppendFloatExtraData(pkChildNode, ED_JOINT_DAMPINGS, fDamping);


    pkBodySpecBlock->ReleaseDesc();
    pkSwingTwistBlock->ReleaseDesc();
    pkTranslateBlock->ReleaseDesc();

    return W3D_STAT_OK;
}
//---------------------------------------------------------------------------
int NiMAXPhysicsConverter::ConvertJoint(INode* pMaxNode, NiNode* pkNode,
    int iJointType)
{
    unsigned int uiID = ms_uiNextShapeID++;

    int iID = (int)uiID;
    NiIntegersExtraData* pkIDED =
        (NiIntegersExtraData*)pkNode->GetExtraData(ED_JOINT_CHILD);
    if (pkIDED)
    {
        // We're a clone
        pkIDED->SetValue(0, iID);
        pkNode->RemoveExtraData(ED_MAX_JOINT_POSN);
        pkNode->RemoveExtraData(ED_MAX_JOINT_AXIS);
        pkNode->RemoveExtraData(ED_MAX_JOINT_NORMAL);
        pkNode->RemoveExtraData(ED_MAX_JOINT_LOCKS);
        pkNode->RemoveExtraData(ED_MAX_JOINT_LIMITS);
    }
    else
    {
        pkIDED = NiNew NiIntegersExtraData(1, &iID);
        pkNode->AddExtraData(ED_JOINT_CHILD, pkIDED);
    }

    // Find the parent.
    INode* pMaxParent = pMaxNode->GetParentNode();
    while (pMaxParent && pMaxParent->IsGroupMember())
        pMaxParent = pMaxParent->GetParentNode();
        
    // Deal with parent
    if (pMaxParent)
    {
        ms_pkJointMap->SetAt(uiID, pMaxParent);
    }
    
    TSTR kMaxStr;
    char* pcStr;
    float afPoint3[3];
    NiFloatsExtraData* pkFloatsED;

    afPoint3[0] = 0.0f;
    afPoint3[1] = 0.0f;
    afPoint3[2] = 0.0f;
    pkFloatsED = NiNew NiFloatsExtraData(3, afPoint3);
    pkNode->AddExtraData(ED_MAX_JOINT_POSN, pkFloatsED);
    
    // Mark the joint as non-optimized, so that
    // it's anchor location remains correct.
    NiStringExtraData* pkOptKeepED =
        (NiStringExtraData*)pkNode->GetExtraData("NiOptimizeKeep");
    if (!pkOptKeepED)
    {
        pkOptKeepED = NiNew NiStringExtraData("NiOptimizeKeep");
        pkNode->AddExtraData("NiOptimizeKeep", pkOptKeepED);
    }

    if (!pMaxNode->GetUserPropString(PHYSX_JOINT_AXIS, kMaxStr))
        return 1; // Error, must be present
    pcStr = (char*)kMaxStr;
    StringToPoint3(pcStr, afPoint3);
    pkFloatsED = NiNew NiFloatsExtraData(3, afPoint3);
    pkNode->AddExtraData(ED_MAX_JOINT_AXIS, pkFloatsED);

    if (!pMaxNode->GetUserPropString(PHYSX_JOINT_NORMAL, kMaxStr))
        return 1; // Error, must be present
    pcStr = (char*)kMaxStr;
    StringToPoint3(pcStr, afPoint3);
    pkFloatsED = NiNew NiFloatsExtraData(3, afPoint3);
    pkNode->AddExtraData(ED_MAX_JOINT_NORMAL, pkFloatsED);

    // Limits
    int aiLocked[6];
    aiLocked[0] = 0;
    aiLocked[1] = 0;
    aiLocked[2] = 0;
    aiLocked[3] = 0;
    aiLocked[4] = 0;
    aiLocked[5] = 0;
    float afLimits[5];
    afLimits[0] = 0.0f;
    afLimits[1] = 0.0f;
    afLimits[2] = 0.0f;
    afLimits[3] = 0.0f;
    afLimits[4] = 0.0f;
    
    if (iJointType == PHYSX_JOINT_HINGE)
    {
        afLimits[1] = -NI_PI;
        afLimits[2] = NI_PI;
        aiLocked[3] = 2;
    }
    if (iJointType == PHYSX_JOINT_SPHERE)
    {
        aiLocked[0] = 0;
        aiLocked[1] = 0;
        aiLocked[2] = 0;
        aiLocked[3] = 2;
        aiLocked[4] = 2;
        aiLocked[5] = 2;
        afLimits[0] = 0.0f;
        afLimits[1] = -NI_PI;
        afLimits[2] = NI_PI;
        afLimits[3] = NI_PI;
        afLimits[4] = NI_PI;    
    }

    int iHasLimits;
    bool bHasLimits = pMaxNode->GetUserPropInt(PHYSX_JOINT_LIMITS, iHasLimits)
        ? true : false;
    bHasLimits = bHasLimits && iHasLimits;
    
    if (bHasLimits)
    {
        pMaxNode->GetUserPropFloat(PHYSX_JOINT_TWIST_LOW, afLimits[1]);
        pMaxNode->GetUserPropFloat(PHYSX_JOINT_TWIST_HIGH, afLimits[2]);
        afLimits[1] *= NI_PI / 180.0f;
        afLimits[2] *= NI_PI / 180.0f;
        if (afLimits[2] - afLimits[1] >= NI_TWO_PI - 0.01)
        {
            aiLocked[3] = 2;
        }
        else if (afLimits[2] > 0.01 || afLimits[1] < -0.01)
        {
            aiLocked[3] = 1;
        }
        
        if (iJointType == PHYSX_JOINT_SPHERE)
        {
            pMaxNode->GetUserPropFloat(PHYSX_JOINT_SWING_1, afLimits[3]);
            pMaxNode->GetUserPropFloat(PHYSX_JOINT_SWING_2, afLimits[4]);
            afLimits[3] *= NI_PI / 180.0f;
            afLimits[4] *= NI_PI / 180.0f;
            if (afLimits[3] >= NI_PI)
            {
                aiLocked[4] = 2;
                afLimits[3] = NI_PI;
            }
            else if (afLimits[3] > 0)
            {
                aiLocked[4] = 1;  
            }
            if (afLimits[4] >= NI_PI)
            {
                aiLocked[5] = 2;
                afLimits[4] = NI_PI;
            }
            else if (afLimits[4] > 0)
            {
                aiLocked[5] = 1;
            }
        }
    }

    pkFloatsED = NiNew NiFloatsExtraData(5, afLimits);
    pkNode->AddExtraData(ED_MAX_JOINT_LIMITS, pkFloatsED);

    NiIntegersExtraData* pkIntsED = NiNew NiIntegersExtraData(6, aiLocked);
    pkNode->AddExtraData(ED_MAX_JOINT_LOCKS, pkIntsED);

    return 0;
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::SetJointParent(INode* pMaxNode, NiNode* pkNiNode)
{
    // Look for this node as a joint parent, and tag it if so.
    NiTMapIterator iter = ms_pkJointMap->GetFirstPos();
    unsigned int uiJointID;
    NiTPrimitiveArray<unsigned int> kIdArray;
    while (iter)
    {
        INode* pkMaxParent;
        ms_pkJointMap->GetNext(iter, uiJointID, pkMaxParent);
        if (pkMaxParent == pMaxNode)
        {
            AppendIntExtraData(pkNiNode, ED_JOINT_PARENT, uiJointID);
            kIdArray.Add(uiJointID);
        }
    }
    
    for (unsigned int ui = 0; ui < kIdArray.GetSize(); ui++)
        ms_pkJointMap->RemoveAt(kIdArray.GetAt(ui));
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::AppendIntExtraData(NiNode* pkNode,
    const char* pcTag, int iVal)
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
void NiMAXPhysicsConverter::AppendVectorExtraData(NiNode* pkNode,
    const char* pcTag, float* afAddVals)
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
void NiMAXPhysicsConverter::AppendFloatExtraData(NiNode* pkNode,
    const char* pcTag, float fVal)
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
void NiMAXPhysicsConverter::AppendStringExtraData(NiNode* pkNode,
    const char* pcTag, const char* pcVal)
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
unsigned int NiMAXPhysicsConverter::GetProxyID(const char* pcName)
{
    unsigned int uiID;
    if (ms_pkProxyMap->GetAt(pcName, uiID))
        return uiID;
    
    return 0;
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::SetProxyID(const char* pcName, unsigned int uiID)
{
    ms_pkProxyMap->SetAt(pcName, uiID);
}
//---------------------------------------------------------------------------
unsigned int NiMAXPhysicsConverter::GetCompoundID(INode* pMaxNode)
{
    unsigned int uiID;
    if (ms_pkCompoundMap->GetAt(pMaxNode, uiID))
        return uiID;
    
    return 0;
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::SetCompoundID(INode* pMaxNode,
    unsigned int uiID)
{
    ms_pkCompoundMap->SetAt(pMaxNode, uiID);
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::ProcessScale(ScaleInfo *pkScale,
    bool& bFlipX, bool& bFlipY, bool& bFlipZ, bool& bUniform)
{
    bFlipX = (pkScale->scale.s.x < 0.0f);
    bFlipY = (pkScale->scale.s.y < 0.0f);
    bFlipZ = (pkScale->scale.s.z < 0.0f);

    bUniform = NiOptimize::CloseTo(
        NiAbs(pkScale->scale.s.x), NiAbs(pkScale->scale.s.y))
        && NiOptimize::CloseTo(
        NiAbs(pkScale->scale.s.x), NiAbs(pkScale->scale.s.z));
}
//---------------------------------------------------------------------------
void NiMAXPhysicsConverter::StringToPoint3(const char *pcStr, float* afPoint3)
{
    // Get rid of any extraneous characters
    char* pcCopy = (char*)NiMalloc(strlen(pcStr) + 4);
    NiStrcpy(pcCopy, strlen(pcStr) + 4, pcStr);
    for (unsigned int ui = 0; ui < strlen(pcCopy); ui++)
    {
        if (pcCopy[ui] == ',' ||
            pcCopy[ui] == '[' ||
            pcCopy[ui] == ']' ||
            pcCopy[ui] == '\r' ||
            pcCopy[ui] == '\n')
        {
            pcCopy[ui] = ' ';
        }
    }
    
    char* pcSubStr;
    char* pcContext;
    pcSubStr = NiStrtok(pcCopy, " ", &pcContext);
    afPoint3[0] = (float)atof(pcSubStr);
    pcSubStr = NiStrtok(0, " ", &pcContext);
    afPoint3[1] = (float)atof(pcSubStr);
    pcSubStr = NiStrtok(0, " ", &pcContext);
    afPoint3[2] = (float)atof(pcSubStr);
    
    NiFree(pcCopy);
}
//---------------------------------------------------------------------------
