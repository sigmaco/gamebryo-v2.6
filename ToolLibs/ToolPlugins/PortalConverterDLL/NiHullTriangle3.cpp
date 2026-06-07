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
#include "NiHullTriangle3.h"

//----------------------------------------------------------------------------
NiHullTriangle3::NiHullTriangle3(int iV0, int iV1, int iV2)
{
    V[0] = iV0;
    V[1] = iV1;
    V[2] = iV2;
    A[0] = 0;
    A[1] = 0;
    A[2] = 0;
    Sign = 0;
    Time = -1;
    OnStack = false;
}
//----------------------------------------------------------------------------
int NiHullTriangle3::GetSign(int i, const NiPoint3* akVertex)
{
    if (i != Time)
    {
        Time = i;

        __int64 iV0x = (__int64)akVertex[V[0]].x;
        __int64 iV0y = (__int64)akVertex[V[0]].y;
        __int64 iV0z = (__int64)akVertex[V[0]].z;
        __int64 iV1x = (__int64)akVertex[V[1]].x;
        __int64 iV1y = (__int64)akVertex[V[1]].y;
        __int64 iV1z = (__int64)akVertex[V[1]].z;
        __int64 iV2x = (__int64)akVertex[V[2]].x;
        __int64 iV2y = (__int64)akVertex[V[2]].y;
        __int64 iV2z = (__int64)akVertex[V[2]].z;
        __int64 iV3x = (__int64)akVertex[i].x;
        __int64 iV3y = (__int64)akVertex[i].y;
        __int64 iV3z = (__int64)akVertex[i].z;


        __int64 iD10x = iV1x - iV0x;
        __int64 iD10y = iV1y - iV0y;
        __int64 iD10z = iV1z - iV0z;
        __int64 iD20x = iV2x - iV0x;
        __int64 iD20y = iV2y - iV0y;
        __int64 iD20z = iV2z - iV0z;
        __int64 iD30x = iV3x - iV0x;
        __int64 iD30y = iV3y - iV0y;
        __int64 iD30z = iV3z - iV0z;

        __int64 iDet =
            iD10x * (iD20y * iD30z - iD20z * iD30y) +
            iD20x * (iD30y * iD10z - iD30z * iD10y) +
            iD30x * (iD10y * iD20z - iD10z * iD20y);

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
void NiHullTriangle3::AttachTo(NiHullTriangle3* pkAdj0,
    NiHullTriangle3* pkAdj1, NiHullTriangle3* pkAdj2)
{
    // assert:  The input adjacent triangles are correctly ordered.
    A[0] = pkAdj0;
    A[1] = pkAdj1;
    A[2] = pkAdj2;
}
//----------------------------------------------------------------------------
int NiHullTriangle3::DetachFrom(int iAdj, NiHullTriangle3* pkAdj)
{
    NIASSERT(0 <= iAdj && iAdj < 3 && A[iAdj] == pkAdj);

    A[iAdj] = 0;
    for (int i = 0; i < 3; i++)
    {
        if (pkAdj->A[i] == this)
        {
            pkAdj->A[i] = 0;
            return i;
        }
    }
    return -1;
}
//----------------------------------------------------------------------------
