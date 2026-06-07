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
//  NiTPointerAllocator inline functions

//---------------------------------------------------------------------------
template <class T> inline
void* NiTPointerAllocator<T>::Allocate()
{
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Lock();
#ifdef NIDEBUG
    NiTAbstractPoolAllocator<T>::ms_uiAllocated++;
    if (NiTAbstractPoolAllocator<T>::ms_uiAllocated > 
        NiTAbstractPoolAllocator<T>::ms_uiMaxAllocated)
    {
        NiTAbstractPoolAllocator<T>::ms_uiMaxAllocated = 
            NiTAbstractPoolAllocator<T>::ms_uiAllocated;
    }
#endif

    if (!NiTAbstractPoolAllocator<T>::ms_pkFreeMem)
        NiTPointerAllocator<T>::CreateFreeMem();

    typename NiTAbstractPoolAllocator<T>::AllocNode* pTmp = 
        NiTAbstractPoolAllocator<T>::ms_pkFreeMem;
    NiTAbstractPoolAllocator<T>::ms_pkFreeMem = 
        NiTAbstractPoolAllocator<T>::ms_pkFreeMem->m_pkNext;
    pTmp->m_element = 0;
    pTmp->m_pkNext = 0;
    pTmp->m_pkData = 0;
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Unlock();
    return pTmp;
} 
//---------------------------------------------------------------------------
template <class T> inline
void NiTPointerAllocator<T>::Deallocate(void* pkDel) 
{
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Lock();
#ifdef NIDEBUG
    NiTAbstractPoolAllocator<T>::ms_uiAllocated--;
#endif

    // Node being freed - Just set the freepointer
    // here and the next to the previous free
    // In debug, memset(0).
    typename NiTAbstractPoolAllocator<T>::AllocNode* pDel = 
        (typename NiTAbstractPoolAllocator<T>::AllocNode*)pkDel;

    pDel->m_pkData = 0;
    pDel->m_pkNext = NiTAbstractPoolAllocator<T>::ms_pkFreeMem;
    NiTAbstractPoolAllocator<T>::ms_pkFreeMem = pDel;
    NiTAbstractPoolAllocator<T>::ms_kCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
