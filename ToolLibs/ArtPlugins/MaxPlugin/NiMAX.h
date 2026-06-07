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

#ifndef NIMAX_H
#define NIMAX_H

// Temporarily disable "for" scope conformance option
#pragma conform(forScope, push, f, off)

#pragma warning(push)
// unreferenced formal parameter
#pragma warning(disable: 4100)
// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
// nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable: 4238)
// nonstandard extension used : 'default argument' : conversion from '' to ' &'
#pragma warning(disable: 4239)
// 'argument' : conversion from '' to '', signed/unsigned mismatch
#pragma warning(disable: 4245)
// '' : assignment operator could not be generated
#pragma warning(disable: 4512)

#include "max.h"
#include "shaders.h"
#include "macrorec.h"
#include "gport.h"
#include "utilapi.h"
#include "simpobj.h"
#include "modstack.h"
#include "stdmat.h"
#include "spline3d.h"
#include "splshape.h"
#include "decomp.h"
#include "bmmlib.h"
#include "ikctrl.h"
#include "strclass.h"
#include "interpik.h"
#include "notetrck.h"
#include "lslights.h"

#include "iparamb2.h"
#include "iparamm2.h"
#include "simpspl.h"
#include "ISkin.h"

// Max 9+ are API compatable
#if MAX_RELEASE == 8901 || MAX_RELEASE == 9000 || MAX_RELEASE == 10000
#include "NiMAX90ExtraTypes.h"
#elif MAX_RELEASE == 11000
#include "NiMAX2009ExtraTypes.h"
#else
#error "Unknown MAX Version!"
#endif

#ifndef NO_CSTUDIO
#include "phyexp.h"
#endif

// Re-enable "for" scope conformance option
#pragma conform(forScope, pop, f)

// Reinclude assert header to override assert definition from MAX headers
// (which asserts even in Release mode).
#ifdef NI_OLD_STYLE_HEADERS
#include "assert.h"
#else
#include "cassert"
#endif

#pragma warning( pop )

#endif

