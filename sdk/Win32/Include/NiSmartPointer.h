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

#ifndef NISMARTPOINTER_H
#define NISMARTPOINTER_H

#include <NiMemObject.h>
// Smart pointers. The class T must be derived from NiRefObject.

template <class T> class NiPointer : public NiMemObject
{
public:
    // construction and destruction
    inline NiPointer(T* pObject = (T*) 0);
    inline NiPointer(const NiPointer& ptr);
    inline ~NiPointer();

    // implicit conversions
    inline operator T*() const;
    inline T& operator*() const;
    inline T* operator->() const;

    // assignment
    inline NiPointer& operator=(const NiPointer& ptr);
    inline NiPointer& operator=(T* pObject);

    // comparisons
    inline bool operator==(T* pObject) const;
    inline bool operator!=(T* pObject) const;
    inline bool operator==(const NiPointer& ptr) const;
    inline bool operator!=(const NiPointer& ptr) const;

protected:
    // the managed pointer
    T* m_pObject;
};

#define NiSmartPointer(classname) \
    class classname; \
    typedef NiPointer<classname> classname##Ptr

// Use for casting a smart pointer of one type to a pointer or smart pointer
// of another type.
#define NiSmartPointerCast(type, smartptr) ((type*) (void*) (smartptr))

//---------------------------------------------------------------------------
template <class T>
inline NiPointer<T>::NiPointer(T* pObject)
{
    m_pObject = pObject;
    if (m_pObject)
        m_pObject->IncRefCount();
}
//---------------------------------------------------------------------------
template <class T>
inline NiPointer<T>::NiPointer(const NiPointer& ptr)
{
    m_pObject = ptr.m_pObject;
    if (m_pObject)
        m_pObject->IncRefCount();
}
//---------------------------------------------------------------------------
template <class T>
inline NiPointer<T>::~NiPointer()
{
    if (m_pObject)
        m_pObject->DecRefCount();
}
//---------------------------------------------------------------------------
template <class T>
inline NiPointer<T>::operator T*() const
{
    return m_pObject;
}
//---------------------------------------------------------------------------
template <class T>
inline T& NiPointer<T>::operator*() const
{
    return *m_pObject;
}
//---------------------------------------------------------------------------
template <class T>
inline T* NiPointer<T>::operator->() const
{
    return m_pObject;
}
//---------------------------------------------------------------------------
template <class T>
inline NiPointer<T>& NiPointer<T>::operator=(const NiPointer& ptr)
{
    if (m_pObject != ptr.m_pObject)
    {
        if (m_pObject)
            m_pObject->DecRefCount();
        m_pObject = ptr.m_pObject;
        if (m_pObject)
            m_pObject->IncRefCount();
    }
    return *this;
}
//---------------------------------------------------------------------------
template <class T>
inline NiPointer<T>& NiPointer<T>::operator=(T* pObject)
{
    if (m_pObject != pObject)
    {
        if (m_pObject)
            m_pObject->DecRefCount();
        m_pObject = pObject;
        if (m_pObject)
            m_pObject->IncRefCount();
    }
    return *this;
}
//---------------------------------------------------------------------------
template <class T>
inline bool NiPointer<T>::operator==(T* pObject) const
{
    return (m_pObject == pObject);
}
//---------------------------------------------------------------------------
template <class T>
inline bool NiPointer<T>::operator!=(T* pObject) const
{
    return (m_pObject != pObject);
}
//---------------------------------------------------------------------------
template <class T>
inline bool NiPointer<T>::operator==(const NiPointer& ptr) const
{
    return (m_pObject == ptr.m_pObject);
}
//---------------------------------------------------------------------------
template <class T>
inline bool NiPointer<T>::operator!=(const NiPointer& ptr) const
{
    return (m_pObject != ptr.m_pObject);
}
//---------------------------------------------------------------------------

#endif // NISMARTPOINTER_H
