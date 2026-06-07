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

#ifndef NISPDATASTREAM_H
#define NISPDATASTREAM_H

#include "NiFloodgateLibType.h"
#include "NiUniversalTypes.h"

/**
    This class defines the properties and functions that Floodgate requires
    of streams of data.
    
    This is the interface class from which platform specific data streams are
    derived.
*/
class NIFLOODGATE_ENTRY NiSPDataStream
{
public:
    /**
        The types of lock observed by Floodgate.
        
        The enumeration should be "derived" by sub-classes, such as
        NiDataStream in the NiMesh library. If updating, check for
        consistency.
    */
    enum SPLockType
    {
        /// Data should be locked for reading.
        LOCK_READ  = 0x01,

        /// Data should be locked for writing.
        LOCK_WRITE = 0x02 
    };

    /// Empty destructor to keep compilers happy.
    virtual ~NiSPDataStream();

    /**
        Gets the block count for the specified region.

        @param uiRegionIdx Index of the region to lock.
    */
    virtual NiUInt32 GetBlockCount(NiUInt32 uiRegionIdx) const = 0;

    /// Gets the stride of data in the stream.
    virtual NiUInt32 GetStride() const = 0;

    /**
        @name Locking Methods
        Methods to assist with the locking and unlocking of NiDataStream
        objects.
    */
    //@{
    /**
        Locks the NiDataStream at the region specified.
        
        The form of lock is determined by calls to EnableWriteFlag and
        EnableReadFLag.

        @param uiRegionIdx Index of the region to lock.
    */
    virtual const void* LockSource(NiUInt32 uiRegionIdx) = 0;
    
    /// Unlocks a data source locked with LockSource.
    virtual void UnlockSource() = 0;
    
    /**
        Enable the write lock flag for the stream.

        When this function is called, derived classes need to remember that
        they should lock for writing when LockSource is called. It is legal to
        lock for both reading and writing.
    */
    virtual void EnableWriteFlag() = 0;
    
    /**
        Enable the read lock flag for the stream.

        When this function is called, derived classes need to remember that
        they should lock for reading when LockSource is called. It is legal to
        lock for both reading and writing.
    */
    virtual void EnableReadFlag() = 0;
    //@}

protected:
    /// Gets the lock flags.
    virtual NiUInt32 GetLockFlags() const = 0;

    /// Clears the lock flags.
    virtual void ClearLockFlags() = 0;
};

#include "NiSPDataStream.inl"

#endif  // #ifndef NISPDATASTREAM_H
