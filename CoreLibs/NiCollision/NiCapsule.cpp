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

#include "NiCapsule.h"

//---------------------------------------------------------------------------
bool NiCapsule::operator==(const NiCapsule& kCapsule) const
{
    return m_kSegment == kCapsule.m_kSegment
        && m_fRadius == kCapsule.m_fRadius;
}
//---------------------------------------------------------------------------
bool NiCapsule::operator!=(const NiCapsule& kCapsule) const
{
    return !operator==(kCapsule);
}
//---------------------------------------------------------------------------
void NiCapsule::LoadBinary(NiStream& kStream)
{
    m_kSegment.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
void NiCapsule::SaveBinary(NiStream& kStream)
{
    m_kSegment.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
