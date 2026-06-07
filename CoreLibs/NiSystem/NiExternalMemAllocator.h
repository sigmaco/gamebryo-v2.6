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

#ifndef NIEXTERNALMEMALLOCATOR_H
#define NIEXTERNALMEMALLOCATOR_H

#include "NiAllocator.h"


class NiExternalMemAllocator : public NiAllocator
{
public:
    virtual void* Allocate(size_t& stSize,
        size_t& stAlignment,
        NiMemHint kHint,
        NiMemEventType eEventType,
        const char* pcFile,
        int iLine,
        const char* pcFunction);

    virtual void Deallocate(void* pvMemory,
        NiMemEventType eEventType,
        size_t stSizeinBytes);

    virtual void* Reallocate(void* pvMemory,
        size_t& stSize,        
        size_t& stAlignment,
        NiMemHint kHint,
        NiMemEventType eEventType,
        size_t stSizeCurrent,
        const char* pcFile,
        int iLine,
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
};


//---------------------------------------------------------------------------
// Inline include
#include "NiExternalMemAllocator.inl"
//---------------------------------------------------------------------------

#endif // NIEXTERNALMEMALLOCATOR_H
