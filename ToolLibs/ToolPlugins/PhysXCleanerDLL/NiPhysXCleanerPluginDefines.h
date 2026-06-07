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

#ifndef NIPHYSXCLEANERPLUGINDEFINES_H
#define NIPHYSXCLEANERPLUGINDEFINES_H

/// The following definitions are for constant character strings
/// used by NiPhysXExtractorPlugin, its corresponding NiPluginInfo object,
/// and its management NiDialog. Using #defines is useful in avoiding
/// string typo mistakes that can be costly to track down when debugging.


// Extra data tags
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

#endif  // #ifndef NIPHYSXCLEANERPLUGINDEFINES_H
