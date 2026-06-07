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

#include "NiSphere.h"

//---------------------------------------------------------------------------
bool NiSphere::operator==(const NiSphere& kSphere) const
{
    return m_kCenter == kSphere.m_kCenter && m_fRadius == kSphere.m_fRadius;
}
//---------------------------------------------------------------------------
bool NiSphere::operator!=(const NiSphere& kSphere) const
{
    return !operator==(kSphere);
}
//---------------------------------------------------------------------------
void NiSphere::LoadBinary(NiStream& kStream)
{
    m_kCenter.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
void NiSphere::SaveBinary(NiStream& kStream)
{
    m_kCenter.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
