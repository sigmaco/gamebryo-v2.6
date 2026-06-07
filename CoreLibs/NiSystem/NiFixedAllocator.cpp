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
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above 
//     copyright notice appear in all copies and that both that copyright 
//     notice and this permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
//---------------------------------------------------------------------------

// Precompiled Header
#include "NiSystemPCH.h"

#include "NiFixedAllocator.h"
#include "NiAllocator.h"
#include "NiBuildConfiguration.h"
#include <algorithm>

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::Init
// Initializes a chunk object
//---------------------------------------------------------------------------

void NiFixedAllocator::Chunk::Init(NiAllocator* pkAllocator, 
    size_t stBlockSize, unsigned char ucBlocks)
{
    NIASSERT(pkAllocator);
    NIASSERT(stBlockSize > 0);
    NIASSERT(ucBlocks > 0);
    // Overflow check
    NIASSERT((stBlockSize * ucBlocks) / stBlockSize == ucBlocks);

    m_pkAllocator = pkAllocator;

    size_t stSizeInBytes = sizeof(unsigned char)*stBlockSize*ucBlocks;
    size_t stAlignment = NI_MEM_ALIGNMENT_DEFAULT;

    m_pucData = reinterpret_cast<unsigned char*>(m_pkAllocator->Allocate( 
        stSizeInBytes,
        stAlignment,
        NiMemHint::USAGE_POOL,
        NI_ALIGNEDMALLOC,
        __FILE__, __LINE__, __FUNCTION__));

#if defined(_DEBUG)
    if (m_pucData == 0)
        NiOutputDebugString("WARNING: MEM1 is full.  "
            "No more small allocator chunks can be created.\n");
#endif

    NIASSERT(m_pucData);

    Reset(stBlockSize, ucBlocks);
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::Reset
// Clears an already allocated chunk
//---------------------------------------------------------------------------

void NiFixedAllocator::Chunk::Reset(size_t stBlockSize,
    unsigned char ucBlocks)
{
    NIASSERT(stBlockSize > 0);
    NIASSERT(ucBlocks > 0);
    // Overflow check
    NIASSERT((stBlockSize * ucBlocks) / stBlockSize == ucBlocks);

    m_ucFirstAvailableBlock = 0;
    m_ucBlocksAvailable = ucBlocks;

    unsigned char i = 0;
    unsigned char* p = m_pucData;
    for (; i != ucBlocks; p += stBlockSize)
    {
        *p = ++i;
    }
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::Release
// Releases the data managed by a chunk
//---------------------------------------------------------------------------

void NiFixedAllocator::Chunk::Release()
{
    NIASSERT(m_pkAllocator);

    m_pkAllocator->Deallocate(m_pucData, NI_ALIGNEDFREE, 0);
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::Allocate
// Allocates a block from a chunk
//---------------------------------------------------------------------------

void* NiFixedAllocator::Chunk::Allocate(size_t stBlockSize)
{
    if (!m_ucBlocksAvailable) 
        return 0;
    
    NIASSERT((m_ucFirstAvailableBlock * stBlockSize) / stBlockSize == 
        m_ucFirstAvailableBlock);

    unsigned char* pucResult =
        m_pucData + (m_ucFirstAvailableBlock * stBlockSize);
    m_ucFirstAvailableBlock = *pucResult;
    --m_ucBlocksAvailable;
    
    return pucResult;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::Deallocate
// Dellocates a block from a chunk
//---------------------------------------------------------------------------

void NiFixedAllocator::Chunk::Deallocate(void* p, size_t stBlockSize)
{
    NIASSERT(p >= m_pucData);

    unsigned char* toRelease = static_cast<unsigned char*>(p);
    // Alignment check
    NIASSERT((toRelease - m_pucData) % stBlockSize == 0);

    *toRelease = m_ucFirstAvailableBlock;
    m_ucFirstAvailableBlock = static_cast<unsigned char>(
        (toRelease - m_pucData) / stBlockSize);
    // Truncation check
    NIASSERT(m_ucFirstAvailableBlock == (toRelease - m_pucData) / stBlockSize);

    ++m_ucBlocksAvailable;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::HasAvailable
//---------------------------------------------------------------------------
bool NiFixedAllocator::Chunk::HasAvailable(unsigned char ucNumBlocks) const
{
    return m_ucBlocksAvailable == ucNumBlocks;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::HasAvailable
//---------------------------------------------------------------------------
bool NiFixedAllocator::Chunk::HasBlock(unsigned char * p, 
    size_t stChunkLength) const
{
    return ( m_pucData <= p ) && ( p < m_pucData + stChunkLength ); 
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Chunk::HasAvailable
//---------------------------------------------------------------------------
bool NiFixedAllocator::Chunk::IsFilled( void ) const
{
    return ( 0 == m_ucBlocksAvailable );
}

//---------------------------------------------------------------------------
// NiFixedAllocator::NiFixedAllocator
//---------------------------------------------------------------------------

NiFixedAllocator::NiFixedAllocator()
    : m_pkAllocator(0)
    , m_stBlockSize(0)
    , m_pkChunks(0)
    , m_stNumChunks(0)
    , m_stMaxNumChunks(0)
    , m_pkAllocChunk(0)
    , m_pkDeallocChunk(0)
    , m_pkEmptyChunk(0)
{
}

//---------------------------------------------------------------------------
// Creates a NiFixedAllocator object of a fixed block size
//---------------------------------------------------------------------------
void NiFixedAllocator::Init(NiAllocator* pkAllocator, size_t stBlockSize)
{
    NIASSERT(pkAllocator);

    m_pkAllocator = pkAllocator;
    m_stNumChunks  = 0;
    m_stMaxNumChunks = 0;
    m_pkChunks = NULL;
    m_pkAllocChunk = NULL;
    m_stBlockSize = stBlockSize;
    size_t stNumBlocks = DEFAULT_CHUNK_SIZE / stBlockSize;

    if (stNumBlocks > UCHAR_MAX) 
        stNumBlocks = UCHAR_MAX;
    else if (stNumBlocks == 0) 
        stNumBlocks = 8 * stBlockSize;
    
    m_ucNumBlocks = static_cast<unsigned char>(stNumBlocks);
    NIASSERT(m_ucNumBlocks == stNumBlocks);
}

//---------------------------------------------------------------------------
// NiFixedAllocator::~NiFixedAllocator
//---------------------------------------------------------------------------

NiFixedAllocator::~NiFixedAllocator()
{  
    NIASSERT(m_pkAllocator);

    for (size_t i = 0; i < m_stNumChunks; ++i)
    {
       // sckime - This test has been commented out because the 
       // NiMemTracker will report any leaks.
       // NIASSERT(i->m_ucBlocksAvailable == m_ucNumBlocks);
       m_pkChunks[i].Release();
    }

    m_pkAllocator->Deallocate(m_pkChunks, NI_ALIGNEDFREE, 0);
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Push_Back
// Adds a chunk to the end of the chunks array
//---------------------------------------------------------------------------
void NiFixedAllocator::Push_Back(Chunk& kChunk)
{
    size_t stCount = m_stNumChunks;
    Reserve(m_stNumChunks+1);
    m_pkChunks[stCount] = kChunk;
    m_stNumChunks++;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Push_Back
// Adds a chunk to the end of the chunks array
//---------------------------------------------------------------------------
void NiFixedAllocator::Pop_Back()
{
    --m_stNumChunks;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Reserve
// Guarantees space for a certain number of chunks
//---------------------------------------------------------------------------
void NiFixedAllocator::Reserve(size_t stNewSize)
{
    NIASSERT(m_pkAllocator);

    if (stNewSize > m_stMaxNumChunks)
    {
        size_t stNewSizeInBytes = stNewSize*sizeof(Chunk);
        size_t stCurrentSizeInBytes = m_stMaxNumChunks*sizeof(Chunk);
        size_t stAlignment = NI_MEM_ALIGNMENT_DEFAULT;

        m_pkChunks = reinterpret_cast<Chunk*>(m_pkAllocator->Reallocate(
            m_pkChunks, 
            stNewSizeInBytes,
            stAlignment,
            NiMemHint::USAGE_POOL,
            NI_ALIGNEDREALLOC, 
            stCurrentSizeInBytes,
            __FILE__, __LINE__, __FUNCTION__));

        NIASSERT(m_pkChunks);

        m_stMaxNumChunks = stNewSize;
    }
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Allocate
// Allocates a block of fixed size
//---------------------------------------------------------------------------

void* NiFixedAllocator::Allocate()
{
    m_kCriticalSection.Lock();
    // Prove that the empty chunk points to either nothing or
    // a chunk with no elements allocated.
    NIASSERT( (NULL == m_pkEmptyChunk) || 
            (m_pkEmptyChunk->HasAvailable(m_ucNumBlocks)));

    if (m_pkAllocChunk && m_pkAllocChunk->IsFilled())
        m_pkAllocChunk = NULL;

    // Recycle the empty chunk if possible
    if (NULL != m_pkEmptyChunk)
    {
        m_pkAllocChunk = m_pkEmptyChunk;
        m_pkEmptyChunk = NULL;
    }

    if (m_pkAllocChunk == 0)
    {
        for (size_t i = 0; i < m_stNumChunks; ++i)
        {
            if (!m_pkChunks[i].IsFilled())
            {
                m_pkAllocChunk = &m_pkChunks[i];
                break;
            }
        }

        // If no alloc chunk has space,
        // add an alloc chunk
        if (NULL == m_pkAllocChunk)
        {
            // Initialize
            Reserve(m_stNumChunks + 1);
            Chunk newChunk;
            newChunk.Init(m_pkAllocator, m_stBlockSize, m_ucNumBlocks);
            Push_Back(newChunk);
            m_pkAllocChunk = &m_pkChunks[m_stNumChunks - 1];
            m_pkDeallocChunk = &m_pkChunks[0];
        }
    }
    NIASSERT(m_pkAllocChunk != 0);
    NIASSERT(!m_pkAllocChunk->IsFilled() );

    // Prove that the empty chunk points to either nothing or
    // a chunk with no elements allocated.
    NIASSERT( (NULL == m_pkEmptyChunk) || 
            (m_pkEmptyChunk->HasAvailable(m_ucNumBlocks)));

    
    void* pvMem = m_pkAllocChunk->Allocate(m_stBlockSize);
    m_kCriticalSection.Unlock();
    return pvMem;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::Deallocate
// Deallocates a block previously allocated with Allocate
// (undefined behavior if called with the wrong pointer)
//---------------------------------------------------------------------------

void NiFixedAllocator::Deallocate(void* p)
{
    m_kCriticalSection.Lock();
    NIASSERT(m_stNumChunks != 0);
    NIASSERT(&m_pkChunks[0] <= m_pkDeallocChunk);
    NIASSERT(&m_pkChunks[m_stNumChunks - 1] >= m_pkDeallocChunk);
    
    m_pkDeallocChunk  = VicinityFind(p);
    NIASSERT(m_pkDeallocChunk);

    DoDeallocate(p);
    m_kCriticalSection.Unlock();
}

//---------------------------------------------------------------------------
// NiFixedAllocator::VicinityFind (internal)
// Finds the chunk corresponding to a pointer, using an efficient search
//---------------------------------------------------------------------------

NiFixedAllocator::Chunk* NiFixedAllocator::VicinityFind(void* p)
{
    NIASSERT(m_stNumChunks != 0);
    NIASSERT(m_pkDeallocChunk);

    const size_t stChunkLength = m_ucNumBlocks * m_stBlockSize;

    Chunk* lo = m_pkDeallocChunk;
    Chunk* hi = m_pkDeallocChunk + 1;
    Chunk* loBound = &m_pkChunks[0];
    Chunk* hiBound = &m_pkChunks[m_stNumChunks - 1] + 1;

    if (hi == hiBound)
        hi = 0;

    for (;;)
    {
        if (lo)
        {
            if ( lo->HasBlock( (unsigned char*)p, stChunkLength ) )
                return lo;

            if ( lo == loBound )
            {
                lo = NULL;
                if ( NULL == hi ) 
                    break;
            }
            else 
                --lo;

        }
        
        if (hi)
        {
            if ( hi->HasBlock( (unsigned char*)p, stChunkLength ) ) 
                return hi;
            if ( ++hi == hiBound )
            {
                hi = NULL;
                if ( NULL == lo )
                    break;
            }

        }
    }
    
    NIASSERT(!"Could not find pointer p in "
        "NiFixedAllocator::VicinityFind()");
    return 0;
}

//---------------------------------------------------------------------------
// NiFixedAllocator::DoDeallocate (internal)
// Performs deallocation. Assumes m_pkDeallocChunk points to the correct chunk
//---------------------------------------------------------------------------

void NiFixedAllocator::DoDeallocate(void* p)
{
    NIASSERT(m_pkDeallocChunk->m_pucData <= p);
    NIASSERT( m_pkDeallocChunk->HasBlock(static_cast<unsigned char *>(p),
        m_ucNumBlocks * m_stBlockSize));

    // prove either m_pkEmptyChunk points nowhere, or points to a truly empty 
    // Chunk.
    NIASSERT((NULL == m_pkEmptyChunk) || 
           (m_pkEmptyChunk->HasAvailable(m_ucNumBlocks)));

    // call into the chunk, will adjust the inner list but won't release memory
    m_pkDeallocChunk->Deallocate(p, m_stBlockSize);

    if (m_pkDeallocChunk->HasAvailable(m_ucNumBlocks))
    {
        NIASSERT(m_pkEmptyChunk != m_pkDeallocChunk);

        // m_pkDeallocChunk is empty, but a Chunk is only released if there 
        // are 2 empty chunks.  Since m_pkEmptyChunk may only point to a 
        // previously cleared Chunk, if it points to something else 
        // besides m_pkDeallocChunk, then FixedAllocator currently has 2 empty
        // Chunks.
        if (NULL != m_pkEmptyChunk)
        {
            // If last Chunk is empty, just change what m_pkDeallocChunk
            // points to, and release the last.  Otherwise, swap an empty
            // Chunk with the last, and then release it.
            Chunk* pkLastChunk = &m_pkChunks[m_stNumChunks - 1];
            
            if (pkLastChunk == m_pkDeallocChunk)
                m_pkDeallocChunk = m_pkEmptyChunk;
            else if (pkLastChunk != m_pkEmptyChunk )
                std::swap( *m_pkEmptyChunk, *pkLastChunk);

            NIASSERT(pkLastChunk->HasAvailable(m_ucNumBlocks));
            pkLastChunk->Release();
            Pop_Back();
            m_pkAllocChunk = m_pkDeallocChunk;
        }
        m_pkEmptyChunk = m_pkDeallocChunk;
    }

    // prove either m_pkEmptyChunk points nowhere, or points to a truly empty 
    // Chunk.
    NIASSERT((NULL == m_pkEmptyChunk) || 
           (m_pkEmptyChunk->HasAvailable(m_ucNumBlocks)));
}
//---------------------------------------------------------------------------
