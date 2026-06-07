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

#ifndef NISPLINE_H
#define NISPLINE_H

// Key (interpolant) classes which can be used to support 
// interpolation for translation & rotation animation routines.
// The NiAnimation class makes use of all these key types 
// to create a node class that supports most varieties of 
// animation (currently) found in 3DStudio Max.
#include "NiPosKey.h"
#include "NiLinPosKey.h"
#include "NiBezPosKey.h"
#include "NiTCBPosKey.h"

// rotation interpolation
#include "NiRotKey.h"
#include "NiLinRotKey.h"
#include "NiBezRotKey.h"
#include "NiTCBRotKey.h"
#include "NiEulerRotKey.h"

// The following float based classes are used to interpolate scalar (as
// opposed to vector) values.  They exist to be used for Euler angle 
// rotations in support of that functionality in 3DStudio Max (among 
// other things).
#include "NiFloatKey.h"
#include "NiLinFloatKey.h"
#include "NiBezFloatKey.h"
#include "NiTCBFloatKey.h"

// A class used by NiAnimationNode to determine (binary) 
// visibility state for the node.
#include "NiBoolKey.h"

// A class used to store time stamped notes. Used by the
// NiAnimBlender class to morph between animations.
#include "NiTextKey.h"

// The following classes provide a simple means of interpolating
// an NiColorA based animation. Currently this type of animation
// is only used by the particle system class for particle aging
// effects.
#include "NiColorKey.h"
#include "NiLinColorKey.h"

// morphing
#include "NiInterpScalar.h"

#endif

