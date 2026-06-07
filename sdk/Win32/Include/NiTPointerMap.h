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

#ifndef NITPOINTERMAP_H
#define NITPOINTERMAP_H

#include "NiTMapBase.h"
#include "NiTPointerAllocator.h"

template <class TKEY, class TVAL,
    class THASH = NiTMapHashFunctor<TKEY>,
    class TEQUALS = NiTMapEqualsFunctor<TKEY> > class NiTPointerMap :
    public NiTMapBase<NiTPointerAllocator<size_t>, TKEY, TVAL, THASH, TEQUALS>
{
public:
     inline NiTPointerMap(unsigned int uiHashSize = 37) : NiTMapBase<
         NiTPointerAllocator<size_t>, TKEY, TVAL, THASH, TEQUALS>(uiHashSize)
         {}
    ~NiTPointerMap(); 
    virtual NiTMapItem<TKEY, TVAL>* NewItem();
    virtual void DeleteItem(NiTMapItem<TKEY, TVAL>* pkItem);
};

#include "NiTPointerMap.inl"

#endif
