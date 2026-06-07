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
//---------------------------------------------------------------------------

#ifndef NITABSTRACTPOOLALLOCATOR_H
#define NITABSTRACTPOOLALLOCATOR_H

#include <NiRefObject.h>
#include <NiRTLib.h>
#include <NiCriticalSection.h>

#define NiAllocatorShutdown(T) NiTAbstractPoolAllocator<T>::_SDMShutdown();

#if defined(_PS3) || defined(_WII)
#ifdef NIDEBUG
#define NiAllocatorDeclareStatics(T, BLOCKSIZE) \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkBlockHeader = NULL; \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkFreeMem = NULL; \
        template <> NiCriticalSection \
            NiTAbstractPoolAllocator<T>::ms_kCriticalSection = 0; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiBlockSize = BLOCKSIZE; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiAllocated = 0; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiMaxAllocated = 0;
#else
#define NiAllocatorDeclareStatics(T, BLOCKSIZE) \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkBlockHeader = NULL; \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkFreeMem = NULL; \
        template <> NiCriticalSection \
            NiTAbstractPoolAllocator<T>::ms_kCriticalSection = 0; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiBlockSize = BLOCKSIZE; 
#endif
#else
#ifdef NIDEBUG
    #define NiAllocatorDeclareStatics(T, BLOCKSIZE) \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkBlockHeader = NULL; \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkFreeMem = NULL; \
        template <> NiCriticalSection \
            NiTAbstractPoolAllocator<T>::ms_kCriticalSection; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiBlockSize = BLOCKSIZE; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiAllocated = 0; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiMaxAllocated = 0;
#else
    #define NiAllocatorDeclareStatics(T, BLOCKSIZE) \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkBlockHeader = NULL; \
        template <> NiTAbstractPoolAllocator<T>::AllocNode* \
            NiTAbstractPoolAllocator<T>::ms_pkFreeMem = NULL; \
        template <> unsigned int \
            NiTAbstractPoolAllocator<T>::ms_uiBlockSize = BLOCKSIZE; \
        template <> NiCriticalSection \
            NiTAbstractPoolAllocator<T>::ms_kCriticalSection;
#endif
#endif

#define NiAllocatorSetBlockSize(T, BLOCKSIZE) \
    NiTAbstractPoolAllocator<T>::SetBlockSize(BLOCKSIZE);
#define NiAllocatorEnsureFreeMem(T) \
    NiTAbstractPoolAllocator<T>::EnsureFreeMem();
                           
template <class T> class NiTAbstractPoolAllocator
{
public: 

    // Virtuals are not used because they add additional memory for each
    // class derived. This class can be thought of as abstract with the
    // following commented classes as pure virtuals.
    // virtual void* Allocate() = 0;
    // virtual void Deallocate (void* p) = 0; 
  
  static void _SDMShutdown();
  static void SetBlockSize(unsigned int uiBlockSize);
  static void EnsureFreeMem();
  static void CreateFreeMem();

protected:
    // list is doubly-linked
    class AllocNode : public NiMemObject
  {
    public:
        AllocNode* m_pkNext;
        void* m_pkData;
        T m_element;
  };

  static AllocNode* ms_pkBlockHeader;
  static AllocNode* ms_pkFreeMem;
  static unsigned int ms_uiBlockSize;

  static NiCriticalSection ms_kCriticalSection;

#ifdef NIDEBUG
  static unsigned int ms_uiAllocated;
  static unsigned int ms_uiMaxAllocated;
#endif
};

//---------------------------------------------------------------------------
// Inline include
#include "NiTAbstractPoolAllocator.inl"
//---------------------------------------------------------------------------

#endif
