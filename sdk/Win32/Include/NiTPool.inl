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

//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTPoolContainer<T,TAlloc>::NiTPoolContainer(unsigned int uiSize) :
    m_uiSize(uiSize),
    m_pkNext(NULL)
{
    if (uiSize > 0)
    {
        m_pkObjectArray = TAlloc::Allocate(uiSize);
        NIASSERT(m_pkObjectArray != NULL);
    }
    else
    {
        m_pkObjectArray = NULL;
    }
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTPoolContainer<T,TAlloc>::~NiTPoolContainer()
{
    TAlloc::Deallocate(m_pkObjectArray);
    NiDelete m_pkNext;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T* NiTPoolContainer<T,TAlloc>::GetObject(unsigned int uiIndex)
{
    if (uiIndex >= m_uiSize)
        return NULL;
    return &m_pkObjectArray[uiIndex];
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPoolContainer<T,TAlloc>::SetNext(NiTPoolContainer<T,TAlloc>* pkNext)
{
    m_pkNext = pkNext;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTPool<T,TAlloc>::NiTPool(unsigned int uiInitialSize) :
    m_pkContainers(NULL),
    m_uiCurrentSize(uiInitialSize),
    m_uiInitialSize(uiInitialSize)
{
    NIASSERT(uiInitialSize > 0);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTPool<T,TAlloc>::~NiTPool()
{
    m_kFreeObjects.RemoveAll();
    NiDelete m_pkContainers;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
T* NiTPool<T,TAlloc>::GetFreeObject()
{
    if (m_kFreeObjects.GetSize() == 0)
    {
        CreateNewObjects(m_uiCurrentSize);
        m_uiCurrentSize *= 2;
    }

    T* pkReturn = m_kFreeObjects.GetAt(0);
    m_kFreeObjects.RemoveAt(0);
    return pkReturn;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPool<T,TAlloc>::ReleaseObject(T* pkObject)
{
    // You hit this assert if you try to release the same object twice.
    NIASSERT(m_kFreeObjects.Find(pkObject) == -1);
    m_kFreeObjects.Add(pkObject);
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPool<T,TAlloc>::CreateNewObjects(unsigned int uiSize)
{
    NiTPoolContainer<T,TAlloc>* pkNewContainer = NiNew NiTPoolContainer<T,
        TAlloc>(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kFreeObjects.Add(pkNewContainer->GetObject(i));
    }

    pkNewContainer->SetNext(m_pkContainers);
    m_pkContainers = pkNewContainer;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPool<T,TAlloc>::PurgeAllObjects()
{
    m_kFreeObjects.RemoveAll();
    NiDelete m_pkContainers;
    m_pkContainers = NULL;
    m_uiCurrentSize = m_uiInitialSize;
}
//---------------------------------------------------------------------------
template <class T> inline
NiTObjectPool<T>::NiTObjectPool(unsigned int uiInitialSize) : 
    NiTPool<T, NiTNewInterface<T> >(uiInitialSize)
{  
}
//---------------------------------------------------------------------------
template <class T> inline
NiTPrimitivePool<T>::NiTPrimitivePool(unsigned int uiInitialSize) :
    NiTPool<T, NiTMallocInterface<T> >(uiInitialSize)
{  
}
//---------------------------------------------------------------------------
