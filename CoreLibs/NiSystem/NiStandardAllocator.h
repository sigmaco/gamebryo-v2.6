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

#ifndef NISTANDARDALLOCATOR_H
#define NISTANDARDALLOCATOR_H

#include "NiExternalMemAllocator.h"
#include "NiTSmallAllocStrategy.h"

#if defined(_WII)
#include "NiWiiMemAllocator.h"

// Use Wii-specific allocator that is aware of MEM1 and MEM2
// arenas and uses a NiMemHint object passed to the allocator to 
// allocate memory from appropriate arena.
typedef NiTSmallAllocStrategy<NiWiiMemAllocator>
    NiStandardAllocator;
#else
typedef NiTSmallAllocStrategy<NiExternalMemAllocator>
    NiStandardAllocator;
#endif

#endif // NISTANDARDALLOCATOR_H
