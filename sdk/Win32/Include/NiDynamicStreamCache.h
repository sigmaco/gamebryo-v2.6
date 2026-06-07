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

#ifndef NIDYNAMICSTREAMCACHE_H
#define NIDYNAMICSTREAMCACHE_H

#include <NiTArray.h>
#include <NiMemorydefines.h>
#include <NiDataStream.h>

#include "NiTerrainLibType.h"

/**
    A dynamic stream cache is a manager that takes care of allocation and
    de-allocation of dynamic position and normal/tangent streams on behalf of a
    terrain sector.
    
    It should be regarded as internal use only.
    
    @internal
 */
class NITERRAIN_ENTRY NiDynamicStreamCache : public NiRefObject
{
public:

    enum STREAM_TYPES 
    {
        /// Vertex positions.
        POSITION = 0,

        /// Normals and Tangents.
        NORMAL_TANGENT = 1,

        /// Defines the maximum number of deformable stream types.
        MAX_TYPE = 2,
    };

public:

    /**
        Default constructor
        @param uiVertsPerBlock The number of vertices that will be allocated
            to each stream
     */
    NiDynamicStreamCache(NiUInt32 uiVertsPerBlock);

    /// Destructor
    virtual ~NiDynamicStreamCache();

    /**
        Request a stream of the given type from the cache.
        
        If no streams of desired type are available the cache will grow
        by the preset 'GrowBy' amount.

        @param eStreamType The type of stream requested
        @return A data stream from the cache, who's data is allocated but
            undefined.
     */
    NiDataStream* RequestStream(STREAM_TYPES eStreamType);

    /**
        Initializes the stream cache for the given type and grows it to the
        given initial size.

        @param eStreamType The stream type for which to initialize the cache
        @param uiInitialSize The number of entries that will initially be 
            available for request in the cache
        @param uiGrowBy When the cache is full, expand it by this amount
        @param kElements Elements that define the structure of each cache entry
            for this type
        @param ucAccessMask The stream access mask that will be given to each 
            cache entry
        @param eUsage The stream usage that will be given to each cache entry

        @return false if not all streams could be created. Note, that some 
            entries may have been created if this occurs
     */
    bool InitializeStreamCache(STREAM_TYPES eStreamType, 
        NiUInt32 uiInitialSize, NiUInt32 uiGrowBy,
        const NiDataStreamElementSet& kElements,
        NiUInt8 ucAccessMask, NiDataStream::Usage eUsage);

    /// Get the number of streams that have been requested for the given
    /// type.
    NiUInt32 GetCurrentSize(STREAM_TYPES eStreamType);
    
    /// Get the total number of streams that are contained within the cache
    /// for the given type.
    NiUInt32 GetMaxSize(STREAM_TYPES eStreamType);
    
    /// Get the number of streams of the given type that may be requested
    /// before the cache needs to grow.
    NiUInt32 GetNumAvailable(STREAM_TYPES eStreamType);

private:

    NiUInt32 m_uiStreamSize;

    NiTObjectArray<NiDataStreamPtr> m_akStreams[MAX_TYPE];
    NiUInt32 m_auiCurrentPosition[MAX_TYPE];

    NiDataStreamElementSet m_akElements[MAX_TYPE];
    NiDataStream::Usage m_aeUsage[MAX_TYPE];
    NiUInt8 m_aucAccessMask[MAX_TYPE];
};

NiSmartPointer(NiDynamicStreamCache);

#endif // AEDYNAMICSTREAMCACHE_H
