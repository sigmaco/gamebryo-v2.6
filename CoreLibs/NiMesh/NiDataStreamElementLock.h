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

#ifndef NIDATASTREAMELEMENTLOCK_H
#define NIDATASTREAMELEMENTLOCK_H

#include "NiFixedString.h"
#include "NiDataStream.h"
#include "NiDataStreamElement.h"
#include "NiDataStreamRef.h"
#include "NiTStridedRandomAccessIterator.h"
#include "NiTPrimitiveRandomAccessIterator.h"

class NiMesh;

/**
    This class encapsulates locking an NiDataStream bound to a specific 
    NiMesh and iterating through the data contained within a given submesh.
    This class will lock upon construction and will unlock upon destruction.
    Note that this class does not add a reference to the NiDataStream it is
    locking. Therefore, if the source NiMesh is destroyed or the
    NiDataStreamRef used by this lock is removed from the NiMesh prior to the
    lock's destruction, a memory access exception will occur.

    @note If iteration of primitives is desired, use NiDataStreamPrimitiveLock 
        instead. If iteration over explicit regions is desired instead of
        submeshes, use NiDataStreamLock.
*/
class NIMESH_ENTRY NiDataStreamElementLock : public NiMemObject
{
public:

    /// Default constructor.
    NiDataStreamElementLock();

    /**
        Constructs a NiDataStreamElementLock from a NiMesh that
        automatically locks upon construction.
        
        No lock is obtained if the
        given semantic with the given properties cannot be found on the mesh.
        Use IsLocked to test the success of this constructor.

        @param pkMesh NiMesh to lock for access.
        @param kSemantic Semantic of the data of interest.
        @param uiSemanticIndex Index of the semantic.
        @param eFormat Format of the NiDataStream.
        @param uiLockMask Mask indicating lock flags.
    */
    NiDataStreamElementLock(NiMesh* pkMesh,
        const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex, 
        NiDataStreamElement::Format eFormat = NiDataStreamElement::F_UNKNOWN,
        NiUInt8 uiLockMask = NiDataStream::LOCK_WRITE);

    /**
        Copy constructor.

        This will lock using the data from kOther. The
        underlying data stream will be locked a second time.
    */
    NiDataStreamElementLock(const NiDataStreamElementLock& kOther);

    /// Assignment operator.
    ///
    /// The underlying data stream will be locked a second time.
    NiDataStreamElementLock& operator=(const NiDataStreamElementLock& kRHS);

    /// Virtual destructor which unlocks the stream.
    virtual ~NiDataStreamElementLock();

    /// Gets the starting iterator for the vertex elements from the given
    /// sub-mesh, which assumes a valid Lock.
    template<typename T> NiTStridedRandomAccessIterator<T> begin(
        NiUInt32 uiSubmeshIdx = 0) const;
    
    /// Gets the ending iterator for the vertex elements from the given
    /// sub-mesh, which assumes a valid lock.
    template<typename T> NiTStridedRandomAccessIterator<T> end(
        NiUInt32 uiSubmeshIdx = 0) const; 

    /// Get number of vertex elements in the given sub-mesh, which assumes
    /// a valid lock.
    inline NiUInt32 count(NiUInt32 uiSubmeshIdx = 0) const;

    /**
        Returns true if the internal reference to the mesh data stream is
        not null.

        Use this function to determine if a semantic and index exists on a
        mesh.
    */
    inline bool DataStreamExists() const;

    /// Returns true if the data stream exists and is locked.
    ///
    /// Use this function for error checking creating an iterator.
    inline bool IsLocked() const;

    /// Get the lock mask used to create this lock.
    inline NiUInt8 GetLockMask() const;
    
    /// Get the submesh count.
    inline NiUInt32 GetSubmeshCount() const;

    /// Get the data stream ref for the data we have locked.
    inline NiDataStreamRef* GetDataStreamRef();

    /// Get the data stream element for the data we have locked.
    inline const NiDataStreamElement& GetDataStreamElement();

    /// Get the datastream that is being locked.
    inline NiDataStream* GetDataStream();

    /**
        Unlock the datastream before destruction.
        
        Generally, when the class is destroyed, it will unlock itself.
        If the user doesn't wish to scope the lock accordingly, they can
        add a call the unlock method.
    */
    inline void Unlock();

private:
    /// NiDataStreamRef containing the locked NiDataStream.
    NiDataStreamRef* m_pkRef;

    /// The vertex element description that will be iterated.
    NiDataStreamElement m_kElement;

    /// The count of submeshes on the source NiMesh.
    NiUInt32 m_uiSubmeshCount;

    /// Pointer to raw data.
    char* m_pcBuffer;

    /// Type of locking operation to perform.
    NiUInt8 m_uiLockType;
};

#include "NiDataStreamElementLock.inl"

#endif
