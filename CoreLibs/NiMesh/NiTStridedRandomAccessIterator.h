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

#ifndef NITSTRIDEDRANDOMACCESSITERATOR_H
#define NITSTRIDEDRANDOMACCESSITERATOR_H

/**
    Templated iterator that traverses a byte buffer at a fixed
    rate.

    This class is useful for iterating the vertex elements of an NiDataStream.
    T can be any class or primitive type. 
    @note The sizeof T does not have to be the same size as the stride. This 
    may be useful when the compiler inserts padding at the end of a class or
    when a user only wishes to traverse one element of a compound structure.
*/
template<typename T>
class NiTStridedRandomAccessIterator
{
private:
    /// The pointer to the internal byte buffer.
    char* m_pcObject;

    /// The size between elements in the stream.
    ptrdiff_t m_dtStride;
public:
    /// Construct an uninitialized iterator
    NiTStridedRandomAccessIterator();
    
    /// Construct an iterator that points to pkObject in a buffer 
    /// with stride dtStride
    NiTStridedRandomAccessIterator(T* pkObject, ptrdiff_t dtStride);
    
    /// Existence test. True if the iterator can be accessed
    inline bool Exists() const;

    /// @name Iterator Math Operators
    //@{
    /// Prefix increment operator 
    NiTStridedRandomAccessIterator& operator++();

    /// Prefix decrement operator
    NiTStridedRandomAccessIterator& operator--();
    
    /// Postfix increment operator
    NiTStridedRandomAccessIterator operator++(int);

    /// Postfix decrement operator
    NiTStridedRandomAccessIterator operator--(int);

    /// Pointer math += operator
    NiTStridedRandomAccessIterator& operator+=(ptrdiff_t dtElements);

    /// Pointer math -= operator
    NiTStridedRandomAccessIterator& operator-=(ptrdiff_t dtElements);

    /// Pointer math + operator
    NiTStridedRandomAccessIterator operator+(ptrdiff_t dtElements) const;

    /// Pointer math - operator
    NiTStridedRandomAccessIterator operator-(ptrdiff_t dtElements) const;

    /// Pointer math difference operator; valid only for iterators pointing 
    /// to the same buffer.
    inline ptrdiff_t operator-(
        const NiTStridedRandomAccessIterator& kRhs) const;
    //@}

    /// @name Data Access
    //@{
    /// Dereference operator *
    inline T& operator*() const;

    /// Dereference operator ->
    inline T* operator->() const;

    /// Indexing operator
    inline T& operator[](size_t stIndex) const;
    //@}

    /// @ Iterator Comparison Operators
    //@{
    /// Equality operator
    inline bool operator==(const NiTStridedRandomAccessIterator& kRhs) const;
    /// Inequality operator
    inline bool operator!=(const NiTStridedRandomAccessIterator& kRhs) const;
    /// Less than operator
    inline bool operator<(const NiTStridedRandomAccessIterator& kRhs) const;
    /// Greater than operator
    inline bool operator>(const NiTStridedRandomAccessIterator& kRhs) const;
    /// Less than or equal to operator
    inline bool operator<=(const NiTStridedRandomAccessIterator& kRhs) const;
    /// Greater than or equal to operator
    inline bool operator>=(const NiTStridedRandomAccessIterator& kRhs) const;
    //@}

};

#include "NiTStridedRandomAccessIterator.inl"

#endif // #ifndef NITSTRIDEDRANDOMACCESSITERATOR_H
