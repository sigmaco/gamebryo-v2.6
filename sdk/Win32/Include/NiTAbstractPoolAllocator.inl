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
//  NiTAbstractPoolAllocator inline functions

//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::EnsureFreeMem()
{
    if (!ms_pkFreeMem)
        CreateFreeMem();  
}
//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::CreateFreeMem()
{
    NIASSERT(ms_pkFreeMem == 0);
    NIASSERT(ms_uiBlockSize >= 2);

    ms_pkFreeMem = NiNew typename NiTAbstractPoolAllocator<T>::AllocNode[
        ms_uiBlockSize];      

    // Handle end point
    typename NiTAbstractPoolAllocator<T>::AllocNode *pkCurr = 
        ms_pkFreeMem + ms_uiBlockSize - 1;
    pkCurr->m_pkNext = 0;

    unsigned int i = 1;

    do
    {
        pkCurr = ms_pkFreeMem + i;
        pkCurr->m_pkNext = pkCurr + 1;
    } while(i++ < ms_uiBlockSize - 2);

    if (ms_pkBlockHeader)
    {
        // add block to head of list
        ms_pkFreeMem->m_pkNext = ms_pkBlockHeader;
        ms_pkBlockHeader = ms_pkFreeMem;
    }
    else
    {
        ms_pkBlockHeader = ms_pkFreeMem;
        ms_pkBlockHeader->m_pkNext = 0;
    }

    ms_pkFreeMem = ms_pkFreeMem + 1;
}
//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::_SDMShutdown()
{
    typename NiTAbstractPoolAllocator<T>::AllocNode* pkCurr = ms_pkBlockHeader;

    while (pkCurr)
    {
        typename NiTAbstractPoolAllocator<T>::AllocNode* pkNext = 
            pkCurr->m_pkNext;
        NiDelete [] pkCurr;
        pkCurr = pkNext;
    }

    ms_pkBlockHeader = NULL;
    ms_pkFreeMem = NULL;
}
//---------------------------------------------------------------------------
template <class T> inline
void NiTAbstractPoolAllocator<T>::SetBlockSize(unsigned int uiBlockSize)
{
    NIASSERT(uiBlockSize >= 2);
    ms_uiBlockSize = uiBlockSize; 
}
//---------------------------------------------------------------------------
