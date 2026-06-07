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

#ifndef NIGLOBALSTRINGTABLE_H
#define NIGLOBALSTRINGTABLE_H

#include "NiMainLibType.h"
#include "NiMemObject.h"
#include "NiTArray.h"
#include <NiSystem.h>

#define NUM_GLOBAL_STRING_HASH_BUCKETS 513
#define NUM_ENTRIES_PER_BUCKET_GROWBY 10

class NIMAIN_ENTRY NiGlobalStringTable : public NiMemObject
{
public:
    typedef char* GlobalStringHandle;

    // Global string handle interfaces
    static const GlobalStringHandle AddString(const char* pcString);
    inline static void IncRefCount(GlobalStringHandle& kHandle);
    inline static void DecRefCount(GlobalStringHandle& kHandle);
    inline static const char* GetString(const GlobalStringHandle& kHandle);
    inline static size_t GetLength(const GlobalStringHandle& kHandle);
    inline static size_t GetRefCount(const GlobalStringHandle& kHandle);
    inline static bool ValidateString(const GlobalStringHandle& kHandle);
    
    // Begin Emergent internal use only
    static void _SDMInit();
    static void _SDMShutdown();
    static size_t GetBucketSize(unsigned int uiWhichBucket);
    inline static size_t GetTotalNumberOfBuckets() 
        {return NUM_GLOBAL_STRING_HASH_BUCKETS;};
    static size_t GetMaxBucketSize();

    static const GlobalStringHandle NULL_STRING;
    static size_t ms_stNumStrings;
    static size_t ms_stNumCollisions;

    enum
    {
        GSTABLE_LEN_MASK        = 0x0000FFFF,
        GSTABLE_LEN_SHIFT       = 0,
        GSTABLE_HASH_MASK       = 0xFFFF0000,
        GSTABLE_HASH_SHIFT      = 16
    };
    // End Emergent internal use only


protected:
    const GlobalStringHandle FindString(const char* pcString, size_t stLen);
    void InsertString(const GlobalStringHandle& kHandle,
        unsigned int uiHash);
    void RemoveString(const GlobalStringHandle& kHandle, 
        unsigned int uiHashValue);
    inline static char* GetRealBufferStart(const GlobalStringHandle& kHandle);

    static unsigned int HashFunction(const char* pcKey, size_t stLen);
    inline static unsigned int GetHashValue(const GlobalStringHandle& kHandle);

    NiGlobalStringTable();
    static NiGlobalStringTable* ms_pkTable;

    NiTPrimitiveArray<GlobalStringHandle> 
        m_kHashArray[NUM_GLOBAL_STRING_HASH_BUCKETS];
    NiCriticalSection m_kCriticalSection;
};

#include "NiGlobalStringTable.inl"
#endif
