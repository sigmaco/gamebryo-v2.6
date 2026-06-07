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

#ifndef NIMEMORYDEFINES_H
#define NIMEMORYDEFINES_H

#include "NiSystemLibType.h"
#include "NiBuildConfiguration.h"
#include "NiDebug.h"
#include "NiMemHint.h"

#if defined(_PS3) || defined(_WII)
#include <stddef.h>
#include <alloca.h>
#else
#include <malloc.h>
#endif

//---------------------------------------------------------------------------
// Determine whether or not to store the File, Line, Function data.
//---------------------------------------------------------------------------
// Uncomment this #if block in order to turn on memory debugging in
// debug builds.

// Use memory debugging only in NiMemory builds
#if NI_USE_MEMORY_MANAGEMENT && !defined(NI_MEMORY_DEBUGGER)
    #define NI_MEMORY_DEBUGGER
#endif

//---------------------------------------------------------------------------
// NiMemEventType.
//
// Determine the overall allocation type. When an object is allocated with
// an allocation routine, it should be deallocated with its corresponding
// deallocation routine.
//---------------------------------------------------------------------------

enum NiMemEventType
{
    NI_UNKNOWN = 0,                 // Unknown allocation/deallocation type.
                                    // This type will cause assertions in
                                    // the memory management interfaces.
    NI_OPER_NEW,                    // Allocated through NiNew. 
                                    // Should be deallocated with NiDelete
    NI_OPER_NEW_ARRAY,              // Allocated through NiNew[]. 
                                    // Should be deallocated with NiDelete[]
    NI_OPER_DELETE,                 // Deallocated with NiDelete.

    NI_OPER_DELETE_ARRAY,           // Deallocated with NiDelete[]
    
    NI_MALLOC,                      // Allocated through NiMalloc.
                                    // Should be deallocated with NiFree
    NI_REALLOC,                     // Allocated through NiRealloc.
                                    // Should be deallocated with NiFree.
    NI_ALIGNEDMALLOC,               // Allocated through NiAlignedAlloc.
                                    // Should be deallocated with 
                                    // NiAlignedFree
    NI_ALIGNEDREALLOC,              // Allocated through NiAlignedRealloc.
                                    // Should be deallocated with 
                                    // NiAlignedFree
    NI_FREE,                        // Deallocated with NiFree
    NI_ALIGNEDFREE,                 // Deallocated with NiAlignedFree
    NI_EXTERNALALLOC,               // Used to track external allocations
    NI_EXTERNALFREE                 // Used to track external frees
};


//---------------------------------------------------------------------------
// Allocation macros and functions.
//
// NiNew and NiDelete are ONLY used for NiMemObject-derived classes.
// This lets Gamebryo store the file, line, function information depending
// on the build target.
//
// NiAlloc/NiMalloc/NiCalloc/NiRealloc and NiFree should be used for 
// any non-NiMemObject object or intrinsic type. Note that they do not call
// global new or delete, nor will they call constructors or destructors.
//---------------------------------------------------------------------------

#define NI_MEM_FILE_DEFAULT 0
#define NI_MEM_LINE_DEFAULT -1
#define NI_MEM_FUNCTION_DEFAULT 0
#define NI_MEM_DEALLOC_SIZE_DEFAULT (size_t)-1
#define NI_MEM_MARKER_BEGIN "BEGIN"
#define NI_MEM_MARKER_END "END"

#ifdef _XENON
    #define NI_MEM_ALIGNMENT_DEFAULT  16
#elif defined(_PS3)
    #define NI_MEM_ALIGNMENT_DEFAULT  16
#elif defined(_WII)
    #define NI_MEM_ALIGNMENT_DEFAULT  32
#else
    #define NI_MEM_ALIGNMENT_DEFAULT  4
#endif

#if defined(_WII) && defined(__CWCC__)
    // The CodeWarrior compiler on Wii uses extra bytes to store info for
    // object arrays (arrays created using new[])
    #define NI_MEM_WII_CW_NEW_ARRAY_OVERHEAD  (sizeof(size_t)*4)
#endif

// Build-independent memory management routines
NISYSTEM_ENTRY void _NiFree(void* pvMemory);
NISYSTEM_ENTRY void _NiAlignedFree(void* pvMemory);

#define NI_IS_POWER_OF_TWO(x)   (((x)&(x-1)) == 0)

// Build-dependent memory management routines
#if defined(NI_MEMORY_DEBUGGER) && !defined(__SPU__)
    NISYSTEM_ENTRY void* _NiMalloc(size_t stSizeInBytes, NiMemHint kHint,
        const char* pcSourceFile, int iSourceLine, const char* pcFunction);

    NISYSTEM_ENTRY void* _NiAlignedMalloc(size_t stSizeInBytes, 
        size_t stAlignment, NiMemHint kHint, const char* pcSourceFile,
        int iSourceLine, const char* pcFunction);

    NISYSTEM_ENTRY void* _NiRealloc(void *memblock, size_t stSizeInBytes,
        NiMemHint kHint, const char* pcSourceFile, int iSourceLine, 
        const char* pcFunction); 

    NISYSTEM_ENTRY void* _NiAlignedRealloc(void *memblock, 
        size_t stSizeInBytes, size_t stAlignment, NiMemHint kHint,
        const char* pcSourceFile, int iSourceLine, const char* pcFunction);

    NISYSTEM_ENTRY bool NiVerifyAddress(const void* pvMemory);

    NISYSTEM_ENTRY bool _NiTrackAlloc(const void* pvMemory, 
        size_t stSizeInBytes, NiMemHint kHint, const char* pcSourceFile,
        int iSourceLine, const char* pcFunction); 

    NISYSTEM_ENTRY bool _NiTrackFree(const void* pvMemory); 

    NISYSTEM_ENTRY bool _NiMemMarker(const char* pcMarkerType, 
        const char* pcClassifier, const char* pcString);
    NISYSTEM_ENTRY bool _NiMemMarker(const char* pcMarkerType, 
        const char* pcClassifier, unsigned int uiValue);
    NISYSTEM_ENTRY bool _NiMemMarker(const char* pcMarkerType, 
        const char* pcClassifier, const void* const pvValue);

    #define NiNew new(NI_MEMHINT_NONE,__FILE__,__LINE__,__FUNCTION__)
    #define NiNew2(hint) new(hint,__FILE__,__LINE__,__FUNCTION__)
    #define NiDelete delete

    #define NiAlloc(T, count) ((T*)_NiMalloc(sizeof(T)*(count), \
        NI_MEMHINT_NONE, __FILE__, __LINE__, __FUNCTION__))

    #define NiAlloc2(T, count, hint) ((T*)_NiMalloc(sizeof(T)*(count), \
        hint, __FILE__, __LINE__, __FUNCTION__))

    #define NiAlignedAlloc(T, count, alignment) ((T*)_NiAlignedMalloc( \
        sizeof(T)*(count), alignment, NI_MEMHINT_NONE, __FILE__, __LINE__, \
        __FUNCTION__)) 

    #define NiAlignedAlloc2(T, count, alignment, hint) \
        ((T*)_NiAlignedMalloc(sizeof(T)*(count), alignment, hint, __FILE__, \
        __LINE__, __FUNCTION__)) 

    #define NiAlignedMalloc(size, alignment) (_NiAlignedMalloc(size, \
        alignment, NI_MEMHINT_NONE, __FILE__, __LINE__, __FUNCTION__)) 

    #define NiAlignedMalloc2(size, alignment, hint) (_NiAlignedMalloc(size, \
        alignment, hint, __FILE__, __LINE__, __FUNCTION__)) 

    #define NiMalloc(size) (_NiMalloc(size, NI_MEMHINT_NONE, __FILE__, \
        __LINE__, __FUNCTION__)) 

    #define NiMalloc2(size, hint) (_NiMalloc(size, hint, __FILE__, \
        __LINE__, __FUNCTION__)) 

    #define NiRealloc(memblock, size) (_NiRealloc(memblock, size, \
        NI_MEMHINT_NONE, __FILE__, __LINE__, __FUNCTION__)) 

    #define NiRealloc2(memblock, size, hint) (_NiRealloc( memblock, size, \
        hint, __FILE__, __LINE__, __FUNCTION__)) 

    #define NiAlignedRealloc(memblock, size, alignment) (_NiAlignedRealloc( \
        memblock, size, alignment, NI_MEMHINT_NONE, __FILE__, __LINE__, \
        __FUNCTION__))

    #define NiAlignedRealloc2(memblock, size, alignment, hint) \
        (_NiAlignedRealloc(memblock, size, alignment, hint, __FILE__, \
        __LINE__, __FUNCTION__))

    #define NiFree(p) _NiFree(p)
    #define NiAlignedFree(p) _NiAlignedFree(p)

    #define NiTrackAlloc(pointer, sizeInBytes) (_NiTrackAlloc(pointer, \
        sizeInBytes, NI_MEMHINT_NONE, __FILE__, __LINE__, __FUNCTION__))

    #define NiTrackAlloc2(pointer, sizeInBytes, hint) (_NiTrackAlloc( \
        pointer, sizeInBytes, hint, __FILE__, __LINE__, __FUNCTION__))

    #define NiTrackFree(pointer)  (_NiTrackFree(pointer)) 
    #define NiMemMarker(markerType, classifier, string) (_NiMemMarker( \
        markerType, classifier, string)) 
#else
    NISYSTEM_ENTRY void* _NiMalloc(size_t stSizeInBytes, NiMemHint kHint);

    NISYSTEM_ENTRY void* _NiAlignedMalloc(size_t stSizeInBytes,
        size_t stAlignment, NiMemHint kHint);

    NISYSTEM_ENTRY void* _NiRealloc(void *memblock, size_t stSizeInBytes,
        NiMemHint kHint);

    NISYSTEM_ENTRY void* _NiAlignedRealloc(void *memblock,size_t \
        stSizeInBytes, size_t stAlignment, NiMemHint kHint);

    #define NiVerifyAddress(x) false
    
    #define NiNew new(NiMemHint(NiMemHint::NONE))
    #define NiNew2(hint) new(hint)
    #define NiDelete delete

    #define NiAlloc(T, count) ((T*)_NiMalloc(sizeof(T)*(count), \
        NI_MEMHINT_NONE))

    #define NiAlloc2(T, count, hint) ((T*)_NiMalloc(sizeof(T)*(count), hint))

    #define NiAlignedAlloc(T, count, alignment) ((T*)_NiAlignedMalloc( \
        sizeof(T)*(count), alignment, NI_MEMHINT_NONE))

    #define NiAlignedAlloc2(T, count, alignment, hint) \
        ((T*)_NiAlignedMalloc(sizeof(T)*(count), alignment, hint))

    #define NiAlignedMalloc(size, alignment) (_NiAlignedMalloc(size, \
        alignment, NI_MEMHINT_NONE)) 

    #define NiAlignedMalloc2(size, alignment, hint) (_NiAlignedMalloc(size, \
        alignment, NI_MEMHINT_NONE)) 

    #define NiMalloc(size) (_NiMalloc(size, NI_MEMHINT_NONE)) 

    #define NiMalloc2(size, hint) (_NiMalloc(size, hint)) 

    #define NiRealloc(memblock, size) (_NiRealloc(memblock, size, \
        NI_MEMHINT_NONE))

    #define NiRealloc2(memblock, size, hint) (_NiRealloc(memblock, size, \
        hint))

    #define NiAlignedRealloc(memblock, size, alignment) (_NiAlignedRealloc( \
        memblock, size, alignment, NI_MEMHINT_NONE))

    #define NiAlignedRealloc2(memblock, size, alignment, hint) \
        (_NiAlignedRealloc(memblock, size, alignment, hint))

    #define NiFree(p) _NiFree(p)
    #define NiAlignedFree(p) _NiAlignedFree(p)

    #define NiTrackAlloc(p, sizeInBytes) false
    #define NiTrackAlloc2(p, sizeInBytes, hint) false

    #define NiTrackFree(p) false
    #define NiMemMarker(markerType, classifier, string) false
#endif // NI_MEMORY_DEBUGGER

// In many cases, external resources will need to be allocated.
// In this case, use the following macros:
#define NiExternalNew new
#define NiExternalDelete delete
#define NiExternalAlloc(T, count) ((T*)malloc(sizeof(T)*(count)))
#define NiExternalMalloc malloc
#define NiExternalRealloc realloc
#define NiExternalFree free
#define NiExternalCalloc calloc

#if defined(_PS3)
    #define NiExternalAlignedMalloc(size, alignment) memalign(alignment, size)
    #define NiExternalAlignedRealloc reallocalign
    #define NiExternalAlignedFree free
    #define NiAlignOf(T) __alignof__(T)
    #define NiStackAlloc(T, count) (T*)alloca(sizeof(T)*(count))
    #define NiStackFree(x)
#elif defined(_WII)
    #include <pool_alloc.h>

    enum NiWiiMemArenaType
    {
        NI_WII_MEM1_ARENA,
        NI_WII_MEM2_ARENA,
        NI_WII_MAX_ARENAS
    };

    // The following functions are declared extern "C" so that they can be 
    // accessed from RSO loader apps.  The corresponding typedefs are 
    // used to cast the function pointer that comes back from 
    // RSOFindExportSymbolAddr.  They must match the function
    // declarations.
    typedef void* (*SysAllocFunctionPtr) (size_t, size_t, NiWiiMemArenaType);
    typedef void (*SysFreeFunctionPtr) (void*);
    extern "C"
    {
        // CodeWarrior doesn't provide aligned malloc functions, thus these 
        // functions are implemented in NiMemoryDefines_Wii.cpp rather than 
        // being simple pass-thru macros to standard functions.
        void* NiExternalAlignedMalloc(size_t stSize, size_t stAlignment, 
            NiWiiMemArenaType eMemArena = NI_WII_MEM1_ARENA);

        void* NiExternalAlignedRealloc(
            void *pvMemBlock, 
            size_t stNewSize,
            size_t stAlignment, 
            NiWiiMemArenaType eMemArena = NI_WII_MEM1_ARENA);

        void NiExternalAlignedFree(void *pvMemBlock);
    }

    #define NiAlignOf(T) __alignof__(T)
    #define NiStackAlloc(T, count) (T*)alloca(sizeof(T)*(count))
    #define NiStackFree(x)

#if defined(__CWCC__)
    // Optional implementation of CodeWarrior __sys_alloc and __sys_free, 
    // which are used by CRT malloc and free functions. These functions must 
    // precede CodeWarrior implementation in the link order. This can be done
    // by simply including this macro in the application's initialization code
    // source file. If an application does not use this implementation, it 
    // must explicitly call NiWiiHeapManager::Initialize(...) prior to calling
    // NiInit() to ensure Gamebryo has access to at least one Wii memory heap.
    #define NiWiiSysAllocAndSysFreeImplementation \
        void* __sys_alloc(__std(size_t) blocksize ) \
        { \
            if (!NiWiiHeapManager::IsInitialized()) \
            { \
                NiWiiHeapManager::Initialize(); \
            } \
            return MEMAllocFromExpHeap(NiWiiHeapManager::GetHeap( \
                NI_WII_MEM1_ARENA), blocksize); \
        } \
        void __sys_free(void*  block) \
        { \
            if (!NiWiiHeapManager::IsInitialized()) \
            { \
                NiWiiHeapManager::Initialize(); \
            } \
            MEMFreeToExpHeap(NiWiiHeapManager::GetHeap(NI_WII_MEM1_ARENA), \
                block); \
        }
#endif // defined(__CWCC__)

#else
    #define NiExternalAlignedMalloc _aligned_malloc
    #define NiExternalAlignedRealloc _aligned_realloc
    #define NiExternalAlignedFree _aligned_free
    #define NiAlignOf(T) __alignof(T)
    #define NiStackAlloc(T, count) (T*)_malloca(sizeof(T)*(count))
    #define NiStackFree _freea
#endif

class NiAllocUnit;
NISYSTEM_ENTRY void _NiLogMemAssert(const char* pcCondition, 
    NiAllocUnit* pkUnit, const char* pcFile, int iLine, 
    const char* pcFunction);

#if defined(NI_MEMORY_DEBUGGER) && defined(NIDEBUG)
    #define NIMEMASSERT(value) {NIASSERT(value);}
    #define NIMEMASSERTUNIT(value, unit) {NIASSERT(value);}
#elif defined(NI_MEMORY_DEBUGGER) && !defined(NIDEBUG)
    #define NIMEMASSERT(value) {bool bValue = 0 != (value); \
        if (!bValue) {_NiLogMemAssert(#value, 0, __FILE__, __LINE__, \
        __FUNCTION__);}}
    #define NIMEMASSERTUNIT(value, unit) {bool bValue = (bool)(value); \
        if (!bValue) {_NiLogMemAssert(#value, pkUnit, __FILE__, __LINE__, \
        __FUNCTION__);}}
#else
    #define NIMEMASSERT(value) (void)0
    #define NIMEMASSERTUNIT(value, unit) (void)0
#endif // if defined(NI_MEMORY_DEBUGGER)

// Platform-specific allocation routines
#ifdef _XENON
#ifdef NI_MEMORY_DEBUGGER
    NISYSTEM_ENTRY void* _NiPhysicalAlloc(size_t stSizeInBytes, 
        size_t stDesiredPhysicalAddress, size_t stAlignment,
        unsigned int uiFlags, const char* pcSourceFile, int iSourceLine,
        const char* pcFunction);
    NISYSTEM_ENTRY void _NiPhysicalFree(void* p);
    #define NiPhysicalAlloc(size, desiredAddress, alignment, flags) \
        (_NiPhysicalAlloc(size, desiredAddress, alignment, flags, \
        __FILE__, __LINE__, __FUNCTION__))
    #define NiPhysicalFree(p) (_NiPhysicalFree(p))

#else
    NISYSTEM_ENTRY void* _NiPhysicalAlloc(size_t stSizeInBytes, 
        size_t stDesiredPhysicalAddress, size_t stAlignment,
        unsigned int uiFlags);
    NISYSTEM_ENTRY void _NiPhysicalFree(void* p);
    #define NiPhysicalAlloc(size, desiredAddress, alignment, flags) \
        (_NiPhysicalAlloc(size, desiredAddress, alignment, flags))
    #define NiPhysicalFree(p) (_NiPhysicalFree(p))

#endif // NI_MEMORY_DEBUGGER
#endif // _XENON
#endif // NIMEMORYDEFINES_H
