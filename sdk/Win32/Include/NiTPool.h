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

#ifndef NITPOOL_H
#define NITPOOL_H

// This template class implements an unordered set. Because it does not
// set removed elements to 0, it should not be used for smart pointers.
#include "NiRTLib.h"
#include <NiTSet.h>
#include "NiTCollection.h"

template <class T, class TAlloc> class NiTPoolContainer  : public NiMemObject
{
public:
    NiTPoolContainer(unsigned int uiSize);
    ~NiTPoolContainer();

    inline T* GetObject(unsigned int uiIndex);

    inline void SetNext(NiTPoolContainer<T, TAlloc>* pkNext);
    
protected:
    T* m_pkObjectArray;
    unsigned int m_uiSize;

    NiTPoolContainer<T,TAlloc>* m_pkNext;

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTPoolContainer(const NiTPoolContainer&);
    NiTPoolContainer& operator=(const NiTPoolContainer&);
};

template <class T, class TAlloc> class NiTPool  : public NiMemObject
{
public:
    NiTPool(unsigned int uiInitialSize = 8);
    ~NiTPool();

    inline T* GetFreeObject();
    inline void ReleaseObject(T* pkObject);
    
    inline void PurgeAllObjects();

protected: 
    inline void CreateNewObjects(unsigned int uiSize);

    NiTPrimitiveSet<T*> m_kFreeObjects;
    NiTPoolContainer<T, TAlloc>* m_pkContainers;

    unsigned int m_uiCurrentSize;
    unsigned int m_uiInitialSize;
    
private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTPool(const NiTPool&);
    NiTPool& operator=(const NiTPool&);
};

template <class T> class NiTObjectPool : public 
    NiTPool<T, NiTNewInterface<T> >
{
public:
    NiTObjectPool(unsigned int uiInitialSize = 8);
};

template <class T> class NiTPrimitivePool : public 
    NiTPool<T, NiTMallocInterface<T> >
{
public:
    NiTPrimitivePool(unsigned int uiInitialSize = 8);
};

#include "NiTPool.inl"

#endif // NITPOOL_H
