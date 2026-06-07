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

#ifndef NIHULLTRIANGLE3_H
#define NIHULLTRIANGLE3_H

#include <NiPoint3.h>
#include <NiMemObject.h>


class NiHullTriangle3 : public NiMemObject
{
public:
    NiHullTriangle3(int iV0, int iV1, int iV2);

    int GetSign(int i, const NiPoint3* akVertex);

    void AttachTo(NiHullTriangle3* pkAdj0, NiHullTriangle3* pkAdj1,
        NiHullTriangle3* pkAdj2);

    int DetachFrom(int iAdj, NiHullTriangle3* pkAdj);

    NiHullTriangle3* Remove();

    int V[3];
    NiHullTriangle3* A[3];
    int Sign;
    int Time;
    bool OnStack;
};

#endif
