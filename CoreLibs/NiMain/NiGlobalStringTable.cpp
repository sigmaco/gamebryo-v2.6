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
#include "NiMainPCH.h"
#include "NiGlobalStringTable.h"

NiGlobalStringTable* NiGlobalStringTable::ms_pkTable = NULL;
const NiGlobalStringTable::GlobalStringHandle 
    NiGlobalStringTable::NULL_STRING = NULL;
size_t NiGlobalStringTable::ms_stNumStrings = 0;
size_t NiGlobalStringTable::ms_stNumCollisions = 0;

//---------------------------------------------------------------------------
NiGlobalStringTable::NiGlobalStringTable()
{
    // Assert that we don't have too many buckets since we're storing the
    // hash value in 16 bits.
    NIASSERT(NUM_GLOBAL_STRING_HASH_BUCKETS < 65536);

    // Make sure we have enough room for storing the hash value and the
    // length. Later on, we'll pack them into a size_t object which must
    // be at least 4B.
    NIASSERT(sizeof(size_t) >= 4);

    for (unsigned int ui = 0; ui < NUM_GLOBAL_STRING_HASH_BUCKETS; ui++)
    {
        m_kHashArray[ui].SetGrowBy(NUM_ENTRIES_PER_BUCKET_GROWBY);
    }
}
//---------------------------------------------------------------------------
const NiGlobalStringTable::GlobalStringHandle NiGlobalStringTable::AddString(
     const char* pcString)
{
    if (pcString == NULL)
    {
        return NULL_STRING;
    }

    NIASSERT(ms_pkTable);

    size_t stStrLen = strlen(pcString);
    
    // The limit for a fixed string is 65535 since we store the length in
    // 16 bits.
    NIASSERT(stStrLen < 65536);

    // Determine if the string already exists in the table
    // Because two threads could enter AddString with the same value
    // concurrently, the entire hash table will need to be locked.
    ms_pkTable->m_kCriticalSection.Lock();
    GlobalStringHandle kHandle = ms_pkTable->FindString(pcString, stStrLen);
    
    if (kHandle != NULL_STRING)
    {
        IncRefCount(kHandle);
        ms_pkTable->m_kCriticalSection.Unlock();
        return kHandle;
    }   

    // Add in space for ref count, length, and null terminator.
    size_t stAllocLen = stStrLen + 2*sizeof(size_t) + sizeof(char);

    // since we need the size_t header to be properly aligned
    if (stAllocLen % sizeof(size_t) != 0)
    {
        stAllocLen += sizeof(size_t) - (stAllocLen % sizeof(size_t));
    }
    
    void * pvMem = NiMalloc(stAllocLen);   

    char* pcMem = (char*) pvMem + 2*sizeof(size_t);
    kHandle = pcMem;     
    size_t* kMem = (size_t*) pvMem;

    unsigned int uiHash = HashFunction(pcString, stStrLen);
    
    kMem[0] = 2;
    kMem[1] = (stStrLen << GSTABLE_LEN_SHIFT) |
        ((uiHash << GSTABLE_HASH_SHIFT) & GSTABLE_HASH_MASK);
    memcpy(pcMem, pcString, stStrLen+1);

    ms_pkTable->InsertString(kHandle, uiHash);
    ms_pkTable->m_kCriticalSection.Unlock();
    return kHandle;
}
//---------------------------------------------------------------------------
unsigned int NiGlobalStringTable::HashFunction(const char* pcString, 
    size_t stLen)
{
    unsigned int uiHash = 0;
    // Unroll the loop 4x
    unsigned int uiUnroll =
        static_cast<unsigned int>(stLen & ~0x3);

    for (unsigned int ui = 0; ui < uiUnroll; ui += 4)
    {
        unsigned int uiHash0 = *pcString;
        uiHash = (uiHash << 5) + uiHash + uiHash0;
        unsigned int uiHash1 = *(pcString + 1);
        uiHash = (uiHash << 5) + uiHash + uiHash1;
        unsigned int uiHash2 = *(pcString + 2);
        uiHash = (uiHash << 5) + uiHash + uiHash2;
        unsigned int uiHash3 = *(pcString + 3);
        uiHash = (uiHash << 5) + uiHash + uiHash3;
        pcString += 4;
    }

    while (*pcString)
        uiHash = (uiHash << 5) + uiHash + *pcString++;

    return uiHash % NUM_GLOBAL_STRING_HASH_BUCKETS;
}
//---------------------------------------------------------------------------
const NiGlobalStringTable::GlobalStringHandle 
    NiGlobalStringTable::FindString(const char* pcString, size_t stLen)
{
    unsigned int uiHash = HashFunction(pcString, stLen);

    m_kCriticalSection.Lock();
    unsigned int uiBucketSize = m_kHashArray[uiHash].GetSize();
    for (unsigned int ui = 0; ui < uiBucketSize; ++ui)
    {
        NiGlobalStringTable::GlobalStringHandle kPossibleMatchString = 
            m_kHashArray[uiHash].GetAt(ui);

        if (kPossibleMatchString == pcString ||
            (GetString(kPossibleMatchString) && 
            GetLength(kPossibleMatchString) == stLen &&
            strcmp(GetString(kPossibleMatchString), pcString) == 0))
        {
            m_kCriticalSection.Unlock();
            return kPossibleMatchString;
        }
    }

    m_kCriticalSection.Unlock();
    return NULL_STRING;
}
//---------------------------------------------------------------------------
void NiGlobalStringTable::InsertString(const GlobalStringHandle& kHandle,
    unsigned int uiHash)
{
    NIASSERT(ValidateString(kHandle));

    // Adding and removing strings from the hash table should
    // trip the critical section.
    m_kCriticalSection.Lock();
    if (m_kHashArray[uiHash].GetEffectiveSize() != 0)
    {
        ms_stNumCollisions++;
    }

    m_kHashArray[uiHash].AddFirstEmpty(kHandle);
    
    ++ms_stNumStrings;
    
#ifdef TEST_HASHING_FUNCTION
    if (m_kHashArray[uiHash].GetSize() > 10)
    {
        NiOutputDebugString("Found > 10:\n");
        for (unsigned int ui = 0; ui < m_kHashArray[uiHash].GetSize(); ui++)
        {
            char acString[1024];
            NiSprintf(acString, 1024, "[%d] = \"%s\"\n",
                ui, m_kHashArray[uiHash].GetAt(ui));
            NiOutputDebugString(acString);
        }
    }
#endif

    m_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiGlobalStringTable::RemoveString(const GlobalStringHandle& kHandle,
    unsigned int uiHashValue)
{
    const char* pcString = GetString(kHandle);
    
    // Adding and removing strings from the hash table should
    // trip the critical section. 
    // N.B. The hash value is valid because we grabbed it from DecRefCount
    // before the decrement. However, kHandle may have been deleted at this
    // point. If so, we will fall through the loop. The only failure case
    // occurs if another thread grabbed the same memory and allocated a string
    // in the same hash bucket with that memory. In that case, the ref count
    // will only be 1 if the other context has decided to delete that string
    // as well. However, it is safe to delete from this context even
    // though we think we're deleting a different string. The second context
    // will pass a valid hash value into this function and fall through the
    // loop because the string has been removed. This error can safely cascade 
    // as many times as it occurs, but the table will be valid at any point,
    // and invalid memory will never be dereferenced.
    m_kCriticalSection.Lock();
    unsigned int ui = 0;

    unsigned int uiSize = m_kHashArray[uiHashValue].GetSize();
    for (; ui < uiSize; ui++)
    {
        if (m_kHashArray[uiHashValue].GetAt(ui) == pcString)
        {
            size_t* pkMem = (size_t*)GetRealBufferStart(kHandle);
            if (GetRefCount(kHandle) == 1 &&
                NiAtomicDecrement(pkMem[0]) == 0)
            {
                NiFree(GetRealBufferStart(kHandle));
                m_kHashArray[uiHashValue].SetAt(ui, 0);
                --ms_stNumStrings;
            }
            break;
        }
    }
    m_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiGlobalStringTable::_SDMInit()
{
    NIASSERT(!ms_pkTable);
    ms_pkTable = NiNew NiGlobalStringTable();
}
//---------------------------------------------------------------------------
void NiGlobalStringTable::_SDMShutdown()
{
    NiDelete ms_pkTable;
    ms_pkTable = NULL;
}
//---------------------------------------------------------------------------
size_t NiGlobalStringTable::GetBucketSize(unsigned int uiWhichBucket)
{
    NIASSERT(ms_pkTable);
    NIASSERT(uiWhichBucket < NUM_GLOBAL_STRING_HASH_BUCKETS);
    ms_pkTable->m_kCriticalSection.Lock();
    size_t stSize = ms_pkTable->m_kHashArray[uiWhichBucket].GetSize();
    ms_pkTable->m_kCriticalSection.Unlock();
    return stSize;
}
//---------------------------------------------------------------------------
size_t NiGlobalStringTable::GetMaxBucketSize()
{
    NIASSERT(ms_pkTable);
    ms_pkTable->m_kCriticalSection.Lock();
    size_t stSize = 0;
    for (unsigned int ui = 0; ui < NUM_GLOBAL_STRING_HASH_BUCKETS; ui++)
    {
        if (stSize < ms_pkTable->m_kHashArray[ui].GetSize())
            stSize = ms_pkTable->m_kHashArray[ui].GetSize();
    }
    ms_pkTable->m_kCriticalSection.Unlock();
    return stSize;
}
//---------------------------------------------------------------------------
