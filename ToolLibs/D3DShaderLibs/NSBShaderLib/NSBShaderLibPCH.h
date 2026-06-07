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

#ifndef NSBSHADERLIBPCH_H
#define NSBSHADERLIBPCH_H

#if defined(NI_USE_PCH)

#include "NSBShaderLibLibType.h"
#include "NSBLoader.h"
#include "NSBShaderLib.h"
#include "NSBShaderLibSDM.h"
#include "NSBShaderLibrary.h"

#ifdef WIN32
#include "NSBD3D10ShaderLibrary.h"
#endif  // #ifdef WIN32

#endif  //#if defined(NI_USE_PCH)

#endif  // #ifndef NSBSHADERLIBPCH_H
