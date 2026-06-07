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

#ifndef TERMINATORDATA_H
#define TERMINATORDATA_H

class NiHullTriangle;
#include <NiMemObject.h>

class TerminatorData : public NiMemObject
{
public:
    TerminatorData(int iV0 = -1, int iV1 = -1, int iNullIndex = -1,
        NiHullTriangle3* pkTri = 0)
    {
        V[0] = iV0;
        V[1] = iV1;
        NullIndex = iNullIndex;
        Tri = pkTri;
    }

    int V[2];
    int NullIndex;
    NiHullTriangle3* Tri;
};

#endif
