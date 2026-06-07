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

#ifndef NITTERRAINRANDOMACCESSITERATOR_H
#define NITTERRAINRANDOMACCESSITERATOR_H

#include "NiTerrainConfiguration.h"

///@cond EMERGENT_INTERNAL

/**
    This class acts as a replacement of the NiTStridedRandomAccessIterator, for
    special use only within the terrain library. Its primary purpose is to act
    as an abstraction layer between the raw stream data and the information it
    represents.

    This allows us to arbitrarily enable and disable morphing and compression
    on the data streams, change the number of components etc without having
    to modify the program flow in the rest of the terrain library.

    Currently this class supports Position, Normal and Tangent streams. UV and
    Index streams are not supported, since they are generally not affected by
    the NiTerrainConfiguration class.
 */
template<int T>
class NITERRAIN_ENTRY NiTTerrainRandomAccessIterator
{
public:

    /// The types of streams supported by iterator. Used for template 
    /// specialization
    enum TERRAIN_STREAM_ELEMENTS
    {
        POSITION,
        NORMAL,
        TANGENT
    };

    /// Stream components
    enum COMPONENT
    {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3
    };

    /// Default constructor
    inline NiTTerrainRandomAccessIterator();

    /**
        Parameterized constructor

        Initializes the iterator, pointing it to the start of the specified
            region in the stream pointed to by the given lock.

        Compression and morphing settings are read from the given configuration
            which define the stride of the stream.

        @param pkLock Lock which the stream buffer that we will be iterating
            over
        @param uiRegionIndex Stream region at which to begin iteration
        @param kConfiguration Terrain configuration from which to read 
            morphing, compression, stride and component details
     */
    inline NiTTerrainRandomAccessIterator(NiDataStreamLock* pkLock,
        NiUInt32 uiRegionIndex, const NiTerrainConfiguration& kConfiguration);

    /**
        Get the raw data from the stream at the given index.

        Use with caution; these should only be used if you dont want any
            decomression to occur and know the component size of the stream.
     */
    //@{
    inline void Get(NiUInt32 uiIndex, NiPoint4& kPoint) const;
    inline void Get(NiUInt32 uiIndex, NiPoint3& kPoint) const;
    inline void Get(NiUInt32 uiIndex, NiPoint2& kPoint) const;
    //@}

    /**
        Retrieve the value of the specified component of the point at the given
        index.

        Use with caution; the stream must contain the component you request.
     */
    inline float GetComponent(NiUInt32 uiIndex, COMPONENT eComponent) const;

    /**
        Set the raw data at the specified index.

        Use with caution; these should only be used if you dont want any
            comression to occur and know the component size of the stream.
     */
    inline void Set(NiUInt32 uiIndex, const NiPoint4& kPoint) const;
    inline void Set(NiUInt32 uiIndex, const NiPoint3& kPoint) const;
    inline void Set(NiUInt32 uiIndex, const NiPoint2& kPoint) const;

    /**
        Set the value of the specified component of the point at the given
        index.

        Use with caution; the stream must contain the component you request.
    */
    inline void SetComponent(
        NiUInt32 uiIndex, COMPONENT eComponent, float fValue) const;

    /**
        Get and Set values in the streams. These functions are the preferred
        method of accessing data in the streams.

        High detail corresponds to the 'regular' data in a stream; it is always
            present regardless of the NiTerrainConfiguration in use.

        Low detail corresponds to the 'morphing' data in a stream; it is only
            present when compression and morphing are enabled in the active
            terrain configuration.

        Both high and low detail information can be present at any given index 
            on a stream.

        There are NiPoint2 and NiPoint3 variations of all these accessor 
            functions; the iterator will automatically convert the stream data
            to the format you requested (and vice versa). NiPoint2 corresponds
            to a 'compressed' entry, NiPoint3 is 'uncompressed'.

        Even if a stream is marked as uncompressed, you can get the data in a
            compressed form by simply calling the NiPoint2 variation of the
            function. Likeways if a stream is marked as compressed, you can
            set and retrieve the data in an uncompressed form by calling the
            NiPoint3 variation. The iterator will convert the data as required.
     */
    //@{
    inline void GetHighDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const;
    inline void GetLowDetail(NiUInt32 uiIndex, NiPoint3& kPoint) const;
    inline void GetHighDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const;
    inline void GetLowDetail(NiUInt32 uiIndex, NiPoint2& kPoint) const;
    inline void SetHighDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const;
    inline void SetLowDetail(NiUInt32 uiIndex, const NiPoint3& kPoint) const;
    inline void SetHighDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const;
    inline void SetLowDetail(NiUInt32 uiIndex, const NiPoint2& kPoint) const;
    //@}

    /// Total stride of all elements in the stream
    inline NiUInt32 GetTotalStride() const;

    /// Stride of just the element this iterator corresponds too
    inline NiUInt32 GetElementStride() const;

    /// True if morphing was enabled in the NiTerrainConfiguration provided to
    /// this iterator
    inline bool GetMorphingEnabled() const;

    /// True if compression was enabled on the normal and tangent streams in 
    /// the NiTerrainConfiguration provided to this iterator
    inline bool GetCompressionEnabled() const;

    /// True if the stream buffer is valid
    inline bool Exists() const;

    /// Copy the entry from the given iterator to this iterator
    inline void CopyFrom(NiUInt32 uiDestIndex, 
        const NiTTerrainRandomAccessIterator<T>& kSource, 
        NiUInt32 uiSourceIndex) const;

protected:

    float* m_pfBuffer;
    NiUInt32 m_uiElementStride;
    NiUInt32 m_uiTotalStride;
    bool m_bCompressionEnabled;
    bool m_bMorphingEnabled;
};

#include "NiTTerrainRandomAccessIterator.inl"

/// Specialized iterator for use on Position streams
typedef NiTTerrainRandomAccessIterator<
    NiTTerrainRandomAccessIterator<0>::POSITION> 
    NiTerrainPositionRandomAccessIterator;

/// Specialized iterator for use on Normal streams
typedef NiTTerrainRandomAccessIterator<
    NiTTerrainRandomAccessIterator<0>::NORMAL> 
    NiTerrainNormalRandomAccessIterator;

/// Specialized iterator for use on Tangent streams
typedef NiTTerrainRandomAccessIterator<
    NiTTerrainRandomAccessIterator<0>::TANGENT> 
    NiTerrainTangentRandomAccessIterator;

///@endcond

#endif // NITTERRAINRANDOMACCESSITERATOR_H
