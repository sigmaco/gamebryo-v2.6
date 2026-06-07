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

#ifndef NIMORPHINGKERNEL_H
#define NIMORPHINGKERNEL_H

#include <NiSPKernelMacros.h>

/// Maximum morph targets supported by the kernel.
/// This number is to maintain some sanity. Some renderers may not actually
/// support this maximum number. When the modifier is attached it validates
/// the current number of morph targets with the maximum allowed amount by
/// the renderer. If the number exceeds this amount the modifier will fail to
/// attach.
#define MAX_MORPH_TARGETS 32

/**
    The NiMorphingKernel is a Floodgate kernel class used
    to perform morphing operations.
    
    It is used by the NiMorphMeshModifier class and should never
    need to be used directly by an application.
*/ 
NiSPDeclareKernel(NiMorphingKernel)

#endif

