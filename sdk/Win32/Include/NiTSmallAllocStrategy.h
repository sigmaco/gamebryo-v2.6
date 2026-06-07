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

#ifndef NITSMALLALLOCSTRATEGY_H
#define NITSMALLALLOCSTRATEGY_H

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 256
#endif

#include <NiRTLib.h>
#include <NiSystem.h>
#include "NiFixedAllocator.h"
#include "NiMemoryDefines.h"

template <class TheAllocator>
class NiTSmallAllocStrategy : public NiAllocator
{
public:
    NiTSmallAllocStrategy(size_t stChunkSize = DEFAULT_CHUNK_SIZE);

    // Methods defined by NiAllocator
    virtual void* Allocate(
        size_t& stSizeInBytes, 
        size_t& stAlignment,
        NiMemHint kHint,
        NiMemEventType eEventType, 
        const char* pcFile, 
        int iLine,
        const char* pcFunction);

    virtual void Deallocate(
        void* pvMemory, 
        NiMemEventType eEventType, 
        size_t stSizeInBytes);

    virtual void* Reallocate(
        void* pvMemory, 
        size_t& stSizeInBytes,
        size_t& stAlignment, 
        NiMemHint kHint,
        NiMemEventType eEventType, 
        size_t stSizeCurrent,
        const char* pcFile, int iLine,
        const char* pcFunction);

    virtual bool TrackAllocate(
        const void* const pvMemory, 
        size_t stSizeInBytes, 
        NiMemHint kHint,
        NiMemEventType eEventType, 
        const char* pcFile, 
        int iLine,
        const char* pcFunction);

    virtual bool TrackDeallocate(
        const void* const pvMemory, 
        NiMemEventType eEventType);

    virtual bool SetMarker(const char* pcMarkerType, const char* pcClassifier,
        const char* pcString);

    virtual void Initialize();
    virtual void Shutdown();
    virtual bool VerifyAddress(const void* pvMemory);

private:
    NiTSmallAllocStrategy(const NiTSmallAllocStrategy&){};
    NiTSmallAllocStrategy& operator=(const NiTSmallAllocStrategy&){
        return *(this);};

    void* Allocate(size_t stNumBytes);
    void Deallocate(void* p, size_t stSize);
    NiFixedAllocator* GetFixedAllocatorForSize(size_t stNumBytes);
    size_t GetSizeFromAddress(void* pMemory);
    void SetSizeToAddress(void* pMemory, size_t stSize);

    TheAllocator m_kAllocator;
    NiFixedAllocator m_kPool[MAX_SMALL_OBJECT_SIZE];
    size_t m_stChunkSize;
};

#include "NiTSmallAllocStrategy.inl"

#endif // NITSMALLALLOCSTRATEGY_H
