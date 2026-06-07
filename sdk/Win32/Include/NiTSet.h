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

#ifndef NITSET_H
#define NITSET_H

// This template class implements an unordered set. Because it does not
// set removed elements to 0, it should not be used for smart pointers.
//
// Note that there is an additional template argument, TAlloc.
// This class must declare static functions matching the following prototypes:
//
//      static T* Allocate(unsigned int uiNumElements)
//      static void Deallocate(T* pArray)

#include <NiRTLib.h>
#include <NiMemObject.h>

#include "NiTCollection.h"

template <class T, class TAlloc> class NiTSet  : public NiMemObject
{
public:
    NiTSet(unsigned int uiInitialSize = 0);
    ~NiTSet();

    inline unsigned int GetSize() const;
    inline T *GetBase() const;
    inline const T& GetAt(unsigned int uiIndex) const;
    inline T& GetAt(unsigned int uiIndex);
    inline unsigned int Add(const T& element);
    inline unsigned int AddUnique(const T& element); // Slow. Use with caution.
    inline void RemoveAt(unsigned int uiIndex);
    inline void OrderedRemoveAt(unsigned int uiIndex); // Preserve order.
    inline void ReplaceAt(unsigned int uiIndex, const T& element);
    inline void RemoveAll();
    inline int Find(const T& element) const; // Slow. Use with caution.
    
    // Resize the array to be the number of used items.
    inline void Realloc();

    // This method clamp the size to being >= m_uiUsed.
    void Realloc(unsigned int uiNewSize);

    // This method will reallocate only if the new size is greater than
    // the size already allocated.
    inline void ReallocNoShrink(unsigned int uiNewSize);

    // Get the maximum number of items before a reallocation
    // is forced.
    inline unsigned int GetAllocationSize() const;
protected: 
    T *m_pBase;
    unsigned int m_uiAlloced;
    unsigned int m_uiUsed;

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTSet(const NiTSet&);
    NiTSet& operator=(const NiTSet&);
};

template <class T> class NiTObjectSet: 
    public NiTSet<T, NiTNewInterface<T> >
{
public:
    NiTObjectSet(unsigned int uiInitialSize = 0);
};

template <class T> class NiTPrimitiveSet: 
    public NiTSet<T, NiTMallocInterface<T> >
{
public:
    NiTPrimitiveSet(unsigned int uiInitialSize = 0);
};


typedef NiTPrimitiveSet<unsigned int> NiUnsignedIntSet;
typedef NiTPrimitiveSet<unsigned short> NiUnsignedShortSet;
typedef NiTPrimitiveSet<float> NiFloatSet;

#include "NiTSet.inl"

#endif // NITSET_H
