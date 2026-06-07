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

#include "NiMemoryDefines.h"
#include "NiMemManager.h"
#include "NiMemTracker.h"

//---------------------------------------------------------------------------
// _NiMalloc (see notes above)
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
void* _NiMalloc(size_t stSizeInBytes, NiMemHint kHint,
    const char* pcSourceFile, int iSourceLine,  const char* pcFunction)
#else
void* _NiMalloc(size_t stSizeInBytes, NiMemHint kHint)
#endif
{
    NIASSERT(NiMemManager::IsInitialized());

    if (stSizeInBytes == 0)
        stSizeInBytes = 1;

    // Actually allocate the memory
    void* pvMem = NiMemManager::Get().Allocate(stSizeInBytes, 
        NI_MEM_ALIGNMENT_DEFAULT, kHint, NI_MALLOC
#ifdef NI_MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
        );
    
    NIASSERT(pvMem != NULL);
    
    // Return the allocated memory advanced by the size of the header
    return pvMem;
}
//---------------------------------------------------------------------------
// _NiAlignedMalloc (see notes above)
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
void* _NiAlignedMalloc(
    size_t stSizeInBytes, 
    size_t stAlignment, 
    NiMemHint kHint,
    const char* pcSourceFile, 
    int iSourceLine, 
    const char* pcFunction)
#else
void* _NiAlignedMalloc(size_t stSizeInBytes, size_t stAlignment, 
    NiMemHint kHint)
#endif
{
    NIASSERT(NiMemManager::IsInitialized());
    if (stSizeInBytes == 0)
        stSizeInBytes = 1;

    // Actually allocate the memory
    return NiMemManager::Get().Allocate(stSizeInBytes,  stAlignment, kHint,
        NI_ALIGNEDMALLOC
#ifdef NI_MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
        );
}
//---------------------------------------------------------------------------
// _NiRealloc (see notes above)
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
void* _NiRealloc(
    void *pvMem, 
    size_t stSizeInBytes, 
    NiMemHint kHint,
    const char* pcSourceFile,
    int iSourceLine, 
    const char* pcFunction)
#else
void* _NiRealloc(void *pvMem, size_t stSizeInBytes, NiMemHint kHint)
#endif
{
    NIASSERT(NiMemManager::IsInitialized());

    // If the intention is to use NiRealloc like NiFree, just use
    // NiFree.
    if (stSizeInBytes == 0 && pvMem != 0)
    {
        NiFree(pvMem);
        return NULL;
    }
    else if (pvMem == 0)
    {
        return _NiMalloc(stSizeInBytes,
            kHint
#ifdef NI_MEMORY_DEBUGGER
            , pcSourceFile, iSourceLine, pcFunction
#endif
            );
    }

    // Actually reallocate the memory
    pvMem = NiMemManager::Get().Reallocate(pvMem, stSizeInBytes,  
        NI_MEM_ALIGNMENT_DEFAULT, kHint, NI_REALLOC, 
        NI_MEM_DEALLOC_SIZE_DEFAULT
#ifdef NI_MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
        );

    return pvMem; 
}
//---------------------------------------------------------------------------
// _NiAlignedRealloc (see notes above)
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
void* _NiAlignedRealloc(
    void *pvMem, 
    size_t stSizeInBytes, 
    size_t stAlignment, 
    NiMemHint kHint,
    const char* pcSourceFile, 
    int iSourceLine, 
    const char* pcFunction)
#else
void* _NiAlignedRealloc(void *pvMem, size_t stSizeInBytes, size_t stAlignment,
    NiMemHint kHint)
#endif
{
    NIASSERT(NiMemManager::IsInitialized());

    // If the intention is to use NiRealloc like NiFree, just use
    // NiFree.
    if (stSizeInBytes == 0 && pvMem != 0)
    {
        NiAlignedFree(pvMem);
        return NULL;
    }
    else if (pvMem == 0)
    {
        return _NiAlignedMalloc(stSizeInBytes, stAlignment, kHint
#ifdef NI_MEMORY_DEBUGGER
            , pcSourceFile, iSourceLine, pcFunction
#endif
        );
    }

    // Actually reallocate the memory
    return NiMemManager::Get().Reallocate(pvMem, stSizeInBytes,  stAlignment,
        kHint, NI_ALIGNEDREALLOC, NI_MEM_DEALLOC_SIZE_DEFAULT
#ifdef NI_MEMORY_DEBUGGER
        , pcSourceFile, iSourceLine, pcFunction
#endif
        );

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiVerifyAddress (see notes above)
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
// Check the memory management system to make sure that the address is 
// currently active and was allocated by our system. For convenience,
// NULL is the only invalid address that is acceptable for this function.
bool  NiVerifyAddress(const void* pvMemory)
{
    if (pvMemory == NULL)
        return true;

    NIASSERT(NiMemManager::IsInitialized());
    return NiMemManager::Get().VerifyAddress(pvMemory);
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiTrackAlloc / NiTrackFree (see notes above)
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
bool _NiTrackAlloc(const void* const pvMemory, size_t stSizeInBytes, 
    NiMemHint kHint, const char* pcSourceFile, int iSourceLine,
    const char* pcFunction)
{
    if (pvMemory == NULL)
        return true;

    NIASSERT(NiMemManager::IsInitialized());
    return NiMemManager::Get().TrackAllocate(pvMemory, stSizeInBytes, kHint,
        NI_EXTERNALALLOC, pcSourceFile, iSourceLine, pcFunction);
}
//---------------------------------------------------------------------------
bool _NiTrackFree(const void* const pvMemory)
{
    if (pvMemory == NULL)
        return true;

    NIASSERT(NiMemManager::IsInitialized());
    return NiMemManager::Get().TrackDeallocate(pvMemory, NI_EXTERNALFREE);
}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// _NiFree (see notes above)
//---------------------------------------------------------------------------
void _NiFree(void* pvMem)
{
    if (pvMem == NULL)
        return;

    NiMemManager::Get().Deallocate(pvMem, NI_FREE);
}
//---------------------------------------------------------------------------
// _NiAlignedFree (see notes above)
//---------------------------------------------------------------------------
void _NiAlignedFree(void* pvMem)
{
    if (pvMem == NULL)
        return;

    NiMemManager::Get().Deallocate(pvMem, NI_ALIGNEDFREE);
}
//---------------------------------------------------------------------------
void _NiLogMemAssert(const char* pcCondition, NiAllocUnit* pkUnit, 
    const char* pcFile, int iLine, const char* pcFunction)
{
    if (NiAssertFail::ms_pfnNiAssertFailProc != NULL)
    {
        NiAssertFail::ms_pfnNiAssertFailProc(pcCondition,
            pcFile, pcFunction, iLine);
    }

    NILOG("%s:%d (%s) %s -- NiMemAssert failed.\n",
        pcFile, iLine, pcFunction, pcCondition);
    if (pkUnit)
    {
        NIASSERT(NiMemTracker::Get());
        NiMemTracker::Get()->LogAllocUnit(pkUnit, NIMESSAGE_GENERAL_0);
    }
}
//---------------------------------------------------------------------------
#ifdef NI_MEMORY_DEBUGGER
bool _NiMemMarker(const char* pcMarkerType, const char* pcClassifier, 
    const char* pcString)
{
    NiMemManager::Get().SetMarker(pcMarkerType, pcClassifier, pcString);
    return true;
}
//---------------------------------------------------------------------------
bool _NiMemMarker(const char* pcMarkerType, const char* pcClassifier, 
    unsigned int uiValue)
{
    char acBufferString[256];
    NiSprintf(acBufferString, 256, "%d", uiValue);
    NiMemManager::Get().SetMarker(pcMarkerType, pcClassifier, acBufferString);
    return true;
}
//---------------------------------------------------------------------------
bool _NiMemMarker(const char* pcMarkerType, const char* pcClassifier, 
    const void* const pvValue)
{
    char acBufferString[256];
    NiSprintf(acBufferString, 256, "0x%p", pvValue);
    NiMemManager::Get().SetMarker(pcMarkerType, pcClassifier, acBufferString);
    return true;
}
#endif
//---------------------------------------------------------------------------
