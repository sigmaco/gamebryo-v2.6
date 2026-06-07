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

#include "NiSceneGraphPrinter.h"
#include "NiPhysXSceneGraphPrinter.h"

bool NiPhysXSceneGraphPrinter::ms_bShowDetails = false;
unsigned int NiPhysXSceneGraphPrinter::ms_uiActorCount = 0;
unsigned int NiPhysXSceneGraphPrinter::ms_uiJointCount = 0;
unsigned int NiPhysXSceneGraphPrinter::ms_uiClothCount = 0;
unsigned int NiPhysXSceneGraphPrinter::ms_uiFluidCount = 0;

#define PRINT_INDENT(X) \
    { for (unsigned short i = 0; i < (X); i++) *pkOstr << " "; }
#define PRINT_NXVEC3(V) \
    "<" << V.x << "," << V.y << "," << V.z << ">"
#define PRINT_NXQUAT(Q) \
    "<" << Q.x << "," << Q.y << "," << Q.z << "," << Q.w << ">"

//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::PhysXPrintID(NiObject* pkObject,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PRINT_INDENT(usIndent * usTabStop);

    //
    // Print type
    //
    *pkOstr << pkObject->GetRTTI()->GetName();

    NiObjectNET *pNET = NiDynamicCast(NiObjectNET, pkObject);
    if (pNET != NULL)
    {
        //
        // Print name
        //
        *pkOstr << ":<";
        
        if (pNET->GetName())
            *pkOstr << pNET->GetName();
        
        *pkOstr << ">";
    }

    if (NiSceneGraphPrinter::ms_bShowMemoryAddress)
        *pkOstr << " <0x" << pkObject << ">";

    *pkOstr << std::endl;
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::NxMat33Print(NxMat33& kM,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    NxVec3 row;
    unsigned int i;
    for (i = 0; i < 3; i ++)
    {
        PRINT_INDENT(usIndent * usTabStop);
        row = kM.getRow(i);
        *pkOstr << "Rotate: <" << row.x << "," << row.y 
            << "," << row.z << ">" << std::endl;
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::NxMat34Print(NxMat34& kM,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    NxMat33Print(kM.M, usIndent);

    PRINT_INDENT(usIndent * usTabStop);
    *pkOstr << "Translate: " << PRINT_NXVEC3(kM.t) << std::endl; 
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::SourcePrint(NiPhysXSrc* pkSrc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkSrc, usIndent);
    
    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Active: "
            << (pkSrc->GetActive() ? "true" : "false") << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Interpolate: "
            << (pkSrc->GetInterpolate() ? "true" : "false") << std::endl;

        if (NiIsKindOf(NiPhysXRigidBodySrc, pkSrc))
        {
            NiPhysXRigidBodySrc* pkRBSrc = (NiPhysXRigidBodySrc*)pkSrc;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Source: <0x"
                << pkRBSrc->GetSource() << ">" << std::endl;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::DestinationPrint(NiPhysXDest* pkDest,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDest, usIndent);
    
    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Active: "
            << (pkDest->GetActive() ? "true" : "false") << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Interpolate: "
            << (pkDest->GetInterpolate() ? "true" : "false") << std::endl;

        if (NiIsKindOf(NiPhysXRigidBodyDest, pkDest))
        {
            NiPhysXRigidBodyDest* pkRBDest = (NiPhysXRigidBodyDest*)pkDest;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "OptimizeSleep: "
                << (pkRBDest->GetOptimizeSleep() ? "true" : "false")
                << std::endl;
            if (NiIsKindOf(NiPhysXTransformDest, pkDest))
            {
                NiPhysXTransformDest* pkTDest = (NiPhysXTransformDest*)pkDest;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "Target: <0x"
                    << pkTDest->GetTarget() << ">" << std::endl;
            }
            if (NiIsKindOf(NiPhysXPoseBufferDest, pkDest))
            {
                NiPhysXPoseBufferDest* pkIDest =
                    (NiPhysXPoseBufferDest*)pkDest;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "EvaluatorIndex: "
                    << pkIDest->GetEvaluatorIndex() << std::endl;
            }
        }
        if (NiIsKindOf(NiPhysXAccumRigidBodyDest, pkDest))
        {
            NiPhysXAccumRigidBodyDest* pkRBDest =
                (NiPhysXAccumRigidBodyDest*)pkDest;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            bool bX, bY, bZ;
            pkRBDest->GetAccumTranslation(bX, bY, bZ);
            *pkOstr << "Translation: "
                << (bX ? "X " : "  ")
                << (bY ? "Y " : "  ")
                << (bZ ? "Z" : " ")
                << std::endl;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            pkRBDest->GetAccumRotation(bX, bY, bZ);
            *pkOstr << "Translation: "
                << (bX ? "X " : "  ")
                << (bY ? "Y " : "  ")
                << (bZ ? "Z" : " ")
                << std::endl;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            unsigned char ucAxis;
            pkRBDest->GetAccumFrontFacing(ucAxis, bX);
            *pkOstr << "Front Facing: ";
            switch (ucAxis)
            {
                case 0:
                    *pkOstr << (bX ? "-" : "") << "X" << std::endl;
                    break;
                case 1:
                    *pkOstr << (bX ? "-" : "") << "Y" << std::endl;
                    break;
                case 2:
                    *pkOstr << (bX ? "-" : "") << "Z" << std::endl;
                    break;
            }
            if (NiIsKindOf(NiPhysXAccumTransformDest, pkDest))
            {
                NiPhysXAccumTransformDest* pkTDest =
                    (NiPhysXAccumTransformDest*)pkDest;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "AccumTarget: <0x"
                    << pkTDest->GetAccumTarget() << ">" << std::endl;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "NonAccumTarget: <0x"
                    << pkTDest->GetNonAccumTarget() << ">" << std::endl;
            }
            if (NiIsKindOf(NiPhysXAccumPoseBufferDest, pkDest))
            {
                NiPhysXAccumPoseBufferDest* pkIDest =
                    (NiPhysXAccumPoseBufferDest*)pkDest;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "AccumEvaluatorIndex: "
                    << pkIDest->GetAccumEvaluatorIndex() << std::endl;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "NonAccumEvaluatorIndex: "
                    << pkIDest->GetNonAccumEvaluatorIndex() << std::endl;
            }
        }
        if (NiIsKindOf(NiPhysXFluidDest, pkDest))
        {
            NiPhysXFluidDest* pkFluidDest = (NiPhysXFluidDest*)pkDest;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Target: <0x"
                << pkFluidDest->GetTarget() << ">" << std::endl;
        }
        if (NiIsKindOf(NiPhysXPSParticleSystemDest, pkDest))
        {
            NiPhysXPSParticleSystemDest* pkPSysDest =
                (NiPhysXPSParticleSystemDest*)pkDest;
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Target: <0x"
                << pkPSysDest->GetTarget() << ">" << std::endl;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::ModifierPrint(NiMesh* pkMesh,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkMesh, usIndent);

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "PhysX Modifiers :";

    bool bFound = false;
    for (NiUInt32 ui = 0; ui < pkMesh->GetModifierCount(); ui++)
    {
        NiMeshModifier* pkModifier = pkMesh->GetModifierAt(ui);
        
        if (!pkModifier)
            continue;
        
        if (NiIsKindOf(NiPhysXClothModifier, pkModifier))
        {
            if (!bFound)
                 *pkOstr << std::endl;
                 
            PhysXPrintID(pkModifier, usIndent + 2);
            
            if (ms_bShowDetails)
            {
                NiPhysXClothModifier* pkClothModifier =
                    (NiPhysXClothModifier*)pkModifier;
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "MeshGrowFactor: " <<
                    pkClothModifier->GetMeshGrowFactor() << std::endl;
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "Update Normals: "
                    << (pkClothModifier->GetUpdateNormals() ? "true" : "false")
                    << std::endl;
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "TearableHint: "
                    << (pkClothModifier->GetTearableHint() ? "true" : "false")
                    << std::endl;
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "DoSleepXforms: "
                    << (pkClothModifier->GetDoSleepXforms() ? "true" : "false")
                    << std::endl;
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "NBT Set: " << pkClothModifier->GetNBTSet()
                    << std::endl;
                
            }
            
            bFound = true;
        }
    }

    if (!bFound)
        *pkOstr << "None" << std::endl;
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::ShapePrint(NiPhysXShapeDesc* pkDesc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        switch (pkDesc->GetType())
        {
            case NX_SHAPE_PLANE: {
                NxPlaneShapeDesc kPlane;
                pkDesc->ToPlaneDesc(kPlane);
                *pkOstr << "Plane: Normal " << PRINT_NXVEC3(kPlane.normal)
                    << " Distance: " << kPlane.d << std::endl;
                } break;

            case NX_SHAPE_SPHERE: {
                NxSphereShapeDesc kSphere;
                pkDesc->ToSphereDesc(kSphere);
                *pkOstr << "Sphere: Radius " << kSphere.radius << std::endl;
                } break;

            case NX_SHAPE_CAPSULE: {
                NxCapsuleShapeDesc kCapsule;
                pkDesc->ToCapsuleDesc(kCapsule);
                *pkOstr << "Capsule: Radius " << kCapsule.radius
                    << " Height " << kCapsule.height
                    << " Flags ";
                if (kCapsule.flags & NX_SWEPT_SHAPE)
                    *pkOstr << "NX_SWEPT_SHAPE" << std::endl;
                else
                    *pkOstr << "None" << std::endl;
                } break;

            case NX_SHAPE_BOX: {
                NxBoxShapeDesc kBox;
                pkDesc->ToBoxDesc(kBox);
                *pkOstr << "Box: Dimensions " << PRINT_NXVEC3(kBox.dimensions)
                    << std::endl;
                } break;
            
            case NX_SHAPE_CONVEX:
            case NX_SHAPE_MESH: {
                NiPhysXMeshDescPtr spMeshDesc = pkDesc->GetMeshDesc();
                if (spMeshDesc->GetIsConvex())
                    *pkOstr << "Convex:" << std::endl;
                else
                    *pkOstr << "Triangle Mesh:" << std::endl;
                PRINT_INDENT((usIndent + 2) * usTabStop);
                *pkOstr << "Name <";
                if (spMeshDesc->GetName())
                {
                    *pkOstr << "\"" << spMeshDesc->GetName() << "\">"
                        << std::endl;
                }
                else
                {
                    *pkOstr << "NULL>" << std::endl;
                }
                PRINT_INDENT((usIndent + 2) * usTabStop);
                unsigned int uiMeshSize;
                unsigned char* pucMeshData;
                spMeshDesc->GetData(uiMeshSize, &pucMeshData);
                if (uiMeshSize)
                {
                    *pkOstr << "Mesh Data: 0x" << (void*)pucMeshData
                        << " Size " << uiMeshSize << std::endl;
                }
                else
                {
                    *pkOstr << "No mesh data" << std::endl;
                }
                PRINT_INDENT((usIndent + 2) * usTabStop);
                *pkOstr << "Platform: ";
                NxPlatform kPlatform;
                if (spMeshDesc->GetPlatform(kPlatform))
                {
                    switch (kPlatform)
                    {
                        case PLATFORM_PC:
                            *pkOstr << "PLATFORM_PC" << std::endl;
                            break;
                        case PLATFORM_PLAYSTATION3:
                            *pkOstr << "PLATFORM_PLAYSTATION3" << std::endl;
                            break;
                        case PLATFORM_XENON:
                            *pkOstr << "PLATFORM_XENON" << std::endl;
                            break;
                    }
                }
                else
                {
                    *pkOstr << "UNKNOWN" << std::endl;
                }
                PRINT_INDENT((usIndent + 2) * usTabStop);
                NxU32 uiFlags = spMeshDesc->GetFlags();
                *pkOstr << "Flags: ";
                if (uiFlags & NX_MESH_SMOOTH_SPHERE_COLLISIONS)
                    *pkOstr << "NX_MESH_SMOOTH_SPHERE_COLLISIONS ";
                if (uiFlags & NX_MESH_DOUBLE_SIDED)
                    *pkOstr << "NX_MESH_DOUBLE_SIDED ";
                *pkOstr << std::endl;
                PRINT_INDENT((usIndent + 1) * usTabStop);
                *pkOstr << "Mesh Paging Mode: ";
                switch (spMeshDesc->GetMeshPagingMode())
                {
                    case NX_MESH_PAGING_MANUAL:
                        *pkOstr << "NX_MESH_PAGING_MANUAL" << std::endl;
                        break;
                    case NX_MESH_PAGING_FALLBACK:
                        *pkOstr << "NX_MESH_PAGING_FALLBACK" << std::endl;
                        break;
                    case NX_MESH_PAGING_AUTO:
                        *pkOstr << "NX_MESH_PAGING_AUTO" << std::endl;
                        break;
                    default:
                        *pkOstr << "Invalid" << std::endl;
                        break;
                }
                } break;
            
            default:;
        }

        NxPlaneShapeDesc kPlane;
        pkDesc->ToShapeDesc(kPlane);
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Local Pose:" << std::endl;
        NxMat34Print(kPlane.localPose, usIndent + 2);

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Shape Flags: ";
        if (kPlane.shapeFlags & NX_TRIGGER_ON_ENTER)
            *pkOstr << "NX_TRIGGER_ON_ENTER ";
        if (kPlane.shapeFlags & NX_TRIGGER_ON_LEAVE)
            *pkOstr << "NX_TRIGGER_ON_LEAVE ";
        if (kPlane.shapeFlags & NX_TRIGGER_ON_STAY)
            *pkOstr << "NX_TRIGGER_ON_STAY ";
        if (kPlane.shapeFlags & NX_TRIGGER_ENABLE)
            *pkOstr << "NX_TRIGGER_ENABLE ";
        if (kPlane.shapeFlags & NX_SF_VISUALIZATION)
            *pkOstr << "NX_SF_VISUALIZATION ";
        if (kPlane.shapeFlags & NX_SF_DISABLE_COLLISION)
            *pkOstr << "NX_SF_DISABLE_COLLISION ";
        if (kPlane.shapeFlags & NX_SF_FEATURE_INDICES)
            *pkOstr << "NX_SF_FEATURE_INDICES ";
        if (kPlane.shapeFlags & NX_SF_DISABLE_RAYCASTING)
            *pkOstr << "NX_SF_DISABLE_RAYCASTING ";
        if (kPlane.shapeFlags & NX_SF_POINT_CONTACT_FORCE)
            *pkOstr << "NX_SF_POINT_CONTACT_FORCE ";
        if (kPlane.shapeFlags & NX_SF_FLUID_DRAIN)
            *pkOstr << "NX_SF_FLUID_DRAIN ";
        if (kPlane.shapeFlags & NX_SF_FLUID_DISABLE_COLLISION)
            *pkOstr << "NX_SF_FLUID_DISABLE_COLLISION ";
        if (kPlane.shapeFlags & NX_SF_FLUID_TWOWAY)
            *pkOstr << "NX_SF_FLUID_TWOWAY ";
        if (kPlane.shapeFlags & NX_SF_DISABLE_RESPONSE)
            *pkOstr << "NX_SF_DISABLE_RESPONSE ";
        if (kPlane.shapeFlags & NX_SF_DYNAMIC_DYNAMIC_CCD)
            *pkOstr << "NX_SF_DYNAMIC_DYNAMIC_CCD ";
        if (kPlane.shapeFlags & NX_SF_DISABLE_SCENE_QUERIES)
            *pkOstr << "NX_SF_DISABLE_SCENE_QUERIES ";
        if (kPlane.shapeFlags & NX_SF_CLOTH_DRAIN)
            *pkOstr << "NX_SF_CLOTH_DRAIN ";
        if (kPlane.shapeFlags & NX_SF_CLOTH_DISABLE_COLLISION)
            *pkOstr << "NX_SF_CLOTH_DISABLE_COLLISION ";
        if (kPlane.shapeFlags & NX_SF_CLOTH_TWOWAY)
            *pkOstr << "NX_SF_CLOTH_TWOWAY ";
         if (kPlane.shapeFlags & NX_SF_SOFTBODY_DRAIN)
            *pkOstr << "NX_SF_SOFTBODY_DRAIN ";
        if (kPlane.shapeFlags & NX_SF_SOFTBODY_DISABLE_COLLISION)
            *pkOstr << "NX_SF_SOFTBODY_DISABLE_COLLISION ";
        if (kPlane.shapeFlags & NX_SF_SOFTBODY_TWOWAY)
            *pkOstr << "NX_SF_SOFTBODY_TWOWAY ";
        *pkOstr << std::endl;
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Shape Group: " << kPlane.group << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Material Index: " << kPlane.materialIndex << std::endl;
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Density: " << kPlane.density << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Mass: " << kPlane.mass << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Skin Width: " << kPlane.skinWidth << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        if (kPlane.name)
            *pkOstr << "Name: <\"" << kPlane.name << "\">" << std::endl;
        else
            *pkOstr << "Name: <NULL>" << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Non Interacting Compartment Type: " 
            << kPlane.nonInteractingCompartmentTypes << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Groups Mask: " << kPlane.groupsMask.bits0 << " "
            << kPlane.groupsMask.bits1 << " "
            << kPlane.groupsMask.bits2 << " "
            << kPlane.groupsMask.bits3 << std::endl;
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::ActorPrint(NiPhysXActorDesc* pkDesc,
        unsigned int uiNumStates, unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    PRINT_INDENT((usIndent + 1) * usTabStop);
    if (pkDesc->GetBodyDesc())
    {
        NiPhysXBodyDesc* pkBodyDesc = pkDesc->GetBodyDesc();
        NxBodyDesc kNxBodyDesc;
        pkBodyDesc->ToBodyDesc(kNxBodyDesc, 0, NiPhysXTypes::NXMAT33_ID);
        if (kNxBodyDesc.flags & NX_BF_KINEMATIC)
            *pkOstr << "Kinematic" << std::endl;
        else
            *pkOstr << "Dynamic" << std::endl;
        
        if (ms_bShowDetails)
        {
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Mass Local Pose:" << std::endl;
            NxMat34Print(kNxBodyDesc.massLocalPose, usIndent + 3);
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Mass Space Inertia: "
                << PRINT_NXVEC3(kNxBodyDesc.massSpaceInertia) << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Mass: " << kNxBodyDesc.mass << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Wake Up Counter: "
                << kNxBodyDesc.wakeUpCounter << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Linear Damping: "
                << kNxBodyDesc.linearDamping << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Angular Damping: "
                << kNxBodyDesc.angularDamping << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Max Angular Velocity: "
                << kNxBodyDesc.maxAngularVelocity << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "CCD Motion Threshold: "
                << kNxBodyDesc.CCDMotionThreshold << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Flags: ";
            if (kNxBodyDesc.flags & NX_BF_DISABLE_GRAVITY)
                *pkOstr << "NX_BF_DISABLE_GRAVITY ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_POS_X)
                *pkOstr << "NX_BF_FROZEN_POS_Y ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_POS_Y)
                *pkOstr << "NX_BF_FROZEN_POS_X ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_POS_Z)
                *pkOstr << "NX_BF_FROZEN_POS_Z ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_ROT_X)
                *pkOstr << "NX_BF_FROZEN_ROT_X ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_ROT_Y)
                *pkOstr << "NX_BF_FROZEN_ROT_Y ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_ROT_Z)
                *pkOstr << "NX_BF_FROZEN_ROT_Z ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_POS)
                *pkOstr << "NX_BF_FROZEN_POS ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN_ROT)
                *pkOstr << "NX_BF_FROZEN_ROT ";
            if (kNxBodyDesc.flags & NX_BF_FROZEN)
                *pkOstr << "NX_BF_FROZEN ";
            if (kNxBodyDesc.flags & NX_BF_KINEMATIC)
                *pkOstr << "NX_BF_KINEMATIC ";
            if (kNxBodyDesc.flags & NX_BF_VISUALIZATION)
                *pkOstr << "NX_BF_VISUALIZATION ";
            if (kNxBodyDesc.flags & NX_BF_FILTER_SLEEP_VEL)
                *pkOstr << "NX_BF_FILTER_SLEEP_VEL ";
#if NX_SUPPORT_ENERGY_SLEEPING
            if (kNxBodyDesc.flags & NX_BF_ENERGY_SLEEP_TEST)
                *pkOstr << "NX_BF_ENERGY_SLEEP_TEST ";
#endif
            *pkOstr << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Sleep Linear Velocity: "
                << kNxBodyDesc.sleepLinearVelocity << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Sleep Angular Velocity: "
                << kNxBodyDesc.sleepAngularVelocity << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Solver Iteration Count: "
                << kNxBodyDesc.solverIterationCount << std::endl;
#if NX_SUPPORT_ENERGY_SLEEPING
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Sleep Energy Threshold: "
                << kNxBodyDesc.sleepEnergyThreshold << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Sleep Damping: "
                << kNxBodyDesc.sleepDamping << std::endl;
#endif
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Contact Report Threshold: "
                << kNxBodyDesc.contactReportThreshold << std::endl;

            for (unsigned int ui = 0; ui < uiNumStates; ui++)
            {
                NxVec3 kLinVel;
                NxVec3 kAngVel;
                pkBodyDesc->GetVelocities(kLinVel, kAngVel, ui);
                PRINT_INDENT((usIndent + 2) * usTabStop);
                *pkOstr << "Linear Velocity " << ui << ": "
                    << PRINT_NXVEC3(kLinVel) << std::endl;
                PRINT_INDENT((usIndent + 2) * usTabStop);
                *pkOstr << "Angular Velocity " << ui << ": "
                    << PRINT_NXVEC3(kAngVel) << std::endl;
            }
        }
    }
    else
    {
        *pkOstr << "Static" << std::endl;
    }

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Actor Parent: ";
        if (pkDesc->GetActorParent())
            *pkOstr << "<0x" << pkDesc->GetActorParent() << ">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Source: ";
        if (pkDesc->GetSource())
            *pkOstr << "<0x" << pkDesc->GetSource() << ">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Destination: ";
        if (pkDesc->GetDest())
            *pkOstr << "<0x" << pkDesc->GetDest() << ">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Name: ";
        NiFixedString kActorName = pkDesc->GetActorName();
        if (kActorName)
            *pkOstr << "<\"" << kActorName << "\">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Density: " << pkDesc->GetDensity() << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Flags: ";
        NxU32 uiFlags = pkDesc->GetActorFlags();
        if (uiFlags & NX_AF_DISABLE_COLLISION)
            *pkOstr << "NX_AF_DISABLE_COLLISION ";
        if (uiFlags & NX_AF_DISABLE_RESPONSE)
            *pkOstr << "NX_AF_DISABLE_RESPONSE ";
        if (uiFlags & NX_AF_LOCK_COM)
            *pkOstr << "NX_AF_LOCK_COM ";
        if (uiFlags & NX_AF_FLUID_DISABLE_COLLISION)
            *pkOstr << "NX_AF_FLUID_DISABLE_COLLISION ";
        if (uiFlags & NX_AF_CONTACT_MODIFICATION)
            *pkOstr << "NX_AF_CONTACT_MODIFICATION ";
        if (uiFlags & NX_AF_FORCE_CONE_FRICTION)
            *pkOstr << "NX_AF_FORCE_CONE_FRICTION ";
        *pkOstr << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Actor Group: " << pkDesc->GetActorGroup() << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Dominance Group: " << pkDesc->GetDominanceGroup() << 
            std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Contact Report Flags: " << pkDesc->GetContactReportFlags() 
            << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Force Field Material: " << pkDesc->GetForceFieldMaterial() 
            << std::endl;

        for (unsigned int ui = 0; ui < uiNumStates; ui++)
        {
            NxMat34 kPose = pkDesc->GetPose(ui);
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Pose " << ui << ":" << std::endl;
            NxMat34Print(kPose, usIndent + 2);
        }
       
    }

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Shapes:" << std::endl;
    NiTObjectArray<NiPhysXShapeDescPtr>& kShapes = pkDesc->GetActorShapes();
    for (unsigned int i = 0; i < kShapes.GetSize(); i++)
    {
        NiPhysXShapeDescPtr spShape = kShapes.GetAt(i);
        if (!spShape)
            continue;
        ShapePrint(spShape, usIndent + 2);
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::ClothPrint(NiPhysXClothDesc* pkDesc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Name: ";
        NiFixedString kClothName = pkDesc->GetClothName();
        if (kClothName)
            *pkOstr << "<\"" << kClothName << "\">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;

        NiPhysXMeshDesc* pkMeshDesc = pkDesc->GetMeshDesc();
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Mesh:" << std::endl;
        if (pkMeshDesc)
        {
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Name <";
            if (pkMeshDesc->GetName())
            {
                *pkOstr << "\"" << pkMeshDesc->GetName() << "\">"
                    << std::endl;
            }
            else
            {
                *pkOstr << "NULL>" << std::endl;
            }
            PRINT_INDENT((usIndent + 2) * usTabStop);
            unsigned int uiMeshSize;
            unsigned char* pucMeshData;
            pkMeshDesc->GetData(uiMeshSize, &pucMeshData);
            if (uiMeshSize)
            {
                *pkOstr << "Mesh Data: 0x" << (void*)pucMeshData
                    << " Size " << uiMeshSize << std::endl;
            }
            else
            {
                *pkOstr << "No mesh data" << std::endl;
            }
            PRINT_INDENT((usIndent + 2) * usTabStop);
            bool bHardware = pkMeshDesc->GetHardware();
            *pkOstr << "Hardware: " << (bHardware ? "true" : "false")
                << std::endl;
        }
        else
        {
            *pkOstr << "NULL" << std::endl;
        }

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Thickness: " << pkDesc->GetThickness() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Density: " << pkDesc->GetDensity() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "BendingStiffness: " <<
            pkDesc->GetBendingStiffness() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "StretchingStiffness: " <<
            pkDesc->GetStretchingStiffness() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "DampingCoefficient: " <<
            pkDesc->GetDampingCoefficient() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Friction: " << pkDesc->GetFriction() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Pressure: " << pkDesc->GetPressure() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "TearFactor: " << pkDesc->GetTearFactor() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "CollisionResponseCoefficient: " <<
            pkDesc->GetCollisionResponseCoefficient() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "AttachmentResponseCoefficient: " <<
            pkDesc->GetAttachmentResponseCoefficient() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "AttachmentTearFactor: " <<
            pkDesc->GetAttachmentTearFactor() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "ToFluidResponse: " << 
            pkDesc->GetToFluidResponseCoefficient() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "FromFluidResponse: " << 
            pkDesc->GetFromFluidResponseCoefficient() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "MinAdhereVelocity: " << 
            pkDesc->GetToFluidResponseCoefficient() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "RelativeGridSpacing: " << 
            pkDesc->GetRelativeGridSpacing() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "SolverIterations: " <<
            pkDesc->GetSolverIterations() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "ExternalAccelerationOut: "
            << PRINT_NXVEC3(pkDesc->GetExternalAcceleration()) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "WindAcceleration: "
            << PRINT_NXVEC3(pkDesc->GetWindAcceleration()) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "WakeUpCounter: " <<
            pkDesc->GetWakeUpCounter() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "SleepLinearVelocity: " <<
            pkDesc->GetSleepLinearVelocity() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "CollisionGroup: " <<
            pkDesc->GetCollisionGroup() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "CollisionMask: " <<
            pkDesc->GetCollisionMask().bits0 << " " <<
            pkDesc->GetCollisionMask().bits1 << " " <<
            pkDesc->GetCollisionMask().bits2 << " " <<
            pkDesc->GetCollisionMask().bits3 << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "ForceFieldMaterial: " <<
            pkDesc->GetForceFieldMaterial() << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Flags: ";
        NxU32 uiFlags = pkDesc->GetClothFlags();
        if (uiFlags & NX_CLF_PRESSURE)
            *pkOstr << "NX_CLF_PRESSURE ";
        if (uiFlags & NX_CLF_STATIC)
            *pkOstr << "NX_CLF_STATIC ";
        if (uiFlags & NX_CLF_DISABLE_COLLISION)
            *pkOstr << "NX_CLF_DISABLE_COLLISION ";
        if (uiFlags & NX_CLF_SELFCOLLISION)
            *pkOstr << "NX_CLF_SELFCOLLISION ";
        if (uiFlags & NX_CLF_VISUALIZATION)
            *pkOstr << "NX_CLF_VISUALIZATION ";
        if (uiFlags & NX_CLF_GRAVITY)
            *pkOstr << "NX_CLF_GRAVITY ";
        if (uiFlags & NX_CLF_BENDING)
            *pkOstr << "NX_CLF_BENDING ";
        if (uiFlags & NX_CLF_BENDING_ORTHO)
            *pkOstr << "NX_CLF_BENDING_ORTHO ";
        if (uiFlags & NX_CLF_DAMPING)
            *pkOstr << "NX_CLF_DAMPING ";
        if (uiFlags & NX_CLF_COLLISION_TWOWAY)
            *pkOstr << "NX_CLF_COLLISION_TWOWAY ";
        if (uiFlags & NX_CLF_TRIANGLE_COLLISION)
            *pkOstr << "NX_CLF_TRIANGLE_COLLISION ";
        if (uiFlags & NX_CLF_TEARABLE)
            *pkOstr << "NX_CLF_TEARABLE ";
        if (uiFlags & NX_CLF_HARDWARE)
            *pkOstr << "NX_CLF_HARDWARE ";
        if (uiFlags & NX_CLF_COMDAMPING)
            *pkOstr << "NX_CLF_COMDAMPING ";
        if (uiFlags & NX_CLF_VALIDBOUNDS)
            *pkOstr << "NX_CLF_VALIDBOUNDS ";
        if (uiFlags & NX_CLF_FLUID_COLLISION)
            *pkOstr << "NX_CLF_FLUID_COLLISION ";
        if (uiFlags & NX_CLF_DISABLE_DYNAMIC_CCD)
            *pkOstr << "NX_CLF_DISABLE_DYNAMIC_CCD ";
        if (uiFlags & NX_CLF_ADHERE)
            *pkOstr << "NX_CLF_ADHERE ";

        *pkOstr << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Saved States:" << std::endl;
        for (NiUInt32 ui = 0; ui < pkDesc->GetNumStates(); ui++)
        {
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "State " << ui << ":" << std::endl;

            PRINT_INDENT((usIndent + 3) * usTabStop);
            *pkOstr << "Pose:" << std::endl;
            NxMat34 kMat = pkDesc->GetGlobalPose(ui);
            NxMat34Print(kMat, usIndent + 4);

            PRINT_INDENT((usIndent + 3) * usTabStop);
            *pkOstr << "Num Vertex Positions: " <<
                pkDesc->GetVertexPositionsCount(ui) << std::endl;

            if (pkDesc->GetVertexPositionsCount(ui))
            {
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "Vertex Positions: " << std::endl;
                
                const NxVec3* pkPosns = pkDesc->GetVertexPositions(ui);
                NiUInt32 uiCount = pkDesc->GetVertexPositionsCount(ui);
                for (NiUInt32 uj = 0; uj < uiCount; uj++)
                {
                    PRINT_INDENT((usIndent + 4) * usTabStop);
                    *pkOstr << PRINT_NXVEC3(pkPosns[uj]) << std::endl;
                }
            }

            PRINT_INDENT((usIndent + 3) * usTabStop);
            *pkOstr << "Num Torn Vertices: " <<
                pkDesc->GetTornVerticesCount(ui) << std::endl;

            if (pkDesc->GetTornVerticesCount(ui))
            {
                PRINT_INDENT((usIndent + 3) * usTabStop);
                *pkOstr << "Torn Vertices: " << std::endl;
                
                const NiUInt16* puiIndices = pkDesc->GetTornVertices(ui);
                const NxVec3* pkPlanes = pkDesc->GetTornSplitPlanes(ui);
                NiUInt32 uiCount = pkDesc->GetTornVerticesCount(ui);
                for (NiUInt32 uj = 0; uj < uiCount; uj++)
                {
                    PRINT_INDENT((usIndent + 4) * usTabStop);
                    *pkOstr << "Index " << puiIndices[uj] << " Split Plane "
                    << PRINT_NXVEC3(pkPlanes[uj]) << std::endl;
                }
            }
        }

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Attachments:" << std::endl;
        unsigned int uiCount = pkDesc->GetAttachmentCount();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiPhysXClothDesc::ClothAttachment* pkAttachment =
                pkDesc->GetAttachmentAt(ui);
            if (!pkAttachment)
                continue;

            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Shape <";
            if (pkAttachment->m_spShape)
            {
                *pkOstr << "0x" << pkAttachment->m_spShape << ">"
                    << std::endl;
            }
            else
            {
                *pkOstr << "NULL>" << std::endl;
            }

            for (unsigned int uj = 0; uj < pkAttachment->m_uiVertexCount;
                uj++)
            {
                PRINT_INDENT((usIndent + 2) * usTabStop);
                *pkOstr << "Vertex Attachment " << uj <<
                    ": ID " << pkAttachment->m_puiVertexIDs[uj] << "; Loc " <<
                    PRINT_NXVEC3(pkAttachment->m_pkPositions[uj]) <<
                    "; Flags ";
                uiFlags = pkAttachment->m_puiFlags[uj];
                if (uiFlags & NX_CLOTH_ATTACHMENT_TWOWAY)
                    *pkOstr << "NX_CLOTH_ATTACHMENT_TWOWAY ";
                if (uiFlags & NX_CLOTH_ATTACHMENT_TEARABLE)
                    *pkOstr << "NX_CLOTH_ATTACHMENT_TEARABLE ";
                *pkOstr << std::endl;
            }
        }

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Parent Actor: ";
        if (pkDesc->GetParentActor())
            *pkOstr << "<0x" << pkDesc->GetParentActor() << ">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Destination: ";
        if (pkDesc->GetDest())
            *pkOstr << "<0x" << pkDesc->GetDest() << ">" << std::endl;
        else
            *pkOstr << "NULL" << std::endl;
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::JointPrint(NiPhysXJointDesc* pkDesc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    PRINT_INDENT((usIndent + 1) * usTabStop);
    if (pkDesc->GetActor(0))
    {
        *pkOstr << "Actor 0: <0x" << pkDesc->GetActor(0) << ">" << std::endl;
    }
    else
    {
        *pkOstr << "Actor 0: <NULL>" << std::endl;
    }
    PRINT_INDENT((usIndent + 1) * usTabStop);
    if (pkDesc->GetActor(1))
    {
        *pkOstr << "Actor 1: <0x" << pkDesc->GetActor(1) << ">" << std::endl;
    }
    else
    {
        *pkOstr << "Actor 1: <NULL>" << std::endl;
    }
    
    if (ms_bShowDetails)
    {
        NIASSERT(NiIsKindOf(NiPhysXD6JointDesc, pkDesc));
        
        NiPhysXD6JointDesc* pkD6Desc = (NiPhysXD6JointDesc*)pkDesc;
        NxD6JointDesc kNxDesc;
        pkD6Desc->GetD6JointDesc(kNxDesc);
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        if (kNxDesc.name)
            *pkOstr << "Name: <\"" << kNxDesc.name << "\">" << std::endl;
        else
            *pkOstr << "Name: <NULL>" << std::endl;
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Frame 0:" << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Normal: "
            << PRINT_NXVEC3(kNxDesc.localNormal[0]) << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Axis: "
            << PRINT_NXVEC3(kNxDesc.localAxis[0]) << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Anchor: "
            << PRINT_NXVEC3(kNxDesc.localAnchor[0]) << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Frame 1:" << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Normal: "
            << PRINT_NXVEC3(kNxDesc.localNormal[1]) << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Axis: "
            << PRINT_NXVEC3(kNxDesc.localAxis[1]) << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Anchor: "
            << PRINT_NXVEC3(kNxDesc.localAnchor[1]) << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Max Force: " << kNxDesc.maxForce << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Max Torque: " << kNxDesc.maxTorque << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Joint-Solving Extrapolation Factor: "
            << kNxDesc.solverExtrapolationFactor << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Acceleration Spring Used?: "
            << kNxDesc.useAccelerationSpring << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Joint Flags: ";
        if (kNxDesc.jointFlags & NX_JF_COLLISION_ENABLED)
            *pkOstr << "NX_JF_COLLISION_ENABLED ";
        if (kNxDesc.jointFlags & NX_JF_VISUALIZATION)
            *pkOstr << "NX_JF_VISUALIZATION ";
        *pkOstr << std::endl; 


        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "X Motion Limit: ";
        switch (kNxDesc.xMotion)
        {
            case NX_D6JOINT_MOTION_LOCKED:
                *pkOstr << "LOCKED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_LIMITED:
                *pkOstr << "LIMITED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_FREE:
                *pkOstr << "FREE" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Y Motion Limit: ";
        switch (kNxDesc.yMotion)
        {
            case NX_D6JOINT_MOTION_LOCKED:
                *pkOstr << "LOCKED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_LIMITED:
                *pkOstr << "LIMITED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_FREE:
                *pkOstr << "FREE" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Z Motion Limit: ";
        switch (kNxDesc.zMotion)
        {
            case NX_D6JOINT_MOTION_LOCKED:
                *pkOstr << "LOCKED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_LIMITED:
                *pkOstr << "LIMITED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_FREE:
                *pkOstr << "FREE" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Swing 1 Motion Limit: ";
        switch (kNxDesc.swing1Motion)
        {
            case NX_D6JOINT_MOTION_LOCKED:
                *pkOstr << "LOCKED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_LIMITED:
                *pkOstr << "LIMITED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_FREE:
                *pkOstr << "FREE" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Swing 2 Motion Limit: ";
        switch (kNxDesc.swing2Motion)
        {
            case NX_D6JOINT_MOTION_LOCKED:
                *pkOstr << "LOCKED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_LIMITED:
                *pkOstr << "LIMITED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_FREE:
                *pkOstr << "FREE" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Twist Motion Limit: ";
        switch (kNxDesc.twistMotion)
        {
            case NX_D6JOINT_MOTION_LOCKED:
                *pkOstr << "LOCKED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_LIMITED:
                *pkOstr << "LIMITED" << std::endl;
                break;
            case NX_D6JOINT_MOTION_FREE:
                *pkOstr << "FREE" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
                break;
        }

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Linear Limit:"
            << " Value: " << kNxDesc.linearLimit.value
            << " Restitution: " << kNxDesc.linearLimit.restitution
            << " Spring: " << kNxDesc.linearLimit.spring
            << " Damping: " << kNxDesc.linearLimit.damping << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Swing 1 Limit:"
            << " Value: " << kNxDesc.swing1Limit.value
            << " Restitution: " << kNxDesc.swing1Limit.restitution
            << " Spring: " << kNxDesc.swing1Limit.spring
            << " Damping: " << kNxDesc.swing1Limit.damping << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Swing 2 Limit:"
            << " Value: " << kNxDesc.swing2Limit.value
            << " Restitution: " << kNxDesc.swing2Limit.restitution
            << " Spring: " << kNxDesc.swing2Limit.spring
            << " Damping: " << kNxDesc.swing2Limit.damping << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Twist Limits:" << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Twist Low:"
            << " Value: " << kNxDesc.twistLimit.low.value
            << " Restitution: " << kNxDesc.twistLimit.low.restitution
            << " Spring: " << kNxDesc.twistLimit.low.spring
            << " Damping: " << kNxDesc.twistLimit.low.damping << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Twist High:"
            << " Value: " << kNxDesc.twistLimit.high.value
            << " Restitution: " << kNxDesc.twistLimit.high.restitution
            << " Spring: " << kNxDesc.twistLimit.high.spring
            << " Damping: " << kNxDesc.twistLimit.high.damping << std::endl;
     
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "X Drive: Type: ";
        if (kNxDesc.xDrive.driveType.getFlag(NX_D6JOINT_DRIVE_POSITION))
        {
            *pkOstr << "POSITION";
            *pkOstr << " Spring: " << kNxDesc.xDrive.spring
                << " Damping: " << kNxDesc.xDrive.damping
                << " Force Limit: " << kNxDesc.xDrive.forceLimit << std::endl;
        }
        else if (kNxDesc.xDrive.driveType.getFlag(NX_D6JOINT_DRIVE_VELOCITY))
        {
            *pkOstr << "VELOCITY";
            *pkOstr << " Spring: " << kNxDesc.xDrive.spring
                << " Damping: " << kNxDesc.xDrive.damping
                << " Force Limit: " << kNxDesc.xDrive.forceLimit << std::endl;
        }
        else
        {
            *pkOstr << "None" << std::endl;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Y Drive: Type: ";
        if (kNxDesc.yDrive.driveType.getFlag(NX_D6JOINT_DRIVE_POSITION))
        {
            *pkOstr << "POSITION";
            *pkOstr << " Spring: " << kNxDesc.yDrive.spring
                << " Damping: " << kNxDesc.yDrive.damping
                << " Force Limit: " << kNxDesc.yDrive.forceLimit << std::endl;
        }
        else if (kNxDesc.yDrive.driveType.getFlag(NX_D6JOINT_DRIVE_VELOCITY))
        {
            *pkOstr << "VELOCITY";
            *pkOstr << " Spring: " << kNxDesc.yDrive.spring
                << " Damping: " << kNxDesc.yDrive.damping
                << " Force Limit: " << kNxDesc.yDrive.forceLimit << std::endl;
        }
        else
        {
            *pkOstr << "None" << std::endl;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Z Drive: Type: ";
        if (kNxDesc.zDrive.driveType.getFlag(NX_D6JOINT_DRIVE_POSITION))
        {
            *pkOstr << "POSITION";
            *pkOstr << " Spring: " << kNxDesc.zDrive.spring
                << " Damping: " << kNxDesc.zDrive.damping
                << " Force Limit: " << kNxDesc.zDrive.forceLimit << std::endl;
        }
        else if (kNxDesc.zDrive.driveType.getFlag(NX_D6JOINT_DRIVE_VELOCITY))
        {
            *pkOstr << "VELOCITY";
            *pkOstr << " Spring: " << kNxDesc.zDrive.spring
                << " Damping: " << kNxDesc.zDrive.damping
                << " Force Limit: " << kNxDesc.zDrive.forceLimit << std::endl;
        }
        else
        {
            *pkOstr << "None" << std::endl;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Swing Drive: Type: ";
        if (kNxDesc.swingDrive.driveType.getFlag(NX_D6JOINT_DRIVE_POSITION))
        {
            *pkOstr << "POSITION";
            *pkOstr << " Spring: " << kNxDesc.swingDrive.spring
                << " Damping: " << kNxDesc.swingDrive.damping
                << " Force Limit: " << kNxDesc.swingDrive.forceLimit
                << std::endl;
        }
        else if
            (kNxDesc.swingDrive.driveType.getFlag(NX_D6JOINT_DRIVE_VELOCITY))
        {
            *pkOstr << "VELOCITY";
            *pkOstr << " Spring: " << kNxDesc.swingDrive.spring
                << " Damping: " << kNxDesc.swingDrive.damping
                << " Force Limit: " << kNxDesc.swingDrive.forceLimit
                << std::endl;
        }
        else
        {
            *pkOstr << "None" << std::endl;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Twist Drive: Type: ";
        if (kNxDesc.twistDrive.driveType.getFlag(NX_D6JOINT_DRIVE_POSITION))
        {
            *pkOstr << "POSITION";
            *pkOstr << " Spring: " << kNxDesc.twistDrive.spring
                << " Damping: " << kNxDesc.twistDrive.damping
                << " Force Limit: " << kNxDesc.twistDrive.forceLimit
                << std::endl;
            }
        else if
            (kNxDesc.twistDrive.driveType.getFlag(NX_D6JOINT_DRIVE_VELOCITY))
        {
            *pkOstr << "VELOCITY";
            *pkOstr << " Spring: " << kNxDesc.twistDrive.spring
                << " Damping: " << kNxDesc.twistDrive.damping
                << " Force Limit: " << kNxDesc.twistDrive.forceLimit
                << std::endl;
        }
        else
        {
            *pkOstr << "None" << std::endl;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Slerp Drive: Type: ";
        if (kNxDesc.slerpDrive.driveType.getFlag(NX_D6JOINT_DRIVE_POSITION))
        {
            *pkOstr << "POSITION";
            *pkOstr << " Spring: " << kNxDesc.slerpDrive.spring
                << " Damping: " << kNxDesc.slerpDrive.damping
                << " Force Limit: " << kNxDesc.slerpDrive.forceLimit
                << std::endl;
        }
        else if
            (kNxDesc.slerpDrive.driveType.getFlag(NX_D6JOINT_DRIVE_VELOCITY))
        {
            *pkOstr << "VELOCITY";
            *pkOstr << " Spring: " << kNxDesc.slerpDrive.spring
                << " Damping: " << kNxDesc.slerpDrive.damping
                << " Force Limit: " << kNxDesc.slerpDrive.forceLimit
                << std::endl;
        }
        else
        {
            *pkOstr << "None" << std::endl;
        }
         
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Drive Position:"
            << PRINT_NXVEC3(kNxDesc.drivePosition) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Drive Orientation:"
            << PRINT_NXQUAT(kNxDesc.driveOrientation) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Drive Linear Velocity:"
            << PRINT_NXVEC3(kNxDesc.driveLinearVelocity) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Drive Angular Velocity:"
            << PRINT_NXVEC3(kNxDesc.driveAngularVelocity) << std::endl;
         
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Projection Mode: ";
        switch(kNxDesc.projectionMode)
        {
            case NX_JPM_NONE:
                *pkOstr << "NX_JPM_NONE" << std::endl;
                break;
            case NX_JPM_POINT_MINDIST:
                *pkOstr << "NX_JPM_POINT_MINDIST" << std::endl;
                break;
            case NX_JPM_LINEAR_MINDIST:
                *pkOstr << "NX_JPM_LINEAR_MINDIST" << std::endl;
                break;
            default:
                *pkOstr << "Invalid" << std::endl;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Projection Distance: "
            << kNxDesc.projectionDistance << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Projection Angle: "
            << kNxDesc.projectionAngle << std::endl;
         
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Gear Ratio: " << kNxDesc.gearRatio << std::endl;
         
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "D6 Joint Flags:";
        if (kNxDesc.flags & NX_D6JOINT_SLERP_DRIVE)
            *pkOstr << "NX_D6JOINT_SLERP_DRIVE ";
        if (kNxDesc.flags & NX_D6JOINT_GEAR_ENABLED)
            *pkOstr << "NX_D6JOINT_GEAR_ENABLED ";
        *pkOstr << std::endl; 

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Limit Point: "
            << PRINT_NXVEC3(pkDesc->GetLimitPoint()) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Limit Planes:" << std::endl;
        unsigned int uiNumLimits = pkDesc->GetNumLimits();
        for (unsigned int i = 0; i < uiNumLimits; i++)
        {
            PRINT_INDENT((usIndent + 2) * usTabStop);
            NxVec3 kNorm;
            NxReal kD;
            NxReal kR;
            pkDesc->GetLimitPlane(i, kNorm, kD, kR);
            *pkOstr << "Normal: "
                << PRINT_NXVEC3(kNorm) << " " << kD << " " << kR << std::endl;
        }
    }

}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::MaterialPrint(NiPhysXMaterialDesc* pkDesc,
    unsigned int uiNumStates, unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    for (unsigned int ui = 0; ui < uiNumStates; ui++)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "State " << ui << ":" << std::endl;
        
        NxMaterialDesc kMatDesc;
        pkDesc->ToMaterialDesc(kMatDesc, ui);

        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Dynamic Friction: "
            << kMatDesc.dynamicFriction << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Static Friction: "
            << kMatDesc.staticFriction << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Restitution: "
            << kMatDesc.restitution << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Dynamic Friction V: "
            << kMatDesc.dynamicFrictionV << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Static Friction V: "
            << kMatDesc.staticFrictionV << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Direction of Anisotropy: "
            << PRINT_NXVEC3(kMatDesc.dirOfAnisotropy) << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Flags: ";
        if (kMatDesc.flags & NX_MF_ANISOTROPIC)
            *pkOstr << "NX_MF_ANISOTROPIC ";
        if (kMatDesc.flags & NX_MF_DISABLE_FRICTION)
            *pkOstr << "NX_MF_DISABLE_FRICTION ";
        if (kMatDesc.flags & NX_MF_DISABLE_STRONG_FRICTION)
            *pkOstr << "NX_MF_DISABLE_STRONG_FRICTION ";
        *pkOstr << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Friction Combine Mode: ";
        switch (kMatDesc.frictionCombineMode)
        {
            case NX_CM_AVERAGE: *pkOstr << "NX_CM_AVERAGE" << std::endl;
                break;
            case NX_CM_MIN: *pkOstr << "NX_CM_MIN" << std::endl;
                break;
            case NX_CM_MULTIPLY: *pkOstr << "NX_CM_MULTIPLY" << std::endl;
                break;
            case NX_CM_MAX: *pkOstr << "NX_CM_MAX" << std::endl;
                break;
            default: *pkOstr << "Invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Restitution Combine Mode: ";
        switch (kMatDesc.restitutionCombineMode)
        {
            case NX_CM_AVERAGE: *pkOstr << "NX_CM_AVERAGE" << std::endl;
                break;
            case NX_CM_MIN: *pkOstr << "NX_CM_MIN" << std::endl;
                break;
            case NX_CM_MULTIPLY: *pkOstr << "NX_CM_MULTIPLY" << std::endl;
                break;
            case NX_CM_MAX: *pkOstr << "NX_CM_MAX" << std::endl;
                break;
            default: *pkOstr << "Invalid" << std::endl;
                break;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::FluidEmitterPrint(
    NiPhysXFluidEmitterDesc* pkDesc, unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);
    
    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        if (pkDesc->GetName())
            *pkOstr << "Name: <\"" << pkDesc->GetName() << "\">" << std::endl;
        else
            *pkOstr << "Name: <NULL>" << std::endl;

        NxFluidEmitterDesc kNxDesc = pkDesc->GetNxFluidEmitterDesc();
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Relative Pose:" << std::endl;
        NxMat34Print(kNxDesc.relPose, usIndent + 2);
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Type: " << kNxDesc.type << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Max particles: " << kNxDesc.maxParticles << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Shape: ";
        if (kNxDesc.shape == NX_FE_RECTANGULAR)
            *pkOstr << "Rectangular" << std::endl;
        else if (kNxDesc.shape == NX_FE_ELLIPSE)
            *pkOstr << "Elliptical" << std::endl;
        else
            *pkOstr << "Invalid" << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Dimensions: " << kNxDesc.dimensionX << " "
            << kNxDesc.dimensionY << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Random Pos: "
            << PRINT_NXVEC3(kNxDesc.randomPos) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Random Angle: " << kNxDesc.randomAngle << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Fluid Velocity Magnitude: "
            << kNxDesc.fluidVelocityMagnitude << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Rate: " << kNxDesc.rate << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Particle Lifetime: "
            << kNxDesc.particleLifetime << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Repulsion Coefficient: "
            << kNxDesc.repulsionCoefficient<< std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Flags: ";
        if (kNxDesc.flags & NX_FF_VISUALIZATION)
            *pkOstr << "NX_FF_VISUALIZATION ";
        if (kNxDesc.flags & NX_FEF_FORCE_ON_BODY)
            *pkOstr << "NX_FEF_FORCE_ON_BODY ";
        if (kNxDesc.flags & NX_FEF_ADD_BODY_VELOCITY)
            *pkOstr << "NX_FEF_ADD_BODY_VELOCITY ";
        if (kNxDesc.flags & NX_FEF_ENABLED)
            *pkOstr << "NX_FEF_ENABLED ";
        *pkOstr << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Frame Shape: <";
        if (pkDesc->GetFrameShape())
        {
            *pkOstr << "0x" << pkDesc->GetFrameShape() << ">" << std::endl;
        }
        else
        {
            *pkOstr << "NULL>" << std::endl;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::FluidPrint(NiPhysXFluidDesc* pkDesc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Particle System: <";
    if (pkDesc->GetParticleSystem())
    {
        *pkOstr << "0x" << pkDesc->GetParticleSystem() << ">" << std::endl;
    }
    else
    {
        *pkOstr << "NULL>" << std::endl;
    }

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        if (pkDesc->GetName())
            *pkOstr << "Name: <\"" << pkDesc->GetName() << "\">" << std::endl;
        else
            *pkOstr << "Name: <NULL>" << std::endl;

        NxFluidDesc kNxDesc = pkDesc->GetNxFluidDesc();
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Max Particles: " << kNxDesc.maxParticles << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Number Reserved Particles: "
            << kNxDesc.numReserveParticles << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Rest Particles per Meter: "
            << kNxDesc.restParticlesPerMeter << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Rest Density: " << kNxDesc.restDensity << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Kernel Radius Multiplier: "
            << kNxDesc.kernelRadiusMultiplier << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Motion Limit Multiplier: "
            << kNxDesc.motionLimitMultiplier << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Packet Size Multiplier: "
            << kNxDesc.packetSizeMultiplier << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Stiffness: " << kNxDesc.stiffness << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Viscosity: " << kNxDesc.viscosity << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Surface Tension: " << kNxDesc.surfaceTension << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Damping: " << kNxDesc.damping << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Fade In Time: " << kNxDesc.fadeInTime << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "External Acceleration: "
            << PRINT_NXVEC3(kNxDesc.externalAcceleration) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Projection Plane: "
            << PRINT_NXVEC3(kNxDesc.projectionPlane.normal) << ", " << 
            kNxDesc.projectionPlane.d << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Restitution For Static Shapes: "
            << kNxDesc.restitutionForStaticShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Dynamic Friction For Static Shapes: "
            << kNxDesc.dynamicFrictionForStaticShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Static Friction For Static Shapes: "
            << kNxDesc.staticFrictionForStaticShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Attraction For Static Shapes: "
            << kNxDesc.attractionForStaticShapes<< std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Restitution For Dynamic Shapes: "
            << kNxDesc.restitutionForDynamicShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Dynamic Friction For Dynamic Shapes: "
            << kNxDesc.dynamicFrictionForDynamicShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Static Friction For Dynamic Shapes: "
            << kNxDesc.staticFrictionForDynamicShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Attraction For Dynamic Shapes: "
            << kNxDesc.attractionForDynamicShapes << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Collision Response Coefficient: "
            << kNxDesc.collisionResponseCoefficient << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Collision Distance Multiplier: "
            << kNxDesc.collisionDistanceMultiplier << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Simulation Method: "
            << kNxDesc.simulationMethod << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Collision Method: "
            << kNxDesc.collisionMethod << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Force Field Material: "
            << kNxDesc.forceFieldMaterial << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Flags: ";
        if (kNxDesc.flags & NX_FF_VISUALIZATION)
            *pkOstr << "NX_FF_VISUALIZATION ";
        if (kNxDesc.flags & NX_FF_DISABLE_GRAVITY)
            *pkOstr << "NX_FF_DISABLE_GRAVITY ";
        if (kNxDesc.flags & NX_FF_COLLISION_TWOWAY)
            *pkOstr << "NX_FF_COLLISION_TWOWAY ";
        if (kNxDesc.flags & NX_FF_ENABLED)
            *pkOstr << "NX_FF_ENABLED ";
        if (kNxDesc.flags & NX_FF_HARDWARE)
            *pkOstr << "NX_FF_HARDWARE ";
        if (kNxDesc.flags & NX_FF_PRIORITY_MODE)
            *pkOstr << "NX_FF_PRIORITY_MODE ";
        if (kNxDesc.flags & NX_FF_PROJECT_TO_PLANE)
            *pkOstr << "NX_FF_PROJECT_TO_PLANE ";
        if (kNxDesc.flags & NX_FF_FORCE_STRICT_COOKING_FORMAT)
            *pkOstr << "NX_FF_FORCE_STRICT_COOKING_FORMAT ";
        *pkOstr << std::endl;
    }
    
    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Emitters:" << std::endl;
    NiTObjectArray<NiPhysXFluidEmitterDescPtr>& kEmits = pkDesc->GetEmitters();
    for (unsigned int ui = 0; ui < kEmits.GetSize(); ui++)
    {
        FluidEmitterPrint(kEmits.GetAt(ui), usIndent + 2);
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::PropDescPrint(NiPhysXPropDesc* pkDesc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    unsigned int uiNumStates = pkDesc->GetNumStates();

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Rigid Body Compartment ID: "
        << pkDesc->GetRBCompartmentID() << std::endl;

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Cloth Compartment ID: "
        << pkDesc->GetClothCompartmentID() << std::endl;

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Fluid Compartment ID: "
        << pkDesc->GetFluidCompartmentID() << std::endl;

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Actors:" << std::endl;
    for (unsigned int i = 0; i < pkDesc->GetActorCount(); i++)
    {
        ActorPrint(pkDesc->GetActorAt(i), uiNumStates, usIndent + 2);
        ms_uiActorCount++;
    }

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Joints:" << std::endl;
    for (unsigned int i = 0; i < pkDesc->GetJointCount(); i++)
    {
        JointPrint(pkDesc->GetJointAt(i), usIndent + 2);
        ms_uiJointCount++;
    }

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Clothes:" << std::endl;
    for (unsigned int i = 0; i < pkDesc->GetClothCount(); i++)
    {
        ClothPrint(pkDesc->GetClothAt(i), usIndent + 2);
        ms_uiClothCount++;
    }

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Materials:" << std::endl;
    NiTMapIterator iter = pkDesc->GetFirstMaterial();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterial;
        pkDesc->GetNextMaterial(iter, usKey, spMaterial);
        if (spMaterial)
        {
            PhysXPrintID(spMaterial, usIndent + 2);

            PRINT_INDENT((usIndent + 3) * usTabStop);
            *pkOstr << "Index: " << usKey << std::endl;
            if (ms_bShowDetails)
            {
                MaterialPrint(spMaterial, uiNumStates, usIndent + 2);
            }
        }
    }
            
    if (NiIsKindOf(NiPhysXFluidPropDesc, pkDesc))
    {
        NiPhysXFluidPropDesc* pkFluidDesc = (NiPhysXFluidPropDesc*)pkDesc;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Fluids:" << std::endl;
        for (unsigned int i = 0; i < pkFluidDesc->GetNumFluids(); i++)
        {
            FluidPrint(pkFluidDesc->GetFluidDesc(i), usIndent + 2);
            ms_uiFluidCount++;
        }
    }
    
    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Named States:" << std::endl;
    iter = pkDesc->GetFirstStateName();
    while (iter)
    {
        NiFixedString kName;
        NiUInt32 uiIndex;
        pkDesc->GetNextStateName(iter, kName, uiIndex);
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Name: " << kName << "; Index: " << uiIndex << std::endl;
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::SceneDescPrint(NiPhysXSceneDesc* pkDesc,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkDesc, usIndent);

    if (ms_bShowDetails)
    {    
        NxSceneDesc kNxDesc = pkDesc->GetNxSceneDesc();
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Gravity: " << PRINT_NXVEC3(kNxDesc.gravity) << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Max Time Step: " << kNxDesc.maxTimestep << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Max Iterations: " << kNxDesc.maxIter << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Time Step Method: ";
        switch (kNxDesc.timeStepMethod)
        {
            case NX_TIMESTEP_FIXED:
                *pkOstr << "NX_TIMESTEP_FIXED" << std::endl;
                break;
            case NX_TIMESTEP_VARIABLE:
                *pkOstr << "NX_TIMESTEP_VARIABLE" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }

        if (kNxDesc.maxBounds)
        {
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Max Bounds: "
                << "Min: " << PRINT_NXVEC3(kNxDesc.maxBounds->min) << " "
                << "Max: " << PRINT_NXVEC3(kNxDesc.maxBounds->max)
                << std::endl;
        }
        else
        {
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Max Bounds: none" << std::endl;
        }
        if (kNxDesc.limits)
        {
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Scene Limits:" << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Max Actors: "
                << kNxDesc.limits->maxNbActors << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Max Bodies: "
                << kNxDesc.limits->maxNbBodies << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Max Static Shapes: "
                << kNxDesc.limits->maxNbStaticShapes << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Max Dynamic Shapes: "
                << kNxDesc.limits->maxNbDynamicShapes << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Max Joints: "
                << kNxDesc.limits->maxNbJoints << std::endl;
        }
        else
        {
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Scene Limits: none" << std::endl;
        }

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Simulation Type: ";
        switch (kNxDesc.simType)
        {
            case NX_SIMULATION_SW:
                *pkOstr << "NX_SIMULATION_SW" << std::endl;
                break;
            case NX_SIMULATION_HW:
                *pkOstr << "NX_SIMULATION_HW" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Ground Plane: "
            << (kNxDesc.groundPlane ? "true" : "false") << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Bounds Planes: "
            << (kNxDesc.boundsPlanes ? "true" : "false") << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Flags: ";
        if (kNxDesc.flags & NX_SF_DISABLE_SSE)
            *pkOstr << "NX_SF_DISABLE_SSE ";
        if (kNxDesc.flags & NX_SF_DISABLE_COLLISIONS)
            *pkOstr << "NX_SF_DISABLE_COLLISIONS ";
        if (kNxDesc.flags & NX_SF_SIMULATE_SEPARATE_THREAD)
            *pkOstr << "NX_SF_SIMULATE_SEPARATE_THREAD ";
        if (kNxDesc.flags & NX_SF_ENABLE_MULTITHREAD)
            *pkOstr << "NX_SF_ENABLE_MULTITHREAD ";
        if (kNxDesc.flags & NX_SF_ENABLE_ACTIVETRANSFORMS)
            *pkOstr << "NX_SF_ENABLE_ACTIVETRANSFORMS ";
        if (kNxDesc.flags & NX_SF_RESTRICTED_SCENE)
            *pkOstr << "NX_SF_RESTRICTED_SCENE ";
        if (kNxDesc.flags & NX_SF_DISABLE_SCENE_MUTEX)
            *pkOstr << "NX_SF_DISABLE_SCENE_MUTEX ";
        if (kNxDesc.flags & NX_SF_FORCE_CONE_FRICTION)
            *pkOstr << "NX_SF_FORCE_CONE_FRICTION ";
        if (kNxDesc.flags & NX_SF_SEQUENTIAL_PRIMARY)
            *pkOstr << "NX_SF_SEQUENTIAL_PRIMARY ";
        if (kNxDesc.flags & NX_SF_FLUID_PERFORMANCE_HINT)
            *pkOstr << "NX_SF_FLUID_PERFORMANCE_HINT ";
        *pkOstr << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Internal Thread Count: "
            << kNxDesc.internalThreadCount << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Background Thread Count: "
            << kNxDesc.backgroundThreadCount << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Thread Mask: "
            << kNxDesc.threadMask << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Background Thread Mask: "
            << kNxDesc.backgroundThreadMask << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Background Thread Priority: ";
        switch (kNxDesc.backgroundThreadPriority)
        {
            case NX_TP_HIGH:
                *pkOstr << "NX_TP_HIGH" << std::endl;
                break;
            case NX_TP_NORMAL:
                *pkOstr << "NX_TP_NORMAL" << std::endl;
                break;
            case NX_TP_LOW:
                *pkOstr << "NX_TP_LOW" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Sim Thread Stack Size: "
            << kNxDesc.simThreadStackSize << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Sim Thread Priority: ";
        switch (kNxDesc.simThreadPriority)
        {
            case NX_TP_HIGH:
                *pkOstr << "NX_TP_HIGH" << std::endl;
                break;
            case NX_TP_NORMAL:
                *pkOstr << "NX_TP_NORMAL" << std::endl;
                break;
            case NX_TP_LOW:
                *pkOstr << "NX_TP_LOW" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Worker Thread Stack Size: "
            << kNxDesc.workerThreadStackSize << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Worker Thread Priority: ";
        switch (kNxDesc.workerThreadPriority)
        {
            case NX_TP_HIGH:
                *pkOstr << "NX_TP_HIGH" << std::endl;
                break;
            case NX_TP_NORMAL:
                *pkOstr << "NX_TP_NORMAL" << std::endl;
                break;
            case NX_TP_LOW:
                *pkOstr << "NX_TP_LOW" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Up Axis: "
            << kNxDesc.upAxis << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Subdivision Level: "
            << kNxDesc.subdivisionLevel << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Static Structure: ";
        switch (kNxDesc.staticStructure)
        {
            case NX_PRUNING_NONE:
                *pkOstr << "NX_PRUNING_NONE" << std::endl;
                break;
            case NX_PRUNING_OCTREE:
                *pkOstr << "NX_PRUNING_OCTREE" << std::endl;
                break;
            case NX_PRUNING_QUADTREE:
                *pkOstr << "NX_PRUNING_QUADTREE" << std::endl;
                break;
            case NX_PRUNING_DYNAMIC_AABB_TREE:
                *pkOstr << "NX_PRUNING_DYNAMIC_AABB_TREE" << std::endl;
                break;
            case NX_PRUNING_STATIC_AABB_TREE:
                *pkOstr << "NX_PRUNING_STATIC_AABB_TREE" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Dynamic Structure: ";
        switch (kNxDesc.dynamicStructure)
        {
            case NX_PRUNING_NONE:
                *pkOstr << "NX_PRUNING_NONE" << std::endl;
                break;
            case NX_PRUNING_OCTREE:
                *pkOstr << "NX_PRUNING_OCTREE" << std::endl;
                break;
            case NX_PRUNING_QUADTREE:
                *pkOstr << "NX_PRUNING_QUADTREE" << std::endl;
                break;
            case NX_PRUNING_DYNAMIC_AABB_TREE:
                *pkOstr << "NX_PRUNING_DYNAMIC_AABB_TREE" << std::endl;
                break;
            case NX_PRUNING_STATIC_AABB_TREE:
                *pkOstr << "NX_PRUNING_STATIC_AABB_TREE" << std::endl;
                break;
            default:
                *pkOstr << "invalid" << std::endl;
                break;
        }
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Dynamic Tree Rebuild Rate Hint: "
            << kNxDesc.dynamicTreeRebuildRateHint << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Group Collision Flags: " << std::endl;
        for (unsigned int i = 0; i < 32; i++)
        {
            PRINT_INDENT((usIndent + 2) * usTabStop);
            for (unsigned int j = 0; j < 32; j++)
                *pkOstr << (pkDesc->GetGroupCollisionFlag(
                    (unsigned char)i, (unsigned char)j) ? "1 " : "0 ");
            *pkOstr << std::endl;
        }

        NxFilterOp akFilterops[3];
        pkDesc->GetFilterOps(akFilterops[0], akFilterops[1], akFilterops[2]);
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Filter Ops: ";
        switch (akFilterops[0])
        {
            case NX_FILTEROP_AND:
                *pkOstr << "NX_FILTEROP_AND ";
                break;
            case NX_FILTEROP_OR:
                *pkOstr << "NX_FILTEROP_OR ";
                break;
            case NX_FILTEROP_XOR:
                *pkOstr << "NX_FILTEROP_XOR ";
                break;
            case NX_FILTEROP_NAND:
                *pkOstr << "NX_FILTEROP_NAND ";
                break;
            case NX_FILTEROP_NOR:
                *pkOstr << "NX_FILTEROP_NOR ";
                break;
            case NX_FILTEROP_NXOR:
                *pkOstr << "NX_FILTEROP_NXOR ";
                break;
            case NX_FILTEROP_SWAP_AND:
                *pkOstr << "NX_FILTEROP_SWAP_AND ";
                break;
        } 
        switch (akFilterops[1])
        {
            case NX_FILTEROP_AND:
                *pkOstr << "NX_FILTEROP_AND ";
                break;
            case NX_FILTEROP_OR:
                *pkOstr << "NX_FILTEROP_OR ";
                break;
            case NX_FILTEROP_XOR:
                *pkOstr << "NX_FILTEROP_XOR ";
                break;
            case NX_FILTEROP_NAND:
                *pkOstr << "NX_FILTEROP_NAND ";
                break;
            case NX_FILTEROP_NOR:
                *pkOstr << "NX_FILTEROP_NOR ";
                break;
            case NX_FILTEROP_NXOR:
                *pkOstr << "NX_FILTEROP_NXOR ";
                break;
            case NX_FILTEROP_SWAP_AND:
                *pkOstr << "NX_FILTEROP_SWAP_AND ";
                break;
        } 
        switch (akFilterops[2])
        {
            case NX_FILTEROP_AND:
                *pkOstr << "NX_FILTEROP_AND" << std::endl;
                break;
            case NX_FILTEROP_OR:
                *pkOstr << "NX_FILTEROP_OR" << std::endl;
                break;
            case NX_FILTEROP_XOR:
                *pkOstr << "NX_FILTEROP_XOR" << std::endl;
                break;
            case NX_FILTEROP_NAND:
                *pkOstr << "NX_FILTEROP_NAND" << std::endl;
                break;
            case NX_FILTEROP_NOR:
                *pkOstr << "NX_FILTEROP_NOR" << std::endl;
                break;
            case NX_FILTEROP_NXOR:
                *pkOstr << "NX_FILTEROP_NXOR" << std::endl;
                break;
            case NX_FILTEROP_SWAP_AND:
                *pkOstr << "NX_FILTEROP_SWAP_AND" << std::endl;
                break;
        } 
        NxGroupsMask kGroupMask;
        pkDesc->GetFilterConstant0(kGroupMask);
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Filter Constant 0: "
            << kGroupMask.bits0 << " " << kGroupMask.bits1 << " "
            << kGroupMask.bits2 << " " << kGroupMask.bits3 << std::endl;
        pkDesc->GetFilterConstant1(kGroupMask);
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Filter Constant 1: "
            << kGroupMask.bits0 << " " << kGroupMask.bits1 << " "
            << kGroupMask.bits2 << " " << kGroupMask.bits3 << std::endl;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Filter Bool: "
            << (pkDesc->GetFilterBool() ? "true" : "false") << std::endl;
            
        NiTMap<unsigned int, NxCompartmentDesc*>& kCompartmentMap =
            pkDesc->GetCompartmentMap();
        NiTMapIterator kIter = kCompartmentMap.GetFirstPos();
        while (kIter)
        {
            unsigned int uiID;
            NxCompartmentDesc* pkCompDesc;
            kCompartmentMap.GetNext(kIter, uiID, pkCompDesc);
            
            PRINT_INDENT((usIndent + 1) * usTabStop);
            *pkOstr << "Compartment " << uiID << ":" << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Type: ";
            switch (pkCompDesc->type)
            {
                case NX_SCT_RIGIDBODY:
                    *pkOstr << "NX_SCT_RIGIDBODY" << std::endl;
                    break;
                case NX_SCT_FLUID:
                    *pkOstr << "NX_SCT_FLUID" << std::endl;
                    break;
                case NX_SCT_CLOTH:
                    *pkOstr << "NX_SCT_CLOTH" << std::endl;
                    break;
            } 
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Device Code " << pkCompDesc->deviceCode << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Grid Hash Cell Size " <<
                pkCompDesc->gridHashCellSize << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Grid Hash Table Power " <<
                pkCompDesc->gridHashTablePower << std::endl;
            PRINT_INDENT((usIndent + 2) * usTabStop);
            *pkOstr << "Compartment Flags: ";
            if (pkCompDesc->flags & NX_CF_SLEEP_NOTIFICATION)
                    *pkOstr << "NX_CF_SLEEP_NOTIFICATION" << std::endl;
            if (pkCompDesc->flags & NX_CF_CONTINUOUS_CD)
                    *pkOstr << "NX_CF_CONTINUOUS_CD" << std::endl;
            if (pkCompDesc->flags & NX_CF_RESTRICTED_SCENE)
                    *pkOstr << "NX_CF_RESTRICTED_SCENE" << std::endl;
            if (pkCompDesc->flags & NX_CF_INHERIT_SETTINGS)
                    *pkOstr << "NX_CF_INHERIT_SETTINGS" << std::endl;
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::PSysPrint(NiPhysXPSParticleSystem* pkPSys,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkPSys, usIndent);

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Uses PhysX Space: "
            << (pkPSys->GetUsesPhysXSpace() ? "true" : "false")
            << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Uses PhysX Rotations: "
            << (pkPSys->GetUsesPhysXRotations() ? "true" : "false")
            << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Compartment ID: "
            << pkPSys->GetCompartmentID() << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Default Actor Pool Size: "
            << pkPSys->GetDefaultActorPoolSize() << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Fill Actor Pools on Load: "
            << (pkPSys->GetFillActorPoolOnLoad() ? "true" : "false")
            << std::endl;

        NiPoint3 kCenter;
        NiPoint3 kDim;
        pkPSys->GetActorPoolRegion(kCenter, kDim);
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Actor Pool Region: Center ["
            << kCenter.x << " " << kCenter.y << " " << kCenter.z
            << "] Dimensions [" << kDim.x << " " << kDim.y << " " << kDim.z
            << "]" << std::endl;
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Keeps Actor Meshes: "
            << (pkPSys->GetKeepsActorMeshes() ? "true" : "false")
            << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Particle Actor: " << std::endl;
        if (!pkPSys->GetActorDesc())
            *pkOstr << "None" << std::endl;
        else
        {
            ActorPrint(pkPSys->GetActorDesc(), 0, usIndent + 2);
        }
   }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::MeshPSysPrint(
    NiPhysXPSMeshParticleSystem* pkPSys, unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkPSys, usIndent);

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Uses PhysX Space: "
            << (pkPSys->GetUsesPhysXSpace() ? "true" : "false")
            << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Uses PhysX Rotations: "
            << (pkPSys->GetUsesPhysXRotations() ? "true" : "false")
            << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Compartment ID: "
            << pkPSys->GetCompartmentID() << std::endl;
    
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Default Actor Pools Size: "
            << pkPSys->GetDefaultActorPoolsSize() << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Fill Actor Pools on Load: "
            << (pkPSys->GetFillActorPoolsOnLoad() ? "true" : "false")
            << std::endl;

        NiPoint3 kCenter;
        NiPoint3 kDim;
        pkPSys->GetActorPoolRegion(kCenter, kDim);
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Actor Pool Region: Center ["
            << kCenter.x << " " << kCenter.y << " " << kCenter.z
            << "] Dimensions [" << kDim.x << " " << kDim.y << " " << kDim.z
            << "]" << std::endl;
        
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Keeps Actor Meshes: "
            << (pkPSys->GetKeepsActorMeshes() ? "true" : "false")
            << std::endl;

        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Particle Actors: " << std::endl;
        if (!pkPSys->GetActorDescCount())
            *pkOstr << "None" << std::endl;
        else
        {
            for (NiUInt32 ui = 0; ui < pkPSys->GetActorDescCount(); ui++)
                ActorPrint(pkPSys->GetActorDescAt(ui), 0, usIndent + 2);
        }
   }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::PropPrint(NiObject* pkObject,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkObject, usIndent);
    
    NiPhysXProp* pkProp = NiDynamicCast(NiPhysXProp, pkObject);

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Scale PhysX to World: "
            << pkProp->GetScaleFactor() << std::endl;
            
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Keep Meshes: "
            << (pkProp->GetKeepMeshes() ? "true" : "false") << std::endl;
    }
    
    // Sources and Destinations
    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Sources: ";
    if (!pkProp->GetSourcesCount())
        *pkOstr << "None" << std::endl;
    else
        *pkOstr << std::endl;
    for (unsigned int i = 0; i < pkProp->GetSourcesCount(); i++)
    {
        SourcePrint(pkProp->GetSourceAt(i), usIndent + 2);
    }

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Destinations: ";
    if (!pkProp->GetDestinationsCount())
        *pkOstr << "None" << std::endl;
    else
        *pkOstr << std::endl;
    for (unsigned int i = 0; i < pkProp->GetDestinationsCount(); i++)
    {
        DestinationPrint(pkProp->GetDestinationAt(i), usIndent + 2);
    }

    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Modified Meshes: ";
    if (!pkProp->GetModifiedMeshCount())
        *pkOstr << "None" << std::endl;
    else
        *pkOstr << std::endl;
    for (unsigned int i = 0; i < pkProp->GetModifiedMeshCount(); i++)
    {
        ModifierPrint(pkProp->GetModifiedMeshAt(i), usIndent + 2);
    }

    if (pkProp->GetSnapshot())
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Snapshot:" << std::endl;
        PropDescPrint(pkProp->GetSnapshot(), usIndent + 2);
    }
    if (NiIsKindOf(NiPhysXPSParticleSystemProp, pkProp))
    {
        NiPhysXPSParticleSystemProp* pkPSysProp =
            (NiPhysXPSParticleSystemProp*)pkProp;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Systems:";
        if (!pkPSysProp->GetSystemCount())
            *pkOstr << " None";
        else
            *pkOstr << std::endl;
        for (unsigned int i = 0; i < pkPSysProp->GetSystemCount(); i++)
        {
            PSysPrint(pkPSysProp->GetSystemAt(i), usIndent + 2);
        }
    }
    if (NiIsKindOf(NiPhysXPSMeshParticleSystemProp, pkProp))
    {
        NiPhysXPSMeshParticleSystemProp* pkPSysProp =
            (NiPhysXPSMeshParticleSystemProp*)pkProp;
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Systems:";
        if (!pkPSysProp->GetSystemCount())
            *pkOstr << " None";
        else
            *pkOstr << std::endl;
        for (unsigned int i = 0; i < pkPSysProp->GetSystemCount(); i++)
        {
            MeshPSysPrint(pkPSysProp->GetSystemAt(i), usIndent + 2);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::ScenePrint(NiObject* pkObject,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkObject, usIndent);
    
    NiPhysXScene* pkScene = NiDynamicCast(NiPhysXScene, pkObject);

    if (ms_bShowDetails)
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Scene Xform:" << std::endl;
        
        NiPoint3 row;
        unsigned int i;
        for (i = 0; i < 3; i ++)
        {
            PRINT_INDENT((usIndent + 2) * usTabStop);
            pkScene->GetSceneXform().m_Rotate.GetRow(i, row);
            *pkOstr << "Rotate: <" << row.x << "," << row.y 
                << "," << row.z << ">" << std::endl;
        }
        
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Translate: <"
            << pkScene->GetSceneXform().m_Translate.x << ","
            << pkScene->GetSceneXform().m_Translate.y << "," 
            << pkScene->GetSceneXform().m_Translate.z << ">" << std::endl;

        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Scale: "
            << pkScene->GetSceneXform().m_fScale << std::endl;
            
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Scale PhysX to World: "
            << pkScene->GetScaleFactor() << std::endl;
            
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Time step ";
        if (pkScene->GetDoFixedStep())
        {
            *pkOstr << "fixed at " << pkScene->GetTimestep() << std::endl;
        }
        else
        {
            *pkOstr << "variable" << std::endl;
        }
            
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Flags:" << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Update Sources: "
            << (pkScene->GetUpdateSrc() ? "true" : "false") << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Update Destinations: "
            << (pkScene->GetUpdateDest() ? "true" : "false") << std::endl;
        PRINT_INDENT((usIndent + 2) * usTabStop);
        *pkOstr << "Debug Render: "
            << (pkScene->GetDebugRender() ? "true" : "false") << std::endl;
    }
    
    // Props
    PRINT_INDENT((usIndent + 1) * usTabStop);
    *pkOstr << "Props: ";
    if (!pkScene->GetPropCount())
        *pkOstr << "None" << std::endl;
    else
        *pkOstr << std::endl;
    for (unsigned int i = 0; i < pkScene->GetPropCount(); i++)
    {
        PropPrint(pkScene->GetPropAt(i), usIndent + 2);
    }

    if (pkScene->GetSnapshot())
    {
        PRINT_INDENT((usIndent + 1) * usTabStop);
        *pkOstr << "Snapshot:" << std::endl;
        SceneDescPrint(pkScene->GetSnapshot(), usIndent + 2);
    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::SDKPrint(NiObject* pkObject,
    unsigned short usIndent)
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;
    unsigned short usTabStop = NiSceneGraphPrinter::ms_usTabStop;

    PhysXPrintID(pkObject, usIndent);
    
    NiPhysXSDKDesc* pkDesc = NiDynamicCast(NiPhysXSDKDesc, pkObject);
    
    if (ms_bShowDetails)
    {
        usIndent += 1;
        
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_PENALTY_FORCE: "
            << pkDesc->GetParameter(NX_PENALTY_FORCE) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_SKIN_WIDTH: "
            << pkDesc->GetParameter(NX_SKIN_WIDTH) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_DEFAULT_SLEEP_LIN_VEL_SQUARED: "
            << pkDesc->GetParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_DEFAULT_SLEEP_ANG_VEL_SQUARED: "
            << pkDesc->GetParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_BOUNCE_THRESHOLD: "
            << pkDesc->GetParameter(NX_BOUNCE_THRESHOLD) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_DYN_FRICT_SCALING: "
            << pkDesc->GetParameter(NX_DYN_FRICT_SCALING) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_STA_FRICT_SCALING: "
            << pkDesc->GetParameter(NX_STA_FRICT_SCALING) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_MAX_ANGULAR_VELOCITY: "
            << pkDesc->GetParameter(NX_MAX_ANGULAR_VELOCITY) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_CONTINUOUS_CD: "
            << pkDesc->GetParameter(NX_CONTINUOUS_CD) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZATION_SCALE: "
            << pkDesc->GetParameter(NX_VISUALIZATION_SCALE) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_WORLD_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_WORLD_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_BODY_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_BODY_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_BODY_MASS_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_BODY_MASS_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_BODY_LIN_VELOCITY: "
            << pkDesc->GetParameter(NX_VISUALIZE_BODY_LIN_VELOCITY) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_BODY_ANG_VELOCITY: "
            << pkDesc->GetParameter(NX_VISUALIZE_BODY_ANG_VELOCITY) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_BODY_JOINT_GROUPS: "
            << pkDesc->GetParameter(NX_VISUALIZE_BODY_JOINT_GROUPS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_JOINT_LOCAL_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_JOINT_LOCAL_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_JOINT_WORLD_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_JOINT_WORLD_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_JOINT_LIMITS: "
            << pkDesc->GetParameter(NX_VISUALIZE_JOINT_LIMITS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CONTACT_POINT: "
            << pkDesc->GetParameter(NX_VISUALIZE_CONTACT_POINT) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CONTACT_NORMAL: "
            << pkDesc->GetParameter(NX_VISUALIZE_CONTACT_NORMAL) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CONTACT_ERROR: "
            << pkDesc->GetParameter(NX_VISUALIZE_CONTACT_ERROR) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CONTACT_FORCE: "
            << pkDesc->GetParameter(NX_VISUALIZE_CONTACT_FORCE) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_ACTOR_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_ACTOR_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_AABBS: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_AABBS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_SHAPES: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_SHAPES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_AXES: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_AXES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_COMPOUNDS: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_COMPOUNDS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_VNORMALS: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_VNORMALS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_FNORMALS: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_FNORMALS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_EDGES: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_EDGES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_SPHERES: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_SPHERES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_STATIC: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_STATIC) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_DYNAMIC: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_DYNAMIC) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_FREE: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_FREE) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_CCD: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_CCD) << "\n";    
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_COLLISION_SKELETONS: "
            << pkDesc->GetParameter(NX_VISUALIZE_COLLISION_SKELETONS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_EMITTERS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_EMITTERS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_POSITION: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_POSITION) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_VELOCITY: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_VELOCITY) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_KERNEL_RADIUS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_KERNEL_RADIUS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_BOUNDS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_BOUNDS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_PACKETS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_PACKETS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_MOTION_LIMIT: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_MOTION_LIMIT) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_DYN_COLLISION: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_DYN_COLLISION) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_STC_COLLISION: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_STC_COLLISION) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_MESH_PACKETS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_MESH_PACKETS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_DRAINS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_DRAINS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FLUID_PACKET_DATA: "
            << pkDesc->GetParameter(NX_VISUALIZE_FLUID_PACKET_DATA) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_MESH: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_MESH) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_COLLISIONS: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_COLLISIONS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_SELFCOLLISIONS: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_SELFCOLLISIONS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_WORKPACKETS: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_WORKPACKETS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_SLEEP: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_SLEEP) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_SLEEP_VERTEX: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_SLEEP_VERTEX) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_TEARABLE_VERTICES: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_TEARABLE_VERTICES)
            << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_TEARING: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_TEARING) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_ATTACHMENT: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_ATTACHMENT) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_CLOTH_VALIDBOUNDS: "
            << pkDesc->GetParameter(NX_VISUALIZE_CLOTH_VALIDBOUNDS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_MESH: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_MESH) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_COLLISIONS: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_COLLISIONS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_WORKPACKETS: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_WORKPACKETS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_SLEEP: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_SLEEP) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX)
            << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES)
            << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_TEARING: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_TEARING) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_ATTACHMENT: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_ATTACHMENT) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_SOFTBODY_VALIDBOUNDS: "
            << pkDesc->GetParameter(NX_VISUALIZE_SOFTBODY_VALIDBOUNDS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_ADAPTIVE_FORCE: "
            << pkDesc->GetParameter(NX_ADAPTIVE_FORCE) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_COLL_VETO_JOINTED: "
            << pkDesc->GetParameter(NX_COLL_VETO_JOINTED) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_TRIGGER_TRIGGER_CALLBACK: "
            << pkDesc->GetParameter(NX_TRIGGER_TRIGGER_CALLBACK) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_SELECT_HW_ALGO: "
            << pkDesc->GetParameter(NX_SELECT_HW_ALGO) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_ACTIVE_VERTICES: "
            << pkDesc->GetParameter(NX_VISUALIZE_ACTIVE_VERTICES) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_CCD_EPSILON: "
            << pkDesc->GetParameter(NX_CCD_EPSILON) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_SOLVER_CONVERGENCE_THRESHOLD: "
            << pkDesc->GetParameter(NX_SOLVER_CONVERGENCE_THRESHOLD) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_BBOX_NOISE_LEVEL: "
            << pkDesc->GetParameter(NX_BBOX_NOISE_LEVEL) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_IMPLICIT_SWEEP_CACHE_SIZE: "
            << pkDesc->GetParameter(NX_IMPLICIT_SWEEP_CACHE_SIZE) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_DEFAULT_SLEEP_ENERGY: "
            << pkDesc->GetParameter(NX_DEFAULT_SLEEP_ENERGY) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_CONSTANT_FLUID_MAX_PACKETS: "
            << pkDesc->GetParameter(NX_CONSTANT_FLUID_MAX_PACKETS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP: "
            << pkDesc->GetParameter(NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP)
            << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_ASYNCHRONOUS_MESH_CREATION: "
            << pkDesc->GetParameter(NX_ASYNCHRONOUS_MESH_CREATION) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_VISUALIZE_FORCE_FIELDS: "
            << pkDesc->GetParameter(NX_VISUALIZE_FORCE_FIELDS) << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON: "
            << pkDesc->GetParameter(NX_FORCE_FIELD_CUSTOM_KERNEL_EPSILON) 
            << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "NX_IMPROVED_SPRING_SOLVER: "
            << pkDesc->GetParameter(NX_IMPROVED_SPRING_SOLVER) << "\n";

        NxPhysicsSDKDesc kSDKDesc;
        pkDesc->GetSDKDesc(kSDKDesc);
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "hwPageSize: " << kSDKDesc.hwPageSize << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "hwPageMax: " << kSDKDesc.hwPageMax << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "hwConvexMax: " << kSDKDesc.hwConvexMax << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "cookerThreadMask: " << kSDKDesc.cookerThreadMask << "\n";
        PRINT_INDENT(usIndent * usTabStop);
        *pkOstr << "Flags: ";
        if (kSDKDesc.flags & NX_SDKF_NO_HARDWARE)
            *pkOstr << "NX_SDKF_NO_HARDWARE ";
        *pkOstr << std::endl;

    }
}
//---------------------------------------------------------------------------
void NiPhysXSceneGraphPrinter::SummaryPrint()
{
    std::ostream* pkOstr = NiSceneGraphPrinter::ms_pkOstr;

    *pkOstr << "Actor Count: " << ms_uiActorCount << "\n";
    *pkOstr << "Joint Count: " << ms_uiJointCount << "\n";
    *pkOstr << "Cloth Count: " << ms_uiClothCount << "\n";
    *pkOstr << "Fluid Count: " << ms_uiFluidCount << "\n";
}
//---------------------------------------------------------------------------



