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

#ifndef NIPHYSXSCENEGRAPHPRINTER_H
#define NIPHYSXSCENEGRAPHPRINTER_H

class NiObject;
#include <iostream>
#include <fstream>
#include <NiMain.h>
#include <NiPhysX.h>
#include <NiPhysXParticle.h>
#include <NiPhysXFluid.h>

class NiPhysXSceneGraphPrinter
{
public:
    static void PhysXPrintID(NiObject* pkObject, unsigned short usIndent);
    static void NxMat33Print(NxMat33& kM, unsigned short usIndent);
    static void NxMat34Print(NxMat34& kM, unsigned short usIndent);
    static void SourcePrint(NiPhysXSrc* pkSrc, unsigned short usIndent);
    static void DestinationPrint(NiPhysXDest* pkDest, unsigned short usIndent);
    static void ModifierPrint(NiMesh* pkMesh, unsigned short usIndent);
    static void ShapePrint(NiPhysXShapeDesc* pkDesc, unsigned short usIndent);
    static void ActorPrint(NiPhysXActorDesc* pkDesc, unsigned int uiNumStates,
        unsigned short usIndent);
    static void JointPrint(NiPhysXJointDesc* pkDesc, unsigned short usIndent);
    static void ClothPrint(NiPhysXClothDesc* pkDesc, unsigned short usIndent);
    static void MaterialPrint(NiPhysXMaterialDesc* pkDesc,
        unsigned int uiNumStates, unsigned short usIndent);
    static void FluidEmitterPrint(NiPhysXFluidEmitterDesc* pkDesc,
        unsigned short usIndent);
    static void FluidPrint(NiPhysXFluidDesc* pkDesc, unsigned short usIndent);
    static void PSysPrint(NiPhysXPSParticleSystem* pkPSys,
        unsigned short usIndent);
    static void MeshPSysPrint(NiPhysXPSMeshParticleSystem* pkPSys,
        unsigned short usIndent);

    static void PropDescPrint(NiPhysXPropDesc* pkDesc,
        unsigned short usIndent);
    static void SceneDescPrint(NiPhysXSceneDesc* pkDesc,
        unsigned short usIndent);
    
    static void PropPrint(NiObject* pkObject, unsigned short usIndent);
    
    static void ScenePrint(NiObject* pkObject, unsigned short usIndent);
    static void SDKPrint(NiObject* pkObject, unsigned short usIndent);
    static void SummaryPrint();

    static bool ms_bShowDetails;

    static unsigned int ms_uiActorCount;
    static unsigned int ms_uiJointCount;
    static unsigned int ms_uiClothCount;
    static unsigned int ms_uiFluidCount;

private:


};

#endif // #ifndef NIPHYSXSCENEGRAPHPRINTER_H
