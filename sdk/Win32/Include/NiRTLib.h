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

#ifndef NIRTLIB_H
#define NIRTLIB_H


// WIN32 system headers demote/disable some useful warnings, so push
// warning state here and pop it at after includes.
#pragma warning(push, 3)

#include <cassert>
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <wchar.h>
#include <stddef.h>

#include <sys/stat.h>

#pragma warning(pop)

#define NI_DATA_ALIGNMENT(size)

#include "NiMemManager.h"

#endif // NIRTLIB_H
