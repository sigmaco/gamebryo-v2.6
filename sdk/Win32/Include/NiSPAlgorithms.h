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

#ifndef NISPALGORITHMS_H
#define NISPALGORITHMS_H

#include <NiUniversalTypes.h>
#include "NiFloodgateLibType.h"

/**
    The NiSPAlgorithms class encapsulates Floodgate-related algorithms.

    Quick sort on a collection and 128 byte alignment functions are provided.
*/
class NIFLOODGATE_ENTRY NiSPAlgorithms
{
public:
    /**
        @name Sorting Methods.
        Helper methods to assist in the sorting of data collections.
    */
    //@{
    /**
        Sorts a collection of objects using quick sort.

        C is the type of container class pkCollection. T is the type of
        contained object in pkCollection (e.g. NiObject).

        @param pkCollection The collection to sort.
        @param iLeft Leftmost index.
        @param iRight Rightmost index.
    */
    template <class C, class T>
    static void QuickSortObjects(C* pkCollection, NiInt32 iLeft, 
        NiInt32 iRight);

    /**
        Sorts a collection of pointers-to-objects using quick sort.

        C is the type of container class pkCollection. T is the type of
        contained object in pkCollection (e.g. NiObjectPtr or NiObject*).

        @param pkCollection The collection to sort.
        @param iLeft Leftmost index.
        @param iRight Rightmost index.
    */
    template <class C, class T>
    static void QuickSortPointers(C* pkCollection, NiInt32 iLeft, 
        NiInt32 iRight);
    //@}

    /**
        @name Alignment Helper Methods
        Helper methods to assist in the alignment of data.
    */
    //@{
    /**
        Compute the lowest multiple of 128 bytes that is greater than or
        equal to the given size.

        @param uiSize Original size to align.
        @return Aligned buffer size.
    */
    static NiUInt64 Align128(NiUInt64 uiSize);

    /**
        Align the given pointer to a 128 byte boundary that is lower than
        or equal to the pointer.

        @param uiSize The original pointer to align.
        @return Aligned address.
    */
    static void* PreAlign128(void* uiSize);
    //@}

protected:

    /**
        Partitions the collection of objects.

        @param pkCollection The collection to partition.
        @param iLeft Leftmost index.
        @param iRight Rightmost index.
        @return Pivot point.
    */
    template <class C, class T>
    static NiInt32 PartitionObjects(C* pkCollection, NiInt32 iLeft, 
        NiInt32 iRight);

    /**
        Partitions the collection of objects.

        @param pkCollection The collection to partition.
        @param iLeft Leftmost index.
        @param iRight Rightmost index.
        @return Pivot point.
    */
    template <class C, class T>
    static NiInt32 PartitionPointers(C* pkCollection, NiInt32 iLeft, 
        NiInt32 iRight);

    /**
        Swaps objects located at the indices specified.

        @param pkCollection The collection to partition.
        @param iIndex1 Leftmost index.
        @param iIndex2 Rightmost index.
        @return Pivot point.
    */
    template <class C, class T>
    static void SwapObjects(C* pkCollection, NiInt32 iIndex1, 
        NiInt32 iIndex2);

    /**
        Swaps ponters located at the indices specified.

        @param pkCollection The collection to partition.
        @param iIndex1 Leftmost index.
        @param iIndex2 Rightmost index.
    */
    template <class C, class T>
    static void SwapPointers(C* pkCollection, NiInt32 iIndex1, 
        NiInt32 iIndex2);

    /// Protected constructor.
    inline NiSPAlgorithms(){}

    /// Destructor.
    inline ~NiSPAlgorithms(){}
};

#include "NiSPAlgorithms.inl"

#endif  // #ifndef NISPALGORITHMS_H
