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

#include "NiPhysXPCH.h"

#include "NiPhysXAllocator.h"

//---------------------------------------------------------------------------
void* NiPhysXAllocator::malloc(size_t stSize)
{
    return NiMalloc2(stSize, NiMemHint::USAGE_PHYSICS); 
}
//---------------------------------------------------------------------------
void* NiPhysXAllocator::malloc(size_t stSize, NxMemoryType)
{
    return NiMalloc2(stSize, NiMemHint::USAGE_PHYSICS);
}
//---------------------------------------------------------------------------
void* NiPhysXAllocator::mallocDEBUG(size_t stSize, const char* pcFileName,
    int iLine)
{
    NI_UNUSED_ARG(iLine);
    NI_UNUSED_ARG(pcFileName);
#ifdef NI_MEMORY_DEBUGGER
    return _NiMalloc(stSize, NiMemHint::USAGE_PHYSICS, pcFileName, iLine,
        __FUNCTION__);
#else
    return NiMalloc2(stSize, NiMemHint::USAGE_PHYSICS);
#endif // NI_MEMORY_DEBUGGER 
}    
//---------------------------------------------------------------------------
void* NiPhysXAllocator::mallocDEBUG(size_t stSize, const char* pcFileName,
    int iLine, const char*, NxMemoryType)

{
    NI_UNUSED_ARG(iLine);
    NI_UNUSED_ARG(pcFileName);
#ifdef NI_MEMORY_DEBUGGER
    return _NiMalloc(stSize, NiMemHint::USAGE_PHYSICS, pcFileName, iLine,
        __FUNCTION__);
#else
    return NiMalloc2(stSize, NiMemHint::USAGE_PHYSICS);
#endif // NI_MEMORY_DEBUGGER 
}    
//---------------------------------------------------------------------------
void* NiPhysXAllocator::realloc(void* pvMemory, size_t stSize)
{
    return NiRealloc2(pvMemory, stSize, NiMemHint::USAGE_PHYSICS);
}   
//---------------------------------------------------------------------------
void NiPhysXAllocator::free(void* pvMemory)
{
    NIMEMASSERT(NULL != pvMemory);
    NiFree(pvMemory);
}
//---------------------------------------------------------------------------
