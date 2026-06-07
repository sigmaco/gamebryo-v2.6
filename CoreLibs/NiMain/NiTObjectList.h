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

#ifndef NITOBJECTLIST_H
#define NITOBJECTLIST_H

#include "NiMainLibType.h"
#include "NiRTLib.h"

#include "NiTListBase.h"
#include "NiTObjectAllocator.h"

// NiTObjectList is designed to hold compound objects efficiently. It
// is responsible for allocating and freeing the elements in the list. The
// interface does not support copy construction assignment of T directly.
// Instead, element access is provided via a pointer to the element.
// T is block allocated from a shared memory pool. The template class 
// requires that a default constructor for T exists, even if it does nothing.
//
// The pool uses some non-local static variables that must be declared using
// the NiAllocatorDeclareStatics macro. For example:
//   NiAllocatorDeclareStatics(T, size);
// where T is the template type T and size is the block size for the memory
// pool. To free this memory pool, use:
//   NiAllocatorShutdown(T);
// NiAllocatorShutdown assumes that any NiTObjectList<T> instances have been
// destroyed. Accessing a pre-existing NiTObjectList<T> after calling
// NiAllocatorShutdown(T) will cause a memory access violation. After calling
// NiAllocatorShutdown(T), new NiTObjectList<T> instances may be created, but
// of course they should be cleaned up with another call to
// NiAllocatorShutdown(T).
// 
// Remove and FindPos search for an element based on pointer equality. 
//
// Example of iteration from head to tail:
//
//   NiTObjectList<T> kList;
//   NiTListIterator kPos = kList.GetHeadPos();
//   while (kPos)
//   {
//       T* element = kList.GetNext(kPos);
//       <process element here>;
//   }

template <class T> class NiTObjectList :
    public NiTListBase<NiTObjectAllocator<T>, T>
{
public:
    virtual ~NiTObjectList();

    // Add/Insert functions return a pointer to a new object.
    // No constructor is called; data must be explicitly initialized.
    inline T* AddNewHead();
    inline T* AddNewTail();
    inline T* InsertNewBefore(NiTListIterator kPos, NiTListIterator& kNewIterator);
    inline T* InsertNewAfter(NiTListIterator kPos, NiTListIterator& kNewIterator);
    inline T* InsertNewBefore(NiTListIterator kPos);
    inline T* InsertNewAfter(NiTListIterator kPos);

    // Element access
    inline T* GetHead() const;
    inline T* GetTail() const;
    inline T* Get(NiTListIterator kPos) const;

    // Element access with iteration support
    inline T* GetNext(NiTListIterator& kPos) const;
    inline T* GetPrev(NiTListIterator& kPos) const;

    // Remove functions free the object but do not call the destructor.
    // If cleanup is required it must be done explicitly.
    inline void RemoveHead();
    inline void RemoveTail();
    inline void Remove(const T* element);
    inline void RemovePos(NiTListIterator& kPos);

    // Element search
    inline NiTListIterator FindPos(const T* element, 
        NiTListIterator kStart = 0) const;
    
protected:
    virtual NiTListItem<T>* NewItem();
    virtual void DeleteItem(NiTListItem<T>* pkItem);

};

//---------------------------------------------------------------------------
// Inline include
#include "NiTObjectList.inl"
//---------------------------------------------------------------------------

#endif // NITOBJECTLIST_H
