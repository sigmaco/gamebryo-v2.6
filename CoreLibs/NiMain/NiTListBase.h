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

#ifndef NITLISTBASE_H
#define NITLISTBASE_H

#include "NiRTLib.h"
#include "NiTCollection.h"

typedef void* NiTListIterator;
//---------------------------------------------------------------------------
template <class T> class NiTListItem : public NiMemObject
{
public:
    NiTListItem* m_pkNext;
    NiTListItem* m_pkPrev;
    T m_element;
};
//---------------------------------------------------------------------------
template <class TheAllocator, class T> class NiTListBase : public NiMemObject
{
public:
    // counting elements in list
    inline unsigned int GetSize() const;
    inline bool IsEmpty() const;
   
    // clear list
    inline void RemoveAll();

    // iteration support
    inline NiTListIterator GetHeadPos() const;
    inline NiTListIterator GetTailPos() const;  
    
    inline NiTListIterator GetNextPos(NiTListIterator kPos) const;
    inline NiTListIterator GetPrevPos(NiTListIterator kPos) const;

protected:
    // construction and destruction
    NiTListBase();
    virtual ~NiTListBase();

    // add or remove elements
    virtual NiTListItem<T>* NewItem() = 0;
    virtual void DeleteItem(NiTListItem<T>* pkItem) = 0;

    inline void AddNodeHead(NiTListItem<T>* pkNode);
    inline void AddNodeTail(NiTListItem<T>* pkNode);

    inline NiTListIterator AddNodeAfter(NiTListIterator pos,
        NiTListItem<T>* pkNode);
    inline NiTListIterator AddNodeBefore(NiTListIterator pos,
        NiTListItem<T>* pkNode);

    // list is doubly-linked
    NiTListItem<T>* m_pkHead;              // points to head of list
    NiTListItem<T>* m_pkTail;              // points to tail of list

    struct AntiBloatAllocator : public TheAllocator
    {
        // We reduce TheAllocator by 4 bytes by deriving
        // See http://www.cantrip.org/emptyopt.html
        unsigned int m_uiCount;      // number of elements in list
    };

    AntiBloatAllocator m_kAllocator;  // points to the Allocator
private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTListBase(const NiTListBase&);
    NiTListBase& operator=(const NiTListBase&);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Inline include
#include "NiTListBase.inl"
//---------------------------------------------------------------------------

#endif // NITLISTBASE_H
