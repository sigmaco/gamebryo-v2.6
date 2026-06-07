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
#include "EdgeKey.h"

//---------------------------------------------------------------------------
EdgeKey::EdgeKey (int iV0, int iV1)
{
    // Store the integer pair as <vmin,vmax>.
    if (iV0 < iV1)
    {
        m_aiV[0] = iV0;
        m_aiV[1] = iV1;
    }
    else
    {
        m_aiV[0] = iV1;
        m_aiV[1] = iV0;
    }
}
//---------------------------------------------------------------------------
EdgeKey::EdgeKey (const EdgeKey& kKey)
{
    m_aiV[0] = kKey.m_aiV[0];
    m_aiV[1] = kKey.m_aiV[1];
}
//---------------------------------------------------------------------------
EdgeKey& EdgeKey::operator=(const EdgeKey& kKey)
{
    m_aiV[0] = kKey.m_aiV[0];
    m_aiV[1] = kKey.m_aiV[1];
    return *this;
}
//---------------------------------------------------------------------------
bool EdgeKey::operator==(const EdgeKey& kKey) const
{
    return (m_aiV[0] == kKey.m_aiV[0] && m_aiV[1] == kKey.m_aiV[1]);
}
//---------------------------------------------------------------------------
EdgeKey::operator long()
{
    // The probability that the number of vertices in a mesh is larger than
    // 2^{16} is quite small.  The hash key generation should produce unique
    // values.
    return (m_aiV[1] << 16) & m_aiV[0];
}
//---------------------------------------------------------------------------
