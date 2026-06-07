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

// Precompiled Header
#include "NiSystemPCH.h"

#include "NiMemObject.h"
#include "NiMemManager.h"

#if !defined(NI_DISABLE_EXCEPTIONS)
#include <exception> // for std::bad_alloc
#include <new>
#endif

//---------------------------------------------------------------------------
#if defined(NI_MEMORY_DEBUGGER)
    void* NiMemObject::operator new(size_t, NiMemHint)
#if defined(NI_DISABLE_EXCEPTIONS)
        throw()
#endif
    {
#if defined(NI_DISABLE_EXCEPTIONS)
        return NULL;
#else
        throw std::bad_alloc();
#endif
    }

    void* NiMemObject::operator new[](size_t, NiMemHint)
#if defined(NI_DISABLE_EXCEPTIONS)
        throw()
#endif
    {
#if defined(NI_DISABLE_EXCEPTIONS)
        return NULL;
#else
        throw std::bad_alloc();
#endif
    }

    void* NiMemObject::operator new(
        size_t stSizeInBytes, 
        NiMemHint kHint,
        const char* pcSourceFile, 
        int iSourceLine, 
        const char* pcFunction)
    {
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

        kHint |= NiMemHint::COMPILER_PROVIDES_SIZE_ON_DEALLOCATE;
        void* p = NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, kHint, NI_OPER_NEW,
            pcSourceFile, iSourceLine, pcFunction);

#if !defined(NI_DISABLE_EXCEPTIONS)
        if (p == 0)
            throw std::bad_alloc();
#endif

        return p;
    }

    void* NiMemObject::operator new[](
        size_t stSizeInBytes,
        NiMemHint kHint,
        const char* pcSourceFile, 
        int iSourceLine, 
        const char* pcFunction)
    {
       
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

#if defined(_PS3)
        // On GCC the allocated size is passed into operator delete[] 
        // so there is no need for the allocator to save the size of
        // the allocation.  
        kHint |= NiMemHint::COMPILER_PROVIDES_SIZE_ON_DEALLOCATE;
#endif

        void* p = NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, kHint, NI_OPER_NEW_ARRAY,
            pcSourceFile, iSourceLine, pcFunction);        

#if !defined(NI_DISABLE_EXCEPTIONS)
        if (p == 0)
            throw std::bad_alloc();
#endif

        return p;
    }
#else
    void* NiMemObject::operator new(size_t stSizeInBytes,
        NiMemHint kHint)
    {
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;
        
        kHint |= NiMemHint::COMPILER_PROVIDES_SIZE_ON_DEALLOCATE;
        void* p =  NiMemManager::Get().Allocate(stSizeInBytes,
            NI_MEM_ALIGNMENT_DEFAULT, kHint, NI_OPER_NEW);

#if !defined(NI_DISABLE_EXCEPTIONS)
        if (p == 0)
            throw std::bad_alloc();
#endif

        return p;
    }

    void* NiMemObject::operator new[](size_t stSizeInBytes,
        NiMemHint kHint)
    {
        NIASSERT(NiMemManager::IsInitialized());
        if (stSizeInBytes == 0)
            stSizeInBytes = 1;

#if defined(_PS3)
        // On GCC the allocated size is passed into operator delete[] 
        // so there is no need for the allocator to save the size of
        // the allocation.
        kHint |= NiMemHint::COMPILER_PROVIDES_SIZE_ON_DEALLOCATE;
#endif

        void* p =  NiMemManager::Get().Allocate(stSizeInBytes, 
            NI_MEM_ALIGNMENT_DEFAULT, kHint, NI_OPER_NEW_ARRAY);
        
#if !defined(NI_DISABLE_EXCEPTIONS)
        if (p == 0)
            throw std::bad_alloc();
#endif

        return p;
    }
#endif
//---------------------------------------------------------------------------
void NiMemObject::operator delete(void* pvMem, size_t stElementSize)
{
    if (pvMem)
    {
        NIASSERT(NiMemManager::IsInitialized());
        NiMemManager::Get().Deallocate(pvMem, NI_OPER_DELETE, stElementSize);
    }
}
//---------------------------------------------------------------------------
#ifdef _PS3
// On GCC the allocated size is passed into operator delete[] so there is no
// need for the allocator to save the size of the allocation.  
void NiMemObject::operator delete[](void* pvMem, size_t stElementSize)
{
    if (pvMem)
    {
        NIASSERT(NiMemManager::IsInitialized());       
        NiMemManager::Get().Deallocate(pvMem, NI_OPER_DELETE_ARRAY,
            stElementSize);
    } 
}
#else
void NiMemObject::operator delete[](void* pvMem, size_t)
{
    if (pvMem)
    {
        NIASSERT(NiMemManager::IsInitialized());       
        NiMemManager::Get().Deallocate(pvMem, NI_OPER_DELETE_ARRAY);
    } 
}
#endif
//---------------------------------------------------------------------------
