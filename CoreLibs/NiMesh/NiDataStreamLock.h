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

#ifndef NIDATASTREAMLOCK_H
#define NIDATASTREAMLOCK_H

#include "NiFixedString.h"
#include "NiDataStream.h"
#include "NiDataStreamElement.h"
#include "NiDataStreamRef.h"
#include "NiTStridedRandomAccessIterator.h"

class NiMesh;

/**
    This class encapsulates locking an arbitrary NiDataStream and iterating 
    through the data contained within. Iteration can be for the entire buffer
    or individual NiDataStream::Regions of the stream. This class will lock 
    upon construction and will unlock upon destruction.

    @note If iteration of primitives is desired, use NiDataStreamPrimtiveLock
        instead. If iteration over submeshes is desired instead of regions, 
        use NiDataStreamElementLock instead.
*/
class NIMESH_ENTRY NiDataStreamLock : public NiMemObject
{
public:

    /// Default constructor.
    NiDataStreamLock();

    /**
        Constructs a NiDataStreamLock for an NiDataStream that
        automatically locks upon construction.
        
        Use IsLocked to check
        for failure of the constructor to lock the stream.

        @param pkDataStream NiDataStream to lock for access.
        @param stElementOffset Offset to the element that will be iterated.
        @param uiLockMask Mask indicating lock flags.
    */
    NiDataStreamLock(NiDataStream* pkDataStream, 
        size_t stElementOffset = 0, 
        NiUInt8 uiLockMask = NiDataStream::LOCK_WRITE);

    /**
        Copy constructor.
        
        This will lock using the data from kOther. The underlying data stream
        will be locked a second time.
    */
    NiDataStreamLock(const NiDataStreamLock& kLock);

    /// Assignment operator.
    ///
    /// The underlying data stream will be locked a second time.
    inline NiDataStreamLock& operator=(const NiDataStreamLock& kRHS);

    /// Destructor.
    ~NiDataStreamLock();

    /// Returns true if the internal reference to the data stream is not null.
    inline bool DataStreamExists() const;

    /// Returns true if the pointer to locked data is not null.
    inline bool IsLocked() const;

    /// Accessor for the internal reference to the data stream.
    inline NiDataStream* GetDataStream();

    /**
        Unlock the datastream before destruction.
        
        Generally, when the class is destroyed, it will unlock itself. This
        requires that the underlying data stream exist during destruction.
        If the user doesn't wish to scope the lock accordingly, they can add
        a call the unlock method.
    */
    inline void Unlock();
    
    /// @name NiDataStream Full Iteration
    //@{
    /// Gets the starting iterator for all of the vertex elements in the 
    /// datastream, which assumes a valid lock.
    template<typename T> NiTStridedRandomAccessIterator<T> begin() const;
    
    /// Gets the ending iterator for all of the vertex elements in the
    /// datastream, which assumes a valid lock.
    template<typename T> NiTStridedRandomAccessIterator<T> end() const; 

    /// Get number of vertex elements in the datastream, which assumes a
    /// valid lock.
    inline NiUInt32 count() const;
    //@}


    /// @name NiDataStream Region Iteration
    //@{
    /// Gets the starting iterator for the vertex elements in the specified
    /// region, which assumes a valid lock.
    template<typename T> NiTStridedRandomAccessIterator<T> begin_region(
        NiUInt32 uiRegionIdx) const;
    
    /// Gets the ending iterator for the vertex elements in the specified 
    /// region, which assumes a valid lock.
    template<typename T> NiTStridedRandomAccessIterator<T> end_region(
        NiUInt32 uiRegionIdx) const; 

    /// Get number of vertex elements in the specified region, which assumes
    /// a valid lock.
    inline NiUInt32 count_region(NiUInt32 uiRegionIdx) const;

    /// Get the number of regions on this datastream.
    inline NiUInt32 GetNumRegions() const;
    //@}

protected:    
    /// Pointer to the locked datastream.
    NiDataStream* m_pkDataStream;

    /// Offset to element of interest.
    size_t  m_stElementOffset;

    /// Pointer to raw data.
    char* m_pcBuffer;

    /// Type of locking operation to perform.
    NiUInt8 m_uiLockType;
};

#include "NiDataStreamLock.inl"

#endif // #ifndef NIDATASTREAMLOCK_H
