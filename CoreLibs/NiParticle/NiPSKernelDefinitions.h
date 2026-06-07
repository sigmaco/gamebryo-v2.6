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

#ifndef NIPSKERNELDEFINITIONS_H
#define NIPSKERNELDEFINITIONS_H

#include <NiColor.h>

/// A color key structure for use in Floodgate kernels.
struct NiPSKernelColorKey
{
    NiRGBA m_kColor;
    float m_fTime;
};

/// An epsilon to be used when comparing floating point variables in Floodgate
/// kernels.
const float NIPSKERNEL_EPSILON = 0.0001f;

/// The boundary to which data buffers used in Floodgate kernels should be
/// aligned.
const size_t NIPSKERNEL_ALIGNMENT =
#ifdef _PS3
    128;
#else   // #ifdef _PS3
    16;
#endif  // #ifdef _PS3

#endif  // #ifndef NIPSKERNELDEFINITIONS_H
