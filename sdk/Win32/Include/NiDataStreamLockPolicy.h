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

#ifndef NIDATASTREAMLOCKPOLICY_H
#define NIDATASTREAMLOCKPOLICY_H

#include "NiCriticalSection.h"

#include "NiMeshLibType.h"

/**
    Implements the locking policy used by NiDataStream
    subclasses with static write access. This is a template class - the
    template argument must be a sub-class of NiDataStream.
*/
template <class TDataStream>
class StaticLockPolicy 
{
public:

    /// Constructor.
    StaticLockPolicy();

    /// Locks the stream for read access and returns a pointer to the data.
    void* LockRead(TDataStream* pkDS, bool bForceRead);

    /// Locks the stream for write access and returns a pointer to the data.
    ///
    /// This lock will succeed only once to write the initial data.
    void* LockWrite(TDataStream* pkDS, bool bForceWrite);
    
    /**
        Locks the stream for read/write access and returns a pointer to the
        data.
        
        This lock will succeed only once to write the initial data.
    */
    void* LockReadWrite(TDataStream* pkDS, bool bForceRead, bool bForceWrite);

    /// Unlocks a stream that was previously locked for read access.
    void UnlockRead(TDataStream* pkDS, bool bForceRead);
    
    /// Unlocks a stream that was previously locked for write access.
    void UnlockWrite(TDataStream* pkDS, bool bForceWrite);
    
    /// Unlocks a stream that was previously locked for read/write access.
    void UnlockReadWrite(TDataStream* pkDS, bool bForceRead, bool bForceWrite);

    /// In debug builds, asserts that the stream is not locked and hence
    /// is safe to delete.
    void Delete(TDataStream* pkDS);

protected:
    NiUInt32 m_uiWriters;
    NiUInt32 m_uiReadLockCount;
};

/**
    Implements the locking policy used by NiDataStream
    subclasses with volatile read/write access. This is a template class - the
    template argument must be a sub-class of NiDataStream.
*/
template <class TDataStream>
class VolatileLockPolicy 
{
public:

    /// Constructor.
    VolatileLockPolicy();

    /// Locks the stream for read access and returns a pointer to the data.
    void* LockRead(TDataStream* pkDS, bool bForceRead);

    /// Locks the stream for write access and returns a pointer to the data.
    ///
    /// The content of the data stream is undefined.
    void* LockWrite(TDataStream* pkDS, bool bForceWrite);
    
    /// Asserts false and returns NULL, as it is not possible to read and
    /// write from a volatile buffer with one lock.
    void* LockReadWrite(TDataStream* pkDS, bool bForceRead, bool bForceWrite);

    /// Unlocks a stream that was previously locked for read access.
    void UnlockRead(TDataStream* pkDS, bool bForceRead);
    
    /// Unlocks a stream that was previously locked for write access.
    void UnlockWrite(TDataStream* pkDS, bool bForceWrite);
    
    /// Unlocks a stream that was previously locked for read/write access.
    void UnlockReadWrite(TDataStream* pkDS, bool bForceRead, bool bForceWrite);

    /// In debug builds, asserts that the stream is not locked and hence
    /// is safe to delete.
    void Delete(TDataStream* pkDS);

protected:
    NiUInt32 m_uiWriteLockCount;
};

/**
    Implements the locking policy used by NiDataStream
    subclasses with mutable read/write access. This is a template class - the
    template argument must be a sub-class of NiDataStream.
*/
template <class TDataStream>
class MutableLockPolicy 
{
public:
    /// Constructor.
    MutableLockPolicy();
    
    /// Locks the stream for read access and returns a pointer to the data.
    void* LockRead(TDataStream* pkDS, bool bForceRead);

    /// Locks the stream for write access and returns a pointer to the data.
    void* LockWrite(TDataStream* pkDS, bool bForceWrite);
    
    /// Locks the stream for read/write access and returns a pointer to the
    /// data.
    void* LockReadWrite(TDataStream* pkDS, bool bForceRead, bool bForceWrite);

    /// Unlocks a stream that was previously locked for read access.
    void UnlockRead(TDataStream* pkDS, bool bForceRead);
    
    /// Unlocks a stream that was previously locked for write access.
    void UnlockWrite(TDataStream* pkDS, bool bForceWrite);
    
    /// Unlocks a stream that was previously locked for read/write access.
    void UnlockReadWrite(TDataStream* pkDS, bool bForceRead, bool bForceWrite);

    /// In debug builds, asserts that the stream is not locked and hence
    /// is safe to delete.
    void Delete(TDataStream* pkDS);

protected:
    NiCriticalSection m_kLock;

    /// Number of read locks.
    NiUInt32 m_uiReadLockCount;
    
    /// Number of write locks.
    NiUInt32 m_uiWriteLockCount;
};

#include "NiDataStreamLockPolicy.inl"

#endif // NIDATASTREAMLOCKPOLICY_H

