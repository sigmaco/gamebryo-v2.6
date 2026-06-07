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
inline void* NiExternalMemAllocator::Allocate(
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemHint,
    NiMemEventType,
    const char*,
    int,
    const char*)
{
    NIASSERT(NI_IS_POWER_OF_TWO(stAlignment));

    return NiExternalAlignedMalloc(stSizeInBytes, stAlignment);
}
//---------------------------------------------------------------------------
inline void NiExternalMemAllocator::Deallocate(
    void* pvMemory, 
    NiMemEventType, 
    size_t)
{
    if (pvMemory == NULL)
        return;

    NiExternalAlignedFree(pvMemory);
}
//---------------------------------------------------------------------------
inline void* NiExternalMemAllocator::Reallocate(
    void* pvMemory,
    size_t& stSizeInBytes,
    size_t& stAlignment,
    NiMemHint,
    NiMemEventType,
    size_t,
    const char*,
    int,
    const char*)
{
    NIASSERT(NI_IS_POWER_OF_TWO(stAlignment));

    // The deallocation case should have been caught by us before in
    // the allocation functions.
    NIASSERT(stSizeInBytes != 0);
    
    return NiExternalAlignedRealloc(pvMemory, stSizeInBytes, stAlignment);
}
//---------------------------------------------------------------------------
inline bool NiExternalMemAllocator::TrackAllocate(
    const void* const, 
    size_t, 
    NiMemHint,
    NiMemEventType, 
    const char*, 
    int,
    const char*)
{
    return false;
}
//---------------------------------------------------------------------------
inline bool NiExternalMemAllocator::TrackDeallocate(
    const void* const, 
    NiMemEventType)
{
    return false;
}
//---------------------------------------------------------------------------
inline bool NiExternalMemAllocator::SetMarker(const char*,
    const char*, const char*)
{
    return true;
}
//---------------------------------------------------------------------------
inline void NiExternalMemAllocator::Initialize()
{
}
//---------------------------------------------------------------------------
inline void NiExternalMemAllocator::Shutdown()
{
}
//---------------------------------------------------------------------------
inline bool NiExternalMemAllocator::VerifyAddress(const void*)
{
    return true;
}
//---------------------------------------------------------------------------
