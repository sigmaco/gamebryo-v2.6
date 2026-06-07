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

// Precompiled Header
#include "NiCollisionPCH.h"

#include "NiSegment.h"

//---------------------------------------------------------------------------
NiPoint3 NiSegment::GetPoint(float fS) const
{
    return m_kOrigin + (fS * m_kDirection);
}
//---------------------------------------------------------------------------
bool NiSegment::operator==(const NiSegment& kSegment) const
{
    return m_kOrigin == kSegment.m_kOrigin 
        && m_kDirection == kSegment.m_kDirection;
}
//---------------------------------------------------------------------------
bool NiSegment::operator!=(const NiSegment& kSegment) const
{
    return !operator==(kSegment);
}
//---------------------------------------------------------------------------
void NiSegment::LoadBinary(NiStream& kStream)
{
    m_kOrigin.LoadBinary(kStream);
    m_kDirection.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiSegment::SaveBinary(NiStream& kStream)
{
    m_kOrigin.SaveBinary(kStream);
    m_kDirection.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
