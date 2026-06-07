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

#ifndef NITSIMPLEARRAY_H
#define NITSIMPLEARRAY_H

/**
    Templated fixed-size array class. 

    Useful when using an array on the stack. It is also useful to 
    describe a nested array for use with an NiTStridedRandomAccessIterator for
    vertex elements like blend indices or blend weights.
*/
template<typename T, size_t COUNT>
class NiTSimpleArray
{
private:
    /// A fixed-size array of COUNT elements.
    T m_kArray[COUNT];
public:
    /// Get the number of elements.
    inline size_t Count() const;

    /// Indexing operator.
    inline T& operator[](size_t stIndex);
    
    /// Indexing operator (const-safe).
    inline const T& operator[](size_t stIndex) const;
};

#include "NiTSimpleArray.inl"

#endif // #ifndef NITSIMPLEARRAY_H
