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

#ifndef NITPTRSET_H
#define NITPTRSET_H

#include <NiTSet.h>

// This template class implements an unordered set. It is safe for smart
// pointer use.
template <class T, class TAlloc> class NiTPtrSet : public NiTSet<T, TAlloc>
{
public:
    inline NiTPtrSet(unsigned int uiInitialSize = 1);
    inline ~NiTPtrSet();

    inline void RemoveAt(unsigned int uiIndex);
    inline void OrderedRemoveAt(unsigned int uiIndex); // Preserve order.
    inline void RemoveAll();

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    NiTPtrSet(const NiTPtrSet&);
    NiTPtrSet& operator=(const NiTPtrSet&);
};

template <class T> class NiTObjectPtrSet: 
    public NiTPtrSet<T, NiTNewInterface<T> >
{
public:
    inline NiTObjectPtrSet(unsigned int uiInitialSize = 0);
};

template <class T> class NiTPrimitivePtrSet: 
    public NiTPtrSet<T, NiTMallocInterface<T> >
{
public:
    inline NiTPrimitivePtrSet(unsigned int uiInitialSize = 0);
};

//---------------------------------------------------------------------------
template <class T> inline
NiTObjectPtrSet<T>::NiTObjectPtrSet(unsigned int uiInitialSize) :
    NiTPtrSet<T, NiTNewInterface<T> >(uiInitialSize)
{
}
//---------------------------------------------------------------------------
template <class T> inline
NiTPrimitivePtrSet<T>::NiTPrimitivePtrSet(unsigned int uiInitialSize):
    NiTPtrSet<T, NiTMallocInterface<T> >(uiInitialSize)
{
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTPtrSet<T, TAlloc>::NiTPtrSet(unsigned int uiInitialSize) :
    NiTSet<T, TAlloc>(uiInitialSize)
{
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
NiTPtrSet<T, TAlloc>::~NiTPtrSet()
{
    RemoveAll();
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPtrSet<T, TAlloc>::RemoveAt(unsigned int uiIndex)
{
    NIASSERT((uiIndex < NiTSet<T, TAlloc>::m_uiUsed));

    NiTSet<T, TAlloc>::m_pBase[uiIndex] = 
        NiTSet<T, TAlloc>::m_pBase[--NiTSet<T, TAlloc>::m_uiUsed];
    NiTSet<T, TAlloc>::m_pBase[NiTSet<T, TAlloc>::m_uiUsed] = 0;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPtrSet<T, TAlloc>::OrderedRemoveAt(unsigned int uiIndex)
{
    NIASSERT((uiIndex < NiTSet<T, TAlloc>::m_uiUsed));

    for (unsigned int ui = uiIndex; ui < NiTSet<T, TAlloc>::m_uiUsed - 1; ui++)
    {
        NiTSet<T, TAlloc>::m_pBase[ui] = NiTSet<T, TAlloc>::m_pBase[ui + 1];
    }
    NiTSet<T, TAlloc>::m_uiUsed--;
    NiTSet<T, TAlloc>::m_pBase[NiTSet<T, TAlloc>::m_uiUsed] = 0;
}
//---------------------------------------------------------------------------
template <class T, class TAlloc> inline
void NiTPtrSet<T, TAlloc>::RemoveAll()
{
    for (unsigned int ui = 0; ui < NiTSet<T, TAlloc>::m_uiUsed; ui++)
    {
        NiTSet<T, TAlloc>::m_pBase[ui] = 0;
    }
    NiTSet<T, TAlloc>::m_uiUsed = 0;
}
//---------------------------------------------------------------------------

#endif // #ifndef NITPTRSET_H
