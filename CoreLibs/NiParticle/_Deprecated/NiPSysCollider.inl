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

//---------------------------------------------------------------------------
inline float NiPSysCollider::GetBounce() const
{
    return m_fBounce;
}
//---------------------------------------------------------------------------
inline bool NiPSysCollider::GetSpawnOnCollide() const
{
    return m_bSpawnOnCollide;
}
//---------------------------------------------------------------------------
inline bool NiPSysCollider::GetDieOnCollide() const
{
    return m_bDieOnCollide;
}
//---------------------------------------------------------------------------
inline NiPSysSpawnModifier* NiPSysCollider::GetSpawnModifier() const
{
    return m_pkSpawnModifier;
}
//---------------------------------------------------------------------------
inline NiPSysCollider* NiPSysCollider::GetNext() const
{
    return m_spNext;
}
//---------------------------------------------------------------------------
