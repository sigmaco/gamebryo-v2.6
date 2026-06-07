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

#ifndef NITINDEXGENERATIONITERATOR_H
#define NITINDEXGENERATIONITERATOR_H

/**
    Templated class to generate a stream of indices algorithmically.
*/
template<typename T>
class NiTIndexGenerationIterator
{
    T m_kValue;
public:
    
    /// Construct an iterator that starts with index kValue
    NiTIndexGenerationIterator(T kValue);
    
    /// Construct an iterator that starts with zero
    NiTIndexGenerationIterator();

    /// @name Iterator math operators
    //@{
    /// Prefix increment operator 
    NiTIndexGenerationIterator& operator++();

    /// Prefix decrement operator
    NiTIndexGenerationIterator& operator--();
    
    /// Postfix increment operator
    NiTIndexGenerationIterator operator++(int);

    /// Postfix decrement operator
    NiTIndexGenerationIterator operator--(int);

    /// Pointer math += operator
    NiTIndexGenerationIterator& operator+=(ptrdiff_t dtElements);

    /// Pointer math -= operator
    NiTIndexGenerationIterator& operator-=(ptrdiff_t dtElements);

    /// Pointer math + operator
    NiTIndexGenerationIterator operator+(ptrdiff_t dtElements) const;

    /// Pointer math - operator
    NiTIndexGenerationIterator operator-(ptrdiff_t dtElements) const;

    /// Pointer math difference operator; valid only for iterators pointing 
    /// to the same buffer.
    inline ptrdiff_t operator-(const NiTIndexGenerationIterator& kRhs) const;
    //@}

    /// @name Data Access
    //@{
    /// Dereference operator *
    inline T operator*() const;

    /// Dereference operator ->
    inline const T* operator->() const;

    /// Indexing operator
    inline T operator[](size_t stIndex) const;

    /// @name Iterator Comparison Operators
    //@{
    /// Equality operator
    inline bool operator==(const NiTIndexGenerationIterator& kRhs) const;
    /// Inequality operator
    inline bool operator!=(const NiTIndexGenerationIterator& kRhs) const;
    /// Less than operator
    inline bool operator<(const NiTIndexGenerationIterator& kRhs) const;
    /// Greater than operator
    inline bool operator>(const NiTIndexGenerationIterator& kRhs) const;
    /// Less than or equal to operator
    inline bool operator<=(const NiTIndexGenerationIterator& kRhs) const;
    /// Greater than or equal to operator
    inline bool operator>=(const NiTIndexGenerationIterator& kRhs) const;
    //@}

};

#include "NiTIndexGenerationIterator.inl"

#endif // #ifndef NITINDEXGENERATIONITERATOR_H
