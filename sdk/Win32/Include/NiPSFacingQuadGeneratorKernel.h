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

#ifndef NIPSFACINGQUADGENERATORKERNEL_H
#define NIPSFACINGQUADGENERATORKERNEL_H

#include <NiParticleLibType.h>
#include <NiSPKernelMacros.h>
#include <NiPSKernelDefinitions.h>
#include <NiPoint3.h>

/// A structure containing data required by the NiPSFacingQuadGeneratorKernel
/// Floodgate kernel.
struct NiPSFacingQuadGeneratorKernelStruct
{
    /// Camera up vector in model space.
    NiPoint3 m_kCamUp;

    /// Camera right vector in model space.
    NiPoint3 m_kCamRight;

    /// Normal vector in model space.
    NiPoint3 m_kNormal;
};

/**
    Generates rotated, screen-facing quads for a particle system.

    Each quad is composed of two triangles. Each vertex has a normal and
    optionally a color. Rotation angles may or may not be available. Quads
    are rotated about their screen-facing normal by the rotation angle.
    A positive rotation angle results in a clockwise rotation.
*/
NiSPDeclareKernelLib(NiPSFacingQuadGeneratorKernel, NIPARTICLE_ENTRY)

#endif  // #ifndef NIPSFACINGQUADGENERATORKERNEL_H
