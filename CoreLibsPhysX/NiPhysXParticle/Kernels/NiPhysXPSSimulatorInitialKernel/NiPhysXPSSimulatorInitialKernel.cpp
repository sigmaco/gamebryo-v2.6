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
#include <NiPhysXParticlePCH.h>

#include "NiPhysXPSSimulatorInitialKernel.h"
#include <NiPSSimulatorKernelHelpers.h>
#include <NiMatrix3.h>
#include <NxVec3.h>
#include <NxMat34.h>

//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiPhysXPSSimulatorInitialKernel)
{
    // Get input streams.
    const NiPhysXPSSimulatorInitialKernelStruct* pkIStruct =
        kWorkload.GetInput<NiPhysXPSSimulatorInitialKernelStruct>(0);
    const NxMat34* pkIActorPoses = kWorkload.GetInput<NxMat34>(1);
    const NxVec3* pkIActorVelocities = kWorkload.GetInput<NxVec3>(2);
    
    // Get output streams.
    NiPoint3* pkOPositions = kWorkload.GetOutput<NiPoint3>(0);
    NiPoint3* pkOVelocities = kWorkload.GetOutput<NiPoint3>(1);
    NiPoint3* pkOLastVelocities = kWorkload.GetOutput<NiPoint3>(2);
    float* pfORotationAngles = kWorkload.GetOutput<float>(3);
    NiPoint3* pkORotationAxes = kWorkload.GetOutput<NiPoint3>(4);

    NiUInt32 uiBlockCount = kWorkload.GetBlockCount();
    
    if (pkIStruct->m_bPhysXSpace)
    {
        // Optimize for case where the particle system and PhysX root are
        // in the same coordinate system.
        for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
        {
            NxMat34 kPose = pkIActorPoses[ui];

            pkOPositions[ui].x = kPose.t.x * pkIStruct->m_fScalePToW;
            pkOPositions[ui].y = kPose.t.y * pkIStruct->m_fScalePToW;
            pkOPositions[ui].z = kPose.t.z * pkIStruct->m_fScalePToW;
            if (pkIStruct->m_bPhysXRotations)
            {

#ifdef _PS3
                NxQuat kQuat;
                kQuat.setWXYZ(0,0,0,0);
                kQuat = NxQuat(kPose.M);
#else
                NxQuat kQuat(kPose.M);
#endif
                NxVec3 kAxis;
                NxReal kAngle;
                kQuat.getAngleAxis(kAngle, kAxis);
                pfORotationAngles[ui] = kAngle * NI_PI / 180.0f;
                pkORotationAxes[ui].x = kAxis.x;
                pkORotationAxes[ui].y = kAxis.y;
                pkORotationAxes[ui].z = kAxis.z;
            }
            
            NxVec3 kVec = pkIActorVelocities[ui];
            NiPoint3 kVelocity(kVec.x, kVec.y, kVec.z);
            kVelocity *= pkIStruct->m_fScalePToW;
            pkOVelocities[ui] = kVelocity;
            pkOLastVelocities[ui] = kVelocity; 
        }
        return;
    }
    
    for (NiUInt32 ui = 0; ui < uiBlockCount; ui++)
    {
        NxMat34 kPose = pkIActorPoses[ui];

        NxVec3 kTransformedPose = (pkIStruct->m_kPhysXToPSys * kPose.t) * 
            pkIStruct->m_fScalePToW;
        pkOPositions[ui].x = kTransformedPose.x;
        pkOPositions[ui].y = kTransformedPose.y;
        pkOPositions[ui].z = kTransformedPose.z;

        if (pkIStruct->m_bPhysXRotations)
        {
            NxMat33 kRot = pkIStruct->m_kPhysXToPSys.M * kPose.M;
            
#ifdef _PS3
            NxQuat kQuat;
            kQuat.setWXYZ(0,0,0,0);
            kQuat = NxQuat(kRot);
#else
            NxQuat kQuat(kRot);
#endif
            NxVec3 kAxis;
            NxReal kAngle;
            kQuat.getAngleAxis(kAngle, kAxis);
            pfORotationAngles[ui] = kAngle * NI_PI / 180.0f;
            pkORotationAxes[ui].x = kAxis.x;
            pkORotationAxes[ui].y = kAxis.y;
            pkORotationAxes[ui].z = kAxis.z;
        }
        
        NxVec3 kVelocity = (pkIStruct->m_kPhysXToPSys.M *
            pkIActorVelocities[ui]) * pkIStruct->m_fScalePToW;
        pkOVelocities[ui].x = kVelocity.x;
        pkOVelocities[ui].y = kVelocity.y;
        pkOVelocities[ui].z = kVelocity.y;
        pkOLastVelocities[ui] = pkOVelocities[ui];
    }
}
NiSPEndKernelImpl(NiPhysXPSSimulatorInitialKernel)
//---------------------------------------------------------------------------
