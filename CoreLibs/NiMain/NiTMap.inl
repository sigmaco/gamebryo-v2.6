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
template <class TKEY, class TVAL, class THASH, class TEQUALS> inline
NiTMap<TKEY, TVAL, THASH, TEQUALS>::~NiTMap()
{
    // RemoveAll is called from here because it depends on virtual functions
    // implemented in NiTAllocatorMap.  It will also be called in the 
    // parent destructor, but the map will already be empty.
    NiTMap<TKEY, TVAL, THASH, TEQUALS>::RemoveAll();
}
//---------------------------------------------------------------------------
template <class TKEY, class TVAL, class THASH, class TEQUALS> inline
NiTMapItem<TKEY, TVAL>* NiTMap<TKEY, TVAL, THASH, TEQUALS>::NewItem()
{
    return (NiTMapItem<TKEY, TVAL>*)NiTMapBase<NiTDefaultAllocator<TVAL>, 
        TKEY, TVAL, THASH, TEQUALS >::m_kAllocator.Allocate();
}
//---------------------------------------------------------------------------
template <class TKEY, class TVAL, class THASH, class TEQUALS> inline
void NiTMap<TKEY, TVAL, THASH, TEQUALS>::
    DeleteItem(NiTMapItem<TKEY, TVAL>* pkItem)
{
    // set key and val to zero so that if they are smart pointers
    // their references will be decremented.
    pkItem->m_val = 0;
    NiTMapBase<NiTDefaultAllocator<TVAL>, TKEY, 
        TVAL, THASH, TEQUALS >::m_kAllocator.Deallocate(pkItem);
}
//---------------------------------------------------------------------------
template <class TKEY, class TVAL, class THASH, class TEQUALS>
const NiUInt32 NiTMap<TKEY, TVAL, THASH, TEQUALS>::NUM_PRIMES = 18;
//---------------------------------------------------------------------------
template <class TKEY, class TVAL, class THASH, class TEQUALS>
const NiUInt32 NiTMap<TKEY, TVAL, THASH, TEQUALS>::PRIMES[18] = {
    7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 999983};
//---------------------------------------------------------------------------
template <class TKEY, class TVAL, class THASH, class TEQUALS>
inline NiUInt32 NiTMap<TKEY, TVAL, THASH, TEQUALS>::
    NextPrime(const NiUInt32 uiTarget)
{
    NiUInt32 ui;
    for (ui = 0; ui < NUM_PRIMES - 1 && PRIMES[ui] < uiTarget; ui++) { }
    return PRIMES[ui];
}
//---------------------------------------------------------------------------
