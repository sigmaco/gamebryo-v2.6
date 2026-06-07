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

#ifndef NIINTERFACESMARTPOINTER_H
#define NIINTERFACESMARTPOINTER_H

#include <NiMemObject.h>

// Smart pointers for interfaces. The class T must implement the following
// interface:
//
// void AddReference();
// void RemoveReference();

template<class T> class NiInterfacePointer : public NiMemObject
{
public:
    // Construction and destruction.
    NiInterfacePointer(T* pkInterface = (T*) 0);
    NiInterfacePointer(const NiInterfacePointer& kInterfacePtr);
    ~NiInterfacePointer();

    // Implicit conversions.
    operator T*() const;
    inline T& operator*() const;
    inline T* operator->() const;

    // Assignment.
    NiInterfacePointer& operator=(const NiInterfacePointer& kInterfacePtr);
    NiInterfacePointer& operator=(T* pkInterface);

    // Comparisons.
    inline bool operator==(T* pkInterface) const;
    inline bool operator!=(T* pkInterface) const;
    inline bool operator==(const NiInterfacePointer& kInterfacePtr) const;
    inline bool operator!=(const NiInterfacePointer& kInterfacePtr) const;

private:
    // The managed pointer.
    T* m_pkInterface;
};

#define NiInterfaceSmartPointer(classname) \
    class classname; \
    typedef NiInterfacePointer<classname> classname##IPtr

// Use for casting a smart pointer of one type to a pointer or smart pointer
// of another type.
#define NiInterfaceSmartPointerCast(type, smartptr) \
    ((type*) (void*) (smartptr))

#include "NiInterfaceSmartPointer.inl"

#endif // NIINTERFACESMARTPOINTER_H
