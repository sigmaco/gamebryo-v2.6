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

#ifndef NIPHYSXPSSIMULATORINITIALKERNEL_H
#define NIPHYSXPSSIMULATORINITIALKERNEL_H

#include <NiPhysXParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiUniversalTypes.h>

#if defined(_PS3)
#pragma GCC system_header
#endif
#include <NxMat34.h>

/// @cond EMERGENT_INTERNAL
struct NiPhysXPSSimulatorInitialKernelStruct
{
    NxMat34 m_kPhysXToPSys;
    float m_fScalePToW;
    bool m_bPhysXSpace;
    bool m_bPhysXRotations;
};
/// @endcond

NiSPDeclareKernelLib(NiPhysXPSSimulatorInitialKernel, NIPHYSXPARTICLE_ENTRY)

#endif  // #ifndef NIPHYSXPSSIMULATORINITIALKERNEL_H
