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

#ifndef NITQUEUE_H
#define NITQUEUE_H

// This template class implements a FIFO queue. 
#include <NiMemObject.h>
#include "NiTCollection.h"

template <class T, class TAlloc> class NiTQueue  : public NiMemObject
{
public:
    NiTQueue(unsigned int uiInitialSize = 1);
    ~NiTQueue();

    inline void Add(T element);
    inline void AddUnique(T element); // Slow. Use with caution.
    inline T Remove();
    inline T Head() const;
    inline bool IsEmpty() const;
    inline unsigned int GetSize() const;
    inline void Realloc();

protected:
    inline bool IsFull() const;
    inline void Realloc(unsigned int uiNewSize);

    T *m_pBase;
    unsigned int m_uiAlloced, m_uiHead, m_uiTail;

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTQueue(const NiTQueue&);
    NiTQueue& operator=(const NiTQueue&);
};

template <class T> class NiTObjectQueue : public 
    NiTQueue<T, NiTNewInterface<T> >
{
public:
    NiTObjectQueue(unsigned int uiInitialSize = 1);
};

template <class T> class NiTPrimitiveQueue : public 
    NiTQueue<T, NiTMallocInterface<T> >
{
public:
    NiTPrimitiveQueue(unsigned int uiInitialSize = 1);
};

#include "NiTQueue.inl"

#endif // NITQUEUE_H
