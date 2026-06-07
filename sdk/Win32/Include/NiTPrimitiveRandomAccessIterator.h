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

#ifndef NITPRIMITIVERANDOMACCESSITERATOR_H
#define NITPRIMITIVERANDOMACCESSITERATOR_H

#include "NiDebug.h"

#include "NiBool.h"
#include "NiTPrimitiveRandomAccessIterator.h"
#include "NiTStridedRandomAccessIterator.h"
#include "NiTIndexGenerationIterator.h"

/**
    Templated iterator class for traversing primitives using an index stream.

    Primarily for use in traversing an index buffer, consuming a
    primitive's worth of indices with each step. INDEX_TYPE is meant to be the
    C++ type (unsigned short or unsigned int) of the indices. 
    ITERATOR_TYPE is meant to contain the rate at which the indices are
    consumed. 
    
    @note There are four typedefs meant to cover the most common cases 
    of primitive iterators:
    - NiIndexedPrimitiveIterator32: Indexed 32-bit indices
    - NiIndexedPrimitiveIterator16: Indexed 16-bit indices
    - NiNonIndexedPrimitiveIterator32: Unindexed 32-bit indices
    - NiNonIndexedPrimitiveIterator16: Unindexed 16-bit indices
*/
template<typename INDEX_TYPE, typename ITERATOR_TYPE>
class NiTPrimitiveRandomAccessIterator
{
public:
    
    /**
        Nested class meant to contain the indices for a given primitive.
    */
    class NiTPrimitive
    {

    public:
        /// Constructor. for consuming an index buffer to generate primitives
        inline NiTPrimitive(const ITERATOR_TYPE& iter,
            NiUInt32 uiPrimIndexCount,
            NiBool bReverseOddWindings, NiBool bIsOdd) : m_kBuffer(iter),
            m_uiPrimIndexCount(uiPrimIndexCount), 
            m_bReverseOddWindings(bReverseOddWindings), m_bIsOdd(bIsOdd)
        {
        }

        /// Pseudo-copy constructor
        inline NiTPrimitive(const NiTPrimitive& kOther,
            NiBool bReverseOddWindings,
            NiBool bIsOdd) : m_kBuffer(kOther.m_kBuffer),
            m_uiPrimIndexCount(kOther.m_uiPrimIndexCount),
            m_bReverseOddWindings(kOther.m_bReverseOddWindings), 
            m_bIsOdd(kOther.m_bIsOdd)
        {
        }

        /// Return the number of indices in this primitive.
        inline NiUInt32 count() const 
        {
            return m_uiPrimIndexCount;
        }

        /// Return the value of the primitive at the specified array index
        inline INDEX_TYPE operator[](NiUInt32 stIndex) const 
        {
            NIASSERT(stIndex < m_uiPrimIndexCount);
            if (m_bReverseOddWindings && m_bIsOdd)
            {
                // Assume only triStrips make use of reverse windings.
                NIASSERT(m_uiPrimIndexCount == 3);
                switch(stIndex)
                {
                    case 0: return (INDEX_TYPE) m_kBuffer[1];
                    case 1: return (INDEX_TYPE) m_kBuffer[0];
                    default: return (INDEX_TYPE) m_kBuffer[stIndex];
                }
            }
            return (INDEX_TYPE) m_kBuffer[stIndex];
        }
    
        /// Alternative mechanism for consuming the primitive's indices
        inline INDEX_TYPE GetAt(NiUInt32 stIndex) const 
        {
            // Assume only triStrips make use of reverse windings.
            NIASSERT(stIndex < m_uiPrimIndexCount);
            if (m_bReverseOddWindings && m_bIsOdd)
            {
                NIASSERT(m_uiPrimIndexCount == 3);
                switch(stIndex)
                {
                    case 0: return (INDEX_TYPE) m_kBuffer[1];
                    case 1: return (INDEX_TYPE) m_kBuffer[0];
                    default: return (INDEX_TYPE) m_kBuffer[stIndex];
                }
            }
            return (INDEX_TYPE) m_kBuffer[stIndex];
        }

        /// Method for retrieving all indices at once.
        inline void Get(INDEX_TYPE* pkBuffer, NiUInt32 uiBufferSize) const
        {
            NI_UNUSED_ARG(uiBufferSize);
            NIASSERT(m_uiPrimIndexCount == uiBufferSize);
            if (m_bReverseOddWindings)
            {
                NIASSERT(m_uiPrimIndexCount == 3);
                if (m_bIsOdd)
                {
                    *pkBuffer++ = (INDEX_TYPE)m_kBuffer[1];
                    *pkBuffer++ = (INDEX_TYPE)m_kBuffer[0];
                    *pkBuffer = (INDEX_TYPE)m_kBuffer[2];
                }
                else
                {
                    *pkBuffer++ = (INDEX_TYPE)m_kBuffer[0];
                    *pkBuffer++ = (INDEX_TYPE)m_kBuffer[1];
                    *pkBuffer = (INDEX_TYPE)m_kBuffer[2];
                }
            }
            else
            {
                NiUInt32 ui = 0;
                do
                {
                    *pkBuffer++ = (INDEX_TYPE)m_kBuffer[ui++];
                } while (ui < m_uiPrimIndexCount);
            }
        }

        /// Determines whether or not this is a degenerate primitive
        inline bool IsDegenerate() const
        {
            // To be degenerate, no index can be the same as any
            // other index. This loop iterates over the indices for this 
            // primitive and determines if that index is the same as any other
            // index.
            for (NiUInt32 ui = 0; ui < m_uiPrimIndexCount; ui++)
            {
                // iterate through the rest of the array. Since we have 
                // checked from the beginning, no need to double-check the 
                // previously iterated elements.
                for (NiUInt32 uj = ui + 1; uj < m_uiPrimIndexCount; 
                    uj++)
                {
                    if (m_kBuffer[ui] == m_kBuffer[uj])
                        return true;
                }
            }

            return false;
        }

        /// Determine if this primitive contains the given token.
        inline bool ContainsRestart(INDEX_TYPE kRestartToken)
        {
            for (NiUInt32 ui = 0; ui < m_uiPrimIndexCount; ui++)
            {
                if (m_kBuffer[ui] == kRestartToken)
                    return true;
            }

            return false;
        }

        /// Index iterator
        ITERATOR_TYPE m_kBuffer;

        /// Number of indices in the given primitive
        NiUInt32 m_uiPrimIndexCount;        

        /// Should the index buffer reverse the order it provides indices for 
        /// odd primitives?
        NiBool m_bReverseOddWindings;

        /// Is the current primitive an odd number?
        NiBool m_bIsOdd;
    };

    typedef NiTPrimitive Primitive;

    /** 
        Constructor for consuming a buffer of indices.

        @param pBuffer Pointer to the index buffer.
        @param uiPrimIndexCount Number of indices in the buffer.
        @param uiPrimIndexStepCount Stride to get to the next primitive.
        @param bReverseOddWindings Should the index buffer reverse every other
            element?
        @param bIsOdd Is the current pointer to an odd element?
    */
    NiTPrimitiveRandomAccessIterator(
        INDEX_TYPE* pBuffer, 
        NiUInt32 uiPrimIndexCount,
        NiUInt32 uiPrimIndexStepCount,
        NiBool bReverseOddWindings = false,
        NiBool bIsOdd = false) :
        m_kCurrent(ITERATOR_TYPE(pBuffer, sizeof(INDEX_TYPE)),
            uiPrimIndexCount, bReverseOddWindings, bIsOdd),
        m_uiPrimIndexCount(uiPrimIndexCount),
        m_uiPrimIndexStepCount(uiPrimIndexStepCount)
    {
    }

    /** 
        Constructor for consuming indices. There is no buffer. The indices are
        generated algorithmically.

        @param uiPrimIndexCount Number of indices in the buffer.
        @param uiPrimIndexStepCount Stride to get to the next primitive.
        @param bReverseOddWindings Should the index buffer reverse every other
            element?
        @param bIsOdd Is the current pointer to an odd element?
        @param kValue The value of the starting index in the iterator.
    */
    NiTPrimitiveRandomAccessIterator(
        NiUInt32 uiPrimIndexCount, 
        NiUInt32 uiPrimIndexStepCount, 
        NiBool bReverseOddWindings = false,
        NiBool bIsOdd = false,
        INDEX_TYPE kValue = INDEX_TYPE(0)) : 
        m_kCurrent(ITERATOR_TYPE(kValue), uiPrimIndexCount,
            bReverseOddWindings, bIsOdd),
        m_uiPrimIndexCount(uiPrimIndexCount),
        m_uiPrimIndexStepCount(uiPrimIndexStepCount)
    {
    }

    /** 
        Constructor for consuming indices. This can be used with indexed or 
        generated indices.

        @param kIter Iterator for generating the buffer
        @param uiPrimIndexCount Number of indices in the buffer.
        @param uiPrimIndexStepCount Stride to get to the next primitive.
        @param bReverseOddWindings Should the index buffer reverse every other
            element?
        @param bIsOdd Is the current pointer to an odd element?
    */
    NiTPrimitiveRandomAccessIterator(
        ITERATOR_TYPE& kIter, 
        NiUInt32 uiPrimIndexCount,
        NiUInt32 uiPrimIndexStepCount,
        NiBool bReverseOddWindings = false, 
        NiBool bIsOdd = false) :
        m_kCurrent(kIter, uiPrimIndexCount, bReverseOddWindings, bIsOdd), 
        m_uiPrimIndexCount(uiPrimIndexCount),
        m_uiPrimIndexStepCount(uiPrimIndexStepCount)
    {
    }

    /// @name Data Access
    //@{
    /// Array operator
    inline Primitive operator[](size_t stIndex) const 
    {
        if (m_kCurrent.m_bIsOdd)
        {
            return Primitive(m_kCurrent.m_kBuffer + 
                stIndex * m_uiPrimIndexStepCount,
                m_uiPrimIndexCount, m_kCurrent.m_bReverseOddWindings,
                !(stIndex & 1));
        }
        else
        {
            return Primitive(m_kCurrent.m_kBuffer + 
                stIndex * m_uiPrimIndexStepCount,
                m_uiPrimIndexCount, m_kCurrent.m_bReverseOddWindings,
                (NiBool)(stIndex & 1));
        }
    } 

    /// Dereference operator *
    inline const Primitive& operator*() const
    {
        return m_kCurrent;
    }

    /// Dereference operator ->
    inline const Primitive* operator->() const
    {
        return &m_kCurrent;
    }
    //@}


    /// @name Iterator Math Operators
    //@{
    /// Prefix increment operator 
    inline NiTPrimitiveRandomAccessIterator& operator++()
    {
        m_kCurrent.m_bIsOdd = !m_kCurrent.m_bIsOdd;
        m_kCurrent.m_kBuffer += m_uiPrimIndexStepCount;
        return *(this);
    }

    /// Prefix decrement operator
    inline NiTPrimitiveRandomAccessIterator& operator--()
    {
        m_kCurrent.m_bIsOdd = !m_kCurrent.m_bIsOdd;
        m_kCurrent.m_kBuffer -= m_uiPrimIndexStepCount;
        return *(this);
    }
    
    /// Postfix increment operator
    inline NiTPrimitiveRandomAccessIterator operator++(int)
    {
        NiTPrimitiveRandomAccessIterator<INDEX_TYPE, ITERATOR_TYPE>
            kResult(m_kCurrent.m_kBuffer, m_uiPrimIndexCount,
            m_uiPrimIndexStepCount, m_kCurrent.m_bReverseOddWindings,
            m_kCurrent.m_bIsOdd);

        operator++();
        return kResult;
    }

    /// Postfix decrement operator
    inline NiTPrimitiveRandomAccessIterator operator--(int)
    {
        NiTPrimitiveRandomAccessIterator<INDEX_TYPE, ITERATOR_TYPE>
            kResult(m_kCurrent.m_kBuffer, m_uiPrimIndexCount,
            m_uiPrimIndexStepCount, m_kCurrent.m_bReverseOddWindings,
            m_kCurrent.m_bIsOdd);

        operator--();
        return kResult;
    }

    /// Pointer math += operator
    inline NiTPrimitiveRandomAccessIterator& operator+=(ptrdiff_t dtElements)
    {
        if (m_kCurrent.m_bIsOdd)
            m_kCurrent.m_bIsOdd = !(dtElements & 1);
        else
            m_kCurrent.m_bIsOdd = (NiBool)(dtElements & 1);

        m_kCurrent.m_kBuffer += (m_uiPrimIndexStepCount * dtElements);
        return *(this);
    }

    /// Pointer math -= operator
    inline NiTPrimitiveRandomAccessIterator& operator-=(ptrdiff_t dtElements)
    {
        if (m_kCurrent.m_bIsOdd)
            m_kCurrent.m_bIsOdd = !(dtElements & 1);
        else
            m_kCurrent.m_bIsOdd = (NiBool)(dtElements & 1);

        m_kCurrent.m_kBuffer -= (m_uiPrimIndexStepCount * dtElements);
        return *(this);
    }

    /// Pointer math + operator
    inline NiTPrimitiveRandomAccessIterator operator+(
        ptrdiff_t dtElements) const
    {
        ITERATOR_TYPE kIter(m_kCurrent.m_kBuffer + 
            (m_uiPrimIndexStepCount * dtElements));
        if (m_kCurrent.m_bIsOdd)
        {
            NiTPrimitiveRandomAccessIterator<INDEX_TYPE, ITERATOR_TYPE> 
                kResult(kIter, m_uiPrimIndexCount,
                m_uiPrimIndexStepCount, m_kCurrent.m_bReverseOddWindings, 
                (NiBool)!(dtElements & 1)); // Odd + (odd, even) = (even, odd)
            return kResult;
        }
        else
        {
            NiTPrimitiveRandomAccessIterator<INDEX_TYPE, ITERATOR_TYPE> 
                kResult(kIter, m_uiPrimIndexCount,
                m_uiPrimIndexStepCount, m_kCurrent.m_bReverseOddWindings, 
                (NiBool)(dtElements & 1)); // Even + (odd, even) = (odd, even)
            return kResult;
        }
    }

    /// Pointer math - operator
    inline NiTPrimitiveRandomAccessIterator operator-(
        ptrdiff_t dtElements) const
    {
        ITERATOR_TYPE kIter(m_kCurrent.m_kBuffer -
            (m_uiPrimIndexStepCount * dtElements));
        if (m_kCurrent.m_bIsOdd)
        {
            NiTPrimitiveRandomAccessIterator<INDEX_TYPE, ITERATOR_TYPE> 
                kResult(kIter, m_uiPrimIndexCount, m_uiPrimIndexStepCount,
                m_kCurrent.m_bReverseOddWindings, !(dtElements & 1));

            return kResult;
        }
        else
        {
            NiTPrimitiveRandomAccessIterator<INDEX_TYPE, ITERATOR_TYPE> 
                kResult(kIter, m_uiPrimIndexCount, m_uiPrimIndexStepCount,
                m_kCurrent.m_bReverseOddWindings, (bool)(dtElements & 1));

            return kResult;
        }        
    }

    /// Pointer math difference operator; valid only for iterators pointing 
    /// to the same buffer.
    inline ptrdiff_t operator-(
        const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return (m_kCurrent.m_kBuffer - kRhs.m_kCurrent.m_kBuffer) / 
            m_uiPrimIndexStepCount;
    }
    //@}
    
    /// @name Iterator Comparison Operators
    //@{
    /// Equality operator
    inline bool operator==(const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return m_kCurrent.m_kBuffer == kRhs.m_kCurrent.m_kBuffer;
    }
    /// Inequality operator
    inline bool operator!=(const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return m_kCurrent.m_kBuffer != kRhs.m_kCurrent.m_kBuffer;
    }
    /// Less than operator
    inline bool operator<(const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return m_kCurrent.m_kBuffer < kRhs.m_kCurrent.m_kBuffer;
    }
    /// Greater than operator
    inline bool operator>(const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return m_kCurrent.m_kBuffer > kRhs.m_kCurrent.m_kBuffer;
    }
    /// Less than or equal to operator
    inline bool operator<=(const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return m_kCurrent.m_kBuffer <= kRhs.m_kCurrent.m_kBuffer;
    }
    /// Greater than or equal to operator
    inline bool operator>=(const NiTPrimitiveRandomAccessIterator& kRhs) const
    {
        return m_kCurrent.m_kBuffer >= kRhs.m_kCurrent.m_kBuffer;
    }
    //@}

protected:

    /// Current primitive
    Primitive m_kCurrent;

    /// Number of indices in the buffer
    NiUInt32 m_uiPrimIndexCount;

    /// Step size to the next primitive
    NiUInt32 m_uiPrimIndexStepCount;
};

/// Random access iterator for a 32-bit index primitive buffer
typedef NiTPrimitiveRandomAccessIterator<NiUInt32,
    NiTStridedRandomAccessIterator<NiUInt32> > NiIndexedPrimitiveIterator32;

/// Random access iterator for a 16-bit index primitive buffer
typedef NiTPrimitiveRandomAccessIterator<NiUInt16,
    NiTStridedRandomAccessIterator<NiUInt16> > NiIndexedPrimitiveIterator16;

/// Random access iterator for a 32-bit unindexed primitive
typedef NiTPrimitiveRandomAccessIterator<NiUInt32,
    NiTIndexGenerationIterator<NiUInt32> > NiNonIndexedPrimitiveIterator32;

/// Random access iterator for a 16-bit unindexed primitive
typedef NiTPrimitiveRandomAccessIterator<NiUInt16,
    NiTIndexGenerationIterator<NiUInt16> > NiNonIndexedPrimitiveIterator16;

#endif // #ifndef NITPRIMITIVERANDOMACCESSITERATOR_H
