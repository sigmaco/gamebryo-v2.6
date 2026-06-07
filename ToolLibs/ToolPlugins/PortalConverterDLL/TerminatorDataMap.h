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

#ifndef TERMINATORDATAMAP_H
#define TERMINATORDATAMAP_H

#include <NiTMapBase.h>
#include <NiPoint3.h>
#include "TerminatorData.h"

// custom allocator for TerminatorDataMap
class TerminatorDataAllocator
{
public:
    inline void* Allocate() 
    {
        return NiNew AllocNode;
    }
    inline void Deallocate(void* pvNode) 
    {
        NiDelete (AllocNode*)pvNode; 
    }

protected:
    class AllocNode  : public NiMemObject
    {
        public:
            AllocNode* m_pkNext;
            int m_iVIndex;
            TerminatorData m_kData;
    };
};

class TerminatorDataMap :
    public NiTMapBase<TerminatorDataAllocator, int, TerminatorData>
{
public:
    TerminatorDataMap(unsigned int uiHashSize = 37) :
        NiTMapBase<TerminatorDataAllocator, int, TerminatorData>(uiHashSize)
    {
        /**/
    }

    inline virtual ~TerminatorDataMap()
    {
        RemoveAll();
    }

    inline virtual NiTMapItem<int, TerminatorData>* NewItem()
    {
        return (NiTMapItem<int, TerminatorData>*)m_kAllocator.Allocate();
    }

    inline virtual void DeleteItem(NiTMapItem<int, TerminatorData>* pkItem)
    {
        m_kAllocator.Deallocate(pkItem);
    }
};

#endif
