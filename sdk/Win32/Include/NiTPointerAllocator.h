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

#ifndef NITPOINTERALLOCATOR_H
#define NITPOINTERALLOCATOR_H

#include <NiTAbstractPoolAllocator.h>

#if defined(WIN32)
// Instantiate the allocator used by NiTPointerList and NiTPointerMap.
// Disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
#if defined (NIMAIN_IMPORT) && \
    !defined (NITPOINTERALLOCATOR_INSTANTIATED)
    extern template class NIMAIN_ENTRY NiTAbstractPoolAllocator<size_t>;
#endif 
#endif // #if defined(WIN32)

template <class T> class NiTPointerAllocator :
    public NiTAbstractPoolAllocator<T>
{
public: 
    inline void* Allocate();
    inline void Deallocate (void* p);  
};

// Inline include
#include "NiTPointerAllocator.inl"

#endif
