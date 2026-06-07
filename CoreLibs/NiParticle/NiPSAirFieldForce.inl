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
inline const NiPoint3& NiPSAirFieldForce::GetLastFieldPosition() const
{
    return m_kLastFieldPosition;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetLastFieldPosition(
    const NiPoint3& kLastFieldPosition)
{
    m_kLastFieldPosition = kLastFieldPosition;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::ClearLastFieldPosition()
{
    m_kLastFieldPosition = ms_kUninitializedPosition;
    m_fLastUpdateTime = -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiPSAirFieldForce::GetDirection() const
{
    return m_kInputData.m_kDirection;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetDirection(const NiPoint3& kDirection)
{
    m_kInputData.m_kDirection = kDirection;

    // Create the unitized direction.
    m_kInputData.m_kUnitDirection = kDirection;
    m_kInputData.m_kUnitDirection.Unitize();

    // Check for a valid direction.
    NIASSERT(m_kInputData.m_kUnitDirection != NiPoint3::ZERO);
    if (m_kInputData.m_kUnitDirection == NiPoint3::ZERO)
    {
        m_kInputData.m_kUnitDirection = NiPoint3::UNIT_X;
    }
}
//---------------------------------------------------------------------------
inline float NiPSAirFieldForce::GetAirFriction() const
{
    return m_kInputData.m_fAirFriction;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetAirFriction(float fAirFriction)
{
    NIASSERT(fAirFriction >= 0.0f && fAirFriction <= 1.0f);

    // Verify we are in the range 0.0 to 1.0.
    fAirFriction = NiMax(0.0f, fAirFriction);
    fAirFriction = NiMin(1.0f, fAirFriction);

    m_kInputData.m_fAirFriction = fAirFriction;
}
//---------------------------------------------------------------------------
inline float NiPSAirFieldForce::GetInheritedVelocity() const
{
    return m_kInputData.m_fInheritedVelocity;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetInheritedVelocity(float fInheritedVelocity)
{
    NIASSERT(fInheritedVelocity >= 0.0f && fInheritedVelocity <= 1.0f);

    // Verify we are in the range 0.0 to 1.0.
    fInheritedVelocity = NiMax(0.0f, fInheritedVelocity);
    fInheritedVelocity = NiMin(1.0f, fInheritedVelocity);

    m_kInputData.m_fInheritedVelocity = fInheritedVelocity;
}
//---------------------------------------------------------------------------
inline bool NiPSAirFieldForce::GetInheritRotation() const
{
    return m_kInputData.m_bInheritRotation;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetInheritRotation(bool bInheritRotation)
{
    m_kInputData.m_bInheritRotation = bInheritRotation;
}
//---------------------------------------------------------------------------
inline bool NiPSAirFieldForce::GetEnableSpread() const
{
    return m_kInputData.m_bEnableSpread;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetEnableSpread(bool bEnableSpread)
{
    m_kInputData.m_bEnableSpread = bEnableSpread;
}
//---------------------------------------------------------------------------
inline float NiPSAirFieldForce::GetSpread() const
{
    return m_kInputData.m_fSpread;
}
//---------------------------------------------------------------------------
inline void NiPSAirFieldForce::SetSpread(float fSpread)
{
    NIASSERT(fSpread >= 0.0f && fSpread <= 1.0f);

    // Verify we are in the range 0.0 to 1.0.
    fSpread = NiMax(0.0f, fSpread);
    fSpread = NiMin(1.0f, fSpread);

    m_kInputData.m_fSpread = fSpread;
}
//---------------------------------------------------------------------------
