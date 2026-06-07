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

//---------------------------------------------------------------------------
// NiSmallObjectAllocator::NiSmallObjectAllocator
// Creates an allocator for small objects given chunk size and maximum 'small'
//     object size
//---------------------------------------------------------------------------
template <class TheAllocator>
NiTSmallAllocStrategy<TheAllocator>::NiTSmallAllocStrategy(size_t stChunkSize)
    : m_stChunkSize(stChunkSize)
{   
    for (size_t st = 1; st <= MAX_SMALL_OBJECT_SIZE; st++)
    {
        m_kPool[st-1].Init(&m_kAllocator, st);
    }
}
//---------------------------------------------------------------------------
// NiSmallObjectAllocator::Allocate
// Allocates 'numBytes' memory
// Uses an internal pool of NiFixedAllocator objects for small objects  
//---------------------------------------------------------------------------
template <class TheAllocator>
void* NiTSmallAllocStrategy<TheAllocator>::Allocate(
    size_t stNumBytes)
{
    NIASSERT(stNumBytes != 0);
    NIASSERT(stNumBytes <= MAX_SMALL_OBJECT_SIZE);
    NIASSERT(m_kPool[stNumBytes-1].BlockSize() == stNumBytes);
    return m_kPool[stNumBytes-1].Allocate();  
}
//---------------------------------------------------------------------------
// NiSmallObjectAllocator::Deallocate
// Deallocates memory previously allocated with Allocate
// (undefined behavior if you pass any other pointer)
//---------------------------------------------------------------------------
template <class TheAllocator>
void NiTSmallAllocStrategy<TheAllocator>::Deallocate(void* p,
    size_t stNumBytes)
{
    NIASSERT(stNumBytes != 0);
    NIASSERT(stNumBytes <= MAX_SMALL_OBJECT_SIZE);
    NIASSERT(m_kPool[stNumBytes-1].BlockSize() == stNumBytes);
    return m_kPool[stNumBytes-1].Deallocate(p);  
}
//---------------------------------------------------------------------------
// NiSmallObjectAllocator::GetFixedAllocatorForSize
// Returns the NiFixedAllocator for the given input byte size
//---------------------------------------------------------------------------
template <class TheAllocator>
NiFixedAllocator* NiTSmallAllocStrategy<TheAllocator>::
    GetFixedAllocatorForSize(size_t stNumBytes)
{
    NIASSERT(stNumBytes != 0);
    NIASSERT(stNumBytes <= MAX_SMALL_OBJECT_SIZE);
    NIASSERT(m_kPool[stNumBytes-1].BlockSize() == stNumBytes);
    return &m_kPool[stNumBytes-1];
}
//---------------------------------------------------------------------------
template <class TheAllocator>
size_t NiTSmallAllocStrategy<TheAllocator>::GetSizeFromAddress(
    void* pMemory)
{
    size_t stSize;
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    NIASSERT(sizeof(size_t) == 4);
    pcSize[0] = pcMemory[0];
    pcSize[1] = pcMemory[1];
    pcSize[2] = pcMemory[2];
    pcSize[3] = pcMemory[3];
    
    return stSize;
}
//---------------------------------------------------------------------------
template <class TheAllocator>
void NiTSmallAllocStrategy<TheAllocator>::SetSizeToAddress(
    void* pMemory, size_t stSize)
{
    char* pcMemory = (char*) pMemory;
    char* pcSize = (char*)&stSize;
    
    NIASSERT(sizeof(size_t) == 4);
    pcMemory[0] = pcSize[0];
    pcMemory[1] = pcSize[1];
    pcMemory[2] = pcSize[2];
    pcMemory[3] = pcSize[3];
}
//---------------------------------------------------------------------------
// Implementation of NiAllocator 
//---------------------------------------------------------------------------
template <class TheAllocator>
void* NiTSmallAllocStrategy<TheAllocator>::Allocate(
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemHint kHint,
    NiMemEventType eEventType,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    NIASSERT(NI_IS_POWER_OF_TWO(stAlignment));

    bool bPrependSize = false;
    void* pvMemory = NULL;

    if (stAlignment == NI_MEM_ALIGNMENT_DEFAULT && 
        (kHint.CompilerProvidesSizeOnDeallocate() == false) &&
        eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC)
    {
        stSizeInBytes += NI_MEM_ALIGNMENT_DEFAULT;
        bPrependSize = true;
    }

    if (eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC && 
        stSizeInBytes <= MAX_SMALL_OBJECT_SIZE && 
        stAlignment == NI_MEM_ALIGNMENT_DEFAULT)
    {
        pvMemory = Allocate(stSizeInBytes);
    }
    else
    {
        pvMemory = m_kAllocator.Allocate(
            stSizeInBytes,
            stAlignment,
            kHint,
            eEventType,
            pcFile,
            iLine,
            pcFunction);
    }

    if (pvMemory && bPrependSize)
    {
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //*((size_t*) pvMemory) = stSizeInBytes;
        SetSizeToAddress(pvMemory, stSizeInBytes);
        pvMemory = ((char*)pvMemory) + NI_MEM_ALIGNMENT_DEFAULT;
    }

    return pvMemory;
}
//---------------------------------------------------------------------------
template <class TheAllocator>
void NiTSmallAllocStrategy<TheAllocator>::Deallocate(
    void* pvMemory, 
    NiMemEventType eEventType, 
    size_t stSizeInBytes)
{
    if (pvMemory == NULL)
        return;

    if (eEventType != NI_ALIGNEDFREE && eEventType != NI_ALIGNEDREALLOC && 
        NI_MEM_DEALLOC_SIZE_DEFAULT == stSizeInBytes)
    {
        pvMemory = ((char*)pvMemory) - NI_MEM_ALIGNMENT_DEFAULT;
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //stSizeInBytes = *((size_t*) pvMemory); 
        stSizeInBytes = GetSizeFromAddress(pvMemory);
    }

    if (stSizeInBytes <= MAX_SMALL_OBJECT_SIZE)
    {
        Deallocate(pvMemory, stSizeInBytes);
        return;
    }

    m_kAllocator.Deallocate(pvMemory, eEventType, stSizeInBytes);
}
//---------------------------------------------------------------------------
template <class TheAllocator>
void* NiTSmallAllocStrategy<TheAllocator>::Reallocate(
    void* pvMemory,
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemHint kHint,
    NiMemEventType eEventType,
    size_t stSizeCurrent,
    const char* pcFile,
    int iLine,
    const char* pcFunction)
{
    NIASSERT(NI_IS_POWER_OF_TWO(stAlignment));

    // The deallocation case should have been caught by us before in
    // the allocation functions.
    NIASSERT(stSizeInBytes != 0);
    
    // Determine the current size for the address
    if (eEventType != NI_ALIGNEDFREE && eEventType != NI_ALIGNEDREALLOC && 
        NI_MEM_DEALLOC_SIZE_DEFAULT == stSizeCurrent && 
        (kHint.CompilerProvidesSizeOnDeallocate() == false))
    {
        pvMemory = ((char*)pvMemory) - NI_MEM_ALIGNMENT_DEFAULT;
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //stSizeCurrent = *((size_t*) pvMemory); 
        stSizeCurrent = GetSizeFromAddress(pvMemory);
    }

    // pad the allocation for the new current size
    bool bPrependSize = false;
    if (stAlignment == NI_MEM_ALIGNMENT_DEFAULT && 
        (kHint.CompilerProvidesSizeOnDeallocate() == false) &&
        eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC)
    {
        stSizeInBytes += NI_MEM_ALIGNMENT_DEFAULT;
        bPrependSize = true;
    }

    if (stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT && 
        stSizeInBytes <= stSizeCurrent && bPrependSize)
    {
        stSizeInBytes = stSizeCurrent;
        pvMemory = ((char*)pvMemory) + NI_MEM_ALIGNMENT_DEFAULT;
        return pvMemory;
    }

    void* pvNewMemory = NULL;

    if (eEventType != NI_ALIGNEDMALLOC &&
        eEventType != NI_ALIGNEDREALLOC &&
        (stSizeCurrent <= MAX_SMALL_OBJECT_SIZE || 
        stSizeInBytes <= MAX_SMALL_OBJECT_SIZE))
    {
        NIASSERT(stAlignment ==  NI_MEM_ALIGNMENT_DEFAULT);
        NIASSERT(stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT);

        // Since the small object allocator does not support reallocation
        // we must manually reallocate.

        // Remove any header information that we have added
        // in this call.
        stSizeInBytes -= NI_MEM_ALIGNMENT_DEFAULT;

        // Allow the allocation call to do its job, which
        // should mean that the size will be embedded just before the
        // address that we receive.
        pvNewMemory = Allocate(stSizeInBytes, stAlignment, kHint, eEventType,
            pcFile, iLine, pcFunction);
        bPrependSize = false;

        if (stSizeCurrent != NI_MEM_DEALLOC_SIZE_DEFAULT)
        {
            char* pvMemoryToCopy = ((char*)pvMemory) + 
                NI_MEM_ALIGNMENT_DEFAULT;
            size_t stSizeToCopy = stSizeCurrent - NI_MEM_ALIGNMENT_DEFAULT;

            NiMemcpy(pvNewMemory, stSizeInBytes, pvMemoryToCopy, stSizeToCopy);
            Deallocate(pvMemory, eEventType, stSizeCurrent);
        }
    }
    else
    {
        pvNewMemory = m_kAllocator.Reallocate(
            pvMemory,
            stSizeInBytes,
            stAlignment,
            kHint,
            eEventType,
            stSizeCurrent,
            pcFile,
            iLine,
            pcFunction);
    }

    if (pvNewMemory && bPrependSize)
    {
        NIASSERT(NI_MEM_ALIGNMENT_DEFAULT >= sizeof(size_t));
        //*((size_t*) pvNewMemory) = stSizeInBytes;
        SetSizeToAddress(pvNewMemory, stSizeInBytes);
        pvNewMemory = ((char*)pvNewMemory) + NI_MEM_ALIGNMENT_DEFAULT;
    }

    return pvNewMemory;
}
//---------------------------------------------------------------------------
template <class TheAllocator>
bool NiTSmallAllocStrategy<TheAllocator>::TrackAllocate(
    const void* const pvMemory, 
    size_t stSizeInBytes, 
    NiMemHint kHint,
    NiMemEventType eEventType, 
    const char* pcFile, 
    int iLine,
    const char* pcFunction)
{
    return m_kAllocator.TrackAllocate(
        pvMemory, 
        stSizeInBytes, 
        kHint,
        eEventType, 
        pcFile, 
        iLine,
        pcFunction);
}
//---------------------------------------------------------------------------
template <class TheAllocator>
bool NiTSmallAllocStrategy<TheAllocator>::TrackDeallocate(
    const void* const pvMemory, 
    NiMemEventType eEventType)
{
    return m_kAllocator.TrackDeallocate(pvMemory, eEventType);
}
//---------------------------------------------------------------------------
template <class TheAllocator>
bool NiTSmallAllocStrategy<TheAllocator>::SetMarker(const char* pcMarkerType,
    const char* pcClassifier, const char* pcString)
{
    return m_kAllocator.SetMarker(pcMarkerType, pcClassifier, pcString);
}
//---------------------------------------------------------------------------
template <class TheAllocator>
void NiTSmallAllocStrategy<TheAllocator>::Initialize()
{
    m_kAllocator.Initialize();
}
//---------------------------------------------------------------------------
template <class TheAllocator>
void NiTSmallAllocStrategy<TheAllocator>::Shutdown()
{
    m_kAllocator.Shutdown();
}
//---------------------------------------------------------------------------
template <class TheAllocator>
bool NiTSmallAllocStrategy<TheAllocator>::VerifyAddress(
    const void* pvMemory)
{
    return m_kAllocator.VerifyAddress(pvMemory);
}
//---------------------------------------------------------------------------
