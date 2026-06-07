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
inline const NiPoint3& NiPSGravityFieldForce::GetDirection() const
{
    return m_kInputData.m_kDirection;
}
//---------------------------------------------------------------------------
inline void NiPSGravityFieldForce::SetDirection(const NiPoint3& kDirection)
{
    m_kInputData.m_kDirection = kDirection;

    // Build the unitized direction.
    m_kInputData.m_kUnitDirection = kDirection;
    m_kInputData.m_kUnitDirection.Unitize();

    // Check for near zero.
    NIASSERT(m_kInputData.m_kUnitDirection != NiPoint3::ZERO);
    if (m_kInputData.m_kUnitDirection == NiPoint3::ZERO)
    {
        m_kInputData.m_kUnitDirection = -NiPoint3::UNIT_Y;
    }
}
//---------------------------------------------------------------------------
