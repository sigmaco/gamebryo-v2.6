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

#ifdef _PS3
#include "NiMainPCH.h"
#endif

#include "NiMainLibType.h"
#include "NiTAbstractPoolAllocator.h"

#if defined(_WII)
// CW needs to have this first or there will be compiler errors
NiAllocatorDeclareStatics(size_t, 2048)
#endif

#define NITPOINTERALLOCATOR_INSTANTIATED
template class NIMAIN_ENTRY NiTAbstractPoolAllocator<size_t>;

#include "NiTPointerAllocator.h"

#if !defined(_WII)
NiAllocatorDeclareStatics(size_t, 2048)
#endif


