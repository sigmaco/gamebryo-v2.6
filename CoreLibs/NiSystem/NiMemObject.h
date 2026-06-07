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

#ifndef NIMEMOBJECT_H
#define NIMEMOBJECT_H

#ifdef __SPU__

class NiMemObject
{
    // No-op on SPU
};

#else

#include "NiSystemLibType.h"
#include "NiMemoryDefines.h"

// NiMemObject implements interfaces overriding the global new and
// delete operators. All derived classes will now funnel their
// allocations/deallocations through the NiMemManager object.
//
// External code should never call "new" or "delete"
// on NiMemObjects directly. Rather, they should use
// "NiNew" and "NiDelete". This will allow allocation tracking
// in builds defining the NI_MEMORY_DEBUGGER symbol.

class NISYSTEM_ENTRY NiMemObject
{
#ifdef NI_MEMORY_DEBUGGER
    private:
        // The memory debugger uses the file, line, function 
        // routines. Therefore new and new[] should be 
        // unavailable to the outer application.
        static void* operator new(size_t stSize,
            NiMemHint kHint = NI_MEMHINT_NONE)
#if defined(NI_DISABLE_EXCEPTIONS)
            throw()
#endif
            ;
        static void* operator new[](size_t stSize,
            NiMemHint kHint = NI_MEMHINT_NONE)
#if defined(NI_DISABLE_EXCEPTIONS)
            throw()
#endif
            ;

    public:
        static void *operator new(
            size_t stSize,
            NiMemHint kHint,
            const char *pcSourceFile, 
            int iSourceLine, 
            const char* pcFunction);

        static void *operator new[](
            size_t stSize, 
            NiMemHint kHint,
            const char *pcSourceFile, 
            int iSourceLine, 
            const char* pcFunction);

        // Required for exception handling in the compiler. These 
        // should not be used in practice.
        static void operator delete(
            void*, 
            NiMemHint,
            const char*, 
            int, 
            const char*){}

        static void operator delete[](
            void*, 
            NiMemHint,
            const char*, 
            int, 
            const char*){}
#else
    public:
        static void* operator new(size_t stSize, NiMemHint kHint);
        static void* operator new[](size_t stSize, NiMemHint kHint);

        // Required for exception handling in the compiler. These 
        // should not be used in practice.
        static void operator delete( void *, NiMemHint) {}
        static void operator delete[]( void *, NiMemHint) {}

#endif
    public:
        // delete and delete[] must be implemented in NI_MEMORY_DEBUGGER
        // and non-NI_MEMORY_DEBUGGER builds.
        static void operator delete(void *pvMem, size_t stElementSize);
        static void operator delete[](void *pvMem, size_t stElementSize);

        // We don't want to hide the placement new functions:
        inline static void* operator new( size_t, void* p ) { return p; }
        inline static void* operator new[]( size_t, void* p ) { return p; }

        // Conversely we also then need a placement delete function:
        inline static void operator delete( void *, void* ) {}
        inline static void operator delete[]( void *, void* ) {}

};

#endif // __SPU__

#endif // NIMEMOBJECT_H
