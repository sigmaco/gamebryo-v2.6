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

//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////

// Last update: June 20, 2001

#ifndef NIFIXEDALLOCATOR_H
#define NIFIXEDALLOCATOR_H

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 25600
#endif

#include <NiRTLib.h>
#include <NiSystem.h>

class NiAllocator;

//---------------------------------------------------------------------------
// class NiFixedAllocator
// Offers services for allocating fixed-sized objects
//---------------------------------------------------------------------------
class NISYSTEM_ENTRY NiFixedAllocator
{
private:
    struct NISYSTEM_ENTRY Chunk
    {
    public:
        void Init(NiAllocator* pkAllocator, size_t stBlockSize, 
            unsigned char ucBlocks);
        void* Allocate(size_t stBlockSize);
        void Deallocate(void* p, size_t stBlockSize);
        void Reset(size_t stBlockSize, unsigned char ucBlocks);
        void Release();
        bool HasAvailable(unsigned char ucNumBlocks) const;
        bool HasBlock(unsigned char * p, size_t stChunkLength) const;
        bool IsFilled( void ) const;

        unsigned char* m_pucData;
        unsigned char m_ucFirstAvailableBlock;
        unsigned char m_ucBlocksAvailable;

    private:
        NiAllocator* m_pkAllocator;
    };

    void Push_Back(Chunk& kChunk);
    void Pop_Back();
    void Reserve(size_t stNewSize);
        
    // Internal functions        
    void DoDeallocate(void* p);
    Chunk* VicinityFind(void* p);
    
    // Data 
    NiAllocator* m_pkAllocator;
    size_t m_stBlockSize;
    unsigned char m_ucNumBlocks;
    
    Chunk* m_pkChunks;
    size_t m_stNumChunks;
    size_t m_stMaxNumChunks;
    Chunk* m_pkAllocChunk;
    Chunk* m_pkDeallocChunk;
    Chunk* m_pkEmptyChunk;
    NiCriticalSection m_kCriticalSection;
public:
    NiFixedAllocator();
    ~NiFixedAllocator();

    // Create a NiFixedAllocator able to manage blocks of 'blockSize' size
    void Init(NiAllocator* pkAllocator, size_t stBlockSize);

    // Allocate a memory block
    void* Allocate();

    // Deallocate a memory block previously allocated with Allocate()
    // (if that's not the case, the behavior is undefined)
    void Deallocate(void* p);

    // Returns the block size with which the NiFixedAllocator was initialized
    size_t BlockSize() const
    { return m_stBlockSize; }

    // Comparison operator for sorting 
    bool operator<(size_t rhs) const
    { return BlockSize() < rhs; }

    // Returns the number of chunks allocated
    size_t GetNumChunks() const
    { return m_stNumChunks;}
};

#endif // NIFIXEDALLOCATOR_H
