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

#include "StdAfx.h"
#include "NiHullEdge2.h"

//----------------------------------------------------------------------------
NiHullEdge2::NiHullEdge2(int iV0, int iV1)
{
    V[0] = iV0;
    V[1] = iV1;
    A[0] = 0;
    A[1] = 0;
    Sign = 0;
    Time = -1;
}
//----------------------------------------------------------------------------
int NiHullEdge2::GetSign(int i, const NiPoint2* akVertex)
{
    if (i != Time)
    {
        Time = i;

        __int64 iV0x = (__int64)(akVertex[V[0]].x);
        __int64 iV0y = (__int64)(akVertex[V[0]].y);
        __int64 iV1x = (__int64)(akVertex[V[1]].x);
        __int64 iV1y = (__int64)(akVertex[V[1]].y);
        __int64 iV2x = (__int64)(akVertex[i].x);
        __int64 iV2y = (__int64)(akVertex[i].y);

        __int64 iD10x = iV1x - iV0x;
        __int64 iD10y = iV1y - iV0y;
        __int64 iD20x = iV2x - iV0x;
        __int64 iD20y = iV2y - iV0y;

        __int64 iDet = iD20x * iD10y - iD20y * iD10x;

        if ( iDet > 0 )
            Sign = 1;
        else if ( iDet < 0 )
            Sign = -1;
        else
            Sign = 0;
    }

    return Sign;
}
//----------------------------------------------------------------------------
void NiHullEdge2::Insert(NiHullEdge2* pkAdj0, NiHullEdge2* pkAdj1)
{
    pkAdj0->A[1] = this;
    pkAdj1->A[0] = this;
    A[0] = pkAdj0;
    A[1] = pkAdj1;
}
//----------------------------------------------------------------------------
void NiHullEdge2::DeleteSelf()
{
    if (A[0])
        A[0]->A[1] = 0;

    if (A[1])
        A[1]->A[0] = 0;

    NiDelete this;
}
//----------------------------------------------------------------------------
void NiHullEdge2::DeleteAll()
{
    NiHullEdge2* pkAdj = A[1];
    while (pkAdj && pkAdj != this)
    {
        NiHullEdge2* pkSave = pkAdj->A[1];
        NiDelete pkAdj;
        pkAdj = pkSave;
    }

    NIASSERT(pkAdj == this);
    NiDelete this;
}
//----------------------------------------------------------------------------
void NiHullEdge2::GetIndices(int& iHQuantity, int*& aiHIndex)
{
    // Count the number of edge vertices and allocate the index array.
    iHQuantity = 0;
    NiHullEdge2* pkCurrent = this;
    do
    {
        iHQuantity++;
        pkCurrent = pkCurrent->A[1];
    }
    while (pkCurrent != this);
    aiHIndex = NiAlloc(int, iHQuantity);

    // Fill the index array.
    iHQuantity = 0;
    pkCurrent = this;
    do
    {
        aiHIndex[iHQuantity++] = pkCurrent->V[0];
        pkCurrent = pkCurrent->A[1];
    }
    while (pkCurrent != this);
}
//----------------------------------------------------------------------------
