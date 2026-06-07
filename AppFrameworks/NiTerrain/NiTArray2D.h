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

#ifndef NITARRAY2D_H
#define NITARRAY2D_H

#include <NiMemManager.h>

/**
    A two dimensional version of the standard NiTArray which can exist with
    independent sizes on both the X and Y dimension. 

    End users should not use NiTArray2D instances directly. Instead 
    NiTObjectArray2D or NiTPrimitiveArray2D should be used.
 */
template <class T, class TAlloc> class NiTArray2D : 
    public NiMemObject
{
public:

    /**
        Constructor.

        @param uiMaxSizeX Specify the initial amount of memory allocated for 
            the X dimension
        @param uiMaxSizeY Specify the initial amount of memory allocated for 
            the Y dimension
        @param uiGrowBy The amount that the array will grow in the required
            dimension, if it becomes full. Must be greater than zero
     */
    NiTArray2D(NiUInt32 uiMaxSizeX, NiUInt32 uiMaxSizeY, NiUInt32 uiGrowBy);
    
    /// Virtual Destructor
    virtual ~NiTArray2D();

    /// Get the current allocated size of the array in the X dimension.
    NiUInt32 GetMaxX() const;

    /// Get the current allocated size of the array in the Y dimension.
    NiUInt32 GetMaxY() const;

    /// Set the current allocated size of the array in both dimensions.
    void SetSize(NiUInt32 uiMaxSizeX, NiUInt32 uiMaxSizeY);

    /// Set the ratio by which to grow an array dimension by when it becomes
    /// full.
    void SetGrowBy(NiUInt32 uiGrowBy);

    /// Get the ratio by which to grow an array dimension by when it becomes
    /// full.
    NiUInt32 GetGrowBy() const;

    /**
        Get the value of the entry at the given position in the array.
        @note The index is assumed to be in range for efficiency reasons.
     */
    const T& GetAt(NiUInt32 uiLocX, NiUInt32 uiLocY) const;

    /**
        Set the value of the entry at the given position in the array.
        @note The index is assumed to be in range for efficiency reasons.
     */
    void SetAt(NiUInt32 uiLocX, NiUInt32 uiLocY, const T& element);

    /**
        Set the value of the entry at the given position in the array. If the
        position is out of the current allocated space, then the array is 
        resized in the required dimensions.
     */
    NiUInt32 SetAtGrow(NiUInt32 uiLocX, NiUInt32 uiLocY, const T& element);

    /// Set the value of the element at the given position to T(0)
    T RemoveAt(NiUInt32 uiLocX, NiUInt32 uiLocY);

    /// Set the value of all elements in the array to T(0)
    inline void RemoveAll();

protected:

    NiUInt32 m_uiGrowBy;
    NiUInt32 m_uiMaxSizeX;
    NiUInt32 m_uiMaxSizeY;

    T* m_pBase; 

private:
    
    /// Private virtual constructor to silence linker errors    
    NiTArray2D();

};

/**
    A two dimensional version of the standard NiTObjectArray which can exist
    with independent sizes on both the X and Y dimension.
*/
template <class T> class NiTObjectArray2D : public 
    NiTArray2D<T, NiTNewInterface<T> >
{
public:
    /**
        Constructor.

        @param uiMaxSizeX Specify the initial amount of memory allocated for 
            the X dimension
        @param uiMaxSizeY Specify the initial amount of memory allocated for 
            the Y dimension
        @param uiGrowBy The amount that the array will grow in the required
            dimension, if it becomes full. Must be greater than zero
     */
    NiTObjectArray2D(NiUInt32 uiMaxSizeX = 0, 
                     NiUInt32 uiMaxSizeY = 0, 
                     NiUInt32 uiGrowBy = 1);
};

/**
    A two dimensional version of the standard NiTPrimitiveArray which can
    exist with independent sizes on both the X and Y dimension.
*/
template <class T> class NiTPrimitiveArray2D : public 
    NiTArray2D<T, NiTMallocInterface<T> >
{
public:
    /**
        Constructor.

        @param uiMaxSizeX Specify the initial amount of memory allocated for 
            the X dimension
        @param uiMaxSizeY Specify the initial amount of memory allocated for 
            the Y dimension
        @param uiGrowBy The amount that the array will grow in the required
            dimension, if it becomes full. Must be greater than zero
     */
    NiTPrimitiveArray2D(NiUInt32 uiMaxSizeX = 0, 
                        NiUInt32 uiMaxSizeY = 0, 
                        NiUInt32 uiGrowBy = 1);
};

#include "NiTArray2D.inl"

#endif // NITARRAY2D_H
