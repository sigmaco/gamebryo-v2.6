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
inline char* NiGlobalStringTable::GetRealBufferStart(
    const GlobalStringHandle& kHandle)
{
    NIASSERT(kHandle != NULL);
    return ((char*)kHandle - 2*sizeof(size_t));
}
//---------------------------------------------------------------------------
inline void NiGlobalStringTable::IncRefCount(GlobalStringHandle& kHandle)
{
    if (kHandle == NULL_STRING)
        return;

    NIASSERT(ValidateString(kHandle));
    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    NiAtomicIncrement(pkMem[0]);
}
//---------------------------------------------------------------------------
inline void NiGlobalStringTable::DecRefCount(GlobalStringHandle& kHandle)
{
    if (kHandle == NULL_STRING)
        return;

    NIASSERT(ValidateString(kHandle));
    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);

    // We must get the hash value before we decrement. After decrementing,
    // the fixed string could be deleted in another thread which would
    // make the results of any hash retrieval invalid.
    unsigned int uiHash = GetHashValue(kHandle);
    
    if (NiAtomicDecrement(pkMem[0]) == 1)
    {
        NIASSERT(ms_pkTable);
        ms_pkTable->RemoveString(kHandle, uiHash);
    }
}
//---------------------------------------------------------------------------
inline const char* NiGlobalStringTable::GetString(
    const GlobalStringHandle& kHandle) 
{
    NIASSERT(ValidateString(kHandle));
    // No need to perform an if NULL_STRING check, because
    // it will correctly return NULL if kHandle == NULL_STRING
    return (const char*) kHandle;
}
//---------------------------------------------------------------------------
inline size_t NiGlobalStringTable::GetLength(
    const GlobalStringHandle& kHandle) 
{
    if (kHandle == NULL_STRING)
    {
        return 0;
    }
    else
    {
        NIASSERT(ValidateString(kHandle));
        size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
        return ((unsigned int)pkMem[1] & GSTABLE_LEN_MASK) >> 
            GSTABLE_LEN_SHIFT;
    }
}
//---------------------------------------------------------------------------
inline size_t NiGlobalStringTable::GetRefCount(
    const GlobalStringHandle& kHandle) 
{
    if (kHandle == NULL_STRING)
    {
        return 0;
    }
    else
    {
        NIASSERT(ValidateString(kHandle));
        size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
        return (unsigned int)pkMem[0];
    }
}
//---------------------------------------------------------------------------
inline unsigned int NiGlobalStringTable::GetHashValue(
    const GlobalStringHandle& kHandle)
{
    // Get the hash value stored in the global string handle. We don't 
    // perform a null check here. Since this method is protected, we assume
    // the calling context of NiGlobalStringTable has verified the handle to
    // be non-null.
    NIASSERT(ValidateString(kHandle));
    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    return ((unsigned int)pkMem[1] & GSTABLE_HASH_MASK) >> GSTABLE_HASH_SHIFT;
}
//---------------------------------------------------------------------------
inline bool NiGlobalStringTable::ValidateString(
    const GlobalStringHandle& kHandle)
{
    if (kHandle == NULL_STRING)
        return true;

    size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
    size_t stLength = (pkMem[1] & GSTABLE_LEN_MASK) >> GSTABLE_LEN_SHIFT;
        
    if (stLength != strlen((const char*)kHandle))
        return false;

    return true;
}
//---------------------------------------------------------------------------
