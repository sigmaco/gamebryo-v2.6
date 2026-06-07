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

#include "NiTrigon.h"

//---------------------------------------------------------------------------
NiPoint3 NiTrigon::GetPoint(float fS0, float fS1) const
{
    return m_kOrigin + (fS0 * m_kEdge0) + (fS1 * m_kEdge1);
}
//---------------------------------------------------------------------------
NiPoint3 NiTrigon::GetNormal() const
{
    NiPoint3 kNormal = m_kEdge0.Cross(m_kEdge1);
    kNormal.Unitize();
    return kNormal;
}
//---------------------------------------------------------------------------
bool NiTrigon::operator==(const NiTrigon& kTri) const
{
    return m_kOrigin == kTri.m_kOrigin && m_kEdge0 == kTri.m_kEdge0
        && m_kEdge1 == kTri.m_kEdge1;
}
//---------------------------------------------------------------------------
bool NiTrigon::operator!=(const NiTrigon& kTri) const
{
    return !operator==(kTri);
}
//---------------------------------------------------------------------------
void NiTrigon::LoadBinary(NiStream& kStream)
{
    m_kOrigin.LoadBinary(kStream);
    m_kEdge0.LoadBinary(kStream);
    m_kEdge1.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiTrigon::SaveBinary(NiStream& kStream)
{
    m_kOrigin.SaveBinary(kStream);
    m_kEdge0.SaveBinary(kStream);
    m_kEdge1.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
