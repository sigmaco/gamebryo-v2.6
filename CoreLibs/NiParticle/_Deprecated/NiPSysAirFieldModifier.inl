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
inline const NiPoint3& NiPSysAirFieldModifier::GetDirection() const
{
    return m_kDirection;
}
//---------------------------------------------------------------------------
inline void NiPSysAirFieldModifier::SetDirection(const NiPoint3& kDirection)
{
    m_kDirection = kDirection;

    // Create the Unitized Direction
    m_kUnitDirection = kDirection;
    m_kUnitDirection.Unitize();

    // Check for a valid direction
    NIASSERT(m_kUnitDirection != NiPoint3::ZERO);
    if (m_kUnitDirection == NiPoint3::ZERO)
    {
        m_kUnitDirection = NiPoint3::UNIT_X;
    }
}
//---------------------------------------------------------------------------
inline float NiPSysAirFieldModifier::GetAirFriction() const
{
    return m_fAirFriction;
}
//---------------------------------------------------------------------------
inline float NiPSysAirFieldModifier::GetInheritVelocity() const
{
    return m_fInheritVelocity;
}
//---------------------------------------------------------------------------
inline bool NiPSysAirFieldModifier::GetInheritRotation() const
{
    return m_bInheritRotation;
}
//---------------------------------------------------------------------------
inline bool NiPSysAirFieldModifier::GetComponentOnly() const
{
    return m_bComponentOnly;
}
//---------------------------------------------------------------------------
inline bool NiPSysAirFieldModifier::GetEnableSpread() const
{
    return m_bEnableSpread;
}
//---------------------------------------------------------------------------
inline float NiPSysAirFieldModifier::GetSpread() const
{
    return m_fSpread;
}
//---------------------------------------------------------------------------
