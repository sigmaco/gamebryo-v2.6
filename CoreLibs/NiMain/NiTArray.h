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

#ifndef NITARRAY_H
#define NITARRAY_H

#include "NiRTLib.h"
#include "NiTCollection.h"

// While the template arrays work best for pointer types T, other types
// certainly will work when they satisfy the conditions below.  The template
// class assumes that type T has the following:
//   1.  A "zero" element (i.e., T var; var = 0; is supported)
//       which is considered to be a null array element.
//   2.  The default constructor for T must exist and create the
//       "zero" element.  The constructor must also handle all necessary
//       actions for constructing elements.  That is, the template array
//       class cannot make any post-construction member calls that are
//       specific to class T.
//   3.  Copy constructor T::T(const T&) must work properly.  The class T is
//       responsible for implementing this if need be.
//   4.  The destructor must handle all necessary actions for destroying
//       elements.  That is, the template array class cannot make any
//       pre-destruction member calls that are specific to class T.
//   5.  bool operator== (const T&);
//   6.  bool operator!= (const T&);
//   7.  T& operator= (const T&);
//
// An example to illustrate what the members of NiTArray mean.  Shown is
// an array of elements (0 = null element, x = an element)
//     index:    0 1 2 3 4 5 6 7 8 9
//     element:  x 0 x x 0 0 x 0 0 0
//
//     m_uiMaxSize = 10 (number of slots in array)
//     m_uiSize    =  7 (next available slot, useful for traversing minimum
//                       block of elements while searching for non-null items)
//     m_uiESize   =  4 (number of used slots)
//
// Note that when m_uiSize = m_uiMaxSize, an attempt to add a new element
// requires growing the array.  SetAtGrow does this.  SetAt does not.
//
// Note that there is an additional template argument, TAlloc.
// This class must declare static functions matching the following prototypes:
//
//      static T* Allocate(unsigned int uiNumElements)
//      static void Deallocate(T* pArray)

template <class T, class TAlloc> class NiTArray : public NiMemObject
{
public:
    // construction and destruction
    NiTArray(unsigned int uiMaxSize = 0, unsigned int uiGrowBy = 1);
    virtual ~NiTArray();

    // Number of slots used in array, including zeroed items.
    // Use this number to iterate over all items in array, (some may be 0)
    inline unsigned int GetSize() const; 

    // Number of non-zero items in array, less or equal to GetSize()
    inline unsigned int GetEffectiveSize() const;

    inline unsigned int GetAllocatedSize() const;
    inline unsigned int GetGrowBy() const;
    void SetSize(unsigned int uiSize);
    inline void SetGrowBy(unsigned int uiGrowBy);

    // set and get elements
    inline T* GetBase();
    inline const T& GetAt(unsigned int uiIndex) const;
    inline T& GetAt(unsigned int uiIndex);
    void SetAt(unsigned int uiIndex, const T& element);
    unsigned int SetAtGrow(unsigned int uiIndex, const T& element);
    
    // add and remove elements
    inline unsigned int Add(const T& element);
    unsigned int AddFirstEmpty(const T& element);
    inline T RemoveAt(unsigned int uiIndex);
    // Removes last element in array and places it at uiIndex 
    inline T RemoveAtAndFill(unsigned int uiIndex);
    inline T RemoveEnd();
    inline void RemoveAll();

    // Remove the first found occurrence of the specified element.  If the
    // element is not T(0) and is in the array, the array slot is set to T(0)
    // and its index is returned.  If the element is T(0) or not in the array,
    // the maximum unsigned int is returned ((unsigned int)~0).  NOTE:  If
    // the element occurs multiple times in the array, only the first
    // occurrence is removed.
    inline unsigned int Remove(const T& element);

    // Find the first occurance of the specified element from the 
    // starting index. If the element is not T(0) and is in the array, 
    // the array index is returned. If the element is T(0) or not in the array,
    // the maximum unsigned int is returned ((unsigned int)~0).  NOTE:  If
    // the element occurs multiple times in the array, only the first
    // occurrence from the starting index is found.
    inline unsigned int Find(const T& element, unsigned int uiStartIdx = 0);

    // Compact all elements to contiguous space starting at the beginning of
    // the array.  Reallocation is performed to eliminate unused slots.
    inline void Compact();

    // After deletions before m_uiSize slot, m_uiSize no longer points to
    // the first available slot.  This routine resets it to the first
    // available slot.
    inline void UpdateSize();

// *** begin Emergent internal use only ***

    // Operator to simulate access like a conventional array
    inline T& operator [] (unsigned int uiIndex) const;

    // Slow insert and delete functions to maintain the order
    inline void Insert(unsigned int uiIndex, const T& element);
    inline T Delete(unsigned int uiIndex);

// *** end Emergent internal use only ***
    

protected:
    T* m_pBase;                // pointer to the array storage
    unsigned short m_usMaxSize;  // number of slots in array
    unsigned short m_usSize;     // first available empty slot in array
    unsigned short m_usESize;    // number of filled slots
    unsigned short m_usGrowBy;   // number of slots to grow array when full

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTArray(const NiTArray&);
    NiTArray& operator=(const NiTArray&);
};

template <class T> class NiTObjectArray : public 
    NiTArray<T, NiTNewInterface<T> >
{
public:
    NiTObjectArray(unsigned int uiMaxSize = 0, unsigned int uiGrowBy = 1);
};

template <class T> class NiTPrimitiveArray : public 
    NiTArray<T, NiTMallocInterface<T> >
{
public:
    NiTPrimitiveArray(unsigned int uiMaxSize = 0, unsigned int uiGrowBy = 1);
};


#include "NiTArray.inl"

#endif // NITARRAY_H
