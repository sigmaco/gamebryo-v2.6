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

#include "NiParallelogram.h"

//---------------------------------------------------------------------------
NiPoint3 NiParallelogram::GetPoint(float fS0, float fS1) const
{
    return m_kOrigin + fS0 * m_kEdge0 + fS1 * m_kEdge1;
}
//---------------------------------------------------------------------------
bool NiParallelogram::operator==(const NiParallelogram& kPgm) const
{
    return m_kOrigin == kPgm.m_kOrigin && m_kEdge0 == kPgm.m_kEdge0
        && m_kEdge1 == kPgm.m_kEdge1;
}
//---------------------------------------------------------------------------
bool NiParallelogram::operator!=(const NiParallelogram& kPgm) const
{
    return !operator==(kPgm);
}
//---------------------------------------------------------------------------
void NiParallelogram::LoadBinary(NiStream& kStream)
{
    m_kOrigin.LoadBinary(kStream);
    m_kEdge0.LoadBinary(kStream);
    m_kEdge1.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiParallelogram::SaveBinary(NiStream& kStream)
{
    m_kOrigin.SaveBinary(kStream);
    m_kEdge0.SaveBinary(kStream);
    m_kEdge1.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
