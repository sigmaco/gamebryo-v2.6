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

#ifndef EDGEMAP_H
#define EDGEMAP_H

#include <NiTMapBase.h>
#include <NiPoint3.h>
#include "EdgeKey.h"

// custom allocator for the edge map
class EdgeAllocator
{
public:
    inline void* Allocate() 
    {
        return NiNew AllocNode;
    }
    inline void Deallocate (void* pvNode) 
    {
        NiDelete (AllocNode*)pvNode; 
    }

protected:
    class AllocNode : public NiMemObject
    {
        public:
            AllocNode* m_pkNext;
            int m_aiV[2];
            NiPoint3 m_kEdge;
    };
};

class EdgeMap : public NiTMapBase<EdgeAllocator, EdgeKey, NiPoint3>
{
public:
    EdgeMap(unsigned int uiHashSize = 37) :
        NiTMapBase<EdgeAllocator, EdgeKey, NiPoint3>(uiHashSize)
    {
        /**/
    }

    inline virtual ~EdgeMap()
    {
        RemoveAll();
    }

    inline virtual NiTMapItem<EdgeKey, NiPoint3>* NewItem()
    {
        return (NiTMapItem<EdgeKey, NiPoint3>*)m_kAllocator.Allocate();
    }

    inline virtual void DeleteItem(NiTMapItem<EdgeKey, NiPoint3>* pkItem)
    {
        m_kAllocator.Deallocate(pkItem);
    }
};

#endif
