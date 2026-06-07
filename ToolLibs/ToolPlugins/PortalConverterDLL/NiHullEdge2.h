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

#ifndef NIHULLEDGE2_H
#define NIHULLEDGE2_H

#include <NiPoint2.h>
#include <NiMemObject.h>


class NiHullEdge2 : public NiMemObject
{
public:
    NiHullEdge2(int iV0, int iV1);

    int GetSign(int i, const NiPoint2* akVertex);

    void Insert(NiHullEdge2* pkAdj0, NiHullEdge2* pkAdj1);
    void DeleteSelf();
    void DeleteAll();

    void GetIndices(int& iHQuantity, int*& aiHIndex);

    int V[2];
    NiHullEdge2* A[2];
    int Sign;
    int Time;
};

#endif
