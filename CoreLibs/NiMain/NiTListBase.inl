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
//  NiTListBase inline functions

//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListBase<TheAllocator,T>::NiTListBase()
{
    m_kAllocator.m_uiCount = 0;
    m_pkHead = 0;
    m_pkTail = 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListBase<TheAllocator,T>::~NiTListBase()
{
    // RemoveAll() must be called from derived classes as it relies on
    // the pure virtual DeleteItem(). If RemoveAll is called here without
    // being called in the derived class, R6025 runtime error will be
    // generated (pure virtual function called).
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
unsigned int NiTListBase<TheAllocator,T>::GetSize() const
{
    return m_kAllocator.m_uiCount;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
bool NiTListBase<TheAllocator,T>::IsEmpty() const
{
    return m_kAllocator.m_uiCount == 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
void NiTListBase<TheAllocator,T>::RemoveAll()
{
    NiTListItem<T>* pkCurr = m_pkHead;
    while (pkCurr)
    {
        NiTListItem<T>* pkDel = pkCurr;
        pkCurr = pkCurr->m_pkNext;
        DeleteItem(pkDel);
    }

    m_kAllocator.m_uiCount = 0;
    m_pkHead = 0;
    m_pkTail = 0;
} 
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListIterator NiTListBase<TheAllocator,T>::GetHeadPos() const
{
    return m_pkHead;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListIterator NiTListBase<TheAllocator,T>::GetTailPos() const
{
    return m_pkTail;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListIterator NiTListBase<TheAllocator,T>::
    GetNextPos(NiTListIterator kPos) const
{
    return kPos ? ((NiTListItem<T>*) kPos)->m_pkNext : 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListIterator NiTListBase<TheAllocator,T>::
    GetPrevPos(NiTListIterator kPos) const
{
    return kPos ? ((NiTListItem<T>*) kPos)->m_pkPrev : 0;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
void NiTListBase<TheAllocator,T>::AddNodeHead(NiTListItem<T>* pkNode)
{
    pkNode->m_pkPrev = 0;
    pkNode->m_pkNext = m_pkHead;
    
    if (m_pkHead)
        m_pkHead->m_pkPrev = pkNode;
    else
        m_pkTail = pkNode;
    
    m_pkHead = pkNode;
    m_kAllocator.m_uiCount++;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
void NiTListBase<TheAllocator,T>::AddNodeTail(NiTListItem<T>* pkNode)
{
    pkNode->m_pkNext = 0;
    pkNode->m_pkPrev = m_pkTail;
    
    if (m_pkTail)
        m_pkTail->m_pkNext = pkNode;
    else
        m_pkHead = pkNode;
    
    m_pkTail = pkNode;
    m_kAllocator.m_uiCount++;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListIterator NiTListBase<TheAllocator,T>::
    AddNodeBefore(NiTListIterator kPos, NiTListItem<T>* pkNode)
{
    NiTListItem<T>* pkNext = (NiTListItem<T>*) kPos;

    pkNode->m_pkNext = pkNext;
    pkNode->m_pkPrev = pkNext->m_pkPrev;
    
    if (pkNext->m_pkPrev)
        pkNext->m_pkPrev->m_pkNext = pkNode;
    else
        m_pkHead = pkNode;

    pkNext->m_pkPrev = pkNode;

    m_kAllocator.m_uiCount++;
    return pkNode;
}
//---------------------------------------------------------------------------
template <class TheAllocator, class T> inline
NiTListIterator NiTListBase<TheAllocator,T>::
    AddNodeAfter(NiTListIterator kPos, NiTListItem<T>* pkNode)
{
    NiTListItem<T>* pkPrev = (NiTListItem<T>*) kPos;

    pkNode->m_pkPrev = pkPrev;
    pkNode->m_pkNext = pkPrev->m_pkNext;
    
    if (pkPrev->m_pkNext)
        pkPrev->m_pkNext->m_pkPrev = pkNode;
    else
        m_pkTail = pkNode;

    pkPrev->m_pkNext = pkNode;

    m_kAllocator.m_uiCount++;
    return pkNode;
}
//---------------------------------------------------------------------------

