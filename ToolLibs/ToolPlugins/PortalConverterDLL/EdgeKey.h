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

#ifndef EDGEKEY_H
#define EDGEKEY_H

#include <NiMemObject.h>

class EdgeKey : public NiMemObject
{
public:
    // Required functions for TKEY in the NiTMapBase<TKEY,TVALUE> template
    // class.
    EdgeKey (int iV0 = -1, int iV1 = -1);
    EdgeKey (const EdgeKey& kKey);
    EdgeKey& operator=(const EdgeKey& kKey);
    bool operator==(const EdgeKey& kKey) const;
    operator long();

    int m_aiV[2];
};

#endif
