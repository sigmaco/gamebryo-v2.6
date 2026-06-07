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

#ifndef NIINDEX_H
#define NIINDEX_H

#include <NiMemManager.h>
#include <NiMemObject.h>
#include <NiUniversalTypes.h>

#include "NiTerrainLibType.h"

/**
    A simple class that represents an X-Y index in a 2D grid, which provides
    some common arithmetic operators.
*/
class NITERRAIN_ENTRY NiIndex : public NiMemObject 
{
public:
    
    /// X component
    NiUInt32 x;

    /// Y component
    NiUInt32 y;

    /// Static index with both X and Y as zero
    static const NiIndex ZERO;

    /// Static index with both X and Y as one
    static const NiIndex UNIT;

    /// Default constructor, initializes values to zero
    NiIndex() : x(0), y(0) {};

    /**
        Parameterized constructor.
        
        @param _x Initial value for the x component
        @param _y Initial value for the y component
     */
    NiIndex(NiUInt32 _x, NiUInt32 _y) : x(_x), y(_y) {};

    /// Equality check against another index
    inline bool operator== (const NiIndex& pt) const;

    /// Inequality check against another index
    inline bool operator!= (const NiIndex& pt) const { return !(*this == pt); }

    /// Element-wise addition of given index
    inline NiIndex operator+ (const NiIndex& pt) const;

    /// Element-wise subtraction of given index
    inline NiIndex operator- (const NiIndex& pt) const;

    /// Sum of element-wise multiplication
    inline NiUInt32 operator* (const NiIndex& pt) const;

    /// Multiplication of each element by given scalar
    inline NiIndex operator* (NiUInt32 scalar) const;
    
    /// Division of each element by given scalar
    inline NiIndex operator/ (NiUInt32 scalar) const;

    /// Element-wise multiplication by scalar and index
    friend NiIndex operator* (NiUInt32 scalar, 
        const NiIndex& pt);

    /// Element-wise addition of index to this index
    inline NiIndex& operator+= (const NiIndex& pt);

    /// Element-wise subtraction of index to this index
    inline NiIndex& operator-= (const NiIndex& pt);

    /// Element-wise multiplication by scalar to this index
    inline NiIndex& operator*= (NiUInt32 fScalar);

    /// Element-wise division by scalar to this index
    inline NiIndex& operator/= (NiUInt32 fScalar);
};

#include "NiIndex.inl"

#endif // NITERRAININDEX
