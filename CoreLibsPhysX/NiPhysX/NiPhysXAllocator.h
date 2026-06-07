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

#ifndef NIPHYSXALLOCATOR_H
#define NIPHYSXALLOCATOR_H

#include "NiPhysXLibType.h"

#include <NiSystem.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxUserAllocator.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable: 4275) // DLL export warning due to PhysX
#endif

/**
    This class implements the PhysX's memory allocation interface by
    passing all memory calls through to the current Gamebryo allocator.

    If no other allocator is passed to NiPhysXManager::Initialize then an
    instance of this class is created and passed to the PhysX SDK
    initialization function. Users wishing to override this behavior should
    create their own class that implements NxUserAllocator and pass it to
    NiPhysXManager::Initialize. 
*/
class NIPHYSX_ENTRY NiPhysXAllocator :
    public NxUserAllocator, public NiMemObject
{

public:
    /// Allocate the given number of bytes.
    ///
    /// Implemented using NiMalloc.
    virtual void* malloc(size_t stSize);
    
    /// Allocate the given number of bytes, with type information.
    ///
    /// Implemented using NiMalloc, ignoring the type argument.
    virtual void* malloc(size_t stSize, NxMemoryType eType);

    /// Allocate the given number of bytes, with debug information.
    ///
    /// Uses NiMalloc, or _NiMalloc if NI_MEMORY_DEBUGGER is defined.
    virtual void* mallocDEBUG(size_t stSize, const char* pcFileName, int iLine);

    /// Allocate the given number of bytes, with even more debug information.
    ///
    /// Uses NiMalloc, or _NiMalloc if NI_MEMORY_DEBUGGER is defined.
    virtual void* mallocDEBUG(size_t stSize, const char* pcFileName, int iLine,
        const char* pcClassName, NxMemoryType eType);

    /// Reallocate the given buffer.
    ///
    /// Implemented using NiRealloc.
    virtual void* realloc(void* pvMemory, size_t stSize);   

    /// Free the given buffer.
    ///
    /// Implemented using NiFree.
    virtual void free(void* pvMemory);
};

#if defined (WIN32)
#pragma warning(pop)
#endif

#endif // #ifndef NIPHYSXALLOCATOR_H
