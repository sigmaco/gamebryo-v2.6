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

#ifndef NIDATASTREAMPRIMITIVELOCK_H
#define NIDATASTREAMPRIMITIVELOCK_H

#include "NiFixedString.h"
#include "NiDataStream.h"
#include "NiDataStreamElement.h"
#include "NiDataStreamRef.h"
#include "NiTStridedRandomAccessIterator.h"
#include "NiTPrimitiveRandomAccessIterator.h"
#include "NiMesh.h"

/**
    This class encapsulates locking the index buffer and iterating through
    all of the primitives associated with a given mesh. Supported primitive 
    configurations are: indexed or non-indexed primitives, 16bit and
    32bit index types, and all known NiPrimitiveType::Type types. This class 
    will lock upon construction and will unlock upon destruction.

    @note If iteration of individual elements in a region is desired, 
        use NiDataStreamLock. If iteration over submeshes is desired instead 
        of regions, use NiDataStreamElementLock instead.
*/
class NIMESH_ENTRY NiDataStreamPrimitiveLock : public NiMemObject
{
public:

    /**
        Constructs an NiDataStreamPrimitiveLock from a NiMesh that
        automatically locks upon construction.
        
        Use IndexBufferLocked to
        check that the constructor succeeded in locking the index buffer.

        @param pkMesh NiMesh to lock for access.
        @param uiLockMask Mask indicating lock flags.
    */
    NiDataStreamPrimitiveLock(NiMesh* pkMesh,    
        NiUInt8 uiLockMask = NiDataStream::LOCK_WRITE);

    /**
        Copy constructor.
        
        This will lock using the data from kOther. The underlying data stream
        will be locked a second time.
    */
    NiDataStreamPrimitiveLock(const NiDataStreamPrimitiveLock& kOther);

    /// Assignment operator.
    ///
    /// The underlying data stream will be locked a second time.
    NiDataStreamPrimitiveLock& operator=(
        const NiDataStreamPrimitiveLock& kRHS);

    /// Virtual destructor.
    virtual ~NiDataStreamPrimitiveLock();

    /// Gets a 32bit indexed begin iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiIndexedPrimitiveIterator32 BeginIndexed32(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets a 32bit indexed end iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiIndexedPrimitiveIterator32 EndIndexed32(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets a 16bit indexed begin iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiIndexedPrimitiveIterator16 BeginIndexed16(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets a 16bit indexed end iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiIndexedPrimitiveIterator16 EndIndexed16(
        NiUInt32 uiSubmeshIdx = 0) const;

    /// Gets a 32bit non-indexed begin iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiNonIndexedPrimitiveIterator32 BeginNonIndexed32(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets a 32bit non-indexed end iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiNonIndexedPrimitiveIterator32 EndNonIndexed32(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets a 16bit non-indexed begin iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiNonIndexedPrimitiveIterator16 BeginNonIndexed16(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets a 16bit non-indexed end iterator for accessing primitives in 
    /// the locked data stream for the specified sub-mesh.
    inline NiNonIndexedPrimitiveIterator16 EndNonIndexed16(
        NiUInt32 uiSubmeshIdx = 0) const;

    /// Get number of primitives for the specified sub-mesh, which assumes
    /// a valid lock.
    inline NiUInt32 count(NiUInt32 uiSubmeshIdx = 0) const; 

    /// Get the primitive type of the mesh this lock was created on.
    inline NiPrimitiveType::Type GetPrimitiveType() const;

    /// Get the submesh count.
    inline NiUInt32 GetSubmeshCount() const;

    /// Returns true if the index buffer exists.
    inline bool IndexBufferExists() const;

    /// Determine if the index buffer was locked.
    inline bool IndexBufferLocked() const;

    /// Returns whether or not the index buffer exists and supports a 16-bit
    /// iterator.
    inline bool Has16BitIndexBuffer() const;

    /// Returns whether or not the index buffer exists and supports a 32-bit
    /// iterator.
    inline bool Has32BitIndexBuffer() const;

    /**
        Unlock the datastream before destruction.
    
        Generally, when the class is destroyed, it will unlock itself. If
        the user doesn't wish to scope the lock accordingly, they can add
        a call the unlock method.
    */
    inline void Unlock();

protected:
    inline NiUInt32 ComputeNonIndexedCount(NiUInt32 uiSubmeshIdx) const;

    /// The NiDataStreamRef containing the datastream referenced by the lock
    NiDataStreamRef* m_pkRef;

    /// The element data associated with the semantic name/index pair
    NiDataStreamElement m_kElement;

    /// The submesh count for the parent mesh
    NiUInt32 m_uiSubmeshCount;

    /// The primitive type of the mesh
    NiPrimitiveType::Type m_eType;
    
    /// Indicates if the data is indexed or not.
    bool m_bIndexed;

    /// Pointer to raw data.
    char* m_pcBuffer;

    /// Type of locking operation to perform.
    NiUInt8 m_uiLockType;
};

#include "NiDataStreamPrimitiveLock.inl"

#endif
